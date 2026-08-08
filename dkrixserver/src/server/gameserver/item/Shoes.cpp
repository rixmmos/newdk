//////////////////////////////////////////////////////////////////////////////
// Filename    : Shoes.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Shoes.h"

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
ShoesInfoManager* g_pShoesInfoManager = NULL;

ItemID_t Shoes::m_ItemIDRegistry = 0;
Mutex Shoes::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Shoes::Shoes()

{
    setItemType(0);
    setDurability(0);
    // m_EnchantLevel = 0;
}

Shoes::Shoes(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    setItemType(itemType);
    setOptionType(optionType);
    // m_EnchantLevel = 0;
    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Shoes::Shoes() : Invalid item type or option type");
        throw("Shoes::Shoes() : Invalid item type or optionType");
    }
}

//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Shoes::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertShoesStmt(pConn, "INSERT INTO ShoesObject "
                                                 "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                 " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                 " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertShoesStmt.bindUInt(1, m_ItemID);
        insertShoesStmt.bindUInt(2, m_ObjectID);
        insertShoesStmt.bindUInt(3, getItemType());
        insertShoesStmt.bindString(4, ownerID);
        insertShoesStmt.bindInt(5, (int)storage);
        insertShoesStmt.bindUInt(6, storageID);
        insertShoesStmt.bindInt(7, (int)x);
        insertShoesStmt.bindInt(8, (int)y);
        insertShoesStmt.bindString(9, optionField);
        insertShoesStmt.bindUInt(10, getDurability());
        insertShoesStmt.bindInt(11, getGrade());
        insertShoesStmt.bindInt(12, (int)m_CreateType);
        insertShoesStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Shoes::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveShoesStmt(pConn, string("UPDATE ShoesObject SET ") + field + " WHERE ItemID=?");
        tinysaveShoesStmt.bindUInt(1, m_ItemID);
        tinysaveShoesStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Shoes::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE ShoesObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << getItemType
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
        PreparedStatement saveShoesStmt(
            pConn, "UPDATE ShoesObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveShoesStmt.bindUInt(1, m_ObjectID);
        saveShoesStmt.bindUInt(2, getItemType());
        saveShoesStmt.bindString(3, ownerID);
        saveShoesStmt.bindInt(4, (int)storage);
        saveShoesStmt.bindUInt(5, storageID);
        saveShoesStmt.bindInt(6, (int)x);
        saveShoesStmt.bindInt(7, (int)y);
        saveShoesStmt.bindString(8, optionField);
        saveShoesStmt.bindUInt(9, getDurability());
        saveShoesStmt.bindInt(10, getGrade());
        saveShoesStmt.bindInt(11, (int)getEnchantLevel());
        saveShoesStmt.bindUInt(12, m_ItemID);
        saveShoesStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Shoes::toString() const

{
    StringStream msg;

    msg << "Shoes("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Shoes::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pShoesInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Shoes::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pShoesInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Shoes::getWeight() const

{
    __BEGIN_TRY

    return g_pShoesInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t Shoes::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pShoesInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}

Defense_t Shoes::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pShoesInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ShoesInfo::toString() const

{
    StringStream msg;

    msg << "ShoesInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void ShoesInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM ShoesInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectShoesInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM ShoesInfo");
        pResult = selectShoesInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            ShoesInfo* pShoesInfo = new ShoesInfo();

            pShoesInfo->setItemType(pResult->getInt(++i));
            pShoesInfo->setName(pResult->getString(++i));
            pShoesInfo->setEName(pResult->getString(++i));
            pShoesInfo->setPrice(pResult->getInt(++i));
            pShoesInfo->setVolumeType(pResult->getInt(++i));
            pShoesInfo->setWeight(pResult->getInt(++i));
            pShoesInfo->setRatio(pResult->getInt(++i));
            pShoesInfo->setDurability(pResult->getInt(++i));
            pShoesInfo->setDefenseBonus(pResult->getInt(++i));
            pShoesInfo->setProtectionBonus(pResult->getInt(++i));
            pShoesInfo->setReqAbility(pResult->getString(++i));
            pShoesInfo->setItemLevel(pResult->getInt(++i));
            pShoesInfo->setDefaultOptions(pResult->getString(++i));
            pShoesInfo->setUpgradeRatio(pResult->getInt(++i));
            pShoesInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pShoesInfo->setNextOptionRatio(pResult->getInt(++i));
            pShoesInfo->setNextItemType(pResult->getInt(++i));
            pShoesInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pShoesInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void ShoesLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
            << "OptionType, Durability, EnchantLevel FROM ShoesObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectShoesLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM ShoesObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectShoesLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectShoesLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Shoes* pShoes = new Shoes();

                pShoes->setItemID(pResult->getDWORD(++i));
                pShoes->setObjectID(pResult->getDWORD(++i));
                pShoes->setItemType(pResult->getDWORD(++i));

                if (g_pShoesInfoManager->getItemInfo(pShoes->getItemType())->isUnique())
                    pShoes->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pShoes->setOptionType(optionTypes);

                pShoes->setDurability(pResult->getInt(++i));
                pShoes->setGrade(pResult->getInt(++i));
                pShoes->setEnchantLevel(pResult->getInt(++i));
                pShoes->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pShoes)) {
                        pInventory->addItemEx(x, y, pShoes);
                    } else {
                        processItemBugEx(pCreature, pShoes);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pShoes);
                        } else {
                            processItemBugEx(pCreature, pShoes);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pShoes);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pShoes);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pShoes);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pShoes);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pShoes);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pShoes);
                    } else
                        pStash->insert(x, y, pShoes);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pShoes);
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
void ShoesLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneShoesStmt(pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                                                     " OptionType, Durability, EnchantLevel, ItemFlag FROM ShoesObject"
                                                     " WHERE Storage = ? AND StorageID = ?");
        selectZoneShoesStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneShoesStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneShoesStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Shoes* pShoes = new Shoes();

            pShoes->setItemID(pResult->getInt(++i));
            pShoes->setObjectID(pResult->getInt(++i));
            pShoes->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pShoes->setOptionType(optionTypes);

            pShoes->setDurability(pResult->getInt(++i));
            pShoes->setEnchantLevel(pResult->getInt(++i));
            pShoes->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pShoes);
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
void ShoesLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

ShoesLoader* g_pShoesLoader = NULL;
