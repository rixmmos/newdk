# Step 2: Server build + config + launch

Goal: install build deps, compile the three server binaries
(`sharedserver`, `loginserver`, `gameserver`), rewrite the three
config files so they point at `127.0.0.1` and the correct
`HomePath`, then bring the three daemons up in the right order and
watch them bind ports without crashing.

## Why this step is three-in-one

I originally split "server build" and "server config + launch" into
separate tasks, but the build hasn't actually happened yet — there's
no `dkrixserver/build/` and no binaries under `dkrixserver/bin/`.
Keeping build and launch in one pass is better anyway because they
share the same surface area for diagnosing failures: if launch
crashes looking for a symbol or a data file, we want both sets of
logs in front of us.

Launch order matters: `sharedserver` exposes the shared-world state
that both `loginserver` and `gameserver` talk to, so it goes up
first; `loginserver` second; `gameserver` last.

## 1. Install build dependencies

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    pkg-config \
    libmysqlclient-dev \
    liblua5.1-0-dev \
    libxerces-c-dev
```

Why each:
- `build-essential` + `cmake` + `pkg-config` — the toolchain.
- `libmysqlclient-dev` — server uses the MySQL C API directly
  (`mysql_real_connect`, etc.). The MariaDB-compatible alternative
  is `libmariadb-dev-compat` if you'd rather not pull MySQL client
  libs in — either works.
- `liblua5.1-0-dev` — the server embeds Lua 5.1 for quest scripts.
  `CMakeLists.txt` does `pkg_check_modules(LUA51 lua5.1)` first and
  falls back to manual path probing.
- `libxerces-c-dev` — XML config parsing.

Verify they're visible to pkg-config / the linker:

```bash
pkg-config --exists lua5.1 && echo "lua5.1 OK"
dpkg -l libmysqlclient-dev libxerces-c-dev 2>&1 | tail -4
```

## 2. Build the server

```bash
cd /mnt/c/dev/newdk/dkrixserver
make debug 2>&1 | tee /tmp/server_build.log
```

The `debug` target runs:

    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build -jN

Expected outcome: three binaries land in `dkrixserver/bin/`:
`gameserver`, `loginserver`, `sharedserver`.

Verify:

```bash
ls -la /mnt/c/dev/newdk/dkrixserver/bin/
file /mnt/c/dev/newdk/dkrixserver/bin/gameserver
```

If the build fails, same drill as the client: paste me
`grep -m1 'error:' /tmp/server_build.log` and I'll add fixes to the
same workflow. Likely failure modes given recent phases:

- `mysql.h: No such file or directory` → `libmysqlclient-dev` not
  picked up; check `find /usr/include -name mysql.h`.
- `lua.h: No such file` → `liblua5.1-0-dev` not picked up; check
  `pkg-config --cflags lua5.1`.
- `undefined reference to mysql_*` → linker couldn't find
  libmysqlclient; `ls /usr/lib/x86_64-linux-gnu/libmysql*`.
- Xerces parse errors → `libxerces-c-dev` missing.

## 3. Rewrite the three config files

Currently all three configs point at `192.168.0.16` (the original
dev LAN box) and `HomePath : /home/genius/project/opendarkeden/server`
(somebody's home dir from 2003). Both need to become local.

### 3a. Swap `192.168.0.16` → `127.0.0.1`

```bash
cd /mnt/c/dev/newdk/dkrixserver/conf

# Back up first
cp gameserver.conf gameserver.conf.bak
cp loginserver.conf loginserver.conf.bak
cp sharedserver.conf sharedserver.conf.bak

# Rewrite
sed -i 's/192\.168\.0\.16/127.0.0.1/g' gameserver.conf loginserver.conf sharedserver.conf

# Verify: expect 0 hits
grep -n "192.168.0.16" gameserver.conf loginserver.conf sharedserver.conf
```

### 3b. Rewrite `HomePath`

`HomePath` is load-bearing — it's prefixed onto every SMP/SSI data
file path (`HomePath/data/<zone>.smp`, etc.) and at least 29
call sites across 10 server files read it. On Enrico's box the
dkrixserver tree lives at `/mnt/c/dev/newdk/dkrixserver` so that's what
goes in.

```bash
sed -i 's|^HomePath : .*|HomePath : /mnt/c/dev/newdk/dkrixserver|' \
    gameserver.conf loginserver.conf sharedserver.conf

