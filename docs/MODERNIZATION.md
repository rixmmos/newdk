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

### Phase 2 — Shrink `basic/Platform.h` (client, in progress 2026-04-17)
- [x] Fix the duplicate `id_t` typedefs at Platform.h:128, 130, 358,
      371; collapse to a single `typedef uint32_t id_t;` with a comment
      noting the POSIX `<sys/types.h>` collision risk.
- [~] Delete the dead `_T`, `_tcscpy`, `_tcscat`, `stricmp`, `_itoa`
      macro shims. The claim that all five were unused was half right:
      `_tcs*` family (`_tcscpy`, `_tcscat`, `_tcslen`, `_tcschr`,
      `_tcsrchr`, `_stprintf`, `_tprintf`, `_tmain`) and `_L(x)` had no
      callers and were deleted outright. `_T(x)` / `TEXT(x)` / `TCHAR`
      and `stricmp` are still live; there is no `_itoa` shim at all
      (the six `itoa` call sites are inside `#ifdef __DEBUG_OUTPUT__`
      dead code). Remaining live shims migrate touch-as-you-go per
      Phase 6.
- [x] Delete the fake `HRESULT`/`S_OK`/`S_FALSE`/`SUCCEEDED`/`FAILED`
      machinery. The only live SDL-path consumers were three DXLib
      adapter methods that always returned `S_OK` and a `::InitFail()`
      in `Client/Client.cpp` whose return value nobody read; those
      signatures were switched to `BOOL` or `void`. Every other
      `HRESULT` reference in the tree is in a build-excluded file
      (Imm/\*, `VS_UI/WinMain.cpp`, `WebBrowser.cpp`, `CDirectInput.cpp`,
      etc.), inside `#ifdef PLATFORM_WINDOWS` or `#ifdef OUTPUT_DEBUG`,
      or inside a comment.
- [x] `platform_get_scan_code()` / `SCAN_CODE()` — no action needed.
      The orphan claim in the original plan was wrong:
      `platform_get_scan_code()` is defined in `basic/PlatformSDL.cpp`
      and `SCAN_CODE()` forwards to it through `basic/PlatformUtil.h`.
      Verified by grep 2026-04-17.
- [x] Back `CRITICAL_SECTION` with `std::recursive_mutex` instead of a
      hand-rolled `pthread_mutex_t` + `initialized` flag. Windows
      `CRITICAL_SECTION` is recursive; `std::recursive_mutex` has
      matching semantics. The `Initialize/Enter/Leave/Delete`
      wrappers are kept as one-liners so the ~60 existing call sites
      in `ProfileManager`, `WhisperManager`, `RequestUserManager`,
      `RequestClientPlayerManager`, `RequestServerPlayerManager`,
      `MWorkThread`, `DebugLog`, `CMessageArray`, and the global
      `g_Lock` / `g_log_lock` don't have to change. Per-site migration
      to `std::lock_guard` is deferred to Phase 6.
- [!] GDI stubs (`DeleteObject`, `CreateFontIndirect`, `LOGFONT`)
      deferred to Phase 5 as planned — they come out when
      `VS_UI_Base.cpp` stops calling them.
- Outcome: `Platform.h` went from **1,968 to 1,950 lines** (−18).
  The under-600 target was aspirational; the remaining bulk is the
  Windows-type forward-declaration storm (`WAVEFORMATEX`, `MMCKINFO`,
  `DDSURFACEDESC`, `DDCAPS`, `DDSCAPS2`, `DSBUFFERDESC`,
  `DSBPOSITIONNOTIFY`, `LOGFONT`, the DirectDraw/DirectSound opaque
  struct forwards, `RECT`, `POINT`, `FILETIME`, `SECURITY_ATTRIBUTES`,
  `MAKELONG`/`LOWORD`/`HIWORD`, etc.) and the platform-thread/mutex/
  library-loader C API around `platform_*`. Those deletions are
  downstream of Phase 3 (DXLib collapse) and Phase 5 (GDI removal)
  because the types are consumed by code those phases will rewrite.

### Phase 3 — Collapse DXLib into a thin SDL facade (client, done 2026-04-18)

Audit of `Client/DXLib/` (46 files) against `DXLIB_SOURCES` in
`Client/DXLib/CMakeLists.txt` turned up **25 files / ~5,933 lines that
are not in the build target at all**. Those are the low-risk start.

Sub-commits in the order they will land:

