//////////////////////////////////////////////////////////////////////////////
// Filename    : HolyBlast.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "HolyBlast.h"

#include "EffectAftermath.h"
#include "EffectBloodDrain.h"
#include "GCRemoveEffect.h"
#include "GCSkillToSelfOK1.h"
#include "GCSkillToSelfOK2.h"
#include "GCSkillToTileOK1.h"
#include "GCSkillToTileOK2.h"
#include "GCSkillToTileOK3.h"
#include "GCSkillToTileOK4.h"
#include "GCSkillToTileOK5.h"
#include "GCSkillToTileOK6.h"
#include "SimpleTileCureSkill.h"
#include "ZoneUtil.h"


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void HolyBlast::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    SkillInput input(pSlayer, pSkillSlot);
    SkillOutput output;
    input.TargetType = SkillInput::TARGET_OTHER;
    computeOutput(input, output);

    SIMPLE_SKILL_INPUT param;
    param.SkillType = getSkillType();
    param.SkillDamage = output.Damage;
    param.Delay = output.Delay;
    param.STRMultiplier = 1;
    param.DEXMultiplier = 1;
    param.INTMultiplier = 8;
    param.Level = input.SkillLevel;

    SIMPLE_SKILL_OUTPUT result;

    g_SimpleTileCureSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void HolyBlast::execute(Slayer* pSlayer, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    SkillInput input(pSlayer, pSkillSlot);
    SkillOutput output;
    input.TargetType = SkillInput::TARGET_SELF;
    computeOutput(input, output);

    SIMPLE_SKILL_INPUT param;
    param.SkillType = getSkillType();
    param.SkillDamage = output.Damage;
    param.Delay = output.Delay;
    param.STRMultiplier = 1;
    param.DEXMultiplier = 1;
    param.INTMultiplier = 8;
    param.Level = input.SkillLevel;

    SIMPLE_SKILL_OUTPUT result;

    g_SimpleTileCureSkill.execute(pSlayer, pSkillSlot, param, result);

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

void HolyBlast::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    SkillInput input(pSlayer, pSkillSlot);
    SkillOutput output;
    input.TargetType = SkillInput::TARGET_SELF;
    computeOutput(input, output);

    SIMPLE_SKILL_INPUT param;
    param.SkillType = getSkillType();
    param.SkillDamage = output.Damage;
    param.Delay = output.Delay;
    param.STRMultiplier = 1;
    param.DEXMultiplier = 1;
    param.INTMultiplier = 8;
    param.Level = input.SkillLevel;

    SIMPLE_SKILL_OUTPUT result;

    cout << "Tile X :" << (int)X << "Tile Y : " << (int)Y << endl;
    g_SimpleTileCureSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// class SimpleTileMissileSkill member methods
//////////////////////////////////////////////////////////////////////////////

void HolyBlast::execute(Slayer* pSlayer, int X, int Y, SkillSlot* pSkillSlot, const SIMPLE_SKILL_INPUT& param,
                        SIMPLE_SKILL_OUTPUT& result, CEffectID_t CEffectID) {
    __BEGIN_TRY

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();
        Assert(pPlayer != NULL);
        Assert(pZone != NULL);

        
        
        if (param.ItemClass != Item::ITEM_CLASS_MAX) {
            Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
            if (pItem == NULL || pItem->getItemClass() != param.ItemClass) {
                executeSkillFailException(pSlayer, param.SkillType);
                return;
            }
        }

        bool bIncreaseDomainExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);

        GCSkillToTileOK1 _GCSkillToTileOK1;
        GCSkillToTileOK2 _GCSkillToTileOK2;
        GCSkillToTileOK3 _GCSkillToTileOK3;
        GCSkillToTileOK4 _GCSkillToTileOK4;
        GCSkillToTileOK5 _GCSkillToTileOK5;
        GCSkillToTileOK6 _GCSkillToTileOK6;

        GCSkillToSelfOK1 _GCSkillToSelfOK1;
        GCSkillToSelfOK2 _GCSkillToSelfOK2;

        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(param.SkillType);
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();
        SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = verifyDistance(pSlayer, X, Y, pSkillInfo->getRange());

        if (bManaCheck && bTimeCheck && bRangeCheck) {
            
            decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

            
            ZoneCoord_t myX = pSlayer->getX();
            ZoneCoord_t myY = pSlayer->getY();
            Dir_t dir = calcDirection(myX, myY, X, Y);
            Damage_t Damage = 0;
            Damage_t MaxDamage = 0;
            Damage_t TotalDamage = 0;
            
            
            uint HealPoint = 30 + param.Level / 8; // param.SkillDamage;
            uint RealHealPoint = 0;
            bool bCriticalHit = false;
            bool bHit = false;
            bool bHeal = false;

            // VSRect rect(1, 1, pZone->getWidth()-2, pZone->getHeight()-2);

            int Splash = 3 + pSkillSlot->getExpLevel() / 10 + 1;

            list<Creature*> cList;
            list<Creature*> creatureList;
            getSplashVictims(pZone, X, Y, Creature::CREATURE_CLASS_MAX, creatureList, Splash);

            list<Creature*>::iterator itr = creatureList.begin();
            for (; itr != creatureList.end(); itr++) {
                Creature* pTargetCreature = (*itr);
                Assert(pTargetCreature != NULL);

                if (pTargetCreature != pSlayer) 
                {
                    bool bSameRaceCheck = pTargetCreature->isSlayer();
                    bool bZoneLevelCheck = checkZoneLevelToHitTarget(pTargetCreature);

                    
                    if (bSameRaceCheck && bZoneLevelCheck) {
                        
                        
                        

                        EffectBloodDrain* pEffectBloodDrain = NULL;

                        bool bHPCheck = false; 

                        if (!pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
                            Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
                            Assert(pTargetSlayer != NULL);

                            HP_t CurrentHP = pTargetSlayer->getHP(ATTR_CURRENT);
                            HP_t MaxHP = pTargetSlayer->getHP(ATTR_MAX);

                            if (CurrentHP < MaxHP) {
                                bHPCheck = true;
                            }

                            if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN)) {
                                Effect* pEffect = pTargetCreature->findEffect(Effect::EFFECT_CLASS_BLOOD_DRAIN);
                                pEffectBloodDrain = dynamic_cast<EffectBloodDrain*>(pEffect);
                                Assert(pEffectBloodDrain != NULL);

                                if (pEffectBloodDrain->getLevel() < param.Level) {
                                    bHPCheck = true;
                                    bHeal = true;
                                }
                            }

                            bool bHitRoll = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);

                            if (bHitRoll && bHPCheck && pTargetCreature->isAlive()) {
                                
                                _GCSkillToSelfOK1.setSkillType(SKILL_CURE_EFFECT);
                                _GCSkillToSelfOK1.setDuration(0);
                                pTargetSlayer->getPlayer()->sendPacket(&_GCSkillToSelfOK1);

                                _GCSkillToSelfOK2.setObjectID(pTargetSlayer->getObjectID());
                                _GCSkillToSelfOK2.setSkillType(SKILL_CURE_EFFECT);
                                _GCSkillToSelfOK2.setDuration(0);
                                pZone->broadcastPacket(pTargetSlayer->getX(), pTargetSlayer->getY(), &_GCSkillToSelfOK2,
                                                       pTargetSlayer);


                                
                                if (pEffectBloodDrain != NULL && pEffectBloodDrain->getLevel() < param.Level) {
                                    
                                    if (pTargetSlayer->isFlag(Effect::EFFECT_CLASS_AFTERMATH)) {
                                        Effect* pEffect = pTargetSlayer->findEffect(Effect::EFFECT_CLASS_AFTERMATH);
                                        EffectAftermath* pEffectAftermath = dynamic_cast<EffectAftermath*>(pEffect);
                                        pEffectAftermath->setDeadline(5 * 600); 
                                    } else {
                                        EffectAftermath* pEffectAftermath = new EffectAftermath(pTargetSlayer);
                                        pEffectAftermath->setDeadline(5 * 600); 
                                        pTargetSlayer->addEffect(pEffectAftermath);
                                        pTargetSlayer->setFlag(Effect::EFFECT_CLASS_AFTERMATH);
                                        pEffectAftermath->create(pTargetSlayer->getName());
                                    }

                                    pEffectBloodDrain->destroy(pTargetSlayer->getName());
                                    pTargetSlayer->deleteEffect(Effect::EFFECT_CLASS_BLOOD_DRAIN);
                                    pTargetSlayer->removeFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);

                                    SLAYER_RECORD prev;
                                    pTargetSlayer->getSlayerRecord(prev);
                                    pTargetSlayer->initAllStat();
                                    pTargetSlayer->sendRealWearingInfo();

                                    if (pTargetSlayer == pSlayer) {
                                        pTargetSlayer->addModifyInfo(prev, _GCSkillToTileOK1);
                                    } else {
                                        pTargetSlayer->addModifyInfo(prev, _GCSkillToTileOK2);
                                    }

                                    GCRemoveEffect gcRemoveEffect;
                                    gcRemoveEffect.setObjectID(pTargetSlayer->getObjectID());
                                    gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_BLOOD_DRAIN);
                                    pZone->broadcastPacket(pTargetSlayer->getX(), pTargetSlayer->getY(),
                                                           &gcRemoveEffect);
                                }

                                
                                HP_t CurrentHP = pTargetSlayer->getHP(ATTR_CURRENT);
                                HP_t MaxHP = pTargetSlayer->getHP(ATTR_MAX);

                                
                                if (CurrentHP + HealPoint <= MaxHP) {
                                    RealHealPoint = max(0, (int)HealPoint);
                                } else {
                                    RealHealPoint = max(0, (MaxHP - CurrentHP));
                                }

                                CurrentHP = min((int)MaxHP, (int)(CurrentHP + HealPoint));
                                pTargetSlayer->setHP(CurrentHP, ATTR_CURRENT);
                                bHeal = true;
                            }
                        }

                    }
                    
                    else {
                        bool bPK = verifyPK(pSlayer, pTargetCreature);
                        bool bRaceCheck = pTargetCreature->isVampire() || pTargetCreature->isMonster();
                        bool bHitRoll = HitRoll::isSuccess(pSlayer, pTargetCreature, SkillLevel);

                        if (bPK && bRaceCheck && bZoneLevelCheck && bHitRoll) {
                            Damage = 0;
                            bCriticalHit = false;

                            if (param.bAdd) {
                                Damage += computeDamage(pSlayer, pTargetCreature, SkillLevel / 2, bCriticalHit);
                            }

                            if (param.bMagicDamage) {
                                Damage += computeMagicDamage(pTargetCreature, param.SkillDamage, param.SkillType);
                            } else {
                                Damage += param.SkillDamage;
                            }

                            MaxDamage = max(Damage, MaxDamage);
                            TotalDamage += Damage;

                            
                            Damage = getPercentValue(Damage, 100); // penalty);

                            
                            ObjectID_t targetObjectID = pTargetCreature->getObjectID();
                            cList.push_back(pTargetCreature);

                            _GCSkillToTileOK1.addCListElement(targetObjectID);
                            _GCSkillToTileOK2.addCListElement(targetObjectID);
                            _GCSkillToTileOK5.addCListElement(targetObjectID);

                            
                            setDamage(pTargetCreature, Damage, pSlayer, param.SkillType, NULL, &_GCSkillToTileOK1);
                            computeAlignmentChange(pTargetCreature, Damage, pSlayer, NULL, &_GCSkillToTileOK1);

                            increaseAlignment(pSlayer, pTargetCreature, _GCSkillToTileOK1);

                            bHit = true;
                        }
                    }
                }
            }

            if (bHit) {
                if (bIncreaseDomainExp) {
                    if (param.bExpForTotalDamage) {
                        int ExpUp = max((unsigned int)TotalDamage, RealHealPoint);
                        shareAttrExp(pSlayer, ExpUp, param.STRMultiplier, param.DEXMultiplier, param.INTMultiplier,
                                     _GCSkillToTileOK1);
                    } else {
                        int ExpUp = max((unsigned int)MaxDamage, RealHealPoint);
                        shareAttrExp(pSlayer, ExpUp, param.STRMultiplier, param.DEXMultiplier, param.INTMultiplier,
                                     _GCSkillToTileOK1);
                    }

                    increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToTileOK1);
                    increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToTileOK1);
                }
            }

            
            decreaseDurability(pSlayer, NULL, pSkillInfo, &_GCSkillToTileOK1, NULL);

            _GCSkillToTileOK1.setSkillType(param.SkillType);
            _GCSkillToTileOK1.setCEffectID(CEffectID);
            _GCSkillToTileOK1.setX(X);
            _GCSkillToTileOK1.setY(Y);
            _GCSkillToTileOK1.setRange(dir);
            _GCSkillToTileOK1.setDuration(0);

            _GCSkillToTileOK2.setObjectID(pSlayer->getObjectID());
            _GCSkillToTileOK2.setSkillType(param.SkillType);
            _GCSkillToTileOK2.setX(X);
            _GCSkillToTileOK2.setY(Y);
            _GCSkillToTileOK2.setRange(dir);
            _GCSkillToTileOK2.setDuration(0);

            _GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
            _GCSkillToTileOK5.setSkillType(param.SkillType);
            _GCSkillToTileOK5.setX(X);
            _GCSkillToTileOK5.setY(Y);
            _GCSkillToTileOK5.setRange(dir);
            _GCSkillToTileOK5.setDuration(0);

            pPlayer->sendPacket(&_GCSkillToTileOK1);

            
            for (list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++) {
                Creature* pTargetCreature = *itr;
                Assert(pTargetCreature != NULL);

                if (pTargetCreature->isPC()) {
                    _GCSkillToTileOK2.clearList();

                    
                    HP_t targetHP = 0;
                    if (pTargetCreature->isSlayer()) {
                        targetHP = (dynamic_cast<Slayer*>(pTargetCreature))->getHP(ATTR_CURRENT);
                    } else if (pTargetCreature->isVampire()) {
                        targetHP = (dynamic_cast<Vampire*>(pTargetCreature))->getHP(ATTR_CURRENT);
                    } else if (pTargetCreature->isOusters()) {
                        targetHP = (dynamic_cast<Ousters*>(pTargetCreature))->getHP(ATTR_CURRENT);
                    }

                    _GCSkillToTileOK2.addShortData(MODIFY_CURRENT_HP, targetHP);

                    
                    decreaseDurability(NULL, pTargetCreature, pSkillInfo, NULL, &_GCSkillToTileOK2);

                    
                    pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK2);
                } else if (pTargetCreature->isMonster()) {
                    
                    Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
                    pMonster->addEnemy(pSlayer);
                }
            }

            cList.push_back(pSlayer);

            pZone->broadcastPacket(myX, myY, &_GCSkillToTileOK5, cList);

            // set Next Run Time
            pSkillSlot->setRunTime(param.Delay);
        } else {
            executeSkillFailNormal(pSlayer, param.SkillType, NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, param.SkillType);
    }

    __END_CATCH
}


HolyBlast g_HolyBlast;
