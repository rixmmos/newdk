//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersWristlet.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

// include files
#include "OustersWristlet.h"

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
OustersWristletInfoManager* g_pOustersWristletInfoManager = NULL;

ItemID_t OustersWristlet::m_ItemIDRegistry = 0;
Mutex OustersWristlet::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
OustersWristlet::OustersWristlet()

{
    setItemType(0);
    setDurability(0);
    setBonusDamage(0);
}

OustersWristlet::OustersWristlet(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_OptionType( optionType )
{
    setItemType(itemType);
    setOptionType(optionType);
    setBonusDamage(0);

    // m_EnchantLevel = 0;
    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "OustersWristlet::OustersWristlet() : Invalid item type or option type");
        throw("OustersWristlet::OustersWristlet() : Invalid item type or optionType");
    }
}

//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void OustersWristlet::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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

        PreparedStatement insertOustersWristletStmt(pConn,
                                                      "INSERT INTO OustersWristletObject "
                                                      "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                      " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                      " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertOustersWristletStmt.bindUInt(1, m_ItemID);
        insertOustersWristletStmt.bindUInt(2, m_ObjectID);
        insertOustersWristletStmt.bindUInt(3, getItemType());
        insertOustersWristletStmt.bindString(4, ownerID);
        insertOustersWristletStmt.bindInt(5, (int)storage);
        insertOustersWristletStmt.bindUInt(6, storageID);
        insertOustersWristletStmt.bindInt(7, (int)x);
        insertOustersWristletStmt.bindInt(8, (int)y);
        insertOustersWristletStmt.bindString(9, optionField);
        insertOustersWristletStmt.bindUInt(10, getDurability());
        insertOustersWristletStmt.bindInt(11, getGrade());
        insertOustersWristletStmt.bindInt(12, (int)m_CreateType);
        insertOustersWristletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersWristlet::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveOustersWristletStmt(
            pConn, string("UPDATE OustersWristletObject SET ") + field + " WHERE ItemID=?");
        tinysaveOustersWristletStmt.bindUInt(1, m_ItemID);
        tinysaveOustersWristletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersWristlet::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE OustersWristletObject SET "
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
        PreparedStatement saveOustersWristletStmt(
            pConn, "UPDATE OustersWristletObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, "
                   "X=?, Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveOustersWristletStmt.bindUInt(1, m_ObjectID);
        saveOustersWristletStmt.bindUInt(2, getItemType());
        saveOustersWristletStmt.bindString(3, ownerID);
        saveOustersWristletStmt.bindInt(4, (int)storage);
        saveOustersWristletStmt.bindUInt(5, storageID);
        saveOustersWristletStmt.bindInt(6, (int)x);
        saveOustersWristletStmt.bindInt(7, (int)y);
        saveOustersWristletStmt.bindString(8, optionField);
        saveOustersWristletStmt.bindUInt(9, getDurability());
        saveOustersWristletStmt.bindInt(10, getGrade());
        saveOustersWristletStmt.bindInt(11, (int)getEnchantLevel());
        saveOustersWristletStmt.bindUInt(12, m_ItemID);
        saveOustersWristletStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersWristlet::toString() const

{
    StringStream msg;

    msg << "OustersWristlet("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t OustersWristlet::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t OustersWristlet::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t OustersWristlet::getWeight() const

{
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's minDamage
//--------------------------------------------------------------------------------
Damage_t OustersWristlet::getMinDamage() const

{
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(m_ItemType)->getMinDamage() + m_BonusDamage;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set weapon's maxDamage
//--------------------------------------------------------------------------------
Damage_t OustersWristlet::getMaxDamage() const

{
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(m_ItemType)->getMaxDamage() + m_BonusDamage;

    __END_CATCH
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
int OustersWristlet::getCriticalBonus(void) const

{
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(m_ItemType)->getCriticalBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get Elemental Type
//--------------------------------------------------------------------------------
ElementalType OustersWristlet::getElementalType(void) const {
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(getItemType())->getElementalType();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get Elemental
//--------------------------------------------------------------------------------
Elemental_t OustersWristlet::getElemental(void) const {
    __BEGIN_TRY

    return g_pOustersWristletInfoManager->getItemInfo(getItemType())->getElemental();

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersWristletInfo::toString() const

{
    StringStream msg;

    msg << "OustersWristletInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",minDamage:" << m_MinDamage << ",maxDamage:" << m_MaxDamage
        << ",ReqAbility:"
        << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void OustersWristletInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM OustersWristletInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectOustersWristletInfoStmt(
            pConn, "SELECT "
                   "ItemType,Name,EName,Price,Volume,Weight,Ratio,Durability,minDamage,maxDamage,Speed,ReqAbility,ItemLevel, "
                   "CriticalBonus, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, "
                   "DowngradeRatio, ElementalType, Elemental FROM OustersWristletInfo"
                   //"SELECT
                   // ItemType,Name,EName,Price,Volume,Weight,Ratio,Durability,minDamage,maxDamage,Speed,ReqAbility,ItemLevel,
                   // CriticalBonus, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType,
                   // DowngradeRatio FROM OustersWristletInfo"
        );
        pResult = selectOustersWristletInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersWristletInfo* pOustersWristletInfo = new OustersWristletInfo();

            pOustersWristletInfo->setItemType(pResult->getInt(++i));
            pOustersWristletInfo->setName(pResult->getString(++i));
            pOustersWristletInfo->setEName(pResult->getString(++i));
            pOustersWristletInfo->setPrice(pResult->getInt(++i));
            pOustersWristletInfo->setVolumeType(pResult->getInt(++i));
            pOustersWristletInfo->setWeight(pResult->getInt(++i));
            pOustersWristletInfo->setRatio(pResult->getInt(++i));
            pOustersWristletInfo->setDurability(pResult->getInt(++i));
            pOustersWristletInfo->setMinDamage(pResult->getInt(++i));
            pOustersWristletInfo->setMaxDamage(pResult->getInt(++i));
            pOustersWristletInfo->setSpeed(pResult->getInt(++i));
            pOustersWristletInfo->setReqAbility(pResult->getString(++i));
            pOustersWristletInfo->setItemLevel(pResult->getInt(++i));
            pOustersWristletInfo->setCriticalBonus(pResult->getInt(++i));
            pOustersWristletInfo->setDefaultOptions(pResult->getString(++i));
            pOustersWristletInfo->setUpgradeRatio(pResult->getInt(++i));
            pOustersWristletInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pOustersWristletInfo->setNextOptionRatio(pResult->getInt(++i));
            pOustersWristletInfo->setNextItemType(pResult->getInt(++i));
            pOustersWristletInfo->setDowngradeRatio(pResult->getInt(++i));
            pOustersWristletInfo->setElementalType((ElementalType)pResult->getInt(++i));
            pOustersWristletInfo->setElemental((Elemental_t)pResult->getInt(++i));

            addItemInfo(pOustersWristletInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void OustersWristletLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM OustersWristletObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectOustersWristletLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade,EnchantLevel, "
                   "ItemFlag FROM OustersWristletObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectOustersWristletLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectOustersWristletLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                OustersWristlet* pOustersWristlet = new OustersWristlet();

                pOustersWristlet->setItemID(pResult->getDWORD(++i));
                pOustersWristlet->setObjectID(pResult->getDWORD(++i));
                pOustersWristlet->setItemType(pResult->getDWORD(++i));

                if (g_pOustersWristletInfoManager->getItemInfo(pOustersWristlet->getItemType())->isUnique())
                    pOustersWristlet->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pOustersWristlet->setOptionType(optionTypes);

                pOustersWristlet->setDurability(pResult->getInt(++i));
                pOustersWristlet->setGrade(pResult->getInt(++i));
                pOustersWristlet->setEnchantLevel(pResult->getInt(++i));
                pOustersWristlet->setCreateType((Item::CreateType)pResult->getInt(++i));

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Ousters* pOusters = NULL;
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
                } else if (pCreature->isOusters()) {
                    pOusters = dynamic_cast<Ousters*>(pCreature);
                    pInventory = pOusters->getInventory();
                    pStash = pOusters->getStash();
                } else
                    throw UnsupportedError("Monster,NPC     .");

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pOustersWristlet)) {
                        pInventory->addItemEx(x, y, pOustersWristlet);
                    } else {
                        processItemBugEx(pCreature, pOustersWristlet);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer() || pCreature->isVampire()) {
                        processItemBugEx(pCreature, pOustersWristlet);
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pOustersWristlet);
                        } else {
                            processItemBugEx(pCreature, pOustersWristlet);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pOustersWristlet);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pOustersWristlet);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pOustersWristlet);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pOustersWristlet);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pOustersWristlet);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pOustersWristlet);
                    } else {
                        pStash->insert(x, y, pOustersWristlet);
                    }
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pOustersWristlet);
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
void OustersWristletLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneOustersWristletStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM OustersWristletObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneOustersWristletStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneOustersWristletStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneOustersWristletStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersWristlet* pOustersWristlet = new OustersWristlet();

            pOustersWristlet->setItemID(pResult->getInt(++i));
            pOustersWristlet->setObjectID(pResult->getInt(++i));
            pOustersWristlet->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pOustersWristlet->setOptionType(optionTypes);

            pOustersWristlet->setDurability(pResult->getInt(++i));
            pOustersWristlet->setEnchantLevel(pResult->getInt(++i));
            pOustersWristlet->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pOustersWristlet);
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
void OustersWristletLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

OustersWristletLoader* g_pOustersWristletLoader = NULL;
