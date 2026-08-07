# SpriteLib SDL2 Backend - Complete Integration Guide

## Overview

SpriteLib SDL2 Backend provides cross-platform sprite rendering for the Dark Eden game client. It replaces the Windows-specific DirectDraw implementation with SDL2, enabling the client to run on macOS, Linux, and other platforms.

**Status**: **Unverified.** Corrected 2026-08-06.

> An earlier revision of this file claimed "Production Ready — All phases
> completed (Phases 1-6), Integration tests passing." That claim was never
> substantiated. **No CI run exists for this repository**, so nothing in this
> document has been confirmed by a compile, and there is no integration test
> suite for this backend. The API described below is real and is called by the
> game; its correctness is untested.
>
> The architecture diagram was also wrong: it attributed the SDL2 backend to
> `tools/engine/sprite/`. It does not. See the corrected diagram below.
>
> Background: [`docs/adr/0001-sprite-pipeline.md`](../../../docs/adr/0001-sprite-pipeline.md).

---

## Architecture

```
   Game Code (MTopView, MCreature, MGuildMarkManager, ...)
   Uses original CSprite* APIs
                    |
                    v
   SpriteLib Adapter Layer
     CSpriteSurface_Adapter.cpp   (preserves the C++ API)
     CSprite_SDL.cpp              (backend handle management)
                    |
                    v
   SpriteLibBackend.h  (C interface)
     spritectl_create_surface()
     spritectl_create_sprite()
     spritectl_blt_sprite()
                    |
                    v
   SpriteLibBackendSDL.cpp  (1,371 LOC)
     The one and only implementation behind this interface.
     Self-contained: includes SpriteLibBackendSDL.h, stdlib.h,
     string.h, stdio.h — and nothing else.
```

**`SpriteLibBackendSDL.cpp` does not use `tools/engine/sprite/`.** It contains
zero references to it. `engine/sprite` is a separate 7,749-line **C** library,
compiled only under `BUILD_ENGINE`, and linked only by `tools/viewers/`
(`item_viewer`, `map_viewer`). Two independent SDL2 implementations exist in
this repository; only this one renders the game.

The "Windows Backend (DirectDraw)" branch shown in the old diagram is also gone
— `USE_SDL_BACKEND` is forced `ON` in `CMakeLists.txt` and
`basic/PlatformWindows.cpp` has been deleted. SDL2 is the only backend on every
platform.

Callers of `spritectl_*` in the game: `Client/Client.cpp`,
`Client/SDLMain.cpp`, `Client/ClientFunction.cpp`, plus handle storage in
`CAlphaSprite` and `CIndexSprite` (`m_backend_sprite`).

---

## Quick Start

### 1. Build the Backend

`USE_SDL_BACKEND` is forced `ON` in `CMakeLists.txt`, so the backend is built
as part of the normal client build. From the workspace root (`newdk/`), on
this project's authoritative Windows path:

```powershell
cmake -S dkrix -B dkrix\build -G "Visual Studio 17 2022" -A x64 `
      -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build dkrix\build --config Debug
```

> The old instructions here read `cd /path/to/opendarkeden/client/build` and
> `make SpriteLib`. That path layout does not exist — there is no `client/`
> directory and no separate `opendarkeden` checkout. See `docs/README.md`.

### 2. Use in Your Code

```cpp
#include "SpriteLibBackend.h"

// Initialize SDL2
SDL_Init(SDL_INIT_VIDEO);

// Create a surface
spritectl_surface_t surface = spritectl_create_surface(800, 600, SPRITECTL_FORMAT_RGB565);

// Create sprite from pixel data
WORD* pixels = /* your pixel data */;
spritectl_sprite_t sprite = spritectl_create_sprite(64, 64, SPRITECTL_FORMAT_RGB565, pixels, size);

// Render sprite
spritectl_blt_sprite(surface, 100, 100, sprite, 0, 255); // x, y, sprite, flags, alpha

