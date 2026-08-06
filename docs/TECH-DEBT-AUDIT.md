# Tech Debt Audit — 2026-08-06

Measured against the working tree at `f19c4d3`, branch `modernize/phase-4-sprite`.
Every figure below was re-derived by direct inspection; commands are in the appendix.
**No figure has been confirmed by a compile** — no CI run exists for this repo.

All 17 non-LOC figures below were re-derived a second time and matched. The LOC
figures did **not** match on first pass — see the appendix note on `wc -l | tail -1`.

Scoring: `Priority = (Impact + Risk) × (6 − Effort)`, each dimension 1–5.
Impact = how much it slows work. Risk = what happens if ignored. Effort = cost to fix.

Scale context: client `dkrix/` is **652,346 LOC** (excluding build trees), server
`dkrixserver/src/` is **632,669 LOC** — about **1.29 million lines** total. One
maintainer.

---

## Ranked backlog

| # | Item | Type | I | R | E | **Pri** |
|---|---|---|---|---|---|---|
| 1 | CI has never executed | Infrastructure | 5 | 5 | 2 | **40** |
| 2 | `SPRITELIB_BACKEND_README.md` claims "Production Ready" falsely | Documentation | 3 | 4 | 1 | **35** |
| 3 | Branch has no upstream; 106-commit divergence | Infrastructure | 4 | 4 | 2 | **32** |
| 4 | 34 plaintext DB passwords in 13 tracked conf files | Security | 2 | 5 | 2 | **28** |
| 5 | Phase 4 premise in `MODERNIZATION.md` is factually wrong | Documentation | 3 | 3 | 1 | **30** |
| 6 | Duplicate-symbol `.cpp` files across two directories | Code | 3 | 3 | 2 | **24** |
| 7 | CMake `file(GLOB)` + 34 `FILTER`/`REMOVE_ITEM` lines | Architecture | 4 | 3 | 3 | **21** |
| 8 | Zero tests on 1.29M LOC (except `engine/sprite`'s 11 files) | Test | 4 | 5 | 4 | **18** |
| 9 | Two independent SDL2 backends | Architecture | 3 | 3 | 3 | **18** |
| 10 | Lua quest scripts get full `io` via `luaL_openlibs` | Security | 1 | 5 | 3 | **18** |
| 11 | 10 dead server `main.cpp` entry points | Code | 2 | 2 | 2 | **16** |
| 12 | `basic/Platform.h` at 1,996 lines | Architecture | 4 | 3 | 4 | **14** |
| 13 | `DXLib/` — 46 files, 10,163 LOC vestigial shim | Architecture | 3 | 2 | 3 | **15** |
| 14 | Build artifacts tracked in git (~6.6 MB `compile_commands.json`) | Infrastructure | 2 | 2 | 1 | **20** |
| 15 | 26 `sprintf(query, …)` SQL construction sites | Security | 2 | 4 | 4 | **12** |
| 16 | ~~`WinLib/` — 3 files, zero callers~~ **retracted, see below** | Code | — | — | — | **—** |
| 19 | 8 dead `*PackList555/565` files, not in the build | Code | 1 | 1 | 1 | **10** |
| 17 | `docs/README.md` describes a three-repo layout that doesn't exist | Documentation | 1 | 1 | 1 | **10** |
| 18 | `CSpriteSurface.cpp` at 13,644 lines | Code | 4 | 3 | 5 | **7** |

---

## Where this disagrees with `MODERNIZATION.md`

The audit's ranking and the plan's phase ordering diverge in four places. Each is a
case where the plan is either mis-sized or resting on an unverified claim.

**1. Documentation debt is badly underweighted.** The plan treats docs as Phase 0
("done"). But `SPRITELIB_BACKEND_README.md` currently asserts *"Status: Production
Ready — All phases completed (Phases 1-6), Integration tests passing"* and draws an
architecture diagram attributing the SDL2 backend to `engine/sprite`. Both claims are
false (see ADR-0001). This scores #2 in the whole backlog because the fix is an
hour's editing and the cost of leaving it is that the next person to touch sprites —
including a future agent session — starts from a wrong mental model of the system.
Cheap, high-consequence, and currently invisible to the plan.

**2. Phase 4 is mis-sized by roughly 35×.** The plan scopes it as "20 pixel-format
variant files." Those files are 1,500 LOC of SpriteLib's 53,566. The plan's stated
justification — SDL handles pixel conversion natively — does not apply, because those
classes override only `SaveToFile`/`LoadFromFile` and are SPK **deserializers**.
Executing Phase 4 as written is the lowest-value, highest-risk change available.
Full analysis in ADR-0001.

**3. Phase 8's sizing figure still does not reproduce, and the plan knows it.**
`MODERNIZATION.md` flags that the "~87 `sprintf(query, …)` sites" number is
unreproducible. Confirmed today: **26** matches for `sprintf(query`, **426** total
`sprintf` calls under `src/server/`. The exact commands are now in the appendix so
this stops being re-litigated. At 26 sites this is a much smaller phase than its
position in the plan implies — but see item #4 below, which is the part of Phase 8
that actually matters and is far cheaper.

**4. Secrets are a bigger and easier win than SQL injection.** Phase 8 bundles "SQL
and secrets" together. They should be split. The SQL work is 26 call sites of real
refactoring. The secrets work is 34 plaintext passwords across 13 conf files that are
**tracked in git** — including `conf/backup/` copies. Extracting those to environment
variables is a couple of hours and removes credentials from version history going
forward. It should not wait behind a query-parameterization project.

**One place the plan is right and this audit endorses it:** Phase -1 blocks
everything. Item #1 outscores every code-quality item in this backlog, and it isn't
close.

---

## Detail on the top items

### 1. CI has never executed — Priority 40

`.github/workflows/client.yml` and `server.yml` exist (committed in `53ac594`) but
have never run, because the branch has never been pushed. Consequence: **not one
claim in `MODERNIZATION.md`, this audit, or ADR-0001 has been verified by a
compiler.** Neither toolchain is reachable from an agent session — the client needs
Windows + VS2022 + vcpkg, the server needs Linux + libmysqlclient/lua5.1/xerces-c.

This is the whole ceiling. Until a build is green, every other item on this list is
a change that only Enrico can judge, one at a time, by hand. Fix this and items
6, 7, 11, 12, 13, 16 become delegable.

*Fix:* resolve item #3, push, iterate until green. Expect the first several runs to
fail on portability debt already documented — `Platform.h` drags `pthread.h` into
Windows builds, some helpers include `sys/time.h`.

### 3. Branch has no upstream — Priority 32

| Comparison | Result |
|---|---|
| `modernize/phase-4-sprite` upstream | none configured |
| vs `origin/main` | 4 ahead, 0 behind |
| vs `origin/modernize/phase4-sprite` | 4 ahead, **106 behind** |

The local branch was cut from `main`, not from the similarly-named remote branch.
`MODERNIZATION.md` calls this a "branch name mismatch"; it is a content divergence of
106 commits. Those commits are either abandoned work or work that needs recovering,
and that question has to be answered before anything is pushed — which blocks item #1.

Remote branches present: `main`, `modernize/phase1-dead-code`,
`modernize/phase2-platform-h`, `modernize/phase4-sprite`.

### 4. Plaintext credentials in tracked config — Priority 28

34 `PASSWORD` lines across 13 tracked files in `dkrixserver/conf/`, including a
`conf/backup/` set that duplicates them. Sample: `DB_PASSWORD : elca005` appears in
`excel96-gameserver.conf`, `excel96-gameserver2.conf`, `excel96-loginserver.conf`,
`excel96-sharedserver.conf`; `gameserver.conf` carries `elca110`. Dev IPs
(`192.168.0.16`) are alongside them.

Impact on daily work is low — hence I=2 — but risk is maximal if this repo ever
becomes public, and it is already in git history. Cheapest meaningful security fix
available.

### 6. Duplicate-symbol files — Priority 24

Two files exist at two paths each:

- `Client/GCNotifyWin.cpp` and `Client/Packet/Gpackets/GCNotifyWin.cpp`
- `Client/GCNotifyWinHandler.cpp` and `Client/Packet/Gpackets/GCNotifyWinHandler.cpp`

The other seven files `MODERNIZATION.md` lists under this heading
(`GlobalVariables`, `MissingGlobals`, `GameHelpers`, `GameFunctions`,
`GamePacketFunctions`, `ActionFunctions`, `MitemTableinit2`) exist at **one path
each** — they are not duplicates in the filesystem sense. They may still be
duplicate *symbol* sources, which only a link step can confirm. Another claim
waiting on item #1.

### 7. CMake GLOB + FILTER — Priority 21

`dkrix/CMakeLists.txt` is 697 lines containing 2 `file(GLOB…)` calls and **34**
`FILTER`/`REMOVE_ITEM` lines that subtract files back out. This is why item #6
exists: the build discovers files by pattern, then excludes the ones that collide.
Every new file silently joins the build. Converting to explicit source lists — which
`dkrixserver/` already does — removes a whole class of surprise and is the structural
fix that makes item #6 unnecessary.

### 8. Test debt — Priority 18

`tools/engine/sprite/tests/` holds 11 test files (3,898 lines). That is the **entire
automated test surface for ~1.29 million lines of code**, and it covers a library the
game client does not link. The server's `test`/`testserver`/`testAlone` directories are retired binaries,
not test suites — `testAlone/Mutex.h` defines a "mutex" whose lock/unlock just sets
a bool.

Wiring `engine/sprite`'s existing tests into CI is the cheapest possible first
green check and should ride along with item #1.

### 10. Lua sandbox — Priority 18

`LuaState.cpp:27` calls `luaL_openlibs(m_pState)`, giving quest scripts the full
standard library including `io.open` — filesystem access from script content. Impact
on development is nil (I=1), but if quest scripts are ever loaded from anywhere
untrusted this is arbitrary file access. Enum values are also pulled via
`lua_tonumber` with no range check.

### 16. RETRACTED — `WinLib/` is not dead

This audit's first pass copied `MODERNIZATION.md`'s claim that `Client/WinLib/`
is "effectively dead: `CWinUpdate` has no subclasses and no live callers." That
claim is **false**, and was caught only when preparing to act on it:

| Evidence | Location |
|---|---|
| `class CGameUpdate : public CWinUpdate` | `Client/CGameUpdate.h:13` |
| `#include "WinLib/WinLib.h"` | `Client/Client.h:34` |
| live `CWinUpdate* pCurrentUpdate = g_pUpdate;` | `Client/Client.cpp:3417` |
| globbed into the build | `dkrix/CMakeLists.txt:531` |

14 references exist outside `Client/WinLib/`. Retiring it is a refactor of the
client update path, not a deletion. `MODERNIZATION.md` has been corrected and
its Phase 1 "Delete `Client/WinLib/`" item withdrawn.

**Process lesson:** two of this repo's "provably dead" claims have now failed
on inspection — this one and the Phase 4 premise. Verify liveness by grepping
for *subclassing and pointer use*, not just `#include`, and check the CMake
source lists before believing anything is unbuilt.

### 19. Eight dead `*PackList555/565` files — Priority 10

Genuinely dead, verified three ways:

- `CAlphaSpritePackList{555,565}.cpp` and `CSpritePackList{555,565}.cpp` are
  **not in `SPRITELIB_SOURCES`** (`Client/SpriteLib/CMakeLists.txt:32-90`), and
  `add_library(SpriteLib STATIC ${SPRITELIB_SOURCES})` is the only consumer —
  so they are not compiled at all.
- The only references to `CSpritePackList555/565` outside their own files are
  **commented out** (`VS_UI/src/VS_UI_Item.cpp:280,282`,
  `VS_UI/src/VS_UI_util.cpp:686,688`).
- `CAlphaSpritePackList555/565` have zero references of any kind.

Their four `.h` files appear in `SPRITELIB_HEADERS` (lines 103, 104, 118, 119),
a variable that is **set and never used**. Removing those lines is inert.

312 lines across 8 files. Note this is *twice* what ADR-0001 originally
identified — the ADR listed only the `CAlphaSpritePackList` pair.

### 14. Tracked build artifacts — Priority 20

`dkrix/compile_commands.json` (3.4 MB) and `dkrix/Client/compile_commands.json`
(3.2 MB) are tracked. These are generated output and should be gitignored. Also
tracked: `dkrixserver/src/server/old_update.tar` (2.2 MB) and two SQL backups
(7.2 MB + 3.6 MB).

Good news the plan hasn't recorded: `Darkeden.rar` (~700 MB) and the `gameserver_*.out`
logs are **not** tracked — the `.gitignore` added in Phase -1 is doing its job.
Total `.git` is 46 MB, which is healthy.

---

## Phased remediation

Sequenced so that nothing requiring a build lands before a build exists.

### Now — no build required (items 2, 3, 5, 14, 16, 17)

Judgeable by reading. Roughly a day.

1. [ ] Correct `SPRITELIB_BACKEND_README.md` — drop the false `engine/sprite`
       attribution and the "Production Ready / tests passing" claims
2. [ ] Correct the Phase 4 section of `MODERNIZATION.md`; link ADR-0001
3. [ ] Fix `docs/README.md`'s three-repo description
4. [ ] Gitignore `compile_commands.json`; untrack both copies
5. [ ] Delete the 8 dead `*PackList555/565` files and drop lines 103, 104, 118,
       119 from `Client/SpriteLib/CMakeLists.txt` — see item 19 for the evidence.
       ~~Delete `Client/WinLib/`~~ — **retracted, see item 16. Do not.**
6. [ ] **Resolve the branch question** and push

### Next — the gate (item 1)

7. [ ] Iterate CI to green on both trees. Expect portability failures first.
8. [ ] Add `engine/sprite`'s 11 test files to the server/tools CI job — first real
       automated signal this project has ever had

*Everything below this line stays blocked until step 7 is green.*

### Then — cheap and mechanical (items 4, 6, 11)

9. [ ] Extract the 34 conf passwords to environment variables; document in the
       server runbook
10. [ ] Resolve the two genuine duplicate-path files; let the linker confirm the
        other seven
11. [ ] Archive the 10 dead server `main.cpp` entry points (Phase 7)

### Then — structural (items 7, 9, 12, 13)

12. [ ] Replace `file(GLOB)` with explicit source lists, mirroring `dkrixserver/`
13. [ ] Collapse the two SDL2 backends (ADR-0001, step 4b)
14. [ ] Shrink `Platform.h` toward 600 lines
15. [ ] Retire `DXLib/`'s 46-file shim

### Deferred — needs evidence or a rendering harness (items 15, 18, and Phase 4c)

16. [ ] Re-scope the SQL work off the confirmed 26 sites, not the phantom 87
17. [ ] Audit shipped SPK assets for real pixel encodings before touching the
        555/565 serializers
18. [ ] `CSpriteSurface.cpp` — needs a rendering test harness first; do not start
        without one

---

## Appendix — commands

From `newdk/`:

```bash
# scale
# NOTE: `wc -l | tail -1` is WRONG here. On large file sets xargs splits into
# batches and each batch prints its own "total", so tail -1 reports only the
# last batch. Sum per-file with awk instead. An earlier pass of this audit
# undercounted the server by 4x from exactly this mistake.
find dkrix \( -path '*/build*' -prune \) -o \( -name '*.cpp' -o -name '*.h' \) -print \
    | xargs wc -l | awk '$2!="total"{s+=$1} END{print s}'   # 652346
find dkrixserver/src \( -name '*.cpp' -o -name '*.h' \) -print \
    | xargs wc -l | awk '$2!="total"{s+=$1} END{print s}'   # 632669

# item 1 / 3 — branch state
git rev-parse --abbrev-ref --symbolic-full-name @{u}                    # fatal: none
git rev-list --left-right --count origin/main...HEAD                    # 0  4
git rev-list --left-right --count origin/modernize/phase4-sprite...HEAD # 106  4

# item 4 — secrets
grep -rn "PASSWORD" dkrixserver/conf/ | wc -l                           # 34
git ls-files dkrixserver/conf/ | wc -l                                  # 13

# item 6 / 7 — build hygiene
grep -c "FILTER\|REMOVE_ITEM" dkrix/CMakeLists.txt                      # 34
grep -c "file(GLOB"           dkrix/CMakeLists.txt                      # 2
find dkrix -name 'GCNotifyWin.cpp'                                      # 2 paths

# item 8 — tests
ls dkrix/tools/engine/sprite/tests/*.c | wc -l                          # 11 files
find dkrix/tools/engine/sprite/tests -name '*.c' -print \
    | xargs wc -l | awk '$2!="total"{s+=$1} END{print s}'                # 3898 lines

# item 10 — lua
grep -rn "luaL_openlibs" dkrixserver/src/

# item 12 / 13 / 16
wc -l dkrix/basic/Platform.h                                            # 1996
ls dkrix/Client/DXLib/ | wc -l                                          # 46
ls dkrix/Client/WinLib/                                                 # 3 files

# item 15 — the disputed figure
grep -rn "sprintf(query" --include=*.cpp --include=*.h dkrixserver/src/ | wc -l  # 26
grep -rn "sprintf" --include=*.cpp --include=*.h dkrixserver/src/server/ | wc -l # 426

# item 14 — what's actually tracked
git ls-files -z | xargs -0 du -h | sort -rh | head -8
git ls-files --error-unmatch Darkeden.rar          # not tracked — .gitignore works
```
