# Security & correctness audit — Phase 18

**First written 2026-08-10 against 21 commits. Rewritten 2026-08-10 (late) against
the finished branch.** The original accumulated inline "superseded" notes faster
than it could be annotated; this is a rewrite from the repository, not a patch of
the old text. What survives verbatim is §3, the record of refuted claims — it is
extended, never trimmed.

> **Already partly superseded — read this first.** This rewrite was made at the end
> of wave 2. Wave 3 landed immediately after and is **not** covered below:
> **18-AE** (a live client-reachable OOB read on the learn-skill path via the
> `SOUL_CHAIN` carve-out, plus two `Zone::movePC` bugs), **18-AF** (the half-wired
> exchange subsystem made to fail closed, its compare-and-swap made real, both
> packet factories unregistered) and **18-AG** (47 `Assert`-wrapped XML parse calls
> hoisted, the last 6 `(T&)` reference casts fixed). Two audits also returned clean
> verdicts that belong here: the combat path is **genuinely authoritative**, and
> movement geometry is **solid** — but there is a **working speed hack**, because
> `verifySpeed`'s `CG_MOVE` branch is an empty body and the `CGVerifyTime` gate is
> dead by construction. See `MODERNIZATION.md`, "Phase 18 — waves 2 and 3", which
> is current.
>
> The lesson this document keeps re-teaching: a security audit written against a
> moving branch is stale before it is committed. Trust the commit messages and
> `git log` over any prose here, including this rewrite.

**Sources.** `git log main..HEAD` on `fix/bug-18b-loginserver-result-uaf`
(**49 commits**, cited by hash — the commit messages are the primary source and
are more complete than any doc), `docs/MODERNIZATION.md` (cited `MOD`),
`docs/ci-server-ubsan.md`, and direct reads of the tree (cited `file:line`).
Every claim below carries one of those. Where a claim could not be verified it
says so in place rather than being repeated.

## Where this work actually lives

This matters for reading everything below, and the old document got it wrong.

| | |
|---|---|
| Branch | `fix/bug-18b-loginserver-result-uaf`, tip `efe35f8` |
| Merged | **PR #2 squash-merged into `main` as `669a9fe`**, 2026-08-10 19:46 UTC — covering `f15bb13`…`d9b6af2`, i.e. **42 of the 49 commits** |
| **Not merged** | the last **7**: `8bb1118` (UBSan CI), `9cfe522` (18-AA), `4d92150` (**18-AB**), `6f45414` (18-AC), `053b69f` (18-AD), `b9fac7d` + `efe35f8` (client `CGExchangeBuy`) |
| Local `origin/main` | **stale** at `9422e9c`. `git fetch` before measuring anything against `main` |

**Measured against the committed tip `efe35f8`.** At the time of writing the
working tree also carried **uncommitted** changes to 27 server files from a
concurrent session — further `Assert` hoists in `GQuest*`/`TriggerParser` (the
18-AD shape, including the `GQuestTouchWayPointElement` parse that 18-AA left
inside the macro) and edits to `exchange/ExchangeService.{h,cpp}` and
`ExchangeDB.cpp`. **§2 rows 2 and 9 may already be moving.** Nothing below
counts that work; re-check those two rows against `git log` before acting.

**The single most severe finding of the whole effort — 18-AB, the unauthenticated
UDP admin channel — is in the unmerged tail.** See §2 row 1.

## Verification vocabulary — used strictly

| Level | Means |
|---|---|
| **runtime-verified** | the fixed code was executed and behaved — on the workstation (smoke run and/or AddressSanitizer) or in the CI boot smoke |
| **compile-verified** | it builds; the path was never executed |
| **reasoned** | argued from reading code only |

Three runtime events back the "runtime-verified" labels, and nothing else does:

1. **ASan-clean end-to-end session** (`0bdc648`, before the hardening wave). All
   three servers boot under AddressSanitizer with zero reports; a client
   completes login → character select → enter world → gameplay → movement →
   clean logout.
2. **Client session after the wave** (`d9b6af2`). Enrico ran the real client
   against the live server and completed login → **character creation** → enter
   world → item pickup → equip → unequip. Zero exceptions or assertions in
   `gameserver.log` / `loginserver.log`. **Plain `make debug`, not ASan** — this
   does *not* restore claim 1 across the wave.
3. **CI boot smoke PASSES** (`cc1fcd4`). All three servers boot under ASan
   against a seeded MySQL 5.7 container and hold 30s with no sanitizer report —
   first time in the project's history. Extended to a UBSan leg by `8bb1118`.

**Everything in the unmerged tail (18-AA…18-AD, the client `CGExchangeBuy`
change) is compile-verified only**, and the client half is not even that — this
workstation cannot build the client, so CI is its only possible gate and its
status at `efe35f8` is **not confirmed by this document**.

---

## 1. Fixed

30 numbered bugs (18-A … 18-Z, then 18-AA … 18-AD) plus unnumbered items.
Ordered by ID.

### 1.1 Merged to `main` (in `669a9fe`)

