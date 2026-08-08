//////////////////////////////////////////////////////////////////////////////
// Filename    : Coat.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Coat.h"

#include "Belt.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Vampire.h"

// global variable declaration
CoatInfoManager* g_pCoatInfoManager = NULL;

ItemID_t Coat::m_ItemIDRegistry = 0;
Mutex Coat::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Coat::Coat()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    //	m_EnchantLevel = 0;
}

Coat::Coat(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    //	m_EnchantLevel = 0;
    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Coat::Coat() : Invalid item type or option type");
        throw("Coat::Coat() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Coat::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);


        PreparedStatement insertCoatObjectStmt(
            pConn, "INSERT INTO CoatObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Durability, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?,  ?, ?)");
        insertCoatObjectStmt.bindLong(1, m_ItemID);
        insertCoatObjectStmt.bindLong(2, m_ObjectID);
        insertCoatObjectStmt.bindInt(3, getItemType());
        insertCoatObjectStmt.bindString(4, ownerID);
        insertCoatObjectStmt.bindInt(5, (int)storage);
        insertCoatObjectStmt.bindLong(6, storageID);
        insertCoatObjectStmt.bindInt(7, (int)x);
        insertCoatObjectStmt.bindInt(8, (int)y);
        insertCoatObjectStmt.bindString(9, optionField);
        insertCoatObjectStmt.bindInt(10, getDurability());
        insertCoatObjectStmt.bindInt(11, (int)getGrade());
        insertCoatObjectStmt.bindInt(12, (int)m_CreateType);
        insertCoatObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Coat::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveCoatObjectStmt(pConn, string("UPDATE CoatObject SET ") + field + " WHERE ItemID=?");
        tinysaveCoatObjectStmt.bindLong(1, m_ItemID);
        tinysaveCoatObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Coat::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE CoatObject SET "
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
        PreparedStatement updateCoatObjectStmt(
            pConn, "UPDATE CoatObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        updateCoatObjectStmt.bindLong(1, m_ObjectID);
        updateCoatObjectStmt.bindInt(2, getItemType());
        updateCoatObjectStmt.bindString(3, ownerID);
        updateCoatObjectStmt.bindInt(4, (int)storage);
        updateCoatObjectStmt.bindLong(5, storageID);
        updateCoatObjectStmt.bindInt(6, (int)x);
        updateCoatObjectStmt.bindInt(7, (int)y);
        updateCoatObjectStmt.bindString(8, optionField);
        updateCoatObjectStmt.bindInt(9, getDurability());
        updateCoatObjectStmt.bindInt(10, (int)getGrade());
        updateCoatObjectStmt.bindInt(11, (int)getEnchantLevel());
        updateCoatObjectStmt.bindLong(12, m_ItemID);
        updateCoatObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Coat::toString() const

{
    StringStream msg;

    msg << "Coat("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Coat::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pCoatInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Coat::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pCoatInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Coat::getWeight() const

{
    __BEGIN_TRY

    return g_pCoatInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t Coat::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pCoatInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t Coat::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pCoatInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string CoatInfo::toString() const

{
    StringStream msg;

    msg << "CoatInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void CoatInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectCoatInfoStmt(pConn, "SELECT MAX(ItemType) FROM CoatInfo");
        Result* pResult = selectCoatInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectCoatInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, "
                   "ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, "
                   "NextItemType, DowngradeRatio FROM CoatInfo");
        pResult = selectCoatInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            CoatInfo* pCoatInfo = new CoatInfo();

            pCoatInfo->setItemType(pResult->getInt(++i));
            pCoatInfo->setName(pResult->getString(++i));
            pCoatInfo->setEName(pResult->getString(++i));
            pCoatInfo->setPrice(pResult->getInt(++i));
            pCoatInfo->setVolumeType(pResult->getInt(++i));
            pCoatInfo->setWeight(pResult->getInt(++i));
            pCoatInfo->setRatio(pResult->getInt(++i));
            pCoatInfo->setDurability(pResult->getInt(++i));
            pCoatInfo->setDefenseBonus(pResult->getInt(++i));
            pCoatInfo->setProtectionBonus(pResult->getInt(++i));
            pCoatInfo->setReqAbility(pResult->getString(++i));
            pCoatInfo->setItemLevel(pResult->getInt(++i));
            pCoatInfo->setDefaultOptions(pResult->getString(++i));
            pCoatInfo->setUpgradeRatio(pResult->getInt(++i));
            pCoatInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pCoatInfo->setNextOptionRatio(pResult->getInt(++i));
            pCoatInfo->setNextItemType(pResult->getInt(++i));
            pCoatInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pCoatInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void CoatLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM CoatObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectCoatObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM CoatObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectCoatObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectCoatObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Coat* pCoat = new Coat();

                pCoat->setItemID(pResult->getDWORD(++i));
                pCoat->setObjectID(pResult->getDWORD(++i));
                pCoat->setItemType(pResult->getDWORD(++i));

                if (g_pCoatInfoManager->getItemInfo(pCoat->getItemType())->isUnique())
                    pCoat->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pCoat->setOptionType(optionTypes);

                pCoat->setDurability(pResult->getInt(++i));
                pCoat->setGrade(pResult->getInt(++i));
                pCoat->setEnchantLevel(pResult->getInt(++i));
                pCoat->setCreateType((Item::CreateType)pResult->getInt(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Motorcycle* pMotorcycle = NULL;
                Inventory* pMotorInventory = NULL;
                // Item*       pItem           = NULL;
                Stash* pStash = NULL;
                // Belt*       pBelt           = NULL;
                // Inventory*  pBeltInventory  = NULL;

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
                    if (pInventory->canAddingEx(x, y, pCoat)) {
                        pInventory->addItemEx(x, y, pCoat);
                    } else {
                        processItemBugEx(pCreature, pCoat);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pCoat);
                        } else {
                            processItemBugEx(pCreature, pCoat);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pCoat);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pCoat);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pCoat);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pCoat);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pCoat);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pCoat);
                    } else
                        pStash->insert(x, y, pCoat);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pCoat);
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
void CoatLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneCoatObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, "
                   "EnchantLevel, ItemFlag FROM CoatObject WHERE Storage = ? AND StorageID = ?");
        loadZoneCoatObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneCoatObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneCoatObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Coat* pCoat = new Coat();

            pCoat->setItemID(pResult->getInt(++i));
            pCoat->setObjectID(pResult->getInt(++i));
            pCoat->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pCoat->setOptionType(optionTypes);

            pCoat->setDurability(pResult->getInt(++i));
            pCoat->setEnchantLevel(pResult->getInt(++i));
            pCoat->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pCoat);
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
void CoatLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

CoatLoader* g_pCoatLoader = NULL;
