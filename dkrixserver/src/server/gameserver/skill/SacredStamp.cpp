//////////////////////////////////////////////////////////////////////////////
// Filename    : SacredStamp.cpp
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SacredStamp.h"

#include "RankBonus.h"
#include "SimpleTileMissileSkill.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void SacredStamp::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SkillSlot* pSkillSlot, CEffectID_t CEffectID) {
    __BEGIN_TRY

    Zone* pZone = pSlayer->getZone();
    Assert(pZone != NULL);

    Creature* pTargetCreature = pZone->getCreature(targetObjectID);
    // Assert(pTargetCreature != NULL);

    
    if (pTargetCreature == NULL) {
        executeSkillFailException(pSlayer, getSkillType());
        return;
    }

    execute(pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID);

    __END_CATCH
}

void SacredStamp::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SkillSlot* pSkillSlot, CEffectID_t CEffectID) {
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

    SkillInput input(pSlayer, pSkillSlot);
    SkillOutput output;
    computeOutput(input, output);

    SIMPLE_SKILL_INPUT param;
    param.SkillType = getSkillType();
    param.SkillDamage = output.Damage;
    param.Delay = output.Delay;
    param.ItemClass = Item::ITEM_CLASS_MAX;
    param.STRMultiplier = 1;
    param.DEXMultiplier = 1;
    param.INTMultiplier = 8;
    param.bMagicHitRoll = true;
    param.bMagicDamage = true;
    param.bAdd = false;

    for (int i = -2; i <= 2; ++i)
        for (int j = -2; j <= 2; ++j) {
            param.addMask(i, j, 100);
        }

    SIMPLE_SKILL_OUTPUT result;

    
    if (pSlayer->hasRankBonus(RankBonus::RANK_BONUS_HOLY_SMASHING)) {
        RankBonus* pRankBonus = pSlayer->getRankBonus(RankBonus::RANK_BONUS_HOLY_SMASHING);
        Assert(pRankBonus != NULL);

        param.SkillDamage += pRankBonus->getPoint();
    }

    g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

    __END_CATCH
}

SacredStamp g_SacredStamp;
