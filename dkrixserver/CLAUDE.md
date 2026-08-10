# DarkEden Server (`dkrixserver`)

Guidance for working in the server tree. Workspace-level layout, the client
release pipeline, and DB backup rules live in `../CLAUDE.md` at the workspace
root; engineering principles live in `../docs/CLAUDE.md`.

## Build Commands

### Building the project
```bash
# Standard release build
make

# Debug build
make debug

# Clean build artifacts
make clean
```

The project uses CMake. The Makefile wraps CMake commands for convenience:
- `make` - **Builds Debug** (`Makefile:11` is `all: debug`). This line previously
  said Release; that was wrong and is load-bearing, because `Assert()` is
  `((void)0)` only under `NDEBUG` — which Debug does not set. Two separate
  security reviews concluded from this doc line that every `Assert`-based bounds
  check is compiled out in production and reported exploitable overflows on that
  basis. They are not: the deployed binaries are Debug, `Assert` throws
  `AssertionError` (`src/Core/Assert.h:34-35`), and the stringified assert
  expressions are present in `bin/gameserver` [measured 2026-08-10].
- `make release` - Release (`-O2 -DNDEBUG`). **This is the configuration where
  every `Assert`-based bounds check disappears.** Nothing in CI or the smoke-test
  runbook builds it.
- `make debug` - Builds with CMake in Debug mode (`-DCMAKE_BUILD_TYPE=Debug`)
- Build output binaries go to `bin/` directory
- Build output libraries go to `lib/` directory

For development, always choose debug build!

### Build trees per environment

Several build directories coexist and must not be cross-contaminated — each
holds a `CMakeCache.txt` pinned to its own toolchain and paths:

- `build/` — generic / local
- `build-wsl/` — WSL build (the usual dev path from this Windows workstation)
- `build-docker/`, `build-docker20/` — container builds (Ubuntu base variants)

If CMake complains about a moved source or compiler path, delete the offending
build dir rather than editing its cache.

### Docker

```bash
cd docker && docker compose up --build
```

`docker/docker-compose.yml` starts MySQL 5.7 (seeded from `initdb/`) plus the
server image built from `Dockerfile.pub`. `docker/start-servers.sh` launches
loginserver → sharedserver → gameserver and tears all three down if any one
exits. Exposed ports: 9999, 9998, 9997 (TCP) and 9997/UDP. See
`docker_install.md` for the fuller walkthrough; `Dockerfile.dev` is the
development image.

### Code formatting
```bash
# Format all C++ code
make fmt

# Check format for modified files only (fast)
make fmt-check

# Check format for all files (slow)
make fmt-check-all
```

The project uses clang-format with a `.clang-format` configuration file.
- Format checking is enforced via GitHub Actions on PRs
- Always run `make fmt` before committing changes

## Project Architecture

This is the **DarkEden** game server - an MMORPG server written in C++11.

### Server Architecture

The server is split into multiple coordinated processes:

1. **loginserver** - Handles authentication and character selection
2. **sharedserver** - Manages shared data (e.g., guild info) across game servers
3. **gameserver** - The main game logic server (one per world/zone group)

### Build System Structure

- **CMake** is the primary build system (CMakeLists.txt files throughout)
- **Legacy Makefiles** exist in subdirectories but are superseded by CMake
- Source files are organized by module under `src/`

### Key Directory Structure

```
src/
 Core/                      # Core library - shared utilities, no server-type dependencies
    Packets/               # Protocol packet definitions (GC, CG, CL, LC, GL, LG, GS, SG, GG)
    [core utilities]       # Socket, datagram, player info, items, skills, etc.
 server/
    database/              # Database abstraction layer and connection management
    gameserver/            # Main game server executable
       skill/             # Skill system module
       item/              # Item system module
       billing/           # Billing/payment module
       war/               # War system module
       couple/            # Couple/party system module
       mission/           # Mission system module
       ctf/               # Capture the flag module
       quest/             # Quest system (with Lua scripting)
       mofus/             # Game events module
       exchange/          # Player exchange/auction system
    loginserver/           # Login server executable
    sharedserver/          # Shared server executable
 Core/CMakeLists.txt        # Defines packet libraries and Core library
```

### Packet System

Packets are the primary communication mechanism between servers and clients. They are organized by direction:

- **GC** (Game -> Client): Server sends to client
- **CG** (Client -> Game): Client sends to game server
- **LC** (Login -> Client): Login server sends to client
- **CL** (Client -> Login): Client sends to login server
- **GL** (Game -> Login): Game server communicates with login server
- **LG** (Login -> Game): Login server communicates with game server
- **GS** (Game -> Shared): Game server communicates with shared server
- **SG** (Shared -> Game): Shared server responds to game server
- **GG** (Game -> Game): Inter-game-server communication

Each packet type typically has two files:
- `PacketName.cpp` - Packet class definition
- `PacketNameHandler.cpp` - Handler that processes the packet

