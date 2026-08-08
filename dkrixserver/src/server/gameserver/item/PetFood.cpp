//////////////////////////////////////////////////////////////////////////////
// Filename    : PetFood.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "PetFood.h"

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

PetFoodInfoManager* g_pPetFoodInfoManager = NULL;

ItemID_t PetFood::m_ItemIDRegistry = 0;
Mutex PetFood::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class PetFood member methods
//////////////////////////////////////////////////////////////////////////////

PetFood::PetFood()

{
    m_ItemType = 0;
    m_Num = 1;
}

PetFood::PetFood(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

{
    m_ItemType = itemType;
    m_Num = Num;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "PetFood::PetFood() : Invalid item type or option type");
        throw("PetFood::PetFood() : Invalid item type or optionType");
    }
}

void PetFood::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertPetFoodStmt(
            pConn, "INSERT INTO PetFoodObject "
                   "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertPetFoodStmt.bindUInt(1, m_ItemID);
        insertPetFoodStmt.bindUInt(2, m_ObjectID);
        insertPetFoodStmt.bindUInt(3, m_ItemType);
        insertPetFoodStmt.bindString(4, ownerID);
        insertPetFoodStmt.bindInt(5, (int)storage);
        insertPetFoodStmt.bindUInt(6, storageID);
        insertPetFoodStmt.bindInt(7, (int)x);
        insertPetFoodStmt.bindInt(8, (int)y);
        insertPetFoodStmt.bindInt(9, (int)m_Num);
        insertPetFoodStmt.bindInt(10, (int)m_CreateType);
        insertPetFoodStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void PetFood::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysavePetFoodStmt(pConn, string("UPDATE PetFoodObject SET ") + field + " WHERE ItemID=?");
        tinysavePetFoodStmt.bindUInt(1, m_ItemID);
        tinysavePetFoodStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void PetFood::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE PetFoodObject SET "
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

        PreparedStatement savePetFoodStmt(pConn,
                                           "UPDATE PetFoodObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                           "StorageID=?, X=?, Y=?, Num=? WHERE ItemID=?");
        savePetFoodStmt.bindUInt(1, m_ObjectID);
        savePetFoodStmt.bindUInt(2, m_ItemType);
        savePetFoodStmt.bindString(3, ownerID);
        savePetFoodStmt.bindInt(4, (int)storage);
        savePetFoodStmt.bindUInt(5, storageID);
        savePetFoodStmt.bindInt(6, (int)x);
        savePetFoodStmt.bindInt(7, (int)y);
        savePetFoodStmt.bindUInt(8, m_Num);
        savePetFoodStmt.bindUInt(9, m_ItemID);
        savePetFoodStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string PetFood::toString() const

{
    StringStream msg;

    msg << "PetFood("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ")";

    return msg.toString();
}

VolumeWidth_t PetFood::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pPetFoodInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t PetFood::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pPetFoodInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t PetFood::getWeight() const

{
    __BEGIN_TRY

    return g_pPetFoodInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class PetFoodInfo member methods
//////////////////////////////////////////////////////////////////////////////

string PetFoodInfo::toString() const

{
    StringStream msg;
    msg << "PetFoodInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void PetFoodInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM PetFoodInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectPetFoodInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Target, PetHP, TameRatio FROM PetFoodInfo");
        pResult = selectPetFoodInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            PetFoodInfo* pPetFoodInfo = new PetFoodInfo();

            pPetFoodInfo->setItemType(pResult->getInt(++i));
            pPetFoodInfo->setName(pResult->getString(++i));
            pPetFoodInfo->setEName(pResult->getString(++i));
            pPetFoodInfo->setPrice(pResult->getInt(++i));
            pPetFoodInfo->setVolumeType(pResult->getInt(++i));
            pPetFoodInfo->setWeight(pResult->getInt(++i));
            pPetFoodInfo->setRatio(pResult->getInt(++i));
            pPetFoodInfo->setTarget(pResult->getInt(++i));
            pPetFoodInfo->setPetHP(pResult->getInt(++i));
            pPetFoodInfo->setTameRatio(pResult->getInt(++i));

            addItemInfo(pPetFoodInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class PetFoodLoader member methods
//////////////////////////////////////////////////////////////////////////////

void PetFoodLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM PetFoodObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectPetFoodLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM "
                   "PetFoodObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectPetFoodLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectPetFoodLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                PetFood* pPetFood = new PetFood();

                pPetFood->setItemID(pResult->getDWORD(++i));
                pPetFood->setObjectID(pResult->getDWORD(++i));
                pPetFood->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pPetFood->setNum(pResult->getInt(++i));
                pPetFood->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pPetFood)) {
                        pInventory->addItemEx(x, y, pPetFood);
                    } else {
                        processItemBugEx(pCreature, pPetFood);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pPetFood);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pPetFood);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pPetFood);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pPetFood);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pPetFood);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pPetFood);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pPetFood);
                    } else
                        pStash->insert(x, y, pPetFood);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pPetFood);
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

void PetFoodLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZonePetFoodStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag FROM PetFoodObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZonePetFoodStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZonePetFoodStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZonePetFoodStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            PetFood* pPetFood = new PetFood();

            pPetFood->setItemID(pResult->getInt(++i));
            pPetFood->setObjectID(pResult->getInt(++i));
            pPetFood->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pPetFood->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pPetFood);
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

void PetFoodLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

PetFoodLoader* g_pPetFoodLoader = NULL;
