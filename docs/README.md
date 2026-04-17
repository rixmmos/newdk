# OpenDarkEden — docs

Cross-cutting documentation for the OpenDarkEden project. The project
spans three sibling working trees, each a separate repository, expected
to be checked out side by side:

```
opendarkeden/
├── client/   # The C++ game client (mid-migration from Win32+DirectX to SDL2).
├── server/   # The C++11 game/login/shared servers (MySQL + Lua + xerces-c).
└── docs/     # This repo — project-wide plans, guidelines, and notes.
```

## Start here

- `CLAUDE.md` — project-wide working guidelines. Applies across client,
  server, and docs. Takes precedence over the per-repo `CLAUDE.md` files
  when there is any conflict.
- `MODERNIZATION.md` — the single authoritative, living modernization
  plan. What is done, what is in progress, and what is next. Update this
  file as part of the change that makes the work real.

## Per-repo docs

- `../client/CLAUDE.md` — client-specific guidance (build flags, key
  modules, SDL migration summary).
- `../server/CLAUDE.md` — server-specific guidance (build, packets,
  zone/thread model, config).

## Archived status documents

Historical client migration status documents live under
`../client/docs/archive/2026-migration-notes/`. They contradicted each
other and are preserved only for forensic context. **Do not use them as
a source of truth**; use `MODERNIZATION.md`.

## Other content in this folder

This directory also contains legacy game-data references (Chinese-language
notes on items, monsters, GM commands, SQL setup, etc.) inherited from
older distributions. They are useful for operators but are not part of
the modernization plan.
