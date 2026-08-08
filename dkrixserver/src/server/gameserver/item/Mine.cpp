//////////////////////////////////////////////////////////////////////////////
// Filename    : Mine.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Mine.h"

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
MineInfoManager* g_pMineInfoManager = NULL;

ItemID_t Mine::m_ItemIDRegistry = 0;
Mutex Mine::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Mine::Mine()

    : m_ItemType(0), m_Damage(0), m_Dir(0) {
    m_Num = 1;
    m_InstallerName = "";
    m_InstallerPartyID = 0;
}

Mine::Mine(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType) {
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Mine::Mine() : Invalid item type or option type");
        throw("Mine::Mine() : Invalid item type or optionType");
    }
    m_Num = 1;
    m_Damage = 0;
    m_Dir = 0;
    m_InstallerName = "";
    m_InstallerPartyID = 0;
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Mine::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertMineObjectStmt(
            pConn, "INSERT INTO MineObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?,?)");
        insertMineObjectStmt.bindLong(1, m_ItemID);
        insertMineObjectStmt.bindLong(2, m_ObjectID);
        insertMineObjectStmt.bindInt(3, m_ItemType);
        insertMineObjectStmt.bindString(4, ownerID);
        insertMineObjectStmt.bindInt(5, (int)storage);
        insertMineObjectStmt.bindLong(6, storageID);
        insertMineObjectStmt.bindInt(7, (int)x);
        insertMineObjectStmt.bindInt(8, (int)y);
        insertMineObjectStmt.bindInt(9, (int)m_Num);
        insertMineObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Mine::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveMineObjectStmt(pConn, string("UPDATE MineObject SET ") + field + " WHERE ItemID=?");
        tinysaveMineObjectStmt.bindLong(1, m_ItemID);
        tinysaveMineObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Mine::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE MineObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",Num = " << (int)m_Num
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */
        PreparedStatement updateMineObjectStmt(
            pConn, "UPDATE MineObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, Num=? "
                   "WHERE ItemID=?");
        updateMineObjectStmt.bindLong(1, m_ObjectID);
        updateMineObjectStmt.bindInt(2, m_ItemType);
        updateMineObjectStmt.bindString(3, ownerID);
        updateMineObjectStmt.bindInt(4, (int)storage);
        updateMineObjectStmt.bindLong(5, storageID);
        updateMineObjectStmt.bindInt(6, (int)x);
        updateMineObjectStmt.bindInt(7, (int)y);
        updateMineObjectStmt.bindInt(8, (int)m_Num);
        updateMineObjectStmt.bindLong(9, m_ItemID);
        updateMineObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Mine::toString() const

{
    StringStream msg;

    msg << "Mine("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Mine::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pMineInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Mine::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pMineInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Mine::getWeight() const

{
    __BEGIN_TRY

    return g_pMineInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

Damage_t Mine::getMinDamage() const

{
    __BEGIN_TRY

    return g_pMineInfoManager->getItemInfo(m_ItemType)->getMinDamage();

    __END_CATCH
}

Damage_t Mine::getMaxDamage() const

{
    __BEGIN_TRY

    return g_pMineInfoManager->getItemInfo(m_ItemType)->getMaxDamage();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string MineInfo::toString() const

{
    StringStream msg;

    msg << "MineInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void MineInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMineInfoStmt(pConn, "SELECT MAX(ItemType) FROM MineInfo");
        Result* pResult = selectMineInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMineInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, minDamage, maxDamage FROM MineInfo");
        pResult = selectMineInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MineInfo* pMineInfo = new MineInfo();

            pMineInfo->setItemType(pResult->getInt(++i));
            pMineInfo->setName(pResult->getString(++i));
            pMineInfo->setEName(pResult->getString(++i));
            pMineInfo->setPrice(pResult->getInt(++i));
            pMineInfo->setVolumeType(pResult->getInt(++i));
            pMineInfo->setWeight(pResult->getInt(++i));
            pMineInfo->setRatio(pResult->getInt(++i));
            pMineInfo->setMinDamage(pResult->getInt(++i));
            pMineInfo->setMaxDamage(pResult->getInt(++i));

            addItemInfo(pMineInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void MineLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM MineObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectMineObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM MineObject WHERE OwnerID = "
                   "? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectMineObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMineObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Mine* pMine = new Mine();

                pMine->setItemID(pResult->getDWORD(++i));
                pMine->setObjectID(pResult->getDWORD(++i));
                pMine->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pMine->setNum(pResult->getBYTE(++i));

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
                    if (pInventory->canAddingEx(x, y, pMine)) {
                        pInventory->addItemEx(x, y, pMine);
                    } else {
                        processItemBugEx(pCreature, pMine);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pMine);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pMine);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pMine);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pMine);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pMine);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pMine);
                    } else
                        pStash->insert(x, y, pMine);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pMine);
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
void MineLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneMineObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM MineObject WHERE Storage = ? AND "
                   "StorageID = ?");
        loadZoneMineObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneMineObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneMineObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Mine* pMine = new Mine();

            pMine->setItemID(pResult->getInt(++i));
            pMine->setObjectID(pResult->getInt(++i));
            pMine->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pMine);
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
void MineLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

MineLoader* g_pMineLoader = NULL;
