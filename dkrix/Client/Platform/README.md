> **Moved 2026-08 (Phase 3 item 3):** this directory was `Client/DXLib/`.
> Files kept their names and the library target is still `dxlib`. The
> `#include` examples below were updated to the `Platform/` path; the rest
> of this document predates the move (including mentions of the
> never-ported Windows-DirectX backend).

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
|  |  | `dxlib_input_init()` |
|  |  | `dxlib_input_key_down()` |
|  |  | `dxlib_input_get_mouse_pos()` |
|  |  | `dxlib_input_get_mouse_buttons()` |
|  |  | `dxlib_input_get_mouse_wheel()` |
|  |  | `dxlib_input_update()` |
|  |  | `dxlib_sound_load_wav()` ( SDL_mixer) |
|  |  | `dxlib_sound_play()` |
|  |  | `dxlib_sound_stop()` |
|  |  | `dxlib_sound_set_volume()` |
|  |  | `dxlib_music_load()` ( SDL_mixer) |
|  |  | `dxlib_music_play()` |
| / |  | `dxlib_music_pause()/resume()` |
|  |  | `dxlib_music_set_volume()` |
|  |  | `dxlib_get_backend_name()` |

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


dxlib_input_init(window_handle);


while (running) {

    dxlib_input_update();


    if (dxlib_input_key_down(DIK_W)) {

    }


    int x, y;
    dxlib_input_get_mouse_pos(&x, &y);


    int left, right, center;
    dxlib_input_get_mouse_buttons(&left, &right, &center);
    if (left) {

    }
}


dxlib_input_release();
```

### 

```cpp
#include "Platform/DXLibBackend.h"


dxlib_sound_init(window_handle);


dxlib_sound_t sound = dxlib_sound_load_wav("explosion.wav");


dxlib_sound_play(sound, 0); // 0 = 


dxlib_sound_set_volume(sound, 50);


dxlib_sound_stop(sound);


dxlib_sound_free(sound);


dxlib_sound_release();
```

### 

```cpp
#include "Platform/DXLibBackend.h"


dxlib_music_init(window_handle);


dxlib_music_load("background.mp3");


dxlib_music_play(1);


dxlib_music_set_volume(70);

// /
dxlib_music_pause();
dxlib_music_resume();


dxlib_music_stop();


dxlib_music_release();
```

## API 

### 

|  CDirectInput | DXLibBackend API | SDL  |
|-------------------|------------------|----------|
| `KeyDown(DIK_W)` | `dxlib_input_key_down(DIK_W)` | SDL_GetKeyboardState |
| `m_mouse_x, m_mouse_y` | `dxlib_input_get_mouse_pos()` | SDL_GetMouseState |
| `m_lb_down` | `dxlib_input_get_mouse_buttons()` | SDL_GetMouseState |
| `m_mouse_z` | `dxlib_input_get_mouse_wheel()` | SDL_MouseWheelEvent |
| `UpdateInput()` | `dxlib_input_update()` | SDL_PollEvent |

### 

|  CDirectSound | DXLibBackend API | SDL  |
|-------------------|------------------|----------|
| `LoadWav(filename)` | `dxlib_sound_load_wav()` | Mix_LoadWAV |
| `Play(buffer, loop)` | `dxlib_sound_play(sound, loop)` | Mix_PlayChannel |
| `Stop(buffer)` | `dxlib_sound_stop(sound)` | Mix_HaltChannel |
| `AddVolume(buffer, n)` | `dxlib_sound_set_volume()` | Mix_Volume |
| `RightPan/LeftPan` | `dxlib_sound_set_pan()` |   |

### 

|  CDirectMusic | DXLibBackend API | SDL  |
|-------------------|------------------|----------|
| `Play(filename)` | `dxlib_music_load() + play()` | Mix_LoadMUS + Mix_PlayMusic |
| `Stop()` | `dxlib_music_stop()` | Mix_HaltMusic |
| `Pause()` | `dxlib_music_pause()` | Mix_PauseMusic |
| `Resume()` | `dxlib_music_resume()` | Mix_ResumeMusic |
| `SetCurrentTempo()` | `dxlib_music_set_tempo()` |   |

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

 (`#ifdef DXLIB_BACKEND_SDL`) 

```cpp
// CDirectInput.cpp / CDirectInput_Adapter.cpp
#ifdef DXLIB_BACKEND_SDL
    //  DXLibBackend API (SDL2)
    dxlib_input_init(hWnd);
    dxlib_input_update();
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
    //  dxlib_input_* 
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
dxlib_input_init(window);
if (dxlib_input_key_down(DIK_W)) { ... }
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
