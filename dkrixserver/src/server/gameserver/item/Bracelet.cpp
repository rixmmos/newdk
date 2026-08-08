//////////////////////////////////////////////////////////////////////////////
// Filename    : Bracelet.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Bracelet.h"

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
BraceletInfoManager* g_pBraceletInfoManager = NULL;

ItemID_t Bracelet::m_ItemIDRegistry = 0;
Mutex Bracelet::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Bracelet::Bracelet()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    //	m_EnchantLevel = 0;
}

Bracelet::Bracelet(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    //	m_EnchantLevel = 0;
    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Bracelet::Bracelet() : Invalid item type or option type");
        cerr << "Bracelet::Bracelet() : Invalid item type or optionType" << endl;
        throw("Bracelet::Bracelet() : Invalid item type or optionType");
    }
}

//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Bracelet::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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


        PreparedStatement insertBraceletObjectStmt(
            pConn, "INSERT INTO BraceletObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Durability, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertBraceletObjectStmt.bindLong(1, m_ItemID);
        insertBraceletObjectStmt.bindLong(2, m_ObjectID);
        insertBraceletObjectStmt.bindInt(3, getItemType());
        insertBraceletObjectStmt.bindString(4, ownerID);
        insertBraceletObjectStmt.bindInt(5, (int)storage);
        insertBraceletObjectStmt.bindLong(6, storageID);
        insertBraceletObjectStmt.bindInt(7, (int)x);
        insertBraceletObjectStmt.bindInt(8, (int)y);
        insertBraceletObjectStmt.bindString(9, optionField);
        insertBraceletObjectStmt.bindInt(10, getDurability());
        insertBraceletObjectStmt.bindInt(11, getGrade());
        insertBraceletObjectStmt.bindInt(12, (int)m_CreateType);
        insertBraceletObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Bracelet::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveBraceletObjectStmt(pConn,
                                                     string("UPDATE BraceletObject SET ") + field + " WHERE ItemID=?");
        tinysaveBraceletObjectStmt.bindLong(1, m_ItemID);
        tinysaveBraceletObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Bracelet::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");


        /*
        StringStream sql;

        sql << "UPDATE BraceletObject SET "
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
        PreparedStatement updateBraceletObjectStmt(
            pConn, "UPDATE BraceletObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel=? WHERE ItemID=?");
        updateBraceletObjectStmt.bindLong(1, m_ObjectID);
        updateBraceletObjectStmt.bindInt(2, getItemType());
        updateBraceletObjectStmt.bindString(3, ownerID);
        updateBraceletObjectStmt.bindInt(4, (int)storage);
        updateBraceletObjectStmt.bindLong(5, storageID);
        updateBraceletObjectStmt.bindInt(6, (int)x);
        updateBraceletObjectStmt.bindInt(7, (int)y);
        updateBraceletObjectStmt.bindString(8, optionField);
        updateBraceletObjectStmt.bindInt(9, getDurability());
        updateBraceletObjectStmt.bindInt(10, getGrade());
        updateBraceletObjectStmt.bindInt(11, (int)getEnchantLevel());
        updateBraceletObjectStmt.bindLong(12, m_ItemID);
        updateBraceletObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Bracelet::toString() const

{
    StringStream msg;

    msg << "Bracelet("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Bracelet::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pBraceletInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Bracelet::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pBraceletInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Bracelet::getWeight() const

{
    __BEGIN_TRY

    return g_pBraceletInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t Bracelet::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pBraceletInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t Bracelet::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pBraceletInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}

*/
//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string BraceletInfo::toString() const

{
    StringStream msg;

    msg << "BraceletInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void BraceletInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectBraceletInfoStmt(pConn, "SELECT MAX(ItemType) FROM BraceletInfo");
        Result* pResult = selectBraceletInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectBraceletInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, "
                   "ReqAbility,ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, "
                   "NextItemType, DowngradeRatio FROM BraceletInfo");
        pResult = selectBraceletInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            BraceletInfo* pBraceletInfo = new BraceletInfo();

            pBraceletInfo->setItemType(pResult->getInt(++i));
            pBraceletInfo->setName(pResult->getString(++i));
            pBraceletInfo->setEName(pResult->getString(++i));
            pBraceletInfo->setPrice(pResult->getInt(++i));
            pBraceletInfo->setVolumeType(pResult->getInt(++i));
            pBraceletInfo->setWeight(pResult->getInt(++i));
            pBraceletInfo->setRatio(pResult->getInt(++i));
            pBraceletInfo->setDurability(pResult->getInt(++i));
            pBraceletInfo->setDefenseBonus(pResult->getInt(++i));
            pBraceletInfo->setProtectionBonus(pResult->getInt(++i));
            pBraceletInfo->setReqAbility(pResult->getString(++i));
            pBraceletInfo->setItemLevel(pResult->getInt(++i));
            pBraceletInfo->setDefaultOptions(pResult->getString(++i));
            pBraceletInfo->setUpgradeRatio(pResult->getInt(++i));
            pBraceletInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pBraceletInfo->setNextOptionRatio(pResult->getInt(++i));
            pBraceletInfo->setNextItemType(pResult->getInt(++i));
            pBraceletInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pBraceletInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void BraceletLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM BraceletObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectBraceletObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM BraceletObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectBraceletObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectBraceletObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Bracelet* pBracelet = new Bracelet();

                pBracelet->setItemID(pResult->getDWORD(++i));
                pBracelet->setObjectID(pResult->getDWORD(++i));
                pBracelet->setItemType(pResult->getDWORD(++i));

                if (g_pBraceletInfoManager->getItemInfo(pBracelet->getItemType())->isUnique())
                    pBracelet->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pBracelet->setOptionType(optionTypes);

                pBracelet->setDurability(pResult->getInt(++i));
                pBracelet->setGrade(pResult->getInt(++i));
                pBracelet->setEnchantLevel(pResult->getInt(++i));
                pBracelet->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pBracelet)) {
                        pInventory->addItemEx(x, y, pBracelet);
                    } else {
                        processItemBugEx(pCreature, pBracelet);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pBracelet);
                        } else {
                            processItemBugEx(pCreature, pBracelet);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pBracelet);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pBracelet);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pBracelet);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pBracelet);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pBracelet);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pBracelet);
                    } else
                        pStash->insert(x, y, pBracelet);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pBracelet);
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
void BraceletLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneBraceletObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, "
                   "EnchantLevel, ItemFlag FROM BraceletObject WHERE Storage = ? AND StorageID = ?");
        loadZoneBraceletObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneBraceletObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneBraceletObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Bracelet* pBracelet = new Bracelet();

            pBracelet->setItemID(pResult->getInt(++i));
            pBracelet->setObjectID(pResult->getInt(++i));
            pBracelet->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pBracelet->setOptionType(optionTypes);

            pBracelet->setDurability(pResult->getInt(++i));
            pBracelet->setEnchantLevel(pResult->getInt(++i));
            pBracelet->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pBracelet);
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
void BraceletLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)


    __END_CATCH
}

BraceletLoader* g_pBraceletLoader = NULL;
