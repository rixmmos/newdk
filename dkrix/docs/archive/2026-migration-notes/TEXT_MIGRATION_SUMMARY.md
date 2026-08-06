

## 

DarkEden .inf  ** GBK ** **UTF-8 **

 `MString::ConvertToUTF8IfNeeded()`  UTF-8  CP949/EUC-KR 

## 

**** GBK  UTF-8

## 

### 1. 

****: `tools/convert_inf_to_utf8.py`


- `CTypeTable<MString>` 
- `MStringMap` 
- `Nickname.inf` 

### 2. 

|  |  |  |
|------|------|------|
| Nickname.inf |  | 48  |
| MonsterName.inf |  | 21  |
| String.inf |  | 1607  |
| Help.inf |  | 44  |
| NPC.inf |  | 204  |
| itemOption.inf |  | 9  |
| Item.inf |  | 2724  |
| SkillInfo.inf |  | 79  () |
| Skill.inf |  | 485  |
| Chat.inf / Chat2.inf |  |  |
| Creature.inf |  |  |
| Zone.inf |  | 158  |

****: **13 ** **5500 **

****:
- Creature.inf 
- Zone.inf  "_", "_" 
-  `.backup` 

### 3. 

****

|  |  |  |
|------|------|------|
| STRExp.inf, DEXExp.inf, INTExp.inf | ~2KB |  |
| VampireExp.inf, OustersExp.inf | ~2KB |  |
| OustersRankExp.inf, PetExp.inf | ~1KB |  |
| STRBalanceInfo.inf | - |  |
| CreatureSprite.inf | 5KB | SpriteID |
| AddonSprite.inf | 32KB |  |
| CreatureActionSprite.inf | 29KB |  |
| QuestInfo.inf | 4KB |  |
| GuildMapper.inf | 9KB |  |
| Appendpatch.inf | 22KB |  |
| EffectSpriteType.inf, ActionEffectSpriteType.inf | - |  |

### 4. 



|  |  |  |
|------|------|------|
| eventquest.inf | 432B | + |
| InteractionObject.inf | 26B |  |
| PetMakeType.inf | - |  |
| WearingInfo.inf | - |  |
| HelpLevelInfo.inf | - |  |



### 3. 

#### `Client/MString.cpp`
-  CP949/EUC-KR  UTF-8 
-  UTF-8  Debug 
-  `#if 0` 

#### `Client/MString.h`
-  `ConvertToUTF8IfNeeded()` 

## 

 `.backup` 
- `Nickname.inf.backup`
- `MonsterName.inf.backup`
- `String.inf.backup`
- `Help.inf.backup`

## 

### 
```bash
python3 tools/convert_inf_to_utf8.py /path/to/DarkEden/Data/Info Nickname.inf
```

### 
```bash
python3 tools/convert_inf_to_utf8.py /path/to/DarkEden/Data/Info
```

## 

 UTF-8 

```python
import struct
with open('DarkEden/Data/Info/Nickname.inf', 'rb') as f:
    data = f.read()

#  UTF-8
```

## Item.inf 

 `Client/MItemTable.cpp` 270-350

```
ITEMCLASS_TABLE (CTypeTable<ITEMTYPE_TABLE>)
 4 bytes: class count (91)
 For each class:
     ITEMTYPE_TABLE (CTypeTable<ITEMTABLE_INFO>)
        4 bytes: item count
        For each item:
            ITEMTABLE_INFO
                3  MString (EName, HName, Description)
                   Each: 4 bytes length + data
                Binary data (85 bytes):
                   6  FrameID (2 bytes) = 12 bytes
                   4  SoundID (2 bytes) = 8 bytes
                   Grid Width/Height (1 byte each) = 2 bytes
                   Price (4 bytes)
                   Weight (2 bytes)
                   7  Value (4 bytes each) = 28 bytes
                   Require STR/DEX/INT (1 byte each) = 3 bytes
                   Require SUM (2 bytes)
                   Require Level/AdvLevel (1 byte each) = 2 bytes
                   MaleOnly/FemaleOnly (1 byte each) = 2 bytes
                   UseActionInfo (4 bytes)
                   SilverMax (4 bytes)
                   ToHit (4 bytes)
                   MaxNumber (4 bytes)
                   CriticalHit (4 bytes)
                DefaultOptionListSize (1 byte)
                DefaultOptionList (N bytes, where N = size)
                ItemStyle (4 bytes)
                ElementalType (4 bytes)
                Elemental (2 bytes)
                Race (1 byte)
                DescriptionFrameID (2 bytes)
```

 `NestedCTypeTableConverter::convert_item_file()` 

## Zone.inf 

 `Client/MZoneTable.cpp` 132-167

```
CZoneTable::LoadFromFile
 4 bytes: zone count (158)
 For each zone:
     ZONETABLE_INFO::LoadFromFile
         ID (2 bytes) - Zone ID
         Name (MString) - 4 bytes length + data **[]**
         Property (1 byte)
         MusicIDSlayer (2 bytes)
         MusicIDVampire (2 bytes)
         Filename (MString) - 
         InfoFilename (MString) - 
         TeenFilename (MString) - 
         numSound (4 bytes)
         numSound  soundID (2 bytes each)
         Safety (1 byte)
         CreatureColorSet (2 bytes)
         MinimapSpriteID (2 bytes)
         HolyLand (1 byte)
         ChatMaskByRace (1 byte)
         CannotAttackInSafe (1 byte)
         CannotUseSpecialItem (1 byte)
         CompetenceZone (1 byte)
         PKType (1 byte)
```

 `ZoneConverter::convert_file()`  `Name` 

****
- _
- _
- _
- _

## 

1.  Zone.inf - 158 
2.  Skill.inf / SkillInfo.inf - 
3. eventquest.inf 

**** 
-  Item.inf, itemOption.inf
-  Skill.inf, SkillInfo.inf
-  NPC NPC.inf
-  MonsterName.inf, Creature.inf
-  Zone.inf
-  String.inf, Help.inf
-  Chat.inf, Chat2.inf
-  Nickname.inf

## 

-  .inf  UTF-8
-  `.backup` 
-  `tools/convert_inf_to_utf8.py`  `FORMAT_HANDLERS` 

## 

 `make debug-asan` 

```
error: use of undeclared identifier 'IsValidUtf8'
```

****: `IsValidUtf8`  `LoadFromFile`  `LoadFromFile` 

****:  `LoadFromFile`  UTF-8  208-215 
1.  UTF-8
2.  debug 
3.  `IsValidUtf8`  `#if 0` 

 AddressSanitizer 
