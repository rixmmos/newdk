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

## Verification status — read this first

Every "done" claim below is either **[measured]** (re-verified against the
working tree on 2026-08-06 by direct file inspection) or **[unverified]**
(asserted by a previous pass, not re-checked, and not confirmed by a build).

**No claim in this document has been confirmed by a compile.** The client
requires Windows + Visual Studio 2022 + vcpkg; the server requires Linux with
libmysqlclient, lua5.1, and xerces-c. Neither toolchain is available to an
agent session. Until a CI pipeline exists (Phase 10), "the tree still builds"
is a statement only a human on this workstation can make.

### Open blockers

1. **~6,825 files are modified and uncommitted** in the working tree
   (2,382 of them under `dkrix/`, +661k/−675k lines). Nothing in this plan
   can be safely reasoned about until that delta is either committed,
   branched, or discarded. Any measurement below describes the *working
   tree*, not any commit.
2. **Branch name mismatch**: local `modernize/phase-4-sprite` vs remote
   `origin/modernize/phase4-sprite` (hyphen). Confirm these are the same
   intended branch before pushing.
3. **No CI, no tests, no client `.clang-format`.** There is no automated
   signal that any change is safe.

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
- **[measured] `basic/Platform.h` is 1,996 lines — it grew by 28.** The
  target is under 600. `id_t` appears 5 times, `HRESULT` 4 times, and the
  `_T` / `_tcscpy` shims are still present, as are `DeleteObject` (line 263),
  `LOGFONT` (line 281), and `CreateFontIndirect` (line 366).
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
- Current Windows failures are portability debt in the code, not missing
  tooling: `basic/Platform.h` still drags `pthread.h` into Windows
  builds, and some helper files still include POSIX-only headers such as
  `sys/time.h`.

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
- **[measured] `README.md` is no longer empty** (1,681 bytes), but it still
  describes "three sibling working trees, each a separate repository,
  checked out side by side." That is wrong: `dkrix/` and `dkrixserver/` are
  folders inside the single `newdk` repo. Fix in the next docs pass.
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

### Phase -1 — Make the work verifiable (NEW, blocking, not started)

This did not exist in earlier revisions and it should have. Nothing below
can be trusted without it, and it is the single change that would let
routine phase work be delegated rather than hand-held.

- [ ] Resolve the ~6,825-file uncommitted delta: commit it to a branch,
      stash it, or discard it. Decide deliberately — do not keep working
      on top of it.
- [ ] Reconcile `modernize/phase-4-sprite` with
      `origin/modernize/phase4-sprite`.
- [x] Add a root `.gitignore` (see `../CLAUDE.md` → Repo hygiene). **Done.**
- [ ] Stand up GitHub Actions on `rixmmos/newdk`:
      - server: `make debug` on Ubuntu with libmysqlclient / lua5.1 /
        xerces-c;
      - client: `cmake --build` on `windows-latest` with vcpkg.
      Red build = blocked merge. This is Phase 10's first bullet, promoted
      to blocking because it is the prerequisite for everything else being
      checkable by anyone other than the person at this workstation.
- Success: a change can be proposed, built, and judged without a human
  manually running two toolchains.

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

### Phase 1 — Delete the unambiguously dead (client, ~1/5 done)
- [x] Delete `Client/MItemTable_bak-2007-5-7.cpp` (14,965 lines).
      **[measured 2026-08-06] Confirmed gone.**
- [ ] Delete the nine CMake-excluded duplicate `.cpp` files
      (`GlobalVariables`, `MissingGlobals`, `GameHelpers`,
      `GameFunctions`, `GamePacketFunctions`, `ActionFunctions`,
      `MitemTableinit2`, `GCNotifyWinHandler`, `GCNotifyWin`) —
      ~7,550 lines.
- [ ] Delete `Client/Packet/Cpackets/*Handler.cpp` (35 files,
      ~1,609 lines; these are server-side handlers).
