//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireAmulet.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "VampireAmulet.h"

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
VampireAmuletInfoManager* g_pVampireAmuletInfoManager = NULL;

ItemID_t VampireAmulet::m_ItemIDRegistry = 0;
Mutex VampireAmulet::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
VampireAmulet::VampireAmulet()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    // m_EnchantLevel = 0;
}

VampireAmulet::VampireAmulet(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);

    // m_EnchantLevel = 0;

    //	m_Durability = computeMaxDurability(this);

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "VampireAmulet::VampireAmulet() : Invalid item type or option type");
        throw("VampireAmulet::VampireAmulet() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void VampireAmulet::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertVampireAmuletStmt(pConn,
                                                    "INSERT INTO VampireAmuletObject "
                                                    "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                    " X, Y, OptionType, Grade, ItemFlag)"
                                                    " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertVampireAmuletStmt.bindUInt(1, m_ItemID);
        insertVampireAmuletStmt.bindUInt(2, m_ObjectID);
        insertVampireAmuletStmt.bindUInt(3, getItemType());
        insertVampireAmuletStmt.bindString(4, ownerID);
        insertVampireAmuletStmt.bindInt(5, (int)storage);
        insertVampireAmuletStmt.bindUInt(6, storageID);
        insertVampireAmuletStmt.bindInt(7, (int)x);
        insertVampireAmuletStmt.bindInt(8, (int)y);
        insertVampireAmuletStmt.bindString(9, optionField);
        insertVampireAmuletStmt.bindInt(10, getGrade());
        insertVampireAmuletStmt.bindInt(11, (int)m_CreateType);
        insertVampireAmuletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireAmulet::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveVampireAmuletStmt(
            pConn, string("UPDATE VampireAmuletObject SET ") + field + " WHERE ItemID=?");
        tinysaveVampireAmuletStmt.bindUInt(1, m_ItemID);
        tinysaveVampireAmuletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireAmulet::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE VampireAmuletObject SET "
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
        PreparedStatement saveVampireAmuletStmt(
            pConn, "UPDATE VampireAmuletObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                   "StorageID=?, X=?, Y=?, OptionType=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveVampireAmuletStmt.bindUInt(1, m_ObjectID);
        saveVampireAmuletStmt.bindUInt(2, getItemType());
        saveVampireAmuletStmt.bindString(3, ownerID);
        saveVampireAmuletStmt.bindInt(4, (int)storage);
        saveVampireAmuletStmt.bindUInt(5, storageID);
        saveVampireAmuletStmt.bindInt(6, (int)x);
        saveVampireAmuletStmt.bindInt(7, (int)y);
        saveVampireAmuletStmt.bindString(8, optionField);
        saveVampireAmuletStmt.bindInt(9, getGrade());
        saveVampireAmuletStmt.bindInt(10, (int)getEnchantLevel());
        saveVampireAmuletStmt.bindUInt(11, m_ItemID);
        saveVampireAmuletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireAmulet::toString() const

{
    StringStream msg;

    msg << "VampireAmulet("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t VampireAmulet::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pVampireAmuletInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t VampireAmulet::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pVampireAmuletInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t VampireAmulet::getWeight() const

{
    __BEGIN_TRY

    return g_pVampireAmuletInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t VampireAmulet::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pVampireAmuletInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t VampireAmulet::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pVampireAmuletInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireAmuletInfo::toString() const

{
    StringStream msg;

    msg << "VampireAmuletInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void VampireAmuletInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM VampireAmuletInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectVampireAmuletInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM VampireAmuletInfo");
        pResult = selectVampireAmuletInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireAmuletInfo* pVampireAmuletInfo = new VampireAmuletInfo();

            pVampireAmuletInfo->setItemType(pResult->getInt(++i));
            pVampireAmuletInfo->setName(pResult->getString(++i));
            pVampireAmuletInfo->setEName(pResult->getString(++i));
            pVampireAmuletInfo->setPrice(pResult->getInt(++i));
            pVampireAmuletInfo->setVolumeType(pResult->getInt(++i));
            pVampireAmuletInfo->setWeight(pResult->getInt(++i));
            pVampireAmuletInfo->setRatio(pResult->getInt(++i));
            pVampireAmuletInfo->setDurability(pResult->getInt(++i));
            pVampireAmuletInfo->setDefenseBonus(pResult->getInt(++i));
            pVampireAmuletInfo->setProtectionBonus(pResult->getInt(++i));
            pVampireAmuletInfo->setReqAbility(pResult->getString(++i));
            pVampireAmuletInfo->setItemLevel(pResult->getInt(++i));
            pVampireAmuletInfo->setDefaultOptions(pResult->getString(++i));
            pVampireAmuletInfo->setUpgradeRatio(pResult->getInt(++i));
            pVampireAmuletInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pVampireAmuletInfo->setNextOptionRatio(pResult->getInt(++i));
            pVampireAmuletInfo->setNextItemType(pResult->getInt(++i));
            pVampireAmuletInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pVampireAmuletInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void VampireAmuletLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
            << "OptionType, Durability, EnchantLevel FROM VampireAmuletObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectVampireAmuletLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM VampireAmuletObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectVampireAmuletLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectVampireAmuletLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                VampireAmulet* pVampireAmulet = new VampireAmulet();

                pVampireAmulet->setItemID(pResult->getDWORD(++i));
                pVampireAmulet->setObjectID(pResult->getDWORD(++i));
                pVampireAmulet->setItemType(pResult->getDWORD(++i));

                if (g_pVampireAmuletInfoManager->getItemInfo(pVampireAmulet->getItemType())->isUnique())
                    pVampireAmulet->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pVampireAmulet->setOptionType(optionTypes);

                pVampireAmulet->setDurability(pResult->getInt(++i));
                pVampireAmulet->setGrade(pResult->getInt(++i));
                pVampireAmulet->setEnchantLevel(pResult->getInt(++i));
                pVampireAmulet->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pVampireAmulet)) {
                        pInventory->addItemEx(x, y, pVampireAmulet);
                    } else {
                        processItemBugEx(pCreature, pVampireAmulet);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        processItemBugEx(pCreature, pVampireAmulet);
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pVampireAmulet);
                        } else {
                            processItemBugEx(pCreature, pVampireAmulet);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pVampireAmulet);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pVampireAmulet);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pVampireAmulet);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pVampireAmulet);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pVampireAmulet);
                    } else
                        pStash->insert(x, y, pVampireAmulet);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pVampireAmulet);
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
void VampireAmuletLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneVampireAmuletStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM VampireAmuletObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneVampireAmuletStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneVampireAmuletStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneVampireAmuletStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireAmulet* pVampireAmulet = new VampireAmulet();

            pVampireAmulet->setItemID(pResult->getInt(++i));
            pVampireAmulet->setObjectID(pResult->getInt(++i));
            pVampireAmulet->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pVampireAmulet->setOptionType(optionTypes);

            pVampireAmulet->setDurability(pResult->getInt(++i));
            pVampireAmulet->setEnchantLevel(pResult->getInt(++i));
            pVampireAmulet->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pVampireAmulet);
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
void VampireAmuletLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

VampireAmuletLoader* g_pVampireAmuletLoader = NULL;
