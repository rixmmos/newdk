//////////////////////////////////////////////////////////////////////////////
// Filename    : Sweeper.cpp
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Sweeper.h"

#include <stdio.h>

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
SweeperInfoManager* g_pSweeperInfoManager = NULL;

ItemID_t Sweeper::m_ItemIDRegistry = 0;
Mutex Sweeper::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Sweeper::Sweeper()

    : m_ItemType(0), m_Durability(0) {
    m_EnchantLevel = 0;
}

Sweeper::Sweeper(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_Durability(0) {
    try {
        m_EnchantLevel = 0;

        m_Durability = computeMaxDurability(this);

        if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
            filelog("itembug.log", "Sweeper::Sweeper() : Invalid item type or option type");
            throw("Sweeper::Sweeper() : Invalid item type or optionType");
        }
    } catch (Throwable& t) {
        cout << t.toString().c_str() << endl;
        Assert(false);
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Sweeper::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        StringStream sql;

        sql << "INSERT INTO SweeperObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
            << " X, Y, Durability)"
            << " VALUES(" << m_ItemID << ", " << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', "
            << (int)storage << ", " << storageID << ", " << (int)x << ", " << (int)y << ", " << m_Durability << ")";
        // sql is retained only to reproduce the WarLog.txt audit line verbatim;
        // the query itself now executes via bound parameters below.

        PreparedStatement insertSweeperStmt(pConn,
                                             "INSERT INTO SweeperObject "
                                             "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                             " X, Y, Durability)"
                                             " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertSweeperStmt.bindUInt(1, m_ItemID);
        insertSweeperStmt.bindUInt(2, m_ObjectID);
        insertSweeperStmt.bindUInt(3, m_ItemType);
        insertSweeperStmt.bindString(4, ownerID);
        insertSweeperStmt.bindInt(5, (int)storage);
        insertSweeperStmt.bindUInt(6, storageID);
        insertSweeperStmt.bindInt(7, (int)x);
        insertSweeperStmt.bindInt(8, (int)y);
        insertSweeperStmt.bindUInt(9, m_Durability);
        insertSweeperStmt.execute();
        filelog("WarLog.txt", "%s", sql.toString().c_str());
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Sweeper::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    char query[255];

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        sprintf(query, "UPDATE SweeperObject SET %s WHERE ItemID=%ld", field, m_ItemID);
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound. query[]
        // is retained only to reproduce the WarLog.txt audit line verbatim.
        PreparedStatement tinysaveSweeperStmt(pConn, string("UPDATE SweeperObject SET ") + field + " WHERE ItemID=?");
        tinysaveSweeperStmt.bindUInt(1, m_ItemID);
        tinysaveSweeperStmt.execute();
        filelog("WarLog.txt", "%s", query);
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Sweeper::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE SweeperObject SET "
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

        PreparedStatement saveSweeperStmt(pConn,
                                           "UPDATE SweeperObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                           "StorageID=?, X=?, Y=?, Durability=?, EnchantLevel=? WHERE ItemID=?");
        saveSweeperStmt.bindUInt(1, m_ObjectID);
        saveSweeperStmt.bindUInt(2, m_ItemType);
        saveSweeperStmt.bindString(3, ownerID);
        saveSweeperStmt.bindInt(4, (int)storage);
        saveSweeperStmt.bindUInt(5, storageID);
        saveSweeperStmt.bindInt(6, (int)x);
        saveSweeperStmt.bindInt(7, (int)y);
        saveSweeperStmt.bindUInt(8, m_Durability);
        saveSweeperStmt.bindInt(9, (int)m_EnchantLevel);
        saveSweeperStmt.bindUInt(10, m_ItemID);
        saveSweeperStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Sweeper::toString() const

{
    StringStream msg;

    msg << "Sweeper("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Durability:" << (int)m_Durability
        << ",EnchantLevel:" << (int)m_EnchantLevel << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Sweeper::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pSweeperInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Sweeper::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pSweeperInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Sweeper::getWeight() const

{
    __BEGIN_TRY

    return g_pSweeperInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t Sweeper::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pSweeperInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t Sweeper::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pSweeperInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string SweeperInfo::toString() const

{
    StringStream msg;

    msg << "SweeperInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void SweeperInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM SweeperInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectSweeperInfoStmt(pConn,
                                                 "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, "
                                                 "Defense, Protection, ReqAbility, ItemLevel FROM SweeperInfo");
        pResult = selectSweeperInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            SweeperInfo* pSweeperInfo = new SweeperInfo();

            pSweeperInfo->setItemType(pResult->getInt(++i));
            pSweeperInfo->setName(pResult->getString(++i));
            pSweeperInfo->setEName(pResult->getString(++i));
            pSweeperInfo->setPrice(pResult->getInt(++i));
            pSweeperInfo->setVolumeType(pResult->getInt(++i));
            pSweeperInfo->setWeight(pResult->getInt(++i));
            pSweeperInfo->setRatio(pResult->getInt(++i));
            pSweeperInfo->setDurability(pResult->getInt(++i));
            pSweeperInfo->setDefenseBonus(pResult->getInt(++i));
            pSweeperInfo->setProtectionBonus(pResult->getInt(++i));
            pSweeperInfo->setReqAbility(pResult->getString(++i));
            pSweeperInfo->setItemLevel(pResult->getInt(++i));

            addItemInfo(pSweeperInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void SweeperLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM SweeperObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement deleteSweeperObjectStmt(pConn, "DELETE FROM SweeperObject WHERE OwnerID = ?");
        deleteSweeperObjectStmt.bindString(1, pCreature->getName());
        deleteSweeperObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to zone
//--------------------------------------------------------------------------------
void SweeperLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneSweeperStmt(pConn,
                                                 "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                                                 " Durability, EnchantLevel FROM SweeperObject"
                                                 " WHERE Storage = ? AND StorageID = ?");
        selectZoneSweeperStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneSweeperStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneSweeperStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Sweeper* pSweeper = new Sweeper();

            pSweeper->setItemID(pResult->getInt(++i));
            pSweeper->setObjectID(pResult->getInt(++i));
            pSweeper->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pSweeper->setDurability(pResult->getInt(++i));
            pSweeper->setEnchantLevel(pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pSweeper);
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
void SweeperLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

SweeperLoader* g_pSweeperLoader = NULL;
