//////////////////////////////////////////////////////////////////////////////
// Filename    : MixingItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "MixingItem.h"

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

MixingItemInfoManager* g_pMixingItemInfoManager = NULL;

ItemID_t MixingItem::m_ItemIDRegistry = 0;
Mutex MixingItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class MixingItem member methods
//////////////////////////////////////////////////////////////////////////////

MixingItem::MixingItem()

{
    m_ItemType = 0;
}

MixingItem::MixingItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

{
    m_ItemType = itemType;
    m_Num = Num;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "MixingItem::MixingItem() : Invalid item type or option type");
        throw("MixingItem::MixingItem() : Invalid item type or optionType");
    }
}

void MixingItem::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertMixingItemObjectStmt(
            pConn, "INSERT INTO MixingItemObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, "
                   "Num, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertMixingItemObjectStmt.bindLong(1, m_ItemID);
        insertMixingItemObjectStmt.bindLong(2, m_ObjectID);
        insertMixingItemObjectStmt.bindInt(3, m_ItemType);
        insertMixingItemObjectStmt.bindString(4, ownerID);
        insertMixingItemObjectStmt.bindInt(5, (int)storage);
        insertMixingItemObjectStmt.bindLong(6, storageID);
        insertMixingItemObjectStmt.bindInt(7, (int)x);
        insertMixingItemObjectStmt.bindInt(8, (int)y);
        insertMixingItemObjectStmt.bindInt(9, (int)m_Num);
        insertMixingItemObjectStmt.bindInt(10, (int)m_CreateType);
        insertMixingItemObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void MixingItem::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveMixingItemObjectStmt(pConn, string("UPDATE MixingItemObject SET ") + field +
                                                                  " WHERE ItemID=?");
        tinysaveMixingItemObjectStmt.bindLong(1, m_ItemID);
        tinysaveMixingItemObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void MixingItem::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE MixingItemObject SET "
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

        PreparedStatement updateMixingItemObjectStmt(
            pConn, "UPDATE MixingItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateMixingItemObjectStmt.bindLong(1, m_ObjectID);
        updateMixingItemObjectStmt.bindInt(2, m_ItemType);
        updateMixingItemObjectStmt.bindString(3, ownerID);
        updateMixingItemObjectStmt.bindInt(4, (int)storage);
        updateMixingItemObjectStmt.bindLong(5, storageID);
        updateMixingItemObjectStmt.bindInt(6, (int)x);
        updateMixingItemObjectStmt.bindInt(7, (int)y);
        updateMixingItemObjectStmt.bindInt(8, (int)m_Num);
        updateMixingItemObjectStmt.bindLong(9, m_ItemID);
        updateMixingItemObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string MixingItem::toString() const

{
    StringStream msg;

    msg << "MixingItem("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}

VolumeWidth_t MixingItem::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pMixingItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t MixingItem::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pMixingItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t MixingItem::getWeight() const

{
    __BEGIN_TRY

    return g_pMixingItemInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class MixingItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string MixingItemInfo::toString() const

{
    StringStream msg;
    msg << "MixingItemInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void MixingItemInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMixingItemInfoStmt(pConn, "SELECT MAX(ItemType) FROM MixingItemInfo");
        Result* pResult = selectMixingItemInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMixingItemInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Target-1, Type-1, SlayerLevel, VampireLevel, "
                   "OustersLevel FROM MixingItemInfo");
        pResult = selectMixingItemInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MixingItemInfo* pMixingItemInfo = new MixingItemInfo();

            pMixingItemInfo->setItemType(pResult->getInt(++i));
            pMixingItemInfo->setName(pResult->getString(++i));
            pMixingItemInfo->setEName(pResult->getString(++i));
            pMixingItemInfo->setPrice(pResult->getInt(++i));
            pMixingItemInfo->setVolumeType(pResult->getInt(++i));
            pMixingItemInfo->setWeight(pResult->getInt(++i));
            pMixingItemInfo->setTarget((MixingItemInfo::Target)pResult->getInt(++i));
            pMixingItemInfo->setType((MixingItemInfo::Type)pResult->getInt(++i));
            pMixingItemInfo->setSlayerLevel(pResult->getInt(++i));
            pMixingItemInfo->setVampireLevel(pResult->getInt(++i));
            pMixingItemInfo->setOustersLevel(pResult->getInt(++i));

            addItemInfo(pMixingItemInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class MixingItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void MixingItemLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM MixingItemObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectMixingItemObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM MixingItemObject "
                   "WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectMixingItemObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMixingItemObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                MixingItem* pMixingItem = new MixingItem();

                pMixingItem->setItemID(pResult->getDWORD(++i));
                pMixingItem->setObjectID(pResult->getDWORD(++i));
                pMixingItem->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pMixingItem->setNum(pResult->getInt(++i));
                pMixingItem->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pMixingItem)) {
                        pInventory->addItemEx(x, y, pMixingItem);
                    } else {
                        processItemBugEx(pCreature, pMixingItem);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pMixingItem);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pMixingItem);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pMixingItem);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pMixingItem);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pMixingItem);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pMixingItem);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pMixingItem);
                    } else
                        pStash->insert(x, y, pMixingItem);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pMixingItem);
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

void MixingItemLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneMixingItemObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM MixingItemObject "
                   "WHERE Storage = ? AND StorageID = ?");
        loadZoneMixingItemObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneMixingItemObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneMixingItemObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            MixingItem* pMixingItem = new MixingItem();

            pMixingItem->setItemID(pResult->getInt(++i));
            pMixingItem->setObjectID(pResult->getInt(++i));
            pMixingItem->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pMixingItem->setNum(pResult->getInt(++i));
            pMixingItem->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pMixingItem);
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

void MixingItemLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

MixingItemLoader* g_pMixingItemLoader = NULL;
