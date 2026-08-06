//////////////////////////////////////////////////////////////////////////////
// Filename    : Resurrect.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Resurrect.h"

#include "EffectComa.h"
#include "EffectKillAftermath.h"
#include "GCRemoveEffect.h"
#include "GCSkillToObjectOK1.h"
#include "GCSkillToObjectOK2.h"
#include "GCSkillToObjectOK3.h"
#include "GCSkillToObjectOK4.h"
#include "GCSkillToObjectOK5.h"
#include "GCStatusCurrentHP.h"
#include "Properties.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Resurrect::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();
        Assert(pPlayer != NULL);
        Assert(pZone != NULL);

        Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
        // Assert(pTargetCreature != NULL);

        
        
        if (pTargetCreature == NULL || !pTargetCreature->isSlayer() ||
            (g_pConfig->hasKey("Hardcore") && g_pConfig->getPropertyInt("Hardcore") != 0)) {
            executeSkillFailException(pSlayer, getSkillType());
            return;
        }

        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
        Assert(pTargetSlayer != NULL);

        
        if (!pTargetSlayer->isFlag(Effect::EFFECT_CLASS_COMA) || !pTargetSlayer->isDead()) {
            executeSkillFailException(pSlayer, getSkillType());
            return;
        }

        GCSkillToObjectOK1 _GCSkillToObjectOK1;
        GCSkillToObjectOK2 _GCSkillToObjectOK2;
        GCSkillToObjectOK3 _GCSkillToObjectOK3;
        GCSkillToObjectOK4 _GCSkillToObjectOK4;
        GCSkillToObjectOK5 _GCSkillToObjectOK5;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();

        ZoneCoord_t myX = pSlayer->getX();
        ZoneCoord_t myY = pSlayer->getY();
        ZoneCoord_t targetX = pTargetCreature->getX();
        ZoneCoord_t targetY = pTargetCreature->getY();

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());
        bool bHitRoll = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
        bool bCanResurrect = false;

        EffectComa* pEffectComa = (EffectComa*)(pTargetCreature->findEffect(Effect::EFFECT_CLASS_COMA));
        Assert(pEffectComa != NULL);

        if (pEffectComa->canResurrect()) {
            bCanResurrect = true;
        }

        if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanResurrect) {
            decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

            SkillInput input(pSlayer, pSkillSlot);
            SkillOutput output;
            computeOutput(input, output);

            
            pTargetCreature->deleteEffect(Effect::EFFECT_CLASS_COMA);
            pTargetCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

            
            GCRemoveEffect gcRemoveEffect;
            gcRemoveEffect.setObjectID(pTargetSlayer->getObjectID());
            gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
            pZone->broadcastPacket(targetX, targetY, &gcRemoveEffect);

            
            pTargetSlayer->getEffectManager()->sendEffectInfo(pTargetSlayer, pZone, pTargetSlayer->getX(),
                                                              pTargetSlayer->getY());

            
            
            if (pTargetSlayer->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH)) {
                Effect* pEffect = pTargetSlayer->findEffect(Effect::EFFECT_CLASS_KILL_AFTERMATH);
                EffectKillAftermath* pEffectKillAftermath = dynamic_cast<EffectKillAftermath*>(pEffect);
                pEffectKillAftermath->setDeadline(5 * 600);
            } else {
                EffectKillAftermath* pEffectKillAftermath = new EffectKillAftermath(pTargetSlayer);
                pEffectKillAftermath->setDeadline(5 * 600);
                pTargetSlayer->addEffect(pEffectKillAftermath);
                pTargetSlayer->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);
                pEffectKillAftermath->create(pTargetSlayer->getName());
            }

            
            HP_t CurrentHP = getPercentValue(pTargetSlayer->getHP(ATTR_MAX), 10);
            pTargetSlayer->setHP(CurrentHP, ATTR_CURRENT);
            pTargetSlayer->setMP(0, ATTR_CURRENT);

            
            GCStatusCurrentHP gcStatusCurrentHP;
            gcStatusCurrentHP.setObjectID(pTargetSlayer->getObjectID());
            gcStatusCurrentHP.setCurrentHP(pTargetSlayer->getHP(ATTR_CURRENT));
            pZone->broadcastPacket(targetX, targetY, &gcStatusCurrentHP);

            
            shareAttrExp(pSlayer, pSkillInfo->getPoint(), 1, 1, 8, _GCSkillToObjectOK1);
            increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1);
            increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToObjectOK1);
            increaseAlignment(pSlayer, pTargetCreature, _GCSkillToObjectOK1);

            _GCSkillToObjectOK1.setSkillType(SkillType);
            _GCSkillToObjectOK1.setCEffectID(CEffectID);
            _GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
            _GCSkillToObjectOK1.setDuration(0);

            _GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
            _GCSkillToObjectOK2.setSkillType(SkillType);
            _GCSkillToObjectOK2.setDuration(0);
            _GCSkillToObjectOK2.addShortData(MODIFY_CURRENT_MP, pTargetSlayer->getMP(ATTR_CURRENT));

            _GCSkillToObjectOK3.setObjectID(pSlayer->getObjectID());
            _GCSkillToObjectOK3.setSkillType(SkillType);
            _GCSkillToObjectOK3.setTargetXY(targetX, targetY);

            _GCSkillToObjectOK4.setSkillType(SkillType);
            _GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);

            _GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
            _GCSkillToObjectOK5.setTargetObjectID(TargetObjectID);
            _GCSkillToObjectOK5.setSkillType(SkillType);
            _GCSkillToObjectOK5.setDuration(0);

            pPlayer->sendPacket(&_GCSkillToObjectOK1);

            Player* pTargetPlayer = pTargetSlayer->getPlayer();
            Assert(pTargetPlayer != NULL);
            pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);

            list<Creature*> cList;
            cList.push_back(pSlayer);
            cList.push_back(pTargetCreature);

            cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList);

            pZone->broadcastPacket(myX, myY, &_GCSkillToObjectOK3, cList);
            pZone->broadcastPacket(targetX, targetY, &_GCSkillToObjectOK4, cList);

            pSkillSlot->setRunTime(output.Delay);
        } else {
            executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

    __END_CATCH
}

Resurrect g_Resurrect;
