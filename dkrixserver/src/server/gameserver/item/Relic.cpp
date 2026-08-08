//////////////////////////////////////////////////////////////////////////////
// Filename    : Relic.cpp
// Written By  : Changaya
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Relic.h"

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
RelicInfoManager* g_pRelicInfoManager = NULL;

ItemID_t Relic::m_ItemIDRegistry = 0;
Mutex Relic::m_Mutex;


const string RelicType2String[] = {"RELIC_TYPE_SLAYER", "RELIC_TYPE_VAMPIRE"};


//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Relic::Relic()

    : m_ItemType(0), m_Durability(0) {
    m_EnchantLevel = 0;
}

Relic::Relic(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_Durability(0) {
    try {
        m_EnchantLevel = 0;

        m_Durability = computeMaxDurability(this);

        if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
            filelog("itembug.log", "Relic::Relic() : Invalid item type or option type");
            throw("Relic::Relic() : Invalid item type or optionType");
        }
    } catch (Throwable& t) {
        cout << t.toString().c_str() << endl;
        Assert(false);
    }
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Relic::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        PreparedStatement insertRelicStmt(pConn,
                                           "INSERT INTO RelicObject "
                                           "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
                                           " X, Y, Durability)"
                                           " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertRelicStmt.bindUInt(1, m_ItemID);
        insertRelicStmt.bindUInt(2, m_ObjectID);
        insertRelicStmt.bindUInt(3, m_ItemType);
        insertRelicStmt.bindString(4, ownerID);
        insertRelicStmt.bindInt(5, (int)storage);
        insertRelicStmt.bindUInt(6, storageID);
        insertRelicStmt.bindInt(7, (int)x);
        insertRelicStmt.bindInt(8, (int)y);
        insertRelicStmt.bindUInt(9, m_Durability);
        insertRelicStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Relic::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveRelicStmt(pConn, string("UPDATE RelicObject SET ") + field + " WHERE ItemID=?");
        tinysaveRelicStmt.bindUInt(1, m_ItemID);
        tinysaveRelicStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Relic::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE RelicObject SET "
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

        PreparedStatement saveRelicStmt(pConn,
                                         "UPDATE RelicObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                                         "StorageID=?, X=?, Y=?, Durability=?, EnchantLevel=?  WHERE ItemID=?");
        saveRelicStmt.bindUInt(1, m_ObjectID);
        saveRelicStmt.bindUInt(2, m_ItemType);
        saveRelicStmt.bindString(3, ownerID);
        saveRelicStmt.bindInt(4, (int)storage);
        saveRelicStmt.bindUInt(5, storageID);
        saveRelicStmt.bindInt(6, (int)x);
        saveRelicStmt.bindInt(7, (int)y);
        saveRelicStmt.bindUInt(8, m_Durability);
        saveRelicStmt.bindInt(9, (int)m_EnchantLevel);
        saveRelicStmt.bindUInt(10, m_ItemID);
        saveRelicStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Relic::toString() const

{
    StringStream msg;

    msg << "Relic("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType << ",Durability:" << (int)m_Durability
        << ",EnchantLevel:" << (int)m_EnchantLevel << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Relic::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pRelicInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Relic::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pRelicInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Relic::getWeight() const

{
    __BEGIN_TRY

    return g_pRelicInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t Relic::getDefenseBonus() const

{
    __BEGIN_TRY

    return g_pRelicInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

    __END_CATCH
}
Protection_t Relic::getProtectionBonus() const

{
    __BEGIN_TRY

    return g_pRelicInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string RelicInfo::toString() const

{
    StringStream msg;

    msg << "RelicInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ",Durability:" << m_Durability << ",DefenseBonus:" << m_DefenseBonus << ")";

    return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void RelicInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM RelicInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectRelicInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, ReqAbility, "
                   "ItemLevel, RelicType, ZoneID, XCoord, YCoord, MonsterType FROM RelicInfo");
        pResult = selectRelicInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            RelicInfo* pRelicInfo = new RelicInfo();

            pRelicInfo->setItemType(pResult->getInt(++i));
            pRelicInfo->setName(pResult->getString(++i));
            pRelicInfo->setEName(pResult->getString(++i));
            pRelicInfo->setPrice(pResult->getInt(++i));
            pRelicInfo->setVolumeType(pResult->getInt(++i));
            pRelicInfo->setWeight(pResult->getInt(++i));
            pRelicInfo->setRatio(pResult->getInt(++i));
            pRelicInfo->setDurability(pResult->getInt(++i));
            pRelicInfo->setDefenseBonus(pResult->getInt(++i));
            pRelicInfo->setProtectionBonus(pResult->getInt(++i));
            pRelicInfo->setReqAbility(pResult->getString(++i));
            pRelicInfo->setItemLevel(pResult->getInt(++i));

            pRelicInfo->setRelicType(pResult->getString(++i));
            pRelicInfo->zoneID = pResult->getInt(++i);
            pRelicInfo->x = pResult->getInt(++i);
            pRelicInfo->y = pResult->getInt(++i);
            pRelicInfo->monsterType = pResult->getInt(++i);

            addItemInfo(pRelicInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void RelicLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
            << " OptionType, Durability, EnchantLevel FROM RelicObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement deleteRelicObjectStmt(pConn, "DELETE FROM RelicObject WHERE OwnerID = ?");
        deleteRelicObjectStmt.bindString(1, pCreature->getName());
        deleteRelicObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// load to zone
//--------------------------------------------------------------------------------
void RelicLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneRelicStmt(pConn,
                                               "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
                                               " Durability, EnchantLevel FROM RelicObject"
                                               " WHERE Storage = ? AND StorageID = ?");
        selectZoneRelicStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneRelicStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneRelicStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            Relic* pRelic = new Relic();

            pRelic->setItemID(pResult->getInt(++i));
            pRelic->setObjectID(pResult->getInt(++i));
            pRelic->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pRelic->setDurability(pResult->getInt(++i));
            pRelic->setEnchantLevel(pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pRelic);
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
void RelicLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

RelicLoader* g_pRelicLoader = NULL;
