//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireCoupleRing.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "VampireCoupleRing.h"

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

VampireCoupleRingInfoManager* g_pVampireCoupleRingInfoManager = NULL;

ItemID_t VampireCoupleRing::m_ItemIDRegistry = 0;
Mutex VampireCoupleRing::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class VampireCoupleRing member methods
//////////////////////////////////////////////////////////////////////////////

VampireCoupleRing::VampireCoupleRing()

{
    m_ItemType = 0;
}

VampireCoupleRing::VampireCoupleRing(ItemType_t itemType, const list<OptionType_t>& optionType)

    : m_ItemType(itemType), m_OptionType(optionType) {
    __BEGIN_TRY

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "VampireCoupleRing::VampireCoupleRing() : Invalid item type or option type");
        throw("VampireCoupleRing::VampireCoupleRing() : Invalid item type or optionType");
    }

    __END_CATCH
}

void VampireCoupleRing::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
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
        setOptionTypeToField(m_OptionType, optionField);

        PreparedStatement insertVampireCoupleRingStmt(
            pConn, "INSERT INTO VampireCoupleRingObject "
                   "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, OptionType, Name, PartnerItemID)"
                   " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        insertVampireCoupleRingStmt.bindUInt(1, m_ItemID);
        insertVampireCoupleRingStmt.bindUInt(2, m_ObjectID);
        insertVampireCoupleRingStmt.bindUInt(3, m_ItemType);
        insertVampireCoupleRingStmt.bindString(4, ownerID);
        insertVampireCoupleRingStmt.bindInt(5, (int)storage);
        insertVampireCoupleRingStmt.bindUInt(6, storageID);
        insertVampireCoupleRingStmt.bindInt(7, (int)x);
        insertVampireCoupleRingStmt.bindInt(8, (int)y);
        insertVampireCoupleRingStmt.bindString(9, optionField);
        insertVampireCoupleRingStmt.bindString(10, getName());
        insertVampireCoupleRingStmt.bindUInt(11, getPartnerItemID());
        insertVampireCoupleRingStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireCoupleRing::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysaveVampireCoupleRingStmt(pConn, string("UPDATE VampireCoupleRingObject SET ") + field +
                                                                   " WHERE ItemID=?");
        tinysaveVampireCoupleRingStmt.bindUInt(1, m_ItemID);
        tinysaveVampireCoupleRingStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void VampireCoupleRing::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        /*
        StringStream sql;

        sql << "UPDATE VampireCoupleRingObject SET "
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

        PreparedStatement saveVampireCoupleRingStmt(
            pConn, "UPDATE VampireCoupleRingObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                   "StorageID=?, X=?, Y=?, Name=?, PartnerItemID=? WHERE ItemID=?");
        saveVampireCoupleRingStmt.bindUInt(1, m_ObjectID);
        saveVampireCoupleRingStmt.bindUInt(2, m_ItemType);
        saveVampireCoupleRingStmt.bindString(3, ownerID);
        saveVampireCoupleRingStmt.bindInt(4, (int)storage);
        saveVampireCoupleRingStmt.bindUInt(5, storageID);
        saveVampireCoupleRingStmt.bindInt(6, (int)x);
        saveVampireCoupleRingStmt.bindInt(7, (int)y);
        saveVampireCoupleRingStmt.bindString(8, getName());
        saveVampireCoupleRingStmt.bindUInt(9, getPartnerItemID());
        saveVampireCoupleRingStmt.bindUInt(10, m_ItemID);
        saveVampireCoupleRingStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string VampireCoupleRing::toString() const

{
    StringStream msg;
    msg << "VampireCoupleRing("
        << "ItemID:" << m_ItemID << ",ItemType:" << m_ItemType << ")";
    return msg.toString();
}

VolumeWidth_t VampireCoupleRing::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pVampireCoupleRingInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t VampireCoupleRing::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pVampireCoupleRingInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t VampireCoupleRing::getWeight() const

{
    __BEGIN_TRY

    return g_pVampireCoupleRingInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}

bool VampireCoupleRing::hasPartnerItem()

{
    __BEGIN_TRY

    bool bRet = false;

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement hasPartnerItemStmt(
            pConn, "SELECT count(*) from VampireCoupleRingObject where ItemID=? and Storage IN(0, 1, 2, 3, 4, 9)");
        hasPartnerItemStmt.bindUInt(1, getPartnerItemID());
        Result* pResult = hasPartnerItemStmt.execute();

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
// class VampireCoupleRingInfo member methods
//////////////////////////////////////////////////////////////////////////////

string VampireCoupleRingInfo::toString() const

{
    StringStream msg;
    msg << "VampireCoupleRingInfo("
        << "ItemType:" << m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << m_Weight << ",Description:" << m_Description
        << ")";
    return msg.toString();
}

void VampireCoupleRingInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM VampireCoupleRingInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectVampireCoupleRingInfoStmt(
            pConn, "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM VampireCoupleRingInfo");
        pResult = selectVampireCoupleRingInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireCoupleRingInfo* pVampireCoupleRingInfo = new VampireCoupleRingInfo();

            pVampireCoupleRingInfo->setItemType(pResult->getInt(++i));
            pVampireCoupleRingInfo->setName(pResult->getString(++i));
            pVampireCoupleRingInfo->setEName(pResult->getString(++i));
            pVampireCoupleRingInfo->setPrice(pResult->getInt(++i));
            pVampireCoupleRingInfo->setVolumeType(pResult->getInt(++i));
            pVampireCoupleRingInfo->setWeight(pResult->getInt(++i));
            pVampireCoupleRingInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pVampireCoupleRingInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class VampireCoupleRingLoader member methods
//////////////////////////////////////////////////////////////////////////////

void VampireCoupleRingLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM VampireCoupleRingObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectVampireCoupleRingLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Name, PartnerItemID FROM "
                   "VampireCoupleRingObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9)");
        selectVampireCoupleRingLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectVampireCoupleRingLoaderStmt.execute();

        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
        if (pResult->getRowCount() == 0 && pPC->getFlagSet()->isOn(FLAGSET_IS_COUPLE)) {
            pPC->getFlagSet()->turnOff(FLAGSET_IS_COUPLE);
            pPC->getFlagSet()->save(pPC->getName());

            g_pCoupleManager->removeCoupleForce(pPC);
        }

        while (pResult->next()) {
            try {
                uint i = 0;

                VampireCoupleRing* pVampireCoupleRing = new VampireCoupleRing();

                pVampireCoupleRing->setItemID(pResult->getDWORD(++i));
                pVampireCoupleRing->setObjectID(pResult->getDWORD(++i));
                pVampireCoupleRing->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                string optionField = pResult->getString(++i);
                list<OptionType_t> optionTypes;
                setOptionTypeFromField(optionTypes, optionField);
                pVampireCoupleRing->setOptionType(optionTypes);

                pVampireCoupleRing->setName(pResult->getString(++i));
                pVampireCoupleRing->setPartnerItemID(pResult->getDWORD(++i));


                PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                //				if ( !pVampireCoupleRing->hasPartnerItem() )
                //				if ( pPC != NULL && !g_pCoupleManager->isCouple( pPC, pVampireCoupleRing->getName() ) )
                if (pPC != NULL && (!g_pCoupleManager->isCouple(pPC, pVampireCoupleRing->getName()) ||
                                    !pVampireCoupleRing->hasPartnerItem())) {
                    g_pCoupleManager->removeCoupleForce(pPC, pVampireCoupleRing->getName());
                    // pVampireCoupleRing->destroy();
                    char sql[30];
                    sprintf(sql, "Storage = 10");
                    pVampireCoupleRing->tinysave(sql);
                    SAFE_DELETE(pVampireCoupleRing);


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
                    if (pInventory->canAddingEx(x, y, pVampireCoupleRing)) {
                        pInventory->addItemEx(x, y, pVampireCoupleRing);
                    } else {
                        processItemBugEx(pCreature, pVampireCoupleRing);
                    }
                    break;

                case STORAGE_GEAR:
                    if (pCreature->isSlayer()) {
                        if (!pSlayer->isWear((Slayer::WearPart)x)) {
                            pSlayer->wearItem((Slayer::WearPart)x, pVampireCoupleRing);
                        } else {
                            processItemBugEx(pCreature, pVampireCoupleRing);
                        }
                    } else if (pCreature->isVampire()) {
                        if (!pVampire->isWear((Vampire::WearPart)x)) {
                            pVampire->wearItem((Vampire::WearPart)x, pVampireCoupleRing);
                        } else {
                            processItemBugEx(pCreature, pVampireCoupleRing);
                        }
                    }
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pVampireCoupleRing);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pVampireCoupleRing);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pVampireCoupleRing);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pVampireCoupleRing);
                    break;

                case STORAGE_STASH:
                    processItemBugEx(pCreature, pVampireCoupleRing);
                    /*		if (pStash->isExist(x, y))
                            {
                                processItemBugEx(pCreature, pVampireCoupleRing);
                            }
                            else pStash->insert(x, y, pVampireCoupleRing); */
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pVampireCoupleRing);
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

void VampireCoupleRingLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    cout << "     ." << endl;
    Assert(false);

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZoneVampireCoupleRingStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM VampireCoupleRingObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZoneVampireCoupleRingStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZoneVampireCoupleRingStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZoneVampireCoupleRingStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            VampireCoupleRing* pVampireCoupleRing = new VampireCoupleRing();

            pVampireCoupleRing->setItemID(pResult->getInt(++i));
            pVampireCoupleRing->setObjectID(pResult->getInt(++i));
            pVampireCoupleRing->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pVampireCoupleRing);
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

void VampireCoupleRingLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

VampireCoupleRingLoader* g_pVampireCoupleRingLoader = NULL;
