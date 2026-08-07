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

**No claim in this document has been confirmed by a compile** — except what
run #6 (client) and the server's green run (above) actually verified: this
one commit, on this one run, on `main`. Everything else below is still either
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
- `Client/DXLib/` still carries a double layer: `CDirect{Input,Sound,
  Music,Draw}*` plus matching `_Adapter.cpp` files that wrap the real
  `CSDL*` implementations. The shim is vestigial; callers already bypass
  it in several places.
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
- **[measured] The client build is still held together by
  `file(GLOB …) + FILTER EXCLUDE`** — 34 `FILTER`/`REMOVE_ITEM` lines remain
  in `CMakeLists.txt`. All nine duplicate-symbol `.cpp` files still exist
  (`GlobalVariables`, `MissingGlobals`, `GameHelpers`, `GameFunctions`,
  `GamePacketFunctions`, `ActionFunctions`, `MitemTableinit2`,
  `GCNotifyWinHandler`, `GCNotifyWin`), as do all 35
  `Client/Packet/Cpackets/*Handler.cpp` files.
- **[measured] `MItemTable_bak-2007-5-7.cpp` is gone** (14,965 lines). This
  is the one Phase 1 item that has actually landed.
- **[measured] `Client/WinLib/` still exists** — `CWinUpdate.cpp`,
  `CWinUpdate.h`, `WinLib.h`.
- **[measured] `Client/DXLib/` still holds 46 files**, including the full
  `CDirect{Draw,Input,Music,Sound}` set, their `_Adapter.cpp` twins,
  `CDirectDraw_StaticMembers.cpp`, `CDirectSetupGetVersion.cpp`, and
  `BIT_RES.*`. `Client/Platform/` does not exist yet.
- **[measured] `Client/SpriteLib/` still holds 20 pixel-format variant
  files** (the `555` / `565` / `4444` class explosion), untouched — despite
  the current branch being named `modernize/phase-4-sprite`.
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
  - **[measured] The "~87 `sprintf(query, …)` sites" figure does not
    reproduce.** Current counts: 26 matches for `sprintf(query`, and 425
    `sprintf` calls overall under `src/server/`. Whichever measurement
    produced 87 is unrecorded. Re-derive the real number and write the
    exact grep used into this file before starting Phase 8 — sizing that
    phase off an unreproducible figure is how it stalls.
- Lua 5.1 / LuaJIT is embedded via the C API. `luaL_openlibs()` is
  called, so quest scripts can `io.open` the filesystem. Enum values
  are pulled via `lua_tonumber` with no range check.
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

> **Status reality check (2026-08-06).** The phase order below has not been
> followed. Phase 5 (text) landed first; Phase 1 is ~1/5 done; Phases 2, 3,
> 4 are untouched despite the active branch being named for Phase 4; and
> Phase 2's target file *grew*. Treat the ordering as intent, not history.

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

### Phase 1 — Delete the unambiguously dead (client, done except build verification)
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
  `_bak` file from the prior pass). **Not build-verified** — no compiler
  available to this session (client needs Windows+VS2022+vcpkg). This is
  confidence by grep-based inspection of every deleted symbol's call sites,
  per the delegation model below, not confidence by green build. The next
  native Windows build of `dkrix` is the actual gate; if it's not green,
  treat that as a Phase 1 regression to bisect against the commits in this
  pass (three commits: the nine duplicate files, the 35 Cpackets handlers,
  the CMakeLists.txt cleanup).

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
- [~] Fake `HRESULT`/`S_OK`/`S_FALSE` machinery — partially done.
      Changed `CSDLInput::SetAcquire()` from `HRESULT` to `void`: it's a
      documented no-op on the SDL backend, and all 7 call sites
      (`WinMain.cpp`, `hangul/Ci.cpp`, `GameInit.cpp`, `GameMain.cpp` ×3)
      discard the return value as a bare statement. Commit `8725860`.
      **Did not delete the machinery itself.** `HRESULT`/`S_OK`/`S_FALSE`/
      `SUCCEEDED`/`FAILED` appear in ~20 files tree-wide; some are
      Windows-only (`VS_UI_WebBrowser.cpp`, `Imm/*`, `Client.cpp`,
      `WavePackFileManager.cpp` are excluded from the non-Windows CMake
      source list, so they always get the real SDK `HRESULT` and don't
      depend on the Platform.h shim), but others aren't provably so
      without a build. Also found `CDirectSoundStream.cpp` and
      `CDirectSoundStream_Adapter.cpp` both fully implement
      `CSDLStream::WaveReadFile` (and are both unconditionally in
      `DXLIB_SOURCES` when `HAVE_SDL2_MIXER` is on) — a likely duplicate-
      symbol linker error, and squarely the "delete the `_Adapter.cpp`
      twins" work already scoped to Phase 3. Left alone here to avoid
      colliding with that.
