# Making the ASan gate real — plan and current state

**2026-08-10.** Written to close the third row of
`docs/SECURITY-AUDIT-2026-08-10.md` §4: *"`sanitizers` matrix — build-only,
so it cannot see 18-B/D/E/F/J/L, the entire class it appears to cover.
Making ASan a real gate needs a seeded MySQL service container and a
synthetic packet driver."*

Slice 1 (seeded MySQL + boot) **is implemented** and described below.
Slice 2 (packet driver) **is not**, and is specified here so it can be.

---

## 1. Why the existing job is worthless as a gate

`make debug-asan` compiles and links with `-fsanitize=address` and stops.
AddressSanitizer instruments code and reports **at runtime**. A job that
never runs the binary cannot report anything.

That is not a theoretical objection. On 2026-08-09 the `asan` leg was
flipped to blocking after five consecutive greens — while `KeyInfoManager`
was executing a malformed SQL literal that killed the gameserver on every
boot (18-A), while `Zone.cpp` was freeing `new char[]` with plain `delete`
(18-D), while `ExpTable` overflowed its heap allocation on every boot
(18-E), and while every single login segfaulted on a `Result`
use-after-free (18-B). Every one of those is exactly what ASan exists to
find. The gate was green throughout.

The five bugs that were eventually found by ASan were found by a human
running the servers under it on a workstation, not by CI.

---

## 2. What already existed to build on

Nothing here needed inventing; the pieces were in the tree.

| Piece | Where | State |
|---|---|---|
| Schema + content dumps | `dkrixserver/initdb/DARKEDEN.sql` (3.4 MB, 374 tables), `USERINFO.sql` | tracked |
| DB/user/grant bootstrap | `dkrixserver/initdb/a-setup.sql` | tracked; grants `DARKEDEN`/`USERINFO` to `elcastle`. Since 2026-08-10 it no longer creates the account — the MySQL image does that from `MYSQL_USER`/`MYSQL_PASSWORD` |
| A MySQL 5.7 + server compose file | `dkrixserver/docker/docker-compose.yml` | tracked; mounts `../initdb` as `docker-entrypoint-initdb.d` |
| Config templates with `${DKRIX_*}` placeholders | `dkrixserver/conf/*.conf.template` | tracked; `Properties::load()` expands them from the environment (`src/Core/Properties.cpp:169`) |
| Start ordering | `docker/start-servers.sh`, `docs/smoke-test/start_servers.sh` | shared → login → game |
| The manual runbook | `docs/smoke-test/STEP1_MYSQL.md` … `STEP3_CLIENT.md` | the procedure this job mechanises |
| Sanitizer build targets | `dkrixserver/Makefile` (`debug-asan`) | working |

Two things in that inventory are traps and are handled explicitly:

1. **`WorldDBInfo` ships pointing at `odk-mysql`** — the compose service
   name — in `DARKEDEN.sql`'s seed rows. Servers connect to the right
   database, read `WorldDBInfo`, and then fail to connect to the world it
   names. The seed step rewrites `Host` and `Port`.
2. **`sql_mode`.** The dumps predate `ONLY_FULL_GROUP_BY` and
   `NO_ZERO_DATE` being defaults and will not load under a strict mode.
   Set globally before loading, matching what `dkrixserver/CLAUDE.md`
   documents the server itself requires.

---

## 3. Slice 1 — boot under ASan (IMPLEMENTED)

**Job:** `boot under asan (boot-only, no packets)` in
`.github/workflows/server.yml`.
**Script:** `dkrixserver/scripts/ci-boot-smoke.sh` (`seed` | `conf` |
`boot`).
**Blocking:** no. Non-blocking on introduction, flipping to blocking after
five consecutive greens — the same rule the sanitizer matrix uses
(`MODERNIZATION.md` Phase 10 bullet 3).

### What it does

