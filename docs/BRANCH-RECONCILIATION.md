# Branch reconciliation — 2026-08-06

## Verdict

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

## Recommended order

1. **Do not push this branch to `origin/modernize/phase4-sprite`.** A force-push
   or a careless merge here destroys 106 commits including the only end-to-end
   validation this project has. Back up first:
   `git branch backup/local-checkpoint-20260806`.
2. **Confirm the remote branch's CI actually ran.** Four workflows exist there
   and the branch is pushed, so GitHub Actions should have results — that cannot
   be checked from a sandbox. Open the Actions tab for `rixmmos/newdk`. If those
   runs are green, the "no build has ever verified anything" premise that framed
   all of today's work is simply false, and Phase -1 is already closed.
3. **Branch from the trunk**, not from here:
   `git checkout -b integrate/release-tooling origin/modernize/phase4-sprite`
4. **Cherry-pick the local-only work onto it**, smallest first — the release
   tooling and DB fix scripts are additive and should apply cleanly, since the
   remote has no competing versions of those paths.
5. **Re-run the smoke test** (`docs/archive/smoke-test/` has the full runbook:
   `STEP1_MYSQL.md`, `STEP2_SERVER.md`, `STEP3_CLIENT.md`, plus
   `start_servers.sh`, `client_smoke.sh`, `extract_rpks.sh`) before trusting
   the merged tree.
6. **Then reconcile the two `MODERNIZATION.md` files.** The trunk's copy is
   authoritative and knows about Phases 11–17. This branch's copy — including
   today's Phase 4 rewrite — is a fork of an obsolete revision.
7. **Retire this branch** once its unique commits are transplanted.

---

## What this says about today's work

The audit and ADR written earlier today measured a tree that is ~106 commits
behind the real work. Their *measurements of this branch* are correct and were
double-checked. Their *recommendations* are largely obsolete: they propose doing
work that was completed in April.

Two findings survive independent of the branch question and are worth carrying
forward:

- **`Client/WinLib/` is not dead.** `CGameUpdate` subclasses `CWinUpdate`.
  Both branches still contain `Client/WinLib/` (3 files each), so this
  correction applies to the trunk too.
- **The `SPRITELIB_BACKEND_README.md` claims were false** on this branch. Check
  whether the trunk's copy carries the same "Production Ready / integration
  tests passing" text and the same non-existent test binaries.

The process lesson is the expensive one: **this session spent its entire budget
auditing a branch without first checking whether it was the branch the work was
on.** `git log origin/<branch>` against every remote branch is a two-minute
check that would have reframed everything. Do it before any measurement pass.
