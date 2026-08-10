# Security & correctness audit — 2026-08-10

Consolidated status of the Phase 18 sitting. Sources: `docs/MODERNIZATION.md`
Phase 18 sections (cited `MOD`) and the **21** commits on
`fix/bug-18b-loginserver-result-uaf` (`git log main..HEAD`, cited by hash).
No claim here is derived from source outside those two.

**Verification vocabulary** — used strictly:

| Level | Means |
|---|---|
| **runtime-verified** | the fixed code was executed on the workstation (smoke run and/or AddressSanitizer) and behaved |
| **compile-verified** | it builds; the path was never executed |
| **reasoned** | argued from reading code only |

Two source disagreements found while writing this, flagged inline: **(D1)** `MOD`
documents Bugs 18-A…18-E only — **18-F…18-N exist solely in commit messages**;
**(D2)** `MOD`'s "CONFIRMED and still open" index list was not updated by the
commit that fixed most of it.

---

## 1. Fixed

| ID | What | File | Found by | Verification |
|---|---|---|---|---|
| 18-A | Two SQL statements concatenated into one literal; gameserver could not boot at all since 2026-08-09 | `gameserver/item/Key.cpp` | smoke run 1, first launch | **runtime-verified** (`MOD`) |
| 18-B | `Result` use-after-free — loginserver segfaulted on *every* login, valid or not | `Core/CLLoginHandler.cpp` | crash + gdb | **runtime-verified** (`f15bb13`) |
| 18-C | Same UAF on character select | `Core/CLSelectPCHandler.cpp` | audit of the 18-B candidate set | **runtime-verified, Vampire branch only**; Slayer/Ousters compile-verified (`38dfbbc`, `aef987a`) |
| 18-D | `new char[]` released with plain `delete` (3 vars) | `gameserver/Zone.cpp` | ASan | **runtime-verified** (`6e356af`) |
| 18-E | `ExpTable` sized `MaxLevel` but written 1-based → 4-byte heap overflow **on every boot** | `gameserver/SomethingGrowingUp.h` | ASan | **runtime-verified** (`6e356af`) |
| 18-F | Uninitialised member pointers `delete[]`d in `clear()`; 6 copies across all three servers | `*/GameServer{Group,}InfoManager.cpp` | ASan | **runtime-verified** (`5b8a259`) |
| 18-G | Store index guard `> MAX_ITEM_NUM` where vectors hold exactly 20 → client-reachable heap OOB read **and write** + virtual call | `CGDisplayItem/CGUndisplayItem/CGBuyStoreItemHandler` | packet audit | **compile-verified only** — no store interaction in any smoke session (`5b8a259`) |
| — | `Player::setKey`: uninitialised `pHashTable` read (it gates an `exit(0)`), plus 512-byte leak per `CGConnectSetKey` | `Core/Player.cpp` | packet audit | **runtime-exercised** (login path, ASan-clean run) (`5b8a259`, `0bdc648`) |
| 18-H | `read(m_IdempotencyKey)` bound to the raw template → attacker-chosen pointer read + free. Only such site tree-wide | `Core/CGExchangeBuy.cpp` | packet audit | **compile-verified only**; feature is dead client-side (`a29ee09`) |
| 18-I | Bounds checks added at the accessors: `Inventory::getInventorySlot/getItem`, `isWear/getWearItem/takeOffItem` ×3 races; 3 GM commands ungated by an `||`/`&&` precedence typo; 49 `sprintf`→`snprintf` in `CGSayHandler` | `Inventory.*`, `Slayer/Vampire/Ousters.*`, `CGSayHandler.cpp` | packet audit | **compile-verified only** (`1019602`) |
| 18-J | `PetAttrInfo` ctor `reserve()`+`clear()` → writes into unconstructed storage | `gameserver/PetAttrInfo.h` | ASan at boot | **runtime-verified** (`3e02f6c`) |
| 18-K | 5 mutating calls inside `Assert()` — e.g. `Assert(pStore->removeStoreItem(...))`; silent item duplication **in a Release build only** | `CGShopRequestSell`, `CGUseItemFromGQuestInventory`, `couple/WaitForMeet` | audit | **compile-verified only**; unreachable in the deployed Debug build (`3e02f6c`) |
| 18-L | `GameServerPlayer*` array never initialised; `!= NULL` passed on garbage, then dereferenced | `sharedserver/GameServerManager.cpp` | ASan SEGV | **runtime-verified** (`15c51f0`) |
| 18-M | (a) narrowing-before-check in the 18-H fix itself; (b) `Zone` ctor leaves 4 members indeterminate that `~Zone()` `delete[]`s; (c) `Assert(m_pWearItem[Part] = NULL)` — assignment, not comparison | `CGExchangeBuy`, `Zone.cpp`, `Slayer.cpp` | adversarial review of this branch | **compile-verified only**; (b) fires only on a zone-load failure (`002fcaf`) |
| 18-N | 1-based loops ran to `Num` inclusive on a stack VLA sized `Num` — aborted every login under ASan | `Core/CLGetWorldListHandler.cpp` | ASan during smoke run | **runtime-verified** (`0bdc648`) |
| — | `bindInt` on a `DWORD` StorageID (only such site vs 83 `bindLong`) | `item/Cross.cpp:175` | bind audit | **reasoned**; not reachable at current ID values (`150cf9a`) |

