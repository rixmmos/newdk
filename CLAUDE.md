# CLAUDE.md — Workspace Root

Top-level guidance for the whole `C:\newdk` workspace. It applies across every
subtree and takes precedence over `dkrix/CLAUDE.md` and `dkrixserver/CLAUDE.md`
when there is a conflict. Engineering principles live in `docs/CLAUDE.md`; the
running modernization plan lives in `docs/MODERNIZATION.md`.

This is a private DarkEden-like MMORPG project: a C++11 server, a C++11
isometric client mid-migration from Win32/DirectX to SDL2, plus the release,
launcher, and update-server tooling used to ship builds to testers.

## Actual folder layout

Do not trust older docs that mention `client/`, `server/`, `configs/`, or
`tooling/` — those directories do not exist. The real tree is:

| Path | What it is |
| --- | --- |
| `dkrix/` | Client source (C++11, CMake, SDL2). Has its own `CLAUDE.md`. |
| `dkrixserver/` | Server source: gameserver, loginserver, sharedserver. Has its own `CLAUDE.md`. |
| `docs/` | Project principles (`CLAUDE.md`), modernization plan, legacy KR/CN notes. |
| `tools/release/` | Release/publish/update-server PowerShell + Python scripts. |
| `tools/spk_tools.ps1` | SPK/565-bitmap inspection helpers. |
| `Darkeden/` | The live installed client. Build output is copied here before release. |
| `release_site/darkeden/` | Static update site served to testers (`manifest.json`, `CHANGELOG.md`, `files/`). |
| `Darkeden__quarantine/` | Original pre-modernization client binaries. Reference only — never build or ship from here. |
| `CHANGELOG.md` | Auto-appended release changelog. Written by the release script, not by hand. |
| `_server_logs_tmp/` | Scratch server logs pulled off Docker/WSL runs. Disposable. |

Root `*.cmd` files are thin wrappers over `tools/release/*.ps1`:

- `MAKE_DARKEDEN_RELEASE.cmd` → `New-DarkEdenRelease.ps1`
- `PUBLISH_RELEASE.cmd` → `Publish-DarkEdenRelease.ps1`
- `START_DARKEDEN_UPDATE_SERVER.cmd` → `Start-DarkEdenUpdateServer.ps1`

## Build baselines

The authoritative verification paths on this workstation:

```powershell
# Client — native Windows, Visual Studio 2022 + CMake + vcpkg
cmake -S C:\newdk\dkrix -B C:\newdk\dkrix\build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build C:\newdk\dkrix\build --config Debug
```

```bash
# Server — CMake via the Makefile wrapper (WSL or Docker)
cd /mnt/c/newdk/dkrixserver && make debug
```

Older `make debug-asan` / macOS notes are historical. They are useful as
migration history but are not the current source of truth. Treat a broken
Windows client build or a broken Linux server build as a real blocker.

## Release pipeline

The normal end-to-end flow for shipping a client build to testers:

1. `PUBLISH_RELEASE.cmd` — builds `dkrix` in **Release**, copies `Darkeden.exe`,
   required DLLs, and the MSVC CRT into `C:\newdk\Darkeden`, strips debug and
   runtime-log leftovers, then invokes the manifest step. Pass `-SkipBuild` to
   package an already-built tree. Build log lands in `publish_release.log`.
2. `MAKE_DARKEDEN_RELEASE.cmd` — hashes `C:\newdk\Darkeden` into
   `C:\newdk\release_site\darkeden` (`manifest.json` + `files/`), stamps a
   `yyyy.MM.dd.HHmm` version, and prepends an entry to `CHANGELOG.md`.
   Always pass meaningful `-ReleaseNotes`; the changelog is what testers read.
3. `START_DARKEDEN_UPDATE_SERVER.cmd` — serves `release_site/` over HTTP on
   port 8080 via `serve_release_site.py`, with an IP allowlist.

Testers run `Darkeden/DarkEdenLauncher.cmd`, which reads
`Darkeden/launcher_config.json` (`updateBaseUrl`, currently
`http://90.190.31.134:8080/darkeden`), downloads `manifest.json`, verifies
SHA-256 per file, updates only what changed, and launches `Darkeden.exe`.