- [x] `SCAN_CODE()` / `platform_get_scan_code()` — **doc's premise was
      stale, nothing to do.** `SCAN_CODE` isn't in `Platform.h` at all;
      it's defined in `basic/PlatformUtil.h` (`#ifdef PLATFORM_WINDOWS`
      → `LOBYTE(HIWORD(x))`, else → `platform_get_scan_code(x)`), has
      live callers (`VS_UI_Game.cpp`, `VS_UI_Title.cpp`), and
      `platform_get_scan_code()` is genuinely implemented in
      `PlatformSDL.cpp:312`. No dead macro, no missing function.
- [ ] Mutex primitive unification — **not attempted; scope reported
      instead of a partial fix, per instructions.** ~16 call sites of
      `Enter/Leave/Initialize/DeleteCriticalSection` across 11 files, not
      a uniform mechanical pattern:
      - A global `CRITICAL_SECTION g_Lock` (defined in
        `CMessageArray.cpp`, `extern`-declared in `Client.cpp`,
        `GameInit.cpp`, `RequestServerPlayerManager.cpp`) is
        `InitializeCriticalSection`'d once and `DeleteCriticalSection`'d
        at **6 different early-return points** scattered through the
        ~1,300-line `WinMain()` in `Client.cpp`. Safe to reason about
        (all 6 are alternate exits of one call, not repeated re-init),
        but a real cross-TU refactor, not a search-and-replace.
      - `MWorkThread.h`'s `m_csDeque`/`m_csCurrent` are **never
        Initialize/DeleteCriticalSection'd anywhere** — a latent bug,
        currently inert only because `LockDeque`/`UnlockDeque`/
        `LockCurrent`/`UnlockCurrent` have zero callers.
      - `ProfileManager.h` has a `static_assert(sizeof(CRITICAL_SECTION)
        >= ...)` that encodes assumptions about the shim's layout and
        would need rework for any replacement primitive.
      - 5 other classes (`RequestUserManager`, `WhisperManager`,
        `RequestClientPlayerManager`, `RequestServerPlayerManager`'s own
        `m_Lock`, `ProfileManager`) follow the simple
        member-Init-in-ctor/Delete-in-dtor pattern and would be
        mechanical on their own.
      This needs a working Windows build to verify (std::mutex isn't
      copyable, so anywhere one of these structs is copied/memset would
      only surface at compile time) and is sizeable enough to be its own
      follow-up, not a Phase 2 sub-item done blind.
- [ ] GDI stubs — skipped entirely per instructions (Phase 5 territory,
      `LOGFONT` is now a live parameter type in `Base::SetFont`).
- Target: `Platform.h` shrinks to under 600 lines.
      **[measured 2026-08-06] 1,995 lines** (was 1,996 at session start;
      1,968 baseline). The two safely-completed Platform.h items nearly
      net out — comments explaining *why* the Windows/non-Windows split
      must stay cost about as many lines as the dead code removal saved.
      The real mass reduction available here is the mutex unification
      (~major structural item, deferred above) and a full HRESULT-
      machinery audit across ~20 files (also deferred) — neither is safe
      to do blind without a compiler.

