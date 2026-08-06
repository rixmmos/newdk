//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemUtil.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __ITEMUTIL_H__
#define __ITEMUTIL_H__

#include <list>
#include <string>

#include "Item.h"

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Creature;
class Inventory;
class PlayerCreature;
class Slayer;
class Ousters;
class Corpse;

struct ITEM_TEMPLATE;

enum ItemTraceLogType {
    ITEM_LOG_CREATE = 0, 
    ITEM_LOG_TRADE,      
    ITEM_LOG_MOVE,       
    ITEM_LOG_DELETE,     

    ITEM_LOG_MAX
};

enum ItemTraceDetailType {
    DETAIL_COMMAND = 0, 
    DETAIL_SHOPBUY,     
    DETAIL_PICKUP,      
    DETAIL_DROP,        
    DETAIL_EVENTNPC,    
    DETAIL_SHOPSELL,    
    DETAIL_TIMEOUT,     
    DETAIL_ENCHANT,     
    DETAIL_OPCLEAR,     
    DETAIL_TRADE,       
    DETAIL_MALLBUY,     

    DETAIL_MAX
};

enum ITLType {
    ITL_GET = 0, 
    ITL_DROP,    
    ITL_ETC,     

    ITL_MAX
};

enum ITLDType {
    ITLD_PICKUP = 0, 
    ITLD_TRADE,      
    ITLD_EVENTNPC,   
    ITLD_PETITEM,    // GET
    ITLD_ENCHANT,    // GET/DROP
    ITLD_MIXING,     // GET/DROP
    ITLD_OPTION,     // GET/DROP/ETC

    ITLD_NPCSHOP,     
    ITLD_WEBSHOP,     
    ITLD_PRIVATESHOP, 

    ITLD_GM,      
    ITLD_TIMEOUT, 
    ITLD_DELETE,  
    ITLD_MOVE,    // Mode 		GET/DROP/ETC

    ITLD_MAX
};

const string ItemTraceLogType2String[] = {
    "CREATE", // 0
    "TRADE",  // 1
    "MOVE",   // 2
    "DELETE", // 3
};

const string ItemTraceLogDetailType2String[] = {
    "COMMAND",  // 0
    "SHOPBUY",  // 1
    "PICKUP",   // 2
    "DROP",     // 3
    "EVENTNPC", // 4
    "SHOPSELL", // 5
    "TIMEOUT",  // 6
    "ENCHANT",  // 7
    "OPCLEAR",  // 8
    "TRADE",    // 9
    "MALLBUY",  // 10
};

const string ITLType2String[] = {
    "GET",  // 0
    "DROP", // 1
    "MOVE", // 2
    "ETC",  // 3
};

const string ITLDType2String[] = {
    "PICKUP",   // 0
    "TRADE",    // 1
    "EVENTNPC", // 2
    "PETITEM",  // 3
    "ENCHANT",  // 4
    "MIXING",   // 5
    "OPTION",   // 6

    "NPCSHOP",     // 7
    "WEBSHOP",     // 8
    "PRIVATESHOP", // 9

    "GM",      // 10
    "TIMEOUT", // 11
    "DELETE",  // 12
    "MOVE"     // 13
};

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isStackable(Item::ItemClass IClass);
bool isStackable(const Item* pItem);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isSameItem(Item::ItemClass IClass1, Item::ItemClass IClass2, ItemType_t type1, ItemType_t type2);
bool isSameItem(const Item* pItem1, const Item* pItem2);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool canStack(const Item* pItem1, const Item* pItem2);

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
bool isTwohandWeapon(const Item* pItem);
bool isMeleeWeapon(const Item* pItem);
bool isFighterWeapon(const Item* pItem);
bool isArmsWeapon(const Item* pItem);
bool isClericWeapon(const Item* pItem);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isSuitableMagazine(const Item* pGun, const Item* pMagazine, bool hasVivid);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isSlayerWeapon(Item::ItemClass IClass);
bool isAdvancedSlayerWeapon(Item::ItemClass IClass);
bool isVampireWeapon(Item::ItemClass IClass);
bool isOustersWeapon(Item::ItemClass IClass);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isSlayerArmor(Item::ItemClass IClass);
bool isVampireArmor(Item::ItemClass IClass);
bool isOustersArmor(Item::ItemClass IClass);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isSlayerAccessory(Item::ItemClass IClass);
bool isVampireAccessory(Item::ItemClass IClass);
bool isOustersAccessory(Item::ItemClass IClass);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isRepairableItem(const Item* pItem);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void repairItem(Item* pItem);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Durability_t computeMaxDurability(Item* pItem);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Bullet_t reloadArmsItem(Item* pWeapon, Item* pMagazine);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Bullet_t decreaseBullet(Item* pWeapon);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Bullet_t getRemainBullet(Item* pWeapon);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isPortableItem(Item* pItem);

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
bool isUsableItem(Item* pItem, Creature* pUser);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
ItemNum_t decreaseItemNum(Item* pItem, Inventory* pInventory, const string& OwnerID, Storage storage,
                          StorageID_t storageID, BYTE x, BYTE y);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void processItemBug(Creature* pCreature, Item* pItem);
