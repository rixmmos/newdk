//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGnomesWhisper.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectGnomesWhisper.h"

#include "GCRemoveEffect.h"
#include "Ousters.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectGnomesWhisper::EffectGnomesWhisper(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);
    //	Assert(pCreature->isOusters());

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGnomesWhisper::unaffect(Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "EffectGnomesWhisper" << "unaffect BEGIN" << endl;

    Assert(pCreature != NULL);
    //	Assert(pCreature->isOusters());

    
    pCreature->removeFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    pZone->updateInvisibleScan(pCreature);

    //	Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
    //	Assert( pTargetOusters != NULL );

    
    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GNOMES_WHISPER);
    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

    // cout << "EffectGnomesWhisper" << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGnomesWhisper::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectGnomesWhisper::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectGnomesWhisper("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}
