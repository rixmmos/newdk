# Branch reconciliation — 2026-08-06

## DECISION (2026-08-06): the local line continues; the trunk is parked

Enrico's call, made with the analysis below in hand.

**The local operational tree is the line of record.** It is now `main`. It is
newer by date — it shipped a tester release on 2026-04-30, eight days after the
other line's last commit — and it is the tree that actually builds, runs the
live server, and ships to testers.

**The modernization work is parked, not lost.** It is preserved three ways:

- tag **`archive/modernization-phases-1-17`** → `1c861f5`
- branch `origin/modernize/phase4-sprite` on GitHub, untouched
- this document

Nothing was merged. `main` does not contain Phases 1–17.

### What this costs, stated plainly

The parked line holds work that `main` does not have and will need eventually:
DXLib removal, `shared/Packets/` (which dissolves the client/server packet
duplication), the Lua sandbox whitelist, the SQL-injection ratchet, CI build
matrices, `.clang-format`, `PreparedStatement`, and — most valuable — a
**repeatable end-to-end smoke-test runbook** at `docs/archive/smoke-test/` that
got a client and server to login→gameplay.

The cost of retrieving it grows as `main` drifts. If any of it is wanted later,
take it **one phase at a time** with a build after each, not as a merge; the
parked commits are cleanly structured for exactly that (pin plan → implement →
close out). Do not attempt the 4,132-file merge.

### Why this is a defensible call

The parked line was agent-driven and its CI results were never confirmed. The
local line has something stronger in the only sense that matters commercially:
it is in testers' hands. Choosing the tree that ships over the tree that is
tidier is a reasonable trade for a solo maintainer with no automated safety net.

---

## Verdict (analysis as written before the decision)

**`origin/modernize/phase4-sprite` is the real trunk of this project. The branch
you are on is not.**

`origin/main` has 4 commits and ends 2026-04-17. It is the initial import.
The local branch `modernize/phase-4-sprite` is that stub plus a working-tree
checkpoint. `origin/modernize/phase4-sprite` is that same stub plus **106
commits** of disciplined modernization work spanning 2026-04-17 to 04-22 —
Phases 1 through 17, each following a *pin the plan → implement → close it out
in `MODERNIZATION.md`* cycle.

The `MODERNIZATION.md` in this tree describes Phases 1–4 as untouched or barely
started. On that branch they are **finished**, and so are Phases 5, 7, 8, 9, 10,
and 11–17 — phases this tree's copy of the document does not know exist.

Most importantly: on 2026-04-20 that branch reached
**`dbc3087 smoke test: end-to-end login -> gameplay working (bugs PP-WW)`**,
preceded by `dcb6dbf smoke test: client boots to login screen on WSLg`. That is
a running game, client and server, after all the Phase 1–17 deletions. It is a
far stronger verification signal than the CI this tree has been treating as the
blocking prerequisite.

---

## What the 106 commits actually did

| Phase | Outcome on that branch |
|---|---|
| 1 — dead code | 9 duplicate `.cpp` + orphan headers deleted; server-side `*Handler.cpp` removed from the client tree |
| 2 — `Platform.h` | `id_t` collapsed, `_tcs*`/`_L` shims deleted, fake `HRESULT` machinery removed, `CRITICAL_SECTION` backed by `std::recursive_mutex` |
| 3 — DXLib | 25 dead files deleted (−5,939), `CDirect*` renamed to `CSDL*`, then **`Client/DXLib/` → `Client/Platform/`**, target `dxlib` → `platform` |
| 4 — sprite | 555/565 variants deleted (−1,166), orphaned `CSpriteSurface.cpp` deleted (−13,778), `Blt4444*` removed (−4,014), `tools/engine/sprite/` unhooked and archived |
| 5 — text | GDI stubs and unused GDI constant macros deleted from `Platform.h`; CJK glyph-coverage audit recorded |
| 7 — server | 7 dead server trees deleted (−18,905), `DatabaseManager.old.cpp` gone, `__OLD_GUILD_WAR__` gates retired |
| 8 — SQL | `${VAR}` env-var expansion in `Properties::load()`; **SQL-injection ratchet gate, baseline 567** |
| 9 — Lua | **sandbox whitelist dropping `io`/`os`/`debug`/`package`**; `lua_toboundedenum<T>` helper |
| 10 — hygiene | `dkrix/.clang-format` + `Makefile` with real `fmt`/`fmt-check`; gitignore unification; `compile_commands.json` untracked |
| 11 | `PreparedStatement.{h,cpp}` added to `server/database/` |
| 12 | **top-level `shared/Packets/` (352 files)** + packet-duplicates ratchet; `CGStoreOpen`/`CGStoreClose` migrated |
| 13 | `Core/Endian.h` little-endian wire primitives |
| 14 | VS_UI glob → explicit 56-file list; Client glob → `CONFIGURE_DEPENDS` |
| 15 | `USE_ASAN/TSAN/UBSAN` options; GitHub Actions build matrices for both trees |
| 16 | `ratchets.yml` workflow activating the 8C and 12.0 gates |
| 17 | `audit-packet-divergence.sh` + migration-class addendum |

