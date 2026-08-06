//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradePrepareHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGTradePrepare.h"

#ifdef __GAME_SERVER__
#include "GCTradeError.h"
#include "GCTradePrepare.h"
#include "GCTradeVerify.h"
#include "GamePlayer.h"
#include "Ousters.h"
#include "Slayer.h"
#include "StringStream.h"
#include "TradeManager.h"
#include "Vampire.h"
#include "ZoneUtil.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradePrepareHandler::execute(CGTradePrepare* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    ObjectID_t TargetOID = pPacket->getTargetObjectID();
    BYTE CODE = pPacket->getCode();
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    GCTradePrepare gcTradePrepare;

    Creature* pSender = pGamePlayer->getCreature();
    Assert(pSender != NULL);

    Zone* pZone = pSender->getZone();
    Assert(pZone != NULL);

    TradeManager* pTradeManager = pZone->getTradeManager();
    Assert(pTradeManager != NULL);

    
    Creature* pReceiver = NULL;
    /*
    try { pReceiver = pZone->getCreature(TargetOID); }
    catch (NoSuchElementException) { pReceiver = NULL; }
    */

    
    pReceiver = pZone->getCreature(TargetOID);

    
    if (pReceiver == NULL) {
        pTradeManager->cancelTrade(pSender);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_TARGET_NOT_EXIST);
        return;
    }

    
    
    if (pSender->getName() == pReceiver->getName()) {
        StringStream msg;
        msg << "CGTradePrepare : Error, Same Creature!!! Name[" << pSender->getName() << "]";
        filelog("TradeError.log", "%s", msg.toString().c_str());
        throw ProtocolException(msg.toString());
    }

    
    if (!pReceiver->isPC() || !isSameRace(pSender, pReceiver)) {
        pTradeManager->cancelTrade(pSender);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_RACE_DIFFER);
        return;
    }

    
    if (!isInSafeZone(pSender) || !isInSafeZone(pReceiver)) {
        pTradeManager->cancelTrade(pSender);
        executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_SAFE);
        return;
    }

    
    if (pSender->isSlayer() && pReceiver->isSlayer()) {
        Slayer* pSlayer1 = dynamic_cast<Slayer*>(pSender);
        Slayer* pSlayer2 = dynamic_cast<Slayer*>(pReceiver);

        if (pSlayer1->hasRideMotorcycle() || pSlayer2->hasRideMotorcycle()) {
            pTradeManager->cancelTrade(pSender);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_MOTORCYCLE);
            return;
        }
    }

    if (pSender->isOusters() && pReceiver->isOusters()) {
        Ousters* pOusters1 = dynamic_cast<Ousters*>(pSender);
        Ousters* pOusters2 = dynamic_cast<Ousters*>(pReceiver);

        if (pOusters1->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) ||
            pOusters2->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)) {
            pTradeManager->cancelTrade(pSender);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_MOTORCYCLE);
            return;
        }
    }

    TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
    TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pReceiver->getName());
    Player* pReceiverPlayer = pReceiver->getPlayer();

    
    switch (CODE) {
    ////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////
    case CG_TRADE_PREPARE_CODE_REQUEST:
        
        if (pInfo1 != NULL) {
            pTradeManager->cancelTrade(pSender);
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_ALREADY_TRADING);
            return;
        }

        
        
        if (pInfo2 != NULL) {
            gcTradePrepare.setTargetObjectID(pPacket->getTargetObjectID());
            gcTradePrepare.setCode(GC_TRADE_PREPARE_CODE_BUSY);
            pPlayer->sendPacket(&gcTradePrepare);
            return;
        }

        
        gcTradePrepare.setTargetObjectID(pSender->getObjectID());
        gcTradePrepare.setCode(GC_TRADE_PREPARE_CODE_REQUEST);
        pReceiverPlayer->sendPacket(&gcTradePrepare);

        
        pTradeManager->initTrade(pSender, pReceiver);
        break;

    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    case CG_TRADE_PREPARE_CODE_CANCEL:
        
        if (pTradeManager->isTrading(pSender, pReceiver)) {
            gcTradePrepare.setTargetObjectID(pSender->getObjectID());
            gcTradePrepare.setCode(GC_TRADE_PREPARE_CODE_CANCEL);
            pReceiverPlayer->sendPacket(&gcTradePrepare);
            
            pTradeManager->cancelTrade(pSender, pReceiver);
        } else {
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_TRADING);
            return;
        }
        break;

    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    case CG_TRADE_PREPARE_CODE_ACCEPT:
        
        if (pTradeManager->isTrading(pSender, pReceiver)) {
            gcTradePrepare.setTargetObjectID(pSender->getObjectID());
            gcTradePrepare.setCode(GC_TRADE_PREPARE_CODE_ACCEPT);
            pReceiverPlayer->sendPacket(&gcTradePrepare);
        } else {
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_TRADING);
            return;
        }
        break;

    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    case CG_TRADE_PREPARE_CODE_REJECT:
        
        if (pTradeManager->isTrading(pSender, pReceiver)) {
            gcTradePrepare.setTargetObjectID(pSender->getObjectID());
            gcTradePrepare.setCode(GC_TRADE_PREPARE_CODE_REJECT);
            pReceiverPlayer->sendPacket(&gcTradePrepare);
            
            pTradeManager->cancelTrade(pSender, pReceiver);
        } else {
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_TRADING);
            return;
        }
        break;

    ////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////
    case CG_TRADE_PREPARE_CODE_BUSY:
        if (pTradeManager->isTrading(pSender, pReceiver)) {
            
            gcTradePrepare.setTargetObjectID(pSender->getObjectID());
            gcTradePrepare.setCode(GC_TRADE_PREPARE_CODE_BUSY);
            pReceiverPlayer->sendPacket(&gcTradePrepare);
            
            pTradeManager->cancelTrade(pSender, pReceiver);
        } else {
            executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_TRADING);
            return;
        }
        break;

    
    default:
        throw ProtocolException("CGTradePrepare::execute() :    ");
    }


#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradePrepareHandler::executeError(CGTradePrepare* pPacket, Player* pPlayer, BYTE ErrorCode)

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
