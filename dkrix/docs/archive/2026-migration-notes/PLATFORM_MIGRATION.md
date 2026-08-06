

##  

### 1. Level 1 - Foundation

****
```
basic/
 Platform.h              # 
 PlatformWindows.cpp     # Windows 
 PlatformSDL.cpp         # SDL/POSIX 
 Directory.cpp           # 
 Typedef.h               #  Platform.h
 PlatformUtil.h          # 
 CMakeLists.txt          # 
 README.md               # 
```

### 2. API 

|  Windows API |  API | Windows | SDL/POSIX |
|-----------------|-------------|---------|-----------|
| **** |
| `timeGetTime()` | `platform_get_ticks()` | timeGetTime | SDL_GetTicks |
| `GetTickCount()` | `platform_get_ticks()` | timeGetTime | SDL_GetTicks |
| `QueryPerformanceCounter()` | `platform_get_performance_counter()` | QueryPerformanceCounter | SDL_GetPerformanceCounter |
| `Sleep(ms)` | `platform_sleep(ms)` | Sleep | SDL_Delay |
| **/** |
| `CreateThread()` | `platform_thread_create()` | CreateThread | SDL_CreateThread |
| `WaitForSingleObject(thread)` | `platform_thread_wait()` | WaitForSingleObject | SDL_WaitThread |
| `CreateMutex()` | `platform_mutex_create()` | CreateMutex | SDL_CreateMutex |
| `WaitForSingleObject(mutex)` | `platform_mutex_lock()` | WaitForSingleObject | SDL_LockMutex |
| `ReleaseMutex()` | `platform_mutex_unlock()` | ReleaseMutex | SDL_UnlockMutex |
| `CreateEvent()` | `platform_event_create()` | CreateEvent | SDL_Cond + SDL_Mutex |
| `WaitForSingleObject(event)` | `platform_event_wait()` | WaitForSingleObject | SDL_CondWait |
| `SetEvent()` | `platform_event_signal()` | SetEvent | SDL_CondSignal |
| **** |
| `GetModuleFileName()` | `platform_get_executable_dir()` | GetModuleFileNameA | readlink / _NSGetExecutablePath |
| `GetFileAttributes()` | `platform_file_exists()` | GetFileAttributes | stat |
| `CreateDirectory()` | `platform_create_directory()` | CreateDirectoryA | mkdir |
| **** |
| `LoadLibrary()` | `platform_lib_load()` | LoadLibraryA | SDL_LoadObject |
| `GetProcAddress()` | `platform_lib_get_symbol()` | GetProcAddress | SDL_LoadFunction |
| `FreeLibrary()` | `platform_lib_free()` | FreeLibrary | SDL_UnloadObject |
| **** |
| `RegOpenKeyEx()` | `platform_config_get_string()` | RegOpenKeyExA |  |
| `RegQueryValueEx()` | (included) | RegQueryValueExA |  |
| `RegSetValueEx()` | `platform_config_set_string()` | RegSetValueExA |  |

### 3. 

**CMake **
-  
-   SDL Windows 
-  Windows/Linux/macOS
-  

****
```bash
# Windows ()
cmake -B build -DUSE_SDL_BACKEND=OFF
cmake --build build

# Windows (SDL )
cmake -B build -DUSE_SDL_BACKEND=ON
cmake --build build

# Linux/macOS ( SDL)
cmake -B build
cmake --build build
```

### 4. 

****
-  
-  
-  
-  
-  
-  
-  
-  

****
```bash
cd build
./tests/test_platform
```

##  

### 

 `basic/` ****

```cpp

#include "Typedef.h"          // DWORD, BYTE, WORD 
#include "PlatformUtil.h"     // g_GetCtrlPushState(), SCAN_CODE
#include "Directory.h"        // gC_directory.GetProgramDirectory()
#include "BasicMemory.h"      // DeleteNew, DeleteNewArray
#include "BasicException.h"   // _Error, CheckMemAlloc
#include "DLL.h"              // DllExport, DllImport
```

### 

** Windows API **

#### DXLib 
```cpp

#include <windows.h>
HANDLE hThread = CreateThread(...);


#include "basic/Platform.h"
platform_thread_t hThread = platform_thread_create(...);
```

#### Client 
```cpp

DWORD start = timeGetTime();
Sleep(100);


#include "basic/Platform.h"
DWORD start = platform_get_ticks();
platform_sleep(100);
```

##  

```

  Level 5: Client (Main Executable)                      
  - Game logic, network, UI integration                  

                     

  Level 4: VS_UI (UI Framework)                          
  - Widgets, dialogs, Korean IME                         

                     

  Level 3: Graphics & Rendering                          
   SpriteLib  (Sprite animation, palette system)      
   D3DLib     (Direct3D wrapper)                      
   framelib   (Frame management)                      

                     

  Level 2: Platform Abstraction                          
   DXLib (DirectX wrapper)                   
   WinLib (Windows API)                      

                     

  Level 1: Foundation                           
   basic/Platform.h   ()                     
   basic/Platform*.cpp ()                         
   basic/Typedef.h    ()                      
   basic/Directory.h   ()                     

```

##  

###  2DXLib  3-4 

****  DXLib  basic/Platform.h

****
1.  **CDirectInput** - 
   -  `platform_thread_*` 
   -  SDL2  Windows 

2.  **CDirectSound** - 
   - Windows  DirectSound
   - SDL  SDL_mixer

3.  **CDirectMusic** - 
   - Windows  DirectMusic
   - SDL  SDL_mixer music

4.  **CDirectDraw** - 
   -  engine/sprite  SDL2 

###  3SpriteLib  2-3 

****  engine/sprite  SDL2 

** A**  API engine
```cpp
// CSprite  engine  DecodedSprite
class CSprite {
    DecodedSprite* m_engineSprite;
public:
    void Blt(WORD* pDest, WORD pitch) {
        //  engine
    }
};
```

** B**  engine API
```cpp
//  engine
#include "engine/sprite/include/sprite.h"
Sprite sprite;
sprite_load(&sprite, file);
```

###  4WinLib  2-3 

**** CWinMain  SDL2 

```cpp
// CWinMain  SDL_Window
class CWinMain {
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
public:
    BOOL Init(const WINMAIN_INFO* pInfo) {
        //  SDL 
    }
};
```

###  5VS_UI  4-6 

**** UI  engine/ui 

###  6Client  2-3 

****  SDL2 

##  

###  Level 1: Foundation ()
- [x] Platform.h 
- [x] PlatformWindows.cpp 
- [x] PlatformSDL.cpp 
- [x] Directory.cpp 
- [x] Typedef.h 
- [x] PlatformUtil.h 
- [x] CMakeLists.txt 
- [x]  test_platform
- [x]  README.md

###  Level 2: DXLib ()
- [ ] CDirectInput 
- [ ] CDirectSound 
- [ ] CDirectMusic 
- [ ] CDirectDraw 

###  Level 3: SpriteLib
- [ ]  engine/sprite 
- [ ] 
- [ ] 
- [ ]  ColorSet 

###  Level 4: WinLib
- [ ] CWinMain SDL2 
- [ ] 
- [ ] WinMain 

###  Level 5: VS_UI
- [ ] UI 
- [ ]  engine/ui 

##  

1. **** - 
2. **** - 
3. **** - 
4. **** - Windows  API
5. **** - Linux/macOS  SDL2

##  

****
```
basic/
 Platform.h              # 310 
 PlatformWindows.cpp     # 230 Windows 
 PlatformSDL.cpp         # 320 SDL/POSIX 
 Directory.cpp           # 80 
 Typedef.h               # 
 PlatformUtil.h          # 
 CMakeLists.txt          # 
 README.md               # 280 

tests/
 CMakeLists.txt          # 
 test_platform.cpp       # 270 

CMakeLists.txt              # 
PLATFORM_MIGRATION.md       # 
```

****  1,500 

---

**** 2025.01.14
**** Level 1 (Foundation)  
****  Level 2 (DXLib) 
