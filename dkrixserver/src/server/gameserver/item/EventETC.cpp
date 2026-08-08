//////////////////////////////////////////////////////////////////////////////
// Filename    : EventETC.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventETC.h"

#include "Belt.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "Ousters.h"
#include "OustersArmsband.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Utility.h"
#include "Vampire.h"

EventETCInfoManager* g_pEventETCInfoManager = NULL;

ItemID_t EventETC::m_ItemIDRegistry = 0;
Mutex EventETC::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class EventETC member methods
//////////////////////////////////////////////////////////////////////////////

EventETC::EventETC()

{
    m_ItemType = 0;
}

EventETC::EventETC(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

{
    m_ItemType = itemType;
    m_Num = Num;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "EventETC::EventETC() : Invalid item type or option type");
        throw("EventETC::EventETC() : Invalid item type or optionType");
    }
}

void EventETC::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertEventETCObjectStmt(
            pConn, "INSERT INTO EventETCObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, "
                   "ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertEventETCObjectStmt.bindLong(1, m_ItemID);
        insertEventETCObjectStmt.bindLong(2, m_ObjectID);
        insertEventETCObjectStmt.bindInt(3, m_ItemType);
        insertEventETCObjectStmt.bindString(4, ownerID);
        insertEventETCObjectStmt.bindInt(5, (int)storage);
        insertEventETCObjectStmt.bindLong(6, storageID);
        insertEventETCObjectStmt.bindInt(7, (int)x);
        insertEventETCObjectStmt.bindInt(8, (int)y);
        insertEventETCObjectStmt.bindInt(9, (int)m_Num);
        insertEventETCObjectStmt.bindInt(10, (int)m_CreateType);
        insertEventETCObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void EventETC::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveEventETCObjectStmt(
            pConn, string("UPDATE EventETCObject SET ") + field + " WHERE ItemID=?");
        tinysaveEventETCObjectStmt.bindLong(1, m_ItemID);
        tinysaveEventETCObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

void EventETC::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE EventETCObject SET "
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

        PreparedStatement updateEventETCObjectStmt(
            pConn, "UPDATE EventETCObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateEventETCObjectStmt.bindLong(1, m_ObjectID);
        updateEventETCObjectStmt.bindInt(2, m_ItemType);
        updateEventETCObjectStmt.bindString(3, ownerID);
        updateEventETCObjectStmt.bindInt(4, (int)storage);
        updateEventETCObjectStmt.bindLong(5, storageID);
        updateEventETCObjectStmt.bindInt(6, (int)x);
        updateEventETCObjectStmt.bindInt(7, (int)y);
        updateEventETCObjectStmt.bindInt(8, (int)m_Num);
        updateEventETCObjectStmt.bindLong(9, m_ItemID);
        updateEventETCObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}

string EventETC::toString() const

{
    StringStream msg;

    msg << "EventETC("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}

VolumeWidth_t EventETC::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pEventETCInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t EventETC::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pEventETCInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t EventETC::getWeight() const

{
    __BEGIN_TRY

    return g_pEventETCInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventETCInfo member methods
//////////////////////////////////////////////////////////////////////////////

string EventETCInfo::toString() const

{
    StringStream msg;
    msg << "EventETCInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void EventETCInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectEventETCInfoStmt(pConn, "SELECT MAX(ItemType) FROM EventETCInfo");
        Result* pResult = selectEventETCInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectEventETCInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, `Function` FROM EventETCInfo");
        pResult = selectEventETCInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            EventETCInfo* pEventETCInfo = new EventETCInfo();

            pEventETCInfo->setItemType(pResult->getInt(++i));
            pEventETCInfo->setName(pResult->getString(++i));
            pEventETCInfo->setEName(pResult->getString(++i));
            pEventETCInfo->setPrice(pResult->getInt(++i));
            pEventETCInfo->setVolumeType(pResult->getInt(++i));
            pEventETCInfo->setWeight(pResult->getInt(++i));
            pEventETCInfo->setRatio(pResult->getInt(++i));
            pEventETCInfo->setFunction(pResult->getInt(++i));

            addItemInfo(pEventETCInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EventETCLoader member methods
//////////////////////////////////////////////////////////////////////////////

void EventETCLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM EventETCObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectEventETCObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM EventETCObject "
                   "WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectEventETCObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectEventETCObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                EventETC* pEventETC = new EventETC();

                pEventETC->setItemID(pResult->getDWORD(++i));
                pEventETC->setObjectID(pResult->getDWORD(++i));
                pEventETC->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pEventETC->setNum(pResult->getBYTE(++i));
                pEventETC->setCreateType((Item::CreateType)pResult->getInt(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Ousters* pOusters = NULL;
                Motorcycle* pMotorcycle = NULL;
                Inventory* pMotorInventory = NULL;
                Item* pItem = NULL;
                Stash* pStash = NULL;
                Belt* pBelt = NULL;
                OustersArmsband* pOustersArmsband = NULL;
                Inventory* pBeltInventory = NULL;
                Inventory* pArmsbandInventory = NULL;

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
                    if (pInventory->canAddingEx(x, y, pEventETC)) {
                        pInventory->addItemEx(x, y, pEventETC);
                    } else {
                        processItemBugEx(pCreature, pEventETC);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pEventETC);
                    break;

                case STORAGE_BELT:
                    if (pCreature->isSlayer()) {
                        pItem = pSlayer->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pEventETC)) {
                                pBeltInventory->addItem(x, 0, pEventETC);
                            } else {
                                processItemBugEx(pCreature, pEventETC);
                            }
                        } else {
                            processItemBugEx(pCreature, pEventETC);
                        }
                    } else if (pCreature->isVampire()) {
                        pItem = pVampire->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pEventETC)) {
                                pBeltInventory->addItemEx(x, 0, pEventETC);
                            } else {
                                processItemBugEx(pCreature, pEventETC);
                            }
                        } else {
                            processItemBugEx(pCreature, pEventETC);
                        }
                    } else if (pCreature->isOusters()) {
                        pItem = findItemIID(pOusters, storageID, Item::ITEM_CLASS_OUSTERS_ARMSBAND);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_ARMSBAND) {
                            pOustersArmsband = dynamic_cast<OustersArmsband*>(pItem);
                            pArmsbandInventory = pOustersArmsband->getInventory();
                            if (pArmsbandInventory->canAddingEx(x, 0, pEventETC)) {
                                pArmsbandInventory->addItemEx(x, 0, pEventETC);
                            } else {
                                processItemBugEx(pCreature, pEventETC);
                            }
                        } else {
                            processItemBugEx(pCreature, pEventETC);
                        }
                    } else {
                        processItemBugEx(pCreature, pEventETC);
                    }
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pEventETC);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pEventETC);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pEventETC);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pEventETC);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pEventETC);
                    } else
                        pStash->insert(x, y, pEventETC);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pEventETC);
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

void EventETCLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneEventETCObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM EventETCObject "
                   "WHERE Storage = ? AND StorageID = ?");
        loadZoneEventETCObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneEventETCObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneEventETCObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            EventETC* pEventETC = new EventETC();

            pEventETC->setItemID(pResult->getInt(++i));
            pEventETC->setObjectID(pResult->getInt(++i));
            pEventETC->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pEventETC->setNum(pResult->getBYTE(++i));
            pEventETC->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pEventETC);
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

void EventETCLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

EventETCLoader* g_pEventETCLoader = NULL;
