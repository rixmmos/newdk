//////////////////////////////////////////////////////////////////////////////
// Filename    : Bomb.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Bomb.h"

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
BombInfoManager* g_pBombInfoManager = NULL;

ItemID_t Bomb::m_ItemIDRegistry = 0;
Mutex Bomb::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Bomb::Bomb()

    : m_ItemType(0), m_Damage(0) //, m_Dir(0)
{
    m_Num = 1;
}

Bomb::Bomb(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType) {
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Bomb::Bomb() : Invalid item type or option type");
        throw("Bomb::Bomb() : Invalid item type or optionType");
    }
    m_Num = 1;
    m_Damage = 0;
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Bomb::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertBombObjectStmt(
            pConn, "INSERT INTO BombObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?,?)");
        insertBombObjectStmt.bindLong(1, m_ItemID);
        insertBombObjectStmt.bindLong(2, m_ObjectID);
        insertBombObjectStmt.bindInt(3, m_ItemType);
        insertBombObjectStmt.bindString(4, ownerID);
        insertBombObjectStmt.bindInt(5, (int)storage);
        insertBombObjectStmt.bindLong(6, storageID);
        insertBombObjectStmt.bindInt(7, (int)x);
        insertBombObjectStmt.bindInt(8, (int)y);
        insertBombObjectStmt.bindInt(9, (int)m_Num);
        insertBombObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Bomb::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveBombObjectStmt(pConn, string("UPDATE BombObject SET ") + field + " WHERE ItemID=?");
        tinysaveBombObjectStmt.bindLong(1, m_ItemID);
        tinysaveBombObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Bomb::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE BombObject SET "
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

        PreparedStatement updateBombObjectStmt(
            pConn, "UPDATE BombObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, Num=? "
                   "WHERE ItemID=?");
        updateBombObjectStmt.bindLong(1, m_ObjectID);
        updateBombObjectStmt.bindInt(2, m_ItemType);
        updateBombObjectStmt.bindString(3, ownerID);
        updateBombObjectStmt.bindInt(4, (int)storage);
        updateBombObjectStmt.bindLong(5, storageID);
        updateBombObjectStmt.bindInt(6, (int)x);
        updateBombObjectStmt.bindInt(7, (int)y);
        updateBombObjectStmt.bindInt(8, (int)m_Num);
        updateBombObjectStmt.bindLong(9, m_ItemID);
        updateBombObjectStmt.execute();


    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Bomb::toString() const

{
    StringStream msg;

    msg << "Bomb("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ",Num:" << (int)m_Num << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Bomb::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pBombInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Bomb::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pBombInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Bomb::getWeight() const

{
    __BEGIN_TRY

    return g_pBombInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

Damage_t Bomb::getMinDamage() const

{
    __BEGIN_TRY

    return g_pBombInfoManager->getItemInfo(m_ItemType)->getMinDamage();

    __END_CATCH
}

Damage_t Bomb::getMaxDamage() const

{
    __BEGIN_TRY

    return g_pBombInfoManager->getItemInfo(m_ItemType)->getMaxDamage();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string BombInfo::toString() const

{
    StringStream msg;

    msg << "BombInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void BombInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectBombInfoStmt(pConn, "SELECT MAX(ItemType) FROM BombInfo");
        Result* pResult = selectBombInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectBombInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, minDamage, maxDamage FROM BombInfo");
        pResult = selectBombInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            BombInfo* pBombInfo = new BombInfo();

            pBombInfo->setItemType(pResult->getInt(++i));
            pBombInfo->setName(pResult->getString(++i));
            pBombInfo->setEName(pResult->getString(++i));
            pBombInfo->setPrice(pResult->getInt(++i));
            pBombInfo->setVolumeType(pResult->getInt(++i));
            pBombInfo->setWeight(pResult->getInt(++i));
            pBombInfo->setRatio(pResult->getInt(++i));
            pBombInfo->setMinDamage(pResult->getInt(++i));
            pBombInfo->setMaxDamage(pResult->getInt(++i));

            addItemInfo(pBombInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void BombLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM BombObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectBombObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM BombObject WHERE OwnerID = "
                   "? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectBombObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectBombObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Bomb* pBomb = new Bomb();

                pBomb->setItemID(pResult->getDWORD(++i));
                pBomb->setObjectID(pResult->getDWORD(++i));
                pBomb->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pBomb->setNum(pResult->getBYTE(++i));

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
                    if (pInventory->canAddingEx(x, y, pBomb)) {
                        pInventory->addItemEx(x, y, pBomb);
                    } else {
                        processItemBugEx(pCreature, pBomb);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pBomb);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pBomb);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pBomb);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pBomb);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pBomb);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pBomb);
                    } else
                        pStash->insert(x, y, pBomb);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pBomb);
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
void BombLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneBombObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM BombObject WHERE Storage = ? AND "
                   "StorageID = ?");
        loadZoneBombObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneBombObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneBombObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Bomb* pBomb = new Bomb();

            pBomb->setItemID(pResult->getInt(++i));
            pBomb->setObjectID(pResult->getInt(++i));
            pBomb->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pBomb);
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
void BombLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

BombLoader* g_pBombLoader = NULL;
