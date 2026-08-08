//////////////////////////////////////////////////////////////////////////////
// Filename    : EventTree.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventTree.h"

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

EventTreeInfoManager* g_pEventTreeInfoManager = NULL;

ItemID_t EventTree::m_ItemIDRegistry = 0;
Mutex EventTree::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class EventTree member methods
//////////////////////////////////////////////////////////////////////////////

EventTree::EventTree()

{
    m_ItemType = 0;
}

EventTree::EventTree(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

{
    m_ItemType = itemType;
    m_Num = Num;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "EventTree::EventTree() : Invalid item type or option type");
        throw("EventTree::EventTree() : Invalid item type or optionType");
    }
}

void EventTree::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertEventTreeObjectStmt(
            pConn, "INSERT INTO EventTreeObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, "
                   "ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertEventTreeObjectStmt.bindLong(1, m_ItemID);
        insertEventTreeObjectStmt.bindLong(2, m_ObjectID);
        insertEventTreeObjectStmt.bindInt(3, m_ItemType);
        insertEventTreeObjectStmt.bindString(4, ownerID);
        insertEventTreeObjectStmt.bindInt(5, (int)storage);
        insertEventTreeObjectStmt.bindLong(6, storageID);
        insertEventTreeObjectStmt.bindInt(7, (int)x);
        insertEventTreeObjectStmt.bindInt(8, (int)y);
        insertEventTreeObjectStmt.bindInt(9, (int)m_Num);
        insertEventTreeObjectStmt.bindInt(10, (int)m_CreateType);
        insertEventTreeObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void EventTree::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveEventTreeObjectStmt(
            pConn, string("UPDATE EventTreeObject SET ") + field + " WHERE ItemID=?");
        tinysaveEventTreeObjectStmt.bindLong(1, m_ItemID);
        tinysaveEventTreeObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

void EventTree::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE EventTreeObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",Num = " <<(int)m_Num
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateEventTreeObjectStmt(
            pConn, "UPDATE EventTreeObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateEventTreeObjectStmt.bindLong(1, m_ObjectID);
        updateEventTreeObjectStmt.bindInt(2, m_ItemType);
        updateEventTreeObjectStmt.bindString(3, ownerID);
        updateEventTreeObjectStmt.bindInt(4, (int)storage);
        updateEventTreeObjectStmt.bindLong(5, storageID);
        updateEventTreeObjectStmt.bindInt(6, (int)x);
        updateEventTreeObjectStmt.bindInt(7, (int)y);
        updateEventTreeObjectStmt.bindInt(8, (int)m_Num);
        updateEventTreeObjectStmt.bindLong(9, m_ItemID);
        updateEventTreeObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}

string EventTree::toString() const

{
    StringStream msg;

    msg << "EventTree("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}

VolumeWidth_t EventTree::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pEventTreeInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t EventTree::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pEventTreeInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t EventTree::getWeight() const

{
    __BEGIN_TRY

    return g_pEventTreeInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventTreeInfo member methods
//////////////////////////////////////////////////////////////////////////////

string EventTreeInfo::toString() const

{
    StringStream msg;
    msg << "EventTreeInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void EventTreeInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectEventTreeInfoStmt(pConn, "SELECT MAX(ItemType) FROM EventTreeInfo");
        Result* pResult = selectEventTreeInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectEventTreeInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM EventTreeInfo");
        pResult = selectEventTreeInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            EventTreeInfo* pEventTreeInfo = new EventTreeInfo();

            pEventTreeInfo->setItemType(pResult->getInt(++i));
            pEventTreeInfo->setName(pResult->getString(++i));
            pEventTreeInfo->setEName(pResult->getString(++i));
            pEventTreeInfo->setPrice(pResult->getInt(++i));
            pEventTreeInfo->setVolumeType(pResult->getInt(++i));
            pEventTreeInfo->setWeight(pResult->getInt(++i));
            pEventTreeInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pEventTreeInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventTreeLoader member methods
//////////////////////////////////////////////////////////////////////////////

void EventTreeLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM EventTreeObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectEventTreeObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM EventTreeObject "
                   "WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectEventTreeObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectEventTreeObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                EventTree* pEventTree = new EventTree();

                pEventTree->setItemID(pResult->getDWORD(++i));
                pEventTree->setObjectID(pResult->getDWORD(++i));
                pEventTree->setItemType(pResult->getDWORD(++i));

                if (pEventTree->getItemType() > 12)
                    pEventTree->setQuestItem();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pEventTree->setNum(pResult->getBYTE(++i));
                pEventTree->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pEventTree)) {
                        pInventory->addItemEx(x, y, pEventTree);
                    } else {
                        processItemBugEx(pCreature, pEventTree);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pEventTree);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pEventTree);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pEventTree);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pEventTree);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pEventTree);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pEventTree);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pEventTree);
                    } else
                        pStash->insert(x, y, pEventTree);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pEventTree);
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

void EventTreeLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneEventTreeObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM EventTreeObject "
                   "WHERE Storage = ? AND StorageID = ?");
        loadZoneEventTreeObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneEventTreeObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneEventTreeObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            EventTree* pEventTree = new EventTree();

            pEventTree->setItemID(pResult->getInt(++i));
            pEventTree->setObjectID(pResult->getInt(++i));
            pEventTree->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pEventTree->setNum(pResult->getBYTE(++i));
            pEventTree->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pEventTree);
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

void EventTreeLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

EventTreeLoader* g_pEventTreeLoader = NULL;
