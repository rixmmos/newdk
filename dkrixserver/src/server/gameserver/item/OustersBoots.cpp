//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersBoots.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "OustersBoots.h"

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
OustersBootsInfoManager* g_pOustersBootsInfoManager = NULL;

ItemID_t OustersBoots::m_ItemIDRegistry = 0;
Mutex OustersBoots::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
OustersBoots::OustersBoots()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    //	m_EnchantLevel = 0;
}

OustersBoots::OustersBoots(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    //	m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "OustersBoots::OustersBoots() : Invalid item type or option type");
        throw("OustersBoots::OustersBoots() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void OustersBoots::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertOustersBootsStmt(pConn, "INSERT INTO OustersBootsObject "
                                                        "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                        " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                        " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertOustersBootsStmt.bindUInt(1, m_ItemID);
        insertOustersBootsStmt.bindUInt(2, m_ObjectID);
        insertOustersBootsStmt.bindUInt(3, getItemType());
        insertOustersBootsStmt.bindString(4, ownerID);
        insertOustersBootsStmt.bindInt(5, (int)storage);
        insertOustersBootsStmt.bindUInt(6, storageID);
        insertOustersBootsStmt.bindInt(7, (int)x);
        insertOustersBootsStmt.bindInt(8, (int)y);
        insertOustersBootsStmt.bindString(9, optionField);
        insertOustersBootsStmt.bindUInt(10, getDurability());
        insertOustersBootsStmt.bindInt(11, getGrade());
        insertOustersBootsStmt.bindInt(12, (int)m_CreateType);
        insertOustersBootsStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersBoots::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveOustersBootsStmt(pConn, string("UPDATE OustersBootsObject SET ") + field +
                                                              " WHERE ItemID=?");
        tinysaveOustersBootsStmt.bindUInt(1, m_ItemID);
        tinysaveOustersBootsStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersBoots::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE OustersBootsObject SET "
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
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        string optionField;
        setOptionTypeToField(getOptionTypeList(), optionField);
        PreparedStatement saveOustersBootsStmt(
            pConn, "UPDATE OustersBootsObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveOustersBootsStmt.bindUInt(1, m_ObjectID);
        saveOustersBootsStmt.bindUInt(2, getItemType());
        saveOustersBootsStmt.bindString(3, ownerID);
        saveOustersBootsStmt.bindInt(4, (int)storage);
        saveOustersBootsStmt.bindUInt(5, storageID);
        saveOustersBootsStmt.bindInt(6, (int)x);
        saveOustersBootsStmt.bindInt(7, (int)y);
        saveOustersBootsStmt.bindString(8, optionField);
        saveOustersBootsStmt.bindUInt(9, getDurability());
        saveOustersBootsStmt.bindInt(10, getGrade());
        saveOustersBootsStmt.bindInt(11, (int)getEnchantLevel());
        saveOustersBootsStmt.bindUInt(12, m_ItemID);
        saveOustersBootsStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersBoots::toString() const

{
    StringStream msg;

    msg << "OustersBoots("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t OustersBoots::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pOustersBootsInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t OustersBoots::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pOustersBootsInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t OustersBoots::getWeight() const

{
    __BEGIN_TRY

    return g_pOustersBootsInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t OustersBoots::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pOustersBootsInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t OustersBoots::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pOustersBootsInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersBootsInfo::toString() const

{
    StringStream msg;

    msg << "OustersBootsInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void OustersBootsInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM OustersBootsInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectOustersBootsInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM OustersBootsInfo");
        pResult = selectOustersBootsInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersBootsInfo* pOustersBootsInfo = new OustersBootsInfo();

            pOustersBootsInfo->setItemType(pResult->getInt(++i));
            pOustersBootsInfo->setName(pResult->getString(++i));
            pOustersBootsInfo->setEName(pResult->getString(++i));
            pOustersBootsInfo->setPrice(pResult->getInt(++i));
            pOustersBootsInfo->setVolumeType(pResult->getInt(++i));
            pOustersBootsInfo->setWeight(pResult->getInt(++i));
            pOustersBootsInfo->setRatio(pResult->getInt(++i));
            pOustersBootsInfo->setDurability(pResult->getInt(++i));
            pOustersBootsInfo->setDefenseBonus(pResult->getInt(++i));
            pOustersBootsInfo->setProtectionBonus(pResult->getInt(++i));
            pOustersBootsInfo->setReqAbility(pResult->getString(++i));
            pOustersBootsInfo->setItemLevel(pResult->getInt(++i));
            pOustersBootsInfo->setDefaultOptions(pResult->getString(++i));
            pOustersBootsInfo->setUpgradeRatio(pResult->getInt(++i));
            pOustersBootsInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pOustersBootsInfo->setNextOptionRatio(pResult->getInt(++i));
            pOustersBootsInfo->setNextItemType(pResult->getInt(++i));
            pOustersBootsInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pOustersBootsInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void OustersBootsLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << "OptionType, Durability, EnchantLevel FROM OustersBootsObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectOustersBootsLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM OustersBootsObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectOustersBootsLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectOustersBootsLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                OustersBoots* pOustersBoots = new OustersBoots();

                pOustersBoots->setItemID(pResult->getDWORD(++i));
                pOustersBoots->setObjectID(pResult->getDWORD(++i));
                pOustersBoots->setItemType(pResult->getDWORD(++i));

                if (g_pOustersBootsInfoManager->getItemInfo(pOustersBoots->getItemType())->isUnique())
                    pOustersBoots->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pOustersBoots->setOptionType(optionTypes);

                pOustersBoots->setDurability(pResult->getInt(++i));
                pOustersBoots->setGrade(pResult->getInt(++i));
                pOustersBoots->setEnchantLevel(pResult->getInt(++i));
                pOustersBoots->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pOustersBoots)) {
                        pInventory->addItemEx(x, y, pOustersBoots);
                    } else {
                        processItemBugEx(pCreature, pOustersBoots);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer() || pCreature->isVampire()) {
                        processItemBugEx(pCreature, pOustersBoots);
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pOustersBoots);
                        } else {
                            processItemBugEx(pCreature, pOustersBoots);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pOustersBoots);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pOustersBoots);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pOustersBoots);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pOustersBoots);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pOustersBoots);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pOustersBoots);
                    } else
                        pStash->insert(x, y, pOustersBoots);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pOustersBoots);
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
void OustersBootsLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneOustersBootsStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM OustersBootsObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneOustersBootsStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneOustersBootsStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneOustersBootsStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersBoots* pOustersBoots = new OustersBoots();

            pOustersBoots->setItemID(pResult->getInt(++i));
            pOustersBoots->setObjectID(pResult->getInt(++i));
            pOustersBoots->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pOustersBoots->setOptionType(optionTypes);

            pOustersBoots->setDurability(pResult->getInt(++i));
            pOustersBoots->setEnchantLevel(pResult->getInt(++i));
            pOustersBoots->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pOustersBoots);
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
void OustersBootsLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

OustersBootsLoader* g_pOustersBootsLoader = NULL;
