# Packet divergence audit — 2026-08-07

Re-run of the parked line's Phase 17 audit (`archive/modernization-phases-1-17`,
`dkrixserver/scripts/audit-packet-divergence.sh`) against the current `main`
tree, ahead of sizing Phase 12 (packet unification).

**Tree state:** `main` @ `ce4a73fab35a996ce15f76ddf087d28c83e9326e`, clean
(`git status --porcelain` empty).

## Headline numbers

| Level | Total | Matched (client name found on server) | Client-only | Bit-identical | Cosmetic-only | Divergent |
|---|---|---|---|---|---|---|
| File (.h/.cpp separately) | 326 | 326 | 0 | 0 | 0 | 326 |
| Pair (.h+.cpp per class — the migration unit) | 163 | 163 | 0 | 0 | 0 | 163 |

Server family totals on `main` (CG/GC/CL/LC/GS/SG, `dkrixserver/src/Core/`,
excluding `*Handler*`, maxdepth 1): CG 300, GC 516, CL 34, LC 34, GS 16, SG 20
= **920 files**, all `.h`/`.cpp`-paired (0 orphans) → matches the parked
"920" figure exactly. (GT/TG, which the parked script's pattern also allows
for, contribute 0 on this tree.)

## Buckets

Every one of the 163 matched pairs lands in **divergent** — none survive
normalization (strip `\r`, trailing whitespace, blank-run collapse, `throw(...)`
specs, `//` and `/* */` comments) intact. Diff-size distribution (normalized
changed-line count per pair, `.h`+`.cpp` combined):

| ≤5 | 6–20 | >20 (task buckets) | 21–50 | 51–100 | 101–200 | 201–400 | 401+ |
|---|---|---|---|---|---|---|---|
| 0 | 0 | 163 | 0 | 9 | 120 | 33 | 1 |

Min 92, median 157, p90 230, max 747 normalized-diff lines. The task's ≤5/6–20
cutoffs don't discriminate here — even the smallest pair (a no-payload control
packet) differs on ~90 lines, because the normalization spec doesn't collapse
*intra-line* whitespace, and the two trees use different token-spacing styles
throughout (see below).

## 10 characterized examples

**5 least divergent** (all small, no/near-no-payload packets) — same root
cause throughout: server's `execute(Player*)` absent client-side (client
doesn't dispatch packets), include-path style (`"Packet.h"` vs
`"../Packet.h"`), tabs-vs-spaces, client-only `#include "Client_PCH.h"` and
`#ifdef __DEBUG_OUTPUT__`/`#ifndef __GAME_CLIENT__` guards:
`CLGetServerList` (92), `CLGetPCList` (93), `CLGetWorldList` (93), `CLLogout`
(93), `CGReady` (96).

**5 most divergent** (larger only because they carry more fields — same
style tax repeated per field, not a different kind of divergence):
- `CGSkillToInventory` (290) — same 6 fields (SkillType, ObjectID×2,
  X/Y/TargetX/TargetY); server writes 3-line getter/setter bodies, client
  inlines to one line; `string` vs `std::string`; member reordering.
- `CGAbsorbSoul` (296) — same 8 coordinate/ID fields; identical style-only
  pattern as above.
- `CLLogin` (306) — **real divergence, not just style**: server carries
  `m_bNetmarble`, `m_bAdult`, a second MAC field (`m_strMacAddress` beside
  `m_cMacAddress[6]`), `m_LoginMode`/`LOGIN_MODE_WEBLOGIN`, and a static
  `onChildGuardTimeArea()` (Korean-portal parental-control fields) that the
  client neither declares nor serializes; client's `write()` hardcodes
  `loginMode = 0` instead of sending a read value. This is a wire-shape
  difference, not cosmetic.
- `CLCreatePC` (361) — same field set (name/slot/sex/hair/colors/STR/DEX/INT
  /race via bitset); divergence is style + brace placement, scaled by field
  count.
- `CLRegisterPlayer` (747) — largest packet (~15 string fields: ID, password,
  name, SSN, phone, cellular, zip, address, email, homepage, profile), same
  fields both sides; divergence is `maxXLength` vs `PlayerInfo::maxXLength`
  qualification + `string`/`std::string` + brace style repeated ~15×. Biggest
  diff count in the set, but not the most semantically different.

## Comparison with the parked line's numbers

