//////////////////////////////////////////////////////////////////////////////
// Filename    : EventBall.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventBall.h"

#include "Belt.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Utility.h"
#include "Vampire.h"

EventBallInfoManager* g_pEventBallInfoManager = NULL;

ItemID_t EventBall::m_ItemIDRegistry = 0;
Mutex EventBall::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class EventBall member methods
//////////////////////////////////////////////////////////////////////////////

EventBall::EventBall()

{
    m_ItemType = 0;
}

EventBall::EventBall(ItemType_t itemType, OptionType_t optionType, ItemNum_t Num)

{
    m_ItemType = itemType;
    m_Num = Num;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "EventBall::EventBall() : Invalid item type or option type");
        throw("EventBall::EventBall() : Invalid item type or optionType");
    }
}

void EventBall::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertEventBallObjectStmt(
            pConn, "INSERT INTO EventBallObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertEventBallObjectStmt.bindLong(1, m_ItemID);
        insertEventBallObjectStmt.bindLong(2, m_ObjectID);
        insertEventBallObjectStmt.bindInt(3, m_ItemType);
        insertEventBallObjectStmt.bindString(4, ownerID);
        insertEventBallObjectStmt.bindInt(5, (int)storage);
        insertEventBallObjectStmt.bindLong(6, storageID);
        insertEventBallObjectStmt.bindInt(7, (int)x);
        insertEventBallObjectStmt.bindInt(8, (int)y);
        insertEventBallObjectStmt.bindInt(9, (int)m_Num);
        insertEventBallObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void EventBall::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveEventBallObjectStmt(
            pConn, string("UPDATE EventBallObject SET ") + field + " WHERE ItemID=?");
        tinysaveEventBallObjectStmt.bindLong(1, m_ItemID);
        tinysaveEventBallObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

void EventBall::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE EventBallObject SET "
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

        PreparedStatement updateEventBallObjectStmt(
            pConn, "UPDATE EventBallObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateEventBallObjectStmt.bindLong(1, m_ObjectID);
        updateEventBallObjectStmt.bindInt(2, m_ItemType);
        updateEventBallObjectStmt.bindString(3, ownerID);
        updateEventBallObjectStmt.bindInt(4, (int)storage);
        updateEventBallObjectStmt.bindLong(5, storageID);
        updateEventBallObjectStmt.bindInt(6, (int)x);
        updateEventBallObjectStmt.bindInt(7, (int)y);
        updateEventBallObjectStmt.bindInt(8, (int)m_Num);
        updateEventBallObjectStmt.bindLong(9, m_ItemID);
        updateEventBallObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}

string EventBall::toString() const

{
    StringStream msg;

    msg << "EventBall("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}

VolumeWidth_t EventBall::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pEventBallInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t EventBall::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pEventBallInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t EventBall::getWeight() const

{
    __BEGIN_TRY

    return g_pEventBallInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventBallInfo member methods
//////////////////////////////////////////////////////////////////////////////

string EventBallInfo::toString() const

{
    StringStream msg;
    msg << "EventBallInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void EventBallInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectEventBallInfoStmt(pConn, "SELECT MAX(ItemType) FROM EventBallInfo");
        Result* pResult = selectEventBallInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectEventBallInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM EventBallInfo");
        pResult = selectEventBallInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            EventBallInfo* pEventBallInfo = new EventBallInfo();

            pEventBallInfo->setItemType(pResult->getInt(++i));
            pEventBallInfo->setName(pResult->getString(++i));
            pEventBallInfo->setEName(pResult->getString(++i));
            pEventBallInfo->setPrice(pResult->getInt(++i));
            pEventBallInfo->setVolumeType(pResult->getInt(++i));
            pEventBallInfo->setWeight(pResult->getInt(++i));
            pEventBallInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pEventBallInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventBallLoader member methods
//////////////////////////////////////////////////////////////////////////////

void EventBallLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM EventBallObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectEventBallObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM EventBallObject WHERE "
                   "OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectEventBallObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectEventBallObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                EventBall* pEventBall = new EventBall();

                pEventBall->setItemID(pResult->getDWORD(++i));
                pEventBall->setObjectID(pResult->getDWORD(++i));
                pEventBall->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pEventBall->setNum(pResult->getBYTE(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
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
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pEventBall)) {
                        pInventory->addItemEx(x, y, pEventBall);
                    } else {
                        processItemBugEx(pCreature, pEventBall);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pEventBall);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pEventBall);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pEventBall);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pEventBall);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pEventBall);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pEventBall);
                    } else
                        pStash->insert(x, y, pEventBall);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pEventBall);
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

void EventBallLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneEventBallObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM EventBallObject WHERE "
                   "Storage = ? AND StorageID = ?");
        loadZoneEventBallObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneEventBallObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneEventBallObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            EventBall* pEventBall = new EventBall();

            pEventBall->setItemID(pResult->getInt(++i));
            pEventBall->setObjectID(pResult->getInt(++i));
            pEventBall->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pEventBall->setNum(pResult->getBYTE(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pEventBall);
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

void EventBallLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

EventBallLoader* g_pEventBallLoader = NULL;