**Milestone** (`0bdc648`): with 18-N fixed, all three servers boot clean under
AddressSanitizer and a client completes login → character select → enter world →
gameplay with **zero ASan reports** — a first for this tree. Movement and clean
logout were verified in the earlier non-ASan run (`e362ca2`). Character creation
(`CLCreatePC`) was **never exercised** in any run.

Also landed, non-bug: two format-only commits (`3ca03fe`, `c8fbd67` — Zone.cpp's
1,188-line reformat, verified token- and assembly-equivalent) and the secret
untracking below.

---

## 2. Open, ranked by exploit value

| # | Gain to attacker/cheater | Where | Fix | Status |
|---|---|---|---|---|
| 1 | **Unauthenticated remote shutdown of any server.** Two magic key constants trigger `exit(0)`; `CGConnectSetKey` is registered on both login and game servers with no auth gate | `Core/Player.cpp:236-239` | delete the `exit(0)` branch | **deliberately not fixed** — intentional 2008-era anti-cheat (`// add by viva`); a policy call for the owner, not a mechanical fix (`MOD`) |
| 2 | **Heap OOB read and write from two wire bytes.** `IndexNum = (y*10+x)/2`, max ~1402, against ~30 elements; guarded only by a *lower* bound on the container | `CGAddItemToCodeSheetHandler.cpp:104` (read), `:131` (write) | bound `IndexNum` against the container size at entry | open — untouched by any commit on this branch |
| 3 | **Pre-auth OOB table read on every logged packet.** `readPacket()` calls `toString()` unconditionally; `CGConnect`'s `PCType2String[m_PCType]` (raw `BYTE` into 3 elements) is reachable before authentication | `SocketInputStream.cpp:181` + `CGConnect::toString()` | validate in `read()`, or bound the lookup | open (`MOD`, verified round) |
| 4 | Same shape, post-auth: `CLCreatePC::toString()`'s `HairStyle2String[(bits>>1)&3]` yields 0..3 into a 3-element table | `CLCreatePC.cpp` | widen the table or mask to 0..2 | open — found by the verification round, unclaimed by any auditor |
| 5 | **All Assert-based bounds enforcement is one `make release` away from vanishing** — including checks landed in 18-I's neighbourhood. Separately `Slayer::setPhoneSlotNumber` has no assert at all on a 3-element array, and `getPhoneSlotNumber` asserts `<= MAX_PHONE_SLOT`, itself off by one | `Slayer.h`, tree-wide | convert security-relevant `Assert`s to real runtime checks (18-I did this at the accessors) | open |
| 6 | Resource exhaustion at startup: ~15 legacy `Statement`/`Result` leak sites (several per-zone), `END_DB` frees only on exception. Also `MonsterKillQuest.cpp:64` executes the literal SQL `"-_-"` | list in `MOD` (legacy `Statement` audit) | free on the success path | open, **not** use-after-free |
| 7 | 7 "near-miss" dangling `Result` sites — dangling but never read; one edit from becoming a live 18-B | `sharedserver/GuildManager.cpp:92,108,124,140`, `Key.cpp:198`, `CLLoginHandler.cpp:677`, others in `MOD` | apply the 18-B hoist pattern | open by choice |
| 8 | Wire-format mismatch: server `CGExchangeBuy` now uses a `BYTE`-length-prefixed key capped at 64; the client still sends a bare 4-byte uint | `dkrix/Client/Packet/Cpackets/CGExchangeBuy.cpp` | `uint`→`uint64_t` + length prefix, shipped with the server change | **deliberately not fixed** — house rule: both trees together; the packet never worked, so nothing regresses (`a29ee09`) |
| 9 | Hardcoded remote DB `Connection(...)` in a GM path | `CGSayHandler.cpp:2934` | remove / parameterize | open (`c587490`) |

