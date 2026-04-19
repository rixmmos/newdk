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

### Phase 4 — One sprite pipeline (client, done 2026-04-18)

**Absorption-direction decision (2026-04-18):** `Client/SpriteLib/`
stays canonical; `tools/engine/sprite/` gets retired. The C engine
is a ~7.8k-LOC parallel C/SDL2 implementation with its own unit
tests (14 files under `tools/engine/sprite/tests/`); the C++
SpriteLib is ~54.8k LOC across 86 files and is what the main client
actually links. Absorbing toward SpriteLib keeps the main-client
path untouched and is the minimum-disruption direction. Viewers that
currently pull from `tools/engine/sprite/` migrate to the SpriteLib
C++ API. The C unit tests either get ported onto SpriteLib or
archived with the engine source — Phase 4 D decides.

- [x] Decide here, in writing, whether `tools/engine/sprite/` absorbs
      `Client/SpriteLib/` or the other way around. Update this file
      with the decision before touching code. *(Decision above:
      SpriteLib absorbs, engine retired.)*
- [x] **A — Delete the 555/565 format-variant classes** in
      `Client/SpriteLib/` (`CSprite555`, `CSprite565`,
      `CAlphaSprite{555,565}`, `CAlphaSpritePackList{555,565}`,
      `CIndexSprite{555,565}`, `CSpritePackList{555,565}`). **Done
      2026-04-18.** Folded the 565 subclass's
      `SaveToFile`/`LoadFromFile` bodies into each of the five base
      classes (CSprite, CAlphaSprite, CIndexSprite, CSpritePackList,
      CAlphaSpritePackList), flipped the pure-virtual `= 0` to
      concrete, deleted 20 subclass files. Five external consumers
      updated (Client/GameInit.cpp, Client/MGuildMarkManager.cpp,
      Client/Client.cpp, Client/MTopView.cpp — includes dropped +
      `if (Is565()) new CFoo565 else new CFoo555` collapsed to
      `new CFoo`). Internal consumers: `CTypePack2<CFoo, CFoo555,
      CFoo565>` typedefs collapsed to `CTypePack2<CFoo, CFoo, CFoo>`
      (CTypePack2 machinery untouched — single-type collapse is
      follow-up); `CSpriteSet::Init` and `CAlphaSpritePack::Init`
      Is565() branches collapsed. Net: -1,166 lines.
- [x] **B — Delete `CAlphaSprite::Blt4444*`** methods plus any
      helpers only they use. Check `CSpriteSurface` for matching
      4444 surface paths. **Done 2026-04-18.** The audit surfaced
      that every external caller of the 4444 family lived inside
      `CSpriteSurface.cpp`, and that file was not in
      `SPRITELIB_SOURCES` — it had been fully orphaned by the SDL
      adapter. Split into two commits. **4B.1** deleted
      `CSpriteSurface.cpp` (-13,778 lines); its static members
      (`s_Value1`, `s_Value2`, `s_pMemcpyEffectFunction`, …) were
      already redefined in `CSpriteSurface_SDL.cpp`, so no link-level
      fallout. **4B.2** then deleted the now-provably-dead methods
      from four classes (-4,014 lines): `CAlphaSprite` (11 Blt4444*
      + 2 memcpy helpers), `CAlphaSpritePal` (same 11 with
      `MPalette&` overloads + 2 helpers), `CSprite` (2
      `BltAlpha4444*` + 2 static memcpy helpers), `CShadowSprite`
      (5 `Blt4444*` + 5 `BltSmall4444*` + `memcpyShadow4444`).
      Headers got "Phase 4B: Blt4444* family deleted" defer comments
      pointing back to 4B.1. `CFilter::Blt4444` and
      `CFilter::Blt4444Color` remain — different class, live code
      called from `MTopView.cpp:10486`. Combined 4B trim: -17,792
      lines across 9 files.
- [x] **C — Migrate viewers to one sprite lib.** Viewers split:
      creature_viewer and item_viewer ship both `main.c` and
      `main.cpp` (dual-consumer); map_viewer and zone_parser lean
      on `tools/engine/sprite/`; sprite_viewer and effect_viewer
      already use SpriteLib. Port the engine-side viewers onto
      SpriteLib, delete the `main.c` halves, leave one `main.cpp`
      per viewer. **Done 2026-04-18.** Audit showed the `.cpp`
      siblings were already the ones in `CMakeLists.txt` — the
      `main.c` / `viewer.c` / `viewer.h` halves were dead.
      **4C.1** deleted six `.c` viewer sources (-1,698 lines):
      `creature_viewer/main.c`, `item_viewer/main.{c,viewer.c,h}`,
      `map_viewer/main.c`, `zone_parser/main.c`. **4C.2** unhooked
      the `sprite` static library by `git-mv`'ing the only
      still-live file — `tools/engine/sprite/src/zone.c` plus its
      `zone.h`/`error.h` headers — into `tools/viewers/map_viewer/`
      and compiling it directly into the `map_viewer` target. The
      `sprite` link was dropped from `map_viewer` and
      `effect_viewer` (the latter's linkage was vestigial —
      `effect_viewer/main.cpp` never included any engine/sprite
      headers). Top-level `CMakeLists.txt` lost
      `add_subdirectory(tools/engine/sprite)` and the
      `BUILD_ENGINE` option. `zone.c` verified to compile cleanly
      standalone (`gcc -std=c99`), `zoneloader.cpp` the same
      (`g++ -std=c++11`).
