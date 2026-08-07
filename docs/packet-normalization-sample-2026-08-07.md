# Packet style-normalization — sample validation, 2026-08-07

Prototype and evidence for the Phase 12 sizing prescription in
`docs/packet-divergence-2026-08-07.md`: *"one scripted style-normalization
pass over the ~160 twin pairs plus targeted manual protocol review of the CL
login/account family."* This builds that pass's normalizer, validates it on
samples, and — since the verdict pass is cheap (1.7 s) — runs it read-only
over **all 163 pairs**, so the projection below is a full-population
measurement, not an extrapolation.

**Tool:** `dkrixserver/scripts/normalize-packet-style.py` (python3, stdlib
only, deterministic, read-only over both packet trees). Placed next to the
parked line's `audit-packet-divergence.sh` and the two ratchet scripts —
`dkrixserver/scripts/` is the established home for cross-tree packet
tooling. python3 rather than sh because the residual measurement must
reproduce the Phase 17 re-run's difflib-based counts (its `normalize.py` is
embedded verbatim as `--audit-check`), and because guard folding and
statement re-layout need a string-literal-aware scanner, which is exactly
where the parked sed pipeline broke (its CRLF-sensitive PCH match).

**Tree state:** packet trees byte-identical to the divergence audit's tree
(`git diff --stat ce4a73f..HEAD` over `dkrixserver/src/Core/` +
`dkrix/Client/Packet/` is empty) [measured], so numbers are directly
comparable.

## Measurement continuity with the Phase 17 re-run [measured]

`--audit-check` (the doc's normalizer verbatim, difflib changed-line count)
over all 163 pairs reproduces `docs/packet-divergence-2026-08-07.md`
exactly: min 92 (`CLGetServerList`), median 157, p90 230, max 747
(`CLRegisterPlayer`). Every doc-named per-pair figure also matches: 96
`CGReady`, 93 `CLGetPCList`/`CLGetWorldList`/`CLLogout`, 290
`CGSkillToInventory`, 296 `CGAbsorbSoul`, 306 `CLLogin`, 361 `CLCreatePC`,
747 `CLRegisterPlayer`.

## Canonical target style (what the eventual pass converges on)

Follows the divergence doc's own prescription; each item is a decision the
rewrite pass applies, and the normalizer treats as an equivalence when
comparing:

1. **Layout**: server's `.clang-format` (LLVM base, 4-space, no tabs,
   attached braces, pointer-left, col 120) — the only machine-enforced
   style in the repo (`make fmt-check`, CI, pinned clang-format 18.1.8).
2. **No dynamic-exception specs** — server already dropped `throw(...)`;
   deprecated in C++11.
3. **`std::string`** (the doc's "fix `string`→`std::string`"; matches
   `docs/CLAUDE.md`). The normalizer compares qualifier-insensitively; the
   rewrite direction is toward explicit `std::`.
4. **Flat `"X.h"` includes** (unified files land server-side), include
   block sorted; `Assert.h` ≡ `Assert1.h` (both define the identical
   `Assert(expr)` macro; 23 packet .cpp each side [measured]).
5. **Client-only lines fold into the unified file**: `Client_PCH.h`,
   `#ifdef __DEBUG_OUTPUT__`, `#ifndef __GAME_CLIENT__` (guard lines fold,
   guarded *content* is kept and compared), and the Handler-dispatch call
   in `execute()` (guarded server-side call vs deleted client-side).
6. **Size-constant note**: `szuint` ≡ `szDWORD` (both `sizeof` of 32-bit
   types on the two supported ABIs) [measured in both
   `types/SystemTypes.h`]. `szlong`/`szulong` are **not** equivalent —
   client pins `sizeof(int32_t)`, server has `sizeof(long)` = 8 on LP64 —
   but no packet file uses them (0 hits) [measured].

## Verdict machinery — why residual 0 is not the safety argument