// Cleanup
spritectl_destroy_sprite(sprite);
spritectl_destroy_surface(surface);
```

---

## API Reference

### Types

```cpp
typedef struct spritectl_surface_s* spritectl_surface_t;  // Surface handle
typedef struct spritectl_sprite_s* spritectl_sprite_t;   // Sprite handle
typedef struct spritectl_pack_s* spritectl_pack_t;       // Sprite pack handle
```

### Constants

```cpp
#define SPRITECTL_INVALID_SURFACE  ((spritectl_surface_t)NULL)
#define SPRITECTL_INVALID_SPRITE   ((spritectl_sprite_t)NULL)
#define SPRITECTL_INVALID_PACK      ((spritectl_pack_t)NULL)

// Pixel formats
#define SPRITECTL_FORMAT_RGB565     0
#define SPRITECTL_FORMAT_RGB555     1
#define SPRITECTL_FORMAT_RGBA8888   2

// Blitting flags
#define SPRITECTL_BLT_ALPHA         0x01
#define SPRITECTL_BLT_FLIP_HORIZONTAL 0x02
#define SPRITECTL_BLT_FLIP_VERTICAL   0x04
```

### Surface Operations

#### `spritectl_create_surface()`
```cpp
spritectl_surface_t spritectl_create_surface(int width, int height, int format);
```
**Creates a drawing surface**
- **Parameters**:
  - `width`: Surface width in pixels
  - `height`: Surface height in pixels
  - `format`: Pixel format (SPRITECTL_FORMAT_RGB565, etc.)
- **Returns**: Surface handle or SPRITECTL_INVALID_SURFACE on failure

#### `spritectl_destroy_surface()`
```cpp
void spritectl_destroy_surface(spritectl_surface_t surface);
```
**Destroys a surface and frees resources**
- **Parameters**:
  - `surface`: Surface handle to destroy

### Sprite Operations

#### `spritectl_create_sprite()`
```cpp
spritectl_sprite_t spritectl_create_sprite(int width, int height, int format,
                                         void* data, size_t size);
```
**Creates a sprite from pixel data**
- **Parameters**:
  - `width`: Sprite width in pixels
  - `height`: Sprite height in pixels
  - `format`: Pixel format
  - `data`: Pointer to pixel data (RGB565 format)
  - `size`: Size of pixel data in bytes (width * height * sizeof(WORD))
- **Returns**: Sprite handle or SPRITECTL_INVALID_SPRITE on failure
- **Note**: The pixel data is copied; you can free your buffer after calling this

#### `spritectl_destroy_sprite()`
```cpp
void spritectl_destroy_sprite(spritectl_sprite_t sprite);
```
**Destroys a sprite and frees resources**
- **Parameters**:
  - `sprite`: Sprite handle to destroy

### Rendering Operations

#### `spritectl_blt_sprite()`
```cpp
void spritectl_blt_sprite(spritectl_surface_t surface, int x, int y,
                          spritectl_sprite_t sprite, int flags, int alpha);
```
**Renders a sprite to a surface**
- **Parameters**:
  - `surface`: Destination surface
  - `x`, `y`: Destination position
  - `sprite`: Source sprite
  - `flags`: Blitting flags (0 for normal, SPRITECTL_BLT_ALPHA for alpha blending)
  - `alpha`: Alpha value (0-255, only used with SPRITECTL_BLT_ALPHA flag)

#### `spritectl_blt_sprite_scaled()`
```cpp
void spritectl_blt_sprite_scaled(spritectl_surface_t surface, int x, int y,
                                   spritectl_sprite_t sprite, int scale, int flags);
```
**Renders a scaled sprite**
- **Parameters**:
  - `surface`: Destination surface
  - `x`, `y`: Destination position
  - `sprite`: Source sprite
  - `scale`: Scale factor (256 = 1x, 128 = 0.5x, 512 = 2x)
  - `flags`: Blitting flags

---

## Sprite Classes Backend Integration

All sprite classes now have backend support:

### CSprite (and CSprite565, CSprite555)
```cpp
class CSprite {
protected:
#ifdef SPRITELIB_BACKEND_SDL
    spritectl_sprite_t  m_backend_sprite;  // Backend handle
    bool                m_backend_dirty;   // Dirty flag

public:
    spritectl_sprite_t  GetBackendSprite() const;
    bool                IsBackendDirty() const;
    void                SetBackendSprite(spritectl_sprite_t sprite);
    void                SetBackendDirty(bool dirty);
#endif
};
```

### Usage Pattern

```cpp
// 1. Create sprite as normal
CSprite565 sprite;
sprite.LoadFromFile(file);