- [ ] Remove the matching `REMOVE_ITEM` / `FILTER EXCLUDE` lines from
      `CMakeLists.txt`.
- [ ] ~~Delete `Client/WinLib/` (3 files, all dead).~~ **Withdrawn
      2026-08-06 — the "all dead" premise is false.** `CGameUpdate`
      subclasses `CWinUpdate` and the client holds live pointers to it.
      See the Ground-truth note above. Reclassify as a refactor of the
      update path and re-file under Phase 3 or later.
- [ ] Add `build/`, `compile_commands.json`, `*.dsp`, `*.dsw`,
      `*_bak-*.cpp`, `*.bak` to `.gitignore`.
- Success: tree is ~24k lines lighter; the Windows client build is still
  green. (The old success criterion named `make debug-asan`, a macOS path
  that is no longer authoritative.)

### Phase 2 — Shrink `basic/Platform.h` (client, not started — file grew)
- [ ] Fix the duplicate `id_t` typedefs at Platform.h:128, 130, 358,
      371; collapse to a single `using id_t = std::uint32_t;`.
- [ ] Delete the dead `_T`, `_tcscpy`, `_tcscat`, `stricmp`, `_itoa`
      macro shims (no callers).
- [ ] Delete the fake `HRESULT`/`S_OK`/`S_FALSE` machinery; update the
      DXLib adapters that return it but never have it checked.
- [ ] Either define `platform_get_scan_code()` or delete the
      `SCAN_CODE()` macro that claims to call it.
- [ ] Pick one mutex primitive (`SDL_mutex *` or `std::mutex`); remove
      the `CRITICAL_SECTION` → pthread shim and its 20-odd callers.
- [ ] Defer the GDI stubs (`DeleteObject`, `CreateFontIndirect`,
      `LOGFONT`) to Phase 5. **Superseded — see the Phase 5 note below.
      `VS_UI_Base.cpp` has already stopped *calling* them, but `LOGFONT`
      is still a parameter type in `Base::SetFont`, so this is now a
      signature change rather than a deletion.**
- Target: `Platform.h` shrinks to under 600 lines.
      **[measured 2026-08-06] Currently 1,996 lines — it has grown by 28
      since the 1,968 baseline. `id_t` × 5, `HRESULT` × 4, `_T`/`_tcscpy`
      shims still present.** Whatever is adding to this file is working
      against the phase; find it before starting.

### Phase 3 — Collapse DXLib into a thin SDL facade (client)
- [ ] Rename `CSDLInput` → `InputManager`, `CSDLAudio` →
      `AudioManager`; delete `CDirect{Input,Sound,Music,Draw}` shims
      and their `_Adapter.cpp` twins; grep-replace callers.
- [ ] Delete `CDirectSetup*`, `CDirectDrawSurface*`, `BIT_RES.*`,
      `CDirectDraw_StaticMembers.cpp`.
- [ ] Move the surviving `CSDL*` + `DXLibBackendSDL.cpp` out of
      `Client/DXLib/` into `Client/Platform/`. Delete `Client/DXLib/`.

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

### Phase 7 — Server: retire dead binaries (not started)

> **[measured 2026-08-06]** All of `cacheserver/`, `chinabilling/`,
> `theoneserver/`, `updateserver/`, `gameserver/mofus/testserver/`, and
> `gameserver/testAlone/` are still present, as is one `.old.cpp` under
> `src/server/database/`.

- [ ] Archive or delete `cacheserver/`, `chinabilling/` (all three
      subdirs), `theoneserver/`, `updateserver/`,
      `gameserver/mofus/testserver/`. Verify CMake isn't building
      them.
- [ ] Replace the stub `testAlone/Mutex.h` with a real primitive or
      delete `testAlone/` outright.
- [ ] Retire `__OLD_GUILD_WAR__` `#ifdef` gates — either delete the
      branches or move to a runtime config flag.
- [ ] Delete `.old.cpp` files in `server/database/`.

### Phase 8 — Server: SQL and secrets
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
