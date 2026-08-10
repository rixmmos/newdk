# LOGIN_SMOKE.md — end-to-end login + character create + enter world

**Prereqs satisfied:** client+server binaries built; MariaDB 10.6 installed
with `DARKEDEN` + `USERINFO` schemas loaded; user `elcastle` / `password`
reachable on `127.0.0.1:3306`.

This runbook picks up where `STEP3_CLIENT.md` stopped (title screen,
no login packet sent). The goal is:

1. Client ↔ loginserver auth succeeds.
2. Character select shows slot UI.
3. Create one character, enter the world, walk one step.

Every new bug that surfaces past this point is **Bug PP** and following
— almost certainly in the Phase 17 packet-divergence surface (163 class
pairs, 0 bit-identical in the pre-tonight audit).

---

## 1. Patches applied for this run (already in tree)

Three server confs and one client file were patched in-place against
a legacy-operator config:

| File | What changed |
|---|---|
| `dkrixserver/conf/loginserver.conf` | `HomePath`, Billing/DB/Log hosts: `192.168.0.16` + `/home/genius/...` → `127.0.0.1` + `/mnt/c/dev/newdk/dkrixserver` |
| `dkrixserver/conf/sharedserver.conf` | Same IP + HomePath sweep |
| `dkrixserver/conf/gameserver.conf` | Same IP + HomePath sweep (BillingServerIP left at `111.111.111.111` — fake, unused on `IsNetMarble=0` path) |
| `Darkeden data/Data/Info/ServerInfo.inf` | `LoginServerAddress : 61.160.221.177` → `127.0.0.1`; `LoginServerPort : 6999` → `9999`; `LoginServerBasePort : 9996` → `9900` (matched to `loginserver.conf`) |

Credentials (`elcastle` / `password`) stay in plaintext in the confs for
this local smoke run. The `${DKRIX_*}` env-var template re-application
is still a separate follow-up; see SMOKE_TEST_RESULTS.md step 3.

---

## 2. Runtime-state checklist (before starting servers)

Run these on WSL, from any directory. Paste output if something is red.

```bash
# MySQL: reachable and schemas present?
systemctl is-active mariadb 2>/dev/null || sudo service mariadb status
mysql -uelcastle -ppassword -e "SHOW DATABASES;"
mysql -uelcastle -ppassword DARKEDEN -e "SHOW TABLES LIKE 'Player'; DESCRIBE Player;" | head -10

# No stale server processes on the target ports?
ss -ltnp | grep -E ':(3306|9977|9998|9999) '
pgrep -af 'sharedserver|loginserver|gameserver' || echo "(none running)"

# Env: no DKRIX_* expected for this run (we hard-coded in conf/).
env | grep -E '^DKRIX_' || echo "(no DKRIX_ vars set — fine)"
```

Expected state: mariadb active, `DARKEDEN` + `USERINFO` listed, `Player`
table with `PlayerID`/`Password` columns, ports 9977/9998/9999 free.

---

## 3. Create the test account

```bash
mysql -uelcastle -ppassword DARKEDEN < /mnt/c/dev/newdk/create_test_account.sql
```

Expected final `SELECT` row:

```
PlayerID  Password   Name      Access  LogOn    creation_date
testuser  testpass   testuser  ALLOW   LOGOFF   2026-04-20
```

If `testuser` already exists (e.g. from a prior attempt), the script is
idempotent — the `INSERT IGNORE` skips, the `UPDATE` resets `LogOn`
back to `LOGOFF` so the "already logged in" branch doesn't fire.

---

## 4. Launch the three-daemon server cluster

```bash
bash /mnt/c/dev/newdk/start_servers.sh start
```

The script starts `sharedserver` → `loginserver` → `gameserver` with
brief sleeps between each so each binary has time to bind its listen
port before the next one opens its outgoing link.

Expected last output:

