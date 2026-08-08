//////////////////////////////////////////////////////////////////////////////
// Filename    : MoonCard.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "MoonCard.h"

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

MoonCardInfoManager* g_pMoonCardInfoManager = NULL;

ItemID_t MoonCard::m_ItemIDRegistry = 0;
Mutex MoonCard::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class MoonCard member methods
//////////////////////////////////////////////////////////////////////////////

MoonCard::MoonCard()

{
    m_ItemType = 0;
}

MoonCard::MoonCard(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

{
    m_ItemType = itemType;
    m_Num = Num;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "MoonCard::MoonCard() : Invalid item type or option type");
        throw("MoonCard::MoonCard() : Invalid item type or optionType");
    }
}

void MoonCard::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertMoonCardObjectStmt(
            pConn, "INSERT INTO MoonCardObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, "
                   "ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertMoonCardObjectStmt.bindLong(1, m_ItemID);
        insertMoonCardObjectStmt.bindLong(2, m_ObjectID);
        insertMoonCardObjectStmt.bindInt(3, m_ItemType);
        insertMoonCardObjectStmt.bindString(4, ownerID);
        insertMoonCardObjectStmt.bindInt(5, (int)storage);
        insertMoonCardObjectStmt.bindLong(6, storageID);
        insertMoonCardObjectStmt.bindInt(7, (int)x);
        insertMoonCardObjectStmt.bindInt(8, (int)y);
        insertMoonCardObjectStmt.bindInt(9, (int)m_Num);
        insertMoonCardObjectStmt.bindInt(10, (int)m_CreateType);
        insertMoonCardObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void MoonCard::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveMoonCardObjectStmt(
            pConn, string("UPDATE MoonCardObject SET ") + field + " WHERE ItemID=?");
        tinysaveMoonCardObjectStmt.bindLong(1, m_ItemID);
        tinysaveMoonCardObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

void MoonCard::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE MoonCardObject SET "
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

        PreparedStatement updateMoonCardObjectStmt(
            pConn, "UPDATE MoonCardObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateMoonCardObjectStmt.bindLong(1, m_ObjectID);
        updateMoonCardObjectStmt.bindInt(2, m_ItemType);
        updateMoonCardObjectStmt.bindString(3, ownerID);
        updateMoonCardObjectStmt.bindInt(4, (int)storage);
        updateMoonCardObjectStmt.bindLong(5, storageID);
        updateMoonCardObjectStmt.bindInt(6, (int)x);
        updateMoonCardObjectStmt.bindInt(7, (int)y);
        updateMoonCardObjectStmt.bindInt(8, (int)m_Num);
        updateMoonCardObjectStmt.bindLong(9, m_ItemID);
        updateMoonCardObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}

string MoonCard::toString() const

{
    StringStream msg;

    msg << "MoonCard("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}

VolumeWidth_t MoonCard::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pMoonCardInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t MoonCard::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pMoonCardInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t MoonCard::getWeight() const

{
    __BEGIN_TRY

    return g_pMoonCardInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class MoonCardInfo member methods
//////////////////////////////////////////////////////////////////////////////

string MoonCardInfo::toString() const

{
    StringStream msg;
    msg << "MoonCardInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight << ")";
    return msg.toString();
}

void MoonCardInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMoonCardInfoStmt(pConn, "SELECT MAX(ItemType) FROM MoonCardInfo");
        Result* pResult = selectMoonCardInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMoonCardInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM MoonCardInfo");
        pResult = selectMoonCardInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MoonCardInfo* pMoonCardInfo = new MoonCardInfo();

            pMoonCardInfo->setItemType(pResult->getInt(++i));
            pMoonCardInfo->setName(pResult->getString(++i));
            pMoonCardInfo->setEName(pResult->getString(++i));
            pMoonCardInfo->setPrice(pResult->getInt(++i));
            pMoonCardInfo->setVolumeType(pResult->getInt(++i));
            pMoonCardInfo->setWeight(pResult->getInt(++i));
            pMoonCardInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pMoonCardInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class MoonCardLoader member methods
//////////////////////////////////////////////////////////////////////////////

void MoonCardLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM MoonCardObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectMoonCardObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM MoonCardObject "
                   "WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectMoonCardObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMoonCardObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                MoonCard* pMoonCard = new MoonCard();

                pMoonCard->setItemID(pResult->getDWORD(++i));
                pMoonCard->setObjectID(pResult->getDWORD(++i));
                pMoonCard->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pMoonCard->setNum(pResult->getBYTE(++i));
                pMoonCard->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pMoonCard)) {
                        pInventory->addItemEx(x, y, pMoonCard);
                    } else {
                        processItemBugEx(pCreature, pMoonCard);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pMoonCard);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pMoonCard);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pMoonCard);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pMoonCard);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pMoonCard);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pMoonCard);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pMoonCard);
                    } else
                        pStash->insert(x, y, pMoonCard);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pMoonCard);
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

void MoonCardLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneMoonCardObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM MoonCardObject "
                   "WHERE Storage = ? AND StorageID = ?");
        loadZoneMoonCardObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneMoonCardObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneMoonCardObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            MoonCard* pMoonCard = new MoonCard();

            pMoonCard->setItemID(pResult->getInt(++i));
            pMoonCard->setObjectID(pResult->getInt(++i));
            pMoonCard->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pMoonCard->setNum(pResult->getBYTE(++i));
            pMoonCard->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pMoonCard);
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

void MoonCardLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

MoonCardLoader* g_pMoonCardLoader = NULL;
