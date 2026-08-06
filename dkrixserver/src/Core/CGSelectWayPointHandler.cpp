//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectWayPointHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSelectWayPoint.h"

#ifdef __GAME_SERVER__
#include <cmath>

#include "CreatureUtil.h"
#include "DynamicZone.h"
#include "DynamicZoneGroup.h"
#include "DynamicZoneInfo.h"
#include "DynamicZoneManager.h"
#include "FlagSet.h"
#include "GCAddHelicopter.h"
#include "GCModifyInformation.h"
#include "GCNoticeEvent.h"
#include "GCSystemMessage.h"
#include "GQuestManager.h"
#include "GamePlayer.h"
#include "LevelWarManager.h"
#include "LevelWarZoneInfoManager.h"
#include "Ousters.h"
#include "PacketUtil.h"
#include "Properties.h"
#include "Store.h"
#include "StringPool.h"
#include "SweeperBonusManager.h"
#include "VariableManager.h"
#include "WayPoint.h"
#include "Zone.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"
#include "ctf/FlagManager.h"
#include "war/WarSystem.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSelectWayPointHandler::execute(CGSelectWayPoint* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__
        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    static map<Level_t, Price_t> sPriceMap;

    try {
        int targetDynamicZoneType = g_pDynamicZoneInfoManager->getDynamicZoneTypeByZoneID(pPacket->getZoneID());
        if (targetDynamicZoneType != DYNAMIC_ZONE_MAX) {
            executeEnterQuestZone(pPacket, pPlayer, targetDynamicZoneType);
        }

        
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
        Assert(pGamePlayer != NULL);
        if (pGamePlayer->getPlayerStatus() != GPS_NORMAL)
            return;

        
        Creature* pCreature = pGamePlayer->getCreature();
        Assert(pCreature != NULL);

        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
        Assert(pPC != NULL);

        if (pPC->getStore()->isOpen())
            return;
        if (pCreature->hasRelicItem())
            return;

        
        if (pCreature->isDead())
            return;

        
        if (pPacket->getZoneID() == 1122) {
            ZONE_COORD pos(1122);

            if (pCreature->isSlayer()) {
                pos.x = 107;
                pos.y = 27;
            } else if (pCreature->isVampire()) {
                pos.x = 18;
                pos.y = 27;
            } else if (pCreature->isOusters()) {
                pos.x = 12;
                pos.y = 103;
            } else
                return;

            if (!canEnterBeginnerZone(pCreature))
                return;

            
#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
            ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pos.id);

            
            if (pZoneInfo == NULL || ((pZoneInfo->isPayPlay() || pZoneInfo->isPremiumZone()) &&
                                      (!pGamePlayer->isPayPlaying() && !pGamePlayer->isFamilyFreePass()))) {
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

            pPC->getGQuestManager()->illegalWarp();
            transportCreature(pCreature, pos.id, pos.x, pos.y, false);
            return;
        }

        if (pPacket->getZoneID() == 1131) {
            if (g_pVariableManager->getVariable(ACTIVE_LEVEL_WAR) == 0) {
                GCSystemMessage gcSystemMessage;
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER));
                pGamePlayer->sendPacket(&gcSystemMessage);
                return;
            }
            /*
                        if (g_pConfig->getPropertyInt("ServerID" ) != 0 )
                        {
                            GCNoticeEvent gcNoticeEvent;

                            gcNoticeEvent.setCode(NOTICE_EVENT_NOT_FIRST_SERVER);
                            pGamePlayer->sendPacket(&gcNoticeEvent);
            //				GCSystemMessage gcSystemMessage;
            //				gcSystemMessage.setMessage(g_pStringPool->getString(STRID_LEVEL_WAR_ONLY_FIRST_SERVER ));
            //				pGamePlayer->sendPacket (&gcSystemMessage);
                            return;
                        }
            */

            
            ZONE_COORD pos(g_pLevelWarZoneInfoManager->getCreatureZoneID(pCreature));

            if (g_pSweeperBonusManager->isAble(g_pLevelWarZoneInfoManager->getCreatureZoneID(pCreature))) {
                GCSystemMessage gcSystemMessage;
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NO_WAR_IN_ACTIVE));
                pGamePlayer->sendPacket(&gcSystemMessage);
                return;
            }

            if (pCreature->isSlayer()) {
                pos.x = 12;
                pos.y = 9;
            } else if (pCreature->isVampire()) {
                pos.x = 117;
                pos.y = 8;
            } else if (pCreature->isOusters()) {
                pos.x = 9;
                pos.y = 111;
            }

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
            Zone* pZone = getZoneByZoneID(pos.id);
            Assert(pZone != NULL);

            LevelWarManager* pLevelWarManager = pZone->getLevelWarManager();
            Assert(pLevelWarManager != NULL);

            if (!pLevelWarManager->hasWar() && !g_pVariableManager->canEnterLevelWarZoneFree() &&
                !pGamePlayer->isPayPlaying() && pGamePlayer->isFamilyFreePass() &&
                !pLevelWarManager->canEnterFreeUser()) {
                GCSystemMessage gcSystemMessage;
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER_LEVEL_WAR_ZONE));
                pGamePlayer->sendPacket(&gcSystemMessage);
                return;
            }