```
  sharedserver pid=NNNN
  loginserver  pid=NNNN
  gameserver   pid=NNNN

Listening ports:
  LISTEN 0  ... 0.0.0.0:9977 ...
  LISTEN 0  ... 0.0.0.0:9998 ...
  LISTEN 0  ... 0.0.0.0:9999 ...
```

If any row says `pid=-`, tail the corresponding log:

```bash
tail -80 /mnt/c/dev/newdk/dkrixserver/sharedserver.log
tail -80 /mnt/c/dev/newdk/dkrixserver/loginserver.log
tail -80 /mnt/c/dev/newdk/dkrixserver/gameserver.log
```

Common first failures to expect (document as new bugs if they surface):

- **DB connect refused** — MariaDB not listening on 127.0.0.1, or
  `elcastle` host grants missing (`%` vs `localhost` vs `127.0.0.1`
  are distinct in MariaDB — all three were granted during the
  STEP1_MYSQL run).
- **`HomePath does not exist`** — a server logs this and aborts if it
  can't `chdir` into the path from `conf/*.conf`. Should be fine now;
  all three confs point at `/mnt/c/dev/newdk/dkrixserver`.
- **Port already in use** — a prior crashed server still listening.
  `bash /mnt/c/dev/newdk/start_servers.sh stop` kills them.

To watch all three logs live while clicking through the UI:

```bash
bash /mnt/c/dev/newdk/start_servers.sh tail
```

---

## 5. Launch the client and attempt login

From a second terminal (keep the server logs tailing):

```bash
bash /mnt/c/dev/newdk/client_smoke.sh
```

The WSLg window opens to the title screen (already verified during
task #103). This time we actually click **LOGIN**.

- ID: `testuser`
- Password: `testpass`

**Expected next packet flow** (from `CLLoginHandler.cpp:273`):

1. Client → loginserver: CGLoginPacket (id+password)
2. Loginserver runs `SELECT Password FROM Player WHERE PlayerID = 'testuser'`
3. Loginserver runs `SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, LoginIP, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = 'testuser'`
4. Loginserver → client: GCLoginOK + server list
5. Client requests character list
6. Client shows empty slot UI

**Any visible error or silent stall past this point is Bug PP.**
Capture:
- The last 30 lines of `loginserver.log`
- A screenshot of the client state
- The value of the first differing byte between the client's sent
  packet and the server's expected layout (Phase 17 audit output)

---

## 6. Create character and enter world

From the slot UI:
- Slot 0 → **New**
- Pick race (Slayer is the safest first choice — most-tested in audit)
- Enter name `rixmain`, finalize

Expected: client switches to the in-world scene, renders a map tile,
keyboard movement works. `Player.LogOn` moves `LOGOFF → LOGON → GAME`.

Verify in DB after entry:
```sql
SELECT PlayerID, LogOn, CurrentServerGroupID
  FROM DARKEDEN.Player
 WHERE PlayerID = 'testuser';
-- expect: LogOn='GAME', CurrentServerGroupID != 0
```

---

## 7. Shut down cleanly

```bash
# close the client window first (ESC → exit, or just close window)
bash /mnt/c/dev/newdk/start_servers.sh stop
```

On a clean shutdown, the servers will flush `LogOn = LOGOFF` for the
test user. If the client crashes, run `create_test_account.sql` again
to reset the flag before the next attempt.

---

## 8. Bug log for this phase

Start numbering new bugs at **Bug PP** in `SMOKE_TEST_RESULTS.md`. The
expected first couple (from pre-tonight Phase 17 audit context):

- **PP (expected):** First packet divergence on the login CS/SC pair.
  Source: `dkrixserver/src/Core/CLLoginPacket.cpp` vs
  `dkrix/Client/Packet/Cpackets/CGLoginPacket.cpp`.
- **QQ (expected):** Character-select packet divergence.
- **RR (expected):** Zone-enter packet divergence.

Any of these that turns out to be bit-identical is a pleasant surprise.
