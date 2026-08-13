//////////////////////////////////////////////////////////////////////////////
// Filename    : CGNPCAskAnswer.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGNPCAskAnswer.h"

#ifdef __GAME_SERVER__
#include <fstream>

#include "CheckedCast.h"
#include "GCNPCResponse.h"
#include "GamePlayer.h"
#include "NPC.h"
#include "quest/Action.h"
#include "quest/Condition.h"
#include "quest/Trigger.h"
#include "quest/TriggerManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
void CGNPCAskAnswerHandler::execute(CGNPCAskAnswer* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    Creature* pCreature = NULL;

    if (pPC == NULL)
        return;

    Zone* pZone = pPC->getZone();

    if (pZone == NULL)
        return;

     
    // cout << pPacket->toString().c_str() << endl;

    
    pCreature = pZone->getCreature(pPacket->getObjectID());

    if (pCreature == NULL || !pCreature->isNPC()) {
        GCNPCResponse okpkt;
        pPlayer->sendPacket(&okpkt);

        // cout << okpkt.toString().c_str() << endl;

        return;
    }

    NPC* pNPC = checkedCast<NPC*>(pCreature);

    COND_ANSWERED_BY cond;
    cond.ScriptID = pPacket->getScriptID();
    cond.AnswerID = pPacket->getAnswerID();

    // get NPC's trigger manager
    const TriggerManager& triggerManager = pNPC->getTriggerManager();

    // check main condition
    if (triggerManager.hasCondition(Condition::CONDITION_ANSWERED_BY)) {
        const list<Trigger*>& triggers = triggerManager.getTriggers();
        for (list<Trigger*>::const_iterator itr = triggers.begin(); itr != triggers.end(); itr++) {
            Trigger* pTrigger = *itr;
            if (pTrigger == NULL) {
                // cout << "*** shit trigger is NULL ***";
                return;
            }

            // check all condition after check main condition
            if (pTrigger->hasCondition(Condition::CONDITION_ANSWERED_BY) &&
                pTrigger->isAllSatisfied(Trigger::PASSIVE_TRIGGER, pNPC, pPC, (void*)&cond)) {
                
                pTrigger->activate(pNPC, pPC);
                break;
            }
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