// 2. Backend sprite is created automatically on first use
POINT point = {100, 100};
spriteSurface->BltSprite(&point, &sprite);  // Backend sprite created here

// 3. If pixels change, mark as dirty
sprite.SetPixel(data, width, height);
sprite.SetBackendDirty(true);  // Backend will be recreated on next blit
```

---

## Examples

### Example 1: Simple Sprite Rendering

```cpp
#include "SpriteLibBackend.h"

// Create surface
spritectl_surface_t surface = spritectl_create_surface(800, 600, SPRITECTL_FORMAT_RGB565);

// Create gradient sprite
WORD pixels[64 * 64];
for (int y = 0; y < 64; y++) {
    for (int x = 0; x < 64; x++) {
        WORD r = (x * 31) / 64;
        WORD g = (y * 63) / 64;
        WORD b = 15;
        pixels[y * 64 + x] = (r << 11) | (g << 5) | b;
    }
}

spritectl_sprite_t sprite = spritectl_create_sprite(64, 64, SPRITECTL_FORMAT_RGB565, pixels, sizeof(pixels));

// Render sprite
spritectl_blt_sprite(surface, 100, 100, sprite, 0, 255);

// Cleanup
spritectl_destroy_sprite(sprite);
spritectl_destroy_surface(surface);
```

### Example 2: Alpha Blending

```cpp
// Render with alpha blending
int alpha = 128;  // 50% transparency
spritectl_blt_sprite(surface, x, y, sprite, SPRITECTL_BLT_ALPHA, alpha);
```

### Example 3: Scaling

```cpp
// Render at half size
spritectl_blt_sprite_scaled(surface, x, y, sprite, 128, 0);  // 128 = 0.5x