For each side the script extracts a **wire signature**: the ordered
read/readEncrypt and write/writeEncrypt call sequences and the
commutatively-canonicalized `getPacketSize()` expression (wire-relevant:
the client writes `getPacketSize()` into the outgoing packet header —
`dkrix/Client/Packet/Packet.h`, `writeHeaderNBody`). Verdict:
`real-divergence` if any signature differs; else `style-only` (residual 0)
or `style-residual`. Two proofs that normalization is style-only:

- `--selfcheck`: wire signatures extracted after the full pipeline equal
  those from a fold-free pipeline on **326/326 pair-sides** [measured] —
  the fold rules never touch wire-describing text.
- The 9 `real-divergence` flags below were each verified against the raw
  files, and include both pairs the divergence doc flagged from manual
  reading (`CLLogin`, `CLCreatePC`) — the detector reproduces the human
  findings and adds seven more.

Caveat: `CGPortCheck` is the one Datagram (UDP) packet; the stream
extractor finds no bodies there (noted by the tool) and its `style-only`
verdict rests on residual 0 alone.

## Sample validation [measured]

Raw = changed lines, raw text; audit = under the Phase 17 re-run
normalizer (doc-comparable); residual = under full style normalization.
`.h`+`.cpp` combined, difflib changed-line counts.

| Pair | Raw | Audit | Residual | Verdict |
|---|---|---|---|---|
| CGReady | 165 | 96 | 0 | style-only |
| CLGetPCList | 159 | 93 | 0 | style-only |
| CLGetServerList | 151 | 92 | 0 | style-only |
| CLGetWorldList | 140 | 93 | 0 | style-only |
| CLLogout | 156 | 93 | 0 | style-only |
| CLVersionCheck | 198 | 131 | 0 | style-only |
| CGAttack | 358 | 252 | 4 | style-residual |
| CLSelectServer | 161 | 106 | 6 | style-residual |
| CGRequestIP | 308 | 207 | 29 | style-residual (decl-order + include-set drift) |
| CGAbsorbSoul | 336 | 296 | 36 | style-residual (size spelling wire-equal; decl order; getter return-type drift) |
| CGMove | 292 | 205 | 4 | **real-divergence** (read/write order) |
| CGSkillToInventory | 355 | 290 | 4 | **real-divergence** (size_expr) |
| CGUseItemFromInventory | 307 | 259 | 35 | **real-divergence** (size_expr) |

Account-family anchors — measurement continuity only, excluded from the
normalization sample per the divergence doc's manual-review flag:

| Pair | Raw | Audit | Residual | Verdict |
|---|---|---|---|---|
| CLRegisterPlayer | 955 | 747 | **1** | style-residual (one `#include "PlayerInfo.h"` line) |
| CLLogin | 426 | 306 | 83 | **real-divergence** (read/write; the doc's finding, reproduced) |

The doc's largest diff (`CLRegisterPlayer`, 747) collapses to a single
include line: the ~15× `PlayerInfo::maxXLength` qualification drift, the
`string`/`std::string` drift and the brace style were all style. The
doc's "smallest" pairs go to zero.

## Full-population result [measured, text-level — nothing here has survived a compile]

All 163 pairs, 1.7 s:

| style-only (residual 0) | style-residual ≤5 | style-residual >5 | real-divergence |
|---|---|---|---|
| **62** | **44** | **48** | **9** |

Style-residual population: n = 92, min 1, median 6, max 36. Residual
content, from inspection: declaration-order drift, include-set drift,
getter return-type drift (`Coord_t` vs `ZoneCoord_t`), size-expression
spelling (wire-equal after canonicalization), `toString()` text drift —
all wire-matched as measured, all resolved by rewriting both sides to the
server layout.

### The 9 real-divergence pairs (protocol review queue)

| Pair | Signature diff | Reading |
|---|---|---|
| CLLogin | read+write | Doc's confirmed case: server-only Netmarble/adult/MAC/child-guard fields; client hardcodes loginMode. |
| CLCreatePC | size | Client counts `szRace`, server doesn't — the doc's "worth re-checking" confirmed by machine. |
| CGSkillToInventory | size | Server header counts `szObjectID` twice; **both** sides' streams write 6 fields; client's "viva" formula matches the streams. Server's own header is stale vs its own `read()`. |
| CGUseItemFromInventory | size | Same class: client size dropped one `szObjectID`. |
| CGMove | read+write | Plain (non-encrypt) branch order: server reads `Dir,X,Y`, client writes `X,Y,Dir`; encrypt branch matches. Live bug iff the non-encrypt path can run — confirm `__USE_ENCRYPTER__` is unconditional in shipped builds. |
| CGGuildChat | read+write | Member rename `m_Type`/`m_bType`, both `BYTE` [measured] — layout-equal, lowest-risk of the nine. |
| CGModifyNickname | read+write | `m_ItemObjectID` vs `m_NicknameID` — same slot, different *meaning*; needs a human to say which is authoritative. |
| CGExchangeBuy | read+write+size | Server-side exchange protocol moved ahead: `m_IdempotencyKey`, `(uint64_t)` listing ID; client copy is stale. |
| CGExchangeList | read+write+size | Same: server has seller-filter string + length-prefixed reads the client lacks. |

Two consequences for the pass design: **"pick the server version" is not a
safe default** — for the two size-formula pairs the *server* header is the
stale side, and the client writes `getPacketSize()` into every outgoing
packet header, so adopting the server formula would corrupt client
framing. The streams, not either header, are the authority. And the
exchange pair suggests checking whether the live client can currently use
the exchange feature at all.

## Recommended batch plan for the real pass

Run on the workstation (both toolchains), as its own CI-gated change
series — never mixed with behavior work. Long-term scoreboard:
`check-packet-duplicates.sh` ratchet, baseline **326**, stepping down as
pairs unify.

1. **Wave 1 — 62 style-only pairs.** Mechanical: rewrite both sides to
   canonical (in practice: adopt the server file + folded guards/PCH/
   dispatch, delete the Cpackets sibling, update client build lists).
   Tool-verified residual 0 before and after; land in batches of ~20 with
   client + server CI green per batch; ratchet 326 → ~202.
2. **Wave 2 — 44 pairs, residual ≤5.** Same flow plus a per-pair review of
   the ≤5 leftover lines (median 6-line class starts here).
3. **Wave 3 — 48 pairs, residual >5.** Needs the rewrite to also converge
   declaration order / include sets / getter types onto the server layout;
   re-run the tool after each batch — residual must be 0 post-rewrite.
4. **Protocol review queue — the 9 flagged pairs** (plus the rest of the
   CL account family the doc already reserved: `CLReconnectLogin`,
   `CLDeletePC`). Each needs an explicit wire decision recorded in
   `MODERNIZATION.md`; the script must never auto-merge these.

Phase 13 note: unifying a pair also collapses its
`SocketInputStream`/`SocketOutputStream` include shape — do the client
half of Phase 13 in the same series, per the plan.

## Appendix — commands

```
# verdict pass, all pairs (1.7 s), TSV + summary on stderr
python3 dkrixserver/scripts/normalize-packet-style.py --all --tsv

# one pair, JSON verdict + normalized forms and residual diff
python3 dkrixserver/scripts/normalize-packet-style.py --pair CGReady --emit /tmp/pktnorm

# reproduce docs/packet-divergence-2026-08-07.md distribution
python3 dkrixserver/scripts/normalize-packet-style.py --audit-check

# prove fold rules wire-neutral (exit 1 on any mismatch)
python3 dkrixserver/scripts/normalize-packet-style.py --selfcheck
```

Raw-file verifications behind the table: `CGMove.cpp` plain-branch order
(server lines 29–31 vs client 32–34); `CGSkillToInventory.{h,cpp}` size
formula vs 6-field streams both sides; `CGGuildChat.h` member types (both
`BYTE`); `dkrix/Client/Packet/Packet.h` `writeHeaderNBody` writing
`getPacketSize()`; `types/SystemTypes.h` both trees for the `sz*`
constant table; `grep -c` for `Assert1.h`/`Assert.h` (23/23) and
`szlong|szulong` in packet files (0).
