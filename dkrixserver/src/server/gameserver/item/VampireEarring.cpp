//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireEarring.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "VampireEarring.h"

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
VampireEarringInfoManager* g_pVampireEarringInfoManager = NULL;

ItemID_t VampireEarring::m_ItemIDRegistry = 0;
Mutex VampireEarring::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
VampireEarring::VampireEarring()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    // m_EnchantLevel = 0;
}

VampireEarring::VampireEarring(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    // m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "VampireEarring::VampireEarring() : Invalid item type or option type");
        throw("VampireEarring::VampireEarring() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void VampireEarring::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertVampireEarringStmt(pConn, "INSERT INTO VampireEarringObject "
                                                          "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                          " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                          " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertVampireEarringStmt.bindUInt(1, m_ItemID);
        insertVampireEarringStmt.bindUInt(2, m_ObjectID);
        insertVampireEarringStmt.bindUInt(3, getItemType());
        insertVampireEarringStmt.bindString(4, ownerID);
        insertVampireEarringStmt.bindInt(5, (int)storage);
        insertVampireEarringStmt.bindUInt(6, storageID);
        insertVampireEarringStmt.bindInt(7, (int)x);
        insertVampireEarringStmt.bindInt(8, (int)y);
        insertVampireEarringStmt.bindString(9, optionField);
        insertVampireEarringStmt.bindUInt(10, getDurability());
        insertVampireEarringStmt.bindInt(11, getGrade());
        insertVampireEarringStmt.bindInt(12, (int)m_CreateType);
        insertVampireEarringStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireEarring::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveVampireEarringStmt(pConn, string("UPDATE VampireEarringObject SET ") + field +
                                                                " WHERE ItemID=?");
        tinysaveVampireEarringStmt.bindUInt(1, m_ItemID);
        tinysaveVampireEarringStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireEarring::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE VampireEarringObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << getItemType()
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << ",OptionType = " <<(int)getOptionTypeList()
            << ",Durability = " << getDurability()
            << ",EnchantLevel = " <<(int)m_EnchantLevel
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement saveVampireEarringStmt(
            pConn, "UPDATE VampireEarringObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveVampireEarringStmt.bindUInt(1, m_ObjectID);
        saveVampireEarringStmt.bindUInt(2, getItemType());
        saveVampireEarringStmt.bindString(3, ownerID);
        saveVampireEarringStmt.bindInt(4, (int)storage);
        saveVampireEarringStmt.bindUInt(5, storageID);
        saveVampireEarringStmt.bindInt(6, (int)x);
        saveVampireEarringStmt.bindInt(7, (int)y);
        saveVampireEarringStmt.bindString(8, optionField);
        saveVampireEarringStmt.bindUInt(9, getDurability());
        saveVampireEarringStmt.bindInt(10, getGrade());
        saveVampireEarringStmt.bindInt(11, (int)getEnchantLevel());
        saveVampireEarringStmt.bindUInt(12, m_ItemID);
        saveVampireEarringStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireEarring::toString() const

{
    StringStream msg;

    msg << "VampireEarring("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}

/*
//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t VampireEarring::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pVampireEarringInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t VampireEarring::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pVampireEarringInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t VampireEarring::getWeight() const

{
    __BEGIN_TRY

    return g_pVampireEarringInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t VampireEarring::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pVampireEarringInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t VampireEarring::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pVampireEarringInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireEarringInfo::toString() const

{
    StringStream msg;

    msg << "VampireEarringInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void VampireEarringInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT ifnull(MAX(ItemType),0) FROM VampireEarringInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        if (pResult->next()) {
            m_InfoCount = pResult->getInt(1);
        } else {
            m_InfoCount = 0;
        }

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectVampireEarringInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM VampireEarringInfo");
        pResult = selectVampireEarringInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireEarringInfo* pVampireEarringInfo = new VampireEarringInfo();

            pVampireEarringInfo->setItemType(pResult->getInt(++i));
            pVampireEarringInfo->setName(pResult->getString(++i));
            pVampireEarringInfo->setEName(pResult->getString(++i));
            pVampireEarringInfo->setPrice(pResult->getInt(++i));
            pVampireEarringInfo->setVolumeType(pResult->getInt(++i));
            pVampireEarringInfo->setWeight(pResult->getInt(++i));
            pVampireEarringInfo->setRatio(pResult->getInt(++i));
            pVampireEarringInfo->setDurability(pResult->getInt(++i));
            pVampireEarringInfo->setDefenseBonus(pResult->getInt(++i));
            pVampireEarringInfo->setProtectionBonus(pResult->getInt(++i));
            pVampireEarringInfo->setReqAbility(pResult->getString(++i));
            pVampireEarringInfo->setItemLevel(pResult->getInt(++i));
            pVampireEarringInfo->setDefaultOptions(pResult->getString(++i));
            pVampireEarringInfo->setUpgradeRatio(pResult->getInt(++i));
            pVampireEarringInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pVampireEarringInfo->setNextOptionRatio(pResult->getInt(++i));
            pVampireEarringInfo->setNextItemType(pResult->getInt(++i));
            pVampireEarringInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pVampireEarringInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void VampireEarringLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
            << "OptionType, Durability, EnchantLevel FROM VampireEarringObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectVampireEarringLoaderStmt(
            pConn,
            "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, Durability, Grade, "
            "EnchantLevel, ItemFlag FROM VampireEarringObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectVampireEarringLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectVampireEarringLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                VampireEarring* pVampireEarring = new VampireEarring();

                pVampireEarring->setItemID(pResult->getDWORD(++i));
                pVampireEarring->setObjectID(pResult->getDWORD(++i));
                pVampireEarring->setItemType(pResult->getDWORD(++i));

                if (g_pVampireEarringInfoManager->getItemInfo(pVampireEarring->getItemType())->isUnique())
                    pVampireEarring->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pVampireEarring->setOptionType(optionTypes);

                pVampireEarring->setDurability(pResult->getInt(++i));
                pVampireEarring->setGrade(pResult->getInt(++i));
                pVampireEarring->setEnchantLevel(pResult->getInt(++i));
                pVampireEarring->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pVampireEarring)) {
                        pInventory->addItemEx(x, y, pVampireEarring);
                    } else {
                        processItemBugEx(pCreature, pVampireEarring);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        processItemBugEx(pCreature, pVampireEarring);
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pVampireEarring);
                        } else {
                            processItemBugEx(pCreature, pVampireEarring);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pVampireEarring);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pVampireEarring);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pVampireEarring);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pVampireEarring);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pVampireEarring);
                    } else
                        pStash->insert(x, y, pVampireEarring);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pVampireEarring);
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
void VampireEarringLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneVampireEarringStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM VampireEarringObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneVampireEarringStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneVampireEarringStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneVampireEarringStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireEarring* pVampireEarring = new VampireEarring();

            pVampireEarring->setItemID(pResult->getInt(++i));
            pVampireEarring->setObjectID(pResult->getInt(++i));
            pVampireEarring->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pVampireEarring->setOptionType(optionTypes);

            pVampireEarring->setDurability(pResult->getInt(++i));
            pVampireEarring->setEnchantLevel(pResult->getInt(++i));
            pVampireEarring->setCreateType((Item::CreateType)pResult->getInt(++i));


            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pVampireEarring);
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
void VampireEarringLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

VampireEarringLoader* g_pVampireEarringLoader = NULL;