| ID | What | Where | Found by | Verification |
|---|---|---|---|---|
| 18-A | Two SQL statements concatenated into one literal; gameserver could not boot at all since 2026-08-09 | `gameserver/item/Key.cpp` | smoke run 1 | **runtime-verified** (`9422e9c`) |
| 18-B | `Result` use-after-free — loginserver segfaulted on *every* login, valid or not | `Core/CLLoginHandler.cpp` | crash + gdb | **runtime-verified** (`f15bb13`) |
| 18-C | Same UAF on character select | `Core/CLSelectPCHandler.cpp` | audit of the 18-B candidate set | **runtime-verified, Vampire branch only**; Slayer/Ousters compile-verified (`38dfbbc`, `aef987a`) |
| 18-D | `new char[]` released with plain `delete` (3 vars) | `gameserver/Zone.cpp:798-800` | ASan | **runtime-verified** (`6e356af`) |
| 18-E | `ExpTable` sized `MaxLevel` but written 1-based → 4-byte heap overflow **on every boot** | `gameserver/SomethingGrowingUp.h` | ASan | **runtime-verified** (`6e356af`) |
| 18-F | Uninitialised member pointers `delete[]`d in `clear()`; 6 copies across all three servers | `*/GameServer{Group,}InfoManager.cpp` | ASan | **runtime-verified** (`5b8a259`) |
| 18-G | Store index guard `> MAX_ITEM_NUM` where vectors hold exactly 20 → client-reachable heap OOB read **and write** + virtual call | `CGDisplayItem` / `CGUndisplayItem` / `CGBuyStoreItemHandler` | packet audit | **compile-verified only** — no store interaction in any session (`5b8a259`) |
| — | `Player::setKey`: uninitialised `pHashTable` read, plus 512-byte leak per `CGConnectSetKey` | `Core/Player.cpp` | packet audit | **runtime-verified** (login path, ASan-clean run) (`5b8a259`, `0bdc648`) |
| 18-H | `read(m_IdempotencyKey)` bound to the raw template `read<T>()` → attacker-chosen pointer read + free | `Core/CGExchangeBuy.cpp` | packet audit | **compile-verified only** (`a29ee09`). ⚠ its "only such site tree-wide" claim is **false** — see §3 |
| 18-I | Bounds checks at the accessors: `Inventory::getInventorySlot/getItem`, `isWear/getWearItem/takeOffItem` ×3 races; 3 GM commands ungated by an `\|\|`/`&&` precedence typo; 49 `sprintf`→`snprintf` in `CGSayHandler` | `Inventory.*`, `Slayer/Vampire/Ousters.*`, `CGSayHandler.cpp` | packet audit | **runtime-verified** for the inventory/wear accessors (pickup, equip, unequip — `d9b6af2`); the GM-command and `snprintf` parts remain compile-verified (`1019602`) |
| 18-J | `PetAttrInfo` ctor `reserve()`+`clear()` → writes into unconstructed storage | `gameserver/PetAttrInfo.h` | ASan at boot | **runtime-verified** (`3e02f6c`) |
| 18-K | 5 mutating calls inside `Assert()`; silent item duplication **in a Release build only** | `CGShopRequestSell`, `CGUseItemFromGQuestInventory`, `couple/WaitForMeet` | audit | **compile-verified only** (`3e02f6c`). Swept properly later — see 18-AD |
| 18-L | `GameServerPlayer*` array never initialised; `!= NULL` passed on garbage, then dereferenced | `sharedserver/GameServerManager.cpp` | ASan SEGV | **runtime-verified** (`15c51f0`) |
| 18-M | (a) narrowing-before-check in the 18-H fix itself; (b) `Zone` ctor leaves 4 members indeterminate that `~Zone()` `delete[]`s; (c) `Assert(m_pWearItem[Part] = NULL)` — assignment, not comparison | `CGExchangeBuy`, `Zone.cpp`, `Slayer.cpp` | adversarial review of this branch | **compile-verified only** (`002fcaf`) |
| 18-N | 1-based loops ran to `Num` inclusive on a stack VLA sized `Num` — aborted every login under ASan | `Core/CLGetWorldListHandler.cpp` | ASan during smoke run | **runtime-verified** (`0bdc648`) |
| 18-O | Only 2 of 150 handlers checked distance: ground loot could be vacuumed zone-wide. Reach test (Chebyshev radius 8) added to 3 pickup handlers via a new `ZoneUtil::isWithinReach()` | `CGAddZoneToInventory/Mouse`, `CGPickupMoney` | packet audit | **runtime-verified** — ASan-clean boot after the change; item pickup exercised (`d4f4d27`, `d9b6af2`) |
| 18-P | `reserve()` without `assign()` → writes through `operator[]` into unsized storage; 3 sites. Also `ItemGradeManager` left two of three vectors unsized until `load()` ran | `ZoneGroupManager`, `ItemGradeManager`, `war/DragonEyeManager` | audit | **compile-verified only** (`35b11b2`) |
| 18-Q | Same shape as 18-J in `PetExpInfoManager`; also leaked all 51 objects at shutdown. **ASan cannot catch this one** — reserved capacity covers every write | `gameserver/PetExpInfo.*` | audit | **compile-verified only** (`a894d7c`) |
| 18-R | `deleteWearItem` — last unguarded wear-slot write accessor, 3 races. **Plus four network-reachable client stack overflows**: `GCPartySay`, `GCGuildChat`, `GCSystemMessage` (guard was `if (szMessage > 256)` on a `BYTE` — tautologically false), `GCWhisper` (cap 128 into `char[128]`, the NUL overflows). The launcher updates over plain HTTP, so an on-path attacker can stand in for the server | server: 3 race files; client: 4 `GC*Handler.cpp` | audit | **compile-verified** server-side; **client half CI-verified only** (`75455ec`, formatted `42688d6`) |
| 18-S | `CGSayHandler::opnotice` opened a `Connection` to a hardcoded **remote** host with hardcoded credentials compiled into the shipped binary and published, then wrote a chat-supplied string into it as raw SQL. Disabled; also deleted `billing/test/` (two more tracked `.conf` with passwords) | `Core/CGSayHandler.cpp` | audit | **compile-verified only** (`2c5ebe9`) |
| 18-T | Three out-of-bounds packet fields rejected at `read()`: `CGAddItemToCodeSheet` (`IndexNum` reaches ~1402 against a **30**-element vector — OOB read *and* write), `CGConnect` `PCType` (pre-auth, 3-element table), `CLCreatePC` `HairStyle` **and `Slot`** — the latter a raw 0–255 `BYTE` into a 3-element table that also feeds SQL binds | `shared/Packets/` | packet audit | **runtime-verified** — character creation and world entry exercised (`4b76cc5`, `d9b6af2`). ⚠ its `CLDeletePCHandler` credit is **wrong** — see §3 |
| 18-U | 25 wire-reachable `Assert` bounds checks converted to **real** checks that survive `NDEBUG`: Slayer phone accessors, `Datagram`/`SerialDatagram` read/write bounds, pet stash, 11 NPC shop-rack accessors, 4 `ItemRack` accessors | tree-wide | audit | **partly runtime-verified** — wear-slot accessors exercised (`d9b6af2`); **shop rack, phone slots, pet stash and the whole UDP `Datagram` path remain compile-verified only** (`326c298`) |
| 18-V | 8 files' pre-declared `Result*` scoped to their statements — the near-miss siblings of 18-B. All dangled; none was read, so none was live | 8 files, incl. `FlagManager.cpp:355` (undocumented) | audit finding #7 | **runtime-verified** for `CLLoginHandler` (login); rest compile-verified (`c5bebb7`, `d9b6af2`) |
| 18-W | Success-path `SAFE_DELETE(pStmt)` in 21 legacy `BEGIN_DB` blocks (`END_DB` frees only on the exception path); 3 uninitialised `Statement* pStmt` where `END_DB`'s `delete` is reachable before assignment; `MonsterKillQuest::save()`'s literal SQL `"-_-"` replaced by a documented `throw` | 20 files | audit finding #6 | **runtime-verified** for the boot loaders (all three servers booted); rest compile-verified (`65c07ad`, `d9b6af2`) |
| 18-X | `ExpTable::load()` fed a runtime-built `snprintf` buffer to the **variadic printf-style** `executeQuery()` — the buffer became the format string. Switched to `executeQueryString()`. Only live site of its class. Plus an uninitialised `Statement*` and a real bounds check beside the `Assert` on a DB-indexed heap write | `gameserver/SomethingGrowingUp.cpp` | audit | **runtime-verified** at boot; the `%`-injection path itself never exercised (`36727b4`, `d9b6af2`) |
| 18-Y | **Unauthenticated remote shutdown killswitch deleted.** `Player::setKey` compared the client-supplied key pair against two hardcoded constants (`0xAEB7`, `0x9B3E`, both in published source) and called `exit(0)`. `CGConnectSetKey` is registered on **both** the login and game servers with no auth gate. 2008-era anti-cheat; **the owner decided to remove it** | `Core/Player.cpp:242` | §2 row 1 of the first revision | **runtime-verified** — `setKey` is on the login path (`5e088a4`, `d9b6af2`) |
| 18-Z | The seed dump `initdb/DARKEDEN.sql` **had never been loadable**. An English translation pass collapsed multi-byte strings to ASCII fallbacks: duplicate primary keys in 6 tables, then empty `ZoneInfo` names, then empty NPC names. Restored wholesale from the intact sibling `backup_darkeden_after_english_20260424.sql`. **This broke the documented human runbook too, not only CI** | `dkrixserver/initdb/DARKEDEN.sql` | CI boot gate | **runtime-verified** — boot smoke now PASSES (`7270626`, `cc1fcd4`) |