- [x] **D — Retire `tools/engine/sprite/`**: move the directory to
      `docs/archive/` (source-only, no build) once no viewer
      references it. CMake `add_subdirectory(tools/engine/sprite)`
      and any `target_link_libraries(... sprite)` call sites get
      deleted. **Done 2026-04-18.** `git-mv`'d 43 files
      (CMakeLists.txt, 18 headers, 12 src .c's, 11 tests) to
      `docs/archive/2026-engine-sprite/` and added a README
      explaining the retirement rationale — including that
      `zone.c`/`zone.h`/`error.h` are **not** in the archive
      because they were absorbed by `map_viewer` in 4C.2. The empty
      `tools/engine/` parent directory was also removed. CMake
      cleanup from 4C.2 already covered the
      `add_subdirectory`/link-line work.
- [x] **E — Phase 4 close-out**: MODERNIZATION.md update with
      outcome block (net line count, deferred items). **Done
      2026-04-18.** See Phase 4 outcome below.

**Phase 4 outcome (landed on `modernize/phase4-sprite`):**

Eight commits, all line-count deltas measured against the phase
baseline (`b6a2921 docs: pin Phase 4 absorption-direction decision`):

| Sub | Commit    | What                                                     | Δ lines |
|-----|-----------|----------------------------------------------------------|---------|
| 4A  | `2554309` | Delete 555/565 format-variant classes (20 files)         | -1,166  |
| 4A  | `c74a301` | Check off Phase 4A in MODERNIZATION.md                   |    +12  |
| —   | `3ba2d95` | Defer Windows MSVC build to Phase 10 build-hygiene       |    +17  |
| 4B.1| `4557195` | Delete orphaned `Client/SpriteLib/CSpriteSurface.cpp`    | -13,778 |
| 4B.2| `d656337` | Delete dead `Blt4444*` / `BltSmall4444*` / helpers       | -4,014  |
| 4C.1| `1d96da6` | Delete six legacy `.c` viewer sources                    | -1,698  |
| 4C.2| `74ec792` | Unhook `sprite` library; absorb `zone.c` into map_viewer |     -8  |
| 4D  | `0ae1a98` | Archive `tools/engine/sprite/` → `docs/archive/`         |    +66  |

Net across all of Phase 4: **-20,570 lines** over ~100 files. The
`Client/SpriteLib/` directory shrunk from ~55 KLOC to ~36 KLOC. The
`tools/engine/` directory is gone. Nothing in the live build
references the retired `sprite` static library; the only surviving
artifact of the C engine in-tree is
`tools/viewers/map_viewer/zone.c` (plus its two headers), and those
are compiled directly into the `map_viewer` target.

**Deferred out of Phase 4:**

- **CTypePack2 single-type collapse.** 4A collapsed the
  `CTypePack2<CFoo, CFoo555, CFoo565>` template triples to
  `CTypePack2<CFoo, CFoo, CFoo>`. The `CTypePack2` machinery itself
  still takes three types — follow-up work (outside Phase 4) would
  specialise it to one type or replace it with a single `CTypePack`.
- **Other orphaned `.cpp` in `Client/SpriteLib/`.** The 4A audit
  surfaced three more files in-tree but not in `SPRITELIB_SOURCES`:
  `CAlphaSpritePackList.cpp`, `CIndexSpritePack.cpp`,
  `CShadowSpritePack.cpp`. They may be candidates for 4B-style
  deletion after a caller audit; left for a separate pass rather
  than smuggled into Phase 4.
- **Windows MSVC toolchain build.** Still deferred to Phase 10
  build-hygiene; 4D eliminated the `typeof()`-using `frame.c` /
  `framepack.c` from the live tree (archived), which removes one
  of the four categories of MSVC errors documented under Phase 10.
- **Port the C unit tests** under
  `docs/archive/2026-engine-sprite/tests/` onto the C++ SpriteLib
  API. The tests archive the intent but not the coverage; a
  future test-coverage pass would translate `test_sprite.c`,
  `test_framepack.c`, etc. into equivalent GTest/Catch2 cases
  against `CSprite` / `CFramePack` / etc.

### Phase 5 — One text pipeline (client) — done 2026-04-18
- [x] In `VS_UI/src/VS_UI_Base.cpp`, remove the `#ifdef
      PLATFORM_WINDOWS` GDI branch and route Windows through
      `TextSystem` like every other platform. **(5A, audit-only —
      2026-04-18.)**
  - **Already done upstream.** `VS_UI_Base.cpp` no longer contains
    any `#ifdef PLATFORM_WINDOWS` / `WIN32` guards; the destructor,
    `SetFont`, and `InitSurface` are unconditional and call into
    `TextSystem::EncodeFontSizeHandle()` directly. Comments in the
    file document the removal (`// GDI removed (SDL2) - All platforms
    use TextSystem ...`). Audit verified: `grep -nE 'ifdef
    PLATFORM_WINDOWS|ifndef PLATFORM_WINDOWS|if.*WIN32'
    dkrix/VS_UI/src/VS_UI_Base.cpp` returns zero matches. No code
    delta needed in 5A.
- [x] Verify Korean and Chinese glyph coverage in the fallback fonts.
      **(5D, audit-only — 2026-04-18.)**
  - Primary font: `Data/Font/NotoSansCJK-Regular.ttc` (Noto Sans CJK
    Regular). Noto Sans CJK ships Korean Hangul + Hanja, Chinese
    Simplified + Traditional, and Japanese in a single TTC, so the
    primary load path covers every script the client renders.
  - Fallback chain (see `Client/TextSystem/TextBackendSDL.cpp`
    lines 90–97): `NotoSans-Regular.ttf` →  `DejaVuSans.ttf` →
    `Hiragino Sans GB.ttc` (bundled) →  macOS system
    `Helvetica.ttc` →  macOS system `Hiragino Sans GB.ttc`. Order
    is well-chosen: NotoSansCJK is tried first, and Hiragino Sans GB
    (Chinese Simplified) is retained as a last-resort CJK fallback
    on macOS if the bundled assets are missing.
  - Legacy-encoding inputs are normalised to UTF-8 before reaching
    `TTF_RenderUTF8_Blended`. `TextService::NormalizeToUTF8` tries
    `CP949 → EUC-KR → GBK → GB2312 → BIG5` via `SDL_iconv` and
    returns the first that decodes cleanly
    (`Client/TextSystem/TextService.cpp:99`). This matches the
    encodings the pre-SDL client used on-disk.
  - Note: the `.ttc` / `.ttf` files themselves are runtime assets
    shipped under `Data/Font/` and are **not** in the source tree
    (confirmed: `find dkrix -path '*/Data/Font/*'` returns empty).
    Audit is correctness-of-wiring only; asset presence is a
    packaging concern tracked separately.
- [x] Delete the GDI stubs in `Platform.h` (previously deferred from
      Phase 2). **Narrow scope:** delete only the function-shaped
      stubs whose implementations are dead; keep the data-shape
      types (`LOGFONT`, `COLORREF`, `HDC`, `HFONT`) and the `RGB()`
      macro because the client still uses them pervasively as
      cross-platform font-descriptor / color types. **(5B + 5C —
      2026-04-18.)**
  - **5B:** deleted `CreateFontIndirect`, `DeleteObject`, and
    `GetStockObject` stubs from `basic/Platform.h` (-17 lines). The
    sole live caller of `GetStockObject` in `Client/Client.cpp`
    (inside a `RegisterClass`/`CreateWindow` block) was guarded
    behind `#ifdef PLATFORM_WINDOWS`; on non-Windows
    `wc.hbrBackground` is left NULL because `RegisterClass` is
    itself a no-op stub there. The two `DeleteObject` call paths
    (`Client.cpp:4299`, `VS_UI_WebBrowser.cpp:220`) were both
    already preprocessor-dead — `VS_UI_WebBrowser.cpp` is excluded
    from the non-Windows build via `list(FILTER … EXCLUDE REGEX)`
    in `Client/VS_UI/CMakeLists.txt`, and the Win32 branch in
    `Client.cpp` is gated by `#ifdef __WEB_BROWSER__` (never
    defined). `CreateFontIndirect` had zero callers tree-wide.
  - **5C:** trimmed 45 unused `FW_*` / `OUT_*` / `CLIP_*` / `FF_*` /
    `TA_*` / charset / pitch / quality / brush constants from
    `Platform.h` (-35 lines). Kept the 12 constants actually
    referenced by the live SDL build: `FW_NORMAL`, `FW_BOLD`,
    `DEFAULT_CHARSET`, `OUT_DEFAULT_PRECIS`, `CLIP_DEFAULT_PRECIS`,
    `DEFAULT_QUALITY`, `DEFAULT_PITCH`, `FF_DONTCARE`, `TRANSPARENT`
    (background mode), `TA_LEFT`, `TA_RIGHT`, `TA_CENTER`.

**Outcome (2026-04-18):**

| #    | Commit    | Subject                                                         |
| ---- | --------- | --------------------------------------------------------------- |
| 0001 | `26770be` | `docs: pin Phase 5 plan in MODERNIZATION.md`                    |
| 0002 | `6bee386` | `docs: 5A — record that VS_UI_Base GDI branch was already gone` |
| 0003 | `680cdad` | `client: 5B — delete dead GDI stubs from Platform.h`            |
| 0004 | `1a71063` | `client: 5C — trim unused GDI constant macros in Platform.h`    |
| 0005 | `ac8018a` | `docs: 5D — record CJK glyph-coverage audit outcome`            |
| 0006 | ` ------` | `docs: 5E — close out Phase 5 in MODERNIZATION.md`              |

Net code delta: **-52 lines** in `basic/Platform.h`, **+6 lines**
(one `#ifdef PLATFORM_WINDOWS` guard with `else`-branch comment)
in `Client/Client.cpp`. No source files deleted; this phase was
a dead-stub sweep, not a tree retirement. No behaviour change on
either Windows or non-Windows targets — everything removed was
either provably unreachable (preprocessor-gated dead code, CMake
exclusions) or unreferenced (zero callers tree-wide).

**Deferred from Phase 5 (narrow-scope reasons):**

- **`LOGFONT` / `COLORREF` type renames** — both are used as
  cross-platform data-shape types across ~49 files. A rename to
  `UI_FontDesc` / `SDL_Color`-equivalent is Phase-6
  touch-as-you-go territory, not a flag-day.
- **`SetFont(PrintInfo&, LOGFONT&, COLORREF, ...)` API signature
  change** — same cross-module ripple argument.
- **Win32 `RegisterClass` / `CreateWindow` path cleanup** — the
  `Client.cpp` window-setup code still carries the Windows-only
  path verbatim. Not touched in Phase 5; the `GetStockObject`
  call site was guarded behind `#ifdef PLATFORM_WINDOWS` and the
  rest of the path is already under that same guard transitively.

**Plan (sub-commits):**

- **5A — Audit outcome commit.** Document in this file that the
  GDI branch in `VS_UI_Base.cpp` was already absorbed upstream; no
  code delta in 5A. Effectively converts item #1 from a
  deletion-task into an audit-task and records the finding.
- **5B — Delete truly-dead GDI stubs from `Platform.h`.** Three
  stubs confirmed callable-but-dead:
  - `CreateFontIndirect(LOGFONT*)` — 0 callers in the tree (a
    comment in `VS_UI_Base.cpp` notes the historical caller; the
    code now calls `TextSystem::EncodeFontSizeHandle()`).
  - `DeleteObject(void*)` — 2 callers (`Client.cpp:4299`,
    `VS_UI_WebBrowser.cpp:220`) that are both type-confused; they
    pass `IWebBrowser*` objects and want `delete p` semantics, not
    GDI object deletion. Rewrite the two sites as `delete p;` in
    the same commit.
  - `GetStockObject(int)` — 3 callers (`Client.cpp:685,1674`,
    `VS_UI/WinMain.cpp:3435`) all inside Win32-only `RegisterClass`
    / `CreateWindow` code that's already dead on non-Windows (and
    `WinMain.cpp` itself is Win32-only). Guard those three call
    sites behind `#ifdef PLATFORM_WINDOWS` (or delete, if the whole
    block is already so guarded) in the same commit.
