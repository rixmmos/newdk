# Platform Abstraction Layer

## 

 Dark Eden  Windows API  WindowsLinux  macOS 

## 

1. ** API ** - 
2. **** - 
3. **** - 
4. **** -  APIWindows SDL2

## 

```

     SpriteLib           

               

     basic/*.h         

 Typedef.h, PlatformUtil.h,          
 Directory.h, BasicMemory.h, etc.    

               

     Platform.h           

                              
       
 Platform             PlatformSDL     
 Windows.cpp          .cpp            
       
 Windows API          SDL2 + POSIX    
       
```

## 

```
basic/
 Platform.h              # 
 PlatformWindows.cpp     # Windows 
 PlatformSDL.cpp         # SDL/
 Typedef.h               # 
 PlatformUtil.h          # 
 Directory.h/.cpp        # 
 BasicMemory.h           # 
 BasicException.h        # 
 DLL.h                   # 
 CMakeLists.txt          # 
```

## 

###  
- `platform_get_ticks()` -  timeGetTime
- `platform_get_performance_counter()` - 
- `platform_get_performance_frequency()` - 
- `platform_sleep()` - 

###  /
- `platform_thread_create()` -  CreateThread
- `platform_thread_wait()` - 
- `platform_mutex_create/lock/unlock()` -  CreateMutex
- `platform_event_create/wait/signal()` -  CreateEvent

###  /
- `platform_get_path_separator()` - 
- `platform_file_exists()` - 
- `platform_get_executable_dir()` - 
- `platform_create_directory()` - 

###  
- `platform_is_ctrl_pressed()` -  Ctrl 
- `platform_get_scan_code()` - 

###  
- `platform_config_get_string()` -  RegQueryValueEx
- `platform_config_set_string()` -  RegSetValueEx

**** Windows Linux/macOS 

###  
- `platform_lib_load()` -  LoadLibrary
- `platform_lib_get_symbol()` -  GetProcAddress
- `platform_lib_free()` -  FreeLibrary

## 

### Windows

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### WindowsSDL 

```bash
mkdir build && cd build
cmake -DUSE_SDL_BACKEND=ON ..
cmake --build .
```

### Linux/macOS

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## 



```bash

cmake --build .


./tests/test_platform  # Linux/macOS
tests\Debug\test_platform.exe  # Windows
```


-  
-  
-  
-  
-  
-  
-  
-  

## 

### 

****
```cpp
HANDLE hThread = CreateThread(NULL, 0, ThreadFunc, param, 0, &threadId);
WaitForSingleObject(hThread, INFINITE);
CloseHandle(hThread);
```

****
```cpp
platform_thread_t hThread = platform_thread_create(ThreadFunc, param);
platform_thread_wait(hThread);
```

### 

****
```cpp
DWORD start = timeGetTime();
Sleep(100);
DWORD elapsed = timeGetTime() - start;
```

****
```cpp
DWORD start = platform_get_ticks();
platform_sleep(100);
DWORD elapsed = platform_get_ticks() - start;
```

### 

****
```cpp
HKEY hKey;
RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\DarkEden", 0, KEY_READ, &hKey);
DWORD size = sizeof(buffer);
RegQueryValueEx(hKey, "RunFileName", NULL, NULL, (BYTE*)buffer, &size);
RegCloseKey(hKey);
```

****
```cpp
DWORD size = sizeof(buffer);
platform_config_get_string("SOFTWARE\\DarkEden", "RunFileName", buffer, &size);
```

## 

 basic/ 

```cpp
#include "Typedef.h"           //   Platform.h
#include "PlatformUtil.h"      //   platform_is_ctrl_pressed()
#include "Directory.h"         //   platform_get_executable_dir()
```

****

## 

###  2 DXLib/

 `DXLib/PlatformDX.h`

```cpp
// CDirectInput  platform_thread_*  SDL2
// CDirectSound  SDL_mixer
```

###  3 SpriteLib

 engine/sprite  SDL2 

```cpp
#include "engine/sprite/include/sprite.h"
```

###  4 WinLib



```cpp
// CWinMain  SDL_Window + SDL_Renderer
```

## 

1. **** - Windows  `DarkEden.conf`
2. **** - Windows  `\` `/`
3. **** - Windows `.dll`Linux `.so`macOS `.dylib`
4. **** -  DWORD Windows 

## 

- Platform Abstraction Layer: 2025.01.14

## 

 Dark Eden 
