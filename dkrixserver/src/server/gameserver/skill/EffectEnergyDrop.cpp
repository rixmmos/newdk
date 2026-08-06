///////////////////////////////////////////////////////////////////////////
// Project     : DARKEDEN
// Module      : Skill - Effect
// File Name   : EffectEnergyDrop.cpp

// Date        : 2002.3.28
// Description :


//
// History
//     DATE      WRITER         DESCRIPTION
// =========== =========== =====================================================

//

#include "EffectEnergyDrop.h"

#include "EffectEnergyDropToCreature.h"
#include "GCAddEffect.h"
#include "GCModifyInformation.h"
#include "GCStatusCurrentHP.h"
#include "GamePlayer.h"
#include "Monster.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"

EffectEnergyDrop::EffectEnergyDrop(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY)

{
    __BEGIN_TRY

    m_pZone = pZone;
    m_X = zoneX;
    m_Y = zoneY;
    m_UserObjectID = 0;
    //	m_CasterName ="";
    //	m_PartyID = 0;

    __END_CATCH
}


bool EffectEnergyDrop::affectCreature(Creature* pTargetCreature, bool bAffectByMove)

{
    __BEGIN_TRY

    // cout << "EffectEnergyDrop " << "affectCreature Begin " << endl;

    Assert(pTargetCreature != NULL);

    
    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_ENERGY_DROP_TO_CREATURE)) {
        // cout << "EffectEnergyDrop " << "affectCreature End(Already Effected) " << endl;
        return false;
    }

    
    // 2003.1.10 by bezz, Sequoia
    if (!checkZoneLevelToHitTarget(pTargetCreature)) {
        return false;
    }

    Zone* pZone = pTargetCreature->getZone();

    
    int DropDamage = computeMagicDamage(pTargetCreature, m_Damage, SKILL_ENERGY_DROP);

    // cout << "EffectEnergyDrop(Damage:" << DropDamage << ") Affected" << endl;
    if (DropDamage > 0) {
        

        
        
        
        
        
        
        
        
        
        //   m_Tick
        //   m_Count
        

        EffectEnergyDropToCreature* pEffectEnergyDropToCreature = new EffectEnergyDropToCreature(pTargetCreature);

        
        //		pEffectEnergyDropToCreature->setCasterName(m_CasterName);
        //		pEffectEnergyDropToCreature->setPartyID(m_PartyID);
        pEffectEnergyDropToCreature->setUserObjectID(m_UserObjectID);

        pEffectEnergyDropToCreature->setLevel(m_Level);
        pEffectEnergyDropToCreature->setPoint(DropDamage / 3);
        pEffectEnergyDropToCreature->setDeadline(16); 
        pEffectEnergyDropToCreature->setTick(5);      
        pEffectEnergyDropToCreature->affect(pTargetCreature);
        pTargetCreature->addEffect(pEffectEnergyDropToCreature);
        pTargetCreature->setFlag(Effect::EFFECT_CLASS_ENERGY_DROP_TO_CREATURE);

        
        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pTargetCreature->getObjectID());
        gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ENERGY_DROP_TO_CREATURE);
        gcAddEffect.setDuration(m_Duration);
        pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
    }

    // cout << "EffectEnergyDrop " << "affectCreature End " << endl;

    return true;

    __END_CATCH
}


void EffectEnergyDrop::affect()

{
    __BEGIN_TRY

    __END_CATCH
}

void EffectEnergyDrop::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectEnergyDrop::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectEnergyDrop::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectEnergyDrop::unaffect()

{
    __BEGIN_TRY

    Tile& tile = m_pZone->getTile(m_X, m_Y);
    tile.deleteEffect(m_ObjectID);

    __END_CATCH
}

void EffectEnergyDrop::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObjbect)

    {__BEGIN_TRY __END_CATCH}

string EffectEnergyDrop::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectEnergyDrop("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

EffectEnergyDropLoader* g_pEffectEnergyDropLoader = NULL;
