# Security & correctness audit — Phase 18

**Scope.** 39 numbered defects (18-A … 18-Z, 18-AA … 18-AM) plus unnumbered
items, found 2026-08-10 → 2026-08-11 across five waves on
`fix/bug-18b-loginserver-result-uaf`; extended 2026-08-13 by wave 9 (18-AU …
18-AY) working the §1 backlog directly on `main`. **The commit messages are the
record; this file is an index over them.** Every claim below carries a commit hash or a
`file:line`. Anything that could not be verified from the repository says so in
place rather than being restated.

**This file is organised so it does not go stale.** It was rewritten twice as a
snapshot and was superseded both times before it was committed. The structure is
now: what is open (§1), what is fixed (§2, a table keyed by ID), what has been
disproved (§3, append-only), what keeps recurring (§4), and what has actually
been *executed* (§5). Volatile facts — merge state, counts, wave numbering — are
confined to §8. See §9 for the maintenance rule.

---

## 1. Open now, ranked by exploit value

Nothing else belongs above this section. Each row is either exploitable today or
blocks something that is. Rows close by moving to §2, not by being edited in
place.

**Highest-value row right now is 19 — gold duplication in `TradeManager`.** It
is live rather than latent, it is the only *duplication* defect this effort has
found (every other custody bug destroyed value rather than creating it), and it
needs a decision on fix shape rather than an obvious patch. Rows are otherwise
in their original rank order; 19 was found late, by wave 10, and is not
re-numbered because rows close by moving, not by renumbering.

| # | Gain to attacker / cheater | Where | Status |
|---|---|---|---|
| ~~**1**~~ | ~~UDP denial of service that needs no bug at all~~ — **fixed 2026-08-13 as 18-AU, moved to §2.** |  |  |
| **2** | **A working speed hack.** `GamePlayer::verifySpeed`'s `PACKET_CG_MOVE` branch computed a round-trip interval and discarded it; there is no last-move timestamp on `Creature`. The `CGVerifyTime` gate meant to catch it is dead three ways over — `verifySpeed` runs twice per packet (`GamePlayer.cpp:541` + `CGVerifyTimeHandler.cpp:34`), the outer call pushes the deadline 60s ahead so the inner test is always false, the two calls decrement and increment one counter 1:1, and the current client never sends `CGVerifyTime` at all. 18-AJ added **measurement only** — a histogram plus min-interval and max-per-tick counters flushed to `MoveRate.log` (`GamePlayer.cpp:1106`). **Enforcement is still impossible**: every "speed" identifier under `dkrixserver/src` is *weapon attack* speed; movement cadence lives in the client's binary `MCreatureTable` (`MoveTimes`, `MoveTimesMotor`, `MoveRatio`) and `CreatureTable` appears nowhere server-side | `gameserver/GamePlayer.cpp:943-1110` | **open — needs data, then an owner decision** (`89b2892`, `72d392e`) |
| **3** | **`CGMove` field order disagrees between the trees.** Server's non-encrypted branch reads `Dir, X, Y` (`dkrixserver/src/Core/CGMove.cpp:29-31`); client writes `X, Y, Dir` (`dkrix/Client/Packet/Cpackets/CGMove.cpp:59-61`). All three are `BYTE`, so the fields are silently permuted and the server loads the client's X into `m_Dir`. Both `SHUFFLE_STATEMENT_3` branches agree, so only the `getEncryptCode() == 0` fallback is affected — but the client does send `CGMove` live and it is the highest-frequency packet in the game | both trees | **open — needs a coordinated two-tree commit** (`6f45414`) |
| ~~**4**~~ | ~~The login-failure lockout never locks anyone out~~ — **fixed 2026-08-13 as 18-AY, moved to §2. Read the new §1 row 16 before treating this as closed:** the counter now counts, but it is per-TCP-connection, so it does not constitute brute-force protection. |  |  |
| ~~**5**~~ | ~~`nMaxPlayers` is 2000 against glibc's fixed 1024-bit `fd_set`~~ — **fixed 2026-08-13 as 18-BB, moved to §2.** The row asked for a two-line test first; it was run, and confirmed the claim empirically. |  |  |
| ~~6~~ | ~~`CGSilverCoatingHandler.cpp:39` race chain~~ — **fixed 2026-08-13 as 18-AV, moved to §2.** The row's "`dynamic_cast` returns NULL" was wrong; it is a `checkedCast` since 18-AQ, so it threw `InvalidCastError` instead. |  |  |
| 7 | `GCExchangeBuy::read()` still carries 18-H's arbitrary-read primitive: `iStream.read(m_Message)` on a `std::string` binds to the raw template `read<T>()`, reinterpreting wire bytes as a live `std::string` — attacker-chosen source pointer and length, and the wrap branch frees an attacker-chosen pointer. `write()` emits it unprefixed. **Now doubly latent**: no factory registers it, and 18-AF `#if 0`'d both `CGExchange*` registrations too (`PacketFactoryManager.cpp:1193-1197`), so the whole subsystem is off the wire | `Core/GCExchangeBuy.cpp:23` | **open, latent — downgraded by `ec27e95`** |
| 8 | `CGExchangeList` desyncs the stream — server reads a trailing length-prefixed seller filter, client writes 19 bytes and stops. Same downgrade as row 9: `CGExchangeListFactory` is no longer registered | both trees | **open, latent** (`b9fac7d`, `ec27e95`) |
| 9 | `CGUnburrow` direction persists unbounded. 18-AC deliberately did **not** reject `Dir` in `read()` — `DIR_NONE` aliases `DIR_MAX`, so 8 is a live sentinel and a range check would be unsafe. Lookups are bounded, but `addUnburrowCreature` → `Creature::setXYDir` stores the out-of-range `dir` verbatim into creature state, where it resurfaces through `PCVampireInfo3::toString()` | `gameserver/`, `Creature.h:234` | **open — sentinel contract must be decided first** (`6f45414`) |
| 10 | Assert residual. 18-U converted the 25 wire-reachable bounds Asserts, 18-AD hoisted 6 mutating calls, 18-AG hoisted 47 parse calls, and 18-AH/AL hardened the surviving accessors — so `make release` is far less dangerous than it was. But the conversions were scoped to *wire-reachable* checks; server-side-index Asserts in `ItemInfoManager` and friends were deliberately left, as were `CGSMSSend`'s size caps (policy limits; converting them turns soft caps into disconnects) | tree-wide | **open, much reduced** |
| ~~11~~ | ~~`TradeManager` residuals~~ — **fixed 2026-08-13 as 18-BD, moved to §2.** The row was wrong on both counts: the item-loss path was *reachable*, not gated by `canTrade`, and the asset `cancelTrade` stranded is **gold**, not items — items are never escrowed. **Superseded by row 19**, a third and worse defect found in the same file. |  |  |
| ~~12~~ | ~~Startup fragility in `InfoClassManager::init()` and `SkillParentInfo::load()`~~ — **fixed 2026-08-13 as 18-AW, moved to §2.** |  |  |
| 13 | `initdb/DARKEDEN.sql:11568` seeds `WorldDBInfo` with Host/User/Password **as data**, read at startup by `DatabaseManager.cpp:108-134`. No env var reaches it, so changing the DB password requires an `UPDATE` | seed dump | **open by necessity** (`3dcfb1b`) |
| 14 | `DB_PORT` is hardcoded to 3306 in the conf templates, so the boot script's `DB_PORT` reaches the seeding step but not the servers. CI works only because the service container happens to be on 3306 | `dkrixserver/conf/*.template` | **open, CI/repro only** (`cc1fcd4`) |
| ~~15~~ | ~~Both `memoryError()` handlers `exit(0)` on a fatal OOM~~ — **fixed 2026-08-13 as 18-AX, moved to §2.** The row did not say that the *gameserver* already called `abort()`; this was inconsistency between the three binaries, not a uniform bug. |  |  |
| ~~**16**~~ | ~~The login server still has no brute-force protection~~ — **closed 2026-08-13 as 18-BF, moved to §2.** Owner-approved as a new control. Residuals it does *not* close are recorded in that row. Original text: **The login server still has no brute-force protection**, and 18-AY must not be read as having provided it. `LoginPlayer::m_FailureCount` is a `uint` in memory, allocated per accepted TCP connection and destroyed with it — per-connection, not per-account or per-IP, nothing persisted. The stock client opens a **new connection for every login attempt** (`Execute_UI_LOGIN` → `InitSocket` → `ReleaseSocket` + reconnect), so an attacker who reconnects per guess — which is exactly what the normal client does — is entirely unaffected. 18-AY caps *pipelined* guesses on one socket at 3, which was previously unlimited; that is its whole value. Real protection is a per-account or per-IP counter with a time window, i.e. a **new control to be designed**, not a bug to be fixed | `loginserver/LoginPlayer.h:217`, `Core/CLLoginHandler.cpp` | **open — needs an owner decision on policy** (`fd98c81`) |
| ~~**17**~~ | ~~`UserGateway::clear()` reserves 20 then indexes `[0,20)`~~ — **closed 2026-08-13 as 18-AZ by deletion, moved to §2.** The file could not have compiled in any build: header declared six members with no exception specification, `.cpp` defined all six `throw(Error)`. |  |  |
| ~~**18**~~ | ~~`quest/Squest/`~~ — **closed 2026-08-13 as 18-BA, moved to §2.** 29 files, not 13. |  |  |
| ~~**19**~~ | ~~Gold duplication on the trade-completion path~~ — **fixed 2026-08-13 as 18-BE, moved to §2**, owner-approved fix shape (clear the escrow). Original text: **Gold duplication on the trade-completion path — live, and the only defect this effort has found that *creates* value rather than destroying it.** After the trade commits and gold is credited via `setGoldEx` (which persists), `processTrade` still dereferences `getPlayer()->getSocket()->getHost()` and runs a `TradeLog` `INSERT` under `BEGIN_DB`/`END_DB` **before** `pInfo1->clearAll()`/`pInfo2->clearAll()`. `END_DB` rethrows on any SQL failure, so a failed log insert — or a NULL socket — aborts with `TradeInfo::m_Gold` still populated on both sides; the disconnect that follows calls `cancelTrade`, which credits that gold **a second time**. Items are not duplicated (the stale pointer list is never re-applied); gold is. Two candidate fixes — clear the escrow immediately after the credit, or contain the logging tail — and choosing between them is a decision about what a failed `TradeLog` write should mean | `gameserver/TradeManager.cpp:823-870` | **open, live — owner decision on fix shape** (`f33e12a`) |
| **20** | **The gameserver has the same connection-parking gap the loginserver just closed.** `GPS_BEGIN_SESSION` carries a 300s deadline that **slides** — refreshed by every accepted packet, exactly the shape 18-BC replaced on the login side, where one cheap packet every few minutes parked a descriptor forever. Same fix applies; it was simply outside the scope wave 10 was given | `gameserver/GamePlayer.cpp:66` | **open** (`b3e9322`) |
| **21** | **`acceptNewConnection` is an unrate-limited log amplifier.** Every accepted connection on internet-facing TCP 9999 does a `log()` plus `cout` plus `cerr` ("NEW CONNECTION FROM…") — a file write per connection *attempt*, sitting three lines from 18-AK's carefully rate-limited **rejection** log. Now that 18-BC caps how long a connection can park, this is the cheapest remaining thing to hammer | `loginserver/LoginPlayerManager.cpp:490-494` | **open** (`b3e9322`) |
| **22** | **`Ousters.h:369` and `Vampire.h:356` — `addWearItem` has no bounds check**, so `m_pWearItem[Part] = pItem` is an arbitrary-offset heap write. Every neighbour in both classes (`isWear`, `deleteWearItem`, `getWearItem`) has `if (Part < 0 || Part >= *_WEAR_MAX) return;`. Slayer's got one from 18-R, **whose commit message calls it "the last wear-slot write" — it was not**, and the two twins were missed. Latent: `addWearItem` has no callers anywhere in the tree. Found by the pattern-2 gate, not by a person | `gameserver/Ousters.h:369`, `Vampire.h:356` | **open, latent** (`f73767b`) |
| **23** | **`addSkillParentInfo` is the residue of 18-AW** — that fix bounded `load()` and `getSkillParentInfo()` but left `if (m_SkillParentInfoList[pSkillParentInfo->getSkillType()] != NULL)` indexing the raw DB-sourced `SkillType` in order to test it, one function away from the code it fixed. `SkillInfo.cpp:404`, `RankBonusInfo.cpp:173` and three `AttrBalanceInfo` sites are identical twins — the load-time duplicate-key check is a recurring shape in its own right | `gameserver/SkillParentInfo.cpp:288` + 5 twins | **open, low** (`f73767b`) |

**Two changes no compiler will ever see**, carried deliberately and flagged
(`c5bebb7`, `65c07ad`): `gameserver/GuildManager.cpp`'s `init()` body is inside
`#ifdef __SHARED_SERVER__` but the file is only ever built as `__GAME_SERVER__`;
`MonsterKillQuest.cpp` is in no CMake source list. A typo in either would not
surface. Two more files are in the same state — see §3.