#endif

            pPC->getGQuestManager()->illegalWarp();
            transportCreature(pCreature, pos.id, pos.x, pos.y, false);
            return;
        }

        if (pPacket->getZoneID() == 72) {
            if (!g_pWarSystem->hasActiveRaceWar()) {
                GCSystemMessage gcSystemMessage;
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NO_WAR_IN_ACTIVE));
                pGamePlayer->sendPacket(&gcSystemMessage);

                return;
            }
            /*
                        if (g_pConfig->getPropertyInt("ServerID" ) != 0 )
                        {
                            GCNoticeEvent gcNoticeEvent;

                            gcNoticeEvent.setCode(NOTICE_EVENT_NOT_FIRST_SERVER);
                            pGamePlayer->sendPacket(&gcNoticeEvent);
            //				GCSystemMessage gcSystemMessage;
            //				gcSystemMessage.setMessage(g_pStringPool->getString(STRID_LEVEL_WAR_ONLY_FIRST_SERVER ));
            //				pGamePlayer->sendPacket (&gcSystemMessage);
                            return;
                        }
            */

            
            ZONE_COORD pos;

            if (pCreature->isSlayer()) {
                pos.id = 73;
                pos.x = 30;
                pos.y = 124;
            } else if (pCreature->isVampire()) {
                pos.id = 71;
                pos.x = 104;
                pos.y = 128;
            } else if (pCreature->isOusters()) {
                pos.id = 72;
                pos.x = 67;
                pos.y = 165;
            }
             
            if (!g_pVariableManager->isActiveRaceWarLimiter() ||
                pCreature->isFlag(Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET)) {
                pPC->getGQuestManager()->illegalWarp();
                transportCreature(pCreature, pos.id, pos.x, pos.y, false);
                return;
            } else {
                GCSystemMessage gcSystemMessage;
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER_DURING_RACE_WAR));
                pGamePlayer->sendPacket(&gcSystemMessage);
                return;
            }
        }

        if (!pCreature->isSlayer() && !pCreature->isOusters()) {
            
            return;
        }

        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)) {
            
            return;
        }

        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER)) {
            
            return;
        }

        // Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        // Assert(pSlayer != NULL);

        bool bCancel = false;

        
        if (pCreature->isOusters() ||
            (pCreature->isSlayer() && pCreature->isFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL))) {
            ZoneID_t id = pPacket->getZoneID();
            ZoneCoord_t x = pPacket->getX();
            ZoneCoord_t y = pPacket->getY();

            if (id == 0 && x == 0 && y == 0) {
                bCancel = true;
            } else {
                
                if (pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)) {
                    bCancel = true;
                }

                
                
                if (!g_pWayPointManager->isValidWayPoint(id, x, y, pCreature->getRace())) {
                    
                    bCancel = true;

                    // return;
                }

                try {
                    if (!bCancel) {
#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
                        ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(id);

                        
                        if (pZoneInfo == NULL || ((pZoneInfo->isPayPlay() || pZoneInfo->isPremiumZone()) &&
                                                  (!pGamePlayer->isPayPlaying() && !pGamePlayer->isFamilyFreePass()))) {
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

                                bCancel = true;
                            }
                        }
#endif

                        if (!bCancel) {
                            
                            if (pCreature->isSlayer())
                                pCreature->removeFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL);

                            if (pCreature->isOusters()) {
                                Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
                                Assert(pOusters != NULL);

                                GCNoticeEvent gcNoticeEvent;

                                
                                if (!pOusters->getFlagSet()->isOn(FLAGSET_GNOMES_HORN)) {
                                    gcNoticeEvent.setCode(NOTICE_EVENT_CONTRACT_GNOMES_HORN);
                                    pPlayer->sendPacket(&gcNoticeEvent);
                                    return;
                                }

                                Level_t level = pOusters->getLevel();
                                Price_t price = sPriceMap[level];

                                if (price == 0) {
                                    price = (Price_t)(pow((double)level, 1.3) * 100) / 2;
                                    sPriceMap[level] = price;
                                }

                                /*if (g_pFlagManager->hasFlagWar() &&
                                        pPacket->getZoneID() == 32 &&
                                        pPacket->getX() == 124 &&
                                        pPacket->getY() == 144 ) price = 0;*/

                                if (pOusters->getGold() < price) {
                                    gcNoticeEvent.setCode(NOTICE_EVENT_NOT_ENOUGH_MONEY);
                                    pPlayer->sendPacket(&gcNoticeEvent);
                                    return;
                                } else {
                                    pOusters->decreaseGoldEx(price);
                                    GCModifyInformation gcMI;
                                    gcMI.addLongData(MODIFY_GOLD, pOusters->getGold());
                                    pPlayer->sendPacket(&gcMI);
                                }
                            }

                            
                            pPC->getGQuestManager()->illegalWarp();
                            transportCreature(pCreature, id, x, y, false);
                        }
                    }
                } catch (NoSuchElementException&) {
                    bCancel = true;
                }
            }
        }

        if (bCancel && pCreature->isSlayer()) {
            Zone* pZone = pCreature->getZone();
            Assert(pZone != NULL);

            
            pCreature->removeFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL);

            
            GCAddHelicopter gcAddHelicopter;
            gcAddHelicopter.setObjectID(pCreature->getObjectID());
            gcAddHelicopter.setCode(1);
            pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddHelicopter);
        }
    } catch (Throwable& t) {
        cerr << t.toString() << endl;
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}


void CGSelectWayPointHandler::executeEnterQuestZone(CGSelectWayPoint* pPacket, Player* pPlayer,
                                                    int targetDynamicZoneType)

{
    __BEGIN_TRY

#ifdef __GAME_SERVER__

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Assert(pPC != NULL);

    bool bQuestCondition = pPC->getGQuestManager()->canEnterDynamicZone(pPacket->getZoneID());

    DynamicZoneGroup* pDynamicZoneGroup = g_pDynamicZoneManager->getDynamicZoneGroup(targetDynamicZoneType);
    Assert(pDynamicZoneGroup != NULL);

    bool bDynamicZoneAvailable = pDynamicZoneGroup->canEnter();

    if (bQuestCondition && bDynamicZoneAvailable) {
        DynamicZone* pDynamicZone = pDynamicZoneGroup->getAvailableDynamicZone();

        transportCreature(pPC, pDynamicZone->getZoneID(), 15, 15, true);

        pPC->getGQuestManager()->enterDynamicZone(pPacket->getZoneID());
    }

#endif

    __END_CATCH
}
