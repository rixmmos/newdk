# Effect Viewer - 

##  

 Effect Viewer ****

### 

#### 1. EffectSpriteType  
****: `EffectSpriteTypeTable.h/cpp`

 `Data/Info/EffectSpriteType.inf`  EffectID  BltType

```cpp

m_effectSpriteTypeTable = new EffectSpriteTypeTable();
m_effectSpriteTypeTable->LoadFromFile("Data/Info/EffectSpriteType.inf");

//  BltType
BYTE bltType = m_effectSpriteTypeTable->GetBltType(effectID);
```

#### 2.  BltType  

****:  Effect 74  BLT_EFFECT
```
Effect 74 -> BltType=1 (BLT_EFFECT) //  
```

****:  BltType
```
Effect 74 -> BltType=3 (BLT_SCREEN) //  
```

### 

#### Effect 74 (Screen Blend Mode)
```
Effect 74 -> BltType=3 (BLT_SCREEN) from EffectSpriteType table
========================================
Created Effect Type 74 (BltType=3)
FrameID=74 | MaxFrame=4 | Direction=0
========================================

Update #1 | EffectID=74 | FrameID=74 | Frame=0->1
=== Render Info ===
EffectID=74 | FrameID=74 | Frame=1 | Dir=0 | BltType=3
SpriteID=856 | Offset=(-26,-29)
   Rendering ScreenSprite ID=856 at (374,271)
     Size=101x61 | Pitch=800
```

##  

### 
```bash
cd /Users/genius/project/opendarkeden/client
./build_and_run_effect_viewer.sh
```

### 
|  |  |
|------|------|
| **1-9** |  Effect Type |
| **/** | / Effect |
| **/** |  (8) |
| **SPACE** | / |
| **T** |  |
| **ESC** |  |

##  

###  
- Alpha FPK: 1348 effects (BLT_EFFECT)
- Normal FPK: 67 effects (BLT_NORMAL)
- Screen FPK: 578 effects (BLT_SCREEN)
- Shadow FPK: 1 effect (BLT_SHADOW)

###  EffectSpriteType 
-  1994 
-  EffectID  BltType
-  100% 

###  
- **Alpha Blending**: RenderAlphaSprite()
- **Normal Blending**: RenderNormalSprite()
- **Screen Blending**: RenderScreenSprite()
- **Shadow Blending**: RenderShadowSprite() ()

##  

 Effect Viewer ** Effect **

 **** BltType 
 ** 4 **
 ****
 
 
 

**** 
