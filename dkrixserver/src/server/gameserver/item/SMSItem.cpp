//////////////////////////////////////////////////////////////////////////////
// Filename    : SMSItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SMSItem.h"

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

SMSItemInfoManager* g_pSMSItemInfoManager = NULL;

ItemID_t SMSItem::m_ItemIDRegistry = 0;
Mutex SMSItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class SMSItem member methods
//////////////////////////////////////////////////////////////////////////////

SMSItem::SMSItem()

{
    setItemType(0);
}

SMSItem::SMSItem(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "SMSItem::SMSItem() : Invalid item type or option type");
        throw("SMSItem::SMSItem() : Invalid item type or optionType");
    }
}

void SMSItem::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertSMSItemStmt(
            pConn, "INSERT INTO SMSItemObject "
                   "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertSMSItemStmt.bindUInt(1, m_ItemID);
        insertSMSItemStmt.bindUInt(2, m_ObjectID);
        insertSMSItemStmt.bindUInt(3, getItemType());
        insertSMSItemStmt.bindString(4, ownerID);
        insertSMSItemStmt.bindInt(5, (int)storage);
        insertSMSItemStmt.bindUInt(6, storageID);
        insertSMSItemStmt.bindInt(7, (int)x);
        insertSMSItemStmt.bindInt(8, (int)y);
        insertSMSItemStmt.bindInt(9, (int)m_CreateType);
        insertSMSItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void SMSItem::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveSMSItemStmt(pConn, string("UPDATE SMSItemObject SET ") + field + " WHERE ItemID=?");
        tinysaveSMSItemStmt.bindUInt(1, m_ItemID);
        tinysaveSMSItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void SMSItem::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE SMSItemObject SET "
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

        PreparedStatement saveSMSItemStmt(pConn,
                                           "UPDATE SMSItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                           "StorageID=?, X=?, Y=? WHERE ItemID=?");
        saveSMSItemStmt.bindUInt(1, m_ObjectID);
        saveSMSItemStmt.bindUInt(2, getItemType());
        saveSMSItemStmt.bindString(3, ownerID);
        saveSMSItemStmt.bindInt(4, (int)storage);
        saveSMSItemStmt.bindUInt(5, storageID);
        saveSMSItemStmt.bindInt(6, (int)x);
        saveSMSItemStmt.bindInt(7, (int)y);
        saveSMSItemStmt.bindUInt(8, m_ItemID);
        saveSMSItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string SMSItem::toString() const

{
    StringStream msg;

    msg << "SMSItem("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType() << ")";

    return msg.toString();
}

/*VolumeWidth_t SMSItem::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pSMSItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t SMSItem::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pSMSItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t SMSItem::getWeight() const

{
    __BEGIN_TRY

    return g_pSMSItemInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}
*/
//////////////////////////////////////////////////////////////////////////////
// class SMSItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string SMSItemInfo::toString() const

{
    StringStream msg;
    msg << "SMSItemInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void SMSItemInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM SMSItemInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectSMSItemInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Charge FROM SMSItemInfo");
        pResult = selectSMSItemInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            SMSItemInfo* pSMSItemInfo = new SMSItemInfo();

            pSMSItemInfo->setItemType(pResult->getInt(++i));
            pSMSItemInfo->setName(pResult->getString(++i));
            pSMSItemInfo->setEName(pResult->getString(++i));
            pSMSItemInfo->setPrice(pResult->getInt(++i));
            pSMSItemInfo->setVolumeType(pResult->getInt(++i));
            pSMSItemInfo->setWeight(pResult->getInt(++i));
            pSMSItemInfo->setRatio(pResult->getInt(++i));
            pSMSItemInfo->setCharge(pResult->getInt(++i));

            addItemInfo(pSMSItemInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class SMSItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void SMSItemLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM SMSItemObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectSMSItemLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag FROM "
                   "SMSItemObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectSMSItemLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectSMSItemLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                SMSItem* pSMSItem = new SMSItem();

                pSMSItem->setItemID(pResult->getDWORD(++i));
                pSMSItem->setObjectID(pResult->getDWORD(++i));
                pSMSItem->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pSMSItem->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pSMSItem)) {
                        pInventory->addItemEx(x, y, pSMSItem);
                    } else {
                        processItemBugEx(pCreature, pSMSItem);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pSMSItem);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pSMSItem);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pSMSItem);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pSMSItem);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pSMSItem);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pSMSItem);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pSMSItem);
                    } else
                        pStash->insert(x, y, pSMSItem);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pSMSItem);
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

void SMSItemLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneSMSItemStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag FROM SMSItemObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneSMSItemStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneSMSItemStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneSMSItemStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            SMSItem* pSMSItem = new SMSItem();

            pSMSItem->setItemID(pResult->getInt(++i));
            pSMSItem->setObjectID(pResult->getInt(++i));
            pSMSItem->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pSMSItem->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pSMSItem);
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

void SMSItemLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

SMSItemLoader* g_pSMSItemLoader = NULL;
