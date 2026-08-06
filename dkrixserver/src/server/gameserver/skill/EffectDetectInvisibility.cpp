//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDetectInvisibility.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectDetectInvisibility.h"

#include "Creature.h"
#include "DB.h"
#include "GCRemoveEffect.h"
#include "Slayer.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectDetectInvisibility::EffectDetectInvisibility(Creature* pCreature)

{
    __BEGIN_TRY

    
    Assert(pCreature != NULL);
    Assert(pCreature->isSlayer());

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDetectInvisibility::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDetectInvisibility::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDetectInvisibility::unaffect(Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "EffectDetectInvisibility " << "unaffect BEGIN" << endl;

    Assert(pCreature != NULL);
    Assert(pCreature->isSlayer());

    
    pCreature->removeFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY);

    
    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);
    pZone->updateInvisibleScan(pCreature);

    
    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_DETECT_INVISIBILITY);
    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

    // cout << "EffectDetectInvisibility " << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDetectInvisibility::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectDetectInvisibility " << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    // cout << "EffectDetectInvisibility " << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDetectInvisibility::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

    {__BEGIN_TRY __END_CATCH}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectDetectInvisibility::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectDetectInvisibility("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}