Then 2026-04-20 to 04-22: runtime smoke-test bug fixes (Bugs Q through XX),
`.smp` zone-parser format correction, `TextBackendSDL` Linux font fallback,
`SocketOutputStream` overload fix, and `PreparedStatement` SELECT
materialization (545→540 injection sites).

### Measured comparison

| | this branch | `phase4-sprite` |
|---|---|---|
| `Client/DXLib/` files | 46 | **0** |
| `Client/Platform/` files | 0 | **20** |
| `Client/Packet/Cpackets/` files | 361 | **0** (moved to `shared/Packets/`) |
| top-level `shared/` files | 0 | **352** |
| SpriteLib 555/565 variants | 12 | **0** |
| `basic/Platform.h` lines | 1,996 | 1,951 |
| `dkrix/.clang-format` | absent | **present** |
| ratchet scripts + baselines | 0 | **7** |
| CI workflow files | 2 (written today, never run) | **4** |
| `conf/*.conf.template` | absent | **present** |

4,132 files differ between the two branches.

---

## The Phase 4 question — answered

`ADR-0001` (written today, before this branch was examined) argued that deleting
the 555/565 classes was unsafe because they override only
`SaveToFile`/`LoadFromFile` and are therefore SPK **deserializers**, not
renderers — so "SDL converts pixel formats natively" does not justify removing
them.

**The risk was real; that branch handled it correctly.** Commit `ebcfe52`
does not simply delete the variants. It *folds their serialization into the base
classes* — `CAlphaSprite.cpp` gains 115 lines and `CIndexSprite.cpp` gains 111,
under a comment reading:

```cpp
// SaveToFile / LoadFromFile (folded from CAlphaSprite565 in Phase 4A)
```

Callers were migrated in the same commit
(`MGuildMarkManager.cpp`, `GameInit.cpp`, `MTopView.cpp`):

```diff
-#include "CSprite565.h"
+#include "CSprite.h"
-  pSprite = new CSprite565;
+  pSprite = new CSprite;
```

They picked 565 as the single on-disk format and kept its codec. Two days later
the client booted and played end-to-end. **The approach is validated by a
running game**, which is the evidence `ADR-0001` said was required.

**Consequence:** `ADR-0001` should be marked superseded rather than
implemented. Its Finding 1 (three sprite paths, `SpriteLibBackendSDL.cpp`
independent of `engine/sprite`) and its correction of the
`SPRITELIB_BACKEND_README.md` claims may still hold — those should be
re-checked against the trunk rather than assumed.

---

## What exists only on this branch

This branch is not worthless. It carries work from 2026-04-23 to 04-30 that the
remote never received:

| Item | Why it matters |
|---|---|
| `tools/release/` (4 scripts) + `tools/spk_tools.ps1` | The entire release/publish/update-server pipeline |
| `MAKE_DARKEDEN_RELEASE.cmd`, `PUBLISH_RELEASE.cmd`, `START_DARKEDEN_UPDATE_SERVER.cmd` | Root wrappers testers depend on |
| `CHANGELOG.md` | Auto-appended release history |
| `npc_eslanian_fix_*`, `npc_eslanian_positions_backup_*`, `pet_dog_spawn_backup_*`, `char_zone_rescue_*`, `fix_zoneinfo_case.sql` | Live DB fixes and their backups — gameplay data, not reproducible |
| `CLAUDE.md`, `.gitattributes`, `docs/MOVE-RUNBOOK.md` | Workspace docs written 2026-08-06 |
| `docs/TECH-DEBT-AUDIT.md`, `docs/adr/0001-sprite-pipeline.md` | Today's analysis — value now mostly historical |

