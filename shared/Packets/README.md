# `shared/Packets/` — canonical packet-class tree

Scaffolding for the Phase 12 packet-schema unification described in
`docs/MODERNIZATION.md` ("Decided 2026-08-08: build `shared/Packets/`").
Empty of packet classes at this commit on purpose — this only sets up the
target directory and the CMake wiring; classes move in one at a time,
per-PR, as they're reconciled.

## Why this exists at the repo root, not under one of the sibling trees

`dkrix/` (client) and `dkrixserver/` (server) are sibling top-level
directories, each with its own `CMakeLists.txt`. Parking the canonical
packet-class tree under either one makes the other tree's build reach
"across" a logical boundary — and an earlier attempt to do exactly that
(point the client's CMake source list at the physical file under
`dkrixserver/src/Core/`) failed for a structural reason, not a style one:
`#include "Packet.h"` / `"PacketFactory.h"` resolution always checks the
including file's own directory first, ahead of any `-I` path. A client
translation unit compiling the physical server-tree file would silently
pull in the server's `Packet.h` / `PacketFactory.h` / `SocketInputStream.h`
— confirmed different from the client's own copies (44- and 303-line diffs)
— a structural break, not a formatting nit. See the "Wave 1 attempt,
2026-08-07 — blocked, nothing landed" note in `docs/MODERNIZATION.md`'s
Phase 12 section for the full account.

A third top-level `shared/` directory is neutral: both builds reach into it
symmetrically via `add_subdirectory(../shared/Packets ...)`, using an
absolute path for the binary-dir argument and an absolute base for the
source list so it resolves the same regardless of which tree's CMake
invoked it first.

This mirrors the mechanism the parked development line
(`archive/modernization-phases-1-17`, tag — never merge from it directly)
built and proved end-to-end with a login→gameplay smoke test in April.
Adapted here against `main`'s current tree, not copied file-for-file: this
tree's CMake floors, target names, and packet-family CMakeLists structure
have all moved on since April (Phase 3 dissolved `DXLib/` into
`Client/Platform/`, Phase 11 added `PreparedStatement`, etc.).

## Migration shape

Per-PR template for moving one packet class (or small family group):

1. Run `python3 dkrixserver/scripts/normalize-packet-style.py --pair <Name>
   --emit <dir>` to confirm the wire-signature verdict is `style-only` (or
   review the `residual.diff` carefully if not) before touching anything.
   **A pair is not byte-identical between the trees even when the tool
   verdict is `style-only`** — the emitted normalized text is a comparison
   aid, not something to paste in as-is. Reconcile by hand: prefer the
   server file's layout (no `throw()` specs, unconditional
   `getPacketName()`/`toString()` rather than `#ifdef __DEBUG_OUTPUT__`
   gated), but keep any `#ifndef __GAME_CLIENT__` guard around the
   `*Handler` class/call — the merged file compiles into both the client
   (`DarkEden` defines `__GAME_CLIENT__=1`) and the server (which never
   defines it), and the guard is what keeps the client build from needing
   a definition of a handler it has no game-state code to run.
