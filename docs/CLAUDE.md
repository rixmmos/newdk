# CLAUDE.md — Engineering Principles

Layout, build commands, release pipeline, and operational workflow live in
`../CLAUDE.md` at the workspace root. That file wins on any factual conflict.
This one covers *how to make changes* — the judgment calls, not the paths.

The running plan lives in `docs/MODERNIZATION.md` and is the single source of
truth for migration status.

## What this codebase is

Roughly 2000–2010-era C++ with the matching baggage: Hungarian notation,
mixed Korean/Chinese/English comments, dead copy-protection and anti-cheat
hooks, fake platform wrappers, and overlapping abstractions. Treat it with
respect for its history but without sentimentality.

## Priorities, in order

1. **Architectural cleanliness.** Prefer designs that make the system easier to
   reason about over designs that match what's already there.
2. **Incremental modernization.** Small well-scoped steps, not big-bang
   rewrites.
3. **Build stability.** Every change leaves the tree buildable. The native
   Windows client build and the server `make debug` are the ground truth.
4. **Platform layer cleanup.** Remove fake Win32 / DirectX wrappers wherever
   SDL2, the Win32 SDK, or the C++ standard library can do the job directly.
5. **SDL2-oriented client modernization.** Continue DirectX → SDL2: delete dead
   D3D code, collapse pixel-format variants, remove always-true branches like
   `CDirect3D::IsHAL()`.
6. **Long-term maintainability over quick hacks.** If a change makes the code
   harder to modify next quarter, don't ship it.

## Guidelines

- **No shallow hacks just to compile.** If the build breaks because an
  abstraction is wrong, fix the abstraction. No `#ifdef` papering, empty stubs,
  or `reinterpret_cast`s that hide real bugs.
- **Small, reviewable steps.** Each change independently reviewable and
  revertable. A series of focused commits beats one sweeping diff.
- **Explain architectural reasoning before and after.** State the motivation
  and expected effect before editing; summarize what actually changed after.
- **Call out as you find them:**
  - Dead code — legacy copy protection, unreachable `#ifdef` branches,
    orphaned files.
  - Duplicate abstractions — e.g. the two sprite systems (`Client/SpriteLib/`
    and `tools/engine/sprite/`), overlapping surface/text classes.
  - Fake platform wrappers that only forward to SDL or the standard library.
  - Unsafe legacy patterns — raw `new`/`delete` without RAII, unchecked
    `strcpy`, globals pretending to be thread-safe.
- **Favor real platform APIs over typedef chaos.** Use SDL2 types where they
  belong; use `<cstdint>`, `std::string`, and friends instead of homegrown
  `DWORD` / `BOOL` / `CString` clones.
- **Compatibility layers only when they enable a cleaner migration.** A shim
  that lets the next refactor land is good. A shim that permanently freezes a
  bad interface is not.
- **Preserve gameplay logic, not bad architecture.** Damage math, skill
  effects, drop tables, and packet semantics are load-bearing and must be kept
  unless the user explicitly agrees to change them.
- **Keep the plan current.** Update `docs/MODERNIZATION.md` as part of the
  change that makes it stale, not afterwards.

## Working agreements

- Source encoding is UTF-8. Migrate mixed-language comments to English as you
  touch files.
- Run the formatter before committing — server: `make fmt` (clang-format);
  client: match the prevailing style of the file.
- Do not commit generated binaries, build outputs, or secrets.
- Prefer a new commit over amending, and do not skip hooks.
- When a legacy compatibility header conflicts with real Windows SDK types, fix
  the abstraction so the Windows build uses the SDK definitions instead of
  repo-local typedef clones.
- Client and server changes that alter the wire protocol must land together;
  packet semantics are shared between `dkrix` and `dkrixserver`.

## Contents of this folder

`MODERNIZATION.md` and `BRANCH-RECONCILIATION.md` are current and maintained,
as are the dated working docs (`TECH-DEBT-AUDIT.md`, `adr/`,
`phase3-item2-liveness-2026-08-07.md`, `packet-divergence-2026-08-07.md`,
`packet-normalization-sample-2026-08-07.md`). `howto/`, `client_source_overview/`,
and the Korean/Chinese `.txt` notes (GM commands, item codes, monster tables, DB
setup walkthroughs) are inherited legacy reference — useful for archaeology,
often out of date. Verify against the code before acting on it.

## When in doubt

Ask. If a request conflicts with these priorities — for example a "just make it
compile" request that needs a shallow hack — raise the tradeoff explicitly and
propose the smallest clean alternative before falling back to the hack.
