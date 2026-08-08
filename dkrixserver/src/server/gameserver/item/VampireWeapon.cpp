//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireWeapon.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

// include files
#include "VampireWeapon.h"

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
VampireWeaponInfoManager* g_pVampireWeaponInfoManager = NULL;

ItemID_t VampireWeapon::m_ItemIDRegistry = 0;
Mutex VampireWeapon::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
VampireWeapon::VampireWeapon()

{
    setItemType(0);
    setDurability(0);
    setBonusDamage(0);
}

VampireWeapon::VampireWeapon(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_OptionType( optionType )
{
    setItemType(itemType);
    setOptionType(optionType);
    setBonusDamage(0);

    // m_EnchantLevel = 0;
    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "VampireWeapon::VampireWeapon() : Invalid item type or option type");
        throw("VampireWeapon::VampireWeapon() : Invalid item type or optionType");
    }
}

//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void VampireWeapon::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertVampireWeaponStmt(pConn,
                                                    "INSERT INTO VampireWeaponObject "
                                                    "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                    " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                    " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertVampireWeaponStmt.bindUInt(1, m_ItemID);
        insertVampireWeaponStmt.bindUInt(2, m_ObjectID);
        insertVampireWeaponStmt.bindUInt(3, getItemType());
        insertVampireWeaponStmt.bindString(4, ownerID);
        insertVampireWeaponStmt.bindInt(5, (int)storage);
        insertVampireWeaponStmt.bindUInt(6, storageID);
        insertVampireWeaponStmt.bindInt(7, (int)x);
        insertVampireWeaponStmt.bindInt(8, (int)y);
        insertVampireWeaponStmt.bindString(9, optionField);
        insertVampireWeaponStmt.bindUInt(10, getDurability());
        insertVampireWeaponStmt.bindInt(11, getGrade());
        insertVampireWeaponStmt.bindInt(12, (int)m_CreateType);
        insertVampireWeaponStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireWeapon::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveVampireWeaponStmt(pConn,
                                                      string("UPDATE VampireWeaponObject SET ") + field + " WHERE ItemID=?");
        tinysaveVampireWeaponStmt.bindUInt(1, m_ItemID);
        tinysaveVampireWeaponStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireWeapon::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE VampireWeaponObject SET "
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
        PreparedStatement saveVampireWeaponStmt(
            pConn, "UPDATE VampireWeaponObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveVampireWeaponStmt.bindUInt(1, m_ObjectID);
        saveVampireWeaponStmt.bindUInt(2, getItemType());
        saveVampireWeaponStmt.bindString(3, ownerID);
        saveVampireWeaponStmt.bindInt(4, (int)storage);
        saveVampireWeaponStmt.bindUInt(5, storageID);
        saveVampireWeaponStmt.bindInt(6, (int)x);
        saveVampireWeaponStmt.bindInt(7, (int)y);
        saveVampireWeaponStmt.bindString(8, optionField);
        saveVampireWeaponStmt.bindUInt(9, getDurability());
        saveVampireWeaponStmt.bindInt(10, getGrade());
        saveVampireWeaponStmt.bindInt(11, (int)getEnchantLevel());
        saveVampireWeaponStmt.bindUInt(12, m_ItemID);
        saveVampireWeaponStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireWeapon::toString() const

{
    StringStream msg;

    msg << "VampireWeapon("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t VampireWeapon::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pVampireWeaponInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t VampireWeapon::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pVampireWeaponInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t VampireWeapon::getWeight() const

{
    __BEGIN_TRY

    return g_pVampireWeaponInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's minDamage
//--------------------------------------------------------------------------------
Damage_t VampireWeapon::getMinDamage() const

{
    __BEGIN_TRY

    return g_pVampireWeaponInfoManager->getItemInfo(m_ItemType)->getMinDamage() + m_BonusDamage;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's maxDamage
//--------------------------------------------------------------------------------
Damage_t VampireWeapon::getMaxDamage() const

{
    __BEGIN_TRY

    return g_pVampireWeaponInfoManager->getItemInfo(m_ItemType)->getMaxDamage() + m_BonusDamage;

    __END_CATCH
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
int VampireWeapon::getCriticalBonus(void) const

{
    __BEGIN_TRY

    return g_pVampireWeaponInfoManager->getItemInfo(m_ItemType)->getCriticalBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireWeaponInfo::toString() const

{
    StringStream msg;

    msg << "VampireWeaponInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",minDamage:" << m_MinDamage << ",maxDamage:" << m_MaxDamage
        << ",ReqAbility:"
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void VampireWeaponInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM VampireWeaponInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectVampireWeaponInfoStmt(
            pConn, "SELECT "
                   "ItemType,Name,EName,Price,Volume,Weight,Ratio,Durability,minDamage,maxDamage,Speed,ReqAbility,ItemLevel, "
                   "CriticalBonus, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, "
                   "DowngradeRatio FROM VampireWeaponInfo");
        pResult = selectVampireWeaponInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireWeaponInfo* pVampireWeaponInfo = new VampireWeaponInfo();

            pVampireWeaponInfo->setItemType(pResult->getInt(++i));
            pVampireWeaponInfo->setName(pResult->getString(++i));
            pVampireWeaponInfo->setEName(pResult->getString(++i));
            pVampireWeaponInfo->setPrice(pResult->getInt(++i));
            pVampireWeaponInfo->setVolumeType(pResult->getInt(++i));
            pVampireWeaponInfo->setWeight(pResult->getInt(++i));
            pVampireWeaponInfo->setRatio(pResult->getInt(++i));
            pVampireWeaponInfo->setDurability(pResult->getInt(++i));
            pVampireWeaponInfo->setMinDamage(pResult->getInt(++i));
            pVampireWeaponInfo->setMaxDamage(pResult->getInt(++i));
            pVampireWeaponInfo->setSpeed(pResult->getInt(++i));
            pVampireWeaponInfo->setReqAbility(pResult->getString(++i));
            pVampireWeaponInfo->setItemLevel(pResult->getInt(++i));
            pVampireWeaponInfo->setCriticalBonus(pResult->getInt(++i));
            pVampireWeaponInfo->setDefaultOptions(pResult->getString(++i));
            pVampireWeaponInfo->setUpgradeRatio(pResult->getInt(++i));
            pVampireWeaponInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pVampireWeaponInfo->setNextOptionRatio(pResult->getInt(++i));
            pVampireWeaponInfo->setNextItemType(pResult->getInt(++i));
            pVampireWeaponInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pVampireWeaponInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void VampireWeaponLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM VampireWeaponObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectVampireWeaponLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM VampireWeaponObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectVampireWeaponLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectVampireWeaponLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                VampireWeapon* pVampireWeapon = new VampireWeapon();

                pVampireWeapon->setItemID(pResult->getDWORD(++i));
                pVampireWeapon->setObjectID(pResult->getDWORD(++i));
                pVampireWeapon->setItemType(pResult->getDWORD(++i));

                if (g_pVampireWeaponInfoManager->getItemInfo(pVampireWeapon->getItemType())->isUnique())
                    pVampireWeapon->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pVampireWeapon->setOptionType(optionTypes);

                pVampireWeapon->setDurability(pResult->getInt(++i));
                pVampireWeapon->setGrade(pResult->getInt(++i));
                pVampireWeapon->setEnchantLevel(pResult->getInt(++i));
                pVampireWeapon->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pVampireWeapon)) {
                        pInventory->addItemEx(x, y, pVampireWeapon);
                    } else {
                        processItemBugEx(pCreature, pVampireWeapon);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pVampireWeapon);
                        } else {
                            processItemBugEx(pCreature, pVampireWeapon);
                        }
                    } else if (pCreature->isSlayer()) {
                        processItemBugEx(pCreature, pVampireWeapon);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pVampireWeapon);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pVampireWeapon);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pVampireWeapon);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pVampireWeapon);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pVampireWeapon);
                    } else {
                        pStash->insert(x, y, pVampireWeapon);
                    }
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pVampireWeapon);
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
void VampireWeaponLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneVampireWeaponStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM VampireWeaponObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneVampireWeaponStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneVampireWeaponStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneVampireWeaponStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireWeapon* pVampireWeapon = new VampireWeapon();

            pVampireWeapon->setItemID(pResult->getInt(++i));
            pVampireWeapon->setObjectID(pResult->getInt(++i));
            pVampireWeapon->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pVampireWeapon->setOptionType(optionTypes);

            pVampireWeapon->setDurability(pResult->getInt(++i));
            pVampireWeapon->setEnchantLevel(pResult->getInt(++i));
            pVampireWeapon->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pVampireWeapon);
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
void VampireWeaponLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

VampireWeaponLoader* g_pVampireWeaponLoader = NULL;