# Verify
grep -n "^HomePath" gameserver.conf loginserver.conf sharedserver.conf
```

Note: loginserver and sharedserver may not actually have a
`HomePath` line (they don't need data files). The sed is a no-op
where the line doesn't exist — harmless.

### 3c. Leave the placeholder IPs alone (deliberately)

Four values currently read `111.111.111.111`:

- `gameserver.conf`: `BillingServerIP`, `LogServerIP`
- `loginserver.conf`: `BillingServerIP`, `LogServerIP`

These are external services we don't have. The billing server is
required for real-money transactions; the log server is a separate
central logging daemon. For a smoke test we want the server to
attempt connection, fail, and continue — which is what happens with
an unreachable IP (the TCP connect times out / gets refused and the
server moves on). If you replace them with `127.0.0.1`, the
connect will fail *instantly* with `ECONNREFUSED`, which some code
paths handle worse than a slow timeout. Leave them.

If the server refuses to start because of these, we'll deal with
it reactively.

### 3d. `BillingGameNo` and `Dimension`

Both have sensible defaults (`1`) in gameserver.conf. No change.

## 4. Launch order: shared → login → game

Each server daemonizes by default, writing its own log. For a
smoke test I want them in foreground so we can see crashes
immediately. Three terminals:

### Terminal 1 — sharedserver

```bash
cd /mnt/c/dev/newdk/dkrixserver
./bin/sharedserver conf/sharedserver.conf 2>&1 | tee /tmp/shared.log
```

Look for:
- No "failed to connect to DB" errors
- `Listening on port 9977` or equivalent
- No immediate exit

### Terminal 2 — loginserver (only after sharedserver is stable)

```bash
cd /mnt/c/dev/newdk/dkrixserver
./bin/loginserver conf/loginserver.conf 2>&1 | tee /tmp/login.log
```

Look for:
- DB connects to DARKEDEN + USERINFO cleanly
- Binds port 9999
- Connects to sharedserver on 127.0.0.1:9977

### Terminal 3 — gameserver (last)

```bash
cd /mnt/c/dev/newdk/dkrixserver
./bin/gameserver conf/gameserver.conf 2>&1 | tee /tmp/game.log
```

Look for:
- DB connects cleanly
- Data/*.bin and Data/*.tab loads succeed
- Binds port 9998
- Connects to sharedserver + loginserver

## 5. Verify ports are up

In a fourth terminal:

```bash
ss -ltnp | grep -E ':(9977|9998|9999)\s'
# expect three LISTEN sockets, one per server
```

## 6. Known ways this fails and what to do

- **"Access denied for user 'elcastle'@'localhost'"** — `elcastle`
  not created for the `localhost` host. Rerun the `CREATE USER
  'elcastle'@'localhost'` block from step 1 section 3.
- **"Unknown database 'DARKEDEN'"** — the schema dump didn't land.
  Reverify with `mysql -u elcastle -pelca110 -e "SHOW DATABASES;"`.
- **Segfault on startup with Xerces error** — XML config somewhere
  (not the `.conf`; there's a separate XML file for item data).
  Check `dkrixserver/data/` for `.xml` files and look at the
  stack trace in the log.
- **"Cannot open Data/XXX.smp"** — `HomePath` is wrong.
  Double-check it points at a directory that contains `data/`
  subdir with the `.bin` / `.smp` files.
- **gameserver hangs at billing-server connect** — expected behavior
  if `BillingServerIP : 111.111.111.111` times out slowly. If it
  hangs for more than ~2 minutes, we'll patch the billing client
  to fail-fast in a follow-up.

## 7. Report back

Paste me:

1. `ls -la /mnt/c/dev/newdk/dkrixserver/bin/` after build
2. Last 20 lines of each of `/tmp/shared.log`, `/tmp/login.log`,
   `/tmp/game.log` — especially any "ERROR" or stack traces
3. Output of `ss -ltnp | grep -E ':(9977|9998|9999)\s'`

That tells me whether we can move to step 3 (client connect probe)
or whether we have to debug a launch failure first.
