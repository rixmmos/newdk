# Tech Debt Audit — 2026-08-06

> ## ⚠️ Read this before acting on anything below
>
> **Decision 2026-08-06:** the local line continues as `main`; the modernization
> line is parked at tag `archive/modernization-phases-1-17`. That makes this
> audit **valid again** — it measures the tree that is now the line of record.
>
> The caveat that remains: for most items below, the parked tag already contains
> a finished implementation. Before starting any of them, look at how it was done
> there and lift the approach. `git show archive/modernization-phases-1-17:docs/MODERNIZATION.md`
> has the phase-by-phase record. The comparison table below is retained for that
> purpose.
>
> ---
>
> This audit measures the **local** branch (then named `modernize/phase-4-sprite`,
> now `main`), which is
> `origin/main` + a checkpoint. `origin/main` has only **4 commits** and ends
> 2026-04-17 — it is the initial import, not a trunk.
>
> `origin/modernize/phase4-sprite` (no hyphen) has **110 commits** and contains
> 106 commits of modernization work from 2026-04-17 to 04-22 that this branch
> does not have. It is **far ahead** on exactly the items ranked below:
>
> | | this branch | `origin/modernize/phase4-sprite` |
> |---|---|---|
> | `Client/DXLib/` files | 46 | **0** — Phase 3 done |
> | `Client/Platform/` files | 0 | **20** — the replacement exists |
> | SpriteLib 555/565 variants | 12 | **0** — Phase 4 done |
> | `basic/Platform.h` lines | 1,996 | 1,951 |
> | `dkrix/.clang-format` | absent | **present** |
> | `conf/*.conf.template` | absent | **present** |
> | SQL-injection / packet / socket ratchet scripts | absent | **present, with baselines** |
> | CI workflow files | 2, written today | **4**, incl. `ratchets.yml` |
>
> That branch also carries `docs/archive/2026-engine-sprite/`, meaning the
> Phase 4 sprite decision was already made and acted on there — before
> `ADR-0001` in this tree re-derived it from scratch.
>
> 4,132 files differ between the two branches. 99 files exist only on the remote
> branch; 622 exist only here, and most of those are files the remote branch
> **deleted as dead code**.
>
> **Item 3 (the branch question) is now closed** — resolved in favour of this
> line. Every other item stands as written and is real work still to do here.

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
| 1 | ~~CI has never executed~~ → **server and client both GREEN** (see `MODERNIZATION.md` for current run numbers) | Infrastructure | 4 | 4 | 2 | **32** |
| 2 | `SPRITELIB_BACKEND_README.md` claims "Production Ready" falsely | Documentation | 3 | 4 | 1 | **35** |
| 3 | Branch has no upstream; 106-commit divergence | Infrastructure | 4 | 4 | 2 | **32** |
| 4 | 34 plaintext DB passwords in 13 tracked conf files | Security | 2 | 5 | 2 | **28** |
| 5 | Phase 4 premise in `MODERNIZATION.md` is factually wrong | Documentation | 3 | 3 | 1 | **30** |
| 6 | Duplicate-symbol `.cpp` files across two directories | Code | 3 | 3 | 2 | **24** |
| 7 | CMake `file(GLOB)` + 34 `FILTER`/`REMOVE_ITEM` lines | Architecture | 4 | 3 | 3 | **21** |
| 8 | Zero tests on 1.29M LOC (except `engine/sprite`'s 11 files) | Test | 4 | 5 | 4 | **18** |
| 9 | Two independent SDL2 backends | Architecture | 3 | 3 | 3 | **18** |
| 10 | ~~Lua quest scripts get full `io` via `luaL_openlibs`~~ → **whitelisted to base/string/math** (see item 10) | Security | 1 | 5 | 3 | **18** |
| 11 | 10 dead server `main.cpp` entry points | Code | 2 | 2 | 2 | **16** |
| 12 | `basic/Platform.h` at 1,996 lines | Architecture | 4 | 3 | 4 | **14** |
| 13 | `DXLib/` — 46 files, 10,163 LOC vestigial shim | Architecture | 3 | 2 | 3 | **15** |
| 14 | Build artifacts tracked in git (~6.6 MB `compile_commands.json`) | Infrastructure | 2 | 2 | 1 | **20** |
| 15 | 26 `sprintf(query, …)` SQL construction sites | Security | 2 | 4 | 4 | **12** |
| 16 | ~~`WinLib/` — 3 files, zero callers~~ **retracted, see below** | Code | — | — | — | **—** |
| 19 | 8 dead `*PackList555/565` files, not in the build | Code | 1 | 1 | 1 | **10** |
| 20 | 66 dead `__LINUX__`/`__WINDOWS__` directives in 16 files | Code | 2 | 3 | 2 | **20** |
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

### 1. CI has never executed — **SERVER GREEN 2026-08-06.** Priority 40 → 20 (client only)

> **The server build is green.** `make debug` completes on a clean Ubuntu
> runner; `gameserver`, `loginserver` and `sharedserver` are all produced and
> verified present. The `clang-format` job is green and genuinely inspecting
> files.
>
> Three things got it there:
> - `60f4c35` — the format job could not fail; it skipped every file.
> - `b8c24f0` — `ld: cannot find -lnsl`. libnsl left glibc in 2.32 and is not
>   preinstalled on `ubuntu-latest`. The build had already reached 86%, so the
>   tree itself was fine.
> - Nothing else. The failure this document predicted — Windows portability
>   debt — was measured and disproved beforehand and never appeared.
>
> **What it buys.** This audit was written under "no automated signal exists".
> For `dkrixserver/` that is over. Items 6, 7, 11, 12, 13, 16 and 20 are now
> checkable by machine on the server side, and the phases `MODERNIZATION.md`
> lists as delegable-once-green (1, 2, 3, 7, 10) are delegable there today.
>
> **[Update] The client is green too.** Run #3 died at *configure* on a stale
> generator pin; that and two further fixes (see `MODERNIZATION.md`) got
> client CI to a verified green run. Both toolchains now have real CI signal —
> this item is closed. Left in place for the history of how it got there.
>
> Rescored on the client alone: **I=4, R=4, E=2 → 32.** Lower than the original
> 40 because half the original scope is delivered and the remaining half has a
> fix already in flight — not because the client matters less. It is still the
> highest-priority item in this document.

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

> **[2026-08-06] One workflow bug found and fixed before the first run.**
> `server.yml` set `working-directory: dkrixserver` at workflow level, which also
> applied to the `format` job. `git diff --name-only` emits repo-relative paths,
> so `[ -f "$f" ]` looked for `dkrixserver/dkrixserver/src/…`, skipped every
> file, and the job exited green having checked nothing. Reproduced both ways in
> a scratch repo (old: `checked=0`, exit 0; new: `checked=1`, exit 1) and fixed
> in `60f4c35`, which also adds a guard that fails when a non-empty change list
> inspects zero files.
>
> Worth generalising: **a CI job that cannot fail is worse than no CI job**,
> because it reads as evidence. Anything added here should be tested against a
> case it is supposed to reject before it is trusted.

### 20. Dead `__LINUX__` / `__WINDOWS__` conditionals — Priority 20 — **addressed 2026-08-09**

> **Partially converted.** CI went green on both trees (2026-08-06/07), which
> lifted the "do not do it before CI is green" gate this item carried. A
> re-measurement on 2026-08-09 found 75 raw occurrences of `__LINUX__` /
> `__WINDOWS__` across 19 files (the file count grew from 16 because the
> original sweep undercounted files where every occurrence sits inside a
> comment — see below). Of those, **63 were converted** to the real
> `PLATFORM_LINUX` / `PLATFORM_WINDOWS` macros `SocketAPI.h` already
> establishes; **1 was deliberately left dead** (behavior-change risk, not a
> compile risk); **11 were left untouched** because they are not live
> directives at all (inside `//` or `/* */` comments, or an unreachable `#elif`
> arm of an always-true `#ifdef __GAME_CLIENT__`).
>
> **Converted (bucket a — mirrors already-live code, high confidence it
> compiles):**
> - `Client/Packet/Assert.h:25`, `Assert1.h:25` — the `#elif __LINUX__` arm of
>   the `Assert(expr)` macro chain now reads `defined(PLATFORM_LINUX)`.
> - `Client/Packet/Datagram.h:16,20` — socket header include guard.
> - `Client/Packet/Exception.h:16` (`#pragma warning` guard) and `:148`
>   (`__BEGIN_DEBUG`/`__END_DEBUG` OR-chain).
> - `Client/Packet/FileAPI.cpp` — all 21 occurrences (every `open_ex`/
>   `read_ex`/`write_ex`/`close_ex`/`fcntl_ex`/`ioctl_ex`/`dup_ex`/`lseek_ex`
>   POSIX branch). Verified line-for-line against `dkrixserver/src/Core/
>   FileAPI.cpp`, which compiles this same code today under the server's own
>   (real, CMake-defined) `__LINUX__` — see below.
> - `Client/Packet/SocketAPI.cpp` — all 25 occurrences, same rationale as
>   FileAPI.cpp; compared against `dkrixserver/src/Core/SocketAPI.cpp`.
> - `Client/Packet/SocketImpl.cpp:17,20` and `SocketInputStream.cpp:22,24,602,649`.
> - `Client/Packet/Types/SystemTypes.h:23,46,58,61` — this header does its own
>   independent `_WIN32`/`__APPLE__`/`__linux__` platform detection (identical
>   to `SocketAPI.h`'s), so the fix is self-contained regardless of include
>   order or which target compiles it. Note: line 46/58's `__LINUX__` term was
>   previously being masked by a real (but separately buggy) CMake definition
>   — `dkrix/CMakeLists.txt` sets `PLATFORM_MACOS` on `if(NOT WIN32)` for the
>   `DarkEden`/`VS_UI`/`TextSystem` targets, i.e. it mislabels a Linux build as
>   macOS. That bug is out of scope here (it's not a `__LINUX__`/`__WINDOWS__`
>   conditional) but is worth its own item — flagging for a future audit pass.
> - `Client/Packet/Lpackets/LCDeletePCOKHandler.cpp:50` — **only** the
>   `#elif __WINDOWS__` arm. See below for why the sibling `#if __LINUX__` arm
>   in the same block was left alone.
>
> **Deliberately left dead (bucket b — 1 occurrence):**
> - `Client/Packet/Lpackets/LCDeletePCOKHandler.cpp:34` (`#if __LINUX__`).
>   The `#elif __WINDOWS__` arm right below it was converted because it
>   mirrors the sibling `LCCreatePCOKHandler.cpp`'s unconditional (always-live)
>   body almost line for line — `UI_DeleteCharacterOK()`, send `CLGetPCList`,
>   `setPlayerStatus(CPS_AFTER_SENDING_CL_GET_PC_LIST)`, plus
>   `g_ModeNext = MODE_WAIT_PCLIST`. The `__LINUX__` arm, by contrast, only
>   prints a `cout` banner and sends `CLGetPCList` — it skips the UI call and
>   the mode transition entirely, which reads as a console-era leftover from
>   before this client had an SDL2 GUI on Linux too (the `sanitizers-linux` CI
>   job now builds the real `DarkEden` GUI target on Linux, not a headless
>   variant). Activating it as-is would give Linux a different, weaker
>   post-delete state than Windows — a gameplay-behavior change, not just a
>   dead-code fix, so it wasn't converted. Left as `__LINUX__` (still
>   permanently inert); worth a real look at what the Linux/SDL2 path should
>   do before touching it.
>
> **Left untouched — not live directives (bucket c — 11 occurrences, 9 files):**
> - `Client/Packet/Cpackets/CGAttack.h:18-20`, `CGBloodDrain.h:18-20` — the
>   `#ifdef __LINUX__` body is only a commented-out `#include`; nothing to
>   activate either way.
> - `Client/Packet/Gpackets/GCMoveErrorHandler.cpp:35` — the text
>   `#elif __WINDOWS__` sits inside a `/* ... */` block comment (comment
>   removal happens before directive parsing), so it was never a real
>   directive; the code around it is guarded only by the real, always-defined
>   `__GAME_CLIENT__` and already runs unconditionally today.
> - `Client/Packet/Gpackets/{GCPetUseSkillHandler.cpp:207,
>   GCSMSAddressListHandler.cpp:32}` — real `#elif __WINDOWS__` directives, but
>   as the *alternative* arm to `#ifdef __GAME_CLIENT__` (not nested inside
>   it). `__GAME_CLIENT__=1` is always defined for the `DarkEden` target
>   (`dkrix/CMakeLists.txt`), so this `#elif` is permanently unreachable
>   regardless of `__WINDOWS__`'s spelling, and both bodies are empty anyway.
> - `Client/Packet/Gpackets/{GCAddNicknameHandler.cpp:26,
>   GCExecuteElementHandler.cpp:47, GCGQuestInventoryHandler.cpp:61,
>   GCNicknameListHandler.cpp:28}` — `//`-commented, not directives.
>
> Preprocessor balance (`#if`/`#ifdef`/`#ifndef` count vs `#endif` count) was
> checked in every touched file before and after; all ten stayed balanced.
> `docs/adr/`-style deep dive not warranted — this was a mechanical macro
> rename plus a documented set of skips, not a redesign.
>
> **[measured 2026-08-06, superseded above]** 66 `#if __LINUX__` /
> `#elif __WINDOWS__` directives across 16 files, mostly under
> `Client/Packet/`. **Neither macro was defined anywhere** — not by CMake, not
> by any header, not by any compiler. Both arms evaluated false on every
> platform, so every one of those blocks was inert.
>
> The tree built regardless because `Client/Packet/SocketAPI.h:12-21` performs
> real platform detection from `_WIN32` / `__APPLE__` / `__linux__`, defines
> `PLATFORM_WINDOWS` / `PLATFORM_LINUX` / `PLATFORM_MACOS`, and includes the
> right socket headers. Everything else picks them up transitively.

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

### 4. Plaintext credentials in tracked config — Priority 28 — **mechanism landed 2026-08-06**

> **Half done.** `Properties::load()` now expands `${VAR}` against the
> environment, and `conf/{gameserver,loginserver,sharedserver}.conf.template`
> exist with 16 `DKRIX_*` placeholders covering every credential and host/IP.
> Provable no-op on the current deployment — no conf file contains `${`.
>
> **Still open:** the plaintext values remain in the tracked conf files and in
> git history. Removing them means rotating the deployment to the template flow
> and validating against a running server, which is why it did not land with the
> mechanism. Priority unchanged until that happens.

34 `PASSWORD` lines across 13 tracked files in `dkrixserver/conf/`, including a
`conf/backup/` set that duplicates them. Sample: a `DB_PASSWORD : <redacted>` line
appears in `excel96-gameserver.conf`, `excel96-gameserver2.conf`,
`excel96-loginserver.conf`, `excel96-sharedserver.conf`; `gameserver.conf` carries a
second one. Dev IPs (`192.168.0.16`) are alongside them.

**Resolved 2026-08-10.** All 13 are untracked: `c587490` and `0bdc648` took
`conf/{game,login,shared}server.conf` and `docker/conf/*.conf`; the remaining 10
(`excel96-*`, `.new`, `conf/backup/*`, `updateserver.conf`) went in this wave, with
`.gitignore` widened to cover `*.new` and `conf/backup/` — neither of which the
pre-existing `conf/*.conf` rule matched. Only the three `.conf.template` files remain
tracked. The literal values are redacted above because this repo has been public since
2026-08-08; the committed credential is stale, and the live one was never committed
(`docs/SECURITY-AUDIT-2026-08-10.md` §3). Rotation is still an open operator action.

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

#### 7a. Which exclusions are actually load-bearing

**[measured 2026-08-06]** Each `FILTER`/`REMOVE_ITEM` line was evaluated against
the real file tree by expanding the same globs the build uses and testing every
regex against the result.

Of 25 pattern-based exclusions, **8 match nothing** — they exclude files that no
longer exist:

| Line | Pattern | Why it is dead |
|---|---|---|
| 576 | `.*Imm.*` | no matching client source |
| 577 | `.*D3D.*` | `Client/D3DLib/` was deleted |
| 578 | `.*Direct3.*` | same |
| 580 | `.*EXECryptor.*` | gone |
| 582 | `.*\.bak.*` | defensive; nothing matches |
| 583 | `.*_bak-.*` | `MItemTable_bak-2007-5-7.cpp` deleted in Phase 1 |
| 598 | `.*VolumeOut.*\.cpp` | gone |
| 601 | `WinLib/CWinMain\.cpp` | no such file |

The other 17 are live and genuinely suppress files.

All 10 explicit `REMOVE_ITEM` entries (line 546) still resolve to existing
files, so that block is entirely load-bearing.

**Deliberately not "cleaned up."** Removing a filter that matches nothing is a
no-op today, so it buys nothing, and two of the eight (`.bak`, `_bak-`) are
defensive against files reappearing. The real fix is the one item 7 already
names — explicit source lists, as `dkrixserver/` uses — which makes the whole
category disappear rather than trimming it. Doing that needs a build.

The value of this measurement is knowing that **17 exclusions are hiding real
files** from a glob. That is the actual fragility: the build's contents depend
on regexes evaluated against whatever happens to be on disk.

### 8. Test debt — Priority 18

`tools/engine/sprite/tests/` holds 11 test files (3,898 lines). That is the **entire
automated test surface for ~1.29 million lines of code**, and it covers a library the
game client does not link. The server's `test`/`testserver`/`testAlone` directories are retired binaries,
not test suites — `testAlone/Mutex.h` defines a "mutex" whose lock/unlock just sets
a bool.

Wiring `engine/sprite`'s existing tests into CI is the cheapest possible first
green check and should ride along with item #1.

### 10. Lua sandbox — Priority 18 — **addressed 2026-08-07** (docs caught up 2026-08-09)

> **Fixed.** `luaL_openlibs()` (single call site, `LuaState::init()` in
> `dkrixserver/src/server/gameserver/quest/luaScript/LuaState.cpp`) is
> replaced with an explicit whitelist: `base`, `string`, `math` only, loaded
> via the same `lua_pushcfunction` + `lua_pushstring` + `lua_call(L, 1, 0)`
> idiom Lua 5.1's own `linit.c` uses. `io`, `os`, `debug`, `package`
> (`require`/`loadlib`), and `table` are all dropped. Commits: `45d10fc`
> (2026-04-19, initial `base`/`table`/`string`/`math` whitelist), `b204ebd`
> (2026-08-07, re-verified against a real WSL `liblua5.1-0-dev` build and
> narrowed further to drop `table` too).
>
> **Inventory — every real `*.lua` under the repo, re-confirmed 2026-08-09:**
> 18 files (15 under `dkrixserver/data/lua/**`, 3 under
> `dkrixserver/src/server/gameserver/quest/luaScript/test/*.lua`; the sibling
> `test/lt` is a leftover compiled ELF, not a script). Grepped for
> `io.`, `os.`, `debug.`, `require`/`package.`, `table.`, `string.`, `math.`:
> **zero matches of any kind, in any file.** The only stdlib-shaped calls
> anywhere are bare `random(1, getn(t))` in the six `xmasEventCommon.lua`
> variants — Lua 4.x/5.0-era globals, not `math.random`/`table.getn`. A
> throwaway C harness against this WSL image's actual `liblua5.1-0-dev`
> (`LUA_COMPAT_GETN` is `#undef`'d in its `luaconf.h`) confirmed both
> `random` and `getn` are already nil globals even under the old,
> unrestricted `luaL_openlibs()` — those six scripts' `selectOne()` already
> threw "attempt to call a nil value" before this change, on `main`,
> independent of the sandbox. Dropping `table` from the whitelist does not
> make them any more broken than they already were.
>
> **Kept:** `base`, `string`, `math`. This integration is one-directional —
> C++ `luaL_dofile`s a script (`LuaSelectItem::executeFile()`) and then
> `lua_getglobal`s the four result variables the script sets
> (`ItemClass`/`ItemType`/`OptionType`/`OptionType2`); there is no
> `lua_register`/`luaL_register` anywhere in `dkrixserver/src` exposing
> custom C++ game functions (e.g. `GiveItem`, `Say`) back into Lua — checked
> repo-wide, not just under `quest/`. So `base`/`string`/`math` are kept
> purely because the scripts' own control-flow and table-building logic use
> them, not because any C++ callback surface depends on the standard
> library being present.
> **Dropped:** `io` (filesystem read/write), `os` (`os.execute`,
> `os.remove`, `os.rename`, `os.getenv`, …), `debug` (VM reflection /
> sandbox escape), `package` (`require`/`loadlib`, dynamic C module
> loading), `table` (unused — see inventory above). No script exercises any
> function in a dropped library, dangerous or otherwise — no stop-and-report
> case.
>
> The enum-range-check half of this item's finding (`lua_tonumber` cast with
> no bounds check) is also fixed, as a separate follow-on: `LuaState.h`'s
> `lua_toboundedenum<T>()` wraps all four call sites in
> `LuaSelectItem.cpp::executeFile()` (`ItemClass`, `ItemType`, `OptionType`,
> `OptionType2`), throwing `InvalidProtocolException` on a non-numeric or
> out-of-range slot instead of silently reinterpret-casting. See
> `docs/MODERNIZATION.md` Phase 9 for the full writeup and the build
> verification (`make debug` in WSL, all three binaries link clean).
>
> **Original finding, for record:**

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

### Now — no build required — **all done 2026-08-06**

1. [x] ~~Correct `SPRITELIB_BACKEND_README.md`~~ — dropped the false
       `engine/sprite` attribution, the "Production Ready / tests passing"
       claims, and a Testing section citing three binaries that never existed
2. [x] ~~Correct the Phase 4 section of `MODERNIZATION.md`~~; ADR-0001 linked
3. [x] ~~Fix `docs/README.md`'s three-repo description~~
4. [x] ~~Untrack both `compile_commands.json`~~ (6.6 MB)
5. [x] ~~Delete the 8 dead `*PackList555/565` files~~ and the 4 header lines in
       `Client/SpriteLib/CMakeLists.txt`.
       ~~Delete `Client/WinLib/`~~ — **retracted, see item 16. Do not.**
6. [x] ~~Resolve the branch question~~ — local promoted to `main`, the
       modernization line parked at tag `archive/modernization-phases-1-17`.
       See `BRANCH-RECONCILIATION.md`.
7. [x] ~~Land the `${VAR}` secrets mechanism~~ (item 4, half) — provable no-op
       on the current deployment
8. [x] ~~Fix the CI format job~~ — it passed green without inspecting anything
9. [x] ~~Lift the smoke-test runbook onto `main`~~ — `docs/smoke-test/`

### Next — the gate (item 1)

10. [ ] Iterate CI to green on both trees. **Not** the portability debt this
        document used to predict — that claim was false, see the note under
        item 1. Cause unknown until the first run.
11. [ ] Add `engine/sprite`'s 11 test files to the CI job — first real
        automated signal this project has ever had

*Everything below this line stays blocked until step 10 is green.*

### Then — cheap and mechanical (items 4, 6, 11, 20)

12. [ ] **Finish the secrets rotation** — the mechanism landed; the plaintext
        is still in the tracked confs and in git history. Needs a running
        server: `.template` → `.conf` on the host, export the 16 `DKRIX_*`
        vars, restart, confirm it connects.
13. [ ] Resolve the two genuine duplicate-path files; let the linker confirm
        the other seven
14. [ ] Archive the 10 dead server `main.cpp` entry points (Phase 7)
15. [ ] Replace the 66 dead `__LINUX__`/`__WINDOWS__` directives with the
        `PLATFORM_*` macros `SocketAPI.h` establishes (item 20)

### Then — structural (items 7, 9, 12, 13)

16. [ ] Replace `file(GLOB)` with explicit source lists, mirroring
        `dkrixserver/`. 17 exclusions currently hide real files — see 7a.
17. [ ] Collapse the two SDL2 backends (ADR-0001, step 4b)
18. [ ] Shrink `Platform.h` toward 600 lines
19. [ ] Retire `DXLib/`'s 46-file shim

### Deferred — needs evidence or a rendering harness (items 15, 18, and Phase 4c)

20. [ ] Re-scope the SQL work off the confirmed 26 sites, not the phantom 87
21. [ ] Audit shipped SPK assets for real pixel encodings before touching the
        555/565 serializers. If Phase 4 is revived, port `ebcfe52` from the
        parked tag rather than re-deriving it.
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