### Phase 3 — Collapse DXLib into a thin SDL facade (client, item 1 done 2026-08-07)
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
      - Not build-verified — no compiler toolchain in this environment (see
        `CLAUDE.md`). Verification was grep-based call-site tracing plus
        reading every file in the duplicate/shim set end to end, not a
        compile.
- [ ] **Item 2** — Delete `CDirectSetup*`, `CDirectDrawSurface*`, `BIT_RES.*`,
      `CDirectDraw_StaticMembers.cpp`. Not started. Note from item 1 (checked
      independently, not just carried over from the parked branch):
      `CDirectDrawSurface` is not dead — `Client/SpriteLib/CSpriteSurface.cpp`
      / `.h` reference it and are themselves pulled in by
      `MEventManager`, `MGuildMarkManager`, `MTopView`, `ProfileManager`,
      `UIMessageManager`, `UtilityFunction`, and two `VS_UI` headers.
      Confirm exactly what's live vs. dead in that surface before deleting —
      this may need to become a rename instead, same as item 1's
      `CDirectDraw` → `CSDLGraphics`.
- [ ] **Item 3** — Move the surviving `CSDL*` + `DXLibBackendSDL.cpp` out of
      `Client/DXLib/` into `Client/Platform/`. Delete `Client/DXLib/`. Not
      started; update `dkrix/CMakeLists.txt` include paths and source globs
      to match.

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

Safe now (judgeable by reading):

- [ ] Correct `Client/SpriteLib/SPRITELIB_BACKEND_README.md` — its diagram
      attributes the SDL2 backend to `engine/sprite` and it is stamped
      "Production Ready / integration tests passing". Both false.
- [ ] Delete 8 dead files (312 lines): `CAlphaSpritePackList{555,565}.{cpp,h}`
      and `CSpritePackList{555,565}.{cpp,h}`. None of the four `.cpp` are in
      `SPRITELIB_SOURCES` (so none is compiled) and every external reference
      to `CSpritePackList555/565` is commented out. Also drop lines 103, 104,
      118, 119 of `Client/SpriteLib/CMakeLists.txt`.

Blocked on green CI (Phase -1):

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

### Phase 6 — Modern C++ as we touch it (ongoing)
- Rule of thumb when a file is already being modified for another
  reason:
  - Prefer `std::unique_ptr` / RAII over raw `new`/`delete`.
  - Use `<thread>`, `<mutex>`, `<condition_variable>` for new
    concurrency.
  - Use `<cstdint>` types (`std::uint32_t`, etc.) for new code; do
    not mass-rename existing `DWORD`/`BYTE`/`BOOL` usage.
  - Translate Korean/Chinese comments to English.

### Phase 7 — Server: retire dead binaries — done 2026-08-06

> **Correction to the original wording:** `chinabilling/` top-level is
> **not** dead — `src/server/chinabilling/CMakeLists.txt` builds it into
> the `GameServerCBilling` and `LoginServerCBilling` static libs, which
> `gameserver` and `loginserver` link. Only its two subdirs (`stress/`,
> `testserver/`) were dead. "All three subdirs" in the earlier wording
> was wrong on both count and contents — there are two.

- [x] Delete `cacheserver/`, `theoneserver/`, `updateserver/`,
      `chinabilling/stress/`, `chinabilling/testserver/`,
      `gameserver/mofus/testserver/`. Verified with a repo-wide grep
      of every `CMakeLists.txt` for each directory name (zero hits)
      before deleting, and confirmed with a CMake configure pass
      (MSVC generator; this workstation lacks the Linux toolchain
      for a real `make debug` at grep time) that reached the
      `find_package` stage without any `add_subdirectory` path
      errors. The only remaining references anywhere in the tree are
      the legacy `src/**/Makefile` files, already superseded by
      CMake — `dkrixserver/Makefile`'s `make`/`make debug` targets
      only invoke CMake, never touch them. Left in place; cleaning up
      stale Makefiles is a separate build-hygiene pass, out of scope
      here.
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

