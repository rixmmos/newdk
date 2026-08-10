# Step 2 — green launch snapshot

Captured 2026-04-20 after Task #96 closed. All three daemons running,
ports LISTEN, inter-server handshake flowing. Keep this file as the
reference for "what working looks like" so we can diff against it if
anything regresses later.

## ss -ltnp (the money shot)

```
LISTEN 0 5  0.0.0.0:9977  users:(("sharedserver",pid=81989,fd=3))
LISTEN 0 5  0.0.0.0:9998  users:(("gameserver",pid=82254,fd=4))
LISTEN 0 5  0.0.0.0:9999  users:(("loginserver",pid=82197,fd=8))
```

All three ports bound on all interfaces. No port conflicts.

## sharedserver (tail of /tmp/shared.log)

Clean path through: config dump → `SharedServer::init() start` →
`Init DatabaseManager` → `Connecting....... Another DB Server` →
`Connectiong: WorldID=0, HOST=127.0.0.1, DB=DARKEDEN, User=elcastle, Port=3306` →
`MAX SERVER GROUP = 1` → GameServerInfoManager / GameServerGroupInfoManager /
GameWorldInfoManager loads → `---------- Start SharedServer ----------` →
`Receive:GSRequestGuildInfo` → `Send:473[2,0] SGGuildInfo()`.

The last two lines are the handshake response to gameserver's guild-info
request — confirms the inter-server TCP channel is live in both
directions.

## loginserver (tail of /tmp/login.log)

Repeated cycles of `Loading GameWorldInfoManager... / GameWorldInfo(WorldID:1,...) /
Size : 0 / End GameWorldInfoManager Load / addGameServerGroupInfo: 1, 0 : Server1`.

Loginserver re-polls its world/group state in a loop — nothing is
failing; Size:0 just means there are no connected users yet, which is
correct pre-client-connect. Not a warning to chase.

## gameserver (tail of /tmp/game.log)

Full init chain, all landmarks hit:
`OPEN LOGIN DB` → `Mofus THREAD CONNECT DB` →
`THREAD CONNECT UIIRIBUTION DB` (x2) →
`connection to sharedserver established` →
`Send:438[0,0] GSRequestGuildInfo` → `Receive:SGGuildInfo()` →
**`>>> ALL INITIALIZATIONS ARE COMPLETED SUCCESSFULLY.`** →
**`>>> STARTING ClientManager->start() INFINITE LOOP...`** →
`GDR Lair THREAD CONNECT DB`.

Gameserver is now in its ClientManager accept loop, ready for
TCP connections on 9998.

## What got fixed in step 2

Four real bugs + one config fix, in order discovered:

1. **Bug Q — shadowing Endian.h** (`src/Core/Endian.h`). Case-insensitive
   DrvFs let our unused Phase-13B file shadow glibc's `<endian.h>`,
   breaking `htole32` / `le32toh` everywhere. Deleted the file;
   verification probe included in `server_build_fix.sh`.

2. **Bug R — `MYSQL_BIND::is_null` type** (`PreparedStatement.{h,cpp}`).
   Old code used `reinterpret_cast<char*>` which worked for MySQL 5.7's
   `my_bool*` but not 8.0's `bool*`. Replaced the typedef with
   `std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type` so it
   derives from the installed library header. No version-sniff `#ifdef`.

3. **Bug S — `catch (Error&)` in all three server main.cpps**.
   `IOException` lives in the `Exception` subtree of `Throwable`, parallel
   to `Error` — so config-load failures were escaping to `terminate()`
   with `what(): std::exception`. Widened to `catch (Throwable&)` in
   `sharedserver/main.cpp`, both catches in `loginserver/main.cpp`, and
   both catches in `gameserver/main.cpp` (also uncommented the cerr
   prints that had been commented out, making gameserver's config
   failures silent).

4. **Bug T — CRLF line endings in `Properties::load`**. Our confs on
   `/mnt/c/...` DrvFs are CRLF. `getline()` strips `\n` but leaves `\r`.
   `Properties::WhiteSpaces` is `" \t"` only, so blank-between-section
   lines containing just `\r` weren't recognised as empty and failed
   the `find(':')` check, throwing `IOException("missing separator")`.
   One-line fix: strip trailing `\r` before the empty/comment check.
   This also silently fixed a would-have-been-next-issue: values like
   `DB_PASSWORD : password` were parsing as `password\r`, which would
   have failed MySQL auth and looked like a wrong-password bug.

5. **Config — `odk-mysql` hostname in `WorldDBInfo`**. The shipped
   `DARKEDEN.sql` seeds the cross-world DB routing table with the Docker
   Compose service name. Added `127.0.0.1 odk-mysql` to `/etc/hosts`
   rather than mutating the seed data.

All four source fixes are in the tree. `server_build_fix.sh` carries
Bugs Q and R idempotently; Bugs S and T were applied directly to the
mains / Properties.cpp and should be checked into whatever branch the
smoke test lives on.

## Launch order reminder

```
Terminal 1:  ./bin/sharedserver  -f conf/sharedserver.conf
Terminal 2:  ./bin/loginserver   -f conf/loginserver.conf    (after #1 binds 9977)
Terminal 3:  ./bin/gameserver    -f conf/gameserver.conf     (after #2 binds 9999)
```

`-f` is required — the main.cpps throw `Usage : ... -f È¯°æÆÄÀÏ` without it.
