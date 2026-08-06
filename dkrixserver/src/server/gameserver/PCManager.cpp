//////////////////////////////////////////////////////////////////////////////
// Filename    : PCManager.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "PCManager.h"

#include <stdio.h>

#include <algorithm> // find_if ()

#include "AlignmentManager.h"
#include "Assert.h"
#include "BloodBibleBonusManager.h"
#include "Creature.h"
#include "CreatureUtil.h"
#include "Event.h"
#include "EventResurrect.h"
#include "GamePlayer.h"
#include "HolyLandManager.h"
#include "IncomingPlayerManager.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "LogClient.h"
#include "MonsterCorpse.h"
#include "Ousters.h"
#include "OustersCorpse.h"
#include "PKZoneInfoManager.h"
#include "PacketUtil.h"
#include "PlayerCreature.h"
#include "Relic.h"
#include "RelicUtil.h"
#include "ResurrectLocationManager.h"
#include "SkillHandlerManager.h"
#include "Slayer.h"
#include "SlayerCorpse.h"
#include "Thread.h"
#include "Tile.h"
#include "TimeManager.h"
#include "TradeManager.h"
#include "Vampire.h"
#include "VampireCorpse.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "ZoneUtil.h"
// #include "SweeperBonusManager.h"
#include "EffectComa.h"
#include "EffectPKZoneResurrection.h"
#include "EffectSummonCasket.h"
#include "EffectTryingPosition.h"
#include "EventTransport.h"
#include "EventZoneInfo.h"
#include "GCAddEffect.h"
#include "GCCreatureDied.h"
#include "GCGetOffMotorCycle.h"
#include "GCHolyLandBonusInfo.h"
#include "GCRemoveEffect.h"
#include "GCRemoveFromGear.h"
#include "GCSystemMessage.h"
#include "GQuestManager.h"
#include "LevelWarZoneInfoManager.h"
#include "Properties.h"
#include "SiegeManager.h"
#include "skill/EffectHarpoonBomb.h"
#include "war/WarSystem.h"
// #include "GCSweeperBonusInfo.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
PCManager::PCManager()

