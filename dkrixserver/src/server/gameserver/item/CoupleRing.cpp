//////////////////////////////////////////////////////////////////////////////
// Filename    : CoupleRing.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CoupleRing.h"

#include <stdio.h>

#include "Belt.h"
#include "DB.h"
#include "FlagSet.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Vampire.h"
#include "couple/CoupleManager.h"

CoupleRingInfoManager* g_pCoupleRingInfoManager = NULL;

ItemID_t CoupleRing::m_ItemIDRegistry = 0;
Mutex CoupleRing::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class CoupleRing member methods
//////////////////////////////////////////////////////////////////////////////

CoupleRing::CoupleRing()

{
    m_ItemType = 0;
}

CoupleRing::CoupleRing(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_OptionType(optionType) {
    __BEGIN_TRY

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "CoupleRing::CoupleRing() : Invalid item type or option type");
        throw("CoupleRing::CoupleRing() : Invalid item type or optionType");
    }

    __END_CATCH
}

void CoupleRing::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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
        setOptionTypeToField(m_OptionType, optionField);

        PreparedStatement insertCoupleRingObjectStmt(
            pConn, "INSERT INTO CoupleRingObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, "
                   "OptionType, Name, PartnerItemID) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertCoupleRingObjectStmt.bindLong(1, m_ItemID);
        insertCoupleRingObjectStmt.bindLong(2, m_ObjectID);
        insertCoupleRingObjectStmt.bindInt(3, m_ItemType);
        insertCoupleRingObjectStmt.bindString(4, ownerID);
        insertCoupleRingObjectStmt.bindInt(5, (int)storage);
        insertCoupleRingObjectStmt.bindLong(6, storageID);
        insertCoupleRingObjectStmt.bindInt(7, (int)x);
        insertCoupleRingObjectStmt.bindInt(8, (int)y);
        insertCoupleRingObjectStmt.bindString(9, optionField);
        insertCoupleRingObjectStmt.bindString(10, getName());
        insertCoupleRingObjectStmt.bindInt(11, getPartnerItemID());
        insertCoupleRingObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void CoupleRing::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave
        // precedent (batch 9). Only ItemID is bound.
        PreparedStatement tinysaveCoupleRingObjectStmt(pConn, string("UPDATE CoupleRingObject SET ") + field +
                                                                  " WHERE ItemID=?");
        tinysaveCoupleRingObjectStmt.bindLong(1, m_ItemID);
        tinysaveCoupleRingObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void CoupleRing::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE CoupleRingObject SET "
            << "ObjectID = " << m_ObjectID
            << ",ItemType = " << m_ItemType
            << ",OwnerID = '" << ownerID << "'"
            << ",Storage = " <<(int)storage
            << ",StorageID = " << storageID
            << ",X = " <<(int)x
            << ",Y = " <<(int)y
            << " WHERE ItemID = " << m_ItemID;

        pStmt->executeQueryString(sql.toString());
        */

        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateCoupleRingObjectStmt(
            pConn, "UPDATE CoupleRingObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                   "Name = ?, PartnerItemID=? WHERE ItemID=?");
        updateCoupleRingObjectStmt.bindLong(1, m_ObjectID);
        updateCoupleRingObjectStmt.bindInt(2, m_ItemType);
        updateCoupleRingObjectStmt.bindString(3, ownerID);
        updateCoupleRingObjectStmt.bindInt(4, (int)storage);
        updateCoupleRingObjectStmt.bindLong(5, storageID);
        updateCoupleRingObjectStmt.bindInt(6, (int)x);
        updateCoupleRingObjectStmt.bindInt(7, (int)y);
        updateCoupleRingObjectStmt.bindString(8, getName());
        updateCoupleRingObjectStmt.bindLong(9, getPartnerItemID());
        updateCoupleRingObjectStmt.bindLong(10, m_ItemID);
        updateCoupleRingObjectStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string CoupleRing::toString() const

{
    StringStream msg;
    msg << "CoupleRing("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ")";
    return msg.toString();
}

VolumeWidth_t CoupleRing::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pCoupleRingInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t CoupleRing::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pCoupleRingInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t CoupleRing::getWeight() const

{
    __BEGIN_TRY

    return g_pCoupleRingInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

bool CoupleRing::hasPartnerItem()

{
    __BEGIN_TRY

    bool bRet = false;

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectCoupleRingObjectStmt(
            pConn, "SELECT count(*) from CoupleRingObject where ItemID=? and Storage IN(0, 1, 2, 3, 4, 9)");
        selectCoupleRingObjectStmt.bindLong(1, getPartnerItemID());
        Result* pResult = selectCoupleRingObjectStmt.execute();


        if (pResult->next()) {
            int count = pResult->getInt(1);


            Assert(count >= 0);
            Assert(count <= 1);

            if (count == 1)
                bRet = true;
        } else {
            bRet = false;
        }
    }
    END_DB(pStmt)

    return bRet;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class CoupleRingInfo member methods
//////////////////////////////////////////////////////////////////////////////

string CoupleRingInfo::toString() const

{
    StringStream msg;
    msg << "CoupleRingInfo("
        << "ItemType:" << m_ItemType << ",Name:" << getName().c_str() << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";
    return msg.toString();
}

void CoupleRingInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectCoupleRingInfoStmt(pConn, "SELECT MAX(ItemType) FROM CoupleRingInfo");
        Result* pResult = selectCoupleRingInfoStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectCoupleRingInfoStmt2(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM CoupleRingInfo");
        pResult = selectCoupleRingInfoStmt2.execute();

        while (pResult->next()) {
            uint i = 0;

            CoupleRingInfo* pCoupleRingInfo = new CoupleRingInfo();

            pCoupleRingInfo->setItemType(pResult->getInt(++i));
            pCoupleRingInfo->setName(pResult->getString(++i));
            pCoupleRingInfo->setEName(pResult->getString(++i));
            pCoupleRingInfo->setPrice(pResult->getInt(++i));
            pCoupleRingInfo->setVolumeType(pResult->getInt(++i));
            pCoupleRingInfo->setWeight(pResult->getInt(++i));
            pCoupleRingInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pCoupleRingInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class CoupleRingLoader member methods
//////////////////////////////////////////////////////////////////////////////

void CoupleRingLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM CoupleRingObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectCoupleRingObjectStmt2(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Name, PartnerItemID FROM "
                   "CoupleRingObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectCoupleRingObjectStmt2.bindString(1, pCreature->getName());
        Result* pResult = selectCoupleRingObjectStmt2.execute();

        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
        if (pResult->getRowCount() == 0 && pPC->getFlagSet()->isOn(FLAGSET_IS_COUPLE)) {
            pPC->getFlagSet()->turnOff(FLAGSET_IS_COUPLE);
            pPC->getFlagSet()->save(pPC->getName());

            g_pCoupleManager->removeCoupleForce(pPC);
        }

        while (pResult->next()) {
            try {
                uint i = 0;

                CoupleRing* pCoupleRing = new CoupleRing();

                pCoupleRing->setItemID(pResult->getDWORD(++i));
                pCoupleRing->setObjectID(pResult->getDWORD(++i));
                pCoupleRing->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pCoupleRing->setOptionType(optionTypes);

                pCoupleRing->setName(pResult->getString(++i));
                pCoupleRing->setPartnerItemID(pResult->getDWORD(++i));


                if (pPC != NULL &&
                    (!g_pCoupleManager->isCouple(pPC, pCoupleRing->getName()) || !pCoupleRing->hasPartnerItem())) {
                    g_pCoupleManager->removeCoupleForce(pPC, pCoupleRing->getName());
                    // pCoupleRing->destroy();
                    char sql[30];
                    sprintf(sql, "Storage = 10");
                    pCoupleRing->tinysave(sql);
                    SAFE_DELETE(pCoupleRing);


                    pPC->getFlagSet()->turnOff(FLAGSET_IS_COUPLE);
                    pPC->getFlagSet()->save(pPC->getName());
                    continue;
                }

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
                    if (pInventory->canAddingEx(x, y, pCoupleRing)) {
                        pInventory->addItemEx(x, y, pCoupleRing);
                    } else {
                        processItemBugEx(pCreature, pCoupleRing);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pCoupleRing);
                        } else {
                            processItemBugEx(pCreature, pCoupleRing);
                        }
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pCoupleRing);
                        } else {
                            processItemBugEx(pCreature, pCoupleRing);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pCoupleRing);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pCoupleRing);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pCoupleRing);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pCoupleRing);
                    break;

                case STORAGE_STASH:
                    processItemBugEx(pCreature, pCoupleRing);
                    /*		if (pStash->isExist(x, y))
                            {
                                processItemBugEx(pCreature, pCoupleRing);
                            }
                            else pStash->insert(x, y, pCoupleRing); */
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pCoupleRing);
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

void CoupleRingLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    cout << "     ." << endl;
    Assert(false);

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // StorageID_t/int values only (Storage enum, pZone->getZoneID()); no
        // string/user input. Migrated for consistency with the rest of the file.
        PreparedStatement loadZoneCoupleRingObjectStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM CoupleRingObject WHERE Storage = "
                   "? AND StorageID = ?");
        loadZoneCoupleRingObjectStmt.bindInt(1, (int)STORAGE_ZONE);
        loadZoneCoupleRingObjectStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = loadZoneCoupleRingObjectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            CoupleRing* pCoupleRing = new CoupleRing();

            pCoupleRing->setItemID(pResult->getInt(++i));
            pCoupleRing->setObjectID(pResult->getInt(++i));
            pCoupleRing->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pCoupleRing);
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

void CoupleRingLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

CoupleRingLoader* g_pCoupleRingLoader = NULL;
