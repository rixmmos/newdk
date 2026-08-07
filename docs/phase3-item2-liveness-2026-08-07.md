# Phase 3 item 2 — DXLib DirectDraw-era liveness audit (2026-08-07)

Read-only audit of the item-2 candidate set in `dkrix/Client/DXLib/`. All paths below are relative to `dkrix/`.
Method per file: CMake source-list membership, includer map, word-bounded type usage (members/params/`new`/subclassing/static calls), symbol-duplication check. Commands in the appendix. Build context: `USE_SDL_BACKEND` is forced ON (`CMakeLists.txt:68`), so every claim below is about the one verified configuration (client CI green at `5ca240a`, Windows + SDL backend).

## Verdict table

| File | In build? | Includers | Type usage beyond includes | Verdict |
|---|---|---|---|---|
| `CDirectDraw.h` | yes (via every DXLib TU) | 7 (`CDirectDrawSurface.h:31`, `DXLib/DXLib.h:14`, `Client/DXLib.h:15`, `CShadowPartManager.cpp:6`, `ProfileManager.cpp:13`, own .cpp, StaticMembers) | class inside is `CSDLGraphics` — used in 20+ files; token `CDirectDraw` as a type: only 2 dead sites | RENAME → `CSDLGraphics.h` |
| `CDirectDraw.cpp` | yes — `DXLib/CMakeLists.txt:56` | n/a | defines only `CSDLGraphics::` methods; zero `CDirectDraw::` definitions in tree | RENAME → `CSDLGraphics.cpp` |
| `CDirectDraw_StaticMembers.cpp` | yes — `DXLib/CMakeLists.txt:63` | n/a | 26 `CSDLGraphics::` static defs; its own banner already says `CSDLGraphics_StaticMembers.cpp` | RENAME → `CSDLGraphics_StaticMembers.cpp` |
| `CDirectDrawSurface.h` | yes (header of a compiled TU) | 7 (incl. `MEventManager.h:11`, `MGuildMarkManager.h:14`, `ProfileManager.cpp:14` via the `Client/CDirectDrawSurface.h` wrapper) | heavily live: value members, stack objects, `new`, map values, API params — see below | REFACTOR (keep in item 2) |
| `CDirectDrawSurface.cpp` | yes — `DXLib/CMakeLists.txt:57` | n/a | sole definer of `CDirectDrawSurface::` (28 defs; no duplicate twin) | REFACTOR (keep in item 2) |
| `CDirectSetup.h` | no | 0 live — both umbrella includes are commented out (`Client/DXLib.h:21`, `DXLib/DXLib.h:19`); only its own .cpp | class `CDirectSetup` referenced nowhere; its macros (`DSETUP_REINSTALL`, `SHOW_*`) used nowhere else (each grep count = 1, its own header) | DELETE |
| `CDirectSetupGetVersion.cpp` | no — absent from `DXLIB_SOURCES` (:51-63) and from top-level CMake (0 hits) | n/a | defines `CDirectSetup::GetVersion` (SDL_GetVersion wrapper); never called | DELETE |
| `BIT_RES.CPP` (DXLib copy) | no — absent from `DXLIB_SOURCES` | only dead files: `DXLib/huffman.{h,cpp}`, `DXLib/mp3.h` (none compiled by any target) | duplicate of the LIVE `Client/BIT_RES.CPP` (diff = 2 lines, PCH include) | DELETE |
| `BIT_RES.H` (DXLib copy) | no | same dead includer set + own .CPP | `Bit_Reserve` users resolve to the `Client/` copy (quote-include, includer-dir-first) | DELETE |

Effort/risk, one line each:
- Deletes (4 files): trivial, zero build impact; wrong-guess cost ~nil — none is compiled and no live TU includes them. Guessing wrong on BIT_RES direction (deleting the `Client/` copy instead) would break MP3 music: `Client/huffman.cpp:398` and `Client/mp3.h:95` consume `Bit_Reserve`, chain feeds `CMP3` used by GameInit/GameMain/GameUI/UIMessageManager.
- Renames (3 files): small — 3 `git mv` + ~6 include-line edits + 3 CMake lines; header guard is already `__CSDLGRAPHICS_H__` (`CDirectDraw.h:14`). Wrong-guess cost: compile errors only, immediately visible in CI.
- CDirectDrawSurface pair: multi-day, Phase-4-coupled; wrong guess breaks event backgrounds, guild marks, profile/ help-message surfaces, VS_UI JPG cache — ≥12 TUs plus a silent runtime bridge (below).

## Key findings

