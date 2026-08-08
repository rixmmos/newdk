//////////////////////////////////////////////////////////////////////////////
// Filename    : Mitten.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Mitten.h"

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
MittenInfoManager* g_pMittenInfoManager = NULL;

ItemID_t Mitten::m_ItemIDRegistry = 0;
Mutex Mitten::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Mitten::Mitten()

{
    setItemType(0);
    setDurability(0);
}

Mitten::Mitten(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);
    setOptionType(optionType);

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Mitten::Mitten() : Invalid item type or option type");
        throw("Mitten::Mitten() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Mitten::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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


        PreparedStatement insertMittenObjectStmt(
            pConn, "INSERT INTO MittenObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Durability, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertMittenObjectStmt.bindLong(1, m_ItemID);
        insertMittenObjectStmt.bindLong(2, m_ObjectID);
        insertMittenObjectStmt.bindInt(3, getItemType());
        insertMittenObjectStmt.bindString(4, ownerID);
        insertMittenObjectStmt.bindInt(5, (int)storage);
        insertMittenObjectStmt.bindLong(6, storageID);
        insertMittenObjectStmt.bindInt(7, (int)x);
        insertMittenObjectStmt.bindInt(8, (int)y);
        insertMittenObjectStmt.bindString(9, optionField);
        insertMittenObjectStmt.bindInt(10, getDurability());
        insertMittenObjectStmt.bindInt(11, getGrade());
        insertMittenObjectStmt.bindInt(12, (int)m_CreateType);
        insertMittenObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Mitten::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveMittenObjectStmt(
            pConn, string("UPDATE MittenObject SET ") + field + " WHERE ItemID=?");
        tinysaveMittenObjectStmt.bindLong(1, m_ItemID);
        tinysaveMittenObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Mitten::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement updateMittenObjectStmt(
            pConn, "UPDATE MittenObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        updateMittenObjectStmt.bindLong(1, m_ObjectID);
        updateMittenObjectStmt.bindInt(2, getItemType());
        updateMittenObjectStmt.bindString(3, ownerID);
        updateMittenObjectStmt.bindInt(4, (int)storage);
        updateMittenObjectStmt.bindLong(5, storageID);
        updateMittenObjectStmt.bindInt(6, (int)x);
        updateMittenObjectStmt.bindInt(7, (int)y);
        updateMittenObjectStmt.bindString(8, optionField);
        updateMittenObjectStmt.bindInt(9, getDurability());
        updateMittenObjectStmt.bindInt(10, getGrade());
        updateMittenObjectStmt.bindInt(11, (int)getEnchantLevel());
        updateMittenObjectStmt.bindLong(12, m_ItemID);
        updateMittenObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Mitten::toString() const

{
    StringStream msg;

    msg << "Mitten("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string MittenInfo::toString() const

{
    StringStream msg;

    msg << "MittenInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void MittenInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMittenInfoStmt(pConn, "SELECT MAX(ItemType) FROM MittenInfo");
        Result* pResult = selectMittenInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMittenInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, "
                   "ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, "
                   "NextItemType, DowngradeRatio FROM MittenInfo");
        pResult = selectMittenInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MittenInfo* pMittenInfo = new MittenInfo();

            pMittenInfo->setItemType(pResult->getInt(++i));
            pMittenInfo->setName(pResult->getString(++i));
            pMittenInfo->setEName(pResult->getString(++i));
            pMittenInfo->setPrice(pResult->getInt(++i));
            pMittenInfo->setVolumeType(pResult->getInt(++i));
            pMittenInfo->setWeight(pResult->getInt(++i));
            pMittenInfo->setRatio(pResult->getInt(++i));
            pMittenInfo->setDurability(pResult->getInt(++i));
            pMittenInfo->setDefenseBonus(pResult->getInt(++i));
            pMittenInfo->setProtectionBonus(pResult->getInt(++i));
            pMittenInfo->setReqAbility(pResult->getString(++i));
            pMittenInfo->setItemLevel(pResult->getInt(++i));
            pMittenInfo->setDefaultOptions(pResult->getString(++i));
            pMittenInfo->setUpgradeRatio(pResult->getInt(++i));
            pMittenInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pMittenInfo->setNextOptionRatio(pResult->getInt(++i));
            pMittenInfo->setNextItemType(pResult->getInt(++i));
            pMittenInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pMittenInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void MittenLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMittenObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM MittenObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectMittenObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMittenObjectStmt.execute();

        while (pResult->next()) {
            try {
                uint i = 0;

                Mitten* pMitten = new Mitten();

                pMitten->setItemID(pResult->getDWORD(++i));
                pMitten->setObjectID(pResult->getDWORD(++i));
                pMitten->setItemType(pResult->getDWORD(++i));

                if (g_pMittenInfoManager->getItemInfo(pMitten->getItemType())->isUnique())
                    pMitten->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pMitten->setOptionType(optionTypes);

                pMitten->setDurability(pResult->getInt(++i));
                pMitten->setGrade(pResult->getInt(++i));
                pMitten->setEnchantLevel(pResult->getInt(++i));
                pMitten->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pMitten)) {
                        pInventory->addItemEx(x, y, pMitten);
                    } else {
                        processItemBugEx(pCreature, pMitten);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer() || pCreature->isVampire()) {
                        processItemBugEx(pCreature, pMitten);
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pMitten);
                        } else {
                            processItemBugEx(pCreature, pMitten);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pMitten);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pMitten);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pMitten);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pMitten);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pMitten);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pMitten);
                    } else
                        pStash->insert(x, y, pMitten);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pMitten);
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
void MittenLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void MittenLoader::load(StorageID_t storageID, Inventory* pInventory)

    {__BEGIN_TRY

         __END_CATCH}

MittenLoader* g_pMittenLoader = NULL;
