# Runtime smoke test — results and findings

**Run date:** 2026-04-20
**Runner:** Enrico (WSL2 Ubuntu 22.04 on WINDOWS-8I5Q22Q)
**Repo root:** `/mnt/c/newdk/` (DrvFs NTFS mount — *not* native ext4)
**Goal (from `SMOKE_TEST.md`):** get one client to the login screen,
talking to a server cluster you just built, against a fresh MySQL.
Write down every failure as you go.

## TL;DR

The three-daemon server cluster builds, runs, binds its ports, exchanges
inter-server handshakes, and accepts client TCP. The client binary
(`DarkEden`) builds. We proved console-level reachability end-to-end;
we did not drive a real login packet through the client because that
requires a display and/or a custom packet-encoding probe neither of which
is smoke-test scope.

Four source bugs were found and fixed during the run (Bugs Q, R, S, T
below). Two non-bug configuration gaps were closed (`odk-mysql`
hostname, CRLF-on-DrvFs confs). One compatibility gap was discovered
and *not* fixed: retail-format `.smp` map files don't parse against the
port's zone parser. It is the single finding most worth acting on.

## Environment deltas from the runbook

The actual run diverged from `SMOKE_TEST.md` in three places. None
affected the outcome; noting them for the record.

- **Database engine:** runbook assumed MySQL 8; run used MariaDB 10.6.
  Wire-compatible, schema loaded cleanly. No `sql_mode` tweaks needed —
  MariaDB 10.6 is permissive enough for the MySQL-5.7-era dumps.
- **Config strategy:** runbook assumed `.conf.template` files with Phase
  8B env-var substitution (`${DKRIX_DB_*}`). That template layer was
  not used — we edited `conf/*.conf` directly via `sed` to swap IPs and
  `HomePath`. Credentials ended up in the confs in plaintext. For a
  smoke test that's fine; for anything past this, re-apply the
  templates.
- **Repo path:** runbook assumed `~/work/dkrix-upstream/`; actual tree
  lives at `/mnt/c/newdk/` on DrvFs. This mattered: DrvFs is
  case-insensitive (→ Bug Q) and carries CRLF line endings through to
  Linux tools (→ Bug T). A future run on native ext4 would avoid both
  issues without patching anything.

## What worked

Concrete evidence for each layer, top to bottom:

**MySQL bootstrap (`STEP1_MYSQL.md`):** Both databases created and loaded.
Verified row counts: DARKEDEN 374 tables, USERINFO 7 tables. User
`elcastle` granted from `%`, `localhost`, and `127.0.0.1` (MariaDB
treats them as distinct hosts).

**Server build (`STEP2_SERVER.md`):** `make debug` produced all three
binaries in `dkrixserver/bin/` after Bugs Q and R were applied.
libmysqlclient, liblua5.1, libxerces-c all discovered via pkg-config
without manual path overrides.

**Client build:** `dkrix/build/bin/DarkEden` exists (plus six console
viewer/parser tools: `zone_parser`, `resource_validator`, `map_viewer`,
`sprite_viewer`, `creature_viewer`, `effect_viewer`, `item_viewer`).

**Server launch:** All three daemons in foreground, all three ports
bound, inter-server handshake flowing. Captured in
`STEP2_GREEN_SNAPSHOT.md`. The money-shot `ss -ltnp`:

```
LISTEN 0 5  0.0.0.0:9977  sharedserver (pid 81989)
LISTEN 0 5  0.0.0.0:9998  gameserver   (pid 82254)
LISTEN 0 5  0.0.0.0:9999  loginserver  (pid 82197)
```

Notable log landmarks seen: `---------- Start SharedServer ----------`,
`Receive:GSRequestGuildInfo` / `Send:473[2,0] SGGuildInfo()` (inter-server
handshake), `>>> ALL INITIALIZATIONS ARE COMPLETED SUCCESSFULLY.`,
`>>> STARTING ClientManager->start() INFINITE LOOP...`.

**Client TCP reachability:** `nc 127.0.0.1 9999` succeeded; loginserver
printed `NEW CONNECTION FROM 127.0.0.1:<port>` and correctly raised
`ConnectException : connect closed.` down through
`Player::processInput() → SocketInputStream::fill() → SocketImpl::receive()`
when we closed the nc without speaking. Protocol is client-initiates
(no greeting banner).

**Port-shipped asset readability:** `zone_parser adam_c.smp` parsed a
port-shipped `.smp` cleanly (ZoneID 72, 254×2 sectors, format header
`=MAP_2000_05_10=`). The port's parser works.

## Bugs found and fixed during the run

All four live in the repo now. For the first two, `server_build_fix.sh`
carries them idempotently so a clean re-clone can replay them; the
other two are source edits that should be committed.

### Bug Q — shadowing `src/Core/Endian.h`

