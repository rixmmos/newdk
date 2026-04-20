# MODERNIZATION.md — Running Plan

This is the single source of truth for the DarkEden-like project's
modernization work, spanning `server/`, `client/`, `docs/`, configs, and
tooling. It supersedes the per-area status documents in `client/*.md`
(which remain in git history for reference but should not be treated as
current).

When anything here falls out of date, update this file as part of the
change that made it stale — not after the fact.

## Ground truth (as of 2026-04-20)

The following is what the trees actually contain, verified by direct read
and grep. Where it disagrees with a per-area status doc, this file wins.

**Runtime verification update (2026-04-20):** the client and server
cluster now boot end-to-end. Full live gameplay was exercised — login
→ character-select → gameserver handoff → spawn → zone travel → NPCs
→ combat → XP → level-up → stat allocation. See Phase 18 below for
the runtime bug list (PP–WW) that this pass shook out, and
[`ROADMAP.md`](./ROADMAP.md) for product-facing follow-ups (UHD,
English-only localization, branding scrub, Windows .exe distribution,
bleeding-effect gate) that are explicitly *not* modernization work.

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
- [x] Follow-up: runtime verification. `make debug-asan` itself is
  still not wired into CI, but the Phase 1 tree now boots
  end-to-end through a full gameplay loop (see Phase 18) on
  WSL2 + WSLg. Nothing in the deleted-files set regressed. The
  original "human verification pending" note resolves here; the
  asan-build CI task is tracked separately as a Phase 15
  follow-up, not a Phase 1 blocker.
- [!] WinLib deletion: still present in the tree
  (`Client/WinLib/{CWinUpdate.cpp,CWinUpdate.h,WinLib.h}`).
  Folded into Phase 3 at the time; Phase 3 closed without
  removing it because the update-state hierarchy refactor it
  depends on was out of Phase 3's scope. Remains a legitimate
  follow-up — small, but needs the `CWaitUIUpdate` /
  `CWaitPacketUpdate` / `CGameUpdate` / `COpeningUpdate`
  subclasses to be re-homed first.

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

### Phase 15 — CI build matrix — done 2026-04-19
Deferred 2026-04-19 from Phase 10. Server has one workflow today
(clang-format diff gate); client has no CI. Adding a real build
matrix means landing infrastructure, not just a YAML file.

**Scope corrections from the initial Phase 10-deferred block
(audit 2026-04-19):**

1. **No Boost dep.** `dkrixserver/CMakeLists.txt` calls
   `find_package(Threads)`, pkg-config for Lua 5.1, and
   `find_path/find_library` for MySQL and Xerces-C. Zero
   `Boost*`, zero `#include <boost/...>` anywhere in
   `dkrixserver/src/`. Dockerfile.dev confirms: apt installs
   `gcc build-essential libxerces-c-dev libmysqlclient-dev
   liblua5.1-dev xutils-dev psmisc` — no boost.
2. **Client does not link lua.** Initial plan listed
   `liblua5.1-0-dev` in client deps. That's wrong —
   `dkrix/CMakeLists.txt` never calls `find_package(Lua)` nor
   links a lua library. Client scripting, if any, is future
   work.
3. **Server CMake has NO sanitizer options.** Initial plan
   assumed `USE_ASAN/TSAN/UBSAN` were already wired in both
   trees. Client has them at `dkrix/CMakeLists.txt:28-30`;
   server has nothing. 15B must add the three `option()`s and
   the sanitizer-flags pipeline to `dkrixserver/CMakeLists.txt`
   before sanitizer Makefile targets are wired.
4. **Client Makefile has no build targets.** Phase 10B shipped
   a format-only Makefile (`fmt` / `fmt-check` / `fmt-check-all`)
   — no `debug`, no `release`, no build targets at all.
   Sanitizer targets presume there's already a `debug` target
   to base them on; for client, 15C has to add build targets
   first.
5. **Repo boundary.** `dkrix/` and `dkrixserver/` publish to
   separate upstreams (`opendarkeden/client` and
   `opendarkeden/server`), confirmed by the existing
   `dkrixserver/.github/workflows/format-check.yml` being
   nested under the server tree. CI workflows therefore land
   under each tree's own `.github/workflows/build.yml`, NOT at
   the combined-repo root. Two workflow files, not one.

**Corrected plan items:**

- [x] **Server CMake sanitizer wiring.** Add
      `option(USE_ASAN ...)` / `option(USE_TSAN ...)` /
      `option(USE_UBSAN ...)` and the `SANITIZER_FLAGS`
      pipeline to `dkrixserver/CMakeLists.txt`, mirroring the
      client block at `dkrix/CMakeLists.txt:28-56`. Port the
      same flag strings (`-fsanitize=address
      -fno-omit-frame-pointer`, `-fsanitize=thread`,
      `-fsanitize=undefined`) and the same CMAKE_CXX/C_FLAGS
      application. **Shipped in 15B (`9f3a860`)**: +39 lines,
      verbatim copy of client block with an extra `message
      (WARNING ...)` on unknown compilers.
- [x] **Sanitizer Makefile targets, both trees.**
      `debug-asan`, `debug-tsan`, `debug-ubsan` targets that
      invoke `cmake -B build-<san> -DCMAKE_BUILD_TYPE=Debug
      -DUSE_<SAN>=ON && cmake --build build-<san>`. Each
      sanitizer uses its own build dir so switches don't
      reconfigure. Client's fmt-only Makefile grew `debug` /
      `release` / `clean` from scratch. **Shipped in 15C
      (`e224e77`)**: server +38 / client +72 lines.
- [x] **Server CI toolchain install script.** `apt install -y
      build-essential cmake pkg-config libxerces-c-dev
      libmysqlclient-dev liblua5.1-dev` (matches Dockerfile.dev
      plus cmake/pkg-config defensively). No boost, no sdl2 —
      pure server deps. **Shipped in 15D (`4712a0b`)**.
- [x] **Client CI toolchain install script.** Ubuntu: `apt
      install -y build-essential cmake pkg-config libsdl2-dev
      libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
      libjpeg-dev`. macOS: `brew install sdl2 sdl2_image
      sdl2_ttf sdl2_mixer jpeg`. iconv is in glibc on Linux
      and libiconv is handled by the existing CMake probe on
      macOS. **Shipped in 15E (`3ac9f61`)**.
- [x] **Per-tree `build.yml` workflows.** Each tree gets its
      own `.github/workflows/build.yml`. Server: `{os:
      [ubuntu-latest], sanitizer: [none, asan, ubsan]}` — 3
      jobs. Client: `{os: [ubuntu-latest, macos-latest],
      sanitizer: [none, asan, ubsan]}` — 6 jobs. TSan
      excluded from default matrix (still runnable locally
      via `make debug-tsan`). **Shipped in 15D + 15E**.

**Plan (sub-commits):**

- **15A — Pin plan.** (This commit.) Records the 5 scope
  corrections above and the sub-commit order below.
