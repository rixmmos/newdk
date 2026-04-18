# Archived: `tools/engine/sprite/` (retired Phase 4D, 2026-04)

This directory preserves the C-based `sprite` engine library that used to
live at `tools/engine/sprite/`. It was a parallel, SDL-oriented
reimplementation of the pixel/sprite/zone logic already present in the
C++ `Client/SpriteLib/` tree.

## Why it was retired

Phase 4 of MODERNIZATION.md consolidated the project around a single
sprite stack. The decision — pinned in Phase 4's plan block — was that
`Client/SpriteLib/` (C++, the live game client) wins, and the engine-side
C tree is absorbed into it.

By the start of Phase 4D:

- **4A** deleted the 555/565 pixel-format variant classes that only the
  C-engine viewers consumed.
- **4B** deleted the `Blt4444*` / `BltSmall4444*` / `BltAlpha4444*`
  method families from `CAlphaSprite`, `CAlphaSpritePal`, `CSprite`, and
  `CShadowSprite`, along with the orphaned `CSpriteSurface.cpp` whose
  static members were already redefined in `CSpriteSurface_SDL.cpp`.
- **4C.1** deleted the legacy `main.c` / `viewer.c` / `viewer.h` viewer
  sources that were never in CMake's source lists (they had `.cpp`
  siblings that were the real build).
- **4C.2** git-mv'd the only still-live C file — `zone.c` (plus its
  `zone.h` / `error.h` headers) — into
  `tools/viewers/map_viewer/`, added it to the `map_viewer` target's
  sources directly, and dropped the `sprite` library from both
  `map_viewer` and `effect_viewer` link lines. The `BUILD_ENGINE` option
  and `add_subdirectory(tools/engine/sprite)` call were removed from the
  top-level `CMakeLists.txt`.

After 4C nothing in the live build referenced `tools/engine/sprite/`.
This directory is the snapshot taken at that point, kept as a reference
for:

- historical algorithms (especially in `src/frame.c`, `src/framepack.c`,
  `src/shadow_sprite.c`) that may be useful when validating the C++
  implementations;
- the `tests/` directory, which exercised the C API and can seed
  equivalent test cases against the C++ API if someone picks up
  test-coverage work;
- the `sdl_framework.c` reference for SDL initialisation patterns.

## Archive contents

- `CMakeLists.txt` — the old library build rules (no longer loaded).
- `include/` — 18 public headers (API surface).
- `src/` — 12 C translation units (behaviour).
- `tests/` — 11 test files under the old `tests/test_*.c` convention.

## Notable: what was NOT archived

`zone.c`, `zone.h`, and `error.h` were **moved**, not copied, in 4C.2.
They live at `tools/viewers/map_viewer/zone.c`, `.../zone.h`, and
`.../error.h` and are still compiled into the `map_viewer` target.

## Do not resurrect blindly

The live stack has moved on: `Client/SpriteLib/` now relies on
`SPRITELIB_BACKEND_SDL` with the SDL backend in `CSpriteSurface_SDL.cpp`
and peers, and several format-variant paths (555/565/4444) documented
here have been deleted on the C++ side. Any resurrected code needs to be
re-checked against the current pixel-format assumptions and the
consolidated class hierarchy before it can be wired back in.
