# Phase 12 residual triage — the 48-pair `>5` set (2026-08-09)

Per-pair reading of the packet pairs that stand between Phase 12's Wave 4 and
its endgame. Prepared **read-only** against `main` @ `cd8c3c6`
(worktree `worktree-agent-a44b86fb5f6606bcd`). **Nothing here has been
compiled or run** — no compiler for either tree exists in this environment,
same caveat as every prior Phase 12 wave. Every claim is `[measured]`
(file:line on this tree) or `[unverified]`.

Companion docs, cited not restated: the migration recipe and residual-class
vocabulary are `docs/MODERNIZATION.md` §Phase 12 (Waves 1–4); the population
audit is `docs/packet-divergence-2026-08-07.md`; the verdict table is
`docs/packet-normalization-sample-2026-08-07.md`; the authority model (§0)
and the 9 real-divergence briefs are
`docs/packet-protocol-review-2026-08-09.md`.

This doc **triages**. It decides nothing that changes a byte; it names what
each residual is, and which of the 48 a mechanical batch can take without a
per-pair judgement call.

---

## 0. Scope, and a correction to the Wave 4 batch 2 tally

**Measured on this tree** [measured, `python dkrixserver/scripts/normalize-packet-style.py --all --tsv`
@ `cd8c3c6`]:

```
# summary: pairs 71 | style-only 0 | style-residual<=5 9 | style-residual>5 53 | real-divergence 9
```

`check-packet-duplicates.sh --count` = **142** = 71 pairs [measured].

The Wave 4 batch 2 note in `docs/MODERNIZATION.md` records the split as
`style-residual<=5 14 | style-residual>5 48`. **That is off by five pairs, and
the five are named there.** `CGPetGamble`, `CGPickupMoney`, `CGSkillToTile`,
`CGTakeOutGood`, and `CGTameMonster` are listed as "(residual 4)"; all five
measure **residual 6** on this tree [measured, TSV rows above]. The
normalizer has not changed since it was introduced (`54b2790`, the only
commit touching `normalize-packet-style.py` [measured, `git log`]).

