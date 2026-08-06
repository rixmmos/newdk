//----------------------------------------------------------------------
//
// Filename    : EffectContinualGroundAttack.cpp
// Written by  : elca
//
//----------------------------------------------------------------------

// include files
#include "EffectContinualGroundAttack.h"

#include "Assert.h"
#include "EffectGroundAttack.h"
#include "EffectMeteorStrike.h"
#include "GCAddEffectToTile.h"
#include "MonsterManager.h"
#include "PCManager.h"
#include "SkillUtil.h"
#include "Zone.h"
#include "ZoneUtil.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
EffectContinualGroundAttack::EffectContinualGroundAttack(Zone* pZone, EffectClass attackEffect, Turn_t delay)

{
    __BEGIN_TRY

    Assert(pZone != NULL);

    m_pZone = pZone;

    m_AttackEffect = attackEffect;

    m_Delay = delay;

    setNextTime(10); 
    setDeadline(delay);

    m_MinNumber = 1;
    m_MaxNumber = 1;

    
    m_bBroadcastingEffect = false;

    __END_CATCH
}


//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
EffectContinualGroundAttack::~EffectContinualGroundAttack()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}

void EffectContinualGroundAttack::affect()

{
    __BEGIN_TRY

    Assert(m_pZone != NULL);

    int creatureNum = m_pZone->getPCManager()->getSize() + m_pZone->getMonsterManager()->getSize();

    
    
    if (creatureNum > 0) {
        
        int range = min(100, (m_MaxNumber - m_MinNumber)); 
        int number = (range > 0 ? m_MinNumber + rand() % range : m_MinNumber);

        // cout << "EffectContinualGroundAttack: " << (int)m_pZone->getZoneID() << ", num= " << number << endl;

        VSRect rect(0, 0, m_pZone->getWidth() - 1, m_pZone->getHeight() - 1);

        
        for (int i = 0; i < number; i++) {
            const BPOINT& pt = m_pZone->getRandomEmptyTilePosition();

            if (!rect.ptInRect(pt.x, pt.y))
                continue;

            
            Tile& tile = m_pZone->getTile(pt.x, pt.y);

            int X = pt.x;
            int Y = pt.y;

            if (!tile.canAddEffect())
                continue;

            int DamagePercent = 100;

            
            Effect* pAttackEffect = NULL;

            switch (m_AttackEffect) {
            case EFFECT_CLASS_GROUND_ATTACK: {
                
                Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_GROUND_ATTACK);
                if (pOldEffect != NULL) {
                    EffectGroundAttack* pGAEffect = dynamic_cast<EffectGroundAttack*>(pOldEffect);
                    pGAEffect->setDamagePercent(max(DamagePercent, pGAEffect->getDamagePercent()));
                    continue;
                }

                EffectGroundAttack* pEffect = new EffectGroundAttack(m_pZone, X, Y);
                pEffect->setDeadline(22); 
                pEffect->setDamagePercent(DamagePercent);

                pAttackEffect = pEffect;
            } break;

            case EFFECT_CLASS_METEOR_STRIKE: {
                
                Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_METEOR_STRIKE);
                if (pOldEffect != NULL) {
                    ObjectID_t effectID = pOldEffect->getObjectID();
                    m_pZone->deleteEffect(effectID);
                }

                EffectMeteorStrike* pEffect = new EffectMeteorStrike(m_pZone, X, Y);
                pEffect->setDeadline(10); 

                // 400 ~ 600 100%
                // 200 ~ 300 50%
                // 100 ~ 150 25%
                pEffect->setDamage(400 + rand() % 200);

                pAttackEffect = pEffect;
            } break;

            default:
                break;
            }


            if (pAttackEffect != NULL) {
                
                ObjectRegistry& objectregister = m_pZone->getObjectRegistry();
                objectregister.registerObject(pAttackEffect);

                
                m_pZone->addEffect(pAttackEffect);
                tile.addEffect(pAttackEffect);

                
                GCAddEffectToTile gcAddEffectToTile;
                gcAddEffectToTile.setEffectID(pAttackEffect->getEffectClass());
                gcAddEffectToTile.setObjectID(pAttackEffect->getObjectID());
                gcAddEffectToTile.setXY(X, Y);
                gcAddEffectToTile.setDuration(20); 

                m_pZone->broadcastPacket(X, Y, &gcAddEffectToTile);
            }
        }
    }

    setNextTime(m_Delay);

    __END_CATCH
}

//--------------------------------------------------------------------
// unaffect()
//--------------------------------------------------------------------
void EffectContinualGroundAttack::unaffect()

    {__BEGIN_TRY

         __END_CATCH}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string EffectContinualGroundAttack::toString() const

{
    StringStream msg;

    if (m_pZone) {
        msg << "EffectContinualGroundAttack("
            << "ZoneID:" << (int)m_pZone->getZoneID();
    }

    msg << ",Delay:" << (int)m_Delay << ",Deadline:" << (int)m_Deadline.tv_sec << "." << (int)m_Deadline.tv_usec << ")";

    return msg.toString();
}