**Build verification — [measured 2026-08-06], not just grep.** This
workstation has a full Linux toolchain via WSL (`Ubuntu`, with
`libmysqlclient-dev`, `liblua5.1-dev`, `libxerces-c-dev`,
`libnsl-dev` already installed). Ran the actual authoritative command
— `cd dkrixserver && make debug` — from `/mnt/c/dev/newdk/...` against
every change in this phase (all four commits below). **It exited 0**
and produced all three binaries fresh: `bin/gameserver` (403 MB
debug), `bin/loginserver` (17 MB), `bin/sharedserver` (9.8 MB) — the
last build target reached and linked was `gameserver`, which is the
binary that actually contains every `__OLD_GUILD_WAR__` edit (`Guild.h`
×2, `GuildManager.cpp` ×2, `war/*`, `quest/Action*Reinforce/Siege.cpp`)
plus the deleted-directory and `.old.cpp` changes. Only warnings
(`-Wdeprecated-declarations` on `std::binary_function`,
`-Wsign-compare`), no errors. This supersedes the CMake-configure-only
check recorded in the per-item notes above, which was a weaker signal
taken before a Linux toolchain was confirmed available in this
environment. `clang-format` is not installed in this WSL image, so
`make fmt` could not be run; formatting was matched by hand to the
surrounding style in each edit.

**Outcome (2026-08-06):** one commit per checklist item —
`__OLD_GUILD_WAR__` gates, dead binary trees, `testAlone/`, and
`DatabaseManager.old.cpp` — landed on a review branch, not `main`,
and now have a green `make debug` behind them.

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
- [ ] Introduce a `PreparedStatement` wrapper over `mysql_stmt_*`.
- [ ] Migrate the ~87 `sprintf(query, ...)` sites module by module,
      starting with user-string call sites (chat, pet names, custom
      options).
- [ ] Add a CI grep-gate that fails on new `sprintf(query,` call
      sites in `src/server/`.
- [ ] Move `DB_PASSWORD`, `DB_HOST`, `LoginServerIP`,
      `SharedServerIP`, `LogServerIP` from `conf/*.conf` into
      environment variables. Keep `*.conf` as a template with
      `${VAR}` placeholders.

### Phase 9 — Server: Lua sandbox and packet schema
- [ ] Replace `luaL_openlibs()` with a whitelist (`base`, `math`,
      `string`); drop `io`.
- [ ] Wrap every `lua_to*` → enum cast in a range-checked helper.
- [ ] Share packet definitions between client and server: either a
      git submodule of `src/Core/Packets/` consumed by both, or a
      generated file from a small schema (pick when we get there,
      record the decision here).
- [ ] Add endian-safe read/write primitives in
      `SocketInputStream` / `SocketOutputStream`.

### Phase 10 — Build hygiene & CI
- [ ] Add `.clang-format` to `client/`; implement
      `make fmt` / `fmt-check` (remove the TODO stubs).
- [ ] Replace `file(GLOB …)` with explicit source lists in the
      client CMake.
- [ ] Add a GitHub Actions matrix that runs `make debug-asan` on
      Linux and macOS.
- [ ] Both trees: `.gitignore` for `build/`,
      `compile_commands.json`, editor detritus.

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
and cannot compile any of them.

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

**The honest summary:** the ceiling on unsupervised work is set by Phase -1.
Until CI exists, every code change is a proposal that a human must build and
judge, so "start the project and walk away" is not available. Once CI is
green on both trees, Phases 1, 2, 3, 7 and 10 become genuinely delegable in
sequence, and the interference drops to reviewing pull requests.

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