**1. `CDirectDraw` the class does not exist; `CDirectDraw.h` defines `class CSDLGraphics` (`CDirectDraw.h:171`).**
No typedef/#define alias anywhere (grep for `#define CDirectDraw`/`typedef ... CDirectDraw`: 0). The only two textual type-uses are dead code:
- `VS_UI/WinMain.cpp:95-96` (`CDirectDraw gC_DD; CSpriteSurface gC_DDSurface;`) — file is excluded from the VS_UI lib on WIN32 by `CMakeLists.txt:194` (`list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX ".*WinMain.*")`). All `gC_DD*` references live only in this file. Caution: the `if(NOT WIN32)` branch (CMakeLists.txt:206-216) has no WinMain filter, so a hypothetical Linux client configure would glob it in and fail on line 95 — latent breakage, VS_UI territory, not item 2.
- `Client/MTopViewDraw.inl:321` — inside macro `DRAW_ALPHA_BOX_2D` (defined at :318 and :335, `PLATFORM_WINDOWS`-guarded); the macro is expanded nowhere in `Client/` or `VS_UI/`, so the token never compiles.
Residue note: on Windows the header still does `#include <ddraw.h>` (`CDirectDraw.h:22-24`) and keeps `LPDIRECTDRAW7`-typed stub members — real DirectX-era leftovers; the rename does not remove them (item 3 / later refactor).

**2. `CDirectDrawSurface` is live and structural — the prior lead confirms and widens.**
`class CDirectDrawSurface : public CSDLGraphics` (`CDirectDrawSurface.h:36`). Live usage in compiled TUs (DarkEden exe globs `Client/*.cpp`, `CMakeLists.txt:530-531`; VS_UI lib globs `VS_UI/src/*.cpp`):
- value member: `MEventManager.h:139` `CTypeTable<CDirectDrawSurface> m_EventBackGround`; pointer getter :133
- stack instances: `MGuildMarkManager.cpp:112,121,823,832`; `ProfileManager.cpp:424,467`; `UIMessageManager.cpp:8492,8498`; member `VS_UI/src/header/VS_UI_ExtraDialog.h:140`
- heap + container: `vs_ui_gamecommon2.cpp:10633,10717` `new CDirectDrawSurface`; `VS_UI_GameCommon2.h:976` `std::map<std::string, CDirectDrawSurface*>`
- API params: `UtilityFunction.h:34,39` `LoadImageToSurface/SaveSurfaceToImage(..., CDirectDrawSurface&)` — 7/3 caller files
- static call: `GameInit.cpp:1486` `CDirectDrawSurface::SetGammaFunction()`
- bridge: `MTopView.cpp:10952` and `:17706` `reinterpret_cast<CSpriteSurface*>(pSurface)` — under the SDL backend `CSpriteSurface` is standalone, NOT derived from `CDirectDrawSurface` (`SpriteLib/CSpriteSurface.h:22-28` vs the Windows-backend branch at :329), so these casts bridge two unrelated classes. This is why the pair is REFACTOR, not RENAME: the honest fix is unification with `CSpriteSurface`, which is Phase 4 territory.
Many of its methods are stubs ("Not implemented - use SDL_BlitSurface instead", `CDirectDrawSurface.cpp:154,162,170`), yet the class carries live state (dims, clip, `m_ddsd`) and is the VS_UI image-cache type. No symbol duplication: the .cpp is the only definer.

**3. The item-1 duplication pattern reappears as directory twins, dead side in DXLib this time.**
The MP3 decoder exists twice: `Client/{BIT_RES.CPP,huffman.cpp,mp3.*,reader.*,soundbuf.*,subdecoder.cpp,synfilt.cpp}` (live, compiled into DarkEden via the `Client/*.cpp` glob) and `Client/DXLib/` copies (dead — not in `DXLIB_SOURCES`, no compiled includers). Item 2 covers only `BIT_RES.{CPP,H}`; the other six dead DXLib twins are a same-evidence follow-up ticket. Glob trap for later: `Client/*.cpp` matches `BIT_RES.CPP` only on case-insensitive filesystems (fine on Windows CI; a Linux client build would drop it).
Softer duplication: `basic/ColorDraw` ("extracted from CDirectDraw", `ColorDraw.cpp:7`) re-defines the mask tables and `InitMask` in its own class — no ODR clash, but two sources of truth (SpriteLib uses ColorDraw per `CSpriteSurface.h:327`; DXLib code uses `CSDLGraphics`). Drift risk to note, not item-2 work.

**4. Historical `compile_commands.json` is from the parked line — do not cite it for main.** Its file entries point at `/Users/genius/project/opendarkeden/...` (dated Apr 22); it compiled a `Client/`-level `CDirectSetupGetVersion.cpp` that main does not build.

