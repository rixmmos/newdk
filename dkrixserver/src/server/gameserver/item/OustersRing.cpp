//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersRing.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "OustersRing.h"

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
OustersRingInfoManager* g_pOustersRingInfoManager = NULL;

ItemID_t OustersRing::m_ItemIDRegistry = 0;
Mutex OustersRing::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
OustersRing::OustersRing()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    // m_EnchantLevel = 0;
}

OustersRing::OustersRing(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    //	m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "OustersRing::OustersRing() : Invalid item type or option type");
        throw("OustersRing::OustersRing() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void OustersRing::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertOustersRingStmt(pConn,
                                                  "INSERT INTO OustersRingObject "
                                                  "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                                  " X, Y, OptionType, Durability, Grade, ItemFlag)"
                                                  " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertOustersRingStmt.bindUInt(1, m_ItemID);
        insertOustersRingStmt.bindUInt(2, m_ObjectID);
        insertOustersRingStmt.bindUInt(3, getItemType());
        insertOustersRingStmt.bindString(4, ownerID);
        insertOustersRingStmt.bindInt(5, (int)storage);
        insertOustersRingStmt.bindUInt(6, storageID);
        insertOustersRingStmt.bindInt(7, (int)x);
        insertOustersRingStmt.bindInt(8, (int)y);
        insertOustersRingStmt.bindString(9, optionField);
        insertOustersRingStmt.bindUInt(10, getDurability());
        insertOustersRingStmt.bindInt(11, getGrade());
        insertOustersRingStmt.bindInt(12, (int)m_CreateType);
        insertOustersRingStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersRing::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveOustersRingStmt(pConn,
                                                    string("UPDATE OustersRingObject SET ") + field + " WHERE ItemID=?");
        tinysaveOustersRingStmt.bindUInt(1, m_ItemID);
        tinysaveOustersRingStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void OustersRing::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE OustersRingObject SET "
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
        PreparedStatement saveOustersRingStmt(
            pConn, "UPDATE OustersRingObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, "
                   "Y=?, OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        saveOustersRingStmt.bindUInt(1, m_ObjectID);
        saveOustersRingStmt.bindUInt(2, getItemType());
        saveOustersRingStmt.bindString(3, ownerID);
        saveOustersRingStmt.bindInt(4, (int)storage);
        saveOustersRingStmt.bindUInt(5, storageID);
        saveOustersRingStmt.bindInt(6, (int)x);
        saveOustersRingStmt.bindInt(7, (int)y);
        saveOustersRingStmt.bindString(8, optionField);
        saveOustersRingStmt.bindUInt(9, getDurability());
        saveOustersRingStmt.bindInt(10, getGrade());
        saveOustersRingStmt.bindInt(11, (int)getEnchantLevel());
        saveOustersRingStmt.bindUInt(12, m_ItemID);
        saveOustersRingStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersRing::toString() const

{
    StringStream msg;

    msg << "OustersRing("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t OustersRing::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t OustersRing::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t OustersRing::getWeight() const

{
    __BEGIN_TRY

    return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t OustersRing::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t OustersRing::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersRingInfo::toString() const

{
    StringStream msg;

    msg << "OustersRingInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void OustersRingInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM OustersRingInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectOustersRingInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, "
                   "Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, "
                   "NextOptionRatio, NextItemType, DowngradeRatio FROM OustersRingInfo");
        pResult = selectOustersRingInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersRingInfo* pOustersRingInfo = new OustersRingInfo();

            pOustersRingInfo->setItemType(pResult->getInt(++i));
            pOustersRingInfo->setName(pResult->getString(++i));
            pOustersRingInfo->setEName(pResult->getString(++i));
            pOustersRingInfo->setPrice(pResult->getInt(++i));
            pOustersRingInfo->setVolumeType(pResult->getInt(++i));
            pOustersRingInfo->setWeight(pResult->getInt(++i));
            pOustersRingInfo->setRatio(pResult->getInt(++i));
            pOustersRingInfo->setDurability(pResult->getInt(++i));
            pOustersRingInfo->setDefenseBonus(pResult->getInt(++i));
            pOustersRingInfo->setProtectionBonus(pResult->getInt(++i));
            pOustersRingInfo->setReqAbility(pResult->getString(++i));
            pOustersRingInfo->setItemLevel(pResult->getInt(++i));
            pOustersRingInfo->setDefaultOptions(pResult->getString(++i));
            pOustersRingInfo->setUpgradeRatio(pResult->getInt(++i));
            pOustersRingInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pOustersRingInfo->setNextOptionRatio(pResult->getInt(++i));
            pOustersRingInfo->setNextItemType(pResult->getInt(++i));
            pOustersRingInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pOustersRingInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void OustersRingLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << "OptionType, Durability, EnchantLevel FROM OustersRingObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectOustersRingLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM OustersRingObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectOustersRingLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectOustersRingLoaderStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                OustersRing* pOustersRing = new OustersRing();

                pOustersRing->setItemID(pResult->getDWORD(++i));
                pOustersRing->setObjectID(pResult->getDWORD(++i));
                pOustersRing->setItemType(pResult->getDWORD(++i));

                if (g_pOustersRingInfoManager->getItemInfo(pOustersRing->getItemType())->isUnique())
                    pOustersRing->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pOustersRing->setOptionType(optionTypes);

                pOustersRing->setDurability(pResult->getInt(++i));
                pOustersRing->setGrade(pResult->getInt(++i));
                pOustersRing->setEnchantLevel(pResult->getInt(++i));
                pOustersRing->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pOustersRing)) {
                        pInventory->addItemEx(x, y, pOustersRing);
                    } else {
                        processItemBugEx(pCreature, pOustersRing);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer() || pCreature->isVampire()) {
                        processItemBugEx(pCreature, pOustersRing);
                    } else if (pCreature->isOusters()) {
                        if (!pOusters->isWear((Ousters::WearPart)x)) {
                            pOusters->wearItem((Ousters::WearPart)x, pOustersRing);
                        } else {
                            processItemBugEx(pCreature, pOustersRing);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pOustersRing);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pOustersRing);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pOustersRing);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pOustersRing);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pOustersRing);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pOustersRing);
                    } else
                        pStash->insert(x, y, pOustersRing);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pOustersRing);
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
void OustersRingLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneOustersRingStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                   " OptionType, Durability, EnchantLevel, ItemFlag FROM OustersRingObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneOustersRingStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneOustersRingStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneOustersRingStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            OustersRing* pOustersRing = new OustersRing();

            pOustersRing->setItemID(pResult->getInt(++i));
            pOustersRing->setObjectID(pResult->getInt(++i));
            pOustersRing->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pOustersRing->setOptionType(optionTypes);

            pOustersRing->setDurability(pResult->getInt(++i));
            pOustersRing->setEnchantLevel(pResult->getInt(++i));
            pOustersRing->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pOustersRing);
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
void OustersRingLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

OustersRingLoader* g_pOustersRingLoader = NULL;
