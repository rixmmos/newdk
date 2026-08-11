//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeMoneyHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGTradeMoney.h"

#ifdef __GAME_SERVER__
#include "CheckedCast.h"
#include "GCTradeError.h"
#include "GCTradeMoney.h"
#include "GCTradeVerify.h"
#include "GamePlayer.h"
#include "Ousters.h"
#include "Slayer.h"
#include "TradeManager.h"
#include "Vampire.h"
#include "ZoneUtil.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeMoneyHandler::execute(CGTradeMoney* pPacket, Player* pPlayer)

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
        throw ProtocolException("CGTradeMoney::execute() :     ");

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeMoneyHandler::executeSlayer(CGTradeMoney* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    Gold_t Amount = pPacket->getAmount();
    BYTE Code = pPacket->getCode();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Slayer* pSender = checkedCast<Slayer*>(pPC);
    Slayer* pReceiver = checkedCast<Slayer*>(pTargetPC);
    Player* pTargetPlayer = pTargetPC->getPlayer();
    Gold_t finalAmount = Amount;
    Gold_t margin = 0;
    GCTradeMoney gcTradeMoney;
    GCTradeVerify gcTradeVerify;

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pReceiver->getName());

    
    if (Code == CG_TRADE_MONEY_INCREASE) {
        
        if (pSender->getGold() >= Amount) {
            
            
            Gold_t receiverGold = pReceiver->getGold() + pInfo1->getGold();
            if (receiverGold + Amount > MAX_MONEY) {
                margin = receiverGold + Amount - MAX_MONEY;
                finalAmount = finalAmount - margin;
            }

            
            pSender->setGold(pSender->getGold() - finalAmount);
            pInfo1->setGold(pInfo1->getGold() + finalAmount);

            
            if (pInfo1->getStatus() == TRADE_FINISH) {
                
                gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_MONEY_INCREASE);
                pPlayer->sendPacket(&gcTradeVerify);
            }

            
            pInfo1->setStatus(TRADE_TRADING);
            pInfo2->setStatus(TRADE_TRADING);

            
            
            gcTradeMoney.setTargetObjectID(TargetOID);
            gcTradeMoney.setCode(GC_TRADE_MONEY_INCREASE_RESULT);
            gcTradeMoney.setAmount(finalAmount);
            pPlayer->sendPacket(&gcTradeMoney);

            
            gcTradeMoney.setTargetObjectID(pSender->getObjectID());
            gcTradeMoney.setCode(GC_TRADE_MONEY_INCREASE);
            gcTradeMoney.setAmount(finalAmount);
            pTargetPlayer->sendPacket(&gcTradeMoney);
        } else {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_INCREASE_MONEY);
            return;
        }
    }
    
    else if (Code == CG_TRADE_MONEY_DECREASE) {
        
        if (pInfo1->getGold() >= Amount) {
            
            Gold_t senderGold = pSender->getGold() + pInfo2->getGold();
            if (senderGold + Amount > MAX_MONEY) {
                margin = senderGold + Amount - MAX_MONEY;
                finalAmount = finalAmount - margin;
            }

            
            pSender->setGold(pSender->getGold() + finalAmount);
            pInfo1->setGold(pInfo1->getGold() - finalAmount);

            
            if (pInfo1->getStatus() == TRADE_FINISH) {
                
                gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_MONEY_DECREASE);
                pPlayer->sendPacket(&gcTradeVerify);
            }

            
            pInfo1->setStatus(TRADE_TRADING);
            pInfo2->setStatus(TRADE_TRADING);

            
            
            gcTradeMoney.setTargetObjectID(TargetOID);
            gcTradeMoney.setCode(GC_TRADE_MONEY_DECREASE_RESULT);
            gcTradeMoney.setAmount(finalAmount);
            pPlayer->sendPacket(&gcTradeMoney);

            
            gcTradeMoney.setTargetObjectID(pSender->getObjectID());
            gcTradeMoney.setCode(GC_TRADE_MONEY_DECREASE);
            gcTradeMoney.setAmount(finalAmount);
            pTargetPlayer->sendPacket(&gcTradeMoney);
        } else {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_DECREASE_MONEY);
            return;
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeMoneyHandler::executeVampire(CGTradeMoney* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    Gold_t Amount = pPacket->getAmount();
    BYTE Code = pPacket->getCode();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Vampire* pSender = checkedCast<Vampire*>(pPC);
    Vampire* pReceiver = checkedCast<Vampire*>(pTargetPC);
    Player* pTargetPlayer = pTargetPC->getPlayer();
    Gold_t finalAmount = Amount;
    Gold_t margin = 0;
    GCTradeMoney gcTradeMoney;
    GCTradeVerify gcTradeVerify;

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pReceiver->getName());

    
    if (Code == CG_TRADE_MONEY_INCREASE) {
        
        if (pSender->getGold() >= Amount) {
            
            
            Gold_t receiverGold = pReceiver->getGold() + pInfo1->getGold();
            if (receiverGold + Amount > MAX_MONEY) {
                margin = receiverGold + Amount - MAX_MONEY;
                finalAmount = finalAmount - margin;
            }

            
            pSender->setGold(pSender->getGold() - finalAmount);
            pInfo1->setGold(pInfo1->getGold() + finalAmount);

            
            if (pInfo1->getStatus() == TRADE_FINISH) {
                
                gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_MONEY_INCREASE);
                pPlayer->sendPacket(&gcTradeVerify);
            }

            
            pInfo1->setStatus(TRADE_TRADING);
            pInfo2->setStatus(TRADE_TRADING);

            
            
            gcTradeMoney.setTargetObjectID(TargetOID);
            gcTradeMoney.setCode(GC_TRADE_MONEY_INCREASE_RESULT);
            gcTradeMoney.setAmount(finalAmount);
            pPlayer->sendPacket(&gcTradeMoney);

            
            gcTradeMoney.setTargetObjectID(pSender->getObjectID());
            gcTradeMoney.setCode(GC_TRADE_MONEY_INCREASE);
            gcTradeMoney.setAmount(finalAmount);
            pTargetPlayer->sendPacket(&gcTradeMoney);
        } else {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_INCREASE_MONEY);
            return;
        }
    }
    
    else if (Code == CG_TRADE_MONEY_DECREASE) {
        
        if (pInfo1->getGold() >= Amount) {
            
            Gold_t senderGold = pSender->getGold() + pInfo2->getGold();
            if (senderGold + Amount > MAX_MONEY) {
                margin = senderGold + Amount - MAX_MONEY;
                finalAmount = finalAmount - margin;
            }

            
            pSender->setGold(pSender->getGold() + finalAmount);
            pInfo1->setGold(pInfo1->getGold() - finalAmount);

            
            if (pInfo1->getStatus() == TRADE_FINISH) {
                
                gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_MONEY_DECREASE);
                pPlayer->sendPacket(&gcTradeVerify);
            }

            
            pInfo1->setStatus(TRADE_TRADING);
            pInfo2->setStatus(TRADE_TRADING);

            
            
            gcTradeMoney.setTargetObjectID(TargetOID);
            gcTradeMoney.setCode(GC_TRADE_MONEY_DECREASE_RESULT);
            gcTradeMoney.setAmount(finalAmount);
            pPlayer->sendPacket(&gcTradeMoney);

            
            gcTradeMoney.setTargetObjectID(pSender->getObjectID());
            gcTradeMoney.setCode(GC_TRADE_MONEY_DECREASE);
            gcTradeMoney.setAmount(finalAmount);
            pTargetPlayer->sendPacket(&gcTradeMoney);
        } else {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_DECREASE_MONEY);
            return;
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeMoneyHandler::executeOusters(CGTradeMoney* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    Gold_t Amount = pPacket->getAmount();
    BYTE Code = pPacket->getCode();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Ousters* pSender = checkedCast<Ousters*>(pPC);
    Ousters* pReceiver = checkedCast<Ousters*>(pTargetPC);
    Player* pTargetPlayer = pTargetPC->getPlayer();
    Gold_t finalAmount = Amount;
    Gold_t margin = 0;
    GCTradeMoney gcTradeMoney;
    GCTradeVerify gcTradeVerify;

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pReceiver->getName());

    
    if (Code == CG_TRADE_MONEY_INCREASE) {
        
        if (pSender->getGold() >= Amount) {
            
            
            Gold_t receiverGold = pReceiver->getGold() + pInfo1->getGold();
            if (receiverGold + Amount > MAX_MONEY) {
                margin = receiverGold + Amount - MAX_MONEY;
                finalAmount = finalAmount - margin;
            }

            
            pSender->setGold(pSender->getGold() - finalAmount);
            pInfo1->setGold(pInfo1->getGold() + finalAmount);

            
            if (pInfo1->getStatus() == TRADE_FINISH) {
                
                gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_MONEY_INCREASE);
                pPlayer->sendPacket(&gcTradeVerify);
            }

            
            pInfo1->setStatus(TRADE_TRADING);
            pInfo2->setStatus(TRADE_TRADING);

            
            
            gcTradeMoney.setTargetObjectID(TargetOID);
            gcTradeMoney.setCode(GC_TRADE_MONEY_INCREASE_RESULT);
            gcTradeMoney.setAmount(finalAmount);
            pPlayer->sendPacket(&gcTradeMoney);

            
            gcTradeMoney.setTargetObjectID(pSender->getObjectID());
            gcTradeMoney.setCode(GC_TRADE_MONEY_INCREASE);
            gcTradeMoney.setAmount(finalAmount);
            pTargetPlayer->sendPacket(&gcTradeMoney);
        } else {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_INCREASE_MONEY);
            return;
        }
    }
    
    else if (Code == CG_TRADE_MONEY_DECREASE) {
        
        if (pInfo1->getGold() >= Amount) {
            
            Gold_t senderGold = pSender->getGold() + pInfo2->getGold();
            if (senderGold + Amount > MAX_MONEY) {
                margin = senderGold + Amount - MAX_MONEY;
                finalAmount = finalAmount - margin;
            }

            
            pSender->setGold(pSender->getGold() + finalAmount);
            pInfo1->setGold(pInfo1->getGold() - finalAmount);

            
            if (pInfo1->getStatus() == TRADE_FINISH) {
                
                gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_MONEY_DECREASE);
                pPlayer->sendPacket(&gcTradeVerify);
            }

            
            pInfo1->setStatus(TRADE_TRADING);
            pInfo2->setStatus(TRADE_TRADING);

            
            
            gcTradeMoney.setTargetObjectID(TargetOID);
            gcTradeMoney.setCode(GC_TRADE_MONEY_DECREASE_RESULT);
            gcTradeMoney.setAmount(finalAmount);
            pPlayer->sendPacket(&gcTradeMoney);

            
            gcTradeMoney.setTargetObjectID(pSender->getObjectID());
            gcTradeMoney.setCode(GC_TRADE_MONEY_DECREASE);
            gcTradeMoney.setAmount(finalAmount);
            pTargetPlayer->sendPacket(&gcTradeMoney);
        } else {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_DECREASE_MONEY);
            return;
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeMoneyHandler::executeError(CGTradeMoney* pPacket, Player* pPlayer, BYTE ErrorCode)

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