- **5C — Trim unused GDI constant macros in `Platform.h`.** Audit
  the `FW_*`, `OUT_*`, `CLIP_*`, `FF_*`, `TA_*`, charset, and
  quality constants; delete any not referenced anywhere in
  `dkrix/`. Expected to keep `FW_NORMAL`, `FW_BOLD`, `FW_LIGHT`,
  `DEFAULT_*`, `TRANSPARENT`, `TA_RIGHT`, `DEFAULT_CHARSET`,
  `FF_DONTCARE`, `DEFAULT_PITCH`. Pure delete-if-unused pass.
- **5D — Glyph-coverage audit.** Inspect the fallback font wired
  into `TextSystem` (likely under `Client/TextSystem/`) and confirm
  Korean (Hangul) and Chinese (GB2312 / Big5 depending on
  `IsChinese()` branch) glyphs render with the bundled `.ttf` /
  `.ttc`. Documentation-only outcome unless we discover a missing
  font, in which case escalate.
- **5E — Close-out.** Flip this block to `done`, fill in outcome
  notes and commit table.

**Explicit non-goals for Phase 5:**

- Removing `LOGFONT` and `COLORREF` types themselves (used as
  font-descriptor / color data shapes across ~49 files). Touching
  them is a separate modernization pass, probably Phase 6
  touch-as-you-go (`LOGFONT` → `UI_FontDesc`, `COLORREF` →
  `SDL_Color`-equivalent).
- Replacing `SetFont(PrintInfo&, LOGFONT&, COLORREF, ...)` API
  signatures. Same reason — cross-module ripple.
- Auditing or rewriting the Win32 `RegisterClass` / `CreateWindow`
  paths themselves. Gated under `PLATFORM_WINDOWS` in 5B.

### Phase 6 — Modern C++ as we touch it (ongoing)
- Rule of thumb when a file is already being modified for another
  reason:
  - Prefer `std::unique_ptr` / RAII over raw `new`/`delete`.
  - Use `<thread>`, `<mutex>`, `<condition_variable>` for new
    concurrency.
  - Use `<cstdint>` types (`std::uint32_t`, etc.) for new code; do
    not mass-rename existing `DWORD`/`BYTE`/`BOOL` usage.
  - Translate Korean/Chinese comments to English.

### Phase 7 — Server: retire dead binaries — done 2026-04-18
- [x] Delete the dead top-level server trees. Audit (2026-04-18)
      confirms the following are not in any `add_subdirectory(...)`
      call reachable from `dkrixserver/CMakeLists.txt` and have no
      live CMake target anywhere:
      `cacheserver/`, `theoneserver/`, `updateserver/`,
      `chinabilling/stress/`, `chinabilling/testserver/`,
      `gameserver/mofus/testserver/`, `gameserver/testAlone/`.
  - **Correction to earlier wording:** `chinabilling/` top-level
    is **not** dead — it builds the `GameServerCBilling` and
    `LoginServerCBilling` static libs (linked by `gameserver` and
    `loginserver` via their CMake targets). Only its two subdirs
    (`stress/`, `testserver/`) retire, not the parent. "All three
    subdirs" in the previous wording was wrong on both count and
    contents.
  - Only stale references are in the legacy `src/**/Makefile`
    files (already orphaned from the CMake build). Those Makefiles
    are themselves candidates for a separate build-hygiene pass
    but are left in place for Phase 7 to keep scope narrow.
- [x] Delete the stub `testAlone/Mutex.h` — handled as part of the
      `gameserver/testAlone/` tree-delete above (the whole dir
      goes, so the stub goes with it).
- [x] Retire `__OLD_GUILD_WAR__` `#ifdef` gates. Audit shows 41
      gate occurrences across 26 files; `__OLD_GUILD_WAR__` is
      never defined in any `CMakeLists.txt`, `target_compile_def*`,
      or command line — the `#ifdef` branches are preprocessor-
      dead. Plan: delete the `#if[n]def __OLD_GUILD_WAR__ ... [#else
      ...] #endif` blocks keeping only the `#else` side (i.e. the
      "new guild war" code path that has been live since the flag
      was introduced). No runtime behaviour change.
- [x] Delete `.old.cpp` files in `server/database/`. Audit: one
      file, `server/database/DatabaseManager.old.cpp`, not in
      CMake. Single-file delete.

**Outcome (2026-04-18):**

| #    | Commit    | Subject                                                    |
| ---- | --------- | ---------------------------------------------------------- |
| 0001 | `31fc11c` | `docs: pin Phase 7 plan in MODERNIZATION.md`               |
| 0002 | `50196f5` | `server: 7B — delete 7 dead server trees (-18,905 lines)`  |
| 0003 | `baba038` | `server: 7C — delete DatabaseManager.old.cpp (-234 lines)` |
| 0004 | `53c8e93` | `server: 7D — retire __OLD_GUILD_WAR__ gates (-237 lines)` |
| 0005 | ` ------` | `docs: 7E — close out Phase 7 in MODERNIZATION.md`         |

Net code delta: **-19,376 lines** across 135 files. No behaviour
change — every path touched was already preprocessor-dead or
orphaned from the live CMake build. Verified with `grep -rl
'__OLD_GUILD_WAR__' dkrixserver/src/` returning zero matches after
7D, and with `grep -rn '#include.*\(cacheserver\|theoneserver\|
updateserver\|chinabilling/stress\|chinabilling/testserver\|mofus/
testserver\|gameserver/testAlone\|testAlone/Mutex\)'` returning
zero matches before 7B.

**Deferred from Phase 7 (narrow-scope reasons):**

- **Legacy `src/**/Makefile` files** that reference the deleted
  trees (e.g. `alltheoneserver`, `updateserver clean`). These
  aren't in the live CMake build but still make the tree messier
  than it has to be. Separate build-hygiene sweep — out of Phase
  7 scope.
- **`chinabilling/` modernisation.** The parent dir stays live
  (builds `GameServerCBilling` / `LoginServerCBilling`). It still
  uses Korean-encoded comments and legacy naming conventions that
  Phase 6 (touch-as-you-go) would clean up incrementally.
- Items previously deferred from Phases 3–5 remain deferred.

**Plan (sub-commits):**

- **7A — Pin plan in `MODERNIZATION.md`.** (This commit.)
- **7B — Delete the 7 dead server trees.** One `git rm -r`
  commit. No CMake edits needed because none of the seven dirs is
  in any `add_subdirectory(...)` call. Legacy `Makefile`
  references stay; they are orphaned from the live build already.
- **7C — Delete `DatabaseManager.old.cpp`.** Single-file `git rm`.
- **7D — Retire `__OLD_GUILD_WAR__` gates.** Walk the 26 files
  identified by `grep -rl '__OLD_GUILD_WAR__' dkrixserver/src/`
  and collapse each `#ifdef __OLD_GUILD_WAR__` / `#ifndef
  __OLD_GUILD_WAR__` block, keeping the non-defined branch. Purely
  mechanical dead-code removal.
- **7E — Close-out.** Flip this block to `done`, fill in outcome
  notes and commit table.

**Explicit non-goals for Phase 7:**

- Touching `chinabilling/` top-level. It's live; rename / modernise
  in a later phase if desired.
- Modernising the legacy `src/**/Makefile` files themselves. They
  reference the dead trees but aren't part of the live CMake
  build. Build-hygiene sweep, out of scope here.
- `testAlone/Mutex.h` "replace with real primitive" — the whole
  `testAlone/` dir is unreferenced dead code (it has its own
  `Mutex.h` shadowing the live `server/Mutex.h`), so the stub
  goes with the dir. Zero live call sites to port.

