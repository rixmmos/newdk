# CLAUDE.md — Project-wide Instructions

This file provides top-level guidance that applies across the whole DarkEden-like
MMORPG project. It complements, and takes precedence over, the per-repo `CLAUDE.md`
files in the `client/` and `server/` trees when there is any conflict.

## Project scope

This is a legacy DarkEden-like MMORPG codebase. It spans:

- **server/** — C++11 game, login, and shared servers (MySQL + Lua + xerces-c).
- **client/** — C++ isometric game client, mid-migration from Win32 + DirectX to SDL2.
- **docs/** — Mixed-language (English / Korean / Chinese) legacy documentation and notes.
- **configs/** — Runtime configuration (`*.conf`, `*.inf`, SQL schema, etc.).
- **tooling/** — Build wrappers (Make → CMake), formatters, scripts.

The code is roughly 2000–2010-era and carries the usual baggage: Hungarian
notation, mixed-language comments, dead copy-protection / anti-cheat hooks,
fake platform wrappers, and overlapping abstractions. Treat the codebase with
respect for its history but without sentimentality.

## Priorities (in order)

1. **Architectural cleanliness.** Prefer designs that make the system
   easier to reason about over designs that happen to match what's already there.
2. **Incremental modernization.** Move the codebase forward in small,
   well-scoped steps rather than big-bang rewrites.
3. **Build stability.** Every change should leave the tree buildable.
   `make debug` (server) and `make debug-asan` (client) are the ground truth.
4. **Platform layer cleanup.** Eliminate fake Win32 / DirectX wrappers
   where SDL2 or the C++ standard library can do the job directly.
5. **SDL2-oriented client modernization.** Continue the DirectX → SDL2
   migration: remove dead D3D code, simplify pixel-format variants, collapse
   `CDirect3D::IsHAL()`-style always-true branches.
6. **Long-term maintainability over quick hacks.** If a change would make
   the code harder to modify next quarter, don't ship it.

## Guidelines

- **No shallow hacks just to compile.** If the build breaks because an
  abstraction is wrong, fix the abstraction. Do not paper over it with
  `#ifdef`s, empty stubs, or `reinterpret_cast`s that hide real bugs.
- **Small, reviewable steps.** Each change should be independently
  reviewable and revertable. Prefer a series of focused commits over one
  large sweeping diff.
- **Explain architectural reasoning before and after edits.** State the
  motivation and the expected effect before touching code, and summarize
  what actually changed (and why) after.
- **Identify and call out:**
  - Dead code (including legacy copy protection, unreachable `#ifdef`
    branches, orphaned files).
  - Duplicate abstractions (e.g. overlapping sprite / surface / text classes).
  - Fake platform wrappers that only forward to SDL or the standard library.
  - Unsafe legacy patterns (raw `new`/`delete` pairs without RAII,
    unchecked `strcpy`, global singletons pretending to be thread-safe, etc.).
- **Favor real platform APIs over custom typedef chaos.** Use SDL2 types
  directly where they belong; use `<cstdint>`, `std::string`, and friends
  instead of homegrown `DWORD` / `BOOL` / `CString` equivalents.
- **Use compatibility layers only when they enable a cleaner migration
  path.** A shim that lets the next refactor land is good. A shim that
  permanently freezes a bad interface is not.
- **Preserve gameplay logic where possible, but not bad architecture.**
  Observable game behavior (damage math, skill effects, drop tables, packet
  semantics) is load-bearing and must be kept unless the user explicitly
  agrees to change it. The structure *around* that logic is fair game.
- **Keep a running modernization plan in the repo.** Maintain an ongoing
  document (e.g. `docs/MODERNIZATION.md`) that tracks what has been done,
  what is in progress, and what is next. Update it as part of the change
  that makes the work real, not after the fact.

## Working agreements

- Source encoding is **UTF-8**. When touching a file with mixed-language
  comments, migrate the comments to **English** as you go.
- Run the project's formatter before committing:
  - Server: `make fmt` (clang-format).
  - Client: match the prevailing style of the file until a project-wide
    format is adopted.
- Do not commit generated binaries, build outputs, or secrets.
- Prefer creating a new commit over amending, and do not skip hooks.

## When in doubt

Ask. If a request conflicts with these priorities — for example, a
"just make it compile" request that would require a shallow hack — raise
the tradeoff explicitly and propose the smallest clean alternative before
falling back to the hack.

## Continuing in a new chat

When the user says "continue" (or similar) at the start of a fresh
session, do this **before** taking any action:

1. **Read the two source-of-truth plans in this order:**
   - [`docs/MODERNIZATION.md`](./MODERNIZATION.md) — all code-hygiene /
     toolchain / infra work. Status legend `[ ]` / `[~]` / `[x]` /
     `[!]`. Jump to the Phase 18 section first — it's the most recent
     state and links back to what's still open.
   - [`docs/ROADMAP.md`](./ROADMAP.md) — all product / feature work
     (R1 UHD, R2 English-only, R3 bleeding gate, R4 branding scrub,
     R5 Windows .exe). Separate backlog; separate dependencies.
2. **Check git state:** `git -C <repo-root> log --oneline -5` and
   `git -C <repo-root> status --short`. The repo root is wherever
   `dkrix/` and `dkrixserver/` live side-by-side (on the user's
   machine that is `/mnt/c/newdk` via WSL2; in a fresh session it
   will likely be a different path — do not hard-code).
3. **Ask which track**, unless the user's "continue" message already
   names one. Use `AskUserQuestion` if available. The two tracks
   are meaningfully different:
   - **Modernization track** — infra, usually safe to pick up any
     open sub-item without further clarification. See "Modernization
     entry points" below.
   - **Roadmap track** — product-shaped, usually wants a clarifying
     question about scope or MVP definition before work starts.
     See "Roadmap entry points" below.

### Modernization entry points (what's open, ordered by lowest friction)

Everything below comes from MODERNIZATION.md; always re-read that
file before starting — it moves faster than this list.

- **Phase 12 packet migration (ratchet-driven).** 163 class pairs
  to unify into `shared/Packets/`. Start with the 7 cosmetic-only
  `.cpp` candidates identified by Phase 17:
  `CGDisplayItem`, `CGGQuestCancel`, `CGRequestStoreInfo`,
  `CGStoreClose`, `CGStoreOpen`, `CGStoreSign`, `CGUndisplayItem`.
  Each PR = one class pair + `check-packet-duplicates.sh --update`.
  Good first task for a new session — bounded and mechanical.
- **Phase 11.2 SQL-injection migration (ratchet-driven).** 567
  call sites to migrate onto the existing `PreparedStatement` API
  (already shipped in Phase 11.1). Priority order per plan:
  user-string interpolation first (~180 sites), then operator-
  trusted (~200), then constant/numeric (remainder). Each PR
  drops the `check-sql-injection.sh` baseline.
- **Phase 1 WinLib deletion.** `Client/WinLib/{CWinUpdate.cpp,
  CWinUpdate.h,WinLib.h}` still on disk. Needs the update-state
  hierarchy (`CWaitUIUpdate` / `CWaitPacketUpdate` / `CGameUpdate`
  / `COpeningUpdate`) re-homed first. Small structural refactor.
- **Phase 2 Platform.h bulk reduction.** Currently at -18 lines
  of an aspirational "under 600" target. Phase 3 + 5 dependencies
  are now done so this is unblocked. The remaining bulk is
  Windows-type forward-declarations and the `platform_*` C API.
- **Phase 13.3 / 13.4 endian-safe stream migration.** Blocked
  until Phase 12 drains (needs unified `Socket*Stream.h` to avoid
  writing the same migration twice).
- **Phase 18 follow-ups.**
  - Bug SS (#133): SDL port's `DXKeyboardEvent` drops letter keys.
    Needs `SDL_TEXTINPUT` vs `SDL_KEYDOWN` event-loop split.
  - `DrawLightBuffer3D` proper SDL2 port (currently null-guarded;
    dark zones render lit — see `MTopView.cpp` around line 10469).

### Roadmap entry points (what's open — product-shaped, clarify first)

Each R-item in ROADMAP.md has an explicit MVP definition and
non-goals; read those before asking scoping questions.

- **R1 UHD / high-DPI.** Biggest lift. Likely sequence: MVP is
  integer-scale wrapper around the existing fixed surface; DPI-
  aware rewrite of `VS_UI/` coordinate handling is the real fix
  and comes later.
- **R2 English-only localization.** Overlaps with Phase 6
  (translate CJK comments touch-as-you-go) — R2 is the explicit
  sweep that finishes the job. Overlaps with R4 on string-table
  work; consider landing together.
- **R3 Bleeding VFX gate.** Smallest R-item. Needs an audit of
  `MEffectManager` / `MCreature::Draw` to find the trigger site,
  then a single HP-threshold check.
- **R4 Branding scrub.** String-table grep is easy; baked-into-
  sprite watermarks require image editing. Server-side item
  descriptions need a separate pass.
- **R5 Windows .exe.** Wants a stable tree first (R2–R4 landed).
  MinGW-w64 cross-compile from Linux is the cleanest story.

### Live context cues

- **Current branch convention.** `modernize/phase4-sprite` was
  the branch as of Phase 18 close-out. Fresh sessions: check
  `git rev-parse --abbrev-ref HEAD` — don't assume.
- **Bug / task numbering.** Pre-smoke-test build fixes were
  lettered Q–OO; smoke-test bugs are PP–WW. New runtime or
  build bugs continue the letter sequence (XX, YY, ZZ, AAA…).
  New modernization sub-items take the next phase number.
  New product items take the next R-number.
- **Commit-message style.** `<area>: <phase>Y — <subject>` for
  modernization phase commits (e.g. `server: 11B — add
  PreparedStatement.{h,cpp}`). `smoke test: <subject>` for
  runtime-bug commits. Co-Authored-By footer on Claude
  commits.
- **Runtime verification.** The tree boots end-to-end on WSL2
  Ubuntu 22.04 + WSLg. `SMOKE_TEST_RESULTS.md` and
  `LOGIN_SMOKE.md` at the repo root have the most recent
  runtime findings. Don't re-derive what's already written
  down there.
- **APPLY_PHASE*.md at repo root.** Per-phase patch-bundle
  application notes. Useful if a session needs to understand
  how a specific phase was landed on the user's machine.

### What NOT to do in a fresh session

- Don't re-audit what's in the plan files. They are authoritative.
- Don't propose a new phase number without checking the last
  one used. Re-read MODERNIZATION.md's `^### Phase` lines first.
- Don't hard-code absolute paths that were specific to a previous
  sandbox. Derive from `git rev-parse --show-toplevel`.
- Don't treat the per-area `CLAUDE.md` files (`dkrix/CLAUDE.md`,
  `dkrixserver/CLAUDE.md`) as current on migration state — they
  carry some residue. This file and MODERNIZATION.md win on any
  conflict.
- Don't auto-commit. The user commits explicitly when ready.
