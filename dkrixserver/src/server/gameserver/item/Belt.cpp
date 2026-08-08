//////////////////////////////////////////////////////////////////////////////
// Filename    : Belt.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Belt.h"

#include "DB.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "PCItemInfo.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Vampire.h"

// global variable declaration
BeltInfoManager* g_pBeltInfoManager = NULL;

ItemID_t Belt::m_ItemIDRegistry = 0;
Mutex Belt::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Belt::Belt()

//: m_ItemType(0), m_Durability(0), m_pInventory(NULL)
{
    setItemType(0);
    setDurability(0);
    m_pInventory = NULL;
    //	m_EnchantLevel = 0;
}

Belt::Belt(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0), m_pInventory(NULL)
{
    __BEGIN_TRY

    setItemType(itemType);
    setOptionType(optionType);

    BeltInfo* pBeltInfo = dynamic_cast<BeltInfo*>(g_pBeltInfoManager->getItemInfo(getItemType()));

    m_pInventory = new Inventory(pBeltInfo->getPocketCount(), 1);

    //	m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Belt::Belt() : Invalid item type or option type");
        throw("Belt::Belt() : Invalid item type or optionType");
    }

    __END_CATCH
}


//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
Belt::~Belt()

{
    SAFE_DELETE(m_pInventory);
}