## Proposed execution order (safe-first, one green CI run between steps)

1. DELETE `CDirectSetup.h` + `CDirectSetupGetVersion.cpp`; optionally drop the two commented `//#include "CDirectSetup.h"` lines (`Client/DXLib.h:21`, `DXLib/DXLib.h:19`).
2. DELETE `DXLib/BIT_RES.CPP` + `DXLib/BIT_RES.H`. (File follow-up ticket for the six remaining dead DXLib decoder twins.)
3. RENAME `CDirectDraw.h/.cpp`, `CDirectDraw_StaticMembers.cpp` → `CSDLGraphics.h/.cpp`, `CSDLGraphics_StaticMembers.cpp`. Edit: `DXLib/CMakeLists.txt:56,63`, includes at `CDirectDrawSurface.h:31`, `DXLib/DXLib.h:14`, `Client/DXLib.h:15`, `CShadowPartManager.cpp:6`, `ProfileManager.cpp:13`.
4. STOP. `CDirectDrawSurface.{h,cpp}` stay as-is (live); their refactor is scheduled with the CSpriteSurface unification.

## What item 2 must NOT touch

- `Client/SpriteLib/CSpriteSurface.*` (incl. `_SDL/_Adapter/_Effects`) — SpriteLib = Phase 4. Boundary fact: only `_SDL` + `_Effects` compile (`SpriteLib/CMakeLists.txt:37-38`); `CSpriteSurface.cpp` and `_Adapter.cpp` are Phase 4's dead-code questions.
- The `reinterpret_cast` bridges (`MTopView.cpp:10952,17706`; `vs_ui_gamecommon2.cpp:~10607`) — behavior-preserving refactor, Phase 4.
- `Client/CDirectDrawSurface.h` (6-line redirect wrapper) and the `Client/DXLib.h` umbrella — item 3's file-organization move. (Step 3 may edit include *lines* inside them; it must not move/delete the files.)
- `Client/BIT_RES.{CPP,H}` and the rest of the live `Client/` MP3 decoder — live music path.
- `VS_UI/WinMain.cpp` — VS_UI territory; carries the dead `gC_DD`/`gC_DDSurface` globals and the latent non-WIN32 glob breakage noted above.
- `basic/ColorDraw` — separate dedup decision.
- `docs/MODERNIZATION.md` — untouched per instruction; step 1's commit should update it.

## Appendix — exact commands (run from `dkrix/`, 2026-08-07)

```sh
# CMake membership
grep -n "CDirectDraw.cpp\|CDirectDrawSurface.cpp\|CDirectDraw_StaticMembers.cpp\|set(DXLIB_SOURCES" Client/DXLib/CMakeLists.txt
grep -rn "BIT_RES\|CDirectSetup" Client/DXLib/CMakeLists.txt CMakeLists.txt   # -> 0
grep -n "set(USE_SDL_BACKEND\|file(GLOB CLIENT_MAIN_SOURCES\|add_executable(DarkEden" CMakeLists.txt   # 68 / 530 / 596
# Includers (count first, then list)
grep -rniE '#\s*include.*CDirect(Draw|DrawSurface|Setup)\.h|#\s*include.*BIT_RES\.H' --exclude-dir={.git,build} .
# Type usage beyond includes (word-bounded)
grep -rnw -I CDirectDraw . --exclude-dir={.git,build} | grep -v '#include'
grep -rnw -I CDirectDrawSurface . --exclude-dir={.git,build} | grep -v '#include' | grep -v 'Client/DXLib/'
grep -rn -I '#define CDirectDraw\b\|typedef[^;]*CDirectDraw\b' . --exclude-dir={.git,build}   # -> 0
grep -rn -I DRAW_ALPHA_BOX_2D Client VS_UI    # only the two #define sites
# Symbol duplication
grep -rn "CDirectDraw::" --include=*.cpp . | grep -v CDirectDrawSurface::    # -> 0 definers
grep -rn "CDirectDrawSurface::" --include=*.cpp . | cut -d: -f1 | sort | uniq -c   # -> only DXLib/CDirectDrawSurface.cpp defines
# Twins
diff Client/BIT_RES.CPP Client/DXLib/BIT_RES.CPP | wc -l    # 2
grep -rnw Bit_Reserve Client/huffman.h Client/huffman.cpp Client/mp3.h
for m in DSETUP_REINSTALL SHOW_UPGRADES SHOW_PROBLEMS; do grep -rnw -I $m . --exclude-dir={.git,build} | wc -l; done  # 1 each
grep -o '"file": "[^"]*"' compile_commands.json | grep -iE 'BIT_RES|CDirectSetup'   # parked-line macOS paths only
```
