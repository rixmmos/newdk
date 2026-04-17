# MODERNIZATION.md — Running Plan

This is the single source of truth for the DarkEden-like project's
modernization work, spanning `server/`, `client/`, `docs/`, configs, and
tooling. It supersedes the per-area status documents in `client/*.md`
(which remain in git history for reference but should not be treated as
current).

When anything here falls out of date, update this file as part of the
change that made it stale — not after the fact.

## Ground truth (as of 2026-04)

The following is what the trees actually contain, verified by direct read
and grep. Where it disagrees with a per-area status doc, this file wins.

### Client (`client/`)
- SDL2 is the only supported backend on every platform. The old Windows
  native backend has been deleted (`basic/PlatformWindows.cpp` is gone;
  `USE_SDL_BACKEND` is forced `ON` in `CMakeLists.txt`).
- `Client/D3DLib/` has been removed. The references to it in the client's
  `CLAUDE.md` are stale.
- `Client/WinLib/` still exists but is effectively dead: `CWinUpdate` has
  no subclasses and no live callers.
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
- Text rendering is split: `VS_UI/src/VS_UI_Base.cpp` still calls GDI
  (`CreateFontIndirect`, `DeleteObject`) on Windows, while macOS and
  Linux go through `TextSystem` (SDL2 + freetype2). TextSystem works on
  Windows too — the fork exists only because nobody has flipped the
  switch.
- `basic/Platform.h` is 1,968 lines, declares `id_t` four times (once
  duplicated back-to-back), and keeps fake `HRESULT`, stub `DeleteObject`,
  and unused `_T()` macros alive.
- The client build is held together by `file(GLOB …) + FILTER EXCLUDE`.
  Nine `.cpp` files (~7,550 lines) with duplicate symbols are excluded by
  explicit regex, as are 35 `Client/Packet/Cpackets/*Handler.cpp` files
  (1,609 lines of server-side handlers in the client tree) and one
  19-year-old `MItemTable_bak-2007-5-7.cpp` (14,965 lines). Total
  tree-resident dead weight: ~24,100 lines.
- Client has no `.clang-format`; the `fmt` and `fmt-check` Makefile
  targets are stubbed `TODO`s. Format is not enforced.
- Sanitizer builds (`make debug-asan`, `make debug-tsan`) are real and
  wired up correctly.

### Server (`server/`)
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
- Database access is libmysqlclient through a thin wrapper. ~87
  `sprintf(query, "... '%s' ...", field.c_str())` sites exist. No
  prepared-statement abstraction. `DatabaseManager` is thread-local,
  not pooled.
- Lua 5.1 / LuaJIT is embedded via the C API. `luaL_openlibs()` is
  called, so quest scripts can `io.open` the filesystem. Enum values
  are pulled via `lua_tonumber` with no range check.
- `conf/gameserver.conf` contains plaintext `DB_PASSWORD: elca110`,
  dev IPs (`192.168.0.16`), and double-encoded Chinese comments from
  a prior encoding migration.

### Docs (`docs/`)
- Mixed-language reference material (mostly Chinese filenames) plus
  `howto/`, `pic/`, `client_source_overview/`. `README.md` is empty.
- Project-wide `CLAUDE.md` now lives here as the cross-cutting spec.
- This file is the living modernization plan.

## Roadmap

Phases are ordered for least-risk-first and for unblocking downstream
work. "P0" is a prerequisite for everything else; the others can
interleave once P0 is done.

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

### Phase 1 — Delete the unambiguously dead (client, in progress 2026-04-17)
- [~] Delete `Client/MItemTable_bak-2007-5-7.cpp` (14,965 lines).
      **Not present in the current tree or git history** as of
      2026-04-17. The file was apparently deleted before this plan was
      written. No action taken; noted for the record.
- [x] Delete the nine CMake-excluded duplicate `.cpp` files
      (`GlobalVariables`, `MissingGlobals`, `GameHelpers`,
      `GameFunctions`, `GamePacketFunctions`, `ActionFunctions`,
      `MitemTableinit2`, `GCNotifyWinHandler`, `GCNotifyWin`) plus
      the two orphan headers that came with them
      (`GameHelpers.h`, `Client/GCNotifyWin.h` — the latter a
      duplicate of `Client/Packet/Gpackets/GCNotifyWin.h`). Measured
      ~7,750 lines removed (the .cpp alone were 7,550; the two
      orphan headers added ~180). The `REMOVE_ITEM` block in
      `dkrix/CMakeLists.txt` was shrunk to just the
      `${VS_UI_CLIENT_SOURCES}` entry in the same commit.
