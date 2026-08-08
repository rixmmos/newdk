//////////////////////////////////////////////////////////////////////////////
// Filename    : Skull.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Skull.h"

#include "Belt.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "Ousters.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Vampire.h"

// global variable declaration
SkullInfoManager* g_pSkullInfoManager = NULL;

ItemID_t Skull::m_ItemIDRegistry = 0;
Mutex Skull::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Skull::Skull()

{
    m_ItemType = 0;
    m_Num = 1;
}

Skull::Skull(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    m_ItemType = itemType;
    m_Num = 1;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Skull::Skull() : Invalid item type or option type");
        throw("Skull::Skull() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Skull::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        sql << "INSERT INTO SkullObject "
            << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) VALUES ("
            << m_ItemID << "," << m_ObjectID << ","
            << m_ItemType << ",'" << ownerID << "',"
            << (int)storage << ", " << storageID << ", "
            << (int)x << "," << (int)y << "," << (int)m_Num
            << ")";

        pStmt->executeQueryString(sql.toString());
        */
        PreparedStatement insertSkullStmt(
            pConn, "INSERT INTO SkullObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, "
                   "Num) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertSkullStmt.bindUInt(1, m_ItemID);
        insertSkullStmt.bindUInt(2, m_ObjectID);
        insertSkullStmt.bindUInt(3, m_ItemType);
        insertSkullStmt.bindString(4, ownerID);
        insertSkullStmt.bindInt(5, (int)storage);
        insertSkullStmt.bindUInt(6, storageID);
        insertSkullStmt.bindInt(7, (int)x);
        insertSkullStmt.bindInt(8, (int)y);
        insertSkullStmt.bindInt(9, (int)m_Num);
        insertSkullStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Skull::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveSkullStmt(pConn, string("UPDATE SkullObject SET ") + field + " WHERE ItemID=?");
        tinysaveSkullStmt.bindUInt(1, m_ItemID);
        tinysaveSkullStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Skull::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE SkullObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",Num = " << (int)m_Num
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement saveSkullStmt(pConn, "UPDATE SkullObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                               "StorageID=?, X=?, Y=?, Num=? WHERE ItemID=?");
        saveSkullStmt.bindUInt(1, m_ObjectID);
        saveSkullStmt.bindUInt(2, m_ItemType);
        saveSkullStmt.bindString(3, ownerID);
        saveSkullStmt.bindInt(4, (int)storage);
        saveSkullStmt.bindUInt(5, storageID);
        saveSkullStmt.bindInt(6, (int)x);
        saveSkullStmt.bindInt(7, (int)y);
        saveSkullStmt.bindInt(8, (int)m_Num);
        saveSkullStmt.bindUInt(9, m_ItemID);
        saveSkullStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Skull::toString() const

{
    StringStream msg;
    msg << "Skull("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Num:" << (int)m_Num << ")";
    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Skull::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pSkullInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Skull::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pSkullInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Skull::getWeight() const

{
    __BEGIN_TRY

    return g_pSkullInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string SkullInfo::toString() const

{
    StringStream msg;

    msg << "SkullInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ",ItemLevel:" << (int)m_ItemLevel << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void SkullInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM SkullInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i < m_InfoCount + 1; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectSkullInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, ItemLevel FROM SkullInfo");
        pResult = selectSkullInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            SkullInfo* pSkullInfo = new SkullInfo();

            pSkullInfo->setItemType(pResult->getInt(++i));
            pSkullInfo->setName(pResult->getString(++i));
            pSkullInfo->setEName(pResult->getString(++i));
            pSkullInfo->setPrice(pResult->getInt(++i));
            pSkullInfo->setVolumeType(pResult->getInt(++i));
            pSkullInfo->setWeight(pResult->getInt(++i));
            pSkullInfo->setRatio(pResult->getInt(++i));
            pSkullInfo->setItemLevel(pResult->getInt(++i));

            addItemInfo(pSkullInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void SkullLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM SkullObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectSkullLoaderStmt(pConn,
                                                "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM "
                                                "SkullObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectSkullLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectSkullLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Skull* pSkull = new Skull();

                pSkull->setItemID(pResult->getDWORD(++i));
                pSkull->setObjectID(pResult->getDWORD(++i));
                pSkull->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pSkull->setNum(pResult->getBYTE(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Ousters* pOusters = NULL;
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
                } else if (pCreature->isOusters()) {
                    pOusters = dynamic_cast<Ousters*>(pCreature);
                    pInventory = pOusters->getInventory();
                    pStash = pOusters->getStash();
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pSkull)) {
                        pInventory->addItemEx(x, y, pSkull);
                    } else {
                        processItemBugEx(pCreature, pSkull);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pSkull);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pSkull);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pSkull);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pSkull);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pSkull);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pSkull);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pSkull);
                    } else
                        pStash->insert(x, y, pSkull);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pSkull);
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
void SkullLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneSkullStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM SkullObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneSkullStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneSkullStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneSkullStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Skull* pSkull = new Skull();

            pSkull->setItemID(pResult->getInt(++i));
            pSkull->setObjectID(pResult->getInt(++i));
            pSkull->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pSkull->setNum(pResult->getDWORD(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pSkull);
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
void SkullLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

SkullLoader* g_pSkullLoader = NULL;
