//////////////////////////////////////////////////////////////////////////////
// Filename    : DragonTornado.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "DragonTornado.h"

#include "EffectDragonTornado.h"
#include "GCAddEffectToTile.h"
#include "SimpleMeleeSkill.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void DragonTornado::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    SkillInput input(pSlayer, pSkillSlot);
    SkillOutput output;
    computeOutput(input, output);

    SIMPLE_SKILL_INPUT param;
    param.SkillType = getSkillType();
    param.SkillDamage = output.Damage;
    param.Delay = output.Delay;
    param.ItemClass = Item::ITEM_CLASS_BLADE;
    param.STRMultiplier = 8;
    param.DEXMultiplier = 1;
    param.INTMultiplier = 1;
    param.bMagicHitRoll = false;
    param.bMagicDamage = false;
    param.bAdd = true;

    SIMPLE_SKILL_OUTPUT result;

    // cout << "DragonTornado damage = " << output.Damage << endl;

    g_SimpleMeleeSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);

    if (result.bSuccess) {
        // cout << "DragonTornado Success" << endl;
        Zone* pZone = pSlayer->getZone();
        Assert(pZone != NULL);

        Creature* pCreature = pZone->getCreature(TargetObjectID);

        if (pCreature != NULL) {
            
            
            if (pCreature->isMonster()) {
                Monster* pMonster = dynamic_cast<Monster*>(pCreature);

                
                if (!pMonster->isMaster()) {
                    Timeval delay;
                    delay.tv_sec = 1;
                    delay.tv_usec = 0; // 500000;
                    pMonster->addAccuDelay(delay);
                }
            }

            Tile& tile = pZone->getTile(pCreature->getX(), pCreature->getY());

            if (tile.canAddEffect()) {
                EffectDragonTornado* pEffect = new EffectDragonTornado(pZone, pCreature->getX(), pCreature->getY());
                pEffect->setUserOID(pSlayer->getObjectID());
                pEffect->setDamage(output.Range);
                pEffect->setChildDamage(output.Tick);
                pEffect->setDeadline(output.Duration);
                pEffect->setNextTime(10);
                pZone->registerObject(pEffect);
                tile.addEffect(pEffect);
                pZone->addEffect(pEffect);

                GCAddEffectToTile gcAE;
                gcAE.setEffectID(pEffect->getSendEffectClass());
                gcAE.setObjectID(pEffect->getObjectID());
                gcAE.setDuration(output.Duration);
                gcAE.setXY(pCreature->getX(), pCreature->getY());
                pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAE);
            }
        }
    }

    

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

DragonTornado g_DragonTornado;