- **15B — Server CMake sanitizer options.** Add USE_ASAN /
  USE_TSAN / USE_UBSAN options + the flags pipeline to
  `dkrixserver/CMakeLists.txt`. Pure build-infrastructure
  edit; no source changes, no new dependencies.
- **15C — Makefile targets: server + client.** Server gets
  `debug-asan` / `debug-tsan` / `debug-ubsan`. Client's
  fmt-only Makefile gets `debug` / `release` / sanitizer
  targets so there's something for the CI to invoke.
- **15D — Server `.github/workflows/build.yml`.** Matrix
  `{sanitizer: [none, asan, ubsan]}` on ubuntu-latest. Apt-
  installs the Dockerfile.dev dep set. Builds the server and
  runs the resulting binary through `--help` or similar
  smoke check (if one exists — fallback: confirm the binary
  was produced).
- **15E — Client `.github/workflows/build.yml`.** Matrix
  `{os: [ubuntu-latest, macos-latest], sanitizer: [none,
  asan, ubsan]}`. Conditional apt/brew install scripting.
  Builds `DarkEden` target and confirms binary produced.
- **15F — Close-out.** Flip this block to `done`, outcome
  block, commit table. No CLAUDE.md changes expected — the
  new Makefile targets are self-documenting in `make help`.

**Explicit non-goals for Phase 15:**

- **TSan in the default matrix.** Built as a Makefile target
  (operators can run it locally) but CI matrix stops at
  none/asan/ubsan to keep runner time reasonable.
- **macOS server CI.** Server CMake has macOS branches for
  MySQL/Lua/Xerces homebrew paths, but we've never built the
  server on macOS. Out of scope — add later if a contributor
  actually needs it.
- **Docker-based CI.** Dockerfile.dev exists for local dev;
  using it in CI would slow runs down for no additional
  coverage. Apt-install in the workflow step is enough.
- **Cache keys / build artifacts / release workflow.**
  Benchmarking, caching, and artifact upload are separate
  sophistication layers on top of a working build matrix.
  Once the matrix is green, those can be added in a follow-
  up without touching Phase 15's scope.
- **Windows MSVC build.** Same disposition as in every prior
  phase: out of scope.
- **Running the actual game/server.** CI confirms the build
  succeeds. Smoke-testing that the server accepts
  connections or the client opens a window needs real
  service deps (MySQL, SDL2 video sink, X11/Wayland) that
  are awkward in CI — separate problem.

**Outcome (2026-04-19):**

| #    | Commit    | Subject                                                                       |
| ---- | --------- | ----------------------------------------------------------------------------- |
| 0001 | `d830799` | `docs: pin Phase 15 plan (CI build matrix, with scope corrections)`           |
| 0002 | `9f3a860` | `server: 15B — USE_ASAN/TSAN/UBSAN options + sanitizer flags`                 |
| 0003 | `e224e77` | `15C — Makefile sanitizer + build targets (server + client)`                  |
| 0004 | `4712a0b` | `server: 15D — GitHub Actions build matrix (ubuntu + sanitizers)`             |
| 0005 | `3ac9f61` | `client: 15E — GitHub Actions build matrix (ubuntu + macos × sanitizers)`     |
| 0006 | `15ea8e2` | `docs: 15F — close out Phase 15 in MODERNIZATION.md`                          |

Net delta: **+440 lines / -34 lines across 6 files**
(`docs/MODERNIZATION.md` +144/-17 across 15A+15F,
`dkrixserver/CMakeLists.txt` +39/-0,
`dkrix/Makefile` +72/-17,
`dkrixserver/Makefile` +38/-17,
`dkrixserver/.github/workflows/build.yml` +74 new,
`dkrix/.github/workflows/build.yml` +107 new). All new
files; zero source-code edits.

What shipped
------------

- **Shipped (15B):** Sanitizer plumbing added to
  `dkrixserver/CMakeLists.txt` — three options
  (`USE_ASAN` / `USE_TSAN` / `USE_UBSAN`) plus a
  compiler-guarded flags pipeline verbatim copy of the
  client block at `dkrix/CMakeLists.txt:28-56`. Default
  OFF; no build behavior change without explicit opt-in.
- **Shipped (15C):** Makefile sanitizer targets on both
  trees. Server gains `debug-asan` / `debug-tsan` /
  `debug-ubsan` (it already had `debug` / `release`);
  client's previously format-only Makefile gains `debug` /
  `release` / `clean` plus the three sanitizer variants.
  Each sanitizer uses a dedicated build dir
  (`build-asan` / `build-tsan` / `build-ubsan`) so
  switching between them doesn't force a reconfigure.
- **Shipped (15D):** Server `.github/workflows/build.yml`.
  Matrix `{sanitizer: [none, asan, ubsan]}` on
  ubuntu-latest. Deps from Dockerfile.dev
  (`build-essential libxerces-c-dev libmysqlclient-dev
  liblua5.1-dev`) plus `cmake` / `pkg-config` defensively.
  Verify step confirms `bin/` is non-empty after build.
  Triggers on PR + push to master.
- **Shipped (15E):** Client `.github/workflows/build.yml`.
  Matrix `{os: [ubuntu-latest, macos-latest],
  sanitizer: [none, asan, ubsan]}` — 6 jobs. Branching
  dep-install: apt for Linux, `brew` for macOS
  (`sdl2 sdl2_image sdl2_ttf sdl2_mixer jpeg`). Verify
  step reads the per-sanitizer build dir and confirms
  `DarkEden` executable landed.

Deployment notes for operators:

1. **Nothing breaks on pull.** Default OFF on all
   `USE_<SAN>=ON` options; `make` on server still runs
   debug build; client's `make` default changed from
   `help` to `debug` (now actually builds instead of
   printing a menu). If you were relying on `make` doing
   nothing in the client tree, use `make help`.
2. **Client needs SDL2 installed to build.** The new
   client Makefile targets invoke cmake, which does
   `find_package(SDL2 REQUIRED)`. If your local dev
   setup didn't have SDL2 before (because you only ever
   ran `make fmt`), install it via your OS package
   manager before running `make debug`.
3. **`.github/workflows/build.yml` triggers on PR to
   master and push to master.** If either repo has
   branch-protection rules, add the new matrix job names
   (build (none) / build (asan) / build (ubsan) /
   build (ubuntu-latest, none) / etc.) to required
   checks once the first run has gone green.
4. **First CI run will be slow.** No caching yet — every
   PR reinstalls deps and does a full build from scratch.
   Phase 15 deliberately didn't add caching (see non-
   goals). A future phase can layer `actions/cache@v4`
   on top of the existing matrix without touching 15's
   structure.
5. **macOS runners have monthly billing quotas.** If
   running against a private repo, the 6-job client
   matrix × macOS surface will eat minutes fast. Public
   repos get unlimited macOS runner time on GitHub-
   hosted runners.

