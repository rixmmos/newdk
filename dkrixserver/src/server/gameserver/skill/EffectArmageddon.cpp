//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectArmageddon.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectArmageddon.h"

#include "GCRemoveEffect.h"
#include "GCStatusCurrentHP.h"
#include "Monster.h"
#include "Player.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectArmageddon::EffectArmageddon(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectArmageddon::affect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);

    Assert(pCreature != NULL);

    affect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectArmageddon::affect(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    
    
    // by bezz. 2003.1.4
    // Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

    // ZoneCoord_t X = pCreature->getX();
    // ZoneCoord_t Y = pCreature->getY();

    // if ( pCreature->isFlag( Effect::EFFECT_CLASS_NO_DAMAGE ) )
    //{
    //	return;
    // }

    // cout << "EffectArmageddon affect damage :" << m_Damage << endl;

    
    /*if ( pCreature->isSlayer() )
    {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        GCModifyInformation gcMI;
        ::setDamage( pSlayer, m_Damage, pCastCreature, SKILL_ARMAGEDDON, &gcMI );

        pSlayer->getPlayer()->sendPacket( &gcMI );
    }
    else if ( pCreature->isVampire() )
    {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        GCModifyInformation gcMI;
        ::setDamage( pVampire, m_Damage, pCastCreature, SKILL_ARMAGEDDON, &gcMI );

        pVampire->getPlayer()->sendPacket( &gcMI );
    }
    else if ( pCreature->isMonster() )
    {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);
        ::setDamage( pMonster, m_Damage, pCastCreature, SKILL_ARMAGEDDON );
    }*/

    setNextTime(m_Delay);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectArmageddon::unaffect(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    // cout << "EffectArmageddon unaffect.." << endl;

    
    pCreature->removeFlag(Effect::EFFECT_CLASS_ARMAGEDDON);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    
    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ARMAGEDDON);
    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectArmageddon::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectArmageddon::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectArmageddon("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectArmageddon::decreaseHP(Damage_t damage) throw() {
    // cout << "EffectArmageddon Damaged : " << (int)damage << endl;

    HP_t RemainHP = max(0, m_HP - damage);

    setHP(RemainHP);
    
    if (RemainHP == 0)
        setDeadline(0);
}
