# `shared/Packets/` — canonical packet-class tree

Landed 2026-04-19 as Phase 12.0 scaffolding. Empty on purpose:
this commit only sets up the target directory and the
duplicate-count ratchet; the actual file moves are ongoing
ratchet-driven work done per-PR.

## Policy

When a packet class from `dkrixserver/src/Core/*(CG|GC|CL|LC|GS|SG)*.{h,cpp}`
gets migrated, it moves HERE. The server-side CMake source
list in `dkrixserver/src/Core/CMakeLists.txt` drops the entry;
`shared/Packets/CMakeLists.txt` picks it up (file doesn't exist
yet — the first migration PR adds it). Both builds' include
paths point at this directory.

If the packet has a client-side duplicate in
`dkrix/Client/Packet/Cpackets/`, that file is **deleted in the
same commit as the server-side move** — the ratchet only drops
the count if BOTH sides are unified. A migration PR that moves
the server copy without deleting the client copy creates a
third copy and leaves the ratchet unchanged; CI won't fail it
(the count didn't GROW), but it also didn't help.

## Why this exists at the repo root, not under one of the
## sibling trees

`dkrix/` (client) and `dkrixserver/` (server) are sibling
subdirectories with their own top-level `CMakeLists.txt`.
Parking packet classes under either one makes the other
tree's build reach "across" a logical boundary. A third top-
level `shared/` directory is neutral: both builds reach into
it symmetrically via `add_subdirectory(../shared/Packets)`
(relative paths resolve the same from both sides).

Per-tree alternatives considered and rejected:

- **Canonical tree on the server side (server includes, client
  reaches across).** Works for code-ownership (server is the
  more-edited tree) but makes client builds depend on a path
  inside the server tree, which is weird for cross-tree
  dependency tooling (ccache, IDE include-path resolvers,
  etc.).
- **Canonical tree on the client side.** Inverse of the above;
  same problem in the other direction.
- **Git submodule for `shared/`.** Overkill at this scope; no
  external consumers of packet classes, and a submodule adds
  release-management overhead (tag pinning, submodule update
  dance on every packet change) in exchange for no
  organisational benefit.

## Migration shape

Per-PR template for moving one packet family:

1. `git mv dkrixserver/src/Core/XYZ.{h,cpp} shared/Packets/`
   (and any same-family files that move as a group).
2. Drop the entries from
   `dkrixserver/src/Core/CMakeLists.txt`'s `CORE_SOURCES` list.
3. Add them to `shared/Packets/CMakeLists.txt`'s source list
   (create the file if this is the first migration PR).
4. Ensure the affected build targets pick up
   `shared/Packets/` on their include path (via
   `target_include_directories` on the `shared_packets`
   library target, or a direct `include_directories(...)`
   upgrade on the root CMakeLists).
5. If the packet has a Cpackets duplicate, delete it:
   `git rm dkrix/Client/Packet/Cpackets/XYZ.{h,cpp}`.
   The client build picks up the shared copy via its new
   include path.
6. Run
   `dkrixserver/scripts/check-packet-duplicates.sh --update`
   and commit the new baseline alongside the migration.
7. Compile both trees; no behaviour change expected —
   **but this requires reconciliation work first** (see
   "Migration class" below; the two copies are almost
   never byte-identical).

## Migration class (what "reconciliation" means)

Phase 17's tree-wide divergence audit (run
`dkrixserver/scripts/audit-packet-divergence.sh`) classifies
each of the 163 complete class pairs (all four of
server.cpp, server.h, client.cpp, client.h present) into:

| Class     | Pair-level count | What it means                                                                |
| --------- | ---------------: | ---------------------------------------------------------------------------- |
| identical |                0 | Byte-equal .cpp AND .h across trees; `git mv` is safe.                       |
| cosmetic  |                0 | Only whitespace / `throw()` / PCH / guard differences in both files.         |
| semantic  |              163 | At least one of .cpp or .h still differs after aggressive normalization.     |

**There are no zero-effort canaries.** Every one of the 163
classes has at least one semantic-diff file. Per-PR migration
effort always includes at least one reconciliation step.

**Reconciliation checklist** (per-PR, before step 1 in the
template above):

- Diff server.cpp vs client.cpp side-by-side; pick canonical
  version. The server side is usually more modern (no
  `throw()` exception specs, which are deprecated in C++17);
  prefer it unless the diff has evidence to the contrary
  (e.g. client has a correct fix for a bug the server copy
  still carries).
- Diff server.h vs client.h; same choice logic. Headers
  differ more often than .cpp files because they carry
  `#ifdef __DEBUG_OUTPUT__` gates around `toString()` /
  `getPacketName()` that the server copy has unconditional.
- Verify wire format hasn't drifted: `getPacketSize()`
  expressions should be algebraically equivalent (different
  textual spellings of the same sum are OK); `read()` and
  `write()` method bodies must call `readByte/Short/Long`
  and `writeByte/Short/Long` in the same order with the same
  types. If either diverges, the packets have been versioned
  by accident and the migration PR needs to stop and flag.
- Verify method signatures are call-site compatible on both
  sides. If server declares `void execute(Player* pPlayer)`
  and client declares `void execute(Player* pPlayer) throw
  (ProtocolException, Error)`, picking the server version is
  safe — `throw()` specs are exception behavior, not part of
  the ABI as far as call sites are concerned, and C++17
  ignores them anyway. If signatures differ in parameter
  types or order, the migration PR needs to stop and flag.

**First-wave candidates** (smallest expected reconciliation
work): the 7 classes whose .cpp file is at most cosmetic
(their .h files will still need reconciliation, but the .cpp
side is a straight `git mv`):

- `CGDisplayItem`
- `CGGQuestCancel`
- `CGRequestStoreInfo`
- `CGStoreClose`
- `CGStoreOpen`
- `CGStoreSign`
- `CGUndisplayItem`

Pick one of these for the first migration PR to minimize
new-pattern risk; the remaining 156 classes follow the same
shape once the first migration's CMake wiring is in place.

Run
`dkrixserver/scripts/audit-packet-divergence.sh --by-file`
after each migration to see the updated file-level
breakdown + current cosmetic-.cpp shortlist.

## Ratchet

The current duplicate count is tracked in
`dkrixserver/scripts/.packet-duplicates-baseline`. CI fails
on count increase. The gate lives in the server-side scripts
directory (alongside `check-sql-injection.sh`) for
consistency with the Phase 8C pattern; the script itself
reaches across into `dkrix/Client/Packet/Cpackets/` for the
comparison half.

At baseline 0, the ratchet becomes the "duplicates never
reappear" CI structural check originally scoped as 12.3 —
same guarantee, same mechanism, no additional implementation
work needed.

## What does NOT belong here

- Server-side `*Handler.cpp` files — these are server-only
  (they dispatch the received packet into game-state mutation
  code that has no analog on the client). They stay in
  `dkrixserver/src/Core/` alongside the server tree's other
  non-packet code.
- `SocketInputStream.{h,cpp}` / `SocketOutputStream.{h,cpp}`
  — wire-stream infrastructure duplicated across
  `dkrixserver/src/Core/` and `dkrix/Client/Packet/`. Flagged
  by Phase 13's close-out as wanting consolidation, but that
  is SEPARATE from packet-class unification — the stream
  files need an endian-safe template-migration pass
  (Phase 13.3) that's pinned to post-Phase-12. They'll
  eventually land in `shared/Core/` or similar, not here.
- `Cpackets/` itself as a directory name — the shared tree is
  flat. Migration PRs drop the Cpackets subdirectory path
  entirely once the last duplicate is lifted out.