**↳ Both halves of that note were investigated 2026-08-13 and neither is what it
says.** The `GuildManager` half is **refuted** — there are two files of that
name and the premise conflates them (§3.1). The `MonsterKillQuest` half is true
but far too narrow: it is 1 of 13 files in a wholly uncompiled subsystem (§1 row
18). What survives is only the narrow observation that a typo in either would
not surface, which remains correct and remains uncorrected. **The complete
measurement now exists: 82 `.cpp` files under `dkrixserver/src/` are in no
source list, with 0 dangling entries in the other direction** [measured
2026-08-13 against all 18 `CMakeLists.txt`, `a8503bf`]. A structural argument
bounds the risk — the tree links, so no orphan can define a symbol that compiled
code calls; the residual hazard is silent shadowing (6 files, all with a
compiled sibling of the same name) and never-registered runtime types (8
packets).

---

## 2. Fixed — reference table

One row per ID, sorted by ID, each citing its commit. Verification uses §5's
vocabulary strictly.

**Cut line for `main`** [measured 2026-08-13 by `git branch -r --contains`]:
**every ID in this table is on `main`** — 18-A … 18-BF, the whole effort — via
merge commits `669a9fe` (PR #2), `90a926e` (PR #3), `760b48f` (PR #4),
`e86a9dd` (wave 8, merged directly rather than by PR), and waves 9 and 10
(18-AU … 18-BF, `a8503bf`…`b236e70`) committed straight onto `main` on
2026-08-13. **There is no tail.** That single sentence is the only place merge
state appears in this table; update it here when that stops being true.

| ID | What | Where | Commit | Verification |
|---|---|---|---|---|
| 18-A | Two SQL statements concatenated into one literal; gameserver could not boot at all since 2026-08-09 | `gameserver/item/Key.cpp` | `9422e9c` | runtime-verified |
| 18-B | `Result` use-after-free — loginserver segfaulted on *every* login, valid or not | `Core/CLLoginHandler.cpp` | `f15bb13` | runtime-verified |
| 18-C | Same UAF on character select | `Core/CLSelectPCHandler.cpp` | `38dfbbc`, `aef987a` | runtime-verified, **Vampire branch only**; Slayer/Ousters compile-verified |
| 18-D | `new char[]` released with plain `delete`, 3 vars | `gameserver/Zone.cpp:798-800` | `6e356af` | runtime-verified (ASan) |
| 18-E | `ExpTable` sized `MaxLevel` but written 1-based → 4-byte heap overflow **on every boot** | `gameserver/SomethingGrowingUp.h` | `6e356af` | runtime-verified (ASan) |
| 18-F | Uninitialised member pointers `delete[]`d in `clear()`; 6 copies across all three servers | `*/GameServer{Group,}InfoManager.cpp` | `5b8a259` | runtime-verified (ASan) |
| 18-G | Store index guard `> MAX_ITEM_NUM` where vectors hold exactly 20 → client-reachable heap OOB read **and write** + virtual call | `CGDisplayItem`, `CGUndisplayItem`, `CGBuyStoreItemHandler` | `5b8a259` | compile-verified — no store interaction in any session |
| 18-H | `read(m_IdempotencyKey)` bound to the raw template `read<T>()` → attacker-chosen pointer read + free | `Core/CGExchangeBuy.cpp` | `a29ee09` | compile-verified. Its "only such site tree-wide" claim is **false** — §3 |
| 18-I | Bounds checks at the accessors: `Inventory::getInventorySlot/getItem`, `isWear/getWearItem/takeOffItem` ×3 races; 3 GM commands ungated by a `\|\|`/`&&` precedence typo; 49 `sprintf`→`snprintf` | `Inventory.*`, `Slayer/Vampire/Ousters.*`, `CGSayHandler.cpp` | `1019602` | **partly runtime-verified** — inventory/wear accessors exercised (`d9b6af2`); GM-command and `snprintf` parts compile-verified |
| 18-J | `PetAttrInfo` ctor `reserve()`+`clear()` → writes into unconstructed storage | `gameserver/PetAttrInfo.h` | `3e02f6c` | runtime-verified (ASan at boot) |
| 18-K | 5 mutating calls inside `Assert()`; silent item duplication **in a Release build only** | `CGShopRequestSell`, `CGUseItemFromGQuestInventory`, `couple/WaitForMeet` | `3e02f6c` | compile-verified. Swept properly by 18-AD |
| 18-L | `GameServerPlayer*` array never initialised; `!= NULL` passed on garbage, then dereferenced | `sharedserver/GameServerManager.cpp` | `15c51f0` | runtime-verified (ASan SEGV) |
| 18-M | (a) narrowing-before-check in 18-H's own fix; (b) `Zone` ctor leaves 4 members indeterminate that `~Zone()` `delete[]`s; (c) `Assert(m_pWearItem[Part] = NULL)` — assignment, not comparison | `CGExchangeBuy`, `Zone.cpp`, `Slayer.cpp` | `002fcaf` | compile-verified |
| 18-N | 1-based loops ran to `Num` inclusive on a stack VLA sized `Num` — aborted every login under ASan | `Core/CLGetWorldListHandler.cpp` | `0bdc648` | runtime-verified (ASan) |
| 18-O | Only 2 of 150 handlers checked distance: ground loot could be vacuumed zone-wide. Chebyshev radius-8 reach test added to 3 pickup handlers via `ZoneUtil::isWithinReach()` | `CGAddZoneToInventory/Mouse`, `CGPickupMoney` | `d4f4d27` | runtime-verified — pickup exercised (`d9b6af2`) |
| 18-P | `reserve()` without `assign()` → writes through `operator[]` into unsized storage, 3 sites; `ItemGradeManager` left two of three vectors unsized until `load()` | `ZoneGroupManager`, `ItemGradeManager`, `war/DragonEyeManager` | `35b11b2` | compile-verified |
| 18-Q | 18-J's shape in `PetExpInfoManager`; also leaked all 51 objects at shutdown. **ASan cannot catch this one** — reserved capacity covers every write | `gameserver/PetExpInfo.*` | `a894d7c` | compile-verified |
| 18-R | `deleteWearItem`, the last unguarded wear-slot write accessor, 3 races. **Plus four network-reachable client stack overflows**: `GCPartySay`, `GCGuildChat`, `GCSystemMessage` (guard was `if (szMessage > 256)` on a `BYTE` — tautologically false), `GCWhisper` (cap 128 into `char[128]`, the NUL overflows). The launcher updates over plain HTTP, so an on-path attacker can stand in for the server | 3 server race files; 4 client `GC*Handler.cpp` | `75455ec`, fmt `42688d6` | compile-verified server-side; **client half CI-verified only** |
| 18-S | `CGSayHandler::opnotice` opened a `Connection` to a hardcoded **remote** host with credentials compiled into the shipped, published binary, then wrote a chat-supplied string into it as raw SQL. Disabled; `billing/test/` deleted (two more tracked `.conf` with passwords) | `Core/CGSayHandler.cpp` | `2c5ebe9` | compile-verified |
| 18-T | Three OOB packet fields rejected at `read()`: `CGAddItemToCodeSheet` `IndexNum` (~1402 against a **30**-element vector, read *and* write), `CGConnect` `PCType` (pre-auth, 3-element table), `CLCreatePC` `HairStyle` **and `Slot`** — the latter a raw 0–255 `BYTE` into a 3-element table that also feeds SQL binds | `shared/Packets/` | `4b76cc5` | runtime-verified — character creation exercised (`d9b6af2`). Its `CLDeletePCHandler` credit is **wrong** — §3 |
| 18-U | 25 wire-reachable `Assert` bounds checks converted to **real** checks that survive `NDEBUG`: Slayer phone accessors, `Datagram`/`SerialDatagram` bounds, pet stash, 11 NPC shop-rack accessors, 4 `ItemRack` accessors | tree-wide | `326c298` | **partly runtime-verified** — wear-slot accessors only; shop rack, phone slots, pet stash and the whole UDP `Datagram` path remain compile-verified |
| 18-V | 8 files' pre-declared `Result*` scoped to their statements — 18-B's near-miss siblings. All dangled; none was read, so none was live | 8 files, incl. `FlagManager.cpp:355` | `c5bebb7` | **partly runtime-verified** — `CLLoginHandler` only |
| 18-W | Success-path `SAFE_DELETE(pStmt)` in 21 legacy `BEGIN_DB` blocks (`END_DB` frees only on the exception path); 3 uninitialised `Statement*` where `END_DB`'s `delete` is reachable before assignment; `MonsterKillQuest::save()`'s literal SQL `"-_-"` → a documented `throw` | 20 files | `65c07ad` | **partly runtime-verified** — boot loaders only |
| 18-X | `ExpTable::load()` fed a runtime-built `snprintf` buffer to the **variadic printf-style** `executeQuery()` — the buffer became the format string. Switched to `executeQueryString()`. Only live site of its class. Plus an uninitialised `Statement*` and a real bounds check beside an `Assert` on a DB-indexed heap write | `gameserver/SomethingGrowingUp.cpp` | `36727b4` | runtime-verified at boot; the `%`-injection path itself never exercised |
| 18-Y | **Unauthenticated remote shutdown killswitch deleted.** `Player::setKey` compared a client-supplied key pair against two hardcoded constants (`0xAEB7`, `0x9B3E`, both in published source) and called `exit(0)`. `CGConnectSetKey` is registered on **both** servers with no auth gate. 2008-era anti-cheat; the owner chose removal | `Core/Player.cpp:242` | `5e088a4` | runtime-verified — `setKey` is on the login path |
| 18-Z | The seed dump `initdb/DARKEDEN.sql` **had never been loadable**. An English translation pass collapsed multi-byte strings to ASCII fallbacks: duplicate primary keys in 6 tables, then empty `ZoneInfo` names, then empty NPC names. Restored wholesale from `backup_darkeden_after_english_20260424.sql`. This broke the documented human runbook too, not only CI | `dkrixserver/initdb/DARKEDEN.sql` | `7270626`, `cc1fcd4` | runtime-verified — boot smoke PASSES |
| 18-AA | **The first runtime UB this tree has ever reported.** (1) misaligned type-punned load/store in the packet path — `*((T*)(m_Buffer + m_Tail)) = buf` and its mirror, i.e. *every packet the server writes*; replaced with `std::memcpy` of `sizeof(T)`, byte layout unchanged. (2) `memset(binds.data(), …)` where an empty vector's `data()` is `nullptr` against a `nonnull` parameter — the memset was redundant. (3) **`GQuestTouchWayPointElement::makeElement` cast five members to `DWORD&` and let `XMLTree::GetAttribute` write four bytes through each — four are `WORD`, one is `BYTE`; all five were out-of-bounds writes past their member.** Correct only by luck: writes happen in declaration order, each one's collateral damage repaired by the next, the last landing in tail padding. **Reordering the members would have silently corrupted quest waypoints** | `Core/Socket{Input,Output}Stream.h`, `database/PreparedStatement.cpp`, `gameserver/GQuestTouchWayPointElement.cpp:27-51` | `9cfe522` | **compile-verified.** The *findings* came from a real UBSan boot; the *fix* has never been re-run — `9cfe522` explicitly declines to claim the reports are gone |
| 18-AB | **The worst hole found in this project.** `GGCommandHandler` dispatched GM commands — `*shutdown`, `*command setGold`, `*command killAllPC`, `*kick`, `*set <economy ratios>`, `*load` — with **no privilege check of any kind**, passing `pGamePlayer = NULL`, from **UDP datagrams** with no source check, shared secret or sequence number. **UDP 9997 is deliberately port-forwarded.** One packet from anywhere on the internet could shut the server down, mint gold, or wipe a zone. No login, no character, no GM flag. **The loginserver had the identical hole.** Gated centrally in each receive loop immediately before `execute(NULL)`, with an **inverted** allowlist (client-facing IDs enumerated, so anything added later is peer-gated by default); gameserver allows only `PACKET_CG_PORT_CHECK`, loginserver's allowlist is empty. Peers = loopback + configured peer IP + the `GameServerInfo` IP column, so no new config. Alongside: `opcommand`'s ~66 `pGamePlayer->` dereferences behind 2 guards (a one-packet remote DoS and a live crash on the legitimate `*allworld` fan-out); **a remote OOB read in `UserInfoManager::getUserInfo()`** — an unvalidated packet-supplied `BYTE` `WorldID` (0–255) indexing an array of typically 3, so `WorldID=255` read ~250 `unordered_map`s past the allocation and called `.find()` on garbage; and 4 SQL-concatenation sites parameterised (`bug_report`'s sanitiser only ever replaced the **first** quote and backslash, so `a''b` still injected) | `gameserver/LoginServerManager.cpp`, `loginserver/GameServerManager.cpp`, `loginserver/UserInfoManager.cpp:161-175`, `Core/CGSayHandler.cpp`, `Core/GMServerInfoHandler.cpp` | `4d92150` | **compile-verified. None of these paths has been exercised at runtime** |
| 18-AC | The packet layer holds **exactly six** wire-integer→enum conversions; three were 18-T's and **all three of the rest were unguarded**: `CLDeletePC` `Slot` (raw 0–255 into `Slot2String[3]`, feeding **six SQL binds**, reachable from `LPS_PC_MANAGEMENT` by any authenticated account), `CLRegisterPlayer` `Sex` and `Nation`. Five `toString()` lookups bounded independently, since `readPacket()` calls `toString()` on **every** packet | `shared/Packets/CLDeletePC.cpp:27`, `CLRegisterPlayer.cpp:46,91`, `CGMove`, `CGUnburrow`, `CLSelectPC` | `6f45414` | compile-verified |
| 18-AD | 18-K's shape swept properly — every function called inside an `Assert` tree-wide extracted and vetted; 6 live sites in 4 files. **`WaitForApart.cpp:79,80` is item duplication, not a theoretical one**: `Assert(removeCoupleItem())` ×2. Under `NDEBUG` the calls vanish but `removeCouple()` and the `FLAGSET_IS_COUPLE` clear still run, so the pair re-couples and `WaitForMeet::giveCoupleRing()` mints a second pair — unbounded, rings carrying a random stat option. Plus `ActionTradeGQuestEventItem.cpp:163` (two DB rows at one `(x,y)`), `ActionQuestReward.cpp:86` (reward lost), `TriggerParser.cpp:21,22` (triggers silently inert). All six are **pure hoists** | `couple/`, `quest/` | `053b69f` | compile-verified |
| 18-AE | **Live client-reachable OOB read on the learn-skill path.** `SkillDomainType_t` is a wire `BYTE`; `Slayer::getSkillDomainLevel` indexes a 6-entry `m_SkillDomainLevels` with no bounds check. Every other skill is protected by the `actualDomainType > SKILL_DOMAIN_ETC` test, but `SKILL_SOUL_CHAIN` short-circuits it via the `chengh 2006-06-14` carve-out and is in the accepted-skill whitelist — so `CGLearnSkill{SkillType=185, DomainType=0xFF}` read up to 250 bytes past a 6-byte member. Plus `SkillHandlerManager.cpp:849` read `m_SkillHandlers[SkillType]` *then* tested the bound, with `>` off by one on a 0-based array. Plus two `Zone::movePC` bugs: `GCMoveError` echoed the client's own claimed coordinates back (and `GCMoveErrorHandler` feeds them into `SetServerPosition`, making a desync **permanent**), and stepping off the map edge threw `InvalidProtocolException` → `DisconnectException`, i.e. **a player on an edge tile walking outward was kicked** | `CGLearnSkillHandler.cpp:530`, `SkillHandlerManager.cpp:849`, `gameserver/Zone.cpp` | `89b2892` | compile-verified |
| 18-AF | The half-wired exchange subsystem made to **fail closed**. `moveItemToExchangeStorage` had both its `save` and its `deleteItem` commented out and returned `true`; `moveItemFromExchangeStorage` likewise transferred nothing; `claimItem` marked delivered without transferring and was infinitely repeatable. `ExchangeDB::markListingSold` **returned `true` unconditionally**, so its `AND Status = 0` compare-and-swap was decorative and two threads could both sell one listing. A single `isOperational()` gate (`ExchangeService.cpp:38`, used at `:170,:309,:501`) now refuses every entry point **before the first DB write on its path**; `markListingSold` returns `getAffectedRowCount() > 0` (`ExchangeDB.cpp:215`), making the CAS real. Both packet factories unregistered (`PacketFactoryManager.cpp:1193-1197`) after proving a fully closed call chain in the client | `gameserver/exchange/`, `Core/PacketFactoryManager.cpp` | `ec27e95` | compile-verified |
| 18-AG | 48 `Assert`-wrapped XML parse calls found across 18 files, 1 dead in a comment, **47 hoisted**. Under `NDEBUG` the call is `((void)0)` and the parse never happens — worse than "skipped": `GQuestGiveItemElement` would read item class and type from uninitialised locals, **minting an arbitrary item**; `GQuestCheckPoint` would spawn every checkpoint from stack garbage; `GQuestWarpElement` would send every warp to zone 0 at 0,0. Plus the **last 6 `(T&)` reference casts**, two genuinely out of bounds — `(int&)m_TargetZoneID` writes 4 bytes into a `WORD`, `(int&)m_Num` 4 into a `BYTE`; both were the last member of their class so the overspill landed in tail padding. Sweep of `(int&)`, `(DWORD&)`, `(WORD&)`, `(BYTE&)`, `reinterpret_cast<T&>` and a generic scalar-reference regex: **zero live sites remain** | `gameserver/GQuest*`, `ItemUtil` | `8a26163` | compile-verified |
| 18-AH | `Assert`-only accessors hardened — `Zone::getTile` (both overloads) and `getSector`, the four Slayer domain accessors, `Slayer::sendSlayerSkillInfo`'s stack-array write, `SkillDomainInfoManager`, `VampEXPInfo`, `OustersEXPInfo`. **Two are live OOB reads today, not `NDEBUG`-only.** (1) `SkillDomainInfoManager::getDomainInfo` has been out of bounds in the deployed Debug build all along: `init()` sizes each domain from its own DB rows but the guard was the constant `Level < 151`; measured against the shipped data, domains 0–4 hold 151, domains 6–7 hold 150, and **domain 5 (`ETC`) holds 101** — fifty short. Levelling ETC past 100 is an OOB pointer read the `Assert` then dereferences, and the destructor's `j <= 150` loop was freeing fifty slots past the end. (2) `ActionTeachSkill` is reachable **by walking into a vampire town**: it dispatches on the *player's* race rather than the NPC's, and live `Triggers` rows 672 (Kaim) and 727 (Bricolacas) carry `DomainType : VAMPIRE`, so a Slayer at a vampire trainer reached `m_GoalExp[6]` on a six-entry array. Fixed at **both** ends — hardening the accessor alone would have made a legitimate NPC conversation start throwing. Also a `y`/`sy` typo in `getSector`'s assert (guarding the wrong variable) and `SkillUtil`'s read-before-check loop with a `>` off-by-one | `gameserver/Zone.cpp`, `Slayer.cpp`, `SkillDomainInfoManager.cpp:118,153-175`, `SkillUtil.cpp` | `00a5372` | compile-verified |
| 18-AI | Two uninitialised `QuestID_t`s reaching `QuestManager::questRewarded()` — which is a bare `m_Quests.erase(qID)`, no membership check, no return value, so **garbage in erases an arbitrary in-progress quest**. `ActionGiveLotto.cpp:57` left `qID` uninitialised and `\|\|` short-circuited before the only writer on two of three paths. Its twin is worse: `CGLotterySelectHandler.cpp:63` calls `questRewarded(qID)` on **all three** paths and is reachable **directly from the packet path**. Intent was determined, not guessed — every sibling action either guards the erase on `successEventQuest()`'s return or never reads `qID`. Plus `ConditionEnterCastleDungeon.cpp:79` discarding `WarSystem::getAttackGuildID`'s return value (which writes its out-param only on `true`, and takes `m_MutexActiveWars` separately from the guard above it, so castle-dungeon entry could be decided against garbage), and 3 `Assert`-only castle zone IDs | `gameserver/quest/`, `CGLotterySelectHandler.cpp`, `ConditionEnterCastleDungeon.cpp` | `1177bd0` | compile-verified |
| 18-AJ | **41 SQL splices parameterised** — `ItemDestroyer.cpp` spliced `ownerID` into 41 quoted literals, and `ownerID` is attacker-controlled: `destroyAll`'s only call site passes `CLDeletePC::getName()`, copied verbatim off the socket with a client-supplied length byte, and the handler's ownership check happens **after** the name is already in a query. Semantics verified by extracting the `DELETE FROM <table>` sequence old and new: 41 vs 41, identical set and order. **Two amplification vectors closed**: `Datagram.cpp:174` wrote a filelog per rejected datagram and the receive loop added four `cerr` lines plus another filelog — two file open/write/close per ~11-byte spoofable UDP packet, ~36× by volume, now rate-limited 16 lines/60s with a suppressed count; `SocketInputStream.cpp:181` called `toString()` and `cout` on **every** packet received (`StringStream` is a `list<string>`, so a 10-byte `CGMove` cost ~17 allocations plus an unbounded write to `gameserver.out`), now behind `__TRACE_PACKETS__`. Plus **measurement-only** move-rate telemetry (§1 row 3) | `loginserver/ItemDestroyer.cpp`, `Core/Datagram.cpp`, `Core/SocketInputStream.cpp`, `gameserver/GamePlayer.cpp` | `72d392e` | compile-verified |
| 18-AK | **`fd_set` overflow made a refusal on all three servers.** `m_ReadFDs`/`m_WriteFDs` are glibc's default 1024-bit sets (`FD_SETSIZE` is never defined anywhere in this tree) but `nMaxPlayers` is 2000 and the accept paths did not bound against 1024 — descriptors 1024–1999 were accepted and `FD_SET` wrote out of bounds past `m_ReadFDs` into the adjacent `m_WriteFDs`. Crossing the limit was **memory corruption, not refusal**, on a login server internet-exposed on TCP 9999. All three accept paths now reject at `min(FD_SETSIZE, capacity)` before any `FD_SET`, close cleanly, and log once per rate-limited window (8 lines/60s, 18-AJ's pattern — an unbounded per-connection log would be its own amplifier). **The sharedserver was worse than the reported bug and had no check at all**: `addGameServerPlayer` writes `m_pGameServerPlayers[fd]` into a **100**-entry array, so descriptor ≥ 100 was an unconditional out-of-bounds pointer write — a bound ten times tighter. Mitigated only by 9977 not being in the Docker port list. No working capacity removed: nothing above 1023 ever worked | all three servers' accept paths, `sharedserver/GameServerManager.cpp` | `2e73f5c` | compile-verified |
| 18-AL | Four more **constant-guard-vs-DB-sized-array** defects. Method validated by first reproducing 18-AH's measured numbers exactly before being trusted on anything new. (1) **`EventQuestAdvance` is live, one instance per player**, and had the `reserve()`-for-`resize()` landmine 18-AH reported as merely latent — except compiled and running: `size()` stayed 0, so every `m_Advances[i]` indexed past the end and `clear()` iterated `begin()`…`end()` over nothing, **leaking every `EventQuestAdvance` ever allocated**; `getStatus`/`rewarded` take a client-supplied level via `CGLotterySelectHandler`. (2) `InfoClassManager::getItemInfo` had **no bounds check at all** — the only one was commented out, leaving `Assert(m_pItemInfos[itemType] != NULL)`, which **is** the out-of-bounds read because it indexes in order to test; `addItemInfo`'s `Assert` was the wrong dimension entirely (`ITEM_CLASS_MAX` = 90, an item *class* count, bounding an item *type* that reaches 75). (3) `ActionAskByQuestLevel` indexes `m_ScriptID[getQuestLevel()]` on a `ScriptID_t[5]` and `getQuestLevel()` returns **−1** once all five levels are advanced — a read *before* the array. (4) `PetTypeInfo`: `reserve()` again, and both bounds checked `capacity()` rather than `size()`; measured PetType 1..5 so slot 0 is never written, and `reserve()` does not value-initialise, so `getPetTypeInfo(0)` **passed its check and returned an indeterminate pointer** | `gameserver/EventQuestAdvance.*`, `InfoClassManager.cpp`, `quest/ActionAskByQuestLevel.cpp`, `PetTypeInfo.*` | `1f1ded5` | compile-verified |
| 18-AM | **Four Ousters NULL dereferences that crash the process.** A pointer initialised to NULL, assigned only inside a race chain that does not cover all three races, then dereferenced unconditionally — a SIGSEGV rather than a `Throwable` `__END_CATCH` could absorb, **so the gameserver dies, it does not log**. All four are the same Lua copy-paste. **Two are live in the shipped trigger data**: `ActionGiveAccountEventItem` (rows 26 Smirovich, 634 Ayman) and `ActionGiveTestServerReward` (rows 678 Kaim, 841 Smith, 1013 Stormov); `ActionGiveEventItem` and `ActionTradeGiftBox` are latent but any live-DB row arms them. What the Ousters path should do was proved *structurally*: the headers declare only Slayer and Vampire members, no `LuaTradeEventOustersItem` class exists anywhere, `read()` never parses an `OustersFilename`, and grep for that property across the whole repo returns nothing — the case is **unimplemented, not broken**. Each now returns the file's own failure reply plus `QUIT_DIALOGUE` and deliberately does **not** set `m_FlagSetType`, so the character is not burned if an Ousters script is ever added; Slayer and Vampire behaviour byte-for-byte unchanged. Also: `SkillUtil`'s 4-arg `computeDamage` returned 0 before assigning its `bCritical` out-param; the two `grade[0]` reads on a possibly-empty string that 18-AG hoisted the Asserts for; `GQuestCheckPoint`'s unbounded race index straight from XML; and `RareOptionUpgradeInfo` changed to `resize()` with Release-safe range guards | `gameserver/quest/`, `SkillUtil.cpp`, `GQuest*`, `OptionInfo.cpp` | `d70271e` | compile-verified |
| 18-AN | `[[noreturn]]` on both assert helpers (so GCC can prove the post-assert path non-NULL); six wild `delete`s; `LoginPlayer`'s `lastSlot` left uninitialised on the **normal** re-login path | `Assert.*`, `LoginPlayer.*` | wave 6 | compile-verified |
| 18-AO | A parser that validated a length byte **before reading it**; 41 format-string sites; a self-aliasing `sprintf`; an `m_HotKey[8]` off-by-one | various | wave 6 | compile-verified |
| 18-AP | A **remote sharedserver crash**, a **stack overflow** on guild-master transfer, UB reachable in every build config, and unbounded remote memory growth | `sharedserver/`, guild | `ca17d9a` | compile-verified |
| 18-AQ | **`checkedCast` introduced** — a `dynamic_cast` that raises `InvalidCastError` (derived from `AssertionError`, hence `Error`) instead of yielding NULL to a caller that dereferences it on the next line. Deriving it from anything in the `Exception` family would have been silently wrong: `__END_DEBUG_EX`, which closes handlers in **456 files** under `Core/`, swallows every non-`Error` `Exception` and returns normally. 250 sites converted, +24 in wave 8 = **274 of 1,930** (14.2%). Handlers invoked *outside* the packet boundary (`CGLogoutHandler`, `CGSayHandler`) deliberately excluded | `Core/CheckedCast.*` + 274 call sites | `91bdfbf`, `6e7cc2b` | compile-verified |
| 18-AR | **`reserve()`-for-`resize()`, instances 5 and 6 — closes the previous revision's §1 row 4 and open action 12.** `OptionInfoManager::m_OptionClassInfos` is live on the rare-enchant path (`getRareUpgradeRatio()` from `CGAddItemToItemHandler`, 3 sites): indexed writes into a `size()`-0 vector, a read of indeterminate memory that the `if (pOCI == NULL)` guard on the next line **cannot** detect, and a `begin()`…`end()` walk over an empty range leaking every `OptionClassInfo` ever loaded. `SkillPropertyManager::m_SkillProperties` was found while fixing it and is worse in kind — `init()`'s defensive NULL-fill of `[0, SKILL_MAX)` **was itself the out-of-bounds write**. Both → `assign(N, NULL)`; `clear()`/`toString()` rebounded on `size()` because both run from the destructor; `getOptionClassInfo()` now returns NULL out of range and `CGMixItemHandler` absorbs it, along with two `Assert`-only `front()`-on-empty-list checks it needed anyway | `gameserver/OptionInfo.*`, `SkillPropertyManager.cpp`, `Core/CGMixItemHandler.cpp` | `4fb9591` | compile-verified |
| 18-AS | **Two-byte stack overflow in `getDBString`**, the second byte attacker-supplied. Escaped into `char ret[100]` and checked the bound **after** both writes, so a 100-character nickname wrote `ret[100]` then the terminator at `ret[101]`; an escaped character consumes two slots, so 50 quotes reached the same place. Rewritten over `std::string` — the shape `SMSServiceThread`'s identically-named member already used correctly. **No callers left** (two stale `extern`s in `PetItem.cpp`, `NicknameBook.cpp`), which is why it survived | `Core/CGModifyNicknameHandler.cpp` | `d281882` | compile-verified |
| 18-AT | `PetInfo::PetInfo()` assigned `m_PetLevel = 0` **twice** and left `m_PetCreatureType`, `m_PetAttrLevel`, `m_PetFoodType` and `m_pPetItem` indeterminate — `write()` puts three of them on the wire, `getItemObjectID()` branches on the indeterminate pointer then dereferences it. Plus three NULL derefs: both pet-stash handlers dereferenced `getPetInfo()` unguarded (`CGDepositPetHandler`'s **very next statement** already tested it), and `CGThrowBombHandler` dereferenced `hasSkill()`'s NULL return — a SIGSEGV, not a `Throwable`, so the gameserver died rather than logged | `Core/PetInfo.cpp`, `CGDepositPetHandler.cpp`, `CGWithdrawPetHandler.cpp`, `CGThrowBombHandler.cpp` | `6ea0086` | compile-verified |
| 18-AU | **The UDP throughput ceiling that made §1 row 1 a no-bug DoS.** Both peer receive loops handled one datagram per pass around a 1ms sleep (the gameserver slept **twice**, ~500 dgram/s; the loginserver **once**, ~1000 — it was never the exact mirror the row claimed). **The socket is blocking** — `DatagramSocket` never calls `setsocketnonblocking_ex` and exposes no way to — so `receive()` already parks the thread and those sleeps guarded nothing; they were a pure throttle. Each pass now drains the queue, bounded at 256 datagrams so a flood cannot starve the periodic work, ending on `FIONREAD == 0` rather than a would-block return that a blocking socket never gives. The first `receive()` of a pass still blocks, so idle behaviour is unchanged. Removes the *logic* ceiling only: a line-rate flood still fills the kernel buffer, and one core can now spin consuming one — the intended trade against "nobody can log in" | `gameserver/LoginServerManager.cpp`, `loginserver/GameServerManager.cpp` | `c58ec35` | compile-verified |
| 18-AV | **`CGSilverCoatingHandler`'s missing Ousters arm**, §4's fifth race-chain instance. §1 row 6 said the cast returns NULL; it is a `checkedCast` since 18-AQ, so an Ousters got `InvalidCastError` → per-connection disconnect, and the second `else` would have charged an Ousters' gold through a `Vampire*` had the cast still been unchecked. Rebuilt to match `CGRequestRepairHandler::executeNormal`, its structural twin: three bools initialised `false`, three cast arms, three gold arms. An unrecognised race now matches no arm and falls into the existing `SILVER_COATING_FAIL_ITEM_NOT_EXIST` return. A sweep for a sixth instance came back **clean** | `Core/CGSilverCoatingHandler.cpp` | `2253645` | compile-verified |
| 18-AW | **Two startup loaders hardened against one bad DB row** (§1 row 12). `InfoClassManager::init()`'s unconditional `m_pItemInfos[i]` deref is guarded — and so is the `Assert(m_pItemInfos[0] != NULL)` one line later, which the row missed and which **indexes the array in order to test it**, faulting rather than catching (§4 pattern 2 again); slot 0 is a real invariant, so it degrades to `m_TotalRatio = 0` and `getRandomItemType()`'s existing early return. `SkillParentInfoManager`'s destructor freed the pointer array but never its elements; and `load()`'s `tempSkillType` cursor seeded with 0 made a `SkillType` 0 row deref the still-NULL slot 0. Keying off the slot also drops an unstated assumption that rows arrive grouped — **the query has no `ORDER BY`**, so non-adjacent duplicates previously threw `DuplicatedException` and leaked | `gameserver/InfoClassManager.cpp`, `SkillParentInfo.cpp` | `b2e7d48` | compile-verified |
| 18-AX | **`memoryError()` → `abort()`** in login and shared (§1 row 15). The row omitted that the **gameserver already did this correctly**, so it was inconsistency, not a uniform bug; both now match its body verbatim. `abort()` is specifically right for a `new_handler`: one that cannot free memory must not return, and unlike `exit()` it skips `atexit` handlers and static destructors that would allocate on an exhausted heap. Yields SIGABRT, which a supervisor reads as a crash | `loginserver/main.cpp`, `sharedserver/main.cpp` | `0e8e489` | compile-verified |
| 18-AY | **The login-failure counter now counts** (§1 row 4). All four sites stored the value they read; each now stores the incremented one, and the **reset on success — missing entirely — is added at three places**. Deliberately *not* placed at "credentials verified", because `CLLoginHandler.cpp:482` counts failures occurring *after* verification. Two corrections to the row: the two `CLLoginHandler` sites **never** had an increment in this repo's history (`git log -S'++nFailed'` on that file is empty — upstream shipped it half broken), and the two register sites are dead code on this build (`PACKET_CL_REGISTER_PLAYER` is a closed state loop with no entry point). **This is not brute-force protection — see §1 row 16** | `Core/CLLoginHandler.cpp`, `CLRegisterPlayerHandler.cpp` | `fd98c81` | compile-verified |
| 18-AZ | **`UserGateway` deleted — the `reserve()` gate's first catch, and closed by deletion rather than repair.** Instance 7 of §4 pattern 4: `clear()` reserved 20 then wrote `m_UserInfos[i] = 0` across `[0,20)` into a `size()`-0 vector, with four readers indexing it and `clear()` called from the constructor. The death certificate is stronger than §1 row 17's build-system argument: **the file cannot compile** — the header declares six members with no exception specification while the `.cpp` defines all six `throw(Error)`, ill-formed in C++11. The test file went with it (absent even from the legacy `test/Makefile` `OBJS` list that names its seven siblings). Retirement was **deliberate, not rot** — ~40 call sites commented out across four files *including the `#include` lines*, signed `// by sigi. 2002.12.30`, and predating this repository | `server/UserGateway.*`, `gameserver/test/GameServerUserGatewayTest.*` | `dbddf7b` | compile-verified |
| 18-BA | **`quest/Squest/` deleted — 29 files, not the 13 §1 row 18 named.** Beyond the 13 `.cpp`/`.h` pairs: two headers with no `.cpp` (the real risk, since a header can be included by live code — both had zero outside includes), a `Makefile`, a `ResultBug.log` and a `vssver.scc`. Three references looked live and were each run to ground: `Zone.cpp:19`'s **unqualified** `#include "QuestManager.h"` resolves to `mission/` (the directory is on no include path — `GAMESERVER_INCLUDE_DIRS` lists `mission`, `quest`, `quest/luaScript`, never `quest/Squest`); the `PlayerCreature.cpp` and `NPC.cpp` uses sit inside block comments **and** behind `#ifdef __ACTIVE_QUEST__`, defined nowhere. The two `QuestManager` classes share the class name **and** the include guard `__QUEST_MANAGER_H__`, so one would silently suppress the other. Genuine successor: `mission/`'s takes a `PlayerCreature*` owner and carries DB `load()`, packets and rewards; Squest's is an owner-less `list<Quest*>`. `quest/Makefile:219`'s uncommented `$(MAKE) -C Squest clean` was the one dangling reference and is commented to match its siblings | `gameserver/quest/Squest/` | `229340d` | compile-verified |
| 18-BB | **`nMaxPlayers` 2000 → 1024** (§1 row 5). The row asked for a two-line test before acting; it was run. On glibc 2.39 / g++ 13.3.0, `#define FD_SETSIZE 4096` before `<sys/select.h>` leaves `FD_SETSIZE` at 1024 and `sizeof(fd_set)` at 128 bytes — byte-identical to the control. glibc `#undef`s the user value and re-derives from `__FD_SETSIZE`, hard-wired in `bits/typesizes.h`. **Confirmed empirically rather than by citation.** All 28 readers checked first: every index is an accepted file descriptor, with no config bound, DB column or printed capacity, so the value was free to change. 1024 not 1000, because the array is fd-indexed and a lower bound would refuse descriptors `select()` can represent; the ~1000 usable ceiling is now stated rather than implied. A guarded `static_assert` ties the two together | `server/PlayerManager.h` | `c7c5728` | compile-verified |
| 18-BC | **An absolute pre-authentication deadline** (open action 15). The gap was not a missing deadline — `maxIdleSec` = 900s already existed, but it **slides**, refreshed by every accepted packet, and `LPS_BEGIN_SESSION` accepts `CL_VERSION_CHECK` and `CG_ENCODE_KEY`, so **one cheap packet every 14 minutes parked a descriptor forever**. The 900s was never the problem; its refresh was. The new deadline is armed at accept, never refreshed, and consulted only in `LPS_NONE`/`LPS_BEGIN_SESSION` — leaving that state *is* "credentials accepted", so a logged-in session is unaffected. Checked ahead of the `LPS_WAITING_FOR_GL_KICK_VERIFY` early return so it cannot be skipped, and `processCommands()` walks every player rather than only readable sockets, so a silent connection is still evaluated. 30s chosen against measured client behaviour (socket opened *after* credentials are typed; `CLLogin` sub-second; failure reconnects) — ~30× headroom, 30× tighter than the 900s it backstops. Cost to hold 1000 slots rises ~1.1 → ~34 conn/s. Tunable via optional `MaxPreAuthIdleSeconds` | `loginserver/LoginPlayer.*`, `conf/loginserver.conf.template` | `b3e9322` | compile-verified |
| 18-BD | **The trade swap is atomic, and cancelled trades no longer strand gold** (§1 row 11, which was wrong on both counts). The item-loss path was **reachable**, not gated: `canTrade` returns 0/1/**2** and the guard was `if (!canTrade(...))`, so `2` — the one verdict the swap cannot honour — passed through, kept out only by `CGTradeFinishHandler` separately testing `== 1`; and `canTrade` never calls `getBlackGiftBoxType`, which `processTrade` called mid-placement. Now three passes — inspect, detach, place — with rollback restoring exact original slots newest-first, the original `throw` preserved, and a commit point separating all non-undoable work. Callback order is unchanged, and the reordering is safe because no callback reads the inventory (`Item::whenPCLost` touches the Store listing; `PetItem`'s touches `petInfo`/`getPetItems()`/`HAS_PET`). **Items are never escrowed** — only pointers are recorded — so what `cancelTrade` stranded was **gold**; the refund is now unconditional via in-zone → `g_pPCFinder` → a direct `Gold` `UPDATE`, the same mechanism `GSQuitGuildHandler.cpp:217` uses for absent guild members. **Superseded by §1 row 19** | `gameserver/TradeManager.cpp` | `f33e12a` | compile-verified |
| 18-BE | **The trade escrow is cleared before the logging tail, not after it** (§1 row 19). `processTrade` credited both sides with `setGoldEx` (which persists), then ran trace logging and a `TradeLog` `INSERT`, and cleared `TradeInfo::m_Gold` only at the very end. Every line of that tail can throw — the `getSocket()->getHost()` dereference is unguarded and `END_DB` rethrows a failed insert — and any throw left the escrow populated, so the following disconnect called `cancelTrade()` and credited the same gold **a second time**. `clearAll()` now runs immediately after the credit. Safe to move, checked not assumed: `tradeGold1`/`tradeGold2` are copied into locals ~240 lines earlier and nothing between the credit and the old position reads `pInfo1`/`pInfo2`. Also disposes of the NULL-socket variant. **Owner-approved fix shape** | `gameserver/TradeManager.cpp` | `0b8f560` | compile-verified |
| 18-BF | **A cross-connection brute-force rate limit** (§1 row 16), owner-approved as a new control. Two fixed tables — by source address and by account — of `Entry[4096]` at 48 bytes, **384 KiB total, allocated with the object and never grown**, because both key spaces are attacker-chosen and anything allocating per distinct key would itself be the memory-exhaustion primitive. Open addressing, 8-slot probe; a full window **fails open** (failing closed would let an attacker deny login to everyone by stuffing the table), with eviction by fewest-failures/oldest-window so a tripped record is the hardest thing to displace and the ~7N failures needed to evict it are counted by the *other* table. 30/300s per IP, 10/600s per account, bucketed windows never extended by traffic. **Only wrong-credential failures count** — `ALREADY_CONNECTED` above all is excluded, since it means the password was *right* and is the stuck-in-`LOGON` retry loop; that exclusion is what makes a loose per-IP threshold safe on a household, PC-bang or CGNAT gateway. Success clears the account record but deliberately **not** the IP record (registering an account is free, so an attacker could otherwise reset their own budget). Residuals recorded in the commit: targeted lockout DoS is inherent to any per-account limiter, and spraying is metered only per-IP | `loginserver/LoginThrottle.*`, `Core/CLLoginHandler.cpp`, `conf/loginserver.conf.template` | `b236e70` | **boot-verified** (conf keys parsed, §5 event 5) |

**Unnumbered, same effort:**

| What | Commit | Verification |
|---|---|---|
| `Player::setKey`: uninitialised `pHashTable` read + 512-byte leak per `CGConnectSetKey` | `5b8a259` | runtime-verified (login path, ASan-clean run) |
| **UBSan CI leg links for the first time in the project's history.** `build (ubsan)` had failed 17-for-17, always at *link* time: `-fsanitize=undefined` implies `vptr`, which emits `typeinfo for T` references from three sharedserver call sites into `GameServerPackets`, which sharedserver deliberately does not link. One flag, `-fno-sanitize=vptr`, appended **after** `-fsanitize=undefined` (order load-bearing; `CMakeLists.txt:35`'s plain `set()` overrides the cache). Boot job became a matrix over `asan`/`ubsan` | `8bb1118` | runtime-verified **locally** — `make debug-ubsan` links, exit 0, all three binaries. Leg stays non-blocking; see §5 |
| Client `CGExchangeBuy` matched to the hardened server format: 8-byte listing ID, 1-byte length prefix rejected above 64, `getPacketMaxSize()` 73 both sides. The divergence was wider than first described — the listing ID *width* was wrong too (4 client vs 8 server) and the client truncated an already-`int64` `listingID` at the call site | `b9fac7d`, fmt `efe35f8` | **not locally compile-verified** — this workstation cannot build the client; CI is the only possible gate |
| Secret untracking + credential sweep; Docker parameterisation; seed dump repair | `c587490`, `0bdc648`, `3dcfb1b`, `430bcc9`, `b86e20e` | see §7 |
| CI: branch allowlist removed, changed-lines `clang-format`, SQL ratchet rewritten, SQL literal lint, ASan boot job | `0733065`, `35b11b2`, `cc1fcd4` | see §6 |
| CI: `-O2` warning-scan leg (informational, uncached, no baseline, cannot fail) + **`paths:` blind spot fixed** — neither workflow's filter included `shared/Packets/**`, which `dkrixserver/CMakeLists.txt` compiles into all three servers and `dkrix/CMakeLists.txt` absorbs into the client, so a change confined to that directory triggered **no CI on either tree** | `ff62f9a` | see §6 |

---

## 3. Investigated and REFUTED, or corrected

**The highest-value section in this document.** It exists so nobody re-chases a
lead already found false, and so a claim that was acted on before being checked
is recorded as wrong rather than quietly dropped. **Rows are never deleted.**

### 3.1 Claims investigated and refuted

| Claim believed | What is actually true | Evidence |
|---|---|---|
| `gameserver/GuildManager.cpp`'s `init()` is inside `#ifdef __SHARED_SERVER__` but the file is only ever built as `__GAME_SERVER__`, so guild init silently compiles to nothing | **The premise conflates two different files.** There are two `GuildManager.cpp` — `server/gameserver/` (787 lines, built into `gameserver`) and `server/sharedserver/` (744 lines, built into `sharedserver` per `sharedserver/CMakeLists.txt:6,22`), with different content. The sharedserver copy carries the same `#ifdef` and **is** active there, called from `SharedServer.cpp:123`. Guilds *are* loaded from the DB — on the sharedserver. The gameserver's is a **replicated cache, not a DB reader**: `GSRequestGuildInfo` → `GSRequestGuildInfoHandler.cpp:43` `makeSGGuildInfo()` → `SGGuildInfo` → `SGGuildInfoHandler.cpp:40,85` `clear_NOBLOCKED()` + `addGuild_NOBLOCKED()`, with incremental updates via `GSAddGuildHandler`/`SGAddGuildOKHandler`. The packet pair is linked into **both** `GameServerPackets` and `SharedServerPackets` (`Core/CMakeLists.txt:744-753, 819-823`) and each library's include path lists its own server dir first, so each side binds the right class. So `ObjectManager.cpp:541`'s `init()` compiling to nothing is **intentional** — a gameserver reading `GuildInfo` directly would fight the sharedserver's authoritative copy | `a8503bf` [measured 2026-08-13]. Residual is cosmetic only: the gameserver copy carries four `#ifdef __SHARED_SERVER__` blocks duplicating the sharedserver file, where a typo would be invisible |
| `Assert` is compiled out of the shipping build, so every `Assert`-based bounds check is a live exploit | **The deployed build is Debug and `Assert` throws.** `Makefile:11` is `all: debug`; `server.yml` uses `make debug`; no build tree sets `NDEBUG`; `bin/gameserver` still contains the stringified assert expressions | `MOD` packet audit [measured]. **Root cause: a wrong line in `dkrixserver/CLAUDE.md` documenting `make` as Release — it misled two independent security reviews.** Corrected in `a29ee09` |
| The live DB password is in published history (repo public since 2026-08-08) | **It was not.** The committed value was a stale 7-character credential; the live 9-character one existed only as an uncommitted working-tree modification. Any `git add -A` would have published it | `c587490`; swept `430bcc9`; history rewrite still judged unwarranted |
| Legacy `Statement`: a `const char*` from `getField()` is invalidated by `next()` | **No such hazard.** `Statement::executeQuery()` uses `mysql_store_result` (`Statement.cpp:116`), which buffers the whole set client-side. The claim would hold only under `mysql_use_result`, never called here | `MOD` — explicitly **WITHDRAWN 2026-08-10** |
| Turning on an ASan job in CI would have caught 18-B | **A build-only ASan job catches neither 18-A nor 18-B.** ASan reports at runtime; CI never logged in | `6e82736`; boot job landed `0733065`, first passed `cc1fcd4` |
| 203 `Result*` sites share the bug shape → 203 candidate bugs | Shape count, not bug count. 129 files / 200 assignment sites audited → **1** live bug (18-C). `Result* p = stmt.execute();` is **safe by construction** (359 sites); only the 25 pre-declared `Result*` can break it. Tree-wide UAF candidates on a brace-accurate scan = **0** | `MOD` audit table; `192ccfe`, `65c07ad` |
| The 18-B crash was in `Statement::executeQuery()` | Wrong function. The IP offset was computed against `dmesg`'s mapped-segment base, not the ELF load base — off by `0x10000` | `MOD` correction block |
| `CLCreatePC::toString()` is never called | It is — `LoginPlayer.cpp:217` → `readPacket` → `toString` | `MOD` verification round |
| A third loginserver segfault means something is still crashing | Not attributable and **not live** — the instance that ran the end-to-end session started 12:22:12 and stayed up | `e362ca2` |
| The Phase 11 migration probably broke SQL/binds too | **Clean.** All 1,575 `PreparedStatement` constructions across 208 files audited: 0 defects. The damage was object *lifetime* (18-B/C) and one malformed literal (18-A). Independently reproduced by machine over 1,572 literals | `655ade7`, `0733065` |
| ~487 legacy `executeQuery` sites remain | Roughly half are inside `/* … */`. 489 raw hits → 261 live after stripping comments (1.87× inflation) → **172 live `createStatement()` calls** | `65c07ad` [measured] |
| The SQL-injection ratchet reporting **0** meant there was no injection surface | The gate was **broken and frozen green**. Its regex forbade any `)` between the call paren and the format specifier; it counted lines not call sites, ignored comments, was single-line only, and its unanchored `Statement` alternative matched `PreparedStatement(` — so *migrating toward the safe form would have raised the count*. `--update`d to a baseline of 0 while broken. True count on a correct metric: **258** | `35b11b2`, `0733065` |
| The seed dump `initdb/DARKEDEN.sql` is a working reference database | **It had never been loadable.** Verified byte-level as genuine data loss, not a decode artifact. The first repair (six `INSERT IGNORE`s) was itself wrong — it suppressed symptoms | `b86e20e` (superseded), `7270626`, `cc1fcd4` |
| The Docker stack worked | Broken and unrun since `a894d7c`: `odk-server` had **no `DKRIX_*` variables set at all**, so `Properties::expandEnvVars` left placeholders literal. No CI job exercised the compose path, so nothing went red | `3dcfb1b` |
| `PacketValidator`'s `reserve()` sizing bug is worth fixing | **Dead code server-side**: `PLAYER_STATUS_MAX` is 0 in all three binaries because its enum body is behind `#if defined(__GAME_CLIENT__)` | `35b11b2` |
| The *GGCommand UDP gate* is unsourced — "get a source before acting on it" | **It was real, it was the most severe finding of the entire effort, and it is now fixed.** The first revision recorded it as untraceable because neither `MOD` nor the then-21 commits mentioned it; that was a gap in the *record*, not evidence of absence | `4d92150` (18-AB) |
| Raising `FD_SETSIZE` would let the servers use their full 2000-player capacity | **It does nothing on glibc.** `fd_set` is sized by `__FD_SETSIZE`, fixed at 1024 in `bits/typesizes.h`; `FD_SETSIZE` is an alias and defining it before the include is silently ignored. It *does* work that way on Winsock, which is probably why the current state looks deliberate. **Not compile-confirmed on the CI glibc** — worth a two-line test before anyone acts on it | `2e73f5c` |
| `FameLimitInfo` is the highest-value remaining follow-up — the closest twin of the `getDomainInfo` defect | **It is uncompiled dead code.** 0 hits in any `CMakeLists.txt` (versus 20 each for its live siblings `SkillDomainInfoManager` and `SkillParentInfo`), every wiring point in `ObjectManager` commented out, zero call sites, and **the table exists in neither dump**, so `load()` would fail on its first `SELECT`. It does carry the identical latent defects. Left untouched deliberately: editing an uncompiled, unverifiable file only signals to the next auditor that it matters. **Recommend deleting `FameLimitInfo.{h,cpp}`** | `1f1ded5`; re-verified 2026-08-11: `grep -rl FameLimitInfo --include=CMakeLists.txt` → no match |
| `EventBallInfo`'s manager is never constructed while `EventBall.cpp` calls through it — a live NULL deref | **True but harmless: `EventBall.cpp` is not compiled either.** It is absent from `dkrixserver/src/server/gameserver/item/CMakeLists.txt` (107 lines, explicit source list, no glob). `g_pEventBallInfoManager` is declared `NULL` at `EventBall.cpp:20` and assigned nowhere, and `:174/:184/:194` dereference it — but no translation unit builds them. Same class as `FameLimitInfo`; **recommend deleting the pair rather than fixing them** | `1f1ded5` reported it as live; corrected here 2026-08-11 by measurement |
| `SkillParentInfo`, `RankEXPInfo`, `AttrBalanceInfo`, `MonsterAI`, `War.cpp`'s `Status`, `EffectSchedule`'s `WorkCode`, `SweeperSet`'s `OwnerRace`, `RegenZoneManager` share the constant-guard defect | **Measured negatives, all of them.** The first three already bound on their loaded counts (105+1, 50+1, 315+1); `MonsterAI`'s 38 slots are all assigned and its input validated upstream; `War.cpp`'s `Status` is an SQL `enum('WAIT','START','END','CANCEL')` and cannot drift; `EffectSchedule`'s `WorkCode` is `==`-compared, never an index; `SweeperSet`'s `OwnerRace` has exactly one distinct value; `RegenZoneManager`'s table is absent and its `Owner` is never an index | `1f1ded5` [measured] |
| ↳ **partially corrected 2026-08-13.** Two of that row's three "already bound on their loaded counts" entries are dead on a second and stronger ground: **`RankEXPInfo.cpp` and `AttrBalanceInfo.cpp` are in no CMake source list**, with every `ObjectManager` wiring line commented out. The negative verdict stands; the reasoning given for it was more generous than the code deserved. `SkillParentInfo` *is* compiled, and wave 9 found two real defects in it anyway (18-AW) — the row was answering a different question (constant guards) than the one that mattered there | `a8503bf` [measured] |
| The money/repair handlers, `TradeManager`, and the `item/*.cpp` cluster share 18-AM's race-chain crash | **Verified false positives.** The money and repair handlers end their chains in `throw`; `TradeManager` likewise; the ~79 chains across `item/*.cpp` all terminate in `throw UnsupportedError`, so an Ousters gets a caught exception logged to `itemLoadError.txt` rather than a crash | `d70271e` [measured] |
| Combat might not be authoritative | **It is.** The client sends intent only; no damage, hit, crit, EXP or cooldown value exists on any `CG` packet in the path — all **216** `CG` definitions grepped for `Damage_t`/`Exp_t`/`HP_t`/`Level_t`. `CGAttack` even carries X/Y/Dir that the handler ignores in favour of the server's position. Skill ownership, mana, cooldown and range are recomputed server-side; cooldown stamped by `setRunTime` rather than trusted. Item use verifies the item's real `ObjectID` against the claimed one | `89b2892` [measured] |
| Movement geometry might be exploitable as a teleport | **It is not.** `CGMove` is a *direction from a verified position*: the client sends its believed current position plus a direction, the server requires that to equal its own and derives the destination itself. `verifyDistance` is applied at ~200 call sites; portals and waypoints validate against server-side tables; 18-O's reach check covers all three ground-pickup paths. The exploitable part is *rate*, not geometry — §1 row 3 | `89b2892` [measured] |
| The item economy's core paths need auditing | **Audited and cleared**: player trade, NPC buy/sell, personal stores, ground pickup, container moves, stack arithmetic, quest reward claims | `MOD` "waves 2 and 3" |

### 3.2 Corrections — claims that were believed and acted on before being checked

These matter more than the additions. Each was stated as fact, in a commit
message or a doc, and was wrong.

| Stated | Correction | Evidence |
|---|---|---|
| "The three unchecked `getShopItem()` dereferences remain open" (`326c298`'s own commit message, and a `MOD` paragraph) | **There are four, not three, and all four are guarded.** `:106` by `isExistShopItem()` returning early at `:98`, eleven lines above; `:194`, `:466` and `:666` sit in `executeNormal`/`executeMotorcycle`/`executeEvent`, whose **only** call sites tree-wide are `:132`/`:130`/`:155` inside `execute()`, downstream of the same guards at `:98` and `:143`. The underlying hazard is real — `ItemRack::get()` returns `m_ppItem[index]` and a NULL slot is *normal* — so throwing from the accessors remains correct. It is the "still open" claim that was wrong | `36727b4`; `MOD` "REFUTED 2026-08-10" [measured: three call sites, no others] |
| 18-H's `read(m_IdempotencyKey)` is "the only such site tree-wide" — **published as `[measured]`** | **False.** `Core/GCExchangeBuy.cpp:23` has the identical unbounded one-argument `read()` on a `std::string`, with `write()` likewise emitting it unprefixed | `b9fac7d`; verified in-tree. Now §1 row 9 |
| 18-T's fix covered `CLDeletePCHandler.cpp:73-129` | **It did not.** That handler takes a **`CLDeletePC`** packet — a different class whose `read()` was never touched. Its six `Slot2String[getSlot()]` SQL binds stayed driven by a raw 0–255 wire `BYTE` against a 3-element table, reachable from `LPS_PC_MANAGEMENT` by any authenticated account | `6f45414` (18-AC) fixed it |
| **The idle timeout cannot fire on a silent connection**, because the check lives inside `processInput`, which only runs for readable sockets — relayed as a reason the `fd_set` overflow was cheap to reach | **Wrong. It fires on both servers.** The check is in `processCommand` (`gameserver/GamePlayer.cpp:427`), driven by `processCommands()` — a **separate loop** iterating `m_MinFD`…`m_MaxFD` with **no `FD_ISSET` filter** (`ZonePlayerManager.cpp:415`; the file's only three `FD_ISSET` uses are at `:301`, `:683`, `:774`, none in that loop) — and `m_ExpireTime` is armed in the **constructor** (`GamePlayer.cpp:155-156`), so a client that never sends a byte has a deadline from the moment of accept. **300s on the gameserver, 900s on the login server** (`GamePlayer.cpp:66` = `60*5`, `LoginPlayer.cpp:32` = `60*15`). No sweep is needed; the hook exists and is unconditional. The residual is *tuning*: a connection can still be parked for the full window, and the right lever is a much shorter **pre-authentication** deadline cleared once the player leaves `LPS_BEGIN_SESSION` — which must be evaluated **ahead of** `processCommand`'s early return for `LPS_WAITING_FOR_GL_KICK_VERIFY` or it will be skipped in that state | `2e73f5c`; re-verified in-tree 2026-08-11 |
| `RareOptionUpgradeInfo` has zero callers — dead code, a landmine rather than a live bug (18-AH) | **Wrong. It is live on the rare-enchant path.** `OptionInfo.cpp:607-612` loads `RareEnchantInfo` at startup and **throws if the table is empty**; the seed ships 40 rows; `getRareUpgradeRatio()` is read from `CGAddItemToItemHandler` at three sites. Severity was also overstated in the other direction: `reserve(grade)` does allocate capacity for `0..grade-1`, so with shipped data the writes stay inside the allocation — UB with `size()` 0, not present corruption, though one `Grade == TotalGrade` row would write one past | `d70271e`; `OptionInfo.cpp:1011-1020` |
| `FameLimitInfo` is the highest-value follow-up | Uncompiled dead code — see §3.1 | `1f1ded5` |
| `EventBallInfo` is a live NULL-deref | Uncompiled dead code — see §3.1 | measured 2026-08-11 |
| **PR #2 was squash-merged, so `main` carries one squashed commit rather than the 42 individual ones, and `git branch --contains` misleads** (`MOD`, "Phase 18 — waves 2 and 3", process note) | **Both PRs were true merge commits, not squashes.** `669a9fe` has parents `9422e9c` + `d9b6af2`; `90a926e` has parents `669a9fe` + `fc7fae7`. The individual commits **are** reachable from `main` — `git merge-base --is-ancestor a29ee09 origin/main` and the same for `4d92150` and `fc7fae7` all return true. Containment is therefore reliable here. The *other* half of that note is correct: wave 2's commits, including 18-AB, were not on `main` until PR #3 | measured 2026-08-11: `git log -1 --format=%P` on both merges. **Belongs in `MOD`; this document does not own it** |
| 18-AB "is not on `main`" and "merging the 7-commit tail is the highest-value action on this page" (this document, previous revision, §2 row 1 and §5 action 7) | **Superseded by PR #3, `90a926e`, 2026-08-11 00:30 UTC.** 18-AB is on `main`. The highest-value operational action is now merging **waves 4 and 5** — §1 row 1 | measured 2026-08-11 |
| The recorded SQL ratchet baseline was `format=70 stream=24 splice=164` | **Already stale when 18-AJ measured it**: `HEAD` alone measured `format=67 stream=23` — earlier work had reduced them without ratcheting down. Only the `splice` 164 → 123 drop is attributable to 18-AJ. Current baseline, read from the file: `format=67 stream=23 splice=123 total=213` | `72d392e`; `dkrixserver/scripts/.sql-injection-baseline` |
| 18-AA's fixes make the UBSan reports go away | **Not claimed and not verified.** `9cfe522` says in terms: "Whether these reports are now absent needs a re-run under rebuilt UBSan binaries." See §5 | `9cfe522` |

---

## 4. Failure patterns this effort keeps finding

Standing section. These predict where the next bug is; check them first on any
new file. Each carries its instance count as of 2026-08-11.

**1. Controls that exist but do nothing — 4 instances, and one of them survived
being fixed.** The login-failure lockout never incremented because `++nFailed`
lived inside a commented-out `cout`. 18-AY made it increment — and it *still*
does not protect anything, because the counter is per-TCP-connection and the
stock client reconnects for every attempt (§1 row 16). **Corollary to the rule
below: fixing the mechanism is not the same as obtaining the control. Ask what
the attacker has to do differently now, and if the answer is "nothing", the
control is still absent.** The `CGVerifyTime` speed gate is dead three ways over (§1 row 2). The
SQL-injection ratchet reported `0` while 188 sites were invisible to it. The
`sanitizers` matrix was build-only, so it could not see the class it appeared to
cover. **Rule: when a gate here reports clean, check that it CAN report anything
else.**

**2. `Assert` as the only bounds check.** `Assert` throws in this tree's Debug
build, so these are not all NDEBUG-only theory — but `Assert(m_pItemInfos[t] !=
NULL)` **is** the out-of-bounds read, because it indexes in order to test
(18-AL). Instances: 18-K (5), 18-U (25), 18-AD (6), 18-AG (47), 18-AH, 18-AL,
18-AI (3 castle IDs). Residual scoped and recorded at §1 row 10.

**One sub-shape of this is now gated** — `f73767b`,
`scripts/check-guard-index.sh`, baseline **25**. It counts guards that
subscript a container *in order to test whether the element exists*, which is
the question that cannot be asked without already being in bounds. Validated
the same way the `reserve()` gate was: counts across the pre-fix trees run
43 → 42 → 41 → 41 → 41 → 34 → 34 → 27 → 25, and **all 18 drops are accounted
for** by a named fix or a file deletion, none by the checker changing its mind.

**Read the scope limit before trusting it.** 18-K, 18-AD, 18-AG and 18-AI show
*no movement*, correctly: they are this pattern's other sub-shapes —
side-effecting calls inside `Assert`, and `Assert(index < MAX)` comparisons —
and 18-U's other conversions (`Zone::getTile`, the NPC shop rack, `Datagram`,
the Slayer phone accessors) contain no `Assert` with a subscript at all. **The
gate covers one sub-shape of pattern 2, not pattern 2.** Two further limits are
written into the script: a *written* bound is taken at face value and never
checked for correctness (18-AH's `j <= 150` over arrays of 101 was a bound, and
wrong — that is pattern 3), and **another `Assert` is never accepted as a
bound**, since under NDEBUG both vanish together. `TimeChecker.cpp:118` is the
cleanest illustration: `Assert(type >= 0 && type < TIME_PERIOD_MAX)` sits
directly above the site.

Two of the 25 look like real defects rather than deliberate residue and are now
§1 rows 22 and 23 — including a wear-slot write that **18-R's own commit message
claimed was the last one**.

**3. A constant guard against a DB-sized array.** A container is sized from its
own DB rows at load, but access is bounded by a hardcoded constant that was true
of some other table. 18-AH's `SkillDomainInfoManager` (guard `< 151`, domain 5
holds 101) and `ActionTeachSkill` (`m_GoalExp[6]`, six entries); 18-AL's
`InfoClassManager` (`ITEM_CLASS_MAX` = 90 bounding an item *type* reaching 75),
`EventQuestAdvance`, `ActionAskByQuestLevel` (`getQuestLevel()` returns −1),
`PetTypeInfo`. **The method that finds these** — validated by first reproducing
18-AH's measured domain counts exactly before trusting it on anything new — is:
find the `load()` that sizes the container, then diff its real row counts against
every constant used to bound it.

**4. `reserve()` where `resize()` was meant — 7 instances, and now gated.**
Nastier than it
looks: `reserve()` *allocates*, so writes usually land inside the allocation and
the code appears to work, while `size()` stays 0 — which silently empties every
range-based loop, `clear()` and iteration over the container. That is how
`EventQuestAdvance` leaked every object it ever allocated. And `reserve()` does
not value-initialise, so a NULL check on a "reserved" slot tests indeterminate
memory. Named instances: `RareOptionUpgradeInfo`, `EventQuestAdvance`,
`PetTypeInfo`, plus `OptionInfoManager::m_OptionClassInfos` and
`SkillPropertyManager::m_SkillProperties` — **all six now fixed** (18-AR). The
same family had already appeared as 18-J, 18-P (3 sites) and 18-Q under the
label "`reserve()` without `assign()`".

**Five of the six were found *after* the pattern was named here**, which was the
argument for a mechanical gate. **That gate now exists** — `821d5e5`,
`scripts/check-reserve-sizing.sh`, baseline 2. Run over the tree as it stood
before the first fix (`3e02f6c^`) it reports 17 sites and **every one of the ten
historical call sites is among them**, with no false positives against the
tree's 14 correct `reserve()` uses. Ten by hand across five waves; all ten in
1.1 seconds by machine.

**It found instance 7 on its first run** — `UserGateway::clear()`, §1 row 17 —
which is the outcome the pattern predicted and the reason to keep the gate
rather than treat the family as closed.

The `SkillPropertyManager` instance is still the sharpest illustration of why
the family is hard to see by reading — its `init()` NULL-filled `[0, SKILL_MAX)`
through `operator[]`, so **the defensive fill was itself the out-of-bounds
write**.

**5. An incomplete race chain, then an unconditional dereference — 5
instances.** A pointer initialised to NULL, assigned only inside a
Slayer/Vampire chain that has no Ousters arm, then dereferenced. It is a SIGSEGV,
not a `Throwable` that `__END_CATCH` absorbs, **so the gameserver dies without
logging**. 18-AM fixed four (`ActionGiveAccountEventItem`,
`ActionGiveTestServerReward`, `ActionGiveEventItem`, `ActionTradeGiftBox`); the
fifth, `CGSilverCoatingHandler.cpp:39`, is open (§1 row 6). Verified false
positives are recorded in §3.1 so they are not re-swept.

**6. Gates that report clean because they are structurally blind — 4
instances.** The SQL-injection ratchet (regex could not match the dominant call
shape). The build-only sanitizer matrix (never executed a server). The branch
allowlist (hid a whole branch's commits from CI — count disputed, §6). The
`paths:` filter that omitted
`shared/Packets/**`, which compiles into all three servers *and* the client — so
a change confined to that directory triggered no CI on either tree, and it is the
one tree this workstation cannot build locally. Overlaps pattern 1 by
construction; kept separate because the fix is different — pattern 1 is a missing
statement, this is a missing *input*.

**Measured root cause behind a whole class of these** [measured, `1177bd0`]:
**`-Wmaybe-uninitialized` cannot fire in any configuration this project builds.**
`CMAKE_CXX_FLAGS_DEBUG` is `-g` with **no `-O` flag**, and GCC's uninitialized
analysis is a dataflow pass that only runs under optimisation; `debug`,
`debug-asan`, `debug-tsan` and `debug-ubsan` are all `CMAKE_BUILD_TYPE=Debug`,
and CI builds nothing else. Corroborated against a real build log: 31 warnings,
**zero** containing "uninitialized". **Six bugs on this branch share that root —
18-F, 18-L, 18-M(b), 18-W, 18-AA, 18-AI — every one found by reading code,
because the compiler was structurally unable to speak.** `ff62f9a` added an
informational `-O2` warning-scan leg rather than `-Og` on the Debug flags,
because **nothing in this repo builds with `-Werror`**: `-Og` would print
warnings and gate exactly nothing, while changing the codegen of the shipping
server, both sanitizer legs and both boot legs. Identical payoff, strictly more
risk.

---

## 5. Verification ledger — what has actually been executed

| Level | Means |
|---|---|
| **runtime-verified** | the fixed code was executed and behaved — workstation smoke run, AddressSanitizer, or the CI boot smoke |
| **compile-verified** | it builds (`make debug`, all three binaries); the path was never executed |
| **reasoned** | argued from reading code only |

**Most of this effort is compile-verified only.** Four runtime events back every
"runtime-verified" label in §2, and nothing else does:

1. **ASan-clean end-to-end session** (`0bdc648`) — all three servers boot under
   AddressSanitizer with zero reports; a client completes login → character
   select → enter world → gameplay → movement → clean logout. **This was before
   the hardening wave.**
2. **One client session after the wave** (`d9b6af2`) — the real client against
   the live server: login → **character creation** → enter world → item pickup →
   equip → unequip. Zero exceptions or assertions in `gameserver.log` /
   `loginserver.log`. **Plain `make debug`, not ASan** — it does *not* restore
   event 1 across the wave.
3. **CI boot smoke PASSES under ASan** (`cc1fcd4`) — all three servers boot
   against a seeded MySQL 5.7 container and hold 30s with no sanitizer report,
   first time in the project's history.
4. **A UBSan boot that produced 18-AA's findings** (`9cfe522`) — all three
   servers came up under UBSan and stayed up, yielding the first runtime UB
   reports in this project's history. Raw logs kept outside the repo.
5. **An ASan boot smoke on the wave-10 tip** [2026-08-13, workstation WSL,
   glibc 2.39 / g++ 13.3.0] — **the first ASan execution since wave 1.** Built
   with `make debug-asan`; all three binaries confirmed to carry `__asan_init`
   before the run, because an "ASan run" on an uninstrumented binary proves
   nothing. Seeded a clean MySQL 5.7 from `initdb/`, materialised `conf/*.conf`
   from the templates, booted sharedserver → loginserver → gameserver, all
   three reached their ports (9977 / 9999 / 9998) and held 30s.
   **Zero sanitizer reports** — verified independently of the script's own
   verdict by confirming no `asan.*` log file was produced at all. The only
   output from this run was 5 `luaError.log` lines reading `unknown error`,
   the same shape and message the 2026-08-10 run produced 60 of; pre-existing,
   and uninformative enough to be its own small observability gap.

   **Read what this does and does not cover.** It exercised process startup,
   config parsing — including the five *new* keys from 18-BC and 18-BF, so
   their optional-key fallbacks are executed rather than merely argued — DB
   connection, the load of all 374 `DARKEDEN` tables, and 30s of idle
   operation. It exercised **no packets**. So 18-BD's rollback, 18-AU's
   `FIONREAD` drain, 18-BF's throttle counting and 18-BC's 30s timer are all
   still unexecuted. This is the CI boot leg reproduced locally on a tip CI
   has not yet run, not the end-to-end session of event 1.

**The gap, narrowed at one end and unchanged at the other** [2026-08-13].
Event 5 above ran ASan on the wave-10 tip, so "nothing since wave 1 has been
through ASan at all" is no longer true — every fix through 18-BF has now at
least been *loaded and started* under it, which was worth having: 40-odd
defects deep, a startup-order or initialisation fault would have shown here.

**But ASan's actual value in this project came from packets, and that half is
untouched.** It found 18-D/E/F/J/L/N in one sitting by playing the game, not by
booting. The boot leg covers **boot only, never packets** — its own job name
says so — and event 5 is that leg, run locally. So every wire-reachable fix
from 18-O onward remains unexercised, and the honest summary is: **boot-clean
through 18-BF, packet-unverified from 18-O onward.**

The remaining action is unchanged and is now the *only* thing standing between
this effort and real verification: one end-to-end client session against ASan
binaries, of the kind event 1 recorded. That needs the Windows client and a
person at the keyboard.

**Waves 9 and 10 widened this gap rather than narrowing it** [2026-08-13]. It is
now roughly 40 defects deep. **Wave 10 added the single change in this whole
effort that most needs execution: 18-BD restructured the trade item swap into
detach/place passes with a rollback path that has never been run.** The
rollback is reached only on a failure that is itself hard to provoke, so
neither the compiler nor a normal play session will exercise it; a bug there
destroys player property. 18-BC likewise changes who gets disconnected on the
live login path, on a 30s timer no one has watched fire.

Two of wave 9's changes are the same kind of thing a compiler cannot judge:

- **18-AU's drain loop depends on `FIONREAD`-on-UDP semantics** — including the
  documented case where a pending *zero-length* datagram also reads 0. That
  behaviour was taken from `udp(7)` and reasoned about; it has not been observed
  on this socket. The UDP path is also, per open action 12, one of the paths
  that has **never been executed at all**.
- **18-AY changes who gets disconnected on a live authentication path.** The
  argument that no legitimate player can be locked out rests on the stock client
  opening a fresh connection per attempt — read out of the client source, never
  watched on the wire.

`dkrixserver/build-asan/` is still configured from wave 1, so the setup cost for
open action 9 is close to zero.

**Explicitly not verified from this repository:**

- ~~**Whether the UBSan *boot* leg has ever passed on a runner.**~~ **RESOLVED —
  a run is now cited.** This entry was correct when written: nothing *in the tree*
  supported the claim, and refusing to accept it on assertion was the right call.
  The evidence is outside the tree, in the Actions API, which the rewriting agent
  could not reach.
  **Actions run `31440542647`** (server, push, `314e0dd`, started 2026-08-10
  22:59Z): `boot under ubsan (boot-only, no packets)` → **success**, alongside
  `build (ubsan)` → success and `boot under asan` → success.
  Because that job fails on the **presence of a sanitizer report file** rather
  than on exit status, and a local run before the fix produced 6 findings across
  2 report files, a clean pass means 18-AA's reports are **absent at runtime**.
  **18-AA's row in §2 is upgraded from compile-verified to runtime-verified on
  that basis**, and `docs/ci-server-ubsan.md:6-11` and `:197-200` are now stale
  and should be corrected. Note this verifies the *boot path only* — nothing
  downstream of `listen()` has been executed under UBSan.
- **CI status of waves 4 and 5.** `90a926e`'s merge message asserts "CI green on
  both trees and both trigger types" for waves 2–3; there is no equivalent
  statement for waves 4–5 and no CLI is available in this environment to query
  Actions.
- **Which binary is actually deployed.** Waves 4–7 are on `main` as of `760b48f` (PR #4, 2026-08-11), so the question is no longer "are the fixes merged" but "was the running binary rebuilt from the merged tip". This document cannot answer that; only the workstation can.
- **The client half of 18-R and the `CGExchangeBuy` client change.** This
  workstation cannot build the client; CI is the only possible gate.

---

## 6. Safety nets — current state

The "was" column is preserved deliberately; it is the part worth remembering.

| Gate | Was | Now |
|---|---|---|
| SQL-injection ratchet | Reported **0** against a baseline of 0, and the 0 was meaningless (§3). Saw only single-line calls carrying a printf specifier; multi-line `StringStream` construction, the dominant shape here, was invisible **by construction**, as was 18-A | **Rewritten** (`35b11b2`, `0733065`). Isolates each sink's SQL argument by splitting the argument list on top-level commas, then classifies it. Ratcheted **per category** so a new `format` site cannot hide behind two removed `splice` sites. Re-baselined at 258; **current baseline `format=67 stream=23 splice=123 total=213`** (`dkrixserver/scripts/.sql-injection-baseline`, read 2026-08-11) after 18-AJ's 41 conversions |
| SQL syntax lint over literals | **Did not exist.** This was the cheap gate that would have caught 18-A | **Exists**: `dkrixserver/scripts/check-sql-literals.sh` (`0733065`). Concatenates adjacent literals as the compiler would, then checks fused statements, glued keywords, multiple verbs, unbalanced quotes/parens, quoted `?`, and bind-count mismatch. **Verified against the defect it exists for**: over `9422e9c^`'s `Key.cpp` it fires three independent rules and exits 1; over the fixed file, 0. Allowlist is **empty**, so it is green on merit |
| Server + client CI triggers | **Never ran on this branch.** `server.yml` triggered on `[main, "modernize/**"]` and PRs to `main`; the branch matched neither, so a long run of commits went unbuilt (**27** per `0733065`'s own count at the time, **34** per `ff62f9a`'s later restatement — the discrepancy is unresolved and neither is re-derivable now). Two format-only commits were written to satisfy a gate that never ran | **Branch allowlist removed from both workflows** (`0733065`) — push triggers on **every** branch, still `paths:`-filtered, `workflow_dispatch` on both. Deliberately not extended to `fix/**`: an allowlist only moves the trap to the next unanticipated prefix. **`paths:` blind spot fixed** (`ff62f9a`): `shared/Packets/**` now triggers both trees — it compiles into all three servers and is absorbed into the client, and previously triggered neither |
| `sanitizers` matrix | **Build-only.** It ran `make debug-asan` and never executed a server, so it structurally could not see 18-B/D/E/F/J/L — the entire class it appeared to cover. Two green ASan builds sat there while every login segfaulted. The `ubsan` leg was 0-for-17 | Still build-only, joined by a **runtime** job: `boot under <san> (boot-only, no packets)` seeds a mysql:5.7 service container, materialises conf from templates, and boots all three servers under the sanitizer. **The ASan leg PASSES** (`cc1fcd4`). Covers 18-A/D/E/J/L — **5 of 39** — and **covers nothing needing packets**. Both boot legs non-blocking until five consecutive greens; **`ubsan`'s counter is 0** (§5) |
| UBSan specifically | Red on **every run it ever had**, 17-for-17, always at link time; never reported a single UB finding | **Links** (`8bb1118`, one flag, order load-bearing — `docs/ci-server-ubsan.md`) and **has produced real findings**: 18-AA, including five out-of-bounds writes that worked only by luck. `halt_on_error=0` deliberately, so one boot enumerates every distinct site; detection is by report file, not exit status. **Whether the leg has passed on a runner is unverified** — §5 |
| `clang-format` job | Red on legacy files you touch — it checked **whole changed files**. `Zone.cpp` carried 535 pre-existing violations: a bias against touching old code, which cost two format-only commits | Server checks **changed lines** via `tools/ci/clang-format-changed-lines.sh` (`0733065`). Measured on this branch: the whole-file gate fails **29 of 43** changed server files on 871 lines of pre-existing drift; the changed-lines gate checks all 43 and passes. `clang-format==18.1.8` pinned — unpinned apt gives 18.1.3, which disagrees with later versions on two CJK-comment files |
| Optimisation-dependent warnings | **Structurally impossible to emit** — see §4's root cause | **New informational leg** `warning scan (-O2, informational)` (`ff62f9a`). Flags go in `CMAKE_CXX_FLAGS`, not `..._DEBUG`, because `CMakeLists.txt:35`'s plain `set()` overrides the cache. Build type stays **Debug**: Release would add `-DNDEBUG`, compiling `Assert()` to `((void)0)` and deleting whole branches from the dataflow being analysed. **Deliberately not cached** — an incremental build re-emits warnings only for what recompiled, so a cache hit would turn a 400-warning inventory into "3" and read as an improvement, the same failure shape as the SQL ratchet. Covers the whole optimisation-dependent family — uninit, use-after-free/dangling (18-B), array-bounds and stringop-overflow (18-E), mismatched-new-delete (18-D). **No baseline, no threshold: the leg cannot fail**; the step summary flags a zero family count as *suspect*, not good. Seeding procedure is written into the job as a comment |

---

## 7. Operational actions

### Done

1. ~~**Rotate the DB credential.**~~ **DONE.** The live value was rotated
   separately and exists only in untracked `conf/*.conf`. The burned historical
   value was swept from tracked files in one byte-level pass — 55 replacements
   across 19 files plus 4 placeholder normalisations and 6 prose corrections,
   with CRLF, legacy CJK encoding and two 3.5 MB SQL dumps preserved
   byte-for-byte otherwise (`430bcc9`). The old value remains in published
   history; that is accepted.
2. ~~**Untrack the secret-bearing configs.**~~ **DONE** (`c587490`, `0bdc648`,
   `3dcfb1b`). `git ls-files dkrixserver/conf` prints exactly the three
   `.conf.template` files. The pre-existing ignore rule reached only 8 of 10 —
   `*.conf` does not match `.new` and does not cross a `/` into `backup/` — so it
   was widened and verified with `git check-ignore` in both directions.
3. ~~**Parameterise Docker.**~~ **DONE** (`3dcfb1b`). `docker-compose.yml` and
   `initdb/a-setup.sql` take `${VAR:?message}` from `docker/.env`.
4. ~~**Decide the `Player::setKey` killswitch.**~~ **DONE — deleted** (`5e088a4`,
   18-Y). Constants confirmed dead tree-wide; no other `exit(0)` is
   packet-reachable.
5. ~~**Get CI onto this work.**~~ **DONE** (`0733065`, PR #2), and the last
   trigger blind spot closed (`ff62f9a`).
6. ~~**Answer the UDP question.**~~ **Answered the hard way.** UDP 9997 *is*
   reachable off-LAN and carried an unauthenticated admin channel (18-AB).
7. ~~**Merge 18-AB to `main`.**~~ **DONE** — PR #3, `90a926e`, 2026-08-11.
8. ~~**Merge waves 4 and 5.**~~ **DONE** — PR #4, `760b48f`, 2026-08-11, which
   carried waves 4 through 7 (`00a5372`…`5e8e153`). The residual is **not**
   merge state any more, it is deployment state: whether the running binaries
   were rebuilt from that tip. See §5.
9. ~~**Fix `OptionInfoManager::m_OptionClassInfos`.**~~ **DONE** — 18-AR,
   `4fb9591`. It was not one instance but two: `SkillPropertyManager` fell out
   of the same fix. The prescription written here — "one-line `reserve` →
   `resize(OPTION_MAX, NULL)`" — **understated it**; `release()` runs from
   `init()` as well as the destructor, so a plain `clear()` there re-arms the
   same bug on the next load, and `clear()`/`toString()` had to be rebounded on
   `size()` rather than on the constant.

### Open

9. **Run an end-to-end client session against ASan binaries — still the
   highest-value action on this page, now narrowed to exactly one thing.**
   The *boot* half was done 2026-08-13 (§5 event 5): all three servers built
   with `make debug-asan`, instrumentation confirmed, booted against a freshly
   seeded MySQL, zero sanitizer reports. What that leaves is the half that
   actually earned ASan its reputation here — **packets**. 18-D/E/F/J/L/N were
   all found by playing, not by booting.

   Specifically unexecuted and specifically worth aiming at: **18-BD's rollback
   path** (reached only on a failure that is hard to provoke, and it moves
   player property), **18-AU's `FIONREAD` drain** on the `Datagram` path that
   action 12 records as never having been run at all, **18-BF's throttle
   counting**, and **18-BC's 30s pre-auth timer**. The setup cost is now
   nothing: `build-asan/` holds current instrumented binaries, and
   `scripts/ci-boot-smoke.sh seed|conf|boot` brings the world up in one step.
   It needs the Windows client and a person at the keyboard.
10. **Re-run the servers under UBSan** to confirm 18-AA's three reports are
    actually gone — `9cfe522` explicitly does not claim they are — and to
    establish whether the CI UBSan boot leg passes at all (§5).
11. **Read the `-O2` warning-scan leg's first output and seed a baseline.**
    `ff62f9a` deliberately shipped it un-baselined and unable to fail; if its
    findings go unread, that is the trigger to revisit `-Og`.
12. **Exercise the paths still never run**: NPC shop rack (no buy or sell has
    ever been performed), `CGAddItemToCodeSheet`, phone slots, pet stash, and the
    entire `Datagram`/`SerialDatagram` UDP path — which is exactly where 18-AB
    and 18-U's UDP bounds checks live.
13. **Schedule the coordinated wire changes** — `CGMove` field order (§1 row 3)
    and, if the exchange subsystem is ever enabled, `GCExchangeBuy` /
    `CGExchangeList` (§1 rows 7, 8). House rule: both trees in one commit.
14. ~~**Delete `FameLimitInfo.{h,cpp}` and `EventBall.{h,cpp}`**~~ — **done
    2026-08-13** (`a8503bf`). Both were verified dead first, not assumed:
    `FameLimitInfo`'s six references are all commented out and its DB table
    exists in neither seed dump; `EventBall` returns an `ITEM_CLASS_EVENT_BALL`
    enumerator that **does not exist**, so the header could not compile if
    anyone tried to use it. The action's third file, `MonsterKillQuest.cpp`,
    was **not** deleted — it turned out to be one of 13 files in an uncompiled
    legacy subsystem. That is now §1 row 18.
15. ~~**Decide the pre-authentication idle deadline**~~ — **done 2026-08-13**
    (`b3e9322`, 18-BC), 30s, tunable via `MaxPreAuthIdleSeconds`. The action
    was framed as "decide a deadline"; the finding was that **a deadline
    already existed and slid**, which is a different and worse bug than having
    none. The same shape is still live on the gameserver — §1 row 20 — and the
    cheapest thing to flood is now the accept-path log, §1 row 21.
16. **Do not run `make release` casually.** §1 row 10.
17. ~~**Add a `reserve()` grep gate.**~~ — **done 2026-08-13** (`821d5e5`).
    `dkrixserver/scripts/check-reserve-sizing.sh`, wired into the `ratchets`
    job, baseline **2**. It is not the naive grep this action asked for: it
    masks comments and literals first (all of `LuckInfoManager`, two `reserve()`
    calls included, sits inside one block comment), keys on the receiver's last
    top-level identifier, and excludes map/set by declared type. **Validated
    against history rather than asserted** — run over the pre-fix tree at
    `3e02f6c^` it reports 17 sites containing *every* historical instance, 0
    false positives and 0 false negatives, with the per-wave counts falling
    17→16→9→8→6→4→2 as the fixes landed. It immediately found **instance 7**,
    which this action predicted would otherwise be written: §1 row 17.
18. **Do not rewrite history.** No live secret was ever committed, and a rewrite
    breaks the `archive/modernization-phases-1-17` tag and every published SHA.

---

## 8. Where the work lives

**Everything volatile is in this section.** When merge state changes, edit here
and the one cut-line sentence in §2 — nothing else in this document should need
to move.

| | |
|---|---|
| Branch | Waves 9 and 10 work **directly on `main`** — no feature branch. `fix/bug-18b-loginserver-result-uaf` (tip `3b77c4e`) is fully merged and retained but dormant |
| Effort span | 2026-08-10 (`f15bb13`) → 2026-08-13 (`f73767b`), **89 non-merge commits** since `9422e9c` |
| On `main` | **All of it.** Waves 1–8 via `669a9fe` (PR #2), `90a926e` (PR #3), `760b48f` (PR #4) and `e86a9dd` (wave 8). **All four are real merges, not squashes** — individual commits are reachable and `--contains` is reliable (§3.2). Waves 9–10 are `a8503bf`…`f73767b`, committed straight onto `main` |
| **Not on `main`** | **Nothing.** Empty since 2026-08-10 |
| Waves | 1 = 18-A…18-Z; 2 = 18-AA…18-AD; 3 = 18-AE…18-AG; 4 = 18-AH…18-AJ; 5 = 18-AK…18-AM; 6 = 18-AN…18-AP; 7 = 18-AQ; 8 = 18-AR…18-AT; 9 = 18-AU…18-AY; 10 = 18-AZ…18-BD; 11 = 18-BE, 18-BF (owner-directed, not a sweep) |
| Defect count | **58** numbered, 18-A…18-BF |
| Gates | Four ratchets in the `ratchets` job: SQL injection **213**, packet duplicates **90**, `reserve()`-sizing **1** (`821d5e5`), guard-indexes-container **25** (`f73767b`). The last two are **validated against history** — each was run over the pre-fix trees and every count change is accounted for by a named fix or deletion |
| Companion record | `docs/MODERNIZATION.md`, "Phase 18 —" sections. This document does not own it |

**Wave 10 — 2026-08-13, five parallel workstreams, immediately after wave 9.**
Six commits, `dbddf7b`…`f73767b`, on `main`. `make debug` green (all three
binaries) and `make fmt-check` green at the tip.

It closed §1 rows 5, 11, 17 and 18 plus open action 15, and **opened rows 19
through 23**. That ratio is the point: two consecutive waves aimed at this
section's backlog have each opened roughly as many rows as they closed, because
fixing a defect properly means reading the code around it. Row 19 — live gold
duplication — is now the highest-value row on the page, and it was found by an
agent sent to fix something else in the same file.

**Three more of this document's claims were wrong.** §1 row 5's `FD_SETSIZE`
assertion was never tested and is now measured. §1 row 11 was wrong twice over:
the item-loss path was reachable rather than gated, and the asset `cancelTrade`
stranded is gold, because items are never escrowed. Open action 15 asked for a
deadline to be *decided* when one already existed and slid — a worse bug than
having none. Two deletions also came in bigger than recorded: `quest/Squest/` is
29 files rather than 13, and `UserGateway` could not have compiled at all.

**Wave 9 — 2026-08-13, five parallel workstreams against this section's own
backlog.** Seven commits, `a8503bf`…`821d5e5`, on `main`, working tree clean.
`make debug` green (all three binaries relinked) and `make fmt-check` green at
the tip; `make fmt` was deliberately **not** run, since its target reformats the
whole of `src` — only the four files the wave touched were passed to
clang-format 18.1.8, and the churn was trailing whitespace.

It closed §1 rows 1, 4, 6, 12 and 15 plus open actions 14 and 17, and **opened
rows 16, 17 and 18** — two of which are consequences of the closures rather than
new discoveries. Four of this document's own claims were corrected in the
process (§1 row 1's paths and the loginserver's sleep count; row 6's cast kind;
row 15's omission that the gameserver was already correct; and §3.1's
`RankEXPInfo`/`AttrBalanceInfo` reasoning), and the `GuildManager` note under §1
was **refuted outright** — see §3.1.

**Measured against `main` @ `e86a9dd`, working tree clean.** The
previous revision recorded **uncommitted** changes to 22 source files from a
concurrent session and warned that nothing in it counted them. **That work is
now committed** — 25 files across `4fb9591`…`6e7cc2b`, reviewed and built
before committing (`make debug` in WSL, all three binaries; `git-clang-format`
clean on the changed lines). It is wave 8 above, and it is what closed the
previous revision's §1 row 4.

**One commit on the way in has a junk message.** `2c53137` is titled `sd` and
carries the wave-8 refresh of this document and of `MODERNIZATION.md`. It is not
being fixed: it is published, and §7 action 18 forbids rewriting history for
exactly this kind of reason. `e86a9dd`, the merge commit, carries the real
description. **Rule 8 of §9 — "the commit messages outrank this file" — has one
documented exception, and this is it.**

**§1 was renumbered when rows 1 and 4 closed.** Old → new: 2→1, 3→2, 5→3,
6→4, 7→5, 8→6, 9→7, 10→8, 11→9, 12→10, 13→11, 14→12, 15→13, 16→14,
17→15. Cross-references elsewhere in this file were updated with it; **any
external citation of a §1 row number predating 2026-08-13 is off by one or
two.** This is the cost of ranked-by-number rows and is worth remembering before
citing one from outside.

**Filename.** This file is named for a single day and now spans four, eight waves
and 46 defects — the name is itself a staleness generator, and a reader who sees
`2026-08-10` reasonably assumes the content stops there. **Recommend renaming to
`docs/SECURITY-AUDIT-PHASE-18.md`** (a `git mv`, plus the four referrers:
`docs/MODERNIZATION.md`, and the cross-references in `docs/ci-server-ubsan.md`
and this file's own §-links). **Not performed** — renaming is the orchestrator's
call because `MOD` cites the old path.

---

## 9. How to keep this document current

The previous two revisions were snapshots, and both were superseded before they
were committed. The rule that follows from that:

18. **Never write "as of this rewrite."** Every row carries its own commit or
   date, so a stale row is visible locally instead of invalidating the page.
19. **§1 is the only ranked prose.** Closing an item means deleting its row from
   §1 and adding a row to §2 — never editing §1 in place to say "now fixed".
20. **§2 is append-only and keyed by ID.** A new wave adds rows. The only
   merge-state fact in it is one sentence above the table.
21. **§3 is append-only, and rows are never deleted.** It is the reason this
   effort has not re-chased the same dead leads five times. When a claim you
   acted on turns out wrong, §3.2 is where it goes — not the commit message
   alone, and not nowhere.
22. **§4 counts go up, never down.** A pattern that stops recurring stays listed
   with its final count.
23. **§8 absorbs all volatility.** Merge state, commit counts, wave boundaries.
24. **Verification vocabulary is §5's and is not negotiable.** If you cannot cite
   the run, it is compile-verified. "The CI leg exists" is not "the CI leg
   passed".
25. **The commit messages outrank this file.** If they disagree, the commits are
   right and this file has a bug — fix it here and note the correction in §3.2.
