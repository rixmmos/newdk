//////////////////////////////////////////////////////////////////////////////
// Filename    : CastleSymbol.cpp
// Written By  : Changaya
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CastleSymbol.h"

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
CastleSymbolInfoManager* g_pCastleSymbolInfoManager = NULL;

ItemID_t CastleSymbol::m_ItemIDRegistry = 0;
Mutex CastleSymbol::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
CastleSymbol::CastleSymbol()

    : m_ItemType(0), m_Durability(0) {
    m_EnchantLevel = 0;
}

CastleSymbol::CastleSymbol(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_Durability(0) {
    try {
        m_EnchantLevel = 0;

        m_Durability = computeMaxDurability(this);

        if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
            filelog("itembug.log", "CastleSymbol::CastleSymbol() : Invalid item type or option type");
            throw("CastleSymbol::CastleSymbol() : Invalid item type or optionType");
        }
    } catch (Throwable& t) {
        cout << t.toString().c_str() << endl;
        Assert(false);
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void CastleSymbol::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
                          ItemID_t itemID)

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
        PreparedStatement insertCastleSymbolObjectStmt(
            pConn, "INSERT INTO CastleSymbolObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "Durability ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertCastleSymbolObjectStmt.bindLong(1, m_ItemID);
        insertCastleSymbolObjectStmt.bindLong(2, m_ObjectID);
        insertCastleSymbolObjectStmt.bindInt(3, m_ItemType);
        insertCastleSymbolObjectStmt.bindString(4, ownerID);
        insertCastleSymbolObjectStmt.bindInt(5, (int)storage);
        insertCastleSymbolObjectStmt.bindLong(6, storageID);
        insertCastleSymbolObjectStmt.bindInt(7, (int)x);
        insertCastleSymbolObjectStmt.bindInt(8, (int)y);
        insertCastleSymbolObjectStmt.bindInt(9, m_Durability);
        insertCastleSymbolObjectStmt.execute();
        filelog("WarLog.txt", "%s", sql.toString().c_str());
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void CastleSymbol::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    char query[255];

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        sprintf(query, "UPDATE CastleSymbolObject SET %s WHERE ItemID=%ld", field, m_ItemID);
        filelog("WarLog.txt", "%s", query);

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound. `query` above is retained
        // solely to preserve the WarLog.txt debug logging of the SQL text that
        // used to be executed verbatim; it is no longer the string that runs.
        PreparedStatement tinysaveCastleSymbolObjectStmt(pConn, string("UPDATE CastleSymbolObject SET ") + field +
                                                                    " WHERE ItemID=?");
        tinysaveCastleSymbolObjectStmt.bindLong(1, m_ItemID);
        tinysaveCastleSymbolObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void CastleSymbol::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE CastleSymbolObject SET "
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

        PreparedStatement updateCastleSymbolObjectStmt(
            pConn, "UPDATE CastleSymbolObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, Durability=?, EnchantLevel=? WHERE ItemID=?");
        updateCastleSymbolObjectStmt.bindLong(1, m_ObjectID);
        updateCastleSymbolObjectStmt.bindInt(2, m_ItemType);
        updateCastleSymbolObjectStmt.bindString(3, ownerID);
        updateCastleSymbolObjectStmt.bindInt(4, (int)storage);
        updateCastleSymbolObjectStmt.bindLong(5, storageID);
        updateCastleSymbolObjectStmt.bindInt(6, (int)x);
        updateCastleSymbolObjectStmt.bindInt(7, (int)y);
        updateCastleSymbolObjectStmt.bindInt(8, m_Durability);
        updateCastleSymbolObjectStmt.bindInt(9, (int)m_EnchantLevel);
        updateCastleSymbolObjectStmt.bindLong(10, m_ItemID);
        updateCastleSymbolObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string CastleSymbol::toString() const

{
    StringStream msg;

    msg << "CastleSymbol("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Durability:" << (int)m_Durability
        << ",EnchantLevel:" << (int)m_EnchantLevel << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t CastleSymbol::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pCastleSymbolInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t CastleSymbol::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pCastleSymbolInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t CastleSymbol::getWeight() const

{
    __BEGIN_TRY

    return g_pCastleSymbolInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t CastleSymbol::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pCastleSymbolInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t CastleSymbol::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pCastleSymbolInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string CastleSymbolInfo::toString() const

{
    StringStream msg;

    msg << "CastleSymbolInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void CastleSymbolInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectCastleSymbolInfoStmt(pConn, "SELECT MAX(ItemType) FROM CastleSymbolInfo");
        Result* pResult = selectCastleSymbolInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectCastleSymbolInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, "
                   "ReqAbility, ItemLevel FROM CastleSymbolInfo");
        pResult = selectCastleSymbolInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            CastleSymbolInfo* pCastleSymbolInfo = new CastleSymbolInfo();

            pCastleSymbolInfo->setItemType(pResult->getInt(++i));
            pCastleSymbolInfo->setName(pResult->getString(++i));
            pCastleSymbolInfo->setEName(pResult->getString(++i));
            pCastleSymbolInfo->setPrice(pResult->getInt(++i));
            pCastleSymbolInfo->setVolumeType(pResult->getInt(++i));
            pCastleSymbolInfo->setWeight(pResult->getInt(++i));
            pCastleSymbolInfo->setRatio(pResult->getInt(++i));
            pCastleSymbolInfo->setDurability(pResult->getInt(++i));
            pCastleSymbolInfo->setDefenseBonus(pResult->getInt(++i));
            pCastleSymbolInfo->setProtectionBonus(pResult->getInt(++i));
            pCastleSymbolInfo->setReqAbility(pResult->getString(++i));
            pCastleSymbolInfo->setItemLevel(pResult->getInt(++i));

            addItemInfo(pCastleSymbolInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void CastleSymbolLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM CastleSymbolObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement deleteCastleSymbolObjectStmt(pConn, "DELETE FROM CastleSymbolObject WHERE OwnerID = ?");
        deleteCastleSymbolObjectStmt.bindString(1, pCreature->getName());
        deleteCastleSymbolObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to zone
//--------------------------------------------------------------------------------
void CastleSymbolLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneCastleSymbolObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Durability, EnchantLevel FROM "
                   "CastleSymbolObject WHERE Storage = ? AND StorageID = ?");
        loadZoneCastleSymbolObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneCastleSymbolObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneCastleSymbolObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            CastleSymbol* pCastleSymbol = new CastleSymbol();

            pCastleSymbol->setItemID(pResult->getInt(++i));
            pCastleSymbol->setObjectID(pResult->getInt(++i));
            pCastleSymbol->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pCastleSymbol->setDurability(pResult->getInt(++i));
            pCastleSymbol->setEnchantLevel(pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pCastleSymbol);
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
void CastleSymbolLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

CastleSymbolLoader* g_pCastleSymbolLoader = NULL;
