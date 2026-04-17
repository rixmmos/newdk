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