{
    __BEGIN_TRY

    m_bRefreshHolyLandPlayer = false;
    //	m_bRefreshLevelWarBonusZonePlayer = false;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor

//////////////////////////////////////////////////////////////////////////////
PCManager::~PCManager()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////////////




//





//////////////////////////////////////////////////////////////////////////////
void PCManager::processCreatures()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    // cerr << "TID[" << Thread::self() << "]" << "PCM::process go" << endl;

    Timeval currentTime;
    getCurrentTime(currentTime);

    try {
        unordered_map<ObjectID_t, Creature*>::iterator before = m_Creatures.end();
        unordered_map<ObjectID_t, Creature*>::iterator current = m_Creatures.begin();

        while (current != m_Creatures.end()) {
            Creature* pCreature = current->second;
            Assert(pCreature != NULL);

            
            if (pCreature->isSlayer()) {
                Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
                Assert(pSlayer != NULL);
                pSlayer->heartbeat(currentTime);

                 

                
                if (m_bRefreshHolyLandPlayer && !g_pWarSystem->hasActiveRaceWar()) {
                    SLAYER_RECORD prev;

                    pSlayer->getSlayerRecord(prev);
                    pSlayer->initAllStat();
                    pSlayer->sendRealWearingInfo();
                    pSlayer->sendModifyInfo(prev);
                    
                    pSlayer->sendSlayerSkillInfo();
                }
            } else if (pCreature->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                Assert(pVampire != NULL);
                pVampire->heartbeat(currentTime);

                 
                if (pVampire->isFlag(Effect::EFFECT_CLASS_COMA)) {
                    HP_t currentHP = pVampire->getHP(ATTR_CURRENT);
                    HP_t maxHP = pVampire->getHP(ATTR_MAX);

                    if (currentHP * 3 >= maxHP) {
                        // cout << "Target HP is over 1/3" << endl;

                        EffectComa* pEffectComa =
                            dynamic_cast<EffectComa*>(pVampire->findEffect(Effect::EFFECT_CLASS_COMA));
                        Assert(pEffectComa != NULL);

                        if (pEffectComa->canResurrect()) {
                            // cout << "Can Resurrect!" << endl;

                            
                            pVampire->deleteEffect(Effect::EFFECT_CLASS_COMA);
                            pVampire->removeFlag(Effect::EFFECT_CLASS_COMA);

                            
                            GCRemoveEffect gcRemoveEffect;
                            gcRemoveEffect.setObjectID(pVampire->getObjectID());
                            gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
                            pVampire->getZone()->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcRemoveEffect);

                            
                            pVampire->getEffectManager()->sendEffectInfo(pVampire, pVampire->getZone(),
                                                                         pVampire->getX(), pVampire->getY());
                        }
                    }
                }

                
                if (m_bRefreshHolyLandPlayer && !g_pWarSystem->hasActiveRaceWar()) {
                    VAMPIRE_RECORD prev;

                    pVampire->getVampireRecord(prev);
                    pVampire->initAllStat();
                    pVampire->sendRealWearingInfo();
                    pVampire->sendModifyInfo(prev);
                    
                    pVampire->sendVampireSkillInfo();
                }
            } else if (pCreature->isOusters()) {
                Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
                Assert(pOusters != NULL);
                pOusters->heartbeat(currentTime);
            }

            if (pCreature->isDead()
                
                
                || pCreature->isFlag(Effect::EFFECT_CLASS_COMA) && pCreature->isVampire()) {
                if (!pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
                    ///////////////////////////////////////////////////////////////////
                    
                    ///////////////////////////////////////////////////////////////////
                    dropRelicToZone(pCreature);
                    dropFlagToZone(pCreature);
                    dropSweeperToZone(pCreature);

                    ///////////////////////////////////////////////////////////////////
                    
                    ///////////////////////////////////////////////////////////////////
                    Zone* pZone = pCreature->getZone();
                    Assert(pZone != NULL);

                    Tile& rTile = pZone->getTile(pCreature->getX(), pCreature->getY());
                    EffectTryingPosition* pTryingTile =
                        dynamic_cast<EffectTryingPosition*>(rTile.getEffect(Effect::EFFECT_CLASS_TRYING_POSITION));
                    if (pTryingTile != NULL) {
                        MonsterCorpse* pTower = pTryingTile->getTower();
                        Assert(pTower != NULL);

                        Effect* pTryingTower =
                            pTower->getEffectManager().findEffect(Effect::EFFECT_CLASS_SLAYER_TRYING_1);
                        if (pTryingTower != NULL)
                            pTryingTower->setDeadline(0);

                        pTryingTower = pCreature->findEffect(Effect::EFFECT_CLASS_TRYING);
                        if (pTryingTower != NULL)
                            pTryingTower->setDeadline(0);
                    }

                    Slayer* pSlayer = NULL;
                    Vampire* pVampire = NULL;
                    Ousters* pOusters = NULL;

                    Creature::CreatureClass CClass = pCreature->getCreatureClass();

                    if (CClass == Creature::CREATURE_CLASS_SLAYER) {
                        pSlayer = dynamic_cast<Slayer*>(pCreature);
                        // pCorpse = new SlayerCorpse(dynamic_cast<Slayer*>(pCreature));

                        int SumAttr =
                            pSlayer->getSTR(ATTR_BASIC) + pSlayer->getDEX(ATTR_BASIC) + pSlayer->getINT(ATTR_BASIC);

                        
                        if (SumAttr > 40 && pSlayer->getCompetence() == 3) {
                            Alignment_t alignment = pSlayer->getAlignment();
                            ItemNum_t DropItemNum = g_pAlignmentManager->getDropItemNum(alignment, pSlayer->isPK());

                            
                            
                            for (int i = 0; DropItemNum > 0 && i < Slayer::WEAR_MAX; i++) {
                                Item* pItem = pSlayer->getWearItem((Slayer::WearPart)i);

                                if (pItem != NULL && pItem->isUnique() && !pItem->isTimeLimitItem()) {
                                    // by sigi. 2002.11.7
                                    pSlayer->removeShape(pItem->getItemClass(), true);
                                    /*
                                    GCRemoveFromGear gcRemoveFromGear;
                                    gcRemoveFromGear.setSlotID(i);
                                    gcRemoveFromGear.setDestroy(false);
                                    pSlayer->getPlayer()->sendPacket(&gcRemoveFromGear);
                                    */


                                    if (isTwohandWeapon(pItem)) {
                                        pSlayer->deleteWearItem(Slayer::WEAR_LEFTHAND);
                                        pSlayer->deleteWearItem(Slayer::WEAR_RIGHTHAND);
                                    } else {
                                        pSlayer->deleteWearItem((Slayer::WearPart)i);
                                    }

                                    
                                    TPOINT pt = pZone->addItem(pItem, pSlayer->getX(), pSlayer->getY());

                                    if (pt.x != -1) {
                                        filelog("uniqueItem.txt", "DropByKilled: %s %s", pSlayer->getName().c_str(),
                                                pItem->toString().c_str());
                                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);

                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            char zoneName[15];
                                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                            remainTraceLog(pItem, pCreature->getName(), zoneName, ITEM_LOG_MOVE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_MOVE,
                                                              pZone->getZoneID(), pt.x, pt.y);
                                        }
                                    } else {
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            remainTraceLog(pItem, pCreature->getName(), "GOD", ITEM_LOG_DELETE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_DELETE);
                                        }
                                        pItem->destroy();
                                        SAFE_DELETE(pItem);
                                    }

                                    log(LOG_DROP_ITEM_DIE, pSlayer->getName(), "");

                                    
                                    DropItemNum--;

                                    
                                    break;
                                }
                            }


                            for (int i = 0; i < DropItemNum; i++) {
                                int RandomValue = Random(0, (int)Slayer::WEAR_MAX - 1);
                                Item* pItem = pSlayer->getWearItem(Slayer::WearPart(RandomValue));

                                
                                // 2003.3.14
                                if (pItem != NULL && !isCoupleRing(pItem) && !pItem->isTimeLimitItem()) {
                                    // by sigi. 2002.11.7
                                    pSlayer->removeShape(pItem->getItemClass(), true);
                                    /*
                                    GCRemoveFromGear gcRemoveFromGear;
                                    gcRemoveFromGear.setSlotID(i);
                                    gcRemoveFromGear.setDestroy(false);
                                    pSlayer->getPlayer()->sendPacket(&gcRemoveFromGear);
                                    */

                                    if (isTwohandWeapon(pItem)) {
                                        pSlayer->deleteWearItem(Slayer::WEAR_LEFTHAND);
                                        pSlayer->deleteWearItem(Slayer::WEAR_RIGHTHAND);
                                    } else {
                                        pSlayer->deleteWearItem(Slayer::WearPart(RandomValue));
                                    }

                                    // pItem->destroy();
                                    // pCorpse->addTreasure(pItem);

                                    
                                    TPOINT pt = pZone->addItem(pItem, pSlayer->getX(), pSlayer->getY());

                                    if (pt.x != -1) {
                                        filelog("dropItem.txt", "DropByKilled: %s %s", pSlayer->getName().c_str(),
                                                pItem->toString().c_str());
                                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);

                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            char zoneName[15];
                                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                            remainTraceLog(pItem, pCreature->getName(), zoneName, ITEM_LOG_MOVE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_MOVE,
                                                              pZone->getZoneID(), pt.x, pt.y);
                                        }
                                    } else {
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            remainTraceLog(pItem, pCreature->getName(), "GOD", ITEM_LOG_DELETE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_DELETE);
                                        }
                                        pItem->destroy();
                                        SAFE_DELETE(pItem);
                                    }

                                    log(LOG_DROP_ITEM_DIE, pSlayer->getName(), "");
                                }
                            }
                        }

                        
                        pSlayer->setPK(false);
                    } else if (CClass == Creature::CREATURE_CLASS_VAMPIRE) {
                        pVampire = dynamic_cast<Vampire*>(pCreature);
                        // pCorpse = new VampireCorpse(dynamic_cast<Vampire*>(pCreature));

                        
                        // 2003. 1. 17. Sequoia, DEW
                        if (pVampire->isFlag(Effect::EFFECT_CLASS_HIDE)) {
                            if (canUnburrow(pZone, pVampire->getX(), pVampire->getY())) {
                                addUnburrowCreature(pZone, pVampire, pVampire->getX(), pVampire->getY(),
                                                    pVampire->getDir());
                            }
                        }

                        
                        if (pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
                            pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF) ||
                            pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
                            Zone* pZone = pVampire->getZone();
                            Assert(pZone != NULL);
                            addUntransformCreature(pZone, pVampire, true);
                        }

                        
                        /*						if ( pVampire->isFlag( Effect::EFFECT_CLASS_CASKET ) )
                                                {
                                                    EffectSummonCasket* pEffect = dynamic_cast<EffectSummonCasket*>(
                           pVampire->findEffect( Effect::EFFECT_CLASS_CASKET ) ); Assert( pEffect != NULL );

                                                    pEffect->setDeadline(0);
                                                }*/

                        
                        if (pVampire->getLevel() > 10 && pVampire->getCompetence() == 3) {
                            Alignment_t alignment = pVampire->getAlignment();
                            ItemNum_t DropItemNum = g_pAlignmentManager->getDropItemNum(alignment, pVampire->isPK());

                            
                            
                            for (int i = 0; DropItemNum > 0 && i < Vampire::VAMPIRE_WEAR_MAX; i++) {
                                Item* pItem = pVampire->getWearItem((Vampire::WearPart)i);

                                if (pItem != NULL && pItem->isUnique() && !pItem->isTimeLimitItem()) {
                                    // by sigi. 2002.11.7
                                    pVampire->removeShape(pItem->getItemClass(), true);
                                    /*
                                    GCRemoveFromGear gcRemoveFromGear;
                                    gcRemoveFromGear.setSlotID(i);
                                    gcRemoveFromGear.setDestroy(false);
                                    pVampire->getPlayer()->sendPacket(&gcRemoveFromGear);
                                    */

                                    if (isTwohandWeapon(pItem)) {
                                        pVampire->deleteWearItem(Vampire::WEAR_LEFTHAND);
                                        pVampire->deleteWearItem(Vampire::WEAR_RIGHTHAND);
                                    } else {
                                        pVampire->deleteWearItem((Vampire::WearPart)i);
                                    }

                                    
                                    TPOINT pt = pZone->addItem(pItem, pVampire->getX(), pVampire->getY());

                                    if (pt.x != -1) {
                                        filelog("uniqueItem.txt", "DropByKilled: %s %s", pVampire->getName().c_str(),
                                                pItem->toString().c_str());
                                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            char zoneName[15];
                                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                            remainTraceLog(pItem, pCreature->getName(), zoneName, ITEM_LOG_MOVE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_MOVE,
                                                              pZone->getZoneID(), pt.x, pt.y);
                                        }
                                    } else {
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            remainTraceLog(pItem, pCreature->getName(), "GOD", ITEM_LOG_DELETE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_DELETE);
                                        }
                                        pItem->destroy();
                                        SAFE_DELETE(pItem);
                                    }

                                    log(LOG_DROP_ITEM_DIE, pVampire->getName(), "");

                                    
                                    DropItemNum--;

                                    
                                    break;
                                }
                            }


                            for (int i = 0; i < DropItemNum; i++) {
                                int RandomValue = Random(0, (int)Vampire::VAMPIRE_WEAR_MAX - 1);
                                Item* pItem = pVampire->getWearItem(Vampire::WearPart(RandomValue));

                                
                                // 2003.3.14
                                if (pItem != NULL && !isCoupleRing(pItem) && !pItem->isTimeLimitItem()) {
                                    // by sigi. 2002.11.7
                                    pVampire->removeShape(pItem->getItemClass(), true);
                                    /*
                                    GCRemoveFromGear gcRemoveFromGear;
                                    gcRemoveFromGear.setSlotID(i);
                                    gcRemoveFromGear.setDestroy(false);
                                    pVampire->getPlayer()->sendPacket(&gcRemoveFromGear);
                                    */

                                    if (isTwohandWeapon(pItem)) {
                                        pVampire->deleteWearItem(Vampire::WEAR_LEFTHAND);
                                        pVampire->deleteWearItem(Vampire::WEAR_RIGHTHAND);
                                    } else {
                                        pVampire->deleteWearItem(Vampire::WearPart(RandomValue));
                                    }
                                    // pItem->destroy();
                                    // pCorpse->addTreasure(pItem);

                                    
                                    TPOINT pt = pZone->addItem(pItem, pVampire->getX(), pVampire->getY());

                                    if (pt.x != -1) {
                                        filelog("dropItem.txt", "DropByKilled: %s %s", pVampire->getName().c_str(),
                                                pItem->toString().c_str());
                                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            char zoneName[15];
                                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                            remainTraceLog(pItem, pCreature->getName(), zoneName, ITEM_LOG_MOVE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_MOVE,
                                                              pZone->getZoneID(), pt.x, pt.y);
                                        }
                                    } else {
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            remainTraceLog(pItem, pCreature->getName(), "GOD", ITEM_LOG_DELETE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_DELETE);
                                        }
                                        pItem->destroy();
                                        SAFE_DELETE(pItem);
                                    }

                                    log(LOG_DROP_ITEM_DIE, pVampire->getName(), "");
                                }
                            }
                        }

                        SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_EXTREME);
                        Assert(pSkillHandler != NULL);
                        
                        pSkillHandler->execute(pVampire);

                        
                        pVampire->setPK(false);
                    } else if (CClass == Creature::CREATURE_CLASS_OUSTERS) {
                        pOusters = dynamic_cast<Ousters*>(pCreature);

                        
                        if (pOusters->getLevel() > 10 && pOusters->getCompetence() == 3) {
                            Alignment_t alignment = pOusters->getAlignment();
                            ItemNum_t DropItemNum = g_pAlignmentManager->getDropItemNum(alignment, pOusters->isPK());

                            
                            
                            for (int i = 0; DropItemNum > 0 && i < Ousters::OUSTERS_WEAR_MAX; i++) {
                                Item* pItem = pOusters->getWearItem((Ousters::WearPart)i);

                                if (pItem != NULL && pItem->isUnique() && !pItem->isTimeLimitItem()) {
                                    // by sigi. 2002.11.7
                                    pOusters->removeShape(pItem->getItemClass(), true);
                                    /*
                                    GCRemoveFromGear gcRemoveFromGear;
                                    gcRemoveFromGear.setSlotID(i);
                                    gcRemoveFromGear.setDestroy(false);
                                    pOusters->getPlayer()->sendPacket(&gcRemoveFromGear);
                                    */

                                    if (isTwohandWeapon(pItem)) {
                                        pOusters->deleteWearItem(Ousters::WEAR_LEFTHAND);
                                        pOusters->deleteWearItem(Ousters::WEAR_RIGHTHAND);
                                    } else {
                                        pOusters->deleteWearItem((Ousters::WearPart)i);
                                    }

                                    
                                    TPOINT pt = pZone->addItem(pItem, pOusters->getX(), pOusters->getY());

                                    if (pt.x != -1) {
                                        filelog("uniqueItem.txt", "DropByKilled: %s %s", pOusters->getName().c_str(),
                                                pItem->toString().c_str());
                                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            char zoneName[15];
                                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                            remainTraceLog(pItem, pCreature->getName(), zoneName, ITEM_LOG_MOVE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_MOVE,
                                                              pZone->getZoneID(), pt.x, pt.y);
                                        }
                                    } else {
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            remainTraceLog(pItem, pCreature->getName(), "GOD", ITEM_LOG_DELETE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_DELETE);
                                        }
                                        pItem->destroy();
                                        SAFE_DELETE(pItem);
                                    }

                                    log(LOG_DROP_ITEM_DIE, pOusters->getName(), "");

                                    
                                    DropItemNum--;

                                    
                                    break;
                                }
                            }

                            for (int i = 0; i < DropItemNum; i++) {
                                int RandomValue = Random(0, (int)Ousters::OUSTERS_WEAR_MAX - 1);
                                Item* pItem = pOusters->getWearItem(Ousters::WearPart(RandomValue));

                                
                                // 2003.3.14
                                if (pItem != NULL && !isCoupleRing(pItem) && !pItem->isTimeLimitItem()) {
                                    // by sigi. 2002.11.7
                                    pOusters->removeShape(pItem->getItemClass(), true);
                                    /*
                                    GCRemoveFromGear gcRemoveFromGear;
                                    gcRemoveFromGear.setSlotID(i);
                                    gcRemoveFromGear.setDestroy(false);
                                    pOusters->getPlayer()->sendPacket(&gcRemoveFromGear);
                                    */

                                    if (isTwohandWeapon(pItem)) {
                                        pOusters->deleteWearItem(Ousters::WEAR_LEFTHAND);
                                        pOusters->deleteWearItem(Ousters::WEAR_RIGHTHAND);
                                    } else {
                                        pOusters->deleteWearItem(Ousters::WearPart(RandomValue));
                                    }
                                    // pItem->destroy();
                                    // pCorpse->addTreasure(pItem);

                                    
                                    TPOINT pt = pZone->addItem(pItem, pOusters->getX(), pOusters->getY());

                                    if (pt.x != -1) {
                                        filelog("dropItem.txt", "DropByKilled: %s %s", pOusters->getName().c_str(),
                                                pItem->toString().c_str());
                                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            char zoneName[15];
                                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                            remainTraceLog(pItem, pCreature->getName(), zoneName, ITEM_LOG_MOVE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_MOVE,
                                                              pZone->getZoneID(), pt.x, pt.y);
                                        }
                                    } else {
                                        
                                        if (pItem != NULL && pItem->isTraceItem()) {
                                            remainTraceLog(pItem, pCreature->getName(), "GOD", ITEM_LOG_DELETE,
                                                           DETAIL_DROP);
                                            remainTraceLogNew(pItem, pCreature->getName(), ITL_DROP, ITLD_DELETE);
                                        }
                                        pItem->destroy();
                                        SAFE_DELETE(pItem);
                                    }

                                    log(LOG_DROP_ITEM_DIE, pOusters->getName(), "");
                                }
                            }
                        }

                        
                        pOusters->setPK(false);
                    } else {
                        throw Error("invalid creature class");
                    }

                    
                    
                    EffectComa* pEffectComa = new EffectComa(pCreature);
                    pEffectComa->setStartTime();
                    if (pTryingTile != NULL)
                        pEffectComa->setDeadline(0);
                    else
                        pEffectComa->setDeadline(600);

                    EffectManager* pEffectManager = pCreature->getEffectManager();
                    Assert(pEffectManager != NULL);
                    pEffectManager->addEffect(pEffectComa);

                    pCreature->setFlag(Effect::EFFECT_CLASS_COMA);

                    PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
                    Assert(pPlayerCreature != NULL);

                    pPlayerCreature->getGQuestManager()->killed();

                    // Inventory* pInventory = pPlayerCreature->getInventory();

                    if (pCreature->isSlayer()) {
                        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

                        
                        if (pSlayer->hasRideMotorcycle()) {
                            Zone* pZone = pCreature->getZone();
                            Assert(pZone != NULL);

                            pSlayer->getOffMotorcycle();
                            GCGetOffMotorCycle _GCGetOffMotorCycle;
                            _GCGetOffMotorCycle.setObjectID(pSlayer->getObjectID());
                            pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCGetOffMotorCycle);
                        }

                        
                        if (pSlayer->isFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL)) {
                            pSlayer->removeFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL);

                            
                            // GCAddHelicopter gcAddHelicopter;
                            // gcAddHelicopter.setObjectID(pSlayer->getObjectID());
                            // gcAddHelicopter.setCode(1);
                            // pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddHelicopter);
                        }
                    } else if (pCreature->isVampire()) {
                        // Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                        
                        if (pCreature->isFlag(Effect::EFFECT_CLASS_CASKET)) {
                            Effect* pEffectCasket = pCreature->findEffect(Effect::EFFECT_CLASS_CASKET);

                            if (pEffectCasket != NULL) {
                                // cout << "Coma --> casket unaffect" << endl;
                                pEffectCasket->unaffect();
                            }

                            pCreature->deleteEffect(Effect::EFFECT_CLASS_CASKET);
                            pCreature->removeFlag(Effect::EFFECT_CLASS_CASKET);
                        }
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_EXPLOSION_WATER);
                        if (pEffect != NULL)
                            pEffect->unaffect();
                        pCreature->removeFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_EXPLOSION_WATER);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1);
                        if (pEffect != NULL)
                            pEffect->unaffect();
                        pCreature->removeFlag(Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_INSTALL_TURRET);
                        if (pEffect != NULL)
                            pEffect->unaffect();
                        pCreature->removeFlag(Effect::EFFECT_CLASS_INSTALL_TURRET);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_INSTALL_TURRET);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_DIVINE_GUIDANCE)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_DIVINE_GUIDANCE);
                        if (pEffect != NULL)
                            pEffect->unaffect();
                        pCreature->removeFlag(Effect::EFFECT_CLASS_DIVINE_GUIDANCE);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_DIVINE_GUIDANCE);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_GLACIER)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_GLACIER);
                        if (pEffect != NULL)
                            pEffect->unaffect();
                        pCreature->removeFlag(Effect::EFFECT_CLASS_GLACIER);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_GLACIER);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_ACID_ERUPTION)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_ACID_ERUPTION);
                        if (pEffect != NULL)
                            pEffect->unaffect();
                        pCreature->removeFlag(Effect::EFFECT_CLASS_ACID_ERUPTION);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_ACID_ERUPTION);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)) {
                        pCreature->removeFlag(Effect::EFFECT_CLASS_FADE_OUT);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_FADE_OUT);
                        
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET)) {
                        pCreature->removeFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET);
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_REFINIUM_TICKET);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_SUMMON_SYLPH);

                        if (pEffect != NULL) {
                            pEffect->unaffect();
                        }
                        
                        pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_SUMMON_SYLPH);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_INVISIBILITY);

                        if (pEffect != NULL) {
                            pEffect->unaffect();
                        }
                        
                        pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_INVISIBILITY);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_INVISIBILITY);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_SNIPING_MODE);

                        if (pEffect != NULL) {
                            pEffect->unaffect();
                        }
                        
                        pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_SNIPING_MODE);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_SNIPING_MODE);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)) {
                        Effect* pEffectPal = pCreature->findEffect(Effect::EFFECT_CLASS_PARALYZE);

                        if (pEffectPal != NULL) {
                            pEffectPal->unaffect();
                        }
                        
                        pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_PARALYZE);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_PARALYZE);
                    }

                    
                    if (pCreature->isFlag(Effect::EFFECT_CLASS_HALLUCINATION)) {
                        Effect* pEffectHallu = pCreature->findEffect(Effect::EFFECT_CLASS_HALLUCINATION);

                        if (pEffectHallu != NULL) {
                            pEffectHallu->unaffect();
                        }

                        pCreature->deleteEffect(Effect::EFFECT_CLASS_HALLUCINATION);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_HALLUCINATION);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_MAGNUM_SPEAR)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_MAGNUM_SPEAR);
                        if (pEffect != NULL) {
                            pEffect->unaffect();
                        }
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_MAGNUM_SPEAR);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_MAGNUM_SPEAR);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);
                        if (pEffect != NULL) {
                            pEffect->unaffect();
                        }
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);
                    }

                    
                    
                    if (pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN)) {
                        // pCreature->deleteEffect( Effect::EFFECT_CLASS_SOUL_CHAIN );
                        pCreature->removeFlag(Effect::EFFECT_CLASS_SOUL_CHAIN);
                    }

                    
                    
                    if (pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN)) {
                        pCreature->removeFlag(Effect::EFFECT_CLASS_LOVE_CHAIN);
                    }

                    
                    if (pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM)) {
                        pCreature->deleteEffect(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM);
                    }

                    
                    if (pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP)) {
                        Effect* pEffectSleep = pCreature->findEffect(Effect::EFFECT_CLASS_SLEEP);

                        if (pEffectSleep != NULL) {
                            pEffectSleep->unaffect();
                        }

                        pCreature->deleteEffect(Effect::EFFECT_CLASS_SLEEP);
                    }
                    if (pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)) {
                        Effect* pEffectArma = pCreature->findEffect(Effect::EFFECT_CLASS_ARMAGEDDON);

                        if (pEffectArma != NULL) {
                            pEffectArma->unaffect();
                        }

                        pCreature->deleteEffect(Effect::EFFECT_CLASS_ARMAGEDDON);
                    }

                    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED)) {
                        Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_TRAPPED);

                        if (pEffect != NULL) {
                            pEffect->unaffect();
                        }

                        pCreature->deleteEffect(Effect::EFFECT_CLASS_TRAPPED);
                    }

                    
                    GCAddEffect gcAddEffect;
                    gcAddEffect.setObjectID(pCreature->getObjectID());
                    gcAddEffect.setEffectID(Effect::EFFECT_CLASS_COMA);
                    gcAddEffect.setDuration(300);
                    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);

                    if (g_pConfig->hasKey("Hardcore") && g_pConfig->getPropertyInt("Hardcore") != 0) {
                        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                        Assert(pPC != NULL);

                        deletePC(pPC);
                        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
                        Assert(pGamePlayer != NULL);

                        filelog("DeletePC.log", "  : %s", pPC->getName().c_str());

                        pGamePlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                        pGamePlayer->setItemRatioBonusPoint(1);
                    }

                    before = current++;
                } else {
                    EffectManager* pEffectManager = pCreature->getEffectManager();
                    Assert(pEffectManager != NULL);

                    EffectComa* pEffectComa = (EffectComa*)(pEffectManager->findEffect(Effect::EFFECT_CLASS_COMA));
                    Assert(pEffectComa != NULL);

                    if (pEffectComa->getDeadline() < currentTime) {
                        
                        pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_COMA);
                        pCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

                        
                        
                        killCreature(pCreature);

                        
                        m_Creatures.erase(current);

                        if (before == m_Creatures.end()) // first element
                        {
                            current = m_Creatures.begin();
                        } else // !first element
                        {
                            current = before;
                            current++;
                        }
                    } else {
                        //						Effect* pHarpoonBomb = pEffectManager->findEffect(
                        // Effect::EFFECT_CLASS_HARPOON_BOMB ); 						if ( pHarpoonBomb != NULL &&
                        // pHarpoonBomb->getNextTime() < currentTime )
                        //						{
                        //							pHarpoonBomb->affect();
                        //						}
                        //
                        before = current++;
                    }
                }
            } else {
                before = current++;

                
                pCreature->getEffectManager()->heartbeat(currentTime);
            }
        }

        
        
        if (m_bRefreshHolyLandPlayer && !g_pWarSystem->hasActiveRaceWar()) {
            
            GCHolyLandBonusInfo gcHolyLandBonusInfo;
            g_pBloodBibleBonusManager->makeHolyLandBonusInfo(gcHolyLandBonusInfo);
            g_pHolyLandManager->broadcast(&gcHolyLandBonusInfo);
        }

        /*
        if ( m_bRefreshLevelWarBonusZonePlayer && g_pSweeperBonusManager->isAble() )
        {
            GCSweeperBonusInfo gcSweeperBonusInfo;
            g_pSweeperBonusManager->makeSweeperBonusInfo( gcSweeperBonusInfo );
            g_pLevelWarZoneInfoManager->broadcast( &gcSweeperBonusInfo );
        }

        if ( m_bRefreshLevelWarBonusZonePlayer && !g_pSweeperBonusManager->isAble() )
        {
            GCSweeperBonusInfo gcSweeperBonusInfo;
            g_pSweeperBonusManager->makeVoidSweeperBonusInfo( gcSweeperBonusInfo );
            g_pLevelWarZoneInfoManager->broadcast( &gcSweeperBonusInfo );
        }
        */

        m_bRefreshHolyLandPlayer = false;
        //	m_bRefreshLevelWarBonusZonePlayer = false;

    } catch (Throwable& t) {
        filelog("PCManagerBug.log", "ProcessCreatureBug : %s", t.toString().c_str());
        // cerr << t.toString() << endl;
    }

    // cerr << "TID[" << Thread::self() << "]" << "PCM::process end" << endl;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////