### Phase 8 — Server: SQL and secrets — done 2026-04-18
- [ ] Introduce a `PreparedStatement` wrapper over `mysql_stmt_*`.
      **Scope correction (2026-04-18 audit):** codebase has zero
      `mysql_stmt_*` usage today (grep -rn 'mysql_stmt_' → 0).
      Current data path is `Statement::executeQuery(fmt, ...)` →
      `vsprintf` into buffer → `mysql_real_query`. Adoption is a
      from-scratch API design, not a refactor. **Deferred to
      Phase 11.1** — see below.
- [ ] Migrate the `sprintf(query, ...)` sites module by module.
      **Scope correction (2026-04-18 audit):** the "~87" figure in
      the earlier plan was stale. Actual count in the live CMake
      build:
        - `grep -rnE 'executeQuery\s*\('` → **1,510 total call sites**
        - `grep -rnE 'executeQuery\s*\(\s*"[^"]*%[sd]'` →
          **625 sites with `%s` / `%d` format interpolation**
          (the SQL-injection surface).
      Migrating 625 sites — each to a prepared-statement wrapper
      that doesn't exist yet — is several weeks of work. **Deferred
      to Phase 11.2** as ongoing module-by-module work starting
      with user-string call sites (chat / say / whisper / pet names
      / custom options).
- [x] Add a CI grep-gate that fails on new injection-shaped
      `executeQuery("…%s…")` call sites in `dkrixserver/src/`.
      Script pinned to a line-count baseline (567 today — the
      stricter `%[sdluxc]` pattern plus exclusion of `*.backup`,
      `Query.txt`, and `testdb.cpp` trims the wider 625-count
      audit down to only compiled sites); fails only when the
      count grows. Lets existing sites stay uncleaned until
      Phase 11.2 while blocking regression. **(8C — 2026-04-18.)**
- [x] Move `DB_PASSWORD`, `DB_HOST`, `LoginServerIP`,
      `SharedServerIP`, `LogServerIP` — and any other
      `conf/*.conf` value — from plaintext into environment
      variables via `${VAR}` placeholder expansion in
      `Properties::load()`. Existing plaintext values pass through
      unchanged; any value wrapped in `${...}` is resolved via
      `getenv()` at load time. Template `.conf.template` files
      added alongside the originals. **(8B — 2026-04-18.)**

**Plan (sub-commits):**

- **8A — Pin corrected plan.** (This commit.)
- **8B — `${VAR}` env-var expansion in `Properties::load()`.**
  Single function change in `Core/Properties.cpp` plus
  `loginserver.conf.template` / `sharedserver.conf.template` /
  `excel96-gameserver.conf.template` showing the placeholder
  syntax for the five named secrets. Leaves existing `.conf`
  files untouched — operators migrate on their own schedule by
  editing their runtime config to use `${DKRIX_DB_PASSWORD}` etc.
- **8C — CI grep-gate script.** `dkrixserver/scripts/check-sql-injection.sh`
  that (a) counts `executeQuery("…%s/%d…")` sites, (b) compares
  to a pinned baseline in `dkrixserver/scripts/.sql-injection-baseline`,
  (c) exits non-zero if the count grew. Runnable standalone or
  from a CI workflow.
- **8D — Close-out.** Flip this block to `done`, outcome block,
  commit table, and the formal Phase-11 deferral record.

**Explicit non-goals for Phase 8:**

- **Building the `PreparedStatement` class itself.** The wrapper
  needs `mysql_stmt_init` / `mysql_stmt_prepare` /
  `mysql_stmt_bind_param` / `mysql_stmt_execute` with parameter
  binding helpers for each type we pass today (`int`, `long`,
  `string`, `time_t`). That's a designed API, not a find-replace
  — recorded as Phase 11.1 and intentionally not shipped here
  without a way to test it (no MySQL headers in the sandbox).
- **Migrating the 625 injection-shaped sites.** Incremental
  module-by-module work in Phase 11.2. The CI gate added in 8C
  prevents regression while those sites await migration.
- **Rewriting any `.conf` file in place.** Operators may still
  have hand-tuned configurations; rewriting them risks losing
  those customisations. 8B is strictly additive — the parser now
  understands `${VAR}`, the files can use it when an operator
  decides to.

**Outcome (2026-04-18):**

| #    | Commit    | Subject                                                        |
| ---- | --------- | -------------------------------------------------------------- |
| 0001 | `03e0e58` | `docs: pin Phase 8 plan (with scope correction) + add Phase 11`|
| 0002 | `0971ec0` | `server: 8B — expand ${VAR} env-var placeholders`              |
| 0003 | `1642dcd` | `server: 8C — add SQL-injection ratchet gate (baseline 567)`   |
| 0004 | `TBD`     | `docs: 8D — close out Phase 8 in MODERNIZATION.md`             |

Net delta: **+415 lines across 7 files** (pure addition — no
existing files rewritten). The bulk is 3 new `.conf.template`
files (~175 lines), the `check-sql-injection.sh` gate (~100
lines), 65 lines of new C++ in `Properties.cpp`, and the updated
Phase 8 + Phase 11 blocks in this file.

What shipped vs. what was deferred:

- **Shipped (8B):** `${VAR}` expansion in `Properties::load()`
  with the "missing = literal + stderr warning" rule, plus three
  template `.conf` files (loginserver / sharedserver /
  excel96-gameserver) demonstrating the named-secret pattern for
  DB credentials, log-server IP, billing IPs, and monitor-client
  IP. Backward-compatible: existing plaintext `.conf` files parse
  identically.
- **Shipped (8C):** POSIX-sh ratchet gate with a 567-site
  baseline on the variadic SQL API. Gates `check`, `--count`,
  `--list`, `--update`. Stops new injection-shaped call sites
  from landing while the existing surface awaits migration.
- **Deferred to Phase 11.1:** PreparedStatement wrapper over
  `mysql_stmt_*`. Not a refactor — zero existing `mysql_stmt_*`
  usage in the tree — so this is a designed API to build, not a
  find-replace. Non-shippable in the sandbox without MySQL
  headers to link against.
- **Deferred to Phase 11.2:** migration of the 567 injection-
  shaped sites. Incremental module-by-module work, prioritising
  player-string → SQL paths (chat / say / whisper / nicknames /
  custom options) first.

Deployment notes for operators:

1. Copy the `.template` files alongside the live configs (they do
   NOT replace existing configs — operators choose the cut-over).
2. Export `DKRIX_*` env vars in the service unit / init script
   (systemd `Environment=`, or shell `export` in the launcher).
3. Start the server. If any `${VAR}` is unset, `Properties.cpp`
   prints a stderr warning naming the missing var and leaves the
   literal placeholder in the parsed value — the first SQL
   connect or IP dial will then fail visibly rather than the
   server silently reading an empty credential.

### Phase 9 — Server: Lua sandbox and packet schema — done 2026-04-19
- [x] Replace `luaL_openlibs()` with a whitelist (`base`, `table`,
      `string`, `math`); drop `io`, `os`, `debug`, `package`,
      `coroutine`. **Scope audit (2026-04-19):** single call site
      at `dkrixserver/src/server/gameserver/quest/luaScript/LuaState.cpp:27`.
      Tractable in one commit. **Shipped in 9B (`5c2c28f`).**
- [x] Wrap every `lua_to*` → enum cast in a range-checked helper.
      **Scope audit (2026-04-19):** only four call sites, all in
      `LuaSelectItem.cpp::executeFile()` (lines 21–24, reading
      `ItemClass` / `ItemType` / `OptionType` / `OptionType2` from
      the Lua stack via `lua_tonumber` + C-style cast). Tractable
      in one commit. **Shipped in 9C (`499a33d`).**
- [ ] Share packet definitions between client and server.
      **Scope audit (2026-04-19) — DEFERRED TO PHASE 12.** The
      current state is 149 `.cpp/.h` files under
      `dkrix/Client/Packet/` plus another 326 under
      `dkrix/Client/Packet/Cpackets/`, near-duplicated against
      ~800 matching files under `dkrixserver/src/Core/` (CG*, GC*,
      CL*, LC*, GS*, SG*, GT*, TG*). Spot-check of
      `CGAbsorbSoul.{h,cpp}` shows the pair differs only in
      whitespace, exception-spec, and comment formatting — the
      wire format is identical. Unifying them requires either:
      (a) promoting one tree to canonical + deleting the other +
      adjusting both builds, or (b) extracting a third shared
      submodule. Both are multi-day projects. Booked as Phase 12.