void processItemBugEx(Creature* pCreature, Item* pItem);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Item* getRandomMysteriousItem(Creature* pCreature, Item::ItemClass itemClass, int maxLevel = 0);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

bool hasOptionType(const list<OptionType_t>& optionTypes, OptionType_t optionType);


bool hasOptionClass(const list<OptionType_t>& optionTypes, OptionType_t optionType);


void setOptionTypeFromField(list<OptionType_t>& optionTypes, const string& optionField);


void setOptionTypeToField(const list<OptionType_t>& optionTypes, string& optionField);


string getOptionTypeToString(const list<OptionType_t>& optionTypes);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isPossibleNextOption(ITEM_TEMPLATE* pTemplate);

ItemType_t getUpgradeItemType(Item::ItemClass IClass, ItemType_t itemType, ItemType_t upgradeCount);
ItemType_t getDowngradeItemType(Item::ItemClass IClass, ItemType_t itemType);
bool isPossibleUpgradeItemType(Item::ItemClass IClass);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

// TPOINT checkEventTree( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY );
// TPOINT checkEventDocument( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY );
// TPOINT checkEventDoll( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY );
TPOINT checkEventPuzzle(PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY, int start);


void deleteInventoryItem(Inventory* pInventory, CoordInven_t invenX0, CoordInven_t invenY0, CoordInven_t invenX1,
                         CoordInven_t invenY1);


bool addNewbieItemToInventory(Slayer* pSlayer, bool sendPacket = false);
bool addNewbieGoldToInventory(Slayer* pSlayer, bool sendPacket = false);
bool addNewbieItemToGear(Slayer* pSlayer, bool sendPacket = false);

bool addNewbieItemToInventory(Ousters* pOusters, bool sendPacket = false);
bool addNewbieGoldToInventory(Ousters* pOusters, bool sendPacket = false);
bool addNewbieItemToGear(Ousters* pOusters, bool sendPacket = false);

Item::ItemClass getBestNewbieWeaponClass(Slayer* pSlayer);


void makeOptionList(const string& options, list<OptionType_t>& optionList);

void saveDissectionItem(Creature* pCreature, Item* pTreasure, int x, int y);

bool canDecreaseDurability(Item* pItem);

bool canSell(Item* pItem);
bool canPutInStash(Item* pItem);
bool canTrade(Item* pItem);
bool isCoupleRing(Item* pItem);

//////////////////////////////////////////////////////////////////////////////
// Exchange System: Point-only trade item check functions
//////////////////////////////////////////////////////////////////////////////
// Check if item is Blue Sapphire (hard currency)
bool isBlueSapphire(Item* pItem);

// Get base option type by following PreviousType chain
OptionType_t getBaseOptionType(OptionType_t type);

// Check if item has 3 options and at least one is upgraded
bool isUpgradedThreeOptionItem(Item* pItem);

// Check if item can ONLY be traded via exchange (points)
bool isPointOnlyTradeItem(Item* pItem);

bool suitableItemClass(Item::ItemClass iClass, SkillDomainType_t domainType);



void setItemGender(Item* pItem, GenderRestriction gender);


bool bTraceLog(Item* pItem);


void remainTraceLog(Item* pItem, const string& preOwner, const string& owner, ItemTraceLogType logType,
                    ItemTraceDetailType detailType);
void remainTraceLogNew(Item* pItem, const string& owner, ITLType logType, ITLDType detailType, ZoneID_t zid = 0,
                       int x = 0, int y = 0);


void remainMoneyTraceLog(const string& preOwner, const string& owner, ItemTraceLogType logType,
                         ItemTraceDetailType detailType, int amount);


Item* createItemByGoodsID(DWORD goodsID);


bool bWinPrize(DWORD rewardID, DWORD questLevel);

void deleteFlagEffect(Corpse* pFlagPole, Item* pFlag);

Item* fitToPC(Item* pItem, PlayerCreature* pPC);

#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)

ItemType_t getItemTypeByItemLimit(Item::ItemClass itemClass, ItemType_t itemType);

#endif // __THAILAND_SERVER__

#endif
