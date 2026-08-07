# Porting note — read before running any of this

**Lifted 2026-08-06 from tag `archive/modernization-phases-1-17`**
(`docs/archive/smoke-test/`), the parked modernization line.

That line got a client and server to **end-to-end login → gameplay** on
2026-04-20 (`dbc3087`). This runbook is the procedure that got it there. It is
the only repeatable client+server validation path this project has ever had,
and `main` did not have it.

## Status: inherited, not yet verified on this line

**Nothing here has been run against `main`.** It was written against a tree
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
| `server_build_fix.sh` (10 KB) | **27 references to parked-line-only code** — `PreparedStatement.{h,cpp}` (Phase 11) does not exist on `main`. Running it here would fail or do damage. If you need a server build fix, write a fresh one. |
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
| STEP1_MYSQL | not yet | |
| STEP2_SERVER | not yet | |
| STEP2_GREEN_SNAPSHOT | not yet | |
| STEP3_CLIENT | not yet | |
| LOGIN_SMOKE | not yet | |