2. `git mv dkrixserver/src/Core/<Name>.{h,cpp} shared/Packets/`, using the
   reconciled content from step 1 (a plain `git mv` is only safe if the
   file truly needs no edits, which essentially never happens per Phase
   17's audit — 0 of 163 pairs were byte-identical or purely cosmetic).
3. Drop `<Name>.cpp` from the matching family list (e.g. `CL_PACKET_SOURCES`)
   in `dkrixserver/src/Core/CMakeLists.txt`. Leave `<Name>Handler.cpp` where
   it is — handler files are server-only and never move (see "What does NOT
   belong here" below).
4. Add `<Name>.cpp` to the matching `_SHARED_PACKETS_<FAMILY>_SOURCES` list
   in `shared/Packets/CMakeLists.txt`.
5. Wire the build targets that need it:
   - The server target that actually compiles the family (e.g.
     `LoginServerPackets` for a `CL` packet) needs
     `target_link_libraries(<Target> PRIVATE shared_packets)` so it absorbs
     the `.cpp` as a source with its own `__LOGIN_SERVER__` /
     `__GAME_SERVER__` / `__SHARED_SERVER__` define applied.
   - `dkrixserver/src/Core/PacketFactoryManager.cpp` is compiled into
     *all three* server packet libraries (`PACKET_COMMON_SOURCES`) and
     `#include`s every packet header unconditionally, even though the
     `addFactory(...)` call for a given family is guarded by
     `#if defined(__LOGIN_SERVER__)` (etc.). Any server target that does
     **not** own the family's handler must still see the header —
     `target_include_directories(<Target> PRIVATE
     ${CMAKE_SOURCE_DIR}/../shared/Packets)` without linking
     `shared_packets`. Linking it there instead would absorb the `.cpp` as
     a source in a binary that has no definition of `<Name>Handler::execute`,
     which is a link error, not a warning.
   - The client (`DarkEden` target in `dkrix/CMakeLists.txt`) links
     `shared_packets` directly — it globs `Client/Packet/**/*.cpp` for its
     own duplicate, so once that duplicate is deleted (next step) the
     shared copy, absorbed via the INTERFACE target, is the only copy left.
6. Delete the client duplicate: `git rm
   dkrix/Client/Packet/Cpackets/<Name>.{h,cpp}`, and repoint every
   `#include ".../Cpackets/<Name>.h"` site under `dkrix/` at the new,
   flat `#include "<Name>.h"` (resolved via `shared_packets`'s
   `INTERFACE` include directory). `git grep` for the old path afterward —
   it must return nothing outside `.vcxproj`/`.vcxproj.filters` (the VS6/
   VS2022-project-file bookkeeping, not part of the CMake build).
7. Run `dkrixserver/scripts/check-packet-duplicates.sh --update` and commit
   the new `.packet-duplicates-baseline` alongside the migration. Confirm
   `--count` dropped by exactly 1 (one class pair × 2 files each side).

## Verifying without a compiler

Neither toolchain (MSVC/vcpkg for the client, the Linux server toolchain) is
always available in every environment that touches this tree. When it
isn't, verify by reading, paranoidly, given this exact family of bug has
caused three CI-red incidents in one day here (case-sensitive `find` vs.
case-insensitive Windows glob, files silently dropped from a CMake source
list):

- Every `#include` site that named the old `Cpackets/<Name>.h` path is
  updated — `git grep` for it returns nothing under `dkrix/` (excluding
  project-file bookkeeping).
- The family source list in `dkrixserver/src/Core/CMakeLists.txt` no
  longer names `<Name>.cpp` but still names `<Name>Handler.cpp`.
- `shared/Packets/CMakeLists.txt` names `<Name>.cpp` exactly once, in the
  correct family group.
- The moved `.h`/`.cpp` actually exist at `shared/Packets/<Name>.{h,cpp}`
  and no longer exist under `dkrixserver/src/Core/` or
  `dkrix/Client/Packet/Cpackets/`.
- Re-run `normalize-packet-style.py --pair <Name>` — impossible to point at
  the old two-tree layout once one side has moved, so instead diff the
  final merged file against both pre-migration originals by eye and check
  the wire-relevant bits specifically: `read()`/`write()` call order and
  types, and the `getPacketSize()`/`getPacketMaxSize()` expressions.

**Both-tree CI is still the real gate.** Nothing above is a substitute for
an actual compile; it only bounds the risk in the gap before one runs.

## What does NOT belong here

- Server-side `*Handler.cpp` files — server-only, dispatch the received
  packet into game-state mutation code with no client analog. Stay in
  `dkrixserver/src/Core/`.
- `SocketInputStream.{h,cpp}` / `SocketOutputStream.{h,cpp}` — wire-stream
  infrastructure duplicated across both trees (`docs/MODERNIZATION.md`
  Phase 13). Consolidating them is a related but separate move, pinned to
  land per-pair alongside that pair's Phase 12 migration (each unified
  packet pair collapses its own stream include shape), not as a
  standalone sweep.
- `Cpackets/` as a directory name — this tree is flat. A migration PR drops
  the `Cpackets/` subpath from every `#include` it touches.

## Ratchet

`dkrixserver/scripts/check-packet-duplicates.sh` (`--count` / `--list` /
`--update`) is the scoreboard: same-named packet files present in both
`dkrixserver/src/Core/` and `dkrix/Client/Packet/Cpackets/`. Baseline lives
in `dkrixserver/scripts/.packet-duplicates-baseline` and is wired into
`server.yml`'s `ratchets` CI job — it fails the build on any *increase*.
Check the current baseline before quoting it; it steps down by exactly 2
files (one class pair) per completed migration.