### Phase 11 — SQL injection remediation (11.1 only) — done 2026-04-19
Originally deferred 2026-04-18 from Phase 8 after scope audit
showed 625 `executeQuery("…%[sd]…")` call sites (not the ~87
originally estimated) and zero existing `mysql_stmt_*`
infrastructure to migrate onto.

**Scope correction from the initial deferred block (audit
2026-04-19):**

Ship 11.1 (PreparedStatement wrapper API) as a single close-
out phase now; treat 11.2 (call-site migrations) as an
ongoing post-11.1 effort driven by the Phase 8C CI ratchet
rather than trying to land it under a single "Phase 11 done"
banner. Rationale: the ratchet infrastructure *already exists*
(8C pinned the baseline at 567 in `scripts/check-sql-
injection.sh` and CI fails any PR that pushes the count up);
every migration PR that ships afterwards is measured by that
gate. Forcing all 567 migrations under one phase close-out
either blocks Phase 11 indefinitely or produces a rubber-stamp
close-out that doesn't reflect actual migration work. Split:
11.1 is a one-off API commit that can close cleanly; 11.2 is
ongoing work that closes when the ratchet reaches 0.

**Corrected plan items:**

- [x] **11.1 — `PreparedStatement` wrapper API.** One-off
      design + implementation commit. Locked shape:
      ```cpp
      class PreparedStatement {
      public:
          PreparedStatement(Connection*, const std::string& sqlWithQMarks);
          ~PreparedStatement();

          // Parameter binding (1-indexed to match `?` position).
          void bindInt    (uint idx, int v);
          void bindUInt   (uint idx, uint v);
          void bindLong   (uint idx, long long v);
          void bindULong  (uint idx, unsigned long long v);
          void bindDouble (uint idx, double v);
          void bindString (uint idx, const std::string& v);
          void bindTime   (uint idx, time_t t);
          void bindNull   (uint idx);

          // Execution. Ownership of Result* transferred to caller.
          Result* execute();

          // Last-ditch diagnostics for exception paths.
          std::string getError() const;
          uint        getAffectedRowCount() const;
          uint        getInsertID() const;
      };
      ```
      Wraps `mysql_stmt_init` / `mysql_stmt_prepare` /
      `mysql_stmt_bind_param` / `mysql_stmt_execute` /
      `mysql_stmt_bind_result` / `mysql_stmt_fetch`. Lives in
      `dkrixserver/src/server/database/PreparedStatement.
      {h,cpp}`. Registered in
      `dkrixserver/src/server/database/CMakeLists.txt` so all
      five compile flavors (`Database`, `GameServerDatabase`,
      `LoginServerDatabase`, `SharedServerDatabase`,
      `UpdateServerDatabase`) pick it up. Throws
      `SQLException` on driver error, same as the existing
      `Statement` class.
- [ ] **~~11.2~~ — Migrate 567 injection-shaped call sites.**
      **Reclassified as ongoing follow-up work, not a Phase 11
      sub-item.** Tracked by the Phase 8C CI ratchet in
      `scripts/check-sql-injection.sh` (baseline pinned at
      567; CI fails on any PR that increases the count). Each
      migration PR is its own small commit against the
      existing ratchet; there is no "Phase 11.2 close-out" —
      the work closes when the ratchet reaches 0 and the gate
      flips from "count didn't grow" to "zero matches".

      Priority order (unchanged from the deferred block):
      1. User-string interpolation sites (chat, say, whisper,
         pet names, custom options) — ~180 sites, biggest
         blast radius.
      2. Operator-trusted interpolation (admin commands, log
         lookups) — ~200 sites.
      3. Constant / numeric-only interpolation (IDs, enums cast
         to int) — remainder, pure hygiene.

**Plan (sub-commits):**

- **11A — Pin plan.** (This commit.) Records the scope
  correction that separates 11.1 (one-off API commit) from
  11.2 (ongoing ratchet-driven migration).
- **11B — Add `PreparedStatement.{h,cpp}`.** The full API
  above, implementation routed through `mysql_stmt_*`.
  Registered in all five database-library flavors in
  `server/database/CMakeLists.txt`. No call-site migrations
  in this commit — the API lands first, migrations follow as
  separate PRs driven by the 8C ratchet.
- **11C — Close-out.** Flip this block to `done (11.1 only)`,
  commit table, pointer to the 8C ratchet for 11.2 progress
  tracking.

**Explicit non-goals for Phase 11 (this round):**

- **No call-site migrations in Phase 11 itself.** 11B ships
  the API; migrations happen as separate PRs measured against
  the 8C ratchet. This keeps Phase 11's diff small and easy
  to review — exactly the kind of infrastructure change that
  *should* land in isolation from behavior-affecting call-
  site churn.
- **No end-to-end MySQL test harness.** The API is exercised
  via unit-style examples in the header comments; full round-
  trip testing requires a live MySQL instance, which is out
  of scope for this phase (same rationale as Phase 10's
  "don't wire a MySQL container into CI").
- **No deprecation of `Statement`.** Both APIs coexist until
  the ratchet reaches 0; only then does `Statement` get
  deprecated, and that's a separate commit.

**Outcome (2026-04-19):**

| #    | Commit    | Subject                                                                |
| ---- | --------- | ---------------------------------------------------------------------- |
| 11A  | `6ba74bc` | `docs: 11A — pin Phase 11 plan (scope: 11.1 API only)`                 |
| 11B  | `d5fb1b1` | `server: 11B — add PreparedStatement.{h,cpp} to server/database/`      |
| 11C  | `HEAD`    | `docs: 11C — close out Phase 11 (11.1 only) in MODERNIZATION.md`       |

Net delta this phase: **+505 lines across 2 new files + 1 CMake
edit** (`PreparedStatement.h`, `PreparedStatement.cpp`,
`server/database/CMakeLists.txt`) plus **docs-only edits** in
MODERNIZATION.md. Zero edits to existing source files. The new
class compiles into all five database-library flavors but isn't
referenced by any call site yet — first migration PR picks the
first one up.

Implementation notes vs. the 11A sketch:

- **Server-side prepared statements** via `mysql_stmt_*` as
  planned, not a client-side escape-and-substitute shim.
- **`bindUInt`/`bindULong`/`bindDouble`/`bindNull` included
  alongside the sketch's `bindInt`/`bindLong`/`bindString`/
  `bindTime`.** The server uses unsigned IDs in many places
  (player / item / creature IDs are `uint`/`UINT64`), so the
  unsigned variants are load-bearing for call-site
  readability.
- **`execute()` returns `NULL` for SELECT too (not just
  INSERT/UPDATE/DELETE) in 11.1 scope.** Full Result*
  integration for SELECT requires plumbing
  `mysql_stmt_bind_result` + `mysql_stmt_fetch` into a new
  Result construction path. Deferred to the first call-site
  migration PR that needs a SELECT — lands alongside that PR
  instead of as dead code here. See the header's scope
  block.

