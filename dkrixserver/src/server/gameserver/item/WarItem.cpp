//////////////////////////////////////////////////////////////////////////////
// Filename    : WarItem.cpp
// Written By  : Changaya
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "WarItem.h"

#include <stdio.h>

#include "DB.h"
#include "ItemInfoManager.h"
#include "PreparedStatement.h"

// global variable declaration
WarItemInfoManager* g_pWarItemInfoManager = NULL;

ItemID_t WarItem::m_ItemIDRegistry = 0;
Mutex WarItem::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
WarItem::WarItem()

    : m_ItemType(0) {}

WarItem::WarItem(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType) {
    try {
        if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
            filelog("itembug.log", "WarItem::WarItem() : Invalid item type or option type");
            throw("WarItem::WarItem() : Invalid item type or optionType");
        }
    } catch (Throwable& t) {
        cout << t.toString().c_str() << endl;
        Assert(false);
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void WarItem::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        sql << "INSERT INTO WarItemObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
            << " X, Y)"
            << " VALUES(" << m_ItemID << ", " << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', "
            << (int)storage << ", " << storageID << ", " << (int)x << ", " << (int)y << ")";
        // sql is retained only to reproduce the WarLog.txt audit line verbatim;
        // the query itself now executes via bound parameters below.

        PreparedStatement insertWarItemStmt(pConn, "INSERT INTO WarItemObject "
                                                   "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                   " X, Y)"
                                                   " VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
        insertWarItemStmt.bindUInt(1, m_ItemID);
        insertWarItemStmt.bindUInt(2, m_ObjectID);
        insertWarItemStmt.bindUInt(3, m_ItemType);
        insertWarItemStmt.bindString(4, ownerID);
        insertWarItemStmt.bindInt(5, (int)storage);
        insertWarItemStmt.bindUInt(6, storageID);
        insertWarItemStmt.bindInt(7, (int)x);
        insertWarItemStmt.bindInt(8, (int)y);
        insertWarItemStmt.execute();
        filelog("WarLog.txt", "%s", sql.toString().c_str());
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void WarItem::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    char query[255];

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        sprintf(query, "UPDATE WarItemObject SET %s WHERE ItemID=%ld", field, m_ItemID);
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound. query[]
        // is retained only to reproduce the WarLog.txt audit line verbatim.
        PreparedStatement tinysaveWarItemStmt(pConn, string("UPDATE WarItemObject SET ") + field + " WHERE ItemID=?");
        tinysaveWarItemStmt.bindUInt(1, m_ItemID);
        tinysaveWarItemStmt.execute();
        filelog("WarLog.txt", "%s", query);
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void WarItem::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveWarItemStmt(pConn,
                                          "UPDATE WarItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                          "StorageID=?, X=?, Y=? WHERE ItemID=?");
        saveWarItemStmt.bindUInt(1, m_ObjectID);
        saveWarItemStmt.bindUInt(2, m_ItemType);
        saveWarItemStmt.bindString(3, ownerID);
        saveWarItemStmt.bindInt(4, (int)storage);
        saveWarItemStmt.bindUInt(5, storageID);
        saveWarItemStmt.bindInt(6, (int)x);
        saveWarItemStmt.bindInt(7, (int)y);
        saveWarItemStmt.bindUInt(8, m_ItemID);
        saveWarItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string WarItem::toString() const

{
    StringStream msg;

    msg << "WarItem("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t WarItem::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pWarItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t WarItem::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pWarItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t WarItem::getWeight() const

{
    __BEGIN_TRY

    return g_pWarItemInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string WarItemInfo::toString() const

{
    StringStream msg;

    msg << "WarItemInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void WarItemInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM WarItemInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectWarItemInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM WarItemInfo");
        pResult = selectWarItemInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            WarItemInfo* pWarItemInfo = new WarItemInfo();

            pWarItemInfo->setItemType(pResult->getInt(++i));
            pWarItemInfo->setName(pResult->getString(++i));
            pWarItemInfo->setEName(pResult->getString(++i));
            pWarItemInfo->setPrice(pResult->getInt(++i));
            pWarItemInfo->setVolumeType(pResult->getInt(++i));
            pWarItemInfo->setWeight(pResult->getInt(++i));
            pWarItemInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pWarItemInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void WarItemLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to zone
//--------------------------------------------------------------------------------
void WarItemLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void WarItemLoader::load(StorageID_t storageID, Inventory* pInventory)

    {__BEGIN_TRY

         __END_CATCH}

WarItemLoader* g_pWarItemLoader = NULL;