void PCManager::killCreature(Creature* pDeadCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pDeadCreature != NULL);

    
    
    // Assert(pDeadCreature->isDead());

    Zone* pZone = pDeadCreature->getZone();
    Assert(pZone != NULL);

    ZoneCoord_t cx = pDeadCreature->getX();
    ZoneCoord_t cy = pDeadCreature->getY();

    // Eternity
    pDeadCreature->removeFlag(Effect::EFFECT_CLASS_ETERNITY);

    
    if (g_pPKZoneInfoManager->isPKZone(pZone->getZoneID())) {
        EffectPKZoneResurrection* pEffect = new EffectPKZoneResurrection(pDeadCreature);
        pDeadCreature->addEffect(pEffect);
        pDeadCreature->setFlag(pEffect->getEffectClass());
    }

    
    PartyInviteInfoManager* pPIIM = pZone->getPartyInviteInfoManager();
    pPIIM->cancelInvite(pDeadCreature);

    
    uint PartyID = pDeadCreature->getPartyID();
    if (PartyID != 0) {
        LocalPartyManager* pLPM = pZone->getLocalPartyManager();
        pLPM->deletePartyMember(PartyID, pDeadCreature);
    }

    
    TradeManager* pTradeManager = pZone->getTradeManager();
    TradeInfo* pInfo = pTradeManager->getTradeInfo(pDeadCreature->getName());
    if (pInfo != NULL) {
        pTradeManager->cancelTrade(pDeadCreature);
    }

    
    if (pDeadCreature->isFlag(Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL)) {
        pDeadCreature->removeFlag(Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL);
    }

    bool addCorpse = true;
    if (pDeadCreature->isFlag(Effect::EFFECT_CLASS_HARPOON_BOMB)) {
        Effect* pEffect = pDeadCreature->findEffect(Effect::EFFECT_CLASS_HARPOON_BOMB);
        if (pEffect == NULL)
            addCorpse = false;
    }

    if (addCorpse) {
        
        Corpse* pCorpse = NULL;

        if (pDeadCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pDeadCreature);
            pCorpse = new SlayerCorpse(pSlayer);
            pCorpse->setLevel((int)(pSlayer->getHighestSkillDomainLevel()));
            pCorpse->setExp((Exp_t)computeCreatureExp(pSlayer, BLOODDRAIN_EXP));
        } else if (pDeadCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pDeadCreature);
            pCorpse = new VampireCorpse(pVampire);
            pCorpse->setLevel((int)(pVampire->getLevel()));
            pCorpse->setExp((Exp_t)computeCreatureExp(pVampire, BLOODDRAIN_EXP));
        } else if (pDeadCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pDeadCreature);
            pCorpse = new OustersCorpse(pOusters);
            pCorpse->setLevel((int)(pOusters->getLevel()));
            pCorpse->setExp((Exp_t)computeCreatureExp(pOusters, BLOODDRAIN_EXP));
        } else {
            throw Error("PlayerCreature class type ");
        }

        // by sigi. 2002.12.12
        addCorpseToZone(pCorpse, pZone, cx, cy);

        if (pDeadCreature->isFlag(Effect::EFFECT_CLASS_HARPOON_BOMB)) {
            EffectHarpoonBomb* pEffect =
                dynamic_cast<EffectHarpoonBomb*>(pDeadCreature->findEffect(Effect::EFFECT_CLASS_HARPOON_BOMB));
            if (pEffect != NULL) {
                EffectHarpoonBomb* pZoneEffect = new EffectHarpoonBomb(pZone, pCorpse->getX(), pCorpse->getY());
                pZoneEffect->setDamage(pEffect->getDamage());
                pZoneEffect->setUserObjectID(pEffect->getUserObjectID());
                pZoneEffect->setNextTime(pEffect->getNextTime());
                pZoneEffect->setDeadline(pEffect->getRemainDuration());
                //				pDeadCreature->deleteEffect( Effect::EFFECT_CLASS_HARPOON_BOMB );
                pZone->registerObject(pZoneEffect);
                pZone->getTile(pCorpse->getX(), pCorpse->getY()).addEffect(pZoneEffect);
                pZone->addEffect(pZoneEffect);
            }
        }
    }

    
    GCCreatureDied gcCreatureDied;
    gcCreatureDied.setObjectID(pDeadCreature->getObjectID());
    pDeadCreature->getPlayer()->sendPacket(&gcCreatureDied);
    pZone->broadcastPacket(cx, cy, &gcCreatureDied, pDeadCreature);

    
    
    Tile& tile = pZone->getTile(cx, cy);
    Assert(tile.getCreature(pDeadCreature->getMoveMode()) == pDeadCreature);
    tile.deleteCreature(pDeadCreature->getObjectID());

     

    // *NOTE
    
    
    
    
    ZoneID_t ZoneID = 0;
    ZoneCoord_t ZoneX = 0;
    ZoneCoord_t ZoneY = 0;
    ZONE_COORD ResurrectCoord;
    Zone* pResurrectZone = NULL;

    
    
    ZoneInfo* pZoneInfo = NULL;
    ZoneGroup* pZoneGroup = NULL;

     
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pDeadCreature);
    Assert(pPC != NULL);

    
    EventZoneInfo* pEventZoneInfo = EventZoneInfoManager::Instance().getEventZoneInfo(pPC->getZoneID());
    if (pEventZoneInfo != NULL) {
        ResurrectCoord.id = pPC->getZoneID();
        ResurrectCoord.x = pEventZoneInfo->getResurrectX();
        ResurrectCoord.y = pEventZoneInfo->getResurrectY();
    }
    
    
    else if (g_pPKZoneInfoManager->isPKZone(pPC->getZoneID())) {
        if (!g_pPKZoneInfoManager->getResurrectPosition(pPC->getZoneID(), ResurrectCoord))
            g_pResurrectLocationManager->getPosition(pPC, ResurrectCoord);
    }
    
    else if (pPC->getZoneID() == 1410) {
        ResurrectCoord.id = 1410;
        ResurrectCoord.x = 120;
        ResurrectCoord.y = 70;
    } else if (pPC->getZoneID() == 1411) {
        ResurrectCoord.id = 1411;
        ResurrectCoord.x = 126;
        ResurrectCoord.y = 60;
    } else if (SiegeManager::Instance().isSiegeZone(pPC->getZoneID())) {
        ResurrectCoord.id = pPC->getZoneID();
        if (pPC->isFlag(Effect::EFFECT_CLASS_SIEGE_DEFENDER) || pPC->isFlag(Effect::EFFECT_CLASS_SIEGE_REINFORCE)) {
            ResurrectCoord.x = 172;
            ResurrectCoord.y = 38;
        } else {
            ResurrectCoord.x = 20;
            ResurrectCoord.y = 232;
        }
    } else {
        g_pResurrectLocationManager->getPosition(pPC, ResurrectCoord);
    }

    ZoneID = ResurrectCoord.id;
    ZoneX = ResurrectCoord.x;
    ZoneY = ResurrectCoord.y;

    pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneID);
    pZoneGroup = g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());

    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pDeadCreature->getPlayer());
    EventResurrect* pEventResurrect = new EventResurrect(pGamePlayer);
    pEventResurrect->setDeadline(0);

    
    pResurrectZone = pZoneGroup->getZone(ZoneID);
    Assert(pResurrectZone != NULL);

    if (pZone->isHolyLand() != pResurrectZone->isHolyLand()) {
        
        
        pDeadCreature->setFlag(Effect::EFFECT_CLASS_INIT_ALL_STAT);
    }

    if (g_pLevelWarZoneInfoManager->isCreatureBonusZone(pDeadCreature, pZone->getZoneID()) !=
        g_pLevelWarZoneInfoManager->isCreatureBonusZone(pDeadCreature, pResurrectZone->getZoneID())) {
        pDeadCreature->setFlag(Effect::EFFECT_CLASS_INIT_ALL_STAT);
    }

    if (pZone->isLevelWarZone() != pResurrectZone->isLevelWarZone()) {
        pDeadCreature->setFlag(Effect::EFFECT_CLASS_INIT_ALL_STAT);
    }

    pDeadCreature->setNewZone(pResurrectZone);
    pDeadCreature->setNewXY(ZoneX, ZoneY);

    
    pGamePlayer->addEvent(pEventResurrect);

    
    ZoneCoord_t oldZoneX = pDeadCreature->getX();
    ZoneCoord_t oldZoneY = pDeadCreature->getY();

    
    pDeadCreature->setZone(pResurrectZone);
    pDeadCreature->setXY(ZoneX, ZoneY);

    pDeadCreature->save();
    
    pDeadCreature->setZone(pZone);
    pDeadCreature->setXY(oldZoneX, oldZoneY);

    pGamePlayer->setPlayerStatus(GPS_IGNORE_ALL);

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// transport All Creatures
//////////////////////////////////////////////////////////////////////////////
void PCManager::transportAllCreatures(ZoneID_t ZoneID, ZoneCoord_t ZoneX, ZoneCoord_t ZoneY, Race_t race,
                                      Turn_t delay) const