Packet definitions are mirrored in the client tree (`dkrix`). Any
wire-format change must land on both sides in the same change, or every
existing client build breaks.

### Preprocessor Macros

Key compile definitions that control behavior:
- `__GAME_SERVER__` - Compiled for gameserver
- `__LOGIN_SERVER__` - Compiled for loginserver
- `__SHARED_SERVER__` - Compiled for sharedserver
- `__COMBAT__` - Enables combat-related code

### Configuration

Server configurations are in `conf/`:
- `gameserver.conf` - Game server configuration
- `loginserver.conf` - Login server configuration
- `sharedserver.conf` - Shared server configuration
- `updateserver.conf` - Update server configuration
- `excel96-*.conf` / `.new` - alternate host profile; `conf/backup/` holds prior copies

Important settings:
- `HomePath` - Repository directory path (must be set correctly)
- `DB_HOST` - Database IP address
- `LoginServerIP` - Login server IP

Default ports: gameserver TCP 9998 / UDP 9997, loginserver UDP 9996 (base
9800), sharedserver 9977, client-facing 9999.

**Note**: Database `WorldDBInfo` and `GameServerInfo` tables must match config file settings.

Configs are host-specific and are frequently stale after a machine or network
change. Confirm `HomePath`, `DB_HOST`, and `LoginServerIP` against the actual
target before blaming the code for a connection failure.

## Database Setup

The project requires MySQL 5.7 or 8 with specific SQL mode settings:

```sql
-- Remove NO_ZERO_DATE and STRICT_TRANS_TABLES from sql_mode
set @@global.sql_mode = 'ONLY_FULL_GROUP_BY,NO_ZERO_IN_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';
```

Databases:
- `DARKEDEN` - Main game database
- `USERINFO` - User account database

Load schema with:
```bash
mysql -h 127.0.0.1 -u elcastle -D 'DARKEDEN' -p < initdb/DARKEDEN.sql
mysql -h 127.0.0.1 -u elcastle -D 'USERINFO' -p < initdb/USERINFO.sql
```

### Data fixes against a live database

Content edits (NPC placement, shop scripts, spawns, zone info) are applied
directly to a running database, so treat every one as a migration:

1. **Dump a backup first**, named `<subject>_backup_<yyyyMMdd_HHmmss>.sql` — the
   existing convention, e.g. `backup_jack_shop_npc_20260427_141412.sql`,
   `backup_eslainian_npc_sprites_20260427_160400.sql`.
2. Keep the fix script next to its backup and note in the commit which
   server/DB it was applied to.
3. Never run a script against a live server without confirming with the user
   first, and never rely on `-f` to push past errors.

Backups accumulate in the repo root of this tree. They are history, not
clutter — do not delete them without asking.

## Dependencies

Required libraries:
- **libmysqlclient-dev** (5.7) - MySQL client library
- **lua5.1-dev** or **luajit** - Lua scripting (used by quest system)
- **xerces-c** (3.2.3) - XML parsing (used by SXml in Core)

Install on Ubuntu/Debian:
```bash
sudo apt install libxerces-c-dev libmysqlclient-dev liblua5.1-dev
```

## Key Game Concepts

### Races
- **Slayer** - Human vampire hunters
- **Vampire** - Vampire race
- **Ousters** - Another playable race

### Core Game Systems
- **Zone/ZoneGroup** - Geographic areas where players exist
- **Creature** - Base class for all entities (players, monsters, NPCs)
- **PlayerCreature** - Player-controlled creatures (Slayer, Vampire, Ousters)
- **Zone.inf / NPC tables** - zone and NPC data lives partly in DB tables and
  partly in client-side `Data/Info/` files; the two must stay aligned or NPCs
  render in the wrong place (or not at all)
- **Effect** - Time-based effects applied to creatures
- **Skill** - Combat and utility abilities
- **Guild/Party** - Social grouping systems
- **DynamicZone** - Instanced content (e.g., dungeons)

## Running the Servers

Start servers in this order:
```bash
./bin/loginserver -f ./conf1/loginserver.conf
./bin/sharedserver -f ./conf1/sharedserver.conf
./bin/gameserver -f ./conf1/gameserver.conf
```

## Development Notes

- Source file encoding is **UTF-8** (project was migrated from legacy encodings)
- Use **English** as code comment, there are some legacy Korean or maybe garbled encoding, translate them to English whenever possible
- C++11 standard is used
- Threaded architecture with `ZoneGroupThread` for parallel zone processing
- Extensive use of inheritance (Creature  PlayerCreature  Slayer/Vampire/Ousters)
- Lua scripting is integrated for quest systems (see `quest/luaScript/`)
- Exchange system in `gameserver/exchange/` handles player trading
- Run `make fmt` before committing; format is enforced in CI
- Server stdout captured during debugging lands in `_server_logs_tmp/`
  — that folder is scratch and safe to clear
- Do not commit `build*/`, `bin/`, `lib/`, or generated CMake files
