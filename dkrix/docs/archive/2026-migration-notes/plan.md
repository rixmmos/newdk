# plan.md:  Windows 

## 

### 

 Windows 

####  1:  DirectX  (`if (true)` )
```cpp
#ifdef PLATFORM_WINDOWS
    if (true) {
        //  DirectX 
    }
#endif
```
**** 

####  2: DirectSound 
```cpp
#ifdef PLATFORM_WINDOWS
    if( g_SDLAudio.IsInit() ) {
        // DirectSound 
    }
#endif
```
**** SDL_mixer 

####  3: 
```cpp
#ifdef PLATFORM_WINDOWS
    // Windows 
#else
    // SDL 
#endif
```
****  SDL  Windows 

####  4: 
```cpp
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <MMSystem.h>
#else
#include "basic/Platform.h"
#endif
```
****  `#include "basic/Platform.h"`

####  5: GDI vs TextSystem
```cpp
#ifdef PLATFORM_WINDOWS
    HFONT hfont = CreateFontIndirect(&lf);  // GDI
#else
    pi.hfont = TextSystem::EncodeFontSizeHandle(lf.lfHeight);  // TextSystem
#endif
```
****  TextSystem Windows 

---

## 

### Phase 1:  ()

****
1. `Client/MTopView.cpp` (17  PLATFORM_WINDOWS)
2. `Client/MTopViewDraw.cpp` (8 )
3. `Client/GameMain.cpp` (56  - )

****
-  `if (true)` 
-  SDL 
-  DirectDraw/Direct3D 

### Phase 2: 

****
1. `Client/GameMain.cpp` - MP3/OGG 
2. `Client/soundbuf.cpp`
3. `Client/MMusic.cpp`

****
-  DirectSound 
-  SDL_mixer 

### Phase 3: 

****
 `#ifdef PLATFORM_WINDOWS` 

****
-  `#include <windows.h>`  `#include "basic/Platform.h"`
-  `<MMSystem.h>` Windows

### Phase 4: 

****
1. `VS_UI/src/VS_UI_Base.cpp`
2. `VS_UI/src/VS_UI_Title.cpp`
3.  UI 

****
-  GDI 
-  TextSystem

### Phase 5: 

****
1.  (WSAStartup) - 
2.  -  Windows-only
3. Anti-cheat  -  Windows-only

---

## 

### 

|  |  | PLATFORM_WINDOWS  |  |
|--------|------|----------------------|----------|
| P0 | Client/GameMain.cpp | 56 | / |
| P0 | Client/MTopView.cpp | 17 |  |
| P0 | Client/GameInit.cpp | 17 |  |
| P1 | Client/MinTr.h | 12 |  |
| P1 | Client/MTopViewDraw.cpp | 8 |  |
| P1 | Client/Client.cpp | 8 |  |
| P2 | Client/PacketFunction.cpp | 7 |  |
| P2 | Client/UIMessageManager.cpp | 5 | UI  |
| P2 | Client/ProfileManager.cpp | 4 |  |
| P2 | Client/MPlayer.cpp | 4 |  |
| P3 | VS_UI  () | 2-3  | UI  |

### 

 - 

---

## 

|  |  |  |  |
|------|--------|------|----------|
| Windows  |  |  |  phase  Windows  |
|  |  |  | macOS  Windows |
|  |  |  |  |

---

## 

1. ****
   ```bash
   # macOS
   make debug-asan

   # Windows ()
   cmake --build build --config Debug
   ```

2. ****
   - 
   - UI 
   - 
   - 

---

## 

1.   ()
2. Phase 1: 
3. Phase 2: 
4. Phase 3: 
5. Phase 4: 
6. Phase 5: 
7. 
8. 