- [ ] Add endian-safe read/write primitives in
      `SocketInputStream` / `SocketOutputStream`.
      **Scope audit (2026-04-19) — DEFERRED TO PHASE 13.** The
      current `template<T> read(T&)` does `buf = *(T*)(m_Buffer +
      m_Head)` — a raw host-endian cast
      (`dkrixserver/src/Core/SocketInputStream.h:159`). Zero
      `ntoh*/hton*/htobe*/betoh*` calls in the tree. Both server
      and desktop client are x86 little-endian, so wire format is
      implicitly "LE host order, same as everyone else on the
      wire." Making this endian-safe requires: (1) choosing a
      wire byte order (LE keeps compat, BE breaks every existing
      client build), (2) adding `htole*`/`le*toh` wrappers around
      every primitive read/write, (3) deciding what to do about
      struct-copy reads like `buf = *(Packet*)(buffer)` which
      happen inside the packet classes themselves, not just the
      stream API. Not a single-commit change. Booked as Phase 13.

**Plan (sub-commits):**

- **9A — Pin corrected plan.** (This commit.) Records the audit
  findings that shrink Phase 9 to just the two cheap wins (Lua
  sandbox + lua_to* wrappers) and books the two expensive items
  (packet sharing, endian-safe I/O) as separate phases.
- **9B — Lua library whitelist.** Replace the single
  `luaL_openlibs(m_pState)` call in
  `dkrixserver/src/server/gameserver/quest/luaScript/LuaState.cpp`
  with explicit `luaopen_base` / `luaopen_table` / `luaopen_string`
  / `luaopen_math` via the Lua 5.1 `lua_pushcfunction` + `lua_call`
  idiom. Drop `io`, `os`, `debug`, `package`, `coroutine`. Quest
  scripts run in a locked-down VM that can't touch the filesystem,
  spawn processes, or escape via `require`.
- **9C — lua_to* range-checked enum helper.** New
  `lua_toboundedenum<T>(L, idx, min, max)` inline template in
  `LuaState.h` that throws `InvalidProtocolException` on out-of-
  range values. Migrate the four `(Enum)lua_tonumber(...)` sites
  in `LuaSelectItem.cpp::executeFile`.
- **9D — Close-out.** Flip this block to `done`, outcome block,
  commit table, explicit deferral record for Phase 12 + Phase 13.

**Explicit non-goals for Phase 9:**

- **Packet schema unification.** See Phase 12 block below for
  the full scope. 9 doesn't touch either packet tree.
- **Endian-safe socket stream primitives.** See Phase 13 block
  below. 9 leaves the existing raw-cast `read<T>` / write paths
  untouched.
- **Sandboxing beyond library whitelist.** No instruction count
  cap, no memory ceiling, no `lua_setreadonly` on globals —
  those are policy knobs that need a test harness to exercise,
  and are out of scope here.

**Outcome (2026-04-19):**

| #    | Commit    | Subject                                                                 |
| ---- | --------- | ----------------------------------------------------------------------- |
| 0001 | `03631d2` | `docs: pin Phase 9 plan (with scope correction) + add Phase 12 + Phase 13` |
| 0002 | `5c2c28f` | `server: 9B — Lua sandbox whitelist (drop io/os/debug/package)`         |
| 0003 | `499a33d` | `server: 9C — lua_toboundedenum<T> helper + migrate LuaSelectItem`      |
| 0004 | `99bc7b5` | `docs: 9D — close out Phase 9 in MODERNIZATION.md`                      |

Net delta: **+239 lines / -20 lines across 4 files**. The bulk is
the docs block (~143 lines), the new inline `lua_toboundedenum<T>`
template + comment in `LuaState.h` (~48 lines), the whitelist
rewrite in `LuaState.cpp` (+51 / -35), and the four migrated call
sites in `LuaSelectItem.cpp` (+17 / -10).

What shipped vs. what was deferred:

- **Shipped (9B):** Single-call-site swap of `luaL_openlibs()` for
  an explicit `luaL_Reg`-driven whitelist (`luaopen_base`,
  `luaopen_table`, `luaopen_string`, `luaopen_math`) using the Lua
  5.1 `lua_pushcfunction` + `lua_call` pattern from `linit.c`.
  Quest scripts can no longer `require`, touch the filesystem
  (`io.*`), shell out (`os.execute`), introspect/patch the VM
  (`debug.*`), or load C modules (`package.loadlib`). Base, table,
  string, math — the four libs actually referenced by quest Lua —
  remain available.
- **Shipped (9C):** New inline `lua_toboundedenum<T>(L, idx, min,
  max)` template in `LuaState.h` that throws
  `InvalidProtocolException` with slot index / actual value /
  legal range in the message on non-numeric or out-of-range
  stack slots. Migrated all four call sites in
  `LuaSelectItem.cpp::executeFile`:
  - `m_ItemClass` bounded `[0, Item::ITEM_CLASS_MAX - 1]`
  - `m_ItemType` bounded `[0, 0xFFFF]` (WORD max)
  - `m_OptionType` / `m_OptionType2` bounded `[0, 0xFF]` (BYTE max)
  Confirmed no other `lua_tonumber` + enum-cast sites remain in
  `dkrixserver/src`.
- **Deferred to Phase 12:** packet schema unification across the
  ~475-file client tree (`dkrix/Client/Packet/` + Cpackets) and
  the ~800-file server tree (`dkrixserver/src/Core/` CG/GC/CL/LC/
  GS/SG/GT/TG). Near-duplicated but not a find-replace — requires
  choosing a canonical tree plus cross-build wiring.
- **Deferred to Phase 13:** endian-safe wire I/O. Current
  `template<T> read(T&)` in `SocketInputStream.h:159` does a raw
  host-endian cast. Making this endian-safe requires picking a
  wire byte order, adding `htole*`/`le*toh` wrappers around every
  primitive stream op, and deciding what to do about struct-copy
  reads inside packet classes themselves — not a single-commit
  change.

Deployment notes for operators:

1. **No config or operational changes required.** The Lua
   whitelist is a compile-time change; quest scripts that use
   only base/table/string/math continue to load identically.
2. **Quest scripts that call `io.*`, `os.*`, `require`, or
   `debug.*` will now error at load time with the usual Lua
   "attempt to index global 'io'" message.** If any production
   quest was relying on these — audit pre-deploy. No in-tree
   quest script found that does.
3. **Quest scripts feeding out-of-range enum values to
   `LuaSelectItem` now throw `InvalidProtocolException` at the
   quest-load site** rather than silently wrapping or corrupting
   the dispatch table. Error message names slot index + value +
   legal range for quick triage.

### Phase 10 — Build hygiene & CI — done 2026-04-19
- [x] Add `.clang-format` to `dkrix/` and a `Makefile` with
      `fmt` / `fmt-check` / `fmt-check-all` targets mirroring
      the server Makefile. **Scope audit (2026-04-19):** client
      tree has no `.clang-format` today and no `Makefile`
      (only `build_and_run_effect_viewer.sh`). The "remove the
      TODO stubs" phrasing in the original plan was aspirational
      — there are no actual `TODO`-marked fmt stubs in the
      client tree. Tractable in one commit: copy the server's
      `.clang-format` (tested, 1468-byte LLVM-derived config
      already used in production), add a client `Makefile` with
      the same `fmt` / `fmt-check` / `fmt-check-all` targets.
      **Shipped in 10B (`a760899`).**
- [x] Both trees: `.gitignore` for `build/`, `compile_commands.json`,
      editor detritus, and `git rm --cached` the one tracked
      `compile_commands.json`. **Scope audit (2026-04-19):** gap
      map is narrow — client `.gitignore` has `compile_commands.json`
      and `.cache` but is missing `.vscode/` / `.idea/` / `*.swp` /
      `*.swo` / `*~`; server `.gitignore` has `.vscode/` / `.idea/`
      / editor swap files but is missing `.cache` / `compile_commands.json`.
      Client tree also has `dkrix/Client/compile_commands.json`
      (3.3 MB) tracked in spite of its own `.gitignore` rule —
      added before the rule existed (see `df4895e`). Needs
      `git rm --cached`. Single commit.
      **Shipped in 10C (`ddd8654`).**
