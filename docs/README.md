# docs

Cross-cutting documentation for the project. Everything now lives in **one
repository** — `newdk`, GitHub `rixmmos/newdk` (private).

Earlier revisions of this file described three sibling repositories checked out
side by side. **That was true once and is no longer.** `rixmmos` still hosts the
three original forks of `opendarkeden` — `dkrix`, `dkrixserver`, and `docs`, all
public — and they are what this file was written against. The consolidated
`newdk` repo superseded them; the loose paths that revision gave (`client/`,
`server/`) never matched the fork names either way.

If you are looking for this project on GitHub and see only those three public
repos, you are on the public profile view. `newdk` is private and will not
appear there — go directly to `github.com/rixmmos/newdk` while signed in.

The real tree:

```
newdk/
├── dkrix/        # The C++11 game client (mid-migration from Win32+DirectX to SDL2).
├── dkrixserver/  # The C++11 game/login/shared servers (MySQL + Lua + xerces-c).
├── docs/         # This folder — project-wide plans, guidelines, and notes.
├── tools/        # Release, publish, and update-server scripts; SPK helpers.
├── Darkeden/     # The live installed client. Build output is copied here.
└── release_site/ # Static update site served to testers.
```

## Start here

- `../CLAUDE.md` — workspace-root guidelines. Applies across every
  subtree and takes precedence over `dkrix/CLAUDE.md` and
  `dkrixserver/CLAUDE.md` when there is a conflict.
- `CLAUDE.md` (this folder) — engineering principles.
- `MODERNIZATION.md` — the single authoritative, living modernization
  plan. What is done, what is in progress, and what is next. Update this
  file as part of the change that makes the work real.
- `TECH-DEBT-AUDIT.md` — prioritized debt backlog, measured 2026-08-06.
  Each figure has the command that produced it in the appendix.
- `adr/` — architecture decision records. `adr/0001-sprite-pipeline.md`
  re-scopes Phase 4.

## Per-subtree docs

- `../dkrix/CLAUDE.md` — client-specific guidance (build flags, key
  modules, SDL migration summary).
- `../dkrixserver/CLAUDE.md` — server-specific guidance (build, packets,
  zone/thread model, config).

## A standing caveat

No claim in any document here has been confirmed by a compile. CI
workflows exist at `.github/workflows/` but have never executed. Treat
`[measured]` annotations as "verified by reading the tree", never as
"verified by building it".

## Archived status documents

Historical client migration status documents live under
`../dkrix/docs/archive/2026-migration-notes/`. They contradicted each
other and are preserved only for forensic context. **Do not use them as
a source of truth**; use `MODERNIZATION.md`.

## Other content in this folder

This directory also contains legacy game-data references (Chinese-language
notes on items, monsters, GM commands, SQL setup, etc.) inherited from
older distributions. They are useful for operators but are not part of
the modernization plan.
