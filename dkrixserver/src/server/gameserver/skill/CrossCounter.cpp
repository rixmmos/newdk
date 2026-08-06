//////////////////////////////////////////////////////////////////////////////
// Filename    : CrossCounter.cpp
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CrossCounter.h"

#include "EffectCrossCounter.h"
#include "GCAddEffect.h"
#include "GCCrossCounterOK1.h"
#include "GCCrossCounterOK2.h"
#include "GCCrossCounterOK3.h"
#include "GCRemoveEffect.h"
#include "GCSkillToSelfOK1.h"
#include "GCSkillToSelfOK2.h"
#include "SkillHandlerManager.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CrossCounter::execute(Slayer* pSlayer, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayerself)" << endl;

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();

        Assert(pPlayer != NULL);
        Assert(pZone != NULL);

        
        Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
        if (pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_SWORD) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerself)" << endl;
            return;
        }

        bool bIncreaseDomainExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);

        GCSkillToSelfOK1 _GCSkillToSelfOK1;
        GCSkillToSelfOK2 _GCSkillToSelfOK2;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
        bool bHitRoll = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
        bool bEffected = pSlayer->isFlag(Effect::EFFECT_CLASS_CROSS_COUNTER);

        if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected) {
            decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

            SkillInput input(pSlayer, pSkillSlot);
            SkillOutput output;
            computeOutput(input, output);

            
            EffectCrossCounter* pEffect = new EffectCrossCounter(pSlayer);
            pEffect->setDeadline(output.Duration);
            pSlayer->addEffect(pEffect);
            pSlayer->setFlag(Effect::EFFECT_CLASS_CROSS_COUNTER);

            
            SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
            Exp_t ExpUp = 10 * (Grade + 1);
            if (bIncreaseDomainExp) {
                shareAttrExp(pSlayer, ExpUp, 8, 1, 1, _GCSkillToSelfOK1);
                increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToSelfOK1);
                increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToSelfOK1);
            }

            
            _GCSkillToSelfOK1.setSkillType(SkillType);
            _GCSkillToSelfOK1.setCEffectID(CEffectID);
            _GCSkillToSelfOK1.setDuration(output.Duration);

            _GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
            _GCSkillToSelfOK2.setSkillType(SkillType);
            _GCSkillToSelfOK2.setDuration(output.Duration);

            pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCSkillToSelfOK2, pSlayer);

            pPlayer->sendPacket(&_GCSkillToSelfOK1);

            
            GCAddEffect gcAddEffect;
            gcAddEffect.setObjectID(pSlayer->getObjectID());
            gcAddEffect.setEffectID(Effect::EFFECT_CLASS_CROSS_COUNTER);
            gcAddEffect.setDuration(output.Duration);
            pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddEffect);

            pSkillSlot->setRunTime(output.Delay);
        } else {
            executeSkillFailNormal(pSlayer, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerself)" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
bool CheckCrossCounter(Creature* pAttacker, Creature* pTargetCreature, Damage_t damage, Range_t range) {
    __BEGIN_TRY

    // cout << "CheckCrossCounter Begin" << endl;

    Assert(pAttacker != NULL);
    Assert(pTargetCreature != NULL);

    
    if (pTargetCreature->isSlayer() == false || range != 1) {
        // cout << "CheckCrossCounter End" << endl;
        return false;
    }

    bool bSuccess = false;

    
    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_CROSS_COUNTER) &&
        !pTargetCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
        SkillSlot* pCrossCounterSkillSlot = pTargetSlayer->hasSkill(SKILL_CROSS_COUNTER);
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SKILL_CROSS_COUNTER);
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();
        Zone* pZone = pAttacker->getZone();

        Assert(pTargetSlayer != NULL);
        Assert(pCrossCounterSkillSlot != NULL);
        Assert(pSkillInfo != NULL);
        Assert(pZone != NULL);

        Item* pWeapon = pTargetSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);

        if (!verifyDistance(pAttacker, pTargetCreature, 1) ||
            
            pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_SWORD) {
            return false;
        }

        
        if (HitRoll::isSuccess(pTargetSlayer, pAttacker) && canHit(pTargetSlayer, pAttacker, SKILL_CROSS_COUNTER)) {
            
            
            ObjectID_t targetID = pTargetSlayer->getObjectID();
            ObjectID_t attackerID = pAttacker->getObjectID();

            GCCrossCounterOK1 _GCCrossCounterOK1;
            GCCrossCounterOK2 _GCCrossCounterOK2;
            GCCrossCounterOK3 _GCCrossCounterOK3;

            Level_t SkillLevel = pCrossCounterSkillSlot->getExpLevel();
            SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(SkillLevel);

            ////////////////////////////////////////////////////////////
            
            // SKILL_GRADE_APPRENTICE
            // SKILL_GRADE_ADEPT
            // SKILL_GRADE_EXPERT
            // SKILL_GRADE_MASTER
            // SKILL_GRADE_GRAND_MASTER
            ////////////////////////////////////////////////////////////
            SkillType_t CounterSkillType = SKILL_ATTACK_MELEE;

            bool bCriticalHit;

            
            Damage_t Damage = computeDamage(pTargetSlayer, pAttacker, SkillLevel / 5, bCriticalHit);

            if (Grade == SKILL_GRADE_APPRENTICE) {
                Damage += pTargetSlayer->getDamage(ATTR_CURRENT);
            } else if (Grade == SKILL_GRADE_ADEPT) {
                SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SKILL_DOUBLE_IMPACT);
                Damage_t SkillMinPoint = pSkillInfo->getMinDamage();
                Damage_t SkillMaxPoint = pSkillInfo->getMaxDamage();
                Damage += (int)(SkillMinPoint + (SkillMaxPoint - SkillMinPoint) * (double)(SkillLevel * 0.01));
                CounterSkillType = SKILL_DOUBLE_IMPACT;
            } else if (Grade == SKILL_GRADE_EXPERT) {
                SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SKILL_TRIPLE_SLASHER);
                Damage_t SkillMinPoint = pSkillInfo->getMinDamage();
                Damage_t SkillMaxPoint = pSkillInfo->getMaxDamage();
                Damage += (int)(SkillMinPoint + (SkillMaxPoint - SkillMinPoint) * (double)(SkillLevel * 0.01));
                CounterSkillType = SKILL_TRIPLE_SLASHER;
            } else if (Grade == SKILL_GRADE_MASTER) {
                SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SKILL_THUNDER_SPARK);
                Damage_t SkillMinPoint = pSkillInfo->getMinDamage();
                Damage_t SkillMaxPoint = pSkillInfo->getMaxDamage();
                Damage += (int)(SkillMinPoint + (SkillMaxPoint - SkillMinPoint) * (double)(SkillLevel * 0.01));
                CounterSkillType = SKILL_THUNDER_SPARK;
            } else if (Grade == SKILL_GRADE_GRAND_MASTER) {
                SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SKILL_RAINBOW_SLASHER);
                Damage_t SkillMinPoint = pSkillInfo->getMinDamage();
                Damage_t SkillMaxPoint = pSkillInfo->getMaxDamage();
                Damage += (int)(SkillMinPoint + (SkillMaxPoint - SkillMinPoint) * (double)(SkillLevel * 0.01));
                CounterSkillType = SKILL_RAINBOW_SLASHER;
            }

            _GCCrossCounterOK1.setSkillType(CounterSkillType);
            _GCCrossCounterOK2.setSkillType(CounterSkillType);
            _GCCrossCounterOK3.setSkillType(CounterSkillType);

            if (Random(1, 100) < 60 && !pAttacker->isSlayer()) {
                shareAttrExp(pTargetSlayer, Damage, 8, 1, 1, _GCCrossCounterOK1);
                increaseDomainExp(pTargetSlayer, DomainType, pSkillInfo->getPoint(), _GCCrossCounterOK1);
                increaseSkillExp(pTargetSlayer, DomainType, pCrossCounterSkillSlot, pSkillInfo, _GCCrossCounterOK1);
            }

            setDamage(pAttacker, Damage, pTargetCreature, SKILL_CROSS_COUNTER, &_GCCrossCounterOK2,
                      &_GCCrossCounterOK1);
            // SkillHandler::computeAlignmentChange(pAttacker, Damage, pTargetCreature, &_GCCrossCounterOK2,
            // &_GCCrossCounterOK1);

            _GCCrossCounterOK1.setObjectID(attackerID);
            _GCCrossCounterOK2.setObjectID(targetID);
            _GCCrossCounterOK3.setObjectID(targetID);
            _GCCrossCounterOK3.setTargetObjectID(attackerID);

            pTargetSlayer->getPlayer()->sendPacket(&_GCCrossCounterOK1);

            if (pAttacker->isPC()) {
                pAttacker->getPlayer()->sendPacket(&_GCCrossCounterOK2);
            } else if (pAttacker->isMonster()) {
                Monster* pMonster = dynamic_cast<Monster*>(pAttacker);

                
                
                // pMonster->addEnemy(pTargetSlayer);

                
                if (!pMonster->isMaster()
#ifdef __UNDERWORLD__
                    && !pMonster->isUnderworld() && pMonster->getMonsterType() != 599
#endif
                ) {
                    Timeval NextTurn = pMonster->getNextTurn();
                    Timeval DelayTurn;
                    DelayTurn.tv_sec = 0;
                    DelayTurn.tv_usec = 200000;
                    Timeval NewTurn = NextTurn + DelayTurn;
                    pMonster->setNextTurn(NewTurn);
                }
            }

            list<Creature*> cList;
            cList.push_back(pTargetSlayer);
            cList.push_back(pAttacker);
            pZone->broadcastPacket(pTargetSlayer->getX(), pTargetSlayer->getY(), &_GCCrossCounterOK3, cList);

            bSuccess = true;
        } else {
            
        }
    }

    // cout << "CheckCrossCounter End" << endl;

    return bSuccess;

    __END_CATCH
}

CrossCounter g_CrossCounter;
