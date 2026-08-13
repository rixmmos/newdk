////////////////////////////////////////////////////////////////////////////////
// Filename    : TradeManager.cpp

// Description :
////////////////////////////////////////////////////////////////////////////////

#include "TradeManager.h"

#include <vector>

#include "Creature.h"
#include "DB.h"
#include "EventItemUtil.h"
#include "FlagSet.h"
#include "GCTradeFinish.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemMap.h"
#include "ItemUtil.h"
#include "LogClient.h"
#include "Ousters.h"
#include "PCFinder.h"
#include "Player.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "VSDateTime.h"
#include "Vampire.h"
#include "VariableManager.h"

////////////////////////////////////////////////////////////////////////////////
// class TradeInfo member methods
////////////////////////////////////////////////////////////////////////////////

TradeInfo::TradeInfo()

{
    __BEGIN_TRY

    m_Gold = 0;
    m_Status = TRADE_TRADING;

    Timeval currentTime;
    getCurrentTime(currentTime);
    m_LastOKTime = currentTime;

    __END_CATCH
}

TradeInfo::~TradeInfo()

{
    __BEGIN_TRY

    m_ItemList.clear();

    __END_CATCH_NO_RETHROW
}

bool TradeInfo::hasItem(Item* pItem)

{
    __BEGIN_TRY

    list<Item*>::const_iterator itr = m_ItemList.begin();
    for (; itr != m_ItemList.end(); itr++) {
        if (pItem == (*itr))
            return true;
    }
    return false;

    __END_CATCH
}

bool TradeInfo::addItem(Item* pItem)

{
    __BEGIN_TRY

    if (hasItem(pItem))
        return false;
    m_ItemList.push_back(pItem);
    return true;

    __END_CATCH
}

bool TradeInfo::removeItem(Item* pItem)

{
    __BEGIN_TRY

    list<Item*>::iterator itr = m_ItemList.begin();
    for (; itr != m_ItemList.end(); itr++) {
        if (pItem == (*itr)) {
            m_ItemList.erase(itr);
            return true;
        }
    }
    return false;

    __END_CATCH
}

void TradeInfo::clearAll(void)

{
    __BEGIN_TRY

    m_ItemList.clear();
    m_Gold = 0;
    m_Status = TRADE_TRADING;

    __END_CATCH
}

void TradeInfo::setNextTime(const Timeval& currentTime) {
    m_LastOKTime.tv_sec = (currentTime.tv_sec + 4);
    m_LastOKTime.tv_usec = currentTime.tv_usec;
}