- [ ] Replace `file(GLOB …)` with explicit source lists in the
      client CMake. **Scope audit (2026-04-19) — DEFERRED TO
      PHASE 14.** Two `file(GLOB)` call sites in
      `dkrix/CMakeLists.txt`: line 163 globs `VS_UI/src/*.cpp` +
      `VS_UI/*.cpp` (56 files), line 482 globs `Client/*.cpp` +
      `Client/Packet/*.cpp` + `Client/Packet/**/*.cpp` +
      `Client/SXml/*.cpp` + `Client/WinLib/*.cpp` (1045 files).
      Explicit listing means enumerating ~1100 files in CMake —
      doable but noisy, and every new `.cpp` becomes a CMake
      edit. Has real value (CMake doesn't re-glob on incremental
      builds, so adding a file requires reconfiguring), but not
      a single-commit change. Booked as Phase 14.
- [ ] Add a GitHub Actions matrix that runs `make debug-asan` on
      Linux and macOS. **Scope audit (2026-04-19) — DEFERRED TO
      PHASE 15.** Server has one workflow today
      (`.github/workflows/format-check.yml` — clang-format diff
      gate); client has no `.github/` directory. Neither tree
      has a `make debug-asan` target (server Makefile has `debug`
      without sanitizers; the client has no Makefile). Building
      requires: (a) adding `debug-asan` Makefile targets that
      forward to cmake's `-DUSE_ASAN=ON`, (b) CI runner install
      scripts for every transitive dep (MySQL headers + libmysql
      for server, SDL2 + Lua 5.1 + libpng + libjpeg for client),
      (c) the matrix workflow YAML, (d) deciding what counts as
      a passing build given the tree still has MSVC-unsuitable
      files (see the Windows MSVC deferral below). Not a single-
      commit change. Booked as Phase 15.
- [ ] **Windows MSVC build** — the SDL backend port was never
      validated on MSVC; attempting a build on Windows during 4A
      verification surfaced ~2,700 pre-existing errors, all
      upstream of Phase 4 code:
      - `basic/Platform.h` redefines `ULONG`/`DWORD`/`LONG`/
        `HRESULT` etc. as fallback typedefs, which collide with
        the real Windows SDK (`winnt.h`, `wingdi.h`, `synchapi.h`)
        and cascade into `<ratio>`/`<chrono>` C-linkage breakage
        (~1,900 errors).
      - `tools/engine/sprite/src/frame.c` + `framepack.c` use
        GCC's `typeof()` extension (~40 errors). Moot once Phase
        4D lands — engine tree goes to `docs/archive/`.
      - `Client/SpriteLib/CTypePack.h` includes pre-standard
        `fstream.h` (17 errors). Change to `<fstream>`.
      - `Client/DebugLog.cpp` includes `sys/time.h` (1 error).
        Needs a `_WIN32` branch.
      Fix is guarded by proper `#ifdef _WIN32` / SDK include order
      in Platform.h, not in scope for any of Phases 4–10.

**Plan (sub-commits):**

- **10A — Pin corrected plan.** (This commit.) Records the audit
  findings that narrow Phase 10 to the two cheap wins (client
  clang-format + fmt targets, gitignore unification) and books
  the two expensive items (explicit CMake source lists, CI
  matrix) as Phase 14 and Phase 15.
- **10B — Client clang-format + Makefile fmt targets.** Copy
  `dkrixserver/.clang-format` to `dkrix/.clang-format`; add
  `dkrix/Makefile` with `fmt` / `fmt-check` / `fmt-check-all`
  targets wrapping `clang-format` over `Client/` + `VS_UI/` +
  `basic/`. Does NOT run clang-format over the tree in this
  commit — format-fixing the ~1100-file client tree in a single
  commit would obliterate `git blame` for the next five years.
  The tool + recipe ship now; operators choose when to apply.
- **10C — gitignore unification + un-track stray
  compile_commands.json.** Fill in the missing rules on each
  side (client: editor detritus; server: `.cache` +
  `compile_commands.json`). `git rm --cached
  dkrix/Client/compile_commands.json` to stop tracking the
  stale 3.3 MB artifact; `.gitignore` rule keeps it from being
  re-added.
- **10D — Close-out.** Flip this block to `done`, outcome
  block, commit table, explicit deferral record for Phase 14 +
  Phase 15.

**Explicit non-goals for Phase 10:**

- **Running clang-format over the client tree.** Tool ships,
  blame-preserving choice to apply is operator's. A bulk-format
  commit is an independent operational decision.
- **Replacing `file(GLOB)` in client CMake.** See Phase 14.
- **CI build matrix.** See Phase 15.
- **Windows MSVC build.** Still deferred (see block above).

**Outcome (2026-04-19):**

| #    | Commit    | Subject                                                                  |
| ---- | --------- | ------------------------------------------------------------------------ |
| 0001 | `50516f1` | `docs: pin Phase 10 plan (with scope correction) + add Phase 14 + Phase 15` |
| 0002 | `a760899` | `client: 10B — add .clang-format + Makefile with fmt/fmt-check targets`  |
| 0003 | `ddd8654` | `10C — gitignore unification + un-track stale compile_commands.json`     |
| 0004 | `89e784c` | `docs: 10D — close out Phase 10 in MODERNIZATION.md`                     |

Net delta: **+279 lines / -7348 lines across 6 files**. The
-7348 is almost entirely the un-tracked
`dkrix/Client/compile_commands.json` (-7339 lines); real
additive content is ~+279 lines — the docs plan/close-out
(~+175 lines), the new client `.clang-format` (+54), the client
`Makefile` (+85), the `.gitignore` edits on both trees (+10 net).

What shipped vs. what was deferred:

- **Shipped (10B):** `dkrix/.clang-format` (verbatim copy of
  `dkrixserver/.clang-format` — LLVM-derived, 4-space indent,
  120-col wrap, `PointerAlignment: Left`) plus `dkrix/Makefile`
  with `fmt` / `fmt-check` / `fmt-check-all` targets that mirror
  the server Makefile's recipes. `FMT_DIRS` spans
  `Client VS_UI basic tools` — everything CMake compiles plus
  the Windows-only sources that are currently `list(FILTER
  EXCLUDE)`'d. Narrow `.gitignore` tweak (`!/Makefile` anchor)
  included so the top-level Makefile tracks while the existing
  bare `Makefile` rule still hides any in-source cmake-generated
  Makefile.
- **Shipped (10C):** Gap-fill between the two `.gitignore` files.
  Client gained `.vscode/`, `.idea/`, `*.swp`, `*.swo`, `*~`;
  server gained `.cache` and `compile_commands.json`. Untracked
  the 3.3 MB `dkrix/Client/compile_commands.json` via
  `git rm --cached` — file stays on disk as a regenerated build
  artifact; the existing `.gitignore` rule prevents re-add.
- **Deferred to Phase 14:** Explicit CMake source lists to
  replace the two `file(GLOB)` sites in `dkrix/CMakeLists.txt`
  covering ~1100 `.cpp` files. Real value (CMake globs aren't
  re-evaluated on incremental builds) but not a single-commit
  change.
- **Deferred to Phase 15:** CI build matrix. Neither tree has
  `make debug-asan` targets, dep-install scripting, or a build
  workflow YAML today. Multi-commit infrastructure project.
- **Still deferred:** Windows MSVC build (was and remains out
  of scope — the 2,700-error analysis in the Phase 10 block
  above is retained verbatim).

Deployment notes for operators:

1. **`make fmt` is opt-in.** 10B ships the recipe; no mass-
   format commit was run. Operators choose when (and if) to
   bulk-format the client tree — `git blame` stays usable until
   then.
2. **`make fmt-check` is the CI-friendly target.** Matches what
   the existing server `format-check.yml` workflow invokes; the
   same gate extends to the client tree when Phase 15 lands the
   matrix build.
3. **Old local clones may still have
   `dkrix/Client/compile_commands.json` tracked.** After pulling
   10C, a `git pull` will delete the file from the index (it's
   now ignored). The file remains on disk and any new
   incremental builds regenerate it.

### Phase 14 — Deterministic CMake source lists — done 2026-04-19
Deferred 2026-04-19 from Phase 10 after scope audit showed two
`file(GLOB)` sites in `dkrix/CMakeLists.txt` together covering
~1100 `.cpp` files. CMake globs are not re-evaluated on
incremental builds — adding a new source file silently fails
to compile until the next `cmake` reconfigure — so the payoff
is real (deterministic builds, no mystery-silent-miss), but the
cost is noise (every new `.cpp` becomes a CMake edit).

