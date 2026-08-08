//////////////////////////////////////////////////////////////////////////////
// Filename    : CoreZap.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CoreZap.h"

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
CoreZapInfoManager* g_pCoreZapInfoManager = NULL;

ItemID_t CoreZap::m_ItemIDRegistry = 0;
Mutex CoreZap::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
CoreZap::CoreZap()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
}

CoreZap::CoreZap(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);
    setOptionType(optionType);

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "CoreZap::CoreZap() : Invalid item type or option type");
        throw("CoreZap::CoreZap() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void CoreZap::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);


        PreparedStatement insertCoreZapObjectStmt(
            pConn, "INSERT INTO CoreZapObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertCoreZapObjectStmt.bindLong(1, m_ItemID);
        insertCoreZapObjectStmt.bindLong(2, m_ObjectID);
        insertCoreZapObjectStmt.bindInt(3, getItemType());
        insertCoreZapObjectStmt.bindString(4, ownerID);
        insertCoreZapObjectStmt.bindInt(5, (int)storage);
        insertCoreZapObjectStmt.bindLong(6, storageID);
        insertCoreZapObjectStmt.bindInt(7, (int)x);
        insertCoreZapObjectStmt.bindInt(8, (int)y);
        insertCoreZapObjectStmt.bindString(9, optionField);
        insertCoreZapObjectStmt.bindInt(10, getGrade());
        insertCoreZapObjectStmt.bindInt(11, (int)m_CreateType);
        insertCoreZapObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void CoreZap::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveCoreZapObjectStmt(
            pConn, string("UPDATE CoreZapObject SET ") + field + " WHERE ItemID=?");
        tinysaveCoreZapObjectStmt.bindLong(1, m_ItemID);
        tinysaveCoreZapObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void CoreZap::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE CoreZapObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",OptionType = " <<(int)m_OptionType
            << ",Durability = " << m_Durability
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement updateCoreZapObjectStmt(
            pConn, "UPDATE CoreZapObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Grade=? WHERE ItemID=?");
        updateCoreZapObjectStmt.bindLong(1, m_ObjectID);
        updateCoreZapObjectStmt.bindInt(2, getItemType());
        updateCoreZapObjectStmt.bindString(3, ownerID);
        updateCoreZapObjectStmt.bindInt(4, (int)storage);
        updateCoreZapObjectStmt.bindLong(5, storageID);
        updateCoreZapObjectStmt.bindInt(6, (int)x);
        updateCoreZapObjectStmt.bindInt(7, (int)y);
        updateCoreZapObjectStmt.bindString(8, optionField);
        updateCoreZapObjectStmt.bindInt(9, getGrade());
        updateCoreZapObjectStmt.bindLong(10, m_ItemID);
        updateCoreZapObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string CoreZap::toString() const

{
    StringStream msg;

    msg << "CoreZap("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t CoreZap::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pCoreZapInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t CoreZap::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pCoreZapInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t CoreZap::getWeight() const

{
    __BEGIN_TRY

    return g_pCoreZapInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t CoreZap::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pCoreZapInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t CoreZap::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pCoreZapInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string CoreZapInfo::toString() const

{
    StringStream msg;

    msg << "CoreZapInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void CoreZapInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectCoreZapInfoStmt(pConn, "SELECT MAX(ItemType) FROM CoreZapInfo");
        Result* pResult = selectCoreZapInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectCoreZapInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, OptionClass FROM CoreZapInfo");
        pResult = selectCoreZapInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            CoreZapInfo* pCoreZapInfo = new CoreZapInfo();

            pCoreZapInfo->setItemType(pResult->getInt(++i));
            pCoreZapInfo->setName(pResult->getString(++i));
            pCoreZapInfo->setEName(pResult->getString(++i));
            pCoreZapInfo->setPrice(pResult->getInt(++i));
            pCoreZapInfo->setVolumeType(pResult->getInt(++i));
            pCoreZapInfo->setWeight(pResult->getInt(++i));
            pCoreZapInfo->setRatio(pResult->getInt(++i));
            pCoreZapInfo->setOptionClass((OptionClass)pResult->getInt(++i));

            addItemInfo(pCoreZapInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void CoreZapLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM CoreZapObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectCoreZapObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Grade, ItemFlag FROM "
                   "CoreZapObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectCoreZapObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectCoreZapObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                CoreZap* pCoreZap = new CoreZap();

                pCoreZap->setItemID(pResult->getDWORD(++i));
                pCoreZap->setObjectID(pResult->getDWORD(++i));
                pCoreZap->setItemType(pResult->getDWORD(++i));

                if (g_pCoreZapInfoManager->getItemInfo(pCoreZap->getItemType())->isUnique())
                    pCoreZap->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pCoreZap->setOptionType(optionTypes);

                pCoreZap->setGrade(pResult->getInt(++i));
                pCoreZap->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pCoreZap)) {
                        pInventory->addItemEx(x, y, pCoreZap);
                    } else {
                        processItemBugEx(pCreature, pCoreZap);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pCoreZap);
                        } else {
                            processItemBugEx(pCreature, pCoreZap);
                        }
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pCoreZap);
                        } else {
                            processItemBugEx(pCreature, pCoreZap);
                        }
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pCoreZap);
                        } else {
                            processItemBugEx(pCreature, pCoreZap);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pCoreZap);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pCoreZap);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pCoreZap);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pCoreZap);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pCoreZap);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pCoreZap);
                    } else
                        pStash->insert(x, y, pCoreZap);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pCoreZap);
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
void CoreZapLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneCoreZapObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, ItemFlag FROM "
                   "CoreZapObject WHERE Storage = ? AND StorageID = ?");
        loadZoneCoreZapObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneCoreZapObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneCoreZapObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            CoreZap* pCoreZap = new CoreZap();

            pCoreZap->setItemID(pResult->getInt(++i));
            pCoreZap->setObjectID(pResult->getInt(++i));
            pCoreZap->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pCoreZap->setOptionType(optionTypes);

            pCoreZap->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pCoreZap);
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
void CoreZapLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

CoreZapLoader* g_pCoreZapLoader = NULL;
