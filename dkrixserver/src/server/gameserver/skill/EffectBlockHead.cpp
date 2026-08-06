//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBlockHead.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectBlockHead.h"

#include "GCRemoveEffect.h"
#include "Ousters.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBlockHead::EffectBlockHead(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);
    // Assert(pCreature->isOusters());

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBlockHead::unaffect(Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "EffectBlockHead" << "unaffect BEGIN" << endl;

    Assert(pCreature != NULL);
    //	Assert(pCreature->isOusters());

    
    pCreature->removeFlag(Effect::EFFECT_CLASS_BLOCK_HEAD);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    //	Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
    //	Assert( pTargetOusters != NULL );

    
    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_BLOCK_HEAD);
    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

    // cout << "EffectBlockHead" << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBlockHead::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBlockHead::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectBlockHead("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}