- [x] Replace `file(GLOB_RECURSE VS_UI_SRC_SOURCES
      VS_UI/src/*.cpp VS_UI/*.cpp)` at
      `dkrix/CMakeLists.txt:163` with an explicit list of the
      56 files. Keep the `list(FILTER EXCLUDE)` windows-specific
      drops in place — those are independent of how the list is
      built. **Scope audit (2026-04-19):** 50 files under
      `VS_UI/src/` + 6 under `VS_UI/` root (`find VS_UI -name
      "*.cpp" | sort | wc -l` ⇒ 56). List is deterministic
      (sorted `find` output). **Shipped in 14B (`58154fb`)**:
      56-file explicit list, byte-for-byte equivalent to the old
      glob.
- [x] ~~Replace the larger `file(GLOB CLIENT_MAIN_SOURCES ...)`
      at `dkrix/CMakeLists.txt:482` with an explicit list of the
      ~1045 files.~~ **Pivoted in 14C (`e2357e9`) to
      `CONFIGURE_DEPENDS`** after mid-plan cost/benefit re-
      evaluation — see Outcome block below. The 1045-file
      explicit list would have added ~1060 lines of CMake and
      required an edit for every new `Client/Packet/**` file;
      `CONFIGURE_DEPENDS` (CMake 3.12+) solves the same silent-
      miss bug automatically with no per-file maintenance cost.
      Required bumping `cmake_minimum_required` from 3.10 → 3.12.
- [x] Document the "add a new `.cpp` → edit CMakeLists.txt"
      workflow in `dkrix/CLAUDE.md` so future contributors
      don't hit the silent-miss trap the globs used to hide.
      **Scope reduced in 14D** to match the pivot: only the
      VS_UI source list requires a manual CMake edit now; the
      Client list is auto-refreshed by `CONFIGURE_DEPENDS`. The
      note reflects that split.

**Plan (sub-commits):**

- **14A — Pin plan.** Adds the sub-audit numbers for the two
  glob expansions (56 VS_UI files, 1045 Client files) and sets
  the order of work below.
- **14B — VS_UI glob → explicit list.** Replace the 56-file
  `GLOB_RECURSE` at `dkrix/CMakeLists.txt:163`. Smaller, easier
  to review first.
- **14C — Client glob → `CONFIGURE_DEPENDS`** *(pivot from
  "explicit list")*. Mid-execution, evaluated the cost of
  enumerating 1045 files (~1060 lines of CMake + per-packet
  edits forever after) against the benefit (fixing CMake's
  silent-miss-on-new-file bug). Picked `CONFIGURE_DEPENDS`
  (CMake 3.12+) instead, which solves the same bug with zero
  per-file maintenance cost. Bumped
  `cmake_minimum_required(3.10)` → `cmake_minimum_required(3.12)`;
  3.12 shipped 2018-07 so every mainstream distro already has it.
- **14D — Close-out + CLAUDE.md workflow note.** Flip to
  `done`, outcome block documenting the pivot, commit table.
  Append an "Adding new source files" section to
  `dkrix/CLAUDE.md` — reduced scope relative to the original
  plan: VS_UI requires a manual CMake edit, Client is auto-
  picked-up by `CONFIGURE_DEPENDS`.

**Verification strategy (in lieu of a CMake configure run):**

The sandbox lacks the client deps (SDL2, Lua 5.1, libpng,
libjpeg) needed to run `cmake` on `dkrix/`. Instead, each
sub-commit verifies file-set equivalence by:

1. Regenerating the list with `find <root> -name "*.cpp" | sort`
   and diffing against the committed explicit list. Byte-for-
   byte equivalent ⇒ the CMake target sees the same set of
   source files it saw before the swap.
2. Confirming the surrounding `list(FILTER EXCLUDE)` /
   `list(REMOVE_ITEM)` pipeline is untouched, so the downstream
   filter behavior is unchanged.

Operators with a configured client tree should verify by running
`cmake --build build` after applying the patches — the diff
between pre-14 and post-14 build objects should be zero.

**Explicit non-goals for Phase 14:**

- **Splitting the client executable into static libraries.**
  The current monolithic build is preserved. Library factoring
  is an independent, much larger refactor.
- **Removing the `list(FILTER EXCLUDE)` windows-specific
  drops.** Those filters are part of how the build selects
  cross-platform sources; they stay in place in 14B/14C.
- **Touching the server CMake.** `dkrixserver/CMakeLists.txt`
  uses no globs today (checked in 10 audit) — nothing to fix.

**Outcome (2026-04-19):**

| #    | Commit    | Subject                                                                     |
| ---- | --------- | --------------------------------------------------------------------------- |
| 0001 | `5575387` | `docs: pin Phase 14 plan (explicit CMake source lists)`                     |
| 0002 | `58154fb` | `client: 14B — VS_UI glob → explicit source list (56 files)`                |
| 0003 | `e2357e9` | `client: 14C — Client glob → CONFIGURE_DEPENDS (cmake 3.12+)`               |
| 0004 | `0a004c8` | `docs: 14D — close out Phase 14 in MODERNIZATION.md`                        |

Net delta: **+161 lines / -10 lines across 2 files**
(`dkrix/CMakeLists.txt` +107/-3, `docs/MODERNIZATION.md` +64/-10).
Source files on disk: unchanged — 14B/14C are pure build-
infrastructure edits.

Mid-plan pivot (14C): explicit list → `CONFIGURE_DEPENDS`
---------------------------------------------------------
The plan called for swapping both `file(GLOB)` sites for
explicit source lists. 14B (VS_UI, 56 files) landed that way
as planned. Mid-14C, the cost/benefit of applying the same
pattern to the 1045-file Client glob was re-evaluated:

- **Cost of explicit enumeration at Client scope:** ~1060
  additional lines of CMake, plus a CMake edit every time a
  new `.cpp` lands anywhere under `Client/Packet/**`
  (6 sub-packages, actively grown) or `Client/` root. The
  Packet tree alone is 801 files across 6 sub-packages.
- **Benefit the plan was reaching for:** kill the "add a new
  source file → silent miss until `cmake` reconfigure" bug.
- **Better tool for the same benefit:** `file(GLOB ...
  CONFIGURE_DEPENDS)` (CMake 3.12+) makes CMake re-evaluate
  the glob on every build — same bug-fix, zero per-file
  maintenance cost.

14C therefore:

1. Added `CONFIGURE_DEPENDS` to the Client glob at
   `dkrix/CMakeLists.txt:572` (was line 482 before the
   14B/14C header/comment expansion).
2. Bumped `cmake_minimum_required(VERSION 3.10)` →
   `cmake_minimum_required(VERSION 3.12)`. 3.12 shipped
   2018-07; every mainstream distro has it (Ubuntu 20.04+,
   Debian 11+, macOS Homebrew, RHEL 8+ via EPEL). Only
   Ubuntu 18.04 LTS with system cmake sees the bump
   requirement, and 18.04 is EOL for standard support as of
   2023.
3. Left VS_UI's explicit list (14B, 56 files) in place —
   small, stable, and the audit value of "a reviewer can see
   every source file at a glance" still holds at that size.

Result: asymmetric treatment of the two globs, justified by
asymmetric maintenance cost. VS_UI's manual-edit workflow is
documented in `dkrix/CLAUDE.md`; Client's auto-refresh
behavior is comment-documented at the glob site itself.

What shipped vs. what was pivoted
---------------------------------

- **Shipped (14B):** `dkrix/CMakeLists.txt:163` replaced
  `file(GLOB_RECURSE VS_UI_SRC_SOURCES VS_UI/src/*.cpp
  VS_UI/*.cpp)` with an explicit `set(VS_UI_SRC_SOURCES ...)`
  enumerating 56 files. `find VS_UI -name "*.cpp" | sort`
  produces byte-for-byte the same list. `list(FILTER EXCLUDE)`
  pipeline untouched.
- **Shipped (14C), pivoted scope:** `dkrix/CMakeLists.txt:572`
  `file(GLOB CLIENT_MAIN_SOURCES ...)` → `file(GLOB
  CLIENT_MAIN_SOURCES CONFIGURE_DEPENDS ...)`. Same glob
  patterns (`Client/*.cpp Client/Packet/*.cpp
  Client/Packet/**/*.cpp Client/SXml/*.cpp
  Client/WinLib/*.cpp`), same resulting file set, but CMake
  now re-runs the glob on every build instead of caching it
  at initial configure. `cmake_minimum_required` bumped
  3.10 → 3.12 as a side-effect.
