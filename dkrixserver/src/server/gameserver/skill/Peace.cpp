//////////////////////////////////////////////////////////////////////////////
// Filename    : Peace.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Peace.h"

#include "EffectPeace.h"
#include "GCSkillToSelfOK1.h"
#include "GCSkillToSelfOK2.h"
#include "GCSkillToTileOK1.h"
#include "GCSkillToTileOK2.h"
#include "GCSkillToTileOK3.h"
#include "GCSkillToTileOK4.h"
#include "GCSkillToTileOK5.h"


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Peace::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

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

    SIMPLE_SKILL_OUTPUT result;

    execute(pSlayer, pSlayer->getX(), pSlayer->getY(), pSkillSlot, param, result, CEffectID);

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Peace::execute(Slayer* pSlayer, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    // cout << "Peace: Self Execute" << endl;

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

    SIMPLE_SKILL_OUTPUT result;

    execute(pSlayer, pSlayer->getX(), pSlayer->getY(), pSkillSlot, param, result, CEffectID);

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

void Peace::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

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

    SIMPLE_SKILL_OUTPUT result;

    // cout << "Tile X :" << (int)X << "Tile Y : " << (int)Y << endl;
    execute(pSlayer, X, Y, pSkillSlot, param, result, CEffectID);

    __END_CATCH
}


void Peace::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SkillSlot* pSkillSlot,
                    const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();

        GCSkillToTileOK1 _GCSkillToTileOK1;
        GCSkillToTileOK2 _GCSkillToTileOK2;
        GCSkillToTileOK3 _GCSkillToTileOK3;
        GCSkillToTileOK4 _GCSkillToTileOK4;
        GCSkillToTileOK5 _GCSkillToTileOK5;

        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(param.SkillType);
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);

        ZoneCoord_t myX = pSlayer->getX();
        ZoneCoord_t myY = pSlayer->getY();

        if (bManaCheck && bTimeCheck && bRangeCheck) {
            int Splash = 3 + pSkillSlot->getExpLevel() / 33;

            list<Creature*> cList;
            list<Creature*> creatureList;
            getSplashVictims(pZone, X, Y, Creature::CREATURE_CLASS_MAX, creatureList, Splash);

            bool bSuccess = false;
            //			Duration_t duration = 150+pSlayer->getINT();
            Duration_t duration = 100 + pSlayer->getINT() / 4 + pSkillSlot->getExpLevel() / 2;

            // cout << "Peace: " << creatureList.size() << " victims found" << endl;
            list<Creature*>::iterator itr = creatureList.begin();
            for (; itr != creatureList.end(); itr++) {
                Creature* pTargetCreature = (*itr);
                Assert(pTargetCreature != NULL);

                
                
                if (pTargetCreature->isMonster()) {
                    Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
                    Assert(pMonster != NULL);

                    
                    if (pMonster->isAlive() && !pMonster->isFlag(Effect::EFFECT_CLASS_PEACE)) {
                        bool bHitRoll = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);

                        if (bHitRoll) {
                            
                            EffectPeace* pEffectPeace = new EffectPeace(pMonster, pSlayer->getObjectID());
                            
                            pEffectPeace->setDeadline(duration); 
                            pMonster->addEffect(pEffectPeace);
                            pMonster->setFlag(Effect::EFFECT_CLASS_PEACE);

                            
                            pMonster->deleteEnemy(pSlayer->getObjectID());

                            
                            // cList.push_back(pMonster);

                            bSuccess = true;

                            

                            _GCSkillToTileOK1.addCListElement(pMonster->getObjectID());
                            _GCSkillToTileOK2.addCListElement(pMonster->getObjectID());
                            _GCSkillToTileOK4.addCListElement(pMonster->getObjectID());
                            _GCSkillToTileOK5.addCListElement(pMonster->getObjectID());
                        } else {
                            // cout << "Peace : HitRoll failed" << endl;
                        }
                    } else {
                        // cout << "Peace : dead or already.." << endl;
                    }
                }
            }

            if (bSuccess) {
                
                decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

                
                SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
                Exp_t ExpUp = 10 * (Grade + 1) * 2;

                shareAttrExp(pSlayer, ExpUp, param.STRMultiplier, param.DEXMultiplier, param.INTMultiplier,
                             _GCSkillToTileOK1);
                increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToTileOK1);
                increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToTileOK1);

                pSkillSlot->setRunTime(param.Delay);
                result.bSuccess = true;
            }

            Dir_t dir = calcDirection(myX, myY, X, Y);

            _GCSkillToTileOK1.setSkillType(param.SkillType);
            _GCSkillToTileOK1.setCEffectID(CEffectID);
            _GCSkillToTileOK1.setX(X);
            _GCSkillToTileOK1.setY(Y);
            _GCSkillToTileOK1.setRange(dir);
            _GCSkillToTileOK1.setDuration(duration);

            _GCSkillToTileOK2.setObjectID(pSlayer->getObjectID());
            _GCSkillToTileOK2.setSkillType(param.SkillType);
            _GCSkillToTileOK2.setX(X);
            _GCSkillToTileOK2.setY(Y);
            _GCSkillToTileOK2.setRange(dir);
            _GCSkillToTileOK2.setDuration(duration);

            _GCSkillToTileOK3.setObjectID(pSlayer->getObjectID());
            _GCSkillToTileOK3.setSkillType(param.SkillType);
            _GCSkillToTileOK3.setX(X);
            _GCSkillToTileOK3.setY(Y);

            _GCSkillToTileOK4.setSkillType(param.SkillType);
            _GCSkillToTileOK4.setX(X);
            _GCSkillToTileOK4.setY(Y);
            _GCSkillToTileOK4.setDuration(duration);
            _GCSkillToTileOK4.setRange(dir);

            _GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
            _GCSkillToTileOK5.setSkillType(param.SkillType);
            _GCSkillToTileOK5.setX(X);
            _GCSkillToTileOK5.setY(Y);
            _GCSkillToTileOK5.setRange(dir);
            _GCSkillToTileOK5.setDuration(duration);

            pPlayer->sendPacket(&_GCSkillToTileOK1);

            // cList.push_back(pSlayer);

            
            
             

            cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

            pZone->broadcastPacket(myX, myY, &_GCSkillToTileOK3, cList);
            pZone->broadcastPacket(X, Y, &_GCSkillToTileOK4, cList);
        } else {
            executeSkillFailNormal(pSlayer, param.SkillType, NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, param.SkillType);
    }

    __END_CATCH
}

Peace g_Peace;