**(D2)** `MOD`'s "CONFIRMED and still open" list also names
`CGReloadFromInventoryHandler`, `CGUseItemFromGearHandler`,
`CGAddGearToMouseHandler` and `CGSkillToInventoryHandler`. Commit `1019602`
fixed all four **at the accessors** (chokepoint, real runtime checks, not
`Assert`), and `3e02f6c` narrowed the last one. `MOD` was not updated; treat
those four as fixed-but-compile-verified-only, per §1 row 18-I, and treat the
doc's list as stale.

**Not traceable.** The brief for this document also named a *GGCommand UDP gate*,
a *distance-check class* of cheats, and *client-side overflows* as deliberate
non-fixes. Neither `MOD` nor any of the 21 commits mentions them (`GGCommand.cpp`
exists in the tree; no finding about it is written down anywhere). They are
recorded here as **unsourced** — get a source before acting on them.

**Reverted, and worth knowing why** (`3e02f6c`): adding the TX/TY check to
`isAbleToUseInventorySkill` looked like a security fix and **broke live
gameplay** — Bloody Mark, Bloody Tunnel and all three Transform skills never
initialise `m_TargetX/Y`, so they put uninitialised stack values on the wire.
That is why the check was commented out upstream. No security was lost: the
`Create*` skills that do index the inventory are covered at the accessor.

---

## 3. Investigated and REFUTED

| Claim believed | What is actually true | Evidence |
|---|---|---|
| `Assert` is compiled out of the shipping build, so every Assert-based bounds check is a live exploit | **The deployed build is Debug and `Assert` throws.** `Makefile:11` is `all: debug`; `server.yml:76` uses `make debug`; no build tree sets `NDEBUG`; `bin/gameserver` still contains the stringified assert expressions | `MOD` packet audit [measured]. **Root cause: a wrong line in `dkrixserver/CLAUDE.md` that documented `make` as Release — it misled two independent security reviews.** Corrected in `a29ee09` |
| The live DB password is in published history (repo public since 2026-08-08) | **It is not.** The committed value is a stale 7-character credential; the live 9-character one exists only as an uncommitted working-tree modification. Any `git add -A` would have published it | `c587490`. Untracking landed (`c587490`, `0bdc648`); history rewrite judged unwarranted — it would break the `archive/modernization-phases-1-17` tag and every published SHA |
| Legacy `Statement`: a `const char*` from `getField()` is invalidated by `next()` | **No such hazard.** `Statement::executeQuery()` uses `mysql_store_result` (`Statement.cpp:116`), which buffers the whole set client-side; each row's storage is independent. The claim would hold only under `mysql_use_result`, never called here | `MOD` — explicitly **WITHDRAWN 2026-08-10** |
| Turning on an ASan job in CI would have caught 18-B | **A build-only ASan job catches neither 18-A nor 18-B.** ASan reports at runtime; CI never logs in | `6e82736` — corrects an earlier revision of `MOD` |
| 203 `Result*` sites share the bug shape → 203 candidate bugs | Shape count, not bug count. 129 files / 200 assignment sites audited by five auditors → **1** live bug (18-C). The rule: `Result* p = stmt.execute();` is **safe by construction** (359 sites); only the 25 pre-declared `Result*` can break it | `MOD` audit table; `192ccfe` |
| The 18-B crash was in `Statement::executeQuery()` | Wrong function. The IP offset was computed against `dmesg`'s mapped-segment base, not the ELF load base — off by `0x10000` | `MOD` correction block |
| `CLCreatePC::toString()` is never called | It is — `LoginPlayer.cpp:217` → `readPacket` → `toString` | `MOD` verification round |
| A third loginserver segfault means something is still crashing | Not attributable and **not live** — the instance that ran the end-to-end session started 12:22:12 and is still up; nothing restarts it | `e362ca2` |
| The Phase 11 migration probably broke SQL/binds too | **Clean.** All 1,575 `PreparedStatement` constructions across 208 files audited: 0 defects. The damage was object *lifetime* (18-B, 18-C) and one malformed literal (18-A) | `MOD` bind-audit table; `655ade7` |
| ~487 legacy `executeQuery` sites remain | Roughly half are inside `/* … */` blocks left by the migration. Any "legacy work remaining" estimate from raw grep is inflated ~2× | `MOD` legacy-`Statement` audit |