**Symptom:** build fails with `htole32 was not declared in this scope`
in glibc's `<endian.h>` (really, in our `Endian.h`).

**Cause:** `src/Core/Endian.h` shadows glibc `<endian.h>` on
case-insensitive filesystems (DrvFs). Transitive chain:
*any glibc header* → `<sys/types.h>` → `<endian.h>` resolves to OUR file
first, because `src/Core/` is on the `-I` path and `-I` beats system
dirs. Our file then does `#include <endian.h>` on line 65, which
re-resolves to itself, the header guard bails, and glibc's `htole*` /
`*toh` macros never get declared.

**Fix:** delete the file. Per its own docstring it was an unused
Phase-13B placeholder deferred until Phase 13.3. When 13.3
reintroduces it, use a non-shadowing name (`LEndian.h`, `ByteOrder.h`).

### Bug R — `MYSQL_BIND::is_null` typing (MySQL 5.7 vs 8.0)

**Symptom:** build fails in `PreparedStatement.cpp:204` with
`cannot convert 'char*' to 'bool*' in assignment`.

**Cause:** MySQL 5.7 had `typedef char my_bool`, so
`MYSQL_BIND::is_null` was `my_bool*` (= `char*`). MySQL 8.0 removed
`my_bool` and `is_null` became `bool*`. The `.cpp` hardcoded
`reinterpret_cast<char*>(&p.isNull)` which worked on 5.7 only. Phase 11B
had a header-side `typedef bool my_bool_t` that acknowledged the
problem but the .cpp didn't use it.

**Fix:** replace the typedef with
`typedef std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type my_bool_t;`
(derives from the installed library's own declaration — works on both
MySQL 5.7 / MariaDB *and* MySQL 8.0+, no `#ifdef` version sniffing) and
drop the `reinterpret_cast` at the call site.

### Bug S — `catch (Error&)` in all three server `main.cpp`s

**Symptom:** sharedserver crashed with
`terminate called after throwing an instance of 'IOException'` and
`what(): std::exception`. No useful error text.

**Cause:** `main.cpp` catches only `Error&`. `IOException` lives in
the `Exception` subtree of `Throwable`, parallel to `Error` — so
`IOException` (thrown by `Properties::load`) escaped the catch
entirely and hit `terminate()`. `Throwable::what()` isn't overridden so
it fell through to `std::exception::what()`, printing the literal
string `"std::exception"`.

The other two `try` blocks in each `main.cpp` catch `Throwable&`
correctly — only the first one was narrowed. Gameserver's version was
worse: its catches had the `cout` commented out so config failures
were silently swallowed, leaving `g_pConfig` half-initialised for
subsequent `getProperty` calls to fail obscurely.

**Fix:** widened all five affected catches (one in
`sharedserver/main.cpp`, two each in `loginserver/main.cpp` and
`gameserver/main.cpp`) to `catch (Throwable&)`, and uncommented
gameserver's prints. Config-load catches now call `exit(1)` so failure
can't dereference a broken `g_pConfig`.

### Bug T — CRLF line endings break `Properties::load`

**Symptom:** once Bug S surfaced the real error, it read
`IOException : missing separator`. The config file was visually clean —
every non-comment line had a colon.

**Cause:** confs on `/mnt/c/...` DrvFs have CRLF line endings. `getline()`
strips `\n` but leaves `\r`. `Properties::WhiteSpaces` is `" \t"` only,
so a blank separator line in a CRLF file (just `\r`) isn't empty, isn't
comment-prefixed, and has no `:` — the separator check throws.

**Fix:** one-line change in `Properties::load` — strip trailing `\r`
immediately after `getline()`, before the empty/comment check. This
also silently fixes a latent would-have-been-next-issue: values like
`DB_PASSWORD : elca110` were parsing to `"elca110\r"`, which would have
failed MySQL auth and looked exactly like a wrong-password bug.

## Non-bug configuration gaps closed

### `odk-mysql` hostname in seeded `WorldDBInfo`

The shipped `DARKEDEN.sql` seeds the `WorldDBInfo` cross-world DB table
with hostname `odk-mysql` (from `docker/docker-compose.yml` service
name). Works inside the Docker bridge network, doesn't resolve anywhere
else. Sharedserver's DatabaseManager reads this table and uses whatever
hostname it sees to open a second libmysqlclient connection.

Fixed with `/etc/hosts`:

```
127.0.0.1  odk-mysql
```

Chose this over `UPDATE WorldDBInfo SET HostName='127.0.0.1'...` so
that the seeded data stays byte-identical to the shipped dump.
Reversible, one line.

### CRLF on DrvFs

Covered under Bug T above. The fix is a source change, not a conf
rewrite — because the files themselves are fine on Windows, and future
maintainers editing them from Windows will re-introduce CRLFs
indefinitely. Patching the reader is less brittle than `dos2unix` as a
repeated step.

