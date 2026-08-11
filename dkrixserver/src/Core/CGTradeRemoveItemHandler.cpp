//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeRemoveItemHandler.cpp

// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGTradeRemoveItem.h"

#ifdef __GAME_SERVER__
#include "CheckedCast.h"
#include "GCTradeError.h"
#include "GCTradeRemoveItem.h"
#include "GCTradeVerify.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "Ousters.h"
#include "Slayer.h"
#include "TradeManager.h"
#include "Vampire.h"
#include "ZoneUtil.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeRemoveItemHandler::execute(CGTradeRemoveItem* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    ObjectID_t TargetOID = pPacket->getTargetObjectID();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);

    Creature* pPC = pGamePlayer->getCreature();
    Assert(pPC != NULL);

    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    Creature* pTargetPC = NULL;
    /*
    try { pTargetPC = pZone->getCreature(TargetOID); }
    catch (NoSuchElementException) { pTargetPC = NULL; }
    */

    
    pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL) {
        pTradeManager->cancelTrade(pPC);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_TARGET_NOT_EXIST);
        return;
    }

    
    if (!pTargetPC->isPC() || !isSameRace(pTargetPC, pPC)) {
        pTradeManager->cancelTrade(pPC);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_RACE_DIFFER);
        return;
    }

    
    if (!isInSafeZone(pPC) || !isInSafeZone(pTargetPC)) {
        pTradeManager->cancelTrade(pPC);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_SAFE);
        return;
    }

    
    if (pPC->isSlayer() && pTargetPC->isSlayer()) {
        Slayer* pSlayer1 = checkedCast<Slayer*>(pPC);
        Slayer* pSlayer2 = checkedCast<Slayer*>(pTargetPC);

        if (pSlayer1->hasRideMotorcycle() || pSlayer2->hasRideMotorcycle()) {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_MOTORCYCLE);
            return;
        }
    }

    if (pPC->isOusters() && pTargetPC->isOusters()) {
        Ousters* pOusters1 = checkedCast<Ousters*>(pPC);
        Ousters* pOusters2 = checkedCast<Ousters*>(pTargetPC);

        if (pOusters1->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) ||
            pOusters2->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)) {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_MOTORCYCLE);
            return;
        }
    }

    
    if (!pTradeManager->isTrading(pPC, pTargetPC)) {
        pTradeManager->cancelTrade(pPC);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_TRADING);
        return;
    }

    if (pPC->isSlayer())
        executeSlayer(pPacket, pPlayer);
    else if (pPC->isVampire())
        executeVampire(pPacket, pPlayer);
    else if (pPC->isOusters())
        executeOusters(pPacket, pPlayer);
    else
        throw ProtocolException("CGTradeRemoveItem::execute() :     ");

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeRemoveItemHandler::executeSlayer(CGTradeRemoveItem* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    ObjectID_t ItemOID = pPacket->getItemObjectID();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Slayer* pSender = checkedCast<Slayer*>(pPC);

    
    CoordInven_t X, Y;
    Inventory* pInventory = pSender->getInventory();
    Item* pItem = pInventory->findItemOID(ItemOID, X, Y);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    if (pItem == NULL) {
        pTradeManager->cancelTrade(pPC);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_REMOVE_ITEM);
        return;
    }

    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pTargetPC->getName());

    
    // Assert(pInfo1->removeItem(pItem));
    pInfo1->removeItem(pItem);

    Timeval currentTime;
    getCurrentTime(currentTime);
    pInfo1->setNextTime(currentTime);

    
    if (pInfo1->getStatus() == TRADE_FINISH) {
        
        // << endl;

        
        GCTradeVerify gcTradeVerify;
        gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_REMOVE_ITEM);
        pPlayer->sendPacket(&gcTradeVerify);
    } else {
        
        
    }

    
    
    pInfo1->setStatus(TRADE_TRADING);
    pInfo2->setStatus(TRADE_TRADING);

    
    GCTradeRemoveItem gcTradeRemoveItem;
    gcTradeRemoveItem.setTargetObjectID(pSender->getObjectID());
    gcTradeRemoveItem.setItemObjectID(pItem->getObjectID());

    
    Player* pTargetPlayer = pTargetPC->getPlayer();
    pTargetPlayer->sendPacket(&gcTradeRemoveItem);

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeRemoveItemHandler::executeVampire(CGTradeRemoveItem* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    ObjectID_t ItemOID = pPacket->getItemObjectID();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Vampire* pSender = checkedCast<Vampire*>(pPC);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    CoordInven_t X, Y;
    Inventory* pInventory = pSender->getInventory();
    Item* pItem = pInventory->findItemOID(ItemOID, X, Y);

    
    if (pItem == NULL) {
        pTradeManager->cancelTrade(pPC);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_REMOVE_ITEM);
        return;
    }

    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pTargetPC->getName());

    
    // Assert(pInfo1->removeItem(pItem));
    pInfo1->removeItem(pItem);

    
    if (pInfo1->getStatus() == TRADE_FINISH) {
        
        // << endl;

        
        GCTradeVerify gcTradeVerify;
        gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_REMOVE_ITEM);
        pPlayer->sendPacket(&gcTradeVerify);
    } else {
        
        
    }

    
    
    pInfo1->setStatus(TRADE_TRADING);
    pInfo2->setStatus(TRADE_TRADING);

    
    GCTradeRemoveItem gcTradeRemoveItem;
    gcTradeRemoveItem.setTargetObjectID(pSender->getObjectID());
    gcTradeRemoveItem.setItemObjectID(pItem->getObjectID());

    
    Player* pTargetPlayer = pTargetPC->getPlayer();
    pTargetPlayer->sendPacket(&gcTradeRemoveItem);

#endif

    __END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeRemoveItemHandler::executeOusters(CGTradeRemoveItem* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    ObjectID_t ItemOID = pPacket->getItemObjectID();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Ousters* pSender = checkedCast<Ousters*>(pPC);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    CoordInven_t X, Y;
    Inventory* pInventory = pSender->getInventory();
    Item* pItem = pInventory->findItemOID(ItemOID, X, Y);

    
    if (pItem == NULL) {
        pTradeManager->cancelTrade(pPC);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_REMOVE_ITEM);
        return;
    }

    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pTargetPC->getName());

    
    // Assert(pInfo1->removeItem(pItem));
    pInfo1->removeItem(pItem);

    
    if (pInfo1->getStatus() == TRADE_FINISH) {
        
        // << endl;

        
        GCTradeVerify gcTradeVerify;
        gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_REMOVE_ITEM);
        pPlayer->sendPacket(&gcTradeVerify);
    } else {
        
        
    }

    
    
    pInfo1->setStatus(TRADE_TRADING);
    pInfo2->setStatus(TRADE_TRADING);

    
    GCTradeRemoveItem gcTradeRemoveItem;
    gcTradeRemoveItem.setTargetObjectID(pSender->getObjectID());
    gcTradeRemoveItem.setItemObjectID(pItem->getObjectID());

    
    Player* pTargetPlayer = pTargetPC->getPlayer();
    pTargetPlayer->sendPacket(&gcTradeRemoveItem);

#endif

    __END_DEBUG_EX __END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeRemoveItemHandler::executeError(CGTradeRemoveItem* pPacket, Player* pPlayer, BYTE ErrorCode)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        GCTradeError gcTradeError;
    gcTradeError.setTargetObjectID(pPacket->getTargetObjectID());
    gcTradeError.setCode(ErrorCode);
    pPlayer->sendPacket(&gcTradeError);

#endif

    __END_DEBUG_EX __END_CATCH
}
