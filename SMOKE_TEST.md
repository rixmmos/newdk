# Runtime smoke test — single-host DarkEden bringup

**Goal:** get one client to the login screen, talking to a server cluster you
just built, against a fresh MySQL. Write down every failure as you go; I'll
diagnose them as you paste.

**Your machine assumed:** Ubuntu 22.04 LTS on x86_64, `sudo` available.
macOS notes in a sidebar at the bottom.

**Branch to test:** `modernize/phase4-sprite` at tip `7ee8618` (Phase 17
close-out). This is the current tip of all the modernization work. If your
local repo is stale, pull first.

Everything below assumes the repo is at `~/work/dkrix-upstream/` with
`dkrix/` and `dkrixserver/` as subdirs. Adjust paths to match your layout.

---

## 0. Paste-back contract

For each numbered step: if it works silently, move on. If it errors, paste
back the full error (command + last ~30 lines of output) before trying the
next step. Don't try to fix things yourself unless the fix is obvious —
the point is to see where the tree actually breaks, not to route around
issues.

Reserve a scratch file for yourself at `~/smoke-test-log.txt` if you want
to keep everything in one place.

---

## 1. OS prereqs

```sh
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake git pkg-config \
    libxerces-c-dev libmysqlclient-dev liblua5.1-dev \
    libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
    libjpeg-dev libfreetype-dev \
    mysql-server \
    psmisc tmux
```

Check:

```sh
cmake --version      # expect >= 3.16
g++ --version        # expect 11.x (Ubuntu 22.04 default)
mysql --version      # expect 8.0.x
pkg-config --modversion sdl2 xerces-c
```

**Known gotcha:** the server's CMake uses `find_library(MYSQLCLIENT)`.
If that fails on your box, check `/usr/lib/x86_64-linux-gnu/` has
`libmysqlclient.so`. If it's called `libmysqlclient.so.21` only (no
unversioned symlink), run:

```sh
sudo ln -s /usr/lib/x86_64-linux-gnu/libmysqlclient.so.21 \
           /usr/lib/x86_64-linux-gnu/libmysqlclient.so
```

---

## 2. MySQL: start, fix sql_mode, load schema

### 2.1 Start and secure (one-time)

```sh
sudo systemctl start mysql
sudo systemctl enable mysql
# optional: sudo mysql_secure_installation
```

### 2.2 Relax sql_mode

DarkEden's `initdb/DARKEDEN.sql` was dumped from MySQL 5.7. MySQL 8's
default sql_mode rejects `0000-00-00` date values and non-strict inserts
that are all over the legacy schema. Put this in
`/etc/mysql/mysql.conf.d/mysqld.cnf` under the `[mysqld]` section:

```ini
sql_mode = "NO_ENGINE_SUBSTITUTION"
```

Then restart:

```sh
sudo systemctl restart mysql
```

Verify:

```sh
mysql -uroot -p -e "SELECT @@sql_mode;"
# expect: NO_ENGINE_SUBSTITUTION
```

If sql_mode still contains `STRICT_TRANS_TABLES` or `NO_ZERO_DATE` or
`NO_ZERO_IN_DATE`, the schema load in 2.3 will throw ~20-50 errors.

### 2.3 Load the init scripts (order matters)

From `~/work/dkrix-upstream/dkrixserver/initdb/`:

```sh
cd ~/work/dkrix-upstream/dkrixserver/initdb

# a-setup.sql creates DBs and the 'elcastle' user (password elca110).
# Run this as root.
mysql -uroot -p < a-setup.sql

# Load the DARKEDEN and USERINFO schema dumps as the elcastle user.
mysql -uelcastle -pelca110 DARKEDEN  < DARKEDEN.sql
mysql -uelcastle -pelca110 USERINFO  < USERINFO.sql
```