The parked Phase 17 headline (0 bit-identical / 7 cosmetic / 319 semantic) is
**file-level** (326 files); its own **pair-level** result — the number that
matters, since a class's `.h`+`.cpp` migrate together — was already 0 / **0
cosmetic** / 163 semantic (all 7 cosmetic `.cpp` files paired with a semantic
`.h`). Re-running their unmodified script (pulled read-only from the tag)
against current `main` reproduces that pair-level result exactly: 0/0/163.
**At the level that drives Phase 12 sizing, nothing has changed.** File-level
it now reports 0/326 cosmetic instead of 7/326; tracing one
(`CGStoreClose.cpp`) shows this is a false regression: their
`#include "Client_PCH.h"` deletion rule is a literal end-of-line match that
stops matching under this workstation's CRLF working-tree checkout (blobs are
pure LF per `.gitattributes eol=lf`, confirmed via `git show HEAD:...`; the
working tree is CRLF, a Windows-checkout artifact, not a content change).
This task's normalization strips `\r` unconditionally first, so it's immune
either way. Net: main's fmt/cleanup work has **not** measurably changed the
divergence picture since April.

## Implications for Phase 12 sizing

No bulk `git mv` wave exists at any diff threshold — every pair needs a
per-file touch, confirming the parked line's conclusion. But effort isn't
uniform: (1) **the bulk of the 163** (both least- and most-divergent examples
above, bar one) are style/inlining/qualification twins with identical field
sets — mechanically reconcilable (pick a canonical brace/body style, fix
`string`→`std::string`, fold the `__DEBUG_OUTPUT__`/`__GAME_CLIENT__`/PCH
lines into the unified header) and scriptable as one batch pass; (2) a
**small, so-far-unquantified minority** — `CLLogin` confirmed, `CLCreatePC`/
`CLRegisterPlayer` worth re-checking despite matching here — carry genuine
wire-shape/behavioral differences (legacy Korean-portal Netmarble/adult/
child-guard fields) needing protocol review, not merge-by-script. Size Phase
12 as "1 scripted style-normalization pass + manual protocol review for the
CL login/account family," not "163 independent manual merges."

## Appendix — commands and normalization script

```
# client files matching ^(CG|CL).*\.(h|cpp)$, excluding *Handler* (326)
find dkrix/Client/Packet/Cpackets -maxdepth 1 -type f \( -name "*.h" -o -name "*.cpp" \) \
  | grep -E '/(CG|CL)[^/]*\.(h|cpp)$' | grep -v Handler | wc -l
# server family totals, per prefix, same filters (sums to 920)
for p in CG GC CL LC GS SG; do
  find dkrixserver/src/Core -maxdepth 1 -type f \( -name "*.h" -o -name "*.cpp" \) \
    | grep -E "/${p}[^/]*\.(h|cpp)$" | grep -v Handler | wc -l
done
# basenames (dedup .h/.cpp) both sides, then set-intersect
find dkrix/Client/Packet/Cpackets -maxdepth 1 -type f \( -name "*.h" -o -name "*.cpp" \) \
  | grep -E '/(CG|CL)[^/]*\.(h|cpp)$' | grep -v Handler \
  | sed 's/.*\///; s/\.\(h\|cpp\)$//' | sort -u > client_basenames.txt        # 163
find dkrixserver/src/Core -maxdepth 1 -type f \( -name "*.h" -o -name "*.cpp" \) \
  | sed 's/.*\///; s/\.\(h\|cpp\)$//' | sort -u > server_basenames_all.txt
comm -12 client_basenames.txt server_basenames_all.txt > matched_basenames.txt # 163 (comm -23 client-only = 0)
```
Classification driver `/tmp/pktaudit/compare.py`: for each matched basename,
byte-compare `.h`/`.cpp` per side; if unequal, normalize both with
`normalize.py` below and re-compare. Pair is bit-identical only if both files
raw-equal, cosmetic-only if not raw-equal but both normalize-equal, else
divergent; diff-size via `difflib.unified_diff` changed-line count.

Parked-line reproduction: `git show archive/modernization-phases-1-17:dkrixserver/scripts/audit-packet-divergence.sh`
→ `/tmp/pktaudit/parked_audit.sh`, repoint `server_tree`/`client_tree` to
absolute paths, run unmodified: `sh parked_audit.sh --count` / `--by-file`.

`normalize.py` (strip `\r`, `//` and `/* */` comments, `throw(...)` specs,
trailing whitespace, collapse blank-line runs):
```python
import re, sys
def strip_comments(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    return re.sub(r'//[^\n]*', '', text)
def normalize(text):
    text = strip_comments(text.replace('\r', ''))
    text = re.sub(r'throw\s*\([^)]*\)', '', text)
    lines = [l.rstrip() for l in text.split('\n')]
    out, prev_blank = [], False
    for l in lines:
        blank = (l == '')
        if blank and prev_blank:
            continue
        out.append(l); prev_blank = blank
    while out and out[0] == '': out.pop(0)
    while out and out[-1] == '': out.pop()
    return '\n'.join(out) + '\n'
```