---

## 4. Known-broken safety nets

| Gate | Reports | Why it is not protecting you |
|---|---|---|
| SQL-injection ratchet (`check-sql-injection.sh`) | **0** since 2026-08-09 | It counts only *single-line* `executeQuery`/`setStatement`/`Statement` calls carrying a `%[sdluxc]` specifier. Multi-line `StringStream` sites were never visible to it (`MOD` Phase 11.2, repeatedly), the ~22 deliberate identifier/expression splices are invisible, and Bug 18-A — a malformed SQL *literal* — is invisible by construction. "0" means "no printf-style one-liners", not "no injection surface" |
| Server + client CI | never ran on this branch | `server.yml` triggers on push to `[main, "modernize/**"]` and PRs targeting `main`. `fix/bug-18b-loginserver-result-uaf` matches neither and no PR exists, so **all 21 commits are unbuilt by CI**. The only gate applied was local WSL `make debug`. `c8fbd67`'s "CI will go green" is a prediction, not a result |
| `sanitizers` matrix (`asan` blocking since 2026-08-09) | green builds | **Build-only** — it runs `make debug-asan` and never executes a server, so it cannot see 18-B/D/E/F/J/L, the entire class it appears to cover. The `ubsan` leg is 0-for-17 green and non-blocking. Making ASan a real gate needs a seeded MySQL service container and a synthetic packet driver |
| `clang-format` job | red on legacy files you touch | It checks whole changed files. Zone.cpp carried 535 pre-existing violations, `CLSelectPCHandler` 16 — a bias against touching old code, absorbed here by two separate format-only commits |
| SQL syntax lint over prepared literals | — | Does not exist. This is the cheap gate that would have caught 18-A |

---

## 5. Operational actions

1. **Rotate the DB credential.** The stale password is in public history since 2026-08-08. Do it even though the live value was never published — assume the stale one is burned.
2. **Untrack the 10 remaining `.conf` copies** carrying that same stale secret: `dkrixserver/conf/excel96-*.conf` (+ `.new`, `gameserver2`), `conf/updateserver.conf`, and all 4 under `conf/backup/`. `conf/{game,login,shared}server.conf` and `docker/conf/*.conf` are already untracked (`c587490`, `0bdc648`); the `.template` files stay.
3. **Replace `docker-compose.yml`'s `MYSQL_ROOT_PASSWORD`** (a trivial literal) and parameterize `initdb/a-setup.sql`.
4. **Decide the `Player::setKey` killswitch** — keep or delete. Until then any client can terminate either server at will.
5. **Answer the UDP question:** confirm whether the gameserver's UDP 9997 is reachable off-LAN and firewall it if so. `docs/smoke-test/PORTING-NOTE.md:57` records that `CLAUDE.md` had conflated this port with another — nobody has checked its exposure.
6. **Get CI onto this work** — open a PR to `main` (or push the branch under `modernize/**`). 21 commits, including two format commits written to satisfy a gate that never ran.
7. **Re-run the smoke test under ASan after every further fix.** It is the only gate that has caught anything at runtime, it is cheap locally, and it found 18-D/E/F/J/L/N in one sitting.
8. **Do not run `make release`** without first converting the security-relevant `Assert`s to real checks (§3 row 1).
9. **Do not rewrite history** — no live secret is published, and a rewrite breaks the parked archive tag and every published SHA.