- **Shipped (14D), scope-reduced:** `dkrix/CLAUDE.md` workflow
  note now covers only VS_UI's manual-edit path. The original
  plan called for documenting "edit the list when you add a
  new `.cpp`" globally; after the 14C pivot, that requirement
  only applies to VS_UI.

Deployment notes for operators:

1. **CMake 3.12 minimum.** `cmake_minimum_required` bumped
   3.10 → 3.12. Every mainstream distro ships ≥ 3.12
   (Ubuntu 20.04+, Debian 11+, RHEL 8+, macOS Homebrew
   current). Check with `cmake --version` if in doubt; the
   configure step will fail with a clear version error
   otherwise.
2. **First build after pull re-globs automatically.** Adding
   or deleting a `.cpp` under `Client/` no longer needs a
   manual `rm -rf build/` or `cmake` reconfigure — the next
   `cmake --build build` sees the change via
   `CONFIGURE_DEPENDS`. VS_UI still needs an explicit edit
   of `dkrix/CMakeLists.txt:163` (documented in
   `dkrix/CLAUDE.md`).
3. **Build object diff should be zero.** The glob patterns
   in 14C are identical to pre-Phase 14; only the CMake-side
   caching behavior changed. `cmake --build build` against a
   checkout of the tree should produce the same object files
   before and after Phase 14. The 14B VS_UI swap was
   verified byte-for-byte via `find | sort`.

### Phase 15 — CI build matrix (deferred from Phase 10)
Deferred 2026-04-19 from Phase 10. Server has one workflow today
(clang-format diff gate); client has no CI. Adding a real build
matrix means landing infrastructure, not just a YAML file.

- [ ] Add `debug-asan`, `debug-tsan`, `debug-ubsan` Makefile
      targets to both trees that forward to CMake with the
      existing `USE_ASAN` / `USE_TSAN` / `USE_UBSAN` options
      already wired in `dkrix/CMakeLists.txt:23-25`.
- [ ] Script the server's CI toolchain install: `libmysqlclient-dev`,
      `liblua5.1-0-dev`, `libboost-dev` (which headers the server
      actually consumes needs a sub-audit).
- [ ] Script the client's CI toolchain install: `libsdl2-dev`,
      `libsdl2-mixer-dev`, `libsdl2-image-dev`, `liblua5.1-0-dev`,
      `libpng-dev`, `libjpeg-dev`, plus macOS `brew install`
      equivalents.
- [ ] Write `.github/workflows/build.yml` at the repo root (not
      per-tree) with a matrix `{os: [ubuntu-latest, macos-latest],
      sanitizer: [none, asan, ubsan]}`. Keep TSan out of the
      default matrix — runs slower and produces too much noise
      on an un-audited codebase.

### Phase 11 — SQL injection remediation (deferred from Phase 8)
Deferred 2026-04-18 from Phase 8 after scope audit showed 625
`executeQuery("…%[sd]…")` call sites (not the ~87 originally
estimated) and zero existing `mysql_stmt_*` infrastructure to
migrate onto. Two sub-phases:

- [ ] **11.1 — `PreparedStatement` wrapper API.** One-off design
      work. Expected shape (not binding):
      ```cpp
      class PreparedStatement {
      public:
          PreparedStatement(Connection*, const string& sqlWithQMarks);
          void bindInt   (int idx, int v);
          void bindLong  (int idx, long v);
          void bindString(int idx, const string& v);
          void bindTime  (int idx, time_t t);
          Result* execute();
          ~PreparedStatement();
      };
      ```
      Wraps `mysql_stmt_init` / `mysql_stmt_prepare` /
      `mysql_stmt_bind_param` / `mysql_stmt_execute` /
      `mysql_stmt_bind_result` / `mysql_stmt_fetch`. Lives in
      `dkrixserver/src/server/database/PreparedStatement.{h,cpp}`.
      Should be testable against a real MySQL instance before
      being handed to 11.2. Blocker for 11.2.
- [ ] **11.2 — Migrate 567 injection-shaped call sites.** Ongoing
      module-by-module work after 11.1 lands. The tighter 567
      number is what the 8C ratchet tracks (compiled `.cpp`/`.h`/
      `.inl` only, `%[sdluxc]` format set); the wider 625 from the
      original audit also counted `*.backup`, `Query.txt`, and
      `testdb.cpp`, none of which are in the live build.
      Priority order:
      1. User-string interpolation sites (chat, say, whisper, pet
         names, custom options) — ~180 sites, biggest blast radius.
      2. Operator-trusted interpolation (admin commands, log
         lookups) — ~200 sites, lower risk but still value from
         uniformity.
      3. Constant / numeric-only interpolation (IDs, enums cast
         to int) — remainder, pure hygiene.
      The CI gate from Phase 8C will drive the count down as
      migrations land. When the baseline reaches 0, the gate flips
      from "count didn't grow" to "zero matches".

### Phase 12 — Packet schema unification (deferred from Phase 9)
Deferred 2026-04-19 from Phase 9 after scope audit. Today the
packet definitions live in two places:

- `dkrix/Client/Packet/` (149 files) + `dkrix/Client/Packet/Cpackets/`
  (326 files) on the client side.
- `dkrixserver/src/Core/` — ~800 `CG*/GC*/CL*/LC*/GS*/SG*/GT*/TG*`
  `.cpp`/`.h` pairs mixed with the server-side `*Handler.cpp`
  files.

Spot-check diff on `CGAbsorbSoul.{h,cpp}` pair shows the two
copies differ only in whitespace, exception-specs, and comment
formatting — byte layout on the wire is identical. So the
duplication is a maintenance burden, not a protocol divergence.

- [ ] **12.1 — Canonicalise one tree + delete the other.**
      Pick the server copy as canonical (it's the more-edited
      tree and has richer `*Handler.cpp` companions). Move
      `dkrixserver/src/Core/*{CG,GC,CL,LC,GS,SG,GT,TG}*.{cpp,h}`
      to a new `shared/Packets/` at the repo root. Rewrite both
      builds to consume it.
- [ ] **12.2 — Strip client-tree duplicates + fix includes.**
      Delete `dkrix/Client/Packet/Cpackets/` and any file in
      `dkrix/Client/Packet/` whose name matches a moved file.
      Update client `#include` paths to the new location.
- [ ] **12.3 — CI structural check.** A grep-gate that fails if
      a matching-name pair ever reappears in both trees.

Blocker: Phase 10 (build hygiene) should land first — moving
headers triggers CMake glob updates and the client still uses
`file(GLOB)` today.

### Phase 13 — Endian-safe wire I/O (deferred from Phase 9)
Deferred 2026-04-19 from Phase 9. Current state: the socket
stream API's `template<T> read(T&)` does `buf = *(T*)(buffer +
head)` — host-endian raw cast. Works everywhere on x86 (server +
desktop client both LE), but quietly breaks on any BE port and
gives no protection against partial-struct tears if `T` is a
compound type.

- [ ] **13.1 — Pick and document a wire byte order.** LE keeps
      compatibility with every existing client binary. BE is
      "network order" but would require every deployed client to
      be rebuilt and re-shipped. Default recommendation: LE, with
      explicit `htole*` / `le*toh` wrappers documenting the
      choice. Decision gets recorded here when taken.
- [ ] **13.2 — Add endian-safe primitives.** Inline helpers in
      `Core/Endian.h` wrapping `htole16/32/64` +
      `le16/32/64toh`. For platforms without `<endian.h>`
      (macOS, mingw), provide an `#ifdef` fallback using byte-
      swap intrinsics.
- [ ] **13.3 — Migrate the primitive reads/writes.** The
      `read<T>` / write<T> templates become `read/write` of
      primitive types routing through the helpers; compound
      types continue to use memberwise `read`/`write` of their
      fields (packet classes already do this pattern manually).
- [ ] **13.4 — Audit for remaining raw casts.** Any surviving
      `*(T*)(...)` over stream-buffer memory gets a CI grep-gate
      similar to 8C's SQL-injection ratchet, pinned to 0 once
      13.3 lands.

Blocker: Phase 12 (packet unification) should land first so the
migration only edits one copy of each packet class.

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