{
    __BEGIN_TRY

    vector<ObjectID_t> creatureIDs;

    Zone* pZone = NULL;

    
    
    
    __ENTER_CRITICAL_SECTION(m_Mutex)

    unordered_map<ObjectID_t, Creature*>::const_iterator iCreature = m_Creatures.begin();
    while (iCreature != m_Creatures.end()) {
        Creature* pCreature = iCreature->second;
        Assert(pCreature->isPC());

        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

        
        
        if (race == defaultRaceValue || race == pPC->getRace()) {
            creatureIDs.push_back(iCreature->first);

            pZone = pCreature->getZone();
        }

        iCreature++;
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    
    try {
        vector<ObjectID_t>::const_iterator itr = creatureIDs.begin();

        while (itr != creatureIDs.end()) {
            Creature* pCreature = pZone->getCreature(*itr);

            if (pCreature != NULL) {
                //	transportCreature(pCreature, ZoneID, ZoneX, ZoneY, false);
                GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
                EventTransport* pEventTransport =
                    dynamic_cast<EventTransport*>(pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT));
                bool newEvent = false;
                if (pEventTransport == NULL) {
                    pEventTransport = new EventTransport(pGamePlayer);
                    newEvent = true;
                }

                if (ZoneID == 0xffff) {
                    ZONE_COORD ResurrectCoord;
                    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                    Assert(pPC != NULL);
                    g_pResurrectLocationManager->getPosition(pPC, ResurrectCoord);

                    
                    pEventTransport->setDeadline(100);
                    pEventTransport->setZoneName("");
                    pEventTransport->setTargetZone(ResurrectCoord.id, ResurrectCoord.x, ResurrectCoord.y);
                } else {
                    ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneID);
                    Assert(pZoneInfo != NULL);

                    pEventTransport->setDeadline(delay * 10);
                    pEventTransport->setZoneName(pZoneInfo->getFullName());

                    if (ZoneX == 0xffff) {
                        pEventTransport->setTargetZone(ZoneID, pCreature->getX(), pCreature->getY());
                    } else {
                        pEventTransport->setTargetZone(ZoneID, ZoneX, ZoneY);
                        
                        //						pEventTransport->sendMessage();
                    }
                }

                if (newEvent)
                    pGamePlayer->addEvent(pEventTransport);
            }

            itr++;
        }
    } catch (Throwable& t) {
        filelog("PCManagerBug.log", "transportAllCreaturesBUG : %s", t.toString().c_str());
    }


    __END_CATCH
}

vector<uint> PCManager::getPCNumByRace() const {
    vector<uint> ret;
    ret.push_back(0);
    ret.push_back(0);
    ret.push_back(0);

    unordered_map<ObjectID_t, Creature*>::const_iterator itr = getCreatures().begin();

    for (; itr != getCreatures().end(); ++itr) {
        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(itr->second);
        if (pPC != NULL) {
            ret[pPC->getRace()]++;
        }
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string PCManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "PCManager(" << CreatureManager::toString() << ")";

    return msg.toString();

    __END_CATCH
}