## Findings NOT fixed (for the record)

### Retail `.smp` format != port's `.smp` parser

`zone_parser ~/darkeden_data/Data/Map/cave_office.smp` →
`Failed to parse zone file`. Same tool against
`/mnt/c/newdk/dkrixserver/data/adam_c.smp` parses cleanly and prints a
full header including `Version: =MAP_2000_05_10=`. Both files are
non-zero, well-formed by sampling, and in the same location the
respective consumers expect.

So: the port has diverged from the retail DarkEden `.smp` format.
Plausible reasons (in order of likelihood, not yet confirmed):

1. Port adopted a newer `.smp` version (`MAP_2000_05_10` may be
   newer than the retail pack's shipped version string) and rejects
   older payloads rather than falling back.
2. Port's parser requires fields that retail files don't populate.
3. Same version string but slightly different record layout (tile
   encoding, object table, etc.) that retail wrote and port no longer
   accepts.

Verify by reading the first 16 bytes of each file and comparing version
strings:

```bash
hexdump -C -n 64 ~/darkeden_data/Data/Map/cave_office.smp | head -4
hexdump -C -n 64 /mnt/c/newdk/dkrixserver/data/adam_c.smp | head -4
```

If the version strings differ, case (1) is confirmed and the fix is
either (a) teach zone_parser / the server to fall back to the older
version or (b) transcode retail maps forward.

**Impact on the bigger picture:** for a LAN smoke-test with only the
port-shipped maps, zero impact — that's what the current green run
uses. For any workflow that wants retail assets to be drop-in (common
modding/restoration pattern), this is a blocking divergence.

### Loginserver "Loading GameWorldInfoManager" idle loop

While sitting with zero connected clients, loginserver's log shows
`Loading GameWorldInfoManager... / GameWorldInfo(WorldID:1,...) / Size: 0 /
End GameWorldInfoManager Load / addGameServerGroupInfo: 1, 0 : Server1`
repeating on a short interval. No errors, no CPU burn visible, but it's
a polling loop where a cache or an edge-triggered event would do. Not
smoke-test-blocking; flagging for future cleanup.

### Client login packet never sent

`nc` proved TCP reachability and got a clean `ConnectException`
unwind, which is the protocol boundary's *first* behavior. We didn't
encode and send a real login request packet, so we have no evidence
for or against:

- Client-side handshake byte order on the wire.
- Server's first-read validation (length prefix, version byte,
  auth method negotiation).
- Whether a login against a fresh `USERINFO` account actually
  succeeds.

The next probe for this would be either (a) build a small C++ or
Python client that sends a real login request (requires reading the
client's login packet class — probably under
`dkrix/Client/LoginManager.cpp` or similar), or (b) run `DarkEden` with
a display and drive it manually. Either is post-smoke-test work.

## Recommended next steps (priority ordered)

1. **Commit the source fixes.** Bugs Q, R, S, T are in the tree but not
   in any commit. Bundle them as a single "Phase 13C: runtime-smoke-test
   fixes" commit so they're reviewable and revertable as a unit.
   `server_build_fix.sh` can stay as an applicator but the real changes
   should be in the tree.
2. **Investigate the `.smp` format divergence.** One afternoon of
   forensic comparison would answer whether this is a version-gate
   problem or a real format change. Either way, surfacing the
   divergence with a warning (rather than `Failed to parse zone file`
   with no detail) would help every future contributor who tries
   retail assets.
3. **Re-apply the Phase 8B env-var templates.** The smoke test ended
   with DB passwords sitting in `conf/*.conf`. Copy the `.template`
   versions back over and put credentials in `~/.dkrix-env`.
4. **Apply the same `catch (Error&) → catch (Throwable&)` fix to the
   two submains** that still have the old form:
   `gameserver/billing/main.cpp:41` and `gameserver/mofus/main.cpp:32`.
   These weren't in the critical path for this smoke test but they'll
   bite someone eventually.
5. **If you plan to run this on native ext4,** Bug Q (Endian.h shadow)
   and Bug T (CRLF) both stop being concerns. Worth moving the tree
   off DrvFs next time — also a ~3-5× build-time improvement for
   many-small-files compiles.
6. **For a headless client probe,** the lightest lift is writing a
   ~80-line C++ that `connect()`s to `127.0.0.1:9999`, sends the
   client's first login packet, and prints the server's response. That
   gives you a regression test for wire-format changes without needing
   a display.

## Companion artifacts

- `STEP1_MYSQL.md` — MySQL/MariaDB install + schema load runbook.
- `STEP2_SERVER.md` — server build + config + launch runbook.
- `STEP2_GREEN_SNAPSHOT.md` — captured "this is what a clean green
  launch looks like" for future regression diffs.
- `server_build_fix.sh` — applies Bugs Q and R idempotently.