Also merged, non-bug: four format-only commits (`3ca03fe`, `c8fbd67` — Zone.cpp's
1,188-line reformat, verified token- and assembly-equivalent — `6d6059e`,
`42688d6`), the secret untracking and credential sweep (§5), the Docker repair
(`a894d7c`), and the CI work (§4).

### 1.2 On the branch, **not yet on `main`**

| ID | What | Where | Verification |
|---|---|---|---|
| — | **UBSan CI leg links for the first time in the project's history.** `build (ubsan)` had failed 17-for-17, always at *link* time: `-fsanitize=undefined` implies `vptr`, which emits `typeinfo for T` references from three sharedserver call sites into `GameServerPackets`, which sharedserver deliberately does not link. One flag, `-fno-sanitize=vptr`, appended **after** `-fsanitize=undefined` (order is load-bearing). The boot job became a matrix over `asan`/`ubsan`, so a UBSan binary is now *executed*, not only built | `dkrixserver/CMakeLists.txt`, `.github/workflows/server.yml` | **runtime-verified** — `make debug-ubsan` links, exit 0, all three binaries; the leg stays **non-blocking** (0 consecutive greens) (`8bb1118`, `docs/ci-server-ubsan.md`) |
| 18-AA | **The first runtime UB this tree has ever reported.** Three findings from booting all three servers under UBSan: (1) **misaligned type-punned load/store in the packet path** — `SocketOutputStream.h` did `*((T*)(m_Buffer + m_Tail)) = buf` and `SocketInputStream.h` the mirror, i.e. *every packet the server writes*; replaced with `std::memcpy` of `sizeof(T)`, byte layout unchanged. (2) `memset(binds.data(), …)` where an empty vector's `data()` is `nullptr` against a `nonnull` parameter — the memset was redundant, removed. (3) **`GQuestTouchWayPointElement::makeElement` cast five members to `DWORD&` and let `XMLTree::GetAttribute` write four bytes through each — four are `WORD`, one is `BYTE`. All five were out-of-bounds writes past their member.** It produced correct values **only by luck**: writes happen in declaration order, so each one's collateral damage was repaired by the next, and the last landed in tail padding. **Reordering the members in the header would have silently corrupted quest waypoints** | `Core/SocketInput/OutputStream.h`, `database/PreparedStatement.cpp`, `gameserver/GQuestTouchWayPointElement.cpp:27-51` | **compile-verified** (`9cfe522`). Whether the reports are now absent needs a re-run under rebuilt UBSan binaries |
| 18-AB | **The worst hole found in this project, and it is live.** `GGCommandHandler` dispatches GM commands — `*shutdown`, `*command setGold`, `*command killAllPC`, `*kick`, `*set <economy ratios>`, `*load` — with **no privilege check of any kind**, passing `pGamePlayer = NULL`. Those arrive as **UDP datagrams**, and the gameserver's receive loop called `execute(NULL)` on every datagram with no source check, no shared secret, no sequence number. **UDP 9997 is deliberately port-forwarded** so the owner's friends can play. One packet from anywhere on the internet could shut the server down, mint gold, or wipe every player in a zone. No login, no character, no GM flag. The **loginserver had the identical hole**. Gated centrally in each receive loop immediately before `execute(NULL)`, with an **inverted** allowlist (client-facing IDs enumerated, so anything added later is peer-gated by default): gameserver allows only `PACKET_CG_PORT_CHECK`, **loginserver's allowlist is empty**. Peers = loopback + configured peer IP + the `GameServerInfo` IP column, so no new config. Also fixed alongside: `opcommand` had ~66 `pGamePlayer->` dereferences behind 2 guards (a one-packet remote DoS, and a live crash on the legitimate `*allworld` fan-out); **a remote OOB read in `UserInfoManager::getUserInfo()`** — an unvalidated packet-supplied `BYTE` `WorldID` (0–255) indexing an array of typically 3, so `WorldID=255` read ~250 `unordered_map`s past the allocation and called `.find()` on garbage; and 4 SQL-concatenation sites parameterised (`bug_report`'s sanitiser only ever replaced the **first** quote and backslash, so `a''b` still injected) | `gameserver/LoginServerManager.cpp`, `loginserver/GameServerManager.cpp`, `loginserver/UserInfoManager.cpp:161-175`, `Core/CGSayHandler.cpp`, `Core/GMServerInfoHandler.cpp` | **compile-verified only. None of these paths has been exercised at runtime** (`4d92150`) |
| 18-AC | Sweep for 18-T's shape. The packet layer holds **exactly six** wire-integer→enum conversions; three were 18-T's and **all three of the rest were unguarded**: `CLDeletePC` `Slot` (raw 0–255 into `Slot2String[3]`, feeding **six SQL binds**, reachable from `LPS_PC_MANAGEMENT` by any authenticated account), `CLRegisterPlayer` `Sex` and `Nation` (currently unreachable behind a closed `PacketValidator` loop; fixed anyway). Five `toString()` lookups bounded independently, since `readPacket()` calls `toString()` on **every** packet received | `shared/Packets/CLDeletePC.cpp:27`, `CLRegisterPlayer.cpp:46,91`, `CGMove`, `CGUnburrow`, `CLSelectPC` | **compile-verified only** (`6f45414`) |
| 18-AD | The 18-K shape, swept properly — every function called inside an `Assert` tree-wide was extracted and vetted. 6 live sites in 4 files. **`WaitForApart.cpp:79,80` is item duplication, not a theoretical one**: `Assert(removeCoupleItem())` ×2 (take off + destroy + delete). Under `NDEBUG` the calls vanish but `removeCouple()` and the `FLAGSET_IS_COUPLE` clear still run, so the pair re-couples and `WaitForMeet::giveCoupleRing()` mints a second pair — unbounded, and the rings carry a random stat option. Also `ActionTradeGQuestEventItem.cpp:163` (wraps `Inventory::addItem` → two DB rows at one `(x,y)`), `ActionQuestReward.cpp:86` (reward lost), `TriggerParser.cpp:21,22` (quest triggers silently inert). All six are **pure hoists**: identical behaviour in the deployed Debug build, correct behaviour under `make release` | `couple/WaitForApart.cpp`, `quest/ActionTradeGQuestEventItem.cpp`, `quest/ActionQuestReward.cpp`, `quest/TriggerParser.cpp` | **compile-verified only** (`053b69f`) |
| — | Client `CGExchangeBuy` matched to the hardened server format, closing the last wire divergence from 18-H: 8-byte listing ID, 1-byte length prefix rejected above 64, then the key; `getPacketMaxSize()` 73 on both sides. **The divergence was wider than the first revision described** — the listing ID *width* was wrong too, 4 bytes client vs 8 server, and the client truncated an already-`int64` `listingID` at the call site | `dkrix/Client/Packet/Cpackets/CGExchangeBuy.{h,cpp}` | **not compile-verified** — this workstation cannot build the client; CI is the only possible gate and its status at `efe35f8` is unconfirmed here (`b9fac7d`, `efe35f8`) |