//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Belt::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    if (itemID == 0) {
        __ENTER_CRITICAL_SECTION(m_Mutex)

        m_ItemIDRegistry += g_pItemInfoManager->getItemIDSuccessor();
        m_ItemID = m_ItemIDRegistry;

        __LEAVE_CRITICAL_SECTION(m_Mutex)
    } else {
        m_ItemID = itemID;
    }

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "INSERT INTO BeltObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
            << " X, Y, OptionType, Durability)"
            << " VALUES("
            << m_ItemID << ", "
            << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', " <<(int)storage << ", " << storageID << ", "
            <<(int)x << ", " <<(int)y << ", " <<(int)m_OptionType << ", " << m_Durability << ")";

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement createBeltStmt(
            pConn, "INSERT INTO BeltObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, "
                   "OptionType, Durability, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        createBeltStmt.bindLong(1, m_ItemID);
        createBeltStmt.bindLong(2, m_ObjectID);
        createBeltStmt.bindInt(3, getItemType());
        createBeltStmt.bindString(4, ownerID);
        createBeltStmt.bindInt(5, (int)storage);
        createBeltStmt.bindLong(6, storageID);
        createBeltStmt.bindInt(7, (int)x);
        createBeltStmt.bindInt(8, (int)y);
        createBeltStmt.bindString(9, optionField);
        createBeltStmt.bindInt(10, getDurability());
        createBeltStmt.bindInt(11, getGrade());
        createBeltStmt.bindInt(12, (int)m_CreateType);
        createBeltStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// destroy item
//--------------------------------------------------------------------------------
bool Belt::destroy()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    //-------------------------------------------------------





    //-------------------------------------------------------
    for (int i = 0; i < m_pInventory->getHeight(); i++) {
        for (int j = 0; j < m_pInventory->getWidth(); j++) {
            Item* pItem = m_pInventory->getItem(j, i);
            if (pItem != NULL) {
                pItem->destroy();
            }
        }
    }

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement destroyBeltStmt(pConn, "DELETE FROM BeltObject WHERE ItemID = ?");
        destroyBeltStmt.bindLong(1, m_ItemID);
        destroyBeltStmt.execute();

        if (destroyBeltStmt.getAffectedRowCount() == 0) {
            return false;
        }
    }
    END_DB(pStmt)

    __END_CATCH

    return true;
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Belt::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveBeltStmt(pConn, string("UPDATE BeltObject SET ") + field + " WHERE ItemID=?");
        tinysaveBeltStmt.bindLong(1, m_ItemID);
        tinysaveBeltStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Belt::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE BeltObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",OptionType = " <<(int)m_OptionType
            << ",Durability = " << m_Durability
            << ",EnchantLevel = " <<(int)m_EnchantLevel
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement saveBeltStmt(
            pConn, "UPDATE BeltObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveBeltStmt.bindLong(1, m_ObjectID);
        saveBeltStmt.bindInt(2, getItemType());
        saveBeltStmt.bindString(3, ownerID);
        saveBeltStmt.bindInt(4, (int)storage);
        saveBeltStmt.bindLong(5, storageID);
        saveBeltStmt.bindInt(6, (int)x);
        saveBeltStmt.bindInt(7, (int)y);
        saveBeltStmt.bindString(8, optionField);
        saveBeltStmt.bindInt(9, getDurability());
        saveBeltStmt.bindInt(10, getGrade());
        saveBeltStmt.bindInt(11, (int)getEnchantLevel());
        saveBeltStmt.bindLong(12, m_ItemID);
        saveBeltStmt.execute();

        for (int i = 0; i < m_pInventory->getHeight(); i++) {
            for (int j = 0; j < m_pInventory->getWidth(); j++) {
                Item* pItem = m_pInventory->getItem(j, 0);
                if (pItem != NULL) {
                    pItem->save(ownerID, STORAGE_BELT, m_ItemID, j, 0);
                }
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

void Belt::makePCItemInfo(PCItemInfo& result) const {
    Item::makePCItemInfo(result);

    BYTE SubItemCount = 0;

    
    for (int i = 0; i < getPocketCount(); i++) {
        Item* pBeltItem = getInventory()->getItem(i, 0);
        if (pBeltItem != NULL) {
            SubItemInfo* pSubItemInfo = new SubItemInfo();
            pSubItemInfo->setObjectID(pBeltItem->getObjectID());
            pSubItemInfo->setItemClass(pBeltItem->getItemClass());
            pSubItemInfo->setItemType(pBeltItem->getItemType());
            pSubItemInfo->setItemNum(pBeltItem->getNum());
            pSubItemInfo->setSlotID(i);

            result.addListElement(pSubItemInfo);

            SubItemCount++;
        }
    }

    result.setListNum(SubItemCount);
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Belt::toString() const

{
    StringStream msg;

    msg << "Belt("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Belt::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pBeltInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Belt::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pBeltInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Belt::getWeight() const

{
    __BEGIN_TRY

    return g_pBeltInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}
*/
//--------------------------------------------------------------------------------
// get pocket count
//--------------------------------------------------------------------------------
PocketNum_t Belt::getPocketCount(void) const

{
    __BEGIN_TRY

    BeltInfo* pBeltInfo = dynamic_cast<BeltInfo*>(g_pBeltInfoManager->getItemInfo(getItemType()));
    Assert(pBeltInfo != NULL);
    return pBeltInfo->getPocketCount();

    __END_CATCH
}

/*Defense_t Belt::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pBeltInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}

Protection_t Belt::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pBeltInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string BeltInfo::toString() const

{
    StringStream msg;

    msg << "BeltInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void BeltInfoManager::load()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement maxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM BeltInfo");
        Result* pResult = maxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement loadBeltInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, PocketCount, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, "
                   "UpgradeCrashPercent, NextOptionRatio, NextItemType, DowngradeRatio FROM BeltInfo");
        pResult = loadBeltInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            BeltInfo* pBeltInfo = new BeltInfo();

            pBeltInfo->setItemType(pResult->getInt(++i));
            pBeltInfo->setName(pResult->getString(++i));
            pBeltInfo->setEName(pResult->getString(++i));
            pBeltInfo->setPrice(pResult->getInt(++i));
            pBeltInfo->setVolumeType(pResult->getInt(++i));
            pBeltInfo->setWeight(pResult->getInt(++i));
            pBeltInfo->setRatio(pResult->getInt(++i));
            pBeltInfo->setDurability(pResult->getInt(++i));
            pBeltInfo->setDefenseBonus(pResult->getInt(++i));
            pBeltInfo->setProtectionBonus(pResult->getInt(++i));
            pBeltInfo->setPocketCount(pResult->getBYTE(++i));
            pBeltInfo->setReqAbility(pResult->getString(++i));
            pBeltInfo->setItemLevel(pResult->getBYTE(++i));
            pBeltInfo->setDefaultOptions(pResult->getString(++i));
            pBeltInfo->setUpgradeRatio(pResult->getInt(++i));
            pBeltInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pBeltInfo->setNextOptionRatio(pResult->getInt(++i));
            pBeltInfo->setNextItemType(pResult->getInt(++i));
            pBeltInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pBeltInfo);
        }
    }
    END_DB(pStmt)

    __END_DEBUG
    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void BeltLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, EnchantLevel FROM
        BeltObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement loadBeltStmt(
            pConn,
            "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, EnchantLevel, "
            "ItemFlag FROM BeltObject WHERE OwnerID = ? AND Storage IN (0, 1, 2, 3, 4, 9)");
        loadBeltStmt.bindString(1, pCreature->getName());
        Result* pResult = loadBeltStmt.execute();

        while (pResult->next()) {
            try {
                uint i = 0;

                Belt* pBelt = new Belt();

                pBelt->setItemID(pResult->getDWORD(++i));
                pBelt->setObjectID(pResult->getDWORD(++i));
                pBelt->setItemType(pResult->getDWORD(++i));

                if (g_pBeltInfoManager->getItemInfo(pBelt->getItemType())->isUnique())
                    pBelt->setUnique();

                BeltInfo* pBeltInfo = dynamic_cast<BeltInfo*>(g_pBeltInfoManager->getItemInfo(pBelt->getItemType()));
                Inventory* pBeltInventory = new Inventory(pBeltInfo->getPocketCount(), 1);

                pBelt->setInventory(pBeltInventory);

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pBelt->setOptionType(optionTypes);

                pBelt->setDurability(pResult->getInt(++i));
                pBelt->setGrade(pResult->getInt(++i));
                pBelt->setEnchantLevel(pResult->getInt(++i));
                pBelt->setCreateType((Item::CreateType)pResult->getInt(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Motorcycle* pMotorcycle = NULL;
                Inventory* pMotorInventory = NULL;
                // Item*       pItem           = NULL;
                Stash* pStash = NULL;

                if (pCreature->isSlayer()) {
                    pSlayer = dynamic_cast<Slayer*>(pCreature);
                    pInventory = pSlayer->getInventory();
                    pStash = pSlayer->getStash();
                    pMotorcycle = pSlayer->getMotorcycle();

                    if (pMotorcycle)
                        pMotorInventory = pMotorcycle->getInventory();
                } else if (pCreature->isVampire()) {
                    pVampire = dynamic_cast<Vampire*>(pCreature);
                    pInventory = pVampire->getInventory();
                    pStash = pVampire->getStash();
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pBelt)) {
                        pInventory->addItemEx(x, y, pBelt);
                    } else {
                        processItemBugEx(pCreature, pBelt);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pBelt);
                        } else {
                            processItemBugEx(pCreature, pBelt);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pBelt);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pBelt);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pBelt);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pBelt);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pBelt);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pBelt);
                    } else {
                        pStash->insert(x, y, pBelt);
                    }
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pBelt);
                    break;

                default:
                    throw Error("invalid storage or OwnerID must be NULL");
                }

            } catch (Error& error) {
                filelog("itemLoadError.txt", "[%s] %s", getItemClassName().c_str(), error.toString().c_str());
                throw;
            } catch (Throwable& t) {
                filelog("itemLoadError.txt", "[%s] %s", getItemClassName().c_str(), t.toString().c_str());
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to zone
//--------------------------------------------------------------------------------
void BeltLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (STORAGE_ZONE enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneBeltStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
                   "OptionType, Durability, EnchantLevel, ItemFlag FROM BeltObject "
                   "WHERE Storage = ? AND StorageID = ?");
        loadZoneBeltStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneBeltStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneBeltStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Belt* pBelt = new Belt();

            pBelt->setItemID(pResult->getInt(++i));
            pBelt->setObjectID(pResult->getInt(++i));
            pBelt->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pBelt->setOptionType(optionTypes);

            pBelt->setDurability(pResult->getInt(++i));
            pBelt->setEnchantLevel(pResult->getInt(++i));
            pBelt->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pBelt);
            } break;

            case STORAGE_STASH:
            case STORAGE_CORPSE:
                throw UnsupportedError("       .");

            default:
                throw Error("Storage must be STORAGE_ZONE");
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void BeltLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

// global variable definition
BeltLoader* g_pBeltLoader = NULL;