1. Brings up `mysql:5.7` as a service container with a health check.
2. `make debug-asan`.
3. Seeds: relax `sql_mode`, run `a-setup.sql`, load both dumps, add
   `localhost`/`127.0.0.1` grants (`a-setup.sql` only grants to `'%'`, and
   some MySQL builds resolve a same-host TCP connection to `localhost`),
   repoint `WorldDBInfo`/`GameServerInfo`.
4. Materialises `conf/*.conf` from the templates and rewrites `HomePath`
   to the checkout, so the gameserver's `data/*.bin`, `*.xml`, SMP and SSI
   loads resolve. Refuses to overwrite an existing conf unless
   `FORCE_CONF=1`, so it is safe to run on a workstation.
5. Starts sharedserver → loginserver → gameserver under
   `ASAN_OPTIONS=detect_leaks=0:symbolize=1:print_stacktrace=1:log_path=…`,
   waiting for each to bind its TCP port (9977 / 9999 / 9998) before
   starting the next.
6. Holds 30 s, then asserts all three are still alive and that no
   `asan.*`/`ubsan.*` report file was written. Dumps logs and reports on
   failure; uploads `dkrixserver/ci-boot-logs/` as an artifact either way.

### What it covers

Everything between `main()` and the listening socket. In this tree that is
**five of the fourteen Phase 18 bugs**:

| Bug | Fires at boot because |
|---|---|
| 18-A | `KeyInfoManager::load()` sends the malformed SQL in the `PreparedStatement` ctor |
| 18-D | `Zone.cpp` frees `new char[]` with plain `delete` during zone load |
| 18-E | `ExpTable` is written 1-based into a `MaxLevel`-sized allocation on every boot |
| 18-J | `PetAttrInfo`'s ctor `reserve()`+`clear()` writes into unconstructed storage |
| 18-L | `sharedserver`'s `GameServerPlayer*` array is used uninitialised |

18-A would additionally have been caught by the cheaper
`scripts/check-sql-literals.sh`, which does not need a database at all.

### What it does not cover — say this out loud

- **No packets.** 18-B, 18-C, 18-G, 18-H, 18-I and 18-N are all in packet
  handlers. Nothing here connects, so nothing here sees them. This is the
  single biggest remaining hole and it is what slice 2 is for.
- **No leaks.** `detect_leaks=0`. The processes are killed rather than shut
  down (LeakSanitizer only runs at a normal exit) and the tree has a large
  known leak backlog that would bury any real finding.
- **No `ubsan` leg.** `make debug-ubsan` has never been green here
  (0 for 17 — see `docs/ci-server-ubsan.md`); adding a runtime leg to a
  build that does not link is pointless.
- **No character creation.** `CLCreatePC` was never exercised in any smoke
  run either (`SECURITY-AUDIT-2026-08-10.md` §1 milestone).
- **Unproven on a runner.** The wait/assert/report logic was fault-injected
  locally on 2026-08-10 — server dies before binding, sanitizer report
  present, port already occupied, and the all-clear path — and behaves
  correctly in all four. Whether the *real* servers reach a listening
  socket on a GitHub runner has never been tested. The specific risks are
  (a) the gameserver blocking on `BillingServerIP`/`LogServerIP`, neither
  of which exists in CI, and (b) zone data loading exceeding the 420 s
  per-port timeout. This is why the job is non-blocking on introduction.

---

## 4. Slice 2 — synthetic packet driver (NOT IMPLEMENTED)

To catch the 18-B class, something has to complete a login while the
servers run under ASan. The real client cannot: it is Windows/MSVC, needs
a display and the retail asset pack, and the Linux port is ~17% done.

So: a small driver that speaks the wire protocol directly. It does not
need to render anything, and it does not need to be correct beyond the
handful of exchanges that reach the vulnerable handlers.

### The wire format it must produce

From `src/Core/Packet.h`:

```
struct on the wire:
    PacketID_t     id      ushort   (Packet.h:24-25)
    PacketSize_t   size    uint     (Packet.h:30-31)
    SequenceSize_t seq     BYTE     (Packet.h:36-37)
    <body>                 size bytes
```