### 1.3 The first revision's §2 ranking, resolved

All **nine** rows of the original "open, ranked by exploit value" table are now
closed. Recorded so nobody re-opens them from an old copy.

| Old row | Was | Closed by |
|---|---|---|
| 1 | `exit(0)` killswitch — *"deliberately not fixed, a policy call"* | **the owner decided to delete it** — `5e088a4` (18-Y) |
| 2 | `CGAddItemToCodeSheet` heap OOB read+write from two wire bytes | `4b76cc5` (18-T) |
| 3 | `CGConnect` pre-auth OOB table read | `4b76cc5` (18-T) |
| 4 | `CLCreatePC::toString()` `HairStyle2String` | `4b76cc5` (18-T) — which also found the worse `Slot` defect no auditor had claimed. **`CLDeletePC` was missed entirely by rows 3/4's framing** and needed `6f45414` (18-AC) |
| 5 | Assert-based bounds enforcement one `make release` from vanishing | `326c298` (18-U, 25 conversions) + `053b69f` (18-AD, 6 hoists). Residual in §2 |
| 6 | ~15 legacy `Statement`/`Result` leak sites; `MonsterKillQuest`'s `"-_-"` | `65c07ad` (18-W) — the list was missing 4 sites and wrong about `UniqueItemManager.cpp:75` |
| 7 | 7 near-miss dangling `Result` sites | `c5bebb7` (18-V) — the list was missing `FlagManager.cpp:355` |
| 8 | `CGExchangeBuy` client/server wire mismatch | `b9fac7d` — and the divergence was wider than described (ID width, not only the key) |
| 9 | Hardcoded remote DB `Connection(...)` in a GM path | `2c5ebe9` (18-S) — **already fixed before the first revision was written**, and its path was wrong (`src/Core/`, not `src/server/gameserver/`) |

---

## 2. Open, ranked by exploit value

Nothing here is a repeat of the first revision's list; that list is fully closed
(§1.3). These are new, and most surfaced *because* of the wave.

