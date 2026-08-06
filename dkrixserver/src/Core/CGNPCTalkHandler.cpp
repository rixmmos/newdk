//////////////////////////////////////////////////////////////////////////////
// Filename    : CGNPCTalkHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGNPCTalk.h"

#ifdef __GAME_SERVER__
#include "GCNPCResponse.h"
#include "GCNPCAsk.h"
#include "GCNPCSayDynamic.h"
#include "GQuestManager.h"
#include "GamePlayer.h"
#include "NPC.h"
#include "PlayerCreature.h"
#include "StringPool.h"
#include "quest/Action.h"
#include "quest/Condition.h"
#include "quest/Trigger.h"
#include "quest/TriggerManager.h"
#endif

const string DiffClanSpeech[] = {
    "! !    .",
    "? ..   !      !!",
    "  ?   .. Abscede Hinc!!"};

//////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
void CGNPCTalkHandler::execute(CGNPCTalk* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX __BEGIN_DEBUG
#ifdef __GAME_SERVER__

        try {
        Assert(pPacket != NULL);
        Assert(pPlayer != NULL);

        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
        Assert(pPC != NULL);
        Zone* pZone = pPC->getZone();

        
        
        
        
        
        
        
        if (pPC->isDead()) {
            return;
        }

        Creature* pNPCBase = NULL;

         

        
        pNPCBase = pZone->getCreature(pPacket->getObjectID());

        if (pNPCBase == NULL) {
            return;
        }

        NPC* pNPC = dynamic_cast<NPC*>(pNPCBase);

        
        
        // by sigi. 2002.11.25
        if (pNPC == NULL) {
            return;
        }

        // Exchange System: Check if this is an Exchange Broker NPC
        // NPC ID for exchange broker (can be configured in database)
        const int EXCHANGE_NPC_ID = 10001;  // TODO: Make configurable
        if (pNPC->getNPCID() == EXCHANGE_NPC_ID) {
            // Send exchange menu to player
            // For now, just send a basic response
            // Client will handle opening the exchange UI
            GCNPCResponse gcNPCResponse;
            pPlayer->sendPacket(&gcNPCResponse);

            // Send NPC ask packet with exchange menu options
            // Client should recognize this NPC ID and open exchange UI
            GCNPCAsk askPacket;
            askPacket.setObjectID(pNPC->getObjectID());
            askPacket.setNPCID(pNPC->getNPCID());
            // Set script ID for exchange broker menu
            askPacket.setScriptID(5001); // Exchange Broker Script ID
            pPlayer->sendPacket(&askPacket);

            return;
        }

        
        if (pPC->getGQuestManager()->metNPC(pNPC)) {
            
            GCNPCResponse gcNPCAskAnswer;
            pPlayer->sendPacket(&gcNPCAskAnswer);

            return;
        }

        
        
        if (!pZone->isHolyLand() || pZone->isCastle()) {
            
            if (pNPC->getRace() == NPC_RACE_SLAYER && pPC->isVampire()) {
                
                GCNPCResponse gcNPCAskAnswer;
                pPlayer->sendPacket(&gcNPCAskAnswer);

                
                GCNPCSayDynamic saypkt;
                saypkt.setObjectID(pNPC->getObjectID());
                
                if (pNPC->getNPCID() == 639) {
                    saypkt.setMessage(g_pStringPool->c_str(STRID_EVENT_NPC_1));
                } else {
                    saypkt.setMessage(g_pStringPool->c_str(STRID_ALERT_VAMPIRE));
                }
                pPlayer->sendPacket(&saypkt);
                return;
            }
            
            else if (pNPC->getRace() == NPC_RACE_SLAYER && pPC->isOusters()) {
                
                GCNPCResponse gcNPCAskAnswer;
                pPlayer->sendPacket(&gcNPCAskAnswer);

                
                GCNPCSayDynamic saypkt;
                saypkt.setObjectID(pNPC->getObjectID());
                saypkt.setMessage(g_pStringPool->c_str(STRID_ALERT_OUSTERS_2));
                pPlayer->sendPacket(&saypkt);
                return;
            }
            
            else if (pNPC->getRace() == NPC_RACE_VAMPIRE && pPC->isSlayer()) {
                
                GCNPCResponse gcNPCAskAnswer;
                pPlayer->sendPacket(&gcNPCAskAnswer);

                
                GCNPCSayDynamic saypkt;
                saypkt.setObjectID(pNPC->getObjectID());
                
                if (pNPC->getNPCID() == 638) {
                    saypkt.setMessage(g_pStringPool->c_str(STRID_EVENT_NPC_2));
                } else if (pNPC->getNPCID() == 636) {
                    saypkt.setMessage(g_pStringPool->c_str(STRID_EVENT_NPC_3));
                } else if (pNPC->getNPCID() == 635) {
                    saypkt.setMessage(g_pStringPool->c_str(STRID_EVENT_NPC_4));
                } else {
                    saypkt.setMessage(g_pStringPool->c_str(STRID_ALERT_SLAYER));
                }
                pPlayer->sendPacket(&saypkt);
                return;
            }
            
            else if (pNPC->getRace() == NPC_RACE_VAMPIRE && pPC->isOusters()) {
                
                GCNPCResponse gcNPCAskAnswer;
                pPlayer->sendPacket(&gcNPCAskAnswer);

                GCNPCSayDynamic saypkt;
                saypkt.setObjectID(pNPC->getObjectID());
                saypkt.setMessage(g_pStringPool->c_str(STRID_ALERT_OUSTERS));
                pPlayer->sendPacket(&saypkt);
                return;
            }
            
            else if (pNPC->getRace() == NPC_RACE_OUSTERS && pPC->isSlayer()) {
                
                GCNPCResponse gcNPCAskAnswer;
                pPlayer->sendPacket(&gcNPCAskAnswer);

                GCNPCSayDynamic saypkt;
                saypkt.setObjectID(pNPC->getObjectID());
                saypkt.setMessage(g_pStringPool->c_str(STRID_ALERT_SLAYER_2));
                pPlayer->sendPacket(&saypkt);
                return;
            }
            
            else if (pNPC->getRace() == NPC_RACE_OUSTERS && pPC->isVampire()) {
                
                GCNPCResponse gcNPCAskAnswer;
                pPlayer->sendPacket(&gcNPCAskAnswer);

                GCNPCSayDynamic saypkt;
                saypkt.setObjectID(pNPC->getObjectID());
                saypkt.setMessage(g_pStringPool->c_str(STRID_ALERT_VAMPIRE_2));
                pPlayer->sendPacket(&saypkt);
                return;
            }
        }

        
        
        
        // .....
        
        /*		if (pPC->isVampire() && pNPC->getRace() == NPC_RACE_VAMPIRE)
                {
                    if (pPC->getClanType() != pNPC->getClanType())
                    {
                        GCNPCResponse gcNPCAskAnswer;
                        pPlayer->sendPacket(&gcNPCAskAnswer);

                        GCNPCSayDynamic saypkt;
                        saypkt.setObjectID(pNPC->getObjectID());
                        saypkt.setMessage(DiffClanSpeech[rand()%3]);
                        pPlayer->sendPacket(&saypkt);
                        return;
                    }
                }
        */
        
        const TriggerManager& triggerManager = pNPC->getTriggerManager();
        if (triggerManager.hasCondition(Condition::CONDITION_TALKED_BY)) {
            const list<Trigger*>& triggers = triggerManager.getTriggers();
            for (list<Trigger*>::const_iterator itr = triggers.begin(); itr != triggers.end(); itr++) {
                Trigger* pTrigger = *itr;
                Assert(pTrigger != NULL);

                // check all condition after check main condition
                if (pTrigger->hasCondition(Condition::CONDITION_TALKED_BY) &&
                    pTrigger->isAllSatisfied(Trigger::PASSIVE_TRIGGER, pNPC, pPC)) {
                    
                    pTrigger->activate(pNPC, pPC);
                    break;
                }
            }
        }
        
        
        
        else {
            GCNPCResponse gcNPCResponse;
            gcNPCResponse.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
            pPlayer->sendPacket(&gcNPCResponse);
        }
    } catch (Throwable& t) {
        
        // cerr << t.toString() << endl;
    }

#endif

    __END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
