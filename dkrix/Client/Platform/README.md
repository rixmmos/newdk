> **Moved 2026-08 (Phase 3 item 3):** this directory was `Client/DXLib/`.
> Files kept their names. The library target, `PLATFORM_*` defines, and
> `platform_*` C API were renamed from `dxlib`/`DXLIB_*`/`dxlib_*` in the
> item 3 follow-up (2026-08); the function/define names below were updated
> to match. The `#include` examples were updated to the `Platform/` path;
> the rest of this document predates the move (including mentions of the
> never-ported Windows-DirectX backend and already-deleted `_Adapter.cpp`
> files).

# DXLib 

## 

DXLib  Dark Eden  Windows DirectInput/DirectSound  SDL2 

## 

```
CDirectInput, CDirectSound, CDirectMusic
     ( API )
DXLibBackend.h
    
 DXLibBackendWindows.cppWindows 
 DXLibBackendSDL.cppSDL2 
    

```

## 

### 1. DXLibBackend.h - 



- **Input Backend** - 
- **Sound Backend** - WAV
- **Music Backend** - MIDI, MP3, OGG
- **Stream Backend** - 

### 2. DXLibBackendSDL.cpp - SDL2 

****

|  |  |  |
|------|------|------|
|  |  | `platform_input_init()` |
|  |  | `platform_input_key_down()` |
|  |  | `platform_input_get_mouse_pos()` |
|  |  | `platform_input_get_mouse_buttons()` |
|  |  | `platform_input_get_mouse_wheel()` |
|  |  | `platform_input_update()` |
|  |  | `platform_sound_load_wav()` ( SDL_mixer) |
|  |  | `platform_sound_play()` |
|  |  | `platform_sound_stop()` |
|  |  | `platform_sound_set_volume()` |
|  |  | `platform_music_load()` ( SDL_mixer) |
|  |  | `platform_music_play()` |
| / |  | `platform_music_pause()/resume()` |
|  |  | `platform_music_set_volume()` |
|  |  | `platform_get_backend_name()` |

****

|  |  |  |
|------|------|------|
|  | SDL_mixer  |  |
|  | SDL_mixer  |  |
|  | SDL_mixer  |  |

## 

### Windows ()

```bash
cmake -B build -DUSE_SDL_BACKEND=OFF
cmake --build build
```

### Windows (SDL )

```bash
cmake -B build -DUSE_SDL_BACKEND=ON
cmake --build build
```

### Linux/macOS

```bash
cmake -B build
cmake --build build
```

## 

### 

```cpp
#include "Platform/DXLibBackend.h"


platform_input_init(window_handle);


while (running) {

    platform_input_update();


    if (platform_input_key_down(DIK_W)) {

    }


    int x, y;
    platform_input_get_mouse_pos(&x, &y);


    int left, right, center;
    platform_input_get_mouse_buttons(&left, &right, &center);
    if (left) {

    }
}


platform_input_release();
```

### 

```cpp
#include "Platform/DXLibBackend.h"


platform_sound_init(window_handle);


platform_sound_t sound = platform_sound_load_wav("explosion.wav");


platform_sound_play(sound, 0); // 0 = 


platform_sound_set_volume(sound, 50);


platform_sound_stop(sound);


platform_sound_free(sound);


platform_sound_release();
```

### 

```cpp
#include "Platform/DXLibBackend.h"


platform_music_init(window_handle);


platform_music_load("background.mp3");


platform_music_play(1);


platform_music_set_volume(70);

// /
platform_music_pause();
platform_music_resume();


platform_music_stop();


platform_music_release();
```

## API 

### 

|  CDirectInput | DXLibBackend API | SDL  |
|-------------------|------------------|----------|
| `KeyDown(DIK_W)` | `platform_input_key_down(DIK_W)` | SDL_GetKeyboardState |
| `m_mouse_x, m_mouse_y` | `platform_input_get_mouse_pos()` | SDL_GetMouseState |
| `m_lb_down` | `platform_input_get_mouse_buttons()` | SDL_GetMouseState |
| `m_mouse_z` | `platform_input_get_mouse_wheel()` | SDL_MouseWheelEvent |
| `UpdateInput()` | `platform_input_update()` | SDL_PollEvent |

