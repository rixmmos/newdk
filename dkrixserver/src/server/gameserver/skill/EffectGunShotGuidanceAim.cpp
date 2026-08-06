//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGunShotGuidanceAim.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectGunShotGuidanceAim.h"

#include "GCAddEffectToTile.h"
#include "GCRemoveEffect.h"
#include "GCSkillToObjectOK2.h"
#include "GCSkillToObjectOK4.h"
#include "GCStatusCurrentHP.h"
#include "GamePlayer.h"
#include "HitRoll.h"
#include "Monster.h"
#include "PCFinder.h"
#include "SkillInfo.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "Vampire.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"


int GSGDamageModify[5][5] = {
    {50, 50, 50, 50, 50}, {50, 75, 75, 75, 50}, {50, 75, 100, 75, 50}, {50, 75, 75, 75, 50}, {50, 50, 50, 50, 50}};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectGunShotGuidanceAim::EffectGunShotGuidanceAim(Creature* pCreature, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y)

{
    __BEGIN_TRY

    setTarget(pCreature);
    m_pZone = pZone;
    m_X = x;
    m_Y = y;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGunShotGuidanceAim::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGunShotGuidanceAim::unaffect(Creature* pCastCreature)

{
    __BEGIN_TRY

    Assert(pCastCreature != NULL);

    
    if (!pCastCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM))
        return;

    
    pCastCreature->removeFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM);

    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCastCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM);

    m_pZone->broadcastPacket(pCastCreature->getX(), pCastCreature->getY(), &gcRemoveEffect);

    VSRect rect(0, 0, m_pZone->getWidth() - 1, m_pZone->getHeight() - 1); 

    bool bHit = false;
    Damage_t maxDamage = 0;

    GCSkillToObjectOK2 gcSkillToObjectOK2;
    GCSkillToObjectOK4 gcSkillToObjectOK4;

    Level_t maxEnemyLevel = 0;
    uint EnemyNum = 0;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            int X = m_X + x;
            int Y = m_Y + y;

            if (!rect.ptInRect(X, Y))
                continue;

            
            Tile& tile = m_pZone->getTile(X, Y);
            const forward_list<Object*>& oList = tile.getObjectList();
            forward_list<Object*>::const_iterator itr = oList.begin();

            int DamageModifier = GSGDamageModify[x + 2][y + 2];
            Damage_t Damage = getPercentValue(m_Damage, DamageModifier);

            for (; itr != oList.end(); itr++) {
                Object* pObject = *itr;
                Assert(pObject != NULL);

                if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE) {
                    Creature* pCreature = dynamic_cast<Creature*>(pObject);
                    Assert(pCreature != NULL);

                    
                    if (pCreature == m_pTarget || !canAttack(pCastCreature, pCreature) ||
                        pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
                        continue;
                    }

                    bool bPK = verifyPK(pCastCreature, pCreature);
                    bool bZoneLevelCheck = checkZoneLevelToHitTarget(pCreature);

                    int bonus = 100;
                    if (pCastCreature->isSlayer()) {
                        Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);
                        Assert(pSlayer != NULL);

                        SkillSlot* pSkillSlot = pSlayer->hasSkill(SKILL_GUN_SHOT_GUIDANCE);
                        if (pSkillSlot != NULL)
                            bonus = pSkillSlot->getExpLevel();
                    }

                    if (pCastCreature->isMonster()) {
                        Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
                        Assert(pMonster != NULL);
                        bPK = pMonster->isEnemyToAttack(pCreature);
                    }

                    bool bHitRoll = HitRoll::isSuccess(pCastCreature, pCreature, bonus);

                    if (bPK && bZoneLevelCheck && bHitRoll) {
                        
                        Damage_t FinalDamage = 0;
                        FinalDamage = computeDamage(pCastCreature, pCreature);
                        FinalDamage += Damage;

                        if (pCreature->isPC() && pCreature->getCreatureClass() != pCastCreature->getCreatureClass()) {
                            //							Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

                            GCModifyInformation gcMI;
                            ::setDamage(pCreature, FinalDamage, pCastCreature, SKILL_GUN_SHOT_GUIDANCE,
                                        &gcMI); 

                            pCreature->getPlayer()->sendPacket(&gcMI);

                            
                            gcSkillToObjectOK2.setObjectID(1); 
                            gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
                            gcSkillToObjectOK2.setDuration(0);
                            pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);

                            bHit = true;
                        } else if (pCreature->isMonster()) {
                            Monster* pMonster = dynamic_cast<Monster*>(pCreature);

                            ::setDamage(pMonster, FinalDamage, pCastCreature,
                                        SKILL_GUN_SHOT_GUIDANCE); 

                            pMonster->addEnemy(pCastCreature);
                            bHit = true;
                        } else
                            continue;

                        gcSkillToObjectOK4.setTargetObjectID(pCreature->getObjectID());
                        gcSkillToObjectOK4.setSkillType(SKILL_ATTACK_MELEE);
                        gcSkillToObjectOK4.setDuration(0);
                        m_pZone->broadcastPacket(X, Y, &gcSkillToObjectOK4, pCreature);

                        if (maxEnemyLevel < pCreature->getLevel())
                            maxEnemyLevel = pCreature->getLevel();
                        EnemyNum++;

                        if (FinalDamage > maxDamage)
                            maxDamage = FinalDamage;
                    }
                }
            }
        }
    }

    if (bHit && pCastCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);
        Assert(pSlayer != NULL);

        bool bIncreaseExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);
        if (bIncreaseExp) {
            SkillSlot* pSkillSlot = pSlayer->hasSkill(SKILL_GUN_SHOT_GUIDANCE);
            SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SKILL_GUN_SHOT_GUIDANCE);
            SkillDomainType_t DomainType = pSkillInfo->getDomainType();
            // SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

            GCModifyInformation gcMI;
            shareAttrExp(pSlayer, maxDamage, 1, 8, 1, gcMI);
            increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), gcMI, maxEnemyLevel, EnemyNum);
            increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, gcMI);

            pSlayer->getPlayer()->sendPacket(&gcMI);
        }
    }

    
    GCAddEffectToTile gcAddEffectToTile;
    gcAddEffectToTile.setObjectID(pCastCreature->getObjectID());
    gcAddEffectToTile.setEffectID(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_FIRE);
    gcAddEffectToTile.setXY(m_X, m_Y);
    gcAddEffectToTile.setDuration(10); 

    m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffectToTile);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGunShotGuidanceAim::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget); // by Sequoia

    Assert(pCreature != NULL);

    if (m_pZone != NULL && m_pZone == pCreature->getZone()) {
        unaffect(pCreature);
    } else {
        
        
        Zone* pZone = pCreature->getZone();
        Assert(pZone != NULL);

        pCreature->removeFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM);

        GCRemoveEffect gcRemoveEffect;
        gcRemoveEffect.setObjectID(pCreature->getObjectID());
        gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM);

        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectGunShotGuidanceAim::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectGunShotGuidanceAim("
        << "Zone:" << g_pZoneInfoManager->getZoneInfo(m_pZone->getZoneID())->getFullName() << ",X:" << (int)m_X
        << ",Y:" << (int)m_Y << ",Damage:" << (int)m_Damage << ")";
    return msg.toString();

    __END_CATCH
}
