//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseBonusPointHandler.cpp
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGUseBonusPoint.h"

#ifdef __GAME_SERVER__
#include "CheckedCast.h"
#include "Creature.h"
#include "GamePlayer.h"
#include "SkillHandlerManager.h"
#include "SkillInfo.h"
#include "SkillParentInfo.h"
// #include "LogClient.h"
#include "GCLearnSkillReady.h"
#include "GCUseBonusPointFail.h"
#include "GCUseBonusPointOK.h"
#include "Slayer.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseBonusPointHandler::execute(CGUseBonusPoint* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX __BEGIN_DEBUG

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    if (pGamePlayer->getPlayerStatus() != GPS_NORMAL)
        return;

    Creature* pCreature = pGamePlayer->getCreature();
    BYTE which = pPacket->getWhich();
    Attr_t cur = 0;

    if (pCreature->isSlayer()) {
        Slayer* pSlayer = checkedCast<Slayer*>(pCreature);
        if (pSlayer->getBonus() <= 0) {
            GCUseBonusPointFail failPkt;
            pPlayer->sendPacket(&failPkt);
            return;
        }

        bool success = false;
        if (which == INC_INT) {
            success = pSlayer->putAdvancedBonusToINT();
        } else if (which == INC_STR) {
            success = pSlayer->putAdvancedBonusToSTR();
        } else if (which == INC_DEX) {
            success = pSlayer->putAdvancedBonusToDEX();
        }

        if (success) {
            GCUseBonusPointOK okpkt;
            pGamePlayer->sendPacket(&okpkt);

            pSlayer->saveExps();
            pSlayer->initAllStatAndSend();
        } else {
            GCUseBonusPointFail failPkt;
            pPlayer->sendPacket(&failPkt);
            return;
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = checkedCast<Vampire*>(pCreature);
        if (pVampire->getBonus() <= 0) {
            GCUseBonusPointFail failPkt;
            pPlayer->sendPacket(&failPkt);
            return;
        }

        VAMPIRE_RECORD oldRecord;
        
        pVampire->getVampireRecord(oldRecord);

        if (which == INC_INT) {
            cur = pVampire->getINT(ATTR_BASIC) + 1;
            pVampire->setINT(cur, ATTR_BASIC);

            StringStream sst;
            sst << "INTE = " << (int)cur;
            pVampire->tinysave(sst.toString());

             

            // log(LOG_USE_BONUS_POINT, pVampire->getName(), "", "INT");
        } else if (which == INC_STR) {
            cur = pVampire->getSTR(ATTR_BASIC) + 1;
            pVampire->setSTR(cur, ATTR_BASIC);

            StringStream sst;
            sst << "STR = " << (int)cur;
            pVampire->tinysave(sst.toString());

            // log(LOG_USE_BONUS_POINT, pVampire->getName(), "", "STR");
        } else if (which == INC_DEX) {
            cur = pVampire->getDEX(ATTR_BASIC) + 1;
            pVampire->setDEX(cur, ATTR_BASIC);

            StringStream sst;
            sst << "DEX = " << (int)cur;
            pVampire->tinysave(sst.toString());

            // log(LOG_USE_BONUS_POINT, pVampire->getName(), "", "DEX");
        }

        
        Bonus_t OldBonus = pVampire->getBonus();
        pVampire->setBonus(OldBonus - 1);
        StringStream sst;
        sst << "Bonus = " << (int)(OldBonus - 1);
        pVampire->tinysave(sst.toString());

        
        pVampire->initAllStat();

        
        
        
        

        
        GCUseBonusPointOK okpkt;
        pGamePlayer->sendPacket(&okpkt);

        
        pVampire->sendModifyInfo(oldRecord);
        pVampire->sendRealWearingInfo();
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = checkedCast<Ousters*>(pCreature);
        if (pOusters->getBonus() <= 0) {
            GCUseBonusPointFail failPkt;
            pPlayer->sendPacket(&failPkt);
            return;
        }

        OUSTERS_RECORD oldRecord;
        
        pOusters->getOustersRecord(oldRecord);

        if (which == INC_INT) {
            cur = pOusters->getINT(ATTR_BASIC) + 1;
            pOusters->setINT(cur, ATTR_BASIC);

            StringStream sst;
            sst << "INTE = " << (int)cur;
            pOusters->tinysave(sst.toString());
        } else if (which == INC_STR) {
            cur = pOusters->getSTR(ATTR_BASIC) + 1;
            pOusters->setSTR(cur, ATTR_BASIC);

            StringStream sst;
            sst << "STR = " << (int)cur;
            pOusters->tinysave(sst.toString());
        } else if (which == INC_DEX) {
            cur = pOusters->getDEX(ATTR_BASIC) + 1;
            pOusters->setDEX(cur, ATTR_BASIC);

            StringStream sst;
            sst << "DEX = " << (int)cur;
            pOusters->tinysave(sst.toString());
        }

        
        Bonus_t OldBonus = pOusters->getBonus();
        pOusters->setBonus(OldBonus - 1);
        StringStream sst;
        sst << "Bonus = " << (int)(OldBonus - 1);
        pOusters->tinysave(sst.toString());

        
        pOusters->initAllStat();

        
        GCUseBonusPointOK okpkt;
        pGamePlayer->sendPacket(&okpkt);

        
        pOusters->sendModifyInfo(oldRecord);
        pOusters->sendRealWearingInfo();
    } else {
        GCUseBonusPointFail failPkt;
        pPlayer->sendPacket(&failPkt);
        return;
    }


#endif // __GAME_SERVER__

    __END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