**Neither line is a superset. This is a merge, not a choice.**

---

## Recommended order — SUPERSEDED

*The sequence that was recommended here assumed reconciliation. The decision at
the top of this document overrides it: the local line continues and the
modernization work is parked. It is left below for the record.*

<details>
<summary>Original recommendation (not followed)</summary>

1. Do not push this branch to `origin/modernize/phase4-sprite`.
2. Confirm the remote branch's CI actually ran.
3. Branch from the trunk, not from here.
4. Cherry-pick the local-only work onto it, smallest first.
5. Re-run the smoke test before trusting the merged tree.
6. Reconcile the two `MODERNIZATION.md` files.
7. Retire the local branch once its unique commits are transplanted.

</details>

## What was actually done

1. Tagged the parked line: `archive/modernization-phases-1-17` → `1c861f5`.
2. Promoted the local line to **`main`** — a clean fast-forward, since local was
   already a strict descendant of `origin/main`.
3. Renamed the local branch `modernize/phase-4-sprite` →
   `retired/phase-4-sprite-misnomer`. That name described work this line never
   did, and its near-collision with `origin/modernize/phase4-sprite` was an
   accident waiting to happen.
4. Kept `backup/local-checkpoint-20260806` as a second copy of the local tip.

**If any of the parked work is wanted later**, retrieve it one phase at a time
against a build, starting from the tag. The smoke-test runbook at
`docs/archive/smoke-test/` on that tag is the single highest-value thing there
and is largely independent of the code changes — it could be lifted on its own.

---

## What this says about today's work

The audit and ADR written earlier today measured a tree that is ~106 commits
behind the real work. Their *measurements of this branch* are correct and were
double-checked. Their *recommendations* are largely obsolete: they propose doing
work that was completed in April.

Both were checked against the trunk on 2026-08-06. Results:

**1. WinLib — the trunk already caught this, and caught it better.** Its
`MODERNIZATION.md` Phase 1 entry reads:

> `[!]` Delete `Client/WinLib/` (3 files). **Deferred** — the "no subclasses,
> no live callers" claim was stale: `CWinUpdate` is actively subclassed by
> `CWaitUIUpdate` […] Removing `WinLib/` here would require restructuring the
> update-state hierarchy, which is bigger than "delete the unambiguously dead."

Today's independent rediscovery found one subclass. The trunk found more —
there are **four**: `CGameUpdate`, `COpeningUpdate`, `CWaitPacketUpdate`,
`CWaitUIUpdate`. Nothing to carry forward; the trunk is ahead here too.

One small defect worth fixing there: the trunk's *Ground truth* section still
carries the original stale line ("`CWinUpdate` has no subclasses and no live
callers") while its Phase 1 section retracts it. The document contradicts
itself.

**2. The `SPRITELIB_BACKEND_README.md` correction is worth carrying forward —
it is the one piece of today's work that improves the trunk.** The trunk still
ships the file verbatim, including `**Status**: ✅ **Production Ready**`,
"Integration tests passing", `./bin/test_spritelib_backend`, and the diagram
attributing the SDL2 backend to `engine/sprite`.

On the trunk that last one is **doubly wrong**: Phase 4D deleted
`tools/engine/sprite/` outright (0 files remain; it was archived to
`dkrix/docs/archive/2026-engine-sprite/`). The README's architecture diagram
now points at a directory that does not exist, and its "Files" section lists
`tests/test_spritelib_backend.cpp`, which never existed on either branch.

Port this branch's rewrite of that file to the trunk, adjusting the
`engine/sprite` paragraph to say it was archived rather than merely unused.

The process lesson is the expensive one: **this session spent its entire budget
auditing a branch without first checking whether it was the branch the work was
on.** `git log origin/<branch>` against every remote branch is a two-minute
check that would have reframed everything. Do it before any measurement pass.
