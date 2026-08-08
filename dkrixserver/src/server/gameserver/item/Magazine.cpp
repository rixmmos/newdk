//////////////////////////////////////////////////////////////////////////////
// Filename    : Magazine.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Magazine.h"

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
MagazineInfoManager* g_pMagazineInfoManager = NULL;

ItemID_t Magazine::m_ItemIDRegistry = 0;
Mutex Magazine::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Magazine::Magazine()

    : m_ItemType(0) {}

Magazine::Magazine(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)

    : m_ItemType(itemType), m_Num(Num) {
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Magazine::Magazine() : Invalid item type or option type");
        throw("Magazine::Magazine() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Magazine::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        StringStream sql;

        sql << "INSERT INTO MagazineObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num)"
            << " VALUES("
            << m_ItemID << ", "
            << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', " <<(int)storage << ", " << storageID << ", "
        <<(int)x << ", " <<(int)y << ", " << (int)m_Num << ")";

        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement insertMagazineObjectStmt(
            pConn, "INSERT INTO MagazineObject (ItemID, ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertMagazineObjectStmt.bindLong(1, m_ItemID);
        insertMagazineObjectStmt.bindLong(2, m_ObjectID);
        insertMagazineObjectStmt.bindInt(3, m_ItemType);
        insertMagazineObjectStmt.bindString(4, ownerID);
        insertMagazineObjectStmt.bindInt(5, (int)storage);
        insertMagazineObjectStmt.bindLong(6, storageID);
        insertMagazineObjectStmt.bindInt(7, (int)x);
        insertMagazineObjectStmt.bindInt(8, (int)y);
        insertMagazineObjectStmt.bindInt(9, (int)m_Num);
        insertMagazineObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Magazine::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveMagazineObjectStmt(
            pConn, string("UPDATE MagazineObject SET ") + field + " WHERE ItemID=?");
        tinysaveMagazineObjectStmt.bindLong(1, m_ItemID);
        tinysaveMagazineObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Magazine::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE MagazineObject SET "
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

        PreparedStatement updateMagazineObjectStmt(
            pConn, "UPDATE MagazineObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Num=? WHERE ItemID=?");
        updateMagazineObjectStmt.bindLong(1, m_ObjectID);
        updateMagazineObjectStmt.bindInt(2, m_ItemType);
        updateMagazineObjectStmt.bindString(3, ownerID);
        updateMagazineObjectStmt.bindInt(4, (int)storage);
        updateMagazineObjectStmt.bindLong(5, storageID);
        updateMagazineObjectStmt.bindInt(6, (int)x);
        updateMagazineObjectStmt.bindInt(7, (int)y);
        updateMagazineObjectStmt.bindInt(8, (int)m_Num);
        updateMagazineObjectStmt.bindLong(9, m_ItemID);
        updateMagazineObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Magazine::toString() const

{
    StringStream msg;

    msg << "Magazine("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Magazine::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pMagazineInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Magazine::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pMagazineInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Magazine::getWeight() const

{
    __BEGIN_TRY

    return g_pMagazineInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string MagazineInfo::toString() const

{
    StringStream msg;

    msg << "MagazineInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",maxBullets:" << m_MaxBullets << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void MagazineInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMagazineInfoStmt(pConn, "SELECT MAX(ItemType) FROM MagazineInfo");
        Result* pResult = selectMagazineInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMagazineInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, ItemLevel, MaxBullets, "
                   "MaxSilverBullets, Vivid, GunType-1 FROM MagazineInfo");
        pResult = selectMagazineInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MagazineInfo* pMagazineInfo = new MagazineInfo();

            pMagazineInfo->setItemType(pResult->getInt(++i));
            pMagazineInfo->setName(pResult->getString(++i));
            pMagazineInfo->setEName(pResult->getString(++i));
            pMagazineInfo->setPrice(pResult->getInt(++i));
            pMagazineInfo->setVolumeType(pResult->getInt(++i));
            pMagazineInfo->setWeight(pResult->getInt(++i));
            pMagazineInfo->setRatio(pResult->getInt(++i));
            pMagazineInfo->setItemLevel(pResult->getInt(++i));
            pMagazineInfo->setMaxBullets(pResult->getInt(++i));
            pMagazineInfo->setMaxSilver(pResult->getInt(++i));
            pMagazineInfo->setVivid(pResult->getInt(++i) != 0);
            pMagazineInfo->setGunType((MagazineInfo::GunType)pResult->getInt(++i));

            addItemInfo(pMagazineInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void MagazineLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM MagazineObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectMagazineObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM MagazineObject WHERE "
                   "OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectMagazineObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMagazineObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Magazine* pMagazine = new Magazine();

                pMagazine->setItemID(pResult->getDWORD(++i));
                pMagazine->setObjectID(pResult->getDWORD(++i));
                pMagazine->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pMagazine->setNum(pResult->getBYTE(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
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
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pMagazine)) {
                        pInventory->addItemEx(x, y, pMagazine);
                    } else {
                        processItemBugEx(pCreature, pMagazine);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pMagazine);
                    break;

                case STORAGE_BELT:
                    if (pCreature->isSlayer()) {
                        pItem = pSlayer->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pMagazine)) {
                                pBeltInventory->addItem(x, 0, pMagazine);
                            } else {
                                processItemBugEx(pCreature, pMagazine);
                            }
                        } else {
                            processItemBugEx(pCreature, pMagazine);
                        }
                    } else if (pCreature->isVampire()) {
                        pItem = pVampire->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pMagazine)) {
                                pBeltInventory->addItemEx(x, 0, pMagazine);
                            } else {
                                processItemBugEx(pCreature, pMagazine);
                            }
                        } else {
                            processItemBugEx(pCreature, pMagazine);
                        }
                    }
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pMagazine);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pMagazine);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pMagazine);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pMagazine);
                    } else
                        pStash->insert(x, y, pMagazine);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pMagazine);
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
void MagazineLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneMagazineObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM MagazineObject WHERE "
                   "Storage = ? AND StorageID = ?");
        loadZoneMagazineObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneMagazineObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneMagazineObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Magazine* pMagazine = new Magazine();

            pMagazine->setItemID(pResult->getInt(++i));
            pMagazine->setObjectID(pResult->getInt(++i));
            pMagazine->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pMagazine->setNum(pResult->getBYTE(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pMagazine);
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
void MagazineLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

MagazineLoader* g_pMagazineLoader = NULL;