| # | Gain to attacker / cheater | Where | What it needs | Status |
|---|---|---|---|---|
| 1 | **The fix for the worst hole in the project is not on `main`.** 18-AB closes an unauthenticated internet-reachable UDP admin channel (`*shutdown`, mint gold, wipe a zone) on a **deliberately port-forwarded** port. The fix exists and is compile-verified, but sits in the 7 unmerged commits. Whichever binary is actually deployed decides current exposure — **this document cannot tell you which**, and that is the first thing to establish | `4d92150`, unmerged | merge the tail to `main` and confirm the running binaries were built from it | **open — operational, highest priority** |
| 2 | **`ExchangeService` is half-wired: stub transfers plus a decorative compare-and-swap.** `moveItemToExchangeStorage` has both its `save` and its `deleteItem` **commented out** and returns `true` (`ExchangeService.cpp:611-634`); `moveItemFromExchangeStorage` likewise transfers nothing; `claimItem` marks delivered without transferring. `ExchangeDB::markListingSold` **returns `true` unconditionally** (`ExchangeDB.cpp:185-215`), so its `AND Status = 0` compare-and-swap is decorative and two threads can both sell one listing. Harmless *today* only because `createListing`/`cancelListing`/`claimItem` have no callers outside `exchange/` — **but `buyListing` and `getListings` ARE registered**, so wiring a listing handler turns this into a straight duplication | `gameserver/exchange/ExchangeService.cpp`, `ExchangeDB.cpp` | a decision, not a hoist: finish it or unregister it | **open** (`053b69f` reported it) |
| 3 | **`ActionGiveLotto.cpp:57`: uninitialised `QuestID_t qID` reaches `questRewarded()`** — which would erase an **arbitrary in-progress quest** belonging to the player. Live data loss, not a memory-safety abstraction | `gameserver/quest/ActionGiveLotto.cpp:57` | initialise, or establish what quest it meant | **open** (`053b69f` reported it) |
| 4 | **`CGMove` field order disagrees between the trees.** The server's non-encrypted branch reads `Dir, X, Y` (`dkrixserver/src/Core/CGMove.cpp:29-31`); the client writes `X, Y, Dir` (`dkrix/Client/Packet/Cpackets/CGMove.cpp:59-61`). All three are `BYTE`, so the fields are silently permuted — the server loads the client's X into `m_Dir`. Both `SHUFFLE_STATEMENT_3` branches agree, so only the `getEncryptCode() == 0` fallback is affected, **but the client does send `CGMove` live** and it is the highest-frequency packet in the game | both trees | a coordinated two-tree wire change; decide which order is canonical | **open** (`6f45414`) |
| 5 | **`GCExchangeBuy::read()` has 18-H's arbitrary-read primitive, unfixed.** `iStream.read(m_Message)` on a `std::string` binds to the raw template `read<T>()`, reinterpreting wire bytes as a live `std::string` — attacker-chosen source pointer and length, and the wrap branch frees an attacker-chosen pointer. `write()` emits it unprefixed, so no reader could parse it either. **Latent**: `GCExchangeBuyFactory` is registered nowhere and the client does not implement the packet | `dkrixserver/src/Core/GCExchangeBuy.cpp:23` | a second both-trees wire change, same shape as 18-H's fix | **open, latent** (`b9fac7d`) |
| 6 | **`CGExchangeList` desyncs the stream.** The server reads a trailing length-prefixed seller filter (`CGExchangeList.cpp:27-30`); the client writes 19 bytes and stops, so the server would consume the *next* packet's bytes. `GCExchangeList` is effectively two different packets sharing one ID | both trees | coordinated two-tree change | **open** (`b9fac7d`) |
| 7 | **`CGUnburrow` direction persists unbounded.** 18-AC deliberately did not reject `Dir` in `read()` — `DIR_NONE` aliases `DIR_MAX`, so 8 is a live sentinel and a range check would be unsafe. The lookups are bounded, but `addUnburrowCreature` → `Creature::setXYDir` still stores an out-of-range `dir` verbatim into creature state, where it later resurfaces through `PCVampireInfo3::toString()` | `gameserver/`, `Creature.h:234` | decide the sentinel contract before bounding | **open, recorded not fixed** (`6f45414`) |
| 8 | `TradeManager` residuals: `processTrade` has a partial-commit path (items **lost**, not duped) currently unreachable behind `canTrade`; `cancelTrade` skips the partner's refund when they have left the zone | `gameserver/TradeManager.cpp` | — | **open, low** (`053b69f`) |
| 9 | Assert residual. 18-U converted the 25 wire-reachable bounds Asserts and 18-AD hoisted the 6 mutating calls, so `make release` is far less dangerous than it was — but the conversion was scoped to *wire-reachable* checks and server-side-index Asserts in `ItemInfoManager` and friends were deliberately left. `CGSMSSend`'s size caps were left too (policy limits; converting them turns soft caps into disconnects) | tree-wide | keep §5's "do not `make release` casually" until a full sweep | **open, reduced** (`326c298`, `053b69f`) |
| 10 | `initdb/DARKEDEN.sql:11568` seeds `WorldDBInfo` with Host/User/Password **as data**, read at startup by `DatabaseManager.cpp:108-134`. No env var reaches it, so it cannot be parameterised — changing the DB password requires an `UPDATE` | seed dump | documented in `.env.example` | **open by necessity** (`3dcfb1b`) |
| 11 | `DB_PORT` is hardcoded to 3306 in the conf templates and is not parameterised, so the boot script's `DB_PORT` reaches the seeding step but not the servers. CI works only because the service container happens to be on 3306 | `dkrixserver/conf/*.template` | parameterise | **open, CI/repro only** (`cc1fcd4`) |
| 12 | Both `memoryError()` handlers (login and shared `main.cpp`) `exit(0)` on a fatal OOM, so a supervisor sees a clean exit for a crash | `*/main.cpp` | cosmetic | **open, cosmetic** (`5e088a4`) |