### 

|  CDirectSound | DXLibBackend API | SDL  |
|-------------------|------------------|----------|
| `LoadWav(filename)` | `platform_sound_load_wav()` | Mix_LoadWAV |
| `Play(buffer, loop)` | `platform_sound_play(sound, loop)` | Mix_PlayChannel |
| `Stop(buffer)` | `platform_sound_stop(sound)` | Mix_HaltChannel |
| `AddVolume(buffer, n)` | `platform_sound_set_volume()` | Mix_Volume |
| `RightPan/LeftPan` | `platform_sound_set_pan()` |   |

### 

|  CDirectMusic | DXLibBackend API | SDL  |
|-------------------|------------------|----------|
| `Play(filename)` | `platform_music_load() + play()` | Mix_LoadMUS + Mix_PlayMusic |
| `Stop()` | `platform_music_stop()` | Mix_HaltMusic |
| `Pause()` | `platform_music_pause()` | Mix_PauseMusic |
| `Resume()` | `platform_music_resume()` | Mix_ResumeMusic |
| `SetCurrentTempo()` | `platform_music_set_tempo()` |   |

## 

### SDL 

- **SDL2** - 
- **SDL2_mixer** - 

### Windows 

- **DirectX SDK** - DirectInput, DirectSound, DirectMusic
- **Windows SDK** - Windows API

## 

###  

- [x] DXLibBackend.h 
- [x] DXLibBackendSDL.cpp SDL2 
- [x]  + 
- [x]  SDL_mixer
- [x]  SDL_mixer
- [x] 
- [x] DirectInput 

###  

- [ ] DXLibBackendWindows.cpp Windows 
- [ ] 

###  

- [x] CDirectInput_Adapter.cpp - CDirectInput 
- [x] CDirectSound_Adapter.cpp - CDirectSound 
- [x] CDirectMusic_Adapter.cpp - CDirectMusic 
- [x] CDirectSoundStream_Adapter.cpp - CDirectSoundStream 
- [x] CMakeLists.txt - 

###  

1. **SDL_mixer **
   - pan
   - 
   - 

2. **DirectInput **
   - 
   - 

## 

** 1**

### 

- **CDirectInput_Adapter.cpp** - CDirectInput  SDL2 
- **CDirectSound_Adapter.cpp** - CDirectSound  SDL2 
- **CDirectMusic_Adapter.cpp** - CDirectMusic  SDL2 
- **CDirectSoundStream_Adapter.cpp** - CDirectSoundStream  SDL2 

### 

 (`#ifdef PLATFORM_BACKEND_SDL`) 

```cpp
// CDirectInput.cpp / CDirectInput_Adapter.cpp
#ifdef PLATFORM_BACKEND_SDL
    //  DXLibBackend API (SDL2)
    platform_input_init(hWnd);
    platform_input_update();
#else
    //  DirectInput API (Windows)

#endif
```

****
- 
-  API
- 
- 

****
- 

## 

###  2

 `CDirectInputSDL`, `CDirectSoundSDL` 

```cpp
class CDirectInputSDL {
    //  platform_input_* 
    // API  CDirectInput 
};
```

****
- 
- 

****
- 
- 

###  3

 `DXLibBackend` API

```cpp

// #include "CDirectInput.h"

#include "DXLibBackend.h"

//  API
platform_input_init(window);
if (platform_input_key_down(DIK_W)) { ... }
```

****
- 
- 

****
- 
- 

## 

**** ** 1**  
- 
- 
- 

**** ** 3**
- 
- 

## 



```bash
cd build
./tests/test_dxlib_backend
```


- 
- 
- 
- 

## 

- `DXLibBackend.h` - 
- `DXLibBackendSDL.cpp` - SDL2 
- `DXLibBackendWindows.cpp` - Windows 
- `CDirectInput_Adapter.cpp` - CDirectInput 
- `CDirectSound_Adapter.cpp` - CDirectSound 
- `CDirectMusic_Adapter.cpp` - CDirectMusic 
- `CDirectSoundStream_Adapter.cpp` - CDirectSoundStream 
- `CMakeLists.txt` - 

## 

 Dark Eden 
