# Porting note — read before running any of this

**Lifted 2026-08-06 from tag `archive/modernization-phases-1-17`**
(`docs/archive/smoke-test/`), the parked modernization line.

That line got a client and server to **end-to-end login → gameplay** on
2026-04-20 (`dbc3087`). This runbook is the procedure that got it there. It is
the only repeatable client+server validation path this project has ever had,
and `main` did not have it.

## Status: run 1 in progress (2026-08-10)

**First run against `main` started 2026-08-10** — STEP1, STEP2, and STEP3 are
through (with the drift recorded at the bottom of this file). LOGIN_SMOKE is
**blocked on Bug 18-B**: the client reaches the loginserver and both packets
parse, then the loginserver segfaults. Two runtime defects found in the
sitting: Bug 18-A (fixed) and Bug 18-B (open) — MODERNIZATION.md Phase 18. Historical framing below is kept as written; the
per-step table at the end is the current truth. It was written against a tree
that had Phases 1–17 applied. When lifted (2026-08-06), `main` had none of
them; as of 2026-08-07 `main` carries Phases 1, 2 (partial), 3.1, 4 (safe
items), 7, and 9 (mechanical), so the build-step divergence risk below has
shrunk — but it is not gone, and nothing is verified. Treat every step as a
hypothesis until you run it.

The runbook is far more portable than the code was — the environment setup
(MySQL, schemas, accounts, retail asset extraction, server launch ordering) has
nothing to do with the refactors. But expect the **build steps to diverge**,
because that is exactly where the two lines differ.

## What was changed during the port

| Change | Why |
|---|---|
| Paths `/mnt/c/newdk` → derived at runtime | The tree moved to `C:\dev\newdk`, and `f19c4d3` made the tree location-independent. Hardcoding the new path would repeat the original mistake. |
| `start_servers.sh`, `client_smoke.sh`, `extract_rpks.sh` now compute `REPO_ROOT` from their own location | Same reason. All honour a `REPO_ROOT=` override. |
| All scripts syntax-checked (`bash -n`) | Cheap sanity gate; they parse. That is *not* the same as working. |

## What was deliberately left behind

