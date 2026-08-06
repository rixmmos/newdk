//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillUtil.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILLUTIL_H__
#define __SKILLUTIL_H__

#include "ModifyInfo.h"
#include "Ousters.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////




#define CONSUME_MP 0
#define CONSUME_HP 1
#define CONSUME_BOTH 2

#define MAKEWORD(U, D) (WORD)((WORD)(U) << 8 | (WORD)(D))
#define MAKEDWORD(U, D) (DWORD)((DWORD)(U) << 16 | (DWORD)(D))

// #define VAMPIRE_MAX_LEVEL	150

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Creature;
class Monster;
class SkillInfo;
class SkillSlot;
class VampireSkillSlot;
class Item;
class Zone;

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


Damage_t computeDamage(Creature* pCreature, Creature* pTargetCreature);


Damage_t computePureDamage(Creature* pCreature);



Damage_t computeDamage(Creature* pCreature, Creature* pTargetCreature, int CriticalBonus, bool& bCritical);


double computeFinalDamage(Damage_t minDamage, Damage_t maxDamage, Damage_t realDamage, Protection_t Protection,
                          bool bCritical);


Damage_t computeSlayerDamage(Slayer* pSlayer, Creature* pTargetCreature, bool bCritical);


Damage_t computeVampireDamage(Vampire* pVampire, Creature* pTargetCreature, bool bCritical);


Damage_t computeOustersDamage(Ousters* pOusters, Creature* pTargetCreature, bool bCritical);


Damage_t computeMonsterDamage(Monster* pMonster, Creature* pTargetCreature, bool bCritical);


Damage_t computePureSlayerDamage(Slayer* pSlayer);


Damage_t computePureVampireDamage(Vampire* pVampire);


Damage_t computePureOustersDamage(Ousters* pOusters);


Damage_t computePureMonsterDamage(Monster* pMonster);


Damage_t computeMagicDamage(Creature* pTargetCreature, int Damage, SkillType_t SkillType, bool bVampire = false,
                            Creature* pAttacker = NULL);


Damage_t computeOustersMagicDamage(Ousters* pOusters, Creature* pTargetCreature, int Damage, SkillType_t SkillType);


Damage_t computeSlayerSilverDamage(Creature* pCreature, int Damage, ModifyInfo* pMI);


void computeCriticalBonus(Ousters* pOusters, SkillType_t skillType, Damage_t& Damage, bool& bCriticalHit);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


HP_t setDamage(Creature* pTargetCreature, Damage_t Damage, Creature* pAttacker, SkillType_t SkillType = 0,
               ModifyInfo* pMI = NULL, ModifyInfo* pAttackerMI = NULL, bool canKillTarget = true, bool canSteal = true);


void decreaseDurability(Creature* pCreature, Creature* pTargetCreature, SkillInfo* pSkillInfo, ModifyInfo*,
                        ModifyInfo*);


bool canHit(Creature* pAttacker, Creature* pDefender, SkillType_t SkillType, SkillLevel_t SkillLevel = 0);


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


MP_t decreaseConsumeMP(Vampire* pVampire, SkillInfo* pSkillInfo);


bool hasEnoughMana(Creature* pCaster, int RequiredMP);


int decreaseMana(Creature* pCaster, int MP, ModifyInfo& modifyinfo);


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


Range_t computeSkillRange(SkillSlot* pSkillSlot, SkillInfo* pSkillInfo);


Range_t getDistance(ZoneCoord_t OX, ZoneCoord_t OY, ZoneCoord_t TX, ZoneCoord_t TY);


bool verifyDistance(Creature* pCreature, ZoneCoord_t X, ZoneCoord_t Y, Range_t Distance);


bool verifyDistance(Creature* pCreature, Creature* pTargetCreature, Range_t Distance);


bool verifyRunTime(SkillSlot* pSkillSlot);


bool verifyRunTime(VampireSkillSlot* pSkillSlot);


bool verifyRunTime(OustersSkillSlot* pSkillSlot);


bool verifyPK(Creature* pAttacker, Creature* pDefender);



bool checkZoneLevelToUseSkill(Creature* pCaster);


bool checkZoneLevelToHitTarget(Creature* pTargetCreature);


// bool isMeleeSkill(SkillType_t SkillType);


// bool isMagicSkill(SkillType_t SkillType);

// bool isPhysicSkill(SkillType_t SkillType);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////



void computeAlignmentChange(Creature* pTargetCreature, Damage_t Damage, Creature* pAttacker, ModifyInfo* pMI = NULL,
                            ModifyInfo* pAttackerMI = NULL);




