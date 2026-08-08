//////////////////////////////////////////////////////////////////////////////
// Filename    : Glove.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Glove.h"

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
GloveInfoManager* g_pGloveInfoManager = NULL;

ItemID_t Glove::m_ItemIDRegistry = 0;
Mutex Glove::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Glove::Glove()

//: m_ItemType(0), m_Durability(0)
{
    setItemType(0);
    setDurability(0);
    //	m_EnchantLevel = 0;
}

Glove::Glove(ItemType_t itemType, const list<OptionType_t>& optionType)

//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
    setItemType(itemType);
    setOptionType(optionType);
    //	m_EnchantLevel = 0;

    setDurability(computeMaxDurability(this));

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList())) {
        filelog("itembug.log", "Glove::Glove() : Invalid item type or option type");
        throw("Glove::Glove() : Invalid item type or optionType");
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Glove::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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


        PreparedStatement insertGloveObjectStmt(
            pConn, "INSERT INTO GloveObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID , X, Y, "
                   "OptionType, Durability, Grade, ItemFlag) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertGloveObjectStmt.bindLong(1, m_ItemID);
        insertGloveObjectStmt.bindLong(2, m_ObjectID);
        insertGloveObjectStmt.bindInt(3, getItemType());
        insertGloveObjectStmt.bindString(4, ownerID);
        insertGloveObjectStmt.bindInt(5, (int)storage);
        insertGloveObjectStmt.bindLong(6, storageID);
        insertGloveObjectStmt.bindInt(7, (int)x);
        insertGloveObjectStmt.bindInt(8, (int)y);
        insertGloveObjectStmt.bindString(9, optionField);
        insertGloveObjectStmt.bindInt(10, getDurability());
        insertGloveObjectStmt.bindInt(11, (int)getGrade());
        insertGloveObjectStmt.bindInt(12, (int)m_CreateType);
        insertGloveObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Glove::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveGloveObjectStmt(pConn, string("UPDATE GloveObject SET ") + field + " WHERE ItemID=?");
        tinysaveGloveObjectStmt.bindLong(1, m_ItemID);
        tinysaveGloveObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Glove::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE GloveObject SET "
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
        PreparedStatement updateGloveObjectStmt(
            pConn, "UPDATE GloveObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "OptionType=?, Durability=?, Grade=?, EnchantLevel = ? WHERE ItemID=?");
        updateGloveObjectStmt.bindLong(1, m_ObjectID);
        updateGloveObjectStmt.bindInt(2, getItemType());
        updateGloveObjectStmt.bindString(3, ownerID);
        updateGloveObjectStmt.bindInt(4, (int)storage);
        updateGloveObjectStmt.bindLong(5, storageID);
        updateGloveObjectStmt.bindInt(6, (int)x);
        updateGloveObjectStmt.bindInt(7, (int)y);
        updateGloveObjectStmt.bindString(8, optionField);
        updateGloveObjectStmt.bindInt(9, getDurability());
        updateGloveObjectStmt.bindInt(10, (int)getGrade());
        updateGloveObjectStmt.bindInt(11, (int)getEnchantLevel());
        updateGloveObjectStmt.bindLong(12, m_ItemID);
        updateGloveObjectStmt.execute();

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Glove::toString() const

{
    StringStream msg;

    msg << "Glove("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)getItemType()
        << ",OptionType:" << getOptionTypeToString(getOptionTypeList()).c_str()
        << ",Durability:" << (int)getDurability() << ",EnchantLevel:" << (int)getEnchantLevel() << ")";

    return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Glove::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pGloveInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Glove::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pGloveInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Glove::getWeight() const

{
    __BEGIN_TRY

    return g_pGloveInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t Glove::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pGloveInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t Glove::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pGloveInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string GloveInfo::toString() const

{
    StringStream msg;

    msg << "GloveInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void GloveInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectGloveInfoStmt(pConn, "SELECT MAX(ItemType) FROM GloveInfo");
        Result* pResult = selectGloveInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectGloveInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, "
                   "ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, "
                   "NextItemType, DowngradeRatio FROM GloveInfo");
        pResult = selectGloveInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            GloveInfo* pGloveInfo = new GloveInfo();

            pGloveInfo->setItemType(pResult->getInt(++i));
            pGloveInfo->setName(pResult->getString(++i));
            pGloveInfo->setEName(pResult->getString(++i));
            pGloveInfo->setPrice(pResult->getInt(++i));
            pGloveInfo->setVolumeType(pResult->getInt(++i));
            pGloveInfo->setWeight(pResult->getInt(++i));
            pGloveInfo->setRatio(pResult->getInt(++i));
            pGloveInfo->setDurability(pResult->getInt(++i));
            pGloveInfo->setDefenseBonus(pResult->getInt(++i));
            pGloveInfo->setProtectionBonus(pResult->getInt(++i));
            pGloveInfo->setReqAbility(pResult->getString(++i));
            pGloveInfo->setItemLevel(pResult->getInt(++i));
            pGloveInfo->setDefaultOptions(pResult->getString(++i));
            pGloveInfo->setUpgradeRatio(pResult->getInt(++i));
            pGloveInfo->setUpgradeCrashPercent(pResult->getInt(++i));
            pGloveInfo->setNextOptionRatio(pResult->getInt(++i));
            pGloveInfo->setNextItemType(pResult->getInt(++i));
            pGloveInfo->setDowngradeRatio(pResult->getInt(++i));

            addItemInfo(pGloveInfo);
        }

    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void GloveLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
            << " OptionType, Durability, EnchantLevel FROM GloveObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectGloveObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, "
                   "EnchantLevel, ItemFlag FROM GloveObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectGloveObjectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectGloveObjectStmt.execute();


        while (pResult->next()) {
            try {
                uint i = 0;

                Glove* pGlove = new Glove();

                pGlove->setItemID(pResult->getDWORD(++i));
                pGlove->setObjectID(pResult->getDWORD(++i));
                pGlove->setItemType(pResult->getDWORD(++i));

                if (g_pGloveInfoManager->getItemInfo(pGlove->getItemType())->isUnique())
                    pGlove->setUnique();

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pGlove->setOptionType(optionTypes);

                pGlove->setDurability(pResult->getInt(++i));
                pGlove->setGrade(pResult->getInt(++i));
                pGlove->setEnchantLevel(pResult->getInt(++i));
                pGlove->setCreateType((Item::CreateType)pResult->getInt(++i));

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
                    if (pInventory->canAddingEx(x, y, pGlove)) {
                        pInventory->addItemEx(x, y, pGlove);
                    } else {
                        processItemBugEx(pCreature, pGlove);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pGlove);
                        } else {
                            processItemBugEx(pCreature, pGlove);
                        }
                    } else if (pCreature->isVampire()) {
                        processItemBugEx(pCreature, pGlove);
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pGlove);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pGlove);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pGlove);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pGlove);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pGlove);
                    } else
                        pStash->insert(x, y, pGlove);
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pGlove);
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
void GloveLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneGloveObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, "
                   "EnchantLevel, ItemFlag FROM GloveObject WHERE Storage = ? AND StorageID = ?");
        loadZoneGloveObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneGloveObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneGloveObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Glove* pGlove = new Glove();

            pGlove->setItemID(pResult->getInt(++i));
            pGlove->setObjectID(pResult->getInt(++i));
            pGlove->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            string optionField = pResult->getString(++i);
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionField);
            pGlove->setOptionType(optionTypes);

            pGlove->setDurability(pResult->getInt(++i));
            pGlove->setEnchantLevel(pResult->getInt(++i));
            pGlove->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pGlove);
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
void GloveLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

GloveLoader* g_pGloveLoader = NULL;
