//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireETC.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "VampireETC.h"

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
VampireETCInfoManager* g_pVampireETCInfoManager = NULL;

ItemID_t VampireETC::m_ItemIDRegistry = 0;
Mutex VampireETC::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
VampireETC::VampireETC()

{
    m_ItemType = 0;
}

VampireETC::VampireETC(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    m_ItemType = itemType;
    m_Num = 1;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "VampireETC::VampireETC() : Invalid item type or option type");
        throw("VampireETC::VampireETC() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void VampireETC::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertVampireETCStmt(
            pConn,
            "INSERT INTO VampireETCObject "
            "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertVampireETCStmt.bindUInt(1, m_ItemID);
        insertVampireETCStmt.bindUInt(2, m_ObjectID);
        insertVampireETCStmt.bindUInt(3, m_ItemType);
        insertVampireETCStmt.bindString(4, ownerID);
        insertVampireETCStmt.bindInt(5, (int)storage);
        insertVampireETCStmt.bindUInt(6, storageID);
        insertVampireETCStmt.bindInt(7, (int)x);
        insertVampireETCStmt.bindInt(8, (int)y);
        insertVampireETCStmt.bindInt(9, (int)m_Num);
        insertVampireETCStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireETC::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveVampireETCStmt(pConn,
                                                 string("UPDATE VampireETCObject SET ") + field + " WHERE ItemID=?");
        tinysaveVampireETCStmt.bindUInt(1, m_ItemID);
        tinysaveVampireETCStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireETC::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE VampireETCObject SET "
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

        PreparedStatement saveVampireETCStmt(
            pConn, "UPDATE VampireETCObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                   "StorageID=?, X=?, Y=?, Num=? WHERE ItemID=?");
        saveVampireETCStmt.bindUInt(1, m_ObjectID);
        saveVampireETCStmt.bindUInt(2, m_ItemType);
        saveVampireETCStmt.bindString(3, ownerID);
        saveVampireETCStmt.bindInt(4, (int)storage);
        saveVampireETCStmt.bindUInt(5, storageID);
        saveVampireETCStmt.bindInt(6, (int)x);
        saveVampireETCStmt.bindInt(7, (int)y);
        saveVampireETCStmt.bindInt(8, (int)m_Num);
        saveVampireETCStmt.bindUInt(9, m_ItemID);
        saveVampireETCStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireETC::toString() const

{
    StringStream msg;
    msg << "VampireETC("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ",Num:" << (int)m_Num << ")";
    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t VampireETC::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pVampireETCInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t VampireETC::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pVampireETCInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t VampireETC::getWeight() const

{
    __BEGIN_TRY

    return g_pVampireETCInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireETCInfo::toString() const

{
    StringStream msg;

    msg << "VampireETCInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void VampireETCInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM VampireETCInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectVampireETCInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, ReqAbility FROM VampireETCInfo");
        pResult = selectVampireETCInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireETCInfo* pVampireETCInfo = new VampireETCInfo();

            pVampireETCInfo->setItemType(pResult->getInt(++i));
            pVampireETCInfo->setName(pResult->getString(++i));
            pVampireETCInfo->setEName(pResult->getString(++i));
            pVampireETCInfo->setPrice(pResult->getInt(++i));
            pVampireETCInfo->setVolumeType(pResult->getInt(++i));
            pVampireETCInfo->setWeight(pResult->getInt(++i));
            pVampireETCInfo->setRatio(pResult->getInt(++i));
            pVampireETCInfo->setReqAbility(pResult->getString(++i));

            addItemInfo(pVampireETCInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void VampireETCLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM VampireETCObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectVampireETCLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM "
                   "VampireETCObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectVampireETCLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectVampireETCLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                VampireETC* pVampireETC = new VampireETC();

                pVampireETC->setItemID(pResult->getDWORD(++i));
                pVampireETC->setObjectID(pResult->getDWORD(++i));
                pVampireETC->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pVampireETC->setNum(pResult->getBYTE(++i));

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
                    if (pInventory->canAddingEx(x, y, pVampireETC)) {
                        pInventory->addItemEx(x, y, pVampireETC);
                    } else {
                        processItemBugEx(pCreature, pVampireETC);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pVampireETC);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pVampireETC);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pVampireETC);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pVampireETC);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pVampireETC);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pVampireETC);
                    } else
                        pStash->insert(x, y, pVampireETC);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pVampireETC);
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
void VampireETCLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneVampireETCStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM VampireETCObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneVampireETCStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneVampireETCStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneVampireETCStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireETC* pVampireETC = new VampireETC();

            pVampireETC->setItemID(pResult->getInt(++i));
            pVampireETC->setObjectID(pResult->getInt(++i));
            pVampireETC->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pVampireETC->setNum(pResult->getBYTE(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pVampireETC);
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
void VampireETCLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

VampireETCLoader* g_pVampireETCLoader = NULL;
