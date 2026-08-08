//////////////////////////////////////////////////////////////////////////////
// Filename    : PetEnchantItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "PetEnchantItem.h"

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

PetEnchantItemInfoManager* g_pPetEnchantItemInfoManager = NULL;

ItemID_t PetEnchantItem::m_ItemIDRegistry = 0;
Mutex PetEnchantItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItem member methods
//////////////////////////////////////////////////////////////////////////////

PetEnchantItem::PetEnchantItem()

{
    m_ItemType = 0;
}

PetEnchantItem::PetEnchantItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

{
    m_ItemType = itemType;
    m_Num = Num;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "PetEnchantItem::PetEnchantItem() : Invalid item type or option type");
        throw("PetEnchantItem::PetEnchantItem() : Invalid item type or optionType");
    }
}

void PetEnchantItem::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertPetEnchantItemStmt(pConn, "INSERT INTO PetEnchantItemObject "
                                                          "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, "
                                                          "X, Y, Num, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertPetEnchantItemStmt.bindUInt(1, m_ItemID);
        insertPetEnchantItemStmt.bindUInt(2, m_ObjectID);
        insertPetEnchantItemStmt.bindUInt(3, m_ItemType);
        insertPetEnchantItemStmt.bindString(4, ownerID);
        insertPetEnchantItemStmt.bindInt(5, (int)storage);
        insertPetEnchantItemStmt.bindUInt(6, storageID);
        insertPetEnchantItemStmt.bindInt(7, (int)x);
        insertPetEnchantItemStmt.bindInt(8, (int)y);
        insertPetEnchantItemStmt.bindInt(9, (int)m_Num);
        insertPetEnchantItemStmt.bindInt(10, (int)m_CreateType);
        insertPetEnchantItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void PetEnchantItem::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysavePetEnchantItemStmt(pConn, string("UPDATE PetEnchantItemObject SET ") + field +
                                                                " WHERE ItemID=?");
        tinysavePetEnchantItemStmt.bindUInt(1, m_ItemID);
        tinysavePetEnchantItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void PetEnchantItem::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE PetEnchantItemObject SET "
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

        PreparedStatement savePetEnchantItemStmt(
            pConn, "UPDATE PetEnchantItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                   "StorageID=?, X=?, Y=?, Num=? WHERE ItemID=?");
        savePetEnchantItemStmt.bindUInt(1, m_ObjectID);
        savePetEnchantItemStmt.bindUInt(2, m_ItemType);
        savePetEnchantItemStmt.bindString(3, ownerID);
        savePetEnchantItemStmt.bindInt(4, (int)storage);
        savePetEnchantItemStmt.bindUInt(5, storageID);
        savePetEnchantItemStmt.bindInt(6, (int)x);
        savePetEnchantItemStmt.bindInt(7, (int)y);
        savePetEnchantItemStmt.bindInt(8, (int)m_Num);
        savePetEnchantItemStmt.bindUInt(9, m_ItemID);
        savePetEnchantItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string PetEnchantItem::toString() const

{
    StringStream msg;

    msg << "PetEnchantItem("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}

VolumeWidth_t PetEnchantItem::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pPetEnchantItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t PetEnchantItem::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pPetEnchantItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t PetEnchantItem::getWeight() const

{
    __BEGIN_TRY

    return g_pPetEnchantItemInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string PetEnchantItemInfo::toString() const

{
    StringStream msg;
    msg << "PetEnchantItemInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void PetEnchantItemInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM PetEnchantItemInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectPetEnchantItemInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, `Function`, "
                   "FunctionGrade FROM PetEnchantItemInfo");
        pResult = selectPetEnchantItemInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            PetEnchantItemInfo* pPetEnchantItemInfo = new PetEnchantItemInfo();

            pPetEnchantItemInfo->setItemType(pResult->getInt(++i));
            pPetEnchantItemInfo->setName(pResult->getString(++i));
            pPetEnchantItemInfo->setEName(pResult->getString(++i));
            pPetEnchantItemInfo->setPrice(pResult->getInt(++i));
            pPetEnchantItemInfo->setVolumeType(pResult->getInt(++i));
            pPetEnchantItemInfo->setWeight(pResult->getInt(++i));
            pPetEnchantItemInfo->setRatio(pResult->getInt(++i));
            pPetEnchantItemInfo->setFunction(pResult->getInt(++i));
            pPetEnchantItemInfo->setFunctionGrade(pResult->getInt(++i));

            addItemInfo(pPetEnchantItemInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void PetEnchantItemLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM PetEnchantItemObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectPetEnchantItemLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM "
                   "PetEnchantItemObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectPetEnchantItemLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectPetEnchantItemLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                PetEnchantItem* pPetEnchantItem = new PetEnchantItem();

                pPetEnchantItem->setItemID(pResult->getDWORD(++i));
                pPetEnchantItem->setObjectID(pResult->getDWORD(++i));
                pPetEnchantItem->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pPetEnchantItem->setNum(pResult->getBYTE(++i));
                pPetEnchantItem->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pPetEnchantItem)) {
                        pInventory->addItemEx(x, y, pPetEnchantItem);
                    } else {
                        processItemBugEx(pCreature, pPetEnchantItem);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pPetEnchantItem);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pPetEnchantItem);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pPetEnchantItem);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pPetEnchantItem);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pPetEnchantItem);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pPetEnchantItem);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pPetEnchantItem);
                    } else
                        pStash->insert(x, y, pPetEnchantItem);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pPetEnchantItem);
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

void PetEnchantItemLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZonePetEnchantItemStmt(
            pConn,
            "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM PetEnchantItemObject"
            " WHERE Storage = ? AND StorageID = ?");
        selectZonePetEnchantItemStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZonePetEnchantItemStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZonePetEnchantItemStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            PetEnchantItem* pPetEnchantItem = new PetEnchantItem();

            pPetEnchantItem->setItemID(pResult->getInt(++i));
            pPetEnchantItem->setObjectID(pResult->getInt(++i));
            pPetEnchantItem->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pPetEnchantItem->setNum(pResult->getBYTE(++i));
            pPetEnchantItem->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pPetEnchantItem);
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

void PetEnchantItemLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

PetEnchantItemLoader* g_pPetEnchantItemLoader = NULL;
