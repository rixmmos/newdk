# Windows 

## 
2026-02-03

## 
 Windows  Windows SDL2 

---

## 

### 1.  Windows  

****
- `basic/PlatformWindows.cpp` (275 ) - Windows 

****  Windows  SDL  Windows 

### 2.  CMake  

#### 2.1  CMakeLists.txt
****
```cmake
if(WIN32)
    option(USE_SDL_BACKEND "Use SDL backend instead of native Windows APIs" OFF)
else()
    set(USE_SDL_BACKEND ON CACHE BOOL "Use SDL backend instead of native Windows APIs" FORCE)
endif()
```

****
```cmake
# SDL backend is now mandatory on all platforms (Windows native support removed)
set(USE_SDL_BACKEND ON CACHE BOOL "Use SDL backend instead of native Windows APIs" FORCE)
```

****  SDL  Windows 

#### 2.2 basic/CMakeLists.txt
****
```cmake
if(WIN32 AND NOT USE_SDL_BACKEND)
    list(APPEND BASIC_SOURCES PlatformWindows.cpp)
    message(STATUS "Basic: Compiling PlatformWindows.cpp")
else()
    list(APPEND BASIC_SOURCES PlatformSDL.cpp)
    message(STATUS "Basic: Compiling PlatformSDL.cpp")
endif()
```

****
```cmake
# Windows native implementation removed (SDL2 migration) - Always use SDL backend
list(APPEND BASIC_SOURCES PlatformSDL.cpp)
message(STATUS "Basic: Compiling PlatformSDL.cpp")
```

****  PlatformSDL.cpp PlatformWindows.cpp

---

##  

### 
```
[100%] Built target DarkEden
```
 ****

### 
```
-- Basic: Using SDL backend (required on this platform)
-- Basic: Compiling PlatformSDL.cpp
-- DXLib: Using SDL2 backend (required on this platform)
-- DXLib: SDL2_mixer NOT found - audio support disabled
-- VS_UI library will be built...
-- DarkEden executable will be built...
```

****  SDL2 

---

## 

### PLATFORM_WINDOWS 

****  SDL  `#ifdef PLATFORM_WINDOWS` 

1. ****
   - Windows.h vs Platform.h 
   - HMMIO, MMCKINFO 
   - DirectSound/DirectMusic 

2. ** API **
   - Windows 
   - MessageBox
   - 
   - 

3. ****
   - 
   - 

****  321  PLATFORM_WINDOWS 

****
- 
- SDL  Windows 
- 
-  SDL 

---

## 

 Windows 

###  1:  
-  D3DLib 3 
-  11  CDirect3D 
-  VC6 5 

###  2:  
-  5  EXECryptor/GameGuard/ACProtect 

###  3:  
-  321  PLATFORM_WINDOWS 
-  31 
-  1 mp3.cpp

---

## 

### SDL2 : 100% 

#### 
-  Direct3D  SDL2
-  DirectDraw  SDL2
-  D3DLib 

#### 
-  DirectInput  SDL2
-  /

#### 
-  DirectSound  SDL2_mixer
-  DirectMusic  SDL2_mixer

#### 
-  Windows GDI  SDL2 + freetype2 (TextSystem)

#### 
-  Windows 
-  SDL 

---

## 

### SDL2 Only
```

         Game Client Code           

   UI Layer (VS_UI)                 
   - SDL-based rendering            
   - TextSystem (SDL + freetype2)   

   Game Logic (Client/)             
   - Sprite rendering (SDL2)        
   - Audio (SDL2_mixer)             
   - Input (SDL2)                   

   Platform Abstraction (basic/)     
   - Platform.h (cross-platform)    
   - PlatformSDL.cpp (SDL only)     

              

         SDL2 Libraries             
   SDL2, SDL2_image, SDL2_ttf       
   SDL2_mixer (optional)            

              

      Operating System               
   Windows, macOS, Linux            

```

### 
```
 PlatformWindows.cpp - Windows native implementation
 D3DLib/ - Direct3D stub implementations
 VC6 project files - Visual C++ 6.0 projects
 Windows native build option
 DirectX backend support
```

---

## 

### 
|  |  |
|------|------|
|  | 1  (PlatformWindows.cpp) |
|  | 2  (CMakeLists.txt, basic/CMakeLists.txt) |
|  | 275  |

### 
|  |  |
|------|------|
|  | 10  |
|  | ~30  |
|  | ~1800  |

---

## 

### CMake 
```cmake
#  SDL 
USE_SDL_BACKEND = ON ()


BUILD_ENGINE = ON
BUILD_TESTS = OFF
```

### 
```
PLATFORM_WINDOWS - Windows 
PLATFORM_MACOS   - macOS 
PLATFORM_LINUX   - Linux 
USE_SDL_BACKEND  - SDL 
SPRITELIB_BACKEND_SDL - Sprite  SDL 
```

---

## 

###  Windows 
**** 
1. SDL2 
2. Windows  API

**** 
1. SDL2 

****
-  
-  
-  
-  

****
-   Windows  API

###  Windows 
**** -  SDL2 

---

## 

### 1. 
```bash
# macOS/Linux
./build/debug-asan/bin/DarkEden

# Windows ()
.\build\debug-asan\bin\DarkEden.exe
```

### 2. 

-  321  PLATFORM_WINDOWS 
- 
- 

**** 

### 3. Git 
```bash
git add basic/PlatformWindows.cpp basic/CMakeLists.txt CMakeLists.txt
git commit -m "build: remove Windows native platform support

- Delete PlatformWindows.cpp (Windows native implementation)
- Force SDL2 backend on all platforms including Windows
- Update CMakeLists.txt to remove USE_SDL_BACKEND option
- Simplify basic library build to always use PlatformSDL.cpp

SDL2 migration is complete. All platforms now use SDL2 backend.
Verified: Project builds successfully on macOS with SDL2."
```

---

## 

###  
- [x]  PlatformWindows.cpp
- [x]  CMakeLists.txt  SDL 
- [x]  basic/CMakeLists.txt  Windows 
- [x] 
- [x]  SDL2 

###  SDL2  
- [x] Direct3D  SDL2 
- [x] DirectDraw  SDL2 
- [x] DirectInput  SDL2 
- [x] DirectSound/DirectMusic  SDL2_mixer 
- [x] Windows GDI  SDL2 + freetype2 
- [x] Platform   SDL only 

---

## 

 Windows ** SDL2 **

1. **SDL2  100% ** - 
2. **** - 
3. **** -  SDL2 
4. **** - 

 321  PLATFORM_WINDOWS 
- 
- 
-  SDL2 



---

**** 2026-02-03  
****   SDL2  
****  -  Windows   
**SDL2 ** 100%  
