////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionAskByQuestLevel.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionAskByQuestLevel.h"

#include "Creature.h"
#include "GCNPCAsk.h"
#include "GCNPCResponse.h"
#include "GamePlayer.h"
#include "NPC.h"
#include "PlayerCreature.h"
#include "mission/EventQuestAdvance.h"
#include "mission/QuestManager.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ActionAskByQuestLevel::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY

    try {
        // read script id
        m_ScriptID[0] = propertyBuffer.getPropertyInt("Level1");
        m_ScriptID[1] = propertyBuffer.getPropertyInt("Level2");
        m_ScriptID[2] = propertyBuffer.getPropertyInt("Level3");
        m_ScriptID[3] = propertyBuffer.getPropertyInt("Level4");
        m_ScriptID[4] = propertyBuffer.getPropertyInt("Level5");
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionAskByQuestLevel::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
    Assert(pPC != NULL);

    // getQuestLevel() returns -1 when every event quest level has been advanced,
    // and m_ScriptID is a plain ScriptID_t[5] -- so this indexed one element
    // *before* the array for any player who had finished the chain. The -1 return
    // is not exotic: CGLotterySelectHandler tests for it explicitly.
    //
    // Treated as "no script", which is the reply this action already sends when
    // the slot holds 0, so a finished player gets the quit-dialogue response
    // instead of whatever stack bytes preceded the array.
    const int questLevel = pPC->getQuestManager()->getEventQuestAdvanceManager()->getQuestLevel();
    const ScriptID_t sID =
        (questLevel >= 0 && questLevel < EventQuestAdvanceManager::EVENT_QUEST_LEVEL_MAX) ? m_ScriptID[questLevel] : 0;

    if (sID == 0) {
        GCNPCResponse gcNPCResponse;
        gcNPCResponse.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        Player* pPlayer = pCreature2->getPlayer();
        pPlayer->sendPacket(&gcNPCResponse);
    } else {
        GCNPCAsk gcNPCAsk;
        gcNPCAsk.setObjectID(pCreature1->getObjectID());
        gcNPCAsk.setScriptID(sID);
        gcNPCAsk.setNPCID(dynamic_cast<NPC*>(pCreature1)->getNPCID());

        Player* pPlayer = pCreature2->getPlayer();
        pPlayer->sendPacket(&gcNPCAsk);
    }

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionAskByQuestLevel::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionAskByQuestLevel("
        << ")";

    return msg.toString();

    __END_CATCH
}