void increaseAlignment(Creature* pCreature, Creature* pEnemy, ModifyInfo& mi);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


void shareAttrExp(Slayer* pSlayer, Damage_t Damage, BYTE STRMultiplier, BYTE DEXMultiplier, BYTE INTMultiplier,
                  ModifyInfo&);


void shareVampExp(Vampire*, Exp_t, ModifyInfo&);


void shareOustersExp(Ousters*, Exp_t, ModifyInfo&);


void divideAttrExp(Slayer* pSlayer, Damage_t Damage, BYTE STRMultiplier, BYTE DEXMultiplier, BYTE INTMultiplier,
                   ModifyInfo&, int numPartyMember = -1);


void increaseSkillExp(Slayer* pSlayer, SkillDomainType_t DomainType, SkillSlot* pSkillSlot, SkillInfo* pSkillInfo,
                      ModifyInfo&);


bool increaseDomainExp(Slayer* pSlayer, SkillDomainType_t Domain, Exp_t Exp, ModifyInfo&, Level_t EnemyLevel = 0,
                       int TargetNum = -1);


void increaseVampExp(Vampire*, Exp_t, ModifyInfo&);


void increaseOustersExp(Ousters*, Exp_t, ModifyInfo&);


void increaseFame(Creature* pAttackee, uint amount);


RankExp_t computeRankExp(int myLevel, int otherLevel);
void affectKillCount(Creature* pAttacker, Creature* pDeadCreature);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


int computeArmsWeaponSplashSize(Item* pWeapon, int ox, int oy, int tx, int ty);
int computeArmsWeaponDamageBonus(Item* pWeapon, int ox, int oy, int tx, int ty);
int computeArmsWeaponToHitBonus(Item* pWeapon, int ox, int oy, int tx, int ty);


int getSplashVictims(Zone* pZone, int cx, int cy, Creature::CreatureClass CClass, list<Creature*>& creatureList,
                     int splash);
int getSplashVictims(Zone* pZone, int cx, int cy, Creature::CreatureClass CClass, list<Creature*>& creatureList,
                     int splash, int range);
int getSplashVictims(Zone* pZone, int cx, int cy, list<Creature*>& creatureList, int splash);


void healCreatureForLevelUp(Slayer* pSlayer, ModifyInfo& _ModifyInfo, SLAYER_RECORD* prev);
void healCreatureForLevelUp(Vampire* pVampire, ModifyInfo& _ModifyInfo, VAMPIRE_RECORD* prev);
void healCreatureForLevelUp(Ousters* pOusters, ModifyInfo& _ModifyInfo, OUSTERS_RECORD* prev);


void executeSkillFailNormal(Creature* pCreature, SkillType_t SkillType, Creature* pTargetCreature, BYTE Grade = 0);
void executeAbsorbSoulSkillFail(Creature* pCreature, SkillType_t SkillType, ObjectID_t TargetObjectID, bool bBroadcast,
                                bool bSendTwice);
void executeSkillFailNormalWithGun(Creature* pCreature, SkillType_t SkillType, Creature* pTargetCreature,
                                   BYTE RemainBullet);
void executeSkillFailException(Creature* pCreature, SkillType_t SkillType, BYTE Grade = 0);

void decreaseHP(Zone* pZone, Creature* pCreature, int Damage, ObjectID_t attackerObjectID = 0);

Dir_t getDirectionToPosition(int originX, int originY, int destX, int destY);

Exp_t computeSkillPointBonus(SkillDomainType_t Domain, SkillLevel_t DomainLevel, Item* pWeapon, Exp_t Point);



bool isPassLine(Zone* pZone, ZoneCoord_t sX, ZoneCoord_t sY, ZoneCoord_t eX, ZoneCoord_t eY,
                bool blockByCreature = false);


void getLinePoint(ZoneCoord_t sX, ZoneCoord_t sY, ZoneCoord_t eX, ZoneCoord_t eY, list<TPOINT>& tpList);

ElementalType getElementalTypeFromString(const string& type);

Damage_t computeElementalCombatSkill(Ousters* pOusters, Creature* pTargetCreature, ModifyInfo& AttackerMI);

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
bool canAttack(Creature* pAttacker, Creature* pDefender);

//////////////////////////////////////////////////////////////////////////
// add by Coffee 2007-6-9

//////////////////////////////////////////////////////////////////////////
bool useSkillCrad(Creature* pCreature);


#endif
