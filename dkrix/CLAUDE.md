# OpenDarkEden Client (`dkrix`)

The Dark Eden game client: an isometric MMORPG with Slayer, Vampire, and Ouster
player races. Legacy C++11 under active modernization.

Workspace-level layout, the release pipeline, and DB rules live in
`../CLAUDE.md` at the workspace root. Migration status is tracked in
`../docs/MODERNIZATION.md` — that is the single source of truth. Do not add new
status documents here.

## Build

Primary validation path on this workstation is a native Windows build:

```powershell
cmake -S dkrix -B dkrix\build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build dkrix\build --config Debug
```

Release builds are normally produced by `PUBLISH_RELEASE.cmd` rather
than invoked by hand; it builds the `DarkEden` target in Release and stages the
result into `Darkeden`.

Dependencies: CMake 3.20+, SDL2, SDL2_image, SDL2_ttf, SDL2_mixer (optional but
recommended), Visual Studio 2022 or Build Tools 2022 with MSVC C++.

Platforms: Windows is the primary validation target. macOS is a historical
sanitizer path (`make debug-asan` — not authoritative). Linux is a secondary
portability target. The VC6 / DirectX 9 SDK build is deprecated; old notes
mentioning it are archaeology only.

## Repository structure

- `Client/` — main game client code
- `Client/DXLib/` — SDL-backed input/sound/music behind legacy DirectX-shaped APIs
- `Client/SpriteLib/` — sprite rendering and pack management
- `Client/TextSystem/` — SDL2 + freetype text rendering
- `Client/framelib/` — frame handling
- `Client/WinLib/` — legacy residue, effectively dead
- `VS_UI/` — UI framework and widgets
- `basic/` — low-level utility and platform abstraction
- `tools/launcher/` — `package_client.ps1`, `serve_publish.ps1`, `DarkEdenLauncher.ps1`
- `tools/viewers/` — creature, effect, item, map, sprite viewers and `zone_parser`
- `tools/engine/sprite/` — a *second* sprite system powering the viewers
- `tools/resource_management/` — `extract_macros.py` and related helpers
- `publish/`, `build/`, `test-install/` — generated, gitignored

## Current technical shape

- `D3DLib/` has been deleted.
- `DXLib/` still exposes some DirectX-shaped interfaces backed by SDL.
  **[2026-08] Phase 3 item 1**: the input and audio classes were renamed to
  `InputManager` / `AudioManager` (files `InputManager.{h,cpp}` /
  `AudioManager.{h,cpp}`), and the `CDirectSound_Adapter.cpp` /
  `CDirectMusic_Adapter.cpp` / `CDirectSoundStream_Adapter.cpp` shim twins
  were deleted — they duplicated symbols already defined in the base
  `.cpp` files whenever `HAVE_SDL2_MIXER` was on, and two of the three
  didn't even compile (leftover pre-rename ctor/dtor names). `CDirectMusic`
  and `CDirectSoundStream` keep their names; only their dead `_Adapter.cpp`
  twin was removed. See `docs/MODERNIZATION.md` Phase 3.
- `SpriteLib/` still carries 555/565/4444 pixel-format variants that SDL could
  handle natively; consolidation is in progress.
- `VS_UI/src/VS_UI_Base.cpp` still uses a Windows GDI text path that should be
  routed through `TextSystem`.
- The build relies on `file(GLOB …) + FILTER EXCLUDE` to drop duplicate-symbol
  sources. Adding files can silently collide — check the exclusion regexes in
  `CMakeLists.txt` when a new `.cpp` doesn't seem to compile.
- Two sprite systems exist (`Client/SpriteLib/` and `tools/engine/sprite/`).

Defer to `../docs/MODERNIZATION.md` for the authoritative roadmap.

## Running the game

The live install lives at `Darkeden/` under the workspace root (not inside this
client tree). To test a fresh build, run `PUBLISH_RELEASE.cmd` from the
workspace root, then launch from there:

- `RUN_LOCAL_CLIENT.cmd` — clears trace logs, connects to `127.0.0.1:9999`
- `RUN_TEST_CLIENT.cmd` — clears trace logs, uses the configured server
- `RUN_PUBLIC_CLIENT.cmd` — plain launch

Server IP is set in `Darkeden/Data/Info/GameClient.inf`. Resolution profile is
set in `Darkeden/DarkEdenResolution.cfg` (e.g. `QHD`, `FHD`).

**Verify rendering and visibility changes in more than one resolution
profile.** Multiple past regressions (monster visibility, sight cleanup) only
reproduced at FHD.

Client-side trace logs — `game_heartbeat.log`, `login_flow_trace.log`,
`login_input_trace.log`, `startup_trace.log`, `combat_crash_marker.log`,
`interaction_trace.log`, `zone_transition_trace.log`, `pc_inventory_trace.log` —
are debugging scaffolding. They are excluded from release packaging and should
be removed from the source once the bug they were added for is confirmed fixed.

## Launcher and tester updates

`tools/launcher/package_client.ps1` builds a static update package under
`publish/client/`. For quick LAN testing directly from this repo:

The scripts resolve their own paths, so the defaults are correct from anywhere:

```powershell
powershell -ExecutionPolicy Bypass -File dkrix\tools\launcher\package_client.ps1 -Version dev-001
powershell -ExecutionPolicy Bypass -File dkrix\tools\launcher\serve_publish.ps1 -Prefix http://127.0.0.1:8765/
```

Use `-Prefix http://+:8765/` from an Administrator PowerShell to expose it on
the LAN.

For real tester releases use the root pipeline instead
(`PUBLISH_RELEASE.cmd` → `MAKE_DARKEDEN_RELEASE.cmd` →
`START_DARKEDEN_UPDATE_SERVER.cmd`); it is what
`Darkeden/DarkEdenLauncher.cmd` actually points at.

## Development notes

- Keep source files UTF-8; prefer English comments when touching mixed-language
  files.
- Preserve gameplay behavior unless a human explicitly approves a change.
- Treat Windows build failures as real blockers, not optional portability bugs.
- When repo-local Win32 typedefs or shims conflict with the real Windows SDK,
  fix the abstraction rather than adding more conditional hacks.
- Packet definitions are shared with the server; wire-format changes must land
  in `dkrixserver` at the same time.
- `Darkeden__quarantine/` holds the original pre-modernization
  binaries. Reference only — never build from or ship out of it.

## Related

- Server tree: `dkrixserver` (upstream: https://github.com/opendarkeden/server)
- Upstream client: https://github.com/opendarkeden/client