- [x] Delete `Client/Packet/Cpackets/*Handler.cpp` (35 files,
      ~1,609 lines; server-side handlers). The 14 of these that
      needed a client-side link target already had stubs in
      `Client/CGHandlersStub.cpp`; the other 21 had no client
      references at all. The matching
      `FILTER EXCLUDE ".*Cpackets/.*Handler\\.cpp"` line was
      dropped from `dkrix/CMakeLists.txt` in the same commit.
- [!] Delete `Client/WinLib/` (3 files). **Deferred** — the
      "no subclasses, no live callers" claim was stale:
      `CWinUpdate` is actively subclassed by `CWaitUIUpdate`,
      `CWaitPacketUpdate`, `CGameUpdate`, `COpeningUpdate`, and
      `g_pUpdate` is referenced in `Client.h`, `GameMain.cpp`,
      and `Client.cpp`. Removing `WinLib/` here would require
      restructuring the update-state hierarchy, which is bigger
      than "delete the unambiguously dead." Re-scoped: folded
      into Phase 3 (DXLib collapse) since both involve flattening
      a legacy facade in the same neighborhood.
- [x] Add `compile_commands.json`, `*_bak-*.cpp`, `*.dsp`, `*.dsw`,
      `*.ncb`, `*.opt`, `*.plg` to `dkrix/.gitignore`, and untrack
      the 3.4 MB `dkrix/compile_commands.json` that had been
      checked in. `build/`, `*.bak`, and `*_bak*` were already
      covered.
- Net: roughly **~9,400 lines** of source removed from the client
  tree (plus 3.4 MB of generated JSON untracked), against an
  original headline target of "~24,100 lines." The shortfall is
  entirely the missing `MItemTable_bak-2007-5-7.cpp` (14,965).
- Follow-up: `make debug-asan` not yet exercised —
  the sandbox can't run the client build. Human verification
  pending before declaring Phase 1 fully complete.

### Phase 2 — Shrink `basic/Platform.h` (client)
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
      `LOGFONT`) to Phase 5 — they come out when `VS_UI_Base.cpp` stops
      calling them.
- Target: `Platform.h` shrinks from 1,968 lines to under 600.

### Phase 3 — Collapse DXLib into a thin SDL facade (client)
- [ ] Rename `CSDLInput` → `InputManager`, `CSDLAudio` →
      `AudioManager`; delete `CDirect{Input,Sound,Music,Draw}` shims
      and their `_Adapter.cpp` twins; grep-replace callers.
- [ ] Delete `CDirectSetup*`, `CDirectDrawSurface*`, `BIT_RES.*`,
      `CDirectDraw_StaticMembers.cpp`.
- [ ] Move the surviving `CSDL*` + `DXLibBackendSDL.cpp` out of
      `Client/DXLib/` into `Client/Platform/`. Delete `Client/DXLib/`.

### Phase 4 — One sprite pipeline (client)
- [ ] Decide here, in writing, whether `tools/engine/sprite/` absorbs
      `Client/SpriteLib/` or the other way around. Update this file
      with the decision before touching code.
- [ ] Delete the 555/565/4444 format-variant classes in
      `Client/SpriteLib/` (`CSprite555`, `CSprite565`,
      `CAlphaSprite{555,565}`, `CAlphaSpritePackList{555,565}`,
      `CIndexSprite{555,565}`, `CSpritePackList{555,565}`).
- [ ] Delete `CAlphaSprite::Blt4444*` methods.
- [ ] Ensure viewer binaries and the main client share one sprite lib.

### Phase 5 — One text pipeline (client)
- [ ] In `VS_UI/src/VS_UI_Base.cpp`, remove the `#ifdef
      PLATFORM_WINDOWS` GDI branch and route Windows through
      `TextSystem` like every other platform.
- [ ] Verify Korean and Chinese glyph coverage in the fallback fonts.
- [ ] Delete the GDI stubs in `Platform.h` (previously deferred from
      Phase 2).

### Phase 6 — Modern C++ as we touch it (ongoing)
- Rule of thumb when a file is already being modified for another
  reason:
  - Prefer `std::unique_ptr` / RAII over raw `new`/`delete`.
  - Use `<thread>`, `<mutex>`, `<condition_variable>` for new
    concurrency.
  - Use `<cstdint>` types (`std::uint32_t`, etc.) for new code; do
    not mass-rename existing `DWORD`/`BYTE`/`BOOL` usage.
  - Translate Korean/Chinese comments to English.

### Phase 7 — Server: retire dead binaries
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

## Living index of superseded docs

These docs are historical. They may contradict each other and this
file; trust this file.

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

Phase 0 will move these into `client/docs/archive/2026-migration-notes/`
so they stop competing with the current plan at the repo root.
