//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeFinishHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGTradeFinish.h"

#ifdef __GAME_SERVER__
#include "CheckedCast.h"
#include "GCTradeError.h"
#include "GCTradeFinish.h"
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
void CGTradeFinishHandler::execute(CGTradeFinish* pPacket, Player* pPlayer)

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
    try
    {
        pTargetPC = pZone->getCreature(TargetOID);
    }
    catch (NoSuchElementException)
    {
        pTargetPC = NULL;
    }
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
        throw ProtocolException("CGTradeFinishHanderl::execuete() :     .");

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeFinishHandler::executeSlayer(CGTradeFinish* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    BYTE CODE = pPacket->getCode();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Slayer* pSender = checkedCast<Slayer*>(pPC);
    Slayer* pReceiver = checkedCast<Slayer*>(pTargetPC);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pReceiver->getName());
    Player* pTargetPlayer = pTargetPC->getPlayer();
    GCTradeFinish gcTradeFinish;

    
    Timeval currentTime;
    getCurrentTime(currentTime);

    
    if (CODE == CG_TRADE_FINISH_ACCEPT) {
        
        if (pInfo1->isValidOKTime(currentTime) == false) {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_UNKNOWN);
            return;
        }

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_ACCEPT);
        pTargetPlayer->sendPacket(&gcTradeFinish);

        
        pInfo1->setStatus(TRADE_FINISH);

        

        
        if (pInfo2->getStatus() == TRADE_FINISH) {
            
            // endl;

            
            if (pTradeManager->canTrade(pSender, pReceiver) == 1) {
                
                gcTradeFinish.setTargetObjectID(pSender->getObjectID());
                gcTradeFinish.setCode(GC_TRADE_FINISH_EXECUTE);
                pTargetPlayer->sendPacket(&gcTradeFinish);

                gcTradeFinish.setTargetObjectID(pReceiver->getObjectID());
                gcTradeFinish.setCode(GC_TRADE_FINISH_EXECUTE);
                pPlayer->sendPacket(&gcTradeFinish);

                
                pTradeManager->processTrade(pSender, pReceiver);
            } else if (pTradeManager->canTrade(pSender, pReceiver) == 2) {
                pTradeManager->cancelTrade(pPC);
                executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_EVENT_GIFT_BOX);
                return;
            } else {
                pTradeManager->cancelTrade(pPC);
                executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_ENOUGH_SPACE);
                return;
            }
        }
    }
    
    else if (CODE == CG_TRADE_FINISH_REJECT) {
        
        if (pInfo1->getStatus() == TRADE_FINISH) {
            
            GCTradeVerify gcTradeVerify;
            gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_FINISH_REJECT);
            pPlayer->sendPacket(&gcTradeVerify);
        }

        
        pTradeManager->cancelTrade(pSender, pReceiver);

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_REJECT);
        pTargetPlayer->sendPacket(&gcTradeFinish);
    }
    
    else if (CODE == CG_TRADE_FINISH_RECONSIDER) {
        
        if (pInfo1->getStatus() == TRADE_FINISH) {
            
            GCTradeVerify gcTradeVerify;
            gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_FINISH_RECONSIDER);
            pPlayer->sendPacket(&gcTradeVerify);
        }

        
        pInfo1->setStatus(TRADE_TRADING);

        
        
        pInfo1->setNextTime(currentTime);

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_RECONSIDER);
        pTargetPlayer->sendPacket(&gcTradeFinish);
    }
    
    else
        throw ProtocolException("CGTradeFinish::executeSlayer() :    ");

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeFinishHandler::executeVampire(CGTradeFinish* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    BYTE CODE = pPacket->getCode();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Vampire* pSender = checkedCast<Vampire*>(pPC);
    Vampire* pReceiver = checkedCast<Vampire*>(pTargetPC);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pReceiver->getName());
    Player* pTargetPlayer = pTargetPC->getPlayer();
    GCTradeFinish gcTradeFinish;

    
    Timeval currentTime;
    getCurrentTime(currentTime);

    
    if (CODE == CG_TRADE_FINISH_ACCEPT) {
        
        if (pInfo1->isValidOKTime(currentTime) == false) {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_UNKNOWN);
            return;
        }

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_ACCEPT);
        pTargetPlayer->sendPacket(&gcTradeFinish);

        
        pInfo1->setStatus(TRADE_FINISH);

        
        if (pInfo2->getStatus() == TRADE_FINISH) {
            
            
            // endl;
            if (pTradeManager->canTrade(pSender, pReceiver) == 1) {
                
                gcTradeFinish.setTargetObjectID(pSender->getObjectID());
                gcTradeFinish.setCode(GC_TRADE_FINISH_EXECUTE);
                pTargetPlayer->sendPacket(&gcTradeFinish);

                gcTradeFinish.setTargetObjectID(pReceiver->getObjectID());
                gcTradeFinish.setCode(GC_TRADE_FINISH_EXECUTE);
                pPlayer->sendPacket(&gcTradeFinish);

                
                pTradeManager->processTrade(pSender, pReceiver);
            } else if (pTradeManager->canTrade(pSender, pReceiver) == 2) {
                pTradeManager->cancelTrade(pPC);
                executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_EVENT_GIFT_BOX);
                return;
            } else {
                pTradeManager->cancelTrade(pPC);
                executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_ENOUGH_SPACE);
                return;
            }
        }
    }
    
    else if (CODE == CG_TRADE_FINISH_REJECT) {
        
        if (pInfo1->getStatus() == TRADE_FINISH) {
            
            GCTradeVerify gcTradeVerify;
            gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_FINISH_REJECT);
            pPlayer->sendPacket(&gcTradeVerify);
        }

        
        pTradeManager->cancelTrade(pSender, pReceiver);

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_REJECT);
        pTargetPlayer->sendPacket(&gcTradeFinish);
    }
    
    else if (CODE == CG_TRADE_FINISH_RECONSIDER) {
        
        if (pInfo1->getStatus() == TRADE_FINISH) {
            
            GCTradeVerify gcTradeVerify;
            gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_FINISH_RECONSIDER);
            pPlayer->sendPacket(&gcTradeVerify);
        }

        
        pInfo1->setStatus(TRADE_TRADING);

        
        
        pInfo1->setNextTime(currentTime);

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_RECONSIDER);
        pTargetPlayer->sendPacket(&gcTradeFinish);
    }
    
    else
        throw ProtocolException("CGTradeFinish::executeVampire() :    ");

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeFinishHandler::executeOusters(CGTradeFinish* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        
        ObjectID_t TargetOID = pPacket->getTargetObjectID();
    BYTE CODE = pPacket->getCode();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Zone* pZone = pPC->getZone();
    Creature* pTargetPC = pZone->getCreature(TargetOID);

    
    if (pTargetPC == NULL)
        return;

    Ousters* pSender = checkedCast<Ousters*>(pPC);
    Ousters* pReceiver = checkedCast<Ousters*>(pTargetPC);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pReceiver->getName());
    Player* pTargetPlayer = pTargetPC->getPlayer();
    GCTradeFinish gcTradeFinish;

    
    Timeval currentTime;
    getCurrentTime(currentTime);

    
    if (CODE == CG_TRADE_FINISH_ACCEPT) {
        
        if (pInfo1->isValidOKTime(currentTime) == false) {
            pTradeManager->cancelTrade(pPC);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_UNKNOWN);
            return;
        }

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_ACCEPT);
        pTargetPlayer->sendPacket(&gcTradeFinish);

        
        pInfo1->setStatus(TRADE_FINISH);

        
        if (pInfo2->getStatus() == TRADE_FINISH) {
            
            
            // endl;
            if (pTradeManager->canTrade(pSender, pReceiver) == 1) {
                
                gcTradeFinish.setTargetObjectID(pSender->getObjectID());
                gcTradeFinish.setCode(GC_TRADE_FINISH_EXECUTE);
                pTargetPlayer->sendPacket(&gcTradeFinish);

                gcTradeFinish.setTargetObjectID(pReceiver->getObjectID());
                gcTradeFinish.setCode(GC_TRADE_FINISH_EXECUTE);
                pPlayer->sendPacket(&gcTradeFinish);

                
                pTradeManager->processTrade(pSender, pReceiver);
            } else if (pTradeManager->canTrade(pSender, pReceiver) == 2) {
                pTradeManager->cancelTrade(pPC);
                executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_EVENT_GIFT_BOX);
                return;
            } else {
                pTradeManager->cancelTrade(pPC);
                executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_ENOUGH_SPACE);
                return;
            }
        }
    }
    
    else if (CODE == CG_TRADE_FINISH_REJECT) {
        
        if (pInfo1->getStatus() == TRADE_FINISH) {
            
            GCTradeVerify gcTradeVerify;
            gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_FINISH_REJECT);
            pPlayer->sendPacket(&gcTradeVerify);
        }

        
        pTradeManager->cancelTrade(pSender, pReceiver);

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_REJECT);
        pTargetPlayer->sendPacket(&gcTradeFinish);
    }
    
    else if (CODE == CG_TRADE_FINISH_RECONSIDER) {
        
        if (pInfo1->getStatus() == TRADE_FINISH) {
            
            GCTradeVerify gcTradeVerify;
            gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_FINISH_RECONSIDER);
            pPlayer->sendPacket(&gcTradeVerify);
        }

        
        pInfo1->setStatus(TRADE_TRADING);

        
        
        pInfo1->setNextTime(currentTime);

        
        gcTradeFinish.setTargetObjectID(pSender->getObjectID());
        gcTradeFinish.setCode(GC_TRADE_FINISH_RECONSIDER);
        pTargetPlayer->sendPacket(&gcTradeFinish);
    }
    
    else
        throw ProtocolException("CGTradeFinish::executeOusters() :    ");

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeFinishHandler::executeError(CGTradeFinish* pPacket, Player* pPlayer, BYTE ErrorCode)

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
