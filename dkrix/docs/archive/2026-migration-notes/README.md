# 2026 Migration Notes (archived)

This folder contains client-repo status and planning documents produced
during the 20242026 DirectXSDL2 / Win32cross-platform migration. They
are preserved for historical context only  **do not treat anything in
here as the current state of the project.**

The live modernization plan lives in the sibling repo:

- `../../../docs/MODERNIZATION.md` (relative to this file:
  `opendarkeden/docs/MODERNIZATION.md`)  single source of truth for
  what is done, in progress, and planned.

## Why these were archived

These docs were written at different points during the migration and
they contradict each other. Examples:

- `WINDOWS_NATIVE_CLEANUP_SUMMARY.md` declares the SDL2 migration
  "100% complete".
- `CLEANUP_SUMMARY.md` lists the TextSystem migration (Phase 4) as
  "Major refactoring, Future Work".
- `SDL_MIGRATION_CLEANUP_SUMMARY.md` claims `D3DLib/` was deleted, while
  the top-level `CLAUDE.md` (at the time of archival) still described
  `D3DLib/` as a live compatibility stub.
- `plan.md` / `spec.md` / `tasks.md` are a Chinese-language Windows
  removal plan whose last phases were marked "SKIPPED".

Anyone picking one of these as ground truth will do the wrong work.
Centralizing the plan in `docs/MODERNIZATION.md` resolves the
contradiction; these files remain available as forensic record.

## Contents

| File | Language | Roughly about |
|------|----------|---------------|
| `CLEANUP_SUMMARY.md` | English | SDL2 cross-platform cleanup summary |
| `GDI_USAGE_ANALYSIS.md` | Chinese | Remaining Windows GDI usage |
| `PLATFORM_MIGRATION.md` | Chinese | Platform abstraction layer summary |
| `SDL_MIGRATION_CLEANUP_SUMMARY.md` | Chinese | Post-SDL migration cleanup |
| `TEXT_MIGRATION.md` | English | Text system migration plan |
| `TEXT_MIGRATION_SUMMARY.md` | Chinese | Resource-file encoding migration |
| `WINDOWS_NATIVE_CLEANUP_SUMMARY.md` | Chinese | Windows native code cleanup |
| `plan.md` | Chinese | Windows-specific dependency cleanup plan |
| `spec.md` | Chinese | Windows-specific dependency cleanup spec |
| `tasks.md` | English | SDL2 cross-platform task checklist |

## Policy

Further status documents should **not** be added to the client repo
root. Update `docs/MODERNIZATION.md` as part of the change that makes
the work real.