- [x] **C1 — Delete the 25 files that CMake never sees.** They break
      into three piles:
    - **Native DirectX leftovers (no SDL path)**: `CDirectInput.cpp`,
      `CDirectSetup.h`, `CDirectSetupGetVersion.cpp`. These are the
      pre-SDL Win32 implementations; the SDL path uses the
      `*_Adapter.cpp` files instead and the Windows path is no longer
      built.
    - **`BIT_RES.CPP` / `BIT_RES.H`**: duplicate of the live
      `Client/BIT_RES.*` in the main source tree (the one
      `compile_commands.json` actually references). The DXLib copy is
      orphaned.
    - **Vendored mp3 + vorbis decoder dupes**: `mp3.cpp/h`,
      `huffman.cpp/h`, `subdecoder.cpp`, `synfilt.cpp`, `reader.cpp/h`,
      `soundbuf.cpp/h`, `header.cpp/h`, `codec.h`, `common.h`,
      `config_types.h`, `l3types.h`, `ogg.h`, `os_types.h`,
      `vorbisenc.h`, `vorbisfile.h`. Byte-for-byte duplicates of
      the live copies in `Client/` root. The live copies are
      preserved; these are deleted.

      (Note: `Client/BIT_RES.CPP`, `Client/huffman.cpp`, and the
      sibling vendored files do `#include "bit_res.h"` in lowercase
      while only `BIT_RES.H` exists on disk. That only resolves on
      case-insensitive filesystems (macOS default, Windows). On
      case-sensitive Linux the current build is already broken
      here. Fixing the lowercase-include bug is Phase 6
      touch-as-you-go scope, not Phase 3 — we're only removing the
      DXLib copies.)

- [x] **C2a — Unbreak the `HAVE_SDL2_MIXER=ON` build.** The duplicate-
      symbol problem was structural, not byte-level: CMakeLists
      appended the `_Adapter.cpp` files on top of the plain `.cpp`
      stubs instead of replacing them. Fixed by making the
      `HAVE_SDL2_MIXER` gate an if/else (pick one file per class).
      Also fixed the ctor/dtor/global-instance name mismatches in
      `CDirectSound_Adapter.cpp` and `CDirectSoundStream_Adapter.cpp`
      (`CSDLAudio::CDirectSound()` → `CSDLAudio::CSDLAudio()` etc.).
      On Enrico's current macOS build (`HAVE_SDL2_MIXER=OFF`) the
      behavior is unchanged — the stubs still run. Any box with
      mixer now links.
- [ ] **C2b — Merge each `CSDL*.cpp` / `_Adapter.cpp` pair into a
      single file** that picks stub-vs-SDL by `#ifdef` inside methods.
      With C2a in place the duplicate-symbol bug is structurally
      resolved, so this becomes a pure cleanup (remove the two-file
      split, keep one). Demoted from blocker to touch-as-you-go scope.
- [x] **C2c-pre — Consolidate header duplicates between `Client/` and
      `Client/DXLib/`.** Audit found 5 `CDirect*.h` files living in
      both places with minor diffs, shadowing each other on the
      include path depending on whether the consumer was in `Client/`
      or `Client/DXLib/`. Landed in three commits:
      (1) delete three modulo-whitespace-identical shadow copies in
          `Client/` root (`CDirectInput.h`, `CDirectSound.h`,
          `CDirectSoundStream.h`);
      (2) fold the richer `Client/CDirectMusic.h` content (extra
          `DDSCAPS_*`/`DD_OK` macros used by MTopView/GameInit/
          CShadowPartManager, plus `typedef long long MUSIC_TIME`)
          into the `Client/DXLib/` copy and delete the shadow;
      (3) delete the orphan `Client/CDirectSetup.h` (C1 oversight —
          only refs were in a fully commented-out `/* */` block in
          `Client/Client.cpp`).
      Every class header in scope is now single-source.
- [x] **C2c — Rename `CDirect<Thing>.{h,cpp}` → `CSDL<Thing>.{h,cpp}`.**
      The class names in the headers already read `CSDLInput` /
      `CSDLAudio` / `CSDLMusic` / `CSDLStream`; only the filenames
      carried the DirectX brand. `git mv` on 11 files in
      `Client/DXLib/` plus word-bounded perl substitution across 38
      consumers in `Client/`, `VS_UI/`, `basic/`, `CMakeLists.txt`
      and `CLAUDE.md`. `CDirectSoundBuffer` (a distinct class from
      the Win32 sound API surface) protected by the word boundary
      and left intact. Live class reference in `VS_UI/WinMain.cpp`
      (`extern CDirectInput*` / `new CDirectInput`) also updated so
      the file stops referring to a name that didn't exist anywhere.
      Stale `__CDirectSound_H__` include guard in `CSDLAudio.h`
      renamed to `__CSDLAUDIO_H__`. `git grep -nE '\bCDirect(Input|Sound|Music|SoundStream)\b'` returns nothing
      outside `docs/archive/`.
