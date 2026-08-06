# SDL 

## 
2026-02-03

## 
 SDL2  DirectX  Windows API 

---

##  1:  

### 1.1  D3DLib 
****
- `Client/D3DLib/CDirect3D.h`
- `Client/D3DLib/CDirect3D.cpp`
- `Client/D3DLib/DX3D.h`

****  stub 

### 1.2  CMakeLists.txt
****  D3DLib  include 2 

### 1.3  CDirect3D 
**11 **
- `Client/GameMain.cpp` -  `CDirect3D::Init()` 
- `Client/MTopView.cpp` -  `CDirect3D::GetDevice()->BeginScene()` 2 + DX3D.h include
- `Client/GameInit.cpp` -  `CDirect3D::Init()`  `GetDevice()->GetCaps()` 
- `Client/MTopViewDraw.inl` -  2 DRAW_TEXTURE_SPRITE, DRAW_TEXTURE_SURFACE
- `Client/CTexturePartManager.cpp` -  DX3D.h include
- `Client/CSpriteTexturePartManager.cpp` -  DX3D.h include
- `Client/CShadowPartManager.cpp` -  DX3D.h include
- `Client/CNormalSpriteTexturePartManager.cpp` -  DX3D.h include

### 1.4  VC6 
**5 **
- `VS_UI/lib.dsp`
- `VS_UI/lib.dsw`
- `VS_UI/VS_UI.dsp`
- `VS_UI/Rar.dsp`
- `VS_UI/Rar.dsw`

**** VC6  CMake

---

##  2:  Platform.h 

### 
**** - Platform.h  stub 
- MCI  - 
- Stock objects (GetStockObject, LoadIcon, LoadCursor) - 
- IWebBrowser2 stub - 

****  stub 

---

##  4:  

### 5 
1. **Client/GameFunctions.cpp**
   -  GameGuard (nProtect) 

2. **Client/Client.cpp**
   -  EXECryptor.h include 2 

3. **Client/CGameUpdate.cpp**
   -  EXECryptor  ThemidaSDK include 

4. **Client/Packet/Lpackets/LCReconnectHandler.cpp**
   -  ACProtect.h include 

5. **Client/Packet/Lpackets/ACProtect.h**
   - ****

---

##  3:  

### 
**321 ** `#ifdef PLATFORM_WINDOWS` 

****
|  |  |  |
|------|------|------|
| includes | 173 | Windows.h vs Platform.h- **** |
| other | 97 |  - **** |
| directx_calls | 21 | DirectX API  - **** |
| directsound_cleanup | 15 | DirectSound  - **SDL ** |
| always_true | 11 | `if (true)`  - **** |
| empty_or_comments | 2 |  - **** |
| winapi | 2 | Windows API  - **** |

### 
1. **mp3.cpp** - Windows 
2. **GameInit.cpp** -  1  `#ifdef` 

### 
**** 
- Windows  vs SDL 
- 
- Windows API 
- 

---

##  

### 
```
[100%] Built target DarkEden
```
 ****

### 
-  `register` 
- ****

---

## 

### 
|  |  |
|----------|------|
|  | 9  |
|  | ~20  |
|  | ~500  + ~1000  |

### 
- **** D3DLib 3 
- **** VC6 5 
- **** ACProtect.h1 
- **** CMakeLists.txt D3DLib 
- **** 11  CDirect3D 
- **** 5 

---

## 

### 1. 
```bash

./build/debug-asan/bin/DarkEden
```

****
- 
- /
- 
- 
- UI 

### 2. 

****
-  97  "other" 
-  11  `if (true)` 
-  15  DirectSound  SDL 

****
- 
- 

### 3. Git 
```bash
git add -A
git commit -m "cleanup: remove DirectX and Windows API remnants after SDL migration

- Remove D3DLib directory (stub implementations)
- Remove CDirect3D calls from 11 files
- Delete VC6 project files (5 .dsp/.dsw files)
- Clean up copy protection comments (EXECryptor, GameGuard, ACProtect)
- Audit and simplify PLATFORM_WINDOWS conditionals
- Fix: unify include paths to use forward slashes

All changes verified: project builds successfully on macOS with SDL2 backend.
"
```

---

## 

###  
- [x] D3DLib 
- [x] CDirect3D 
- [x] VC6 
- [x] 
- [x] 
- [x] 

### 
- [ ] 
- [ ] 
- [ ]  git commit

---

## 

 **1500 **
-  D3DLib stub 
- VC6 
- 

SDL2  SDL2 

****  `#ifdef PLATFORM_WINDOWS` 
1. Windows  SDL
2.  API 
3. 



---

**** 2026-02-03  
****    
****  - 
