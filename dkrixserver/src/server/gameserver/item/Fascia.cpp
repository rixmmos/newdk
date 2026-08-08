//////////////////////////////////////////////////////////////////////////////
// Filename    : Fascia.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Fascia.h"

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
FasciaInfoManager* g_pFasciaInfoManager = NULL;

ItemID_t Fascia::m_ItemIDRegistry = 0;
Mutex Fascia::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Fascia::Fascia()

{
    setItemType(0);
}

Fascia::Fascia(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);
    setOptionType(optionType);

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Fascia::Fascia() : Invalid item type or option type");
        throw("Fascia::Fascia() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Fascia::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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


        PreparedStatement insertFasciaObjectStmt(
            pConn, "INSERT INTO FasciaObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertFasciaObjectStmt.bindLong(1, m_ItemID);
        insertFasciaObjectStmt.bindLong(2, m_ObjectID);
        insertFasciaObjectStmt.bindInt(3, getItemType());
        insertFasciaObjectStmt.bindString(4, ownerID);
        insertFasciaObjectStmt.bindInt(5, (int)storage);
        insertFasciaObjectStmt.bindLong(6, storageID);
        insertFasciaObjectStmt.bindInt(7, (int)x);
        insertFasciaObjectStmt.bindInt(8, (int)y);
        insertFasciaObjectStmt.bindString(9, optionField);
        insertFasciaObjectStmt.bindInt(10, (int)getGrade());
        insertFasciaObjectStmt.bindInt(11, (int)m_CreateType);
        insertFasciaObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Fascia::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveFasciaObjectStmt(pConn,
                                                   string("UPDATE FasciaObject SET ") + field + " WHERE ItemID=?");
        tinysaveFasciaObjectStmt.bindLong(1, m_ItemID);
        tinysaveFasciaObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Fascia::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement updateFasciaObjectStmt(
            pConn, "UPDATE FasciaObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        updateFasciaObjectStmt.bindLong(1, m_ObjectID);
        updateFasciaObjectStmt.bindInt(2, getItemType());
        updateFasciaObjectStmt.bindString(3, ownerID);
        updateFasciaObjectStmt.bindInt(4, (int)storage);
        updateFasciaObjectStmt.bindLong(5, storageID);
        updateFasciaObjectStmt.bindInt(6, (int)x);
        updateFasciaObjectStmt.bindInt(7, (int)y);
        updateFasciaObjectStmt.bindString(8, optionField);
        updateFasciaObjectStmt.bindInt(9, getGrade());
        updateFasciaObjectStmt.bindInt(10, (int)getEnchantLevel());
        updateFasciaObjectStmt.bindLong(11, m_ItemID);
        updateFasciaObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Fascia::toString() const

{
    StringStream msg;

    msg << "Fascia("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string FasciaInfo::toString() const

{
    StringStream msg;

    msg << "FasciaInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void FasciaInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectFasciaInfoStmt(pConn, "SELECT MAX(ItemType) FROM FasciaInfo");
        Result* pResult = selectFasciaInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectFasciaInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Defense, Protection, ReqAbility, "
                   "ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, "
                   "DowngradeRatio FROM FasciaInfo");
        pResult = selectFasciaInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            FasciaInfo* pFasciaInfo = new FasciaInfo();

            pFasciaInfo->setItemType(pResult->getInt(++i));
            pFasciaInfo->setName(pResult->getString(++i));
            pFasciaInfo->setEName(pResult->getString(++i));
            pFasciaInfo->setPrice(pResult->getInt(++i));
            pFasciaInfo->setVolumeType(pResult->getInt(++i));
            pFasciaInfo->setWeight(pResult->getInt(++i));
            pFasciaInfo->setRatio(pResult->getInt(++i));
            pFasciaInfo->setDefenseBonus(pResult->getInt(++i));
            pFasciaInfo->setProtectionBonus(pResult->getInt(++i));
            pFasciaInfo->setReqAbility(pResult->getString(++i));
            pFasciaInfo->setItemLevel(pResult->getInt(++i));
            pFasciaInfo->setDefaultOptions(pResult->getString(++i));
            pFasciaInfo->setUpgradeRatio(pResult->getInt(++i));
            pFasciaInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pFasciaInfo->setNextOptionRatio(pResult->getInt(++i));
            pFasciaInfo->setNextItemType(pResult->getInt(++i));
            pFasciaInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pFasciaInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void FasciaLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectFasciaObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Grade, EnchantLevel, "
                   "ItemFlag FROM FasciaObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectFasciaObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectFasciaObjectStmt.execute();

        while (pResult->next()) {
            try {
                uint i = 0;

                Fascia* pFascia = new Fascia();

                pFascia->setItemID(pResult->getDWORD(++i));
                pFascia->setObjectID(pResult->getDWORD(++i));
                pFascia->setItemType(pResult->getDWORD(++i));

                if (g_pFasciaInfoManager->getItemInfo(pFascia->getItemType())->isUnique())
                    pFascia->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pFascia->setOptionType(optionTypes);

                pFascia->setGrade(pResult->getInt(++i));
                pFascia->setEnchantLevel(pResult->getInt(++i));
                pFascia->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pFascia)) {
                        pInventory->addItemEx(x, y, pFascia);
                    } else {
                        processItemBugEx(pCreature, pFascia);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer() || pCreature->isVampire()) {
                        processItemBugEx(pCreature, pFascia);
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pFascia);
                        } else {
                            processItemBugEx(pCreature, pFascia);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pFascia);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pFascia);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pFascia);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pFascia);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pFascia);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pFascia);
                    } else
                        pStash->insert(x, y, pFascia);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pFascia);
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
void FasciaLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void FasciaLoader::load(StorageID_t storageID, Inventory* pInventory)

    {__BEGIN_TRY

         __END_CATCH}

FasciaLoader* g_pFasciaLoader = NULL;