**Follow-up work (not part of Phase 11, tracked by 8C ratchet):**

- **SELECT result-set integration.** First migration PR that
  needs to convert a SELECT site will extend Result (or add a
  sibling class) to iterate `mysql_stmt_fetch`-produced rows.
  Lock the design decision (extend vs. new class) in that PR;
  this close-out deliberately doesn't pre-decide.
- **Call-site migrations against the 8C ratchet.** No
  scheduled phase close-out; each migration PR moves the
  ratchet count down. When it hits 0, the gate flips from
  "count didn't grow" to "zero matches" and `Statement` can
  be deprecated.
- **Stress test the MYSQL_STMT lifecycle under load.**
  libmysqlclient has historically had resource-cleanup bugs
  around `mysql_stmt_close` on shared connections; real-world
  use after migrations start will surface any such issues.

### Phase 12 — Packet schema unification (12.0 scaffolding) — done 2026-04-19
Originally deferred 2026-04-19 from Phase 9; blocker cleared
by Phase 10 (build hygiene) + Phase 14C (CONFIGURE_DEPENDS on
the client GLOB). Audit 2026-04-19 refined the raw scope:

- `dkrixserver/src/Core/` holds **460 + 460 = 920** packet
  `.{h,cpp}` pairs matching
  `^(CG|GC|CL|LC|GS|SG|GT|TG)` and NOT ending in `Handler`:
  300 CG (Client→Game), 516 GC (Game→Client), 34 CL
  (Client→Login), 34 LC (Login→Client), 16 GS (Game→Shared),
  20 SG (Shared→Game). `GT`/`TG` named in the original block
  turned out to be 0 files in tree.
- `dkrix/Client/Packet/Cpackets/` holds **163 + 163 = 326**
  packet `.{h,cpp}` pairs — exclusively CG (294) + CL (32).
  No GC/LC/GS/SG on client: receive-side packets are decoded
  inline from the wire buffer rather than through a class
  hierarchy, so the client only duplicates the send-side
  packet families.
- `dkrix/Client/Packet/` root (148 non-Cpackets files) is
  infrastructure (`SocketInputStream`, `SocketOutputStream`,
  `Encrypter`, `Datagram`, etc.) duplicated against
  `dkrixserver/src/Core/`'s non-packet files — a SEPARATE
  duplication from the packet tree, distinct from Phase 12's
  scope. The Phase 13 close-out flagged these stream files
  (`SocketInputStream.{h,cpp}`, `SocketOutputStream.{h,cpp}`)
  as needing consolidation alongside Phase 12's packet move
  for the 13.3 migration to land once; that remains true but
  is not what this phase owns.
- Duplicate-name intersection between server packet tree and
  client Cpackets: **326** files.

Spot-check diff on `CGAbsorbSoul.{h,cpp}` suggested the two
copies differ only in whitespace, exception-specs, and comment
formatting — byte layout on the wire likely identical. This
spot-check was subsequently refined by Phase 17's tree-wide
divergence audit: 0 of 163 class pairs are actually cosmetic-
only at the class level; every pair has at least one file with
semantic divergence (different `__END_CATCH_NO_RETHROW` vs
`__END_CATCH`, method-body inlining, `toString()` debug-string
drift, etc.). The wire format still looks identical under
algebraic equivalence of `getPacketSize()` expressions, but
per-class reconciliation is needed before each migration PR
can `git mv` + delete. See Phase 17 below for the details.

**Scope correction (2026-04-19, revised under session-
preservation directive):**

Ship 12.0 (scaffolding + duplicate-count ratchet) now; defer
12.1 (the 920-file physical move) and 12.2 + 12.3 to
ratchet-driven follow-up work, same shape as Phase 11.2 under
the 8C ratchet.

Rationale: the 920-file `git mv` + CMake rewiring is a
mechanical but large surgery that doesn't fit a single close-
out commit. Landing it all in one commit risks a broken build
with high debug-cost and no incremental recovery; landing it
piecewise inside "Phase 12" either blocks the phase
indefinitely or produces a rubber-stamp close-out that
misrepresents progress. The Phase 11.1 + 8C precedent works
here: a ratchet-driven gate at baseline 326 turns "Phase 12
migration" into ongoing per-PR-measurable work instead of a
monolithic phase, and Phase 12 itself can close cleanly on
the one-off scaffolding that makes that ratchet possible.

**Corrected plan items:**

- [x] **12.0 — Scaffolding + duplicate-count ratchet.**
      Create `shared/Packets/` at the repo root with a
      canonical-tree README. Add
      `scripts/check-packet-duplicates.sh` +
      `.packet-duplicates-baseline` pinned at 326; script
      shape mirrors `check-sql-injection.sh` — CI fails on
      count increase, `--update` is the explicit knob for
      lowering the baseline after a migration PR. No file
      moves in this commit; this only lands the target
      directory + the tracking gate.
- [ ] **~~12.1~~ — Canonicalise one tree + delete the
      other.** **Deferred to ratchet-driven follow-up
      work.** Each migration PR moves one small packet
      family (GS/SG, a CL subset, one CG-sub-feature, etc.)
      from `dkrixserver/src/Core/` to `shared/Packets/`,
      updates both builds' CMakeLists.txt, and runs
      `check-packet-duplicates.sh --update` to lower the
      baseline by the migrated count. No Phase 12.1 close-
      out commit; the ratchet is the progress indicator.
- [ ] **~~12.2~~ — Strip client-tree duplicates + fix
      includes.** **Folded into 12.1 per-PR work.** Each
      migration PR that moves a server packet also deletes
      its Cpackets duplicate and fixes the client's include
      path. The ratchet counts both trees, so a migration
      only drops the count if BOTH sides are unified.
- [ ] **~~12.3~~ — CI structural check.** **Implemented as
      part of 12.0 (the ratchet IS the gate).** The
      original plan was a grep-gate that fails if a matching
      name ever reappears in both trees; the ratchet does
      exactly that at baseline 0 — and at baselines > 0
      during the migration, it gates against REGRESSION
      which is the same guarantee in weaker form.

**Plan (sub-commits):**

- **12A — Pin plan.** (This commit.) Records the scope
  audit (920 server / 326 client / 326 duplicate-name
  intersection), the scope-correction split (12.0 ships now;
  12.1 + 12.2 fold into ratchet-driven per-PR work; 12.3 is
  the ratchet itself), and the sub-commit order below.
- **12B — Add `shared/Packets/` + duplicate ratchet.**
  Creates `shared/Packets/README.md` documenting the
  canonical-tree policy and the migration-PR shape. Adds
  `dkrixserver/scripts/check-packet-duplicates.sh` + sibling
  `.packet-duplicates-baseline` pinned at 326. Script shape
  matches `check-sql-injection.sh` (check / --count /
  --list / --update modes). No source-tree file moves in
  this commit.
