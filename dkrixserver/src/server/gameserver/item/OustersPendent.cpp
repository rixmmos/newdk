//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersPendent.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "OustersPendent.h"

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
OustersPendentInfoManager* g_pOustersPendentInfoManager = NULL;

ItemID_t OustersPendent::m_ItemIDRegistry = 0;
Mutex OustersPendent::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
OustersPendent::OustersPendent()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    // m_EnchantLevel = 0;
}

OustersPendent::OustersPendent(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    // m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "OustersPendent::OustersPendent() : Invalid item type or option type");
        throw("OustersPendent::OustersPendent() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void OustersPendent::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
                            ItemID_t itemID)

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

        PreparedStatement insertOustersPendentStmt(pConn, "INSERT INTO OustersPendentObject "
                                                          "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                          " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                          " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertOustersPendentStmt.bindUInt(1, m_ItemID);
        insertOustersPendentStmt.bindUInt(2, m_ObjectID);
        insertOustersPendentStmt.bindUInt(3, getItemType());
        insertOustersPendentStmt.bindString(4, ownerID);
        insertOustersPendentStmt.bindInt(5, (int)storage);
        insertOustersPendentStmt.bindUInt(6, storageID);
        insertOustersPendentStmt.bindInt(7, (int)x);
        insertOustersPendentStmt.bindInt(8, (int)y);
        insertOustersPendentStmt.bindString(9, optionField);
        insertOustersPendentStmt.bindUInt(10, getDurability());
        insertOustersPendentStmt.bindInt(11, getGrade());
        insertOustersPendentStmt.bindInt(12, (int)m_CreateType);
        insertOustersPendentStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersPendent::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveOustersPendentStmt(pConn, string("UPDATE OustersPendentObject SET ") + field +
                                                                " WHERE ItemID=?");
        tinysaveOustersPendentStmt.bindUInt(1, m_ItemID);
        tinysaveOustersPendentStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersPendent::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE OustersPendentObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",OptionType = " <<(int)m_OptionType
            << ",Durability = " << m_Durability
            << ",EnchantLevel = " <<(int)m_EnchantLevel
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement saveOustersPendentStmt(
            pConn, "UPDATE OustersPendentObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveOustersPendentStmt.bindUInt(1, m_ObjectID);
        saveOustersPendentStmt.bindUInt(2, getItemType());
        saveOustersPendentStmt.bindString(3, ownerID);
        saveOustersPendentStmt.bindInt(4, (int)storage);
        saveOustersPendentStmt.bindUInt(5, storageID);
        saveOustersPendentStmt.bindInt(6, (int)x);
        saveOustersPendentStmt.bindInt(7, (int)y);
        saveOustersPendentStmt.bindString(8, optionField);
        saveOustersPendentStmt.bindUInt(9, getDurability());
        saveOustersPendentStmt.bindInt(10, getGrade());
        saveOustersPendentStmt.bindInt(11, (int)getEnchantLevel());
        saveOustersPendentStmt.bindUInt(12, m_ItemID);
        saveOustersPendentStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersPendent::toString() const

{
    StringStream msg;

    msg << "OustersPendent("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t OustersPendent::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pOustersPendentInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t OustersPendent::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pOustersPendentInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t OustersPendent::getWeight() const

{
    __BEGIN_TRY

    return g_pOustersPendentInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t OustersPendent::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pOustersPendentInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t OustersPendent::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pOustersPendentInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersPendentInfo::toString() const

{
    StringStream msg;

    msg << "OustersPendentInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void OustersPendentInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM OustersPendentInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectOustersPendentInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM OustersPendentInfo");
        pResult = selectOustersPendentInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersPendentInfo* pOustersPendentInfo = new OustersPendentInfo();

            pOustersPendentInfo->setItemType(pResult->getInt(++i));
            pOustersPendentInfo->setName(pResult->getString(++i));
            pOustersPendentInfo->setEName(pResult->getString(++i));
            pOustersPendentInfo->setPrice(pResult->getInt(++i));
            pOustersPendentInfo->setVolumeType(pResult->getInt(++i));
            pOustersPendentInfo->setWeight(pResult->getInt(++i));
            pOustersPendentInfo->setRatio(pResult->getInt(++i));
            pOustersPendentInfo->setDurability(pResult->getInt(++i));
            pOustersPendentInfo->setDefenseBonus(pResult->getInt(++i));
            pOustersPendentInfo->setProtectionBonus(pResult->getInt(++i));
            pOustersPendentInfo->setReqAbility(pResult->getString(++i));
            pOustersPendentInfo->setItemLevel(pResult->getInt(++i));
            pOustersPendentInfo->setDefaultOptions(pResult->getString(++i));
            pOustersPendentInfo->setUpgradeRatio(pResult->getInt(++i));
            pOustersPendentInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pOustersPendentInfo->setNextOptionRatio(pResult->getInt(++i));
            pOustersPendentInfo->setNextItemType(pResult->getInt(++i));
            pOustersPendentInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pOustersPendentInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void OustersPendentLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << "OptionType, Durability, EnchantLevel FROM OustersPendentObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectOustersPendentLoaderStmt(
            pConn,
            "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
            "EnchantLevel, ItemFlag FROM OustersPendentObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectOustersPendentLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectOustersPendentLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                OustersPendent* pOustersPendent = new OustersPendent();

                pOustersPendent->setItemID(pResult->getDWORD(++i));
                pOustersPendent->setObjectID(pResult->getDWORD(++i));
                pOustersPendent->setItemType(pResult->getDWORD(++i));

                if (g_pOustersPendentInfoManager->getItemInfo(pOustersPendent->getItemType())->isUnique())
                    pOustersPendent->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pOustersPendent->setOptionType(optionTypes);

                pOustersPendent->setDurability(pResult->getInt(++i));
                pOustersPendent->setGrade(pResult->getInt(++i));
                pOustersPendent->setEnchantLevel(pResult->getInt(++i));
                pOustersPendent->setCreateType((Item::CreateType)pResult->getInt(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Ousters* pOusters = NULL;
                Motorcycle* pMotorcycle = NULL;
                Inventory* pMotorInventory = NULL;
                Stash* pStash = NULL;

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
                    if (pInventory->canAddingEx(x, y, pOustersPendent)) {
                        pInventory->addItemEx(x, y, pOustersPendent);
                    } else {
                        processItemBugEx(pCreature, pOustersPendent);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer() || pCreature->isVampire()) {
                        processItemBugEx(pCreature, pOustersPendent);
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pOustersPendent);
                        } else {
                            processItemBugEx(pCreature, pOustersPendent);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pOustersPendent);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pOustersPendent);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pOustersPendent);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pOustersPendent);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pOustersPendent);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pOustersPendent);
                    } else
                        pStash->insert(x, y, pOustersPendent);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pOustersPendent);
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
void OustersPendentLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneOustersPendentStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM OustersPendentObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneOustersPendentStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneOustersPendentStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneOustersPendentStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersPendent* pOustersPendent = new OustersPendent();

            pOustersPendent->setItemID(pResult->getInt(++i));
            pOustersPendent->setObjectID(pResult->getInt(++i));
            pOustersPendent->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pOustersPendent->setOptionType(optionTypes);

            pOustersPendent->setDurability(pResult->getInt(++i));
            pOustersPendent->setEnchantLevel(pResult->getInt(++i));
            pOustersPendent->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pOustersPendent);
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
void OustersPendentLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

OustersPendentLoader* g_pOustersPendentLoader = NULL;
