//////////////////////////////////////////////////////////////////////////////
// Filename    : ComposMei.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ComposMei.h"

#include "Belt.h"
#include "CheckedCast.h"
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
#include "ZoneGroupManager.h"

// global variable declaration
ComposMeiInfoManager* g_pComposMeiInfoManager = NULL;

ItemID_t ComposMei::m_ItemIDRegistry = 0;
Mutex ComposMei::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ComposMei::ComposMei()

//: m_ItemType(0)
{
    setItemType(0);
}

ComposMei::ComposMei(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

//: m_ItemType(itemType), m_Num(Num)
{
    setItemType(itemType);
    setNum(Num);
    // cout << "ComposMei::ComposMei(" << getOptionTypeToString(optionType).c_str() << ")" << endl;
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "ComposMei::ComposMei() : Invalid item type or option type");
        throw("ComposMei::ComposMei() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void ComposMei::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        sql << "INSERT INTO ComposMeiObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) VALUES("
            << m_ItemID << ", "
            << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', "
            <<(int)storage << ", " << storageID << ", " <<(int)x << ", " <<(int)y << ", "
            << (int)m_Num << ")";

        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement insertComposMeiObjectStmt(
            pConn, "INSERT INTO ComposMeiObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertComposMeiObjectStmt.bindLong(1, m_ItemID);
        insertComposMeiObjectStmt.bindLong(2, m_ObjectID);
        insertComposMeiObjectStmt.bindInt(3, getItemType());
        insertComposMeiObjectStmt.bindString(4, ownerID);
        insertComposMeiObjectStmt.bindInt(5, (int)storage);
        insertComposMeiObjectStmt.bindLong(6, storageID);
        insertComposMeiObjectStmt.bindInt(7, x);
        insertComposMeiObjectStmt.bindInt(8, y);
        insertComposMeiObjectStmt.bindInt(9, (int)getNum());
        insertComposMeiObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// destroy
//--------------------------------------------------------------------------------
bool ComposMei::destroy()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // getObjectTableName() returns a fixed per-class literal (see the
        // commented-out override in ComposMei.h), never packet/user input --
        // PreparedStatement cannot bind an identifier, so the table name stays
        // spliced into the SQL text, matching the batch 2/6/7 table-name
        // precedent. Only ItemID is bound.
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
void ComposMei::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveComposMeiObjectStmt(pConn, string("UPDATE ComposMeiObject SET ") + field +
                                                                 " WHERE ItemID=?");
        tinysaveComposMeiObjectStmt.bindLong(1, m_ItemID);
        tinysaveComposMeiObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void ComposMei::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateComposMeiObjectStmt(
            pConn, "UPDATE ComposMeiObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateComposMeiObjectStmt.bindLong(1, m_ObjectID);
        updateComposMeiObjectStmt.bindInt(2, getItemType());
        updateComposMeiObjectStmt.bindString(3, ownerID);
        updateComposMeiObjectStmt.bindInt(4, (int)storage);
        updateComposMeiObjectStmt.bindLong(5, storageID);
        updateComposMeiObjectStmt.bindInt(6, (int)x);
        updateComposMeiObjectStmt.bindInt(7, (int)y);
        updateComposMeiObjectStmt.bindInt(8, (int)getNum());
        updateComposMeiObjectStmt.bindLong(9, m_ItemID);
        updateComposMeiObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ComposMei::toString() const

{
    StringStream msg;

    msg << "ComposMei("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType() << ",Num:" << (int)getNum() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t ComposMei::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pComposMeiInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t ComposMei::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pComposMeiInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t ComposMei::getWeight() const

{
    __BEGIN_TRY

    return g_pComposMeiInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}
*/
int ComposMei::getHPAmount(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getHPAmount();

    __END_CATCH
}

int ComposMei::getMPAmount(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getMPAmount();

    __END_CATCH
}

int ComposMei::getHPDelay(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getHPDelay();

    __END_CATCH
}

int ComposMei::getMPDelay(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getMPDelay();

    __END_CATCH
}

int ComposMei::getHPQuantity(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getHPQuantity();

    __END_CATCH
}

int ComposMei::getMPQuantity(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getMPQuantity();

    __END_CATCH
}

int ComposMei::getHPRecoveryUnit(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getHPRecoveryUnit();

    __END_CATCH
}

int ComposMei::getMPRecoveryUnit(void) const

{
    __BEGIN_TRY

    ComposMeiInfo* pInfo = checkedCast<ComposMeiInfo*>(g_pComposMeiInfoManager->getItemInfo(getItemType()));
    return pInfo->getMPRecoveryUnit();

    __END_CATCH
}

//--------------------------------------------------------------------------------
// parse effect string
//--------------------------------------------------------------------------------
void ComposMeiInfo::parseEffect(const string& effect)

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
string ComposMeiInfo::toString() const

{
    StringStream msg;
    msg << "ComposMeiInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ",HPAmount:" << (int)m_HPAmount << ",MPAmount:" << (int)m_MPAmount
        << ")";
    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void ComposMeiInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectComposMeiInfoStmt(pConn, "SELECT MAX(ItemType) FROM ComposMeiInfo");
        Result* pResult = selectComposMeiInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectComposMeiInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Effect FROM ComposMeiInfo");
        pResult = selectComposMeiInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            ComposMeiInfo* pComposMeiInfo = new ComposMeiInfo();

            pComposMeiInfo->setItemType(pResult->getInt(++i));
            pComposMeiInfo->setName(pResult->getString(++i));
            pComposMeiInfo->setEName(pResult->getString(++i));
            pComposMeiInfo->setPrice(pResult->getInt(++i));
            pComposMeiInfo->setVolumeType(pResult->getInt(++i));
            pComposMeiInfo->setWeight(pResult->getInt(++i));
            pComposMeiInfo->setRatio(pResult->getInt(++i));
            pComposMeiInfo->parseEffect(pResult->getString(++i));

            addItemInfo(pComposMeiInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void ComposMeiLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectComposMeiObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM ComposMeiObject WHERE "
                   "OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectComposMeiObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectComposMeiObjectStmt.execute();

        while (pResult->next()) {
            try {
                uint i = 0;

                ComposMei* pComposMei = new ComposMei();

                pComposMei->setItemID(pResult->getDWORD(++i));
                pComposMei->setObjectID(pResult->getDWORD(++i));
                pComposMei->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pComposMei->setNum(pResult->getBYTE(++i));

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

                OustersArmsband* pOustersArmsband = NULL;
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
                    if (pInventory->canAddingEx(x, y, pComposMei)) {
                        pInventory->addItemEx(x, y, pComposMei);
                    } else {
                        processItemBugEx(pCreature, pComposMei);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pComposMei);
                    break;

                case STORAGE_BELT:
                    if (pCreature->isSlayer()) {
                        pItem = pSlayer->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pComposMei)) {
                                pBeltInventory->addItem(x, 0, pComposMei);
                            } else {
                                processItemBugEx(pCreature, pComposMei);
                            }
                        } else {
                            processItemBugEx(pCreature, pComposMei);
                        }
                    } else if (pCreature->isVampire()) {
                        pItem = pVampire->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pComposMei)) {
                                pBeltInventory->addItemEx(x, 0, pComposMei);
                            } else {
                                processItemBugEx(pCreature, pComposMei);
                            }
                        } else {
                            processItemBugEx(pCreature, pComposMei);
                        }
                    } else if (pCreature->isOusters()) {
                        pItem = findItemIID(pOusters, storageID, Item::ITEM_CLASS_OUSTERS_ARMSBAND);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_ARMSBAND) {
                            pOustersArmsband = dynamic_cast<OustersArmsband*>(pItem);
                            pArmsbandInventory = pOustersArmsband->getInventory();
                            if (pArmsbandInventory->canAddingEx(x, 0, pComposMei)) {
                                pArmsbandInventory->addItemEx(x, 0, pComposMei);
                            } else {
                                processItemBugEx(pCreature, pComposMei);
                            }
                        } else {
                            processItemBugEx(pCreature, pComposMei);
                        }
                    }
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pComposMei);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pComposMei);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pComposMei);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pComposMei);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pComposMei);
                    } else
                        pStash->insert(x, y, pComposMei);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pComposMei);
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
void ComposMeiLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneComposMeiObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM ComposMeiObject WHERE "
                   "Storage = ? AND StorageID = ?");
        loadZoneComposMeiObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneComposMeiObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneComposMeiObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            ComposMei* pComposMei = new ComposMei();

            pComposMei->setItemID(pResult->getInt(++i));
            pComposMei->setObjectID(pResult->getInt(++i));
            pComposMei->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pComposMei->setNum(pResult->getBYTE(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pComposMei);
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
void ComposMeiLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

ComposMeiLoader* g_pComposMeiLoader = NULL;