- **12C — Close-out.** Flip this block to `done (12.0
  scaffolding)`, commit table, pointers to the ratchet-
  driven follow-up for 12.1 + 12.2 and the Phase 13.3
  migration that unblocks once `shared/Core/Socket*Stream.h`
  lands alongside the first few packet migrations.

**Blocker status:** Phase 10 (build hygiene) and Phase 14C
(client-side CONFIGURE_DEPENDS) are both done. The original
"CMake glob update required before moving headers" blocker
is cleared — client picks up new `shared/Packets/*.cpp`
automatically if `add_subdirectory(../shared/Packets)` is
wired into `dkrix/CMakeLists.txt` during the first migration
PR. 12.0 does NOT do that wiring; the first migration PR
does.

**Outcome (2026-04-19):**

| Sub-commit | Hash      | Subject                                                           |
| ---------- | --------- | ----------------------------------------------------------------- |
| 12A        | `de0792c` | `docs: 12A — pin Phase 12 plan (scope-corrected; 12.0 scaffolding only)` |
| 12B        | `f66b2ac` | `server: 12B — add shared/Packets/ + packet-duplicates ratchet`   |
| 12C        | HEAD      | `docs: 12C — close out Phase 12 (12.0 only) in MODERNIZATION.md`  |

Net delta: **+286 lines across 3 new files** (empty
`shared/Packets/` target dir + README; duplicate-count
gate + baseline) plus docs-only edits in `docs/MODERNIZATION.md`.
Zero source-tree file moves; zero edits to existing
`dkrixserver/src/Core/*.{cpp,h}` or `dkrix/Client/Packet/**`
files. The ratchet baseline is the current duplicate count
(326) — the gate fails on increase, lowers via `--update`
after each migration PR.

**Follow-up work (ratchet-driven, not a scheduled phase):**

- **Per-PR packet migrations.** Each PR moves one small
  self-contained packet family (e.g. CL/LC for
  LoginServer traffic, GS/SG for Game↔Shared server
  traffic) from `dkrixserver/src/Core/` into
  `shared/Packets/`, deletes the Cpackets sibling, updates
  both builds' CMakeLists.txt, and runs
  `dkrixserver/scripts/check-packet-duplicates.sh --update`.
  The first such PR also adds `shared/Packets/CMakeLists.txt`
  and wires `add_subdirectory(../shared/Packets)` into both
  root CMake files.

**Ratchet progress (migration PRs landed after 12.0):**

| Date       | Class         | Baseline after | Notes                                                                 |
| ---------- | ------------- | -------------: | --------------------------------------------------------------------- |
| 2026-04-20 | `CGStoreOpen` |            324 | First migration. Also lands `shared/Packets/CMakeLists.txt` (INTERFACE `shared_packets` target), both-tree `add_subdirectory(../shared/Packets)` wiring, and the Korean→English comment sweep on the migrated header. Remaining first-wave cosmetic-`.cpp` candidates: `CGDisplayItem`, `CGGQuestCancel`, `CGRequestStoreInfo`, `CGStoreClose`, `CGStoreSign`, `CGUndisplayItem` (6). |
- **Post-Phase-12.0 Phase 13.3 unblocked.** Once the first
  few migrations land and `shared/Packets/` has a real
  library target, the Socket stream files
  (`SocketInputStream.{h,cpp}`, `SocketOutputStream.{h,cpp}`)
  can be consolidated into `shared/Core/` alongside them,
  at which point Phase 13.3's endian-safe
  `template<T> read(T&)`/`write(T&)` migration becomes a
  single-commit edit of the unified headers rather than
  a two-tree coordinated patch. 13.4 (grep-gate for raw
  casts, pinned at 0) follows immediately after 13.3.
- **Stream-file consolidation itself is not a Phase 12
  item.** It's flagged as a separate follow-up because
  the stream files aren't packet classes — they're the
  wire-format plumbing that packet classes ride on. Same
  shape of duplication (server Core / client Packet), but
  the ratchet script deliberately does NOT count them: a
  second ratchet at baseline 6 (the stream-file pair
  count) can ship when Phase 13.3 needs it, or they can
  ride the first migration PR that touches
  `shared/Core/`.

### Phase 13 — Endian-safe wire I/O (13.1 + 13.2) — done 2026-04-19
Originally deferred 2026-04-19 from Phase 9. Current state: the
socket stream API's `template<T> read(T&)` does `buf = *(T*)
(buffer + head)` — host-endian raw cast. Works everywhere on
x86 (server + desktop client both LE), but quietly breaks on any
BE port and gives no protection against partial-struct tears if
`T` is a compound type.

Audit 2026-04-19 confirmed the bug pattern exists in both trees:

- Server: `dkrixserver/src/Core/SocketInputStream.h:159,170`
  (`read<T>`), `dkrixserver/src/Core/SocketOutputStream.h:174,
  178,191` (`write<T>`).
- Client: `dkrix/Client/Packet/SocketInputStream.{h,cpp}` and
  `SocketOutputStream.{h,cpp}` — duplicate of the server's
  `Core/` stream files with the same raw-cast pattern. Phase 12
  (packet unification) will fold the two copies into one shared
  tree.

**Scope correction from the initial deferred block (audit
2026-04-19):**

Ship 13.1 + 13.2 now; defer 13.3 + 13.4 to post-Phase-12.
Rationale: adding `Core/Endian.h` to both trees today means
physically copying the file into `dkrix/Client/Packet/Endian.h`
and `dkrixserver/src/Core/Endian.h`, only to move one copy and
delete the other when Phase 12 unifies the packet tree. Land
the primitive helpers in the server tree now (server is the
authoritative source for stream code), leave the client
duplicate untouched, and have Phase 12 bring the client over
when it merges the two stream trees. The migration work (13.3)
and the CI grep-gate (13.4) then happen once against the
unified tree instead of twice with intermediate fixups.

**Corrected plan items:**

- [x] **13.1 — Pick and document a wire byte order.** LE keeps
      compatibility with every existing client binary. BE is
      "network order" but would require every deployed client to
      be rebuilt and re-shipped. **Decision: LE.** Every
      deployed server and every deployed desktop client is
      already x86 LE, so picking LE is a no-op on the wire while
      making the choice explicit. Future BE hosts (e.g. a PPC
      port) can opt in by rebuilding against the same helpers.
      Recorded in 13A.
- [x] **13.2 — Add endian-safe primitives (server).** Inline
      helpers in `dkrixserver/src/Core/Endian.h` wrapping
      `htole16/32/64` + `le16/32/64toh`. On Linux this is a
      thin wrapper around `<endian.h>`. On macOS / mingw, the
      same names get `#ifdef` fallbacks built from byte-swap
      intrinsics (`__builtin_bswap*` on GCC/Clang). Header-only
      so nothing gets linked in for LE hosts. Shipped in 13B.