| File | Why |
|---|---|
| `server_build_fix.sh` (10 KB) | **27 references to parked-line-only code.** ~~`PreparedStatement.{h,cpp}` (Phase 11) does not exist on `main`.~~ **Stale as of 2026-08-10** — Phase 11 has since been lifted; `src/server/database/PreparedStatement.{h,cpp}` exists on `main` (2026-08-07/09) and `CLLoginHandler.cpp` uses it. The script is still not worth running (it was written against the parked line's other assumptions); if you need a server build fix, write a fresh one. |
| `client_build_fix.sh` (40 KB) | Built against `Client/Platform/`, which is the parked line's replacement for `Client/DXLib/`. `main` still has `DXLib/`. Wholly inapplicable. |
| `SMOKE_TEST_RESULTS.md` (58 KB) | A historical result log for that tree, not a procedure. Still readable on the tag if you want the failure catalogue — Bugs Q through XX are documented there and several may recur. |

## Known mismatches to expect

- **`SMOKE_TEST.md` names a branch and layout that no longer apply.** It says
  *"Branch to test: `modernize/phase4-sprite` at tip `7ee8618`"* and assumes the
  repo sits at `~/work/dkrix-upstream/`. Read those as historical. You are
  testing `main` at whatever it is now.
- ~~Port numbers are inconsistent with `../../CLAUDE.md`.~~ **Resolved
  2026-08-06 — the scripts were right.** `conf/sharedserver.conf:18` sets
  `TCPPort : 9977`, and every `SharedServerPort` entry in `conf/` agrees.
  `../../CLAUDE.md` had conflated the gameserver's UDP 9997 with the
  sharedserver's TCP port; it has been corrected with the full port table.
  Nothing to change in these scripts.
- **The build steps assume Ubuntu 22.04** and the parked line's CMake layout.
  `main`'s client build is the Windows/VS2022/vcpkg path per `../../CLAUDE.md`;
  the Linux client build here was a smoke-test convenience, not the shipping
  path.

## Suggested first use

Do not attempt the whole thing. Run it in the order the steps are numbered and
stop at the first divergence:

1. `STEP1_MYSQL.md` — schemas and the test account. Almost certainly portable;
   it touches the database, not the code.
2. `STEP2_SERVER.md` + `start_servers.sh` — the server build is where `main`
   and the parked line first diverge. Expect this to need work.
3. `STEP3_CLIENT.md` + `extract_rpks.sh` + `client_smoke.sh` — asset extraction
   should be portable; the client build will not be.

`LOGIN_SMOKE.md` is the narrower "just get to the login screen" variant and is
the cheaper target if the full run stalls.

As each step is confirmed on `main`, mark it here. When a step needed changes,
say what and why — the next person should be able to tell verified from
inherited at a glance.

| Step | Verified on `main`? | Notes |
|---|---|---|
| STEP1_MYSQL | **verified 2026-08-10, with drift** | See "Run 1 drift" below — the database is not where the runbook assumes, and `WorldDBInfo`'s live schema differs from the initdb seed. |
| STEP2_SERVER | **verified 2026-08-10, with drift** | Build needed a fresh build dir and an ownership fix; conf sweep applied as written otherwise. |
| STEP2_GREEN_SNAPSHOT | **verified 2026-08-10** | All of GREEN_SNAPSHOT's landmarks hit on the second launch (first launch died in `KeyInfoManager::load()` → Bug 18-A, fixed same sitting): `Start SharedServer`; loginserver polling + listening 9999; gameserver `>>> ALL INITIALIZATIONS ARE COMPLETED SUCCESSFULLY.` + `ClientManager->start() INFINITE LOOP`; `connection to sharedserver established` and the `GSRequestGuildInfo` → `SGGuildInfo()` round trip in both logs. F10's Billing/Log fast-refuse did **not** kill loginserver — no `111.111.111.111` workaround needed. |
| STEP3_CLIENT | **verified 2026-08-10, with drift** | Login screen renders from the fresh VS2022 Debug build — art, SPK sprites, and text all load. Three divergences from the doc, all recorded under "STEP3 drift" below. §2/§3 (WSLg, Linux runtime libs) and §4b (`.rpk` extraction) do not apply on the Windows route. |
| LOGIN_SMOKE | **blocked 2026-08-10 — Bug 18-B** | Client↔loginserver wire format works (`CGConnectSetKey` then `CLLogin` both received and parsed). The loginserver then segfaults, deterministically, for valid and invalid credentials alike — **use-after-free of the `Result` owned by a stack-local `PreparedStatement`** in `CLLoginHandler.cpp`, crashing in `Result::getField()`. Client shows "Disconnected". See MODERNIZATION.md Phase 18, Bug 18-B. |

## Run 1 drift (2026-08-10, `main` @ `6d6059e`)

Environment and data findings. Per the house convention these are *not*
Phase 18 bug letters — only runtime defects in the code get those.

**The database is a Docker container, not a WSL service.** WSL has no
`mysqld` at all (`service mysql start` → unit not found); the live data
lives in `docker-odk-mysql-1` (compose project `docker`, from
`dkrixserver/docker/docker-compose.yml`), up for 9 days. That compose
service **publishes no ports** — only the sibling `odk-server` container
ever reached it, by service name on the internal network — so nothing on
the host or in WSL can connect until a proxy exists:

```bash
docker run -d --name odk-mysql-proxy --network docker_odk-network -p 3306:3306 \
  alpine/socat tcp-listen:3306,fork,reuseaddr tcp-connect:odk-mysql:3306
```

After that, `mysql -h127.0.0.1 -uelcastle` works from WSL. Two more
container facts: it has **no named volume**, so the live database exists
only inside that container — `docker compose down`/`rm` destroys it, use
`stop`/`start` only; and its `sql_mode`
(`ONLY_FULL_GROUP_BY,NO_ZERO_IN_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION`)
already satisfies the repo rule, so PREFLIGHT §4's `SET GLOBAL` is a no-op
here.

**`mysqldump` needs `--no-tablespaces`** — `elcastle` has database-level
grants only, and MySQL 5.7.31+ demands the global `PROCESS` privilege for
the tablespace probe. Full dumps of both databases are in
`_server_logs_tmp/smoke18/` (2026-08-10 10:0x).

**`WorldDBInfo`'s live schema is not the initdb seed's.** The runbook's
`UPDATE WorldDBInfo SET HostName=…` fails with `Unknown column 'HostName'`.
The live columns are the ones the server actually reads [measured,
`DatabaseManager.cpp:109` — `SELECT WorldID, Host, DB, User, Password, Port
FROM WorldDBInfo`]. Note the consequence: **the gameserver takes its
world-DB credentials from this table, not from `conf/`** — a rotation that
misses this row breaks the server even with perfect conf files.

### Values changed in the live database — RESTORE BEFORE THE NEXT TESTER RELEASE

| Table.column | Was (restore this) | Set to (smoke-test only) |
|---|---|---|
| `WorldDBInfo.Host` (both rows) | `odk-mysql` | `127.0.0.1` |
| `WorldDBInfo.Password` (both rows) | `elca110` | the rotated password |
| `GameServerInfo.IP` | `90.190.31.134` | `127.0.0.1` |

`GameServerInfo.IP` is the address the login flow hands to clients as
"where the gameserver is" — leaving it at `127.0.0.1` would send every
tester to their own machine. Both tables are inside the full dumps above.

**Credential note:** `WorldDBInfo.Password` still held `elca110` on
2026-08-10 — the value published in the public repo — i.e. the 2026-08-09
rotation had not reached the container's `elcastle` user or this row.
Both were rotated during run 1. `docker-compose.yml`'s
`MYSQL_ROOT_PASSWORD: 123456` is likewise public and still current unless
separately rotated.

**Server build drift.** `dkrixserver/build/` and `build-wsl/` carry
`CMakeCache.txt` files generated on other machines/paths
(`/home/darkeden/vs`, `/mnt/c/newdk`) and cannot be reused — cmake refuses.
Build in a fresh directory (`build-smoke/` was used; binaries still land in
`dkrixserver/bin/` regardless). Both stale trees plus `lib/` and `bin/`
were **root-owned from an old `sudo` build**, which fails at the archive
step with `ar: could not create temporary file … Permission denied`. With
no known WSL `sudo` password, the fix is from Windows: delete `lib/` and
`bin/` in PowerShell (Windows ignores Linux ownership), let the build
recreate them. Back up `bin/` first if the previous release's binaries
matter.

### STEP3 drift (2026-08-10, run 1)

STEP3_CLIENT is written for the parked line's Linux/WSLg client. On `main` the
client is the Windows/VS2022 build, so **§2 (WSLg sanity check), §3 (SDL2
runtime libs + CJK fonts), and §4a (ext4 case sensitivity) do not apply at
all.** What did apply, and what it cost:

**§4b (`.rpk` extraction) is a no-op here — do not install `unrar`.** None of
the 8 `.rpk` archives has an extracted sibling directory anywhere in the tree,
yet `RegenTowerInfoManager::LoadRegenTowerInfo()` succeeds. `CRarFile::Open`
(`dkrix/VS_UI/RarFile.cpp`) falls back to the *package directory* when the
mapped `<name>/` directory misses, and `Data\Info\RegenTowerPosition.inf` sits
loose next to `infodata.rpk`. The doc's claim that every `.rpk` "needs to be
pre-extracted or the client aborts with `Cannot Open RTI File`" is true of the
parked line's `RarFile.cpp`, not this one.

**The `Futec(...)` argument must not be wrapped as a single quoted token.**
`Client.cpp:2803` compares the first 8 characters of `lpCmdLine` against
`"00000000"`, and `lpCmdLine` in `WinMain` includes any quotes the caller
wrote. Passing `"00000000 Futec(127.0.0.1:9999)"` as one argument makes the
compared text `"0000000` (leading quote, last char lost), the comparison fails,
and `Client.cpp:2807` returns out of `WinMain` — clean exit 0, no window, no
log, no error. Correct form is two tokens: `00000000 "Futec(127.0.0.1:9999)"`.

> **`Darkeden/RUN_LOCAL_CLIENT.cmd` has this bug** — it passes
> `"00000000 Futec(127.0.0.1:9999)"` as a single quoted argument, so it has
> never actually pointed the client at the local server. Not fixed here; fixing
> it is a one-line change to that `.cmd` and should land on its own.

**The client `chdir`s to its own exe directory and ignores the launch cwd.**
`Client.cpp:2423-2433` does `GetModuleFileName(NULL, g_CWD, …)`, truncates at
the last `\`, then `SetCurrentDirectory(g_CWD)`. So STEP3 §5's "run from the
directory that holds `Data/`" has no effect on Windows: running
`dkrix\build\bin\Debug\DarkEden.exe` with cwd `Darkeden\` still resolves assets
against `build\bin\Debug\`, finds no `Data\`, and aborts (exit 3, ~4 s, no
window, nothing on stdout/stderr).

The fix used in run 1 was a side-by-side runtime directory, so that the tester
folder is never overwritten by debug binaries:

```powershell
# C:\dev\newdk\Darkeden-debug\  — debug binaries + a junction to the real Data
New-Item -ItemType Directory C:\dev\newdk\Darkeden-debug
Copy-Item C:\dev\newdk\dkrix\build\bin\Debug\* C:\dev\newdk\Darkeden-debug -Force
cmd /c mklink /J C:\dev\newdk\Darkeden-debug\Data C:\dev\newdk\Darkeden\Data
Copy-Item C:\dev\newdk\Darkeden\UserSet C:\dev\newdk\Darkeden-debug -Recurse
Copy-Item C:\dev\newdk\Darkeden\DarkEdenResolution.cfg C:\dev\newdk\Darkeden-debug
```

Do **not** copy the debug `DarkEden.exe` into `Darkeden\` instead: Windows
paths are case-insensitive, so it would overwrite the shipped `Darkeden.exe`,
and `jpeg62.dll` / `brotli*.dll` exist under the same names in both the debug
and release sets.

**The client exits on its own when its window is not foreground.** Launched
without focus it dies in roughly 15–60 s (release build: clean exit 0; debug
build: `0xC0000005`). Focused, it runs indefinitely. This is only a problem for
scripted/automated launches — bring the window to the foreground immediately
after starting it, or the run dies mid-test and looks like a crash.

**Where the login credentials actually come from.** The ID field renders a
value persisted in `UserSet\`, but the string used to build `CLLogin` is only
updated by keystrokes delivered in the *current* session. Typing the ID in one
run, letting it persist, and submitting in the next run sends the **old** value
— run 1 sent `ID:222222` while the field displayed `testuser`. Type both
fields in the same session as the submit.

**`start_servers.sh status` is broken when invoked by path.** Line 78
re-invokes the script as `docs/smoke-test/start_servers.sh`, a path relative to
the caller's cwd rather than to `REPO_ROOT`, so `start` prints
`No such file or directory` after the servers come up. The servers themselves
start correctly; only the status echo fails.
