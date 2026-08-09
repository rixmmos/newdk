# Packet protocol-review briefs — the 9 real-divergence pairs (2026-08-09)

Phase 12 (packet unification) decision briefs, one per pair, for sign-off
**pair-by-pair** before any landing. Prepared read-only against `main`
@ `96a1995` (worktree `agent/w4-briefs`). Nothing here has been compiled or
run; every claim is `[measured]` (file:line on this tree) or `[unverified]`.

Companion measurement docs (do not restate here): the verdict table and
per-pair readings are `docs/packet-normalization-sample-2026-08-07.md`; the
population audit is `docs/packet-divergence-2026-08-07.md`. This doc *decides*
what those two *measured*.

---

## 0. How to read these briefs (authority model — applies to all 9)

**Packet direction decides which function is the live wire.** Naming prefix:
`CG` = client→gameserver request, `CL` = client→loginserver request,
`GC`/`LC` = server→client. For a request packet the client **only** calls
`write()` (send) and the server **only** calls `read()` (receive); the client's
`read()` and the server's `write()` are vestigial mirrors kept for symmetry and
debug `toString()`. So the authoritative wire for every pair below is:

> **client `write()` bytes, as consumed by server `read()`.**

**The size formula is itself on the wire — on the client side.** The client
stamps `getPacketSize()` into every outgoing header:
`Packet::writeHeaderNBody()` writes `getPacketID()`, then `getPacketSize()`,
then the body [measured, `dkrix/Client/Packet/Packet.h:598-609`]. The server
reads that header size field, bounds-checks it against the **factory**
`getPacketMaxSize()`, and uses it to frame/skip the next packet [measured,
`dkrixserver/src/server/gameserver/GamePlayer.cpp:378-379,419,428`]. Therefore:

- A wrong **client** `getPacketSize()` mis-frames the stream directly.
- A wrong **server** per-packet `getPacketSize()` is *latent for a request
  packet* (server never sends it) — **but** if the two files are unified into
  one shared class, the client inherits whatever formula the merge picks. So
  "pick the server copy" is unsafe: for the size pairs below the server's own
  `getPacketSize()` is the stale artifact while both sides' *streams* agree.
  **The streams are the authority, not either header.** Verified per pair.

**Scalar widths are identical across trees** [measured, `types/*.h` both sides]:
`ObjectID_t=DWORD` (4), `SkillType_t=WORD` (2), `Coord_t=Dir_t=Race_t=CoordInven_t=BYTE`
(1), `szBYTE=1`, `szuint/szint/szDWORD=4`, `szWORD=2`. No divergence is a width
mismatch; all are field-count, field-order, or formula differences.

**Risk grades:** `land-now` (layout-equal, wire-identical, pure rename/style) ·
`land-after-smoke-test` (wire-equal today but a behavior/validation nuance or a
latent-branch risk wants a runtime look) · `needs-live-window` (behavior or
wire changes; must wait for Phase 18 or a live test).

**Verdict summary**

| # | Pair | Divergence kind | Authoritative side | Wire-live today? | Grade |
|---|---|---|---|---|---|
| 1 | CGGuildChat | member rename (`m_Type`/`m_bType`, both BYTE) | tie (layout-equal) | yes, equal | **land-now** |
| 2 | CGModifyNickname | member rename (`m_ItemObjectID`/`m_NicknameID`, both ObjectID_t) | tie (layout-equal, same meaning) | yes, equal | **land-now** |
| 3 | CLCreatePC | size formula (`szRace`) | **client** | yes, equal | land-after-smoke-test |
| 4 | CGSkillToInventory | size formula (double `szObjectID`) | **client** | yes, equal | land-after-smoke-test |
| 5 | CGUseItemFromInventory | size formula (double `szObjectID`) | **client** | yes, equal | land-after-smoke-test |
| 6 | CGMove | plain-branch field order (Dir,X,Y vs X,Y,Dir) | **client** | encrypt path equal; plain path latent bug | needs-live-window |
| 7 | CLLogin | server-only fields + client netmarble branch + pw bound | mixed | normal path equal | needs-live-window |
| 8 | CGExchangeBuy | server protocol ahead (uint64 + idempotency key) | server (design) | **dead-UI** on client | needs-live-window |
| 9 | CGExchangeList | server protocol ahead (seller filter, len-prefix) | server (design) | **dead-UI** on client | needs-live-window |

**Recommended landing order:** 1 → 2 → (Phase 18) → 3 → 4 → 5 → 6 → 7 →
8 → 9. Rationale in §11.

---

## 1. CGGuildChat  — grade: land-now

Guild/union chat request (client→gameserver). Files:
`dkrixserver/src/Core/CGGuildChat.{h,cpp}` vs
`dkrix/Client/Packet/Cpackets/CGGuildChat.{h,cpp}`.

