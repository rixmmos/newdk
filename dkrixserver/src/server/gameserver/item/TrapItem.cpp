//////////////////////////////////////////////////////////////////////////////
// Filename    : TrapItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "TrapItem.h"

#include "Belt.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "Ousters.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Utility.h"
#include "Vampire.h"

TrapItemInfoManager* g_pTrapItemInfoManager = NULL;

ItemID_t TrapItem::m_ItemIDRegistry = 0;
Mutex TrapItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class TrapItem member methods
//////////////////////////////////////////////////////////////////////////////

TrapItem::TrapItem()

{
    m_ItemType = 0;
}

TrapItem::TrapItem(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    m_ItemType = itemType;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "TrapItem::TrapItem() : Invalid item type or option type");
        throw("TrapItem::TrapItem() : Invalid item type or optionType");
    }
}

void TrapItem::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertTrapItemStmt(pConn, "INSERT INTO TrapItemObject "
                                                    "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, "
                                                    "ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertTrapItemStmt.bindUInt(1, m_ItemID);
        insertTrapItemStmt.bindUInt(2, m_ObjectID);
        insertTrapItemStmt.bindUInt(3, m_ItemType);
        insertTrapItemStmt.bindString(4, ownerID);
        insertTrapItemStmt.bindInt(5, (int)storage);
        insertTrapItemStmt.bindUInt(6, storageID);
        insertTrapItemStmt.bindInt(7, (int)x);
        insertTrapItemStmt.bindInt(8, (int)y);
        insertTrapItemStmt.bindInt(9, (int)m_CreateType);
        insertTrapItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void TrapItem::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveTrapItemStmt(pConn, string("UPDATE TrapItemObject SET ") + field + " WHERE ItemID=?");
        tinysaveTrapItemStmt.bindUInt(1, m_ItemID);
        tinysaveTrapItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void TrapItem::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE TrapItemObject SET "
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

        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveTrapItemStmt(pConn,
                                           "UPDATE TrapItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                           "StorageID=?, X=?, Y=? WHERE ItemID=?");
        saveTrapItemStmt.bindUInt(1, m_ObjectID);
        saveTrapItemStmt.bindUInt(2, m_ItemType);
        saveTrapItemStmt.bindString(3, ownerID);
        saveTrapItemStmt.bindInt(4, (int)storage);
        saveTrapItemStmt.bindUInt(5, storageID);
        saveTrapItemStmt.bindInt(6, (int)x);
        saveTrapItemStmt.bindInt(7, (int)y);
        saveTrapItemStmt.bindUInt(8, m_ItemID);
        saveTrapItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string TrapItem::toString() const

{
    StringStream msg;

    msg << "TrapItem("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ")";

    return msg.toString();
}

VolumeWidth_t TrapItem::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pTrapItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t TrapItem::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pTrapItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t TrapItem::getWeight() const

{
    __BEGIN_TRY

    return g_pTrapItemInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class TrapItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string TrapItemInfo::toString() const

{
    StringStream msg;
    msg << "TrapItemInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void TrapItemInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM TrapItemInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectTrapItemInfoStmt(
            pConn,
            "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, `Function`, Parameter FROM TrapItemInfo");
        pResult = selectTrapItemInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            TrapItemInfo* pTrapItemInfo = new TrapItemInfo();

            pTrapItemInfo->setItemType(pResult->getInt(++i));
            pTrapItemInfo->setName(pResult->getString(++i));
            pTrapItemInfo->setEName(pResult->getString(++i));
            pTrapItemInfo->setPrice(pResult->getInt(++i));
            pTrapItemInfo->setVolumeType(pResult->getInt(++i));
            pTrapItemInfo->setWeight(pResult->getInt(++i));
            pTrapItemInfo->setRatio(pResult->getInt(++i));
            pTrapItemInfo->setFunction(pResult->getInt(++i));
            pTrapItemInfo->setParameter(pResult->getInt(++i));

            addItemInfo(pTrapItemInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class TrapItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void TrapItemLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM TrapItemObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectTrapItemLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag FROM "
                   "TrapItemObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectTrapItemLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectTrapItemLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                TrapItem* pTrapItem = new TrapItem();

                pTrapItem->setItemID(pResult->getDWORD(++i));
                pTrapItem->setObjectID(pResult->getDWORD(++i));
                pTrapItem->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pTrapItem->setCreateType((Item::CreateType)pResult->getInt(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Ousters* pOusters = NULL;
                Motorcycle* pMotorcycle = NULL;
                Inventory* pMotorInventory = NULL;
                Item* pItem = NULL;
                Stash* pStash = NULL;
                Belt* pBelt = NULL;
                Inventory* pBeltInventory = NULL;

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
                } else if (pCreature->isOusters()) {
                    pOusters = dynamic_cast<Ousters*>(pCreature);
                    pInventory = pOusters->getInventory();
                    pStash = pOusters->getStash();
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pTrapItem)) {
                        pInventory->addItemEx(x, y, pTrapItem);
                    } else {
                        processItemBugEx(pCreature, pTrapItem);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pTrapItem);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pTrapItem);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pTrapItem);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pTrapItem);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pTrapItem);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pTrapItem);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pTrapItem);
                    } else
                        pStash->insert(x, y, pTrapItem);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pTrapItem);
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

void TrapItemLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneTrapItemStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag FROM TrapItemObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneTrapItemStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneTrapItemStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneTrapItemStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            TrapItem* pTrapItem = new TrapItem();

            pTrapItem->setItemID(pResult->getInt(++i));
            pTrapItem->setObjectID(pResult->getInt(++i));
            pTrapItem->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pTrapItem->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pTrapItem);
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

void TrapItemLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

TrapItemLoader* g_pTrapItemLoader = NULL;
