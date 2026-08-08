//////////////////////////////////////////////////////////////////////////////
// Filename    : Blade.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

// include files
#include "Blade.h"

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
BladeInfoManager* g_pBladeInfoManager = NULL;

ItemID_t Blade::m_ItemIDRegistry = 0;
Mutex Blade::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Blade::Blade()

{
    setItemType(0);
    setDurability(0);
    setBonusDamage(0);
    setSilver(0);
}

Blade::Blade(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_OptionType(optionType)
{
    setItemType(itemType);
    setOptionType(optionType);
    setBonusDamage(0);
    setSilver(0);

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Blade::Blade() : Invalid item type or option type");
        throw("Blade::Blade() : Invalid item type or optionType");
    }
}

//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Blade::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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


        PreparedStatement insertBladeObjectStmt(
            pConn, "INSERT INTO BladeObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Durability, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertBladeObjectStmt.bindLong(1, m_ItemID);
        insertBladeObjectStmt.bindLong(2, m_ObjectID);
        insertBladeObjectStmt.bindInt(3, getItemType());
        insertBladeObjectStmt.bindString(4, ownerID);
        insertBladeObjectStmt.bindInt(5, (int)storage);
        insertBladeObjectStmt.bindLong(6, storageID);
        insertBladeObjectStmt.bindInt(7, (int)x);
        insertBladeObjectStmt.bindInt(8, (int)y);
        insertBladeObjectStmt.bindString(9, optionField);
        insertBladeObjectStmt.bindInt(10, getDurability());
        insertBladeObjectStmt.bindInt(11, getGrade());
        insertBladeObjectStmt.bindInt(12, (int)m_CreateType);
        insertBladeObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Blade::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveBladeObjectStmt(pConn, string("UPDATE BladeObject SET ") + field + " WHERE ItemID=?");
        tinysaveBladeObjectStmt.bindLong(1, m_ItemID);
        tinysaveBladeObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Blade::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE BladeObject SET "
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
            << ",Silver = " <<(int)m_Silver
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement updateBladeObjectStmt(
            pConn, "UPDATE BladeObject SET ObjectID=?, ItemType=?, OwnerID= ?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, EnchantLevel=?, Silver=?, Grade=? WHERE ItemID=?");
        updateBladeObjectStmt.bindLong(1, m_ObjectID);
        updateBladeObjectStmt.bindInt(2, getItemType());
        updateBladeObjectStmt.bindString(3, ownerID);
        updateBladeObjectStmt.bindInt(4, (int)storage);
        updateBladeObjectStmt.bindLong(5, storageID);
        updateBladeObjectStmt.bindInt(6, (int)x);
        updateBladeObjectStmt.bindInt(7, (int)y);
        updateBladeObjectStmt.bindString(8, optionField);
        updateBladeObjectStmt.bindInt(9, getDurability());
        updateBladeObjectStmt.bindInt(10, (int)getEnchantLevel());
        updateBladeObjectStmt.bindInt(11, (int)getSilver());
        updateBladeObjectStmt.bindInt(12, (int)getGrade());
        updateBladeObjectStmt.bindLong(13, m_ItemID);
        updateBladeObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Blade::toString() const

{
    StringStream msg;

    msg << "Blade("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",Silver:" << (int)getSilver()
        << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Blade::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pBladeInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Blade::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pBladeInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Blade::getWeight() const

{
    __BEGIN_TRY

    return g_pBladeInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's minDamage
//--------------------------------------------------------------------------------
Damage_t Blade::getMinDamage() const

{
    __BEGIN_TRY

    return g_pBladeInfoManager->getItemInfo(m_ItemType)->getMinDamage() + m_BonusDamage;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's maxDamage
//--------------------------------------------------------------------------------
Damage_t Blade::getMaxDamage() const

{
    __BEGIN_TRY

    return g_pBladeInfoManager->getItemInfo(m_ItemType)->getMaxDamage() + m_BonusDamage;

    __END_CATCH
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
int Blade::getCriticalBonus(void) const

{
    __BEGIN_TRY

    return g_pBladeInfoManager->getItemInfo(m_ItemType)->getCriticalBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string BladeInfo::toString() const

{
    StringStream msg;

    msg << "BladeInfo("
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
void BladeInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectBladeInfoStmt(pConn, "SELECT MAX(ItemType) FROM BladeInfo");
        Result* pResult = selectBladeInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectBladeInfoStmt2(
            pConn, "SELECT "
                   "ItemType,Name,EName,Price,Volume,Weight,Ratio,Durability,minDamage,maxDamage,MaxSilver,Speed,ReqAb"
                   "ility,ItemLevel, CriticalBonus, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM BladeInfo");
        pResult = selectBladeInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            BladeInfo* pBladeInfo = new BladeInfo();

            pBladeInfo->setItemType(pResult->getInt(++i));
            pBladeInfo->setName(pResult->getString(++i));
            pBladeInfo->setEName(pResult->getString(++i));
            pBladeInfo->setPrice(pResult->getInt(++i));
            pBladeInfo->setVolumeType(pResult->getInt(++i));
            pBladeInfo->setWeight(pResult->getInt(++i));
            pBladeInfo->setRatio(pResult->getInt(++i));
            pBladeInfo->setDurability(pResult->getInt(++i));
            pBladeInfo->setMinDamage(pResult->getInt(++i));
            pBladeInfo->setMaxDamage(pResult->getInt(++i));
            pBladeInfo->setMaxSilver(pResult->getInt(++i));
            pBladeInfo->setSpeed(pResult->getInt(++i));
            pBladeInfo->setReqAbility(pResult->getString(++i));
            pBladeInfo->setItemLevel(pResult->getInt(++i));
            pBladeInfo->setCriticalBonus(pResult->getInt(++i));
            pBladeInfo->setDefaultOptions(pResult->getString(++i));
            pBladeInfo->setUpgradeRatio(pResult->getInt(++i));
            pBladeInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pBladeInfo->setNextOptionRatio(pResult->getInt(++i));
            pBladeInfo->setNextItemType(pResult->getInt(++i));
            pBladeInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pBladeInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void BladeLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel, Silver FROM BladeObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectBladeObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, "
                   "EnchantLevel, Silver, Grade, ItemFlag FROM BladeObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, "
                   "3, 4, 9)");
        selectBladeObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectBladeObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Blade* pBlade = new Blade();

                pBlade->setItemID(pResult->getDWORD(++i));
                pBlade->setObjectID(pResult->getDWORD(++i));
                pBlade->setItemType(pResult->getDWORD(++i));

                if (g_pBladeInfoManager->getItemInfo(pBlade->getItemType())->isUnique())
                    pBlade->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pBlade->setOptionType(optionTypes);

                pBlade->setDurability(pResult->getInt(++i));
                pBlade->setEnchantLevel(pResult->getInt(++i));
                pBlade->setSilver(pResult->getInt(++i));
                pBlade->setGrade(pResult->getInt(++i));
                pBlade->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pBlade)) {
                        pInventory->addItemEx(x, y, pBlade);
                    } else {
                        processItemBugEx(pCreature, pBlade);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pBlade);
                        } else {
                            processItemBugEx(pCreature, pBlade);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pBlade);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pBlade);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pBlade);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pBlade);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pBlade);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pBlade);
                    } else {
                        pStash->insert(x, y, pBlade);
                    }
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pBlade);
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
void BladeLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneBladeObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, "
                   "EnchantLevel, Silver, ItemFlag FROM BladeObject WHERE Storage = ? AND StorageID = ?");
        loadZoneBladeObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneBladeObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneBladeObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Blade* pBlade = new Blade();

            pBlade->setItemID(pResult->getInt(++i));
            pBlade->setObjectID(pResult->getInt(++i));
            pBlade->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pBlade->setOptionType(optionTypes);

            pBlade->setDurability(pResult->getInt(++i));
            pBlade->setEnchantLevel(pResult->getInt(++i));
            pBlade->setSilver(pResult->getInt(++i));
            pBlade->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pBlade);
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
void BladeLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

BladeLoader* g_pBladeLoader = NULL;