- [ ] **~~13.3~~ — Migrate the primitive reads/writes.**
      **Deferred to post-Phase-12.** The client has its own copy
      of `SocketInputStream.h`/`SocketOutputStream.h`; migrating
      the templates in both trees simultaneously means writing
      the same migration twice and then throwing one copy away
      when Phase 12 unifies the stream files. Once Phase 12
      lands, 13.3 becomes a single-commit edit of the unified
      `shared/Core/Socket*Stream.h` instead of a two-tree
      coordinated patch.
- [ ] **~~13.4~~ — Audit for remaining raw casts.** **Deferred
      to post-Phase-12.** A grep-gate pinned at 0 only works
      once 13.3 has driven the count to 0; pinning it early
      would either fail CI or require a threshold that drifts
      each time Phase 12 moves files.

**Plan (sub-commits):**

- **13A — Pin plan.** (This commit.) Records the LE decision
  (13.1), the scope-correction split (13.2 ships now; 13.3 +
  13.4 move to post-Phase-12), and the sub-commit order below.
- **13B — Add `dkrixserver/src/Core/Endian.h`.** Header-only
  LE helpers wrapping `htole16/32/64` + `le16/32/64toh`. Linux
  path uses `<endian.h>` directly; macOS path uses
  `<libkern/OSByteOrder.h>`'s `OSSwapHostToLittleInt*` or
  `__builtin_bswap*` fallbacks; mingw path uses
  `__builtin_bswap*` fallbacks. Unused until 13.3 migrates the
  stream templates — this commit only lands the file so the
  primitives are available when Phase 12 folds the packet
  trees together.
- **13C — Close-out.** Flip this block to `done`, commit
  table, pointers to the deferred-to-post-Phase-12 follow-ups.
  No source-code edits beyond 13B's new header.

**Blocker cleared:** Phase 12 (packet unification) was the
stated blocker when the deferred block was written. Splitting
13.1 + 13.2 out from 13.3 + 13.4 lets Phase 13 make forward
progress on the *primitives* without waiting on Phase 12; the
*migration* correctly still waits for the unified tree.

**Explicit non-goals for this sub-phase (13A-13C):**

- **No client-side `Endian.h` copy.** The client tree has
  duplicate stream files; adding an `Endian.h` there today
  would be thrown away by Phase 12. The helpers only land in
  the server tree.
- **No template migration.** 13.3 stays deferred until Phase
  12 unifies the stream trees.
- **No CI grep-gate.** 13.4 stays deferred until 13.3 has
  driven `*(T*)(m_Buffer + ...)` casts to 0.
- **No behavior change.** 13B's header is unused code until
  13.3 touches it; nothing on the wire changes in this phase.

**Outcome (2026-04-19):**

| #    | Commit    | Subject                                                                |
| ---- | --------- | ---------------------------------------------------------------------- |
| 13A  | `73d7854` | `docs: 13A — pin Phase 13 plan (LE decision + scope correction)`       |
| 13B  | `bf44d49` | `server: 13B — add Core/Endian.h (LE wire-format primitives)`          |
| 13C  | `HEAD`    | `docs: 13C — close out Phase 13 (13.1 + 13.2) in MODERNIZATION.md`     |

Net delta this phase: **+179 lines across 1 new file** (server-
tree header) plus **docs-only edits** in MODERNIZATION.md. Zero
source-code changes; zero runtime-behavior changes. Nothing
links to Endian.h yet — it sits waiting for 13.3.

**Follow-ups (post-Phase-12):**

- **13.3 — Migrate Socket{Input,Output}Stream<T>::read / write.**
  Replace the host-endian raw cast `*(T*)(m_Buffer + m_Head)`
  with `memcpy` + `le*toh` / `htole*`. Compound types continue
  to use memberwise `read`/`write` as they already do (packet
  classes never relied on the raw-cast template for anything
  past primitive fields). Single-commit edit against the
  unified `shared/Core/Socket*Stream.h` that Phase 12 will
  produce.
- **13.4 — CI grep-gate.** Pattern
  `\*\s*\(\s*\w+\s*\*\s*\)\s*\(\s*m_Buffer\s*\+` at 0; pin
  count once 13.3 lands. Same ratchet style as Phase 8C's SQL-
  injection gate.

Both follow-ups are cleanly unblocked by Phase 12; both are
deliberately *not* scheduled against today's two-tree state.

### Phase 16 — CI ratchet activation — done 2026-04-19
Two ratchet scripts currently exist with baseline files pinned
to today's counts:

- `dkrixserver/scripts/check-sql-injection.sh` (Phase 8C,
  baseline 567 — variadic SQL-format sites that need
  migration to `PreparedStatement` from Phase 11.1).
- `dkrixserver/scripts/check-packet-duplicates.sh` (Phase
  12.0, baseline 326 — CG/CL packet class pairs that
  duplicate between `dkrixserver/src/Core/` and
  `dkrix/Client/Packet/Cpackets/`).

Neither is referenced from any file in
`dkrixserver/.github/workflows/` or `dkrix/.github/workflows/`,
so both fire only when a contributor runs them by hand.
Phases 8C and 12.0 explicitly flagged CI wiring as "small
follow-up not yet phase-banner-wrapped"; this phase is that
wrap-up.

- [x] **16.1 — Wire both ratchets into a single GitHub
      Actions workflow.** Since the scripts live under
      `dkrixserver/scripts/` and the duplicate gate reaches
      across into `dkrix/Client/Packet/Cpackets/` via
      relative path, the simplest shape is a
      `dkrixserver/.github/workflows/ratchets.yml` that
      checks out both trees as sibling directories (the
      same layout they sit in under this repo) and runs
      both scripts in sequence.
- [x] **16.2 — Trigger shape.** Fire on
      `pull_request: [opened, synchronize, reopened]`
      targeting `master`, plus `push: [master]` for
      post-merge verification. Matches the existing
      `build.yml` and `format-check.yml` patterns.
- [x] **16.3 — No separate client-side workflow.** Both
      ratchets are about the SERVER tree (one measures
      server-tree SQL sites; the other measures server vs.
      client packet-tree duplicates). Adding a client-side
      copy of the workflow would just duplicate the same
      two script invocations. Server-side `ratchets.yml`
      is the single source of truth.

**Scope correction note (2026-04-19):**

There's a subtle repo-layout issue here. Under the current
single-repo-with-two-trees layout
(`work/dkrix/` + `work/dkrixserver/` as sibling subdirs of
one git root), the packet-duplicates script's relative-path
resolution (`$script_dir/../..` reaches into
`work/dkrix/Client/Packet/Cpackets/`) works because the
checkout IS the whole repo. If `dkrixserver` and `dkrix` ever
get split into separate upstream repos (each with its own
.git), the ratchet workflow needs either:

1. A checkout of the SIBLING repo in the workflow (easy
   via `actions/checkout` with a `path:` pointing at a
   sibling dir), or
2. A second ratchet in the client-side workflow that
   measures the same thing from the other direction.

For now (single repo), the workflow just checks out the
repo once and both scripts find everything they need. 16A
pins this assumption and calls out the migration path if
the split ever happens.

