# spec.md:  Windows 

## 

 SDL2  Windows  SDL2  Windows  SDL2 

## 

 Windows + DirectX  SDL2  `#ifdef PLATFORM_WINDOWS` 
1.  DirectX/DirectSound 
2. Windows GDI  TextSystem 
3.  Windows  anti-cheat

## 

### 
- ** 321 ** `#ifdef PLATFORM_WINDOWS` 
- ** ~60+ **

### 

|  |  |  |
|------|------|----------|
|  DirectX  | 21 | **** - SDL2  |
| DirectSound  | 15 | **** - SDL_mixer  |
| GDI  |  | **** - TextSystem  |
| Windows  | 173 | **** |
| Anti-cheat/ |  | **** - Windows  |
|  (WSA) |  | **** -  |

###  ( PLATFORM_WINDOWS )

1. **Client/GameMain.cpp** (56 ) - 
2. **Client/MTopView.cpp** (17 ) - 
3. **Client/GameInit.cpp** (17 ) - 
4. **Client/MinTr.h** (12 ) - 
5. **Client/MTopViewDraw.cpp** (8 ) - 

## 

### Phase 1: 

****
- `Client/MTopView.cpp` -  `if (true)`  DirectX 
- `Client/MTopViewDraw.cpp` - 
- `Client/GameMain.cpp` -  Direct3D 

****
```cpp

#ifdef PLATFORM_WINDOWS
    if (true) {
        //  DirectX 
    }
#endif


// ()
```

### Phase 2:  DirectSound 

****
- `Client/GameMain.cpp` - DirectSound 
- `Client/soundbuf.cpp` - 
- `Client/DXLib/soundbuf.cpp`

****
```cpp

#ifdef PLATFORM_WINDOWS
    if( g_SDLAudio.IsInit() ) {
        // DirectSound 
    }
#endif


// SDL_mixer 
```

### Phase 3: 

****
- `VS_UI/src/VS_UI_Base.cpp` - GDI vs TextSystem
- `Client/SpriteLib/*.h` -  windows.h 

****
```cpp

#ifdef PLATFORM_WINDOWS
#include <windows.h>
//  GDI
#else
#include "TextSystem/FontHandleUtil.h"
//  TextSystem
#endif

//  (Windows  TextSystem)
#include "TextSystem/FontHandleUtil.h"
//  TextSystem
```

### Phase 4: 

****
-  `#include <windows.h>` 
-  Windows 

****
```cpp

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <MMSystem.h>
#endif


#include "basic/Platform.h"  //  stub
```

### Phase 5: 

****
-  (WSAStartup/WSACleanup) - BSD sockets 
-  -  Windows 
- Anti-cheat  -  Windows 
-  -  Windows 

## 

-  `basic/Platform.h`  stub 
-  Windows anti-cheat
-  CMake 

## 

1.   `#ifdef PLATFORM_WINDOWS` 
2.   DirectX/DirectSound 
3.  GDI  TextSystem
4.   macOS 
5.   Windows  SDL2
6.  

## 

|  |  |  |
|------|------|----------|
| Windows  |  |  |
|  |  |  |
|  |  |  |

## 

- **** SDL2  
- **** TextSystem  
- **** 