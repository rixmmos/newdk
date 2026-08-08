//////////////////////////////////////////////////////////////////////////////
// Filename    : ShoulderArmor.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ShoulderArmor.h"

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
ShoulderArmorInfoManager* g_pShoulderArmorInfoManager = NULL;

ItemID_t ShoulderArmor::m_ItemIDRegistry = 0;
Mutex ShoulderArmor::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ShoulderArmor::ShoulderArmor()

{
    setItemType(0);
    setDurability(0);
}

ShoulderArmor::ShoulderArmor(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);
    setOptionType(optionType);
    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "ShoulderArmor::ShoulderArmor() : Invalid item type or option type");
        throw("ShoulderArmor::ShoulderArmor() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void ShoulderArmor::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertShoulderArmorStmt(pConn,
                                                    "INSERT INTO ShoulderArmorObject "
                                                    "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                    " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                    " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertShoulderArmorStmt.bindUInt(1, m_ItemID);
        insertShoulderArmorStmt.bindUInt(2, m_ObjectID);
        insertShoulderArmorStmt.bindUInt(3, getItemType());
        insertShoulderArmorStmt.bindString(4, ownerID);
        insertShoulderArmorStmt.bindInt(5, (int)storage);
        insertShoulderArmorStmt.bindUInt(6, storageID);
        insertShoulderArmorStmt.bindInt(7, (int)x);
        insertShoulderArmorStmt.bindInt(8, (int)y);
        insertShoulderArmorStmt.bindString(9, optionField);
        insertShoulderArmorStmt.bindUInt(10, getDurability());
        insertShoulderArmorStmt.bindInt(11, (int)getGrade());
        insertShoulderArmorStmt.bindInt(12, (int)m_CreateType);
        insertShoulderArmorStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void ShoulderArmor::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveShoulderArmorStmt(
            pConn, string("UPDATE ShoulderArmorObject SET ") + field + " WHERE ItemID=?");
        tinysaveShoulderArmorStmt.bindUInt(1, m_ItemID);
        tinysaveShoulderArmorStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void ShoulderArmor::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement saveShoulderArmorStmt(
            pConn, "UPDATE ShoulderArmorObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveShoulderArmorStmt.bindUInt(1, m_ObjectID);
        saveShoulderArmorStmt.bindUInt(2, getItemType());
        saveShoulderArmorStmt.bindString(3, ownerID);
        saveShoulderArmorStmt.bindInt(4, (int)storage);
        saveShoulderArmorStmt.bindUInt(5, storageID);
        saveShoulderArmorStmt.bindInt(6, (int)x);
        saveShoulderArmorStmt.bindInt(7, (int)y);
        saveShoulderArmorStmt.bindString(8, optionField);
        saveShoulderArmorStmt.bindUInt(9, getDurability());
        saveShoulderArmorStmt.bindInt(10, (int)getGrade());
        saveShoulderArmorStmt.bindInt(11, (int)getEnchantLevel());
        saveShoulderArmorStmt.bindUInt(12, m_ItemID);
        saveShoulderArmorStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ShoulderArmor::toString() const

{
    StringStream msg;

    msg << "ShoulderArmor("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ShoulderArmorInfo::toString() const

{
    StringStream msg;

    msg << "ShoulderArmorInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void ShoulderArmorInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM ShoulderArmorInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectShoulderArmorInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM ShoulderArmorInfo");
        pResult = selectShoulderArmorInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            ShoulderArmorInfo* pShoulderArmorInfo = new ShoulderArmorInfo();

            pShoulderArmorInfo->setItemType(pResult->getInt(++i));
            pShoulderArmorInfo->setName(pResult->getString(++i));
            pShoulderArmorInfo->setEName(pResult->getString(++i));
            pShoulderArmorInfo->setPrice(pResult->getInt(++i));
            pShoulderArmorInfo->setVolumeType(pResult->getInt(++i));
            pShoulderArmorInfo->setWeight(pResult->getInt(++i));
            pShoulderArmorInfo->setRatio(pResult->getInt(++i));
            pShoulderArmorInfo->setDurability(pResult->getInt(++i));
            pShoulderArmorInfo->setDefenseBonus(pResult->getInt(++i));
            pShoulderArmorInfo->setProtectionBonus(pResult->getInt(++i));
            pShoulderArmorInfo->setReqAbility(pResult->getString(++i));
            pShoulderArmorInfo->setItemLevel(pResult->getInt(++i));
            pShoulderArmorInfo->setDefaultOptions(pResult->getString(++i));
            pShoulderArmorInfo->setUpgradeRatio(pResult->getInt(++i));
            pShoulderArmorInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pShoulderArmorInfo->setNextOptionRatio(pResult->getInt(++i));
            pShoulderArmorInfo->setNextItemType(pResult->getInt(++i));
            pShoulderArmorInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pShoulderArmorInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void ShoulderArmorLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectShoulderArmorLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, EnchantLevel, "
                   "ItemFlag FROM ShoulderArmorObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectShoulderArmorLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectShoulderArmorLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                ShoulderArmor* pShoulderArmor = new ShoulderArmor();

                pShoulderArmor->setItemID(pResult->getDWORD(++i));
                pShoulderArmor->setObjectID(pResult->getDWORD(++i));
                pShoulderArmor->setItemType(pResult->getDWORD(++i));

                if (g_pShoulderArmorInfoManager->getItemInfo(pShoulderArmor->getItemType())->isUnique())
                    pShoulderArmor->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pShoulderArmor->setOptionType(optionTypes);

                pShoulderArmor->setDurability(pResult->getInt(++i));
                pShoulderArmor->setGrade(pResult->getInt(++i));
                pShoulderArmor->setEnchantLevel(pResult->getInt(++i));
                pShoulderArmor->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pShoulderArmor)) {
                        pInventory->addItemEx(x, y, pShoulderArmor);
                    } else {
                        processItemBugEx(pCreature, pShoulderArmor);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pShoulderArmor);
                        } else {
                            processItemBugEx(pCreature, pShoulderArmor);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pShoulderArmor);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pShoulderArmor);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pShoulderArmor);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pShoulderArmor);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pShoulderArmor);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pShoulderArmor);
                    } else
                        pStash->insert(x, y, pShoulderArmor);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pShoulderArmor);
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
void ShoulderArmorLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void ShoulderArmorLoader::load(StorageID_t storageID, Inventory* pInventory)

    {__BEGIN_TRY

         __END_CATCH}

ShoulderArmorLoader* g_pShoulderArmorLoader = NULL;