**Plan (sub-commits):**

- **16A — Pin plan.** (This commit.) Records the activation
  gap (8C + 12.0 ratchet scripts land but aren't wired
  into CI), the workflow shape, the single-workflow
  decision, and the upstream-split migration note.
- **16B — Add `ratchets.yml` workflow.** One GitHub Actions
  workflow file at
  `dkrixserver/.github/workflows/ratchets.yml`. Fires on
  PR + push-to-master. Installs no build dependencies
  (both scripts are pure POSIX sh + grep + comm); runs
  `dkrixserver/scripts/check-sql-injection.sh` and
  `dkrixserver/scripts/check-packet-duplicates.sh` in
  sequence. Either failing fails the job.
- **16C — Close-out.** Flip this block to `done`, commit
  table, note that both ratchets are now active on every
  PR and lower as migrations happen.

**Blocker status:** None. Both scripts exist and are known-
good (verified in-tree during their respective phases).
This phase is pure workflow wiring.

**Outcome (2026-04-19):**

| Sub-commit | Hash      | Subject                                                          |
| ---------- | --------- | ---------------------------------------------------------------- |
| 16A        | `39c7a0b` | `docs: 16A — pin Phase 16 plan (CI ratchet activation)`          |
| 16B        | `cef4571` | `ci: 16B — add ratchets.yml workflow (activates 8C + 12.0 gates)` |
| 16C        | HEAD      | `docs: 16C — close out Phase 16 in MODERNIZATION.md`             |

Net delta: **+76 lines of workflow** in
`dkrixserver/.github/workflows/ratchets.yml` plus docs-only
edits in `docs/MODERNIZATION.md`. Zero edits to existing
workflows, scripts, source, or CMake. Zero new dependencies.

Both ratchets are now active on every PR targeting master
and on every push to master. Migration PRs that move the
baseline down via `--update` on either script will see the
next CI run print `OK: <lower count> (baseline <higher count>
— N migrated; consider --update).` — the baseline-drop is
noisy-by-design to prompt the PR author to commit the
`--update` result in the same PR.

**Follow-up work:** none. Phases 11.2, 12.1, 12.2 per-PR
migrations now have a CI gate that enforces forward
progress; Phase 13.3/13.4 still blocked on post-Phase-12
stream-file consolidation as before.

### Phase 17 — Packet divergence audit — done 2026-04-19
Phase 12's close-out asserted, based on a single spot-check on
`CGAbsorbSoul.{h,cpp}`, that the 326 name-matched packet pairs
"differ only in whitespace, exception-specs, and comment
formatting — byte layout on the wire is identical." A
full-tree sweep before starting the first ratchet-lowering
migration shows that spot-check was not representative.

**Audit finding:**

At the **file level** (each .cpp and each .h classified
separately) across the 326 name-matched files between
`dkrixserver/src/Core/` and `dkrix/Client/Packet/Cpackets/`:

- **0 files** are bit-identical.
- **7 files** are cosmetic-only divergent — after normalizing
  away whitespace, `throw(...)` exception specifications,
  `#include "Client_PCH.h"`, and `#ifndef __GAME_CLIENT__` /
  `#ifdef __DEBUG_OUTPUT__` guard placement, the two files
  become byte-equal. (All 7 are .cpp files.)
- **319 files** carry semantic divergence that remains after
  that normalization. Examples: different `__END_CATCH` vs
  `__END_CATCH_NO_RETHROW` behavior, different `toString()`
  debug-string contents, method-body inlining differences,
  algebraically-equivalent-but-textually-different
  `getPacketSize()` expressions, `string` vs `std::string`
  qualification, and include-order / include-set
  differences.

But **the migration unit is a class** (a .cpp + its .h
together — they can't move independently). The actual
pair-level count is:

- **163 complete class pairs** (server has both .cpp + .h
  AND client has both .cpp + .h).
- **0 pair-level identical** (no class is byte-equal in
  both files).
- **0 pair-level cosmetic** (no class has both its .cpp
  AND its .h at cosmetic-or-better). All 7 cosmetic .cpp
  files pair with .h files that carry semantic divergence.
- **163 pair-level semantic** (every class has at least
  one file — .cpp or .h or both — that truly differs after
  normalization).

Algebraic equivalence of packet-size expressions and
inlining-style differences argue that the wire format likely
IS identical across all 326 pairs — but that's not something
a mechanical `git mv` can assume. Each migration PR has to
pick a canonical version and verify both builds still compile
+ link against the unified file.

**Implications for Phase 12 migration path:**

- **No zero-effort canaries exist.** The refined audit
  shows that every class has at least one semantic-diff
  file in either its .cpp or its .h. The earliest
  file-level read of this audit (7 cosmetic .cpp files —
  `CGDisplayItem`, `CGGQuestCancel`, `CGRequestStoreInfo`,
  `CGStoreClose`, `CGStoreOpen`, `CGStoreSign`,
  `CGUndisplayItem`) suggested a trivial first wave, but
  each of those 7 classes has a sibling .h file in the
  semantic bucket — so even the "easiest" migration still
  requires reconciling one header.
- **Per-PR migration effort is higher than Phase 12's
  outcome block suggested.** Each PR has to:
  1. Pick canonical content for both .cpp and .h.
  2. Verify method signatures are compatible with both
     sides' existing call sites (server code calls
     `foo(bar)` vs client code calls `foo (bar)` is a
     rename-and-fix the compiler catches, but
     `throw(ProtocolException, Error)` being declared on
     one side and not the other is a C++17 deprecation
     warning that depends on toolchain flags).
  3. Confirm wire format hasn't drifted between copies —
     this is the critical invariant. Algebraic equivalence
     of `getPacketSize()` expressions argues for "yes,
     same wire format", but `read()` and `write()` method
     bodies need direct inspection to confirm field order
     + encoding match.
  4. Migrate, delete Cpackets sibling, update both
     CMakeLists.txt, run `check-packet-duplicates.sh
     --update` to drop the baseline by 1.
- **The ratchet baseline is unaffected.** It counts
  name-matches, not content-equality. Each migration
  (however much reconciliation work it needed) drops the
  count by 1 when the last duplicate is removed, same
  as any other.
- **The first few migration PRs should pick classes with
  the smallest .cpp + .h combined diff.** The 7
  cosmetic-.cpp candidates are still the best starting
  point — a cosmetic .cpp plus a small semantic-.h means
  the reconciliation step is bounded to header work.

**What 17 ships:**

- [x] **17.1 — Audit script.**
      `dkrixserver/scripts/audit-packet-divergence.sh`
      (shape mirrors `check-sql-injection.sh`:
      `--count`, `--list-cosmetic`, `--list-semantic`,
      default = classify summary). Reproducible + can be
      run by a future migration PR to sanity-check
      the cosmetic-pair list.
- [x] **17.2 — Migration-class addendum to
      `shared/Packets/README.md`.** Explicit list of the 7
      cosmetic pairs as first-wave canaries; pointer to the
      audit script for the 319 semantic pairs.
