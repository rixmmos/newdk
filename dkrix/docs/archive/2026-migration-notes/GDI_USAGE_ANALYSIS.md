# Windows GDI 

## 

**Windows GDI  Windows **

## 

### 1. 



#### Windows  (PLATFORM_WINDOWS)
```cpp
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <DSound.h>

//  GDI
void Base::SetFont(...) {
    HFONT hfont = CreateFontIndirect(&lf);  // GDI
    pi.hfont = hfont;
}

void Base::~Base() {
    DeleteObject(m_small_pi.hfont);  // GDI
    DeleteObject(m_chatting_pi.hfont);
    // ...  DeleteObject 
}
#endif
```

####  Windows macOS/Linux
```cpp
#else
#include "../../basic/Platform.h"
#include "TextSystem/FontHandleUtil.h"

//  TextSystem (SDL + freetype2)
void Base::SetFont(...) {
    pi.hfont = TextSystem::EncodeFontSizeHandle(lf.lfHeight);  // SDL
}

// DeleteObject  Platform.h  stub 
static inline int DeleteObject(void* hObject) {
    (void)hObject;
    return 1;  //  TRUE
}
#endif
```

### 2. 

**** `VS_UI/src/VS_UI_Base.cpp`

|  | Windows (GDI) | macOS/Linux (TextSystem) |
|------|---------------|---------------------------|
|  | `CreateFontIndirect()` | `TextSystem::EncodeFontSizeHandle()` |
|  | `DeleteObject()` | Stub |
|  | `HANGUL_CHARSET`, `GB2312_CHARSET` | `DEFAULT_CHARSET` |
|  | "" | "Arial" |

### 3. Platform.h  GDI Stub

 Windows  GDI  stub 

```cpp
// basic/Platform.h

/* GDI object management functions - stub implementations */
static inline int DeleteObject(void* hObject) {
    (void)hObject;
    /* Stub - Windows GDI object deletion */
    return 1; /* Return TRUE */
}

static inline void* GetStockObject(int nIndex) {
    (void)nIndex;
    return NULL;
}
```

**** 

### 4. 

|  | Windows | macOS/Linux |
|------|---------|-------------|
|  | **GDI** () | TextSystem  |
|  | GDI | TextSystem  |
| DeleteObject |  GDI  | Stub |
| CreateFont |  GDI  |  |

---

##  GDI

###  Windows  Windows 

****

1. ** `#ifdef PLATFORM_WINDOWS`  GDI **
   -  CreateFontIndirect 
   -  DeleteObject 
   -  LOGFONT 

2. ** TextSystem**
   -  TextSystem::EncodeFontSizeHandle
   -  GDI stub Platform.h 

3. ****
   - TextSystem 
   - Windows 
   - /

### 

** Windows **

****
- Windows  GDI
- macOS/Linux  TextSystem
- 

****
- 
- 

---

## 

```
VS_UI_Base.cpp
     #ifdef PLATFORM_WINDOWS
           Windows GDI (CreateFontIndirect, DeleteObject)
    
     #else (macOS/Linux)
            TextSystem (SDL + freetype2)
                   FontHandleUtil.h
```

---

## 

1. **GDI ** -  Windows 
2. **macOS/Linux  TextSystem** -  GDI
3. **** - Windows  GDI TextSystem

---

## 

###  A
 Windows 

###  B GDI
 Windows  Windows  TextSystem
1.  TextSystem  Windows 
2.  `#ifdef PLATFORM_WINDOWS`  GDI 
3.  TextSystem

###  C TextSystem
 TextSystem  GDI
```bash
#  TextSystem 
grep -r "TextSystem" VS_UI/src/ --include="*.cpp" | wc -l

#  TextSystem 
ls Client/TextSystem/
```

---

****
1. **** - Windows  GDI
2. ** GDI** -  TextSystem
3. ** TextSystem** -  TextSystem 


