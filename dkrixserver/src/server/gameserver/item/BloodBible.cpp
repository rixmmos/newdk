//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodBible.cpp
// Written By  : Changaya
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "BloodBible.h"

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
BloodBibleInfoManager* g_pBloodBibleInfoManager = NULL;

ItemID_t BloodBible::m_ItemIDRegistry = 0;
Mutex BloodBible::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
BloodBible::BloodBible()

    : m_ItemType(0), m_Durability(0) {
    m_EnchantLevel = 0;
}

BloodBible::BloodBible(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_Durability(0) {
    try {
        m_EnchantLevel = 0;

        m_Durability = computeMaxDurability(this);

        if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
            filelog("itembug.log", "BloodBible::BloodBible() : Invalid item type or option type");
            throw("BloodBible::BloodBible() : Invalid item type or optionType");
        }
    } catch (Throwable& t) {
        cout << t.toString().c_str() << endl;
        Assert(false);
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void BloodBible::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        sql << "INSERT INTO BloodBibleObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
            << " X, Y, Durability)"
            << " VALUES(" << m_ItemID << ", " << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', "
            << (int)storage << ", " << storageID << ", " << (int)x << ", " << (int)y << ", " << m_Durability << ")";
        // sql is retained only to reproduce the WarLog.txt audit line verbatim;
        // the query itself now executes via bound parameters below.

        PreparedStatement insertBloodBibleObjectStmt(
            pConn, "INSERT INTO BloodBibleObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "Durability) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertBloodBibleObjectStmt.bindLong(1, m_ItemID);
        insertBloodBibleObjectStmt.bindLong(2, m_ObjectID);
        insertBloodBibleObjectStmt.bindInt(3, m_ItemType);
        insertBloodBibleObjectStmt.bindString(4, ownerID);
        insertBloodBibleObjectStmt.bindInt(5, (int)storage);
        insertBloodBibleObjectStmt.bindLong(6, storageID);
        insertBloodBibleObjectStmt.bindInt(7, (int)x);
        insertBloodBibleObjectStmt.bindInt(8, (int)y);
        insertBloodBibleObjectStmt.bindInt(9, m_Durability);
        insertBloodBibleObjectStmt.execute();
        filelog("WarLog.txt", "%s", sql.toString().c_str());
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void BloodBible::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    char query[255];

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        sprintf(query, "UPDATE BloodBibleObject SET %s WHERE ItemID=%ld", field, m_ItemID);
        filelog("WarLog.txt", "%s", query);

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound. `query` above is retained
        // solely to preserve the WarLog.txt debug logging of the SQL text that
        // used to be executed verbatim; it is no longer the string that runs.
        PreparedStatement tinysaveBloodBibleObjectStmt(pConn, string("UPDATE BloodBibleObject SET ") + field +
                                                                  " WHERE ItemID=?");
        tinysaveBloodBibleObjectStmt.bindLong(1, m_ItemID);
        tinysaveBloodBibleObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void BloodBible::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE BloodBibleObject SET "
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

        PreparedStatement updateBloodBibleObjectStmt(
            pConn, "UPDATE BloodBibleObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Durability=?, EnchantLevel=? WHERE ItemID=?");
        updateBloodBibleObjectStmt.bindLong(1, m_ObjectID);
        updateBloodBibleObjectStmt.bindInt(2, m_ItemType);
        updateBloodBibleObjectStmt.bindString(3, ownerID);
        updateBloodBibleObjectStmt.bindInt(4, (int)storage);
        updateBloodBibleObjectStmt.bindLong(5, storageID);
        updateBloodBibleObjectStmt.bindInt(6, (int)x);
        updateBloodBibleObjectStmt.bindInt(7, (int)y);
        updateBloodBibleObjectStmt.bindInt(8, m_Durability);
        updateBloodBibleObjectStmt.bindInt(9, (int)getEnchantLevel());
        updateBloodBibleObjectStmt.bindLong(10, m_ItemID);
        updateBloodBibleObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string BloodBible::toString() const

{
    StringStream msg;

    msg << "BloodBible("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Durability:" << (int)m_Durability
        << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t BloodBible::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pBloodBibleInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t BloodBible::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pBloodBibleInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t BloodBible::getWeight() const

{
    __BEGIN_TRY

    return g_pBloodBibleInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t BloodBible::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pBloodBibleInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t BloodBible::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pBloodBibleInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string BloodBibleInfo::toString() const

{
    StringStream msg;

    msg << "BloodBibleInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void BloodBibleInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectBloodBibleInfoStmt(pConn, "SELECT MAX(ItemType) FROM BloodBibleInfo");
        Result* pResult = selectBloodBibleInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectBloodBibleInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, "
                   "ReqAbility, ItemLevel FROM BloodBibleInfo");
        pResult = selectBloodBibleInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            BloodBibleInfo* pBloodBibleInfo = new BloodBibleInfo();

            pBloodBibleInfo->setItemType(pResult->getInt(++i));
            pBloodBibleInfo->setName(pResult->getString(++i));
            pBloodBibleInfo->setEName(pResult->getString(++i));
            pBloodBibleInfo->setPrice(pResult->getInt(++i));
            pBloodBibleInfo->setVolumeType(pResult->getInt(++i));
            pBloodBibleInfo->setWeight(pResult->getInt(++i));
            pBloodBibleInfo->setRatio(pResult->getInt(++i));
            pBloodBibleInfo->setDurability(pResult->getInt(++i));
            pBloodBibleInfo->setDefenseBonus(pResult->getInt(++i));
            pBloodBibleInfo->setProtectionBonus(pResult->getInt(++i));
            pBloodBibleInfo->setReqAbility(pResult->getString(++i));
            pBloodBibleInfo->setItemLevel(pResult->getInt(++i));

            addItemInfo(pBloodBibleInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void BloodBibleLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM BloodBibleObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement deleteBloodBibleObjectStmt(pConn, "DELETE FROM BloodBibleObject WHERE OwnerID = ?");
        deleteBloodBibleObjectStmt.bindString(1, pCreature->getName());
        deleteBloodBibleObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to zone
//--------------------------------------------------------------------------------
void BloodBibleLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneBloodBibleObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Durability, EnchantLevel FROM "
                   "BloodBibleObject WHERE Storage = ? AND StorageID = ?");
        loadZoneBloodBibleObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneBloodBibleObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneBloodBibleObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            BloodBible* pBloodBible = new BloodBible();

            pBloodBible->setItemID(pResult->getInt(++i));
            pBloodBible->setObjectID(pResult->getInt(++i));
            pBloodBible->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pBloodBible->setDurability(pResult->getInt(++i));
            pBloodBible->setEnchantLevel(pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pBloodBible);
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
void BloodBibleLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

BloodBibleLoader* g_pBloodBibleLoader = NULL;
