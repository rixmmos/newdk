//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGateWayMonster.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectGateWayMonster.h"

#include "Creature.h"
#include "GCModifyInformation.h"
#include "GCRemoveEffect.h"
#include "GCStatusCurrentHP.h"
#include "Monster.h"
#include "Player.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectGateWayMonster::EffectGateWayMonster(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);

    
    m_bBroadcastingEffect = false;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGateWayMonster::affect(Creature* pCreature)

{
    __BEGIN_TRY

    cout << "Effect Relic start" << endl;

    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    HP_t HPRecovery = 1;

    if (pCreature->isSlayer() || pCreature->isVampire() || pCreature->isOusters()) {
        cout << "     " << endl;
        return;
    }

    if (pCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);

        if (pMonster->getMonsterType >= 371 && pMonster->getMonsterType() <= 375) {
            HP_t CurrentHP = pMonster->getHP(ATTR_CURRENT);

            if (Current > 0) {
                HP_t PlusHP = min(5000, CurrentHP + 1);

                pMonster->setHP(PlusHP, ATTR_CURRENT);

                GCStatusCurrentHP pkt;
                pkt.setObjectID(pMonster->getObjectID());
                pkt.setCurrentHP(RemainHP);
                pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &pkt);
            } else {
                cout << "      " << endl;
                return;
            }
        } else {
            cout << "    " << endl;
            return 0;
        }

        setNextTime(m_Tick);

        __END_CATCH
    }

    void EffectGateWayMonster::unaffect()

    {
        __BEGIN_TRY

        Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
        unaffect(pCreature);

        __END_CATCH
    }

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    void EffectGateWayMonster::unaffect(Creature * pCreature)

    {
        __BEGIN_TRY
        __BEGIN_DEBUG

        // cout << "EffectGateWayMonster" << "unaffect BEGIN" << endl;

        Assert(pCreature != NULL);

        
        
        pCreature->removeFlag(Effect::EFFECT_CLASS_GATEWAY_MONSTER);

        Zone* pZone = pCreature->getZone();
        Assert(pZone != NULL);

        GCRemoveEffect gcRemoveEffect;
        gcRemoveEffect.setObjectID(pCreature->getObjectID());
        gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GATEWAY_MONSTER);
        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

        // cout << "EffectGateWayMonster" << "unaffect END" << endl;

        __END_DEBUG
        __END_CATCH
    }

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    string EffectGateWayMonster::toString() const {
        __BEGIN_TRY

        StringStream msg;
        msg << "EffectGateWayMonster("
            << "ObjectID:" << getObjectID() << ")";
        return msg.toString();

        __END_CATCH
    }
