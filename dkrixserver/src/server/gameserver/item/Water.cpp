//////////////////////////////////////////////////////////////////////////////
// Filename    : Water.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Water.h"

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
WaterInfoManager* g_pWaterInfoManager = NULL;

ItemID_t Water::m_ItemIDRegistry = 0;
Mutex Water::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Water::Water()

    : m_ItemType(0) {}

Water::Water(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    m_ItemType = itemType;
    m_Num = 1;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Water::Water() : Invalid item type or option type");
        throw("Water::Water() : Invalid item type or optionType");
    }
}

//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Water::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertWaterStmt(pConn,
                                          "INSERT INTO WaterObject "
                                          "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                                          "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertWaterStmt.bindUInt(1, m_ItemID);
        insertWaterStmt.bindUInt(2, m_ObjectID);
        insertWaterStmt.bindUInt(3, m_ItemType);
        insertWaterStmt.bindString(4, ownerID);
        insertWaterStmt.bindInt(5, (int)storage);
        insertWaterStmt.bindUInt(6, storageID);
        insertWaterStmt.bindInt(7, (int)x);
        insertWaterStmt.bindInt(8, (int)y);
        insertWaterStmt.bindInt(9, (int)m_Num);
        insertWaterStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Water::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveWaterStmt(pConn, string("UPDATE WaterObject SET ") + field + " WHERE ItemID=?");
        tinysaveWaterStmt.bindUInt(1, m_ItemID);
        tinysaveWaterStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Water::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE WaterObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " << (int)storage
            << ",StorageID = " << storageID
            << ",X = " << (int)x
            << ",Y = " << (int)y
            << ",Num = " << (int)m_Num
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement saveWaterStmt(pConn, "UPDATE WaterObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                               "StorageID=?, X=?, Y=?, Num=? WHERE ItemID=?");
        saveWaterStmt.bindUInt(1, m_ObjectID);
        saveWaterStmt.bindUInt(2, m_ItemType);
        saveWaterStmt.bindString(3, ownerID);
        saveWaterStmt.bindInt(4, (int)storage);
        saveWaterStmt.bindUInt(5, storageID);
        saveWaterStmt.bindInt(6, (int)x);
        saveWaterStmt.bindInt(7, (int)y);
        saveWaterStmt.bindInt(8, (int)m_Num);
        saveWaterStmt.bindUInt(9, m_ItemID);
        saveWaterStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Water::toString() const

{
    StringStream msg;
    msg << "Water("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ",Num:" << (int)m_Num << ")";
    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Water::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pWaterInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Water::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pWaterInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Water::getWeight() const

{
    __BEGIN_TRY

    return g_pWaterInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string WaterInfo::toString() const

{
    StringStream msg;

    msg << "WaterInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void WaterInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM WaterInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectWaterInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM WaterInfo");
        pResult = selectWaterInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            WaterInfo* pWaterInfo = new WaterInfo();

            pWaterInfo->setItemType(pResult->getInt(++i));
            pWaterInfo->setName(pResult->getString(++i));
            pWaterInfo->setEName(pResult->getString(++i));
            pWaterInfo->setPrice(pResult->getInt(++i));
            pWaterInfo->setVolumeType(pResult->getInt(++i));
            pWaterInfo->setWeight(pResult->getInt(++i));
            pWaterInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pWaterInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void WaterLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM WaterObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectCreatureWaterStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM "
                   "WaterObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectCreatureWaterStmt.bindString(1, pCreature->getName());
        Result* pResult = selectCreatureWaterStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Water* pWater = new Water();

                pWater->setItemID(pResult->getDWORD(++i));
                pWater->setObjectID(pResult->getDWORD(++i));
                pWater->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pWater->setNum(pResult->getBYTE(++i));

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
                    if (pInventory->canAddingEx(x, y, pWater)) {
                        pInventory->addItemEx(x, y, pWater);
                    } else {
                        processItemBugEx(pCreature, pWater);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pWater);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pWater);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pWater);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pWater);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pWater);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pWater);
                    } else
                        pStash->insert(x, y, pWater);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pWater);
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
void WaterLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneWaterStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM WaterObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneWaterStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneWaterStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneWaterStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Water* pWater = new Water();

            pWater->setItemID(pResult->getInt(++i));
            pWater->setObjectID(pResult->getInt(++i));
            pWater->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pWater->setNum(pResult->getBYTE(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pWater);
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
void WaterLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

WaterLoader* g_pWaterLoader = NULL;
