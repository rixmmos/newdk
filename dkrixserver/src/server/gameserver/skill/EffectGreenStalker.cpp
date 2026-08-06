//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGreenStalker.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectGreenStalker.h"

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
EffectGreenStalker::EffectGreenStalker(Creature* pCreature)

{
    __BEGIN_TRY

    m_UserObjectID = 0;
    m_Level = 0;
    m_bVampire = false;
    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenStalker::affect()

{
    __BEGIN_TRY

    // cout << "EffectGreenStalker " << "affect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    affect(pCreature);

    // cout << "EffectGreenStalker " << "affect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenStalker::affect(Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "EffectGreenStalker " << "affect BEGIN" << endl;

    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    
    
    // by bezz. 2003.1.4
    Creature* pCastCreature = pZone->getCreature(m_UserObjectID);

    if (pCastCreature == NULL) {
        setNextTime(m_Tick);

        return;
    }

    int PoisonDamage = computeMagicDamage(pCreature, m_Damage, SKILL_GREEN_STALKER, m_bVampire, pCastCreature);

    
    if (canAttack(pCastCreature, pCreature) && !pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
        
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            Assert(pSlayer != NULL);

            GCModifyInformation gcMI;
            ::setDamage(pSlayer, PoisonDamage, pCastCreature, SKILL_GREEN_STALKER, &gcMI);

            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);
            pPlayer->sendPacket(&gcMI);
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            Assert(pVampire != NULL);

            GCModifyInformation gcMI;
            ::setDamage(pVampire, PoisonDamage, pCastCreature, SKILL_GREEN_STALKER, &gcMI);

            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);
            pPlayer->sendPacket(&gcMI);
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            Assert(pOusters != NULL);

            GCModifyInformation gcMI;
            ::setDamage(pOusters, PoisonDamage, pCastCreature, SKILL_GREEN_STALKER, &gcMI);

            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);
            pPlayer->sendPacket(&gcMI);
        } else if (pCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pCreature);
            Assert(pMonster != NULL);

            ::setDamage(pMonster, PoisonDamage, pCastCreature, SKILL_GREEN_STALKER);
        }

        
        // by sigi. 2002.9.9
        /*		if (pCreature->isDead())
                {
                    Creature* pAttacker = pZone->getCreature( m_UserObjectID );

                    if (pAttacker!=NULL)
                    {
                        affectKillCount(pAttacker, pCreature);
                    }
                }*/
    }

    setNextTime(m_Tick);

    // cout << "EffectGreenStalker " << "affect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenStalker::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenStalker::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    // cout << "EffectGreenStalker " << "unaffect BEGIN" << endl;

    Assert(pCreature != NULL);

    
    pCreature->removeFlag(Effect::EFFECT_CLASS_GREEN_STALKER);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    
    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GREEN_STALKER);
    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

    // cout << "EffectGreenStalker " << "unaffect END" << endl;

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenStalker::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectGreenStalker " << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    // cout << "EffectGreenStalker " << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenStalker::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

    {__BEGIN_TRY __END_CATCH}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectGreenStalker::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectGreenStalker("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}