bool TradeInfo::isValidOKTime(const Timeval& currentTime) {
    if (m_LastOKTime < currentTime)
        return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// class TradeManager member methods
////////////////////////////////////////////////////////////////////////////////

TradeManager::TradeManager()

    {__BEGIN_TRY __END_CATCH}

TradeManager::~TradeManager()

{
    __BEGIN_TRY

    unordered_map<string, TradeInfo*>::iterator itr = m_InfoMap.begin();
    for (; itr != m_InfoMap.end(); itr++) {
        TradeInfo* pInfo = itr->second;
        SAFE_DELETE(pInfo);
    }

    m_InfoMap.clear();

    __END_CATCH_NO_RETHROW
}

void TradeManager::init()

{
    __BEGIN_TRY
    __END_CATCH
}

bool TradeManager::hasTradeInfo(const string& Name)

{
    __BEGIN_TRY

    unordered_map<string, TradeInfo*>::iterator itr = m_InfoMap.find(Name);
    if (itr == m_InfoMap.end())
        return false;
    return true;

    __END_CATCH
}

TradeInfo* TradeManager::getTradeInfo(const string& Name)

{
    __BEGIN_TRY

    unordered_map<string, TradeInfo*>::iterator itr = m_InfoMap.find(Name);
    if (itr == m_InfoMap.end())
        return NULL;
    return itr->second;

    __END_CATCH
}

void TradeManager::addTradeInfo(TradeInfo* pInfo) {
    __BEGIN_TRY

    Assert(pInfo != NULL);
    unordered_map<string, TradeInfo*>::iterator itr = m_InfoMap.find(pInfo->getMainName());
    if (itr != m_InfoMap.end())
        throw DuplicatedException();
    m_InfoMap[pInfo->getMainName()] = pInfo;

    __END_CATCH
}

void TradeManager::removeTradeInfo(const string& Name) {
    __BEGIN_TRY

    unordered_map<string, TradeInfo*>::iterator itr = m_InfoMap.find(Name);
    if (itr == m_InfoMap.end()) {
        cerr << "TradeManager::removeTradeInfo() : NoSuchElementException" << endl;

        // by sigi. 2002.8.31
        // throw NoSuchElementException();
        return;
    }
    TradeInfo* pInfo = itr->second;
    m_InfoMap.erase(itr);
    SAFE_DELETE(pInfo);

    __END_CATCH
}

void TradeManager::initTrade(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY


    if (hasTradeInfo(pCreature1->getName()) || hasTradeInfo(pCreature2->getName())) {
        throw("TradeManager::initTrade() : Trade info already exist!");
    }

    TradeInfo* pInfo1 = new TradeInfo();
    pInfo1->setMainName(pCreature1->getName());
    pInfo1->setTargetName(pCreature2->getName());
    pInfo1->setStatus(TRADE_TRADING);
    addTradeInfo(pInfo1);

    TradeInfo* pInfo2 = new TradeInfo();
    pInfo2->setMainName(pCreature2->getName());
    pInfo2->setTargetName(pCreature1->getName());
    pInfo2->setStatus(TRADE_TRADING);
    addTradeInfo(pInfo2);

    __END_CATCH
}

int TradeManager::canTrade(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    try {
        if (pCreature1 == NULL || pCreature2 == NULL)
            return 0;


        if (!pCreature1->isPC() || !pCreature2->isPC())
            return 0;


        if (!isSameRace(pCreature1, pCreature2))
            return 0;


        if (!isTrading(pCreature1, pCreature2))
            return 0;

        TradeInfo* pInfo1 = getTradeInfo(pCreature1->getName());
        TradeInfo* pInfo2 = getTradeInfo(pCreature2->getName());

        if (pInfo1 == NULL || pInfo2 == NULL) // by sigi. 2002.12.25
            return 0;


        if (pInfo1->getStatus() != TRADE_FINISH || pInfo2->getStatus() != TRADE_FINISH)
            return 0;


        list<Item*> tradeList1 = pInfo1->getItemList();
        list<Item*> tradeList2 = pInfo2->getItemList();
        ItemMap itemMap1;
        ItemMap itemMap2;
        Inventory* pInventory1 = NULL;
        Inventory* pInventory2 = NULL;

        bool bTradeGiftBox = false;
        int EventGiftBoxCount = 0;

        if (pCreature1->isSlayer() && pCreature2->isSlayer()) {
            Slayer* pSlayer1 = dynamic_cast<Slayer*>(pCreature1);
            Slayer* pSlayer2 = dynamic_cast<Slayer*>(pCreature2);

            pInventory1 = new Inventory(pSlayer1->getInventory());
            pInventory2 = new Inventory(pSlayer2->getInventory());
            pInventory1->setDeleteAllFlag(false);
            pInventory2->setDeleteAllFlag(false);
        } else if (pCreature1->isVampire() && pCreature2->isVampire()) {
            Vampire* pVampire1 = dynamic_cast<Vampire*>(pCreature1);
            Vampire* pVampire2 = dynamic_cast<Vampire*>(pCreature2);

            pInventory1 = new Inventory(pVampire1->getInventory());
            pInventory2 = new Inventory(pVampire2->getInventory());
            pInventory1->setDeleteAllFlag(false);
            pInventory2->setDeleteAllFlag(false);
        } else if (pCreature1->isOusters() && pCreature2->isOusters()) {
            Ousters* pOusters1 = dynamic_cast<Ousters*>(pCreature1);
            Ousters* pOusters2 = dynamic_cast<Ousters*>(pCreature2);

            pInventory1 = new Inventory(pOusters1->getInventory());
            pInventory2 = new Inventory(pOusters2->getInventory());
            pInventory1->setDeleteAllFlag(false);
            pInventory2->setDeleteAllFlag(false);
        } else
            throw Error("TradeManager::canTrade() :  !");


        for (list<Item*>::iterator itr = tradeList1.begin(); itr != tradeList1.end(); itr++) {
            Item* pItem = (*itr);
            if (pInventory1->hasItem(pItem->getObjectID())) {
                if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 &&
                    pItem->getItemType() < 6) {
                    /*
                    if ( tradeList1.size() != 1 )
                        goto ErrorCode;
                    */

                    bTradeGiftBox = true;
                }

                if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() >= 16 &&
                    pItem->getItemType() <= 18) {
                    EventGiftBoxCount++;
                }

                pInventory1->deleteItem(pItem->getObjectID());
                itemMap1.addItem(pItem);
            } else
                goto ErrorCode;
        }

        for (list<Item*>::iterator itr = tradeList2.begin(); itr != tradeList2.end(); itr++) {
            Item* pItem = (*itr);
            if (pInventory2->hasItem(pItem->getObjectID())) {
                if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 &&
                    pItem->getItemType() < 6) {
                    /*
                    if ( tradeList2.size() != 1 )
                        goto ErrorCode;
                    */


                    if (!bTradeGiftBox) {
                        SAFE_DELETE(pInventory1);
                        SAFE_DELETE(pInventory2);
                        return 2;
                    }

                    bTradeGiftBox = false;
                }

                if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() >= 16 &&
                    pItem->getItemType() <= 18) {
                    EventGiftBoxCount--;
                }

                pInventory2->deleteItem(pItem->getObjectID());
                itemMap2.addItem(pItem);
            } else
                goto ErrorCode;
        }


        if (bTradeGiftBox || EventGiftBoxCount != 0) {
            SAFE_DELETE(pInventory1);
            SAFE_DELETE(pInventory2);
            return 2;
        }


        for (ItemMap::iterator itr = itemMap1.begin(); itr != itemMap1.end(); itr++) {
            Item* pItem = itr->second;

            if (!pInventory2->addItem(pItem))
                goto ErrorCode;
        }

        for (ItemMap::iterator itr = itemMap2.begin(); itr != itemMap2.end(); itr++) {
            Item* pItem = itr->second;

            if (!pInventory1->addItem(pItem))
                goto ErrorCode;
        }

        SAFE_DELETE(pInventory1);
        SAFE_DELETE(pInventory2);


        return 1;

    ErrorCode:
        SAFE_DELETE(pInventory1);
        SAFE_DELETE(pInventory2);
        return 0;

    } catch (Throwable& t) { // by sigi. 2002.12.25

        filelog("tradeError.txt", "C1=%s, C2=%s, %s", pCreature1->getName().c_str(), pCreature2->getName().c_str(),
                t.toString().c_str());


        return 0;
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// Trade commit helpers
//
// processTrade() moves items by hand between two live inventories. Each step
// below records what it did, so a swap that cannot be completed can be undone
// instead of being abandoned half-applied: the old code threw from the middle
// of the move, leaving the staked items detached from both inventories and
// owned by nobody.
////////////////////////////////////////////////////////////////////////////////

namespace {

// One item and the inventory slot it currently occupies.
struct TradeItemSlot {
    Item* pItem;
    Inventory* pInventory;
    CoordInven_t x;
    CoordInven_t y;

    TradeItemSlot() : pItem(NULL), pInventory(NULL), x(0), y(0) {}
};

// Take pItem out of pInventory, remembering the slot it came from. Returns
// false, having changed nothing, if the item is not actually there.
bool detachTradeItem(Inventory* pInventory, Item* pItem, vector<TradeItemSlot>& slots) {
    if (pInventory == NULL || pItem == NULL)
        return false;

    CoordInven_t x = 0;
    CoordInven_t y = 0;

    if (pInventory->findItemOID(pItem->getObjectID(), x, y) != pItem)
        return false;

    TradeItemSlot slot;
    slot.pItem = pItem;
    slot.pInventory = pInventory;
    slot.x = x;
    slot.y = y;
    slots.push_back(slot);

    pInventory->deleteItem(x, y);
    return true;
}

// Put pItem into pInventory, remembering where it landed. Returns false,
// having changed nothing, if there is no room.
bool placeTradeItem(Inventory* pInventory, Item* pItem, vector<TradeItemSlot>& slots) {
    if (pInventory == NULL || pItem == NULL)
        return false;

    TPOINT pt;
    pt.x = 0;
    pt.y = 0;

    if (!pInventory->addItem(pItem, pt))
        return false;

    TradeItemSlot slot;
    slot.pItem = pItem;
    slot.pInventory = pInventory;
    slot.x = (CoordInven_t)pt.x;
    slot.y = (CoordInven_t)pt.y;
    slots.push_back(slot);

    return true;
}

// Undo a half-finished swap: pull back everything that was placed, then return
// everything that was detached to the slot it came from. Both lists are walked
// newest first, so every slot is free again by the time it is refilled.
void rollbackTradeItems(vector<TradeItemSlot>& placed, vector<TradeItemSlot>& detached) {
    for (vector<TradeItemSlot>::reverse_iterator itr = placed.rbegin(); itr != placed.rend(); ++itr)
        itr->pInventory->deleteItem(itr->x, itr->y);

    placed.clear();

    for (vector<TradeItemSlot>::reverse_iterator itr = detached.rbegin(); itr != detached.rend(); ++itr) {
        if (itr->pInventory->addItem(itr->x, itr->y, itr->pItem))
            continue;

        // Unreachable: the original slot was free a moment ago and nothing else
        // touched these inventories in between. Fall back to any free slot
        // rather than let the item fall out of the world, and log it, because a
        // broken invariant here is worth seeing.
        if (!itr->pInventory->addItem(itr->pItem))
            filelog("tradeError.txt", "[rollback] could not restore item OID %u",
                    (unsigned int)itr->pItem->getObjectID());
    }

    detached.clear();
}

// Credit gold to a character who is no longer in this process, with the same
// direct Gold update the guild refund path uses (GSQuitGuildHandler). Called
// when a trade is cancelled after the partner has gone: the escrow is about to
// be deleted, so the gold has to go back to the stored character or it is
// simply destroyed.
void refundStoredTradeGold(Creature* pPC, const string& targetName, Gold_t gold) {
    if (pPC == NULL || gold == 0)
        return;

    // Trades are same-race only -- CGTradePrepareHandler rejects a mixed pair
    // before initTrade() -- so the partner's table is the caller's table.
    string table;
    if (pPC->isSlayer())
        table = "Slayer";
    else if (pPC->isVampire())
        table = "Vampire";
    else if (pPC->isOusters())
        table = "Ousters";

    if (table.empty()) {
        filelog("tradeError.txt", "[2] cannot refund %u gold to absent partner [%s] : unknown race", (unsigned int)gold,
                targetName.c_str());
        return;
    }

    Statement* pStmt = NULL;
    bool bRefunded = false;

    // END_DB rethrows a query failure as a const char*, and cancelTrade() runs
    // from the character destructors and the logout path, where an escaping
    // exception is fatal. Contain it here.
    try {
        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            // table is one of the three fixed literals above and is never
            // user-controlled, so splicing it into the SQL text is safe; the
            // amount, the cap and the name stay bound parameters.
            PreparedStatement goldStmt(pConn, "UPDATE " + table + " SET Gold = LEAST(Gold + ?, ?) WHERE Name = ?");
            goldStmt.bindUInt(1, gold);
            goldStmt.bindUInt(2, (uint)MAX_MONEY);
            goldStmt.bindString(3, targetName);
            goldStmt.execute();
        }
        END_DB(pStmt)

        bRefunded = true;
    } catch (...) {
        // Already logged to DBError.log by END_DB.
    }

    filelog("tradeError.txt", "[2] %s %u escrowed gold to absent trade partner [%s]",
            bRefunded ? "refunded" : "FAILED to refund", (unsigned int)gold, targetName.c_str());
}

} // namespace

