//////////////////////////////////////////////////////////////////////////////
// Filename    : Key.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Key.h"

#include "Belt.h"
#include "DB.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Vampire.h"

// global variable declaration
KeyInfoManager* g_pKeyInfoManager = NULL;

ItemID_t Key::m_ItemIDRegistry = 0;
Mutex Key::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Key::Key()

    : m_ItemType(0), m_Target(0) {}

Key::Key(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_Target(0) {
    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "Key::Key() : Invalid item type or option type");
        throw("Key::Key() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Key::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        PreparedStatement insertKeyObjectStmt(
            pConn, "INSERT INTO KeyObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Target) "
                   "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertKeyObjectStmt.bindLong(1, m_ItemID);
        insertKeyObjectStmt.bindLong(2, m_ObjectID);
        insertKeyObjectStmt.bindInt(3, m_ItemType);
        insertKeyObjectStmt.bindString(4, ownerID);
        insertKeyObjectStmt.bindInt(5, (int)storage);
        insertKeyObjectStmt.bindLong(6, storageID);
        insertKeyObjectStmt.bindInt(7, (int)x);
        insertKeyObjectStmt.bindInt(8, (int)y);
        insertKeyObjectStmt.bindInt(9, m_Target);
        insertKeyObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Key::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveKeyObjectStmt(pConn, string("UPDATE KeyObject SET ") + field + " WHERE ItemID=?");
        tinysaveKeyObjectStmt.bindLong(1, m_ItemID);
        tinysaveKeyObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Key::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE KeyObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",Target = " << m_Target
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement updateKeyObjectStmt(
            pConn, "UPDATE KeyObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Target=? WHERE ItemID=?");
        updateKeyObjectStmt.bindLong(1, m_ObjectID);
        updateKeyObjectStmt.bindInt(2, m_ItemType);
        updateKeyObjectStmt.bindString(3, ownerID);
        updateKeyObjectStmt.bindInt(4, (int)storage);
        updateKeyObjectStmt.bindLong(5, storageID);
        updateKeyObjectStmt.bindInt(6, (int)x);
        updateKeyObjectStmt.bindInt(7, (int)y);
        updateKeyObjectStmt.bindInt(8, m_Target);
        updateKeyObjectStmt.bindLong(9, m_ItemID);
        updateKeyObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

ItemID_t Key::setNewMotorcycle(Slayer* pSlayer) {
    __BEGIN_TRY

    ItemID_t targetID = 0;


    //	Assert( getTarget() == 0 );
    Assert(pSlayer != NULL);
    Zone* pZone = pSlayer->getZone();
    Assert(pZone != NULL);

    KeyInfo* pKeyInfo = dynamic_cast<KeyInfo*>(g_pItemInfoManager->getItemInfo(getItemClass(), getItemType()));
    Assert(pKeyInfo != NULL);

    list<OptionType_t> option;
    ItemType_t motorcycleType = pKeyInfo->getTargetType();

    if (pKeyInfo->getOptionType() != 0)
        option.push_back(pKeyInfo->getOptionType());

    Item* pMotorcycle = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MOTORCYCLE, motorcycleType, option);
    Assert(pMotorcycle != NULL);
    (pZone->getObjectRegistry()).registerObject(pMotorcycle);

    pMotorcycle->create(pSlayer->getName(), STORAGE_ZONE, pZone->getZoneID(), pSlayer->getX(), pSlayer->getY());
    setTarget(pMotorcycle->getItemID());

    targetID = pMotorcycle->getItemID();

    Statement* pStmt = NULL;
    Result* pResult = NULL;


    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement updateKeyObjectStmt2(pConn, "UPDATE KeyObject SET Target=? WHERE ItemID=?");
        updateKeyObjectStmt2.bindULong(1, targetID);
        updateKeyObjectStmt2.bindULong(2, getItemID());
        pResult = updateKeyObjectStmt2.execute();

    }
    END_DB(pStmt)

    // log
    filelog("motorcycle.txt", "[SetTargetID] Owner = %s, KeyID = %lu, Key's targetID = %lu, MotorcycleID = %lu",
            pSlayer->getName().c_str(), getItemID(), getTarget(), pMotorcycle->getItemID());


    SAFE_DELETE(pMotorcycle);

    return targetID;

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Key::toString() const

{
    StringStream msg;

    msg << "Key("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Target:" << (int)m_Target << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Key::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pKeyInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Key::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pKeyInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Key::getWeight() const

{
    __BEGIN_TRY

    return g_pKeyInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string KeyInfo::toString() const

{
    StringStream msg;

    msg << "KeyInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void KeyInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectKeyInfoStmt(pConn, "SELECT MAX(ItemType) FROM KeyInfo");
        Result* pResult = selectKeyInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectKeyInfoSELECTStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, OptionType, TargetType FROM "
                   "KeyInfoSELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM KeyInfo");
        pResult = selectKeyInfoSELECTStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            KeyInfo* pKeyInfo = new KeyInfo();

            pKeyInfo->setItemType(pResult->getInt(++i));
            pKeyInfo->setName(pResult->getString(++i));
            pKeyInfo->setEName(pResult->getString(++i));
            pKeyInfo->setPrice(pResult->getInt(++i));
            pKeyInfo->setVolumeType(pResult->getInt(++i));
            pKeyInfo->setWeight(pResult->getInt(++i));
            pKeyInfo->setRatio(pResult->getInt(++i));
            pKeyInfo->setOptionType(pResult->getInt(++i));
            pKeyInfo->setTargetType(pResult->getInt(++i));

            addItemInfo(pKeyInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void KeyLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Target FROM KeyObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectKeyObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Target FROM KeyObject WHERE OwnerID "
                   "= ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectKeyObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectKeyObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Key* pKey = new Key();

                pKey->setItemID(pResult->getDWORD(++i));
                pKey->setObjectID(pResult->getDWORD(++i));
                pKey->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pKey->setTarget(pResult->getDWORD(++i));

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
                    if (pInventory->canAddingEx(x, y, pKey)) {
                        pInventory->addItemEx(x, y, pKey);
                    } else {
                        processItemBugEx(pCreature, pKey);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pKey);
                    break;

                case STORAGE_BELT:
                    // processItemBugEx(pCreature, pKey);
                    if (pCreature->isSlayer()) {
                        pItem = pSlayer->findBeltIID(storageID);
                        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BELT) {
                            pBelt = dynamic_cast<Belt*>(pItem);
                            pBeltInventory = pBelt->getInventory();
                            if (pBeltInventory->canAddingEx(x, 0, pKey)) {
                                pBeltInventory->addItem(x, 0, pKey);
                            } else {
                                processItemBugEx(pCreature, pKey);
                            }
                        } else {
                            processItemBugEx(pCreature, pKey);
                        }
                    }
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pKey);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pKey);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pKey);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pKey);
                    } else
                        pStash->insert(x, y, pKey);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pKey);
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
void KeyLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneKeyObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Target FROM KeyObject WHERE Storage "
                   "= ? AND StorageID = ?");
        loadZoneKeyObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneKeyObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneKeyObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Key* pKey = new Key();

            pKey->setItemID(pResult->getInt(++i));
            pKey->setObjectID(pResult->getInt(++i));
            pKey->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pKey->setTarget(pResult->getDWORD(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pKey);
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
void KeyLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

KeyLoader* g_pKeyLoader = NULL;