Release packaging deliberately excludes `CrashDumps/`, `UserSet/`, `Log/`,
`Temp/`, `Update/`, `manifest.json`, `local_manifest.json`,
`RUN_LOCAL_CLIENT.cmd`, and `*.log` / `*.dmp` / `*.tmp` / `*.download`. If you
add a new local-only artifact under `Darkeden/`, extend that exclusion list in
`New-DarkEdenRelease.ps1` rather than relying on it not being noticed.

Never hand-edit `release_site/darkeden/manifest.json` or `CHANGELOG.md` —
regenerate them. A manifest whose hashes don't match `files/` silently breaks
every tester's launcher.

## Running the client locally

From `C:\newdk\Darkeden`:

- `RUN_LOCAL_CLIENT.cmd` — clears trace logs, connects to `127.0.0.1:9999`.
- `RUN_TEST_CLIENT.cmd` — clears trace logs, uses the configured server.
- `RUN_PUBLIC_CLIENT.cmd` — plain launch, no log clearing.
- `DarkEdenLauncher.cmd` — the tester path (update-then-launch).

`DarkEdenResolution.cfg` selects the resolution profile (e.g. `QHD`, `FHD`).
Resolution-dependent rendering bugs must be checked in more than one profile —
several past regressions were visible only at FHD.

Server IP for the client lives in `Darkeden/Data/Info/GameClient.inf`.

## Server operations

- Configs in `dkrixserver/conf/`: `gameserver.conf`, `loginserver.conf`,
  `sharedserver.conf`, `updateserver.conf`, plus `excel96-*` variants.
  `HomePath`, `DB_HOST`, and `LoginServerIP` must match the actual host, and
  the DB `WorldDBInfo` / `GameServerInfo` tables must agree with them.
- Docker: `dkrixserver/docker/docker-compose.yml` brings up MySQL 5.7 plus the
  server image; `start-servers.sh` launches login → shared → game in order and
  exits if any one dies. Ports 9999/9998/9997 (TCP) and 9997/UDP.
- Build trees are separated per environment: `build/`, `build-wsl/`,
  `build-docker/`, `build-docker20/`. Don't cross-contaminate them.

## Database change discipline

Schema and content edits are made against a live MySQL instance, so:

- **Always dump a backup before a data fix**, named
  `<subject>_backup_<yyyyMMdd_HHmmss>.sql`, matching the existing convention
  (`npc_eslanian_positions_backup_20260426_223618.sql`,
  `pet_dog_spawn_backup_20260428_155833.sql`).
- Keep the fix script and its backup together, and say in the commit message
  which server/DB it was applied to.
- Databases are `DARKEDEN` (game) and `USERINFO` (accounts). MySQL `sql_mode`
  must exclude `NO_ZERO_DATE` and `STRICT_TRANS_TABLES`.
- Never point a script at a live server without confirming with the user first.

## Repo hygiene

Root currently holds a lot of incidental debris: `Darkeden.rar` (~700 MB),
`screen_capture*.png`, `gameserver_*.out` / `loginserver_*.out`,
`release_build.log`, `last_build.log`, and a literally-named
`char_zone_rescue_20260426_$(Get-Date -Format yyyyMMdd_HHmmss).sql` (a
PowerShell quoting bug). Do not add more of this. When creating throwaway
artifacts, put them in `_server_logs_tmp/` or outside the repo, and prefer
adding a root `.gitignore` entry over committing them.

Do not commit build outputs (`build*/`, `bin/`, `lib/`, `publish/`),
game data blobs, or secrets.

## Working agreements

- Source encoding is UTF-8. Migrate mixed-language comments to English as you
  touch them.
- Run the formatter before committing: server `make fmt`; client, match the
  prevailing file style.
- Prefer new commits over amending; do not skip hooks.
- Preserve observable gameplay behavior (damage math, skill effects, drop
  tables, packet semantics) unless the user explicitly approves a change. The
  structure around that logic is fair game.
- Small, independently reviewable and revertable changes. No shallow hacks to
  make the build go green — fix the abstraction instead.
- Update `docs/MODERNIZATION.md` as part of the change that makes it stale.

## When in doubt

Ask. If a request conflicts with these priorities — a "just make it compile"
ask that needs a shallow hack, or a DB edit against a live server — raise the
tradeoff and propose the smallest clean alternative first.
