////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionRestore.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionRestore.h"

#include "Creature.h"
#include "GCNPCResponse.h"
#include "NPC.h"
#include "Restore.h"
#include "SkillHandlerManager.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ActionRestore::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionRestore::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    GCNPCResponse okpkt;
    pCreature2->getPlayer()->sendPacket(&okpkt);

    
    NPC* pNPC = dynamic_cast<NPC*>(pCreature1);

    
    SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_RESTORE);
    Assert(pSkillHandler != NULL);

    
    Restore* pRestore = dynamic_cast<Restore*>(pSkillHandler);

    
    pRestore->execute(pNPC, pCreature2);

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionRestore::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionRestore("
        << ")";
    return msg.toString();

    __END_CATCH
}
