//////////////////////////////////////////////////////////////////////////////
// Filename    : Persona.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Persona.h"

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
PersonaInfoManager* g_pPersonaInfoManager = NULL;

ItemID_t Persona::m_ItemIDRegistry = 0;
Mutex Persona::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Persona::Persona()

{
    setItemType(0);
    setDurability(0);
}

Persona::Persona(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);
    setOptionType(optionType);

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Persona::Persona() : Invalid item type or option type");
        throw("Persona::Persona() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Persona::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertPersonaStmt(pConn,
                                             "INSERT INTO PersonaObject "
                                             "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                             " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                             " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertPersonaStmt.bindUInt(1, m_ItemID);
        insertPersonaStmt.bindUInt(2, m_ObjectID);
        insertPersonaStmt.bindUInt(3, getItemType());
        insertPersonaStmt.bindString(4, ownerID);
        insertPersonaStmt.bindInt(5, (int)storage);
        insertPersonaStmt.bindUInt(6, storageID);
        insertPersonaStmt.bindInt(7, (int)x);
        insertPersonaStmt.bindInt(8, (int)y);
        insertPersonaStmt.bindString(9, optionField);
        insertPersonaStmt.bindUInt(10, getDurability());
        insertPersonaStmt.bindInt(11, getGrade());
        insertPersonaStmt.bindInt(12, (int)m_CreateType);
        insertPersonaStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Persona::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysavePersonaStmt(pConn, string("UPDATE PersonaObject SET ") + field + " WHERE ItemID=?");
        tinysavePersonaStmt.bindUInt(1, m_ItemID);
        tinysavePersonaStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Persona::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement savePersonaStmt(
            pConn, "UPDATE PersonaObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        savePersonaStmt.bindUInt(1, m_ObjectID);
        savePersonaStmt.bindUInt(2, getItemType());
        savePersonaStmt.bindString(3, ownerID);
        savePersonaStmt.bindInt(4, (int)storage);
        savePersonaStmt.bindUInt(5, storageID);
        savePersonaStmt.bindInt(6, (int)x);
        savePersonaStmt.bindInt(7, (int)y);
        savePersonaStmt.bindString(8, optionField);
        savePersonaStmt.bindUInt(9, getDurability());
        savePersonaStmt.bindInt(10, getGrade());
        savePersonaStmt.bindInt(11, (int)getEnchantLevel());
        savePersonaStmt.bindUInt(12, m_ItemID);
        savePersonaStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Persona::toString() const

{
    StringStream msg;

    msg << "Persona("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string PersonaInfo::toString() const

{
    StringStream msg;

    msg << "PersonaInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void PersonaInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM PersonaInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectPersonaInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, ReqAbility, "
                   "ItemLevel, DefaultOption, UpgradeCrashPercent, NextOptionRatio, NextItemType FROM PersonaInfo");
        pResult = selectPersonaInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            PersonaInfo* pPersonaInfo = new PersonaInfo();

            pPersonaInfo->setItemType(pResult->getInt(++i));
            pPersonaInfo->setName(pResult->getString(++i));
            pPersonaInfo->setEName(pResult->getString(++i));
            pPersonaInfo->setPrice(pResult->getInt(++i));
            pPersonaInfo->setVolumeType(pResult->getInt(++i));
            pPersonaInfo->setWeight(pResult->getInt(++i));
            pPersonaInfo->setRatio(pResult->getInt(++i));
            pPersonaInfo->setDurability(pResult->getInt(++i));
            pPersonaInfo->setDefenseBonus(pResult->getInt(++i));
            pPersonaInfo->setProtectionBonus(pResult->getInt(++i));
            pPersonaInfo->setReqAbility(pResult->getString(++i));
            pPersonaInfo->setItemLevel(pResult->getInt(++i));
            pPersonaInfo->setDefaultOptions(pResult->getString(++i));
            pPersonaInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pPersonaInfo->setNextOptionRatio(pResult->getInt(++i));
            pPersonaInfo->setNextItemType(pResult->getInt(++i));

            addItemInfo(pPersonaInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void PersonaLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectPersonaLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, EnchantLevel, "
                   "ItemFlag FROM PersonaObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectPersonaLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectPersonaLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Persona* pPersona = new Persona();

                pPersona->setItemID(pResult->getDWORD(++i));
                pPersona->setObjectID(pResult->getDWORD(++i));
                pPersona->setItemType(pResult->getDWORD(++i));

                if (g_pPersonaInfoManager->getItemInfo(pPersona->getItemType())->isUnique())
                    pPersona->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);


                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pPersona->setOptionType(optionTypes);

                pPersona->setDurability(pResult->getInt(++i));
                pPersona->setGrade(pResult->getInt(++i));
                pPersona->setEnchantLevel(pResult->getInt(++i));
                pPersona->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pPersona)) {
                        pInventory->addItemEx(x, y, pPersona);
                    } else {
                        processItemBugEx(pCreature, pPersona);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        processItemBugEx(pCreature, pPersona);
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pPersona);
                        } else {
                            processItemBugEx(pCreature, pPersona);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pPersona);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pPersona);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pPersona);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pPersona);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pPersona);
                    } else
                        pStash->insert(x, y, pPersona);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pPersona);
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
void PersonaLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void PersonaLoader::load(StorageID_t storageID, Inventory* pInventory)

    {__BEGIN_TRY


         __END_CATCH}

PersonaLoader* g_pPersonaLoader = NULL;
