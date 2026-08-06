//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectVigorDropToCreature.cpp

// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectVigorDropToCreature.h"

#include "DB.h"
#include "GCModifyInformation.h"
#include "GCRemoveEffect.h"
#include "GCStatusCurrentHP.h"
#include "Monster.h"
#include "Ousters.h"
#include "Player.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectVigorDropToCreature::EffectVigorDropToCreature(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVigorDropToCreature::affect()

{
    __BEGIN_TRY

    // cout << "EffectVigorDropToCreature " << "begin begin" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    affect(pCreature);

    // cout << "EffectVigorDropToCreature " << "begin end" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVigorDropToCreature::affect(Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "EffectVigorDropToCreature " << "begin" << endl;

    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    
    
    // by bezz. 2003.1.4
    Creature* pCastCreature = pZone->getCreature(m_UserObjectID);

    
    

    Damage_t DropDamage = m_Point;

    if (!(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE)
        
        && canAttack(pCastCreature, pCreature) && !pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

            GCModifyInformation gcMI;
            setDamage(pSlayer, DropDamage, pCastCreature, SKILL_VIGOR_DROP, &gcMI);

            pSlayer->getPlayer()->sendPacket(&gcMI);
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

            GCModifyInformation gcMI;
            setDamage(pVampire, DropDamage, pCastCreature, SKILL_VIGOR_DROP, &gcMI);

            pVampire->getPlayer()->sendPacket(&gcMI);
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

            GCModifyInformation gcMI;
            setDamage(pOusters, DropDamage, pCastCreature, SKILL_VIGOR_DROP, &gcMI);

            pOusters->getPlayer()->sendPacket(&gcMI);
        } else if (pCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pCreature);

            setDamage(pMonster, DropDamage, pCastCreature, SKILL_VIGOR_DROP);
        }

        
        // by sigi. 2002.9.9
        
        // by bezz. 2002.12.31
        /*		if (pCreature->isDead())
                {
                    Creature* pAttacker = pZone->getCreature( m_CasterName );

                    if (pAttacker!=NULL)
                    {
                        affectKillCount(pAttacker, pCreature);
                    }
                }*/
    }

    setNextTime(m_Tick);

    // cout << "EffectVigorDropToCreature " << "end" << endl;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVigorDropToCreature::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVigorDropToCreature::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    pCreature->removeFlag(Effect::EFFECT_CLASS_VIGOR_DROP_TO_CREATURE);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    
    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_VIGOR_DROP_TO_CREATURE);
    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVigorDropToCreature::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVigorDropToCreature::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

    {__BEGIN_TRY __END_CATCH}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectVigorDropToCreature::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectVigorDropToCreature("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}