**Reverted, and worth knowing why** (`3e02f6c`): adding the TX/TY check to
`isAbleToUseInventorySkill` looked like a security fix and **broke live
gameplay** — Bloody Mark, Bloody Tunnel and all three Transform skills never
initialise `m_TargetX/Y` in either tree (only 5 of 13 send sites set them at
all), so they put uninitialised stack values on the wire. That is why the check
was commented out upstream. No security was lost: the `Create*` skills that do
index the inventory are covered at the accessor by 18-I.

**Two changes no compiler will ever see**, carried deliberately and flagged
(`c5bebb7`, `65c07ad`): `gameserver/GuildManager.cpp`'s `init()` body is inside
`#ifdef __SHARED_SERVER__` but the file is only ever built as `__GAME_SERVER__`;
`MonsterKillQuest.cpp` is in no CMake source list. A typo in either would not
surface.

---

## 3. Investigated and REFUTED

The record of claims that turned out false. **Do not delete rows from this
table** — it exists so nobody re-chases a dead lead. Rows added by this rewrite
are marked **[new]**.

| Claim believed | What is actually true | Evidence |
|---|---|---|
| `Assert` is compiled out of the shipping build, so every Assert-based bounds check is a live exploit | **The deployed build is Debug and `Assert` throws.** `Makefile:11` is `all: debug`; `server.yml` uses `make debug`; no build tree sets `NDEBUG`; `bin/gameserver` still contains the stringified assert expressions | `MOD` packet audit [measured]. **Root cause: a wrong line in `dkrixserver/CLAUDE.md` documenting `make` as Release — it misled two independent security reviews.** Corrected in `a29ee09` |
| The live DB password is in published history (repo public since 2026-08-08) | **It was not.** The committed value was a stale 7-character credential; the live 9-character one existed only as an uncommitted working-tree modification. Any `git add -A` would have published it | `c587490`. Untracking landed (`c587490`, `0bdc648`, `3dcfb1b`); the stale value has since been rotated and swept from tracked files (`430bcc9`); history rewrite still judged unwarranted |
| Legacy `Statement`: a `const char*` from `getField()` is invalidated by `next()` | **No such hazard.** `Statement::executeQuery()` uses `mysql_store_result` (`Statement.cpp:116`), which buffers the whole set client-side; each row's storage is independent. The claim would hold only under `mysql_use_result`, never called here | `MOD` — explicitly **WITHDRAWN 2026-08-10** |
| Turning on an ASan job in CI would have caught 18-B | **A build-only ASan job catches neither 18-A nor 18-B.** ASan reports at runtime; CI never logs in. Fixed properly by adding a job that *boots* servers | `6e82736`; the boot job landed in `0733065` and first passed at `cc1fcd4` |
| 203 `Result*` sites share the bug shape → 203 candidate bugs | Shape count, not bug count. 129 files / 200 assignment sites audited → **1** live bug (18-C). The rule: `Result* p = stmt.execute();` is **safe by construction** (359 sites); only the 25 pre-declared `Result*` can break it | `MOD` audit table; `192ccfe`. Later confirmed exhaustively: tree-wide UAF candidates on a brace-accurate scan = **0** (`65c07ad`) |
| The 18-B crash was in `Statement::executeQuery()` | Wrong function. The IP offset was computed against `dmesg`'s mapped-segment base, not the ELF load base — off by `0x10000` | `MOD` correction block |
| `CLCreatePC::toString()` is never called | It is — `LoginPlayer.cpp:217` → `readPacket` → `toString` | `MOD` verification round |
| A third loginserver segfault means something is still crashing | Not attributable and **not live** — the instance that ran the end-to-end session started 12:22:12 and stayed up; nothing restarts it | `e362ca2` |
| The Phase 11 migration probably broke SQL/binds too | **Clean.** All 1,575 `PreparedStatement` constructions across 208 files audited: 0 defects. The damage was object *lifetime* (18-B, 18-C) and one malformed literal (18-A) | `MOD` bind-audit table; `655ade7`. Independently reproduced by machine: the new SQL-literal lint's bind check finds 0 defects over 1,572 literals (`0733065`) |
| ~487 legacy `executeQuery` sites remain | Roughly half are inside `/* … */`. **Corrected counts**: 489 raw hits → 261 live after stripping comments (1.87× inflation) → **172 live `createStatement()` calls**, which is the honest measure of remaining Phase 11 work (2.8× less than the raw figure) | `65c07ad` [measured] |
| **[new]** "The three unchecked `getShopItem()` dereferences remain open" (`326c298`'s own commit message, and an earlier `MOD` paragraph) | **There are four, not three, and all four are guarded.** `:106` by `isExistShopItem()` returning early at `:98`, eleven lines above; `:194`, `:466` and `:666` sit in `executeNormal`/`executeMotorcycle`/`executeEvent`, whose **only** call sites tree-wide are `:132`/`:130`/`:155` inside `execute()`, downstream of the same guard at `:98` and `:143`. The underlying hazard is real — `ItemRack::get()` returns `m_ppItem[index]` and a NULL slot is *normal* — so throwing from the accessors rather than returning a sentinel remains correct. It is the "still open" claim that was wrong | `36727b4`; `MOD` "REFUTED 2026-08-10" block [measured: three call sites, no others] |
| **[new]** 18-H's `read(m_IdempotencyKey)` is "the only such site tree-wide" [measured] | **False.** `dkrixserver/src/Core/GCExchangeBuy.cpp:23` has the identical unbounded one-argument `read()` on a `std::string`, with `write()` likewise emitting it unprefixed. Latent, not exploitable — no factory registers `GCExchangeBuy` and no client implements it — but the claim was wrong, and it was published as *measured* | `b9fac7d`; verified in-tree at `GCExchangeBuy.cpp:23`. Now §2 row 5 |
| **[new]** 18-T's fix covered `CLDeletePCHandler.cpp:73-129` | **It did not.** That handler takes a **`CLDeletePC`** packet — a different class whose `read()` was never touched. Its six `Slot2String[getSlot()]` SQL binds stayed driven by a raw 0–255 wire `BYTE` against a 3-element table, reachable from `LPS_PC_MANAGEMENT` by any authenticated account. The first revision's rows 3/4 framing missed `CLDeletePC` entirely | `6f45414` (18-AC) fixed it; `MOD` packet-audit sweep table |
| **[new]** The *GGCommand UDP gate* is **unsourced** — "get a source before acting on it" | **It was real, it was the most severe finding of the entire effort, and it is now fixed.** `GGCommandHandler.cpp:27-96` dispatched GM commands from unauthenticated UDP with `pGamePlayer = NULL`. The first revision recorded it as untraceable because neither `MOD` nor the then-21 commits mentioned it; that was a gap in the *record*, not evidence of absence. Gated in `4d92150` (18-AB) on both servers | `4d92150`. See §1.2 and §2 row 1 |
| **[new]** The SQL-injection ratchet reporting **0** meant there was no injection surface | The gate was **broken and frozen green**. Its regex forbade any `)` between the call paren and the format specifier, so it matched almost nothing; it also counted lines not call sites, ignored comments, was single-line only, and its unanchored `Statement` alternative matched `PreparedStatement(` — so *migrating toward the safe form would have raised the count*. It had been `--update`d to a baseline of 0 while broken. True count on a correct metric: **258** | `35b11b2`, `0733065`; baseline file `dkrixserver/scripts/.sql-injection-baseline` |
| **[new]** The seed dump `initdb/DARKEDEN.sql` is a working reference database | **It had never been loadable.** Duplicate primary keys in six tables, then empty `ZoneInfo` names, then empty NPC names — an English translation pass collapsing multi-byte strings to ASCII fallbacks. Verified byte-level as genuine data loss, not a decode artifact. The first repair (six `INSERT IGNORE`s) was itself wrong — it suppressed symptoms of the collapse; the dump was replaced wholesale from the intact sibling | `b86e20e` (partial, superseded), `7270626` (18-Z, partial), `cc1fcd4` (correct fix) |
| **[new]** The Docker stack worked | It had been broken and unrun since `a894d7c`: the `odk-server` container had **no `DKRIX_*` variables set at all**, so `Properties::expandEnvVars` left the placeholders literal and the servers would have failed at first SQL connect. No CI job exercised the compose path, so nothing went red | `3dcfb1b` |
| **[new]** `PacketValidator`'s `reserve()` sizing bug is worth fixing | It is **dead code server-side**: `PLAYER_STATUS_MAX` is 0 in all three binaries because its enum body is behind `#if defined(__GAME_CLIENT__)` | `35b11b2` |

---

## 4. Safety nets — state as of this rewrite

The first revision's §4 was titled "Known-broken safety nets" and was accurate
when written. Every row in it has since been rebuilt. The table below is the
**current** state; the "was" column preserves what was wrong, because that is
the part worth remembering.

| Gate | Was | Now |
|---|---|---|
| SQL-injection ratchet | Reported **0** against a baseline of 0, and the 0 was meaningless — see §3. Saw only single-line calls carrying a printf specifier; multi-line `StringStream` construction, the dominant shape here, was invisible **by construction**, as was 18-A | **Rewritten** (`35b11b2`, `0733065`). Isolates each sink's SQL argument by splitting the argument list on top-level commas, then classifies it. Re-baselined at **258** = `format=70 stream=24 splice=164`, ratcheted **per category** so a new `format` site cannot hide behind two removed `splice` sites. The `format=70` set is byte-for-byte the old gate's — nothing was reinterpreted; the other 188 sites were simply never visible. Largest cluster: `loginserver/ItemDestroyer.cpp`, 41 sites splicing an `ownerID` into a quoted literal. Baseline: `dkrixserver/scripts/.sql-injection-baseline` |
| SQL syntax lint over literals | **Did not exist.** This was the cheap gate that would have caught 18-A | **Exists**: `dkrixserver/scripts/check-sql-literals.sh` (`0733065`). Concatenates adjacent literals as the compiler would, then checks fused statements, glued keywords, multiple verbs, unbalanced quotes/parens, quoted `?`, and bind-count mismatch. **Verified against the defect it exists for**: over `9422e9c^`'s `Key.cpp` it fires three independent rules and exits 1; over the fixed file, 0. Allowlist is **empty**, so it is green on merit. Its bind check independently reproduces the manual bind audit: 0 defects over 1,572 literals |
| Server + client CI | **Never ran on this branch.** `server.yml` triggered on `[main, "modernize/**"]` and PRs to `main`; the branch matched neither and no PR existed, so 27 commits were unbuilt. Two format-only commits were written to satisfy a gate that never ran | **Branch allowlist removed from both workflows** (`0733065`) — push triggers on **every** branch, still `paths:`-filtered, `workflow_dispatch` on both. Deliberately not extended to `fix/**`: an allowlist only moves the trap to the next unanticipated prefix. PR #2 then ran the full gate for the first time and **found real format drift** (`42688d6`) — push runs diff against the previous commit, PR runs diff against `main`, so the PR was the first whole-branch check |
| `sanitizers` matrix | **Build-only.** It ran `make debug-asan` and never executed a server, so it structurally could not see 18-B/D/E/F/J/L — the entire class it appeared to cover. Two green ASan builds sat there while every login segfaulted. The `ubsan` leg was 0-for-17 | Still build-only, but joined by a **runtime** job: `boot under <san> (boot-only, no packets)` seeds a mysql:5.7 service container from `initdb/`, materialises conf from templates, and boots all three servers under the sanitizer, asserting each binds its port with no report. **It PASSES** (`cc1fcd4`) after 18-Z. Covers 18-A/D/E/J/L — 5 of 30 — and **covers nothing needing packets**; the job name says so. `ubsan` links for the first time ever (`8bb1118`) and the boot leg is a matrix axis, not a copy. Both boot legs **non-blocking** until five consecutive greens; `ubsan`'s counter is 0 |
| UBSan specifically | Red on **every run it ever had**, 17-for-17, always at link time; never reported a single UB finding | Links (`8bb1118`, one flag, order load-bearing — `docs/ci-server-ubsan.md`), boots, and **has produced real findings**: 18-AA, the first runtime UB in this project's history, including five out-of-bounds writes that worked only by luck. `halt_on_error=0` deliberately, so one boot enumerates every distinct site; detection is by report file, not exit status |
| `clang-format` job | Red on legacy files you touch — it checked **whole changed files**. Zone.cpp carried 535 pre-existing violations, `CLSelectPCHandler` 16: a bias against touching old code, which cost two format-only commits | Server now checks **changed lines**, via the same `tools/ci/clang-format-changed-lines.sh` the client has used since 2026-08-09 (`0733065`). Measured on this branch: the whole-file gate fails **29 of 43** changed server files on 871 lines of pre-existing drift; the changed-lines gate checks all 43 and passes. New code is still fully gated. `clang-format==18.1.8` stays pinned — unpinned apt gives 18.1.3, which disagrees with later versions on two CJK-comment files |

**The gap that remains, and it is the important one:** the **ASan smoke test has
not been re-run since the hardening wave**. It is the only gate in this
project's history that has caught a runtime bug — it found 18-D/E/F/J/L/N in one
sitting — and the post-wave client session (`d9b6af2`) was a plain `make debug`
build, so it is not a substitute. The CI boot smoke covers boot only, never
packets.

**A record-keeping gap, noted not fixed** (this document does not own
`MODERNIZATION.md`): `MOD` carries 18-AC, the `CGMove` field-order finding, the
`CGExchangeBuy` correction and the UBSan CI work, but **has no entry for 18-AA,
18-AB, 18-AD, or the `ExchangeService` findings** — `b9fac7d`'s message claims it
does. Those four are sourced here to their commits only. Flagging it for whoever
owns `MOD` this cycle.

---

## 5. Operational actions

### Done

1. ~~**Rotate the DB credential.**~~ **DONE.** The live value was rotated
   separately and exists only in untracked `conf/*.conf`; `git grep` confirms no
   tracked file contains it. The burned historical value was swept from tracked
   files in one coordinated byte-level pass — 55 replacements across 19 files
   plus 4 placeholder normalisations and 6 prose corrections, with CRLF, legacy
   CJK encoding and two 3.5 MB SQL dumps preserved byte-for-byte otherwise
   (`430bcc9`). The old value remains in published history; that is accepted.
2. ~~**Untrack the secret-bearing configs.**~~ **DONE** across `c587490`,
   `0bdc648` and `3dcfb1b`. `git ls-files dkrixserver/conf` now prints exactly
   the three `.conf.template` files. The pre-existing `dkrixserver/conf/*.conf`
   ignore rule reached only 8 of 10 — `*.conf` does not match `.new` and does
   not cross a `/` into `backup/` — so it was widened and verified with
   `git check-ignore` in both directions.
3. ~~**Parameterise Docker.**~~ **DONE** (`3dcfb1b`). `docker-compose.yml` and
   `initdb/a-setup.sql` take `${VAR:?message}` from `docker/.env`
   (`docker/.env.example` is the template), so compose aborts naming the missing
   variable rather than falling back to a weak default.
4. ~~**Decide the `Player::setKey` killswitch.**~~ **DONE — the owner chose to
   delete it** (`5e088a4`, 18-Y). Constants confirmed dead tree-wide; no other
   `exit(0)` is packet-reachable.
5. ~~**Get CI onto this work.**~~ **DONE** (`0733065`, PR #2). Both workflows run
   on every branch; the PR gate ran and found real drift.
6. ~~**Answer the UDP question.**~~ **Answered the hard way.** UDP 9997 *is*
   reachable off-LAN — it is deliberately port-forwarded — and it carried an
   unauthenticated admin channel (18-AB). Now gated in code on both servers
   (`4d92150`). **Firewalling is no longer the only defence, but the fix is not
   on `main` yet.**

### Open

7. **Merge the 7-commit tail to `main`** — `8bb1118`, `9cfe522` (18-AA),
   `4d92150` (**18-AB**), `6f45414` (18-AC), `053b69f` (18-AD), `b9fac7d`,
   `efe35f8`. Then **confirm the deployed binaries were built from it.** This is
   §2 row 1 and it is the highest-value action on this page.
8. **Re-run the smoke test under ASan.** Nothing in the hardening wave has been
   through it. It is cheap locally and it is the only gate that has ever caught a
   runtime bug here. Do it after the merge, not before.
9. **Re-run the servers under UBSan after 18-AA's fixes** to confirm those three
   reports are actually gone — `9cfe522` explicitly does not claim they are.
10. **Exercise the paths still never run**: NPC shop rack (no buy or sell has
    been performed), `CGAddItemToCodeSheet`, phone slots, pet stash, and the
    entire `Datagram`/`SerialDatagram` UDP path — which is exactly where 18-AB
    and 18-U's UDP bounds checks live.
11. **Decide `ExchangeService`'s fate** (§2 row 2) — finish it or unregister
    `buyListing`/`getListings`. Leaving a registered handler in front of stub
    transfers is the shape that becomes a duplication bug the day someone wires
    a UI to it.
12. **Schedule the two coordinated wire changes** — `CGMove` field order (§2 row
    4) and `GCExchangeBuy` / `CGExchangeList` (§2 rows 5, 6). House rule: both
    trees in one commit.
13. **Do not run `make release` casually.** 18-U and 18-AD removed most of the
    exposure, but the conversion was scoped to wire-reachable checks — see §2
    row 9.
14. **Do not rewrite history.** No live secret was ever committed, and a rewrite
    breaks the `archive/modernization-phases-1-17` tag and every published SHA.
