//////////////////////////////////////////////////////////////////////////////
// Filename    : ShadowWalk.cpp
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ShadowWalk.h"

#include "GCSkillToObjectOK1.h"
#include "GCSkillToObjectOK2.h"
#include "GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void ShadowWalk::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();
        Assert(pPlayer != NULL);
        Assert(pZone != NULL);

        Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
        // Assert(pTargetCreature != NULL);

        
        
        if (pTargetCreature == NULL || pTargetCreature->isNPC()) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }

        
        Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
        if (pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_BLADE) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }

        bool bIncreaseExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);

        GCSkillToObjectOK1 _GCSkillToObjectOK1;
        GCSkillToObjectOK2 _GCSkillToObjectOK2;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();
        SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, computeSkillRange(pSkillSlot, pSkillInfo));
        bool bHitRoll = HitRoll::isSuccess(pSlayer, pTargetCreature, SkillLevel / 2);
        bool bCanHit = canHit(pSlayer, pTargetCreature, SkillType) && canAttack(pSlayer, pTargetCreature);
        bool bPK = verifyPK(pSlayer, pTargetCreature);
        bool bEffected = pSlayer->hasRelicItem() || pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) ||
                         pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER);

        
        
        if (bManaCheck && bTimeCheck && bRangeCheck && bCanHit && bPK && !bEffected) {
            
            if (pZone->moveFastPC(pSlayer, pSlayer->getX(), pSlayer->getY(), pTargetCreature->getX(),
                                  pTargetCreature->getY(), getSkillType())) {
                decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

                SkillInput input(pSlayer, pSkillSlot);
                SkillOutput output;
                computeOutput(input, output);

                bool bCriticalHit = false;

                
                Damage_t Damage = computeDamage(pSlayer, pTargetCreature, SkillLevel / 5, bCriticalHit);
                
                if (!bHitRoll)
                    Damage = (Damage_t)getPercentValue(Damage, 7);
                setDamage(pTargetCreature, Damage, pSlayer, SkillType, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);
                computeAlignmentChange(pTargetCreature, Damage, pSlayer, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);

                //				Exp_t Point = pSkillInfo->getPoint();
                decreaseDurability(pSlayer, pTargetCreature, pSkillInfo, &_GCSkillToObjectOK1, &_GCSkillToObjectOK2);
                
                if (bCriticalHit) {
                    knockbackCreature(pZone, pTargetCreature, pSlayer->getX(), pSlayer->getY());
                }

                if (!pTargetCreature->isSlayer()) {
                    if (bIncreaseExp) {
                        shareAttrExp(pSlayer, Damage, 8, 1, 1, _GCSkillToObjectOK1);
                        increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1,
                                          pTargetCreature->getLevel());
                        increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToObjectOK1);
                    }
                    increaseAlignment(pSlayer, pTargetCreature, _GCSkillToObjectOK1);
                }

                
                _GCSkillToObjectOK1.setSkillType(SkillType);
                _GCSkillToObjectOK1.setCEffectID(CEffectID);
                _GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
                _GCSkillToObjectOK1.setDuration(0);

                pPlayer->sendPacket(&_GCSkillToObjectOK1);

                if (pTargetCreature->isPC()) {
                    _GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
                    _GCSkillToObjectOK2.setSkillType(SkillType);
                    _GCSkillToObjectOK2.setDuration(0);

                    Player* pTargetPlayer = pTargetCreature->getPlayer();
                    Assert(pTargetPlayer != NULL);
                    pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
                } else {
                    Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
                    pMonster->addEnemy(pSlayer);
                }

                // set Next Run Time
                pSkillSlot->setRunTime(output.Delay);
            } else {
                executeSkillFailException(pSlayer, getSkillType());
            }

        } else {
            executeSkillFailNormal(pSlayer, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

ShadowWalk g_ShadowWalk;
