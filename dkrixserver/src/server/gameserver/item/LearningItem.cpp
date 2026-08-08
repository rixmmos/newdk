//////////////////////////////////////////////////////////////////////////////
// Filename    : LearningItem.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "LearningItem.h"

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
LearningItemInfoManager* g_pLearningItemInfoManager = NULL;

ItemID_t LearningItem::m_ItemIDRegistry = 0;
Mutex LearningItem::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
LearningItem::LearningItem()

    : m_ItemType(0) {}

LearningItem::LearningItem(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType) {
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "LearningItem::LearningItem() : Invalid item type or option type");
        throw("LearningItem::LearningItem() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void LearningItem::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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
        PreparedStatement insertLearningItemObjectStmt(
            pConn, "INSERT INTO LearningItemObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
        insertLearningItemObjectStmt.bindLong(1, m_ItemID);
        insertLearningItemObjectStmt.bindLong(2, m_ObjectID);
        insertLearningItemObjectStmt.bindInt(3, m_ItemType);
        insertLearningItemObjectStmt.bindString(4, ownerID);
        insertLearningItemObjectStmt.bindInt(5, (int)storage);
        insertLearningItemObjectStmt.bindLong(6, storageID);
        insertLearningItemObjectStmt.bindInt(7, (int)x);
        insertLearningItemObjectStmt.bindInt(8, (int)y);
        insertLearningItemObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void LearningItem::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveLearningItemObjectStmt(pConn, string("UPDATE LearningItemObject SET ") + field +
                                                                    " WHERE ItemID=?");
        tinysaveLearningItemObjectStmt.bindLong(1, m_ItemID);
        tinysaveLearningItemObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void LearningItem::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE LearningItemObject SET "
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

        PreparedStatement updateLearningItemObjectStmt(
            pConn, "UPDATE LearningItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=? "
                   "WHERE ItemID=?");
        updateLearningItemObjectStmt.bindLong(1, m_ObjectID);
        updateLearningItemObjectStmt.bindInt(2, m_ItemType);
        updateLearningItemObjectStmt.bindString(3, ownerID);
        updateLearningItemObjectStmt.bindInt(4, (int)storage);
        updateLearningItemObjectStmt.bindLong(5, storageID);
        updateLearningItemObjectStmt.bindInt(6, (int)x);
        updateLearningItemObjectStmt.bindInt(7, (int)y);
        updateLearningItemObjectStmt.bindLong(8, m_ItemID);
        updateLearningItemObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string LearningItem::toString() const

{
    StringStream msg;

    msg << "LearningItem("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t LearningItem::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pLearningItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t LearningItem::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pLearningItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t LearningItem::getWeight() const

{
    __BEGIN_TRY

    return g_pLearningItemInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string LearningItemInfo::toString() const

{
    StringStream msg;

    msg << "LearningItemInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",SkillType:" << m_SkillType << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void LearningItemInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectLearningItemInfoStmt(pConn, "SELECT MAX(ItemType) FROM LearningItemInfo");
        Result* pResult = selectLearningItemInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectLearningItemInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, SkillType FROM LearningItemInfo");
        pResult = selectLearningItemInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            LearningItemInfo* pLearningItemInfo = new LearningItemInfo();

            pLearningItemInfo->setItemType(pResult->getInt(++i));
            pLearningItemInfo->setName(pResult->getString(++i));
            pLearningItemInfo->setEName(pResult->getString(++i));
            pLearningItemInfo->setPrice(pResult->getInt(++i));
            pLearningItemInfo->setVolumeType(pResult->getInt(++i));
            pLearningItemInfo->setWeight(pResult->getInt(++i));
            pLearningItemInfo->setRatio(pResult->getInt(++i));
            pLearningItemInfo->setSkillType(pResult->getInt(++i));

            addItemInfo(pLearningItemInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void LearningItemLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM LearningItemObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectLearningItemObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM LearningItemObject WHERE OwnerID "
                   "= ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectLearningItemObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectLearningItemObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                LearningItem* pLearningItem = new LearningItem();

                pLearningItem->setItemID(pResult->getDWORD(++i));
                pLearningItem->setObjectID(pResult->getDWORD(++i));
                pLearningItem->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

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
                    if (pInventory->canAddingEx(x, y, pLearningItem)) {
                        pInventory->addItemEx(x, y, pLearningItem);
                    } else {
                        processItemBugEx(pCreature, pLearningItem);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pLearningItem);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pLearningItem);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pLearningItem);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pLearningItem);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pLearningItem);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pLearningItem);
                    } else
                        pStash->insert(x, y, pLearningItem);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pLearningItem);
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
void LearningItemLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneLearningItemObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM LearningItemObject WHERE Storage "
                   "= ? AND StorageID = ?");
        loadZoneLearningItemObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneLearningItemObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneLearningItemObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            LearningItem* pLearningItem = new LearningItem();

            pLearningItem->setItemID(pResult->getInt(++i));
            pLearningItem->setObjectID(pResult->getInt(++i));
            pLearningItem->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pLearningItem);
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
void LearningItemLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

LearningItemLoader* g_pLearningItemLoader = NULL;
