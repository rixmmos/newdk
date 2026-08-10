//////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItemHandler.cpp

// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGBuyStoreItem.h"

#ifdef __GAME_SERVER__
#include <cstdio>

#include "Assert.h"
#include "DB.h"
#include "GCCreateItem.h"
#include "GCModifyInformation.h"
#include "GCMyStoreInfo.h"
#include "GCNoticeEvent.h"
#include "GCRemoveStoreItem.h"
#include "GCShopSellOK.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "ItemUtil.h"
#include "PacketUtil.h"
#include "PlayerCreature.h"
#include "Store.h"
#include "VariableManager.h"
#include "Zone.h"
#endif

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void CGBuyStoreItemHandler::execute(CGBuyStoreItem* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    GCSystemMessage errorMsg;
    GCNoticeEvent errorNotice;

    if (pPacket->getIndex() >= MAX_ITEM_NUM) {
        filelog("Store.log", "[%s:%s] (%u)  .", pGamePlayer->getID().c_str(), pPC->getName().c_str(),
                pPacket->getIndex());
        return;
    }

    PlayerCreature* pStorePC = dynamic_cast<PlayerCreature*>(pPC->getZone()->getCreature(pPacket->getOwnerObjectID()));
    if (pStorePC == NULL) {
        filelog("Store.log", "[%s:%s] (%u)   .", pGamePlayer->getID().c_str(), pPC->getName().c_str(),
                pPacket->getOwnerObjectID());
        
        errorNotice.setCode(NOTICE_EVENT_CANNOT_FIND_STORE);
        pGamePlayer->sendPacket(&errorNotice);
        return;
    }

    if (pStorePC->getRace() != pPC->getRace()) {
        filelog("Store.log", "[%s:%s] (%s)     .", pGamePlayer->getID().c_str(),
                pPC->getName().c_str(), pStorePC->getName().c_str());
        return;
    }

    Store* pStore = pStorePC->getStore();
    if (pStore == NULL || !pStore->isOpen()) {
        filelog("Store.log", "[%s:%s] (%s)   ..", pGamePlayer->getID().c_str(),
                pPC->getName().c_str(), pStorePC->getName().c_str());
        
        errorNotice.setCode(NOTICE_EVENT_STORE_CLOSED);
        pGamePlayer->sendPacket(&errorNotice);
        return;
    }

    StoreItem& storeItem = pStore->getStoreItem(pPacket->getIndex());
    if (!storeItem.isExists()) {
        filelog("Store.log", "[%s:%s] (%s:%u)    ..", pGamePlayer->getID().c_str(),
                pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex());
        
        errorNotice.setCode(NOTICE_EVENT_ITEM_NOT_FOUND);
        pGamePlayer->sendPacket(&errorNotice);
        return;
    }

    Item* pItem = storeItem.getItem();
    Gold_t price = storeItem.getPrice();

    Assert(pItem != NULL);

    if (pPC->getGold() < price) {
        filelog("Store.log", "[%s:%s] (%s:%u) (%u<%u)  .", pGamePlayer->getID().c_str(),
                pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(), pPC->getGold(), price);
        
        errorNotice.setCode(NOTICE_EVENT_NOT_ENOUGH_MONEY);
        pGamePlayer->sendPacket(&errorNotice);
        return;
    }

    if (pStorePC->getGold() > MAX_MONEY - price) {
        filelog("Store.log", "[%s:%s] (%s:%u) (%u,%u)  .", pGamePlayer->getID().c_str(),
                pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(), pStorePC->getGold(), price);
        
        errorNotice.setCode(NOTICE_EVENT_TOO_MUCH_MONEY);
        pGamePlayer->sendPacket(&errorNotice);
        errorMsg.setMessage(",.");
        pStorePC->getPlayer()->sendPacket(&errorMsg);
        pGamePlayer->sendPacket(&errorMsg);
        return;
    }

    if (pItem->isTimeLimitItem() || !canSell(pItem)) {
        filelog("Store.log", "[%s:%s] (%s:%u) (%s)    .", pGamePlayer->getID().c_str(),
                pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(), pItem->toString().c_str());
        return;
    }

    Inventory* pStoreInventory = pStorePC->getInventory();
    CoordInven_t storeX, storeY;

    Item* pStoreItem = pStoreInventory->findItemOID(pItem->getObjectID(), storeX, storeY);
    if (pStoreItem != pItem) {
        filelog("Store.log", "[%s:%s] (%s:%u) (%p!=%p)      .",
                pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(),
                pStoreItem, pItem);
        errorMsg.setMessage(".");
        pStorePC->getPlayer()->sendPacket(&errorMsg);
        return;
    }

    Inventory* pInventory = pPC->getInventory();
    _TPOINT emptyPos;

    if (!pInventory->getEmptySlot(pItem, emptyPos)) {
        filelog("Store.log", "[%s:%s] (%s:%u)     .", pGamePlayer->getID().c_str(),
                pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex());
        
        errorNotice.setCode(NOTICE_EVENT_NO_INVENTORY_SPACE);
        pGamePlayer->sendPacket(&errorNotice);
        return;
    }

    // The call must NOT live inside Assert(): under NDEBUG that macro expands
    // to ((void)0) and never evaluates its argument, so the listing would not
    // be removed while deleteItem/increaseGoldEx below still run -- the same
    // item then sells again and again. Item duplication, silent, Release-only.
    int removeResult = pStore->removeStoreItem(pPacket->getIndex());
    Assert(removeResult == 0);

    pStoreInventory->deleteItem(storeX, storeY);
    pStorePC->increaseGoldEx(price);

    filelog("StoreBought.log", "[%s:%u/%u]  .", pStorePC->getName().c_str(),
            pItem->getItemClass(), pItem->getItemID());

    GCShopSellOK gcSellOK;
    gcSellOK.setObjectID(pPC->getObjectID());
    gcSellOK.setShopVersion(-1);
    gcSellOK.setItemObjectID(pItem->getObjectID());
    gcSellOK.setPrice(price);
    pStorePC->getPlayer()->sendPacket(&gcSellOK);

    // Hoisted out of Assert() for the same reason: under NDEBUG the buyer would
    // never receive the item while decreaseGoldEx below still charged them.
    bool bAdded = pInventory->addItem(pItem, emptyPos);
    Assert(bAdded);
    pPC->decreaseGoldEx(price);

    char pField[80];
    sprintf(pField, "OwnerID='%s', Storage=%d, X=%d, Y=%d", pPC->getName().c_str(), STORAGE_INVENTORY, emptyPos.x,
            emptyPos.y);
    pItem->tinysave(pField);

    filelog("StoreBought.log", "[%s:%u/%u]  .", pPC->getName().c_str(), pItem->getItemClass(),
            pItem->getItemID());

    if (pItem->isTraceItem()) {
        remainTraceLog(pItem, pStorePC->getName(), pPC->getName(), ITEM_LOG_TRADE, DETAIL_TRADE);
    }

    if (price > g_pVariableManager->getMoneyTraceLogLimit()) {
        remainMoneyTraceLog(pPC->getName(), pStorePC->getName(), ITEM_LOG_TRADE, DETAIL_TRADE, price);
    }

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQuery("INSERT INTO TradeLog (Timeline, Name1, IP1, Name2, IP2, Content) VALUES ('%s', '%s', "
                            "'%s', '%s', '%s', 'Store:[%s(%s)]\n%s\n----\nBuy:[%s(%s)]\nGOLD:%u\n')",
                            VSDateTime::currentDateTime().toString().c_str(), pStorePC->getName().c_str(),
                            pStorePC->getPlayer()->getSocket()->getHost().c_str(), pPC->getName().c_str(),
                            pPC->getPlayer()->getSocket()->getHost().c_str(), pStorePC->getName().c_str(),
                            pStorePC->getPlayer()->getID().c_str(), pItem->toString().c_str(), pPC->getName().c_str(),
                            pPC->getPlayer()->getID().c_str(), price);

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    GCCreateItem gcCreateItem;
    makeGCCreateItem(&gcCreateItem, pItem, emptyPos.x, emptyPos.y);
    pGamePlayer->sendPacket(&gcCreateItem);

    GCModifyInformation gcMI;
    gcMI.addLongData(MODIFY_GOLD, pPC->getGold());
    pGamePlayer->sendPacket(&gcMI);

    GCRemoveStoreItem gcRemoveStoreItem;
    gcRemoveStoreItem.setOwnerObjectID(pStorePC->getObjectID());
    gcRemoveStoreItem.setIndex(pPacket->getIndex());
    pStorePC->getZone()->broadcastPacket(pStorePC->getX(), pStorePC->getY(), &gcRemoveStoreItem, pStorePC);

    GCMyStoreInfo gcInfo;
    gcInfo.setStoreInfo(&(pStore->getStoreInfo()));
    pStorePC->getPlayer()->sendPacket(&gcInfo);

#endif

    __END_DEBUG_EX __END_CATCH
}