- [x] **C3-pre — Consolidate the `CDirectDrawSurface.h` shadow.** Same
      pattern as C2c-pre: `Client/CDirectDrawSurface.h` was a divergent
      shadow of `Client/DXLib/CDirectDrawSurface.h`, winning on the
      `-I` path for every unprefixed consumer outside `DXLib/`. The
      DXLib copy had the `m_ddsd` field declared and Win32-only
      accessors; the shadow had `PLATFORM_WINDOWS`-wrapped stubs but
      no `m_ddsd`. Folded the stubs into the canonical copy (keeping
      `m_ddsd`, which is needed by `CSpriteSurface.cpp`) and deleted
      the shadow. Single source of truth restored.
- [x] **C3 — Rename `CDirectDraw{,Surface}.{h,cpp}` →
      `CSDL{Graphics,Surface}.{h,cpp}`.** Same mechanical shape as
      C2c, 39 files touched. Five `git mv` plus longest-first
      word-bounded perl substitution. `CDirectDrawSurface` became
      `CSDLSurface` cleanly (123 refs). `CDirectDraw` turned out to
      already be half-renamed: the class inside had been changed to
      `CSDLGraphics` (include guard was already `__CSDLGRAPHICS_H__`)
      but 24 stragglers still referred to `CDirectDraw` as a class
      name, including the live `CDirectDraw gC_DD;` declaration in
      `VS_UI/WinMain.cpp`. Finished that rename by aligning the
      filename to the existing class name (`CSDLGraphics.h`) and
      substituting the stragglers. Surviving `CDirect*` symbols are
      `CDirect3D` / `CDirectSetup` (both in commented-out blocks),
      `CDirectSoundBuffer` (distinct class, Win32 surface handle),
      and `CDirectionFramePack` (unrelated).
- [x] **C4 — Move `Client/DXLib/` → `Client/Platform/`, delete
      `Client/DXLib/`.** 19 files renamed via `git mv`. Also:
      deleted the redundant `Client/DXLib/DXLib.h` internal router
      (duplicated `Client/DXLib.h`, differing only in a
      `PLATFORM_WINDOWS` gate on `CSDLGraphics.h`), renamed the
      CMake target `dxlib` → `platform` (17 refs across top-level
      and library `CMakeLists.txt`), updated
      `add_subdirectory(Client/DXLib)` → `Client/Platform`, all
      eight `target_include_directories` paths pointing at the old
      directory, the one include path in
      `Client/SpriteLib/CMakeLists.txt`, and eight
      `#include "DXLib/..."` prefix-path includes to
      `"Platform/..."`. Install destination `include/dxlib` →
      `include/platform`. Left intentionally alone (scope control):
      `DXLibBackend.h` / `DXLibBackendSDL.cpp` filenames and the
      `Client/DXLib.h` client-facing router (10 consumers).
- [x] **C5 — This file, updated with Phase 3 outcomes.**

#### Phase 3 outcome

Final state of `Client/Platform/` (former `Client/DXLib/`): 19 files,
covering the SDL2 abstraction layer. All files now use CSDL*-style
naming for class headers and the `DXLibBackend` pair as the backend
interface. The 25 dead files from C1 (~5,933 lines) plus two shadow
headers from C2c-pre and one from C3-pre are gone; the duplicate
`.cpp`/`_Adapter.cpp` pairs are structurally resolved by the
CMakeLists `if(HAVE_SDL2_MIXER)` gate from C2a.

Net Phase 3 line count across eleven core commits
(`49e7a8a..20ffd70`): roughly **-6,100** deletions against
**+~200** insertions, almost all of the insertions being
CMakeLists adjustments and the C3-pre stub-accessor fold.

Deferred (not blocking close-out):

- **C2b** — merge each `CSDL*.cpp` / `_Adapter.cpp` pair into one
  file with `#ifdef HAVE_SDL2_MIXER` inside the methods. The
  duplicate-symbol bug it would have fixed is structurally resolved
  by C2a already, so this is pure cleanup. Demoted to Phase 6
  touch-as-you-go.
- The `DXLibBackend{.h,SDL.cpp}` filenames and the `Client/DXLib.h`
  client-facing router, left intact in C4. Each has a chain of
  consumers whose rename would be scope creep; defer as
  touch-as-you-go.
- Lowercase `#include "bit_res.h"` / uppercase `BIT_RES.H` mismatch
  in `Client/huffman.cpp` and siblings — Phase 6 scope, noted in
  C1.

Rationale for the commit split: each sub-commit is individually
reviewable and individually revertable. C1 is pure deletion and
can land before any code rewrites; the renames in C2/C3 are the
risky edits; C4 is mechanical path shuffling.

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
