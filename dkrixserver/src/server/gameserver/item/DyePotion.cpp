//////////////////////////////////////////////////////////////////////////////
// Filename    : DyePotion.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "DyePotion.h"

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

DyePotionInfoManager* g_pDyePotionInfoManager = NULL;

ItemID_t DyePotion::m_ItemIDRegistry = 0;
Mutex DyePotion::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class DyePotion member methods
//////////////////////////////////////////////////////////////////////////////

DyePotion::DyePotion()

{
    setItemType(0);
}

DyePotion::DyePotion(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

{
    setItemType(itemType);
    setNum(Num);

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "DyePotion::DyePotion() : Invalid item type or option type");
        throw("DyePotion::DyePotion() : Invalid item type or optionType");
    }
}

void DyePotion::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertDyePotionObjectStmt(
            pConn, "INSERT INTO DyePotionObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, "
                   "ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertDyePotionObjectStmt.bindLong(1, m_ItemID);
        insertDyePotionObjectStmt.bindLong(2, m_ObjectID);
        insertDyePotionObjectStmt.bindInt(3, getItemType());
        insertDyePotionObjectStmt.bindString(4, ownerID);
        insertDyePotionObjectStmt.bindInt(5, (int)storage);
        insertDyePotionObjectStmt.bindLong(6, storageID);
        insertDyePotionObjectStmt.bindInt(7, (int)x);
        insertDyePotionObjectStmt.bindInt(8, (int)y);
        insertDyePotionObjectStmt.bindInt(9, (int)getNum());
        insertDyePotionObjectStmt.bindInt(10, (int)m_CreateType);
        insertDyePotionObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void DyePotion::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveDyePotionObjectStmt(
            pConn, string("UPDATE DyePotionObject SET ") + field + " WHERE ItemID=?");
        tinysaveDyePotionObjectStmt.bindLong(1, m_ItemID);
        tinysaveDyePotionObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

void DyePotion::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateDyePotionObjectStmt(
            pConn, "UPDATE DyePotionObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateDyePotionObjectStmt.bindLong(1, m_ObjectID);
        updateDyePotionObjectStmt.bindInt(2, getItemType());
        updateDyePotionObjectStmt.bindString(3, ownerID);
        updateDyePotionObjectStmt.bindInt(4, (int)storage);
        updateDyePotionObjectStmt.bindLong(5, storageID);
        updateDyePotionObjectStmt.bindInt(6, (int)x);
        updateDyePotionObjectStmt.bindInt(7, (int)y);
        updateDyePotionObjectStmt.bindInt(8, (int)getNum());
        updateDyePotionObjectStmt.bindLong(9, m_ItemID);
        updateDyePotionObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}

string DyePotion::toString() const

{
    StringStream msg;

    msg << "DyePotion("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType() << ",Num:" << (int)getNum() << ")";

    return msg.toString();
}

/*VolumeWidth_t DyePotion::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pDyePotionInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t DyePotion::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pDyePotionInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t DyePotion::getWeight() const

{
    __BEGIN_TRY

    return g_pDyePotionInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}
*/
//////////////////////////////////////////////////////////////////////////////
// class DyePotionInfo member methods
//////////////////////////////////////////////////////////////////////////////

string DyePotionInfo::toString() const

{
    StringStream msg;
    msg << "DyePotionInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Function:" << (int)m_fFunction << ",FunctionValue:" << (int)m_FunctionValue
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void DyePotionInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectDyePotionInfoStmt(pConn, "SELECT MAX(ItemType) FROM DyePotionInfo");
        Result* pResult = selectDyePotionInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectDyePotionInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, FunctionFlag, FunctionValue FROM "
                   "DyePotionInfo");
        pResult = selectDyePotionInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            DyePotionInfo* pDyePotionInfo = new DyePotionInfo();

            pDyePotionInfo->setItemType(pResult->getInt(++i));
            pDyePotionInfo->setName(pResult->getString(++i));
            pDyePotionInfo->setEName(pResult->getString(++i));
            pDyePotionInfo->setPrice(pResult->getInt(++i));
            pDyePotionInfo->setVolumeType(pResult->getInt(++i));
            pDyePotionInfo->setWeight(pResult->getInt(++i));
            pDyePotionInfo->setRatio(pResult->getInt(++i));
            pDyePotionInfo->setFunctionFlag(pResult->getInt(++i));
            pDyePotionInfo->setFunctionValue(pResult->getInt(++i));

            addItemInfo(pDyePotionInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class DyePotionLoader member methods
//////////////////////////////////////////////////////////////////////////////

void DyePotionLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectDyePotionObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM DyePotionObject "
                   "WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectDyePotionObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectDyePotionObjectStmt.execute();

        while (pResult->next()) {
            try {
                uint i = 0;

                DyePotion* pDyePotion = new DyePotion();

                pDyePotion->setItemID(pResult->getDWORD(++i));
                pDyePotion->setObjectID(pResult->getDWORD(++i));
                pDyePotion->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pDyePotion->setNum(pResult->getBYTE(++i));
                pDyePotion->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pDyePotion)) {
                        pInventory->addItemEx(x, y, pDyePotion);
                    } else {
                        processItemBugEx(pCreature, pDyePotion);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pDyePotion);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pDyePotion);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pDyePotion);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pDyePotion);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pDyePotion);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pDyePotion);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pDyePotion);
                    } else
                        pStash->insert(x, y, pDyePotion);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pDyePotion);
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

void DyePotionLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneDyePotionObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM DyePotionObject "
                   "WHERE Storage = ? AND StorageID = ?");
        loadZoneDyePotionObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneDyePotionObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneDyePotionObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            DyePotion* pDyePotion = new DyePotion();

            pDyePotion->setItemID(pResult->getInt(++i));
            pDyePotion->setObjectID(pResult->getInt(++i));
            pDyePotion->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pDyePotion->setNum(pResult->getBYTE(++i));
            pDyePotion->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pDyePotion);
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

void DyePotionLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

DyePotionLoader* g_pDyePotionLoader = NULL;