`szPacketHeader = 2 + 4 + 1 = 7`. `SocketInputStream::readPacket()` skips
the header and calls `pPacket->read()`; there is no encryption layer on
the socket streams. Bodies are little-endian native writes, strings are a
length byte followed by the bytes.

`CLLogin` (id 153, `Packet.h:221`) body, from `CLLogin::read()`:

```
BYTE  idLength
char  id[idLength]
BYTE  passwordLength
char  password[passwordLength]
BYTE  mac[6]
BYTE  loginMode
```

### The sequence to reach the 18-B/18-C code paths

1. TCP connect to loginserver 9999.
2. `CL_LOGIN` (153) with an account seeded into `USERINFO` — reuse
   `docs/smoke-test/create_test_account.sql`, which already exists for
   exactly this purpose. **This alone reaches `CLLoginHandler`, which is
   where 18-B lived.**
3. `CL_GET_WORLD_LIST` (152) — where 18-N lived (a 1-based loop running to
   `Num` inclusive on a stack VLA sized `Num`, which aborted every login
   under ASan).
4. `CL_GET_SERVER_LIST` (151), `CL_GET_PC_LIST` (150).
5. `CL_SELECT_PC` with a character seeded into `DARKEDEN.Slayer` —
   `CLSelectPCHandler`, where 18-C lived. Note the audit records that only
   the Vampire branch was ever runtime-verified; a driver that selects a
   Slayer and an Ousters character covers what a human session did not.
6. Optionally reconnect to gameserver 9998 with `CG_CONNECT` to reach the
   gameserver's packet surface (18-G, 18-H, 18-I).

Steps 1–3 are worth building on their own: they cover 18-B and 18-N, the
two bugs that made the servers unusable, and need no character data.

### Implementation notes

- **Python 3 stdlib, no dependencies.** `socket` + `struct`. Do not add a
  build step to CI for this.
- Put it at `dkrixserver/scripts/ci-packet-driver.py` and drive it from
  `ci-boot-smoke.sh` as a fourth subcommand (`drive`), so the whole thing
  stays one script from the workflow's point of view.
- **Assert on the server side, not the client side.** The driver does not
  need to parse replies correctly to be useful; what matters is that the
  servers stay alive and write no ASan report while handling the traffic.
  Parse only enough to know a step was accepted. A driver that
  over-validates replies becomes a maintenance burden the moment a packet
  changes shape, and this tree changes packet shapes.
- **Send malformed packets too, once the happy path is green.** 18-G
  (`IndexNum` from two wire bytes, unbounded above) and open item 2 of the
  audit (`CGAddItemToCodeSheetHandler`, heap OOB read *and* write from two
  bytes) are reachable by sending out-of-range field values. A dozen
  hand-written hostile packets under ASan is the cheapest fuzzing this
  codebase can get.
- **Keep the seed data in SQL, tracked.**
  `docs/smoke-test/create_test_account.sql` already exists; a companion
  seeding one character of each race belongs next to it.

### Cost estimate

Steps 1–3 are perhaps 150 lines of Python and one seed SQL file. The
gameserver leg and the hostile-packet corpus are the larger half. The
expensive part — a seeded database and a booted server under ASan — is
already done and is what slice 1 delivers.

---

## 5. Honest summary

| Layer | Before | After slice 1 | After slice 2 |
|---|---|---|---|
| Compiles under ASan | yes | yes | yes |
| Boots under ASan | no | **yes** | yes |
| Logs in under ASan | no | no | yes |
| Handles hostile packets under ASan | no | no | yes |
| Phase 18 bugs the gate would have caught | **0** | 5 of 14 | ~11 of 14 |

Build-only was 0 for 14. Boot-only is 5 for 14. Neither is a substitute for
`docs/SECURITY-AUDIT-2026-08-10.md` §5 action 7 — re-run the smoke test
under ASan after every further fix — which remains the only gate that has
ever caught anything at runtime in this tree.
