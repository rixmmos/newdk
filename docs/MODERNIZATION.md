# MODERNIZATION.md — Running Plan

This is the single source of truth for the DarkEden-like project's
modernization work, spanning `dkrixserver/`, `dkrix/`, `docs/`, configs, and
tooling. It supersedes the per-area status documents archived under
`dkrix/docs/archive/2026-migration-notes/`.

When anything here falls out of date, update this file as part of the
change that made it stale — not after the fact.

> **Path note (2026-08):** earlier revisions of this file referred to
> `client/` and `server/`. The real directories are `dkrix/` (client) and
> `dkrixserver/` (server). See `../CLAUDE.md` for the full layout.

## Branch decision — 2026-08-06

This plan describes the **local operational line**, now `main`. It is the tree
that builds, runs the live server, and ships to testers (last release
2026-04-30).

A parallel agent-driven line took this same plan through **Phases 1–17** between
2026-04-17 and 04-22 and reached an end-to-end login→gameplay smoke test. It was
never merged here and has been **parked**, not deleted:

- tag `archive/modernization-phases-1-17` (`1c861f5`)
- branch `origin/modernize/phase4-sprite` on GitHub

**Consequence for this document:** the phase statuses below describe *this*
line only. Where a phase is marked "not started" here, it may well be finished
on the parked tag. Before starting any phase, check what the tag did with it —
`git log archive/modernization-phases-1-17 --oneline --grep "Phase N"` — and
lift the approach even if you don't lift the commits.

Rationale and the full comparison: `BRANCH-RECONCILIATION.md`.

## Verification status — read this first

Every "done" claim below is either **[measured]** (re-verified against the
working tree on 2026-08-06 by direct file inspection) or **[unverified]**
(asserted by a previous pass, not re-checked, and not confirmed by a build).

> ## [2026-08-06] THE SERVER BUILD IS GREEN
>
> First green CI run in this project's history. `make debug` completes on a
> clean Ubuntu runner and all three binaries — `gameserver`, `loginserver`,
> `sharedserver` — are produced and verified present. The `clang-format` job is
> green too, and green *meaningfully*: `60f4c35` fixed the path bug that had
> made it skip every file and pass regardless.
>
> Getting there took two fixes and one correction:
> - `60f4c35` — the format job inspected nothing and could not fail.
> - `b8c24f0` — `ld: cannot find -lnsl`. libnsl left glibc in 2.32 and is not
>   preinstalled on `ubuntu-latest`; added `libnsl-dev`. The build had already
>   reached 86% before this, so most of the tree was fine.
> - The predicted cause — Windows portability debt in `Platform.h` — was
>   measured and found false before the run, and played no part.
>
> **What this changes.** Everything in this document was written under
> "no automated signal exists". For the server, that is over. A change to
> `dkrixserver/` can now be proposed by anyone and judged by machine. The
> phases this file lists as delegable-once-green — 1, 2, 3, 7, 10 — are
> delegable for the server as of now.
>
> **[2026-08-06, later] THE CLIENT BUILD IS GREEN TOO.** CI reached real
> client code for the first time today, and found two genuine bugs — never
> caught before because nothing had ever compiled `dkrix/` as x64 CI:
>
> - Run #4 (`21a9172`) got past *configure* and through 23m of compilation,
>   then failed at **link**: 5 unresolved externals, all `C_VS_UI_WEBBROWSER`
>   methods referenced from `VS_UI_Game.cpp`. Root cause: `dkrix/CMakeLists.txt`
>   excluded `VS_UI_WebBrowser.cpp` — the file with the real COM/ActiveX
>   implementation — specifically `if(WIN32)`, backwards from what
>   `VS_UI_WebBrowser.h` expects (real class on Windows, inline stub
>   elsewhere). Fixed in `84f2e5a` by moving the exclusion to `if(NOT WIN32)`.
> - Run #5 (manual dispatch against that fix) got further — the `.cpp` now
>   compiled — and failed in 10m at a genuine **compile** error instead:
>   `VS_UI_WebBrowser.cpp:61` passed `(LONG*)` where
>   `IWebBrowserApp::get_HWND` wants `SHANDLE_PTR*` (pointer-sized; `LONG` is
>   4 bytes even on x64). Classic VC6/32-bit-era code, never built as x64
>   until now. Fixed in `90f6106`: cast to `(SHANDLE_PTR*)` instead, matching
>   the real `HWND` size of `m_hWnd_Explorer`.
> - **Run #6 (manual dispatch, `90f6106`) — Status: Success, 28m23s.** Only
>   Node.js-runner deprecation warnings. Run #7, dispatched automatically
>   against `84f2e5a` alone (fix #1 without fix #2), **failed** with the same
>   `SHANDLE_PTR` compile error as run #5 — cross-confirms both fixes were
>   independently necessary.
>
> **What this changes.** Both toolchains are now judged by machine. A change
> to `dkrix/` or `dkrixserver/` can be proposed by anyone and verified without
> the workstation owner. Every phase this file lists as delegable-once-green
> is now delegable for both trees.
>
> **Caveat:** GitHub's push-triggered runs lagged their push by 20+ minutes
> both times today (runs #4 and #7) — manual `workflow_dispatch` returned
> results in seconds. If a push-triggered run seems to be missing, that's
> most likely why; it is not evidence the push failed to register.

> **[2026-08-07] The merge wave landed.** Six merges this morning brought the
> first lifted phases onto `main` (tip `5ca240a`): Phase 1, Phase 2 (safe
> items), Phase 3 item 1, Phase 4 safe items, Phase 7, Phase 9 mechanical
> items, plus a 4/5/7 follow-up bundle. Verification is uneven by design:
> the server-side work (7, 9) was compile-verified with `make debug` in WSL
> on its review branches before merging; the client-side work (1, 2, 3, 4)
> is grep-verified only — run #6 predates all of it. **No full CI build
> against the merged tip has been confirmed.** The tip moved once already
> while this audit was in flight: `3117d21` (10:00) hand-fixed include
> order after the `clang-format` job flagged the wave — and the next run
> **still failed on the same three files**. The job checks whole files
> once touched, and hand-ordering can't reproduce `SortIncludes` +
> `IncludeBlocks: Regroup`. Fixed 2026-08-07 in the fmt pass committed
> with this note: all 28 dirty wave-touched server files reformatted with
> clang-format 18 (what the workflow's `apt-get install clang-format`
> yields on ubuntu-latest), verified against the workflow's exact check;
> zero `#include` lines moved and the whitespace-stripped byte stream of
> every file is identical — formatting only.
>
> **[2026-08-07, later] The build verdicts are in (read off Actions):**
> - **Client run #8, at the wave tip `5ca240a` — SUCCESS, 28m30s.** The
>   client-side lifts (Phases 1, 2 partial, 3.1, 4 safe items) are now
>   compile-verified, not just grep-verified.
> - **Server run #6, same tip — `make debug` GREEN, 21m32s.** The run's
>   Failure status came entirely from the 27s `clang-format` job
>   (5 files); zero build errors in its annotations. The server wave is
>   build-verified twice over (WSL pre-merge + CI post-merge).
> - **Run #9 (`8f4ca50`): `make debug` green; `clang-format` red again**,
>   on 2 of the 28 just-formatted files (`ActionRegisterSiege.cpp`,
>   `ActionRegisterReinforce.cpp`). Root cause measured, not guessed: the
>   workflow's unpinned `apt-get install clang-format` yields **18.1.3**
>   on the current runner image, and 18.1.3 is the only version tested
>   that flags those two files — 18.1.8, 19.1.7, 20.1.7, and 22.1.8 all
>   accept every wave file byte-for-byte as committed. (The trigger is
>   CJK-width handling in the Korean-comment quest files; 18.1.3 also
>   flags two siblings the wave never touched.) An unpinned formatter is
>   a nondeterministic gate. Fixed in the commit carrying this note:
>   `server.yml` now pins `clang-format==18.1.8` via pip and prints the
>   version into the log. Match it locally with
>   `pip install clang-format==18.1.8`. No source file needed changing.

> **[2026-08-07, later still] The "now deterministic" prediction above was
> wrong — two more red runs before green.** The very next push (`19f41c4`,
> run #12) failed `make debug (ubuntu)`: `undefined reference to
> PreparedStatement::execute()/bindString()/~PreparedStatement()` from
> `GCFriendChattingHandler.cpp` and `CGWhisperHandler.cpp`, linking
> `gameserver`.
>
> - **Root cause 1 — static-link order.** `gameserver`/`loginserver`/
>   `sharedserver` each listed their `*Database` static library *before*
>   the `*Packets`/`Core` libraries that consume it. GNU ld only pulls
>   symbols from a static archive to satisfy undefined references that
>   exist at the point the archive is processed — it never revisits an
>   earlier archive. Latent since `PreparedStatement` was introduced,
>   surfaced only when 11.2 batch 1 (`ab67704`) added the first
>   `PreparedStatement` call sites inside packet handlers. Fixed in
>   `ff96e46` — pure reordering (`*Database` moved after
>   `*Packets`/`Core`/etc.) in all three executables' `CMakeLists.txt`,
>   no logic change.
> - **Root cause 2 — the mirror-image bug.** Run #13 (`ff96e46`) got
>   `gameserver`/`loginserver` linking; `sharedserver` then failed with
>   the opposite-direction error: `libDatabase.a(Statement.cpp.o):
>   undefined reference to g_ProfileSampleManager /
>   ProfileSampleManager::getProfileSampleSet() /
>   ProfileSampleSet::beginProfile()/endProfile()`. `Statement.cpp` calls
>   into `Profile.cpp`'s query-profiling hooks, which are compiled into
>   `ServerCore` — but `sharedserver` links plain `Database` *after*
>   `ServerCore`. Rather than hand-tune the flat link-order list again
>   (it can't satisfy "*Packets before *Database" and "*Database before
>   ServerCore" at once without the full transitive graph), `421088e`
>   declares the real dependency instead: `target_link_libraries(Database
>   PUBLIC ServerCore)` (and the same for `GameServerDatabase` /
>   `LoginServerDatabase` / `SharedServerDatabase` /
>   `UpdateServerDatabase`) in `src/server/database/CMakeLists.txt`,
>   letting CMake's own transitive link-ordering resolve it instead of a
>   fragile manual list.
> - **Run #14 (`421088e`) — GREEN.** `make debug (ubuntu)` succeeded
>   (17m50s-class), `clang-format` and `ratchets` jobs also green, 21m39s
>   total. Confirmed by opening the run in GitHub Actions
>   (`rixmmos/newdk`, `server.yml`) and reading the job summary directly.
>
> This is also the first compile verification of 11.2 batch 1
> (`ab67704`) — both failures were link-order problems, not defects in
> the migrated call sites themselves, which compiled clean throughout.

**No claim in this document has been confirmed by a compile** — except what
the green runs actually verified (client #6 and #8, server `make debug` in
runs #5 and #6): those commits, on those runs, on `main`. Everything else below is still either
`[measured]` by direct file inspection or `[unverified]`, and a green CI run
today does not retroactively verify a change made tomorrow — re-run CI to
trust it.

### Open blockers

1. ~~**~6,825 files are modified and uncommitted**~~ — **resolved 2026-08-06.**
   Committed as checkpoint `067087f`. The working tree is clean; everything
   source-like is tracked. What remains untracked is only build output
   (`dkrixserver/bin/`, `lib/*.a`), game data (`Darkeden/`, `release_site/`),
   and generated manifests — all correctly excluded per `../CLAUDE.md`.
   Measurements below now describe a commit, not a floating working tree.
2. ~~**Branch name mismatch**~~ — **resolved 2026-08-06.** They were never the
   same branch. The local line is now `main`; the old name was retired to
   `retired/phase-4-sprite-misnomer`; the remote line is parked at tag
   `archive/modernization-phases-1-17`. See the branch decision above.
3. ~~**No CI, no tests, no client `.clang-format`.**~~ — **CI resolved
   2026-08-06** for both trees (see box above). Tests and a client
   `.clang-format` are still absent — there is still no automated signal
   for *correctness*, only for *does it build*.

## Ground truth (re-measured 2026-08-06)

The following is what the trees actually contain. Where it disagrees with a
per-area status doc, this file wins.

### Client (`dkrix/`)
- SDL2 is the only supported backend on every platform. The old Windows
  native backend has been deleted (`basic/PlatformWindows.cpp` is gone;
  `USE_SDL_BACKEND` is forced `ON` in `CMakeLists.txt`).
- The primary client validation path on this workstation is a native
  Windows build with Visual Studio 2022 and CMake. Older macOS
  `make debug-asan` notes are historical references, not the current
  source of truth for day-to-day client bring-up.
- `Client/D3DLib/` has been removed. The references to it in the client's
  `CLAUDE.md` are stale.
- **[measured 2026-08-06] `Client/WinLib/` is NOT dead — this file's earlier
  claim that "`CWinUpdate` has no subclasses and no live callers" is false.**
  `Client/CGameUpdate.h:13` declares `class CGameUpdate : public CWinUpdate`,
  `Client/Client.h:34` includes `WinLib/WinLib.h`, and `Client/Client.cpp:3417`
  holds a live `CWinUpdate*`. 14 references outside `Client/WinLib/` in total.
  `dkrix/CMakeLists.txt:531` globs `Client/WinLib/*.cpp` into the build.
  Do not delete it. Retiring it is a real refactor of the update path, not a
  deletion.
- **[re-measured 2026-08-07]** `Client/DXLib/`'s double layer is half gone:
  Phase 3 item 1 renamed `CSDLInput`→`InputManager` / `CSDLAudio`→
  `AudioManager` and deleted the dead `_Adapter.cpp` twins and shadow
  headers. 11 `CDirect*` files remain (`CDirectDraw*`, `CDirectMusic`,
  `CDirectSoundStream`, `CDirectSetup*`, plus `BIT_RES.*` alongside) —
  Phase 3 items 2–3 territory.
- `Client/SpriteLib/` still contains the 555/565/4444 pixel-format
  class explosion even though SDL handles pixel-format conversion
  natively. The `*_SDL.cpp` backend files are mostly stubs; actual
  drawing happens in the adapter.
- A second sprite system lives at `tools/engine/sprite/`; it powers the
  viewer binaries. We have two sprite systems today.
- **[measured] Text rendering is now unified.** `VS_UI/src/VS_UI_Base.cpp`
  no longer calls GDI on any platform — `Base::SetFont` routes through
  `TextSystem::EncodeFontHandle` and the file includes
  `TextSystem/FontHandleUtil.h`. This is Phase 5's headline item, and it
  landed **out of order**, ahead of Phases 2–4.
  - Caveat: `SetFont` still takes `LOGFONT &lf` and reads `lf.lfFaceName` /
    `lf.lfHeight`. The GDI *stubs* in `Platform.h` are therefore still
    load-bearing as a data structure, even though no GDI *call* remains.
    Removing them is a signature change across callers, not a deletion.
- **[measured 2026-08-06] `basic/Platform.h` is 1,995 lines.** The growth
  from the 1,968 baseline was a real Windows-compile-error fix, not drift —
  see Phase 2 below for the `git log -p` finding. Phase 2's safe sub-items
  (duplicate `id_t`, dead `_tcscpy`/`_tcscat` shims, one unchecked-HRESULT
  signature) are done; the two items that would actually move this number —
  mutex primitive unification and a full HRESULT-machinery audit — need a
  compiler to do safely and are reported as scoped-out, not attempted. `_T`
  and `stricmp` turned out to have live callers (doc was wrong to call them
  dead); `_itoa` was never in this file. `DeleteObject`, `LOGFONT`, and
  `CreateFontIndirect` are untouched, deferred to Phase 5 per instructions.
- **[re-measured 2026-08-07] The client build still uses
  `file(GLOB …) + FILTER EXCLUDE`** — 33 `FILTER`/`REMOVE_ITEM` lines in
  `CMakeLists.txt` (was 34). But the nine duplicate-symbol `.cpp` files and
  all 35 `Client/Packet/Cpackets/*Handler.cpp` files are **gone** (Phase 1,
  merged `04d3820`) — zero `*Handler.cpp` remain under `Cpackets/`.
- **[measured] `MItemTable_bak-2007-5-7.cpp` is gone** (14,965 lines). This
  is the one Phase 1 item that has actually landed.
- **[measured] `Client/WinLib/` still exists** — `CWinUpdate.cpp`,
  `CWinUpdate.h`, `WinLib.h`.
- **[re-measured 2026-08-07] `Client/DXLib/` holds 42 files** (was 46) —
  the `_Adapter.cpp` twins and dead `CDirectInput.cpp` went in Phase 3
  item 1. `CDirectDraw*`, `CDirectDraw_StaticMembers.cpp`,
  `CDirectSetupGetVersion.cpp`, and `BIT_RES.*` remain.
  `Client/Platform/` does not exist yet.
- **[re-measured 2026-08-07] `Client/SpriteLib/` holds 12 pixel-format
  variant files** (was 20 — the 8 dead PackList files went in `76f13e1`).
  The survivors are serializers, not renderers; their fate rests on the
  Phase 4c asset audit, not on the class count.
- **[measured] No `.clang-format` in `dkrix/`.** Format is still unenforced.
- Client has no `.clang-format`; the `fmt` and `fmt-check` Makefile
  targets are stubbed `TODO`s. Format is not enforced.
- Sanitizer builds (`make debug-asan`, `make debug-tsan`) are real and
  wired up correctly.
- **[measured 2026-08-06] The "Windows portability debt" claim is false.** This
  file previously asserted that `basic/Platform.h` "still drags `pthread.h` into
  Windows builds" and that helpers "still include POSIX-only headers such as
  `sys/time.h`". Neither survives inspection. Every POSIX include in `dkrix/` is
  inside a platform guard:
  - `basic/Platform.h:221` — `pthread.h` is inside
    `#if !defined(_WIN32) && !defined(_WIN64)`.
  - `Client/DebugLog.cpp:17`, `Client/GlobalVariables.cpp:45` — `sys/time.h` is
    in the `#else` arm of a `_WIN32 || _WIN64` test.
  - All remaining hits (`unistd.h`, `sys/socket.h`, `netinet/in.h`,
    `arpa/inet.h`, `netdb.h`, `sys/ioctl.h`) sit under `PLATFORM_WINDOWS`,
    `_WIN32`, or `__LINUX__` guards.

  Method: every `#include` of a POSIX-only header was located, then its
  enclosing preprocessor stack was resolved by scanning backwards for the
  nearest unmatched `#if`. Zero unconditional POSIX includes remain. The first
  Windows CI failure, when it comes, will be something else — do not go looking
  here first.

- **[measured 2026-08-06] `__LINUX__` and `__WINDOWS__` are never defined —
  66 preprocessor directives across 16 files are dead.** `#if __LINUX__` and
  `#elif __WINDOWS__` appear throughout `Client/Packet/`, but neither macro is
  defined by CMake, by any header in the tree, or by any compiler. Both arms
  evaluate false, so those blocks contribute nothing on any platform.

  It builds anyway because `Client/Packet/SocketAPI.h:12-21` does the real
  platform detection from compiler-provided macros (`_WIN32`, `__APPLE__`,
  `__linux__`) and defines `PLATFORM_WINDOWS` / `PLATFORM_LINUX` /
  `PLATFORM_MACOS` itself, then includes the correct socket headers. Everything
  else gets them transitively.

  This is a trap rather than a live bug: a Windows-only include added to an
  `#elif __WINDOWS__` arm would be silently ignored. **Do not "fix" it before CI
  is green** — replacing the dead macros with real ones activates code paths
  that have never been compiled.

### Server (`dkrixserver/`)
- C++11, CMake, explicit source lists (no GLOB), `.clang-format`
  present, `make fmt-check` enforced via GitHub Actions. The cleaner of
  the two trees.
- Three documented binaries (`gameserver`, `loginserver`,
  `sharedserver`) plus **ten undocumented `main.cpp` entry points** for
  retired binaries: `cacheserver`, `chinabilling/{.,stress,testserver}`,
  `theoneserver`, `updateserver`, `gameserver/billing`,
  `gameserver/mofus/{.,testserver}`, `gameserver/testAlone`. Most should
  be archived.
- `src/Core/SXml.{h,cpp}` is a thin wrapper over xerces-c 3.2.3. The
  class names (`XMLAttribute`, `XMLTree`) look hand-rolled but the
  parsing calls into xerces.
- `src/server/gameserver/testAlone/Mutex.h` defines a "Mutex" whose
  lock/unlock just sets `m_bLock = true/false`. Not a real mutex.
- Packet serialization is hand-rolled in `read/write` overrides.
  Packets assume x86 little-endian; `htons/htonl` is used only for
  sockaddr ports, not payload fields.
- Database access is libmysqlclient through a thin wrapper. No
  prepared-statement abstraction. `DatabaseManager` is thread-local,
  not pooled.
  - **[re-measured 2026-08-07] The SQL surface, with the greps recorded
    this time.** (The old "~87" never reproduced; the 08-06 count of 26
    is also stale — Phase 7's tree deletions took most of it.)
    `sprintf(query` is down to **9** hits:
    `grep -rn 'sprintf(query' src/server --include='*.cpp' --include='*.h'`.
    But the parked line's audit showed that metric misses the real path:
    `Statement::executeQuery(fmt, …)` → `vsprintf` → `mysql_real_query`.
    On `main` today: **1,456** `executeQuery(` sites, **598** of them with
    `%s`/`%d` format specifiers — under `dkrixserver/src`,
    `grep -rnE 'executeQuery[[:space:]]*\('` and
    `grep -rnE 'executeQuery[[:space:]]*\(\s*"[^"]*%[sd]'`.
    Size Phase 8's SQL half off 598, not 9.
- Lua 5.1 / LuaJIT is embedded via the C API. **[2026-08-07]** The
  `luaL_openlibs()` hole is closed: `LuaState::init()` now whitelists
  `base`/`math`/`string` only (`b204ebd`), and the four enum-cast sites
  go through range-checked `lua_toboundedenum<T>` (`9b1756f`). See
  Phase 9 for the build verification.
- `conf/gameserver.conf` contains a plaintext `DB_PASSWORD` line,
  dev IPs (`192.168.0.16`), and double-encoded Chinese comments from
  a prior encoding migration. **[2026-08-10]** The `.conf` files are
  untracked now (`conf/*.conf.template` + `${DKRIX_*}` expansion replace
  them), and the burned dev credential was swept out of every tracked
  file in favour of the placeholder `password`.

### Docs (`docs/`)
- Mixed-language reference material (mostly Chinese filenames) plus
  `howto/`, `pic/`, `client_source_overview/`.
- ~~`README.md` still describes three sibling repositories.~~ **Rewritten
  2026-08-06.** For the record, that description was *outdated, not wrong*:
  `rixmmos` does host `dkrix`, `dkrixserver` and `docs` as public forks of
  `opendarkeden` — they are the original layout, superseded by the private
  consolidated `newdk`. Only the paths it gave (`client/`, `server/`) were
  never right.
- **[measured] The workspace-level spec now lives at `../CLAUDE.md`**,
  not here. `docs/CLAUDE.md` is the engineering-principles document.
- **[measured] The ten superseded client status docs are archived** at
  `dkrix/docs/archive/2026-migration-notes/`. Phase 0 is genuinely complete.
- This file is the living modernization plan.

## Roadmap

Phases are ordered for least-risk-first and for unblocking downstream
work. "P0" is a prerequisite for everything else; the others can
interleave once P0 is done.

> **Status reality check (2026-08-07).** The phase order was never followed,
> and after the 08-06/07 wave it no longer needs to be: 1 done, 2 partial
> (two compiler-gated items left, now CI-schedulable), 3 item 1 of 3, 4 safe
> items (4b/4c open), 5 main item (glyph check open), 7 done, 9 mechanical
> items done. Still untouched on this line: 3.2–3.3, 4b/4c, 8's SQL half,
> 10 — and everything the parked line numbered 11–18, now booked below.
> Treat the ordering as intent, not history.

### Current next steps (re-audit 2026-08-09, after the second agent wave)

In order; each independently shippable:

1. **Push the 2026-08-09 wave and watch CI.** Ten commits staged in
   `C:\dev\_incoming\wave-2026-08-09\` (apply order + expected outputs in
   its `APPLY.md`; the whole stack `git am`s clean on `96a1995` and was
   ratchet-verified at the integrated tip). Contents: client fmt gate +
   server fmt-check fix (D3), Phase 11.2 close-out (SQL ratchet **15 → 0**),
   Phase 12 Wave 4 batches 1–2 (packet ratchet **202 → 142**, 30 pairs),
   the 9-pair protocol-review briefs doc, and this docs commit. Server
   `ratchets` must print `OK: 0` and `OK: 142`; the client run is the
   compile gate for Wave 4 and the first live run of the new `format` job.

   **[measured 2026-08-09] Pushed. Server run #29 at `e6398ae` — SUCCESS.
   Client run #24 — FAILURE, and the gate did its job:** Wave 4 repointed
   `Client/UIMessageManager.cpp` at the `shared/Packets/` headers, which —
   unlike the `Client/Packet/Cpackets/` originals — do not include
   `../PlayerInfo.h`. That file is the tree's only user of `PlayerInfo::`
   and had been riding the transitive include, so it lost the namespace:
   25 × `error C2653: 'PlayerInfo': is not a class or namespace name`, the
   only error in the run. Fixed by an explicit `#include
   "Packet/PlayerInfo.h"`. The `Viewers and validators` job failed on the
   same symbol (it builds `DarkEden` too); the two Linux sanitizer legs
   also failed but were **already failing at the last green run `96a1995`**
   on an unrelated missing-`<ctime>` problem in `basic/Platform.h` — both
   are `continue-on-error`, so this is pre-existing noise, not new.

   Two things this exposed, neither fixed here:
   - **The `format` job never checked Wave 4's files.** It derives its base
     as `HEAD~1`, so on a ten-commit push it only sees the last commit —
     here a docs commit, hence "No client C++ files changed" and a green
     job. `github.event.before` is the correct base; switching to it will
     immediately flag everything Wave 4 touched.
   - **`UIMessageManager.cpp` is not clang-format clean** (18,350 lines of
     drift; `.clang-format` has `SortIncludes: true` +
     `IncludeBlocks: Regroup`, so a format pass reorders ~300 includes in a
     file that has demonstrably been relying on include order). Deliberately
     not reformatted alongside a one-line build fix.

   **[measured 2026-08-09, follow-up wave] Client run #25 at `22d144e` —
   `MSVC x64 (Debug)` SUCCESS, `Viewers and validators` SUCCESS.** The
   regression is closed. Three follow-ups landed on top of it:

   - **`4f1dd38` — both format gates now check the whole push.** Client and
     server both derived their base as `HEAD~1`, so a multi-commit push was
     only inspected at its tip. Now `github.event.before`, falling back to
     `HEAD~1` when it is absent (`workflow_dispatch`), all-zeros (first
     push), or unreachable (force-push). Both jobs already use
     `fetch-depth: 0`. Measured against the real `96a1995..e6398ae` push:
     client 16 files in range, **all 16 fail** clang-format but **0 are
     regressions** — every one already failed at `96a1995`, i.e. pre-existing
     backlog surfaced by being touched; server 9 in range, 0 fail. Note the
     window has already closed: `origin/main` is past that push, so those 16
     will not be re-flagged and are only swept by a deliberate `make fmt`.
   - **`8cabe14` — `<time.h>` for the non-Windows `GetLocalTime` stub.**
     Verified under g++ 13.3.0 in WSL by compiling `basic/Platform.h`
     standalone against a stub SDL2 header: the pre-fix copy reproduces CI's
     errors at byte-identical line numbers, the fixed copy compiles clean.
   - **The sanitizer legs are still not green, and one line will not do it.**
     The 65 errors per job are two classes: 45 are the `<time.h>` gap above
     (5 TUs), the other 20 are `error: declaration does not declare anything`
     from bare `std::ofstream;` / `std::ifstream;` statements at namespace
     scope in four SpriteLib headers — `CSprite.h:29-30`, `CSpriteDef.h:14-15`,
     `CFilter.h:21-22`, `CAlphaSprite.h:19-20`. Almost certainly decayed
     `using std::ofstream;` declarations. They declare nothing today, so
     deleting them is a no-op *for those files* — but SpriteLib has many
     unqualified `ofstream`/`ifstream` uses (`CSpritePalBase.h:38-39`,
     `CTypePackVector.h:73-74`, several `.cpp`), which today resolve via
     something else entirely. Confirm what that something is before deleting.
     And the build aborted at 2%, so a third class may sit behind these.
   - **Latent, not breaking CI:** `Platform.h:1841`'s `strlen` has no
     `<string.h>` either — it survives only on SDL's transitive include.
     Proved load-bearing by rerunning the probe with that include removed.

   **[measured 2026-08-09, runs #26–#28] Where this actually ended.**
   Client run #28 at `122173e`: **`MSVC x64 (Debug)` SUCCESS,
   `Viewers and validators` SUCCESS, `clang-format` SUCCESS.** The format
   job is green for the first time in the project's history, and on run #27
   it blocked a commit for the first time — correctly, a tab-indented
   `#include` of mine. Commits: `3376ec9`, `945a1a4`, `668ed2c`, `122173e`.

   The **changed-lines ratchet** (`945a1a4`) replaced the whole-file one:
   `tools/ci/clang-format-changed-lines.sh` derives added-line ranges from a
   zero-context diff and passes them to `clang-format --lines`. Enabling it
   required `SortIncludes: Never` in `dkrix/.clang-format` — clang-format
   sorts the whole include block intersecting the given lines, so one new
   `#include` still demanded a full reorder. Measured cost of losing sorting:
   219 of `UIMessageManager.cpp`'s 18,350 drift lines, 6 of `Platform.h`'s
   1,683. `IndentPPDirectives` was checked at the same time and left at
   `None` — measured best fit (20,446 drift lines vs 21,180 `BeforeHash`,
   21,058 `AfterHash`). `server.yml` deliberately keeps its whole-file check;
   that tree is nearly formatted and its gate passes 9 of 9.

   **The sanitizer legs are a port, not a bug — stop treating them as one.**
   Error counts across the run sequence: **65 → 8 → 1 → 3**, while build
   progress went 2% → 10%. Every fix exposed the next layer. Fixed so far:
   `<time.h>` (`8cabe14`), 22 bare `std::ofstream;`/`std::ifstream;`
   statements across 11 SpriteLib headers (`3376ec9`, `668ed2c` — the tree
   is now swept, zero remain), `<libgen.h>` for `PLATFORM_LINUX`,
   `test_zone.c`'s missing `src/` include path, and `<algorithm>` for
   `map_viewer`'s `std::sort`.

   Still open at 10%, and these are **structural, not missing includes**:
   - `Client/MTopView.h:687` — `CFilterPack` does not name a type.
   - `basic/GL_import.h:31` — redefinition of `SetSurfaceInfo`, an ODR
     violation in a header.
   - `VS_UI/src/header/VS_UI_title.h:306` — `Race S_SLOT::Race`, a member
     named identically to its own type. GCC 13 rejects
     (`-Wchanges-meaning`), MSVC accepts. Fixing means renaming a member.

   Both legs are `continue-on-error` and were already failing before
   2026-08-09, so nothing regressed by leaving them red — but the 11
   engine/sprite tests (the only automated suite in the repo) **have still
   never executed**. `ctest` runs after the build step, and the build has
   never completed. They are compiled, not passed; do not record them as
   passing.

   **[measured 2026-08-09, runs #29–#31] Continued past the "book it"
   recommendation on request.** All three structural items above are fixed
   (`0da76a0`): the `CFilterPack` typedef was `#ifdef PLATFORM_WINDOWS`
   while `MTopView.h:687` uses it unconditionally and its three sibling
   aliases are unguarded; `Platform.h` carried a non-Windows duplicate of
   `GL_import.h:31`'s `SetSurfaceInfo` (`S_SURFACEINFO` has four fields, so
   the two were behaviourally identical); `VS_UI_title.h:306`'s
   `Race Race;` became `enum Race Race;`. Then `414ed98` closed that last
   class — a tree-wide sweep found **exactly two** members named identically
   to their type in 1.29M lines. Then `4f7a1f7` for `Assert.h`.

   Run sequence now **65 → 8 → 1 → 3 → 1 → 1**, progress **2% → 10% → 13%**.

   Two things worth carrying forward from this stretch:
   - **MSVC resolves quoted includes from the whole include stack.** GCC
     searches only the includer's own directory. `VS_UI/src/Vs_ui.cpp:21`'s
     bare `#include "Assert.h"` (the file is `Client/Packet/Assert.h`)
     worked on MSVC for that reason alone. Expect more of these as the
     Linux build gets deeper; the fix is to path-qualify, **not** to add the
     directory to the include path.
   - **Do not "fix" these by adding include directories.** Doing exactly
     that for `Client/Packet` changed search order, and VS_UI TUs began
     resolving `#include "Exception.h"` to `Client/Packet/Exception.h`
     (there are two files by that name) — 25 MSVC errors, i.e. it traded a
     broken Linux build for a broken Windows one, the authoritative target.
     Caught by the restored local build before reaching CI.

   **Each fix in a fully-unformatted file forces a reformat of its enclosing
   scope.** Touching one line makes clang-format destabilise the next, and
   the changed-line range grows by one per pass without converging (measured
   on `VS_UI_title.h`: 306:306 → 306:315 over eight iterations). Formatting
   the whole enclosing struct/class does converge. Cost so far: ~200 lines
   of forced whitespace churn across two headers, both verified semantically
   neutral with `git diff -w`. Budget for this continuing.

### Audio has never worked — `bb02c95`, found 2026-08-09

**[measured] The client has had no sound at all, in every build including
shipped releases.** Two independent bugs that were mutually consistent, which
is why it linked cleanly and nobody noticed:

1. `Client/Platform/CMakeLists.txt:45` tested `TARGET SDL2::SDL2_mixer`.
   vcpkg's `SDL2_mixerConfig.cmake` defines **`SDL2_mixer::SDL2_mixer`** —
   the same `X::X` convention as the `SDL2_image::SDL2_image` and
   `SDL2_ttf::SDL2_ttf` targets this tree already links. `find_package`
   succeeded while the check failed, so `HAVE_SDL2_MIXER` was never true.
2. `DXLibBackendSDL.cpp:42` guarded `#include <SDL_mixer.h>` with
   `#ifdef SDL_MIXER_MAJOR_VERSION` — a macro defined at **`SDL_mixer.h:47`**,
   i.e. only by the header the guard protects. It can never fire.

All eight guarded regions therefore fell through to stubs that **return
success while doing nothing** (`platform_sound_play` → `return 1`,
`platform_sound_load_wav` → `NULL`). No error path ever fired.
`HAVE_SDL2_MIXER` appeared in the entire source tree exactly once: in a
comment. The Phase 3 note at `CMakeLists.txt:76` had already observed that
code "landed in PLATFORM_SOURCES whenever HAVE_SDL2_MIXER was on, which never
actually built" — the symptom was seen, the cause was not.

After the fix, configure reports `SDL2_mixer found - audio support enabled`
and the build stages `SDL2_mixerd.dll` plus `ogg`/`vorbis`/`vorbisfile`/
`wavpack`, none of which were present before. Local Debug build exit 0, so the
28 previously-uncompiled `Mix_*` call sites are sound against the real headers.

**NOT verified: that the client produces correct audio.** That needs a human
to run it and listen. `main` now produces audio-enabled builds; the next
`PUBLISH_RELEASE.cmd` would give testers sound for the first time from code
that has never executed. Listen first. `git revert bb02c95` restores the
silent-but-stable status quo without touching anything else.

### Linux client port — state at handover, 2026-08-09 (runs #29–#38)

**Where it got to.** Both sanitizer legs reached **17% of the build with zero
compile errors** at `4e0625c`; every remaining failure is at *link*. The Linux
client compiling at all is new — it was not true at any point before this wave.
Error counts ran **65 → 8 → 1 → 3 → 1 → 1 → 5 → 1 → 2 → link-only**, progress
**2% → 10% → 13% → 15%/16% → 17%**. MSVC, `Viewers and validators` and
`clang-format` were green throughout, except run #31's format job (see below).

**Commits, `e6398ae..00a51ff` (21).** In rough order: the `PlayerInfo`
regression fix; both format gates' base SHA; `<time.h>`; 22 bare
`std::ofstream;`/`std::ifstream;` statements across 11 SpriteLib headers; the
changed-lines ratchet + `SortIncludes: Never`; `<libgen.h>`, `test_zone.c`'s
`src/` path, `<algorithm>`; `CFilterPack`'s stray guard, `Platform.h`'s
duplicate `SetSurfaceInfo`, `enum Race Race` in two files; 68 `Assert.h`
path-qualifications; `Client.cpp`'s Win32 entry-point guards; `CTypeTable::TYPE`;
`asan_interface.h` + the `HFONT` forwarder guards; a redundant ASan block
deleted; `MEffectTarget`'s key function stubbed.

**Open, in the order they will be hit:**

1. **`effect_viewer` link (both legs).** `00a51ff` stubs
   `MEffectTarget::~MEffectTarget()` in the viewer's `stubs.cpp`.
   **Unverified — and unverifiable locally:** `CMakeLists.txt:86-88` forces
   `BUILD_ENGINE OFF` on `WIN32`, so `effect_viewer` exists only on Linux and
   is absent from the Windows viewers job. Run #38 is the first check.
2. ~~**`Client/SDLMain.cpp` has still never compiled.**~~ **Superseded — see
   "Linux client port, thread 2" below.** It does compile, and always did;
   the `CDirectDraw` failure belonged to `VS_UI/WinMain.cpp`, a different
   file. What was true is that it would not have *linked*, and that the
   initialisation `WinMain` does and `main()` does not is a real list. Both
   are worked through in that section.
3. **`ctest` has never executed.** It runs after the build step. The 11
   engine/sprite tests are compiled at best, never run. Do not record them as
   passing.

**Two lessons worth keeping.**
- *In an unformatted file, deleting lines is nearly free; changing them is
  expensive.* The gate only polices added lines. Correcting one call in
  `MZone::AddEffect` required reformatting 802 lines including gameplay boolean
  logic; deleting the same redundant block cost 10 lines and no reformat.
- *Narrowing works, broadening backfires.* Adding `Client/Packet` to VS_UI's
  include path changed search order and broke the Windows build (two
  `Exception.h` files). Adding `Client/MEffectTarget.cpp` to `effect_viewer`
  traded one missing typeinfo for 19, against the stated intent in `stubs.cpp`'s
  own header. Both were reverted for a narrower fix.

**Process note.** Run #31's `clang-format` job failed because two commits were
pushed after verifying only that they built. A local build and the format gate
are separate checks and neither substitutes for the other; the 68-file batch,
which was gate-checked per file, passed first time.

**Attribution correction.** Every commit in this range carries a
`Co-Authored-By: Claude Opus 5` trailer. That is wrong — the session ran on
Sonnet 5. The trailer came from environment configuration, not from checking
what was actually executing. Not rewritten, since the commits are pushed.

### Linux client port, thread 2 — `Client/SDLMain.cpp` (2026-08-09)

**A Linux client build is available on this workstation after all, without
root.** This is the single most useful thing to carry forward from this
thread; the handover assumed CI's two `continue-on-error` sanitizer legs were
the only compile gate, and they are not. WSL has g++ 13.3.0 but no SDL2 and
no passwordless `sudo`. `apt-get download` needs neither:

```bash
mkdir -p /tmp/sdlpkg && cd /tmp/sdlpkg
apt-get download libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
                 libsdl2-mixer-dev libsdl2-2.0-0 libsdl2-image-2.0-0 \
                 libsdl2-ttf-2.0-0 libsdl2-mixer-2.0-0 libjpeg-dev
mkdir root && for d in *.deb; do dpkg -x "$d" root; done
# Debian's SDL_config.h does #include <SDL2/_real_SDL_config.h>, which lives
# under include/x86_64-linux-gnu/SDL2. Targets get -isystem .../include/SDL2
# only, so give that directory a self-referencing SDL2 link:
ln -sfn . root/usr/include/SDL2/SDL2
cp -n root/usr/include/x86_64-linux-gnu/SDL2/* root/usr/include/SDL2/

cmake -S dkrix -B /tmp/dkbuild -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DCMAKE_PREFIX_PATH=/tmp/sdlpkg/root/usr \
      -DJPEG_INCLUDE_DIR=/tmp/sdlpkg/root/usr/include \
      -DJPEG_LIBRARY=/tmp/sdlpkg/root/usr/lib/x86_64-linux-gnu/libjpeg.so
```

The runtime `.so` files matter: `sdl2_ttf-config.cmake` gates
`add_library(SDL2_ttf::SDL2_ttf …)` on `EXISTS` of the versioned library, and
CMake reports a dangling symlink as absent, so the `-dev` packages alone
configure but fail to generate. Linking a whole executable also wants SDL2's
own transitive deps (ALSA, PulseAudio, libdecor, libsamplerate, Xss); for
compile-only work pass `-DCMAKE_EXE_LINKER_FLAGS='-Wl,--allow-shlib-undefined
-Wl,--unresolved-symbols=ignore-in-shared-libs'` rather than chasing them.

With that, `compile_commands.json` gives the exact CI compile line for any
single file, which is a seconds-long check instead of a 25-minute CI round
trip. It also gets far past where CI stops: everything CI reported as a link
failure at 17% is a *viewer/test executable*, and `--target DarkEden -- -k`
walks straight past those into the client's own translation units.

**[measured 2026-08-09] Three files fail to compile past the 17% mark**,
found this way and left for whoever owns them — none is in this thread's
scope, and all three are small:

- `Client/MEffectSpriteTypeTable.cpp:44` — `__asan_address_is_poisoned` is
  not declared. The same fix `a227d26` applied to `MZone`
  (`<sanitizer/asan_interface.h>`) was not applied here. Asan leg only.
- `Client/Packet/Gpackets/GCAddNPCHandler.cpp:43` — `_stricmp` is not
  declared; GCC suggests `stricmp`, which the shim does provide.
- `Client/Packet/Gpackets/GCMonsterKillQuestInfoHandler.cpp:30` —
  `std::auto_ptr` is not a member of `std`. Not a C++11-vs-C++17 problem —
  libstdc++ 13 still declares it under `-std=gnu++11` (checked) — the TU
  simply never includes `<memory>` and got it transitively on MSVC. Three
  cascading errors from the one declaration.

That census is from a run stopped just short of 60% of the `DarkEden`
target, so it is a floor, not a complete list.

**Two handover claims were wrong.** `Client/SDLMain.cpp` compiles clean today
— `-fsyntax-only` and a real `-c`, zero errors, both before and after this
thread's changes. And it never referenced `CDirectDraw`: the only occurrence
of the name in the tree is `CMakeLists.txt:296-315`, describing
`VS_UI/WinMain.cpp`, a different file with a different entry point, which
that comment excludes unconditionally. The two got conflated. What is true is
that the *link* had never reached it, and it would not have linked.

**CMake wiring is correct, no change needed.** `file(GLOB
CLIENT_MAIN_SOURCES … Client/*.cpp)` (`CMakeLists.txt:687`) picks up
`SDLMain.cpp` on every platform; on Windows the whole file is inside
`#ifndef PLATFORM_WINDOWS` and compiles to an empty object.

**Gap analysis: `WinMain` (Client.cpp:2292-3549) vs `main()`
(SDLMain.cpp:182-455).** Line numbers are post-`5c725eb`.

| What Windows does | Client.cpp | SDLMain.cpp | Verdict |
| --- | --- | --- | --- |
| `EnableDpiAwareProcess`, `timeBeginPeriod/timeEndPeriod` | 2296-2297, 3400 | — | Win32 by nature |
| Force a 16bpp display mode, restore on exit | 2311-2317, 3399 | — | Win32 by nature; SDL owns the mode |
| Parse launch mode from the last command-line char | 2319-2342 | 217-243 | ported, equivalent |
| Single-instance mutex `<<<DarkEden>>>` | 2375, 3529 | — | **gap**: two clients can run at once on Linux. Win32 by mechanism, but the intent is portable |
| `FindWindow("PROCEXPL")` anti-debug bail-out | 2408 | — | dead copy protection; correctly absent |
| Fill `g_CWD` from the exe path, `chdir` to it | 2423-2433 | 248-257 | **partly ported** — `getcwd` fills `g_CWD` (fixed here, `69d0efb`), but nothing derives the exe directory or `chdir`s; Linux relies on the caller's cwd |
| Replace `Updater.exe` with `Updater2.exe` | 2478-2489 | — | Windows updater; correctly absent |
| `srand(time(NULL))` | 2626 | — | **gap**, but inert: `VS_UI_Title.cpp:1012` seeds before the title screen. Not landed — the one added line would have cost a 280-line reformat of `main()` |
| `g_pFileDef = new Properties` + load | 2644-2645 | — | correctly absent: `GameInit.cpp:1657` does it inside `InitGame()`, which both paths call. The Windows path allocates it twice and leaks the first |
| Load the Netmarble config, `ParsingNetmarble` | 2652, 3127 | — | **gap**; dead Korean-portal integration |
| `g_MaxNPC = 0` | 2714, 2940 | — | **gap**, cosmetic: `GameMain.cpp:238` leaves it at `MAX_NPC`. `g_bHAL`/`g_bMusicSW` are already `true` there, so those two are correctly absent |
| 8-char command-line auth prefix, `g_Dimension` from `lpCmdLine[8]` | 2736-2849 | — | **gap**: world id never set from the command line |
| `GetFutecAddress(lpCmdLine)` | 2959 | — | **gap**: `g_FutecIP`/`g_FutecPort` never set |
| `CheckTerriblePatch()` | 2992 | — | **gap**: append-patch integrity check never runs. Note it dereferences `g_pFileDef`, which on Linux is null until `InitGame`, so it cannot simply be called in the same place |
| Delete the leftover `Update/` directory | 3044-3100 | — | **gap**; Win32 CRT `_findfirst`/`_rmdir`. Only matters alongside the Windows updater |
| `RemoveProgressBar()` | 3107 | — | correctly absent: the body is `#ifdef PLATFORM_WINDOWS DestroyWindow` and `g_hWndProgress` is only ever set on Windows |
| `ParsingRealServer` → `IsAutoLogIn`, `UserID`, `AutoLogInKeyValue`, `g_Dimension` | 3146, 3247-3272 | — | **the biggest gap**: auto-login is entirely absent on Linux |
| `_APICheck.init()` / `.CheckApi()` | 3158, 3318 | — | correctly absent: `APICheck.cpp:139-151` is a no-op stub off Windows |
| `CheckDarkEdenLanguage()` → `gC_ci` | 3221-3228 | 284 | equivalent: the function is `return DARKEDEN_ENGLISH;` (Client.cpp:636) and the switch has only a `default:` arm |
| `InitCrashReport()` | 3234 | — | Windows minidump; correctly absent |
| `InitApp`, `g_dwVideoMemory`, `SetKorean`, `g_pTopView->Init`, `g_bActiveApp` | 3237-3307 | 297-334 | ported. SDLMain adds `CheckActivate(TRUE)` and `SDL_RaiseWindow` (335, 338), which Windows gets from `WM_ACTIVATEAPP` |
| `PeekMessage`/`GetMessage`/`Translate`/`Dispatch` | 3320-3327 | 362 | replaced by `platform_input_update()`; equivalent by design |
| `g_pUpdate->Update()` | 3342 | 375-378 | ported |
| FPS window using `g_FrameCount`/`g_StartFrameCount` | 3367-3381 | 381-390 | ported; was mixing a local with a global until `69d0efb` |
| `WaitMessage()` when inactive | 3386 | 411 | replaced by `SDL_Delay(10)` |
| `SPI_SETSCREENSAVERRUNNING` | 3391 | — | Win32 by nature |
| `g_bForceExitBynProtect` → the bad-timer dialog loop | 3358-3361, 3405-3477 | — | dead nProtect anti-cheat; correctly absent |
| `ReleaseAllObjects()`, `delete gC_ci` | 3482, 3494 | 443, 446 | ported |
| `delete g_pClientConfig` | 3521 | — | **gap**, minor: leak at exit |
| Re-launch the updater when `g_bNeedUpdate` | 3536-3546 | — | Windows updater; correctly absent |

**Presentation is the one divergence worth a decision.** `CGameUpdate.cpp:5032`
and `:5954` call `CSDLGraphics::Flip()` from inside the update path on every
platform, and `Flip()` (Client.cpp:103-137) already does `SDL_RenderClear` +
`spritectl_present_surface` + `SDL_RenderPresent`. SDLMain's loop then does
all three again around the same `g_pUpdate->Update()` call (372, 393-405),
plus a full `g_pLast`→`g_pBack` blit. So every Linux frame is presented twice
and cleared twice. Left alone deliberately: deciding which of the two owns
presentation needs the client actually running, and neither answer is implied
by the Windows path. **[unverified]** — nothing here has been executed.

**Landed (5 commits, all compile-verified under WSL g++ 13.3.0).**

- `25d2ee5` — `SDLMain.cpp` defined `g_pSDLWindow`, `g_pSDLRenderer` and
  `g_bRunning`, all of which are already strong definitions in objects that
  link into the same binary (`Client.cpp:84-85`, which reaches
  `libVS_UI.a` via `VS_UI_CLIENT_SOURCES` on every platform; and
  `Globals.cpp:25`). Confirmed with `nm` on both sides. Now `extern`.
- `fbc7e75` — deleted an empty `ExecuteActionInfoFromMainNode` stub. The real
  one is `PacketFunction.cpp:1531`, declared in `GameHelpers.h:83`, and
  compiles on Linux. The stub was not even the same function: its tenth
  parameter was `unsigned long` where `DWORD` is `unsigned int` here, so
  `nm` shows two distinct mangled names and no caller could ever have
  selected it.
- `69d0efb` — `g_CWD` and `g_FrameCount` were re-declared as locals inside
  `main()`, shadowing the globals they were ported from. `g_FrameCount` is
  the client's frame clock: `CGameUpdate.cpp:5771` and
  `CWaitUIUpdate.cpp:403` increment it, `MTopView.cpp:13060/13078/17618` read
  it to pick animation frames. SDLMain subtracted its local from the real
  `g_StartFrameCount` and wrote the local's value back into that global once
  a second. Removing the local alone would have double-counted, since SDLMain
  incremented it on top of the update path's own increment and `WinMain` only
  reads — so the extra `++` and the two loop-entry resets went too. After
  this, `nm` reports exactly one defined symbol in `SDLMain.cpp.o`: `main`.
- `ab8469a` — added the `SDL_RENDERER_SOFTWARE` retry that
  `Client.cpp:1755` has. Without it any host with no usable accelerated
  renderer — headless runner, VM with no GL, remote X — fails `InitApp` and
  exits before `InitGame`.
- `5c725eb` — deleted two preprocessor branches in `Client.cpp` that cannot
  compile anywhere now that `WinMain` is Windows-only: the `#ifndef
  PLATFORM_WINDOWS` invalid-DLL whitelist scan, and the `#else` whose body is
  the comment "TODO: Implement SDL2-based game loop" — that loop is
  `SDLMain.cpp`. Details, including why the DLL scan must be deleted rather
  than corrected, are in the commit message.

**Needs a human decision, not a patch.** `Client.cpp:2676`'s
`#if defined(USE_UPDATER) && !defined(PLATFORM_WINDOWS)` is inside the
Windows-only `WinMain` region, so the "empty command line → re-launch
`Updater.exe`" path has never run in a shipping build. Correcting the guard
would change what the released client does, so it was left alone.

**Still open, and what is *not* claimed.** The `DarkEden` link has not been
reached — the target builds to just under 60% locally, and the errors above
keep even `make -k` from finishing it. So nothing here shows the client
links; it shows only that `SDLMain.cpp` no longer contributes a duplicate
symbol to that link, and that the three symbols it used to duplicate really
were duplicated. Everything about runtime behaviour in the table above is
reasoning from the source, not from a run. `ctest` still has never executed.

**A third data point on the format gate, and a rule.** Formatting only the
changed lines still does not converge in this tree: on `SDLMain.cpp` the
range grew every pass for eight passes and was still growing (`211` → `211,
213` → `211, 213-214` → … → `211, 213-214, 216-219, 221, 239, 241-242`).
The cause is that the file is tab-indented and `.clang-format` says
`UseTab: Never`, so any touched line converts to spaces and drags its
neighbour. Practical rule for this tree:

- Deleting lines is free — the gate skips a diff with no added lines.
- Adding a line at **file scope, column 0** is free: no indentation to
  convert, no enclosing scope to drag. Both new `extern` blocks here went in
  that way on purpose.
- Adding a line **inside a function** costs a full reformat of that function.
  `ab8469a` paid it (75 lines, `git diff -w` clean); the `srand` line was
  dropped rather than pay 280 lines for a seed that is already set elsewhere.

   **Design problem this wave exposed, now resolved by `945a1a4`:** the format gate's
   ratchet *was* whole-file — touch a file, the whole file had to be
   clang-format clean. Against a 2,199-file backlog of legacy sources with
   four-figure drift each (`Platform.h` 1,683 lines; `UIMessageManager.cpp`
   18,350), that meant **every genuine one-line bug fix in a legacy file
   tripped the gate**, and the only ways out were a risky whole-file
   reformat or a carve-out. Both fixes in this wave hit it. Resolved by
   ratcheting on changed lines instead — see the run #26–#28 entry above.

   Consequence worth recording: **the 2,199-file backlog is now permanent
   unless deliberately swept.** The gate no longer asks anyone to reformat a
   legacy file, which is the point, but it also means the backlog will never
   shrink as a side effect of ordinary work. A sweep would be its own
   project — and note the client tree is CRLF on disk, so a bulk reformat
   rewrites line endings across thousands of files as well.
2. **Phase 18 — run the smoke test** (prep pack:
   `_incoming\wave-2026-08-09\w6-prep\` — `PREFLIGHT.md` first; it
   resolves which MySQL instance is listening before any destructive step
   and pre-fills the backup commands. Known traps found by static
   verification: SMOKE_TEST §7's account-creation SQL is wrong three ways —
   use `create_test_account.sql`; `smoke_test_finish.sh` is an unported
   parked-line artifact, do not run; launch commands need `-f`; a CJK font
   must be present in `Data\Font` for the Phase 5 glyph check.)
3. **Phase 12 endgame.** The ≤5-residual queue is **empty except for its 2
   recorded skips** (Wave 4 batch 3, 2026-08-09) — `CGUseMessageItemFromInventory`
   waits on its divergent base, `CGConnectSetKey` needs a cross-family target
   extension; both want their own decision, neither is mechanical. Remaining:
   the 48-pair `>5` set, now triaged pair-by-pair in
   `docs/phase12-residual-triage-2026-08-09.md` — **45 mechanical, 3
   needs-decision, 0 blocked**, with a 5A–5E batch order that takes the
   duplicate ratchet 114 → 70. Read that doc's three flagged pairs before
   starting: `CGDissectionCorpse` is the one pair where the standard
   adopt-the-server recipe is *actively wrong*, and `CGStashToMouse` needs its
   merge rule inverted for one `toString()` line. The review queue is down to
   **7** — the two `land-now` pairs (CGGuildChat, CGModifyNickname) landed with
   batch 3. Recommended order for the rest, unchanged: the three size-formula
   pairs after the first smoke run; CGMove and CLLogin need live windows; the
   exchange pair is feature-gated (client UI is dead). Phase 13's client half
   rides the tail of this queue.
4. **Sanitizer legs** — largely done 2026-08-09; see Phase 10 bullet 3 for
   the measured per-leg table. Server asan reached 5 consecutive greens and
   is now blocking (`71e2381`). The `-fno-sanitize=vptr` line was **applied
   2026-08-10** (`docs/ci-server-ubsan.md`); server ubsan now has to earn
   its own 5 greens on a runner before it flips. Also 2026-08-10: the
   build-only sanitizer matrix was joined by a runtime one — the
   `boot under <san> (boot-only, no packets)` job is now a matrix over
   `asan`/`ubsan` rather than an asan-only job, so a UBSan binary is
   actually *executed*. Both boot legs are non-blocking; the ubsan boot leg
   runs with `halt_on_error=0` so one run enumerates every finding.
   The two client legs are gated on the Linux client port finishing, not on a
   rule — and that port moved a long way on 2026-08-09 (see thread 2 below:
   `SDLMain.cpp` was never the blocker, a local Linux client build now exists,
   and it reaches 64%).
5. **Phase 8 secrets step 2** — rotate the deployment onto the `DKRIX_*`
   templates. Live-server window, backups first, human-only.
6. **Phase 4b** — sprite backend collapse; human-validated rendering
   (prep brief pending; folds into a post-smoke-test sitting).

> **2026-08-07 five-stream agent wave (cloud session):** items 3–5 above
> advanced in one parallel pass — 11.2 batch 1 (ratchet 542→529), Phase 3
> item 3 (DXLib/ dissolved), Phase 2 mutex unification (47→12 sites),
> Phase 10 bullet 1 (client fmt infra + census), and the Phase 12
> normalizer (163 pairs verdicted: 62 style-only, 9 real-divergence, 7 of
> those newly machine-found). All grep/script-verified only — the push
> carrying the wave is the compile gate for every stream. Patches,
> manifests, and per-stream decision lists: `C:\dev\_incoming\<stream>\`.

> **2026-08-07, later — four-agent wave (worktree-isolated).** Three of
> four workstreams landed: 11.2 batch 2 (529→510), Phase 10 bullet 2
> (explicit/CONFIGURE_DEPENDS source lists), Phase 10 bullet 3
> (sanitizer options + non-blocking CI legs). The fourth, Phase 12 Wave
> 1, found the include-resolution blocker above and correctly landed
> nothing rather than force a broken approach — see Phase 12. All
> grep/script-verified only; the push carrying this wave is the compile
> gate for the three that landed.

### Phase -1 — Make the work verifiable (done — both trees green 2026-08-06)

This did not exist in earlier revisions and it should have. Nothing below
can be trusted without it, and it is the single change that would let
routine phase work be delegated rather than hand-held.

- [x] ~~Resolve the ~6,825-file uncommitted delta.~~ **Done 2026-08-06** —
      committed as checkpoint `067087f`; tree is clean.
- [x] ~~Reconcile `modernize/phase-4-sprite` with
      `origin/modernize/phase4-sprite`.~~ **Done 2026-08-06** — they were
      never the same branch. Local promoted to `main`, remote parked at
      tag `archive/modernization-phases-1-17`. See `BRANCH-RECONCILIATION.md`.
- [x] Add a root `.gitignore` (see `../CLAUDE.md` → Repo hygiene). **Done.**
- Stand up GitHub Actions on `rixmmos/newdk`:
  - [x] **server — GREEN 2026-08-06.** `make debug` on Ubuntu with
        libmysqlclient / lua5.1 / xerces-c / **libnsl**. All three binaries
        (`gameserver`, `loginserver`, `sharedserver`) produced and verified.
        `clang-format` job green and genuinely inspecting files.
  - [x] **client — GREEN 2026-08-06 (run #6, `90f6106`, 28m23s).** `cmake
        --build` on `windows-latest` with vcpkg. Three real bugs found and
        fixed on the way, none previously catchable because no x64 build
        had ever run in CI: Run #3 failed at *configure* (stale generator
        pin, fixed in `21a9172`). Run #4 failed at **link** — an inverted
        `if(WIN32)`/`if(NOT WIN32)` source exclusion dropped
        `VS_UI_WebBrowser.cpp`'s real implementation on Windows, fixed in
        `84f2e5a`. Run #5 failed at **compile** — a VC6-era `(LONG*)` cast
        where `IWebBrowserApp::get_HWND` wants pointer-sized `SHANDLE_PTR*`,
        fixed in `90f6106`. Run #7 (auto-triggered on `84f2e5a` alone, i.e.
        fix #1 without fix #2) failed with the same compile error as run
        #5, cross-confirming both fixes were independently required.
- Success: a change can be proposed, built, and judged without a human
  manually running two toolchains. **Achieved for both `dkrixserver/` and
  `dkrix/`.**

### Phase 0 — Reset the narrative (done 2026-04-17)
- [x] Create project-wide `docs/CLAUDE.md`.
- [x] Create this file, `docs/MODERNIZATION.md`.
- [x] Archive the ten superseded client status docs under
      `client/docs/archive/2026-migration-notes/` with a folder README
      explaining provenance. (Count was ten, not nine — `plan.md`,
      `spec.md`, and `tasks.md` were also migration-era status docs.)
- [x] Update `client/CLAUDE.md` to current reality: drop deleted
      `D3DLib/`, keep `WinLib/` acknowledgment (still on disk, still
      dead — slated for Phase 1 deletion), replace contradictory
      migration status with a pointer to this file.
- [x] Add `docs/README.md` explaining the three-repo layout and
      routing readers to `CLAUDE.md` / `MODERNIZATION.md`.

### Phase 1 — Delete the unambiguously dead (client, done — build-verified 2026-08-07)
- [x] Delete `Client/MItemTable_bak-2007-5-7.cpp` (14,965 lines).
      **[measured 2026-08-06] Confirmed gone.**
- [x] Delete the nine CMake-excluded duplicate `.cpp` files
      (`GlobalVariables`, `MissingGlobals`, `GameHelpers`,
      `GameFunctions`, `GamePacketFunctions`, `ActionFunctions`,
      `MitemTableinit2`, `GCNotifyWinHandler`, `GCNotifyWin`) —
      7,550 lines. **[measured 2026-08-06, by grep, not by build — no
      compiler available in this session.** Each file's CMake exclusion
      comment claims it duplicates a live file; spot-checked representative
      symbols from every file (`SetPetInfo`, `InitPlayer`,
      `ExecuteActionInfoFromMainNode`, `AffectModifyInfo`,
      `CreateActionResultNode`, `CheckItemForSkillIcon`, the `Skill*`
      family, `InitItem2`, `g_BasicException`, and the `g_x`/
      `g_pSystemMessage`/`g_pSocket`/`g_pZoneLarge` globals) and confirmed a
      real definition exists in a file that IS compiled (`GameMain.cpp`,
      `PacketFunction.cpp`, `SDLMain.cpp`, `BasicException.cpp`,
      `MitemTableInit.cpp`). Confirmed no file `#include`s any of the nine
      `.cpp` files as text, and no legacy `.dsp`/`.vcxproj` references
      them.**
- [x] Delete `Client/Packet/Cpackets/*Handler.cpp` (35 files, 1,605 lines
      per `git diff --stat`; these are server-side handlers).
      **[measured 2026-08-06, by grep.**
      21 of the 35 are called only inside `#ifndef __GAME_CLIENT__`
      (`__GAME_CLIENT__=1` is a real compile definition for this target,
      `CMakeLists.txt:678`), so the call site compiles out of the client
      build entirely. The other 14 are called unconditionally from their
      packet's `::execute()`, but `Client/CGHandlersStub.cpp` (kept, still
      compiled) already supplies an empty-body definition of each one's
      `::execute` for exactly this reason — its own header comment says
      "Stub implementations ... because the actual handler implementations
      are server-side." Deleting the excluded files removes no symbol the
      client links against. One handler, `CGRangerSayHandler`, turned out
      to be declared but never called anywhere (`CGRangerSay.cpp` calls
      `CGWhisperHandler::execute` instead — a pre-existing oddity, left
      as-is).**
- [x] Remove the matching `REMOVE_ITEM` / `FILTER EXCLUDE` lines from
      `CMakeLists.txt`. **[measured 2026-08-06]** Left `${VS_UI_CLIENT_SOURCES}`
      and the unrelated `CSpritePal.cpp` exclusion in place.
- [ ] ~~Delete `Client/WinLib/` (3 files, all dead).~~ **Withdrawn
      2026-08-06 — the "all dead" premise is false.** `CGameUpdate`
      subclasses `CWinUpdate` and the client holds live pointers to it.
      See the Ground-truth note above. Reclassify as a refactor of the
      update path and re-file under Phase 3 or later.
- [x] Add `build/`, `compile_commands.json`, `*.dsp`, `*.dsw`,
      `*_bak-*.cpp`, `*.bak` to `.gitignore`. **[measured 2026-08-06] Already
      present** — root `.gitignore` has `build/` (plus per-environment
      variants and `dkrix/build/`), `compile_commands.json`, `*.dsp`,
      `*.dsw`, `*.bak`, and `*_bak*` (covers `*_bak-*.cpp`); `dkrix/.gitignore`
      also has `build/`, `*.bak`, `*_bak*`. Verified with
      `git check-ignore -v` against representative paths in each pattern.
      No edit needed; this was already done in the Phase -1 pass.
- Success: tree is ~9.2k lines lighter from this phase (~24k including the
  `_bak` file from the prior pass). ~~Not build-verified~~ —
  **build-verified 2026-08-07:** client CI run #8 compiled the merged tip
  `5ca240a` with every Phase 1 deletion in it — SUCCESS, 28m30s. The
  grep-based confidence held. (If a later regression ever points here,
  the pass was three commits: the nine duplicate files, the 35 Cpackets
  handlers, the CMakeLists.txt cleanup.)

### Phase 2 — Shrink `basic/Platform.h` (client, partially done 2026-08-06)

**Why the file grew (checked `git log -p` before touching anything):**
the 1,968 → 1,996 growth was not scope creep. The single growth commit
(`067067f`) added `#include <windows.h>` for real on `PLATFORM_WINDOWS`
and then guarded the repo-local `BYTE`/`WORD`/`DWORD`/... typedefs and
the old unconditional `typedef DWORD id_t;` behind `#ifndef
PLATFORM_WINDOWS` / `#ifndef PLATFORM_ID_T_DEFINED`. Before that fix,
Windows would hit a hard typedef-redefinition conflict: an unconditional
`typedef unsigned int id_t;` followed by an unconditional `typedef DWORD
id_t;` under `#ifdef PLATFORM_WINDOWS`, two different types with the
same name in the same scope. **The growth was a real bug fix that made
the file bigger, not drift to reverse.** Any future shrink work must
keep the Windows/non-Windows split it introduced rather than collapsing
back to a single unconditional typedef.

- [x] `id_t` duplicate typedefs. Found 5 occurrences (matches the
      2026-08-06 measurement), not at the doc's old line numbers
      (128/130/358/371 had already shifted). One was a properly guarded,
      platform-conditional single definition (Windows → real SDK
      `DWORD`, else → `unsigned int`) — correct and, per the above, load
      bearing. The other was a dead unconditional duplicate 250 lines
      later, identical on non-Windows, a silent redefinition-conflict
      risk if the guard around it were ever removed. Deleted the
      duplicate; kept the single guarded definition, changed its
      non-Windows arm from `unsigned int` to `uint32_t` for consistency
      with the rest of the file's stdint-based typedefs. **Deliberately
      did not collapse to a single unconditional `using id_t =
      std::uint32_t;`** as the doc's old wording said — that would
      revert the Windows fix above. Commit `cdf82ef`.
- [x] Dead macro shims — audited each name individually instead of
      deleting the group:
      - `_tcscpy`, `_tcscat`: zero callers anywhere in `dkrix/` (grepped
        both `.h` and `.cpp`). Deleted. Commit `a36c0d3`.
      - `_T`: live callers (`VS_UI/src/vs_ui_gamecommon2.cpp`,
        `Client/MinTr.h`, `Client/GetWinVer.h`). **Kept — doc's "no
        callers" claim was wrong.**
      - `stricmp`: live callers in 5 files (`VS_UI_GameCommon.cpp`,
        `PacketFunction.cpp`, `GCAddNPCHandler.cpp`, `CGameUpdate.cpp`,
        `vs_ui_gamecommon2.cpp`). **Kept — doc's claim was wrong.**
      - `_itoa`: does not exist anywhere in `Platform.h` or the rest of
        `dkrix/`. **Nothing to delete — doc's claim was moot.**
- [x] Fake `HRESULT`/`S_OK`/`S_FALSE` machinery — **done 2026-08-08.**
      Re-ran the census tree-wide (word-boundary grep, `.h`/`.cpp`): 31
      files matched `HRESULT`/`S_OK`/`S_FALSE`/`SUCCEEDED`/`FAILED`
      before this pass — most were false positives from the plain
      substring search (`ITEM_STATUS_OK`, `BS_OK`, `*_ARMS_OK_*`,
      `*_RANK_BONUS_OK` etc. all contain `S_OK` as a substring; excluding
      those, only ~14 files carried a genuine token). Classified
      per-file, not by pattern:
      - **Windows-only, left alone** (excluded from the non-Windows CMake
        source list — verified against the current exclusion regexes):
        `VS_UI_WebBrowser.cpp`, `Imm/*` (5 files), `Client.cpp`,
        `WavePackFileManager.cpp`. `VS_UI/WinMain.cpp` added to this
        bucket after reading it: unconditionally `#include <windows.h>`
        with 3000+ lines of unguarded WinMain/COM code (real
        `CoCreateInstance`/`IWebBrowser2` `HRESULT`), and — a CMake
        oddity worth flagging separately — it's excluded from the
        *Windows* source list (`if(WIN32) ... EXCLUDE REGEX
        ".*WinMain.*"`, always taken since `USE_SDL_BACKEND` is
        `FORCE`d `ON`), not the non-Windows one; the real cross-platform
        entry point is `Client/SDLMain.cpp` (`#ifndef PLATFORM_WINDOWS`
        guarded). `WinMain.cpp` looks like dead/superseded legacy
        residue — candidate for outright deletion in a future pass, but
        that's out of scope here.

        **[measured 2026-08-09] Deleted, independently re-verified.** The
        note above was a snapshot from reading the file in passing, not a
        from-scratch liveness check — and it, and the CMake exclusion it
        described, were both stale by the time of this pass. By now
        `CMakeLists.txt` no longer had that `if(WIN32)`-only exclusion at
        all: the Phase 10 explicit-source-list rewrite plus an
        undocumented same-day follow-up (both 2026-08-07) had already
        replaced it with `list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX
        ".*WinMain.*")` at line 316 — unconditional, outside any
        `if(WIN32)`/`if(NOT WIN32)` guard, inside the `if(USE_SDL_BACKEND
        OR NOT WIN32)` block that always runs since `USE_SDL_BACKEND` is
        `FORCE`d `ON`. So the file was already excluded from the only
        source list that ever named it, on *both* platforms, with the
        CMake comment right there (lines 295–315) explaining why: it
        fails to compile as of the Phase 3 item 3 `CDirectDraw`→
        `CSDLGraphics` rename (`gC_DD` at file scope), and would collide
        at link time regardless — it redefines globals `Client/Client.cpp`
        already defines unconditionally into the same `VS_UI` static
        library (`g_hWnd`, `g_LeftPremiumDays`, `g_pWebBrowser`,
        `g_mapPremiumZone`, `InitSound`, `MouseEventReceiver`,
        `KeyboardEventReceiver`, `UI_ResultReceiver`, `ProgramLoop`).
        Independently re-verified that last claim by grepping each symbol
        tree-wide: every hit outside `WinMain.cpp` itself is a separate,
        independent definition in another file (`Client.cpp`,
        `GameUI.cpp`, `GameInit.cpp`, `ClientFunction.cpp`,
        `MPriceManager.cpp`, `MTopView.cpp`, `VS_UI_Title.cpp`,
        `VS_UI_GameCommon.cpp`, `LCLoginOKHandler.cpp`,
        `GCNoticeEventHandler.cpp`) — name collisions, not references
        into `WinMain.cpp`. Also confirmed: no `#include "WinMain.cpp"`
        anywhere; `CLIENT_MAIN_SOURCES`'s glob (`CMakeLists.txt:687-693`)
        is scoped to `Client/*.cpp` and never reaches `VS_UI/`; and no
        *tracked* `.vcxproj` references it — `dkrix/Client/Client.vcxproj`
        itself isn't tracked (only its already-known-orphaned `.filters`
        and a `.user` are, and neither mentions `WinMain`). Deleted
        `VS_UI/WinMain.cpp` (3,782 lines) on that evidence.
      - **Migrated off the shim** (all callers read first; every one
        discarded the return value as a bare statement, matching
        `8725860`'s pattern):
        - `Client::InitFail()` (`Client.h`/`Client.cpp`): `HRESULT` →
          `void`. 3 live call sites (`GameInit.cpp` ×2, `GameMain.cpp`
          ×1), all bare statements.
        - `CSDLStream::WaveReadFile()` (`Platform/CDirectSoundStream.h`/
          `.cpp`): `HRESULT` → `void`. Zero callers anywhere in the
          tree (protected, unused stub on an already-deprecated class);
          the local `E_FAIL` compat `#define` it existed for is deleted
          too.
        - `InputManager::InitDI()` (`Platform/InputManager.h`/`.cpp`):
          `HRESULT` → `bool`, not `void` — it maps `Init()`'s `BOOL`
          onto success/failure via `S_OK`/`S_FALSE`, a real (if unused —
          zero callers, private) success signal, unlike `SetAcquire`'s
          pure no-op.
      - **Left as-is, provably inert** (four remaining token hits, all
        inside dead/never-taken code — confirmed by checking what gates
        them, not assumed): `CShadowPartManager.cpp:404` (inside a
        `/* */` block, lines 396–432 — not code, never parsed past
        translation phase 3); `GameInit.cpp:1317` (inside
        `#if defined(PLATFORM_WINDOWS) && !defined(USE_SDL_BACKEND)`,
        and `USE_SDL_BACKEND` is unconditionally `target_compile_
        definitions`'d for the `DarkEden` target — branch never taken);
        `MTopView.cpp:467,8233` (inside `#ifdef OUTPUT_DEBUG`, and
        `OUTPUT_DEBUG` — distinct from the `OUTPUT_DEBUG_*` sub-flags —
        is never `#define`d anywhere in the tree); `CWaitUIUpdate.cpp:539`
        is a commented-out `//HRESULT hr;` line. None of these require
        the shim to exist for the current build graph to compile
        (inactive preprocessor branches and comments are discarded
        before semantic analysis ever sees them), so none blocked
        deleting it.
      - The `CDirectSoundStream.cpp`/`_Adapter.cpp` duplicate-symbol
        concern the old note flagged here is stale — resolved by Phase 3
        item 1 (2026-08-07), see below; not re-litigated in this pass.
      **Shim block deleted** from `basic/Platform.h` (`typedef int32_t
      HRESULT`, `S_OK`/`S_FALSE`/`SUCCEEDED`/`FAILED`, ~15 lines);
      `LRESULT`/`UINT_PTR` and the rest of the `#ifndef PLATFORM_WINDOWS`
      block are unrelated and kept. Since the shim only ever existed
      inside `#ifndef PLATFORM_WINDOWS`, the Windows build was never
      affected by any of this — compile gate is the client CI build, not
      run in this sandbox.
- [x] `SCAN_CODE()` / `platform_get_scan_code()` — **doc's premise was
      stale, nothing to do.** `SCAN_CODE` isn't in `Platform.h` at all;
      it's defined in `basic/PlatformUtil.h` (`#ifdef PLATFORM_WINDOWS`
      → `LOBYTE(HIWORD(x))`, else → `platform_get_scan_code(x)`), has
      live callers (`VS_UI_Game.cpp`, `VS_UI_Title.cpp`), and
      `platform_get_scan_code()` is genuinely implemented in
      `PlatformSDL.cpp:312`. No dead macro, no missing function.
- [x] Mutex primitive unification — **done 2026-08-07, branch
      `agent/phase2-mutex` (3 commits: fcc3799, 232b4bf, 1377a25),
      pending client CI.** Re-measured before the change: 47
      Enter/Leave/Initialize/DeleteCriticalSection sites across 14
      client files, not ~16/11 (DebugLog.cpp alone holds 12). After:
      12 sites, all DebugLog.cpp's g_log_lock — outside the scoped
      clusters, deliberately left; trivial follow-up.
      - The five member-lock classes (RequestUserManager,
        WhisperManager, RequestClientPlayerManager,
        RequestServerPlayerManager's m_Lock, ProfileManager) hold
        std::mutex; Lock()/Unlock() bodies call .lock()/.unlock();
        ctor/dtor init/delete removed. Copyability gate passed for all
        five [measured]: heap-only singletons, no copies/memset;
        re-entrancy gate passed [measured]: no locked region calls a
        sibling locking method. ProfileManager's
        static_assert(sizeof(CRITICAL_SECTION)…) removed — it guarded
        completeness of the shim typedef under PCH include-order
        variance, which a std::mutex member declaration enforces by
        itself.
      - g_Lock became std::recursive_mutex, not std::mutex:
        AddRequestServerPlayer holds g_Lock across
        g_pGameMessage->AddFormat(), which re-locks it
        (CMessageArray.cpp:326) [measured]; CRITICAL_SECTION allowed
        that re-entry, a plain mutex would deadlock. Init + all 6
        WinMain exit-path deletes removed (all verified alternate exits
        of one call; one was doubly dead inside a comment). Caveat: the
        whole g_Lock arm is preprocessor-dead — OUTPUT_DEBUG is never
        defined (DebugInfo.h's define block is commented out) — so CI
        does not compile it; that commit is inspection-verified only
        [unverified by compiler].
      - MWorkThread.h m_csDeque/m_csCurrent migrated, fixing their
        never-initialized latent bug (both facts re-verified [measured]:
        zero init sites, zero callers of the four accessors).
      Trivial follow-up landed 2026-08-08: DebugLog.cpp's g_log_lock is
      now std::mutex too — all 12 sites (2 Initialize/Delete, 10
      Enter/Leave across 5 call sites) converted [measured]. Re-entrancy
      gate checked by hand: no locked region in the file calls back into
      another g_log_lock-locking function, so plain std::mutex is
      correct (5 of 6 precedent cases, not the g_Lock exception). The
      now-pointless Windows-only `<windows.h>` include and the
      non-Windows `basic/Platform.h` include (both existed solely to
      supply the CRITICAL_SECTION type) were dropped along with the
      dead `PLATFORM_LOCK_INITIALIZED` define; DebugLog.h:11's comment
      updated to match. Packet/Exception.h's __ENTER/__LEAVE_CRITICAL_SECTION
      macros (name collision — they wrap a passed-in mutex object) and
      GameInitInfo.cpp:11's already-stale comment are untouched, out of
      scope. With DebugLog.cpp off it, Platform.h's CRITICAL_SECTION shim
      [measured] has no remaining consumers in dkrix/ — candidate for a
      future removal pass, not done here.
      **Removal pass done 2026-08-09** — independently re-grepped the whole
      tree (`.h`/`.cpp`/`.H`/`.CPP`, no extension filter as a second pass)
      for `CRITICAL_SECTION`, `InitializeCriticalSection`,
      `EnterCriticalSection`, `LeaveCriticalSection`,
      `DeleteCriticalSection` before touching anything, per this file's own
      documented history of false "provably dead" claims. Confirmed zero
      real consumers [measured]: the only tree-wide hits outside
      `basic/Platform.h` were `CMessageArray.cpp:50` and
      `GameInitInfo.cpp:11` (comments only) and `Packet/Exception.h`'s
      `__ENTER_CRITICAL_SECTION`/`__LEAVE_CRITICAL_SECTION` macros, which
      take a `mutex` parameter and call `.lock()`/`.unlock()` on it — not
      the shim type — and have zero invocations anywhere in the tree
      besides their own definition. Deleted the shim (44 lines:
      `_CRITICAL_SECTION_DEFINED` guard, the pthread-backed
      `CRITICAL_SECTION` typedef, and the four
      Initialize/Enter/Leave/DeleteCriticalSection functions) from
      `basic/Platform.h`. **Found a real structural bug while doing it**,
      the kind this file has bitten prior work with before: the outer
      `#ifndef _CRITICAL_SECTION_DEFINED` did not close at the shim's own
      `#endif` — it stayed open across the entire unrelated GDI-stub block
      (`DeleteObject`, `LOGFONT`, font/charset/clip/quality/pitch
      constants, `CreateFontIndirect`, ~110 lines, confirmed live per the
      GDI-stubs item below) and only closed 113 lines later, right after
      that block's own `#endif`. A naive line-range delete of just the
      shim's nominal `#ifndef ... #endif` span would have orphaned that
      outer `#endif` and desynced every conditional after it. Fix: deleted
      the shim's open/close pair as a unit *and* the now-orphaned outer
      `#endif`, leaving the GDI-stub block's own `#ifndef
      PLATFORM_WINDOWS`/`#endif` standing alone, correctly matched —
      content of that block untouched. Verified with a stack-based
      `#if`/`#endif` balance check over the whole file post-edit: balances
      to 0 [measured]. `Platform.h`: 1,996 → 1,982 lines. No compiler
      available in this sandbox to confirm — gate is client CI, not run
      here.
- [x] Transitive-include sweep — **done 2026-08-09, exhaustively.** This
      file has cost the Linux port four separate CI rounds one missing
      include at a time (`<time.h>` for `GetLocalTime` at `8cabe14`,
      `<libgen.h>`, `<algorithm>` for `std::sort`, and the `strlen` noted
      as still-latent in the run #26–#28 entry above — that note is now
      stale, it is fixed). Enumerated the whole class instead of waiting
      for the next one.
      **Method [measured]:** rebuilt `8cabe14`'s standalone probe — compile
      `basic/Platform.h` alone under g++ 13.3.0 in WSL against a stub
      `SDL2/SDL.h` — and ran it against *two* stubs. One models what real
      `SDL_stdinc.h` drags in (`<stdio.h>`, `<string.h>`, `<stdlib.h>`,
      `<wchar.h>`, …); the other deliberately includes nothing at all. Any
      symbol that resolves under the first and not the second is surviving
      on a transitive include and is one build step from breaking. This is
      the technique that proved `strlen` load-bearing, generalised: it
      turns "what else might be missing?" into a measurement.
      Three sites, and **only** three, in the whole file:
      - `375` — `fprintf`/`stderr` in the `MessageBox` stub → `<stdio.h>`
      - `1462–1464` — `time_t` fields in `struct _finddata_t` → `<time.h>`
      - `1841` — `strlen` in the `GetCurrentDirectory` stub → `<string.h>`
      All three hoisted to the top of the file rather than patched in at
      each use site, so the next stub added below is covered too, and left
      unconditional rather than platform-guarded because all three are ISO
      C and MSVC has them — the Windows/non-Windows split that must stay
      (see the preamble) is about *typedefs*, not about standard headers.
      The now-redundant second copies of `<stdio.h>`/`<time.h>` further
      down were removed. After the change the bare-stub probe compiles
      clean, with and without `-DUNICODE`: **zero transitive survivors
      remain** [measured]. Commit `9d655ef`.
- [x] Duplicate typedefs, the rest of the class `cdf82ef` opened —
      **done 2026-08-09.** Extending the probe to `-DUNICODE`, the one
      branch a plain Linux compile never takes, turned up a hard error:
      `LPTSTR`/`LPCTSTR` were declared twice inside the same
      `#ifndef PLATFORM_WINDOWS`, once as a plain char-pointer pair and
      again 55 lines later off `TCHAR`. Legal only because `TCHAR` is
      `char` while `UNICODE` is undefined; `conflicting declaration` the
      moment it is not. Kept the `TCHAR`-derived pair (it is what the name
      means, and it makes the `UNICODE` branch above it coherent); the
      earlier pair had no use before it, so it was dead in every
      configuration. Commit `7f4dfb8`.
      Then enumerated that class too, the same way as the includes:
      every typedef name in the file, grouped, each repeat checked against
      its enclosing guards to separate real duplicates from `#if`/`#else`
      alternates. Eight names repeat. **Four pairs are correct and were
      kept** — `QWORD`, `ADDRESS_MODE`, `id_t`, `TCHAR` and the four
      `platform_*` handle types are one definition per arm of a mutually
      exclusive conditional, and for `id_t` that is precisely the split
      `067067f` introduced. **Six were genuinely both-active**, declared
      twice under the same condition with nothing conditional between:
      `BOOL` (158/295), `LONG` (157/313), `UINT` (145/328), `HANDLE`
      (319/1223), `HWND` (315/1224), `HINSTANCE` (318/1225). Each pair
      named the same type, so all six compiled — the `LPTSTR` hazard one
      step earlier, waiting for someone to widen one copy and get a
      redefinition error a thousand lines from the edit. Later copy
      deleted in each case, never the earlier. Commit `d878c25`.
      All four probe configurations (two stubs × `-DUNICODE` on/off)
      compile clean after both commits, and the `#if`/`#endif` balance
      re-checks to the single outer include guard — the structural failure
      mode that nearly bit the `CRITICAL_SECTION` removal.
      **Two dead duplicates left in place deliberately, flagged not fixed:**
      `INVALID_HANDLE_VALUE` (433 active, 1322 behind an `#ifndef` that can
      never fire) and `MAKELPARAM` (545 active, 1901 likewise). The second
      `MAKELPARAM` is not equivalent to the first — `545` does not mask the
      high word, `1901` routes through `MAKELONG` which does, so they
      disagree for `h > 0xFFFF`. Unreachable today, but anyone deleting
      `545` would silently change behaviour. Wants a decision, not a
      blind edit.
- [ ] GDI stubs — still deferred, but **the stated reason was stale and is
      corrected here [measured 2026-08-09]:** `LOGFONT` is no longer "a live
      parameter type in `Base::SetFont`". The `LOGFONT` typedef, its
      constants and the `CreateFontIndirect()` stub were removed from
      `Platform.h` under Phase 5 on 2026-08-09 — `Base::SetFont()` /
      `Base::SetDefaultLogfont()` now take a `TextSystem::FontSpec`
      (`Client/TextSystem/FontHandleUtil.h`), and the only `LOGFONT` hits
      left in the tree are comments. Re-checked what the block actually
      still holds, and it stays deferred on its own merits: `DeleteObject`
      has two live callers (`Client/Client.cpp:3553`,
      `VS_UI/src/VS_UI_WebBrowser.cpp:220`), `TRANSPARENT` / `TA_LEFT` /
      `TA_RIGHT` / `TA_CENTER` are live (including as default arguments in
      `VS_UI_Base.h:308`), and `DDSCAPS_SYSTEMMEMORY` — misfiled under
      "GDI" — has dozens of call sites across `Client/` and `VS_UI/`. Only
      `OPAQUE`, `TA_NOUPDATECP`, `TA_TOP`, `TA_UPDATECP` and `TA_BASELINE`
      are unused, which is not worth a commit on its own.
- Target: `Platform.h` shrinks to under 600 lines.
      **[measured 2026-08-09] 1,906 lines** (1,903 before this session's
      three commits; 1,995 on 2026-08-06; 1,968 baseline). Note the
      direction: this session *removed* six duplicate typedefs, two
      duplicate `#include`s and a duplicate typedef pair, and the file
      still ended three lines longer, because the comment explaining why
      the three hoisted standard headers belong at the top costs more
      lines than the declarations it replaced. That is the same effect
      recorded below and it is not a reason to drop the comment — the
      whole reason `<time.h>` was re-broken once already is that nothing
      recorded why it was there.
      **Under-600 is not reachable by this kind of work.** Every pass so
      far has removed genuinely dead code and moved the number by tens of
      lines, because what is left is mostly live: Win32 constants and
      typedefs with real call sites across `Client/` and `VS_UI/`. Getting
      to 600 means migrating callers off the Win32 vocabulary (Phases 4/5
      territory), not further auditing of this file. The target should
      probably be restated as such rather than carried as an open number.
      The two safely-completed Platform.h items nearly
      net out — comments explaining *why* the Windows/non-Windows split
      must stay cost about as many lines as the dead code removal saved.
      The real mass reduction available here is the mutex unification
      (done 2026-08-07 — see the item above; client CI pending) and a full
      HRESULT-machinery audit across ~20 files (still open). Neither was safe
      blind — but client CI has been green since 2026-08-06, so both are
      now schedulable as ordinary CI-gated changes.

### Phase 3 — Collapse DXLib into a thin SDL facade (client, items 1–3 done 2026-08-07)
- [x] **Item 1 — Rename `CSDLInput` → `InputManager`, `CSDLAudio` →
      `AudioManager`; delete the dead/duplicate shim files and grep-replace
      callers.** `git mv CDirectInput.{h,cpp→_Adapter.cpp} →
      InputManager.{h,cpp}` and `CDirectSound.{h,cpp} → AudioManager.{h,cpp}`,
      word-bounded rename of the two class names across every caller
      (`git grep` before/after confirms zero remaining `CSDLInput`/`CSDLAudio`
      hits, including a `CDirectInput` straggler in `VS_UI/WinMain.cpp` from
      an earlier incomplete rename). Deleted four files, verified dead/broken
      first, not just grep-flagged:
      - `Client/DXLib/CDirectInput.cpp` — excluded from `DXLIB_SOURCES`
        entirely (dead; the code that actually ships was always
        `CDirectInput_Adapter.cpp`, now `InputManager.cpp`).
      - `Client/DXLib/CDirectSound_Adapter.cpp` and
        `CDirectSoundStream_Adapter.cpp` — the flagged lead was right about a
        duplicate-symbol risk but understated it: both files fail to
        **compile**, not just link. Their ctor/dtor were still named after
        the pre-rename class (`AudioManager::CDirectSound()`,
        `CSDLStream::CDirectSoundStream()` — not valid member names for
        either class), and `CDirectSound_Adapter.cpp` declared its global
        instance with the type `CDirectSound`, which doesn't exist anywhere
        in the tree. Neither file could ever have built.
      - `Client/DXLib/CDirectMusic_Adapter.cpp` — compiles standalone but
        duplicates every symbol in `CDirectMusic.cpp` (ctor/dtor, global
        `g_SDLMusic`, all ten methods); guaranteed link failure whenever
        `HAVE_SDL2_MIXER` is on, which is the recommended/default config.
      - Also deleted two shadow header forwarders at `Client/CDirectInput.h`
        / `Client/CDirectSound.h` (`#include "DXLib/CDirect....h"` one-liners
        that shadowed the real headers for some callers depending on
        `-I` order) and repointed their ~7 consumers straight at
        `DXLib/InputManager.h` / `DXLib/AudioManager.h`.
      - `CDirectMusic` and `CDirectSoundStream` keep their existing names —
        not in the explicit rename list — only their dead `_Adapter.cpp`
        twin was removed in each case.
      - Net effect: `DXLIB_SOURCES` in `Client/DXLib/CMakeLists.txt` no
        longer branches on `HAVE_SDL2_MIXER` to add the adapters; it's one
        unconditional list with exactly one implementation per class. This
        was previously a guaranteed compile/link failure on the
        recommended build config (SDL2_mixer present) and is now fixed.
      - Left alone, found but out of scope for item 1: `WinMain.cpp:95-96`
        (`CDirectDraw gC_DD; CSpriteSurface gC_DDSurface;`) references types
        that don't exist under those names either (item 2 territory —
        `CDirectDraw` is `CSDLGraphics` now); `Client/GlobalVariables.cpp:199`
        declares an unrelated global `bool g_SDLAudio = false;` that
        collides by name (not type) with the real `AudioManager g_SDLAudio;`
        — a pre-existing bug, not touched; `CDirectMusic.cpp`'s constructor
        never initializes `m_OriginalTempo`/`m_CurrentTempo` (the deleted
        Adapter's ctor did) — pre-existing, unrelated to this item;
        `InputManager::s_KeyName` / `GetKeyName()` has no definition and no
        callers anywhere — pre-existing dead code, harmless while unused.
      - ~~Not build-verified~~ — **build-verified 2026-08-07** by client
        CI run #8 at the merged tip (`5ca240a`, SUCCESS, 28m30s), which
        compiled the renames and deletions. Original verification was
        grep-based call-site tracing plus reading every file in the
        duplicate/shim set end to end.
- [x] **Item 2 — executed 2026-08-07** (three commits, one per step, per
      the audit in `docs/phase3-item2-liveness-2026-08-07.md`). One delta
      found at execution time: a second dead `CDirectSetup.h` twin at
      `Client/` level the audit missed — same zero-reference evidence,
      deleted in step 1. Grep-verified zero dangling references to every
      deleted/renamed name; **not compile-verified in this session** —
      the push-triggered client CI run is the gate, same as item 1
      (which run #8 then confirmed). The executed plan:
      1. DELETE `CDirectSetup.h` + `CDirectSetupGetVersion.cpp` — in no
         CMake source list; both umbrella includes commented out.
      2. DELETE `DXLib/BIT_RES.{CPP,H}` — dead 2-line-diff twin of the
         **live** `Client/BIT_RES.CPP` (item-1's duplication pattern;
         six more dead DXLib MP3-decoder twins flagged for follow-up).
      3. RENAME `CDirectDraw.{cpp,h}` + `CDirectDraw_StaticMembers.cpp`
         → `CSDLGraphics*`: the class inside already IS `CSDLGraphics`
         (`CDirectDraw.h:171`; guard is `__CSDLGRAPHICS_H__`), and no
         `CDirectDraw` type exists anywhere. 2 CMake lines + 5 includes.
      4. STOP — `CDirectDrawSurface.{h,cpp}` stay: live class (value
         members, `new` sites, map values across MEventManager/
         MGuildMark/Profile/UIMessage/VS_UI), bridged to SpriteLib's
         *standalone* `CSpriteSurface` by `reinterpret_cast`
         (`MTopView.cpp:10952,17706`). That bridge is Phase-4-adjacent
         refactor territory, not an item-2 deletion.
      Also surfaced: `VS_UI/WinMain.cpp` is filtered out on WIN32 (its
      `CDirectDraw gC_DD;` resolves to nothing) but unfiltered on
      non-WIN32 — latent breakage for any future Linux client build.
- [x] **Item 3 — executed 2026-08-07** (two commits: delete + move).
      Deleted the ten files of the six dead DXLib MP3-decoder twin
      modules item 2 flagged (mp3/huffman/reader/soundbuf/subdecoder/
      synfilt — in no CMake list, zero external includers, live twins
      in Client/; DXLib/mp3.h even still included the bit_res.h item 2
      deleted). git mv'd the remaining 28 files to Client/Platform/,
      names kept, and deleted Client/DXLib/. Rewired: add_subdirectory
      + 8 parent include-dir lines (in place — -I order preserved) +
      the ${CMAKE_SOURCE_DIR} variant in SpriteLib/CMakeLists.txt:152;
      25 "DXLib/…"→"Platform/…" include lines across 22 files, incl.
      two nonstandard-spelling stragglers ("dxlib/…" in CMP3.cpp,
      "DXLib\…" in mp3.cpp) that only compiled on case-insensitive
      filesystems. Deliberately kept: the dxlib target/project name,
      DXLIB_* var/define, dxlib_* C API, DXLib*.{h,cpp} filenames, and
      Client/DXLib.h (the Client-level umbrella) — renaming the dxlib
      build/API identity is a follow-up item, as is deleting the moved
      dead leftovers (header.{cpp,h}, l3types.h, common.h + the six
      orphaned ogg/vorbis headers, same evidence as the deleted six).
      Grep-verified zero DXLib-dir references in build files and
      includes; **not compile-verified in this session** — the client
      CI run is the gate, same as items 1 and 2.
      **Follow-up executed 2026-08-08**: the deferred build/API identity
      rename. CMake target `dxlib` → `platform` (`Client/Platform/
      CMakeLists.txt`'s `project()`/`add_library()`/`target_link_libraries()`/
      `install(TARGETS …)`, plus 7 `target_link_libraries()` entries in the
      root `dkrix/CMakeLists.txt`); `DXLIB_*` defines → `PLATFORM_*`
      (`DXLIB_USE_SDL_BACKEND`, `DXLIB_BACKEND_{WINDOWS,SDL}`, `DXLIB_CAP_*`,
      the `__DXLIB_BACKEND_H__` include guard, and the `install(FILES …
      DESTINATION include/dxlib)` path → `include/platform`); `dxlib_*` C API
      → `platform_*` at every declaration (`DXLibBackend.h`) and call site
      (`DXLibBackendSDL.cpp`, `InputManager.cpp`, `AudioManager.cpp` comment,
      `CGameUpdate.cpp`, `CWaitUIUpdate.cpp`, `SDLMain.cpp`). Filenames
      (`DXLibBackend.h`, `DXLibBackendSDL.cpp`, `Client/Platform/DXLib.h`,
      `Client/DXLib.h`) and the umbrella header's own prose were left alone,
      per this item's original scope note. `Client/Platform/README.md`'s
      documented API-name table/examples updated to match; its stale
      pre-move content (dead `_Adapter.cpp` references, never-built
      `DXLibBackendWindows.cpp`) is unrelated and untouched. Pure rename, no
      behavior change. Grep-verified zero remaining `dxlib_`/`DXLIB_`/bare
      `dxlib` hits outside the kept filenames/umbrella content; **not
      compile-verified in this session** — next client CI run is the gate.
      **Follow-up 2 executed 2026-08-08 (`0e54e07`)**: deleted the "moved dead
      leftovers" this item flagged above — but at `Client/Platform/`, i.e.
      the ten files the `git mv` had carried over
      (`header.{cpp,h}`, `l3types.h`, `common.h`, `codec.h`, `config_types.h`,
      `ogg.h`, `os_types.h`, `vorbisenc.h`, `vorbisfile.h`) — not in any CMake
      source list or glob, zero external includers, no symbol reach-through;
      the live MP3 decoder already had its own twins of `header.{cpp,h}` and
      `common.h` at `Client/` proper. Also dropped the three now-dead
      `Client/Platform/{ogg,vorbisenc,vorbisfile}.h` `FMT_EXCLUDES` entries
      from `dkrix/Makefile`.
      **Follow-up 3, this task, re-verified from scratch 2026-08-09**: the
      prompt that dispatched this investigation predated `0e54e07` landing
      and still pointed at the (by-then-already-deleted) `Platform/` copies,
      so the first job was re-confirming what actually remains on disk. It
      does not match the old framing — `Client/Platform/` today holds none of
      the ten names above (`0e54e07` really did delete them, confirmed by
      directory listing); the *only* physical files matching
      `header.{cpp,h}` / `l3types.h` / `common.h` / `*ogg*` / `*vorbis*`
      anywhere under `dkrix/Client/` are four at `Client/` root
      (`header.cpp`, `header.h`, `l3types.h`, `common.h`) plus the
      already-known-live `COGGSTREAM.{CPP,H}` (Phase 10's case-sensitivity
      fix, unrelated to this item). `git log --follow` on all four shows none
      were ever touched by the item-3 `git mv` — they've been at `Client/`
      since the initial checkpoint commit, i.e. they are the "live twins in
      Client/" the earlier entries kept referring to, not leftover copies of
      the deleted set. Verified individually, full CMake-list → glob →
      include-chain → symbol-reach-through evidence per file:
      - **`Client/header.h` — LIVE.** `#include`d unconditionally by
        `Client/mp3.h:4`; defines `struct MP3Header`, whose only field types
        (`e_mode`, `e_sample_frequency`) come from `common.h`.
      - **`Client/header.cpp` — LIVE.** Swept into `CLIENT_MAIN_SOURCES` by
        the case-sensitive `Client/*.cpp` glob in `dkrix/CMakeLists.txt:688`
        (exact-case match, no filter excludes it). Defines
        `MP3_ReadHeader()`, called from `Client/mp3.cpp` at five call sites
        (lines 72, 137, 193, 238, 351).
      - **`Client/common.h` — LIVE.** `#include`d by `Client/header.h:4`,
        `Client/mp3.h:5`, and directly by `Client/reader.cpp:3`. Defines
        `e_mode`/`e_sample_frequency`/`UINT`/`BYTE`/`MAX`/`MIN`/`ABS` used
        throughout the live MP3 decoder cluster (`mp3.h`, `header.h`,
        `reader.cpp`).
      - **`Client/l3types.h` — DEAD, deleted this session.** Same evidence
        standard as the six `0e54e07` deleted: not named in any CMake list;
        not swept by any glob (it's a header, never a compiled unit, and no
        glob targets `Client/*.h`); its only two references anywhere in the
        repo are commented-out `#include "l3types.h"` lines in
        `Client/mp3.h:3` and in its own `Client/common.h:4` — never a live
        include. Its three type definitions (`gr_info_s`, `side_info_t`,
        `scalefac_t`) are dead weight even by name: `git grep` for all three
        across `dkrix/` shows every real use resolves to `Client/mp3.h`'s own
        byte-for-byte duplicate inline definitions (`mp3.h:19-56`), which is
        what `mp3.cpp`/`reader.cpp`/`subdecoder.cpp` actually build against
        — `l3types.h` was never in the compiled path. Deleted via
        `git rm dkrix/Client/l3types.h`; the two stale commented-out
        `#include` lines that named it were left untouched (cosmetic,
        already-commented, in two otherwise-live files — not worth a diff
        of its own).
      No compiler in this sandbox; verified by full CMake-source-list +
      glob-filter + include-chain + symbol-usage tracing, the same method
      `a107d63`/`0e54e07` used. Client CI on this push is the gate.

### Phase 4 — One sprite pipeline (client, re-scoped 2026-08-06)

> **Re-scoped by [ADR-0001](adr/0001-sprite-pipeline.md).** The original
> framing of this phase was wrong on three counts. Read the ADR before
> touching sprite code.
>
> 1. **There are three sprite code paths, not two.**
>    `Client/SpriteLib/` (53,566 LOC C++), `SpriteLibBackendSDL.cpp`
>    (1,371 LOC, a C ABI over SDL2 that the game calls directly from
>    `Client.cpp` / `SDLMain.cpp` / `ClientFunction.cpp`), and
>    `tools/engine/sprite/` (7,749 LOC **C**, built only under
>    `BUILD_ENGINE`, used only by `tools/viewers/`).
>    `SpriteLibBackendSDL.cpp` includes only `stdlib/string/stdio` — it has
>    **zero** references to `engine/sprite`. They are two independent SDL2
>    implementations.
> 2. **This phase was mis-sized by ~35×.** The 20 variant files total
>    **1,500 lines** against SpriteLib's 53,566. The real mass is
>    `CSpriteSurface.cpp` (13,644), `CSprite.cpp` (9,593),
>    `CIndexSprite.cpp` (7,244).
> 3. **The variant classes are serializers, not renderers.** `CSprite565`
>    overrides only `SaveToFile`/`LoadFromFile` — they decode SPK asset
>    data. "SDL converts pixel formats natively" does not make them
>    deletable; SDL converts in memory, it does not read a 5:5:5-encoded
>    sprite off disk. Deleting them is a silent-failure change.

Safe now (judgeable by reading) — both done 2026-08-07:

- [x] Correct `Client/SpriteLib/SPRITELIB_BACKEND_README.md` — its diagram
      attributed the SDL2 backend to `engine/sprite` and it was stamped
      "Production Ready / integration tests passing". Both false. The
      2026-08-06 pass (`31493fb`) fixed the top-of-file status stamp and the
      architecture diagram but missed two holdouts further down: the
      "Platform Support" table still claimed macOS "Fully Supported" and
      Windows "Can use DirectDraw or SDL2 backend" (DirectDraw is gone), and
      the closing "Summary" section still said "production-ready!" with
      "API tested and verified" / "Examples working". Also removed a
      "See Examples" snippet pointing at `./bin/sprite_backend_example`, a
      binary that has never existed in this repo. All now read "unverified".
- [x] Delete 8 dead files (312 lines): `CAlphaSpritePackList{555,565}.{cpp,h}`
      and `CSpritePackList{555,565}.{cpp,h}`. **Already done** — commit
      `76f13e1`, 2026-08-06: none of the four `.cpp` files were in
      `SPRITELIB_SOURCES`, every external reference to
      `CSpritePackList555/565` was commented out
      (`VS_UI/src/VS_UI_Item.cpp:280,282`, `VS_UI/src/VS_UI_util.cpp:686,688`),
      and the matching `SPRITELIB_HEADERS` lines were removed with the files.

Unblocked 2026-08-06 (CI green), not started:

- [ ] **4b** — collapse the two SDL2 backends. `SpriteLibBackendSDL.cpp`
      survives (it is what the game calls); `tools/engine/sprite/` is
      demoted to what it already is, the viewer tools' library.
- [x] Wire `engine/sprite`'s 11 test files (3,898 lines) into CI — the only
      automated tests in this repo. **[2026-08-08]** `tools/engine/sprite/CMakeLists.txt`
      now defines two CTest targets: `sprite_tests` (test_main.c's shared
      PASS/FAIL runner + the 9 other files it drives — test_color.c,
      test_animation.c, test_frame.c, test_framepack.c, test_index_sprite.c,
      test_sdl_framework.c, test_shadow_sprite.c, test_sprite.c,
      test_spritepack.c — linked against the `sprite` library) and
      `sprite_test_zone` (test_zone.c alone — it `#include`s `zone.c`
      directly and owns its own `main()`, so linking it against `sprite`
      would duplicate every `zone_*` symbol at link time). `enable_testing()`
      was added to the root `dkrix/CMakeLists.txt` inside the `BUILD_ENGINE`
      guard, since nothing in the tree called it before. Fixed a real bug
      found while wiring this up: `test_zone.c`'s `main()` called
      `test_zone_run_all()` but discarded its return value, so the binary
      always exited 0 regardless of failures — it now returns that result.
      `test_main.c`'s own runner already returned a correct nonzero exit
      code. Wired into `.github/workflows/client.yml`'s `sanitizers-linux`
      job (`ctest --test-dir build-<sanitizer>`) — the only leg where
      `BUILD_ENGINE` is ON; the Windows `build` job has it OFF by design, so
      nothing to run there. **Unverified**: no compiler in the sandbox that
      made this change, and `sanitizers-linux` has never gotten past a
      separate, in-progress `min`/`max` macro bug affecting every libstdc++
      include on Linux — so this is CTest-correct by inspection only, not by
      a passing run, until that bug lands and a subsequent CI run exercises
      this job end to end.
- [x] **4c** — audit shipped SPK assets in `Darkeden/` for the pixel
      encodings actually in use. That evidence, not the class count,
      decides the fate of the 555/565 serializers.

      **[measured 2026-08-08]** The premise ("check what encoding each SPK
      file uses") doesn't hold: SPK files carry no format tag. The answer is
      in the serializer code, not the bytes. `CSprite555::LoadFromFile`
      (`Client/SpriteLib/CSprite555.cpp:103-194`) reads the *identical* byte
      layout as `CSprite565::LoadFromFile` (`CSprite565.cpp:72-150`) — `u16
      width`, `u16 height`, then per row `u16 len` + `len` words of
      RLE-encoded pixel data — and only *after* reading converts every pixel
      565→555 in memory via `ColorDraw::Convert565to555`. `SaveToFile`
      mirrors this: convert 555→565, write the 565 bytes, convert back to
      restore the in-memory 555 copy. `CIndexSprite555/565.cpp` and
      `CAlphaSprite555/565.cpp` follow the same pattern. **On-disk SPK data
      is 5:6:5 in every case that exists in this codebase; "555" is an
      in-memory-only pixel representation, never a file encoding.** There is
      no such thing as a 555-encoded asset to find.

      The runtime switch that picks which in-memory class to build —
      `ColorDraw::Is565()` (`basic/ColorDraw.h:43`) and
      `CSDLGraphics::Is565()` (`Client/Platform/CSDLGraphics.h:231`) — is
      hardcoded `return true` in the SDL2 backend. The `else` branches that
      would construct `CSprite555` / `CIndexSprite555` / `CAlphaSprite555`
      (`CSpriteSet.cpp:58`, `CAlphaSpritePack.cpp:58`, `GameInit.cpp:431`,
      `MGuildMarkManager.cpp:140-141,336-337,848-849`) are unreachable in the
      current build — not just unused on disk, dead at runtime too.

      Cross-checked against the real assets: ran `tools/spk_tools.ps1`'s own
      `[count:u16][w,h,then-per-row-RLE]` reader, structure-only (no bitmap
      decode, for speed), against all 137 non-index sprite-pack files under
      `Darkeden/Data/Image/` (extensions `.spk .sspk .ispk .cfpk .afpk .efpk
      .ifpk .ppk .aspk .sppk`; the matching `*i` files are `CFileIndexTable`
      offset tables, not pixel data, and were excluded). 69 files — every
      `.spk`/`.sspk`/`.ispk` file, the exact families the tool was written
      for — parsed as well-formed RLE streams with no overrun. The other 68,
      entirely `.cfpk`/`.ppk`/`.aspk`/`.sppk`, fail to parse under that
      reader; a hex check of one (`vampire.cfpk`) shows the count field reads
      correctly (10) but the following bytes don't line up as `[w,h,...]`,
      meaning those pack types wrap the per-sprite payload in additional
      container framing (likely `CAlphaSpritePack`/`CFilterPack` header
      fields) that `spk_tools.ps1` was never built to parse. This is a
      **container-framing gap, not a pixel-format one** — the per-sprite row
      encoding those failures would expose, if parsed correctly, is the same
      `CAlphaSprite555/565::LoadFromFile` code path already shown uniform
      above, not a different pack-level pixel format. No file of any
      extension produced evidence of a distinct on-disk 555 encoding.

      Net: the 555/565 serializers are not deletable by evidence of asset
      encoding, because there is no asset-encoding distinction to find — but
      the `else` branches building the `*555` classes are runtime-dead today
      given the hardcoded `Is565()`.

- [x] `CAlphaSprite::Blt4444*` methods — re-check against asset evidence.

      **[measured 2026-08-08]** Not an asset-encoding question. These are
      render-time alpha-blend blit functions (`CAlphaSprite.cpp:1157-2925`:
      `Blt4444`/`Blt4444ClipLeft/Right/Width/Height`/`Blt4444NotTrans*`/
      `Blt4444SmallNotTrans`, plus the `memcpyAlpha4444`/
      `memcpyAlpha4444Small` helpers) that composite an already-loaded
      sprite's pixels (565 or 555 in memory, per above) into a
      4:4:4:4-format destination buffer for translucency — called live from
      `CSpriteSurface.cpp`, `CShadowSprite.cpp`, `CFilter.cpp`,
      `CAlphaSpritePal.cpp`, and `MTopView.cpp`. No `CSprite4444` /
      `CIndexSprite4444` / `CAlphaSprite4444` serializer class exists
      anywhere in `Client/SpriteLib/` — nothing is ever stored on disk in
      4444. 4444 is a render-target format, not a shipped-asset one; it is
      out of scope for the SPK-encoding question this item was flagged
      under.

Explicitly **rejected**: porting the client onto `tools/engine/sprite/`.
It is the better-structured artifact and holds the only tests, but it has
never rendered a frame of the game, and this would be a multi-month
rewrite of the highest-risk subsystem with no test coverage. See ADR-0001
Option B.

### Phase 5 — One text pipeline (client, mostly done — landed out of order)

> **[measured 2026-08-06]** This phase's main item shipped before Phases
> 1–4. `VS_UI_Base.cpp` now includes `TextSystem/FontHandleUtil.h` and
> `Base::SetFont` builds a `TextSystem` font handle for every platform.
> No GDI call remains in the file.
>
> **[measured 2026-08-09]** The GDI-stub-deletion item below is also done:
> `basic/Platform.h` no longer defines `LOGFONT` (or its associated
> constants) on any platform, and `Base::SetFont`/`SetDefaultFontSpec`
> take a `TextSystem::FontSpec` instead. Not compiler-verified — see the
> caveat in that item's own note. Only the glyph-coverage item remains
> open in this phase.

- [x] In `VS_UI/src/VS_UI_Base.cpp`, remove the `#ifdef
      PLATFORM_WINDOWS` GDI branch and route Windows through
      `TextSystem` like every other platform.
- [ ] Verify Korean and Chinese glyph coverage in the fallback fonts.
      **Not verified. This is the risk item created by shipping the switch
      early** — the client renders Korean and Chinese game data, and a
      missing glyph range would show as blank or box characters rather
      than a crash. Needs a human looking at a running client, ideally at
      both QHD and FHD.
- [x] Delete the GDI stubs in `Platform.h` (previously deferred from
      Phase 2). **[measured 2026-08-09] Implemented** — see the
      "Implemented" note below the sizing report. **Original blocker, for
      context:** `Base::SetFont` used to keep
      `LOGFONT &lf` in its signature and read `lf.lfFaceName` /
      `lf.lfHeight`. Removing the stub required introducing a small
      `TextSystem` font-spec struct and updating every caller — a real
      refactor, not a deletion. It was sized first, then implemented.

      **[measured 2026-08-07] Sized, ready to schedule — no code changed.**
      Grepped `dkrix/` for `SetFont(`, `LOGFONT`, `DeleteObject`, and
      `CreateFontIndirect`; every hit is accounted for below.

      *The stub in `basic/Platform.h`.* `DeleteObject`, the `LOGFONT`
      typedef, and `CreateFontIndirect` all live inside one
      `#ifndef PLATFORM_WINDOWS` block (`Platform.h:262-372`). On the
      primary Windows build this code is never compiled at all — real
      `windows.h` supplies the genuine `LOGFONT`/`DeleteObject`/
      `CreateFontIndirect`, field-compatible with the stub by design. The
      stub only exists for the secondary macOS/Linux path. So "delete the
      stub" really means: stop needing a `LOGFONT`-shaped type on *any*
      platform, not just patch the non-Windows fallback.

      *`Base::SetFont` itself already only reads two of `LOGFONT`'s 13
      fields* — `lf.lfFaceName` (matched by `strcmp` against two literal
      strings, `"Cormorant Garamond"` / `"UnifrakturCook"`, to pick a
      `TextSystem::FontFamily*` enum value) and `lf.lfHeight` (passed
      straight through to `TextSystem::EncodeFontHandle`). Callers also set
      `lf.lfWeight` and `lf.lfItalic`, but `SetFont` never reads them —
      those two fields are already dead as far as this call chain goes.
      A replacement struct therefore needs exactly two fields: a font
      family selector (enum or face-name string) and an integer size.
      Something like:
      ```cpp
      struct FontSpec { TextSystem::FontFamily family; int height; };
      ```

      *Callers of `Base::SetFont`, exhaustively — zero outside one file.*
      `SetFont` appears in exactly four files in `dkrix/`:
      `VS_UI/src/header/VS_UI_Base.h` (the declaration),
      `VS_UI/src/VS_UI_Base.cpp` (the definition and every call site), and
      two archived docs under `dkrix/docs/archive/2026-migration-notes/`
      (`GDI_USAGE_ANALYSIS.md`, `TEXT_MIGRATION.md` — reference material,
      not code). There is no caller anywhere else in the client — no other
      `.cpp` file, no subclass, calls `Base::SetFont`. Inside
      `VS_UI_Base.cpp` there are 20 live call sites plus 1 commented-out
      call (`:281`), **all inside the single function `Base::InitFont()`**
      (`VS_UI_Base.cpp:166-333`). Each follows the identical pattern:
      `SetDefaultLogfont(lf)` → mutate 2-4 `lf.*` fields → `SetFont(pi_member,
      lf, color, ...)`. `SetDefaultLogfont` (`VS_UI_Base.cpp:82-99`) has the
      same one-file, one-function blast radius and would be rewritten
      alongside it (it currently zero-initializes all 13 `LOGFONT` fields;
      the replacement only needs to default the 2 that matter).

      *Blast radius:* **3 files** need real changes —
      `basic/Platform.h` (delete the stub block), `VS_UI/src/header/VS_UI_Base.h`
      (change the `SetFont`/`SetDefaultLogfont` signatures from `LOGFONT &lf`
      to `FontSpec &spec`), `VS_UI/src/VS_UI_Base.cpp` (rewrite the two
      function bodies plus all 20 call sites — mechanical, same pattern
      each time, all in one ~170-line function). Two archived `.md` files
      would go stale but need no edit (they're explicitly historical).

      *A separate, unrelated snag in the same stub block:* `DeleteObject`
      (also deleted by this same change) has exactly two live callers
      outside `Platform.h` — `VS_UI/src/VS_UI_WebBrowser.cpp:220` and
      `Client/Client.cpp:3564` — and neither is about fonts. Both pass an
      `IWebBrowser2*` COM pointer to `DeleteObject()` immediately after
      calling `->Release()` on it, which is a pre-existing correctness
      question (COM objects are released, not GDI-deleted) independent of
      this refactor. Removing the stub forces a decision on those two call
      sites — most likely deleting the now-redundant `DeleteObject()` call
      entirely, since `Release()` already ran — but that is a second,
      separable finding, not part of the font-spec work. `CreateFontIndirect`
      has zero live callers anywhere (only comments and the two archived
      docs), so deleting it is a pure no-op.

      *Estimate:* small — 3 files, ~25 call sites, all mechanical and
      concentrated in one function, plus a 2-call-site side-cleanup that
      isn't about fonts at all. Ready to schedule; still gated on Phase 5's
      other open item (Korean/Chinese glyph verification) landing first, or
      at least not regressing, since this touches the same call chain that
      builds every font handle in the UI.

      **[measured 2026-08-09] Implemented, no compiler available to verify
      (see caveat below).** Re-ran the same grep sweep fresh (not just
      trusted the 2026-08-07 numbers) before touching anything; the
      inventory above held with one minor correction — `Base::InitFont()`
      has **21** live `SetFont` call sites, not 20 (the extra one is
      `m_char_chat_pi`; the commented-out `m_chat_dialog_pi` call at the
      old `:281` is still there and still commented out). Everything else
      — zero callers outside `VS_UI/src/VS_UI_Base.{h,cpp}`, the two
      dead-field observations (`lfWeight`/`lfItalic` set but never read),
      and the unrelated `DeleteObject` COM-pointer snag — matched exactly.

      Changes made:
      - `Client/TextSystem/FontHandleUtil.h`: added
        `struct FontSpec { FontFamilyId family; int height; };` next to
        the existing `FontFamilyId` enum and `EncodeFontHandle`/
        `DecodeFont*Handle` helpers it already fed — natural home, no new
        file.
      - `VS_UI/src/header/VS_UI_Base.h`: added
        `#include "TextSystem/FontHandleUtil.h"`; changed
        `SetDefaultLogfont(LOGFONT &lf) const` →
        `SetDefaultFontSpec(TextSystem::FontSpec &spec) const` and
        `SetFont(PrintInfo &pi, LOGFONT &lf, ...)` →
        `SetFont(PrintInfo &pi, const TextSystem::FontSpec &spec, ...)`
        (renamed alongside the type change since "logfont" is no longer
        accurate; `SetFont`'s own name didn't need to change).
      - `VS_UI/src/VS_UI_Base.cpp`: `SetFont` now reads `spec.family` /
        `spec.height` directly — the old `strcmp(lf.lfFaceName, ...)`
        dispatch is gone because `InitFont()` now sets the
        `TextSystem::FontFamilyId` enum value directly instead of a face
        name string, so there is nothing left to string-compare.
        `SetDefaultFontSpec` sets exactly the two fields that matter
        (`height = 0`, `family = FontFamilyCormorantGaramond` — the same
        effective default as before). `InitFont()`'s 21 call sites were
        mechanically rewritten one-for-one (`LOGFONT lf` →
        `TextSystem::FontSpec fs`; `lf.lfHeight = N` → `fs.height = N`;
        `strcpy(lf.lfFaceName, bodyFont/menuFont)` → `fs.family =
        FontFamilyCormorantGaramond/FontFamilyUnifrakturCook` since those
        were the only two strings ever passed); the dead `lf.lfWeight =
        FW_BOLD` / `lf.lfItalic = 1` lines (live and already-commented
        alike) were dropped since `FontSpec` has no equivalent field and
        `SetFont` never read them — confirmed behavior-neutral, not just
        assumed.
      - `basic/Platform.h`: removed the `LOGFONT` typedef, the
        font-weight/charset/output-precision/clip-precision/quality/
        pitch-and-family constant blocks, and the `CreateFontIndirect`
        stub. Left `DeleteObject` (real, unrelated callers — see the
        side-cleanup note above, deliberately not touched by this change),
        `TRANSPARENT`/`OPAQUE`, and the `TA_*` text-alignment constants in
        place — `SetFont`'s own `bk_mode`/`align` parameters still use
        them and they carry no GDI-font-specific semantics. Left a comment
        pointing at this Phase 5 entry for why the block shrank.
      - Verified after the edit: grepped the whole `dkrix/` tree for every
        removed symbol (`LOGFONT`, `CreateFontIndirect`, `FW_*`,
        `*_CHARSET`, `OUT_*_PRECIS`, `CLIP_*`, `*_QUALITY`, `*_PITCH`,
        `FF_*`) — zero live hits remain; the only matches left are this
        doc, the two archived `.md` files (explicitly historical, per the
        sizing report), and the explanatory comments this change itself
        added.

      **Caveat — not compiler-verified.** No Windows+VS2022+vcpkg
      toolchain is available in this sandbox (see `../CLAUDE.md`'s
      "Sandbox capabilities" — CI is the substitute, and this change
      wasn't pushed through it yet). The edit is textually exhaustive and
      mechanical, but the first real build (native Windows or client CI)
      should be treated as the actual verification step, not this note.
      If it doesn't compile clean, the likely culprits are the new
      `#include "TextSystem/FontHandleUtil.h"` in `VS_UI_Base.h` (path
      resolves via `VS_UI`'s existing `${CMAKE_CURRENT_SOURCE_DIR}` and
      `Client/TextSystem` include dirs in `CMakeLists.txt`, both already
      relied on by `VS_UI_Base.cpp`'s pre-existing identical include) or
      the `SetFont`/`SetDefaultFontSpec` signature edits not matching at
      every call site.
      - **Also unverified, pre-existing and unrelated to this change:**
        Phase 5's other open item (Korean/Chinese glyph coverage in the
        fallback fonts) — this refactor doesn't touch glyph rendering, only
        how the family/size get to `TextSystem::EncodeFontHandle`, but
        both should be checked in the same human pass on a running client.

### Phase 6 — Modern C++ as we touch it (ongoing)
- Rule of thumb when a file is already being modified for another
  reason:
  - Prefer `std::unique_ptr` / RAII over raw `new`/`delete`.
  - Use `<thread>`, `<mutex>`, `<condition_variable>` for new
    concurrency.
  - Use `<cstdint>` types (`std::uint32_t`, etc.) for new code; do
    not mass-rename existing `DWORD`/`BYTE`/`BOOL` usage.
  - Translate Korean/Chinese comments to English.

- **[2026-08-09] Dead `__LINUX__`/`__WINDOWS__` conditionals (`docs/
  TECH-DEBT-AUDIT.md` item 20) — partially converted, now that both
  trees' CI is green.** Re-measured 75 raw occurrences across 19
  `Client/Packet/` files (vs. the 2026-08-06 estimate of 66/16 — the
  gap is files where every hit sits inside a comment, which the
  original grep-based count didn't distinguish from live directives).
  63 were renamed to the real `PLATFORM_LINUX`/`PLATFORM_WINDOWS`
  macros `Client/Packet/SocketAPI.h` already defines; 1 was left as
  dead `__LINUX__` on purpose (converting it would have changed
  observable post-character-delete behavior on Linux, not just fixed
  dead code — see the audit doc); 11 were left alone because they were
  never live directives (inside comments, or an unreachable `#elif`
  arm of an always-true `#ifdef __GAME_CLIENT__`). Full file/line list
  and reasoning: `docs/TECH-DEBT-AUDIT.md` item 20. Every touched file
  was checked for `#if`/`#endif` balance before and after; all ten
  stayed balanced. Not yet run through either CI — the client's
  blocking Windows job exercises the `PLATFORM_WINDOWS` arms, the
  non-blocking `sanitizers-linux` job exercises the `PLATFORM_LINUX`
  arms for the first time ever.

### Phase 7 — Server: retire dead binaries — done 2026-08-06/07

> **Correction to the original wording:** `chinabilling/` top-level is
> **not** dead — `src/server/chinabilling/CMakeLists.txt` builds it into
> the `GameServerCBilling` and `LoginServerCBilling` static libs, which
> `gameserver` and `loginserver` link. Only its two subdirs (`stress/`,
> `testserver/`) were dead. "All three subdirs" in the earlier wording
> was wrong on both count and contents — there are two.
>
> **Landed in two passes on two independent review branches, merged
> together 2026-08-07.** The first pass (below) did the directory
> deletions, `__OLD_GUILD_WAR__`, and `DatabaseManager.old.cpp`. A
> second, parallel pass branched from `main` *before* the first pass
> merged, so it independently re-measured this section and (correctly,
> from its own branch's point of view) found the dead trees still
> present — see its findings folded in below. Both passes verified
> clean with `make debug` under WSL; merging them together does not
> require re-verification of either individually, since neither
> touched a file the other did.

- [x] Delete `cacheserver/`, `theoneserver/`, `updateserver/`,
      `chinabilling/stress/`, `chinabilling/testserver/`,
      `gameserver/mofus/testserver/`. Verified with a repo-wide grep
      of every `CMakeLists.txt` for each directory name (zero hits)
      before deleting, and confirmed with a CMake configure pass
      (MSVC generator; this workstation lacks the Linux toolchain
      for a real `make debug` at grep time) that reached the
      `find_package` stage without any `add_subdirectory` path
      errors.
- [x] Delete `gameserver/testAlone/` outright (including a stray
      326 KB prebuilt `testAlone` binary that had been committed
      alongside the sources) rather than upgrading `testAlone/Mutex.h`
      to a real primitive — nothing outside the directory referenced
      it (repo-wide grep for `testAlone` turned up only the
      directory's own Makefile), so there was no live caller left to
      benefit from a real mutex.
- [x] Retire `__OLD_GUILD_WAR__` `#ifdef` gates. 41 occurrences
      across 26 files (40 live directives + one already-commented-out
      `#define` in `Core/Types.h`). `__OLD_GUILD_WAR__` is never
      defined anywhere — no `CMakeLists.txt`, no
      `target_compile_definitions`, no conf file, and the one
      `#define` was commented out. Deleted the dead branch of every
      gate, kept the branch that was actually live (the "new guild
      war" / `SiegeWar` path), and removed the pointless directives.
      No behavior change — every deleted line was preprocessor-dead.
      Verified with a per-file `#if`/`#ifdef`/`#ifndef` vs `#endif`
      balance check across all 25 touched files (all balanced) plus
      the CMake configure pass below.
- [x] Delete `.old.cpp` files in `server/database/` —
      `DatabaseManager.old.cpp`, single file, not in
      `CMakeLists.txt`'s source list.
- [x] **Follow-up pass (2026-08-07), Makefile/packet-dir cleanup:**
  - Legacy Makefile references to the now-deleted trees removed from
    `src/Makefile`, `src/server/Makefile`, `src/Core/Makefile` (no
    target — `alltheoneserver`, `cleanbin`, `clean`, `cleanall`,
    `cleangameserver`, `cleanloginserver`, `cleansharedserver`,
    `allloginserver`, `allgameserver`, `allsharedserver` — still
    invokes `theoneserver`/`updateserver`/`TOpackets`/`Upackets`).
    `chinabilling` references were correctly left alone — it has a
    live `CMakeLists.txt` and is part of the real build, per the
    correction above.
  - `src/Core/TOpackets/`, `src/Core/Upackets/` deleted — both
    Makefile-only (`TOpackets/Makefile` built against `theoneserver/`,
    `Upackets/Makefile` against `updateserver/`+`updateclient/`),
    zero references in any `CMakeLists.txt`.
  - `gameserver/skill/EffectStriking.old.cpp`/`.old.h` deleted — same
    dead-code shape as `DatabaseManager.old.cpp`: the live
    `EffectStriking.cpp` (operating on `Creature*`) is the one listed
    in `gameserver/skill/CMakeLists.txt:370`; the `.old.*` pair
    (operating on the older `Item*` signature) appeared nowhere in any
    `CMakeLists.txt` and had no includer outside itself.
  - **Caught a real break before it shipped:** deleting `TOpackets/`
    would have broken the build via an unconditional
    `#include "TOpackets/GTOAcknowledgement.h"` in
    `gameserver/ClientManager.cpp`, whose only use was already gated
    behind a dead `#if defined(__CHINA_SERVER__) || defined(__THAILAND_SERVER__)`
    (both macros undefined everywhere). Guarded the include to match.

**Build verification — [measured 2026-08-06/07], not just grep.** This
workstation has a full Linux toolchain via WSL (`Ubuntu`, with
`libmysqlclient-dev`, `liblua5.1-dev`, `libxerces-c-dev`,
`libnsl-dev` already installed). Ran the actual authoritative command
— `cd dkrixserver && make debug` — from two separate worktrees against
each pass's changes independently. **Both exited 0** and produced all
three binaries fresh: `bin/gameserver` (403 MB debug), `bin/loginserver`
(17 MB), `bin/sharedserver` (9.8 MB) — the last build target reached
and linked was `gameserver`, which is the binary that actually contains
every `__OLD_GUILD_WAR__` edit (`Guild.h` ×2, `GuildManager.cpp` ×2,
`war/*`, `quest/Action*Reinforce/Siege.cpp`) plus the deleted-directory,
`.old.cpp`, and follow-up-pass changes. Only pre-existing warnings
(`-Wdeprecated-declarations` on `std::binary_function`,
`-Wsign-compare`), no errors. `clang-format` is not installed in this
WSL image, so `make fmt` could not be run in either pass; formatting
was matched by hand to the surrounding style in each edit.

**Outcome:** landed as two review branches, neither pushed nor merged
to `main` at authoring time, each independently green under
`make debug`; merged together into `main` 2026-08-07.

### Phase 8 — Server: SQL and secrets (split; secrets half in progress)

> **[measured 2026-08-06] Split this phase.** The two halves have very
> different costs and should not gate each other:
>
> - **Secrets** — 34 plaintext passwords across 13 tracked `conf/` files.
>   Audit priority 28.
> - **SQL** — 26 `sprintf(query, …)` sites (not the "~87" this file used to
>   claim; that figure never reproduced). Audit priority 12, a real refactor.
>
> **Secrets, step 1 of 2 — done.** `Properties::load()` now expands `${VAR}`
> placeholders against the environment, and
> `conf/{gameserver,loginserver,sharedserver}.conf.template` promote every
> credential and host/IP to a `DKRIX_*` variable (16 distinct). Ported by hand
> from the parked line's `4760660`, preserving this tree's `\r` whitespace fix.
> Verified as far as is possible without a server build: the function compiles
> clean standalone under `-Wall -Wextra` with 13 passing assertions,
> `-fsyntax-only` reports zero errors originating in `Properties.cpp`, and **no
> conf file contains `${`**, so it is a provable no-op on the current
> deployment.
>
> **Secrets, step 2 of 2 — not done.** The plaintext credentials are still in
> the tracked conf files and in git history. Rotating to the template flow
> (copy `.template` → `.conf` on the host, export `DKRIX_*` in the service
> unit) is a deployment change and needs a running server to validate.
- [ ] Introduce a `PreparedStatement` wrapper. **Not a refactor — a
      from-scratch API:** zero `mysql_stmt_*` usage exists anywhere; the
      live path is `Statement::executeQuery(fmt, …)` → `vsprintf` →
      `mysql_real_query` (parked line's audit, still true here). The
      parked line designed, shipped, and build-verified exactly this as
      its Phase 11.1 — lift that design. See Phase 11 below.
- [ ] Migrate call sites module by module, starting with user-string
      sites (chat, pet names, custom options). Real size on `main`
      **[measured 2026-08-07]: 598** `executeQuery` sites with `%s`/`%d`
      format specifiers (not "~87", and not the 9 surviving
      `sprintf(query` hits — exact greps in Ground truth above).
      Ratchet-driven, never a single big-bang close-out.
- [x] ~~Add the CI ratchet.~~ **Done 2026-08-07 (Phase 16):**
      `scripts/check-sql-injection.sh` lifted and re-baselined at **542**
      by its own, broader metric (`executeQuery`/`setStatement`/
      `Statement` with `%[sdluxc]`; the 598 above is `executeQuery`-only
      — both recorded, the script's count is what the gate enforces),
      wired into `server.yml` as the `ratchets` job.
- [x] ~~Move credentials/hosts from `conf/*.conf` into environment
      variables with `${VAR}` templates.~~ **Step 1 shipped** —
      `Properties::load()` expansion + `DKRIX_*` templates (see the note
      above). Step 2 — rotate the real deployment onto the templates and
      purge plaintext from tracked conf — is open, needs a live-server
      window, and is a do-not-delegate item.

### Phase 9 — Server: Lua sandbox and packet schema (mechanical items done 2026-08-07)

> **[measured 2026-08-07]** All three mechanical items below were
> verified with `cd dkrixserver && make debug` in WSL (gcc 13,
> `liblua5.1-0-dev`) — a real compile and link of all three binaries
> (`bin/{gameserver,loginserver,sharedserver}`), not just a grep. This
> is stronger evidence than the syntax-only checks the parked line
> (`archive/modernization-phases-1-17`) could do without a toolchain.
> The fourth item (packet schema sharing) is untouched — see the
> proposal note below it.
>
> **Scope audit — single call site, matches the parked line's finding.**
> `luaL_openlibs()` is called exactly once, in
> `dkrixserver/src/server/gameserver/quest/luaScript/LuaState.cpp:init()`.
> Grepped every real `*.lua` under the repo
> (`dkrixserver/data/lua/**`, `quest/luaScript/test/*.lua`, 18 files
> total; excludes `quest/luaScript/test/lt`, which is a leftover
> compiled ELF binary, not a script) for `io.`, `os.`, `debug.`,
> `require`/`package.`, `table.`, `string.`, `math.` — **zero
> matches** in any of them. `io` is confirmed unused; dropping it is
> not a behavior change.
>
> **`table` is also dropped, not just `io`/`os`/`debug`/`package`,
> matching this file's original `base`/`math`/`string` spec** (the
> parked line's Phase 9 additionally kept `table` — that was not
> re-verified here). Two scripts (`xmasEventCommon.lua` and its
> siblings) call bare `random(...)` and `getn(...)` — old Lua
> 4.x/5.0-era globals, not `math.random`/`table.getn`. Built a
> throwaway C harness against this WSL image's actual
> `liblua5.1-0-dev` (`/usr/include/lua5.1/luaconf.h` has
> `#undef LUA_COMPAT_GETN`) and confirmed **both `random` and `getn`
> are already `nil` globals even with the old, unrestricted
> `luaL_openlibs()`** — i.e. `selectOne()` in those scripts already
> throws "attempt to call a nil value" today, on `main`, independent
> of this change. Dropping `table` from the whitelist does not make
> this any more broken than it already was. Not fixed here — Lua
> version drift in event-script content is a separate, pre-existing
> issue, not a Phase 9 mechanical item.
>
> **Also discovered, not fixed:** `LuaState::randomseed()` calls
> `luaL_dostring(m_pState, "randomseed(1234)")`, and `randomseed` is
> the same kind of missing 4.x/5.0-era global — this call has been
> silently failing (return value unchecked) on every `LuaState::init()`
> under the old `luaL_openlibs()` too. Pre-existing, unrelated to the
> whitelist change; flagging per the "call out as you find them"
> house rule rather than fixing it blind.

- [x] Replace `luaL_openlibs()` with a whitelist (`base`, `math`,
      `string`); drop `io`. **Shipped:** whitelist implemented via the
      `luaopen_base`/`luaopen_string`/`luaopen_math` +
      `lua_pushcfunction`/`lua_call` idiom (mirrors Lua 5.1's own
      `linit.c`) in `LuaState::init()`. `io`/`os`/`debug`/`package`/
      `table` all dropped — see scope audit above for why `table`
      does not break any currently-working script.
- [x] Wrap every `lua_to*` → enum cast in a range-checked helper.
      **Scope audit:** exactly four call sites, all in
      `LuaSelectItem.cpp::executeFile()` (`ItemClass`, `ItemType`,
      `OptionType`, `OptionType2`, read via `lua_tonumber` + C-style
      cast) — matches the parked line's finding. **Shipped:** new
      `template<T> lua_toboundedenum(lua_State*, int idx, long min,
      long max)` in `LuaState.h`, throwing `InvalidProtocolException`
      (slot index + observed value + legal range) on a non-numeric
      slot or an out-of-range value. Migrated all four sites in
      `LuaSelectItem.cpp` (`ItemClass`: `0..ITEM_CLASS_MAX-1`;
      `ItemType_t`/`OptionType_t`: `0..0xFFFF`/`0..0xFF`, their
      natural `WORD`/`BYTE` ceilings).
- [ ] Share packet definitions between client and server: either a
      git submodule of `src/Core/Packets/` consumed by both, or a
      generated file from a small schema (pick when we get there,
      record the decision here). **Not attempted — architectural
      decision, deliberately out of scope for the mechanical pass.**
      **Proposal, not a decision:** the parked line's 2026-04-19 scope
      audit (`archive/modernization-phases-1-17`, now Phase 12 there)
      found ~149 `.cpp/.h` under `dkrix/Client/Packet/` plus ~326
      under `dkrix/Client/Packet/Cpackets/`, near-duplicated against
      ~800 files under `dkrixserver/src/Core/` (`CG*`/`GC*`/`CL*`/
      `LC*`/`GS*`/`SG*`/`GT*`/`TG*`), differing from their server
      counterparts only in whitespace/exception-spec/comments — wire
      format identical on the pairs spot-checked. A git submodule of
      `src/Core/Packets/` consumed by both trees is the lower-risk of
      the two options: it requires no new tooling (a codegen schema
      would need a generator, a template language, and a migration
      of ~1,100 files' worth of hand-written parsing quirks), and the
      packet classes are already near-identical, so promoting one
      side to canonical is mostly a `git mv` + include-path exercise
      per file rather than a rewrite. Recommend booking this as its
      own phase (the parked line called it Phase 12) sized around
      "promote `dkrixserver/src/Core/` packet headers to a submodule,
      point `dkrix/Client/Packet/` at it, delete the client copies" —
      multi-day, not a single commit.
      **Caveat found after that proposal was written — the parked line's
      own later audit (its Phase 17) invalidated the "differs only in
      whitespace" spot-check:** across the 326 name-matched pairs,
      **0 files are bit-identical and only 7 are cosmetic-only
      divergent**. The submodule promotion is still the right shape, but
      it is a per-file reconciliation, not a `git mv`. Read the tag's
      Phase 17 before sizing. Booked as Phase 12 below.
- [x] Add endian-safe read/write primitives in
      `SocketInputStream` / `SocketOutputStream`. **Scope check:**
      zero `ntoh*`/`hton*`/`htole*`/`le*toh` calls anywhere near these
      two classes; the existing `template<T> read(T&)` /
      `template<T> write(T)` do a raw host-order memory cast
      (`buf = *(T*)(m_Buffer + m_Head)`), which is the wire format
      every existing client/server build already speaks (both build
      for x86/x86-64, i.e. little-endian) — confirmed by the
      unrelated `ntohs`/`ntohl` usage that *does* exist elsewhere
      (`Datagram.h`, `chinabilling/CBillingPacketHeader.cpp`), which
      is a different protocol, not this one. **Shipped as new opt-in
      methods, not a replacement:** `template<T> readLE(T&)` on
      `SocketInputStream` and `template<T> writeLE(T)` on
      `SocketOutputStream`, restricted to arithmetic `T` via
      `static_assert`. Both decode/encode explicit little-endian byte
      order via a runtime host-endianness probe + `std::reverse`, so
      they are byte-for-byte identical to `read<T>`/`write<T>` on
      every host this project ships on today, but would still be
      correct on a big-endian target. Existing `read<T>`/`write<T>`
      and every packet call site are untouched — changing the wire
      protocol's default is a coordinated client+server change and is
      out of scope here.

### Phase 10 — Build hygiene & CI (not started here; parked line finished it as its 10+14+15)
- [x] Add `.clang-format` to `dkrix/` plus a client `Makefile` with
      `fmt` / `fmt-check` targets — done 2026-08-07 (`cb9cbae`; adjust
      SHA if applied with `--reset-author`). Premise correction: the
      `a760899` pointer above is dangling [measured — `git cat-file -t`
      fails]; the parked line's real commit is `52b141c` (its 10B),
      whose `.clang-format` is byte-identical to `dkrixserver/`'s —
      that file was copied verbatim. Mechanism improves on 10B: file
      set from `git ls-files` (never `find`), 21 vendored files
      excluded (`Client/JpegLib/`, the flat zlib 1.1.x headers in
      `Client/`, the Xiph headers — at `Client/Platform/` after Phase 3
      item 3), and `fmt-check` uses `git diff --relative` — without it
      every path is repo-root-relative and silently skipped from
      `dkrix/` (the server's own `fmt-check` had that latent bug —
      fixed 2026-08-09, `b147f2b`, reproduced in-tree first; its CI
      job was never affected since it runs from the repo root). [measured] Census at pinned 18.1.8:
      **2,199 of 2,227** in-scope files unformatted, ~946k diff lines;
      0 clang-format errors, 0 non-UTF-8 files, 0 in-repo CRLF. No
      source reformatted. CI gate wired 2026-08-09 (`86208bc`): a
      `format` job in `client.yml` — changed-files-only, pinned
      18.1.8 via pip (ce4a73f rationale), server.yml's checked-zero
      guard (60f4c35), scoped by cd-into-dkrix + `git diff --relative`
      + the Makefile's own pathspecs. Exclude list synced to the
      Makefile's current 12 entries — the stream-② snippet's five
      DXLib Xiph excludes were dropped because those files have since
      been deleted [measured: `git ls-files` returns none]. Scope at
      wiring time [measured 2026-08-09]: 2,090 in-scope files of
      2,106 tracked C++ (the 08-07 census's 2,227/2,248 predates the
      Phase 2/3/5 dead-code deletions). Shell verified by local
      dry-run only — first CI run of the job is the compile-truth
      gate [unverified until it runs]. Mass-reformat decision:
      approved 2026-08-09 — chunked per-directory, each chunk ridden
      through client CI, scheduled only after Phase 18 passes.
- [x] Replace `file(GLOB …)` with explicit source lists in the client
      CMake — done 2026-08-07. Lifted the parked line's Phase 14 approach
      (`4670b06`/`ed23fea`) against `main`'s current tree, not copied
      literally (Phase 3 item 3 moved `DXLib/` to `Platform/` since the
      tag diverged). `VS_UI_SRC_SOURCES`: explicit `set()`, regenerated
      from `find VS_UI -iname "*.cpp" | sort`. `CLIENT_MAIN_SOURCES`:
      kept as `file(GLOB … CONFIGURE_DEPENDS)` (bumped
      `cmake_minimum_required` 3.10 → 3.12), matching the parked line's
      reasoning — the glob still covers ~1,045 files (242 `Client/` +
      66 `Client/Packet/` root + 735 one level under `Packet/` + 1
      `SXml/` + 1 `WinLib/`), confirmed no third-level `Packet/`
      nesting the non-recursive `**` pattern would miss.
      **CI red 2026-08-07 (run #11, `eb9fd2d`), fixed same day
      (`19c99c0`):** the first pass's verification used a plain
      `-name "*.cpp"`, case-sensitive, which silently dropped
      `VS_UI_TITLE_SHOWCHAR.CPP` — a real, live, uppercase-extension
      file defining six `C_VS_UI_NEWCHAR::_ShowCharacter*` methods.
      The file(GLOB) it replaced had matched it fine only because
      CMake's glob ran on case-insensitive Windows/NTFS in CI, not the
      case-sensitive sandbox that generated the replacement list —
      `LNK2019`/`LNK1120` on MSVC once that safety net was gone. One
      file added; the list is now [measured] byte-identical to
      `find -iname` (57/57, diffed directly, not just counted).
      **Compile-verified 2026-08-07**, run pending on `19c99c0`.
- [x] CI build matrix (`make debug-asan` on Linux) — done 2026-08-07.
      `USE_ASAN`/`USE_TSAN`/`USE_UBSAN` CMake options + `debug-asan`/
      `debug-tsan`/`debug-ubsan` Makefile targets in both trees
      (server: `dkrixserver/CMakeLists.txt` + `Makefile`; client:
      `dkrix/CMakeLists.txt` + `Makefile`, Linux/macOS-only — Windows/
      MSVC stays the authoritative client build and the options are a
      no-op there). Confirmed **no Boost dep**, per parked Phase 15.
      TSan deliberately left out of both CI matrices (unaudited thread
      models produce noise, not signal); still available locally via
      `make debug-tsan`. New CI legs are additive — `sanitizers` in
      `server.yml`, `sanitizers-linux` in `client.yml` — neither
      touches the existing green jobs. Both marked
      `continue-on-error: true` (server leg added in a follow-up
      commit): this matrix has never run against the tree before, and
      the plain build job already went red twice today (`ff96e46`,
      `421088e`) before landing green — an unproven leg shouldn't be
      able to flip the whole workflow red while it finds its feet.
      **Compile gate — re-measured 2026-08-09 [measured, job-level,
      off the GitHub Actions REST API, full history rather than the
      #22–#28 window the earlier revision of this bullet sampled].**
      The four legs have real, divergent histories:

      | Leg | First run | Record | Consecutive greens |
      | --- | --- | --- | --- |
      | server `build (asan)` | #15 `eb9fd2d1` | 13 green / 4 red | **5** (#27–#31) |
      | server `build (ubsan)` | #15 `eb9fd2d1` | 0 green / 17 red | 0 |
      | client `Linux sanitizer (asan)` | ≤#20 `11e441d6` | 0 green / 19 red | 0 |
      | client `Linux sanitizer (ubsan)` | ≤#20 `11e441d6` | 0 green / 19 red | 0 |

      Server asan's four reds are #17, #19, #25 and #26 — every one a
      run where the plain `make debug (ubuntu)` job was red too, i.e.
      the tree was compile-broken for everyone. `-fsanitize=address`
      has never failed on its own here. The earlier "asan is at 2" was
      measured before #29–#31 landed.

      **Decision 2026-08-09 (revised): server asan flips to blocking,
      the other three stay non-blocking.** Flip rule unchanged — a leg
      becomes blocking after ≥5 consecutive green runs of that leg.
      Server asan reached 5 and was flipped (`71e2381`); because both
      server legs share one job, `continue-on-error` is now
      `${{ matrix.non_blocking }}` set per matrix leg rather than a
      job-level literal. The `.patch.HOLD` diffs in the D3/D5 stream
      output are superseded by that commit. TSan remains excluded
      (unaudited thread model).

      **Server ubsan is diagnosed, not fixed —
      `docs/ci-server-ubsan.md`.** It is one narrow break, not a
      general UBSan unfitness: `-fsanitize=undefined` implies the
      `vptr` check, which makes GCC emit a `typeinfo for T` reference
      at each member access through a polymorphic reference. Three
      sharedserver sites take a `GC*` packet by reference
      (`Guild.cpp:893`, `GuildManager.cpp:439`/`:460`); those classes'
      vtables and typeinfos live in `GameServerPackets`, which
      `sharedserver` does not link. Three undefined references, one
      `collect2: error`, nothing fails to *compile*. [measured
      2026-08-09 — reproduced in WSL (Ubuntu 24.04, g++ 13.3.0) at
      `cd8c3c6`; `nm -u` on the two objects shows exactly those three
      symbols, and they disappear when `-fno-sanitize=vptr` is
      appended.] **The fix is verified but not applied**: a from-
      scratch build with `-fsanitize=undefined -fno-sanitize=vptr`
      compiles and links the whole tree and produces all three
      binaries, with `sharedserver` linking at the same 26% mark where
      the unfixed build dies [measured 2026-08-09]. It is one line in
      `dkrixserver/CMakeLists.txt`, the flag order is load-bearing, and
      that file was outside the flipping session's scope — so only the
      write-up landed.

      **Applied 2026-08-10.** `dkrixserver/CMakeLists.txt` now appends
      `-fno-sanitize=vptr` after `-fsanitize=undefined` on both the
      compile flags and `CMAKE_EXE_LINKER_FLAGS`, with the rationale and
      the flag-order trap recorded inline. The leg stays
      `non_blocking: true`: the flip rule is ≥5 consecutive greens *of
      that leg*, its counter is still 0, and the fix has been verified in
      one local WSL build but never on a runner. Note also
      that the leg's ~12m runtime is misleading — it fails at ~26% and
      `make` keeps compiling `gameserver` to the end — and that
      Actions logs return 403 unauthenticated even on this public
      repo, so a local repro is the only outside route to them.

      **Client legs are blocked on a live workstream, not on
      neglect.** Both die in the `Build` step, so `Verify binary` and
      the ctest sprite tests have still never executed. The Linux
      client port was handed over at 17% complete with link-only
      failures remaining (`cd8c3c6`); these legs become flippable when
      it lands. The older "`min`/`max` macro bug" attribution is
      stale — that specific blocker has been passed.
- [x] Both trees: `.gitignore` for `build/`, `compile_commands.json`,
      editor detritus — **already done on this line** (Phase -1/Phase 1
      passes, verified with `git check-ignore -v`).

### Phases 11–18 — booked 2026-08-07 from the parked line (none started here)

The parked line continued past this plan's original horizon. Its phase
numbers are kept so `git log archive/modernization-phases-1-17 --oneline
--grep "Phase N"` keeps working. For each: read what the tag did first,
lift the approach one phase at a time against a build — never as a merge.

### Phase 11 — SQL injection remediation (11.1 lifted onto main 2026-08-07; 11.2 complete 2026-08-09 — ratchet 0)
**11.1 — done.** `PreparedStatement.{h,cpp}` lifted at tag-tip content
(which includes the parked line's later SELECT-materialization work)
into `src/server/database/`, added to `DATABASE_SOURCES` so all five
library flavors build it. Required companion, applied by hand rather
than file-copy (the tag's files carry fmt/encoding churn): an additive
materialized backend on `Result` — `FieldValue`, a rows-vector
constructor, and a `BackendType` branch in `next()`/`getField()`/dtor.
The existing `MYSQL_RES` path is preserved line-for-line. **Zero call
sites migrated** — the API lands alone, exactly like the parked 11B;
the SQL ratchet stays at 542. Verified in-session by API cross-check
against main's database layer (`Connection::getMYSQL()` present;
`SQLException`/`SQLQueryException`/`OutOfBoundException` present in
`Core/Exception.h`) and the pinned clang-format gate. **Compile-verified
2026-08-07** on CI run #14 (`421088e`) — GREEN, after two link-order
regressions (`ff96e46`, `421088e`; see the verification-status box
above) had to be fixed first. This sandbox has no server toolchain (apt
is sealed), so in-session work still can't self-verify; CI remains the
gate for the next change here too. PORTING-NOTE's
`server_build_fix.sh` warning is now half-obsolete: the files exist,
the script still isn't runnable here.
**11.2** — migrate the 542 ratcheted sites: ongoing, ratchet-driven
(Phase 16), never a one-phase close-out. Parked priority order stands:
user-string interpolation first (chat, say, whisper, pet names, custom
options), operator-trusted second, constant/numeric hygiene last.
**11.2 batch 1 (2026-08-07, agent stream):** 21 user-string sites migrated
to `PreparedStatement` across the chat/name subsystems —
`GCFriendChattingHandler` (11: friend list/history, incl. the raw
chat-message INSERT), `CGWhisperHandler` (2: offline-target cross-server
lookup), `CGSayHandler` (7: changeGuildMaster's three race branches,
opdeny, opfind, RemoveNick), loginserver's `CLQueryCharacterNameHandler`
(1: the creation-time name check, binding the not-yet-validated candidate
name). SQL text, literals, and connection choices preserved; stack
statements also close several early-return Statement leaks. **Ratchet
542 → 529** [measured], re-baselined via `--update`. Two caveats: only 13
of the 21 sites were counted — multi-line format strings are invisible to
the script's single-line grep, so the baseline understates real sites; and
the plan's "pet names" priority item is empty on `main` (no pet-name SQL
exists; Pet* files carry only tier-2 OwnerID strings plus a non-bindable
`SET %s` fragment). PreparedStatement's ctor throws base `SQLException`,
which `END_DB` does not catch — prepare-time failures skip DBError.log
(execute-time errors unchanged; closed 2026-08-09 by the 11.2 close-out:
the ctor now logs to DBError.log before throwing). **Compile-verified 2026-08-07**: CI run
#14 (`421088e`) went GREEN after two link-order bugs were fixed
(`ff96e46`, `421088e` — see the verification-status box above); both
failures were link-order problems in the surrounding `CMakeLists.txt`,
not in these 21 call sites, which compiled clean throughout.

**11.2 batch 2 (2026-08-07, agent stream):** 19 counted sites migrated
across the guild-membership subsystem — `GSAddGuildMemberHandler` (3:
per-race `GuildID` UPDATE on activation), `GSExpelGuildMemberHandler`
(6: single-member expel + guild-broken mass-update), `GSModifyGuildMemberHandler`
(3: per-race `GuildID` UPDATE on rank promotion), `GSQuitGuildHandler`
(7: single-member quit, guild-broken mass loop, guild-cancel mass loop
with Gold refund). **Ratchet 529 → 510** [measured]. A third
invisibility mode found alongside batch 1's multi-line-format-string
case: 14 further sites in these same files use `'%s'` but were
invisible to the ratchet's single-line grep because the SQL text
itself contains a `)` — from `now()` or `( Receiver, Message )` —
before the first `%s`, tripping the script's `[^)]*` pattern. Migrated
alongside the counted sites rather than left inconsistent within the
same `BEGIN_DB` block. One non-mechanical exception: the guild-cancel
path builds its target table name (`Slayer`/`Vampire`/`Ousters`) from
a fixed literal assigned by `pGuild->getRace()` three lines earlier,
never packet input — `PreparedStatement` can't bind an identifier, so
the table name stays spliced into the SQL text (commented inline);
`Gold` and `Name` are bound. Verified [measured]: the three-literal
assignment re-checked directly against `GSQuitGuildHandler.cpp`, no
other value ever reaches `Table`. **Compile gate: next server CI run
[unverified]** — landed after the last green run (#14, `421088e`).

**11.2 batch 6 (2026-08-08, agent stream): `dkrixserver/src/server/gameserver/CreatureUtil.cpp`.**
33 ratchet-counted sites migrated across seven functions —
`getRaceFromDB` (1: race lookup by Slayer name), `getGuildIDFromDB`
(1: guild-ID lookup, table name spliced — see below), `changeSexEx`
(2: Slayer/Vampire SEX UPDATE), `giveUnderworldGift` (1, guarded by
`#ifdef __UNDERWORLD__`: UnderworldEvent INSERT), `giveGoldMedal`
(1: GoldMedalCount INSERT), `giveLotto` (3: EventLotto
UPDATE/REPLACE/SELECT), and `deletePC` (24 ratchet-visible sites: the
Slayer/Vampire/Ousters deactivation UPDATEs, the SkillSave family,
CoupleInfo, 15 EffectX/EnemyErase tables, FlagSet, TimeLimitItems,
EventQuestAdvance). **Ratchet 437 → 404** [measured], re-baselined via
`--update`.

A fourth invisibility mode, distinct from batch 1's multi-line-format-string
case and batch 2's embedded-`)` case: `deletePC`'s 82-table
object-deletion sweep used
`executeQueryString("DELETE FROM X WHERE OwnerID = '" + ownerID + "'")`
— raw string concatenation with no `%[sdluxc]` placeholder anywhere in
the call, so none of the 82 sites were ever counted by the ratchet's
grep in the first place. All 82 were migrated alongside the 33 counted
sites — same rationale as batch 2: leaving them inconsistent within the
same `BEGIN_DB` block would be worse than migrating them together —
which brings the file's true live-call-site count from roughly 115 to
zero even though the ratchet delta only reflects the 33 sites it could
ever see. Six further textual hits remain in `--list` for this file:
`giveGoldMedal`'s and the fully-commented-out `addOlympicStat`'s dead
`/* */` blocks, plus three `//`-commented lines inside `deletePC`
preserving the historical DELETE-vs-UPDATE-Active choice. All six are
inside comments, never compiled, and were left untouched exactly as
`CLDeletePCHandler.cpp`'s equivalent dead comments were preserved in an
earlier batch — they inflate the ratchet's grep count but are not live
SQL and not call sites.

One non-mechanical judgment call: `getGuildIDFromDB` selects its
target table (`Slayer`/`Vampire`/`Ousters`) via a three-way `if/else`
on the `Race_t` parameter, never packet/user input —
`PreparedStatement` can't bind an identifier, so the table name stays
spliced into the SQL text
(`"SELECT GuildID FROM " + table + " where Name=?"`, inline comment),
matching batch 2's guild-cancel precedent exactly; the `Name`
parameter itself is bound. Every other value in this file — every
`pPC->getName()`/`ownerID`, `Sex2String[...]`, `PlayerID`/`PlayerName`,
and the `type`/`num` counters in `giveLotto` — is bound as a parameter
(`bindString`/`bindUInt`); no other identifier splices were needed.

Stack-allocated `PreparedStatement` locals throughout, per the
established pattern, which also closes several early-return `Statement`
leaks in `getRaceFromDB`/`getGuildIDFromDB`: the old code called
`SAFE_DELETE(pStmt)` before each early `return false`, which the
`PreparedStatement` destructor now makes unnecessary, so those manual
deletes were removed. **Not compile-verified** — no server toolchain in
this sandbox; verified by reading only (bind-index-to-`?`-order
correspondence checked for every statement, `?` count matches bind
count matches original printf-arg count for every site, file-wide
brace/paren balance checked programmatically, `.clang-format`'s
120-column limit checked and the 8 constructor calls that exceeded it
re-wrapped to match the established
`PreparedStatement name(\n    pConn, "...")` line-break style already
used in `CLDeletePCHandler.cpp`). CI is the real gate for this batch,
same caveat as every prior one.

**11.2 batches 3–5** landed across other parallel agent streams earlier
(not written up individually in this section — see the
verification-status boxes above and `git log` for the touched files,
including batch 4's guild-formation/union packet-handler migration
referenced from Phase 12's Wave 1 batch 2 write-up). Ratchet stood at
**437** going into batch 6 above and batch 7 below (the two ran in
parallel against the same 437 baseline, in disjoint files — batch 6's
`CreatureUtil.cpp` vs. batch 7's five guild domain-class files — so
their ratchet deltas stack: 437 → 404 (batch 6) → 352 (both, after
merge; batch 7 alone would have read 437 → 385 had it landed first).

**11.2 batch 7 (2026-08-08, agent stream):** the guild *domain-class*
cluster — a different layer from batch 2's guild-membership packet
handlers. All raw `Statement`/`executeQuery` call sites migrated to
`PreparedStatement` in five files: `server/sharedserver/Guild.cpp`,
`server/gameserver/Guild.cpp` (near-duplicate trees — `GuildMember`
and `Guild` CRUD, both migrated identically), `server/gameserver/
GuildUnion.cpp` (`GuildUnion`, `GuildUnionManager`,
`GuildUnionOfferManager` — union create/destroy/offer/accept/deny),
`server/sharedserver/GuildManager.cpp` and `server/gameserver/
GuildManager.cpp` (`init`/`load`/`deleteGuild`/`hasCastle`/
`hasWarSchedule`/`hasActiveWar`). **Ratchet 437 → 385** [measured],
re-baselined via `--update` — exactly the 52 sites the ratchet's
`--list` counted going in (14/14/14/5/5 per file). Went beyond the
ratchet-visible count: every remaining `executeQuery` call in these
five files was migrated too, including zero-placeholder queries
(`SELECT COUNT(*) FROM GuildInfo`) and ones the ratchet's single-line
grep missed because the SQL text has a `)` — from `COUNT(*)` or a
column list — before the first placeholder (e.g. `GuildManager::init()`'s
five `GuildRace = %d` counts). These files are now off the
`Statement`/vsprintf path entirely rather than left with a mixed old/
new API within the same `BEGIN_DB` block, matching batch 2's
established precedent of migrating whole blocks together.

Three non-mechanical judgment calls:
- `Guild::tinysave(const char* field)` builds a full `"Column = value"`
  SQL fragment in the caller (`GSModifyGuildMemberHandler.cpp`, e.g.
  `Master='Name'`, out of this batch's file scope) — not a single
  bindable value, and `PreparedStatement` cannot parameterise an
  entire dynamic assignment list without changing the function's
  signature and every caller. Left spliced into the SQL text with an
  inline comment; only the `GuildID` condition is bound. The parked
  line (`archive/modernization-phases-1-17`) never migrated this
  call site either — checked directly, same raw form there.
- `GuildUnionManager::removeMasterGuild()`'s `else` branch had a
  pre-existing bug: it allocated a second `Statement` (`pStmt2`) but
  never executed a query on it — the second `SELECT` actually ran
  through the first statement's connection. Preserved as-is: the
  migrated code drops the always-inert second `Statement` object (no
  behavior it produced) but keeps the second query running on the
  same `Connection*` as the first, exactly matching what the old code
  actually executed.
- `GuildUnionOfferManager::makeOfferList()`'s per-row `GuildInfo`
  lookup previously reused one `Statement*` (`pStmt2`) across loop
  iterations without deleting the prior one between iterations (a
  leak). The migrated version declares the `PreparedStatement` inside
  the loop body, so each iteration's object is destroyed via RAII at
  scope exit — this incidentally closes the leak without changing any
  query's SQL or result semantics.

`DATE_FORMAT(Offertime,'%%y%%m%%d')` in the same function became
`DATE_FORMAT(Offertime,'%y%m%d')` — the old code's `%%` was a
vsprintf escape for a literal `%`; `PreparedStatement` sends the SQL
text unmodified, so the single-`%` form is what MySQL now needs to see
the same date-format directive it always received. **Compile gate:
next server CI run [unverified]** — this sandbox has no server
toolchain; landed after the last known-green run referenced above.

**11.2 batch 9 (2026-08-09, agent stream):** the three playable-race
classes plus zone loading — five files: `server/gameserver/Slayer.cpp`,
`server/gameserver/Vampire.cpp`, `server/gameserver/Ousters.cpp`,
`server/gameserver/Zone.cpp`, `server/gameserver/ZoneGroupManager.cpp`.
Ran against the same 352 baseline batch 7 left behind. **Ratchet 352 →
326** [measured], re-baselined via `--update` — 26 of the 29 sites the
ratchet's `--list` counted going in (7/6/6/6/4 per file, matching all
five files' pre-migration `--list` output exactly); the other 3 are
dead code inside `/* */` or `//` comments, see below. Went well beyond
the ratchet-visible count, same as every prior batch: 39 live call
sites were migrated in total, of which the ratchet could only ever see
26 — 13 more were invisible to its single-line grep (3 each in
Slayer.cpp/Vampire.cpp/Ousters.cpp, 2 in Zone.cpp, 2 in
ZoneGroupManager.cpp), all migrated alongside the counted ones rather
than left inconsistent within the same function. Two invisibility
modes not seen in prior batches: `ZoneGroupManager::load()`'s and
`makeDefaultLoadInfo()`'s `SELECT ZoneGroupID FROM ZoneGroupInfo[
ORDER BY ZoneGroupID]` queries take no parameters at all, so they never
had a `%`-placeholder for the grep to match (same category as batch
7's zero-placeholder `COUNT(*)` queries). `Vampire::save()` and
`Ousters::save()` went further still: both built their `UPDATE` via
`StringStream` + `executeQueryString`, splicing `m_Name` straight into
the `WHERE` clause with no escaping and no `%`-format placeholder
anywhere in the call, so the ratchet's grep never had anything to
match. Both are now bound parameters over the same columns that were
ever live (`CurrentHP`/`HP`/`SilverDamage`/`ZoneID`/`XCoord`/`YCoord`/
`Name` for Vampire; Ousters additionally has `CurrentMP`/`MP`), with
the commented-out dead columns (`BatColor`, `Fame`, `Sight`, `F5`-
`F12`, `InMagics`, etc. — Vampire only) preserved as an explanatory
comment rather than as inert `<<` chains that no longer have a
`StringStream` to chain onto. Both functions' `Statement* pStmt;` was
also declared without `= NULL`, unlike every other function in these
files — a latent bug (`END_DB`'s `delete STMT` on an indeterminate
pointer if a `SQLQueryException` hit before the old
`pStmt = createStatement()` assignment ran) that removing that
assignment would have turned from theoretical into guaranteed. Fixed
by initializing both to `= NULL`, matching the pattern everywhere else
in these files; not a gameplay-behavior change.

Three non-mechanical judgment calls, all consistent with established
precedent:
- `Slayer::tinysave`/`Vampire::tinysave`/`Ousters::tinysave` each take
  a caller-built `"Column=value"` fragment (e.g. `saveSilverDamage`'s
  `sprintf(pField, "SilverDamage=%d", ...)`) — not a single bindable
  value, matching the `Guild::tinysave` precedent from batch 7
  exactly. Left spliced into the SQL text with an inline comment; only
  `Name` is bound.
- `Vampire::saveExps()` conditionally appended a `,SilverDamage = %d`
  fragment via a `sprintf`'d `char[40]` spliced through a `%s`
  placeholder — a fragment that changes the statement's shape, not a
  value. Hoisted to two static, fully-parameterised query texts
  (with/without the `SilverDamage` column) selected by the same
  `if (m_SilverDamage != 0)` branch the old code used, rather than
  splicing the fragment. `Ousters::saveExps()` has the identical
  pattern in a `/* */` comment but the live code always includes
  `SilverDamage` unconditionally, so no branch was needed there.
- `ZoneGroupManager::load()`/`makeDefaultLoadInfo()` each carry one
  dead `// Result* pResult = pStmt->executeQuery(...)` comment
  immediately above the live query it once was — the ratchet's grep
  still matches both (they contain the pre-migration `%d` form), so 2
  of the 4 counted sites in this file are inert. Left untouched,
  matching the `CreatureUtil.cpp`/`CLDeletePCHandler.cpp` precedent of
  preserving dead-comment SQL verbatim; `Slayer.cpp` had a fourth case
  of the same thing — a `/* if (reward != 0) { ... } */` block
  (`Reward = 0` UPDATE, `Slayer.cpp`) that has been fully commented out
  since before this batch, contributing 1 of its own 7 ratchet-counted
  sites without being live code. `Vampire.cpp` has the equivalent
  `/* if (reward != 0) { ... } */` block too, but its `executeQuery`
  call was never in `%`-placeholder form (`SQL.toString()` after a
  `StringStream`), so it was already invisible to the ratchet and
  isn't part of the 26-site delta.

Every other value in these five files — every `m_Name`/`field.c_str()`
target, `gold`, `zoneID`, `getRank()`/`getRankGoalExp()`, the
`Alignment`/`Fame`/`GoalExp`/`AdvancementClass`/`Advanced{STR,DEX,INT}`
stat family, `pCreature->getName()`, `pPC->getPlayer()->getID()`, and
`RACE_OUSTERS` — is bound as a parameter (`bindString`/`bindInt`/
`bindUInt`/`bindLong`/`bindULong`), with the bind type chosen to match
each site's original `printf` format specifier (`%d`→`bindInt`,
`%u`→`bindUInt`, `%ld`→`bindLong`, `%lu`→`bindULong`, `%s`→
`bindString`) rather than the C++ declared type of the value, since
several of these fields (e.g. `Fame_t`/`Exp_t` as `DWORD`) were already
being passed to a differently-signed format specifier before this
batch touched them — a pre-existing quirk of the vsprintf-based
`Statement::executeQuery`, preserved rather than fixed. No `Statement`/
`executeQuery`/`executeQueryString` call sites remain live in any of
the five files; `--list` still shows the 3 dead-comment lines above.
**Not compile-verified** — no server toolchain in this sandbox;
verified by reading only (bind-index-to-`?`-order correspondence
checked for every statement, `?` count matches bind count matches
original printf-arg count for every site, file-wide brace/paren
balance checked programmatically against each file's pre-edit count,
`.clang-format`'s 120-column limit checked). CI is the real gate for
this batch, same caveat as every prior one. Ran in parallel with
another Phase 11.2 stream against the same 352 baseline in disjoint
files (per this batch's task brief); the two deltas stack the same way
batches 6/7 did, and whoever merges both is responsible for
reconciling the final combined baseline number.

**11.2 batch 8 (2026-08-09, agent stream, worktree): three named files —
`server/PaySystem.cpp`, `server/gameserver/GamePlayer.cpp`,
`server/loginserver/LoginPlayer.cpp`.** Ratchet baseline going in: **352**
(post batch 6+7 merge, this worktree). Per-file ratchet-visible deltas:
PaySystem.cpp 7 (352→345), GamePlayer.cpp 9 (345→336), LoginPlayer.cpp 5
(336→331). **Combined 352 → 331** [measured], re-baselined via `--update`.

Went well beyond the ratchet-visible count in all three files — reading each
file whole (per this batch's brief) turned up 15 further live call sites the
grep never had a chance to see, all migrated alongside the counted ones:
- **9 more in PaySystem.cpp** (16 `PreparedStatement`s created vs. 7
  ratchet-visible sites): multi-line `executeQuery(...)` calls across
  `loginPayPlayPCRoom`, `loginPayPlay(ip, playerID)`,
  `increasePayPlayTimePCRoom`, and `isPlayInPayPCRoom` — the same
  invisibility mode as batch 1, just concentrated in this file's
  PC-room/billing code.
- **2 more in GamePlayer.cpp** (11 vs. 9): `disconnect()`'s
  `LastLogoutDate=now() WHERE PlayerID = '%s' AND LogOn='GAME'` (batch 2's
  embedded-`)`-before-first-`%s` mode), and `addLogoutPlayerData()`'s
  `executeQueryString(sql.toString())` built from a `StringStream` — batch
  6's fourth invisibility mode (raw concatenation, no `%[sdluxc]` anywhere in
  the call) — migrated to four bound `?` placeholders.
- **4 more in LoginPlayer.cpp** (9 vs. 5): `sendLGKickCharacter()`'s
  `CurrentWorldID` lookup and `makePCList()`'s three column-list SELECTs
  (Slayer/Vampire/Ousters) — all multi-line format strings.

Two non-mechanical judgment calls:
- `loginPayPlayPCRoom()` and `isPlayInPayPCRoom()` each reuse one `Result*`
  across a sequence of differently-shaped queries on the same connection —
  the old `Statement` API supported this because a single `Statement`
  object's owned `Result` is what the pointer aliased; `PreparedStatement`
  has no equivalent "reprepare with new SQL" call, so each of those queries
  became its own named `PreparedStatement` local. The risk this raises is
  real, not stylistic: `PreparedStatement::execute()`'s returned `Result*` is
  owned and freed by that specific statement object (`~PreparedStatement`
  deletes `m_pResult`), so a statement declared inside a narrow `try` block
  and read from outside it would dangle. Every statement in this batch is
  therefore declared at the same scope as its own `Result` reads (never
  nested one level deeper just to wrap the `.execute()` call in a `try`);
  where the original code had a narrow `try/catch(Throwable&)` around the
  `executeQuery(...)` call specifically, that narrow `try` now wraps only the
  `.execute()` call, with construction left outside it — construction-time
  failures (`PreparedStatement`'s ctor throws base `SQLException`) now
  surface at whatever handler is next up the stack rather than the file's own
  `filelog("paySystem.txt", ...)` logging, the same documented Phase 11.1
  caveat, not a new one.
- `sendLGKickCharacter()`'s `Slot='SLOT%d'` (formatted directly from the
  1-based `LastSlot` DB column) was **not** rewritten as
  `Slot2String[lastSlot]` despite that 0-indexed table being the established
  bind pattern elsewhere (`CLCreatePCHandler.cpp`, `CLDeletePCHandler.cpp`) —
  cross-checked `CLSelectPCHandler.cpp:212`
  (`slotStr.at(4) - '0'`) confirms `LastSlot`/`slot` are stored and read as
  the raw 1-based suffix (1/2/3), not the `Slot` enum's 0-based index
  (`SLOT1=0`); `Slot2String[lastSlot]` would have been off-by-one and
  eventually out-of-bounds at `lastSlot==3`. Bound
  `"SLOT" + std::to_string(lastSlot)` instead, preserving the exact original
  string verbatim.

Stack-allocated `PreparedStatement` locals throughout, per the established
pattern; every `SAFE_DELETE(pStmt)` on a migrated call site was dropped as
RAII-redundant, while every bare `Statement* pStmt[/1/2] = NULL;` that only
exists to satisfy an `END_DB(pStmt)` macro invocation elsewhere in the same
function was left in place, unused, exactly matching batch 6/7's precedent.
No gameplay-visible SQL or bind-order changes; `PaySystem.cpp` in particular
(real billing/currency data) was bound with the same int/uint distinctions
the original `%d`/`%u` format specifiers implied (`ObjectID_t`/`DWORD` →
`bindUInt`, `int` → `bindInt`). **Not compile-verified** — no server
toolchain in this sandbox; verified by reading only (every `?` count matched
against its bind-call count and index sequence 1..N, programmatically,
across all three files; brace/paren balance diffed against each file's
pre-edit `HEAD` copy; the two lines that grew past the 120-column
`.clang-format` limit were re-wrapped to the established
`PreparedStatement name(\n    pConn, "...")` style). CI is the real gate for
this batch, same caveat as every prior one. This batch's combined ratchet
delta (352 → 331) does not by itself reflect the tree-wide baseline — a
sibling agent migrated a disjoint file set in parallel in its own worktree;
whoever merges both batches reconciles the final combined number.

**11.2 batch 12 (2026-08-09, agent stream, worktree): everything remaining
outside `item/` and `CreatureUtil.cpp`.** Ran in this worktree against a
305-site baseline (`item/`'s 155 sites plus `CreatureUtil.cpp`'s 6
already-dead-comment sites plus this batch's 144 ratchet-visible sites,
across 85 files derived fresh from `--list`, not reused from an older
estimate). Split four ways across parallel sub-agents by directory cluster
— `src/Core/` packet handlers (17 files), `gameserver/` top-level A–L (21),
`gameserver/` top-level M–Z plus `billing/`/`couple/`/`ctf/` (20), and
`mission/`/`quest/`/`skill/`/`war/` plus a few `server/`-level and
`loginserver/` files (27) — each briefed with the same API reference,
established-pattern example, and lessons-learned list, then reconciled and
verified centrally. **Ratchet 305 → 170** in this worktree [measured],
re-baselined via `--update` — the 170 remaining are exactly `item/`'s 155
(untouched, out of scope) plus `CreatureUtil.cpp`'s 6 (untouched, already
dead comments from batch 6) plus 9 newly-confirmed dead-comment lines inside
this batch's own files (`CLLoginHandler.cpp`, `CLSelectWorldHandler.cpp`,
`ScriptManager.cpp`, `EffectGrayDarkness.cpp`, `Slayer.cpp`,
`ZoneGroupManager.cpp` x2, `LoginPlayer.cpp` x2) — every one individually
confirmed by reading the surrounding `/* */` or `//` context, not assumed.
Two files in scope (`EffectGrayDarkness.cpp`, `LoginPlayer.cpp`) needed zero
changes — their only SQL was already dead or already migrated in an earlier
batch — and were left untouched rather than modified for the sake of it.

Went far beyond the ratchet-visible count, consistent with every prior
batch: **254 live call sites migrated in total across 81 changed files**,
against 144 the ratchet's `--list` could see going in (135 of which were
actually live; the other 9 were the dead comments above). All four
previously-identified invisibility modes recurred here — multi-line format
strings, a SQL text with a `)` (`now()`, `COUNT(*)`) before the first
placeholder, raw `StringStream`+`executeQueryString` concatenation (an
entire file, `FlagSet.cpp`, was invisible this way — all 5 of its sites),
and zero-placeholder queries — plus no new ones; every file was read in
full, not just grepped, per the established practice.

Notable judgment calls, all consistent with prior-batch precedent:
- **Identifier/fragment splices** (value not bindable, left concatenated
  with an inline comment, only actual data values bound): `CoupleManager.cpp`
  (7 sites) splices a `getFieldName`/`getCounterFieldName` column name drawn
  from a fixed 2-entry Sex lookup, never packet input; `RaceWarLimiter.cpp`
  splices `getTableName()` (a per-subclass hardcoded literal); `Item.cpp` and
  `GlobalItemPositionLoader.cpp` splice a table name from
  `getObjectTableName()`/`ItemObjectTableName[itemClass]`; `CastleInfoManager
  ::tinysave` and `WarSchedule.cpp::tinysave` splice a caller-built
  `"Column=value"` fragment, the same `Guild::tinysave`/`Slayer::tinysave`
  shape from batches 7 and 9.
- **`CGModifyNicknameHandler.cpp`, `NicknameBook.cpp`, and both
  `ZoneUtil.cpp` files' `createBulletinBoard`** dropped a pre-bind
  `getDBString()`/`correctString()` manual backslash-escaping wrapper on
  values that are now bound as parameters — re-applying string-literal
  escaping to a value MySQL now receives out-of-band would have stored
  literal backslashes in nicknames/bulletin messages. Not a behavior
  regression: the escaping existed only to survive `vsprintf`-into-quoted-
  SQL-text, which no longer happens.
- **`WarScheduler::load()`** had a latent bug: its inner
  "ReinforceGuildID" query reused the same `Statement*` object as the outer
  per-row `SELECT`, and the old `Statement::executeQuery()` deletes its own
  previous `Result*` on every call — so the outer select's `Result` went
  dangling mid-iteration. `PreparedStatement` can't share one object across
  two SQL texts, so the inner query got its own statement/result variable,
  structurally removing the bug as a side effect of the migration rather
  than as a deliberate fix.
- Several `Statement* pStmt;` declarations without `= NULL`
  (`EffectAftermath.cpp`, `EffectKillAftermath.cpp`,
  `EffectCanEnterGDRLair.cpp`, `EffectEnemyErase.cpp`, `EffectMute.cpp`,
  `CLQueryPlayerIDHandler.cpp`, `CLRegisterPlayerHandler.cpp`,
  `ActionPrepareShop.cpp`, `ActionShowGuildDialog.cpp`,
  `Vampire_backup.cpp::save()`) had their only initializing
  `createStatement()` assignment removed by the migration — fixed to
  `= NULL` per batch 9's precedent (an indeterminate pointer reaching
  `END_DB`'s `delete STMT` on an exception path is a real bug the old code
  only avoided by luck of assignment ordering), not a gameplay change.
- Several per-iteration `Statement*` leaks (`IncomingPlayerManager.cpp`
  init loop, `ZoneUtil.cpp::loadBulletinBoard`, `RaceWarLimiter::
  clearPCList`, `FameLimitInfo::load`, `LevelWarManager`'s record
  functions) were closed by declaring the `PreparedStatement` inside the
  loop body (RAII-scoped per iteration) instead of reusing one object,
  matching batch 7's `GuildUnionOfferManager` precedent.
- `GQuestManager.cpp::eraseQuest` originally quoted a numeric column
  (`QuestID='%u'`); the quotes were dropped since a `?` placeholder can't
  sit inside a quoted literal against the real `MYSQL_STMT` backend — no
  behavior change, MySQL compares the column the same way either type is
  sent.
- `Vampire_backup.cpp` (8 sites, migrated per assignment) was confirmed via
  `grep` across every `CMakeLists.txt` to be referenced by no build target,
  and it `#include`s the current `Vampire.h`, which no longer declares the
  members (`m_Exp`/`getExp()`) this file's original code used — it cannot
  compile against current headers regardless of this batch's changes. It
  also contained a pre-existing invalid raw newline inside a string literal
  that had to be reconstructed as a valid concatenated multi-line literal
  purely to express the migrated call in syntactically valid C++. Left
  migrated rather than skipped (it was in scope and not `item/`/
  `CreatureUtil.cpp`), but its dead/orphaned status and inability to compile
  independent of this change are flagged for whoever next touches it —
  candidate for deletion in a future cleanup phase, out of scope here.
- **[measured 2026-08-09]** `Vampire_backup.cpp` deleted, independently
  re-verified from scratch rather than trusting the passing note above:
  (1) all 18 `CMakeLists.txt` under `dkrixserver/` grepped for
  `Vampire_backup` — zero references, including the `gameserver/`,
  `gameserver/item/`, `gameserver/skill/`, `loginserver/`, and
  `sharedserver/` ones; (2) every `CMakeLists.txt` in the tree grepped for
  `file(GLOB` — zero matches anywhere, so no implicit sweep is possible;
  (3) whole-tree grep for the string `Vampire_backup` — the only hit was
  the file itself (no `Vampire_backup.h` exists, no `#include` of one, no
  other consumer); (4) diffed against the live `Vampire.h`: the backup
  calls `pVampire->getExp()`/`->setExp()` (standalone `increaseVampExpEx`),
  and the class's own `load()`/`save()`/`toString()` directly reference
  `m_Exp`, `m_ExpOffset`, and `m_HotKey[8]` and call `setExpOffset()` /
  `setHotKey()` — every one of these is commented out in current
  `Vampire.h` (`//	Exp_t m_Exp;`, `//	Exp_t getExp() const ...`, etc.,
  mirroring the same commented-out shape in sibling `Slayer.h` and
  `Ousters.h`, evidence of a prior repo-wide refactor) and absent from
  `PlayerCreature.h`, so the class as declared today has no such members or
  methods to bind to — not just unreferenced, structurally incapable of
  compiling against current headers. All four checks independently
  confirmed the batch 12 note; none were ambiguous. Deleted with no
  replacement file (no `.h` counterpart existed).
- Pre-existing bugs found and preserved verbatim, not fixed:
  `CLGetServerListHandler.cpp::execute` writes both halves of a
  `CurrentWorldID, CurrentServerGroupID` SELECT result through
  `setCurrentServerGroupID(...)` twice (the first call, reading column 1,
  looks like it should be `setCurrentWorldID`); `SiegeWar.cpp` uses
  `getConnection("Darkeden")` (lowercase) in six functions against
  `"DARKEDEN"` everywhere else in the codebase.

Stack-allocated `PreparedStatement` locals throughout, `SAFE_DELETE(pStmt)`
dropped as RAII-redundant wherever a migrated site was the only user of the
old `Statement*`, per established pattern. **Not compile-verified** — no
server toolchain in this sandbox. Verified instead by: full-file reads (not
just `--list` greps) in every one of the 85 files; a scope reconciliation
confirming the 81 changed files are exactly the assigned 85 minus the 4 with
no live SQL and that no file outside the assignment, `item/`, or
`CreatureUtil.cpp` was touched; a file-wide open/close brace-and-paren-count
diff against each file's pre-edit `HEAD` copy (9 files carry a pre-existing
off-by-one from a brace/paren character inside a comment or string literal,
each individually confirmed unchanged by this batch's edit — same count
before and after); and `clang-format 18.1.8` (matching the CI-pinned
version) applied to exactly the 81 changed files and confirmed idempotent
(a second format pass is a no-op on all 81). CI is the real gate, same
caveat as every prior batch. This batch's ratchet delta (305 → 170) is
local to this worktree — two sibling agents migrated the disjoint `item/`
file set in parallel in their own worktrees; whoever merges all three
reconciles the final combined baseline number.

**11.2 batch 10 (2026-08-09, agent stream, worktree): `dkrixserver/src/server/
gameserver/item/`, the `A`–`Necklace` alphabetical half (a sibling agent took
`Necklace`-exclusive through the end in its own worktree; a third stream
covered the rest of the tree — all three ran against the same 305 baseline in
disjoint files).** 45 files in scope; two (`CoupleRingBase.cpp`, `Gun.cpp`)
turned out to carry no SQL at all (verified by reading — they're thin
base-class shims whose subclasses own the real `Statement`/`PreparedStatement`
calls). Of the remaining 43, every live call site was migrated: **460**
`PreparedStatement` sites in total, of which the ratchet's grep could only
ever see **80** — the other 380 were invisible to it, overwhelmingly
concentrated in one file. **Ratchet 305 → 225** [measured], re-baselined via
`--update`.

Per-item-class files (42 of the 43) follow a near-identical boilerplate
established across the whole `item/` subsystem — `create()`, `tinysave()`,
`save()`, occasionally `destroy()`, `XInfoManager::load()`, `XLoader::
load(Creature*)`, `XLoader::load(Zone*)`, and a no-op `XLoader::
load(StorageID_t, Inventory*)` — and were migrated by a purpose-built
script (not hand-edited file-by-file) that parses each `pStmt->executeQuery
(fmt, args...)` call's printf-style format string and argument list, and a
second pattern for the `StringStream sql; sql << ...; pStmt->
executeQueryString(sql.toString())` form used by roughly half these files'
`create()` and all of their `Zone*` loaders. Every transformed call was
verified programmatically per file — `?`-placeholder count against bind-call
count and index sequence 1..N, brace/paren balance against the pre-edit
`HEAD` copy, no raw `Statement`/`executeQuery`/`executeQueryString`/
`createStatement`/`SAFE_DELETE(pStmt)` left outside a comment, no line over
clang-format's 120-column limit. The script had two real bugs during
development that the first two verification passes did not catch (a missing
`.` before `bindX(...)` calls, and a regex whose `(?!literal)` lookahead
guard could still start-match one character into the literal it was meant to
exclude — reproducibly splitting `BEGIN_DB {` into `B` + injected code +
`EGIN_DB {` in the two files using the less-common "helpers-before-chain"
argument order, `CoupleRing.cpp` and `EventGiftBox.cpp`); both were caught
only by a third pass that diffed `__BEGIN_TRY`/`__END_CATCH`/`BEGIN_DB`/
`END_DB` token counts against each file's original and grepped for
`[A-Za-z]Connection\* pConn`, and fixed by hand. Called out here because it's
a real lesson for whoever runs a similar script next: token-count and
qmark-count invariants can hold by coincidence over corrupted output — this
batch's worst corruption (`Money.cpp`, below) had a bind-index sequence that
ran 1..20 with no gaps despite being nonsense. One further miss, caught only
by a final `git status` file-count sanity check against the derived scope
list rather than any content check: `Blade.cpp` was typed out of the script's
file list by hand (it sorts immediately after `Belt.cpp`, migrated separately
and first) and was silently skipped through two full verification passes
before anyone noticed the file was missing from `git status`, not that its
content was wrong. Migrated last, same script, same checks, clean.

The **`ItemIDRegistry.cpp` file alone accounts for 164 of the 460 sites** (43%
of the whole batch, all 164 invisible to the ratchet) — 81
`ClassName::initItemIDRegistry()` functions (six generated from one
`#define ITEMIDREGISTRY(CLASS, TABLE)` macro, the rest hand-duplicated
copies of it), each running an `SELECT COUNT(*) from X` followed
conditionally by `SELECT MAX(ItemID) FROM X`. None of these 162 sites ever
had a `%`-placeholder — most pass the table name as a bare literal
(`"SELECT COUNT(*) from MotorcycleObject"`), and the macro's two sites use
`%s` but are invisible to the ratchet for the batch 2 reason (`COUNT(*)`'s
`)` appears before the `%s`). This is the largest single-file invisible-site
count found in any Phase 11.2 batch so far. The macro itself was rewritten
once — `"SELECT COUNT(*) from " TABLE` using adjacent string-literal
concatenation (`TABLE` is always a `"...Object"` string literal at the call
site, e.g. `ITEMIDREGISTRY(CarryingReceiver, "CarryingReceiverObject")`) —
which propagates to its six invocations; the 81 hand-duplicated functions
were migrated by a dedicated regex pass (verified against the macro's
already-hand-checked output, then applied uniformly).

Four non-mechanical judgment calls, three of them variations on the
`tinysave`/table-name-splice precedents already established:
- `tinysave(const char* field)` is the same caller-built fragment as every
  prior batch's `tinysave` — spliced, only `ItemID` bound, inline comment —
  across all 42 item-class files that have it.
- `ComposMei::destroy()` and `Larva::destroy()` call `DELETE FROM %s WHERE
  ItemID = %ld` with `getObjectTableName().c_str()` as the `%s` argument —
  a virtual method returning a fixed per-class literal (e.g. `Item.h`'s pure
  virtual, overridden per subclass to return `"XObject"`), never packet/user
  input. `PreparedStatement` can't bind an identifier, so the table name
  stays spliced (`"DELETE FROM " + getObjectTableName() + " WHERE ItemID =
  ?"`), matching the batch 2/6/7 table-name precedent; only `ItemID` is
  bound. Migrating this also meant fixing `if (pStmt->getAffectedRowCount()
  == 0)` to read off the new `PreparedStatement` instead of the
  now-permanently-`NULL` `pStmt` (the same fix `Belt::destroy()` needed).
- `BloodBible::tinysave()` and `CastleSymbol::tinysave()` build the SQL into
  a `char query[255]` buffer via `sprintf()` before calling
  `pStmt->executeQuery(query)` — a fifth invisibility mode, distinct from
  the four found in batches 1/2/6/9 (the ratchet's grep only matches
  `executeQuery(`/`Statement(`/`setStatement(` calls whose *own* argument
  list contains a `%` specifier; here the specifier lives one line up, inside
  the `sprintf` call, invisible regardless of format). The `sprintf`/
  `filelog("WarLog.txt", ...)` pair is kept (preserves the existing SQL debug
  log), but the buffer is no longer executed — a `PreparedStatement` with the
  same spliced-fragment-plus-bound-`ItemID` shape runs instead.
- `Money::tinysave()` has a variant fragment shape,
  `"UPDATE MoneyObject SET %s, Amount=%ld WHERE ItemID=%ld"` — the caller
  fragment *and* a real bindable `Amount` column share one call. Migrated as
  `string("UPDATE MoneyObject SET ") + field + ", Amount=? WHERE ItemID=?"`
  with both `?`s bound (`Amount` as `bindLong`, matching this site's own
  `%ld`, even though `Money::create()`'s `%d` for the same field binds it
  `bindInt` elsewhere in the same file — preserved verbatim per the
  batch 9 precedent of matching each site's own specifier, not unifying
  across sites).

Every other value across the batch is bound by the same convention as prior
batches: bind method chosen from the original `%`-specifier where the site
had one (`%s`→`bindString` with `.c_str()` stripped, `%d`/`%c`→`bindInt`,
`%u`→`bindUInt`, `%ld`→`bindLong`, `%lu`→`bindULong`), and for the `<<`-chain
`create()` sites that never had a specifier, inferred from an explicit
`(int)`/`(uint)`/`(long)` cast where present or a small fixed lookup table
for the bare fields that recur across nearly every file (`m_ItemID`/
`m_ObjectID`/`storageID`→`bindLong`, `getItemType()`/`getGrade()`/
`m_CreateType`/`m_Num`/`m_ItemType`→`bindInt`) — cross-checked against each
file's own printf-style `save()`, which binds the identical column with the
identical type. `Statement* pStmt;` (no initialiser) was changed to
`= NULL` everywhere its only assignment was removed, matching the batch 9
precedent (the `END_DB` macro's `delete STMT` on catch would otherwise run
on an indeterminate pointer); every `SAFE_DELETE(pStmt)` — including the
`// by sigi` ones inside `Loader::load()`'s `switch` `default:` branches —
was dropped as RAII-redundant. **Not compile-verified** — no server
toolchain in this sandbox; verified by reading and by the programmatic
passes described above, re-run after the corruption fixes until clean. CI is
the real gate, same caveat as every prior batch. This batch's ratchet delta
(305 → 225) does not by itself reflect the tree-wide baseline — two sibling
agents migrated disjoint file sets in parallel in their own worktrees;
whoever merges all three batches reconciles the final combined number.

**11.2 batch 11 (2026-08-09, agent stream, worktree): 47 files in
`server/gameserver/item/`** — every `.cpp` in that directory sorting
alphabetically after `Necklace.cpp` (a sibling agent covered the rest of
the directory, `<= Necklace.cpp`, in a separate worktree against the same
starting baseline). Full list: OustersArmsband/Boots/Chakram/Circlet/Coat/
Pendent/Ring/Stone/SummonItem/Wristlet, Persona, PetEnchantItem/Food/Item,
Potion, Pupa, QuestItem, Relic, ResurrectItem, Ring, SG, SMG, SMSItem, SR,
Serum, Shield, Shoes, ShoulderArmor, Skull, SlayerPortalItem, SubInventory,
Sweeper, Sword, TrapItem, Trouser, VampireAmulet/Bracelet/Coat/CoupleRing/
ETC/Earring/Necklace/PortalItem/Ring/Weapon, WarItem, Water. **Ratchet 305 →
230** [measured], re-baselined via `--update` — a delta of 75 sites, this
worktree's contribution only (per this batch's brief, the sibling stream's
delta on the other half of the directory is reconciled separately at merge
time).

Nearly all 47 files are one of a handful of near-identical item-class
templates — `create()`/`tinysave()`/`save()`/`InfoManager::load()`/
`Loader::load(Creature*)`/`load(Zone*)`/`load(StorageID_t, Inventory*)` —
following the wearable/consumable shape established by the Ousters/Vampire
files in batches 7 and 9. Counting every `PreparedStatement` construction
(not just the ratchet-visible ones) puts the true migrated-site count at
**335** across the 47 files, well above the 75 the ratchet could see. The
260-site gap is concentrated in two invisibility categories already
documented in prior batches, but pervasive here rather than occasional:
every file's `InfoManager::load()` opens with two zero-placeholder queries
(`SELECT MAX(ItemType) FROM XxxInfo` and the full column-list `SELECT`)
that never had a `%`-format argument for the grep to match (batch 6/9's
"zero-placeholder" mode), and most files' `Loader::load(Zone*)` built its
query via `StringStream` + `executeQueryString(sql.toString())` with no
`%`-format anywhere in the call (batch 8's raw-concat mode). Both patterns
recur once or twice per file across the whole 47-file set, so the
aggregate invisible count is large even though no individual file is an
outlier.

Judgment calls, in addition to the SAFE_DELETE-removal/`getObjectTableName()`
splice/`tinysave(field)`-splice precedents already established (batches
2/6/7/9), each applied here exactly as before:
- **PetItem.cpp**: `getDBString(m_pPetInfo->getNickname())` — a
  hand-escaping shim for the old raw-format string embedding — was dropped
  in favor of binding the raw nickname directly; escaping it again before
  binding would double-encode it. `create()`/`save()` each keep their
  original two-branch shape (9-column vs. 21-column INSERT/UPDATE
  depending on `m_pPetInfo == NULL`), migrated branch-for-branch.
- **Potion.cpp, Pupa.cpp**: `destroy()` builds its `DELETE FROM <table>
  WHERE ItemID = ?` using the caller's `getObjectTableName()`, a
  macro-generated compile-time-fixed identifier, never user input — left
  spliced (identifiers can't be bound), matching the batch 2/6/7/9
  precedent.
- **SG.cpp, SMG.cpp, SR.cpp**: the gun-family classes append a
  `BulletCount=?` clause onto the caller-built `tinysave(field)` fragment
  (`UPDATE SGObject SET <field>, BulletCount=? WHERE ItemID=?`) rather than
  the plain single-column splice every other file's `tinysave` uses, and
  each has a separate `saveBullet()` method migrated the same way. `create()`/
  `save()` carry a 13-column list (adds `BulletCount`, `Silver` over the
  12-column wearable template).
- **VampireCoupleRing.cpp**: uses `Name`/`PartnerItemID` instead of
  `Durability`/`Grade`, and has an extra `hasPartnerItem()` method
  (`SELECT count(*) FROM VampireCoupleRingObject WHERE ItemID=? AND
  Storage IN(...)`) migrated alongside the standard methods. Its
  `Loader::load(Zone*)` opens with a pre-existing `Assert(false)` dead-path
  guard, left untouched.
- **VampirePortalItem.cpp**: `Loader::load(Zone*)`'s SELECT lists 8
  columns but the loop reads 11 fields (`TargetZID`/`TargetX`/`TargetY`
  are never selected) — a pre-existing bug, preserved verbatim rather than
  silently fixed; the migrated query text still selects only the original
  8 columns.
- **VampireAmulet.cpp**: the Object-table INSERT/UPDATE has no
  `Durability` column at all (11-column list), unlike every other wearable
  file's 12-column template; the separate Info-table SELECT does have
  `Durability`. Both migrated as read, not reconciled with each other.
- **Sword.cpp**: `save()` carries an extra `Silver` column that `create()`
  does not (13 vs. 12 columns), bound as `bindInt(getSilver())`.
- **Sweeper.cpp, WarItem.cpp**: `create()`/`tinysave()` log the query text
  to `WarLog.txt` for audit purposes. The original `StringStream sql` /
  `sprintf(query, ...)` construction is kept, unexecuted, purely to
  reproduce that audit line verbatim; the query itself now runs via the
  migrated `PreparedStatement`, with an inline comment explaining the
  duplication.

Stack-allocated `PreparedStatement` locals throughout; every
`SAFE_DELETE(pStmt)` on a migrated call site was dropped as RAII-redundant,
including the `default: SAFE_DELETE(pStmt); // by sigi` cases inside
`Loader::load(Creature*)`'s storage-type `switch`, closing the same latent
leak batch 6 documented. Dead `/* ... */`-commented SQL blocks (present in
most of these files, reproducing the pre-migration `StringStream`
construction for `save()`) were left completely untouched, matching every
prior batch's precedent. **Not compile-verified** — no server toolchain in
this sandbox; verified by reading only (every `?` count checked against its
bind-call count and index sequence across all 335 sites; each file's
brace/paren balance checked against its pre-edit copy; grep re-run
per-file after editing to confirm zero live `createStatement()`/
`pStmt->executeQuery(`/`SAFE_DELETE(pStmt)` occurrences remained outside
dead comments and the handful of genuinely empty, no-SQL stub loaders).
CI is the real gate for this batch, same caveat as every prior one.

**11.2 close-out (2026-08-09, agent stream, worktree): ratchet 15 → 0
[measured].** The 15 hits remaining after batches 10–12 merged were
audited one final time, each file read in full plus a string-literal-
aware comment strip: **all 15 were dead comments; zero live sites
remained**. This *confirms* batch 12's "every one individually confirmed
dead" claim and *retracts* the 2026-08-09 audit note (AGENT_WORK_GUIDE
§2 / this wave's brief) that reclassified three of them as live — that
reclassification was a misread caused by verification context windows
(`sed -n '815,830p'`, `'2175,2190p'`, `'88,100p'`) that each start a few
lines below the enclosing block's `/*` opener: `Slayer.cpp:823` sits in
the `/* if (reward != 0) */` block (785–888), `CreatureUtil.cpp:2182`
plus the REPLACE below it sit in `addOlympicStat`'s commented-out body
(2170–2197; the function itself is a live no-op with 18 callers), and
`EffectGrayDarkness.cpp:95` sits in the block (84–135) that comments out
the whole loader — whose class declaration is equally commented out in
its header. Lesson recorded: when auditing live-vs-dead, quote the
enclosing `/* */` bounds, not a fixed context window.

All 15 dead blocks were then **deleted** per decision D2 (approved by
Enrico 2026-08-09): prior batches preserved dead-comment SQL in passing,
but lines that hold a security ratchet above zero are noise the gate
carries forever — a different, defensible call. Whole blocks went, not
just matching lines (Slayer's reward block, EffectGrayDarkness's
commented-out loader, giveGoldMedal's and addOlympicStat's `/* */`
bodies, deletePC's three DELETE-vs-UPDATE history lines, both
LoginPlayer disconnect variants' LogOn-check fragments,
CLSelectWorldHandler's `/* BEGIN_DB */` block, CLLoginHandler's LoginIP
fallback, ZoneGroupManager's and ScriptManager's pre-migration query
comments). Deletion verified comment-only: comment-stripped,
whitespace-stripped token streams of all eight files byte-identical
before/after. A preceding fmt-only commit brought the four files with
pre-existing clang-format drift (Slayer, ZoneGroupManager, CreatureUtil,
EffectGrayDarkness) to 18.1.8-clean, string literals verified
byte-identical, so the format gate judges the deletions on their own
content.

Baseline re-baselined **15 → 0** via `--update` and the gate verified
both ways [measured]: exit 0 at baseline ("OK: 0 sites"), and — since
baseline−1 is impossible at 0 — exit 1 with a deliberately re-added
dummy `%s` site, which the gate printed and which was removed before
commit. Every future `%`-format `executeQuery`/`setStatement`/
`Statement` call in compiled server sources now fails CI outright.

The Phase 11.1 END_DB caveat is also closed: `PreparedStatement`'s ctor
now writes prepare-time failures (`mysql_stmt_init`/`mysql_stmt_prepare`)
to DBError.log before throwing, exception type and text unchanged.
Widening END_DB's catch was rejected — END_DB rethrows `const char*`,
invisible to upstream `catch (Throwable&)`, so widening would reroute
every previously-uncaught SQLException in the tree. `execute()`'s
unbound-parameter SQLException still bypasses END_DB's log by design
(programming-error guard). **Compile gate: server CI on the landing
push [unverified until green]** — this close-out was produced in a
sandbox with no server toolchain, same as every prior batch.

### Phase 12 — Packet schema unification (12.1 scaffolding + pilot landed 2026-08-08; Waves 1–3 landed 2026-08-08/09; Wave 4 batches 1–3 landed 2026-08-09, ratchet 326 → 114)
Booked by Phase 9's proposal above. Parked 12.0 measured the real scope:
**920** packet `.{h,cpp}` pairs in `dkrixserver/src/Core/` (300 CG,
516 GC, 34 CL, 34 LC, 16 GS, 20 SG — `GT`/`TG` turned out to be 0 files),
**326** pairs in `dkrix/Client/Packet/Cpackets/` (CG 294 + CL 32 only:
the client decodes receive-side packets inline, so only the send-side
families are duplicated). **Apply the Phase 17 caveat before sizing** —
the pairs are not whitespace-identical. **Sizing refined 2026-08-07** by
the Phase 17 re-run: one scripted style-normalization pass over the ~160
twin pairs plus targeted manual protocol review of the CL login/account
family — not 163 independent manual merges. Details:
`docs/packet-divergence-2026-08-07.md`.
**Normalizer built + validated 2026-08-07** (`agent/phase12-script`):
`dkrixserver/scripts/normalize-packet-style.py` normalizes both sides of a
pair to the canonical style (server clang-format layout, no throw-specs,
std::string, folded PCH/guard/dispatch lines) and issues a wire-signature
verdict per pair. Full run [measured, text-level]: **62 style-only / 44
residual ≤5 / 48 residual >5 / 9 real-divergence** of 163. The 9 (CLLogin,
CLCreatePC, CGMove, CGGuildChat, CGModifyNickname, CGSkillToInventory,
CGUseItemFromInventory, CGExchangeBuy, CGExchangeList) are the
protocol-review queue — two show the *server* header stale (size
formulas), so "pick server version" is not a safe default; the streams are
the authority. Batch plan and evidence:
`docs/packet-normalization-sample-2026-08-07.md`. The packet-duplicates
ratchet (326) is the scoreboard for the actual migration, stepping down as
pairs unify.

**Wave 1 attempt, 2026-08-07 — blocked, nothing landed.** The sample
doc's batch plan ("adopt the server file... update client build
lists") assumes the client can point its CMake source list straight at
the physical file under `dkrixserver/src/Core/`. It can't: `#include
"Packet.h"`/`"PacketFactory.h"` resolution always checks the directory
containing the including file first, ahead of any `-I` path — fixed by
each pair's `.cpp`'s physical location, not by which target compiles
it. `dkrixserver/src/Core/` and `dkrix/Client/Packet/Cpackets/` each
have their own `Packet.h`/`PacketFactory.h`/`SocketInputStream.h`, and
they are **not** reconciled (`PacketFactory.h` diffs 44 lines,
`SocketInputStream.h` diffs 303, between the trees) [measured]. A
client translation unit compiling the physical server-tree file would
silently pull in the server's incompatible base classes — a structural
break, not a style nit. The parked line's own `shared/Packets/README.md`
(on `archive/modernization-phases-1-17`) reached the same conclusion
independently: it built a genuinely neutral third top-level directory
plus a `shared_packets` CMake INTERFACE target, wired into three
server library targets and the client target with per-target defines.
Its first real migration (`CGStoreOpen`, commit `5805e37`) touched
four CMakeLists.txt across both trees plus 28 client
`#include "Packet/Cpackets/X.h"` sites across 11 files for that one
pair alone [measured, re-derived for the six Wave-1 candidates]. That
is the scope Wave 1 would actually require — the "bigger restructuring"
this section's batch plan explicitly reserved for a deliberate,
separately-recorded decision, not something a first small batch should
back into silently.

**Decided 2026-08-08: build `shared/Packets/`.** It is the only
approach that has ever actually worked here — proven by the parked
line's own end-to-end login→gameplay smoke test, not merely by
argument — and both toolchains are now CI-gated on every push, which
is the safety net the parked line didn't have when it built this the
first time. Rejected alternative: staying blocked indefinitely on
Phase 12 is not actually lower-risk — the packet-duplicates ratchet
(326) sits un-movable, and every subsequent `PreparedStatement`-style
change to a packet handler keeps widening the gap between the two
trees' copies. **Scope of the first change, deliberately small — not
all 62 pairs at once:** stand up the `shared/Packets/` directory and a
`shared_packets` CMake INTERFACE target (lift the mechanism from
`archive/modernization-phases-1-17`'s `shared/Packets/README.md`, sized
against `main`'s current tree, not copied file-for-file), then migrate
exactly **one pilot pair** through it end to end — the parked line's
own first migration (`CGStoreOpen`, `5805e37`) is not necessarily the
right pilot for `main`'s current tree; pick whichever of the confirmed
style-only 62 has the fewest client include sites, to keep the first
proof-of-mechanism blast radius minimal. Both-tree CI must go green on
the pilot before Wave 1 batches resume. This unblocks the batch plan
above; it does not pre-approve doing all 62 pairs in one pass — that
stays batched per the plan (~20 at a time, CI green per batch).

**12.1 landed 2026-08-08 (worktree, not yet on `main`): scaffolding +
one pilot pair.** `shared/Packets/README.md` and `CMakeLists.txt` (a
`shared_packets` CMake INTERFACE target, absolute-path `target_sources`
under a locally-forced `cmake_minimum_required(VERSION 3.13)` since
dkrix's own floor is 3.12) landed first, wired into both root
CMakeLists via `add_subdirectory`, with every packet family's source
list empty — no build-observable change. **Pilot: `CLGetWorldList`**,
chosen by client `#include` site count across the confirmed-style-only
62 (re-verified against the current tree, unchanged from the
2026-08-07 sample doc): 3 real include sites (`GameUI.cpp`,
`Packet/PacketFactoryManager.cpp`,
`Packet/Lpackets/LCLoginOKHandler.cpp`) vs. 4 for `CLLogout` /
`CLVersionCheck` and 6 for `CGReady`. Reconciliation: the server file
was already at canonical style (no `throw()` specs, unconditional
`getPacketName()`/`toString()`); the only real merge was keeping the
`#ifndef __GAME_CLIENT__` guard around `CLGetWorldListHandler` (present
in the client's Cpackets copy, absent from the server's) so the client
build doesn't need a stub definition, and adding `throw()` back to the
four `CLGetWorldListFactory` overrides (`createPacket`,
`getPacketName`, `getPacketID`, `getPacketMaxSize`) to satisfy the
client tree's `PacketFactory` base, which declares them `throw()` —
narrowing an override's exception spec is always legal, so this is
also fine against the server's unconstrained base. Per-target wiring:
`LoginServerPackets` links `shared_packets` (owns the family, absorbs
`CLGetWorldList.cpp` with `__LOGIN_SERVER__`); `GameServerPackets` and
`SharedServerPackets` get `shared/Packets` on their include path only,
no link — `PacketFactoryManager.cpp` (built into all three) `#include`s
every packet header unconditionally regardless of which family's
`addFactory()` calls are guarded to which server type, so linking
`shared_packets` there too would try to absorb `CLGetWorldList.cpp` as
a source in a binary with no `CLGetWorldListHandler::execute` symbol —
a link error. `DarkEden` links `shared_packets` directly. Ratchet:
`check-packet-duplicates.sh --count` 326 → 324 (one class pair, `.h` +
`.cpp`), baseline file updated in the same change.

**Not build-verified — no compiler in the environment that did this
work** (neither MSVC/vcpkg for the client nor the Linux server
toolchain). Verified by reading: every `#include` site naming the old
`Cpackets/CLGetWorldList.h` path is gone (`git grep`, excluding the
orphaned, non-authoritative `Client.vcxproj.filters` — there is no
matching `.vcxproj`, so it isn't part of any real build); the moved
files exist only at `shared/Packets/`, not in either old location; the
normalizer's wire-signature check (`normalize-packet-style.py --pair
CLGetWorldList`) showed `residual: 0` / verdict `style-only` before the
move. **Both-tree CI is the actual gate** — per `docs/CLAUDE.md`, this
client+server change lands as one unit because it jointly defines the
wire format, and this exact family of bug (case-sensitive `find` vs.
case-insensitive Windows glob, a file silently dropped from a CMake
source list) has caused three CI-red incidents in this tree in one day.
Wave 1 proper (batches of ~20 of the remaining 61 style-only pairs)
stays gated on this pilot going green on both trees' CI, per the plan
above — nothing here pre-approves it.

**CI verdict, 2026-08-08: three client-side red runs before green —
all in the include-resolution mechanics, none in the migrated packet
itself.** Server CI was green on the pilot from the first push (run
#16, `b0df2a8`) and never regressed; every failure below was
client-only.

- **Run #13 (`b0df2a8`) — red.** `error C1083: Cannot open include
  file: 'Packet.h'` in `shared/Packets/CLGetWorldList.h`. Root cause:
  `Packet.h`/`PacketFactory.h` live in `Client/Packet/`, not
  `shared/Packets/`; the old `Cpackets/CLGetWorldList.h` resolved them
  only because it was `#include`d from
  `Client/Packet/PacketFactoryManager.cpp`, and MSVC's quoted-include
  search walks the `#include` stack up to each includer's directory.
  `shared/Packets/CLGetWorldList.cpp` is now a standalone translation
  unit (`target_sources` on the `shared_packets` INTERFACE target, not
  `#include`d from anywhere) — no stack to walk.
- **Fix attempt 1 (`0a46249`) — red, worse.** Added `Client/Packet` to
  `DarkEden`'s target-wide include dirs. Got past the original error
  into a much bigger one: run #14 cascaded hundreds of errors in
  `winbase.h` and ATL headers (`STREAM_INFO_LEVELS` syntax error;
  `CreateFile`/`DeleteFile`/`FindFirstFile`/`ReadFile`/`GetFileSize`
  "not a member of global namespace"). `Client/Packet/FileAPI.h` is
  unrelated game code (a `FileAPI::` namespace of exception-throwing
  POSIX-style wrappers) whose name case-insensitively matches Windows'
  real `<fileapi.h>` — once `Client/Packet` was on every one of
  `DarkEden`'s ~1000 translation units' `/I` path, ours shadowed the
  SDK's ahead of the toolchain's own Windows Kits path, and every
  `<windows.h>`-pulling file (ATL, `VS_UI_WebBrowser.cpp`'s COM code)
  lost the real Win32 file API.
- **Fix attempt 2 (`38052e8`) — red, same original error.** Tried
  scoping the include path to only the `shared_packets` source files
  via the per-source-file `INCLUDE_DIRECTORIES` property (additive,
  not a target-wide replacement), set from `dkrix/CMakeLists.txt` —
  the same directory scope as the `DarkEden` target, which should have
  been enough regardless of `CMP0118`. It silently did nothing: run
  #15 hit the identical `Packet.h` C1083 from run #13. Most likely a
  Visual Studio generator quirk for sources absorbed into a target via
  a *linked INTERFACE library's* `target_sources`, rather than added
  to the target directly — untested territory, not worth chasing
  further given a simpler fix existed.
- **Fix attempt 3 (`46746eb`) — green.** A two-file forwarding-header
  shim, `Client/Packet/SharedPacketsShim/{Packet,PacketFactory}.h`,
  each a one-line `#include "../Packet.h"` /
  `#include "../PacketFactory.h"`. Safe to add target-wide because the
  shim directory holds nothing else — no shadow risk, and any other
  accidental quoted-include of `"Packet.h"` would just get correctly
  forwarded rather than broken. **Run #16 (`46746eb`) — SUCCESS**,
  `MSVC x64 (Debug)` and `Viewers and validators` both green, 27m37s.
  The two `sanitizers-linux` legs are still red on this run, but for
  unrelated, pre-existing reasons (`Timer2.h` / `types/SystemTypes.h`
  not found on that path) and are `continue-on-error` by design — they
  did not block the merge and are not part of this pilot's story.

Net effect on the plan: Wave 1's batch template should budget for this
class of failure specifically. The lesson isn't "don't touch include
paths" — it's that a shared/interface-library source, once it stops
being reached via `#include` from an existing file, loses whatever
include-stack resolution that file's location was quietly providing,
and the fix must be scoped as narrowly as the problem (a two-header
shim, not a directory-wide `/I` addition) or it trades one failure
mode for a worse one.

**Wave 1 batch 1 landed 2026-08-08 (worktree, not yet on `main`): 10
CG-family pairs.** `python3 normalize-packet-style.py --all --tsv`
re-run against the current tree confirmed 61 confirmed style-only pairs
(unchanged from the pilot's count, since `CLGetWorldList` — the only
migrated pair — is CL, not CG). Excluded per instruction: `CGSay`,
`CGWhisper`, `GCFriendChatting`, `CLQueryCharacterName`, `CGConnect`,
`CLCreatePC`, `CLLogin`, `CLSelectPC` (a parallel SQL-migration
workstream may touch their `*Handler.cpp` files). Candidate ranking
used client `#include`-site count exactly like the pilot, but the count
itself needed a fix first: a same-directory `Cpackets/<Name>.h`
case-sensitive `git grep` undercounted every pair by roughly half,
because `dkrix/Client/` mixes `Cpackets/`, `CPackets/`, and `cpackets/`
across different files — the exact case-sensitive-`find`-vs-Windows-glob
class of bug this doc's Phase 16 and Phase 12 pilot sections already
flagged, now confirmed to bite the *counting* step too, not just the
build. Re-run case-insensitively (`git grep -io`), the ranking changed
substantially (e.g. what looked like a 1-include pair was actually 3).
`CGSelectBloodBible` initially looked cheapest (1 case-sensitive hit) but
the case-insensitive re-count found its real site was `UIDialog.cpp`'s
`#include "Packet/CPackets/CGSelectBloodBible.h"` (capital `P`) — once
counted correctly it was no cheaper than the pairs below, so it was left
for a future batch rather than re-optimizing the ranking after the fact.
Final 10, all case-insensitive count 2–3, all confirmed `residual: 0` /
`style-only` individually via `--pair`: **CGAddItemToItem, CGAuthKey,
CGUseItemFromGQuestInventory, CGRequestInfo, CGAddItemToCodeSheet,
CGBuyStoreItem, CGWithdrawPet, CGGlobalChat, CGLogout, CGResurrect**.
`CGPortCheck` (case-insensitive count 2) and `CGNPCAskAnswer` (count 4)
were considered and dropped mid-selection as messier than the rest of
the batch: `CGPortCheck` derives from `DatagramPacket` (a second quoted
include, `DatagramPacket.h`, living in `Client/Packet/` like
`Packet.h`/`PacketFactory.h` but not covered by the pilot's
`SharedPacketsShim` forwarders — extending the shim was avoidable by
picking a different pair); `CGNPCAskAnswer` pulls in
`SocketEncryptInputStream.h`/`SocketEncryptOutputStream.h` at different
relative paths between the trees plus a `Assert1.h` (server) vs.
`Assert.h` (client) filename mismatch — a second class of divergence
the batch didn't need to take on yet.

Reconciliation followed the pilot's recipe for 8 of the 10 pairs
exactly: adopt the server's canonical style (no `throw()` specs,
unconditional `getPacketName()`/`toString()` instead of
`#ifdef __DEBUG_OUTPUT__`-gated), add `throw()` back to the four
`<Name>Factory` overrides, and keep `#ifndef __GAME_CLIENT__` around
the `<Name>Handler` class (`.h`) and its dispatch call (`.cpp`). Two —
**`CGBuyStoreItem` and `CGWithdrawPet`** — hit the *other* precedent the
pilot commit named but didn't need: `dkrix/Client/CGHandlersStub.cpp`
already provides empty-body client-side definitions of both handlers'
`execute()` (the "CGStoreOpen precedent" from the parked line, used
when a family's packet needs to compile unconditionally into the client
rather than being guarded out). For these two, the merged header keeps
`<Name>Handler` fully unconditional — no `__GAME_CLIENT__` guard —
matching what both trees already did. The only edit needed in
`CGHandlersStub.cpp` was dropping its stub's
`throw(ProtocolException, Error)` exception spec to match the migrated
header's now-unconstrained declaration (an out-of-line definition's
exception spec must match its declaration exactly; the client's old
per-pair declaration carried `throw()`, the merged/server-style one
doesn't) and repointing its two `#include` lines off `Cpackets/`.

**CMake wiring deviated from the pilot's stated rule, for a structural
reason discovered while implementing this batch, not a style
preference.** The pilot's single combined `shared_packets` INTERFACE
target held every family's sources together; `LoginServerPackets` (CL's
owner) linking it was the only "for real" link that existed, since CL
was the only populated family. This batch is the first time a *second*
family (CG) needs a *different* owner (`GameServerPackets`). Populating
`shared_packets`'s combined source list with CG sources while
`LoginServerPackets` still linked the same combined target would have
made `LoginServerPackets` silently absorb CG's `.cpp` files too (and
symmetrically, `GameServerPackets` linking the combined target for CG
would absorb CL's `CLGetWorldList.cpp`) — each family's
`*Handler.cpp` only exists in its owning target, so an absorbed
foreign-family `.cpp`'s handler-dispatch call has no definition to link
against in the wrong target. Whether this is actually a hard link error
turns on GNU `ld`'s standard archive-member-selection behavior (unused
`.o` members of a linked static archive are never pulled into the final
executable) — plausibly harmless in practice, but unverifiable without a
compiler in this environment, and the pilot's own commit message treated
the equivalent case as a real link error. Rather than bet on that
distinction, `shared/Packets/CMakeLists.txt` now defines one INTERFACE
library **per family** (`shared_packets_cg`, `_gc`, `_cl`, `_lc`), each
with its own `target_sources`; `LoginServerPackets` was repointed from
`shared_packets` to `shared_packets_cl` (a mechanical rename, identical
effective sources, since CL was already all `shared_packets` ever
contained); `GameServerPackets` newly links `shared_packets_cg`;
`DarkEden` links both. This is airtight regardless of the archive-linking
question above, and generalizes cleanly — a future family gets one more
`_add_shared_packets_family()` call and one more consumer link line, with
zero risk of an already-landed family's wiring being disturbed. See
`shared/Packets/CMakeLists.txt`'s header comment and
`shared/Packets/README.md` step 5 for the full mechanism, now updated to
match.

Ratchet: `check-packet-duplicates.sh --count` 324 → 304 (10 class pairs
× `.h` + `.cpp`), baseline updated via `--update` in this batch.
`normalize-packet-style.py --all --tsv` pair count 162 → 152, style-only
61 → 51, both exactly -10 as expected.

**Not build-verified — no compiler in the environment that did this
work**, same as the pilot. Verified by reading: `git grep -i` for every
old `Cpackets/<Name>.h` path (all case variants) returns nothing under
`dkrix/`; the 10 pairs' files exist only at `shared/Packets/`, confirmed
absent from both old locations; `dkrixserver/src/Core/CMakeLists.txt`'s
`CG_PACKET_SOURCES` no longer names any of the 10 `.cpp` files but still
names all 10 `*Handler.cpp`; `shared/Packets/CMakeLists.txt` names each
of the 10 exactly once, under `_SHARED_PACKETS_CG_SOURCES`. Both-tree CI
remains the real gate — per `docs/CLAUDE.md`, this client+server change
lands as one unit.

**CI verdict, 2026-08-08: both trees red on the same push, both from the
identical root cause — a target that consumes the migrated headers
indirectly, one level removed from the target the pilot actually wired
up.** Not a repeat of the pilot's *resolution-mechanics* saga (three
pushes, wrong shim scope) — this time the shim/include-path mechanism
itself was correct, it just wasn't applied everywhere a real consumer
existed.

- **Client — run #17 (`ac642e0`) red.** `error C1083: Cannot open
  include file: 'CGLogout.h'` from `Client/PacketDef.h`, compiled into
  the **`VS_UI`** CMake target (pulled in via `MPlayer.cpp` /
  `MTradeManager.cpp`, both part of `VS_UI_CLIENT_SOURCES`) — a
  completely different target from `DarkEden`, which is the only one
  the pilot ever wired up. Fixed in `e75eb67`: added `shared/Packets`
  and `Client/Packet/SharedPacketsShim` to `VS_UI`'s own
  `target_include_directories`, header-visibility only — deliberately
  not linking `shared_packets_cg`/`_cl` there, which would recompile
  their `.cpp` sources a second time (once into `VS_UI`, once into
  `DarkEden`), a multiply-defined-symbol error.
- **Server — run #17 (`ac642e0`) red, found after the client fix.**
  `fatal error: CGLogout.h: No such file or directory` from
  `src/server/gameserver/ZonePlayerManager.cpp` — a `gameserver`-direct
  source, not part of `GameServerPackets`. Root cause one level more
  subtle than the client case: `GameServerPackets` *does* link
  `shared_packets_cg`, correctly — but `PRIVATE`, so the include path
  never propagates to `gameserver`, which links `GameServerPackets` as
  a dependency. Fixed in `b84723f`: added `shared/Packets` to all three
  server executables' (`gameserver`/`loginserver`/`sharedserver`) own
  include dirs, same header-visibility-only pattern. No shim needed
  server-side — `src/Core`, already on every executable's include
  path, already has the server's own `Packet.h`/`PacketFactory.h`.
- **Run #18 (`e75eb67`/`b84723f`) — both trees green.** Client:
  `MSVC x64 (Debug)` + `Viewers and validators`, 32m7s. Server:
  `make debug` + `clang-format` + `ratchets`, 20m21s.

**Lesson for Wave 2+:** the pilot's DarkEden-only and
GameServerPackets/LoginServerPackets-only wiring was correct for what
existed *then* — nothing was migrated yet that any other target
touched. Every batch that follows needs to be checked against **every**
target that might compile a file referencing the migrated headers, not
just the "obvious" owning target — `git grep` for each migrated name
across the whole tree (not just `src/Core`/`Client/Packet/Cpackets`)
before assuming the existing shim/include-dir wiring is sufficient.

**Wave 1 batch 2 landed 2026-08-08 (worktree, not yet on `main`): 13
more CG-family pairs.** `normalize-packet-style.py --all --tsv` re-run
confirmed 51 style-only pairs (unchanged since batch 1, all CG/CL).
Excluded the same instructed families, plus `CGExpelGuild`,
`CGQuitUnionAccept`, `CGDenyUnion`, `CGQuitUnion` — confirmed via `git
show --stat` on the same day's `1ebedbe` (Phase 11.2 batch 4,
guild-formation/union `PreparedStatement` migration) to be the exact
files that parallel workstream touches. Ranked the remaining 44
candidates by case-insensitive client include-site count; the two
pairs batch 1 flagged as messier (`CGPortCheck` — `DatagramPacket.h`
second local include; `CGNPCAskAnswer` — `SocketEncryptInputStream.h`
path/`Assert1.h` filename divergence) were skipped again for the same
reasons. Final 13, all confirmed `residual: 0` / `style-only`
individually via `--pair`, all case-insensitive include count 1–3:
**CGDisplayItem, CGLotterySelect, CGRequestStoreInfo, CGSelectQuest,
CGSelectRegenZone, CGStoreClose, CGStoreOpen, CGStoreSign,
CGUndisplayItem, CGSelectBloodBible, CGGQuestAccept, CGGQuestCancel,
CGModifyTaxRatio**. None touch SQL (`executeQuery`/`PreparedStatement`
grep on each `*Handler.cpp` — 0 hits), confirming no overlap with the
parallel Phase 11.2 workstream beyond the four names above.

The first 9 already had `dkrix/Client/CGHandlersStub.cpp` stubs (the
"CGStoreOpen precedent" — `CGStoreOpen`, `CGStoreClose`, `CGStoreSign`
are literally that stub file's own store-related entries), so their
merged headers keep the Handler class fully unconditional, exactly
like `CGBuyStoreItem`/`CGWithdrawPet` in batch 1; `CGHandlersStub.cpp`
dropped their `throw(ProtocolException, Error)` specs and repointed 9
`#include` lines off `Cpackets/`. The other 4
(`CGSelectBloodBible`, `CGGQuestAccept`, `CGGQuestCancel`,
`CGModifyTaxRatio`) have no stub, so kept a guard — but the client's
own pre-migration copies were inconsistent about *what* they guarded:
`CGGQuestCancel` and `CGModifyTaxRatio` wrapped the `<Name>Handler`
class declaration itself in `#ifndef __GAME_CLIENT__` (the `CGAuthKey`
pattern); `CGSelectBloodBible` and `CGGQuestAccept` left the class
declaration unguarded and relied on the `.cpp`'s dispatch-call guard
alone (a declared-but-never-ODR-used static method needs no
definition). Each pair's merged header replicates its own pre-existing
choice exactly rather than normalizing the two styles, since either is
behaviorally identical to what already shipped.

**New finding, beyond what batch 1's CI fixes were checked to
cover:** all 13 of this batch's original headers carry explicit
`#include "Exception.h"` / `#include "Types.h"` (client side as
`"../Exception.h"` / `"../Types.h"`) — unlike the pilot and all 10
batch-1 pairs, none of which ever included them explicitly (they rely
on `Packet.h` pulling both in transitively). Keeping the server's
canonical bare form in the merged header is correct for the server
build (`shared/Packets` sits alongside `src/Core` on every consuming
target's include path already), but would not resolve on the client:
`Client/Packet/Exception.h` and `Client/Packet/Types.h` exist only in
the broad `Client/Packet/` directory, which is deliberately kept off
`DarkEden`'s and `VS_UI`'s include paths (the pilot's fix-attempt-1
saga — `Client/Packet/FileAPI.h` shadowing the Windows SDK's
`fileapi.h`). Fixed the same way the pilot fixed the equivalent
`Packet.h`/`PacketFactory.h` gap: two more one-line forwarders,
`dkrix/Client/Packet/SharedPacketsShim/Exception.h` and `Types.h`,
dropped into the existing shim directory — no CMakeLists.txt change,
since that directory is already on `DarkEden`'s and `VS_UI`'s include
paths from the pilot and the `e75eb67` fix. Confirmed only one
`Exception.h` and one `Types.h` exist under `dkrix/Client/` (no
resolution ambiguity the shim could introduce).

Separately, `git grep` for all 13 names across the whole tree (per the
Wave 2+ lesson above) found two more instances of batch 1's *other*
gap class — server executables' own direct sources reaching a migrated
header without going through the owning `*ServerPackets` library:
`dkrixserver/src/server/gameserver/quest/ActionGiveLotto.cpp`
(`CGLotterySelect.h`) and `.../gameserver/Item.cpp`
(`CGRequestStoreInfo.h`), both bare `#include`s. **No new wiring was
needed** — `b84723f`'s fix (added `shared/Packets` to all three server
executables' own include dirs) was already family-agnostic, not
CG-batch-1-specific, so it already covers these. Confirmed by reading:
`gameserver`'s `target_include_directories` already lists
`shared/Packets` unconditionally. This is the one piece of positive
evidence this batch adds that the existing wiring generalizes as
claimed — everywhere else it was checked, it held; only the
Exception.h/Types.h gap above was genuinely new, and it was a header
the pilot/batch-1 pairs never needed, not a hole in the CG-family
target wiring itself.

Ratchet: `check-packet-duplicates.sh --count` 304 → 278 (13 pairs × 2
files), baseline updated via `--update`.
`normalize-packet-style.py --all --tsv`: 152 → 139 pairs, 51 → 38
style-only.

**Not build-verified — no compiler in this environment**, same caveat
as every prior step. Verified by reading: `git grep -i` (all case
variants) for every old `Cpackets/<Name>.h` path returns nothing under
`dkrix/`; the 13 pairs' files exist only at `shared/Packets/`,
confirmed absent from both old locations (`normalize-packet-style.py
--pair <Name>` now reports "not a complete pair" for each, the same
signal batch 1 used); `dkrixserver/src/Core/CMakeLists.txt`'s
`CG_PACKET_SOURCES` no longer names any of the 13 `.cpp` files but
still names all 13 `*Handler.cpp`; `shared/Packets/CMakeLists.txt`
names each of the 13 exactly once under `_SHARED_PACKETS_CG_SOURCES`.
Both-tree CI remains the real gate per `docs/CLAUDE.md`.

**Wave 2 batch B landed 2026-08-08 (worktree, not yet on `main`): 14 more
CG-family pairs, run in parallel with a sibling "batch A" agent on a
different, non-overlapping set of 14 in its own worktree.** Pairs
assigned up front rather than re-derived by ranking:
**CGQuitUnionDeny, CGReady, CGRequestGuildList,
CGRequestGuildMemberList, CGRequestUnion, CGSMSAddressList,
CGSelectGuild, CGSelectGuildMember, CGSelectNickname,
CGSelectTileEffect, CGSetSlayerHotKey, CGSetVampireHotKey,
CGSilverCoating, CGThrowBomb**. Re-verified individually via
`normalize-packet-style.py --pair <Name>` before touching any file: all
14 confirmed `residual: 0` / `style-only` against this worktree's tree
state (a fresh `--all --tsv` run also showed 139 pairs / 38 style-only
going in, unchanged from batch 2's own tally — batch A's parallel work
lives in a separate worktree and isn't visible here). None overlap the
SQL-migration-adjacent exclusion list or batch A's set.

Reconciliation followed the established recipe for all 14: adopt the
server's canonical style, add `throw()` back to the four
`<Name>Factory` overrides (with the pilot-precedent comment), and guard
`<Name>Handler` per each pair's own pre-existing client behavior. 13 of
14 wrap the Handler class itself in `#ifndef __GAME_CLIENT__` (the
`CGAuthKey` pattern, confirmed individually — none of the 14 have a
`CGHandlersStub.cpp` entry, so all needed a guard of some form); the
14th, **`CGSMSAddressList`**, matches the `CGGQuestAccept`/batch-2
precedent instead — its client copy left the Handler class declaration
unguarded and relied solely on the `.cpp` dispatch call's guard, so the
merged header replicates that exactly. Two more pairs
(**`CGRequestGuildList`, `CGRequestGuildMemberList`**) carry a `virtual`
destructor on the packet class itself (`virtual ~CGRequestGuildList()
{};`) where the client's pre-migration copy simply omitted the
destructor entirely — the normalizer's empty-ctor/dtor equivalence rule
already covers this, and the merged header keeps the server's `virtual`
form. Ten of the 14 (`CGQuitUnionDeny`, `CGRequestGuildList`,
`CGRequestGuildMemberList`, `CGRequestUnion`, `CGSMSAddressList`,
`CGSelectGuild`, `CGSelectGuildMember`, `CGSelectNickname`,
`CGSelectTileEffect`, `CGThrowBomb`) explicitly `#include
"Exception.h"`/`"Types.h"`, resolved via the existing
`SharedPacketsShim` (no new shim files needed — batch 2 already added
`Exception.h`/`Types.h` forwarders there). One pre-existing bug carried
forward unchanged, per this repo's "preserve gameplay logic" rule:
`CGSetVampireHotKey::toString()` indexes `m_HotKey[8]` into an 8-element
array (`SkillType_t m_HotKey[8]`, valid indices 0–7) — present
identically in both the server and client pre-migration copies, debug-
output-only (not part of the wire signature the normalizer checks), and
not this batch's to fix.

**New guard variant found, beyond the pilot's/batch-1's/batch-2's
established set:** six pairs (`CGReady`, `CGSelectTileEffect`,
`CGSetSlayerHotKey`, `CGSetVampireHotKey`, `CGSilverCoating`,
`CGThrowBomb`) wrap their entire `<Name>Factory` class — not just
`getPacketName()`/`toString()` as in every prior batch — in `#ifdef
__DEBUG_OUTPUT__` on the client side (the client never needs a factory
to decode its own outgoing CG packets, only for debug tooling that
isn't compiled into real builds). This generalizes the same
reconciliation rule already in use (adopt the server's unconditional
canonical style) rather than requiring a new one: the Factory class has
no side effects, so making it unconditionally compiled is behaviorally
neutral, it just becomes buildable in configurations where it
previously wasn't. `CGSelectTileEffect` additionally has
`createPacket()`/`getPacketName()`/`getPacketID()`/`getPacketMaxSize()`
declared `private` (no `public:` label before them) in *both*
pre-migration copies — preserved as-is; a derived class narrowing
access on an override doesn't affect dispatch through the
`PacketFactory` base pointer, so this is an existing quirk, not a bug
introduced or fixed here.

**Indirect-consumer sweep (the Wave 2+ lesson from batch 1's CI
saga):** `git grep -i` for all 14 old paths across the whole tree
before touching anything, not just `src/Core`/`Cpackets`, found
consumers beyond the already-known DarkEden/VS_UI/server-executable/
Quest set — but none needed new CMake wiring, only include-line
repoints, because all of them already resolve through include paths
Wave 1 established:
- `dkrix/Client/OtherClass/RequestServerPacketFactoryManager.cpp` and
  `RequestClientPacketFactoryManager.cpp` (6 of the 14 names each) —
  confirmed **not part of any real build**: neither file appears in
  `dkrix/CMakeLists.txt`'s globs (`Client/*.cpp` is one level deep only;
  `OtherClass/` is a subdirectory) nor in any `.vcxproj` (only the
  orphaned `Client.vcxproj.filters`, same non-authoritative status the
  pilot already established for that file). Includes repointed anyway
  for consistency, matching batch 1's treatment of the same two files
  for its own migrated names — no CMake change needed since nothing
  compiles them.
- `dkrix/Client/Packet/Gpackets/GCNPCInfoHandler.cpp` and
  `GCUpdateInfoHandler.cpp` (`CGReady`, both client and server copies)
  — real consumers, guarded `#ifdef __GAME_CLIENT__` (client copy,
  compiled into `DarkEden` via the `Client/Packet/**/*.cpp` glob,
  already covered) or `#ifdef __GAME_CLIENT__` again on the server copy
  (never true there — dead code on the server side, matching the
  client file's shape by design, not a new gap).
- `dkrix/Client/SizeOfObjects.cpp` (`CGSilverCoating`, `CGSelectGuild`,
  `CGSelectGuildMember`, `CGRequestGuildMemberList`) — real consumer,
  part of `Client/*.cpp`, compiled into `DarkEden`, already covered.
  **Found in passing, out of this batch's scope:** this file still has
  a stale `#include "packet\Cpackets\CGResurrect.h"` (mixed-case,
  backslash-separated) left over from Wave 1 batch 1 — that batch's own
  grep evidently didn't match the backslash form, so `CGResurrect.h` no
  longer exists at that path. Not touched here (`CGResurrect` isn't one
  of this batch's 14 pairs); flagged for the next batch or a follow-up
  fix, since if `SizeOfObjects.cpp` is genuinely compiled this is a
  live break the “both trees green” claim for batch 1 didn't catch.
  **Closed out as a follow-up (2026-08-08):** the same mixed-case/backslash
  bug also hit two more already-migrated names in this file —
  `#include "packet/CPackets\CGAddItemToItem.h"` (batch 1) and
  `#include "packet/CPackets\CGLotterySelect.h"` (batch 2) — missed by
  the same grep-pattern gap. All three lines repointed to the bare
  `#include "<Name>.h"` form already used elsewhere in this file
  (`shared/Packets` is already on `DarkEden`'s include path from the
  Phase 12 pilot, so no CMake change was needed). A tree-wide sweep for
  the same backslash pattern against every migrated name found no other
  instances outside this file.
- `dkrix/Client/MPlayer.cpp` (`CGSelectTileEffect`) and
  `dkrix/Client/PacketDef.h` (`CGReady`, `CGThrowBomb`,
  `CGSetSlayerHotKey`, `CGSetVampireHotKey`, included transitively by
  `MPlayer.cpp`/`MTradeManager.cpp`) — both `VS_UI` sources, already
  covered by the `e75eb67` fix (`shared/Packets` +
  `SharedPacketsShim` on `VS_UI`'s own `target_include_directories`).
- `dkrix/Client/Packet/PacketFactoryManager.cpp` and
  `dkrix/Client/UIMessageManager.cpp` (all 14 names, several via two
  `#include` sites each in `UIMessageManager.cpp`) — both part of
  `Client/*.cpp`/`Client/Packet/*.cpp`, compiled into `DarkEden`,
  already covered.
- Server side: `dkrixserver/src/Core/PacketFactoryManager.cpp` and each
  pair's own `*Handler.cpp` (all bare, same-directory includes, no
  `Cpackets/`-style prefix to fix) — part of `GameServerPackets`, which
  already links `shared_packets_cg` (Wave 1 batch 1). `git grep` across
  every subdirectory library under `dkrixserver/src/server/gameserver/`
  (skill, item, billing, war, couple, mission, ctf, quest, mofus) and
  the three server executables found **zero** direct references to any
  of the 14 names outside `src/Core` — this batch does not hit the
  Quest-library gap class the pre-batch fix (`377ff47`) found for
  `CGLotterySelect`.

No new `target_include_directories`/`target_link_libraries` changes were
needed anywhere — every real consumer found already sat on an include
path Wave 1 (pilot, batch 1's `e75eb67`/`b84723f`, batch 2's
`Exception.h`/`Types.h` shim) had already generalized. The only
non-mechanical finding this batch contributes is the whole-class
`__DEBUG_OUTPUT__`-guarded Factory variant above, and the stale
`CGResurrect.h` reference flagged for follow-up.

Ratchet: `check-packet-duplicates.sh --count` 278 → 250 (14 pairs × 2
files), baseline updated via `--update`.
`normalize-packet-style.py --all --tsv`: 139 → 125 pairs, 38 → 24
style-only (both exactly −14, confirmed against the script's own
summary line rather than raw `wc -l`, since the TSV's header/summary
rows otherwise throw off a naive line count).

**Not build-verified — no compiler in this environment**, same caveat
as every prior step. Verified by reading: `git grep -i` (all case
variants) for every old `Cpackets/<Name>.h` path returns nothing under
`dkrix/` except the orphaned, non-authoritative `Client.vcxproj.filters`
(no matching `.vcxproj`, same status the pilot already established);
the 14 pairs' files exist only at `shared/Packets/`
(`normalize-packet-style.py --pair <Name>` now reports "not a complete
pair" for each); `dkrixserver/src/Core/CMakeLists.txt`'s
`CG_PACKET_SOURCES` no longer names any of the 14 `.cpp` files but
still names all 14 `*Handler.cpp`; `shared/Packets/CMakeLists.txt`
names each of the 14 exactly once under `_SHARED_PACKETS_CG_SOURCES`.
Both-tree CI remains the real gate per `docs/CLAUDE.md`; this batch and
the sibling batch A agent's parallel 14 pairs are expected to be
reviewed and cherry-picked onto `main` separately.

**Wave 2 batch A landed 2026-08-08 (worktree, not yet on `main`): 14 more
CG-family pairs, migrated in parallel with a second, non-overlapping
14-pair batch on another agent line.** `normalize-packet-style.py --all
--tsv` re-run against the current tree (139 pairs, 38 style-only)
confirmed all 14 candidates named for this batch — **CGAcceptUnion,
CGAppointSubmaster, CGDenyUnion, CGDownSkill, CGExpelGuild, CGFailQuest,
CGModifyGuildIntro, CGModifyGuildMember, CGModifyGuildMemberIntro,
CGNPCAskAnswer, CGPartyLeave, CGPortCheck, CGQuitUnion,
CGQuitUnionAccept** — verdict `style-only` / `residual: 0` individually
via `--pair`. `CGExpelGuild`, `CGQuitUnionAccept`, `CGDenyUnion`,
`CGQuitUnion` were the four batch 2 skipped as belonging to the
guild-formation/union `PreparedStatement` workstream (`1ebedbe`); that
workstream landed before this batch started and only ever touched
`*Handler.cpp` files, never the packet class pair itself, so all four
were confirmed clean and taken in this batch. `CGPortCheck` and
`CGNPCAskAnswer` — the two pairs batches 1 and 2 both skipped as
"messier" (`DatagramPacket.h` base class; `SocketEncryptInputStream.h`
path plus an `Assert1.h`-vs-`Assert.h` filename divergence) — were
deliberately taken on this time rather than deferred a third time.

Reconciliation followed the established recipe (server's canonical
style, `throw()` added back to the four `<Name>Factory` overrides,
`#ifndef __GAME_CLIENT__` kept around `<Name>Handler` and its dispatch
call) for all 14; none have a `CGHandlersStub.cpp` entry, so all 14 keep
the Handler guard rather than going unconditional. Two new findings
beyond the recipe:

- **`CGPortCheck` derives from `DatagramPacket`** (`DatagramPacket.h`,
  living in `Client/Packet/`/`dkrixserver/src/Core/` like
  `Packet.h`/`PacketFactory.h` — not part of this migration itself,
  stays duplicated). Needed one more one-line forwarder in the existing
  shim directory, `SharedPacketsShim/DatagramPacket.h`, alongside the
  pilot's `Packet.h`/`PacketFactory.h` and batch 2's
  `Exception.h`/`Types.h`.
- **`CGNPCAskAnswer.cpp` includes `Assert1.h`, `SocketEncryptInputStream.h`,
  `SocketEncryptOutputStream.h`** (bare form, matching the server's own
  style) for its `__USE_ENCRYPTER__` read/write path. Needed three more
  shim forwarders. `Assert1.h` is a second, identically-named-content
  file alongside `Assert.h` in both trees (the client `.cpp`'s
  pre-migration copy used `Assert.h`; the server's used `Assert1.h` —
  same macro, just the file the server author happened to include). The
  merged file keeps the server's `Assert1.h` spelling per the
  "adopt server's canonical style" rule, so the shim forwards that name.

**Third finding, more consequential — a real safety check, not just a
new shim:** five pairs (`CGModifyGuildIntro`, `CGModifyGuildMember`,
`CGModifyGuildMemberIntro`, `CGNPCAskAnswer`, `CGPartyLeave`) had their
entire `<Name>Factory` class wrapped in `#ifdef __DEBUG_OUTPUT__` on the
client's pre-migration copy — a coarser-grained version of the
method-level `__DEBUG_OUTPUT__` guard the recipe already normalizes away
(batch 1: "adopt the server's canonical style... unconditional
getPacketName()/toString() instead of `#ifdef __DEBUG_OUTPUT__`-gated").
Unlike the `__GAME_CLIENT__` Handler guard (safe to replicate verbatim,
since the server never defines that macro), `__DEBUG_OUTPUT__` is
defined by **no target in either tree** — grepped for
`target_compile_definitions`/`add_definitions` across both CMake trees,
zero hits. Replicating the client's class-level guard into the merged
header would have deleted `CGModifyGuildIntroFactory` (etc.) from the
*server* build too, where `dkrixserver/src/Core/PacketFactoryManager.cpp`
registers it unconditionally (`addFactory(new
CGModifyGuildIntroFactory())`, confirmed at the call site) — a compile
error, and if worked around by also un-registering it, a live protocol
break (the server can no longer decode that packet ID at all). Resolved
the same way the method-level case already is: dropped the
`__DEBUG_OUTPUT__` guard, Factory unconditional, matching the server's
own pre-migration file exactly. On the client this is a no-op
behaviorally — the class never compiled there either (dead code, since
the macro was never defined), so making it unconditional only adds a
small self-contained unused class to the binary, not a behavior change.
`CGFailQuest` is the one pair where the client wrapped *both*
`CGFailQuestFactory` and `CGFailQuestHandler` together in a single
`#ifndef __GAME_CLIENT__` block (rather than the usual Handler-only
guard) — that one **was** replicated verbatim, since `__GAME_CLIENT__`
is safe on the server (never defined there) and this matches batch 2's
"replicate the client's own pre-existing choice exactly" precedent for
genuinely guard-neutral divergences.

**Indirect-consumer sweep (the check every prior batch's CI-red
incident came from) — five real gaps found, all fixed without any new
`target_include_directories` calls, since every affected target already
had `shared/Packets` + `SharedPacketsShim` wired from the pilot / Wave 1
fixes.** `git grep -i` for all 14 old `Cpackets/<Name>.h` paths (all
case/slash variants) across the whole tree, not just `src/Core`/
`Client/Packet/Cpackets`, found real full-path `#include` sites in:

- **`dkrix/Client/Packet/PacketFactoryManager.cpp`** — compiled directly
  into `DarkEden` via the `Client/Packet/**/*.cpp` glob. All 14 names,
  one `#include "Cpackets/CGXxx.h"` line each, repointed to bare
  `"CGXxx.h"`.
- **`dkrix/Client/UIMessageManager.cpp`** and
  **`dkrix/Client/SizeOfObjects.cpp`** — both compiled into `DarkEden`
  via the `Client/*.cpp` glob (neither is in `VS_UI_CLIENT_SOURCES`).
  12 and 6 `#include` lines respectively repointed; `SizeOfObjects.cpp`
  additionally used backslash path separators and mixed
  `Cpackets`/`CPackets`/`cpackets` casing on some lines — same
  case-insensitive-glob class of divergence Phase 16/the pilot already
  flagged, now confirmed in include paths too, not just filenames.
- **`dkrix/Client/PacketDef.h`** — one `CGNPCAskAnswer.h` site, repointed
  (matching the already-bare `CGLogout.h` line from batch 1 in the same
  file). Reached by `VS_UI` via `MPlayer.cpp`/`MTradeManager.cpp`
  (the exact path batch 1's `e75eb67` fix covers) and by `DarkEden`
  directly — both already had the include paths this needs.
- **`dkrix/Client/Packet/Lpackets/LCReconnectHandler.cpp`** — one
  `#include "../Cpackets/CGPortCheck.h"` inside an `#ifdef
  __GAME_CLIENT__` block, compiled into `DarkEden`. Repointed to bare
  `"CGPortCheck.h"`.

Checked and confirmed clean (no new gaps): every subdirectory library
under `dkrixserver/src/server/gameserver/` (`skill`, `item`, `billing`,
`war`, `couple`, `mission`, `ctf`, `quest`, `mofus`) and under
`dkrixserver/src/server/{loginserver,sharedserver}/` — zero `#include`
sites for any of the 14 names, so the `Quest`-library-style gap the
commit just before this batch found and fixed does not recur here.
`VS_UI` and `SpriteLib` likewise have zero *direct* `#include` sites
(`VS_UI`'s one indirect path, via `PacketDef.h`, is covered above).
`dkrixserver/src/Core/CGSayHandler.cpp` (an unrelated pair's handler)
turned out to bare-`#include` `CGModifyGuildIntro.h` and
`CGModifyGuildMemberIntro.h` — already covered, since it's part of
`CG_PACKET_SOURCES` → `GameServerPackets`, which already links
`shared_packets_cg`. Two files were found still pointing at the deleted
`Cpackets/` paths and deliberately left alone:
`dkrix/Client/OtherClass/Request{Server,Client}PacketFactoryManager.cpp`
— confirmed not part of any build (no `.vcxproj` exists anywhere under
`dkrix/Client/` root, `OtherClass/` is not named by any glob or explicit
source list in `dkrix/CMakeLists.txt`) — same treatment as the orphaned
`Client.vcxproj.filters` the pilot already excludes.

Ratchet: `check-packet-duplicates.sh --count` 278 → 250 (14 pairs × 2
files), baseline updated via `--update`. `normalize-packet-style.py
--all --tsv`: 139 → 125 pairs, 38 → 24 style-only, both exactly -14 as
expected.

**Not build-verified — no compiler in this environment**, same caveat as
every prior step. Verified by reading: `git grep -i` (all case/slash
variants) for every old `Cpackets/<Name>.h` path returns nothing under
`dkrix/` except the two confirmed-dead `OtherClass/` files and the
orphaned `.vcxproj.filters`; the 14 pairs' files exist only at
`shared/Packets/`, confirmed absent from both old locations
(`normalize-packet-style.py --pair <Name>` reports "not a complete
pair" for each); `dkrixserver/src/Core/CMakeLists.txt`'s
`CG_PACKET_SOURCES` no longer names any of the 14 `.cpp` files but still
names all 14 `*Handler.cpp`; `shared/Packets/CMakeLists.txt` names each
of the 14 exactly once under `_SHARED_PACKETS_CG_SOURCES`. Both-tree CI
remains the real gate per `docs/CLAUDE.md`. Both batches landed
(cherry-picked onto `main` in the same review pass), bringing the
packet-duplicates ratchet from 278 down to 222 (250 after batch B alone,
222 after both).

**Wave 3 landed 2026-08-09 (worktree, not yet on `main`): 10 pairs,
clearing the confirmed style-only backlog to zero.** Unlike every prior
wave, this batch is **mixed-family** rather than CG-only: 4 CG pairs
(`CGUntransform`, `CGVisible`, `CGWhisper`, `CGWithdrawTax`) plus the
first post-pilot CL pairs (`CLGetPCList`, `CLGetServerList`, `CLLogout`,
`CLQueryCharacterName`, `CLQueryPlayerID`, `CLVersionCheck`) — the CL
family's target (`shared_packets_cl`) already existed from the
`CLGetWorldList` pilot and already linked into `LoginServerPackets`, so
this was purely adding sources to an existing list, no new target/link
wiring. `normalize-packet-style.py --all --tsv` re-run against this
worktree's tree state confirmed all 10 named pairs at `residual: 0` /
`style-only` individually via `--pair` before any file was touched, and
the pre-batch summary line read `pairs 111 | style-only 10` — this batch
*is* the entire remaining style-only set, not a ranked subset of a larger
pool (unlike every prior wave, which picked winners from a bigger
backlog). Two of the ten — `CGWhisper` and `CLQueryCharacterName` — were
on Wave 1 batch 1's exclusion list ("a parallel SQL-migration workstream
may touch their `*Handler.cpp` files"); that workstream landed and only
ever touched the `Handler.cpp` files, never the packet class pair itself
(consistent with the same caveat resolving cleanly for
`CGExpelGuild`/`CGQuitUnionAccept`/`CGDenyUnion`/`CGQuitUnion` in Wave 2
batch A), so both were clean to take here.

Reconciliation followed the established recipe for all 10: adopt the
server's canonical style, add `throw()` back to the four `<Name>Factory`
overrides (with the pilot-precedent comment), drop whole-class
`#ifdef __DEBUG_OUTPUT__` Factory guards where present (`CGUntransform`,
`CGVisible`, all six CL pairs), and keep `#ifndef __GAME_CLIENT__` around
`<Name>Handler` and its dispatch call — none of the 10 have a
`CGHandlersStub.cpp` entry (checked explicitly), so all 10 needed some
form of Handler guard. **One new guard variant, beyond the pilot's/Wave
1's/Wave 2's established set:** `CGWithdrawTax`'s client Cpackets copy
left the `CGWithdrawTaxHandler` class declaration itself unguarded but
wrapped only the single method declaration inside it in
`#ifndef __GAME_CLIENT__` — a half-step between the `CGAuthKey` pattern
(whole class guarded) and the `CGGQuestAccept`/`CGSMSAddressList`
pattern (class unguarded, `.cpp` dispatch call alone guarded). Replicated
verbatim rather than normalized to either existing pattern, per the
"replicate the client's own pre-existing choice exactly" rule — a
declared-but-never-ODR-used static method still needs no definition
regardless of exactly where the guard is drawn. `CLGetServerList`'s
server-side `.cpp` called `CLGetServerListHandler::execute()`
unconditionally (no guard at all, unlike every other pair in this batch
and most prior ones, where the *server* copy already carried the guard
even though the macro is never defined server-side) while the client
copy's `execute()` body omitted the call entirely rather than guarding
it. Both are behaviorally identical outcomes to the standard merged form
(guarded call: fires server-side since `__GAME_CLIENT__` is never
defined there, compiles out client-side) — the merged file uses the
standard guarded form for consistency with the rest of the family, which
changes nothing observable on either side.

**Indirect-consumer sweep (the check every prior wave's CI-red incident
came from) — 13 real consumer files found across `dkrix/`, zero requiring
new `target_include_directories`/`target_link_libraries` wiring**, since
every affected target (`DarkEden`, `VS_UI`, `LoginServerPackets`) already
had `shared/Packets` + `SharedPacketsShim` (client) or `shared/Packets`
alone (server, already on `src/Core`'s own path) wired from the pilot and
Wave 1's fixes. `git grep -i` for all 10 old `Cpackets/<Name>.h` paths
(all case/slash variants) across the whole tree, not just
`src/Core`/`Client/Packet/Cpackets`, found real `#include` sites,
repointed to bare filenames, in: `dkrix/Client/Packet/PacketFactoryManager.cpp`
(all 10, one line each), `dkrix/Client/PacketDef.h` (6 names —
`CLVersionCheck`, `CLLogout`, `CLGetPCList`, `CLQueryPlayerID`,
`CGWhisper`, `CGUntransform`, `CGVisible` — reached by `VS_UI` via
`MPlayer.cpp`/`MTradeManager.cpp`, already covered by the pilot's
`VS_UI` wiring, and by `DarkEden` directly), `dkrix/Client/UIMessageManager.cpp`
(3: `CLGetServerList`, `CLQueryCharacterName`, `CGWithdrawTax`),
`dkrix/Client/SizeOfObjects.cpp` (3, all with the same backslash/
mixed-case divergence Wave 2 batch A already flagged in this file:
`Packet\CPackets\`, `packet\Cpackets\`, `packet/CPackets\...H`
extension), `dkrix/Client/CWaitUIUpdate.cpp` (1: `CLGetServerList`),
`dkrix/Client/WhisperManager.cpp` (1: `CGWhisper`), and four
`dkrix/Client/Packet/Lpackets/*.cpp` handler files
(`LCCreatePCOKHandler.cpp`, `LCDeletePCOKHandler.cpp`,
`LCQueryResultPlayerIDHandler.cpp`, `LCRegisterPlayerOKHandler.cpp`, each
one `../Cpackets/CLGetPCList.h` or `CLQueryPlayerID.h` site). All of
these compile into `DarkEden` (`Client/*.cpp` and
`Client/Packet/**/*.cpp` globs) and were already covered by existing
include paths — only the `#include` text needed repointing, no CMake
change. `LCLoginOKHandler.cpp`'s `//#include "../Cpackets/CLGetPCList.h"`
is dead (commented out, already superseded by a live
`#include "CLGetWorldList.h"` two lines below from the pilot) and was
left alone, matching how commented-out code has been treated throughout
Phase 12. `dkrix/Client/OtherClass/RequestServerPacketFactoryManager.cpp`
and `RequestClientPacketFactoryManager.cpp` (8 sites each) got the same
treatment as every prior wave — repointed for consistency even though
both files are confirmed not part of any real build (no `.vcxproj`
exists under `dkrix/Client/`, `OtherClass/` isn't named by any glob or
explicit source list). `dkrix/Client/Client.vcxproj.filters` is the one
remaining reference anywhere in the tree, left alone as the orphaned,
non-authoritative file every prior wave has already excluded. Server
side: only `dkrixserver/src/Core/PacketFactoryManager.cpp` and each
pair's own `*Handler.cpp` (all bare, same-directory includes) reference
the 10 names, all part of `CG_PACKET_SOURCES`/`CL_PACKET_SOURCES`/
`LC_PACKET_SOURCES` → `GameServerPackets`/`LoginServerPackets`, both
already linking their `shared_packets_<family>` target. Checked and
confirmed zero direct references anywhere under
`dkrixserver/src/server/gameserver/` (including every subdirectory
library: skill, item, billing, war, couple, mission, ctf, quest, mofus),
`dkrixserver/src/server/{loginserver,sharedserver}/`, and `dkrix/VS_UI/`
(direct — its one indirect path via `PacketDef.h` is covered above), so
none of this batch hits the gameserver-subdirectory-library or
VS_UI-own-include-dirs gap classes Wave 1/Wave 2 found and fixed. No new
`SharedPacketsShim` forwarders were needed either: the 10 headers only
reach `Packet.h`/`PacketFactory.h` (all 10) and `Exception.h`/`Types.h`
(`CGWithdrawTax`, `CLQueryCharacterName`, `CLQueryPlayerID`,
`CLVersionCheck` — all explicit, all already forwarded since Wave 1 batch
2); none use `DatagramPacket.h`, `Assert1.h`, or the
`SocketEncrypt{Input,Output}Stream.h` pair.

**[measured 2026-08-09]** `dkrix/Client/OtherClass/RequestServerPacketFactoryManager.{cpp,h}`
and `RequestClientPacketFactoryManager.{cpp,h}` — the pre-existing dead pair
described above, left untouched by Wave 1/2/3 — were independently
re-verified from scratch (not just re-trusting the prior three batches) and
deleted. Confirmed: (1) both classes are near-duplicate reimplementations of
the live `Client/Packet/PacketFactoryManager` (same method surface;
`g_pPacketFactoryManager` is the one actually instantiated, in
`Client/GameInit.cpp`); `g_pRequestServerPacketFactoryManager` /
`g_pRequestClientPacketFactoryManager` are declared but never assigned
anywhere. (2) `dkrix/CMakeLists.txt`'s `CLIENT_MAIN_SOURCES` glob
(`Client/*.cpp`, `Client/Packet/*.cpp`, `Client/Packet/**/*.cpp`,
`Client/SXml/*.cpp`, `Client/WinLib/*.cpp`) does not reach `Client/OtherClass/`
— a sibling directory to `Packet/`, not a descendant. (3) No other
`CMakeLists.txt` in the tree (`basic/`, `Client/framelib/`, `Client/Platform/`,
`Client/SpriteLib/`, `tools/engine/sprite/`, `shared/Packets/`) names
`OtherClass` or either class. (4) A tree-wide grep for both class names
(covering `#include`, type use, and the `g_pRequest...` globals) matched only
the two file pairs themselves plus `Client/Client.vcxproj.filters`, which is
orphaned — `Client/` has no matching `Client.vcxproj`, only `.filters` and
`.user`; the real build is CMake-generated (`build/DarkEden.vcxproj`, no
`Client.vcxproj` among them). Zero real consumers, zero build-system
references confirmed independently — the four files were deleted;
`Client/OtherClass/` is now empty.

Ratchet: `check-packet-duplicates.sh --count` 222 → 202 (10 pairs × 2
files), baseline updated via `--update`. `normalize-packet-style.py --all
--tsv`: 111 → 101 pairs, summary line changed from `style-only 10 |
style-residual<=5 44 | style-residual>5 48 | real-divergence 9` to
`style-only 0 | style-residual<=5 44 | style-residual>5 48 |
real-divergence 9` — **the confirmed style-only backlog measured at 62
pairs on 2026-08-07 is now fully cleared** (pilot 1 + Wave 1's 23 + Wave
2's 28 + Wave 3's 10 = 62). What remains unmigrated (101 of the original
163 pairs — corrected 2026-08-09; the prior 92 counted the
style-residual subset alone, omitting the 9-pair review queue) splits into `style-residual` (92, further split ≤5/>5
normalized lines) and the 9-pair `real-divergence` protocol-review queue
named in the 2026-08-07 sizing (`CLLogin`, `CLCreatePC`, `CGMove`,
`CGGuildChat`, `CGModifyNickname`, `CGSkillToInventory`,
`CGUseItemFromInventory`, `CGExchangeBuy`, `CGExchangeList`) — none of
these are style-only twins the mechanical batch recipe applies to; each
needs a human protocol read (some show the *server* header as the stale
copy, per the 2026-08-07 sizing note), which is a separate future
decision, not a mechanical follow-up batch.

**Not build-verified — no compiler in this environment**, same caveat as
every prior wave. Verified by reading: `git grep -i` (all case/slash
variants) for every old `Cpackets/<Name>.h` path returns nothing under
`dkrix/` except the dead `OtherClass/` files (repointed anyway), the
orphaned `.vcxproj.filters`, and the one dead commented-out line in
`LCLoginOKHandler.cpp`; the 10 pairs' files exist only at
`shared/Packets/` (`normalize-packet-style.py --pair <Name>` now reports
"not a complete pair" for each, spot-checked on `CGUntransform` and
`CLVersionCheck`); `dkrixserver/src/Core/CMakeLists.txt`'s
`CG_PACKET_SOURCES`/`CL_PACKET_SOURCES` no longer name any of the 10
`.cpp` files but still name all 10 `*Handler.cpp`;
`shared/Packets/CMakeLists.txt` names each of the 10 exactly once, in
the correct per-family list (`_SHARED_PACKETS_CG_SOURCES` for 4,
`_SHARED_PACKETS_CL_SOURCES` for 6). Both-tree CI remains the real gate
per `docs/CLAUDE.md`.

**Wave 4 batch 1 landed 2026-08-09 (worktree `agent/w3-packets`, not yet on
`main`): 15 pairs, the first batch drawn from the style-residual queue.**
Unlike Waves 1–3 (which consumed the residual-0 style-only backlog, cleared
by Wave 3), every pair here has a nonzero residual — selection was the 15
lowest-residual pairs of the 44-pair `style-residual<=5` population, ranked
by `normalize-packet-style.py --all --tsv` at `96a1995`, ties alphabetical:
**CGSelectWayPoint, CLRegisterPlayer (residual 1); CGDepositPet,
CGDonationMoney, CGPartyInvite, CGPhoneSay, CGRangerSay, CGRegistGuild,
CGRequestNewbieItem, CGSelectRankBonus, CGShopRequestList, CGTryJoinGuild
(2); CGConnect, CGLearnSkill, CGRequestUnionInfo (3)** — 14 CG + 1 CL. The
hard gate for a residual pair is wire equality after canonicalization, not
residual 0: each pair's `--pair` verdict showed read/write/size/maxsize all
`match`, each residual diff was read and judged provably wire-neutral
(toString text; server-only Handler method/ctor declarations; include-set
drift; one always-false validation check; one client-only setter — the
itemized classes below), and each **merged file was re-verified post-merge**:
its canonicalized wire signature equals both pre-migration originals' and
its normalized style stream is byte-identical to the server original's
(one reviewed exception, CGConnect).

Two pairs ranked above the cut were skipped — skipping is the designed
outcome of the gate, and both are recorded here rather than forced:

- **CGUseMessageItemFromInventory (residual 1) — demoted to the
  protocol-review queue.** Its class derives from and delegates
  `read()`/`write()` to `CGUseItemFromInventory`, one of the 9
  real-divergence pairs (`size_expr`). The derived pair's own fields are
  stream-equal, but its effective wire format is inherited from a divergent
  base, and the migration would need a novel `SharedPacketsShim` forwarder
  into `Cpackets/` for the very header the protocol review will rework.
  It moves when its base moves.
- **CGConnectSetKey (residual 1) — skipped on build structure, not wire.**
  The only pair whose `.cpp` sits in *two* server family lists
  (`CG_PACKET_SOURCES` and `CL_PACKET_SOURCES` — gameserver and loginserver
  both compile the class and its handler). The per-family
  `shared_packets_<fam>` design has no slot for a cross-server-family pair:
  `_cg` alone unlinks it from LoginServerPackets; `_cg` + `_cl` together
  double-compile it into DarkEden. Needs a small deliberate extension (a
  `shared_packets_cg_login`-style third target linked by GameServerPackets,
  LoginServerPackets and DarkEden) — a wiring decision for its own commit,
  not something a mechanical batch should back into. Found for later: the
  server file carries an unused `#include "libcpsso.h"` (legacy
  copy-protection SSO header, server-tree-only) that the eventual merged
  file must drop — provably compile- and wire-neutral.

Wire-neutrality evidence for the residual classes taken (each verified
against the actual declarations, not assumed): **CLRegisterPlayer** — the
divergence doc's "account family worth re-checking" flag resolved by
measurement: the server form's bare `maxIDLength`-family constants
(`Core/types/PlayerTypes.h`) and the client's `Packet/Types/PlayerTypes.h`
copies are value-identical (all 13 compared), both reached via
`Packet.h → Types.h`, so the client-only `PlayerInfo.h` include (a third
duplicate set, namespace-qualified) disappears with validation limits
unchanged. **CGRegistGuild** — the client's extra
`if (szGuildIntro > 256) throw` operates on a declared `BYTE` (max 255):
dead on read and write, dropped with the server form. **CGDonationMoney** —
the server enum's extra `DONATION_TYPE_200505_WEDDING` shifts the client's
`DONATION_TYPE_MAX` 2→3; zero client-side consumers of any
`DONATION_TYPE_*` name exist outside the pair, and the one server consumer
(`quest/ActionShowDonationDialog.cpp`, Quest library — already wired by
`377ff47`) uses only the two value-unchanged enumerators. **CGConnect** —
the one union-keep: the merged header keeps the client's `setMacAddress`
(live consumer `LCReconnectHandler.cpp:167` fills the MAC before sending;
inline memcpy setter, wire-neutral, unused server-side; memcpy availability
proven both sides — the server's own `SocketInputStream.h` uses it at
header scope). **CGDepositPet** — a new no-guard variant: *neither* side
guarded the Handler class, and the client's `execute()` omitted the
dispatch call entirely; merged file keeps the class unguarded (replicating
the client's — and server's — own choice) with the standard guarded
dispatch call, the CLGetServerList precedent, behaviorally identical on
both sides. **CGDonationMoney** is also this wave's only
`CGHandlersStub.cpp` member (Handler unconditional, stub spec dropped, the
CGBuyStoreItem precedent); the other 13 pairs take the whole-class guard,
each checked against its own client copy.

Indirect-consumer sweep (`git grep -i`, all case/slash variants, whole
tree): 42 include sites repointed across 10 client files —
`PacketFactoryManager.cpp`, `UIMessageManager.cpp`, `SizeOfObjects.cpp`
(backslash + mixed-case + one uppercase-extension variant:
`packet/CPackets\CGSelectRankBonus.H`), `PacketDef.h`, `MPlayer.cpp`,
`CGHandlersStub.cpp`, two `Gpackets/*.cpp` and two `Lpackets/*.cpp`
handlers. Server side beyond `src/Core`: `gameserver/GamePlayer.cpp`
(CGConnect) and `quest/ActionShowDonationDialog.cpp` (CGDonationMoney),
both bare includes covered by `b84723f`/`377ff47`. **Zero new
`target_include_directories`/`target_link_libraries` calls and zero new
shim forwarders** — every consumer sits on wiring the pilot and Wave 1
fixes established. Left alone per precedent: `Client.vcxproj.filters`, the
dead commented-out `CGPhoneSay` include in `PacketFactoryManager.cpp`.

Ratchet: `check-packet-duplicates.sh --count` 202 → **172** (15 pairs × 2),
baseline updated via `--update` in the batch commit.
`normalize-packet-style.py --all --tsv`: 101 → 86 pairs,
`style-residual<=5` 44 → 29. **Not build-verified — no compiler in this
environment**; both-tree CI is the gate, per every prior wave.

**Wave 4 batch 2 landed 2026-08-09 (same worktree, on top of batch 1): 15
more pairs, the next 15 by residual rank (3–4 lines each), all CG:**
**CGTradeAddItem, CGTradeFinish, CGTradeMoney, CGTradePrepare,
CGTradeRemoveItem, CGUseItemFromGear (3); CGAddMouseToZone,
CGAddSMSAddress, CGAddZoneToInventory, CGAddZoneToMouse, CGAttack,
CGCrashReport, CGDeleteSMSAddress, CGDropMoney, CGJoinGuild (4).** Same
per-pair gate and post-merge re-verification as batch 1; all 15 merged
style streams byte-identical to the server originals. Residual classes:
the five CGTrade* pairs and CGUseItemFromGear carry server-only
`executeOusters`/`executeCoupleRing` Handler declarations (declaration-only
client-side, never ODR-used; CGUseItemFromGear also keeps the server's
`class GamePlayer;` forward declaration and `GamePlayer*` parameter type on
that guarded declaration); CGJoinGuild repeats CGRegistGuild's always-false
`BYTE > 256` check (verified `BYTE` on both paths); and **eight pairs share
one residual class new to the migration: a server-only explicit empty
destructor of the form `~CGAttack() { __BEGIN_TRY __END_CATCH_NO_RETHROW }`**
(the client copies simply omit the destructor — behaviorally identical, an
empty body can't throw).

That destructor macro is the batch's one piece of new client-side
infrastructure: `__END_CATCH_NO_RETHROW` existed only in the server tree's
`Exception.h`, so `dkrix/Client/Packet/Exception.h` now defines it too —
mirroring the server macro's semantics (catch, record, no rethrow;
destructor-safe) but following the client file's own
`addStack(__FILE__, __LINE__)` convention rather than the server's
`__PRETTY_FUNCTION__` (which MSVC lacks). Behaviorally inert for every
pre-existing client file (nothing referenced the name before), resolved via
the existing `SharedPacketsShim/Exception.h` forwarder — no new shim files,
no CMake change. Guards: whole-class for 13 pairs; **CGAddSMSAddress and
CGDeleteSMSAddress replicate their client copies' dispatch-call-only guard**
(class declaration unguarded — the CGSMSAddressList/CGGQuestAccept
precedent, consistent across the SMS family).

Indirect-consumer sweep: 38 sites repointed across 10 client files — the
usual four (`PacketFactoryManager.cpp`, `PacketDef.h`,
`UIMessageManager.cpp`, `SizeOfObjects.cpp`) plus `MPlayer.cpp`,
`PacketFunction.cpp`, three `Gpackets/*.cpp` handlers, and **`MItem.cpp` —
a consumer no prior wave had hit** (member of both `VS_UI_CLIENT_SOURCES`
and the DarkEden `Client/*.cpp` glob; both targets already wired, so
include-text repoints only). The five `SocketEncrypt{Input,Output}Stream.h`
users and CGCrashReport's `Assert1.h` resolve via Wave 2 batch A's
forwarders. Zero new wiring anywhere. The one remaining old-path reference
in the tree outside the known-dead set is a dated prose citation in
`docs/TECH-DEBT-AUDIT.md` (historical audit appendix — cite, don't
rewrite).

Ratchet: `check-packet-duplicates.sh --count` 172 → **142** (15 pairs × 2),
baseline updated via `--update`. `normalize-packet-style.py --all --tsv`:
86 → **71** pairs — summary line now
`style-only 0 | style-residual<=5 14 | style-residual>5 48 |
real-divergence 9`. Wave 4's two batches moved 30 of the 44 original ≤5
pairs; the 14 left are the 2 recorded skips above plus CGPetGamble,
CGPickupMoney, CGShopRequestBuy, CGSkillToTile, CGSubmitScore,
CGTakeOutGood, CGTameMonster, CGUsePowerPoint, CLDeletePC, CLSelectPC
(residual 4) and CGRequestRepair, CGShopRequestSell (residual 5). The
48-pair `>5` set and the 9-pair protocol-review queue are untouched.
**Not build-verified — no compiler in this environment**; both-tree CI is
the gate, per every prior wave.

**Wave 4 batch 3 landed 2026-08-09 (worktree, on top of batch 2): the last
12 mechanical pairs, plus the review queue's two `land-now` pairs.** Three
commits, ratchet **142 → 114**.

*Part A — the 12 mechanical next-ups batch 2 named* (one commit):
**CGPetGamble, CGPickupMoney, CGRequestRepair, CGShopRequestBuy,
CGShopRequestSell, CGSkillToTile, CGSubmitScore, CGTakeOutGood,
CGTameMonster, CGUsePowerPoint, CLDeletePC, CLSelectPC** — 10 CG + 2 CL.
This empties the ≤5-residual queue except for its two recorded skips.

*Residual-label drift, cause found rather than guessed.* Batch 2's tally
above (`≤5 14 | >5 48`) no longer matches the tree, which measures
`≤5 9 | >5 53` — five pairs it lists at residual 4 (CGPetGamble,
CGPickupMoney, CGSkillToTile, CGTakeOutGood, CGTameMonster) now measure
6. Cause: `cc4ea8b` path-qualified bare `"Assert.h"` includes across 15
`Cpackets/` files, five of them in this set, adding one changed include
line (+2 residual) each. Client-side include spelling, not protocol
drift, and it disappears with the client copy — the merged file takes the
server's `Assert1.h` spelling via the existing `SharedPacketsShim`
forwarder. The 12-pair target set was named explicitly, not derived by
threshold, so it did not change. The correction to batch 2's numbers is
recorded by the parallel triage stream in
`docs/phase12-residual-triage-2026-08-09.md`; it is deliberately not
patched into batch 2's paragraph here, which stands as what was measured
at the time.

*Residual classes taken, each read and judged wire-neutral.* Mostly
repeats of batches 1–2: the server-only empty destructor
`~X() { __BEGIN_TRY __END_CATCH_NO_RETHROW }` (seven pairs); server-only
extra Handler method declarations — `executeAll` (CGRequestRepair),
`executeOpSwapAdvancementItem` (CGShopRequestSell), `executeEvent`
(CGShopRequestBuy, where the client copy instead declared
`sendFailPacket`), all inside the whole-class guard on both sides, with
`git grep` finding **zero** references to any of these 12 Handler classes
anywhere in `dkrix/`; `toString()` text drift; and client-only top-level
`SocketInputStream.h`/`SocketOutputStream.h` includes that `Packet.h`
already pulls in transitively. Two are new and neither was assumed:

- **CLDeletePC — a real validation-bound difference, not an always-false
  check.** `szSSN > 14` (server `read()`) vs `> 18` (client `write()`);
  lengths 15–18 are inside the gap, so batch 1's `BYTE > 256` reasoning
  does **not** apply. Resolved by measurement instead: the client's single
  `CLDeletePC` send site sets the field to the literal `"CONFIRM"`, 7
  characters [measured, `dkrix/Client/UIMessageManager.cpp:1431` — the
  only `setSSN` call in `dkrix/`]. Both bounds are unreachable at that
  length. The server's 14 was kept, which leaves the loginserver's read
  path byte-for-byte as it is today; adopting the client's 18 would have
  **widened** server-side validation and was rejected for that reason.
- **CLSelectPC — four client-only unguarded `cout` debug prints** in
  `read()`/`write()`. Dropped with the server form: stdout, not the wire,
  and `dkrix/` calls `AllocConsole` nowhere [measured], so they have no
  console to reach.

*Three new `SharedPacketsShim` forwarders* — the batch's only new client
infrastructure, all following the pilot's mechanism. `Player.h`
(CLSelectPC.h is the first migrated header to include it; the Cpackets
copy spelled it `"../Player.h"`, and `Client/Packet/Player.h` is the only
`Player.h` in `dkrix/`, so there is nothing to shadow) and
`SocketInputStream.h` / `SocketOutputStream.h` (CGPickupMoney.cpp is the
first migrated `.cpp` to include them at top level rather than
transitively — a standalone TU in `shared/Packets/` has no include stack
to walk, the pilot's own lesson). Note `CGModifyNickname.h`'s
`"NicknameInfo.h"` needed **no** forwarder: `dkrix/Client` is already on
both DarkEden's and VS_UI's include paths, unlike `Client/Packet`.

*Part B — the two `land-now` protocol-review pairs*, per
`docs/packet-protocol-review-2026-08-09.md`'s recommended order, one
commit each. Both briefs' premises were re-checked against the current
tree before landing and both hold: size and maxsize expressions match,
and the only `read_seq`/`write_seq` DIFF in each is a member **name** at
an unchanged position and width.

- **CGGuildChat** — `m_Type` (server) vs `m_bType` (client), both `BYTE`.
  Server form kept, including `getType() const` over the client's
  non-const `GetType()` (called nowhere in `dkrix/`) and
  `setMessage(const string&)` over the client's by-value copy. Two client
  call sites rewritten, `UIMessageManager.cpp:1961,1963`.
- **CGModifyNickname** — `m_ItemObjectID` (server) vs `m_NicknameID`
  (client), both `ObjectID_t`. **One correction to the brief, found by
  measurement while landing it:** "adopt the server file" is not safe
  verbatim here, because the server's setter is
  `setItemObjectID(WORD id)` while the member is `ObjectID_t` and both
  sides read/write it at DWORD width — a latent 32→16-bit truncation. It
  has never bitten the server, which only `read()`s this request packet
  and calls the setter nowhere, but the **client** is the sole caller and
  passes `MItem::GetID()`, a `TYPE_OBJECTID` (`unsigned int`,
  `Client/MTypeDef.h:47`). The merged setter takes `ObjectID_t`. Two
  client call sites rewritten, `UIMessageManager.cpp:9917,9930`; the
  nearby `_CGSelectNickName.setNicknameID` at `:9840` is a different
  packet class and was left alone.

*Constructor-initialisation hazard, checked and clear.* A parallel stream
found on `CGDissectionCorpse` that "adopt the server's canonical form"
can silently drop a client ctor's initialisation of a wire member —
distinct from, and not to be filed with, the behaviour-neutral server-only
empty **destructor** class. All 14 pairs in this batch were re-checked by
brace-matching every constructor body on both sides and diffing the set of
members each assigns: 13 pairs have an empty or absent ctor on **both**
sides (nothing to lose), and the one pair that does initialise —
**CGShopRequestSell** (`m_ObjectID`, `m_ItemObjectID`, `m_OpCode`) —
carries the identical ctor in the *server* copy, so the merged file
preserves all three. No pair loses an initialisation.

*Sweep and wiring.* 32 include sites repointed across 5 client files —
`PacketFactoryManager.cpp`, `PacketDef.h`, `UIMessageManager.cpp`,
`SizeOfObjects.cpp` (the uppercase-backslash `packet/CPackets\…` variant
again) and `Lpackets/LCPCListHandler.cpp`. Every server consumer is
inside `src/Core/` and already resolved by each
`shared_packets_<family>` target's include directory — **zero new
`target_include_directories`/`target_link_libraries` anywhere**.
`Client.vcxproj.filters` left alone per precedent; the review doc's own
prose citation of an old path is a dated record, not a reference to
rewrite.

Ratchet: `check-packet-duplicates.sh` **142 → 114** (14 pairs × 2),
baseline updated via `--update` in each commit.
`normalize-packet-style.py --all`: 71 → **57** pairs — summary line now
`style-only 0 | style-residual<=5 2 | style-residual>5 48 |
real-divergence 7`. What is left: the 2 recorded skips
(`CGUseMessageItemFromInventory`, `CGConnectSetKey`), the 48-pair `>5`
set, and 7 of the 9 review pairs. **Not build-verified — no compiler for
either tree in this environment**; both-tree CI is the gate, per every
prior wave.

**Batch 5A landed 2026-08-09: the first 12 of the `>5`-residual set.**
Ratchet **114 → 90**. Pairs, all CG, all into `_SHARED_PACKETS_CG_SOURCES`:
**CGAddGearToMouse, CGAddMouseToGear, CGAddMouseToQuickSlot,
CGAddQuickSlotToMouse, CGCastingSkill, CGDialUp, CGPhoneDisconnect,
CGReloadFromInventory, CGReloadFromQuickSlot, CGUsePotionFromQuickSlot,
CGNPCTalk, CGRequestPowerPoint** — the batch 5A set from
`docs/phase12-residual-triage-2026-08-09.md`, taken in the order that doc
recommends. Recipe unchanged from Wave 4 batch 3.

*Gates.* Per pair before deletion: `--pair` wire signature all four
`match`, verdict `style-residual`, zero notes. Post-merge against the
pre-migration originals exported from `cd8c3c6`: **merged-vs-server
residual 0 / `style-only` for all 12**, and merged-vs-client reproduces
each pair's pre-migration residual exactly (8 ×10; 6 for `CGNPCTalk` and
`CGRequestPowerPoint`) — no new drift. The ctor hazard was checked with a
detector first validated against `CGDissectionCorpse`, where it correctly
reports client-only init; no pair here initialises on either side.

*The step that nearly shipped a broken client.* Deleting the 12 Cpackets
headers left **25 `#include` sites across 5 files** naming the deleted
path (`PacketFactoryManager.cpp` 11, `PacketDef.h` 11, plus
`GCDeleteandPickUpOKHandler.cpp`, `MItem.cpp`, `UIMessageManager.cpp`).
The ratchet does not see this, the normalizer does not see this, and
nothing in the per-pair gate covers it — **the include sweep is a
required step of the recipe, not an optional tidy-up**, and it is the one
step whose omission produces a hard client build failure. Wave 4 batch 3
did the same sweep across its own 5 files; that it was needed twice
running is the argument for treating it as part of the recipe.

*Format-gate consequence, worth knowing before batch 5B.* One repointed
line sat in a tab-indented include run in `MItem.cpp`. Putting the new
line at column 0 was not enough: clang-format treats a contiguous include
run as a unit, so the changed-lines gate kept demanding the four
neighbours too. De-indenting the whole run converges — the behaviour
recorded in Phase 10 bullet 1 and re-confirmed here. Cost 5 whitespace
lines, verified neutral with `git diff -w`.

**[unverified] against a compiler.** CI is the gate.

### `shared/` had no `.clang-format` at all — found 2026-08-09

While running the gate over batch 5A: there was **no `.clang-format`
anywhere above `shared/`** — not at the repo root, not in `shared/`, not
in `shared/Packets/`. Every `clang-format` invocation against that tree
silently used the built-in LLVM default (2-space indent, 80 columns,
right-aligned pointers), which is this project's style in neither tree.

It went unnoticed because **neither CI format job scopes the directory**:
`server.yml` greps `^dkrixserver/`, `client.yml` does `cd dkrix` and diffs
`--relative`. So the canonical packet tree Phase 12 has spent the month
building — 236 files — is format-checked by nothing.

Fixed only in the narrow sense: `shared/.clang-format` now exists, copied
from `dkrix/`'s rather than `dkrixserver/`'s. The two are byte-identical
apart from include handling, and these files compile into both trees, so
the safer wins — packet headers being exactly where include order has
already broken this project once (Wave 4's `PlayerInfo` regression).

**Still open, and it is a decision, not a task:** 79 of the 236 files do
not match that style, inherited from the recipe (each merged file keeps
its server original's formatting). Wiring a gate means choosing between
reformatting those 79 and ratcheting on new lines only. Note the second
option is weaker here than in the legacy trees: files arrive in
`shared/Packets/` whole, so "new lines only" means every migrated file is
checked in full on arrival anyway.

### Phase 13 — Endian-safe wire I/O (server half done here via Phase 9)
`main` already has the server side: opt-in `readLE`/`writeLE`
(`56e59cc`). Remaining: `dkrix/Client/Packet/SocketInputStream` /
`SocketOutputStream` duplicate the server's raw-cast stream files.
Phase 12 folds the two copies together — do the client half there,
not twice.

### Phase 14 — Deterministic CMake source lists — folded into Phase 10, bullet 2.

### Phase 15 — CI build matrix — folded into Phase 10, bullet 3.

### Phase 16 — CI ratchet activation (done on this line 2026-08-07)
Both scripts lifted verbatim from the tag into `dkrixserver/scripts/`,
re-baselined against `main` via each script's own `--update` mode, wired
as a third `ratchets` job in `server.yml`, and verified both ways: exit 0
on the current tree, exit 1 with the baseline set one below the measured
count.

- `check-sql-injection.sh` — counts `executeQuery`/`setStatement`/
  `Statement` calls carrying a `%[sdluxc]` format spec. **Baseline 542
  at activation; 0 since the 11.2 close-out (2026-08-09).**
  (Parked line: 567 at introduction, 540 at its tip — `main`'s own
  Phase 11.2 run finished the job the parked line started.) The 598 in Ground truth is a narrower
  `executeQuery`-only grep; both are recorded, and the script's own
  count is what the gate enforces.

  **[rewritten 2026-08-10 — that "0" measured nothing.]** The gate saw
  only *single-line* printf-format calls. Multi-line `StringStream`
  construction, the dominant legacy shape, was invisible to it; so were
  expression splices; so was Bug 18-A by construction. It now classifies
  the SQL argument of every sink — `executeQuery`, `executeQueryString`,
  `setStatement`, `Statement`, and `PreparedStatement <name>(` — into
  three separately-ratcheted categories:
  **`format=70` (unchanged, and byte-for-byte the same 70 sites the old
  gate reported), `stream=24`, `splice=164`; total 258** [measured
  2026-08-10]. Per-category so a new format site cannot hide behind two
  removed splice sites. The largest single cluster is
  `loginserver/ItemDestroyer.cpp` — 41 sites splicing an `ownerID`
  straight into a quoted SQL literal, all of them reported as 0 by the
  previous gate. The baseline file is now `key=value`, not a bare
  integer; an old-format baseline exits 2 with an explanation rather than
  passing.
- `check-sql-literals.sh` — **new 2026-08-10.** The gate whose absence
  let Bug 18-A ship. Concatenates the adjacent string literals of every
  SQL sink exactly as the compiler would, then checks the result for
  fused statements (`FROM KeyInfoSELECT ...`), keywords glued to the
  preceding word, stray `;`, a missing leading statement verb,
  unbalanced parens/quotes, `'?'`, and placeholder/bind-index mismatch on
  every `PreparedStatement`. **Verified against the defect it exists
  for:** run over `git show 9422e9c^:.../item/Key.cpp` it reports three
  independent errors and exits 1; over the fixed file, 0. Its
  placeholder/bind check independently reproduces the manual bind audit's
  result — 0 defects over all 1,572 `PreparedStatement` literals. Not a
  ratchet: it fails on the first finding. It found one on landing
  (`MonsterKillQuest::save()`'s `executeQuery("-_-")`), which was fixed
  the same day, so its allowlist is empty.
- `check-packet-duplicates.sh` — same-named packet files present in both
  `dkrixserver/src/Core/` and `dkrix/Client/Packet/Cpackets/`.
  **Baseline 326** — every client packet file has a server sibling.
- CI invokes them via `sh script.sh`, not the shebang: the exec bit
  doesn't survive commits from this workstation (every tracked `*.sh`
  is 100644; `Dockerfile.pub` re-chmods for the same reason).
- **Trigger gap found and fixed in the same change:** the duplicates
  ratchet reads a `dkrix/` directory that `server.yml`'s path filter
  didn't watch; `dkrix/Client/Packet/Cpackets/**` added to `paths`.

### Phase 17 — Packet divergence audit (re-run against `main` 2026-08-07)
Done — `docs/packet-divergence-2026-08-07.md`. Headline: 326 files /
163 class pairs, all matched, **0 bit-identical, 0 cosmetic-only** under
a stricter normalizer; every pair differs by >20 normalized lines
(median 157, max `CLRegisterPlayer` at 747). The parked line's own
script, re-run unmodified, reproduces its pair-level result exactly —
nothing has drifted since April. The refinement that matters for
Phase 12: almost every pair is a style/inlining/PCH-guard twin with
**identical field sets** — reconcilable as one scripted normalization
pass — but the CL login/account family (`CLLogin` above all) carries
server-only fields and a client `write()` that hardcodes a login-mode
value: a real wire-shape gap needing manual protocol review, not
reformatting.

### Phase 18 — End-to-end runtime smoke test against `main` (run 1 started 2026-08-10)
The validation gate for the whole 08-06/07 wave, and the top open item
after CI-on-tip. The runbook is lifted to `docs/smoke-test/` (2026-08-06,
paths derived at runtime); the sitting prep pack is
`C:\dev\_incoming\wave-2026-08-09\w6-prep\` (`PREFLIGHT.md`,
`W6-PREP.md`, `RUN-2026-08-10.md`). Its output is a runtime-bug list — the
parked run's Phase 18 kept finding bugs (PP, QQ, …) in a tree where phases
1–17 all compiled clean, which is exactly why a green build does not close
this. Fold Phase 5's glyph check into the same session. Workstation only.

**Run 1 vindicated that premise within its first hour**: the gameserver has
been unable to boot at all since 2026-08-09, and both CIs were green across
every commit in between (see Bug 18-A).

#### Phase 18 run 1 — 2026-08-10 (`main` @ `6d6059e`)

Route: servers in WSL (fresh `build-smoke/`), client Route A
(Windows/VS2022). Steps reached — full detail and the environment drift in
`docs/smoke-test/PORTING-NOTE.md`:

| Step | Verdict |
|---|---|
| STEP1_MYSQL | verified, with drift (live DB is the Docker container, not WSL; `WorldDBInfo` schema differs from the initdb seed) |
| STEP2_SERVER | verified, with drift (stale CMake caches, root-owned `lib/`+`bin/`) |
| STEP2_GREEN_SNAPSHOT | **verified** after the Bug 18-A fix — all three servers green, incl. the `GSRequestGuildInfo`→`SGGuildInfo()` round trip |
| STEP3_CLIENT | **verified** — login screen renders from the fresh VS2022 Debug build (art, SPK sprites, text). Three Windows-route divergences from the doc, recorded in `PORTING-NOTE.md` under "STEP3 drift" |
| LOGIN_SMOKE | **verified — end-to-end login → gameplay**, after fixing Bugs 18-B and 18-C |

**Run 1 reached end-to-end login → gameplay on 2026-08-10** — the first time
on `main`. [measured] The parked line got there on 2026-04-20 (`dbc3087`);
this tree never had. The full chain: `CGConnectSetKey`/`CLLogin` →
`LCLoginOK` → `CLGetWorldList`/`LCWorldList(Eslanian)` →
`CLSelectWorld`/`LCServerList` → `CLSelectServer`/`LCPCList` →
`CLSelectPC(rixvamp, PC_VAMPIRE)` → `LCReconnect(127.0.0.1:9998)` → gameserver
TCP established → `CGReady` → `GCSetPosition(X:62,Y:64,Dir:2)` in zone 1003,
followed by `GCRealWearingInfo`, `GCSkillInfo`, six `GCAddNPC`, quest and
store info. On screen: Limbo Castle rendering isometrically with HP/MP bars, a
minimap, NPCs, and a working NPC dialogue (Bricolacas). Client working set
~1 GB, TCP `127.0.0.1:9998 Established`.

**Movement and logout verified too**, completing LOGIN_SMOKE goal 3 [measured]:

```
Receive:CGMove(X:32,Y:36,Dir:UP)
Send:284[3,72] GCMoveOK(X:32,Y:35,Dir:UP)
Receive:CGMove(X:32,Y:35,Dir:UP)
Send:284[3,73] GCMoveOK(X:32,Y:34,Dir:UP)
Receive:CGLogout
Send:320[18,74] GCReconnectLogin(LoginServerIP:127.0.0.1,LoginServerPort:9999,...)
```

Walk requests round-trip with `GCMoveOK`, and the session ends with a clean
`CGLogout` → `GCReconnectLogin` handoff rather than a disconnect — 74 packets
in the session.

One thing this run did **not** establish, and it should not be read into the
result: **character creation was never exercised.** The account already had
`rixvamp`, so `CLCreatePC` is still untested. Note that `CLCreatePCHandler` is
one of the files carrying eight assignment-form `execute()` sites (audited and
judged safe in the 18-C sweep, but never run).

**On the third segfault in `dmesg`.** Three `loginserver` SIGSEGVs are on
record for this sitting; two are the Bug 18-B crashes. The third is not
attributable with confidence — the binary has been rebuilt twice since, so
resolving its address against the current build is unsound, and it most likely
corresponds to the deliberately crashed `gdb`-hosted run. It is **not live**:
the loginserver instance running the end-to-end session started at 12:22:12
and is still up, and nothing restarts it automatically, so no segfault has
occurred since the 18-C fix landed.

- **Bug 18-A — the gameserver could not boot; every build since
  2026-08-09 was dead on arrival.** [measured] `KeyInfoManager::load()`
  (`dkrixserver/src/server/gameserver/item/Key.cpp`) throws inside
  `PreparedStatement::PreparedStatement(Connection*, const std::string&)`;
  the process prints `UNHANDLED EXCEPTION OCCURED` and exits long before
  `>>> ALL INITIALIZATIONS ARE COMPLETED SUCCESSFULLY.` Cause: `6822f0b`
  (Phase 11.2 batch 10) folded the live 9-column query and the dead
  7-column alternative into a single string literal and dropped the `//`
  that had commented the second one out, yielding
  `… TargetType FROM KeyInfoSELECT ItemType, … FROM KeyInfo` — one
  malformed statement. The old `executeQuery` call passed SQL text through
  at *call* time and this code path evidently never re-ran during the
  batch's own checks; `PreparedStatement` sends it to MySQL at
  *construction*, so the migration turned latent garbage into a boot-time
  fatal. Fix: delete the dead 7-column tail; the 9-column form is what the
  April binaries ran every boot. Status: **fixed and runtime-verified** —
  after the fix the gameserver reaches `>>> ALL INITIALIZATIONS ARE
  COMPLETED SUCCESSFULLY.` and `ClientManager->start() INFINITE LOOP`,
  connects to sharedserver, and completes a `GSRequestGuildInfo` →
  `SGGuildInfo()` round trip [measured 2026-08-10, run 1]. Found by:
  Phase 18 run 1, first gameserver launch.
- **Why no gate caught it, and what would.** [measured] Prepared SQL is a
  string literal — it compiles regardless, so neither CI, the ratchets, nor
  `clang-format` can see it, and the migrated call site is *syntactically*
  perfect. The cheap future gate is a SQL-syntax lint over the literals
  passed to `PreparedStatement`/`executeQuery*` (a `sqlparse`/`sqlglot`
  pass in the `ratchets` job); the expensive one is booting the server in
  CI against a seeded MySQL service container. Neither exists today.
- **Blast-radius sweep: exactly one instance tree-wide** [measured
  2026-08-10 — multiline scan of every `dkrixserver/src/**/*.cpp` for
  adjacent string literals where the next literal opens with `SELECT` /
  `INSERT` / `UPDATE` / `DELETE` / `REPLACE`; one hit, this one]. The other
  43 files in batch 10 and the other 11.2 batches are unaffected by this
  pattern.
- **`c3a9f96`'s prepare-failure logging works — first real-world proof.**
  [measured 2026-08-10] The wave added `DBError.log` logging for exactly
  this failure class (prepare-time `SQLException`, which `END_DB` used to
  swallow), and Bug 18-A produced its first genuine entry — with the
  offending SQL echoed in full, which is what made the diagnosis a
  one-minute job rather than a debugger session:

  ```
  2026.08.10-10:15:49:092 : PreparedStatement: mysql_stmt_prepare failed:
  You have an error in your SQL syntax; ... near 'FROM KeyInfo' at line 1
  [sql=SELECT ItemType, ... TargetType FROM KeyInfoSELECT ItemType, ... FROM KeyInfo]
  ```

  Worth noting what this pairs with: the crash banner itself
  (`UNHANDLED EXCEPTION OCCURED`) named only the ctor, no SQL. Without
  `c3a9f96` this bug would have cost the sitting far more.
- **Bug 18-B — the loginserver segfaulted on every login attempt.
  FIXED and runtime-verified.**
  [measured 2026-08-10, run 1] The client reaches the login screen, and the
  wire format is *fine* — the loginserver receives and parses both packets:

  ```
  NEW CONNECTION FROM 127.0.0.1:37744
  RECV PACKET from NONE, CGConnectSetKey(483) 11/11
  RECV PACKET from NONE, CLLogin(153) 32/32
  Receive:CLLogin(ID:testuser,Password:testpass)
  ```

  The log ends there and the process is gone; the client shows
  "Disconnected". `dmesg` confirms SIGSEGV:

  ```
  loginserver[688671]: segfault at 5af4963c171a ip 00005af11e004476
  sp 00007ffda64be730 error 4 in loginserver[5af11ddca000+247000]
  ```

  **Deterministic**: reproduced three times, different ASLR bases,
  byte-identical IP offset every time.

  **Root cause: use-after-free of the `Result` returned by a stack-local
  `PreparedStatement`.** [measured — gdb backtrace, run 1]

  ```
  #0 Result::getField (this=0x555555b65430, index=1)  database/Result.cpp:138
  #1 Result::getString (index=1)                      database/Result.cpp:153
  #2 CLLoginHandler::execute                          Core/CLLoginHandler.cpp:368
  #3 CLLogin::execute                                 Core/CLLogin.cpp:95
  #4 LoginPlayer::processCommand                      loginserver/LoginPlayer.cpp:223
  ```

  `CLLoginHandler.cpp:257-265` declares `PreparedStatement passwordStmt` as a
  **stack local inside an inner `else` block** and assigns
  `pResult = passwordStmt.execute()` to a `Result*` declared in the *outer*
  scope (line 229). `execute()` returns `m_pResult`, which the statement owns;
  `~PreparedStatement()` does `delete m_pResult`. So at that block's closing
  brace the `Result` is freed, and **every** use of `pResult` from line 270
  (`getRowCount()`) through line 381 reads freed memory. All four query
  branches — `webLoginStmt`, `freePassStmt`, `oldPasswordStmt`,
  `passwordStmt` — have this shape.

  Why it survives as far as line 368: freed memory still reads plausibly.
  `getRowCount()` is a plain `uint` member and returns the right value, so the
  `bNoPlayer` check at line 270 passes. Only when `m_Rows`'s heap block has
  been recycled does the vector's data pointer come back wild — gdb shows
  `field = <error reading variable: Cannot access memory at address
  0x5550009ad93a>`, `rax = 0x5550009ad93a`, at
  `const FieldValue& field = m_Rows[m_CurrentRowIndex][index - 1];`.

  This is a Phase 11 migration defect: the old `Statement*` came from
  `pConn->createStatement()` and outlived the block, so the same code shape was
  correct before the migration and is a use-after-free after it.

  > **Correction.** An earlier revision of this entry placed the fault in
  > `Statement::executeQuery()`. That was wrong. It came from computing the IP
  > offset against the base in `dmesg`'s `in loginserver[<base>+<len>]` field,
  > which is the start of the *mapped segment containing the IP*, not the ELF
  > load base — off by `0x10000` here, which landed `addr2line` in the
  > neighbouring function. `Statement::executeQuery()` is not implicated; the
  > crash is on the `PreparedStatement` → `BACKEND_MATERIALIZED` path.
  - It is **not** a bad-credentials path. It crashes identically for
    `222222`/`222222` (no such account) and for `testuser`/`testpass` (a valid
    row: `Access='ALLOW'`, `LogOn='LOGOFF'`). Packet length tracks the
    credential strings (28/28 vs 32/32), so parsing is correct in both.
  - `error 4` is a *read* fault from user mode, and both faulting addresses
    are wild rather than null — `0x56291a2dd` (far below that run's
    `0x56290cac5000` image base, i.e. a truncated pointer) and
    `0x5af4963c171a`. That shape points at a corrupted or truncated pointer
    reaching `mysql_real_query` — `m_Statement`'s buffer or
    `m_pConnection->getMYSQL()` — rather than a missing null check.
  - **Blast radius — candidate set, not yet a confirmed count.** [measured
    2026-08-10] `PreparedStatement` appears in **208** `.cpp` files. Splitting
    the `… = <local>.execute()` call sites by shape: **548** are
    `Result* pResult = stmt.execute();` — the statement and the `Result*` are
    declared in the same block, so the statement is destroyed *after* the last
    use and these are safe. **203 sites across 129 files** assign to a
    **pre-declared outer** variable (`pResult = stmt.execute();`), which is the
    shape that bites here. That 203 is an upper bound: a pre-declared variable
    may still be in the same block. Confirming it needs scope analysis, not
    grep. `CLLoginHandler` is the one confirmed instance so far.
  - **Fix applied: hoist the statement to the same scope as `pResult`.**
    The four branches differed only in SQL text and whether `PASSWORD` is
    bound, so the branch chain now selects a `loginSql` string and a
    `bBindPassword` flag, and a single `PreparedStatement loginStmt` is
    constructed next to `pResult`. Branch conditions are preserved exactly
    (the nested `if/else` under `DB_VERSION` is flattened into the same
    `else if` chain); no SQL text, column order, or bind order changed, so the
    `++i` field indexing downstream is untouched. The alternative — having
    `execute()` transfer ownership — would fix all 203 candidate sites at once
    but is a much wider change; it is not done here.
  - **Runtime-verified 2026-08-10** [measured]: after the fix the loginserver
    survives login and the whole pre-gameplay flow completes —
    `LCLoginOK` → `CLGetWorldList`/`LCWorldList(Eslanian)` →
    `CLSelectWorld`/`LCServerList` → `CLSelectServer`/`LCPCList` (three
    `EMPTY SLOT`s). The client advances past the login screen to **Select
    World**. The process stays up. This is the furthest this tree has ever
    been taken against a live client.
  - **Candidate sweep completed 2026-08-10 — see Bug 18-C.** All 129 files
    were audited; exactly one other live instance was found.
- **Bug 18-C — the same use-after-free on the character-select path.
  FIXED (compile-verified; not yet runtime-verified).**
  [measured 2026-08-10] Found by auditing the Bug 18-B candidate set, not by a
  crash. `src/Core/CLSelectPCHandler.cpp` is a structural clone of 18-B:
  `Result* pResult;` is declared at function scope (line 96, uninitialised),
  and each of the three race branches — Slayer (122-128), Vampire (130-135),
  Ousters (137-142) — declares its own `PreparedStatement` and assigns
  `pResult` from it. The `Result` is freed at the branch's closing brace
  (129 / 136 / 143) and the first dereference is `pResult->getRowCount()` at
  line 146, with further uses through line 195.

  This one is on the **character-select** path, i.e. the next thing
  LOGIN_SMOKE exercises after the 18-B fix — it would have crashed the
  loginserver the moment a character was selected. Fixed the same way: the
  branch chain selects a `pcSelectSql` string and one `PreparedStatement` is
  constructed in the scope that holds the uses. All three branches bind the
  same two parameters (PC name, player ID), so only the SQL text varies.

  **Runtime-verified 2026-08-10** [measured], on the second attempt. The first
  smoke run could not exercise this path — `LCPCList` came back as three
  `EMPTY SLOT`s — so it was recorded as compile-verified only. A later run on
  the same account returned a populated `LCPCList` (47 bytes) and selected the
  ACTIVE Vampire `rixvamp`, confirmed in the database as belonging to
  `testuser`. The Vampire branch of the fixed code ran end to end:

  ```
  RECV PACKET from testuser, CLSelectPC(159) 16/16
  Receive:CLSelectPC(PCName:rixvamp,PCType:PC_VAMPIRE)
  WorldID 1, ServerGroupID : 0, ServerID : 1
  Send:450[18,4] LCReconnect(GameServerIP:127.0.0.1,GameServerPort:9998,KEY:2306048)
  ```

  All four column reads after the fix (`getRowCount`, `getWORD(1)`,
  `getString(2)`, `getInt(3)`/`getInt(4)`) succeeded and the loginserver stayed
  up, handing the client off to the gameserver. Note what is *not* claimed: the
  pre-fix crash was never observed on this path, because the bug was found by
  audit rather than by a failure. What is established is that the fixed path
  works at runtime. The Slayer and Ousters branches remain compile-verified
  only.
- **Audit of the whole candidate set — 1 live bug in 129 files.** [measured
  2026-08-10] Five parallel auditors covered all 129 files carrying the risky
  shape, partitioned by site count. Each traced every assignment-form
  `<var> = <stmt>.execute()` to its owning block and to the last dereference
  of the assigned pointer.

  | Group | Files | Assignment sites | Confirmed |
  |---|---|---|---|
  | 1 | 26 | 44 | 0 |
  | 2 | 26 | 42 | **1** (`CLSelectPCHandler`, 3 branches) |
  | 3 | 26 | 40 | 0 |
  | 4 | 26 | 39 | 0 |
  | 5 | 25 | 35 | 0 |
  | **total** | **129** | **200** | **1** |

  So the earlier "203 risky sites" figure was a shape count, not a bug count —
  the true rate is 1 in 200. The dominant safe shape is the
  `XxxInfoManager::load()` template and the `BEGIN_DB { … } END_DB(pStmt)`
  body, where the statement and every dereference are siblings in one block.
  (`BEGIN_DB` expands to a bare `try` and `END_DB` to a `catch` chain —
  `src/server/database/DB.h:16-27` — so those are genuine scopes and the brace
  analysis holds.)

  **Near-misses: dangling but never read.** Several sites *do* leave `pResult`
  dangling when an inner block closes, and are safe only because nothing reads
  it before it is reassigned. These are one edit away from becoming live bugs
  and are worth knowing about:
  `sharedserver/GuildManager.cpp:92,108,124,140`;
  `gameserver/GuildManager.cpp` (the `init()` else-branches, under
  `#ifdef __SHARED_SERVER__`); `gameserver/item/Key.cpp:198`;
  `Core/CGUseItemFromInventoryHandler.cpp:1067`;
  `Core/CLLoginHandler.cpp:677` (the `eventStmt` block);
  `gameserver/quest/ActionRedeemMotorcycle.cpp:134`;
  `Core/CGUsePotionFromQuickSlotHandler.cpp:270`.
  Also noted: `ctf/FlagManager.cpp:317` assigns the result of a `DELETE` and
  never reads it — dead assignment, harmless.
- **Bind-correctness audit of the whole Phase 11 migration — clean.**
  [measured 2026-08-10] Five parallel auditors covered **all 1,575
  `PreparedStatement` constructions** across the 208 files that use the class,
  partitioned by construction count. Each reconstructed the concatenated SQL,
  counted `?` placeholders outside quoted literals, and matched them against
  the bind calls scoped to that statement object.

  | Group | Files | Statements | Defects |
  |---|---|---|---|
  | 1 | 42 | 397 | 0 |
  | 2 | 42 | 352 | 0 |
  | 3 | 42 | 343 | 0 |
  | 4 | 41 | 246 | 0 |
  | 5 | 41 | 237 | 0 |
  | **total** | **208** | **1,575** | **0** |

  Checked: placeholder/bind count mismatch; bad indices (0-based, duplicate,
  gap, out of range); binds naming a different statement object; `?` in
  identifier position or inside a quoted literal; and the adjacent-literal
  concatenation accident that caused Bug 18-A. Placeholder and bind totals
  agree exactly where counted (group 3: 746/746; group 5: 720/720), and every
  index set is a contiguous `1..N`. Group 4 additionally cross-checked the
  migration commits themselves (`ab67704`, `1ef2643`, `c13508f`, `1ebedbe`,
  `825fa92`, `0893e29`, `068712b`, `6822f0b`, `5a10cb1`, `38dfbbc`), comparing
  each hunk's removed `printf` format-specifier count against its added `?`
  and bind counts — 64 flagged hunks, all resolving to benign causes.

  **Conclusion: the migration was mechanically sound on parameter binding.**
  Its damage was concentrated in *object lifetime* (18-B, 18-C) and in one
  malformed literal (18-A), not in the SQL or the binds. That is worth knowing
  before spending more effort hunting SQL-shaped defects.

  **One real finding, fixed:** `item/Cross.cpp:175` bound `storageID` with
  `bindInt`. `StorageID_t` is `DWORD` (unsigned 32-bit,
  `Core/types/ItemTypes.h:29`) and `bindInt` takes a signed `int`, so an ID at
  or above 2^31 binds negative. It was the only `bindInt(n, storageID)` in the
  tree against **83** `bindLong` — including Cross.cpp's own INSERT eleven
  lines earlier at `:93`. Not reachable with current storage IDs, so no
  observable behavior changed.

  **Deliberate residual injection surface, not defects:** ~22 sites splice a
  C++ expression into the SQL instead of binding it — the `tinysave(field)`
  family in the item classes, plus table-name splices
  (`getObjectTableName()`, `ItemObjectTableName[...]`). A placeholder cannot
  bind an identifier, so these cannot be parameterised; each carries an
  explanatory comment. Three auditors independently flagged them as UNCERTAIN
  and independently concluded they are correct. Their literal portions are
  verified; the spliced fragments are fixed `Column=value` strings or fixed
  identifiers, never user input.

  **Trap for any future automated pass over this tree:** these constructors
  take the connection as the *first* argument
  (`g_pDatabaseManager->getConnection("DARKEDEN")`). Naively harvesting every
  string literal in the constructor call concatenates the connection name onto
  the SQL and produces exactly the Bug 18-A signature —
  `DARKEDENSELECT Fame, ... FROM Slayer`. Two auditors generated this false
  positive independently (7 hits each in `CGSayHandler.cpp` and
  `CGConnectHandler.cpp`) before correcting for it. Only the second argument
  onward is SQL.
- **Two further `Result` lifetime shapes audited — both clean, and one gives
  the rule that explains 18-B and 18-C.** [measured 2026-08-10] The earlier
  sweep only covered "statement in an inner block, `Result*` in an outer one".
  Two other ways a `Result` can outlive its owner were audited by five
  auditors:

  **Shape: re-execute invalidation.** `PreparedStatement::execute()` opens with
  `if (m_pResult != NULL) { delete m_pResult; }`, so re-executing a statement
  frees the `Result` its previous execute returned. Three auditors covered the
  33 files where a statement name carries more than one `.execute()`: **0
  bugs**. An independent tree-wide count then showed the real surface is far
  smaller — comparing `PreparedStatement <name>(` declarations against
  `<name>.execute()` calls per file, exactly **two** objects in all 208 files
  are genuinely executed more than declared, both in
  `Core/GCFriendChattingHandler.cpp` (`insertStmt` 40/44, `deleteStmt`
  294/298). Both are INSERT/DELETE: `execute()` returns `NULL` on the
  non-SELECT path (`PreparedStatement.cpp`, after `mysql_stmt_affected_rows`)
  and both return values are discarded, so no `Result` exists to dangle.
  Hand-verified.

  *Methodology note:* the 33-file shortlist was built on **variable-name**
  recurrence per file, which badly over-selects — the same name (`updateStmt`,
  `guildIdStmt`) is reused across different functions and across mutually
  exclusive `#ifdef` or `if/else` arms. Two auditors independently dissolved
  all 18 and all 21 of their candidate groups on exactly that basis. Compare
  declarations to executes, not names.

  **Shape: the `Result` escapes its statement's scope** — returned from a
  function, stored in a member/global/static/container, or a raw
  `char*`/`const char*` from `getString()`/`getField()` retained past the
  statement. Two auditors covered all 208 files: **0 bugs**. Supporting
  tree-wide facts they established: `new PreparedStatement` and
  `PreparedStatement*` do not appear anywhere in `src/`, and no header declares
  one as a member — **every `PreparedStatement` in the codebase is a stack
  local**, so no site is exempt via a longer-lived statement. Of ~211
  `getString()` sinks checked, every consumer takes `const string&` or `string`
  by value, so the pointer is always copied.

  **The rule worth remembering.** In `Result* pX = stmt.execute();` the
  statement must already be in scope at that line, so `pX`'s scope is
  necessarily a *subset* of the statement's — **that form is safe by
  construction**. All 359 direct-initialised sites are safe for this reason
  alone. Only a **pre-declared** `Result*` (declared before, assigned inside a
  narrower block) can break the invariant; there are 25 such sites, all
  checked. Both 18-B and 18-C were exactly this shape. So the review rule is
  narrow and cheap: *scrutinise pre-declared `Result*` pointers; ignore the
  direct-initialised ones.*

  **~~Latent trap — the two `Result` backends have different pointer
  contracts.~~ WITHDRAWN 2026-08-10 — the claim was wrong.** An earlier
  revision of this file asserted that on the legacy `BACKEND_MYSQL_RES` path a
  `const char*` from `getField()` is invalidated by `next()`. It is not.
  `Statement::executeQuery()` uses **`mysql_store_result`**
  (`Statement.cpp:116`), which buffers the entire result set client-side; each
  row's storage is independent, so a pointer taken from row N stays valid — and
  keeps pointing at row N — after `next()` advances. Both backends invalidate
  only when the `Result` is destroyed. The claim would hold under
  `mysql_use_result`, which this codebase never calls. Retaining such a pointer
  therefore degenerates into the ordinary scope question, not a hazard of its
  own.

- **The legacy `Statement` API — audited 2026-08-10, clean.** [measured] All
  the audits above cover `PreparedStatement` only, and
  `Statement::executeQuery()` has *identical* ownership semantics
  (`Statement.cpp`: `if (m_pResult != NULL) delete m_pResult`). Five auditors
  covered all **197 files** containing `executeQuery`, checking four shapes:
  read after `SAFE_DELETE(pStmt)`; read after a re-execute on the same
  statement; `Result*` escaping; and a retained `const char*` across `next()`.
  **Zero bugs.**

  **The surface is about half what a raw grep suggests.** 487 textual
  `executeQuery` occurrences across 198 files, but the auditors'
  comment-state scanners put **roughly half inside `/* … */` blocks** — the
  Phase 11 migration left the old `StringStream sql; …
  executeQueryString(sql.toString())` commented out directly above each new
  `PreparedStatement`. Per group: 48/120 dead, 40/105, 47/93, 39/81, 40/80.
  Whole directories are already fully migrated — **most of `item/*.cpp` has
  zero live legacy sites.** Any future estimate of "legacy work remaining"
  that counts raw grep hits is inflated ~2×.

  Structurally the legacy path was always the safer one: a `Statement*` is
  heap-allocated by `pConn->createStatement()` and freed at function end, so
  it normally *outlives* the block. That is precisely why the pre-migration
  code was correct, and why converting it to a block-scoped
  `PreparedStatement` created 18-B and 18-C.

  **Adjacent defects found but not fixed** (leaks, not use-after-free — the
  `Statement` and its `Result` are never freed on the success path, because
  `END_DB` only deletes on `SQLQueryException`):
  `EventZoneInfo.cpp:74`, `skill/EffectDarkness.cpp:150`,
  `mission/EventQuestInfoManager.cpp:132`, `CastleShrineInfoManager.cpp:119`,
  `PetTypeInfo.cpp:56`, `skill/EffectYellowPoison.cpp:245`,
  `skill/EffectContinualBloodyWall.cpp:151`, `EventHeadCount.cpp:71`,
  `DefaultOptionSetInfo.cpp:33-58`, `skill/EffectIceField.cpp:165-215`,
  `UniqueItemManager.cpp:75`, `RegenZoneManager` (both loaders),
  `skill/EffectGreenPoison.cpp:153`, `LevelWarZoneInfoManager.cpp:144`, and
  `StringPool.cpp` (both copies). Several are per-zone at startup.
  Also: `MonsterKillQuest.cpp:64` executes the literal SQL string `"-_-"`,
  which will throw on every call if that path is ever reached; and
  `CGSayHandler.cpp:2055-2058` shadows `string PlayerID` with a second
  declaration three lines later.
- **Bug 18-D — `new[]` freed with plain `delete` in `Zone::load()`. FIXED.**
  [measured 2026-08-10, AddressSanitizer] `version`, `zonename` and
  `lwrFilename` are `new char[128]`/`new char[256]`
  (`gameserver/Zone.cpp:798-800`) but were released with `SAFE_DELETE`, which
  expands to plain `delete` (`Core/Utility.h:18`). ASan aborts gameserver
  startup with `alloc-dealloc-mismatch (operator new [] vs operator delete)`
  in `Zone::load` → `Zone::init` → `ZoneGroupManager::load`. Undefined
  behaviour; benign in practice for `char` but heap-corrupting in general.
  Fixed by using `SAFE_DELETE_ARRAY`, which already exists two lines below the
  other macro and is used 52 times elsewhere. Note the same bug was fixed for
  the sibling `pDesc` in this very function **in 2002** —
  `// add '_ARRAY' moved to here.. by sigi 2002.5.2` at `Zone.cpp:873` — and
  its three neighbours were missed.
- **Bug 18-E — heap-buffer-overflow write in every `ExpTable::load()`. FIXED.**
  [measured 2026-08-10, AddressSanitizer] `ExpTable` (`SomethingGrowingUp.h`)
  sized its record vector `m_Records(MaxLevel)`, giving valid indices
  `0..MaxLevel-1`, but levels are **1-based** and `load()` writes
  `m_Records[level]` for `level` in `MinLevel..MaxLevel` inclusive — the
  `Assert(level <= MaxLevel)` immediately above says so explicitly. At
  `level == MaxLevel` this writes 4 bytes past the allocation. ASan:
  `heap-buffer-overflow WRITE of size 4 ... 0 bytes after 2520-byte region`
  (315 records × 8 bytes) in
  `ExpTable<unsigned int, unsigned short, 1, 315, unsigned int>::load()`.
  The out-of-range **reads** in `getGoalExp()`/`getAccumExp()` at
  `level == MaxLevel` had the same defect. Fixed by sizing to `MaxLevel + 1`;
  nothing reads `m_Records.size()`, so widening is behaviour-preserving and
  slot 0 stays unused as the 1-based indexing already assumed. This is a
  *silent heap corruption* in the shipping build — the most serious defect
  found in this sitting, and it fires on every gameserver boot. `Zone.cpp`'s
  `Statement`/query buffer in the same header had the 18-D mismatch too
  (`new char[size]` + `SAFE_DELETE`), fixed alongside.
- **Tree-wide sweep for the 18-D shape: clean.** [measured] Every variable
  assigned from `new T[...]` was checked against `SAFE_DELETE(v)` / `delete v;`
  across all `.cpp`, `.h` and `.hpp` under `src/`. After these two fixes there
  are no remaining instances. The detector was validated by re-running it
  against the pre-fix `Zone.cpp`, where it correctly flagged all three
  variables. **Note the first pass of this sweep searched only `*.cpp` and
  therefore missed 18-E, which lives in a header** — a reminder that a
  file-type filter is part of a claim's scope.
### Phase 18 — Bugs 18-F … 18-N (2026-08-10)

Recorded compactly; each has a commit on
`fix/bug-18b-loginserver-result-uaf` carrying the full reasoning. Verification
level is stated per bug and is the thing to trust.

| Bug | Defect | Found by | Verified |
|---|---|---|---|
| 18-F | `GameServerGroupInfoManager` / `GameServerInfoManager` (6 copies) never initialised their array members while `load()` calls `clear()` first, so `clear()` `delete[]`s an indeterminate pointer | ASan, gameserver boot | runtime |
| 18-G | Store index guard read `> MAX_ITEM_NUM` where the vectors hold exactly `MAX_ITEM_NUM` (20), letting client index 20 through to an OOB read and write. `CGDisplayItem`/`CGUndisplayItem`/`CGBuyStoreItem` | packet audit | compile only |
| 18-H | `CGExchangeBuy::read` called `iStream.read()` on a `std::string`, binding to the raw template — 32 wire bytes reinterpreted as a live `std::string`, giving an arbitrary-address read | packet audit, 2 auditors + hand-check | compile only |
| 18-I | Client-controlled indices bounds-checked at the accessors: `Inventory::getInventorySlot`/`getItem`, `isWear`/`getWearItem`/`takeOffItem` ×3 races. Also the `SKILL_INSTALL_MINE` branch no longer resurrects `bSuccess`, and 3 GM commands were ungated by an operator-precedence typo | packet audit | runtime (smoke test) |
| 18-J | `PetAttrInfo` ctor did `reserve(); clear();` — capacity without size, so every `m_PetAttrLevels[i]` wrote into unconstructed storage | ASan, gameserver boot | runtime |
| 18-K | Five mutating calls inside `Assert(...)`, whose argument is unevaluated under `NDEBUG` — `Assert(pStore->removeStoreItem(...))` would leave the listing while the gold change ran. Silent item duplication in a Release build | anti-cheat audit | compile only |
| 18-L | `GameServerManager::m_pGameServerPlayers` never initialised; `!= NULL` tests passed on garbage. Only reachable once the gameserver booted far enough to connect | ASan, sharedserver | runtime |
| 18-M | A bug in the 18-H fix: `write()` narrowed to `BYTE` *before* the cap check, so a 256-byte key truncated to 0 and passed. Plus `Zone`'s ctor left `m_ppLevel`/`m_pSectors` indeterminate while `~Zone()` frees them, and `Slayer::addWearItem` had `Assert(x = NULL)` — an assignment | adversarial branch review | compile only |
| 18-N | `CLGetWorldListHandler` sized a stack VLA `[Num]` but wrote and read it at index `Num` with 1-based loops — aborts every login under ASan | ASan, during smoke test | runtime |

**Milestone [measured 2026-08-10]: all three servers boot clean under
AddressSanitizer (zero reports) and a client completes login → character
select → enter world → gameplay.** The cluster has never previously been
sanitizer-clean. Each ASan fix exposed the next defect — 18-L in particular was
unreachable until the gameserver booted far enough to connect to the
sharedserver.

**What this says about where bugs live.** Nine of the fourteen Phase 18 bugs are
uninitialised memory or off-by-one container sizing, not logic errors — and
every one of the ASan-found ones was invisible to the compiler, to CI, and to
static review until the code actually ran. The single highest-value gate
remains a boot-under-ASan CI job; see the CI notes below.

### Phase 18 — client-to-server packet audit (2026-08-10)

Five auditors covered all 216 `CG*`/`CL*` packet and handler files, then five
more independently **verified** the highest-severity claims adversarially. The
verification round mattered: it refuted a major claim, resolved a direct
contradiction between two reviewers, and found a bug none of them had claimed.
Treat unverified audit output as a lead, not a finding.

**Fixed in this sitting** (see the commits): the store index off-by-one
(`> MAX_ITEM_NUM` where the vectors hold exactly `MAX_ITEM_NUM`, letting index
20 through, in `CGDisplayItemHandler:52`, `CGUndisplayItemHandler:40`,
`CGBuyStoreItemHandler:54`), and `Player::setKey`'s uninitialised `pHashTable`
plus its 512-byte-per-packet leak.

**REFUTED — `Assert` is *not* compiled out of the shipping build.** Two
auditors reported exploitable overflows on the premise that `make` builds
Release and `Assert` becomes `((void)0)` under `NDEBUG`. `Makefile:11` is
`all: debug`; CI (`server.yml:76`) and the smoke-test runbook both use
`make debug`; no configured build tree sets `NDEBUG`; and the deployed
`bin/gameserver` still contains the stringified assert expressions
(`"verifyIndex(index)"`, `"SlotID <= MAX_PHONE_SLOT"`) — impossible if `NDEBUG`
were set [measured 2026-08-10]. `Assert` throws `AssertionError`
(`src/Core/Assert.h:34-35`). **Root cause of the wrong conclusion:
`dkrixserver/CLAUDE.md` documented `make` as Release.** That line has been
corrected; it is load-bearing and misled two independent reviews. The residual
risk is real but different from what was reported: bounds enforcement is one
`make release` away from vanishing, and `Slayer::setPhoneSlotNumber` has no
assert at all on its 3-element array while `getPhoneSlotNumber` asserts
`<= MAX_PHONE_SLOT`, which is itself off by one.

**Residual risk addressed 2026-08-10.** 25 security-relevant `Assert`s on
wire-reachable indices were converted to real runtime checks that survive
`NDEBUG`, using the `1019602` chokepoint idiom (real `if` placed *before* the
`Assert`, so Debug and Release take the same branch). Covered: both `Slayer`
phone accessors including the off-by-one; `Datagram`/`SerialDatagram`
`read`/`write` (the UDP twin of a check `SocketInputStream::read` has always
had); `PlayerCreature` pet-stash, another `<=` off-by-one; 11 `NPC` shop-rack
accessors; 4 `ItemRack` accessors. Two findings from that pass worth keeping:
`getSlotWithPhoneNumber()` and `findEmptyPhoneSlot()` both return
`MAX_PHONE_SLOT` as their *not-found sentinel* and `CGPhoneDisconnectHandler:69`
feeds it straight back into `setPhoneSlotNumber` — unreachable today only
because of a `Success` guard; and the shop accessors had to **throw** rather
than return `NULL`, because `CGShopRequestBuyHandler` dereferences
`getShopItem()` unchecked, so a sentinel would have converted an OOB read into a
remote NULL-deref DoS. This is **compile-verified only**.

**REFUTED 2026-08-10 — the "three unchecked `getShopItem()` dereferences" are not
reachable.** The claim was recorded in `326c298`'s commit message and in an earlier
revision of this paragraph; it does not survive checking. There are **four** such
dereferences, not three (`CGShopRequestBuyHandler.cpp:106`, `:194`, `:466`, `:666` —
the fourth was missed), and every one is guarded:
- `:106` is guarded in place — `isExistShopItem()` returns early at `:98`, eleven
  lines above it.
- `:194`, `:466` and `:666` sit in `executeNormal`, `executeMotorcycle` and
  `executeEvent`, whose **only** call sites tree-wide are `:132`, `:130` and `:155`
  inside `execute()` — downstream of the same `isExistShopItem()` guard at `:98`
  (normal/motorcycle) and `:143` (event) [measured: three call sites, no others].

The underlying hazard is real and worth keeping in mind — `ItemRack::get()` returns
`m_ppItem[index]` directly, and a NULL slot is *normal* (NULL at construction,
`remove()` sets NULL), so `getShopItem()` legitimately returns NULL for an in-range
empty index. The guard is simply upstream rather than local, which makes it fragile
to a future caller but not currently a defect. Throwing rather than returning a
sentinel from the accessors remains the right call as defence in depth.

**CONFIRMED — unguarded client-controlled indices. ALL FIXED 2026-08-10.** Verified
independently; none depended on the `Assert` question. This list was written before
`1019602`, `3e02f6c` and the 2026-08-10 hardening wave and read as open long after it
was not — `docs/SECURITY-AUDIT-2026-08-10.md` flagged the staleness as **(D2)**. Kept
below for the reachability analysis, which is still accurate; each entry now carries
its fix. All are **compile-verified only** unless stated — none of these paths was
exercised at runtime.
- `CGReloadFromInventoryHandler.cpp:52-54` — wire `BYTE` X/Y reach
  `Inventory::getInventorySlot` (`Inventory.h:131`, `m_pInventorySlot[X][Y]`,
  no check) with no guard on any path. Inventories are 10×6 and the outer
  dimension is a pointer array, so `X=255` dereferences a wild pointer. Judged
  the most severe: cheapest to reach, no grooming needed.
  **Fixed `1019602`** — real bounds check at the `Inventory` accessor chokepoint.
- `CGUseItemFromGearHandler.cpp:68-74` and `CGAddGearToMouseHandler.cpp:48-94`
  — wire `BYTE` cast to `WearPart` and used directly in
  `m_pWearItem[Part]` (`Slayer.h:473`, `Vampire.h:357`, `Ousters.h:370`);
  arrays are 21/22/22 elements, no bounds check in any of the three races.
  **Fixed `1019602`** — `isWear`/`getWearItem`/`takeOffItem` guarded in all three
  races.
- `CGSkillToInventoryHandler.cpp:61-69` — the `SKILL_INSTALL_MINE` branch sets
  `bSuccess = true` unconditionally, skipping the *only* coordinate check
  (`CreatureUtil.cpp:954`, `X >= 10 || Y >= 6`) and leaving `pSkillSlot` NULL
  on entry to `InstallMine::execute`. **Fixed `3e02f6c`.**
- `CGAddItemToCodeSheetHandler.cpp` — `IndexNum = (y*10 + x)/2` from two wire
  `BYTE`s, guarded only by `if (OptionType.size() < 30) return;`, a lower bound
  on the container rather than an upper bound on the index. Max index ~1402
  against ~30 elements; OOB read at `:104`, OOB write at `:131`.
  **Fixed 2026-08-10** — `x`/`y` are now rejected against the grid at packet entry,
  before any helper runs, so the worst in-grid index is 29. The container is
  **exactly 30**, not "~30": the grid is 10×6 packed two cells per byte and the
  `CodeSheet` ctor pads `m_OptionType` to 30 (`item/CodeSheet.cpp:37-56`)
  [measured]. The magic `10`/`30`/`9`/`5` literals were replaced by named
  constants so the bound and the arithmetic cannot drift apart again.

**PARTIALLY CONFIRMED — `toString()` table lookups.** `readPacket()` calls
`pPacket->toString()` unconditionally right after `read()`
(`SocketInputStream.cpp:181`), on all three servers — confirmed. Seven of nine
`*2String[` lookups inside `toString()` bodies index small tables with
unvalidated wire bytes. But the blanket "pre-auth" framing is **overbroad**: a
packet-ID state machine (`PacketValidator`) gates `readPacket`, and only
`CGConnect`'s `PCType2String[m_PCType]` (3 elements, raw `BYTE`) is genuinely
reachable before authentication. A reviewer's claim that `CLCreatePC::toString()`
is never called was **wrong** — `LoginPlayer.cpp:217` calls `readPacket`, which
calls `toString`. Two of the nine are safe (`CLSelectPC` validates in `read()`;
`CLCreatePC`'s `Sex2String` is bit-width-bounded). **Newly found, unclaimed:**
`CLCreatePC::toString()`'s `HairStyle2String[(bits>>1) & 3]` yields 0..3 into a
3-element table — a genuine off-by-one.

**Confirmed but deliberately not fixed — `Player::setKey` killswitch.**
`src/Core/Player.cpp:236-239`: two magic constants (`0xAEB7`/`0x9B3E`) trigger
`exit(0)`, and `CGConnectSetKey` is registered on **both** login and game
servers with no auth gate, so any client can terminate the process. This is
intentional 2008-era anti-cheat (`// add by viva 2008-12-31`); removing it is a
policy decision for the owner, not a mechanical fix. The uninitialised-read and
leak around it *were* fixed.

**Confirmed, needs a wire-format decision — `CGExchangeBuy`.**
`read()` calls `iStream.read(m_IdempotencyKey)` on a `std::string`. There is no
one-argument `read(string&)` overload, so it binds to the raw template
(`SocketInputStream.h:157`, `buf = *(T*)(m_Buffer + m_Head)`), reinterpreting 32
wire bytes as a live `std::string` — the client controls both the data pointer
and the length, giving an arbitrary-address read, and the ring-buffer-wrap
branch `memcpy`s over the string's internals so the destructor frees an
attacker-chosen pointer. Not
fixed here because `write()` emits the raw bytes with no length prefix, so the
two sides never agreed on a format and the packet cannot have worked; the
client (`dkrix/Client/Packet/Cpackets/CGExchangeBuy.cpp`) also implements it,
so per the house rule a format change must ship to both trees together.

**Resolved 2026-08-10 — the client half landed.** The server format
(`uint64` listing ID, `BYTE` key length capped at 64, key bytes) is now
mirrored in `dkrix/Client/Packet/Cpackets/CGExchangeBuy.{h,cpp}`:
`m_ListingID` became `int64_t`, `m_IdempotencyKey` was added, and both
`getPacketSize()` and `getPacketMaxSize()` (73) agree with the server. The
client has no 64-bit stream overload, so the listing ID uses the raw
`read(char*, uint)` / `write(const char*, uint)` form — byte-identical to
what the server's `read<T>()`/`write<T>()` templates do. Safe to land because
the feature is dead on both sides [measured]: nothing calls
`C_VS_UI_GAME::RunPointExchange()`, so `BuyItem()` is unreachable, and
`GCExchangeBuyFactory`/`GCExchangeListFactory` are registered in *neither*
tree, so no client can parse a reply. Client change is CI-verified only.

**Correction — `CGExchangeBuy` was not "the only such site tree-wide".**
An earlier revision of this paragraph claimed that as [measured]. A re-scan of
`dkrixserver/src/Core/*.cpp` and `shared/Packets/*.cpp` for one-argument
`read()` calls on a `std::string` finds a second live instance:
`GCExchangeBuy::read()` (`dkrixserver/src/Core/GCExchangeBuy.cpp:23`,
`iStream.read(m_Message)`) — the identical template binding, with `write()`
likewise emitting the string with no length prefix. It is latent, not
exploitable: `GCExchangeBuyFactory` is registered nowhere, so no dispatcher
ever calls that `read()`, and the client does not implement `GCExchangeBuy`
at all. Left alone deliberately — fixing it is a second wire-format change,
and the house rule wants that shipped as its own both-trees commit.

- **`CLLoginHandler.cpp:402` is truncated mid-token — pre-existing, dead.**
  [measured] Under `#ifdef __THAILAND_SERVER__`, the line reads
  `bool bChildGuardArea = onChildGuardTimeArea(g_pConfig->getPropertyInt("CHILDGUARD_START_TIME"),g_pConf`
  — unclosed paren, no semicolon. That guard cannot compile. It is **not**
  migration damage: `git log -L 402,402` dates it to `4123ff3` (2026-04-17),
  the initial import of `dkrixserver` as a regular folder, and
  `__THAILAND_SERVER__` is never defined anywhere in the build. Left alone
  deliberately — repairing it means inventing the missing second argument.
  Recorded so the next person who enables that guard knows what they will hit.
  - **Gate implication — and a correction about ASan.** Both 18-A and 18-B are
    Phase 11 `PreparedStatement` defects that no existing gate can see: 18-A
    because SQL is string data, 18-B because the lifetime error is legal C++.
    It is tempting to conclude "add an ASan job to CI", and an earlier revision
    of this file said exactly that — but **a build-only ASan job would not have
    caught either bug.** ASan reports at *runtime*, when the faulting code
    executes; `make debug-asan` compiling cleanly proves nothing about a
    use-after-free on the login path. CI never logs in, so there is nothing to
    instrument. What actually catches this class is *running* the server under
    ASan while exercising the path — i.e. the smoke test, under an ASan build.
    That is cheap to do locally today (`make debug-asan`, then the normal
    `docs/smoke-test/` procedure) and needs no CI work. Making it a CI gate
    additionally requires a seeded MySQL service container and a synthetic
    packet driver to stand in for the client, which is a much larger piece of
    work and should not be confused with "turn on a sanitizer".
  - Relationship to Bug 18-A: both are Phase 11 `PreparedStatement`
    territory and both are invisible to every existing gate for the same
    reason — SQL is string data, so nothing short of running the server sees
    it. 18-A was a malformed literal caught at construction; 18-B is a
    runtime memory fault. Whether they share a cause is unknown.

### Phase 18 — hardening wave (2026-08-10)

Five parallel workstreams against the open items in
`docs/SECURITY-AUDIT-2026-08-10.md` §2. **Everything below is compile-verified
only** (`make debug` in WSL, all three binaries) — no smoke run, no ASan run.
Re-run the ASan smoke test before trusting any of it at runtime.

**Packet-entry rejection.** Three `read()`-side validations, chosen over
patching each use site so the handler files stay untouched:
- `CGConnect` (now `shared/Packets/`, moved by Phase 12 Wave 1 — the audit's
  path is stale) rejects `PCType > PC_OUSTERS`, closing the pre-auth
  `PCType2String[3]` read. `toString()` was *also* bounded independently,
  because `LCReconnectHandler` builds `CGConnect` objects that never pass
  through `read()`. Matches the existing `CLSelectPC.cpp:31-32` idiom.
- `CLCreatePC` rejects both `HairStyle > HAIR_STYLE3` and — the worse defect,
  unnamed by any audit — `Slot >= SLOT_MAX`. `m_Slot` is a raw wire `BYTE`
  (0..255) indexing a **3-element** table, against the hair style's 0..3, and
  the same unvalidated `getSlot()` feeds six sites in `CLCreatePCHandler.cpp`
  and `CLDeletePCHandler.cpp:73-129` where the result is spliced into SQL.
  There is no legitimate 4th hair style: the enum has 3, every DB decoder
  knows 3, and the client picks from a literal `HairStyle hairStyle[3]`
  (`dkrix/Client/UIMessageManager.cpp:1213`) [measured] — so rejection, not
  table widening, is the correct fix.
- `SocketInputStream.cpp:181` deliberately **not** changed: it is the
  reachability path, not the defect. Removing the unconditional
  `toString()` would alter observable logging and leave every other caller
  exposed.

**Packet-entry rejection, sweep for the same shape.** 18-T fixed three
instances; the tree was then swept mechanically for the whole pattern — a wire
integer converted to an enum with no range check, or a `*2String[]` table
indexed by an unvalidated wire field inside `toString()`. The packet layer holds
exactly **six** explicit enum conversions [measured 2026-08-10: `shared/Packets/`
118 `.cpp` + `src/Core/C[GL]*.cpp` 216 `.cpp`]; three were 18-T's, and all three
of the rest were unguarded:

| Site | Enum | Valid | Table | Feeds |
|---|---|---|---|---|
| `shared/Packets/CLDeletePC.cpp:27` | `Slot` | 0–2 (`SLOT_MAX` 3) | `Slot2String[3]` | 6 SQL binds in `CLDeletePCHandler` |
| `shared/Packets/CLRegisterPlayer.cpp:46` | `Sex` | 0–1 | `Sex2String[2]` | `toString()`, SQL bind at handler `:158` |
| `shared/Packets/CLRegisterPlayer.cpp:91` | `Nation` | 0–2 | `Nation2String[3]` | `toString()` |

**`CLDeletePC` is the one that mattered, and 18-T's own commit message points at
it by accident**: it credits the `CLCreatePC` fix with covering
"`CLDeletePCHandler.cpp:73-129`", but that handler takes a **`CLDeletePC`**
packet, whose `read()` was never touched. The six `Slot2String[getSlot()]` SQL
binds at `:73`, `:80`, `:105`, `:111`, `:123`, `:129` were still driven by a raw
0–255 wire `BYTE` against a 3-element table. It is reachable from
`LPS_PC_MANAGEMENT`, i.e. any authenticated account. `CLRegisterPlayer` is the
same shape but currently **unreachable**: `PacketValidator` only admits it from
`LPS_WAITING_FOR_CL_REGISTER_PLAYER`, and the only packet that sets that state
(`CL_QUERY_PLAYER_ID`, `CLQueryPlayerIDHandler:64`) is itself admitted *only*
from that same state — a closed loop, because `PacketValidator.cpp:177-178`
commented both out of `LPS_BEGIN_SESSION`. Fixed anyway: it is UB regardless,
and one uncommented line re-opens it.

Four `toString()` lookups were additionally bounded with the `CGConnect`
`"UNKNOWN"` idiom, since `readPacket()` calls `toString()` on every packet
received and it must not fault however the object was built:
`CLRegisterPlayer` (`Sex2String`, `Nation2String`), `CLSelectPC`
(`PCType2String`; `read()` validates, the lookup did not), and **`CGMove` /
`CGUnburrow`**, where `Dir2String[]` holds **8** entries (`LEFT`..`LEFTUP`) but
`m_Dir` is a raw `Dir_t` (`BYTE`), so 0–255 reaches the lookup. `CGMove` is the
highest-frequency packet in the game.

**Deliberately *not* rejected in `read()`: the two `Dir` fields.** `DIR_NONE`
aliases `DIR_MAX` (`CreatureTypes.h:339`), so 8 is a live sentinel — `calcDirection`
returns it (`Utility.cpp:122`) and `BombMask[5][9][9]` is sized to accept it — and
the client's direction range could not be proved to exclude it. For `CGMove` a
rejection would also *change* behaviour: `Zone::movePC` already treats
`dir >= DIR_MAX` as a recoverable error and answers `GCMoveError`
(`Zone.cpp:2426`), so an out-of-range direction is an anticipated value, not a
protocol violation. **Residual, recorded not fixed:** the `CGUnburrow` path has
no such guard — `addUnburrowCreature` → `Creature::setXYDir` (`Creature.h:234`)
stores `dir` verbatim, so an out-of-range direction persists into creature state
and later reaches `Dir2String[]` via `PCVampireInfo3::toString()`. Bounding the
lookups closes the `readPacket()` fault but not that persistence.

**Incidental, unfixed, needs a wire decision — `CGMove` field order.** The
server's non-encrypted branch reads `Dir, X, Y` (`CGMove.cpp:29-31`) while the
client's writes `X, Y, Dir` (`dkrix/Client/Packet/Cpackets/CGMove.cpp:59-61`);
all three are `BYTE`, so the fields are silently permuted. Both
`SHUFFLE_STATEMENT_3` branches agree on `X, Y, Dir`, so only the
`getEncryptCode() == 0` fallback is affected. The client does send `CGMove`
(`MPlayer.cpp:5641`). Not touched here — it is a wire-semantics change and the
house rule requires both trees together.

**DB object lifetime** (audit §2 rows 6 and 7). 21 legacy blocks across 20
files gained a success-path `SAFE_DELETE(pStmt)`; 8 files' pre-declared
`Result*` were scoped to their statements. The 18-B fix is a *rule* — the
statement owns the `Result` and deletes it in its destructor — not a code
shape; every site here was already the audit's "safe by construction" form and
needed scoping, not hoisting. Hoisting would have moved `PreparedStatement`
constructions out of `else` branches, adding unconditional prepare round-trips.
Corrections to this file's own lists: 4 leak sites were **missing**
(`EffectAcidSwamp.cpp:234`, `EffectProminence.cpp:199`, `EffectOnBridge.cpp:88`,
`ItemMineInfo.cpp:69`), `FlagManager.cpp:355` was missing from the near-miss
list, and `UniqueItemManager.cpp:75` was **mischaracterised** — all four blocks
already free on success; the real defect is a `return` that jumps over the free.
Also fixed: 3 uninitialised `Statement* pStmt;` where `END_DB`'s `delete STMT`
is reachable before assignment. `EventHeadCount.cpp:65` leaks per-PC on a
timer, so this is not purely a startup concern.

Corrected counts [measured 2026-08-10] — the raw grep figure overstates the
remaining Phase 11 work by ~2.8×:

| Metric | Count |
|---|---|
| Raw `executeQuery` text hits | 489 in 200 files |
| Live after stripping comments | 261 in 104 files (1.87× inflation) |
| Live `createStatement()` — the honest measure | **172 in 101 files** |
| Legacy blocks with no success-path free | **0** (was 21) |
| Tree-wide `Result` use-after-free candidates, brace-accurate | **0** |

`MonsterKillQuest::save()`'s `executeQuery("-_-")` is now a documented `throw`
rather than invented SQL. The literal is in the original import, untouched by
any commit; the TU is in no CMake list and its only caller is gated on
`__ACTIVE_QUEST__`, commented out at its sole definition; and `SimpleQuest` rows
are keyed only by `OwnerID`, so any `UPDATE` would have clobbered every quest
the player holds. The throw preserves the observable outcome — nothing persists
— while failing loudly if the flag is ever re-enabled.

**Two changes no compiler will ever see**, carried deliberately and flagged:
`gameserver/GuildManager.cpp`'s `init()` body is inside `#ifdef
__SHARED_SERVER__` but the file is only ever built as `__GAME_SERVER__`
(`server/gameserver/CMakeLists.txt:6,84`) — kept for twin-file parity with the
`sharedserver` copy. `MonsterKillQuest.cpp` is likewise uncompiled. A typo in
either would not surface.

**Secrets.** The remaining 10 tracked `conf/` copies of the stale credential are
untracked (`excel96-*`, `.new`, `conf/backup/*`, `updateserver.conf`); only the
three `.conf.template` files remain. `.gitignore` was widened because the
pre-existing `dkrixserver/conf/*.conf` rule reached only 8 of 10 — `*.conf` does
not match `.new` and does not cross a `/` into `backup/`. `docker-compose.yml`
and `initdb/a-setup.sql` no longer carry literals; both take `${VAR:?}` from
`docker/.env` (template: `docker/.env.example`). Audit §2 row 9 was already
fixed by `2c5ebe9` and its path is wrong (`src/Core/`, not
`src/server/gameserver/`).

Found while there, **a live break nobody had hit**: since `a894d7c` the
`odk-server` container had no `DKRIX_*` variables set at all, so
`Properties::expandEnvVars` left the placeholders literal and the servers would
have failed at first SQL connect. The Docker stack was broken and unrun.

**Still unparameterised, by necessity:** `initdb/DARKEDEN.sql:11568` seeds the
`WorldDBInfo` table with `Host`/`User`/`Password` as *data*, and
`DatabaseManager.cpp:108-134` reads it at startup to open the per-world
connection. No env var reaches it — change the password and the servers connect
to the default DB, then fail on the world connection. Documented in
`.env.example`. The historical throwaway credential also remains inline at ~40
sites across `README.md`, `docker_install.md` and `docs/smoke-test/`; replacing
it is one coordinated pass, not a partial edit.

**The seed dump has never been loadable** [measured 2026-08-10, mysql:5.7 in a
throwaway container]. `initdb/DARKEDEN.sql` fails with `ERROR 1062 Duplicate entry`
in **six** tables — `FlagSet`, `NicknameBook`, `Slang`, `Slayer`, `SpecialEvent`,
`Vampire` — the last two being the player-character tables, both keyed on `Name`.
The offending keys are `''`, `'0-'` and `'?'`, the signature of a charset pass that
collapsed distinct multi-byte keys into ASCII fallbacks; `backup_darkeden_after_
english_20260424.sql` in the tree suggests the English translation pass. A
`mysqldump` of a live table cannot emit duplicate primary keys, so the blanking
happened after the dump was taken.

Verified byte-level that this is real data loss, not a client-side decode problem:
the entire `Slayer` INSERT contains zero non-ASCII bytes, and `Name` is genuinely
empty. Nothing is recoverable. Those seed rows were junk anyway — the first
"Slayer" row carries `Race = 'VAMPIRE'`, and blank-named characters cannot be
logged into.

Fixed by changing exactly those six statements to `INSERT IGNORE` (6 insertions,
6 deletions, no other bytes touched). That preserves every value, makes the dump
loadable, and reproduces precisely what `mysql --force` produced. Deleting the
offending tuples was rejected as more invasive and harder to review inside a
10,000-line file. **This affects the human runbook too, not just CI** — anyone
seeding a database from scratch by following `docs/smoke-test/` would have hit it.
The live database is unaffected; it predates the dump.

Two related fixes in `scripts/ci-boot-smoke.sh`: the game account is now created
explicitly before `a-setup.sql` instead of being auto-created as a side effect of
its `GRANT`. That side effect was real but silent — relaxing `sql_mode` drops
`NO_AUTO_CREATE_USER`, so the `GRANT` created `elcastle@%` **with an empty
password** — and it only worked at all because `a-setup.sql` stopped creating the
account (it carried a credential and the repo is public), which the seed script
had not been told about. Verified end-to-end: seed exits 0, all three host rows
have passwords, and the game user connects to both databases.

**RUNTIME-VERIFIED 2026-08-10, after the wave.** Enrico ran the real client against
the live server on the workstation and completed **login → character creation →
enter world → pick up items → equip → unequip**. All three servers stayed up;
`gameserver.log` and `loginserver.log` contain **zero** exceptions or assertions for
the session, and `assertion_failed.log` was not touched (its only entry predates the
session). This was a plain `make debug` build, **not** ASan.

This promotes several items in this section from *compile-verified only*:

| Now runtime-verified | Path exercised |
|---|---|
| 18-T `CLCreatePC::read()` — Slot and HairStyle rejection | character creation. **`CLCreatePC` had never been exercised in any previous run** (see §1's milestone note), so this closes the single largest untested gap on the login path |
| 18-T `CGConnect::read()` PCType rejection | entering the world |
| 18-Y killswitch removal (`Player::setKey`) | login, on both servers |
| 18-V `CLLoginHandler` `Result` scoping | login |
| 18-U wear-slot accessors, 18-I inventory accessors | equip / unequip / item pickup |
| 18-W, 18-X boot-path loaders | all three servers booted clean |

**Still not exercised at runtime**, and still compile-verified only: the `NPC`
shop-rack accessors (no buy/sell was performed), `CGAddItemToCodeSheet`, the phone
slots, the pet stash, and the whole `Datagram`/`SerialDatagram` UDP path. Nor does
this run touch the restored seed dump — the live database already holds intact zone
data, so `18-Z`/the dump swap remains verified only by the CI boot smoke.

**The ASan smoke test has still not been re-run since the wave.** It is the only
gate in this project's history that has caught a runtime bug, and this clean session
is not a substitute for it.

**Open, deliberately untouched:** the `CGConnectSetKey` → `exit(0)` killswitch
(audit §2 row 1 — an owner policy call, not a mechanical fix) and the three
unchecked `getShopItem()` dereferences noted above. The `CGExchangeBuy`
client/server wire mismatch (row 8) is **closed** — the client half landed
once both trees had a green CI gate; see the `CGExchangeBuy` entry above.

### Phase 18 — waves 2 and 3 (2026-08-10, late)

Two further five-way waves after the first. **Everything here is compile-verified
only** (`make debug`, WSL, all three binaries) unless a row says otherwise. The
security audit (`docs/SECURITY-AUDIT-2026-08-10.md`) was rewritten from the
repository at the end of wave 2 and therefore does **not** yet carry 18-AE, 18-AF
or 18-AG.

| ID | What | Severity |
|---|---|---|
| 18-AA | First runtime UB this tree has ever reported. Misaligned type-punned stores in the packet write path (`SocketOutputStream.h`, every packet the server writes) → `memcpy`; a null `memset` in `PreparedStatement`; and five out-of-bounds writes in `GQuestTouchWayPointElement` that worked only by declaration order | high |
| 18-AB | **Unauthenticated remote admin over UDP.** `GGCommandHandler` dispatched `*shutdown`, `setGold`, `killAllPC`, `*kick`, `*set` with no privilege check, reachable by one datagram on a port-forwarded UDP 9997. Gated on source address in both servers. Plus `opcommand`'s 45 unguarded NULL dereferences, a remote OOB read in `UserInfoManager`, and 4 SQL concatenations parameterised | **critical** |
| 18-AC | The last three unguarded wire-sourced enum conversions, incl. `CLDeletePC` — which 18-T's message wrongly claimed to cover | high |
| 18-AD | 6 mutating calls hoisted out of `Assert()`, one a **live item-duplication bug** (couple rings, unbounded, Release-only) | high |
| 18-AE | Live client-reachable OOB read on the learn-skill path (`SOUL_CHAIN` carve-out lets a wire `BYTE` index a 6-element array); `SkillHandlerManager` read-before-bound-check; two `Zone::movePC` bugs | high |
| 18-AF | The half-wired exchange subsystem now fails closed; its compare-and-swap made real; both packet factories unregistered | medium |
| 18-AG | 47 `Assert`-wrapped XML parse calls hoisted across 18 files, plus the last 6 `(T&)` reference casts | medium |

**The UBSan leg links for the first time in the project's history** (`8bb1118`) —
17 consecutive failures before, and it had therefore never reported a single UB
finding because it died at link time. One flag, `-fno-sanitize=vptr`, whose
diagnosis had been written up and unapplied since 2026-08-09. It now boots servers
rather than only building them.

**Verdicts worth as much as the findings.** Combat is genuinely authoritative: the
client sends intent only, and no damage, hit, crit, EXP or cooldown value exists on
any `CG*` packet in the path (all 216 definitions checked). Movement geometry is
solid — `CGMove` is not a teleport, and `verifyDistance` is applied at ~200 call
sites. The item economy's core paths — player trade, NPC buy/sell, personal stores,
ground pickup, container moves, stack arithmetic, quest reward claims — were all
audited and **cleared**.

**Open, and needing an owner decision rather than a fix:**

1. **A working speed hack.** `GamePlayer::verifySpeed`'s `PACKET_CG_MOVE` branch is
   an empty body; there is no last-move timestamp on `Creature`. The `CGVerifyTime`
   gate meant to catch it is dead by construction — `verifySpeed` runs twice per
   packet, the outer call pushes the deadline 60s ahead so the inner test is always
   false, and the two calls decrement and increment one counter 1:1, so it can never
   reach the disconnect threshold. A correct check needs per-race and per-effect
   movement intervals plus jitter tolerance, i.e. tuning against real play.
   **Recommendation: land a logging-only detector first** and set any threshold from
   measured data.
2. **`ActionGiveLotto.cpp:57`** — an uninitialised `QuestID_t` reaches
   `questRewarded()`, erasing an arbitrary in-progress quest. Live data loss.
3. **`Zone::getTile`'s bounds are `Assert`-only** and become an unchecked 2-D index
   under `make release` — the most load-bearing unguarded accessor in the combat path.
4. Two `grade[0]` reads on a possibly-empty string in the `GQuest*` parsers; the
   `CGMove` field-order divergence between the trees; and the `CGExchangeList`
   desync — the last two both need a coordinated two-tree change.

**A process note worth keeping — and the note itself was wrong the first time.**
Wave 2's commits, including the critical 18-AB, were **not** on `main` until PR #3.
That much is true and worth remembering: work pushed *after* a PR is merged does not
retroactively join it.

An earlier revision of this paragraph explained it by claiming PR #2 was
**squash-merged** and that `git branch --contains` therefore misleads. **That is
false** [measured 2026-08-11]: `669a9fe` has parents `9422e9c` and `d9b6af2`, and
`90a926e` has `669a9fe` and `fc7fae7` — both are ordinary merge commits, every
individual commit is reachable from `main`, and `--contains` is reliable. The claim
came from a subagent report, was relayed here without checking `git log --format=%p`,
and was believed for two waves.

The real lesson is the one that keeps recurring in §3.2 of the security audit: **a
claim about the repository is one command away from being verified, and relaying it
unverified has now been wrong four times in this effort.** `git log origin/main` is
still the right habit for deciding what is deployed — not because containment lies,
but because it answers the question directly.

### Phase 18 — wave 4 (2026-08-11)

Compile-verified only (`make debug`, all three binaries).

| ID | What |
|---|---|
| 18-AH | `Assert`-only accessors hardened (`Zone::getTile`/`getSector`, Slayer domain accessors, `SkillDomainInfoManager`, `VampEXPInfo`, `OustersEXPInfo`, `SkillUtil`). **Two are live OOB reads today, not `NDEBUG`-only** |
| 18-AI | Two uninitialised `QuestID_t`s reaching an unconditional `m_Quests.erase()`; a discarded return value deciding castle-dungeon entry; 3 `Assert`-guarded castle zone IDs |
| 18-AJ | 41 SQL splices parameterised (ratchet **164 → 123**); two amplification vectors closed; measurement-only move-rate telemetry |

**`SkillDomainInfoManager::getDomainInfo` has been out of bounds in the deployed
Debug build all along** [measured against the shipped dump]. `init()` sizes each
domain from its own DB rows, but the guard was the constant `Level < 151`.
Domains 0–4 hold 151 entries, 6–7 hold 150, and **domain 5 (`ETC`) holds 101** —
fifty short. Levelling `ETC` past 100 is an OOB pointer read that the `Assert`
then dereferences, and the destructor was freeing fifty slots past the end.

**`ActionTeachSkill` is reachable by walking into a vampire town.** It dispatches
on the *player's* race rather than the NPC's, and live `Triggers` rows 672 (Kaim)
and 727 (Bricolacas) carry `DomainType : VAMPIRE`, so a Slayer at a vampire
trainer indexed `m_GoalExp[6]` on a six-entry array. This is the case where
hardening the accessor **alone** would have broken legitimate play — a real NPC
conversation would have started throwing. Fixed at both ends.

**A recurring shape worth naming: controls that exist but do nothing.** Four
found so far. The login-failure lockout never increments, because `++nFailed`
lived inside a `cout` that was commented out. The `CGVerifyTime` speed gate is
dead three ways over — a double call that pushes its own deadline 60s ahead, a
counter incremented and decremented 1:1, and a client that no longer sends the
packet. The SQL-injection ratchet reported `0` while 188 sites were invisible to
it. The `sanitizers` matrix was build-only, so it could not see the entire bug
class it appeared to cover. **When a gate here reports "clean", check that it can
report anything else.**

**Why the uninitialised-variable class keeps recurring** [measured]:
`-Wmaybe-uninitialized` cannot fire in any configuration this project builds.
`CMAKE_CXX_FLAGS_DEBUG` is `-g` with **no `-O` flag**, and GCC's analysis is a
dataflow pass that only runs under optimisation; every `make` target and every CI
job is `CMAKE_BUILD_TYPE=Debug`. A real build log shows 31 warnings, none
containing "uninitialized". 18-F, 18-L, 18-M(b), 18-W, 18-AA and 18-AI all share
this root. **Adding `-Og` to the Debug flags, or a warnings-only `-O2` CI leg, is
the cheap systemic fix.**

**Open, owner decisions.** Ranked:

1. **`FD_SETSIZE` overflow — internet-exposed memory corruption.** `fd_set
   m_ReadFDs[2]` uses glibc's 1024-bit default (`FD_SETSIZE` is never defined in
   the tree) but `nMaxPlayers` is 2000 and the accept path only rejects at 2000.
   Descriptors 1024–1999 are accepted and `FD_SET` writes out of bounds into the
   adjacent `m_WriteFDs`. Made cheap by a second bug: the idle timeout lives
   inside `processInput`, which only runs for readable sockets, so a silent
   connection holds its slot forever. **A global concurrent cap below 1024 closes
   it outright** and is config-shaped, not a tuning judgement.
2. **UDP denial of service needing no bug.** The receive loop `usleep`s twice per
   iteration and handles one datagram per iteration — ~500/s ceiling, so ~144
   kbit/s of spoofed traffic saturates it, and the dropped datagrams include the
   `GL`/`LG` login handoff. Nobody can log in.
3. **The speed hack.** Measurement now exists; enforcement needs movement-speed
   data the server does not have.
4. **The login-failure lockout** — one word (`nFailed++`) at four sites, but it
   changes who gets disconnected during real play.
5. `ActionGiveEventItem` has no Ousters branch but dereferences unconditionally —
   a live gameserver segfault if wired to an NPC an Ousters can reach.
6. `FameLimitInfo` is the closest untested twin of the `getDomainInfo` defect.

### Phase 18 — wave 5 (2026-08-11)

Compile-verified only (`make debug`, all three binaries).

| ID | What |
|---|---|
| 18-AK | All three accept paths refuse descriptors past the `fd_set` bound instead of writing past it. The **sharedserver was worse than the reported bug** — no check at all against a 100-entry array |
| 18-AL | Four more constant-guard-vs-DB-sized-array defects, incl. `EventQuestAdvance`, which was live and leaking every instance ever allocated |
| 18-AM | Four Ousters NULL dereferences that **crash the process**; two are live in the shipped trigger data |
| — | A CI leg that can finally emit optimisation-dependent warnings, plus a `paths:` blind spot that skipped CI entirely for `shared/Packets/**` |

**`FameLimitInfo` — named as the highest-value follow-up — is dead code.** Not in
any CMake list (0 hits, versus 20 each for its live siblings), all wiring
commented out, zero call sites, and the table exists in neither dump. Left
untouched deliberately: editing an uncompiled, unverifiable file only signals to
the next auditor that it matters. **Recommend deleting `FameLimitInfo.{h,cpp}`.**

**Two claims from earlier waves were wrong and are corrected here.** The idle
timeout *does* fire on a silent connection — the check lives in `processCommand`,
driven by an **ungated** `processCommands()` loop with no `FD_ISSET` filter, and
the deadline is armed in the constructor. And `RareOptionUpgradeInfo` is **not**
dead: it is live on the rare-enchant path, loaded at startup from a table whose
absence throws. Both were relayed as fact before being checked.

**`reserve()` where `resize()` was meant is now a named pattern in this tree** —
three instances found (`RareOptionUpgradeInfo`, `EventQuestAdvance`,
`PetTypeInfo`). It is particularly nasty because `reserve()` allocates capacity,
so the writes often land inside the allocation and the code appears to work,
while `size()` stays 0 — so every range-based loop, `clear()` and iteration sees
an empty container. `EventQuestAdvance` leaked every object it ever allocated for
exactly this reason.

**Measured negatives, recorded so they are not re-swept:** `SkillParentInfo`,
`RankEXPInfo` and `AttrBalanceInfo` already bound on loaded counts; `MonsterAI`'s
input is validated upstream; `War.cpp`'s status is an SQL `enum` and cannot
drift; `SweeperSet`'s race column has one distinct value.

**Raising `FD_SETSIZE` does not work on glibc** — `fd_set` is sized by
`__FD_SETSIZE`, fixed at 1024 in `bits/typesizes.h`, and defining `FD_SETSIZE`
first silently does nothing. It works that way on Winsock, which is probably why
the current state looks deliberate. Not compile-confirmed; worth a two-line test
before anyone acts on it.

**Still open, owner decisions** (unchanged from wave 4 except where noted):

1. **UDP DoS needing no bug** — ~144 kbit/s of spoofed traffic saturates the
   receive loop and the dropped datagrams include the login handoff.
2. **The speed hack** — telemetry now lands in `MoveRate.log`; enforcement needs
   movement-speed data the server does not have.
3. **The login-failure lockout** — one word at four sites.
4. **Lower `nMaxPlayers` 2000 → 1024** so the header stops reading as a capacity
   promise. Deliberately not bundled with a security fix; it is a shared header
   and a wide recompile.
5. `CGSilverCoatingHandler:39` — no Ousters arm, reachable only by a crafted
   packet.
6. Delete `FameLimitInfo.{h,cpp}`.

### Phase 18 — wave 6 (2026-08-11)

Compile-verified only (`make debug`, all three binaries).

| ID | What |
|---|---|
| 18-AN | `[[noreturn]]` on both assert helpers; six wild `delete`s; `LoginPlayer`'s always-uninitialised `lastSlot` on the normal re-login path |
| 18-AO | A parser that validated a length byte **before reading it**; 41 format-string sites; a self-aliasing `sprintf`; an `m_HotKey[8]` off-by-one |
| 18-AP | A **remote sharedserver crash**, a **stack overflow** on guild-master transfer, UB reachable in every build config, and unbounded remote memory growth |

**The `-O2` warning-scan leg paid for itself on its first run.** 198 findings in
the target family, 727 total — but 83% is `-Wnull-dereference` and ~150 of those
are **one copy-pasted idiom**: an unchecked `dynamic_cast` dereferenced
immediately. It reads as 164 problems only because the deref lands inside a
trivial inline getter, so GCC reports the *callee's* header line. One file
contributes 18 findings from 17 byte-identical copies of the same two lines.
**The tree has 3,722 `dynamic_cast<T*>` sites** — its entire type dispatch is
unchecked downcasting. A `checked_cast<T>()` helper would retire ~75% of the
family in one reviewable change, and is the highest-leverage follow-up available.

**`Assert1.h` reuses `Assert.h`'s include guard.** 101 files include one, 256 the
other, and only one is live per translation unit. Any tree-wide change to the
assert machinery must touch both or it silently misses 28% of the codebase.

**Do not ratchet the warning count yet**, for a reason worth recording: because
findings key to inlined-callee header lines, the number moves with GCC's
*inlining decisions*. An unrelated refactor shifts it; a genuinely new
null-deref in an already-warned header shifts it by zero. It would fire on noise
and stay silent on regressions. Pin the compiler version first, land
`[[noreturn]]`, then split the counter — `-Wnull-dereference` on its own versus
everything else (~34 findings, mostly real, zero a credible destination).

**The scan is structurally blind to Bug 18-B's class.** No `use-after-free` or
`mismatched-new-delete` findings, but all four such diagnostics are
intraprocedural: without `-flto` GCC cannot see an allocation in one TU freed in
another. Read the zeros as "not looked for", not "not present".

**Corrections landed this wave**, all verified rather than relayed:

- **PR #2 was *not* squash-merged.** Both PRs are ordinary merges; every commit is
  reachable from `main`; `--contains` is reliable. The earlier note here was
  wrong — see the process note above.
- `Core/Assert.h` is **not** shared with the client; `dkrix` has its own copy.
- The Assert fall-throughs are unreachable in **Debug** (what ships) and real
  only under `make release`. The earlier framing was inverted.
- `CLLoginHandler`'s pay-type finding was **overstated** — the unset path exists
  but returns three lines before the consumer.
- The format-string class was **41 sites**, not "4+".
- **`EventBallInfo` is not a live NULL deref** (18-AL said it was) — it is
  uncompiled dead code, like `FameLimitInfo`. Recommend deleting both pairs.

**A fourth `reserve()`-for-`resize()`, live and open:** `OptionInfo.cpp:433`
reserves where it must resize; `OptionInfo.h:541` then does indexed assignment
into a size-0 vector; `:1016` reads indeterminate memory that the `if (pOCI ==
NULL)` on the next line **cannot detect**; and the destructor leaks every
`OptionClassInfo`. Reachable from `CGAddItemToItemHandler` at three sites.

**Open, owner decisions** — added this wave:

7. **A submaster can promote themselves to guild master**
   (`GSModifyGuildMemberHandler.cpp:56-58`) — the 18-I precedence shape again.
8. **The missing `return` in `CLLoginHandler`'s fall-through** — wire-observable,
   since that path currently emits *two* `LCLoginError` packets.
9. Whether to correct the two always-NULL `dynamic_cast`s that make
   `CGQuitGuild` and `CGExpelGuildMember` no-ops — doing so resurrects dormant
   guild operations.

### Phase 18 — wave 7 (2026-08-11)

**18-AQ — `checkedCast`.** The follow-up wave 6 named as highest-leverage,
landed. `Core/CheckedCast.{h,cpp}`: mirrors `dynamic_cast`'s spelling so a
conversion is a pure token rename, with an out-of-line `[[noreturn]]` cold path
so GCC can prove the result non-NULL downstream — that is the mechanism that
retires the `-Wnull-dereference` findings, the same trick `Assert.h` uses.

**What it throws was the hard part, and both obvious answers are traps.**
`__END_DEBUG_EX` rethrows `Error` and `ProtocolException` but **swallows every
other `Exception`** into `packet_exception.txt` and lets the handler return
normally — and **456 files under `Core/` close their handlers with that
macro**. So `NoSuchElementException` or `InvalidArgumentException` would have
turned a failed cast into *a packet that quietly appears to have succeeded*.
`InvalidProtocolException` is also wrong: `GamePlayer::sendPacket()` silently
swallows it, and a failed downcast is a server-side type-invariant violation,
not malformed client input. `InvalidCastError` therefore derives from
`AssertionError`, hence `Error`.

**A conversion rule worth keeping:** any handler invoked *outside* the packet
boundary cannot take a throwing cast. `CGLogoutHandler` is called directly by
`ZonePlayerManager`, outside `pPacket->execute()`'s `catch (...)`, so a throw
would escape into the zone loop. `CGSayHandler` catches `Error` **by value**,
which would swallow it. Both excluded.

**The remaining work is now quantified** [measured, function-scoped,
deliberately conservative] over 3,645 sites in `dkrixserver/src`:

| Category | Sites | % |
|---|---:|---:|
| **Unchecked and dereferenced** | **1,930** | 52.9% |
| Guarded by `Assert(pX != NULL)` | 1,039 | 28.5% |
| Deliberately NULL-tested | 485 | 13.3% |
| Used inline, not bound | 174 | 4.8% |
| Bound but never dereferenced | 17 | 0.5% |

250 converted (13% of the unchecked set). The rest is led by `Zone.cpp` (84),
`skill/SkillUtil.cpp` (52), `CreatureUtil.cpp` (36) — **none of them packet
handlers**, so the packet-boundary catch argument must be re-established per
call path before converting them. The 1,039 `Assert`-guarded sites are a
separate later question: correct in Debug, gone under `make release`.

**Measured effect of `[[noreturn]]` (18-AN):** the `server-warning-scan`
artifact fell from **82,374 to 66,224 bytes** between runs `31440542647` and
`31444606202`. Directional only — the artifact covers all warnings, not just
the family.

**MEASURED EFFECT OF `checkedCast` — and it refutes the claim that prompted it.**
Run `31447699566`, immediately after the 250 conversions: **66,224 → 65,695
bytes.** A 529-byte drop. Essentially nothing, against a predicted 40–90
findings retired.

The reason is the dedup mechanism, reasoned through properly only after the
measurement: **findings key to the inlined-callee header line.** A warning on
`Inventory.h:96` disappears only when the **last** unchecked caller of that
getter is converted. Converting 250 of 1,930 sites (13%) removes almost no
header lines, because nearly every one still has other unconverted callers.

So the wave-6 note calling this "the highest-leverage follow-up — retires ~75%
of the family in one reviewable change" is **wrong as stated**, and the
correct framing is close to its inverse: the remaining ~1,680 conversions are
not 87% of the work for 87% of the benefit, they are **nearly all of the
benefit**, because the metric only moves on the last caller of each getter.
Partial conversion of this idiom is worth very little *as measured*, however
much it improves the code.

Two things follow. **Artifact size is a poor proxy** and should not be quoted
as one again — the earlier 82K→66K drop was real but for a different reason
(`[[noreturn]]` affects whole diagnostic classes, not individual call sites).
And **the correctness value of the 250 conversions stands regardless**: one
turns a live remote gameserver kill into a dropped connection, and the
`Assert`-adjacent sites now fail identically in Debug and Release. It is the
warning-count claim that was wrong, not the change.

### Phase 18 — wave 8 (2026-08-11, committed 2026-08-13)

Four commits, `4fb9591`…`6e7cc2b`. Written 2026-08-11 by the session that ran
concurrently with wave 7 — the audit's §8 flagged them as uncommitted working-
tree changes at the time — reviewed, build-verified and committed 2026-08-13.

**18-AR — `reserve()`-for-`resize()`, instances 5 and 6.** Closes the audit's
§1 row 4 and open action 12. `OptionInfoManager::m_OptionClassInfos` was the
one the wave-6 rewrite found and left unfixed, and it is live on the
rare-enchant path: `getRareUpgradeRatio()` is reached from
`CGAddItemToItemHandler` at three sites, its read returned indeterminate memory
that the `if (pOCI == NULL)` guard on the next line could not detect, and
`release()`'s `begin()`…`end()` walk over an empty range leaked every
`OptionClassInfo` ever loaded.

`SkillPropertyManager::m_SkillProperties` was found while fixing the first and
is the more direct bug of the two: `init()` NULL-filled `[0, SKILL_MAX)`
through `operator[]` on an unsized vector, so **the fill itself was the
out-of-bounds write** — not merely a later read of uninitialised storage.

Both become `assign(N, NULL)`. Loop bounds in `clear()`/`toString()` now come
from the vector's own `size()`, because both run from the destructor and are
reachable without `init()` having sized anything.

**The pattern's count is now 6 and it has not stopped recurring** — five of the
six were found after the pattern was named. That is the argument for a grep
gate over `reserve(` in a constructor or `init()`, which does not exist yet.

**18-AS — a two-byte stack overflow in `getDBString`.** `CGModifyNicknameHandler`'s
free function escaped into a 100-byte stack array and checked its bound *after*
both writes, so a 100-character input wrote `ret[100]` and then the terminator
at `ret[101]`, the first of the two bytes taken verbatim from the input. Escaped
characters consume two slots per iteration, so 50 quotes reached the same place.
Rewritten over `std::string` — the shape `SMSServiceThread`'s identically-named
member function already used correctly. It has **no callers left** (the tree
binds these values as parameters now; two stale `extern` declarations remain in
`PetItem.cpp` and `NicknameBook.cpp`), which is why it survived this long.

**18-AT — uninitialised `PetInfo` fields and three NULL derefs.**
`PetInfo::PetInfo()` assigned `m_PetLevel = 0` **twice** and left
`m_PetCreatureType`, `m_PetAttrLevel`, `m_PetFoodType` and `m_pPetItem`
indeterminate. The duplicated line is what hid it — it reads as a complete
initialiser list. `write()` emits three of those fields onto the wire and
`getItemObjectID()` branches on the indeterminate pointer before dereferencing
it. Add to §4's patterns: **a duplicated assignment in a constructor is a
reliable tell for a missing one.**

The pet-stash handlers both dereferenced `getPetItem()->getPetInfo()`
unguarded; `CGDepositPetHandler`'s very next statement already tested it for
NULL, so the deref simply ran three lines ahead of its own check.
`CGThrowBombHandler` dereferenced `hasSkill(SKILL_THROW_BOMB)`'s NULL return —
**not** absorbed by the enclosing `catch`, because a NULL dereference is a
SIGSEGV rather than a Throwable, so the gameserver died rather than logged.

**18-AQ cont. — 24 further `checkedCast` conversions.** 274 of 1,930 now
converted (14.2%). Per wave 7's measurement, expect no movement in the warning
artifact from this: the metric only moves on the **last** unconverted caller of
each inlined getter.

**Verification: compile-verified only.** `make debug` in WSL, all three
binaries, plus `git-clang-format` clean on the changed lines. Nothing here has
been executed, and the ASan gap called out in the audit's §5 is unchanged —
still nothing since wave 1.

**Merged to `main` 2026-08-13** as `e86a9dd`, together with the three doc
commits that trailed wave 7. That closes Phase 18: every one of the 46
numbered defects is on the line of record, and for the first time since
2026-08-10 the branch is not ahead of `main`. The audit's §8 "not on `main`"
row is empty.

**What is emphatically *not* closed is verification.** Waves 2 through 8 —
roughly 30 defects — are compile-verified and nothing more. The ASan smoke
test, the only gate in this project’s history that has ever caught a runtime
bug, has not run since wave 1. Merging did not change that; it only removed
the reason it was being deferred.

## Explicit non-goals

The following are deliberately out of scope for this modernization
pass. If we change our minds, update this list first.

- Porting to a newer rendering API (Vulkan, Metal, wgpu). SDL2 is good
  enough for this codebase, and the effort is not justified.
- Replacing the custom engine with a general-purpose engine (Unity,
  Godot, etc.). Gameplay logic and wire protocol are too coupled to
  the existing structures.
- Changing observable gameplay behavior — damage math, drop tables,
  skill effects, packet semantics. Treat these as load-bearing until
  a human owner explicitly agrees otherwise.
- Full C++17/20/23 adoption. Server pins C++11; client matches.
  Opportunistic use of later-standard features only where the toolchain
  cleanly supports it.
- Rewriting the UI framework. `VS_UI` is ugly but it works; the text
  path is the only urgent fix.

## How to use this file

- When starting a phase, change its "not started" bullet to a link to
  the PR or branch that's implementing it.
- When finishing a step, check the box the same commit it lands in.
- When you discover a new issue, add it to the appropriate phase (or
  a new phase) in the same commit where you find it. Do not file it
  into a side document.
- When making a decision in a phase (e.g. which sprite system wins in
  Phase 4), write the decision into this file in the PR that commits
  to it.

## Delegation model — what can run without supervision

Added 2026-08-06 to answer directly: *how much of this can be handed off?*

The constraint is not judgment, it is **verification**. An agent session has
no cmake, no MSVC, no SDL2, no libmysqlclient, and no xerces-c, and cannot
reach the live MySQL instance. It can read and edit every file in the repo
and cannot compile any of them. **Two things changed on 2026-08-06/07:**
CI is green on both trees, and sessions running on the workstation have
WSL with the full server toolchain (`make debug` verified Phases 7 and 9
there before merge). Verification is no longer the bottleneck; review is.

**Safe to delegate now** (no build required to judge correctness):
- Documentation, this plan, changelog prose, release notes.
- Read-only analysis: dead-code inventories, grep-derived counts, config
  audits, "where is X used" questions, packet-definition drift between
  `dkrix` and `dkrixserver`.
- Writing the CI workflow files for Phase -1 (they are judged by CI itself).
- Drafting a phase's decision record before code is touched — e.g. the
  Phase 4 sprite-system choice.

**Delegate only with a build gate in place** (Phase -1 done):
- Phase 1 deletions. Mechanical, but "provably dead" is proven by the
  compiler, not by grep.
- Phases 2, 3, 6, 7, 10.

**Do not delegate** (behavioral or destructive, needs a human):
- Anything touching the live `DARKEDEN` / `USERINFO` databases.
- Phase 5's glyph-coverage check — requires eyes on a running client.
- Phase 4 sprite consolidation — it changes rendering, and past regressions
  in this area were resolution-dependent and invisible to a compiler.
- Phase 8's secret migration and Phase 9's Lua sandboxing — both change
  runtime behavior of a live server.
- Any release: `PUBLISH_RELEASE.cmd` writes into `Darkeden` and
  ships to real testers.

**The honest summary (updated 2026-08-07):** Phase -1 is done and the gate
is real — the 08-06/07 wave was produced this way. Delegable with a CI gate
now: Phase 2's two deferred items, Phase 3 items 2–3, Phase 4b, Phase 10,
Phases 11.1, 16, and 17, and Phase 12's file reconciliation. Still human-
only: the do-not-delegate list above, unchanged — plus confirming CI on the
merged tip and the Phase 18 smoke run, which only the workstation can do.

## Living index of superseded docs

These docs are historical. They may contradict each other and this
file; trust this file. **[measured 2026-08-06] All ten have been archived
to `dkrix/docs/archive/2026-migration-notes/`** — the paths below are their
original locations, kept for provenance.

- `client/CLEANUP_SUMMARY.md`
- `client/GDI_USAGE_ANALYSIS.md`
- `client/PLATFORM_MIGRATION.md`
- `client/SDL_MIGRATION_CLEANUP_SUMMARY.md`
- `client/TEXT_MIGRATION.md`
- `client/TEXT_MIGRATION_SUMMARY.md`
- `client/WINDOWS_NATIVE_CLEANUP_SUMMARY.md`
- `client/plan.md`
- `client/spec.md`
- `client/tasks.md`

Phase 0 moved these into `dkrix/docs/archive/2026-migration-notes/` so they
stop competing with the current plan. Done.

Not superseded, listed here only so it is findable: `docs/phase12-residual-triage-2026-08-09.md` — per-pair triage of Phase 12's 48-pair `>5`-residual set (45 mechanical / 3 needs-decision / 0 blocked, with a recommended batch order).

## Windows Build Direction

For current client work on this workstation, the primary smoke test is a native
Windows build:

```powershell
cmake -S dkrix -B dkrix\build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build dkrix\build --config Debug
```

Treat older macOS `make debug-asan` references as historical migration notes
unless the branch explicitly says otherwise.

## Client Launcher / Update Direction

> **[measured 2026-08-06] Superseded.** The tester-facing pipeline now lives
> at the workspace root — `PUBLISH_RELEASE.cmd` → `MAKE_DARKEDEN_RELEASE.cmd`
> → `START_DARKEDEN_UPDATE_SERVER.cmd`, backed by `tools/release/*.ps1`,
> publishing to `release_site/darkeden/` and served on port 8080. Testers run
> `Darkeden/DarkEdenLauncher.cmd`. See `../CLAUDE.md`. The
> `dkrix/tools/launcher/` scripts below still work and remain useful for
> quick LAN tests from the client repo, but they are no longer the shipping
> path.

The first tester-friendly launcher pipeline lives in
`dkrix/tools/launcher/`.

- `package_client.ps1` copies `DarkEden.exe`, runtime DLLs, and optional data
  folders into `publish/client/files/`, then writes a SHA-256
  `manifest.json`.
- `DarkEdenLauncher.ps1` downloads the manifest from a static HTTP endpoint,
  verifies file hashes, downloads only changed files, and launches
  `DarkEden.exe`.
- `serve_publish.ps1` is a local PowerShell HTTP helper for LAN testing; a real
  public test setup can use any static HTTP host.

This is intentionally script-first. A later GUI launcher should keep the same
manifest format so the update protocol does not need to change.

Near-term prerequisites before deeper cleanup phases:
- Remove POSIX-only headers from Windows client paths (`pthread.h`,
  `sys/time.h`, and similar).
- Make repo-local compatibility headers yield to the real Windows SDK instead
  of redefining SDK-owned Win32 types.
- Keep documentation aligned so Windows remains the primary client validation
  surface for this repo.
