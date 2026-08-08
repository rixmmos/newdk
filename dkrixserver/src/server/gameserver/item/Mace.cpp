//////////////////////////////////////////////////////////////////////////////
// Filename    : Mace.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Mace.h"

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
MaceInfoManager* g_pMaceInfoManager = NULL;

ItemID_t Mace::m_ItemIDRegistry = 0;
Mutex Mace::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Mace::Mace()

{
    setItemType(0);
    setDurability(0);
    //	m_EnchantLevel = 0;
    setBonusDamage(0);
    setSilver(0);
}

Mace::Mace(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_OptionType( optionType )
{
    setItemType(itemType);
    setOptionType(optionType);
    setBonusDamage(0);
    setSilver(0);

    //	m_EnchantLevel = 0;
    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Mace::Mace() : Invalid item type or option type");
        throw("Mace::Mace() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Mace::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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


        PreparedStatement insertMaceObjectStmt(
            pConn, "INSERT INTO MaceObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Durability, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertMaceObjectStmt.bindLong(1, m_ItemID);
        insertMaceObjectStmt.bindLong(2, m_ObjectID);
        insertMaceObjectStmt.bindInt(3, getItemType());
        insertMaceObjectStmt.bindString(4, ownerID);
        insertMaceObjectStmt.bindInt(5, (int)storage);
        insertMaceObjectStmt.bindLong(6, storageID);
        insertMaceObjectStmt.bindInt(7, (int)x);
        insertMaceObjectStmt.bindInt(8, (int)y);
        insertMaceObjectStmt.bindString(9, optionField);
        insertMaceObjectStmt.bindInt(10, getDurability());
        insertMaceObjectStmt.bindInt(11, (int)getGrade());
        insertMaceObjectStmt.bindInt(12, (int)m_CreateType);
        insertMaceObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Mace::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveMaceObjectStmt(pConn, string("UPDATE MaceObject SET ") + field + " WHERE ItemID=?");
        tinysaveMaceObjectStmt.bindLong(1, m_ItemID);
        tinysaveMaceObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Mace::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE MaceObject SET "
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
        PreparedStatement updateMaceObjectStmt(
            pConn, "UPDATE MaceObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, EnchantLevel=?, Silver=?, Grade=? WHERE ItemID=?");
        updateMaceObjectStmt.bindLong(1, m_ObjectID);
        updateMaceObjectStmt.bindInt(2, getItemType());
        updateMaceObjectStmt.bindString(3, ownerID);
        updateMaceObjectStmt.bindInt(4, (int)storage);
        updateMaceObjectStmt.bindLong(5, storageID);
        updateMaceObjectStmt.bindInt(6, (int)x);
        updateMaceObjectStmt.bindInt(7, (int)y);
        updateMaceObjectStmt.bindString(8, optionField);
        updateMaceObjectStmt.bindInt(9, getDurability());
        updateMaceObjectStmt.bindInt(10, (int)getEnchantLevel());
        updateMaceObjectStmt.bindInt(11, (int)getSilver());
        updateMaceObjectStmt.bindInt(12, (int)getGrade());
        updateMaceObjectStmt.bindLong(13, m_ItemID);
        updateMaceObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Mace::toString() const

{
    StringStream msg;

    msg << "Mace("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",Silver:" << (int)getSilver()
        << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Mace::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pMaceInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Mace::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pMaceInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Mace::getWeight() const

{
    __BEGIN_TRY

    return g_pMaceInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's minDamage
//--------------------------------------------------------------------------------
Damage_t Mace::getMinDamage() const

{
    __BEGIN_TRY

    return g_pMaceInfoManager->getItemInfo(m_ItemType)->getMinDamage() + m_BonusDamage;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's maxDamage
//--------------------------------------------------------------------------------
Damage_t Mace::getMaxDamage() const

{
    __BEGIN_TRY

    return g_pMaceInfoManager->getItemInfo(m_ItemType)->getMaxDamage() + m_BonusDamage;

    __END_CATCH
}

*/
MP_t Mace::getMPBonus() const

{
    __BEGIN_TRY

    return g_pMaceInfoManager->getItemInfo(getItemType())->getMPBonus();

    __END_CATCH
}
/*
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
int Mace::getCriticalBonus(void) const

{
    __BEGIN_TRY

    return g_pMaceInfoManager->getItemInfo(m_ItemType)->getCriticalBonus();

    __END_CATCH
}
*/
//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string MaceInfo::toString() const

{
    StringStream msg;

    msg << "MaceInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",minDamage:" << m_MinDamage << ",maxDamage:" << m_MaxDamage
        << ",MPBonus:" << m_MPBonus << ",ReqAbility:?"
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void MaceInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaceInfoStmt(pConn, "SELECT MAX(ItemType) FROM MaceInfo");
        Result* pResult = selectMaceInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectMaceInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, minDamage, maxDamage, "
                   "MPBonus, MaxSilver, Speed, ReqAbility, ItemLevel, CriticalBonus, DefaultOption, UpgradeRatio, "
                   "UpgradeCrashPercent, NextOptionRatio, NextItemType, DowngradeRatio FROM MaceInfo");
        pResult = selectMaceInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            MaceInfo* pMaceInfo = new MaceInfo();

            pMaceInfo->setItemType(pResult->getInt(++i));
            pMaceInfo->setName(pResult->getString(++i));
            pMaceInfo->setEName(pResult->getString(++i));
            pMaceInfo->setPrice(pResult->getInt(++i));
            pMaceInfo->setVolumeType(pResult->getInt(++i));
            pMaceInfo->setWeight(pResult->getInt(++i));
            pMaceInfo->setRatio(pResult->getInt(++i));
            pMaceInfo->setDurability(pResult->getInt(++i));
            pMaceInfo->setMinDamage(pResult->getInt(++i));
            pMaceInfo->setMaxDamage(pResult->getInt(++i));
            pMaceInfo->setMPBonus(pResult->getInt(++i));
            pMaceInfo->setMaxSilver(pResult->getInt(++i));
            pMaceInfo->setSpeed(pResult->getInt(++i));
            pMaceInfo->setReqAbility(pResult->getString(++i));
            pMaceInfo->setItemLevel(pResult->getInt(++i));
            pMaceInfo->setCriticalBonus(pResult->getInt(++i));
            pMaceInfo->setDefaultOptions(pResult->getString(++i));
            pMaceInfo->setUpgradeRatio(pResult->getInt(++i));
            pMaceInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pMaceInfo->setNextOptionRatio(pResult->getInt(++i));
            pMaceInfo->setNextItemType(pResult->getInt(++i));
            pMaceInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pMaceInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void MaceLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
            << " OptionType, Durability, EnchantLevel, Silver FROM MaceObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectMaceObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, "
                   "EnchantLevel, Silver, Grade, ItemFlag FROM MaceObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, "
                   "3, 4, 9)");
        selectMaceObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectMaceObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Mace* pMace = new Mace();

                pMace->setItemID(pResult->getDWORD(++i));
                pMace->setObjectID(pResult->getDWORD(++i));
                pMace->setItemType(pResult->getDWORD(++i));

                if (g_pMaceInfoManager->getItemInfo(pMace->getItemType())->isUnique())
                    pMace->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pMace->setOptionType(optionTypes);

                pMace->setDurability(pResult->getInt(++i));
                pMace->setEnchantLevel(pResult->getInt(++i));
                pMace->setSilver(pResult->getInt(++i));
                pMace->setGrade(pResult->getInt(++i));
                pMace->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pMace)) {
                        pInventory->addItemEx(x, y, pMace);
                    } else {
                        processItemBugEx(pCreature, pMace);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pMace);
                        } else {
                            processItemBugEx(pCreature, pMace);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pMace);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pMace);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pMace);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pMace);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pMace);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pMace);
                    } else
                        pStash->insert(x, y, pMace);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pMace);
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
void MaceLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneMaceObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, "
                   "EnchantLevel, Silver, ItemFlag FROM MaceObject WHERE Storage = ? AND StorageID = ?");
        loadZoneMaceObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneMaceObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneMaceObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Mace* pMace = new Mace();

            pMace->setItemID(pResult->getInt(++i));
            pMace->setObjectID(pResult->getInt(++i));
            pMace->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pMace->setOptionType(optionTypes);

            pMace->setDurability(pResult->getInt(++i));
            pMace->setEnchantLevel(pResult->getInt(++i));
            pMace->setSilver(pResult->getInt(++i));
            pMace->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pMace);
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
void MaceLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

MaceLoader* g_pMaceLoader = NULL;