**1. Byte layout** (identical both sides)

| Order | Field | Type | Bytes | Server | Client |
|---|---|---|---|---|---|
| 1 | type | BYTE | 1 | `m_Type` | `m_bType` |
| 2 | color | uint | 4 | `m_Color` | `m_Color` |
| 3 | msg length | BYTE | 1 | `szMessage` | `szMessage` |
| 4 | message | char[len] | len (≤128) | `m_Message` | `m_Message` |

Size `= szBYTE + szuint + szBYTE + m_Message.size()` on **both** sides
[measured, server `CGGuildChat.h:39-43`; client `CGGuildChat.h:39-45`].
Read order (`type`,`color`,`szMessage`,`msg`) and write order identical
[measured, server `.cpp:14-15,38-51`; client `.cpp:14-15,38-51`]. Both bound the
message at 128.

**2. Authority.** True layout-equality — the only difference is the member
*name* `m_Type` (server) vs `m_bType` (client), both `BYTE` [measured, server
`.h:78`; client `.h:67`]. Neither header is stale. Semantics agree: client sends
`SetType(0)` for guild / `SetType(1)` for union [measured,
`UIMessageManager.cpp:1957-1959`]; server handler consumes `pPacket->getType()`
[measured, `CGGuildChatHandler.cpp:46`].

**3. Proposed merged form.** Adopt the server file into `shared/Packets/`;
keep member name `m_Type` and getter `getType()`; fold the client's
`__DEBUG_OUTPUT__`/`__GAME_CLIENT__` guards per the Wave-1–3 recipe. Rewrite the
client's `SetType`/`GetType` call sites (`UIMessageManager.cpp`) to
`setType`/`getType`. Size formula unchanged (agreed). No wire change.

**4. Blast radius if wrong.** None on the wire — layout is provably equal. The
only failure mode is a *compile* break if a `SetType`/`GetType` call site is
missed in the rename sweep; caught by client CI, never reaches a tester. Would
not show in the Phase 18 smoke test because guild chat bytes are unchanged.

**5. Recommendation.** **Land now, first of the nine** — it is the safest
possible exercise of the review→land loop (rename + guard fold, zero wire
delta). Proves the pipeline before any real-wire pair is touched.

---

## 2. CGModifyNickname — grade: land-now

Nickname-change request (client→gameserver). Consumes a nickname item.

**1. Byte layout** (identical both sides)

| Order | Field | Type | Bytes | Server | Client |
|---|---|---|---|---|---|
| 1 | item object ID | ObjectID_t | 4 | `m_ItemObjectID` | `m_NicknameID` |
| 2 | nickname length | BYTE | 1 | `szSTR` | `szSTR` |
| 3 | nickname | char[len] | len (≤`MAX_NICKNAME_SIZE`) | `m_Nickname` | `m_Nickname` |

Size `= szObjectID + szBYTE + m_Nickname.size()` both sides [measured, server
`.h:31`; client `.h:30`]. `getPacketMaxSize = szObjectID + szBYTE +
MAX_NICKNAME_SIZE` both sides [measured, server `.h:74`; client `.h:57`].

**3-vs-2 read/write nuance (wire-equal):** server guards the string with
`if (szSTR != 0)` [measured, `.cpp:28-29,44-45`]; client reads/writes the string
unconditionally [measured, `.cpp:31-32,47`]. For `szSTR == 0` the client's
`read(m_Nickname, 0)` / `write(m_Nickname)` move zero bytes, so the wire is
byte-identical either way.

**2. Authority.** Layout-equal member rename, **and the meanings agree** — this
corrects the sample doc's "same slot, different *meaning*, needs a human" note.
Measured: the client populates the slot with the nickname item's object ID,
`setNicknameID(pItem->GetID())` [measured, `UIMessageManager.cpp:9913`], and the
server reads it as exactly that, `getItemObjectID()` → item OID in the handler
[measured, `CGModifyNicknameHandler.cpp:68`]. Same field, same semantic,
different name. Neither header stale.

**3. Proposed merged form.** Adopt the server file; keep `m_ItemObjectID` /
`getItemObjectID()`. Rewrite the two client call sites
(`UIMessageManager.cpp:9913,9926` `setNicknameID` → `setItemObjectID`) in the
same commit. Keep the server's `if (szSTR != 0)` guard (behaviorally identical,
avoids a zero-length stream call). Size unchanged.

**4. Blast radius if wrong.** None on the wire (layout equal). Only risk is a
missed `setNicknameID` call site → compile break, caught by client CI. Invisible
in Phase 18.

**5. Recommendation.** **Land now, second.** Same risk class as CGGuildChat
(rename + guard). Two clean landings back-to-back establish the loop before the
size pairs.

---

## 3. CLCreatePC — grade: land-after-smoke-test