**Correction, 2026-08-09 — this is drift, and the batch-2 note was right when
written.** The paragraph above originally concluded "transcription error, not
drift" on the strength of `git log -- dkrixserver/src/Core/CGPetGamble.h …`
returning only the import commits. That check inspected the **server** copies
only. The residual is a property of the *pair*, so the client copies had to be
checked too — and they had changed. `cc4ea8b` ("client: path-qualify 68 bare
`Assert.h` includes for GCC", part of the Linux-port wave) rewrote
`#include "Assert.h"` → `#include "Packet/Assert.h"` in 15 `Cpackets/` files,
including all five disputed pairs, one line each [measured,
`git show --stat cc4ea8b -- dkrix/Client/Packet/Cpackets/`]. That single line
is the +2 residual per pair. So the count moved from 4 to 6 *after* the
batch-2 note was written, by a commit made for an unrelated reason in another
workstream. Found by S1 while migrating these same files.

**The transferable lesson:** a residual figure is only valid against the SHA
it was measured at, and either side of a pair can invalidate it. Include-
hygiene commits aimed at the Linux port will keep perturbing Phase 12's
numbers, because they touch client packet copies without touching protocol.
Re-derive residuals at the tip before trusting a recorded count — and when
checking whether a pair has drifted, `git log` **both** trees.

**It does not change this doc's scope.** The task defines the subject as
"everything except the 14 pairs S1 is migrating", and S1's 14 are the ones
the batch-2 note names. Removing those 14 from the measured 62
style-residual pairs leaves exactly **48**, which is also the count the plan
predicted — the membership differs from the measured `>5` set by those five
pairs, the count does not.

**In scope (48):** the 62 style-residual pairs minus S1's 14
(`CGUseMessageItemFromInventory`, `CGConnectSetKey`, `CGPetGamble`,
`CGPickupMoney`, `CGShopRequestBuy`, `CGSkillToTile`, `CGSubmitScore`,
`CGTakeOutGood`, `CGTameMonster`, `CGUsePowerPoint`, `CLDeletePC`,
`CLSelectPC`, `CGRequestRepair`, `CGShopRequestSell`).

**Out of scope:** the 9 `real-divergence` pairs — already briefed in
`docs/packet-protocol-review-2026-08-09.md`, with their own landing order.

**All 48 were read.** Nothing was skipped; §7 says so explicitly and lists
what "read" did and did not cover.

### Method

For each pair, `normalize-packet-style.py --all --emit <dir>` produced a
`<pair>.residual.diff` — the canonicalized server file against the
canonicalized client file, with the reversible style rules already folded
out. The residual is what the normalizer could *not* reconcile. Each of the
48 residual diffs was read in full; anything that touched a member, a bound,
an accessor, or an `#include` was then chased to its call sites with
`git grep` across both trees. Wire equality is the normalizer's own
`read_seq`/`write_seq`/`size_expr`/`maxsize_expr` verdict, which reads
`match` on all four for all 48 [measured, TSV `wire_reasons` column = `-`
for every in-scope row].

### Two structural pre-checks, both clean

- **No cross-family CMake pair.** `CGConnectSetKey` was skipped by Wave 4
  batch 1 because its `.cpp` sits in both `CG_PACKET_SOURCES` and
  `CL_PACKET_SOURCES`, which the per-family `shared_packets_<fam>` design has
  no slot for. **None of the 48 has that problem** — each is named exactly
  once in `dkrixserver/src/Core/CMakeLists.txt` [measured, scripted count over
  all 48 names]. The four CL pairs (`CLChangeServer` :602, `CLReconnectLogin`
  :618, `CLSelectServer` :622, `CLSelectWorld` :623) are CL-only; the other 44
  are inside `CG_PACKET_SOURCES` (:362–600) [measured].
- **No inherited divergence.** `CGUseMessageItemFromInventory` was demoted
  because it derives from a real-divergence pair. **All 48 derive directly
  from `Packet`** [measured, scripted `class <Name> : public Packet` match on
  all 48 server headers, zero exceptions].

### Shim / wiring impact: one new class of include, already covered

The distinct quoted `#include`s across all 96 server-side files of the 48
[measured, scripted]: `Packet.h` (48), `PacketFactory.h` (48), `Exception.h`
(15), `Types.h` (15), `Assert1.h` (7), `SocketEncryptInputStream.h` /
`SocketEncryptOutputStream.h` (6 each), `GamePlayer.h` (2), plus each pair's
own header. All but `GamePlayer.h` already have a forwarder in
`dkrix/Client/Packet/SharedPacketsShim/` [measured, directory listing:
`Assert1.h`, `DatagramPacket.h`, `Exception.h`, `Packet.h`, `PacketFactory.h`,
`SocketEncryptInputStream.h`, `SocketEncryptOutputStream.h`, `Types.h`].

`GamePlayer.h` is a gameserver-library header with no client counterpart, so
a shim is impossible — but **both sites are guarded** and need none:
`dkrixserver/src/Core/CGVerifyTime.cpp:10` and
`dkrixserver/src/Core/CGSay.cpp:10` wrap it in `#ifdef __GAME_SERVER__`, and
the corresponding `.h` lines (`CGVerifyTime.h:15`, `CGSay.h:15`) are
commented out entirely [measured]. `__GAME_SERVER__` is never defined in the
client build, so the merged files reach it only server-side.

**Expected new shim forwarders for all 48: zero.** [measured for the include
set; `[unverified]` that no transitive include surfaces at compile time —
only CI can settle that.]

---

## 1. Residual class catalogue

Classes already coined by Waves 1–4 keep their names and are cited to the
prior write-up rather than re-argued. New classes get an `N`-prefix and an
argument.

### Reused from Waves 1–4

| ID | Class | Where established | Wire effect |
|---|---|---|---|
| **R1** | Server-only explicit empty destructor `~X() { __BEGIN_TRY __END_CATCH_NO_RETHROW }`; client omits it | Wave 4 batch 2 (8 pairs) | none — an empty body can't throw |
| **R2** | Server-only `Handler` static method declarations (`executeOusters`/`executeCoupleRing` pattern) | Wave 4 batch 2 | none — declaration-only client-side, never ODR-used |
| **R3** | Include-set drift (client carries explicit `SocketInputStream.h`/`SocketOutputStream.h`; `Assert1.h` vs `Packet/Assert.h` spelling) | Wave 4 batch 1 ("include-set drift") | none |
| **R4** | `toString()` debug-text drift (spacing, punctuation, field selection) | Wave 4 batch 1 ("toString text") | none — debug output only |
| **R5** | Union-keep: a client-only accessor with a live client consumer, kept in the merged class | Wave 4 batch 1 (`CGConnect::setMacAddress`) | none — not read or written |
| **R6** | Whole-class `#ifdef __DEBUG_OUTPUT__` Factory guard on the client; dropped, Factory made unconditional | Wave 2 batch A; Wave 3 | none — `__DEBUG_OUTPUT__` is defined by no target in either tree |

Not observed anywhere in the 48: the always-false `BYTE > 256` validation
check (Wave 4 batches 1–2, `CGRegistGuild`/`CGJoinGuild`) [measured, scripted
scan of all 48 residual diffs].

### New in this triage

- **N1 — client-only `__BEGIN_DEBUG` / `__END_DEBUG` body wrapper.** Six
  pairs wrap the bodies of `read()`, `write()`, `execute()` and `toString()`
  in this macro pair; the server copies do not.
  **Wire-neutral, proven:** the macros never remove the statements they
  bracket. On Windows they are `((void)0);` [measured,
  `dkrix/Client/Packet/Exception.h:157-159` — `#if defined(NDEBUG) ||
  defined(__WIN32__) || defined(PLATFORM_WINDOWS)`]; on the other branches
  they are `try {` … `} catch (Throwable&) { …; throw; }` [measured,
  `Exception.h:160-178`]. Both forms execute the wrapped statements
  unconditionally. The server tree defines the same macro identically
  [measured, `dkrixserver/src/Core/Exception.h:166-183`]. Adopting the
  server's unwrapped form loses only a redundant catch-and-rethrow nested
  inside the enclosing `__BEGIN_TRY`.
  Example: `dkrix/Client/Packet/Cpackets/CGStashDeposit.cpp:20,24,36,40,52,58,70,80`.
- **N2 — declaration reordering.** Members or inline accessors declared in a
  different order between the trees, with `read()`/`write()` order unchanged.
  The wire is set by the explicit stream calls, never by declaration order,
  so this is inert. Example: `CGAbsorbSoul` moves `m_TargetZoneX`/`Y` to the
  end of the member list [measured, `CGAbsorbSoul.residual.diff`], read order
  unchanged (`read_seq: match`).
- **N3 — double-guarded duplicate `getPacketName`/`toString`.** The client
  copy declares the pair **twice**: once inside `#ifndef __GAME_CLIENT__` and
  again inside `#ifdef __DEBUG_OUTPUT__`. Neither macro is defined in the
  client build, so the client currently compiles *neither*; the normalizer's
  guard-fold emits both, which is what shows up in the residual as an
  apparent duplicate definition. Example:
  `dkrix/Client/Packet/Cpackets/CGUnburrow.h:44-58` [measured]. Adopting the
  server's single unconditional pair is the established recipe and is
  behaviourally neutral on both sides.
- **N4 — client-only Handler method declarations, never defined.** The
  mirror image of R2. `CGMouseToStashHandler::executeSlayer`/`executeVampire`
  [measured, `dkrix/Client/Packet/Cpackets/CGMouseToStash.h:81-82`] and the
  same pair on `CGStashToMouse` [measured, `…/CGStashToMouse.h:85-86`] are
  declared client-side and **defined nowhere in either tree** [measured,
  tree-wide `grep`, zero definitions]. Dropping them with the server form is
  safe.
- **N5 — arithmetic-form drift in a size expression, value-identical.**
  `szObjectID + szZoneCoord + szZoneCoord + …` vs
  `szObjectID + szZoneCoord*2 + …` (`CGAbsorbSoul`); `szDWORD + szBYTE + 128`
  vs `szBYTE + 128 + szuint` (`CGPartySay`). `szDWORD == szuint == 4`
  [measured, protocol-review §0]. Same value, different spelling.
- **N6 — factory `getPacketMaxSize()` bound drift, value-different.** One
  instance: `CGBloodDrain`. Server returns `szObjectID` (4) [measured,
  `dkrixserver/src/Core/CGBloodDrain.h:126-128`]; client returns
  `szObjectID + szCoord + szCoord + szDir` (7) [measured,
  `dkrix/Client/Packet/Cpackets/CGBloodDrain.h:122`]. `getPacketSize()` is
  `szObjectID` on both sides [measured, server `:54-56`, client `:56`]. Under
  §0's model the **server's factory value is the live bound** (it frames the
  incoming stream) and the client's is never consulted, so adopting the
  server form is a byte-for-byte no-op. Adopting the client's would loosen a
  live check. **Merge rule: take the server value, explicitly.**
- **N7 — dead-name enum rename, identical values.** `CGGetEventItem`'s enum
  is `EVENT_TYPE_200412_*` client-side and `EVENT_TYPE_200501_*` server-side,
  with identical ordinals (`=0`, then two, then `_MAX`) [measured,
  `CGGetEventItem.residual.diff`]. Every consumer in the tree uses the
  **server** spelling [measured: `CGGetEventItemHandler.cpp:36,38,40` and
  `gameserver/quest/ActionShowConfirmGetEventItemDialog.cpp:51,53,55`]; the
  client has **zero** consumers of any `EVENT_TYPE_*` name. Adopting the
  server form needs no rename sweep at all.
- **N8 — accessor return-type narrowing on the client.** `CGAbsorbSoul`'s
  client copy declares `Coord_t getTargetZoneX()` over a `ZoneCoord_t`
  member; `Coord_t` is `BYTE`, `ZoneCoord_t` is `WORD` [measured,
  `dkrixserver/src/Core/types/ZoneTypes.h:52`,
  `dkrix/Client/Packet/Types/ZoneTypes.h:44`, plus §0's width table]. The
  getter truncates. It is **never called client-side** — the only call sites
  in the tree are `dkrixserver/src/Core/CGAbsorbSoulHandler.cpp:58-59`
  [measured]. `write()` uses the member directly, so the wire was never
  affected. Adopting the server's `ZoneCoord_t` is a strict fix with a
  zero-site blast radius.
- **N9 — `Assert` bound/spelling drift inside the server-live `read()`.**
  `CGSMSSend` bounds with `<=` server-side and `<` client-side; `CGMixItem`
  spells the macro `Assert` server-side and lowercase `assert` client-side.
  For a `CG` packet the client's `read()` is vestigial (§0), so the server
  form is the only one that has ever run. The client tree defines `Assert`
  compatibly [measured, `dkrix/Client/Packet/Assert.h:24-32`], reached by the
  merged file through the existing `SharedPacketsShim/Assert1.h` forwarder
  [measured, that file's body is `#include "../Assert1.h"`].
- **N10 — client-only constructor member initialization.** The client copy
  has a constructor that zeroes a member; the server's constructor is empty
  or absent. **This class splits, and the split is the whole point of reading
  it:** if the member is *not* on the wire it is inert (mechanical); if it
  *is* on the wire, the client's initializer is load-bearing and dropping it
  changes the bytes the client sends. Both cases occur in the 48. Inert:
  `CGAddInventoryToMouse` / `CGAddMouseToInventory`, whose
  `m_InventoryItemObjectID` is read and written **nowhere** — every stream
  call naming it is commented out on both sides [measured, client
  `…/Cpackets/CGAddInventoryToMouse.cpp:50,69` and
  `…/CGAddMouseToInventory.cpp:48,67`; server
  `dkrixserver/src/Core/CGAddInventoryToMouse.cpp:25,38` and
  `…/CGAddMouseToInventory.cpp:26,39`]. Load-bearing: `CGDissectionCorpse`,
  §3.1.
- **N11 — client-only Factory/Handler default constructor.** An explicit
  `X(){}` on the Factory (`CGRequestIP`) or the Handler
  (`CGStashRequestBuy`). Inert; the implicit one is identical.
- **N12 — client-only `static getPacketMaxSize()` on the packet class.**
  `CGRequestIP`'s client header adds a `static PacketSize_t
  getPacketMaxSize()` returning `szBYTE + 10` alongside the (unchanged)
  factory override. It is called from nowhere [measured, tree-wide `grep` for
  `CGRequestIP` outside the pair itself finds only construction, `setName`,
  `sendPacket` and factory registration: `ProfileManager.cpp:343-346`,
  `WhisperManager.cpp:190-193,425-428`, `PacketFactoryManager.cpp:358,755`].
  Dropping it is inert.
- **N13 — member type spelling drift.** `DWORD m_Color` vs `uint m_Color`
  (`CGPartySay`); `HP_t m_MaxHP, m_HP;` on one line vs two. Both spellings
  are 4 bytes / same type [§0's width table]. Inert.
- **N14 — server-only statement in `read()`.** `CGTypeStringList`'s server
  `read()` calls `clearString()` before filling the list [measured,
  `dkrixserver/src/Core/CGTypeStringList.cpp:25`]; the client's does not.
  `read()` is server-live for a `CG` packet, and the method exists on both
  sides [measured, server `.h:66`, client `.h:64`], so adopting the server
  form preserves exactly today's server behaviour.

---

## 2. Per-pair verdicts

Residual is the normalizer's line count on this tree. `wire` reads `match` on
all four signature fields for every row [measured].

### 2.1 mechanical — 45 pairs

| Pair | Residual | Residual classes | Note |
|---|---|---|---|
| CGNPCTalk | 6 | R3, R4 | — |
| CGRequestPowerPoint | 6 | R1, R4 | — |
| CGMakeItem | 6 | R1, R3 | — |
| CGSkillToNamed | 6 | R1, N2 | `toString()` decl moved |
| CGUnburrow | 6 | N3 | — |
| CLSelectServer | 6 | N2 | — |
| CLSelectWorld | 6 | N2 | — |
| CGVerifyTime | 7 | N3, R2 | server-only `saveSpeedHackPlayer`, defined at `CGVerifyTimeHandler.cpp:46` [measured] |
| CGAddGearToMouse | 8 | R1, R3, R4 | — |
| CGAddMouseToGear | 8 | R1, R3, R4 | — |
| CGAddMouseToQuickSlot | 8 | R1, R3, R4 | — |
| CGAddQuickSlotToMouse | 8 | R1, R3, R4 | — |
| CGCastingSkill | 8 | R1, R3, R4 | — |
| CGDialUp | 8 | R1, R3, R4 | — |
| CGPhoneDisconnect | 8 | R1, R3, R4 | — |
| CGReloadFromInventory | 8 | R1, R3, R4 | — |
| CGReloadFromQuickSlot | 8 | R1, R3, R4 | — |
| CGUsePotionFromQuickSlot | 8 | R1, R3, R4 | — |
| CGTypeStringList | 8 | R1, R2, N14 | enum trailing comma; `executeApartForce` defined at `CGTypeStringListHandler.cpp:160` [measured] |
| CGUsePotionFromInventory | 8 | R1, R3 | server declares an out-of-line ctor **and defines it**, `CGUsePotionFromInventory.cpp:14` [measured] — no link gap |
| CGGetEventItem | 9 | R2, N7 | client's Handler class is empty; server's `execute` + three `__GAME_SERVER__` helpers adopted |
| CGStashRequestBuy | 9 | R3, N1, N11 | — |
| CGBloodDrain | 10 | R1, R3, R4, **N6** | **take the server's `getPacketMaxSize`** |
| CGSelectPortal | 10 | R1, R3, R4, N2 | — |
| CGUseBonusPoint | 10 | R1, R3, R4, N2 | — |
| CGStashDeposit | 11 | N1 | — |
| CGStashList | 11 | R3, N1 | — |
| CGStashWithdraw | 11 | N1 | — |
| CLChangeServer | 11 | R4, N2 | client moves `toString()` out of line |
| CGMixItem | 12 | R2, R3, N9 | three `__GAME_SERVER__` Handler helpers; `Assert`/`assert`; `CGMixItem.h:12` includes `Assert1.h` at header scope [measured] — shim already covers it |
| CGRideMotorCycle | 12 | R1, R3, N2 | — |
| CGSkillToObject | 12 | R1, R3, N2 | — |
| CGSay | 13 | R2 | largest header in the set; see §3.4 |
| CGThrowItem | 14 | R1, R3, R4, N2 | — |
| CGAddInventoryToMouse | 15 | R1, R3, R4, **N10**, **R5** | member exists server-side with its accessors commented out [measured, `dkrixserver/src/Core/CGAddInventoryToMouse.h:45-47,67`]; keep the client's accessors and ctor init — live consumer `dkrix/Client/UIMessageManager.cpp:10868` [measured] |
| CGMouseToStash | 15 | R3, N1, **N4** | — |
| CGAddMouseToInventory | 16 | R1, R3, R4, **N10**, **R5** | member is **commented out** server-side [measured, `dkrixserver/src/Core/CGAddMouseToInventory.h:66`], so the merged class gains it; keep the client's accessors and ctor init — live consumer `dkrix/Client/UIMessageManager.cpp:10807` [measured] |
| CGRelicToObject | 16 | R1, R2, N2 | five `private:` Handler helpers, server-only |
| CGStashToMouse | 17 | R3, R4, N1, **N4** | the **server** `toString()` prints `"CGMouseToStash("` [measured, `dkrixserver/src/Core/CGStashToMouse.cpp:47`] — a copy-paste bug the client copy does not have [measured, `…/Cpackets/CGStashToMouse.cpp:79`]. The one place in the 48 where "adopt the server's form" carries a defect forward; take the client's string. Debug output only |
| CGGetOffMotorCycle | 18 | R1, R3, N3 | — |
| CGSkillToSelf | 18 | R1, R3, N3 | — |
| CGPartyPosition | 23 | R1, R4, **R5**, N13 | `setHP` arity union; see §3.2 |
| CGPartySay | 28 | R1, R4, N5, N13 | — |
| CGRequestIP | 29 | R1, R3, R4, N11, **N12** | — |
| CGAbsorbSoul | 36 | R1, N2, N5, **N8** | — |

### 2.2 needs-decision — 3 pairs

| Pair | Residual | What the decision is | Authoritative side |
|---|---|---|---|
| CGDissectionCorpse | 11 | keep or drop the client ctor's `m_IsPet = 0` | **client** |
| CGSMSSend | 27 | which `MAX_MESSAGE_LENGTH` the shared header defines (80 or 40) | **server** |
| CLReconnectLogin | 11 | which accessor surface the merged class exposes over the wire-live `m_LoginMode` | **client write, server read — they disagree** |

### 2.3 blocked — 0 pairs

Nothing in the 48 waits on anything unlanded. Both blocking mechanisms that
stopped earlier pairs — the cross-family CMake gap (`CGConnectSetKey`) and
inherited divergence (`CGUseMessageItemFromInventory`) — were checked against
all 48 and are absent (§0).

---

## 3. The needs-decision briefs

### 3.1 CGDissectionCorpse — a wire byte that only the client initializes

**The divergence.** The client's constructor sets `m_IsPet = 0` [measured,
`dkrix/Client/Packet/Cpackets/CGDissectionCorpse.cpp:13-19`]. The server's
constructor is empty — `CGDissectionCorpse::CGDissectionCorpse() {__BEGIN_TRY
__END_CATCH}` [measured, `dkrixserver/src/Core/CGDissectionCorpse.cpp:14-16`].
The residual diff renders this as a ctor/dtor swap because the normalizer's
`drop_empty_ctor_dtor` rule removed the server's empty ctor and kept the
client's non-empty one.

**Why it matters.** `m_IsPet` is on the wire in both directions: the client
writes it [measured, `…/Cpackets/CGDissectionCorpse.cpp:72,80` — encrypt and
plain paths] and the server reads it [measured,
`dkrixserver/src/Core/CGDissectionCorpse.cpp:43,61`] and branches on it in
six places [measured, `CGDissectionCorpseHandler.cpp:214,277,327,333,338,354,598`].
The client sets it explicitly in exactly **one** place —
`dkrix/Client/MPlayer.cpp:9188`, `_CGDissectionCorpse.setPet(true)`
[measured, tree-wide `grep` for `setPet` on this class]. Every other
dissection the client sends relies on the constructor's zero.

**The decision.** Adopting the server's canonical form — the default for
every prior wave — would leave `m_IsPet` **indeterminate** on every non-pet
dissection the client sends, and the server branches on it. Under §0's model
the client's `write()` is the authority, and the client's `write()` is
defined only because that constructor runs. **Keep the client's constructor
body verbatim in the merged file.** Cost on the server: an added `m_IsPet =
0` before a read that overwrites it — no observable change.

**Blast radius if wrong:** looted-corpse behaviour and pet-corpse handling go
non-deterministic in the field. Would **not** be caught by either CI leg, and
might not reproduce in a single Phase 18 smoke pass — an uninitialized
stack byte is frequently zero. This is the one pair in the 48 where following
the standard recipe is actively wrong.

### 3.2 CGSMSSend — a shared macro whose value differs, over a server-live bound

**The divergences,** all in the header and the server-live `read()`:

- `#define MAX_MESSAGE_LENGTH` is **80** server-side and **40** client-side
  [measured, `CGSMSSend.residual.diff`; `MAX_NUMBER_LENGTH 11` and
  `MAX_RECEVIER_NUM 5` agree].
- The server bounds with `Assert(size <= MAX_*)`, the client with
  `Assert(size < MAX_*)` [measured, server `CGSMSSend.cpp:22,29,35,39`;
  client `…/Cpackets/CGSMSSend.cpp:21,29,34,38`] — class N9.
- The client's `read()` omits `m_Numbers.push_back(number)`, so its loop
  consumes the bytes and discards them [measured, residual diff]. Vestigial:
  `read()` is server-side for a `CG` packet.
- Client-only `addString(string)` / `clearString()` accessors, **with live
  consumers** — `dkrix/Client/UIMessageManager.cpp:9684,9689` [measured].
  Class R5, union-keep, no wire effect: `write()` iterates `m_Numbers`, and
  `write()` is byte-identical between the trees [measured — the residual diff
  contains no `write()` hunk].

**The decision.** The macros have **no consumer anywhere outside this pair**
[measured, tree-wide `grep` for all three names finds only the two `.cpp`
copies], so unifying the name is free of call-site work — but the *value*
picks the server's live inbound bound. **Define `MAX_MESSAGE_LENGTH` as 80**
(the server's) and keep the server's `<=` comparisons: that is byte-for-byte
today's server behaviour. Choosing 40, or the client's `<`, would make the
server start rejecting SMS bodies in the 40–79 byte range that it accepts
today — a live behaviour change dressed as a style merge. Keep `addString`
and `clearString` in the merged class alongside the server's `read()`.

**Blast radius if wrong:** silently narrower SMS messages, server-side, with
an `Assert` that compiles to `((void)0)` in release builds [measured,
`Assert.h:24`] — i.e. a truncation with no diagnostic.

### 3.3 CLReconnectLogin — two incompatible readings of one wire byte

**The divergence.** Same member, two APIs:

- Server: `void setWebLogin() { m_LoginMode = LOGIN_MODE_WEBLOGIN; }` and
  `bool isWebLogin() const { return m_LoginMode == LOGIN_MODE_WEBLOGIN; }`
  [measured, residual diff; the identical pair exists on `CLLogin` at
  `dkrixserver/src/Core/CLLogin.h:104-108`].
- Client: `void SetLoginMode(BYTE n) { m_LoginMode = n; }` and
  `BYTE GetLoginMode() { return m_LoginMode; }` [measured, residual diff].

Both sides have live consumers. Client:
`dkrix/Client/Packet/Gpackets/GCReconnectLoginHandler.cpp:86` sends
`_CLReconnectLogin.SetLoginMode(g_pUserInformation->IsAutoLogIn)` — an
**arbitrary BYTE**, not a `LOGIN_MODE_*` enumerator [measured]. Server:
`dkrixserver/src/Core/CLReconnectLoginHandler.cpp:49` does
`pLoginPlayer->setWorldID(pPacket->isWebLogin())` — it feeds the *boolean*
`m_LoginMode == 1` into a **world ID** setter, with the enum-based
`setWebLogin` call commented out on the next line [measured, `:50`].

The wire itself is agreed: `read_seq`/`write_seq`/`size_expr` all `match`,
and `m_LoginMode` is one BYTE either way. What diverges is what the two
sides *believe* the byte means — client "auto-login flag", server "web-login
enum, reinterpreted as a world ID".

**The decision, and it is not purely mechanical.** Two separable questions:

1. *Accessor surface.* Cheapest safe answer: keep **both** in the merged
   class — they are non-conflicting names over the same member, so this is a
   pure union-keep (R5) with zero call-site edits on either side. Choosing
   only the server's pair forces a rewrite of
   `GCReconnectLoginHandler.cpp:86`, which cannot be expressed with
   `setWebLogin()` at all (it passes a value, not a flag).
2. *`setWorldID(isWebLogin())`.* This looks like a bug and is almost
   certainly one — but it is **live server behaviour** and squarely inside
   the "preserve observable behavior unless explicitly approved" rule.
   **Do not touch it in the migration.** It lives in
   `CLReconnectLoginHandler.cpp`, which the migration does not move, so
   leaving it alone costs nothing.

Also in this pair's residual: a server-only Handler declaration
`onChildGuardTimeArea(int,int,string)` (class R2) and `toString()` text drift
(R4). Both inert.

**Blast radius if wrong:** the reconnect path — the client's auto-login flow.
Failure would be visible immediately in a Phase 18 smoke test, unlike §3.1's.

### 3.4 Not a decision, but read it before you batch it — CGSay

`CGSay` is the largest header in the 48 and the only one whose residual is
made almost entirely of R2. The server's `CGSayHandler` declares ~40 GM
`op*` static methods; the client's copy wraps a subset in `#ifdef
__GAME_SERVER__` and drops nine of them, and gives `opguild` a
three-parameter signature where the server has four [measured, residual
diff]. This is the `CGModifyGuildIntro` precedent from Wave 2 batch A
inverted: adopting the **client's** guard would delete declarations the
server defines and calls. **Adopt the server's unconditional form**, which is
dead client-side (nothing defines or calls them there) but required
server-side.

Two things that would otherwise be structural risks, checked and clear:
`CGSay.h` forward-declares `Player`, `Creature` and `GamePlayer` at file
scope in **both** trees [measured, server `CGSay.h:29-31`; client copy shows
no residual on those lines], so the `Creature*`/`GamePlayer*` parameter types
resolve without `GamePlayer.h`; and the real `#include "GamePlayer.h"` sits
behind `#ifdef __GAME_SERVER__` in the `.cpp` [measured, `CGSay.cpp:10`].
`CGSayHandler.cpp` carries 16 SQL call sites [measured, `grep -c`] — the
Phase 11.2 workstream's territory — but that file **is not moved** by this
migration, and Phase 11.2 closed out on 2026-08-09
(`check-sql-injection.sh` baseline 0), so the Wave-1 exclusion on `CGSay` has
lapsed the same way it did for `CGWhisper` in Wave 3.

---

## 4. Recommended batch order

Ordered so that each batch introduces at most one unfamiliar class, and the
three decisions land last and separately. Sizes match the 12–15 the recent
waves have sustained.

**Batch 5A — 12 pairs, zero new classes.** Pure R1/R3/R4. Structurally
identical to Wave 4 batch 2; nothing to read per pair beyond confirming the
`--pair` verdict.

> CGAddGearToMouse, CGAddMouseToGear, CGAddMouseToQuickSlot,
> CGAddQuickSlotToMouse, CGCastingSkill, CGDialUp, CGPhoneDisconnect,
> CGReloadFromInventory, CGReloadFromQuickSlot, CGUsePotionFromQuickSlot,
> CGNPCTalk, CGRequestPowerPoint

**Batch 5B — 12 pairs, adds N2 and N3.** Declaration reordering and the
double-guard duplicate. Both are read-once-then-mechanical.

> CGMakeItem, CGSkillToNamed, CGUnburrow, CGVerifyTime, CLSelectServer,
> CLSelectWorld, CGSelectPortal, CGUseBonusPoint, CGThrowItem,
> CGRideMotorCycle, CGGetOffMotorCycle, CLChangeServer

Note: this is the first batch with CL pairs since Wave 3. `shared_packets_cl`
already exists and already links into `LoginServerPackets`, so it is sources
only — no new target or link wiring [measured, `shared/Packets/CMakeLists.txt`].

**Batch 5C — 12 pairs, adds N1, N4, N14, N7.** The `__BEGIN_DEBUG` stash
family plus the Handler-declaration classes in both directions. N1 is the
class most worth re-reading §1 for before starting.

> CGStashRequestBuy, CGStashDeposit, CGStashList, CGStashWithdraw,
> CGMouseToStash, CGStashToMouse, CGGetEventItem, CGTypeStringList,
> CGRelicToObject, CGMixItem, CGSkillToObject, CGSkillToSelf

**Batch 5D — 9 pairs, the ones with a named per-pair merge rule.** Every
row here has a note in §2.1; none needs a *decision*, but each needs its note
honoured. `CGSay` is deliberately last-in-batch.

> CGUsePotionFromInventory, CGAbsorbSoul, CGBloodDrain (N6: server value),
> CGPartySay, CGRequestIP, CGAddInventoryToMouse, CGAddMouseToInventory,
> CGPartyPosition, CGSay

**Batch 5E — 3 pairs, one commit each, after §3 is signed off.** Not a
batch in the mechanical sense.

> CGDissectionCorpse, CGSMSSend, CLReconnectLogin

Order within 5E: `CGSMSSend` → `CLReconnectLogin` → `CGDissectionCorpse`.
Rationale: the first two fail loudly (compile break, or an immediately
visible reconnect/SMS regression); `CGDissectionCorpse` is the one whose
failure mode is silent and intermittent, so it should land against a tree
whose Phase 18 smoke test has already been run, not before.

**Ratchet arithmetic.** 142 → 118 (5A+5B) → 94 (5C) → 76 (5D) → 70 (5E).
Combined with S1's 14 pairs (142 → 114 on its own line), the two lines
together take the ratchet to **18** — the 9 real-divergence pairs, and
nothing else. [unverified — arithmetic only; the two lines have not been
reconciled and both are unmerged.]

---

## 5. Counts

| Verdict | Pairs |
|---|---|
| **mechanical** | **45** |
| **needs-decision** | **3** |
| **blocked** | **0** |
| **total in scope** | **48** |

Not reached: **none**. All 48 residual diffs were read in full.

---

## 6. What a merge still has to do that this doc does not cover

Triage is not the recipe. Every pair still needs, per the Wave 1–4
write-ups and unchanged by anything here:

- the per-pair `--pair <Name>` re-verification immediately before touching
  files (this doc's numbers are from one run at `cd8c3c6`);
- the `#ifndef __GAME_CLIENT__` Handler-guard choice, replicated from each
  pair's own client copy rather than normalized — this doc did **not**
  classify guard variants, because Waves 1–4 established that they are read
  per pair at merge time, not batched;
- the `dkrix/Client/CGHandlersStub.cpp` check (a pair with a stub entry keeps
  its Handler unconditional);
- the indirect-consumer sweep — `git grep -i` for every old
  `Cpackets/<Name>.h` path in all case and slash variants across the whole
  tree, which has found a real CI-red gap in four of the six waves so far;
- post-merge re-verification that the merged file's canonicalized wire
  signature equals both originals'.

---

## 7. Boundaries of this reading

State plainly what "read all 48" did and did not mean.

**Done for all 48:** the full canonicalized residual diff was read; the
normalizer's four wire-signature fields were confirmed `match`; the pair's
base class and its CMake family membership were checked by script; the
server-side include set was enumerated by script and reconciled against the
existing shim directory.

**Done selectively — where the residual touched a member, bound, accessor or
enum:** call-site chase with `git grep` across both trees. That covers every
pair carrying R2, R5, N4, N6, N7, N8, N9, N10, N12 or N14 (18 pairs), plus
`CGSay`. Pairs whose residual is only R1/R3/R4/N1/N2/N3/N11/N13 got no
call-site chase, because those classes cannot reach a call site by
construction — they are destructors, includes, debug text, declaration
order, guard duplicates, defaulted constructors, and type spellings.

**Not done at all, for any pair:** compiling, running, or observing a byte on
a wire. The `match` verdicts are the normalizer's text-level analysis of the
`read()`/`write()`/size expressions, not an execution trace. The one
conclusion in this doc that a compiler could contradict is the "zero new shim
forwarders" claim in §0, which rests on a direct-include enumeration and says
nothing about transitive includes. Both trees' CI remains the gate, exactly
as it has for the pilot and Waves 1–4.