- [x] **17.3 — Update Phase 12 close-out claim.** The
      Phase 12 block's sentence "Spot-check diff on
      `CGAbsorbSoul.{h,cpp}` shows the two copies differ
      only in whitespace..." is based on a narrow spot-check
      that doesn't hold tree-wide. Replace with a pointer
      to the 17 audit.

**Plan (sub-commits):**

- **17A — Pin plan.** (This commit.) Records the audit
  finding (0 / 7 / 319 split), the migration-class
  implications, and the sub-commit order below.
- **17B — Add audit script + README addendum + Phase 12
  claim correction.** Three touched files:
  `dkrixserver/scripts/audit-packet-divergence.sh` (new),
  `shared/Packets/README.md` (canary list added),
  `docs/MODERNIZATION.md` (Phase 12 claim softened from
  "differ only in whitespace" to "spot-check only; see
  Phase 17 for tree-wide audit"). No source tree edits;
  no workflow edits; the audit script is an
  investigation-only tool and does NOT get wired into
  `ratchets.yml` — unlike the two ratchet scripts, this
  one reports classification rather than enforcing a
  gate, so CI noise would be off-message.
- **17C — Close-out.** Flip this block to `done`,
  commit table, pointer to the 7 cosmetic canary pairs
  as the first-wave migration candidates.

**Blocker status:** None. Pure audit + doc work; no source,
CMake, or workflow dependencies.

**Outcome (2026-04-19):**

| Sub-commit | Hash      | Subject                                                                          |
| ---------- | --------- | -------------------------------------------------------------------------------- |
| 17A        | `2fd305b` | `docs: 17A — pin Phase 17 plan (packet divergence audit)`                        |
| 17B        | `804b0a6` | `server: 17B — add packet divergence audit script + README migration-class addendum` |
| 17C        | HEAD      | `docs: 17C — close out Phase 17 in MODERNIZATION.md`                             |

Net delta: **+230 lines** in the new audit script
(`dkrixserver/scripts/audit-packet-divergence.sh`), **+75
lines** added to `shared/Packets/README.md`, and docs-only
edits to `docs/MODERNIZATION.md` (Phase 12 claim softened;
Phase 17 block added and closed out). Zero source-tree
changes; zero CMake changes; zero workflow changes. The
`ratchets.yml` CI workflow from Phase 16 is intentionally
unchanged — the audit script is investigation-only, not a
gate.

**Audit-script re-run verification (2026-04-19):**

- `audit-packet-divergence.sh --count` → pairs: 163 /
  identical: 0 / cosmetic: 0 / semantic: 163.
- `audit-packet-divergence.sh --by-file` → files: 326 /
  identical: 0 / cosmetic: 7 / semantic: 319. Cosmetic
  .cpp shortlist matches the README first-wave list
  (CGDisplayItem, CGGQuestCancel, CGRequestStoreInfo,
  CGStoreClose, CGStoreOpen, CGStoreSign, CGUndisplayItem).
- `check-packet-duplicates.sh` → OK: 326 duplicates
  (matches baseline). Phase 16's ratchet still green.

**Follow-up work:** unchanged from Phase 12 close-out —
per-PR packet migrations under the now-CI-gated 12.0
ratchet, each starting with a reconciliation step as
documented in `shared/Packets/README.md`'s Migration-class
section. First-wave candidates (shortest reconciliation
work expected) listed by hash in the README; re-run
`audit-packet-divergence.sh` after each migration to track
the cosmetic-.cpp shortlist as it shrinks.

### Phase 18 — End-to-end runtime smoke test — done 2026-04-20

The first full live run of the modernized tree — client + three-
server cluster + MySQL — on WSL2 Ubuntu 22.04 / WSLg. Exercised
login → character-select → gameserver handoff → spawn → zone
travel → NPC interaction → combat → XP → level-up → stat
allocation.

Phase 18 is a **validation** phase, not a code-cleanup phase. Its
output is a list of runtime bugs that the compile-only build
hygiene passes (1–17) couldn't catch, plus the fixes that shook
out of actually running the game.

**Bugs found and fixed.** Letter suffixes continue the sequence
started in the pre-smoke-test build fix wave (Q–OO).

| ID | Area   | Summary                                                                          |
| -- | ------ | -------------------------------------------------------------------------------- |
| PP | Server | Server binaries exited on bare invocation — required explicit `-f <conf>`.       |
| QQ | Client | Client MAINMENU stuck, no transition to login form.                              |
| RR | Server | `gameserver` had a PID but wasn't listening on `:9998` — config / startup wiring.|
| SS | Client | **PENDING.** SDL port's `DXKeyboardEvent` drops letter keys. Workaround in place.|
| TT | Server | `loginserver` closed socket after sending `LCPCList` — pre-parse disconnect.     |
| UU | Client | `S_SLOT::ZeroMemory` clobbered `std::string`/`std::vector` members (libstdc++ SSO).|
| VV | Client | `SPK_*_FRIEND` filepath macros didn't match retail data; null-deref at `0xc0` after `SET CHARINFO`. |
| WW | Client | `DrawLightBuffer3D` deref of never-allocated `m_pLightBufferTexture` on dark-zone entry. |

**Commit reference:** `dbc3087` (`modernize/phase4-sprite`), 16
files changed, +1771 / −95. Detailed root-cause write-ups for
each bug live in `SMOKE_TEST_RESULTS.md` and `LOGIN_SMOKE.md`
at the repo root.

**Deferred from Phase 18:**

- **Bug SS (keyboard-event letter-drop).** Workaround keeps
  gameplay possible; real fix pending a proper
  `SDL_TEXTINPUT` vs. `SDL_KEYDOWN` event-loop split in
  the SDL backend. Small follow-up, not blocking.
- **Proper SDL2 `DrawLightBuffer3D` port.** Bug WW's null-
  guard short-circuits the overlay rather than compositing
  it. Dark zones currently render lit. Full port requires
  replicating the old DirectDraw auxiliary-texture
  composite against SDL2 render targets. Not a regression
  (current behaviour is "lit where it should be dark",
  which is strictly better than "SIGSEGV") but worth
  finishing when someone has a rendering-shaped afternoon.
- **Windows-build smoke test.** The current pass validated
  WSL2-Linux only. A cross-compile or native-MSVC build
  will surface its own set of issues (see
  [`ROADMAP.md`](./ROADMAP.md) R5).

**Follow-up work (tracked outside MODERNIZATION.md):**

Runtime-behavior and product-facing items that came out of this
smoke test — English-only localization, bleeding-VFX gate,
branding scrub, standalone Windows `.exe`, UHD / high-DPI
rendering — are **not** modernization work and live in
[`ROADMAP.md`](./ROADMAP.md) as items R1–R5. They share no
scope with this file; update ROADMAP.md when those progress,
not this one.

## Out of scope for modernization

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
