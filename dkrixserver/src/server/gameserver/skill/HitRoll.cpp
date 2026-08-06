//////////////////////////////////////////////////////////////////////////////
// Filename    : HitRoll.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "HitRoll.h"

#include "LogClient.h"
#include "Monster.h"
#include "Ousters.h"
#include "RankBonus.h"
#include "Skill.h"
#include "SkillInfo.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"
// #include <math.h>

#include "EffectHymn.h"
#include "EffectPrecedence.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccess(Creature* pAttacker, Creature* pDefender, int ToHitBonus) {
    Assert(pAttacker != NULL);
    Assert(pDefender != NULL);

    Zone* pZone = pAttacker->getZone();
    Assert(pZone != NULL);

    
    if (pDefender->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
        return false;
    }


    ToHit_t ToHit = 0;
    Defense_t Defense = 0;
    uint timeband = pZone->getTimeband();

    bool isMonster = false;

    
    if (pAttacker->isSlayer()) {
        Slayer* pSlayerAttacker = dynamic_cast<Slayer*>(pAttacker);
        Assert(pSlayerAttacker != NULL);

        ToHit = pSlayerAttacker->getToHit();

        // cout << "HitRoll: " << pSlayerAttacker->getName().c_str() << " --> ";
    } else if (pAttacker->isVampire()) {
        Vampire* pVampireAttacker = dynamic_cast<Vampire*>(pAttacker);
        Assert(pVampireAttacker != NULL);

        ToHit = pVampireAttacker->getToHit();
        ToHit = (ToHit_t)getPercentValue(ToHit, VampireTimebandFactor[timeband]);

        
        
        if (pAttacker->isFlag(Effect::EFFECT_CLASS_HYMN)) {
            EffectHymn* pHymn =
                dynamic_cast<EffectHymn*>(pAttacker->getEffectManager()->findEffect(Effect::EFFECT_CLASS_HYMN));

            ToHit = ToHit * (100 - pHymn->getToHitPenalty()) / 100;
        }
    } else if (pAttacker->isOusters()) {
        Ousters* pOustersAttacker = dynamic_cast<Ousters*>(pAttacker);
        Assert(pOustersAttacker != NULL);

        ToHit = pOustersAttacker->getToHit();
    } else if (pAttacker->isMonster()) {
        Monster* pMonsterAttacker = dynamic_cast<Monster*>(pAttacker);
        Assert(pMonsterAttacker != NULL);

        ToHit = pMonsterAttacker->getToHit();
        ToHit = (ToHit_t)getPercentValue(ToHit, MonsterTimebandFactor[timeband]);
        isMonster = true;

        
        
        if (pAttacker->isFlag(Effect::EFFECT_CLASS_HYMN)) {
            EffectHymn* pHymn =
                dynamic_cast<EffectHymn*>(pAttacker->getEffectManager()->findEffect(Effect::EFFECT_CLASS_HYMN));

            ToHit = ToHit * (100 - pHymn->getToHitPenalty()) / 100;
        }

    } else {
        
        
        
        return false;
    }

    
    if (pDefender->isSlayer()) {
        Slayer* pSlayerDefender = dynamic_cast<Slayer*>(pDefender);
        Defense = pSlayerDefender->getDefense();

        // cout << pSlayerDefender->getName().c_str() << " : ";
    } else if (pDefender->isVampire()) {
        Vampire* pVampireDefender = dynamic_cast<Vampire*>(pDefender);
        Defense = pVampireDefender->getDefense();
        Defense = (Defense_t)getPercentValue(Defense, VampireTimebandFactor[timeband]);
    } else if (pDefender->isOusters()) {
        Ousters* pOustersDefender = dynamic_cast<Ousters*>(pDefender);
        Defense = pOustersDefender->getDefense();
    } else if (pDefender->isMonster()) {
        Monster* pMonsterDefender = dynamic_cast<Monster*>(pDefender);
        Defense = pMonsterDefender->getDefense();
        Defense = (Defense_t)getPercentValue(Defense, MonsterTimebandFactor[timeband]);
#ifndef __CHINA_SERVER__
        isMonster = true;
#endif
    } else {
        
        
        
        return false;
    }

    int RandValue = Random(0, 100);
    int Result = 0;

    if (ToHit >= Defense) {
        
#ifdef __CHINA_SERVER__
        Result = min(90, (int)(((ToHit - Defense) / 1.5) + 60) + ToHitBonus);
#else
        if (isMonster) {
            Result = min(95, (int)(((ToHit - Defense) / 3) + 50) + ToHitBonus / 2);
        } else {
            Result = min(90, (int)(((ToHit - Defense) / 3) + 50) + ToHitBonus / 2);
        }
#endif
    } else {
        
#ifdef __CHINA_SERVER__
        if (isMonster) {
            Result = max(10, (int)(60 - ((Defense - ToHit) / 1.5) + ToHitBonus));
        } else {
            Result = max(20, (int)(60 - ((Defense - ToHit) / 1.5) + ToHitBonus));
        }
#else
        if (isMonster) {
            Result = max(5, (int)(50 - ((Defense - ToHit) / 3) + ToHitBonus / 2));
        } else {
            Result = max(10, (int)(50 - ((Defense - ToHit) / 3) + ToHitBonus / 2));
        }
#endif
    }

    // cout << ToHit << " --> " << Defense << " : " << RandValue << "<=" << Result
    //		<< (RandValue <= Result? "Success" : "Fail") << endl;
    if (RandValue <= Result)
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
 


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Slayer* pSlayer, SkillInfo* pSkillInfo, SkillSlot* pSkillSlot) {
    Assert(pSlayer != NULL);
    Assert(pSkillInfo != NULL);
    Assert(pSkillSlot != NULL);

    int RandValue = Random(1, 100);
#ifdef __CHINA_SERVER__
    int SuccessRatio =
        (int)(45 - pSkillInfo->getLevel() / 2 + (int)((pSlayer->getINT() + pSkillSlot->getExpLevel()) / 2.5));
#else
    int SuccessRatio =
        (int)(60 - pSkillInfo->getLevel() / 3 + (int)((pSlayer->getINT() + pSkillSlot->getExpLevel()) / 2.5));
#endif

    
    if (isSlayerSelfSkill(pSkillSlot->getSkillType()))
        SuccessRatio = max(50, SuccessRatio);

    if (RandValue < SuccessRatio)
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Vampire* pVampire, SkillInfo* pSkillInfo, VampireSkillSlot* pVampireSkillSlot,
                             int BonusPoint) {
    Assert(pVampire != NULL);
    Assert(pSkillInfo != NULL);
    Assert(pVampireSkillSlot != NULL);

    int RandValue = Random(1, 100);
    int Success = (int)(45 - pSkillInfo->getLevel() / 2 + (pVampire->getINT() + pVampire->getLevel()) / 4);

    if (BonusPoint != 0) {
        Success = getPercentValue(Success, 100 + BonusPoint);
    }

    if (RandValue < Success)
        return true;

    //	if (RandValue < (int)(45 - pSkillInfo->getLevel() / 2 + (pVampire->getINT() + pVampire->getLevel()) / 4)) return
    // true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Ousters* pOusters, SkillInfo* pSkillInfo, OustersSkillSlot* pOustersSkillSlot,
                             int BonusPoint) {
    Assert(pOusters != NULL);
    Assert(pSkillInfo != NULL);
    Assert(pOustersSkillSlot != NULL);

    int RandValue = Random(1, 100);
    //	int Success = (int)( 45 - pSkillInfo->getLevel() / 2 + (pOusters->getINT() + pOusters->getLevel())/10 );
    int Success = (int)(45 + (pOusters->getINT() + pOusters->getLevel()) / 4 + pOustersSkillSlot->getExpLevel() / 3);

    if (isOustersSelfSkill(pOustersSkillSlot->getSkillType())) {
        Success = max(Success, 60);
    }

    if (BonusPoint != 0) {
        Success = getPercentValue(Success, 100 + BonusPoint);
    }

    if (RandValue < Success)
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Monster* pMonster, SkillInfo* pSkillInfo) {
    Assert(pMonster != NULL);
    Assert(pSkillInfo != NULL);

    int ratio = (int)(45 - pSkillInfo->getLevel() / 2 + (pMonster->getINT() + pMonster->getLevel()) / 4);
    if (rand() % 100 < ratio)
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


// by sigi. 2002.9.16
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessBloodDrain(Creature* pAttacker, Creature* pDefender, int multiplier) {
    Assert(pAttacker != NULL);
    Assert(pDefender != NULL);

    Zone* pZone = pAttacker->getZone();
    Assert(pZone != NULL);

    
    if (pDefender->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
        return false;
    }


    const int normalMultiplier = 3; 
    bool bHPCheck = false;
    bool bEffected = false;
    uint timeband = pZone->getTimeband();

    int ToHit = 0;
    int Defense = 0;
    // int VampLevel  = 0;
    int OtherLevel = 0;
    int ratio = 0;

    
    
    if (pDefender->isSlayer()) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pDefender);
        int MaxHP = pTargetSlayer->getHP(ATTR_MAX);
        int CurHP = pTargetSlayer->getHP(ATTR_CURRENT);

        bHPCheck = (CurHP * multiplier <= MaxHP) ? true : false;
        bEffected = (normalMultiplier == multiplier) && pTargetSlayer->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
        Defense = pTargetSlayer->getDefense();

        OtherLevel = pTargetSlayer->getSTR(ATTR_CURRENT) + pTargetSlayer->getDEX(ATTR_CURRENT) +
                     pTargetSlayer->getINT(ATTR_CURRENT) + pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_BLADE) +
                     pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_SWORD) +
                     pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_GUN) +
                     pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_HEAL) +
                     pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_ENCHANT);

        OtherLevel = (int)(OtherLevel / 350);

        
        
        if (!bHPCheck || bEffected)
            return false;
    } else if (pDefender->isVampire()) {
        Vampire* pTargetVampire = dynamic_cast<Vampire*>(pDefender);
        int MaxHP = pTargetVampire->getHP(ATTR_MAX);
        int CurHP = pTargetVampire->getHP(ATTR_CURRENT);

        bHPCheck = (CurHP * multiplier <= MaxHP) ? true : false;
        // bEffected  = pTargetVampire->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
        bEffected = (normalMultiplier == multiplier) && pTargetVampire->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
        Defense = pTargetVampire->getDefense() + pTargetVampire->getLevel() / 5;
        OtherLevel = pTargetVampire->getLevel();

        Defense = (Defense_t)getPercentValue(Defense, VampireTimebandFactor[timeband]);

        
        
        if (!bHPCheck || bEffected)
            return false;
    } else if (pDefender->isOusters()) {
        Ousters* pTargetOusters = dynamic_cast<Ousters*>(pDefender);
        int MaxHP = pTargetOusters->getHP(ATTR_MAX);
        int CurHP = pTargetOusters->getHP(ATTR_CURRENT);

        bHPCheck = (CurHP * multiplier <= MaxHP) ? true : false;
        // bEffected  = pTargetOusters->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
        bEffected = (normalMultiplier == multiplier) && pTargetOusters->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
        Defense = pTargetOusters->getDefense() + pTargetOusters->getLevel() / 5;
        OtherLevel = pTargetOusters->getLevel();

        
        
        if (!bHPCheck || bEffected)
            return false;
    } else if (pDefender->isMonster()) {
        Monster* pTargetMonster = dynamic_cast<Monster*>(pDefender);
        int MaxHP = pTargetMonster->getHP(ATTR_MAX);
        int CurHP = pTargetMonster->getHP(ATTR_CURRENT);

        bHPCheck = (CurHP * multiplier <= MaxHP) ? true : false;
        // bEffected  = pTargetMonster->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
        bEffected = (normalMultiplier == multiplier) && pTargetMonster->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
        Defense = pTargetMonster->getDefense() + pTargetMonster->getLevel() / 5;
        OtherLevel = pTargetMonster->getLevel();

        Defense = (Defense_t)getPercentValue(Defense, MonsterTimebandFactor[timeband]);

        
        
        if (!bHPCheck || bEffected)
            return false;

        
        
        if (pTargetMonster->isFlag(Effect::EFFECT_CLASS_PRECEDENCE)) {
            EffectPrecedence* pEffectPrecedence =
                dynamic_cast<EffectPrecedence*>(pTargetMonster->findEffect(Effect::EFFECT_CLASS_PRECEDENCE));
            Assert(pEffectPrecedence != NULL);

            
            if (pAttacker->getName() != pEffectPrecedence->getHostName()) {
                
                
                if (pAttacker->getPartyID() == 0 || pAttacker->getPartyID() != pEffectPrecedence->getHostPartyID()) {
                    return false;
                }
            }
        }
    } else
        Assert(false);

    
    if (pAttacker->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);

        ToHit = pVampire->getToHit();
        // VampLevel = pVampire->getLevel();

        ToHit = (ToHit_t)getPercentValue(ToHit, VampireTimebandFactor[timeband]);
    } else if (pAttacker->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pAttacker);

        ToHit = pMonster->getToHit();
        // VampLevel = pMonster->getLevel();

        ToHit = (ToHit_t)getPercentValue(ToHit, MonsterTimebandFactor[timeband]);
    }

    
    

    if (ToHit >= Defense) {
        
        ratio = min(90, (ToHit - Defense) / 2 + 70);
    } else {
        
        ratio = max(10, 70 - (Defense - ToHit) / 2);
    }

    if ((rand() % 100) < ratio) {
        
        return true;
    }

    
    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessCurse(int MagicLevel, Resist_t resist) {
    
    
    //
    
    
    
    
#ifdef __CHINA_SERVER__
    int prob_penalty = (int)(MagicLevel - resist);
    int curse_prob = 65 + prob_penalty;
#else
    int prob_penalty = (int)(MagicLevel * 2 / 1.5 - resist);
    int curse_prob = 75 + prob_penalty;
    curse_prob = max(5, curse_prob);
#endif
    int randomValue = rand() % 100;

    
    if (randomValue < curse_prob)
        return true;

    
    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessVampireCurse(int MagicLevel, Resist_t resist) {
    
    
    //
    
    
    
    
    int prob_penalty = (int)((int)(MagicLevel / 1.5) - resist);
    int curse_prob = 75 + prob_penalty;
    curse_prob = max(5, curse_prob);

    int randomValue = rand() % 100;

    
    if (randomValue < curse_prob)
        return true;

    
    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessCurePoison(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio) {
    int ratio = Base + SkillLevel - Difficulty - MagicLevel;

    
    ratio = max(MinRatio, ratio);

    if (rand() % 100 < ratio)
        return true;
    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessFlare(Creature* pTargetCreature, int SkillLevel) {
    Assert(pTargetCreature != NULL);

    
    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
        return false;

    int ratio = 0;

    if (pTargetCreature->isPC()) {
        //		Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
        ratio = 75 + SkillLevel - pTargetCreature->getLevel();
    } else if (pTargetCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

        //
        // by sigi. 2002.10.30
        if (pMonster->isMaster()
#ifdef __UNDERWORLD__
            || pMonster->isUnderworld() || pMonster->getMonsterType() == 599
#endif
        ) {
            return false;
        }

        ratio = 75 + SkillLevel - pMonster->getLevel();
    }

    if (rand() % 100 < ratio)
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessRemoveCurse(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio /* = 0 */) {
    int ratio = Base + SkillLevel - Difficulty - MagicLevel;

    
    ratio = max(MinRatio, ratio);

    if (rand() % 100 < ratio)
        return true;
    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessRebuke(Slayer* pSlayer, SkillSlot* pSkillSlot, Creature* pDefender) {
    if (pDefender->isSlayer())
        return false;

    
    if (pDefender->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pDefender);
        if (pVampire->getLevel() >= 80)
            return false;
    } else if (pDefender->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pDefender);
        if (pOusters->getLevel() >= 80)
            return false;
    }

    
    if (pDefender->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pDefender);
        if (pMonster->isMaster()
#ifdef __UNDERWORLD__
            || pMonster->isUnderworld() || pMonster->getMonsterType() == 599
#endif
        )
            return false;
    }

    Attr_t INTE = pSlayer->getINT(ATTR_CURRENT);
    SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

    //	int ratio = ( INTE / 10 ) + ( SkillLevel / 3 );
    int ratio = 20 + (INTE / 10) + (SkillLevel / 2);
    if (rand() % 100 < ratio)
        return true;
    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagicElusion(Slayer* pSlayer) {
    Attr_t SUM = pSlayer->getTotalAttr(ATTR_CURRENT);
    int Ratio = 50 + (SUM / 15);

    return (rand() % 100) < Ratio;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessPoisonMesh(Vampire* pVampire) {
    int Ratio = 30 + (pVampire->getLevel() / 5);

    return (rand() % 100) < Ratio;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessIllusionOfAvenge(Slayer* pSlayer) {
    Attr_t SUM = pSlayer->getTotalAttr(ATTR_CURRENT);
    int Ratio = 50 + (SUM / 15);

    return (rand() % 100) < Ratio;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessWillOfLife(Vampire* pVampire) {
    int Ratio = 50 + pVampire->getLevel() / 5;

    return (rand() % 100) < Ratio;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isCriticalHit(Creature* pCreature, int CriticalBonus) {
    Assert(pCreature != NULL);

    int CriticalRatio = pCreature->getCriticalRatio() + CriticalBonus;

    if (rand() % 1000 < CriticalRatio)
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSlayerSelfSkill(SkillType_t skillType) {
    switch (skillType) {
    case SKILL_DANCING_SWORD:
    case SKILL_CROSS_COUNTER:
    case SKILL_EXPANSION:
    case SKILL_SHARP_SHIELD:

    case SKILL_GHOST_BLADE:
    case SKILL_POTENTIAL_EXPLOSION:
    case SKILL_CHARGING_POWER:
    case SKILL_BERSERKER:
    case SKILL_AIR_SHIELD:

    case SKILL_SNIPING:
    case SKILL_OBSERVING_EYE:

    case SKILL_CREATE_HOLY_WATER:
    case SKILL_LIGHT:
    case SKILL_DETECT_HIDDEN:
    case SKILL_BLESS:
    case SKILL_STRIKING:
    case SKILL_DETECT_INVISIBILITY:
    case SKILL_AURA_SHIELD:
    case SKILL_REFLECTION:
    case SKILL_REBUKE:
    case SKILL_SPIRIT_GUARD:

    case SKILL_CURE_LIGHT_WOUNDS:
    case SKILL_CURE_POISON:
    case SKILL_PROTECTION_FROM_POISON:
    case SKILL_CURE_SERIOUS_WOUNDS:
    case SKILL_REMOVE_CURSE:
    case SKILL_PROTECTION_FROM_CURSE:
    case SKILL_CURE_CRITICAL_WOUNDS:
    case SKILL_PROTECTION_FROM_ACID:
    case SKILL_CURE_ALL:
    case SKILL_PEACE:
    case SKILL_ACTIVATION:
    case SKILL_REGENERATION:
    case SKILL_TURN_UNDEAD:
        return true;

    default:
        return false;
    }
}

bool HitRoll::isOustersSelfSkill(SkillType_t skillType) {
    switch (skillType) {
    case SKILL_EVADE:
    case SKILL_BLUNTING:
    case SKILL_CROSS_GUARD:
    case SKILL_SHARP_CHAKRAM:
        return true;

    default:
        return false;
    }
}
bool HitRoll::isSuccessHallucination(Vampire* pAttacker, Creature* pTarget) {
    if (pTarget->isMonster())
        return true;

    Attr_t attackTotalAttr = pAttacker->getSTR() + pAttacker->getDEX() + pAttacker->getINT();
    Attr_t targetTotalAttr = 0;
    int minRatio = 0, maxRatio = 100;

    if (pTarget->isSlayer()) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTarget);
        targetTotalAttr = pTargetSlayer->getTotalAttr();
        minRatio = 30;
        maxRatio = 60;
    } else if (pTarget->isVampire()) {
        Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTarget);
        targetTotalAttr = pTargetVampire->getSTR() + pTargetVampire->getDEX() + pTargetVampire->getINT();
        minRatio = 10;
        maxRatio = 40;
    } else if (pTarget->isOusters()) {
        Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTarget);
        targetTotalAttr = pTargetOusters->getSTR() + pTargetOusters->getDEX() + pTargetOusters->getINT();
        minRatio = 10;
        maxRatio = 50;

    } else
        return false;

    int Ratio = attackTotalAttr - targetTotalAttr;
    Ratio = max(minRatio, Ratio);
    Ratio = min(maxRatio, Ratio);

    return (rand() % 100) < Ratio;
}

bool HitRoll::isSuccessBackStab(Ousters* pAttacker) {
    int Ratio = min(50, (pAttacker->getINT() / 5) + (pAttacker->getDEX() / 5));
    if (rand() % 100 < Ratio)
        return true;
    return false;
}
