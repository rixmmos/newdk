//////////////////////////////////////////////////////////////////////////////
// Filename    : ETC.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

// include files
#include "ETC.h"

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
ETCInfoManager* g_pETCInfoManager = NULL;

ItemID_t ETC::m_ItemIDRegistry = 0;
Mutex ETC::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ETC::ETC()

    : m_ItemType(0) {}

ETC::ETC(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t num)

    : m_ItemType(itemType), m_Num(num) {
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "ETC::ETC() : Invalid item type or option type");
        throw("ETC::ETC() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void ETC::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertETCObjectStmt(
            pConn, "INSERT INTO ETCObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?,?)");
        insertETCObjectStmt.bindLong(1, m_ItemID);
        insertETCObjectStmt.bindLong(2, m_ObjectID);
        insertETCObjectStmt.bindInt(3, m_ItemType);
        insertETCObjectStmt.bindString(4, ownerID);
        insertETCObjectStmt.bindInt(5, (int)storage);
        insertETCObjectStmt.bindLong(6, storageID);
        insertETCObjectStmt.bindInt(7, (int)x);
        insertETCObjectStmt.bindInt(8, (int)y);
        insertETCObjectStmt.bindInt(9, (int)m_Num);
        insertETCObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void ETC::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveETCObjectStmt(pConn, string("UPDATE ETCObject SET ") + field + " WHERE ItemID=?");
        tinysaveETCObjectStmt.bindLong(1, m_ItemID);
        tinysaveETCObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void ETC::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE ETCObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement updateETCObjectStmt(
            pConn, "UPDATE ETCObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, Num=?  "
                   "WHERE ItemID=?");
        updateETCObjectStmt.bindLong(1, m_ObjectID);
        updateETCObjectStmt.bindInt(2, m_ItemType);
        updateETCObjectStmt.bindString(3, ownerID);
        updateETCObjectStmt.bindInt(4, (int)storage);
        updateETCObjectStmt.bindLong(5, storageID);
        updateETCObjectStmt.bindInt(6, (int)x);
        updateETCObjectStmt.bindInt(7, (int)y);
        updateETCObjectStmt.bindInt(8, (int)m_Num);
        updateETCObjectStmt.bindLong(9, m_ItemID);
        updateETCObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ETC::toString() const

{
    StringStream msg;

    msg << "ETC("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t ETC::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pETCInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t ETC::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pETCInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t ETC::getWeight() const

{
    __BEGIN_TRY

    return g_pETCInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ETCInfo::toString() const

{
    StringStream msg;

    msg << "ETCInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void ETCInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectETCInfoStmt(pConn, "SELECT MAX(ItemType) FROM ETCInfo");
        Result* pResult = selectETCInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectETCInfoStmt2(pConn,
                                             "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM ETCInfo");
        pResult = selectETCInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            ETCInfo* pETCInfo = new ETCInfo();

            pETCInfo->setItemType(pResult->getInt(++i));
            pETCInfo->setName(pResult->getString(++i));
            pETCInfo->setEName(pResult->getString(++i));
            pETCInfo->setPrice(pResult->getInt(++i));
            pETCInfo->setVolumeType(pResult->getInt(++i));
            pETCInfo->setWeight(pResult->getInt(++i));
            pETCInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pETCInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void ETCLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM ETCObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectETCObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM ETCObject WHERE OwnerID = ? "
                   "AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectETCObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectETCObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                ETC* pETC = new ETC();

                pETC->setItemID(pResult->getDWORD(++i));
                pETC->setObjectID(pResult->getDWORD(++i));
                pETC->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pETC->setNum(pResult->getBYTE(++i));

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
                    if (pInventory->canAddingEx(x, y, pETC)) {
                        pInventory->addItemEx(x, y, pETC);
                    } else {
                        processItemBugEx(pCreature, pETC);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pETC);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pETC);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pETC);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pETC);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pETC);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pETC);
                    } else
                        pStash->insert(x, y, pETC);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pETC);
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
void ETCLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneETCObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM ETCObject WHERE Storage = ? "
                   "AND StorageID = ?");
        loadZoneETCObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneETCObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneETCObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            ETC* pETC = new ETC();

            pETC->setItemID(pResult->getInt(++i));
            pETC->setObjectID(pResult->getInt(++i));
            pETC->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pETC->setNum(pResult->getBYTE(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pETC);
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
void ETCLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

ETCLoader* g_pETCLoader = NULL;
