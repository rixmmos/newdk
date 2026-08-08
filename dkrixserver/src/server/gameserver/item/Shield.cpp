//////////////////////////////////////////////////////////////////////////////
// Filename    : Shield.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Shield.h"

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
ShieldInfoManager* g_pShieldInfoManager = NULL;

ItemID_t Shield::m_ItemIDRegistry = 0;
Mutex Shield::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Shield::Shield()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    // m_EnchantLevel = 0;
}

Shield::Shield(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);

    // m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Shield::Shield() : Invalid item type or option type");
        throw("Shield::Shield() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Shield::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertShieldStmt(pConn,
                                            "INSERT INTO ShieldObject "
                                            "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                            " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                            " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertShieldStmt.bindUInt(1, m_ItemID);
        insertShieldStmt.bindUInt(2, m_ObjectID);
        insertShieldStmt.bindUInt(3, getItemType());
        insertShieldStmt.bindString(4, ownerID);
        insertShieldStmt.bindInt(5, (int)storage);
        insertShieldStmt.bindUInt(6, storageID);
        insertShieldStmt.bindInt(7, (int)x);
        insertShieldStmt.bindInt(8, (int)y);
        insertShieldStmt.bindString(9, optionField);
        insertShieldStmt.bindUInt(10, getDurability());
        insertShieldStmt.bindInt(11, getGrade());
        insertShieldStmt.bindInt(12, (int)m_CreateType);
        insertShieldStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Shield::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveShieldStmt(pConn, string("UPDATE ShieldObject SET ") + field + " WHERE ItemID=?");
        tinysaveShieldStmt.bindUInt(1, m_ItemID);
        tinysaveShieldStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Shield::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE ShieldObject SET "
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
        PreparedStatement saveShieldStmt(
            pConn, "UPDATE ShieldObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveShieldStmt.bindUInt(1, m_ObjectID);
        saveShieldStmt.bindUInt(2, getItemType());
        saveShieldStmt.bindString(3, ownerID);
        saveShieldStmt.bindInt(4, (int)storage);
        saveShieldStmt.bindUInt(5, storageID);
        saveShieldStmt.bindInt(6, (int)x);
        saveShieldStmt.bindInt(7, (int)y);
        saveShieldStmt.bindString(8, optionField);
        saveShieldStmt.bindUInt(9, getDurability());
        saveShieldStmt.bindInt(10, getGrade());
        saveShieldStmt.bindInt(11, (int)getEnchantLevel());
        saveShieldStmt.bindUInt(12, m_ItemID);
        saveShieldStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Shield::toString() const

{
    StringStream msg;

    msg << "Shield("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Shield::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pShieldInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Shield::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pShieldInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Shield::getWeight() const

{
    __BEGIN_TRY

    return g_pShieldInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t Shield::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pShieldInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t Shield::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pShieldInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ShieldInfo::toString() const

{
    StringStream msg;

    msg << "ShieldInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void ShieldInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM ShieldInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectShieldInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM ShieldInfo");
        pResult = selectShieldInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            ShieldInfo* pShieldInfo = new ShieldInfo();

            pShieldInfo->setItemType(pResult->getInt(++i));
            pShieldInfo->setName(pResult->getString(++i));
            pShieldInfo->setEName(pResult->getString(++i));
            pShieldInfo->setPrice(pResult->getInt(++i));
            pShieldInfo->setVolumeType(pResult->getInt(++i));
            pShieldInfo->setWeight(pResult->getInt(++i));
            pShieldInfo->setRatio(pResult->getInt(++i));
            pShieldInfo->setDurability(pResult->getInt(++i));
            pShieldInfo->setDefenseBonus(pResult->getInt(++i));
            pShieldInfo->setProtectionBonus(pResult->getInt(++i));
            pShieldInfo->setReqAbility(pResult->getString(++i));
            pShieldInfo->setItemLevel(pResult->getInt(++i));
            pShieldInfo->setDefaultOptions(pResult->getString(++i));
            pShieldInfo->setUpgradeRatio(pResult->getInt(++i));
            pShieldInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pShieldInfo->setNextOptionRatio(pResult->getInt(++i));
            pShieldInfo->setNextItemType(pResult->getInt(++i));
            pShieldInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pShieldInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void ShieldLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << "OptionType, Durability, EnchantLevel FROM ShieldObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectShieldLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM ShieldObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectShieldLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectShieldLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Shield* pShield = new Shield();

                pShield->setItemID(pResult->getDWORD(++i));
                pShield->setObjectID(pResult->getDWORD(++i));
                pShield->setItemType(pResult->getDWORD(++i));

                if (g_pShieldInfoManager->getItemInfo(pShield->getItemType())->isUnique())
                    pShield->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pShield->setOptionType(optionTypes);

                pShield->setDurability(pResult->getInt(++i));
                pShield->setGrade(pResult->getInt(++i));
                pShield->setEnchantLevel(pResult->getInt(++i));
                pShield->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pShield)) {
                        pInventory->addItemEx(x, y, pShield);
                    } else {
                        processItemBugEx(pCreature, pShield);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pShield);
                        } else {
                            processItemBugEx(pCreature, pShield);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pShield);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pShield);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pShield);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pShield);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pShield);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pShield);
                    } else
                        pStash->insert(x, y, pShield);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pShield);
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
void ShieldLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneShieldStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM ShieldObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneShieldStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneShieldStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneShieldStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Shield* pShield = new Shield();

            pShield->setItemID(pResult->getInt(++i));
            pShield->setObjectID(pResult->getInt(++i));
            pShield->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pShield->setOptionType(optionTypes);

            pShield->setDurability(pResult->getInt(++i));
            pShield->setEnchantLevel(pResult->getInt(++i));
            pShield->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pShield);
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
void ShieldLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

ShieldLoader* g_pShieldLoader = NULL;
