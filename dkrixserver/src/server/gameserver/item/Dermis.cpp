//////////////////////////////////////////////////////////////////////////////
// Filename    : Dermis.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Dermis.h"

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
DermisInfoManager* g_pDermisInfoManager = NULL;

ItemID_t Dermis::m_ItemIDRegistry = 0;
Mutex Dermis::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Dermis::Dermis()

{
    setItemType(0);
}

Dermis::Dermis(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);
    setOptionType(optionType);

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Dermis::Dermis() : Invalid item type or option type");
        throw("Dermis::Dermis() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Dermis::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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


        PreparedStatement insertDermisObjectStmt(
            pConn, "INSERT INTO DermisObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertDermisObjectStmt.bindLong(1, m_ItemID);
        insertDermisObjectStmt.bindLong(2, m_ObjectID);
        insertDermisObjectStmt.bindInt(3, getItemType());
        insertDermisObjectStmt.bindString(4, ownerID);
        insertDermisObjectStmt.bindInt(5, (int)storage);
        insertDermisObjectStmt.bindLong(6, storageID);
        insertDermisObjectStmt.bindInt(7, (int)x);
        insertDermisObjectStmt.bindInt(8, (int)y);
        insertDermisObjectStmt.bindString(9, optionField);
        insertDermisObjectStmt.bindInt(10, getGrade());
        insertDermisObjectStmt.bindInt(11, (int)m_CreateType);
        insertDermisObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Dermis::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveDermisObjectStmt(
            pConn, string("UPDATE DermisObject SET ") + field + " WHERE ItemID=?");
        tinysaveDermisObjectStmt.bindLong(1, m_ItemID);
        tinysaveDermisObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Dermis::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement updateDermisObjectStmt(
            pConn, "UPDATE DermisObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        updateDermisObjectStmt.bindLong(1, m_ObjectID);
        updateDermisObjectStmt.bindInt(2, getItemType());
        updateDermisObjectStmt.bindString(3, ownerID);
        updateDermisObjectStmt.bindInt(4, (int)storage);
        updateDermisObjectStmt.bindLong(5, storageID);
        updateDermisObjectStmt.bindInt(6, (int)x);
        updateDermisObjectStmt.bindInt(7, (int)y);
        updateDermisObjectStmt.bindString(8, optionField);
        updateDermisObjectStmt.bindInt(9, getGrade());
        updateDermisObjectStmt.bindInt(10, (int)getEnchantLevel());
        updateDermisObjectStmt.bindLong(11, m_ItemID);
        updateDermisObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Dermis::toString() const

{
    StringStream msg;

    msg << "Dermis("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string DermisInfo::toString() const

{
    StringStream msg;

    msg << "DermisInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void DermisInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectDermisInfoStmt(pConn, "SELECT MAX(ItemType) FROM DermisInfo");
        Result* pResult = selectDermisInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectDermisInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Defense, Protection, ReqAbility, "
                   "ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, "
                   "DowngradeRatio FROM DermisInfo");
        pResult = selectDermisInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            DermisInfo* pDermisInfo = new DermisInfo();

            pDermisInfo->setItemType(pResult->getInt(++i));
            pDermisInfo->setName(pResult->getString(++i));
            pDermisInfo->setEName(pResult->getString(++i));
            pDermisInfo->setPrice(pResult->getInt(++i));
            pDermisInfo->setVolumeType(pResult->getInt(++i));
            pDermisInfo->setWeight(pResult->getInt(++i));
            pDermisInfo->setRatio(pResult->getInt(++i));
            pDermisInfo->setDefenseBonus(pResult->getInt(++i));
            pDermisInfo->setProtectionBonus(pResult->getInt(++i));
            pDermisInfo->setReqAbility(pResult->getString(++i));
            pDermisInfo->setItemLevel(pResult->getInt(++i));
            pDermisInfo->setDefaultOptions(pResult->getString(++i));
            pDermisInfo->setUpgradeRatio(pResult->getInt(++i));
            pDermisInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pDermisInfo->setNextOptionRatio(pResult->getInt(++i));
            pDermisInfo->setNextItemType(pResult->getInt(++i));
            pDermisInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pDermisInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void DermisLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectDermisObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, Grade, EnchantLevel, "
                   "ItemFlag FROM DermisObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectDermisObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectDermisObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Dermis* pDermis = new Dermis();

                pDermis->setItemID(pResult->getDWORD(++i));
                pDermis->setObjectID(pResult->getDWORD(++i));
                pDermis->setItemType(pResult->getDWORD(++i));

                if (g_pDermisInfoManager->getItemInfo(pDermis->getItemType())->isUnique())
                    pDermis->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pDermis->setOptionType(optionTypes);

                pDermis->setGrade(pResult->getInt(++i));
                pDermis->setEnchantLevel(pResult->getInt(++i));
                pDermis->setCreateType((Item::CreateType)pResult->getInt(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
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
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pDermis)) {
                        pInventory->addItemEx(x, y, pDermis);
                    } else {
                        processItemBugEx(pCreature, pDermis);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        processItemBugEx(pCreature, pDermis);
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pDermis);
                        } else {
                            processItemBugEx(pCreature, pDermis);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pDermis);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pDermis);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pDermis);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pDermis);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pDermis);
                    } else
                        pStash->insert(x, y, pDermis);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pDermis);
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
void DermisLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void DermisLoader::load(StorageID_t storageID, Inventory* pInventory)

    {__BEGIN_TRY

         __END_CATCH}

DermisLoader* g_pDermisLoader = NULL;
