//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireCoat.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "VampireCoat.h"

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
VampireCoatInfoManager* g_pVampireCoatInfoManager = NULL;

ItemID_t VampireCoat::m_ItemIDRegistry = 0;
Mutex VampireCoat::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
VampireCoat::VampireCoat()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    // m_EnchantLevel = 0;
}

VampireCoat::VampireCoat(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    // m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "VampireCoat::VampireCoat() : Invalid item type or option type");
        throw("VampireCoat::VampireCoat() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void VampireCoat::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertVampireCoatStmt(pConn, "INSERT INTO VampireCoatObject "
                                                       "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                       " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                       " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertVampireCoatStmt.bindUInt(1, m_ItemID);
        insertVampireCoatStmt.bindUInt(2, m_ObjectID);
        insertVampireCoatStmt.bindUInt(3, getItemType());
        insertVampireCoatStmt.bindString(4, ownerID);
        insertVampireCoatStmt.bindInt(5, (int)storage);
        insertVampireCoatStmt.bindUInt(6, storageID);
        insertVampireCoatStmt.bindInt(7, (int)x);
        insertVampireCoatStmt.bindInt(8, (int)y);
        insertVampireCoatStmt.bindString(9, optionField);
        insertVampireCoatStmt.bindUInt(10, getDurability());
        insertVampireCoatStmt.bindInt(11, getGrade());
        insertVampireCoatStmt.bindInt(12, (int)m_CreateType);
        insertVampireCoatStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireCoat::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveVampireCoatStmt(pConn,
                                                  string("UPDATE VampireCoatObject SET ") + field + " WHERE ItemID=?");
        tinysaveVampireCoatStmt.bindUInt(1, m_ItemID);
        tinysaveVampireCoatStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireCoat::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE VampireCoatObject SET "
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
        PreparedStatement saveVampireCoatStmt(
            pConn, "UPDATE VampireCoatObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveVampireCoatStmt.bindUInt(1, m_ObjectID);
        saveVampireCoatStmt.bindUInt(2, getItemType());
        saveVampireCoatStmt.bindString(3, ownerID);
        saveVampireCoatStmt.bindInt(4, (int)storage);
        saveVampireCoatStmt.bindUInt(5, storageID);
        saveVampireCoatStmt.bindInt(6, (int)x);
        saveVampireCoatStmt.bindInt(7, (int)y);
        saveVampireCoatStmt.bindString(8, optionField);
        saveVampireCoatStmt.bindUInt(9, getDurability());
        saveVampireCoatStmt.bindInt(10, getGrade());
        saveVampireCoatStmt.bindInt(11, (int)getEnchantLevel());
        saveVampireCoatStmt.bindUInt(12, m_ItemID);
        saveVampireCoatStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireCoat::toString() const

{
    StringStream msg;

    msg << "VampireCoat("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}

/*
//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t VampireCoat::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pVampireCoatInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t VampireCoat::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pVampireCoatInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t VampireCoat::getWeight() const

{
    __BEGIN_TRY

    return g_pVampireCoatInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t VampireCoat::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pVampireCoatInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t VampireCoat::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pVampireCoatInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireCoatInfo::toString() const

{
    StringStream msg;

    msg << "VampireCoatInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void VampireCoatInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM VampireCoatInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectVampireCoatInfoStmt(
            pConn,
            "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, ReqAbility, "
            "ItemLevel, DefaultOption, UpgradeCrashPercent, NextOptionRatio, NextItemType FROM VampireCoatInfo");
        pResult = selectVampireCoatInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireCoatInfo* pVampireCoatInfo = new VampireCoatInfo();

            pVampireCoatInfo->setItemType(pResult->getInt(++i));
            pVampireCoatInfo->setName(pResult->getString(++i));
            pVampireCoatInfo->setEName(pResult->getString(++i));
            pVampireCoatInfo->setPrice(pResult->getInt(++i));
            pVampireCoatInfo->setVolumeType(pResult->getInt(++i));
            pVampireCoatInfo->setWeight(pResult->getInt(++i));
            pVampireCoatInfo->setRatio(pResult->getInt(++i));
            pVampireCoatInfo->setDurability(pResult->getInt(++i));
            pVampireCoatInfo->setDefenseBonus(pResult->getInt(++i));
            pVampireCoatInfo->setProtectionBonus(pResult->getInt(++i));
            pVampireCoatInfo->setReqAbility(pResult->getString(++i));
            pVampireCoatInfo->setItemLevel(pResult->getInt(++i));
            pVampireCoatInfo->setDefaultOptions(pResult->getString(++i));
            pVampireCoatInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pVampireCoatInfo->setNextOptionRatio(pResult->getInt(++i));
            pVampireCoatInfo->setNextItemType(pResult->getInt(++i));

            addItemInfo(pVampireCoatInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void VampireCoatLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
            << " OptionType, Durability, EnchantLevel FROM VampireCoatObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectVampireCoatLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM VampireCoatObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectVampireCoatLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectVampireCoatLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                VampireCoat* pVampireCoat = new VampireCoat();

                pVampireCoat->setItemID(pResult->getDWORD(++i));
                pVampireCoat->setObjectID(pResult->getDWORD(++i));
                pVampireCoat->setItemType(pResult->getDWORD(++i));

                if (g_pVampireCoatInfoManager->getItemInfo(pVampireCoat->getItemType())->isUnique())
                    pVampireCoat->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);


                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pVampireCoat->setOptionType(optionTypes);

                pVampireCoat->setDurability(pResult->getInt(++i));
                pVampireCoat->setGrade(pResult->getInt(++i));
                pVampireCoat->setEnchantLevel(pResult->getInt(++i));
                pVampireCoat->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pVampireCoat)) {
                        pInventory->addItemEx(x, y, pVampireCoat);
                    } else {
                        processItemBugEx(pCreature, pVampireCoat);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        processItemBugEx(pCreature, pVampireCoat);
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pVampireCoat);
                        } else {
                            processItemBugEx(pCreature, pVampireCoat);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pVampireCoat);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pVampireCoat);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pVampireCoat);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pVampireCoat);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pVampireCoat);
                    } else
                        pStash->insert(x, y, pVampireCoat);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pVampireCoat);
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
void VampireCoatLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneVampireCoatStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM VampireCoatObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneVampireCoatStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneVampireCoatStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneVampireCoatStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireCoat* pVampireCoat = new VampireCoat();

            pVampireCoat->setItemID(pResult->getInt(++i));
            pVampireCoat->setObjectID(pResult->getInt(++i));
            pVampireCoat->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pVampireCoat->setOptionType(optionTypes);

            pVampireCoat->setDurability(pResult->getInt(++i));
            pVampireCoat->setEnchantLevel(pResult->getInt(++i));
            pVampireCoat->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pVampireCoat);
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
void VampireCoatLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

VampireCoatLoader* g_pVampireCoatLoader = NULL;