Character-creation request (client→loginserver). 9 fields; the divergence is a
**size formula**, not the stream.

**1. Byte layout** (streams identical both sides)

| Order | Field | Type | Bytes | In size? srv | In size? cli |
|---|---|---|---|---|---|
| 1 | name length | BYTE | 1 | yes | yes |
| 2 | name | char[len] | len (≤20) | yes | yes |
| 3 | slot | BYTE | 1 (`szSlot`) | yes | yes |
| 4 | bitset (sex/hair) | BYTE | 1 | yes | yes |
| 5 | colors[SLAYER_COLOR_MAX] | Color_t×6 | `szColor*6` | yes | yes |
| 6 | STR | Attr_t | `szAttr` | yes | yes |
| 7 | DEX | Attr_t | `szAttr` | yes | yes |
| 8 | INT | Attr_t | `szAttr` | yes | yes |
| 9 | **race** | Race_t | **`szRace` (1)** | **NO** | **yes** |

Both `read()` and both `write()` stream all 9 fields including `m_Race`
[measured, server `.cpp:41,73`; client `.cpp:41,74`] — normalizer reports
`read_seq: match`, `write_seq: match`, `size_expr: DIFF` [measured,
`normalize-packet-style.py --pair CLCreatePC`].

- Server `getPacketSize()` = `szBYTE + name + szSlot + szBYTE + szAttr*3 +
  szColor*SLAYER_COLOR_MAX` — **omits `szRace`** [measured, `.h:61-66`].
- Client `getPacketSize()` = same **`+ szRace`** [measured, `.h:64-72`].

**2. Authority.** **Client.** Its formula matches the 9-field stream; the server
`getPacketSize()` is short by one byte (`szRace`) versus its own `read()`. The
server *factory* `getPacketMaxSize()` **does** include `szRace` [measured,
`.h:227-233`], so the server file is internally self-contradictory — proof that
`getPacketSize()` is the stale copy, not the wire. Adopting the server formula
would make the unified client stamp a header one byte short → loginserver frames
the *next* packet starting one byte early → login-stream desync.