void TradeManager::processTrade(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    // canTrade() returns 1 for "go ahead", 2 for a rejected event-gift-box
    // pairing and 0 for everything else. The test used to be !canTrade(), which
    // let 2 through -- the one return value the swap below cannot honour.
    if (canTrade(pCreature1, pCreature2) != 1) {
        throw("TradeManager::processTrade() : the trade is no longer valid!");
    }


    TradeInfo* pInfo1 = getTradeInfo(pCreature1->getName());
    TradeInfo* pInfo2 = getTradeInfo(pCreature2->getName());
    list<Item*> tradeList1 = pInfo1->getItemList();
    list<Item*> tradeList2 = pInfo2->getItemList();
    ItemMap itemMap1;
    ItemMap itemMap2;
    Gold_t tradeGold1 = pInfo1->getGold();
    Gold_t tradeGold2 = pInfo2->getGold();
    Inventory* pInventory1 = NULL;
    Inventory* pInventory2 = NULL;
    Slayer* pSlayer1 = NULL;
    Slayer* pSlayer2 = NULL;
    Vampire* pVampire1 = NULL;
    Vampire* pVampire2 = NULL;
    Ousters* pOusters1 = NULL;
    Ousters* pOusters2 = NULL;
    PlayerCreature* pPlayerCreature1 = dynamic_cast<PlayerCreature*>(pCreature1);
    PlayerCreature* pPlayerCreature2 = dynamic_cast<PlayerCreature*>(pCreature2);

    bool check1 = pPlayerCreature1->checkDBGold(pPlayerCreature1->getGold() + tradeGold1);
    bool check2 = pPlayerCreature2->checkDBGold(pPlayerCreature2->getGold() + tradeGold2);

    if (!check1) {
        filelog("GoldBug.log", "TradeManager::processTrade :   . [%s:%s]", pPlayerCreature1->getName().c_str(),
                pPlayerCreature1->getPlayer()->getID().c_str());
    }

    if (!check2) {
        filelog("GoldBug.log", "TradeManager::processTrade :   . [%s:%s]", pPlayerCreature2->getName().c_str(),
                pPlayerCreature2->getPlayer()->getID().c_str());
    }

    if (!check1 || !check2) {
        GamePlayer* pGamePlayer1 = dynamic_cast<GamePlayer*>(pPlayerCreature1->getPlayer());
        GamePlayer* pGamePlayer2 = dynamic_cast<GamePlayer*>(pPlayerCreature2->getPlayer());

        if (pGamePlayer1 != NULL) {
            pGamePlayer1->setPenaltyFlag(PENALTY_TYPE_KICKED);
            pGamePlayer1->setItemRatioBonusPoint(1);
        }

        if (pGamePlayer2 != NULL) {
            pGamePlayer2->setPenaltyFlag(PENALTY_TYPE_KICKED);
            pGamePlayer2->setItemRatioBonusPoint(1);
        }

        return;
    }


    bool bTradeGiftBox = false;
    int giftBoxType1 = 0;
    int giftBoxType2 = 0;

    if (pCreature1->isSlayer() && pCreature2->isSlayer()) {
        pSlayer1 = dynamic_cast<Slayer*>(pCreature1);
        pSlayer2 = dynamic_cast<Slayer*>(pCreature2);

        pInventory1 = pSlayer1->getInventory();
        pInventory2 = pSlayer2->getInventory();
    } else if (pCreature1->isVampire() && pCreature2->isVampire()) {
        pVampire1 = dynamic_cast<Vampire*>(pCreature1);
        pVampire2 = dynamic_cast<Vampire*>(pCreature2);

        pInventory1 = pVampire1->getInventory();
        pInventory2 = pVampire2->getInventory();
    } else if (pCreature1->isOusters() && pCreature2->isOusters()) {
        pOusters1 = dynamic_cast<Ousters*>(pCreature1);
        pOusters2 = dynamic_cast<Ousters*>(pCreature2);

        pInventory1 = pOusters1->getInventory();
        pInventory2 = pOusters2->getInventory();
    } else
        throw("TradeManager::processTrade() :      !");

    // Pass 1 -- inspect only. Every reason to refuse the trade is found before a
    // single item leaves an inventory. The gift-box pairing in particular used
    // to be resolved in the middle of the swap, where a refusal threw with both
    // inventories already emptied and the staked items owned by nobody.
    for (list<Item*>::iterator itr = tradeList1.begin(); itr != tradeList1.end(); itr++) {
        Item* pItem = (*itr);
        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 &&
            pItem->getItemType() < 6) {
            bTradeGiftBox = true;
            giftBoxType1 = pItem->getItemType();
        }
    }
    for (list<Item*>::iterator itr = tradeList2.begin(); itr != tradeList2.end(); itr++) {
        Item* pItem = (*itr);
        if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 &&
            pItem->getItemType() < 6) {
            // A black gift box is only made by trading one gift box for
            // another, so a one-sided stake is refused.
            if (!bTradeGiftBox)
                throw("TradeManager::processTrade() : an event gift box was staked on one side only!");

            giftBoxType2 = pItem->getItemType();
        }
    }

    // Resolve the combined box up front. getBlackGiftBoxType() returns 0 for any
    // pairing it does not know -- including the one-sided case, where
    // giftBoxType2 is still 0 -- and that used to be a throw from the middle of
    // the swap.
    int blackGiftBoxType = 0;
    if (bTradeGiftBox) {
        blackGiftBoxType = getBlackGiftBoxType(giftBoxType1, giftBoxType2);
        if (blackGiftBoxType == 0)
            throw("TradeManager::processTrade() : the two staked event gift boxes do not combine!");
    }

    // Pass 2 -- detach every staked item from its owner, recording where it came
    // from. Nothing is destroyed on the failure path: the whole move is undone.
    vector<TradeItemSlot> detached;
    vector<TradeItemSlot> placed;

    for (list<Item*>::iterator itr = tradeList1.begin(); itr != tradeList1.end(); itr++) {
        Item* pItem = (*itr);
        if (!detachTradeItem(pInventory1, pItem, detached) || !itemMap1.addItem(pItem)) {
            rollbackTradeItems(placed, detached);
            throw("TradeManager::processTrade() : a staked item is no longer in its owner's inventory!");
        }
    }
    for (list<Item*>::iterator itr = tradeList2.begin(); itr != tradeList2.end(); itr++) {
        Item* pItem = (*itr);
        if (!detachTradeItem(pInventory2, pItem, detached) || !itemMap2.addItem(pItem)) {
            rollbackTradeItems(placed, detached);
            throw("TradeManager::processTrade() : a staked item is no longer in its owner's inventory!");
        }
    }

    // Pass 3 -- place every detached item with its new owner. Still undoable.
    for (ItemMap::iterator itr = itemMap1.begin(); itr != itemMap1.end(); itr++) {
        if (!placeTradeItem(pInventory2, itr->second, placed)) {
            rollbackTradeItems(placed, detached);
            throw("TradeManager::processTrade() : not enough inventory space to complete the trade.");
        }
    }
    for (ItemMap::iterator itr = itemMap2.begin(); itr != itemMap2.end(); itr++) {
        if (!placeTradeItem(pInventory1, itr->second, placed)) {
            rollbackTradeItems(placed, detached);
            throw("TradeManager::processTrade() : not enough inventory space to complete the trade.");
        }
    }

    // ------------------------------------------------------------------------
    // Commit point. Both inventories now hold the other side's items and nothing
    // below can fail the trade, so the ownership callbacks and the item-type
    // rewrites can run without any risk of stranding an item.
    // ------------------------------------------------------------------------

    // whenPCLost() before whenPCTake(), and in stake order, exactly as before.
    for (list<Item*>::iterator itr = tradeList1.begin(); itr != tradeList1.end(); itr++)
        (*itr)->whenPCLost(pPlayerCreature1);
    for (list<Item*>::iterator itr = tradeList2.begin(); itr != tradeList2.end(); itr++)
        (*itr)->whenPCLost(pPlayerCreature2);


    for (ItemMap::iterator itr = itemMap1.begin(); itr != itemMap1.end(); itr++) {
        Item* pItem = itr->second;

        pItem->whenPCTake(pPlayerCreature2);

        if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 0) {
            pItem->setItemType(1);


            FlagSet* pFlagSet = pPlayerCreature2->getFlagSet();
            Assert(pFlagSet != NULL);
            pFlagSet->turnOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX);
            pFlagSet->save(pCreature2->getName());
        }

        // Resolved before anything moved, so it is never 0 here. The guard is
        // kept only so an NDEBUG build cannot write a 0 item type if that ever
        // stops holding -- a wrong box beats a destroyed trade.
        if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 &&
            pItem->getItemType() < 6) {
            Assert(blackGiftBoxType != 0);

            if (blackGiftBoxType != 0)
                pItem->setItemType(blackGiftBoxType);
        }

        if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() >= 16 &&
            pItem->getItemType() <= 18) {
            pItem->setItemType(pItem->getItemType() + 3);
        }


        if (pItem != NULL && pItem->isTraceItem()) {
            remainTraceLog(pItem, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_TRADE, DETAIL_TRADE);
            remainTraceLogNew(pItem, pCreature1->getName(), ITL_DROP, ITLD_TRADE);
            remainTraceLogNew(pItem, pCreature2->getName(), ITL_GET, ITLD_TRADE);
        }
    }
    for (ItemMap::iterator itr = itemMap2.begin(); itr != itemMap2.end(); itr++) {
        Item* pItem = itr->second;

        pItem->whenPCTake(pPlayerCreature1);

        if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 0) {
            pItem->setItemType(1);


            FlagSet* pFlagSet = pPlayerCreature1->getFlagSet();
            Assert(pFlagSet != NULL);
            pFlagSet->turnOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX);
            pFlagSet->save(pCreature1->getName());
        }

        // See the note on the matching block above.
        if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 &&
            pItem->getItemType() < 6) {
            Assert(blackGiftBoxType != 0);

            if (blackGiftBoxType != 0)
                pItem->setItemType(blackGiftBoxType);
        }

        if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() >= 16 &&
            pItem->getItemType() <= 18) {
            pItem->setItemType(pItem->getItemType() + 3);
        }


        if (pItem != NULL && pItem->isTraceItem()) {
            remainTraceLog(pItem, pCreature2->getName(), pCreature1->getName(), ITEM_LOG_TRADE, DETAIL_TRADE);
            remainTraceLogNew(pItem, pCreature2->getName(), ITL_DROP, ITLD_TRADE);
            remainTraceLogNew(pItem, pCreature1->getName(), ITL_GET, ITLD_TRADE);
        }
    }


    pInventory1->save(pCreature1->getName());
    pInventory2->save(pCreature2->getName());


    if (pCreature1->isSlayer()) {
        pSlayer1->setGoldEx(pSlayer1->getGold() + tradeGold2);
        pSlayer2->setGoldEx(pSlayer2->getGold() + tradeGold1);
    } else if (pCreature2->isVampire()) {
        pVampire1->setGoldEx(pVampire1->getGold() + tradeGold2);
        pVampire2->setGoldEx(pVampire2->getGold() + tradeGold1);
    } else if (pCreature2->isOusters()) {
        pOusters1->setGoldEx(pOusters1->getGold() + tradeGold2);
        pOusters2->setGoldEx(pOusters2->getGold() + tradeGold1);
    }


    string ip1 = pCreature1->getPlayer()->getSocket()->getHost();
    string ip2 = pCreature2->getPlayer()->getSocket()->getHost();
    StringStream msg;
    msg << "[" << pCreature1->getName() << "(" << pPlayerCreature1->getPlayer()->getID() << ")] " << "\n";
    msg << "GOLD:" << tradeGold1 << "\n";
    for (ItemMap::iterator itr = itemMap1.begin(); itr != itemMap1.end(); itr++)
        msg << itr->second->toString() << "\n";
    msg << "----" << "\n";
    msg << "[" << pCreature2->getName() << "(" << pPlayerCreature2->getPlayer()->getID() << ")] " << "\n";
    msg << "GOLD:" << tradeGold2 << "\n";
    for (ItemMap::iterator itr = itemMap2.begin(); itr != itemMap2.end(); itr++)
        msg << itr->second->toString() << "\n";


    if (tradeGold1 >= g_pVariableManager->getMoneyTraceLogLimit()) {
        remainMoneyTraceLog(pCreature1->getName(), pCreature2->getName(), ITEM_LOG_TRADE, DETAIL_TRADE, tradeGold1);
    }

    if (tradeGold2 >= g_pVariableManager->getMoneyTraceLogLimit()) {
        remainMoneyTraceLog(pCreature2->getName(), pCreature1->getName(), ITEM_LOG_TRADE, DETAIL_TRADE, tradeGold2);
    }

    // log(LOG_TRADE, pCreature1->getName(), pCreature2->getName(), msg.toString());

    Statement* pStmt = NULL;

    BEGIN_DB {
        StringStream SQL;
        SQL << "INSERT INTO TradeLog (Timeline, Name1, IP1, Name2, IP2, Content) VALUES ("
            << "'" << VSDateTime::currentDateTime().toString() << "',"
            << "'" << pCreature1->getName() << "',"
            << "'" << ip1 << "',"
            << "'" << pCreature2->getName() << "',"
            << "'" << ip2 << "',"
            << "'" << msg.toString() << "'"
            << ")";

        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQueryString(SQL.toString());

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);


    // removeTradeInfo(pCreature1->getObjectID());
    // removeTradeInfo(pCreature2->getObjectID());
    pInfo1->clearAll();
    pInfo2->clearAll();

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void TradeManager::cancelTrade(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    try {
        int nCondition = 0;


        if (pCreature1 == NULL || pCreature2 == NULL)
            nCondition = 1;


        if (!pCreature1->isPC() || !pCreature2->isPC())
            nCondition = 2;


        if (!isSameRace(pCreature1, pCreature2))
            nCondition = 3;


        if (!isTrading(pCreature1, pCreature2))
            nCondition = 4;

        if (nCondition != 0) {
            StringStream msg;
            msg << "TradeManager::cancelTrade()  ... CODE(" << nCondition << ")";
            filelog("tradeError.txt", "[1] %s", msg.toString().c_str());


            return;
        }

        TradeInfo* pInfo1 = getTradeInfo(pCreature1->getName());
        TradeInfo* pInfo2 = getTradeInfo(pCreature2->getName());


        if (pCreature1->isSlayer()) {
            Slayer* pSlayer1 = dynamic_cast<Slayer*>(pCreature1);
            if (pInfo1)
                pSlayer1->setGold(pSlayer1->getGold() + pInfo1->getGold());
        } else if (pCreature1->isVampire()) {
            Vampire* pVampire1 = dynamic_cast<Vampire*>(pCreature1);
            if (pInfo1)
                pVampire1->setGold(pVampire1->getGold() + pInfo1->getGold());
        } else if (pCreature1->isOusters()) {
            Ousters* pOusters1 = dynamic_cast<Ousters*>(pCreature1);
            if (pInfo1)
                pOusters1->setGold(pOusters1->getGold() + pInfo1->getGold());
        }


        if (pCreature2->isSlayer()) {
            Slayer* pSlayer2 = dynamic_cast<Slayer*>(pCreature2);
            if (pInfo2)
                pSlayer2->setGold(pSlayer2->getGold() + pInfo2->getGold());
        } else if (pCreature2->isVampire()) {
            Vampire* pVampire2 = dynamic_cast<Vampire*>(pCreature2);
            if (pInfo2)
                pVampire2->setGold(pVampire2->getGold() + pInfo2->getGold());
        } else if (pCreature2->isOusters()) {
            Ousters* pOusters2 = dynamic_cast<Ousters*>(pCreature2);
            if (pInfo2)
                pOusters2->setGold(pOusters2->getGold() + pInfo2->getGold());
        }


        if (pInfo1)
            removeTradeInfo(pCreature1->getName());
        if (pInfo2)
            removeTradeInfo(pCreature2->getName());

    } catch (Throwable& t) {
        filelog("tradeError.txt", "[1] %s", t.toString().c_str());
        // throw Error(t.toString().c_str());
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void TradeManager::cancelTrade(Creature* pPC)

{
    __BEGIN_TRY

    try {
        if (pPC == NULL)
            return;

        TradeInfo* pInfo1 = getTradeInfo(pPC->getName());
        TradeInfo* pInfo2 = NULL;
        string TargetName;
        Creature* pTargetPC = NULL;
        Zone* pZone = pPC->getZone();

        if (pInfo1 != NULL) {
            pInfo2 = getTradeInfo(pInfo1->getTargetName());
            TargetName = pInfo1->getTargetName();


            if (pPC->isSlayer()) {
                Slayer* pSlayer1 = dynamic_cast<Slayer*>(pPC);
                pSlayer1->setGold(pSlayer1->getGold() + pInfo1->getGold());
            } else if (pPC->isVampire()) {
                Vampire* pVampire1 = dynamic_cast<Vampire*>(pPC);
                pVampire1->setGold(pVampire1->getGold() + pInfo1->getGold());
            } else if (pPC->isOusters()) {
                Ousters* pOusters1 = dynamic_cast<Ousters*>(pPC);
                pOusters1->setGold(pOusters1->getGold() + pInfo1->getGold());
            }

            // The partner's escrowed gold has to come back too, and
            // removeTradeInfo() below deletes the escrow whether or not it did.
            // The zone lookup alone was not enough: a partner who left the zone
            // without their own cancelTrade() running is simply absent here, and
            // their gold used to be destroyed at that point. Widen the search to
            // the whole process, then fall back to a direct credit on the stored
            // character.
            Creature* pInZonePC = (pZone != NULL) ? pZone->getCreature(TargetName) : NULL;

            pTargetPC = pInZonePC;
            if (pTargetPC == NULL || !pTargetPC->isPC())
                pTargetPC = g_pPCFinder->getCreature(TargetName);

            if (pInfo2 != NULL) {
                if (pTargetPC != NULL && pTargetPC->isPC()) {
                    if (pTargetPC->isSlayer()) {
                        Slayer* pSlayer2 = dynamic_cast<Slayer*>(pTargetPC);
                        pSlayer2->setGold(pSlayer2->getGold() + pInfo2->getGold());
                    } else if (pTargetPC->isVampire()) {
                        Vampire* pVampire2 = dynamic_cast<Vampire*>(pTargetPC);
                        pVampire2->setGold(pVampire2->getGold() + pInfo2->getGold());
                    } else if (pTargetPC->isOusters()) {
                        Ousters* pOusters2 = dynamic_cast<Ousters*>(pTargetPC);
                        pOusters2->setGold(pOusters2->getGold() + pInfo2->getGold());
                    }
                } else {
                    // Not in this process any more.
                    refundStoredTradeGold(pPC, TargetName, pInfo2->getGold());
                }
            }

            // Tell the partner's client the trade is off -- but only if they are
            // still in this zone. One who has already left has torn the trade
            // window down and would have no object to attach this to.
            if (pInZonePC != NULL && pInZonePC->isPC() && pInfo2 != NULL) {
                GCTradeFinish gcTradeFinish;
                gcTradeFinish.setTargetObjectID(pPC->getObjectID());
                gcTradeFinish.setCode(GC_TRADE_FINISH_REJECT);

                Player* pTargetPlayer = pInZonePC->getPlayer();
                if (pTargetPlayer != NULL)
                    pTargetPlayer->sendPacket(&gcTradeFinish);
            }


            removeTradeInfo(pPC->getName());
            removeTradeInfo(TargetName);
        }

    } catch (Throwable& t) {
        filelog("tradeError.txt", "[2] %s", t.toString().c_str());
        // throw Error(t.toString().c_str());
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool TradeManager::isTrading(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    TradeInfo* pInfo1 = getTradeInfo(pCreature1->getName());
    TradeInfo* pInfo2 = getTradeInfo(pCreature2->getName());

    if (pInfo1 == NULL || pInfo2 == NULL)
        return false;

    if ((pInfo1->getMainName() == pInfo2->getTargetName()) && (pInfo2->getMainName() == pInfo1->getTargetName()))
        return true;

    return false;

    __END_CATCH
}
