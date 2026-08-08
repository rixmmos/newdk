//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersCirclet.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "OustersCirclet.h"

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
OustersCircletInfoManager* g_pOustersCircletInfoManager = NULL;

ItemID_t OustersCirclet::m_ItemIDRegistry = 0;
Mutex OustersCirclet::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
OustersCirclet::OustersCirclet()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    // m_EnchantLevel = 0;
}

OustersCirclet::OustersCirclet(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    // m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "OustersCirclet::OustersCirclet() : Invalid item type or option type");
        throw("OustersCirclet::OustersCirclet() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void OustersCirclet::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertOustersCircletStmt(pConn, "INSERT INTO OustersCircletObject "
                                                          "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                          " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                          " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertOustersCircletStmt.bindUInt(1, m_ItemID);
        insertOustersCircletStmt.bindUInt(2, m_ObjectID);
        insertOustersCircletStmt.bindUInt(3, getItemType());
        insertOustersCircletStmt.bindString(4, ownerID);
        insertOustersCircletStmt.bindInt(5, (int)storage);
        insertOustersCircletStmt.bindUInt(6, storageID);
        insertOustersCircletStmt.bindInt(7, (int)x);
        insertOustersCircletStmt.bindInt(8, (int)y);
        insertOustersCircletStmt.bindString(9, optionField);
        insertOustersCircletStmt.bindUInt(10, getDurability());
        insertOustersCircletStmt.bindInt(11, getGrade());
        insertOustersCircletStmt.bindInt(12, (int)m_CreateType);
        insertOustersCircletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersCirclet::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveOustersCircletStmt(pConn, string("UPDATE OustersCircletObject SET ") + field +
                                                                " WHERE ItemID=?");
        tinysaveOustersCircletStmt.bindUInt(1, m_ItemID);
        tinysaveOustersCircletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersCirclet::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE OustersCircletObject SET "
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
        PreparedStatement saveOustersCircletStmt(
            pConn, "UPDATE OustersCircletObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveOustersCircletStmt.bindUInt(1, m_ObjectID);
        saveOustersCircletStmt.bindUInt(2, getItemType());
        saveOustersCircletStmt.bindString(3, ownerID);
        saveOustersCircletStmt.bindInt(4, (int)storage);
        saveOustersCircletStmt.bindUInt(5, storageID);
        saveOustersCircletStmt.bindInt(6, (int)x);
        saveOustersCircletStmt.bindInt(7, (int)y);
        saveOustersCircletStmt.bindString(8, optionField);
        saveOustersCircletStmt.bindUInt(9, getDurability());
        saveOustersCircletStmt.bindInt(10, getGrade());
        saveOustersCircletStmt.bindInt(11, (int)getEnchantLevel());
        saveOustersCircletStmt.bindUInt(12, m_ItemID);
        saveOustersCircletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersCirclet::toString() const

{
    StringStream msg;

    msg << "OustersCirclet("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t OustersCirclet::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pOustersCircletInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t OustersCirclet::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pOustersCircletInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t OustersCirclet::getWeight() const

{
    __BEGIN_TRY

    return g_pOustersCircletInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t OustersCirclet::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pOustersCircletInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t OustersCirclet::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pOustersCircletInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersCircletInfo::toString() const

{
    StringStream msg;

    msg << "OustersCircletInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void OustersCircletInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM OustersCircletInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectOustersCircletInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM OustersCircletInfo");
        pResult = selectOustersCircletInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersCircletInfo* pOustersCircletInfo = new OustersCircletInfo();

            pOustersCircletInfo->setItemType(pResult->getInt(++i));
            pOustersCircletInfo->setName(pResult->getString(++i));
            pOustersCircletInfo->setEName(pResult->getString(++i));
            pOustersCircletInfo->setPrice(pResult->getInt(++i));
            pOustersCircletInfo->setVolumeType(pResult->getInt(++i));
            pOustersCircletInfo->setWeight(pResult->getInt(++i));
            pOustersCircletInfo->setRatio(pResult->getInt(++i));
            pOustersCircletInfo->setDurability(pResult->getInt(++i));
            pOustersCircletInfo->setDefenseBonus(pResult->getInt(++i));
            pOustersCircletInfo->setProtectionBonus(pResult->getInt(++i));
            pOustersCircletInfo->setReqAbility(pResult->getString(++i));
            pOustersCircletInfo->setItemLevel(pResult->getInt(++i));
            pOustersCircletInfo->setDefaultOptions(pResult->getString(++i));
            pOustersCircletInfo->setUpgradeRatio(pResult->getInt(++i));
            pOustersCircletInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pOustersCircletInfo->setNextOptionRatio(pResult->getInt(++i));
            pOustersCircletInfo->setNextItemType(pResult->getInt(++i));
            pOustersCircletInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pOustersCircletInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void OustersCircletLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << "OptionType, Durability, EnchantLevel FROM OustersCircletObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectOustersCircletLoaderStmt(
            pConn,
            "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
            "EnchantLevel, ItemFlag FROM OustersCircletObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectOustersCircletLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectOustersCircletLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                OustersCirclet* pOustersCirclet = new OustersCirclet();

                pOustersCirclet->setItemID(pResult->getDWORD(++i));
                pOustersCirclet->setObjectID(pResult->getDWORD(++i));
                pOustersCirclet->setItemType(pResult->getDWORD(++i));

                if (g_pOustersCircletInfoManager->getItemInfo(pOustersCirclet->getItemType())->isUnique())
                    pOustersCirclet->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pOustersCirclet->setOptionType(optionTypes);

                pOustersCirclet->setDurability(pResult->getInt(++i));
                pOustersCirclet->setGrade(pResult->getInt(++i));
                pOustersCirclet->setEnchantLevel(pResult->getInt(++i));
                pOustersCirclet->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pOustersCirclet)) {
                        pInventory->addItemEx(x, y, pOustersCirclet);
                    } else {
                        processItemBugEx(pCreature, pOustersCirclet);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer() || pCreature->isVampire()) {
                        processItemBugEx(pCreature, pOustersCirclet);
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pOustersCirclet);
                        } else {
                            processItemBugEx(pCreature, pOustersCirclet);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pOustersCirclet);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pOustersCirclet);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pOustersCirclet);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pOustersCirclet);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pOustersCirclet);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pOustersCirclet);
                    } else
                        pStash->insert(x, y, pOustersCirclet);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pOustersCirclet);
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
void OustersCircletLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneOustersCircletStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM OustersCircletObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneOustersCircletStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneOustersCircletStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneOustersCircletStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersCirclet* pOustersCirclet = new OustersCirclet();

            pOustersCirclet->setItemID(pResult->getInt(++i));
            pOustersCirclet->setObjectID(pResult->getInt(++i));
            pOustersCirclet->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pOustersCirclet->setOptionType(optionTypes);

            pOustersCirclet->setDurability(pResult->getInt(++i));
            pOustersCirclet->setEnchantLevel(pResult->getInt(++i));
            pOustersCirclet->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pOustersCirclet);
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
void OustersCircletLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

OustersCircletLoader* g_pOustersCircletLoader = NULL;
