//////////////////////////////////////////////////////////////////////////////
// Filename    : Larva.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Larva.h"

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
#include "ZoneGroupManager.h"

// global variable declaration
LarvaInfoManager* g_pLarvaInfoManager = NULL;

ItemID_t Larva::m_ItemIDRegistry = 0;
Mutex Larva::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Larva::Larva()

    : m_ItemType(0) {}

Larva::Larva(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

    : m_ItemType(itemType), m_Num(Num) {
    // cout << "Larva::Larva(" << getOptionTypeToString(optionType).c_str() << ")" << endl;
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Larva::Larva() : Invalid item type or option type");
        throw("Larva::Larva() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Larva::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        // pStmt = g_pDatabaseManager->getConnection("DIST_DARKEDEN")->createStatement();
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "INSERT INTO LarvaObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) VALUES("
            << m_ItemID << ", "
            << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', "
            <<(int)storage << ", " << storageID << ", " <<(int)x << ", " <<(int)y << ", "
            << (int)m_Num << ")";

        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement insertLarvaObjectStmt(
            pConn, "INSERT INTO LarvaObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertLarvaObjectStmt.bindLong(1, m_ItemID);
        insertLarvaObjectStmt.bindLong(2, m_ObjectID);
        insertLarvaObjectStmt.bindInt(3, m_ItemType);
        insertLarvaObjectStmt.bindString(4, ownerID);
        insertLarvaObjectStmt.bindInt(5, (int)storage);
        insertLarvaObjectStmt.bindLong(6, storageID);
        insertLarvaObjectStmt.bindInt(7, x);
        insertLarvaObjectStmt.bindInt(8, y);
        insertLarvaObjectStmt.bindInt(9, (int)m_Num);
        insertLarvaObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// destroy
//--------------------------------------------------------------------------------
bool Larva::destroy()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // getObjectTableName() returns a fixed per-class literal (see the
        // override in Larva.h), never packet/user input -- PreparedStatement
        // cannot bind an identifier, so the table name stays spliced into the
        // SQL text, matching the batch 2/6/7 table-name precedent. Only
        // ItemID is bound.
        PreparedStatement deleteStmt(pConn, "DELETE FROM " + getObjectTableName() + " WHERE ItemID = ?");
        deleteStmt.bindLong(1, m_ItemID);
        deleteStmt.execute();

        if (deleteStmt.getAffectedRowCount() == 0) {
            return false;
        }

    }
    END_DB(pStmt)

    __END_CATCH

    return true;
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Larva::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveLarvaObjectStmt(pConn, string("UPDATE LarvaObject SET ") + field + " WHERE ItemID=?");
        tinysaveLarvaObjectStmt.bindLong(1, m_ItemID);
        tinysaveLarvaObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Larva::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateLarvaObjectStmt(
            pConn, "UPDATE LarvaObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, Num=? "
                   "WHERE ItemID=?");
        updateLarvaObjectStmt.bindLong(1, m_ObjectID);
        updateLarvaObjectStmt.bindInt(2, m_ItemType);
        updateLarvaObjectStmt.bindString(3, ownerID);
        updateLarvaObjectStmt.bindInt(4, (int)storage);
        updateLarvaObjectStmt.bindLong(5, storageID);
        updateLarvaObjectStmt.bindInt(6, (int)x);
        updateLarvaObjectStmt.bindInt(7, (int)y);
        updateLarvaObjectStmt.bindInt(8, (int)m_Num);
        updateLarvaObjectStmt.bindLong(9, m_ItemID);
        updateLarvaObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Larva::toString() const

{
    StringStream msg;

    msg << "Larva("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Larva::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pLarvaInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Larva::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pLarvaInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Larva::getWeight() const

{
    __BEGIN_TRY

    return g_pLarvaInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

int Larva::getHPAmount(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getHPAmount();

    __END_CATCH
}

int Larva::getMPAmount(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getMPAmount();

    __END_CATCH
}

int Larva::getHPDelay(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getHPDelay();

    __END_CATCH
}

int Larva::getMPDelay(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getMPDelay();

    __END_CATCH
}

int Larva::getHPQuantity(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getHPQuantity();

    __END_CATCH
}

int Larva::getMPQuantity(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getMPQuantity();

    __END_CATCH
}

int Larva::getHPRecoveryUnit(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getHPRecoveryUnit();

    __END_CATCH
}

int Larva::getMPRecoveryUnit(void) const

{
    __BEGIN_TRY

    LarvaInfo* pInfo = dynamic_cast<LarvaInfo*>(g_pLarvaInfoManager->getItemInfo(m_ItemType));
    return pInfo->getMPRecoveryUnit();

    __END_CATCH
}

//--------------------------------------------------------------------------------
// parse effect string
//--------------------------------------------------------------------------------
void LarvaInfo::parseEffect(const string& effect)

{
    __BEGIN_TRY

    m_HPAmount = 0;
    m_HPDelay = 0;
    m_HPRecoveryUnit = 0;
    m_MPAmount = 0;
    m_MPDelay = 0;
    m_MPRecoveryUnit = 0;

    if (effect.size() < 5)
        return;

    size_t a = 0, b = 0, c = 0, d = 0, e = 0;

    while (e < effect.size() - 1) {
        ////////////////////////////////////////////////////////////
        //(HP,+50,2,1)(MP+10)
        // a  b   ca
        ////////////////////////////////////////////////////////////
        a = effect.find_first_of('(', e);
        b = effect.find_first_of(',', a + 1);
        c = effect.find_first_of(',', b + 1);
        d = effect.find_first_of(',', c + 1);
        e = effect.find_first_of(')', d + 1);

        if (a > b || b > c || c > d || d > e)
            break;

        string recover = trim(effect.substr(a + 1, b - a - 1));
        uint amount = atoi(effect.substr(b + 1, c - b - 1).c_str());
        uint delay = atoi(effect.substr(c + 1, d - c - 1).c_str());
        uint unit = atoi(effect.substr(d + 1, e - d - 1).c_str());

        if (recover == "HP") {
            m_HPAmount = (int)amount;
            m_HPDelay = (int)delay;
            m_HPRecoveryUnit = (int)unit;
        } else if (recover == "MP") {
            m_MPAmount = (int)amount;
            m_MPDelay = (int)delay;
            m_MPRecoveryUnit = (int)unit;
        }
    }

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string LarvaInfo::toString() const

{
    StringStream msg;
    msg << "LarvaInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ",HPAmount:" << (int)m_HPAmount << ",MPAmount:" << (int)m_MPAmount
        << ")";
    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void LarvaInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectLarvaInfoStmt(pConn, "SELECT MAX(ItemType) FROM LarvaInfo");
        Result* pResult = selectLarvaInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectLarvaInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Effect FROM LarvaInfo");
        pResult = selectLarvaInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            LarvaInfo* pLarvaInfo = new LarvaInfo();

            pLarvaInfo->setItemType(pResult->getInt(++i));
            pLarvaInfo->setName(pResult->getString(++i));
            pLarvaInfo->setEName(pResult->getString(++i));
            pLarvaInfo->setPrice(pResult->getInt(++i));
            pLarvaInfo->setVolumeType(pResult->getInt(++i));
            pLarvaInfo->setWeight(pResult->getInt(++i));
            pLarvaInfo->setRatio(pResult->getInt(++i));
            pLarvaInfo->parseEffect(pResult->getString(++i));

            addItemInfo(pLarvaInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void LarvaLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectLarvaObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM LarvaObject WHERE OwnerID = "
                   "? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectLarvaObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectLarvaObjectStmt.execute();

        while (pResult->next()) {
            try {
                uint i = 0;

                Larva* pLarva = new Larva();

                pLarva->setItemID(pResult->getDWORD(++i));
                pLarva->setObjectID(pResult->getDWORD(++i));
                pLarva->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pLarva->setNum(pResult->getBYTE(++i));

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
                    if (pInventory->canAddingEx(x, y, pLarva)) {
                        pInventory->addItemEx(x, y, pLarva);
                    } else {
                        processItemBugEx(pCreature, pLarva);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pLarva);
                    break;

                case STORAGE_BELT:
                    if (pCreature->isSlayer()) {
                        pItem = pSlayer->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pLarva)) {
                                pBeltInventory->addItem(x, 0, pLarva);
                            } else {
                                processItemBugEx(pCreature, pLarva);
                            }
                        } else {
                            processItemBugEx(pCreature, pLarva);
                        }
                    } else if (pCreature->isVampire()) {
                        pItem = pVampire->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pLarva)) {
                                pBeltInventory->addItemEx(x, 0, pLarva);
                            } else {
                                processItemBugEx(pCreature, pLarva);
                            }
                        } else {
                            processItemBugEx(pCreature, pLarva);
                        }
                    } else if (pCreature->isOusters()) {
                        pItem = pOusters->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pLarva)) {
                                pBeltInventory->addItemEx(x, 0, pLarva);
                            } else {
                                processItemBugEx(pCreature, pLarva);
                            }
                        } else {
                            processItemBugEx(pCreature, pLarva);
                        }
                    }
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pLarva);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pLarva);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pLarva);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pLarva);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pLarva);
                    } else
                        pStash->insert(x, y, pLarva);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pLarva);
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
void LarvaLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneLarvaObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM LarvaObject WHERE Storage = "
                   "? AND StorageID = ?");
        loadZoneLarvaObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneLarvaObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneLarvaObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Larva* pLarva = new Larva();

            pLarva->setItemID(pResult->getInt(++i));
            pLarva->setObjectID(pResult->getInt(++i));
            pLarva->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pLarva->setNum(pResult->getBYTE(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pLarva);
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
void LarvaLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

LarvaLoader* g_pLarvaLoader = NULL;
