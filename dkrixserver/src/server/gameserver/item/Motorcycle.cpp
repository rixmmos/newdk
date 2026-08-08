//////////////////////////////////////////////////////////////////////////////
// Filename    : Motorcycle.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Motorcycle.h"

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
MotorcycleInfoManager* g_pMotorcycleInfoManager = NULL;

ItemID_t Motorcycle::m_ItemIDRegistry = 0;
Mutex Motorcycle::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Motorcycle::Motorcycle()

    : m_ItemType(0), m_Durability(0), m_pInventory(NULL) {}

Motorcycle::Motorcycle(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_OptionType(optionType), m_Durability(0), m_pInventory(NULL) {
    __BEGIN_TRY


    switch (itemType) {
    case 0:
        m_pInventory = new Inventory(10, 6);
        break;
    case 1:
        m_pInventory = new Inventory(10, 6);
        break;
    case 2:
        m_pInventory = new Inventory(10, 6);
        break;
    case 3:
        m_pInventory = new Inventory(10, 6);
        break;
    case 4:
        m_pInventory = new Inventory(10, 6);
        break;
    case 5:
        m_pInventory = new Inventory(10, 6);
        break;
    default:
        m_pInventory = new Inventory(10, 6);
        break;
    }

    m_Durability = computeMaxDurability(this);

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Motorcycle::Motorcycle() : Invalid item type or option type");
        throw("Motorcycle::Motorcycle() : Invalid item type or optionType");
    }

    __END_CATCH
}

Motorcycle::~Motorcycle()

{
    SAFE_DELETE(m_pInventory);
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Motorcycle::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        setOptionTypeToField(m_OptionType, optionField);


        PreparedStatement insertMotorcycleObjectStmt(
            pConn, "INSERT INTO MotorcycleObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Durability) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertMotorcycleObjectStmt.bindLong(1, m_ItemID);
        insertMotorcycleObjectStmt.bindLong(2, m_ObjectID);
        insertMotorcycleObjectStmt.bindInt(3, m_ItemType);
        insertMotorcycleObjectStmt.bindString(4, ownerID);
        insertMotorcycleObjectStmt.bindInt(5, (int)storage);
        insertMotorcycleObjectStmt.bindLong(6, storageID);
        insertMotorcycleObjectStmt.bindInt(7, (int)x);
        insertMotorcycleObjectStmt.bindInt(8, (int)y);
        insertMotorcycleObjectStmt.bindString(9, optionField);
        insertMotorcycleObjectStmt.bindInt(10, m_Durability);
        insertMotorcycleObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Motorcycle::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveMotorcycleObjectStmt(
            pConn, string("UPDATE MotorcycleObject SET ") + field + " WHERE ItemID=?");
        tinysaveMotorcycleObjectStmt.bindLong(1, m_ItemID);
        tinysaveMotorcycleObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Motorcycle::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE MotorcycleObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",OptionType = " <<(int)m_OptionType
            << ",Durability = " << m_Durability
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(m_OptionType, optionField);

        PreparedStatement updateMotorcycleObjectStmt(
            pConn, "UPDATE MotorcycleObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=? WHERE ItemID=?");
        updateMotorcycleObjectStmt.bindLong(1, m_ObjectID);
        updateMotorcycleObjectStmt.bindInt(2, m_ItemType);
        updateMotorcycleObjectStmt.bindString(3, ownerID);
        updateMotorcycleObjectStmt.bindInt(4, (int)storage);
        updateMotorcycleObjectStmt.bindLong(5, storageID);
        updateMotorcycleObjectStmt.bindInt(6, (int)x);
        updateMotorcycleObjectStmt.bindInt(7, (int)y);
        updateMotorcycleObjectStmt.bindString(8, optionField);
        updateMotorcycleObjectStmt.bindInt(9, m_Durability);
        updateMotorcycleObjectStmt.bindLong(10, m_ItemID);
        updateMotorcycleObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Motorcycle::toString() const

{
    StringStream msg;

    msg << "Motorcycle("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType
        << ",OptionType:" << getOptionTypeToString(m_OptionType).c_str() << ",Durability:" << (int)m_Durability << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Motorcycle::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pMotorcycleInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Motorcycle::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pMotorcycleInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Motorcycle::getWeight() const

{
    __BEGIN_TRY

    return g_pMotorcycleInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string MotorcycleInfo::toString() const

{
    StringStream msg;

    msg << "MotorcycleInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void MotorcycleInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMotorcycleInfoStmt(pConn, "SELECT MAX(ItemType) FROM MotorcycleInfo");
        Result* pResult = selectMotorcycleInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMotorcycleInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability FROM MotorcycleInfo");
        pResult = selectMotorcycleInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MotorcycleInfo* pMotorcycleInfo = new MotorcycleInfo();

            pMotorcycleInfo->setItemType(pResult->getInt(++i));
            pMotorcycleInfo->setName(pResult->getString(++i));
            pMotorcycleInfo->setEName(pResult->getString(++i));
            pMotorcycleInfo->setPrice(pResult->getInt(++i));
            pMotorcycleInfo->setVolumeType(pResult->getInt(++i));
            pMotorcycleInfo->setWeight(pResult->getInt(++i));
            pMotorcycleInfo->setRatio(pResult->getInt(++i));
            pMotorcycleInfo->setDurability(pResult->getInt(++i));

            addItemInfo(pMotorcycleInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void MotorcycleLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability FROM
        MotorcycleObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectMotorcycleObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability FROM "
                   "MotorcycleObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectMotorcycleObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMotorcycleObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Motorcycle* pMotorcycle = new Motorcycle();

                pMotorcycle->setItemID(pResult->getDWORD(++i));
                pMotorcycle->setObjectID(pResult->getDWORD(++i));
                pMotorcycle->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pMotorcycle->setOptionType(optionTypes);

                pMotorcycle->setDurability(pResult->getInt(++i));



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
void MotorcycleLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneMotorcycleObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Durability FROM MotorcycleObject "
                   "WHERE Storage = ? AND StorageID = ?");
        loadZoneMotorcycleObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneMotorcycleObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneMotorcycleObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Motorcycle* pMotorcycle = new Motorcycle();

            pMotorcycle->setItemID(pResult->getInt(++i));
            pMotorcycle->setObjectID(pResult->getInt(++i));
            pMotorcycle->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pMotorcycle->setDurability(pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                //						Tile & pTile = pZone->getTile(x,y);
                //						Assert(!pTile.hasItem());
                pZone->addItem(pMotorcycle, x, y);
                //						pTile.addItem(pMotorcycle);
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
void MotorcycleLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

MotorcycleLoader* g_pMotorcycleLoader = NULL;
