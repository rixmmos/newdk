//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectTileEffectHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSelectTileEffect.h"

#ifdef __GAME_SERVER__
#include "Effect.h"
#include "EffectManager.h"
#include "GCEnterVampirePortal.h"
#include "GCSystemMessage.h"
#include "GQuestManager.h"
#include "GamePlayer.h"
#include "PacketUtil.h"
#include "Properties.h"
#include "RelicUtil.h"
#include "Slayer.h"
#include "StringPool.h"
#include "Vampire.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"
#include "skill/EffectVampirePortal.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSelectTileEffectHandler::execute(CGSelectTileEffect* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    try {
        
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
        Assert(pGamePlayer != NULL);
        if (pGamePlayer->getPlayerStatus() != GPS_NORMAL)
            return;

        Creature* pCreature = pGamePlayer->getCreature();
        Assert(pCreature != NULL);

        
        if (pCreature->hasRelicItem() || pCreature->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER)) {
            return;
        }

        Zone* pZone = pCreature->getZone();
        Assert(pZone != NULL);

        Effect* pEffect = NULL;

        
        EffectManager* pVampirePortalManager = pZone->getVampirePortalManager();

        pEffect = pVampirePortalManager->findEffect(pPacket->getEffectObjectID());

        if (pEffect != NULL) {
            // cout << "CGSelectTileEffectHandler::execute() : Effect Exist" << endl;

            switch (pEffect->getEffectClass()) {
            case Effect::EFFECT_CLASS_VAMPIRE_PORTAL:
                executeVampirePortal(pPacket, pPlayer, pEffect);
                break;
            default:
                Assert(false);
                break;
            }
        } else {
            cout << "CGSelectTileEffectHandler::execute() : Effect DOES NOT Exist" << endl;

            
        }
    } catch (Throwable& t) {
        cerr << t.toString() << endl;
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSelectTileEffectHandler::executeVampirePortal(CGSelectTileEffect* pPacket, Player* pPlayer, Effect* pEffect)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);
    Assert(pEffect != NULL);
    Assert(pEffect->getEffectClass() == Effect::EFFECT_CLASS_VAMPIRE_PORTAL);

    try {
        
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
        Assert(pGamePlayer != NULL);
        if (pGamePlayer->getPlayerStatus() != GPS_NORMAL)
            return;

        Creature* pCreature = pGamePlayer->getCreature();
        Assert(pCreature != NULL);

        
        if (!pCreature->isVampire())
            return;

        Zone* pZone = pCreature->getZone();
        Assert(pZone != NULL);

        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        Assert(pVampire != NULL);

        EffectVampirePortal* pEffectVampirePortal = dynamic_cast<EffectVampirePortal*>(pEffect);
        ZONE_COORD zonecoord = pEffectVampirePortal->getZoneCoord();

        
        
        
        if (zonecoord.id == 1122 || zonecoord.id == 8000) {
            return;
        }

        try {
            ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(zonecoord.id);

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
            
            
            if (pZoneInfo == NULL || ((pZoneInfo->isPayPlay() || pZoneInfo->isPremiumZone()) &&
                                      !pGamePlayer->isPayPlaying() && !pGamePlayer->isFamilyFreePass())) {
                // Statement* pStmt = NULL;
                string connectIP = pGamePlayer->getSocket()->getHost();

                
                if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID())) {
                    sendPayInfo(pGamePlayer);
                } else if (pZoneInfo->isPayPlay()) {
                    
                    GCSystemMessage gcSystemMessage;

                    if (g_pConfig->getPropertyInt("IsNetMarble") == 0) {
                        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER));
                    } else {
                        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER));
                    }

                    pGamePlayer->sendPacket(&gcSystemMessage);

                    return;
                }
            }
#endif
        } catch (NoSuchElementException&) {
            return;
        }

        if (pEffectVampirePortal->getCount() > 0) {
            
            
            
            
            pVampire->setFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL);

            
            GCEnterVampirePortal gcEnterVampirePortal;
            gcEnterVampirePortal.setObjectID(pVampire->getObjectID());
            gcEnterVampirePortal.setX(pEffectVampirePortal->getX());
            gcEnterVampirePortal.setY(pEffectVampirePortal->getY());
            pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcEnterVampirePortal);

            pVampire->getGQuestManager()->illegalWarp();
            
            transportCreature(pCreature, zonecoord.id, zonecoord.x, zonecoord.y, false);

            
            pEffectVampirePortal->setCount(pEffectVampirePortal->getCount() - 1);
            // if (pEffectVampirePortal->getCount() == 0) pEffectVampirePortal->setDeadline(0);
        }
    } catch (Throwable& t) {
        cerr << t.toString() << endl;
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
