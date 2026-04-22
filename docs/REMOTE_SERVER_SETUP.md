# Remote Server Setup

This note captures the current internet-facing setup for the local
DarkEden test cluster.

## Current network plan

- Public IP: `90.190.31.134`
- Forwarded internal host: `192.168.0.16`
- Forwarded UDP: `9996-9997`
- Forwarded TCP: `9998-9999`

Those port ranges match the current server config:

- `loginserver.conf`
  - `LoginServerPort : 9999`
  - `LoginServerUDPPort : 9996`
- `gameserver.conf`
  - `TCPPort : 9998`
  - `GameServerUDPPort : 9997`

## Important split: local server config vs public client config

If `loginserver`, `sharedserver`, `gameserver`, and MySQL all run on the
same Windows 11 machine, the server-side config should usually stay on
local loopback for internal hops:

- `LoginServerIP : 127.0.0.1`
- `SharedServerIP : 127.0.0.1`
- `DB_HOST : 127.0.0.1`

Do **not** replace every `127.0.0.1` with the public IP. That would turn
same-machine traffic into unnecessary external routing and can break local
startup.

The public IP belongs in:

- client login config files
- database rows that tell the login server which game server IP to hand to
  remote clients

## Client-side address files

These files are now pre-pointed at the public IP:

- `Darkeden data/Data/Info/ServerInfo.inf`
- `Darkeden data/Data/Info/GameClient.inf`
- `Darkeden data/Data/Info/GameClient2.inf`

All three now use:

- `LoginServerAddress : 90.190.31.134`

## Database changes required for remote login

The login server sends the client to the game server using the
`DARKEDEN.GameServerInfo` table, not from `gameserver.conf`.

After importing `initdb/DARKEDEN.sql`, run:

```sql
USE DARKEDEN;

UPDATE GameServerInfo
SET IP = '90.190.31.134',
    TCPPort = 9998,
    UDPPort = 9997
WHERE ServerID = 1;
```

Verify:

```sql
SELECT ServerID, Nickname, IP, TCPPort, UDPPort, WorldID, GroupID, Stat
FROM GameServerInfo;
```

`WorldDBInfo` can stay on the local DB host if MySQL is on the same box.
The stock import uses a container hostname, so for a same-machine install
you will likely want:

```sql
USE DARKEDEN;

UPDATE WorldDBInfo
SET Host = '127.0.0.1',
    DB = 'DARKEDEN',
    User = 'elcastle',
    Password = 'elca110',
    Port = 3306;
```

## Windows 11 server machine: required software

Recommended path today is to run the server stack under WSL2 on the Windows
machine.

Install:

1. Windows 11 Pro with WSL2 enabled
2. Ubuntu 22.04 inside WSL2
3. CMake, gcc/g++, make
4. MySQL 8.0 or 5.7
5. Server build deps:
   - `libmysqlclient-dev`
   - `libxerces-c-dev`
   - `liblua5.1-dev`
   - `zlib1g-dev`
   - `nsl` support if your distro splits it

Typical Ubuntu packages:

```bash
sudo apt update
sudo apt install -y build-essential cmake git \
  libmysqlclient-dev libxerces-c-dev liblua5.1-dev zlib1g-dev
```

Also open Windows Defender Firewall on the Windows host for:

- UDP `9996-9997`
- TCP `9998-9999`

If MySQL runs on Windows instead of WSL, point the server conf `DB_HOST`
at the reachable host address and keep the SQL table rows aligned with that.

## Startup order

Start the services in this order:

1. MySQL
2. `loginserver`
3. `sharedserver`
4. `gameserver`

Example:

```bash
./bin/loginserver -f ./conf/loginserver.conf
./bin/sharedserver -f ./conf/sharedserver.conf
./bin/gameserver -f ./conf/gameserver.conf
```

## Windows client `.exe` status

The repo currently documents MinGW-w64 cross-compiling as the preferred
path for a standalone Windows client package. This workspace does not yet
have a MinGW toolchain installed, so config is ready for remote testing,
but the actual `.exe` build still needs that toolchain before packaging.