**3. Proposed merged form.** Server file as the base **but** with
`getPacketSize()` corrected to include `+ szRace` (i.e. the client's formula).
Fields/order unchanged. Fold client guards. Result: header size = 9-field body,
matching both streams.

**4. Blast radius if wrong.** If the szRace-less formula ships: every character
creation under-reports body size by 1 → the loginserver's post-CreatePC framing
slips by a byte → the following packet is misparsed → failed/garbled character
creation, and likely a disconnect. A tester notices immediately (cannot create a
character). Phase 18 STEP3 (client login→char-create) surfaces it on the first
attempt.

**5. Recommendation.** **Land after the smoke test proves the loop**, using the
*corrected* (client) formula. Low complexity, but it touches the login framing
path — validate the pipeline on the two rename pairs first, then land with the
size assertion checked live.

---

## 4. CGSkillToInventory — grade: land-after-smoke-test

Skill-to-inventory-target request (client→gameserver). Encrypted gameplay
packet. Divergence is a **size formula** (double-counted `szObjectID`).

**1. Byte layout** (streams identical both sides — 6 fields)

| Order | Field | Type | Bytes | Server | Client |
|---|---|---|---|---|---|
| 1 | skill type | SkillType_t (WORD) | 2 | `m_SkillType` | `m_SkillType` |
| 2 | object ID | ObjectID_t | 4 | `m_ObjectID` | `m_ObjectID` |
| 3 | X | CoordInven_t | 1 | `m_X` | `m_X` |
| 4 | Y | CoordInven_t | 1 | `m_Y` | `m_Y` |
| 5 | target X | CoordInven_t | 1 | `m_TargetX` | `m_TargetX` |
| 6 | target Y | CoordInven_t | 1 | `m_TargetY` | `m_TargetY` |

Body = 2+4+1+1+1+1 = **10 bytes**. Both sides' `read()`/`write()` stream exactly
these 6 (encrypt + plain branch) [measured, `--pair CGSkillToInventory`
`read_seq`/`write_seq` match].

- Server `getPacketSize()` = `szSkillType + szObjectID + szObjectID +
  szCoordInven*4` = **14** — `szObjectID` **twice** [measured, `.h:28`].
- Client `getPacketSize()` = `szSkillType + szObjectID + szCoordInven*4` = **10**
  [measured, `.h:30`]; the old double-ObjectID formula sits commented directly
  above it [measured, `.h:29`, `//...+ szObjectID + szObjectID...`] — the client
  was deliberately corrected, the server never was.

**2. Authority.** **Client.** 10 matches the 6-field stream; the server's 14
double-counts a field the stream writes once. (Server factory
`getPacketMaxSize()` likewise double-counts — same internal staleness as
CLCreatePC.) Adopting 14 makes the client stamp a 4-byte-too-large header →
gameserver waits for 4 bytes that never come, or swallows the next packet's
head → in-zone stream desync.

**3. Proposed merged form.** Server file base, `getPacketSize()` **corrected to
the single-ObjectID (client) formula** (10), delete the stale commented formula.
Keep encrypt/plain branch structure (unchanged, matches). Fold guards.

**4. Blast radius if wrong.** Ship 14 and every skill-use-on-inventory action
over-frames by 4 bytes → gameserver read stalls / desyncs for that connection →
skills silently fail then the player desyncs. Testers notice as "skills don't
fire." Phase 18 gameplay leg (cast a skill) surfaces it.

**5. Recommendation.** **Land after smoke test**, corrected (client) formula.
Batch with pairs 3 and 5 (identical failure mode and fix shape).

---

## 5. CGUseItemFromInventory — grade: land-after-smoke-test

Use-item-from-inventory request (client→gameserver). Encrypted. Same **size
formula** class as pair 4.

**1. Byte layout** (streams identical both sides — 3 fields)

| Order | Field | Type | Bytes | Server | Client |
|---|---|---|---|---|---|
| 1 | object ID | ObjectID_t | 4 | `m_ObjectID` | `m_ObjectID` |
| 2 | inven X | CoordInven_t | 1 | `m_InvenX` | `m_InvenX` |
| 3 | inven Y | CoordInven_t | 1 | `m_InvenY` | `m_InvenY` |

Body = 4+1+1 = **6 bytes**. A second `m_InventoryItemObjectID` read/write is
commented out on **both** sides [measured, server `.cpp:25,32,48,54`; client
`.cpp:44,52,73,81`] — so it is not on the wire on either side.

- Server `getPacketSize()` = `szObjectID + szObjectID + szCoordInven +
  szCoordInven` = **10** — counts the removed second ObjectID [measured, `.h:30`].
- Client `getPacketSize()` = `szObjectID + szCoordInven + szCoordInven` = **6**
  [measured, `.h:32`]; old formula commented above [measured, `.h:31`].

**2. Authority.** **Client** (6 = the 3-field stream). Server's 10 counts a field
neither side serializes. Adopting 10 → client stamps 4 bytes too large → same
desync as pair 4.

**3. Proposed merged form.** Server file base, `getPacketSize()` corrected to 6
(client formula), delete stale commented formula and the dead
`m_InventoryItemObjectID` member/commented lines while touching it. Branch
structure unchanged.

**4. Blast radius if wrong.** Ship 10 → every inventory-item use over-frames by
4 bytes → gameserver desync for that connection → "using items does nothing"
then disconnect. Phase 18 gameplay leg (use a potion/item) surfaces it.

**5. Recommendation.** **Land after smoke test**, corrected formula. Batch with
pairs 3 and 4.

---

## 6. CGMove — grade: needs-live-window · MANDATORY CHECK below

Movement request (client→gameserver), the highest-traffic gameplay packet.
Divergence is a **field-order mismatch in the plain (non-encrypt) branch**.

**1. Byte layout**

| Path | Server `read()` order | Client `write()` order | Match? |
|---|---|---|---|
| encrypt (`SHUFFLE_STATEMENT_3`) | X, Y, Dir | X, Y, Dir | **yes** [srv `.cpp:24-25`, cli `.cpp:51-54`] |
| plain (`else`) | **Dir, X, Y** | **X, Y, Dir** | **NO** [srv `.cpp:29-31`, cli `.cpp:59-61`] |

All three fields are 1 byte (`Coord_t`=`Dir_t`=BYTE), so the sizes agree
(`getPacketSize = szCoord+szCoord+szDir` both) and the mis-order is **invisible
to framing** — it corrupts *values*, not length. In the plain branch the server
reads the client's X byte into `m_Dir`, Y into `m_X`, Dir into `m_Y`.

**2. Authority.** **Client** order (X,Y,Dir), because it is what the encrypt path
(the normal runtime path) already uses on both sides. The server's plain-branch
`Dir,X,Y` is the outlier and must be flipped to `X,Y,Dir`.

### MANDATORY CHECK — is the non-encrypt branch reachable in shipped builds?

**`__USE_ENCRYPTER__` is defined unconditionally, in a header, in both trees —
not by any build config** [measured]:

- `dkrixserver/src/Core/Encrypter.h:18` → `#define __USE_ENCRYPTER__` (plain
  `#define`, no `#ifdef` around it).
- `dkrix/Client/Packet/Encrypter.h:17` → same.
- No CMake/Makefile/`.cmake` in either tree defines or omits it: `grep` over
  `dkrixserver/**/CMakeLists.txt|Makefile|*.cmake` and `dkrix/**/CMakeLists.txt`
  returns **0 hits** [measured]. It cannot be turned off by a build flag.
- `CGMove.cpp` transitively includes it (`CGMove.cpp` → `SocketEncryptInputStream.h`
  → `Encrypter.h`, both trees) [measured, server `CGMove.cpp:10`,
  `SocketEncryptInputStream.h:13`; client `CGMove.cpp:8`], so the
  `#ifdef __USE_ENCRYPTER__` encrypt branch is **compiled into every shipped
  build**.

But "compiled in" is not "always taken" — the branch is a **runtime**
`if (getEncryptCode() != 0) { encrypt } else { plain }`. The plain branch runs
whenever the per-connection code is 0. Measured facts on when that happens:

- The encrypter's code **starts at 0**: `Encrypter()` calls `setCode(0)`
  [measured, `Encrypter.h:36-37`]. Until the server calls
  `GamePlayer::setEncryptCode()` (on zone entry, via `IncomingPlayerManager`
  [measured, `IncomingPlayerManager.cpp:1217`]) the code is 0 and the plain
  branch is live.
- The zone code is `EncryptCode(zoneID, serverID)` =
  `(uchar)((((zoneID)>>8)^(zoneID)) ^ (((serverID)+1)<<4))` [measured,
  `Encrypter.h:21`, `Zone.cpp:719`]. This is an 8-bit value that **is 0 for
  specific (zoneID, serverID) pairs** — e.g. serverID 0 gives `(1<<4)=0x10`, and
  any zone whose `((zoneID>>8)^zoneID)&0xFF == 0x10` (e.g. `zoneID = 16`) yields
  code 0. For such a zone, **every** CGMove takes the plain branch.

**Conclusion:** the field-order mismatch is a **live latent bug**, not dead
code. It is masked in the common case (nonzero zone code → encrypt path, which
matches), but any zone/server pair hashing to 0 — or any CGMove processed before
`setEncryptCode()` runs — takes the plain branch, and the server then reads
movement coordinates transposed with direction. Symptom: players in the affected
zone warp/rubber-band or move in wrong directions; not a crash, so it can hide.

**3. Proposed merged form.** Unify to the **client order `X, Y, Dir` in the
plain branch on both sides** (matches the encrypt path). Server file base with
its plain `read()`/`write()` reordered to X,Y,Dir. Encrypt branch unchanged.
Size unchanged. This is a **behavior fix**, so it must be a labeled, isolated
commit.

**4. Blast radius if wrong.** If the merge picks the server's `Dir,X,Y` for the
plain branch instead, movement corrupts in exactly the zones that currently
work-by-accident (code 0). If done right but untested, the fix is invisible in
normal zones (encrypt path) and only provable in a code-0 zone. Phase 18 must
move the character **and** ideally exercise a zone/server pair with code 0 (or
temporarily force `getEncryptCode()==0`) to see the plain path at all.

**5. Recommendation.** **needs-live-window.** Behavior-affecting, and the buggy
branch is exactly the one the smoke test is least likely to hit. Land bracketed
by a live movement test that can force the plain branch; do not land blind.

---

## 7. CLLogin — grade: needs-live-window · the sample doc's confirmed case

Login request (client→loginserver). The original manually-flagged divergence;
the machine reproduces and refines it.

**1. Byte layout — normal (non-Netmarble) path**

| Order | Field | Type | Bytes | Server `read()` | Client `write()` (normal) |
|---|---|---|---|---|---|
| 1 | ID length | BYTE | 1 | yes | yes |
| 2 | ID | char[len] | len (≤30) | yes | yes |
| 3 | password length | BYTE | 1 | yes | yes |
| 4 | password | char[len] | len | yes | yes |
| 5 | MAC address | BYTE[6] | 6 | yes (`m_cMacAddress`) | yes (`m_MacAddress`) |
| 6 | login mode | BYTE | 1 | `m_LoginMode` | `loginMode = 0` (hardcoded) |

`getPacketSize()` = `szBYTE + m_ID.size() + szBYTE + m_Password.size() + 6 +
szBYTE` on **both** sides for this path [measured, server `.cpp:112`; client
`.cpp:157-160`] — normalizer reports `size_expr: match`. **So the normal-path
wire is byte-compatible today**, including the trailing login-mode byte (client
sends 0; server reads it into `m_LoginMode`).

The `read_seq`/`write_seq` DIFF the normalizer flags comes from three real
source-level divergences that do **not** break the normal wire:

**(a) Client Netmarble branch (server has none).** Client `write()` branches on
`g_pUserInformation->IsNetmarble`: the `else` path writes an **`int` (4-byte) ID
length + ID only**, no password/MAC/mode [measured, `CLLogin.cpp:96-104`]. The
server `read()` has no such branch — it always expects the byte-prefixed 6-field
layout [measured, `.cpp:11-45`]. If `IsNetmarble` is ever true, the client sends
a shape the server cannot parse → login framing corruption. `[unverified]`
whether this revival ever sets `IsNetmarble`; it is a Korean-portal (Netmarble)
integration flag and is presumed false, but this must be confirmed before
landing.

**(b) Password length bound differs.** Server rejects `szPassword > 30`
[measured, `.cpp:37,77`]; client rejects `> 20` [measured, `.cpp:42,79`]. Wire
layout is unaffected (still a byte prefix), but a 21–30-char password is
accepted by the server and refused by the client. Behavioral, not framing.

**(c) Server-only members, none on the normal wire.** `m_bNetmarble`,
`m_bAdult`, a second string MAC `m_strMacAddress`, and a
`__THAILAND_SERVER__`-guarded `onChildGuardTimeArea()` parental-control hook
[measured, `.h:120-124,177-179`] — set by the handler, **not read from the
stream** (server `read()` reads only the 6 fields above; it calls
`setNetmarble(false)` but never reads it). So they do not appear on the wire and
do not affect compatibility.

**2. Authority.** **Mixed — needs a human wire decision.** The normal path is
agreed and authoritative as-is. The open questions are policy, not layout:
(i) is the Netmarble branch dead? (ii) is the password bound 20 or 30?
(iii) should the client actually send `m_LoginMode` (WebLogin) instead of a
hardcoded 0? The server is authoritative on *reading* a real login mode; the
client currently can never signal WebLogin.

**3. Proposed merged form.** Keep the normal 6-field layout and the shared size
formula (unchanged wire). Then, per Enrico's decisions: **if** Netmarble is dead,
delete the client `else` branch (removing the one genuine framing hazard);
**reconcile the password bound to a single value** (recommend 30 to match the
server, so the server never sees a client-rejected length); **decide** whether
`write()` sends `m_LoginMode` or keeps `0`. Retain the server-only members and
the `__THAILAND_SERVER__` hook behind their guards. This is the one pair whose
merged form is genuinely a set of protocol choices, not a mechanical pick.

**4. Blast radius if wrong.** Login is the front door — any framing error here
blocks *all* testers, not a feature. Deleting the Netmarble branch when it is
secretly live, or shifting the mode byte, desyncs the very first packet and no
one can log in. This is the maximum-visibility pair. Phase 18 STEP3
(LOGIN_SMOKE) is exactly the gate; it either logs in or it does not.

**5. Recommendation.** **needs-live-window.** Do not land mechanically. Requires
(a) Enrico confirming the Netmarble flag is dead, (b) a password-bound decision,
(c) a live login test on the normal path before and after. The safest first
change is *pure normalization with zero wire/branch edits* (fold guards, rename
`m_cMacAddress`/`m_MacAddress`), deferring (a)-(c) to the live window.

---

## 8. CGExchangeBuy — grade: needs-live-window · MANDATORY CHECK below

Marketplace "buy listing" request (client→gameserver). The server protocol has
moved **ahead** of the client copy.

**1. Byte layout**

| Order | Field — server | Type | Bytes | Field — client | Type | Bytes |
|---|---|---|---|---|---|---|
| 1 | `m_ListingID` | uint64 | 8 | `m_ListingID` | uint | 4 |
| 2 | `m_IdempotencyKey` | string | var | — | — | — |

- Server `read()`: `read(listingID)` (uint64) then `read(m_IdempotencyKey)`
  [measured, `.cpp:16-20`]; `write()` mirrors [measured, `.cpp:28-29`];
  `getPacketSize()` = `sizeof(m_ListingID) + m_IdempotencyKey.size()` = 8 + key
  [measured, `.cpp:34-38`]; factory max = `sizeof(uint64_t) + 64` [measured,
  `.h:52-55`].
- Client `read()`/`write()`: `m_ListingID` (uint, 4) only [measured,
  `.cpp:26,35`]; `getPacketSize()` = `szint` = 4 [measured, `.h:27`].

Wire-incompatible on **every** axis (ID width, extra field, size) — normalizer
`read_seq`/`write_seq`/`size_expr` all DIFF.

**LATENT SERVER BUG found while reading (report separately):** the server's
`iStream.read(m_IdempotencyKey)` [measured, `.cpp:20`] has **no matching
overload** — `SocketInputStream` offers only `read(char*, uint)`,
`read(string&, uint)`, and a template `read<T>(T&)` [measured,
`SocketInputStream.h:60-63`]. The single-arg string call binds to
`read<std::string>`, whose body does `buf = *(std::string*)(m_Buffer + m_Head)`
[measured, `SocketInputStream.h:157-171`] — i.e. it reinterprets raw wire bytes
as a `std::string` object and assigns it: **undefined behavior / heap
corruption** if ever executed. (The write side is safe: `write(const string&)`
exists, `SocketOutputStream.h:54`.) This compiles — CI cannot catch it — and is
**only reachable if the server ever receives this packet**, which today it does
not (see below). The unified read must use a **length-prefixed string** read
(the pattern CGExchangeList already uses), never the bare `read(string&)`.

### MANDATORY CHECK — can the live client use the exchange feature today?

**No — the marketplace is dead-UI on the client** [measured]:

- **No send site.** The only references to `CGExchangeBuy`/`CGExchangeList` in
  the entire client are the factory registrations in
  `PacketFactoryManager.cpp:1200-1201`; there is **no** UI code that constructs
  or `sendPacket()`s either class [measured, `grep` over `dkrix/Client` for
  stack/heap instantiation returns only the packet's own files]. The client's
  many `Exchange` UI symbols (`UI_RunExchangeAsk`, `IsRunningExchange`, …) are
  the **classic player-to-player trade** system, unrelated to these packets.
- **No reply path.** The server replies with `GCExchangeBuy`/`GCExchangeList`
  (492 etc.) [measured, server `Core/GCExchange*Handler.cpp` present]. The client
  has a `Gpackets/GCExchangeList.{h,cpp}` file but it is **not registered** in
  the client `PacketFactoryManager` (`grep Exchange` there shows only the two CG
  send factories) [measured], and there is no `GCExchangeBuy` on the client at
  all — so the client could not decode a marketplace reply even if it sent a
  request.
- Packet IDs **do** match across trees (484/487/492) [measured, server
  `Packet.h:555,558,563`; client `Packet.h:559,562,567`], so the wiring is
  latent-compatible — but unused.

**So the merge is not a risk, it is a from-scratch design + bug-fix**: no live
behavior depends on the client copy today.

**2. Authority.** The **server design** is the intended protocol (uint64 listing
ID + idempotency key). The client copy is a stale stub. But the server `read()`
must be fixed (length-prefixed key) before it is authoritative in practice.

**3. Proposed merged form.** Adopt the server's field set: `int64/uint64
m_ListingID` + length-prefixed `m_IdempotencyKey` (≤64). Fix the read to
`BYTE len; read(len); read(m_IdempotencyKey, len);` — **not** bare
`read(string&)`. This becomes the canonical `shared/Packets/CGExchangeBuy`. The
client UI that would send it does not yet exist — landing the packet does not
enable the feature.

**4. Blast radius if wrong.** Zero on *current* live behavior (dead-UI). The
danger is future: if the marketplace UI is later wired to a merged class that
still contains the bare-`read(string&)` bug, the first buy request corrupts
server memory. So the fix matters even though nothing calls it today.

**5. Recommendation.** **needs-live-window** — but really "needs the feature to
be designed." Land the corrected server-shape packet only as part of a deliberate
exchange-feature work item, with the read bug fixed, never as a mechanical Phase
12 pick. Until then, leave both copies in place and record the divergence.

---

## 9. CGExchangeList — grade: needs-live-window

Marketplace "browse listings" request (client→gameserver). Same story as pair 8:
server moved ahead, client copy is a stale stub, feature is dead-UI.

**1. Byte layout**

| Order | Field | Type | Bytes | Server | Client |
|---|---|---|---|---|---|
| 1 | page | int | 4 | `m_Page` | `m_Page` |
| 2 | page size | int | 4 | `m_PageSize` | `m_PageSize` |
| 3 | item class | uint8/BYTE | 1 | `m_ItemClass` | `m_ItemClass` |
| 4 | item type | uint16/ushort | 2 | `m_ItemType` | `m_ItemType` |
| 5 | min price | int | 4 | `m_MinPrice` | `m_MinPrice` |
| 6 | max price | int | 4 | `m_MaxPrice` | `m_MaxPrice` |
| 7 | seller filter len | BYTE | 1 | `len` | **absent** |
| 8 | seller filter | char[len] | len (≤64) | `m_SellerFilter` | **absent** |

Server reads fields 1–6 then a length-prefixed seller-filter string [measured,
`.cpp:18-33`]; client reads only 1–6 [measured, `.cpp:31-36`]. Field types match
1–6 (`ushort`≡`uint16_t`, `BYTE`≡`uint8_t`). Server `getPacketSize()` sums all 8
including the length byte + filter [measured, `.cpp:56-66`]; client returns a
fixed `szint*4 + sizeof(BYTE) + sizeof(ushort)` = 19 [measured, `.h:27`].

Note (server internal): the server *factory* `getPacketMaxSize()` lists `m_Page`
/`m_PageSize` as `szBYTE` while `read()` reads them as `int` [measured,
`.h:73-81` vs `.cpp:18-19`] — a loose upper bound, harmless but sloppy; the real
`getPacketSize()` is correct.

**MANDATORY CHECK (shared with pair 8): dead-UI.** Same evidence as §8 — no
client send site, `GCExchangeList` present in client `Gpackets/` but not
registered in the client factory, IDs match but unused. The client cannot browse
the marketplace today.

**2. Authority.** **Server design** (with seller filter). Client copy stale.

**3. Proposed merged form.** Adopt the server field set incl. length-prefixed
`m_SellerFilter`. The server read/write here **already uses the correct
length-prefixed idiom** [measured, `.cpp:26-33,49-51`] — reuse it as the template
for fixing CGExchangeBuy's read. Canonical `shared/Packets/CGExchangeList`.

**4. Blast radius if wrong.** Zero on current live behavior (dead-UI). Future:
must match whatever client browse-UI is eventually written.

**5. Recommendation.** **needs-live-window / feature-gated.** Land only as part
of a real exchange-feature effort, alongside pair 8. Record divergence until
then.

---

## 10. Cross-cutting findings

- **Three size-formula pairs (3,4,5) all say the same thing:** the *client*
  `getPacketSize()` is authoritative and the *server* header double-counts /
  omits a field versus its own stream. The 08-07 finding is confirmed
  independently on each. "Pick server" would corrupt client framing in all
  three. Merged form = server file **with the client's size formula**.
- **The server-header-stale pattern extends to factory `getPacketMaxSize()`**
  (CLCreatePC, CGSkillToInventory, CGExchangeList) — same authors' oversight,
  same direction. Fix them to agree with the corrected `getPacketSize()` when
  touched.
- **One real latent server bug found:** `CGExchangeBuy::read`'s bare
  `iStream.read(m_IdempotencyKey)` (§8) — UB, CI-invisible, unreachable today
  only because the feature is dead-UI. Fix in the same change that lands the pair.
- **Two "member rename" pairs (1,2) are genuinely wire-equal**, and pair 2's
  slot means the *same thing* on both sides (item object ID) — correcting the
  sample doc's "different meaning" flag.

## 11. Landing order (recommended)

1. **CGGuildChat** — rename only, zero wire delta. Proves the review→land loop.
2. **CGModifyNickname** — rename only, same-meaning slot. Confirms the loop.
   *(Run Phase 18 smoke test here — establishes the runtime baseline before any
   wire-touching pair.)*
3. **CLCreatePC** → 4. **CGSkillToInventory** → 5. **CGUseItemFromInventory** —
   the three size-formula pairs, corrected to the client formula, batched; each
   validated by a live create/skill/use action.
6. **CGMove** — behavior fix (plain-branch order); land in a live window that
   can force the plain branch (code-0 zone).
7. **CLLogin** — protocol decisions (Netmarble dead? pw bound? login mode?) +
   live login test; front-door risk.
8. **CGExchangeBuy** → 9. **CGExchangeList** — feature-gated; land only with a
   real exchange-feature effort, with the read bug fixed. Until then, record as
   intentionally divergent.

Rationale: ascending wire risk. The two rename pairs carry no wire risk and
should bracket the first smoke test; the size pairs are mechanical once the loop
is proven; CGMove and CLLogin are behavioral/front-door and need eyes-on; the
exchange pair is not a Phase 12 merge at all but a future feature.

---

### Appendix — commands behind this doc (tip `96a1995`)

```
# per-pair wire verdict (JSON: read_seq / write_seq / size_expr match|DIFF)
python3 dkrixserver/scripts/normalize-packet-style.py --pair <Name>

# __USE_ENCRYPTER__ trace (§6)
grep -rn "define __USE_ENCRYPTER__" dkrixserver/src dkrix           # 2 headers
grep -rn "__USE_ENCRYPTER__" dkrixserver/**/CMakeLists.txt Makefile # 0 (no build define)
sed -n '36,42p' dkrixserver/src/Core/Encrypter.h                    # Encrypter() setCode(0)
sed -n '715,725p' dkrixserver/src/server/gameserver/Zone.cpp        # EncryptCode(zoneID,serverID)

# exchange dead-UI trace (§8/§9)
grep -rn "CGExchangeBuy\|CGExchangeList" dkrix/Client               # only factory reg + own files
grep -n "Exchange" dkrix/Client/Packet/PacketFactoryManager.cpp     # 2 CG factories, no GC

# idempotency read bug (§8)
sed -n '157,171p' dkrixserver/src/Core/SocketInputStream.h          # template read<T> raw cast
grep -n "read(string\|read(char" dkrixserver/src/Core/SocketInputStream.h  # no single-arg read(string&)

# header-authority model (§0)
sed -n '598,609p' dkrix/Client/Packet/Packet.h                      # writeHeaderNBody stamps getPacketSize()
sed -n '378,428p' dkrixserver/src/server/gameserver/GamePlayer.cpp  # server reads/bounds header size
```