You WILL see warnings ("Using a password on the command line interface can
be insecure", charset warnings). Harmless. What you should NOT see is
any line containing `ERROR ` — those are real.

### 2.4 Patch WorldDBInfo hostname

`DARKEDEN.sql` seeds `WorldDBInfo` with hostname `odk-mysql` (the docker
service name from the Docker deploy). For a single-host local test, the
three server binaries need to resolve the MySQL host as `127.0.0.1`:

```sh
mysql -uelcastle -pelca110 DARKEDEN -e "
  UPDATE WorldDBInfo SET HostName='127.0.0.1' WHERE HostName='odk-mysql';
  SELECT * FROM WorldDBInfo;
"
```

Both rows (WorldID 0 and 1) should now show `127.0.0.1` for HostName.

### 2.5 Verify GameServerInfo is ready

```sh
mysql -uelcastle -pelca110 DARKEDEN -e "SELECT * FROM GameServerInfo;"
```

Expected: one row, ID=1, name `game1`, IP `127.0.0.1`, port 9998. If the
IP column is anything else, patch it with the same `UPDATE` pattern.

---

## 3. Build the server

```sh
cd ~/work/dkrix-upstream/dkrixserver
make debug 2>&1 | tee ~/smoke-test-log.txt
```

Expected end state: `bin/loginserver`, `bin/sharedserver`,
`bin/gameserver` exist (top-level `dkrixserver/bin/`, NOT `build/bin/` —
`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is set to `${CMAKE_SOURCE_DIR}/bin` so
binaries land next to `build/`, `conf/`, `data/`).

```sh
ls -la bin/
```

If the build fails before producing binaries:

- First 10 errors are more informative than the last 10 (compilers cascade).
  Paste first 10.
- `make debug` is incremental — re-running after a fix is cheap.
- If a single `.cpp` fails and the rest compiles, `cmake --build build
  --target loginserver` (or `sharedserver`, `gameserver`) isolates which
  binary needs that file.

---

## 4. Build the client

```sh
cd ~/work/dkrix-upstream/dkrix
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug 2>&1 | tee ~/smoke-test-client-cmake.log
make -j$(nproc) 2>&1 | tee -a ~/smoke-test-log.txt
```

Expected: `build/DarkEden` executable exists.

```sh
ls -la DarkEden
```

Known minor issues to watch for:

- **SDL2 version mismatch**: the client needs SDL2 >= 2.0.20 for `SDL_GetHintWithPriority`
  and a couple of recent constants. Ubuntu 22.04 ships 2.0.20, so this
  should pass.
- **JPEG `find_package` failure**: the `find_package(JPEG QUIET)` call
  emits nothing on failure but later linking will complain about
  `jpeg_*` symbols. If that happens, force detection with
  `cmake .. -DJPEG_LIBRARY=/usr/lib/x86_64-linux-gnu/libjpeg.so`.
- **Korean glyphs**: TextSystem wants `NanumGothic.ttf` or similar in
  `Client/Data/Fonts/`. If missing, you'll see a runtime "font not found"
  warning, but the client still starts and you can read Latin text.
  That's fine for smoke test.

---

## 5. Server config: env vars + activate the templates

Phase 8B moved secrets to env-var expansion. Copy the templates over the
active conf files once (they have `${DKRIX_*}` placeholders), then set
the env vars in your launch shell.

```sh
cd ~/work/dkrix-upstream/dkrixserver/conf
cp loginserver.conf.template  loginserver.conf
cp sharedserver.conf.template sharedserver.conf
# gameserver.conf does NOT have a template in this tree; edit it directly.
```

**Patch hardcoded IPs.** All three conf files have `192.168.0.16` as
`DB_HOST`, `LoginServerIP`, `SharedServerIP`, `BillingServerIP`,
`LogServerIP`. Billing + LogServer can be left pointing at anything
(those subsystems aren't running and timeouts won't fail startup). DB
and cross-server IPs MUST be 127.0.0.1 for a single-host test:

```sh
cd ~/work/dkrix-upstream/dkrixserver/conf
for f in loginserver.conf sharedserver.conf gameserver.conf; do
    sed -i 's/192\.168\.0\.16/127.0.0.1/g' "$f"
done
```

(After this, `LoginServerIP`, `SharedServerIP`, `DB_HOST`, `UI_DB_HOST`,
`DIST_DB_HOST` all point at 127.0.0.1 — the billing/log server IPs also
flip but that's harmless.)

**Set the env vars** (put these in a `~/.dkrix-env` file you can `source`
before each launch, so you don't commit them):

```sh
# ~/.dkrix-env
export DKRIX_DB_HOST=127.0.0.1
export DKRIX_DB_PORT=3306
export DKRIX_DB_NAME=DARKEDEN
export DKRIX_DB_USER=elcastle
export DKRIX_DB_PASSWORD=elca110
export DKRIX_UI_DB_HOST=127.0.0.1
export DKRIX_UI_DB_NAME=USERINFO
export DKRIX_UI_DB_USER=elcastle
export DKRIX_UI_DB_PASSWORD=elca110
export DKRIX_DIST_DB_HOST=127.0.0.1
export DKRIX_DIST_DB_NAME=DARKEDEN
export DKRIX_DIST_DB_USER=elcastle
export DKRIX_DIST_DB_PASSWORD=elca110
export DKRIX_BILLING_SERVER_IP=127.0.0.1
```

---

## 6. Launch the servers in order

Order: **loginserver → sharedserver → gameserver**. Each reads its conf at
startup and binds a TCP port; launch must complete before the next one
starts. Use three tmux panes so you can see the logs side by side.

```sh
tmux new-session -d -s dkrix
tmux split-window -h -t dkrix
tmux split-window -v -t dkrix:0.1
# 3 panes: left (login), right-top (shared), right-bottom (game).
```

In pane 0 (login):

```sh
cd ~/work/dkrix-upstream/dkrixserver/bin
source ~/.dkrix-env
./loginserver ../conf/loginserver.conf
```

Expected readiness log: a line like "loginserver listening on port 9999".
Leave it running.

In pane 1 (shared):

```sh
cd ~/work/dkrix-upstream/dkrixserver/bin
source ~/.dkrix-env
./sharedserver ../conf/sharedserver.conf
```

Expected readiness log: "sharedserver listening on port 9977".

In pane 2 (game):

```sh
cd ~/work/dkrix-upstream/dkrixserver/bin
source ~/.dkrix-env
./gameserver ../conf/gameserver.conf
```

Expected readiness log: a GC/CG packet handler registration block then
"gameserver listening on port 9998".

**Likely failure modes here (mostly DB-connection):**

- "Access denied for user 'elcastle'@'localhost'" — the mysql user was
  created with `'elcastle'@'%'` but MySQL 8's auth plugin
  `caching_sha2_password` + localhost-bound client can wedge. Fix:
  `mysql -uroot -p -e "ALTER USER 'elcastle'@'%' IDENTIFIED WITH mysql_native_password BY 'elca110'; FLUSH PRIVILEGES;"`
- "Can't connect to MySQL server on '127.0.0.1'" — check
  `sudo systemctl status mysql`.
- "Unknown database 'DARKEDEN'" — step 2.3 didn't run cleanly; paste
  the load logs.
- Segfault on startup with no log — run under gdb: `gdb --args ./loginserver ../conf/loginserver.conf` then `run`, capture
  backtrace with `bt`.
- "can't open `something.txt`" — the server has a huge amount of
  data-file loading (Item.txt, Monster.txt, Skill.txt, etc. under
  `dkrixserver/data/`). The binary expects to be run from a working
  directory where it can see `../data/` or similar. If this happens,
  try running from `dkrixserver/bin/` instead of `build/bin/` and see
  if the legacy layout is assumed.

---

## 7. Connect the client

```sh
cd ~/work/dkrix-upstream/dkrix/build
./DarkEden
```

The client should:

1. Open an SDL2 window with the login screen.
2. Let you type a username + password.
3. Attempt a connection to `127.0.0.1:9999` (loginserver) on submit.

At the login screen: account `test` / password `test` is the legacy test
credential but the row won't exist yet. Instead, create an account first:

```sh
mysql -uelcastle -pelca110 USERINFO -e "
  INSERT INTO UserInfo (userID, password, status, regDate)
  VALUES ('smoketest', MD5('smoketest'), 0, NOW());
"
```

(Exact columns may differ — run `DESCRIBE UserInfo;` first and adapt.)

Then try `smoketest` / `smoketest` from the client.

**Likely failure modes on the client side:**

- Window never opens → SDL2 can't init a display. Check you're on a
  local session (not SSH without X forwarding). If on WSL2, run under
  WSLg or with a local X server and `DISPLAY=:0`.
- Window opens, login blocks forever → client packet handler rejects
  the loginserver's response. In the loginserver pane, you should see
  a connection accepted + response packet logged when you hit submit.
  If you see the accept but the client hangs, it's a wire-format
  divergence (exactly what Phase 17 expects — remember, no packet
  classes have been unified yet; client and server each still have
  their own potentially-divergent copies).
- "Login failed" → usually correct behavior for bad credentials.
  Verify the `UserInfo` row you inserted has matching
  password hashing (some legacy paths expect plain text, some MD5).

---

## 8. What "success" looks like

- Three server panes running with no ERROR-level log.
- Client window open, login screen visible.
- On login submit: loginserver logs "client connected from
  127.0.0.1:..." and the client transitions to character-select or
  world-enter screen.

If you get all three, we have a working tree. If you get the first two
but not the third, that's the wire-format divergence Phase 17 flagged —
and the next phase of modernization (per-PR packet migrations) starts
being worth doing because you now have a live test setup to validate
each one against.

---

## Sidebar — macOS

Swap step 1 for:

```sh
brew install cmake xerces-c mysql-client lua@5.1 \
             sdl2 sdl2_image sdl2_ttf jpeg freetype \
             mysql psmisc tmux
brew link mysql-client --force
```

Step 2: `brew services start mysql` instead of systemctl. `my.cnf`
location is `/opt/homebrew/etc/my.cnf` (Apple Silicon) or
`/usr/local/etc/my.cnf` (Intel).

Steps 3-4: expect CMake to complain about `pkg-config` paths for xerces.
Fix: `export PKG_CONFIG_PATH="$(brew --prefix xerces-c)/lib/pkgconfig:$PKG_CONFIG_PATH"`
before `cmake ..`.

Step 7: SDL2 on macOS needs `DarkEden` to be launched via a main
thread loop Apple requires. If the window doesn't appear, the fix is
usually already in `Client/platform/SDLInit.cpp` but older macOS SDL2
versions sometimes need `SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0")`
added — flag if hit.

---

## Paste-back template

When something fails, send me this block:

```
Step: X.Y
Command: <what you ran>
First 30 lines of error:
<paste>
Last 30 lines of error:
<paste>
```

That's usually enough for me to diagnose. I'll iterate with you until
each step passes.
