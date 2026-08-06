//////////////////////////////////////////////////////////////////////////////
// Filename    : HitRoll.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __HITROLL_H__
#define __HITROLL_H__

#include "ModifyInfo.h"

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Creature;
class Slayer;
class Vampire;
class Ousters;
class Monster;
class SkillInfo;
class SkillSlot;
class VampireSkillSlot;
class OustersSkillSlot;

//////////////////////////////////////////////////////////////////////////////
// class HitRoll

//////////////////////////////////////////////////////////////////////////////

class HitRoll {
public:
    
    static bool isSuccess(Creature* pAttacker, Creature* pDefender, int ToHitBonus = 0);
    //	static bool isSuccess(ToHit_t ToHit, Creature* pDefender, int ToHitBonus = 0);

    
    static bool isSuccessMagic(Slayer* pSlayer, SkillInfo* pSkillInfo, SkillSlot* pSkillSlot);

    
    static bool isSuccessMagic(Vampire* pVampire, SkillInfo* pSkillInfo, VampireSkillSlot* pVampireSkillSlot,
                               int BonusPoint = 0);

    
    static bool isSuccessMagic(Ousters* pOusters, SkillInfo* pSkillInfo, OustersSkillSlot* pOustersSkillSlot,
                               int BonusPoint = 0);

    
    static bool isSuccessMagic(Monster* pMonster, SkillInfo* pSkillInfo);

    
    static bool isSuccessBloodDrain(Creature* pAttacker, Creature* pDefender, int multiplier = 3);

    
    static bool isSuccessCurse(int MagicLevel, Resist_t resist);
    static bool isSuccessVampireCurse(int MagicLevel, Resist_t resist);

    
    static bool isSuccessCurePoison(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio = 0);

    
    static bool isSuccessFlare(Creature* pTargetCreature, int SkillLevel);

    
    static bool isSuccessRemoveCurse(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio = 0);

    
    static bool isSuccessRebuke(Slayer* pSlayer, SkillSlot* pSkillSlot, Creature* pDefender);

    
    static bool isSuccessMagicElusion(Slayer* pSlayer);

    
    static bool isSuccessPoisonMesh(Vampire* pVampire);

    
    static bool isSuccessIllusionOfAvenge(Slayer* pSlayer);

    
    static bool isSuccessWillOfLife(Vampire* pVampire);

    
    static bool isCriticalHit(Creature* pCreature, int CriticalBonus = 0);

    
    static bool isSlayerSelfSkill(SkillType_t skillType);
    static bool isOustersSelfSkill(SkillType_t skillType);

    
    static bool isSuccessHallucination(Vampire* pAttacker, Creature* pTarget);

    
    static bool isSuccessBackStab(Ousters* pAttacker);
};

#endif
