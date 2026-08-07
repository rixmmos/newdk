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
- `conf/gameserver.conf` contains plaintext `DB_PASSWORD: elca110`,
  dev IPs (`192.168.0.16`), and double-encoded Chinese comments from
  a prior encoding migration.

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

### Current next steps (audit 2026-08-07)

In order; each independently shippable:

1. ~~**CI on the merged tip — one tail open.**~~ — **fully resolved
   2026-08-07.** Client run #8 (tip `5ca240a`): SUCCESS, 28m30s — the
   client wave compiles. Server run #6 (same tip): `make debug` green in
   21m32s; only the `clang-format` job was red, fixed by the fmt pass.
   Run #9 (`8f4ca50`): `make debug` green again; fmt red on
   formatter-version skew, fixed by pinning `clang-format==18.1.8` in
   `server.yml`. **The "now deterministic" call here was wrong**: the
   next push (`19f41c4`, run #12) went RED on a static-link-order bug in
   the three server executables (`*Database` before `*Packets`/`Core`),
   latent since `PreparedStatement` was introduced and surfaced by 11.2
   batch 1's first call sites. Fixed in `ff96e46`; run #13 then surfaced
   a second, opposite-direction ordering bug in `sharedserver`
   (`Database` linked after `ServerCore`), fixed properly in `421088e`
   via `target_link_libraries(Database PUBLIC ServerCore)` rather than
   more manual list-tuning. **Run #14 (`421088e`) — GREEN**, 21m39s, all
   three jobs. Full detail in the verification-status box above. Item 2
   below is now genuinely the top item.
2. **Phase 18 — run the smoke test against `main`** (`docs/smoke-test/`,
   filling PORTING-NOTE's verification table as you go). Workstation + WSL
   + MySQL. Fold Phase 5's Korean/Chinese glyph check into the same session
   — it needs eyes on a running client anyway.
3. **Phase 8 SQL half, via Phase 11.** ~~Lift the parked `PreparedStatement`
   design.~~ **11.1 landed 2026-08-07** (API only, zero call sites; compile
   gate = next server CI run). 11.2 batch 1 done (542→529); **batch 2
   done 2026-08-07** (529→510, guild-membership subsystem). Ongoing —
   ~510 sites remain, ratchet-driven.
4. ~~**Phase 3 items 2–3, then 4b/4c.**~~ **Items 2–3 done 2026-08-07**
   (see Phase 3 above). 4b/4c remain and are explicitly **not
   delegable** — Phase 4 sprite consolidation changes rendering with no
   automated test coverage; needs eyes on a running client (folds into
   item 2's Phase 18 session).
5. ~~**Phase 10 (+ parked 14/15).**~~ **All three bullets done
   2026-08-07**: `.clang-format` + fmt infra (bullet 1, census only, no
   source reformatted), explicit source lists / `CONFIGURE_DEPENDS`
   (bullet 2), `USE_ASAN/TSAN/UBSAN` + CI sanitizer matrix (bullet 3).
   See Phase 10 above for detail and compile-gate caveats.
6. **Phase 8 secrets step 2** — deployment change; needs a live-server
   window, config backups, and Enrico at the wheel.
7. **Phase 12.1 — build `shared/Packets/` infrastructure + one pilot
   pair.** Decided 2026-08-08 (see Phase 12 above): the simple "point
   client at the server file" approach is structurally broken
   (include-resolution pulls in the wrong `Packet.h`); building
   `shared/Packets/` — proven on the parked line — is approved as its
   own scoped, CI-gated change, sized against `main`'s current tree.
   Land the directory + `shared_packets` CMake target + exactly one
   migrated pair first; both-tree CI green on the pilot gates resuming
   the batch plan (Wave 1 proper, ~20 pairs at a time).

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
      Residual CRITICAL_SECTION users: DebugLog.cpp (live, 12 sites),
      comments in DebugLog.h:11 / GameInitInfo.cpp:11 (latter now
      stale), Packet/Exception.h's __ENTER/__LEAVE_CRITICAL_SECTION
      macros (name collision — they wrap a passed-in mutex object). The
      Platform.h CRITICAL_SECTION shim stays for DebugLog.cpp.
- [ ] GDI stubs — skipped entirely per instructions (Phase 5 territory,
      `LOGFONT` is now a live parameter type in `Base::SetFont`).
- Target: `Platform.h` shrinks to under 600 lines.
      **[measured 2026-08-06] 1,995 lines** (was 1,996 at session start;
      1,968 baseline). The two safely-completed Platform.h items nearly
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
- [ ] Wire `engine/sprite`'s 11 test files (3,898 lines) into CI — the only
      automated tests in this repo.
- [ ] **4c** — audit shipped SPK assets in `Darkeden/` for the pixel
      encodings actually in use. That evidence, not the class count,
      decides the fate of the 555/565 serializers.
- [ ] `CAlphaSprite::Blt4444*` methods — re-check against asset evidence.

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

- [x] In `VS_UI/src/VS_UI_Base.cpp`, remove the `#ifdef
      PLATFORM_WINDOWS` GDI branch and route Windows through
      `TextSystem` like every other platform.
- [ ] Verify Korean and Chinese glyph coverage in the fallback fonts.
      **Not verified. This is the risk item created by shipping the switch
      early** — the client renders Korean and Chinese game data, and a
      missing glyph range would show as blank or box characters rather
      than a crash. Needs a human looking at a running client, ideally at
      both QHD and FHD.
- [ ] Delete the GDI stubs in `Platform.h` (previously deferred from
      Phase 2). **Still blocked, for a new reason:** `Base::SetFont` keeps
      `LOGFONT &lf` in its signature and reads `lf.lfFaceName` /
      `lf.lfHeight`. Removing the stub requires introducing a small
      `TextSystem` font-spec struct and updating every caller — a real
      refactor, not a deletion. Size it before scheduling.

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

### Phase 6 — Modern C++ as we touch it (ongoing)
- Rule of thumb when a file is already being modified for another
  reason:
  - Prefer `std::unique_ptr` / RAII over raw `new`/`delete`.
  - Use `<thread>`, `<mutex>`, `<condition_variable>` for new
    concurrency.
  - Use `<cstdint>` types (`std::uint32_t`, etc.) for new code; do
    not mass-rename existing `DWORD`/`BYTE`/`BOOL` usage.
  - Translate Korean/Chinese comments to English.

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
      `dkrix/` (the server's own `fmt-check` has that latent bug; its
      CI job is unaffected). [measured] Census at pinned 18.1.8:
      **2,199 of 2,227** in-scope files unformatted, ~946k diff lines;
      0 clang-format errors, 0 non-UTF-8 files, 0 in-repo CRLF. No
      source reformatted; no CI gate wired yet — proposed job +
      sequencing in the stream-② manifest
      (`_incoming/phase10/MANIFEST.md`).
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
      **Compile gate: next CI run on `main` [unverified]** — no
      compiler in this sandbox to check `-fsanitize=...` actually
      links clean here.
- [x] Both trees: `.gitignore` for `build/`, `compile_commands.json`,
      editor detritus — **already done on this line** (Phase -1/Phase 1
      passes, verified with `git check-ignore -v`).

### Phases 11–18 — booked 2026-08-07 from the parked line (none started here)

The parked line continued past this plan's original horizon. Its phase
numbers are kept so `git log archive/modernization-phases-1-17 --oneline
--grep "Phase N"` keeps working. For each: read what the tag did first,
lift the approach one phase at a time against a build — never as a merge.

### Phase 11 — SQL injection remediation (11.1 lifted onto main 2026-08-07; 11.2 ongoing)
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
(execute-time errors unchanged). **Compile-verified 2026-08-07**: CI run
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

### Phase 12 — Packet schema unification (12.1 scaffolding + pilot landed here 2026-08-08; Wave 1 batches not started)
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
  `Statement` calls carrying a `%[sdluxc]` format spec. **Baseline 542.**
  (Parked line: 567 at introduction, 540 at its tip — after Phase 11
  migrations `main` never ran.) The 598 in Ground truth is a narrower
  `executeQuery`-only grep; both are recorded, and the script's own
  count is what the gate enforces.
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

### Phase 18 — End-to-end runtime smoke test against `main` (not run)
The validation gate for the whole 08-06/07 wave, and the top open item
after CI-on-tip. The runbook is already lifted to `docs/smoke-test/`
(2026-08-06, paths derived at runtime); **nothing in it has been run
against `main`**. Its output is a runtime-bug list — the parked run's
Phase 18 kept finding bugs (PP, QQ, …) in a tree where phases 1–17 all
compiled clean, which is exactly why a green build does not close this.
Fold Phase 5's glyph check into the same session. Workstation only.

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
