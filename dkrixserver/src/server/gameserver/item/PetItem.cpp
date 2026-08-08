//////////////////////////////////////////////////////////////////////////////
// Filename    : PetItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "PetItem.h"

#include "Belt.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "EffectHasPet.h"
#include "GamePlayer.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Motorcycle.h"
#include "Ousters.h"
#include "PetTypeInfo.h"
#include "PetUtil.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Stash.h"
#include "Utility.h"
#include "Vampire.h"

string getDBString(const string& str);

PetItemInfoManager* g_pPetItemInfoManager = NULL;

ItemID_t PetItem::m_ItemIDRegistry = 0;
Mutex PetItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class PetItem member methods
//////////////////////////////////////////////////////////////////////////////

PetItem::PetItem()

{
    m_ItemType = 0;
    m_pPetInfo = NULL;
}

PetItem::PetItem(ItemType_t itemType, const list<OptionType_t>& optionType)

{
    m_ItemType = itemType;
    m_pPetInfo = NULL;

    if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType)) {
        filelog("itembug.log", "PetItem::PetItem() : Invalid item type or option type");
        throw("PetItem::PetItem() : Invalid item type or optionType");
    }
}

void PetItem::create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID)

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

        /*		StringStream sql;

                sql << "INSERT INTO PetItemObject "
                    << "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, ItemFlag) VALUES("
                    << m_ItemID << ", "
                    << m_ObjectID << ", " << m_ItemType << ", '" << ownerID << "', "
                    <<(int)storage << ", " << storageID << ", " <<(int)x << ", " <<(int)y << ", "
                    << (int)m_CreateType << ")";

                pStmt->executeQueryString(sql.toString());*/

        if (m_pPetInfo == NULL) {
            PreparedStatement insertPetItemStmt(
                pConn, "INSERT INTO PetItemObject (ItemID, ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, ItemFlag) "
                       "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
            insertPetItemStmt.bindUInt(1, m_ItemID);
            insertPetItemStmt.bindUInt(2, m_ObjectID);
            insertPetItemStmt.bindUInt(3, m_ItemType);
            insertPetItemStmt.bindString(4, ownerID);
            insertPetItemStmt.bindInt(5, (int)storage);
            insertPetItemStmt.bindUInt(6, storageID);
            insertPetItemStmt.bindInt(7, (int)x);
            insertPetItemStmt.bindInt(8, (int)y);
            insertPetItemStmt.bindInt(9, (int)m_CreateType);
            insertPetItemStmt.execute();
        } else {
            PreparedStatement insertPetItemWithInfoStmt(
                pConn,
                "INSERT INTO PetItemObject (ItemID, ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, ItemFlag, "
                "PetCreatureType, PetLevel, PetExp, PetHP, PetAttr, PetAttrLevel, PetOption, FoodType, "
                "CanGamble, CanCutHead, CanAttack, LastFeedTime) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            insertPetItemWithInfoStmt.bindUInt(1, m_ItemID);
            insertPetItemWithInfoStmt.bindUInt(2, m_ObjectID);
            insertPetItemWithInfoStmt.bindUInt(3, m_ItemType);
            insertPetItemWithInfoStmt.bindString(4, ownerID);
            insertPetItemWithInfoStmt.bindInt(5, (int)storage);
            insertPetItemWithInfoStmt.bindUInt(6, storageID);
            insertPetItemWithInfoStmt.bindInt(7, (int)x);
            insertPetItemWithInfoStmt.bindInt(8, (int)y);
            insertPetItemWithInfoStmt.bindInt(9, (int)m_CreateType);
            insertPetItemWithInfoStmt.bindUInt(10, m_pPetInfo->getPetCreatureType());
            insertPetItemWithInfoStmt.bindUInt(11, m_pPetInfo->getPetLevel());
            insertPetItemWithInfoStmt.bindUInt(12, m_pPetInfo->getPetExp());
            insertPetItemWithInfoStmt.bindUInt(13, m_pPetInfo->getPetHP());
            insertPetItemWithInfoStmt.bindUInt(14, m_pPetInfo->getPetAttr());
            insertPetItemWithInfoStmt.bindUInt(15, m_pPetInfo->getPetAttrLevel());
            insertPetItemWithInfoStmt.bindUInt(16, m_pPetInfo->getPetOption());
            insertPetItemWithInfoStmt.bindUInt(17, m_pPetInfo->getFoodType());
            insertPetItemWithInfoStmt.bindUInt(18, m_pPetInfo->canGamble());
            insertPetItemWithInfoStmt.bindUInt(19, m_pPetInfo->canCutHead());
            insertPetItemWithInfoStmt.bindUInt(20, m_pPetInfo->canAttack());
            insertPetItemWithInfoStmt.bindString(21, m_pPetInfo->getLastFeedTime().toDateTime());
            insertPetItemWithInfoStmt.execute();
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void PetItem::tinysave(const char* field) const

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // field is a caller-built "Column=value" SQL fragment (see callers), not a
        // single bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Slayer::tinysave /
        // Guild::tinysave precedent (batches 7/9). Only ItemID is bound.
        PreparedStatement tinysavePetItemStmt(pConn, string("UPDATE PetItemObject SET ") + field + " WHERE ItemID=?");
        tinysavePetItemStmt.bindUInt(1, m_ItemID);
        tinysavePetItemStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void PetItem::save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        if (m_pPetInfo == NULL) {
            PreparedStatement savePetItemStmt(
                pConn, "UPDATE PetItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, "
                       "StorageID=?, X=?, Y=? WHERE ItemID=?");
            savePetItemStmt.bindUInt(1, m_ObjectID);
            savePetItemStmt.bindUInt(2, m_ItemType);
            savePetItemStmt.bindString(3, ownerID);
            savePetItemStmt.bindInt(4, (int)storage);
            savePetItemStmt.bindUInt(5, storageID);
            savePetItemStmt.bindInt(6, (int)x);
            savePetItemStmt.bindInt(7, (int)y);
            savePetItemStmt.bindUInt(8, m_ItemID);
            savePetItemStmt.execute();
        } else {
            // getDBString() escaped backslash/quote for the old raw-format string
            // embedding; with a bound parameter that escaping would double-encode
            // the nickname, so the raw string is bound instead.
            PreparedStatement savePetItemWithInfoStmt(
                pConn,
                "UPDATE PetItemObject SET ObjectID=?, ItemType=?, OwnerID=?, Storage=?, StorageID=?, X=?, Y=?, "
                "PetCreatureType=?, PetLevel=?, PetAttr=?, PetAttrLevel=?, PetExp=?, PetHP=?, FoodType=?, "
                "CanGamble=?, CanCutHead=?, CanAttack=?, LastFeedTime=?, Nickname=? "
                "WHERE ItemID=?");
            savePetItemWithInfoStmt.bindUInt(1, m_ObjectID);
            savePetItemWithInfoStmt.bindUInt(2, m_ItemType);
            savePetItemWithInfoStmt.bindString(3, ownerID);
            savePetItemWithInfoStmt.bindInt(4, (int)storage);
            savePetItemWithInfoStmt.bindUInt(5, storageID);
            savePetItemWithInfoStmt.bindInt(6, (int)x);
            savePetItemWithInfoStmt.bindInt(7, (int)y);
            savePetItemWithInfoStmt.bindUInt(8, m_pPetInfo->getPetCreatureType());
            savePetItemWithInfoStmt.bindUInt(9, m_pPetInfo->getPetLevel());
            savePetItemWithInfoStmt.bindUInt(10, m_pPetInfo->getPetAttr());
            savePetItemWithInfoStmt.bindUInt(11, m_pPetInfo->getPetAttrLevel());
            savePetItemWithInfoStmt.bindUInt(12, m_pPetInfo->getPetExp());
            savePetItemWithInfoStmt.bindUInt(13, m_pPetInfo->getPetHP());
            savePetItemWithInfoStmt.bindUInt(14, m_pPetInfo->getFoodType());
            savePetItemWithInfoStmt.bindUInt(15, m_pPetInfo->canGamble());
            savePetItemWithInfoStmt.bindUInt(16, m_pPetInfo->canCutHead());
            savePetItemWithInfoStmt.bindUInt(17, m_pPetInfo->canAttack());
            savePetItemWithInfoStmt.bindString(18, m_pPetInfo->getLastFeedTime().toDateTime());
            savePetItemWithInfoStmt.bindString(19, m_pPetInfo->getNickname());
            savePetItemWithInfoStmt.bindUInt(20, m_ItemID);
            savePetItemWithInfoStmt.execute();
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

void PetItem::savePetInfo() const {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        if (m_pPetInfo != NULL) {
            // getDBString() escaped backslash/quote for the old raw-format string
            // embedding; with a bound parameter that escaping would double-encode
            // the nickname, so the raw string is bound instead.
            PreparedStatement savePetInfoStmt(
                pConn, "UPDATE PetItemObject SET "
                       "PetCreatureType=?, PetLevel=?, PetAttr=?, PetAttrLevel=?, PetExp=?, PetHP=?, FoodType=?, "
                       "CanGamble=?, CanCutHead=?, CanAttack=?, LastFeedTime=?, Nickname=? "
                       "WHERE ItemID=?");
            savePetInfoStmt.bindUInt(1, m_pPetInfo->getPetCreatureType());
            savePetInfoStmt.bindUInt(2, m_pPetInfo->getPetLevel());
            savePetInfoStmt.bindUInt(3, m_pPetInfo->getPetAttr());
            savePetInfoStmt.bindUInt(4, m_pPetInfo->getPetAttrLevel());
            savePetInfoStmt.bindUInt(5, m_pPetInfo->getPetExp());
            savePetInfoStmt.bindUInt(6, m_pPetInfo->getPetHP());
            savePetInfoStmt.bindUInt(7, m_pPetInfo->getFoodType());
            savePetInfoStmt.bindUInt(8, m_pPetInfo->canGamble());
            savePetInfoStmt.bindUInt(9, m_pPetInfo->canCutHead());
            savePetInfoStmt.bindUInt(10, m_pPetInfo->canAttack());
            savePetInfoStmt.bindString(11, m_pPetInfo->getLastFeedTime().toDateTime());
            savePetInfoStmt.bindString(12, m_pPetInfo->getNickname());
            savePetInfoStmt.bindUInt(13, m_ItemID);
            savePetInfoStmt.execute();
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

void PetItem::makePCItemInfo(PCItemInfo& result) const {
    Item::makePCItemInfo(result);

    if (m_pPetInfo != NULL) {
        list<OptionType_t> olist;

        if (m_pPetInfo->getPetOption() != 0)
            olist.push_back(m_pPetInfo->getPetOption());

        result.setOptionType(olist);
        result.setDurability(m_pPetInfo->getPetHP());
        result.setEnchantLevel(m_pPetInfo->getPetAttr());
        result.setSilver(m_pPetInfo->getPetAttrLevel());
        result.setGrade((m_pPetInfo->getPetHP() == 0)
                            ? (m_pPetInfo->getLastFeedTime().daysTo(VSDateTime::currentDateTime()))
                            : (-1));
        result.setItemNum(m_pPetInfo->getPetLevel());
        result.setMainColor(0xffff);
    }
}

void PetItem::whenPCTake(PlayerCreature* pPC) {
    Item::whenPCTake(pPC);
    pPC->getPetItems().push_back(this);

    if (!pPC->isFlag(Effect::EFFECT_CLASS_HAS_PET)) {
        
        EffectHasPet* pEffect = new EffectHasPet(pPC);
        pEffect->setNextTime(600);
        pPC->setFlag(Effect::EFFECT_CLASS_HAS_PET);
        pPC->addEffect(pEffect);
    }
}

void PetItem::whenPCLost(PlayerCreature* pPC) {
    Item::whenPCLost(pPC);

    if (m_pPetInfo == pPC->getPetInfo()) {
        pPC->setPetInfo(NULL);
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
        if (pGamePlayer != NULL)
            sendPetInfo(pGamePlayer, true);
    }

    pPC->getPetItems().remove(this);
    if (pPC->getPetItems().empty()) {
        
        Effect* pEffect = pPC->findEffect(Effect::EFFECT_CLASS_HAS_PET);
        if (pEffect != NULL)
            pEffect->setDeadline(0);
    }
}

string PetItem::toString() const

{
    StringStream msg;

    msg << "PetItem("
        << "ItemID:" << m_ItemID << ",ItemType:" << (int)m_ItemType;

    if (m_pPetInfo != NULL) {
        msg << ",PetType:" << (int)m_pPetInfo->getPetType() << ",PetLevel:" << (int)m_pPetInfo->getPetLevel()
            << ",PetExp:" << (int)m_pPetInfo->getPetExp() << ",PetHP:" << (int)m_pPetInfo->getPetHP()
            << ",PetAttr:" << (int)m_pPetInfo->getPetAttr() << ",PetAttrLevel:" << (int)m_pPetInfo->getPetAttrLevel()
            << ",PetOption:" << (int)m_pPetInfo->getPetOption() << ",CanGamble:" << (int)m_pPetInfo->canGamble()
            << ",CanAttack:" << (int)m_pPetInfo->canAttack()
            << ",FeedTime:" << m_pPetInfo->getLastFeedTime().toString();
    }

    msg << ")";

    return msg.toString();
}

VolumeWidth_t PetItem::getVolumeWidth() const

{
    __BEGIN_TRY

    return g_pPetItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

    __END_CATCH
}

VolumeHeight_t PetItem::getVolumeHeight() const

{
    __BEGIN_TRY

    return g_pPetItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

    __END_CATCH
}

Weight_t PetItem::getWeight() const

{
    __BEGIN_TRY

    return g_pPetItemInfoManager->getItemInfo(m_ItemType)->getWeight();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// class PetItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string PetItemInfo::toString() const

{
    StringStream msg;
    msg << "PetItemInfo("
        << "ItemType:" << (int)m_ItemType << ",Name:" << m_Name << ",EName:" << m_EName << ",Price:" << (int)m_Price
        << ",VolumeType:" << Volume2String[m_VolumeType] << ",Weight:" << (int)m_Weight
        << ",Description:" << m_Description << ")";
    return msg.toString();
}

void PetItemInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectMaxItemTypeStmt(pConn, "SELECT MAX(ItemType) FROM PetItemInfo");
        Result* pResult = selectMaxItemTypeStmt.execute();

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0; i <= m_InfoCount; i++)
            m_pItemInfos[i] = NULL;

        PreparedStatement selectPetItemInfoStmt(pConn,
                                                 "SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM PetItemInfo");
        pResult = selectPetItemInfoStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            PetItemInfo* pPetItemInfo = new PetItemInfo();

            pPetItemInfo->setItemType(pResult->getInt(++i));
            pPetItemInfo->setName(pResult->getString(++i));
            pPetItemInfo->setEName(pResult->getString(++i));
            pPetItemInfo->setPrice(pResult->getInt(++i));
            pPetItemInfo->setVolumeType(pResult->getInt(++i));
            pPetItemInfo->setWeight(pResult->getInt(++i));
            pPetItemInfo->setRatio(pResult->getInt(++i));

            addItemInfo(pPetItemInfo);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class PetItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void PetItemLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM PetItemObject"
            << " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
            <<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", "
            <<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", "
            <<(int)STORAGE_GARBAGE << ")";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement selectPetItemLoaderStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag, "
                   "PetCreatureType, PetLevel, PetExp, PetHP, PetAttr, PetAttrLevel, PetOption, FoodType, "
                   "CanGamble, CanCutHead, CanAttack, LastFeedTime, Nickname "
                   "FROM PetItemObject WHERE OwnerID = ? AND Storage IN(0, 1, 2, 3, 4, 9, 13)");
        selectPetItemLoaderStmt.bindString(1, pCreature->getName());
        Result* pResult = selectPetItemLoaderStmt.execute();

        while (pResult->next()) {
            try {
                uint i = 0;

                PetItem* pPetItem = new PetItem();

                pPetItem->setItemID(pResult->getDWORD(++i));
                pPetItem->setObjectID(pResult->getDWORD(++i));
                pPetItem->setItemType(pResult->getDWORD(++i));

                Storage storage = (Storage)pResult->getInt(++i);
                StorageID_t storageID = pResult->getDWORD(++i);
                BYTE x = pResult->getBYTE(++i);
                BYTE y = pResult->getBYTE(++i);

                pPetItem->setCreateType((Item::CreateType)pResult->getInt(++i));

                PetInfo* pPetInfo = new PetInfo;
                pPetInfo->setPetType(pPetItem->getItemType());
                pPetInfo->setPetCreatureType(pResult->getInt(++i));
                pPetInfo->setPetLevel(pResult->getInt(++i));
                pPetInfo->setPetExp(pResult->getInt(++i));
                pPetInfo->setPetHP(pResult->getInt(++i));
                pPetInfo->setPetAttr(pResult->getInt(++i));
                pPetInfo->setPetAttrLevel(pResult->getInt(++i));
                pPetInfo->setPetOption(pResult->getInt(++i));
                pPetInfo->setFoodType(pResult->getInt(++i));
                pPetInfo->setGamble(pResult->getInt(++i));
                pPetInfo->setCutHead(pResult->getInt(++i));
                pPetInfo->setAttack(pResult->getInt(++i));
                pPetInfo->setFeedTime(VSDateTime(pResult->getString(++i)));
                pPetInfo->setNickname(pResult->getString(++i));

                
                pPetItem->setPetInfo(pPetInfo);
                pPetInfo->setPetItem(pPetItem);

                uint ratio = 100;

                if (storage == STORAGE_PET_STASH) {
                    ratio /= 2;
                    pPetInfo->setFeedTurn(2);
                } else {
                    //					refreshHP( pPetInfo );
                    pPetInfo->setFeedTurn(1);
                }

                if (pPetInfo->getPetLevel() == 50)
                    ratio /= 10;
                refreshHP(pPetInfo, ratio);

                PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo(pPetInfo->getPetType());
                if (pPetTypeInfo != NULL) {
                    pPetInfo->setPetCreatureType(pPetTypeInfo->getPetCreatureType(pPetInfo->getPetLevel()));
                }

                Inventory* pInventory = NULL;
                Slayer* pSlayer = NULL;
                Vampire* pVampire = NULL;
                Ousters* pOusters = NULL;
                Motorcycle* pMotorcycle = NULL;
                Inventory* pMotorInventory = NULL;
                Item* pItem = NULL;
                Stash* pStash = NULL;
                Belt* pBelt = NULL;
                Inventory* pBeltInventory = NULL;

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

                PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                Assert(pPC != NULL);

                switch (storage) {
                case STORAGE_INVENTORY:
                    if (pInventory->canAddingEx(x, y, pPetItem)) {
                        pInventory->addItemEx(x, y, pPetItem);
                        pPetItem->whenPCTake(pPC);
                    } else {
                        processItemBugEx(pCreature, pPetItem);
                    }
                    break;

                case STORAGE_GEAR:
                    processItemBugEx(pCreature, pPetItem);
                    break;

                case STORAGE_BELT:
                    processItemBugEx(pCreature, pPetItem);
                    break;

                case STORAGE_EXTRASLOT:
                    if (pCreature->isSlayer())
                        pSlayer->addItemToExtraInventorySlot(pPetItem);
                    else if (pCreature->isVampire())
                        pVampire->addItemToExtraInventorySlot(pPetItem);
                    else if (pCreature->isOusters())
                        pOusters->addItemToExtraInventorySlot(pPetItem);

                    pPetItem->whenPCTake(pPC);
                    break;

                case STORAGE_MOTORCYCLE:
                    processItemBugEx(pCreature, pPetItem);
                    break;

                case STORAGE_STASH:
                    if (pStash->isExist(x, y)) {
                        processItemBugEx(pCreature, pPetItem);
                    } else {
                        pStash->insert(x, y, pPetItem);
                        pPetItem->whenPCTake(pPC);
                    }
                    break;

                case STORAGE_GARBAGE:
                    processItemBug(pCreature, pPetItem);
                    break;


                case STORAGE_PET_STASH:
                     
                    if (pPC->getPetStashItem(storageID) == NULL) {
                        pPC->addPetStashItem(storageID, pPetItem);
                        pPetItem->whenPCTake(pPC);
                    } else
                        processItemBug(pCreature, pPetItem);
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

void PetItemLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectZonePetItemStmt(
            pConn, "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag FROM PetItemObject"
                   " WHERE Storage = ? AND StorageID = ?");
        selectZonePetItemStmt.bindInt(1, (int)STORAGE_ZONE);
        selectZonePetItemStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectZonePetItemStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            PetItem* pPetItem = new PetItem();

            pPetItem->setItemID(pResult->getInt(++i));
            pPetItem->setObjectID(pResult->getInt(++i));
            pPetItem->setItemType(pResult->getInt(++i));

            Storage storage = (Storage)pResult->getInt(++i);
            StorageID_t storageID = pResult->getInt(++i);
            BYTE x = pResult->getInt(++i);
            BYTE y = pResult->getInt(++i);

            pPetItem->setCreateType((Item::CreateType)pResult->getInt(++i));

            switch (storage) {
            case STORAGE_ZONE: {
                Tile& pTile = pZone->getTile(x, y);
                Assert(!pTile.hasItem());
                pTile.addItem(pPetItem);
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

void PetItemLoader::load(StorageID_t storageID, Inventory* pInventory)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {}
    END_DB(pStmt)

    __END_CATCH
}

PetItemLoader* g_pPetItemLoader = NULL;