// Render at double size
spritectl_blt_sprite_scaled(surface, x, y, sprite, 512, 0);  // 512 = 2x
```

---

## Integration Checklist

When integrating SpriteLib backend into your project:

### Build Configuration
- [ ] Add `-DSPRITELIB_BACKEND_SDL` to compile definitions
- [ ] Link against `SpriteLib` library
- [ ] Link against SDL2 (`SDL2::SDL2` or `${SDL2_LIBRARIES}`)
- [ ] Include `Client/SpriteLib` in include paths

### Code Changes
- [ ] Include necessary headers: `SpriteLibBackend.h`, `CSpriteSurface.h`, etc.
- [ ] Use backend API for new code
- [ ] Existing CSprite* code works automatically (lazy creation)
- [ ] Mark sprites dirty when modifying pixels: `SetBackendDirty(true)`

### Runtime Requirements
- [ ] SDL2 initialized before creating surfaces/sprites
- [ ] RGB565 pixel format for sprite data
- [ ] Proper cleanup of backend resources

---

## Testing

**There are no tests for this backend.**

An earlier revision of this section documented three binaries —
`test_spritelib_backend`, `test_sprite_rendering`, and
`sprite_backend_example` — along with their expected output. **None of the
three exists.** Searching the entire `dkrix/` tree for those names returns no
source file, no CMake target, and no reference of any kind. The "Expected
Output" block was describing a program that was never in this repository.

The only automated tests anywhere in `dkrix/` are the 11 files (3,898 lines)
under `tools/engine/sprite/tests/`, and they cover the *other* sprite library
— the one the game client does not link.

If you are adding test coverage here, that gap is the highest-value place to
start; see `docs/TECH-DEBT-AUDIT.md` item 8.

---

## Troubleshooting

### Compilation Errors

**Error**: `SPRITECTL_BACKEND_SDL not defined`
- **Solution**: Add `-DSPRITELIB_BACKEND_SDL` to CXXFLAGS

**Error**: `undefined reference to spritectl_*`
- **Solution**: Link against SpriteLib library

**Error**: `SDL2/SDL.h: No such file or directory`
- **Solution**: Install SDL2 development packages or add SDL2 include path

### Runtime Errors

**Error**: Sprites not rendering
- **Check**: Is SDL2 initialized? (`SDL_Init()`)
- **Check**: Are pixels in RGB565 format?
- **Check**: Did you call spritectl_create_sprite successfully?

**Error**: Crashes on blit
- **Check**: Are surface and sprite handles valid?
- **Check**: Is pixel data size correct? (width * height * 2)

---

## Performance Considerations

### Lazy Creation
- Backend sprites are created only when first used
- Avoids unnecessary memory allocation
- First blit may be slightly slower

### Dirty Tracking
- Backend sprites are only recreated when marked dirty
- Call `SetBackendDirty(true)` after modifying sprite pixels
- Minimizes unnecessary sprite recreations

### Memory Management
- Always destroy sprites and surfaces when done
- Backend manages its own pixel data copies
- Original pixel data can be freed after sprite creation

---

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| macOS | Unverified | SDL2 backend only; no CI run exists |
| Linux | Unverified | SDL2 backend only; requires SDL2 development packages |
| Windows | Unverified | SDL2 backend only — `USE_SDL_BACKEND` is forced `ON`; DirectDraw is gone |

---

## Files Modified/Created

### Backend Implementation
- `Client/SpriteLib/SpriteLibBackend.h` - Unified backend API
- `Client/SpriteLib/SpriteLibBackendSDL.cpp` - SDL2 implementation
- `Client/SpriteLib/CSpriteSurface_SDL.cpp` - Surface adapter
- `Client/SpriteLib/CSpriteSurface_Adapter.cpp` - Blitting methods
- `Client/SpriteLib/CSprite_SDL.cpp` - Sprite backend management

### Sprite Class Updates
- `Client/SpriteLib/CSprite.h` - Added backend support (lines 114-120)
- `Client/SpriteLib/CAlphaSprite.h` - Added backend support (lines 100-106)
- `Client/SpriteLib/CShadowSprite.h` - Added backend support (lines 106-112)
- `Client/SpriteLib/CIndexSprite.h` - Added backend support (lines 159-165)
- `Client/SpriteLib/CSprite.cpp` - Backend initialization/cleanup
- `Client/SpriteLib/CAlphaSprite.cpp` - Backend initialization/cleanup
- `Client/SpriteLib/CShadowSprite.cpp` - Backend initialization/cleanup
- `Client/SpriteLib/CIndexSprite.cpp` - Backend initialization/cleanup

### Tests and Examples

None exist. See "Testing" above.

---

## Next Steps

### For Game Developers

1. **Use the Backend Library**
   ```cpp
   #include "SpriteLibBackend.h"
   // Use backend API directly
   ```

2. **Or Use Sprite Classes**
   ```cpp
   #include "CSprite565.h"
   // Backend support is automatic
   ```

### For Contributors

1. **Advanced Features** (TODO)
   - Implement 4444 format conversion
   - Add scaling optimizations
   - Implement special effects (darkness, color tinting)

2. **Performance Optimization**
   - Profile sprite creation and blitting
   - Optimize hot paths
   - Add sprite caching

3. **Testing**
   - Add unit tests for edge cases
   - Test with actual game sprite files (.SPK)
   - Performance benchmarks

---

## Summary

**SpriteLib SDL2 Backend is unverified, not production-ready.**

**In place**:
- Backend infrastructure implemented (`SpriteLibBackendSDL.cpp`, 1,371 LOC)
- All sprite types (`CSprite`, `CAlphaSprite`, `CShadowSprite`, `CIndexSprite`)
  carry backend handle fields

**Not done**:
- No compile has ever confirmed this code — no CI run exists for this repo
- No automated tests cover it (the only tests in `dkrix/` exercise the
  unrelated `tools/engine/sprite/` library — see "Testing" above)
- No example programs exist despite being referenced by earlier revisions of
  this document

**For questions or issues**, refer to:
- This README for API reference
- `docs/adr/0001-sprite-pipeline.md` for why this backend is the survivor
- `docs/TECH-DEBT-AUDIT.md` for the measured state of the sprite code

> The old text here pointed at "Example programs in `examples/`" and "Test
> programs in `tests/`". Neither directory exists under `Client/SpriteLib/`.

---

*Last updated: 2026-08-07 — removed the remaining "production-ready" claims*
*(Summary section, Platform Support table, phantom example binary) left over*
*from the 2026-08-06 pass.*
*Status: unverified. No CI run exists; nothing here has been compiled.*
