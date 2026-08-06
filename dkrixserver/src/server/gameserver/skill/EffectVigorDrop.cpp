///////////////////////////////////////////////////////////////////////////
// Project     : DARKEDEN
// Module      : Skill - Effect
// File Name   : EffectVigorDrop.cpp

// Date        : 2002.3.28
// Description :


//
// History
//     DATE      WRITER         DESCRIPTION
// =========== =========== =====================================================

//

#include "EffectVigorDrop.h"

#include "EffectVigorDropToCreature.h"
#include "GCAddEffect.h"
#include "GCModifyInformation.h"
#include "GCStatusCurrentHP.h"
#include "GamePlayer.h"
#include "Monster.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"

EffectVigorDrop::EffectVigorDrop(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY)

{
    __BEGIN_TRY

    m_pZone = pZone;
    m_X = zoneX;
    m_Y = zoneY;
    //	m_CasterName ="";
    //	m_PartyID = 0;
    m_UserObjectID = 0;

    __END_CATCH
}


bool EffectVigorDrop::affectCreature(Creature* pTargetCreature, bool bAffectByMove)

{
    __BEGIN_TRY

    // cout << "EffectVigorDrop " << "affectCreature Begin " << endl;

    Assert(pTargetCreature != NULL);

    
    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_VIGOR_DROP_TO_CREATURE)) {
        // cout << "EffectVigorDrop " << "affectCreature End(Already Effected) " << endl;
        return false;
    }

    
    // 2003.1.10 by bezz, Sequoia
    if (!checkZoneLevelToHitTarget(pTargetCreature)) {
        return false;
    }

    Zone* pZone = pTargetCreature->getZone();

    
    int DropDamage = computeMagicDamage(pTargetCreature, m_Damage, SKILL_VIGOR_DROP);

    if (DropDamage > 0) {
        // cout << "EffectVigorDrop(Damage:" << DropDamage << ") Affected" << endl;
        

        
        
        
        
        
        
        
        
        
        //   m_Tick
        //   m_Count
        

        EffectVigorDropToCreature* pEffectVigorDropToCreature = new EffectVigorDropToCreature(pTargetCreature);

        
        // pEffectVigorDropToCreature->setCasterName(m_CasterName);
        // pEffectVigorDropToCreature->setPartyID(m_PartyID);
        pEffectVigorDropToCreature->setUserObjectID(m_UserObjectID);

        pEffectVigorDropToCreature->setLevel(m_Level);
        pEffectVigorDropToCreature->setPoint(DropDamage / 3);
        pEffectVigorDropToCreature->setDeadline(16); 
        pEffectVigorDropToCreature->setTick(5);      
        pEffectVigorDropToCreature->affect(pTargetCreature);
        pTargetCreature->addEffect(pEffectVigorDropToCreature);
        pTargetCreature->setFlag(Effect::EFFECT_CLASS_VIGOR_DROP_TO_CREATURE);

        
        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pTargetCreature->getObjectID());
        gcAddEffect.setEffectID(Effect::EFFECT_CLASS_VIGOR_DROP_TO_CREATURE);
        gcAddEffect.setDuration(m_Duration);
        pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
    }

    // cout << "EffectVigorDrop " << "affectCreature End " << endl;

    return true;

    __END_CATCH
}


void EffectVigorDrop::affect()

{
    __BEGIN_TRY

    __END_CATCH
}

void EffectVigorDrop::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectVigorDrop::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectVigorDrop::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectVigorDrop::unaffect()

{
    __BEGIN_TRY

    Tile& tile = m_pZone->getTile(m_X, m_Y);
    tile.deleteEffect(m_ObjectID);

    __END_CATCH
}

void EffectVigorDrop::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObjbect)

    {__BEGIN_TRY __END_CATCH}

string EffectVigorDrop::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectVigorDrop("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

EffectVigorDropLoader* g_pEffectVigorDropLoader = NULL;
