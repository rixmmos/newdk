//////////////////////////////////////////////////////////////////////////////
// Filename    : Money.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Money.h"

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
MoneyInfoManager* g_pMoneyInfoManager = NULL;

ItemID_t Money::m_ItemIDRegistry = 0;
Mutex Money::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Money::Money()

    : m_ItemType(0), m_Amount(0) {}

Money::Money(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

    : m_ItemType(itemType), m_Amount(0) {
    m_Num = Num;
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Money::Money() : Invalid item type or option type");
        throw("Money::Money() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Money::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        /*
        sql << "INSERT INTO MoneyObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Amount )"
            << " VALUES("
            << m_ItemID << ", "
            << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', " <<(int)storage << ", " << storageID
            << ", " <<(int)x << ", " <<(int)y << ", " << m_Amount << ")";
        */
        PreparedStatement insertMoneyObjectStmt(
            pConn, "INSERT INTO MoneyObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Amount, "
                   "Num ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertMoneyObjectStmt.bindLong(1, m_ItemID);
        insertMoneyObjectStmt.bindLong(2, m_ObjectID);
        insertMoneyObjectStmt.bindInt(3, m_ItemType);
        insertMoneyObjectStmt.bindString(4, ownerID);
        insertMoneyObjectStmt.bindInt(5, (int)storage);
        insertMoneyObjectStmt.bindLong(6, storageID);
        insertMoneyObjectStmt.bindInt(7, (int)x);
        insertMoneyObjectStmt.bindInt(8, (int)y);
        insertMoneyObjectStmt.bindLong(9, m_Amount);
        insertMoneyObjectStmt.bindInt(10, (int)m_Num);
        insertMoneyObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Money::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Amount and ItemID are bound.
        PreparedStatement tinysaveMoneyObjectStmt(pConn, string("UPDATE MoneyObject SET ") + field +
                                                             ", Amount=? WHERE ItemID=?");
        tinysaveMoneyObjectStmt.bindLong(1, m_Amount);
        tinysaveMoneyObjectStmt.bindLong(2, m_ItemID);
        tinysaveMoneyObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Money::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE MoneyObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",Amount = " << m_Amount
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement updateMoneyObjectStmt(
            pConn, "UPDATE MoneyObject SET ObjectID=? ,ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Amount=?,Num=? WHERE ItemID=?");
        updateMoneyObjectStmt.bindLong(1, m_ObjectID);
        updateMoneyObjectStmt.bindInt(2, m_ItemType);
        updateMoneyObjectStmt.bindString(3, ownerID);
        updateMoneyObjectStmt.bindInt(4, (int)storage);
        updateMoneyObjectStmt.bindLong(5, storageID);
        updateMoneyObjectStmt.bindInt(6, (int)x);
        updateMoneyObjectStmt.bindInt(7, (int)y);
        updateMoneyObjectStmt.bindLong(8, m_Amount);
        updateMoneyObjectStmt.bindInt(9, (int)m_Num);
        updateMoneyObjectStmt.bindLong(10, m_ItemID);
        updateMoneyObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Money::toString() const

{
    StringStream msg;

    msg << "Money("
        << "ItemID:" << m_ItemID << ",ItemType:" << (uint)m_ItemType << ",Amount:" << (uint)m_Amount << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Money::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pMoneyInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Money::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pMoneyInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Money::getWeight() const

{
    __BEGIN_TRY

    return g_pMoneyInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string MoneyInfo::toString() const

{
    StringStream msg;

    msg << "MoneyInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void MoneyInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMoneyInfoStmt(pConn, "SELECT MAX(ItemType) FROM MoneyInfo");
        Result* pResult = selectMoneyInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMoneyInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM MoneyInfo");
        pResult = selectMoneyInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MoneyInfo* pMoneyInfo = new MoneyInfo();

            pMoneyInfo->setItemType(pResult->getInt(++i));
            pMoneyInfo->setName(pResult->getString(++i));
            pMoneyInfo->setEName(pResult->getString(++i));
            pMoneyInfo->setPrice(pResult->getInt(++i));
            pMoneyInfo->setVolumeType(pResult->getInt(++i));
            pMoneyInfo->setWeight(pResult->getInt(++i));
            pMoneyInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pMoneyInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void MoneyLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Amount FROM MoneyObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectMoneyObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Amount, Num FROM MoneyObject WHERE "
                   "OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectMoneyObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMoneyObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Money* pMoney = new Money();

                pMoney->setItemID(pResult->getDWORD(++i));
                pMoney->setObjectID(pResult->getDWORD(++i));
                pMoney->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pMoney->setAmount(pResult->getDWORD(++i));
                pMoney->setNum(pResult->getBYTE(++i));

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
                    if (pInventory->canAddingEx(x, y, pMoney)) {
                        pInventory->addItemEx(x, y, pMoney);
                    } else {
                        processItemBugEx(pCreature, pMoney);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pMoney);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pMoney);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pMoney);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pMoney);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pMoney);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pMoney);
                    } else
                        pStash->insert(x, y, pMoney);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pMoney);
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
void MoneyLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneMoneyObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Amount FROM MoneyObject WHERE "
                   "Storage = ? AND StorageID = ?");
        loadZoneMoneyObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneMoneyObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneMoneyObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Money* pMoney = new Money();

            pMoney->setItemID(pResult->getInt(++i));
            pMoney->setObjectID(pResult->getInt(++i));
            pMoney->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pMoney->setAmount(pResult->getDWORD(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pMoney);
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
void MoneyLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

MoneyLoader* g_pMoneyLoader = NULL;
