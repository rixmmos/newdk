//////////////////////////////////////////////////////////////////////////////
// Filename    : EventGiftBox.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventGiftBox.h"

#include "Belt.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "Ousters.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Vampire.h"

EventGiftBoxInfoManager* g_pEventGiftBoxInfoManager = NULL;

ItemID_t EventGiftBox::m_ItemIDRegistry = 0;
Mutex EventGiftBox::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class EventGiftBox member methods
//////////////////////////////////////////////////////////////////////////////

EventGiftBox::EventGiftBox()

{
    m_ItemType = 0;
}

EventGiftBox::EventGiftBox(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType) {
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "EventGiftBox::EventGiftBox() : Invalid item type or option type");
        throw("EventGiftBox::EventGiftBox() : Invalid item type or optionType");
    }
}

void EventGiftBox::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertEventGiftBoxObjectStmt(
            pConn, "INSERT INTO EventGiftBoxObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
        insertEventGiftBoxObjectStmt.bindLong(1, m_ItemID);
        insertEventGiftBoxObjectStmt.bindLong(2, m_ObjectID);
        insertEventGiftBoxObjectStmt.bindInt(3, m_ItemType);
        insertEventGiftBoxObjectStmt.bindString(4, ownerID);
        insertEventGiftBoxObjectStmt.bindInt(5, (int)storage);
        insertEventGiftBoxObjectStmt.bindLong(6, storageID);
        insertEventGiftBoxObjectStmt.bindInt(7, (int)x);
        insertEventGiftBoxObjectStmt.bindInt(8, (int)y);
        insertEventGiftBoxObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void EventGiftBox::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveEventGiftBoxObjectStmt(
            pConn, string("UPDATE EventGiftBoxObject SET ") + field + " WHERE ItemID=?");
        tinysaveEventGiftBoxObjectStmt.bindLong(1, m_ItemID);
        tinysaveEventGiftBoxObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

void EventGiftBox::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE EventGiftBoxObject SET "
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

        PreparedStatement updateEventGiftBoxObjectStmt(
            pConn, "UPDATE EventGiftBoxObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=? "
                   "WHERE ItemID=?");
        updateEventGiftBoxObjectStmt.bindLong(1, m_ObjectID);
        updateEventGiftBoxObjectStmt.bindInt(2, m_ItemType);
        updateEventGiftBoxObjectStmt.bindString(3, ownerID);
        updateEventGiftBoxObjectStmt.bindInt(4, (int)storage);
        updateEventGiftBoxObjectStmt.bindLong(5, storageID);
        updateEventGiftBoxObjectStmt.bindInt(6, (int)x);
        updateEventGiftBoxObjectStmt.bindInt(7, (int)y);
        updateEventGiftBoxObjectStmt.bindLong(8, m_ItemID);
        updateEventGiftBoxObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

string EventGiftBox::toString() const

{
    StringStream msg;
    msg << "EventGiftBox("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ")";
    return msg.toString();
}

VolumeWidth_t EventGiftBox::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pEventGiftBoxInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t EventGiftBox::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pEventGiftBoxInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t EventGiftBox::getWeight() const

{
    __BEGIN_TRY

    return g_pEventGiftBoxInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventGiftBoxInfo member methods
//////////////////////////////////////////////////////////////////////////////

string EventGiftBoxInfo::toString() const

{
    StringStream msg;
    msg << "EventGiftBoxInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";
    return msg.toString();
}

void EventGiftBoxInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectEventGiftBoxInfoStmt(pConn, "SELECT MAX(ItemType) FROM EventGiftBoxInfo");
        Result* pResult = selectEventGiftBoxInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectEventGiftBoxInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM EventGiftBoxInfo");
        pResult = selectEventGiftBoxInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            EventGiftBoxInfo* pEventGiftBoxInfo = new EventGiftBoxInfo();

            pEventGiftBoxInfo->setItemType(pResult->getInt(++i));
            pEventGiftBoxInfo->setName(pResult->getString(++i));
            pEventGiftBoxInfo->setEName(pResult->getString(++i));
            pEventGiftBoxInfo->setPrice(pResult->getInt(++i));
            pEventGiftBoxInfo->setVolumeType(pResult->getInt(++i));
            pEventGiftBoxInfo->setWeight(pResult->getInt(++i));
            pEventGiftBoxInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pEventGiftBoxInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventGiftBoxLoader member methods
//////////////////////////////////////////////////////////////////////////////

void EventGiftBoxLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM EventGiftBoxObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectEventGiftBoxObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM EventGiftBoxObject WHERE OwnerID "
                   "= ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectEventGiftBoxObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectEventGiftBoxObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                EventGiftBox* pEventGiftBox = new EventGiftBox();

                pEventGiftBox->setItemID(pResult->getDWORD(++i));
                pEventGiftBox->setObjectID(pResult->getDWORD(++i));
                pEventGiftBox->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Ousters* pOusters = NULL;
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
                } else if (pCreature->isOusters()) {
                    pOusters = dynamic_cast<Ousters*>(pCreature);
                    pInventory = pOusters->getInventory();
                    pStash = pOusters->getStash();
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pEventGiftBox)) {
                        pInventory->addItemEx(x, y, pEventGiftBox);
                    } else {
                        processItemBugEx(pCreature, pEventGiftBox);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pEventGiftBox);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pEventGiftBox);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pEventGiftBox);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pEventGiftBox);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pEventGiftBox);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pEventGiftBox);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pEventGiftBox);
                    } else
                        pStash->insert(x, y, pEventGiftBox);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pEventGiftBox);
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

void EventGiftBoxLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneEventGiftBoxObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM EventGiftBoxObject WHERE Storage "
                   "= ? AND StorageID = ?");
        loadZoneEventGiftBoxObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneEventGiftBoxObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneEventGiftBoxObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            EventGiftBox* pEventGiftBox = new EventGiftBox();

            pEventGiftBox->setItemID(pResult->getInt(++i));
            pEventGiftBox->setObjectID(pResult->getInt(++i));
            pEventGiftBox->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pEventGiftBox);
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

void EventGiftBoxLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

EventGiftBoxLoader* g_pEventGiftBoxLoader = NULL;
