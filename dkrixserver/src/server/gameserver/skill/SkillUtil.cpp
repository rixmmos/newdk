//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillUtil.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SkillUtil.h"

#include "Monster.h"
#include "Player.h"
#include "SkillInfo.h"
// #include "AttrBalanceInfo.h"
#include <math.h>
#include <stdio.h>

#include "AlignmentManager.h"
#include "CreatureUtil.h"
#include "DynamicZone.h"
#include "EffectAirShield.h"
#include "EffectAlignmentRecovery.h"
#include "EffectArmageddon.h"
#include "EffectAuraShield.h"
#include "EffectBlindness.h"
#include "EffectBlockHead.h"
#include "EffectCanEnterGDRLair.h"
#include "EffectDivineSpirits.h"
#include "EffectEnemyErase.h"
#include "EffectExpansion.h"
#include "EffectExplosionWater.h"
#include "EffectFrozenArmor.h"
#include "EffectGrandMasterOusters.h"
#include "EffectGrandMasterSlayer.h"
#include "EffectGrandMasterVampire.h"
#include "EffectHandsOfFire.h"
#include "EffectHymn.h"
#include "EffectIceFieldToCreature.h"
#include "EffectIceOfSoulStone.h"
#include "EffectInstallTurret.h"
#include "EffectMephisto.h"
#include "EffectPrecedence.h"
#include "EffectReactiveArmor.h"
#include "EffectRediance.h"
#include "EffectRequital.h"
#include "EffectRevealer.h"
#include "EffectShareHP.h"
#include "EffectSharpShield.h"
#include "EffectSleep.h"
#include "EffectStoneSkin.h"
#include "EffectStriking.h"
#include "EffectSwordOfThor.h"
#include "EffectWaterBarrier.h"
#include "EventHeadCount.h"
#include "EventItemUtil.h"
#include "GCAddEffect.h"
#include "GCAddEffectToTile.h"
#include "GCAddInjuriousCreature.h"
#include "GCKickMessage.h"
#include "GCLearnSkillReady.h"
#include "GCOtherModifyInfo.h"
#include "GCRemoveEffect.h"
#include "GCRemoveFromGear.h"
#include "GCSkillFailed1.h"
#include "GCSkillFailed2.h"
#include "GCSkillToObjectOK4.h"
#include "GCSkillToObjectOK6.h"
#include "GCStatusCurrentHP.h"
#include "GCSystemMessage.h"
#include "GDRLairManager.h"
#include "GQuestManager.h"
#include "GamePlayer.h"
#include "GameServerInfoManager.h"
#include "HitRoll.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "LogClient.h"
#include "MasterLairInfoManager.h"
#include "OustersEXPInfo.h"
#include "PKZoneInfoManager.h"
#include "PacketUtil.h"
#include "Party.h"
#include "PrecedenceTable.h"
#include "Properties.h"
#include "SkillDomainInfoManager.h"
#include "SkillPropertyManager.h"
#include "SummonGroundElemental.h"
#include "Thread.h"
#include "VampEXPInfo.h"
#include "VariableManager.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "billing/BillingInfo.h"
#include "mission/EventQuestLootingManager.h"
#include "mission/MonsterKillQuestStatus.h"
#include "mission/QuestManager.h"

//////////////////////////////////////////////////////////////////////////////
// data structure & helper functions
//////////////////////////////////////////////////////////////////////////////
typedef struct DomainStruct {
    int DomainType;
    int DomainLevel;
} SDomain;

class isBig {
public:
    isBig() {};

    bool operator()(const DomainStruct& t, const DomainStruct& b) {
        if (t.DomainLevel > b.DomainLevel)
            return true;
        else
            return false;
    }
};

void checkFreeLevelLimit(PlayerCreature* pPC)

{
    __BEGIN_TRY

    

    // by sigi. 2002.11.19
    
    
    
#if defined(__PAY_SYSTEM_FREE_LIMIT__)

    if (!pPC->isPayPlayAvaiable()) {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());

        pGamePlayer->kickPlayer(30, KICK_MESSAGE_EXPIRE_FREEPLAY);

        // pGamePlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
         
    }
#endif

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// const uint g_pVariableManager->getPremiumExpBonusPercent() = 150;


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeDamage(Creature* pCreature, Creature* pTargetCreature) {
    Assert(pCreature != NULL);
    Assert(pTargetCreature != NULL);

    Damage_t Damage = 0;
    bool bCriticalHit = false;

    try {
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            Assert(pSlayer != NULL);
            Damage = computeSlayerDamage(pSlayer, pTargetCreature, bCriticalHit);
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            Assert(pVampire != NULL);
            Damage = computeVampireDamage(pVampire, pTargetCreature, bCriticalHit);
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            Assert(pOusters != NULL);
            Damage = computeOustersDamage(pOusters, pTargetCreature, bCriticalHit);
        } else if (pCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pCreature);
            Assert(pMonster != NULL);
            Damage = computeMonsterDamage(pMonster, pTargetCreature, bCriticalHit);
        } else {
            
            return 0;
        }
    } catch (Throwable& t) {
        cerr << t.toString() << endl;
    }

    return Damage;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computePureDamage(Creature* pCreature) {
    Damage_t Damage = 0;

    if (pCreature == NULL)
        return Damage;

    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        Assert(pSlayer != NULL);
        Damage = computePureSlayerDamage(pSlayer);
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        Assert(pVampire != NULL);
        Damage = computePureVampireDamage(pVampire);
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
        Assert(pOusters != NULL);
        Damage = computePureOustersDamage(pOusters);
    } else if (pCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);
        Assert(pMonster != NULL);
        Damage = computePureMonsterDamage(pMonster);
    } else {
        
        return 0;
    }

    return Damage;
}

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
Damage_t computeDamage(Creature* pCreature, Creature* pTargetCreature, int CriticalBonus, bool& bCritical) {
    Assert(pCreature != NULL);
    Assert(pTargetCreature != NULL);

    Damage_t Damage = 0;
    bool bCriticalHit = HitRoll::isCriticalHit(pCreature, CriticalBonus);

    try {
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            Assert(pSlayer != NULL);

            Damage = computeSlayerDamage(pSlayer, pTargetCreature, bCriticalHit);
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            Assert(pVampire != NULL);
            Damage = computeVampireDamage(pVampire, pTargetCreature, bCriticalHit);
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            Assert(pOusters != NULL);
            Damage = computeOustersDamage(pOusters, pTargetCreature, bCriticalHit);
        } else if (pCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pCreature);
            Assert(pMonster != NULL);
            Damage = computeMonsterDamage(pMonster, pTargetCreature, bCriticalHit);
        } else {
            // Attacker is none of the four damage-dealing kinds (an NPC). No
            // damage is computed, so report no critical: returning here without
            // writing the out-param left the caller's bool indeterminate.
            bCritical = false;
            return 0;
        }
    } catch (Throwable& t) {
        cerr << t.toString() << endl;
    }

    bCritical = bCriticalHit;

    
    if (bCritical && pTargetCreature->isMonster()) {
        Damage = getPercentValue(Damage, 150);
    }

    return Damage;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
double computeFinalDamage(Damage_t minDamage, Damage_t maxDamage, Damage_t realDamage, Protection_t Protection,
                          bool bCritical) {
    
    if (bCritical)
        return realDamage;

    if (Protection > 640)
        Protection = 640;

    Damage_t FinalDamage;
    FinalDamage = realDamage - (realDamage * (Protection / 8)) / 100;
    // FinalDamage = realDamage - ( realDamage * ( Protection / 10 ) ) / 100;

    return max(1, (int)FinalDamage);
    /*
        Damage_t avgDamage   = (minDamage + maxDamage) >> 1;
        int      DamageRatio = 100;

        if (Protection < avgDamage)
        {
            DamageRatio = 100;
        }
        else if (Protection < getPercentValue(avgDamage, 150))
        {
            DamageRatio = 90;
        }
        else if (Protection < getPercentValue(avgDamage, 200))
        {
            DamageRatio = 80;
        }
        else if (Protection < getPercentValue(avgDamage, 250))
        {
            DamageRatio = 70;
        }
        else if (Protection < getPercentValue(avgDamage, 300))
        {
            DamageRatio = 60;
        }
        else
        {
            DamageRatio = 50;
        }

        return max(1, getPercentValue(realDamage, DamageRatio));
    */
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeSlayerDamage(Slayer* pSlayer, Creature* pTargetCreature, bool bCritical) {
    Assert(pSlayer != NULL);
    Assert(pTargetCreature != NULL);

    Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
    uint timeband = getZoneTimeband(pSlayer->getZone());
    double FinalDamage = 0;

    
    Damage_t MinDamage = pSlayer->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pSlayer->getDamage(ATTR_MAX);

    
    if (pItem != NULL && pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND)) {
        
        
        
        // MinDamage += (pItem->getMinDamage() + pItem->getBonusDamage());
        // MaxDamage += (pItem->getMaxDamage() + pItem->getBonusDamage());
        MinDamage += pItem->getMinDamage();
        MaxDamage += pItem->getMaxDamage();
    }

    
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));

    if (pTargetCreature->isSlayer()) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
        Assert(pTargetSlayer != NULL);

        Protection_t Protection = pTargetSlayer->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isVampire()) {
        Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
        Assert(pTargetVampire != NULL);

        Protection_t Protection = pTargetVampire->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, VampireTimebandFactor[timeband]);

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isOusters()) {
        Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetCreature);
        Assert(pTargetOusters != NULL);

        Protection_t Protection = pTargetOusters->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isMonster()) {
        Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
        Assert(pTargetMonster != NULL);

        Protection_t Protection = pTargetMonster->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, MonsterTimebandFactor[timeband]);

#ifdef __UNDERWORLD__
        if (pTargetMonster->isUnderworld() || pTargetMonster->getMonsterType() == 599) {
            Protection = pTargetMonster->getProtection();
        }
#endif

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else {
        
        return 0;
    }

    
    // FinalDamage += g_pVariableManager->getCombatSlayerDamageBonus();

    return (Damage_t)FinalDamage;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeVampireDamage(Vampire* pVampire, Creature* pTargetCreature, bool bCritical) {
    Assert(pVampire != NULL);
    Assert(pTargetCreature != NULL);

    double FinalDamage = 0;
    Damage_t MinDamage = pVampire->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pVampire->getDamage(ATTR_MAX);
    uint timeband = getZoneTimeband(pVampire->getZone());

    
    Item* pItem = pVampire->getWearItem(Vampire::WEAR_RIGHTHAND);

    
    if (pItem != NULL && pVampire->isRealWearingEx(Vampire::WEAR_RIGHTHAND)) {
        MinDamage += pItem->getMinDamage();
        MaxDamage += pItem->getMaxDamage();
    }

    
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));

    RealDamage = (Damage_t)getPercentValue(RealDamage, VampireTimebandFactor[timeband]);

    if (pTargetCreature->isSlayer()) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
        Assert(pTargetSlayer != NULL);

        Protection_t Protection = pTargetSlayer->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isVampire()) {
        Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
        Assert(pTargetVampire != NULL);

        Protection_t Protection = pTargetVampire->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, VampireTimebandFactor[timeband]);

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isOusters()) {
        Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetCreature);
        Assert(pTargetOusters != NULL);

        Protection_t Protection = pTargetOusters->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isMonster()) {
        Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
        Assert(pTargetMonster != NULL);

        Protection_t Protection = pTargetMonster->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, MonsterTimebandFactor[timeband]);

#ifdef __UNDERWORLD__
        if (pTargetMonster->isUnderworld() || pTargetMonster->getMonsterType() == 599) {
            Protection = pTargetMonster->getProtection();
        }
#endif

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else {
        
        return 0;
    }

    
    // FinalDamage += g_pVariableManager->getCombatVampireDamageBonus();

    return (Damage_t)FinalDamage;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeOustersDamage(Ousters* pOusters, Creature* pTargetCreature, bool bCritical) {
    Assert(pOusters != NULL);
    Assert(pTargetCreature != NULL);

    double FinalDamage = 0;
    Damage_t MinDamage = pOusters->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pOusters->getDamage(ATTR_MAX);
    uint timeband = getZoneTimeband(pOusters->getZone());

    
    Item* pItem = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);

    
    if (pItem != NULL && pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND)) {
        MinDamage += pItem->getMinDamage();
        MaxDamage += pItem->getMaxDamage();
    }

    
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));

    if (pTargetCreature->isSlayer()) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
        Assert(pTargetSlayer != NULL);

        Protection_t Protection = pTargetSlayer->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isVampire()) {
        Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
        Assert(pTargetVampire != NULL);

        Protection_t Protection = pTargetVampire->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, VampireTimebandFactor[timeband]);

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isOusters()) {
        Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetCreature);
        Assert(pTargetOusters != NULL);

        Protection_t Protection = pTargetOusters->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isMonster()) {
        Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
        Assert(pTargetMonster != NULL);

        Protection_t Protection = pTargetMonster->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, MonsterTimebandFactor[timeband]);

#ifdef __UNDERWORLD__
        if (pTargetMonster->isUnderworld() || pTargetMonster->getMonsterType() == 599) {
            Protection = pTargetMonster->getProtection();
        }
#endif

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else {
        
        return 0;
    }

    
    // FinalDamage += g_pVariableManager->getCombatOustersDamageBonus();

    return (Damage_t)FinalDamage;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeMonsterDamage(Monster* pMonster, Creature* pTargetCreature, bool bCritical) {
    Assert(pMonster != NULL);
    Assert(pTargetCreature != NULL);

    double FinalDamage = 0;
    Damage_t MinDamage = pMonster->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pMonster->getDamage(ATTR_MAX);
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));
    uint timeband = getZoneTimeband(pMonster->getZone());

    RealDamage = (Damage_t)getPercentValue(RealDamage, MonsterTimebandFactor[timeband]);

    if (pTargetCreature->isSlayer()) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
        Assert(pTargetSlayer != NULL);

        Protection_t Protection = pTargetSlayer->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isVampire()) {
        Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
        Assert(pTargetVampire != NULL);

        Protection_t Protection = pTargetVampire->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, VampireTimebandFactor[timeband]);

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isOusters()) {
        Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetCreature);
        Assert(pTargetOusters != NULL);

        Protection_t Protection = pTargetOusters->getProtection();

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else if (pTargetCreature->isMonster()) {
        Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
        Assert(pTargetMonster != NULL);

        Protection_t Protection = pTargetMonster->getProtection();
        Protection = (Protection_t)getPercentValue(Protection, MonsterTimebandFactor[timeband]);

#ifdef __UNDERWORLD__
        if (pTargetMonster->isUnderworld() || pTargetMonster->getMonsterType() == 599) {
            Protection = pTargetMonster->getProtection();
        }
#endif

        FinalDamage = computeFinalDamage(MinDamage, MaxDamage, RealDamage, Protection, bCritical);
    } else {
        
        return 0;
    }

    return (Damage_t)FinalDamage;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeMagicDamage(Creature* pTargetCreature, int Damage, SkillType_t SkillType, bool bVampire,
                            Creature* pAttacker) {
    Assert(pTargetCreature != NULL);

    SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
    Assert(pSkillInfo != NULL);

    int MagicDomain = pSkillInfo->getMagicDomain();
    int MagicLevel = pSkillInfo->getLevel();

    int Resist = pTargetCreature->getResist(MagicDomain);

    if (pAttacker != NULL && pAttacker->isVampire()) {
        //		cout << "before mastery " << Resist << endl;
        Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);
        if (pVampire != NULL) {
            if (pSkillInfo->getMagicDomain() >= MAGIC_DOMAIN_POISON &&
                pSkillInfo->getMagicDomain() <= MAGIC_DOMAIN_BLOOD) {
                int mastery = pVampire->getMastery(pSkillInfo->getMagicDomain());
                Resist = getPercentValue(Resist, 100 - mastery);
            }
        }
        //		cout << "after mastery " << Resist << endl;
    }

#ifdef __CHINA_SERVER__
    if (bVampire)
        Resist = (int)(Resist / 1.2);

    
    int penalty = (int)(MagicLevel / 5.0 - Resist);
    penalty = min(penalty, 100);
    penalty = max(penalty, -100);

    Damage = getPercentValue(Damage, 100 + penalty);
    return (Damage_t)max(0, (int)Damage);
#else
    float penalty = 1.5 * (Resist - (MagicLevel / 5.0)) / (Resist - (MagicLevel / 5.0) + 100.0);
    penalty = max(penalty, -0.2f);
    Damage = (int)(Damage * (1.0 - penalty));

    return (Damage_t)max(0, (int)Damage);
#endif
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeOustersMagicDamage(Ousters* pOusters, Creature* pTargetCreature, int Damage, SkillType_t SkillType) {
    Assert(pOusters != NULL);

    Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
    if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET)
        return 0;

    Damage_t MinDamage = pWeapon->getMinDamage();
    Damage_t MaxDamage = pWeapon->getMaxDamage();

    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));
    return RealDamage + Damage;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computeSlayerSilverDamage(Creature* pCreature, int Damage, ModifyInfo* pMI) {
    Assert(pCreature != NULL);

    
    if (pCreature->isSlayer() == false)
        return 0;

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
    Assert(pSlayer != NULL);

    
    Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
    if (pWeapon == NULL)
        return 0;

    Damage_t silverDamage = 0;

    if (isMeleeWeapon(pWeapon)) {
        
        if (isClericWeapon(pWeapon)) {
            silverDamage = max(1, (int)(Damage * 0.1));
            silverDamage = min((int)silverDamage, (int)pWeapon->getSilver());

            if (silverDamage > 0) {
                pWeapon->setSilver(pWeapon->getSilver() - silverDamage);
                // pWeapon->save(pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_RIGHTHAND, 0);

                if (pMI != NULL)
                    pMI->addShortData(MODIFY_SILVER_DURABILITY, pWeapon->getSilver());
            }

            
            silverDamage += max(1, (int)(Damage * 0.1));
        }
        
        else {
            silverDamage = max(1, (int)(Damage * 0.1));
            silverDamage = min((int)silverDamage, (int)pWeapon->getSilver());

            if (silverDamage > 0) {
                pWeapon->setSilver(pWeapon->getSilver() - silverDamage);
                // pWeapon->save(pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_RIGHTHAND, 0);

                if (pMI != NULL)
                    pMI->addShortData(MODIFY_SILVER_DURABILITY, pWeapon->getSilver());
            }
        }
    } else if (isArmsWeapon(pWeapon) && pWeapon->getSilver() > 0) {
        
        
        
        silverDamage = max(1, (int)(Damage * 0.1));
    }

    return silverDamage;
}

void computeCriticalBonus(Ousters* pOusters, SkillType_t skillType, Damage_t& Damage, bool& bCriticalHit) {
    switch (skillType) {
    case SKILL_KASAS_ARROW:
    case SKILL_BLAZE_BOLT:
    case SKILL_EARTHS_TEETH:
    case SKILL_HANDS_OF_NIZIE:
    case SKILL_STONE_AUGER:
    case SKILL_EMISSION_WATER:
    case SKILL_BEAT_HEAD:
    case SKILL_MAGNUM_SPEAR:
    case SKILL_FIRE_PIERCING:
    case SKILL_SUMMON_FIRE_ELEMENTAL:
    case SKILL_ICE_LANCE:
    case SKILL_EXPLOSION_WATER:
    case SKILL_METEOR_STORM: {
        OustersSkillSlot* pSkillSlot = pOusters->hasSkill(SKILL_CRITICAL_MAGIC);
        if (pSkillSlot == NULL)
            return;

        SkillLevel_t level = pSkillSlot->getExpLevel();

        int Ratio = 5 + (level / 3);
        if ((rand() % 100) < Ratio) {
            bCriticalHit = true;
            if (level <= 15) {
                Damage += (Damage_t)(Damage * level / 19.5);
            } else {
                int bonus = (Damage_t)(Damage * (0.1 + (level / 75.0)));
                if (level == 30)
                    bonus = (Damage_t)(bonus * 1.1);

                Damage += bonus;
            }
        }
    }

    default:
        return;
    }
}

RankExp_t computeRankExp(int myLevel, int otherLevel) // by sigi. 2002.12.31
{
    RankExp_t rankExp = 0;

    if (myLevel != 0 && otherLevel != 0) {
        int checkValue = otherLevel * 100 / myLevel;

        if (checkValue > 120) {
            rankExp = (otherLevel - myLevel) * (otherLevel - myLevel) / 10 + otherLevel * 100 / myLevel;
        } else {
            rankExp = otherLevel * 100 / myLevel - 10;
        }

        rankExp = max(0, (int)rankExp);

        // by sigi. 2002.12.31
        rankExp = getPercentValue(rankExp, g_pVariableManager->getVariable(RANK_EXP_GAIN_PERCENT));
    }

    rankExp = getPercentValue(rankExp, g_pVariableManager->getPremiumExpBonusPercent());

    return rankExp;
}

//////////////////////////////////////////////////////////////////////////////

//

// by sigi. 2002.8.31
//////////////////////////////////////////////////////////////////////////////
void affectKillCount(Creature* pAttacker, Creature* pDeadCreature) {
    
    
    
    
    
    if (pAttacker == NULL || pDeadCreature == NULL || !pAttacker->isPC() || pDeadCreature->isAlive()) {
        return;
    }

#ifdef __UNDERWORLD__
    if (pDeadCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pDeadCreature);
        Assert(pMonster != NULL);

        if (pMonster->isUnderworld()) {
            pMonster->setUnderworld(false);
            giveUnderworldGift(pAttacker);
        }
    }
#endif

    if (pDeadCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pDeadCreature);

        if (pMonster->getLastKiller() == 0)
            pMonster->setLastKiller(pAttacker->getObjectID());
    }

    int myLevel = 0;
    int otherLevel = 0;
    // int bonusPercent = 100;

    if (pAttacker->isSlayer()) {
        
        if (pDeadCreature->isSlayer())
            return;

        Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);
        myLevel = pSlayer->getHighestSkillDomainLevel();

        
        if (!pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND))
            return;

        
        if (pDeadCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pDeadCreature);
            otherLevel = pVampire->getLevel();
            // bonusPercent = 150;
        }
        
        else if (pDeadCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pDeadCreature);
            otherLevel = pOusters->getLevel();
            // bonusPercent = 150;
        }
        
        else if (pDeadCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pDeadCreature);

            
            if (pMonster->isMaster()) {
                
                pSlayer->increaseRankExp(MASTER_KILL_RANK_EXP);
                return;
            }

            otherLevel = pMonster->getLevel();
        } else
            return;
    } else if (pAttacker->isVampire()) {
        
        if (pDeadCreature->isVampire())
            return;

        Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);
        myLevel = pVampire->getLevel();

        
        if (pDeadCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pDeadCreature);
            otherLevel = pSlayer->getHighestSkillDomainLevel();
            // bonusPercent = 150;
        }
        
        else if (pDeadCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pDeadCreature);
            otherLevel = pOusters->getLevel();
            // bonusPercent = 150;
        }
        
        else if (pDeadCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pDeadCreature);

            
            if (pMonster->isMaster()) {
                
                pVampire->increaseRankExp(MASTER_KILL_RANK_EXP);
                return;
            }

            otherLevel = pMonster->getLevel();
        } else
            return;
    } else if (pAttacker->isOusters()) {
        
        if (pDeadCreature->isOusters())
            return;

        Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);
        myLevel = pOusters->getLevel();

        
        if (pDeadCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pDeadCreature);
            otherLevel = pSlayer->getHighestSkillDomainLevel();
            // bonusPercent = 150;
        }
        
        if (pDeadCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pDeadCreature);
            otherLevel = pVampire->getLevel();
            // bonusPercent = 150;
        }
        
        else if (pDeadCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pDeadCreature);

            
            if (pMonster->isMaster()) {
                
                pOusters->increaseRankExp(MASTER_KILL_RANK_EXP);
                return;
            }

            otherLevel = pMonster->getLevel();
        } else
            return;
    } else
        return;

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pAttacker);

    if (pDeadCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pDeadCreature);
        PrecedenceTable* pTable = pMonster->getPrecedenceTable();

        //		if ( pMonster->getLastKiller() == 0 )
        //			pMonster->setLastKiller( pAttacker->getObjectID() );

        if (pMonster->getMonsterType() == 722) {
            if (pPC->getPartyID() == 0) {
                if (!pPC->isFlag(Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR)) {
                    EffectCanEnterGDRLair* pEffect = new EffectCanEnterGDRLair(pPC);
                    pEffect->setDeadline(216000);

                    pPC->setFlag(pEffect->getEffectClass());
                    pPC->addEffect(pEffect);

                    pEffect->create(pPC->getName());

                    GCAddEffect gcAddEffect;
                    gcAddEffect.setObjectID(pPC->getObjectID());
                    gcAddEffect.setEffectID(pEffect->getSendEffectClass());
                    gcAddEffect.setDuration(21600);

                    pPC->getZone()->broadcastPacket(pPC->getX(), pPC->getY(), &gcAddEffect);
                }
                //					addSimpleCreatureEffect( pPC, Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR, 216000 );
            } else {
                LocalPartyManager* pLPM = pPC->getLocalPartyManager();
                Assert(pLPM != NULL);
                pLPM->shareGDRLairEnter(pPC->getPartyID(), pPC);
            }

            Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_ITEM, 29, list<OptionType_t>());
            pMonster->setQuestItem(pItem);
        }

        if (pTable != NULL && pTable->canGainRankExp(pPC)) {
            QuestManager* pQM = pPC->getQuestManager();

            if (pQM != NULL && pQM->killedMonster(pMonster)) {
                pPC->sendCurrentQuestInfo();
            }

            if (g_pEventQuestLootingManager->killed(pPC, pMonster))
                pTable->setQuestHostName(pPC->getName());

            if (pMonster->getQuestItem() == NULL && g_pVariableManager->getVariable(EVENT_NEW_YEAR_2005) != 0) {
                Item* pItem = getNewYear2005Item(getNewYear2005ItemKind(pPC, pMonster));
                pMonster->setQuestItem(pItem);
                if (pItem != NULL)
                    logEventItemCount(pItem);
            }

            if (pMonster->getQuestItem() == NULL && g_pVariableManager->isEventMoonCard()) {
                Item* pItem = getCardItem(getCardKind(pPC, pMonster));
                //				setItemGender( pItem, (pPC->getSex()==FEMALE)?GENDER_FEMALE:GENDER_MALE );
                pMonster->setQuestItem(pItem);
            }

            if (pMonster->getQuestItem() == NULL && g_pVariableManager->isEventLuckyBag()) {
                Item* pItem = getLuckyBagItem(getLuckyBagKind(pPC, pMonster));
                pMonster->setQuestItem(pItem);
            }

            if (pMonster->getQuestItem() == NULL && g_pVariableManager->getVariable(NICKNAME_PEN_EVENT) != 0 &&
                canGiveEventItem(pPC, pMonster)) {
                int value = rand() % 100000;
                if (value < g_pVariableManager->getVariable(NICKNAME_PEN_RATIO)) {
                    Item* pItem =
                        g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_GIFT_BOX, 22, list<OptionType_t>());
                    pMonster->setQuestItem(pItem);
                }
            }

            if (pMonster->getQuestItem() == NULL && g_pVariableManager->getVariable(CLOVER_EVENT) != 0 &&
                canGiveEventItem(pPC, pMonster)) {
                int value = rand() % 100000;
                if (value < g_pVariableManager->getVariable(CLOVER_RATIO)) {
                    Item* pItem =
                        g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MOON_CARD, 3, list<OptionType_t>());
                    pMonster->setQuestItem(pItem);
                }
            }

            if (pMonster->getQuestItem() == NULL && g_pVariableManager->getVariable(PINE_CAKE_EVENT) != 0 &&
                canGiveEventItem(pPC, pMonster)) {
                int value = rand() % 100000;
                if (value < g_pVariableManager->getVariable(PINE_CAKE_RATIO)) {
                    int value = rand() % 10;
                    int add = 0;
                    if (value < 6)
                        add = 0;
                    else if (value < 9)
                        add = 1;
                    else
                        add = 2;
                    Item* pItem =
                        g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_ETC, 15 + add, list<OptionType_t>());
                    pMonster->setQuestItem(pItem);
                }
            }

            if (g_pVariableManager->getVariable(NETMARBLE_CARD_EVENT) != 0 && pMonster->getQuestItem() == NULL &&
                g_pVariableManager->getVariable(NETMARBLE_CARD_RATIO) > (rand() % 100000) &&
                canGiveEventItem(pPC, pMonster)) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MOON_CARD, 2, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }

            if (g_pVariableManager->isHeadCount()) {
                GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pAttacker->getPlayer());
                if (pGamePlayer != NULL) {
                    EventHeadCount* pEvent =
                        dynamic_cast<EventHeadCount*>(pGamePlayer->getEvent(Event::EVENT_CLASS_HEAD_COUNT));
                    if (pEvent != NULL)
                        pEvent->cutHead();
                }
            }

            if (canGiveEventItem(pPC, pMonster) &&
                rand() % 100000 < g_pVariableManager->getVariable(GOLD_MEDAL_RATIO)) {
                giveGoldMedal(pPC);
            }

            if (pPC->getLevel() - 20 <= pMonster->getLevel() && pPC->getLevel() >= 30 &&
                !GDRLairManager::Instance().isGDRLairZone(pPC->getZoneID()) && !pMonster->isChief() &&
                !pMonster->isMaster()) {
                addOlympicStat(pPC, 1);
            }

            if (pMonster->isChief()) {
                if (canGiveEventItem(pPC, pMonster) && !GDRLairManager::Instance().isGDRLairZone(pPC->getZoneID()))
                    addOlympicStat(pPC, 3);
            }

            if (pMonster->isMaster()) {
                if (canGiveEventItem(pPC, pMonster))
                    addOlympicStat(pPC, 3);
            }

            if (pMonster->getQuestItem() == NULL && canGiveEventItem(pPC, pMonster) &&
                rand() % 100000 < g_pVariableManager->getVariable(OLYMPIC_ITEM_RATIO)) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MOON_CARD, 4, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }

            if (pMonster->getQuestItem() == NULL && canGiveEventItem(pPC, pMonster) &&
                rand() % 100000 < g_pVariableManager->getVariable(LUCK_CHARM_RATIO)) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_ITEM, 30, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }

            if (pMonster->getQuestItem() == NULL && pMonster->getMonsterType() >= 769 &&
                canGiveEventItem(pPC, pMonster) &&
                rand() % 100000 < g_pVariableManager->getVariable(HOURGLASS_RATIO_S)) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EFFECT_ITEM, 6, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }

            if (pMonster->getQuestItem() == NULL && pMonster->getMonsterType() >= 769 &&
                canGiveEventItem(pPC, pMonster) &&
                rand() % 100000 < g_pVariableManager->getVariable(HOURGLASS_RATIO_M)) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EFFECT_ITEM, 5, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }

            if (pMonster->getQuestItem() == NULL && pMonster->getMonsterType() >= 769 &&
                canGiveEventItem(pPC, pMonster) &&
                rand() % 100000 < g_pVariableManager->getVariable(HOURGLASS_RATIO_L)) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EFFECT_ITEM, 4, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }

            if (pMonster->getZone()->isDynamicZone() &&
                pMonster->getZone()->getDynamicZone()->getTemplateZoneID() == 4002 &&
                pMonster->getQuestItem() == NULL && (rand() % 20) == 0) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_ITEM, 31, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }

            if (g_pVariableManager->getVariable(PET_FOOD_EVENT) != 0 && pMonster->getQuestItem() == NULL &&
                g_pVariableManager->getVariable(PET_FOOD_RATIO) > rand() % 100000 && canGiveEventItem(pPC, pMonster)) {
                bool isHigher = g_pVariableManager->getVariable(HIGHER_PET_FOOD_RATIO) > rand() % 100;

                int itemClassSeed = rand() % 100;
                int RacePetFoodRatio = g_pVariableManager->getVariable(RACE_PET_FOOD_RATIO);
                int RevivalSetRatio = g_pVariableManager->getVariable(REVIVAL_SET_RATIO);

                Item::ItemClass iClass = Item::ITEM_CLASS_PET_FOOD;
                ItemType_t itemType = 1;

                if (itemClassSeed < RevivalSetRatio) {
                    iClass = Item::ITEM_CLASS_PET_ENCHANT_ITEM;
                    itemType = 13;
                }
                //				bool isRace = g_pVariableManager->getVariable(RACE_PET_FOOD_RATIO) > rand()%100;
                else if (itemClassSeed - RevivalSetRatio < RacePetFoodRatio) {
                    switch (pPC->getCreatureClass()) {
                    case Creature::CREATURE_CLASS_SLAYER:
                        itemType = 6;
                        break;

                    case Creature::CREATURE_CLASS_VAMPIRE:
                        itemType = 10;
                        break;

                    case Creature::CREATURE_CLASS_OUSTERS:
                        itemType = 14;
                        break;

                    default:
                        break;
                    }
                }

                if (iClass == Item::ITEM_CLASS_PET_FOOD && isHigher)
                    ++itemType;
                

                Item* pItem = g_pItemFactoryManager->createItem(iClass, itemType, list<OptionType_t>());
                pMonster->setQuestItem(pItem);
            }
        }
    }

    int PartyID = pPC->getPartyID();
    if (PartyID != 0) {
        
        
        LocalPartyManager* pLPM = pPC->getLocalPartyManager();
        Assert(pLPM != NULL);
        pLPM->shareRankExp(PartyID, pAttacker, otherLevel);
        if (pPC->isAdvanced())
            pLPM->shareAdvancementExp(PartyID, pAttacker, computeCreatureExp(pDeadCreature, 1));
    } else {
        PlayerCreature* pAttackPC = dynamic_cast<PlayerCreature*>(pAttacker);
        if (pAttackPC->isAdvanced()) {
            pAttackPC->increaseAdvancementClassExp(computeCreatureExp(pDeadCreature, 1), true);
        }

        
        RankExp_t rankExp = computeRankExp(myLevel, otherLevel);

        if (pDeadCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pDeadCreature);
            PrecedenceTable* pTable = pMonster->getPrecedenceTable();

            if (pTable != NULL && pTable->canGainRankExp(pPC)) {
                pPC->increaseRankExp(rankExp);
            }
        } else {
            pPC->increaseRankExp(rankExp);
        }
    }
}

HP_t setCounterDamage(Creature* pAttacker, Creature* pTarget, Damage_t counterDamage, bool& bBroadcastAttackerHP,
                      bool& bSendAttackerHP) {
    HP_t Result2 = 0;
    
    // 2003.1.10 by bezz, Sequoia
    if (pAttacker != NULL && checkZoneLevelToHitTarget(pAttacker)) {
        if (pAttacker->isSlayer()) {
            Slayer* pSlayerAttacker = dynamic_cast<Slayer*>(pAttacker);
            Result2 = max(0, (int)pSlayerAttacker->getHP() - (int)counterDamage);
            pSlayerAttacker->setHP(Result2, ATTR_CURRENT);

            bBroadcastAttackerHP = true;
            bSendAttackerHP = true;
        } else if (pAttacker->isVampire()) {
            Vampire* pVampireAttacker = dynamic_cast<Vampire*>(pAttacker);
            Result2 = max(0, (int)pVampireAttacker->getHP() - (int)counterDamage);
            pVampireAttacker->setHP(Result2, ATTR_CURRENT);

            bBroadcastAttackerHP = true;
            bSendAttackerHP = true;

            
            if (pVampireAttacker->isFlag(Effect::EFFECT_CLASS_MEPHISTO)) {
                HP_t maxHP = pVampireAttacker->getHP(ATTR_MAX);

                
                if (Result2 * 3 < maxHP) {
                    Effect* pEffect = pVampireAttacker->findEffect(Effect::EFFECT_CLASS_MEPHISTO);
                    if (pEffect != NULL) {
                        pEffect->setDeadline(0);
                    } else {
                        pVampireAttacker->removeFlag(Effect::EFFECT_CLASS_MEPHISTO);
                    }
                    //					pVampireAttacker->getEffectManager()->deleteEffect(
                    // Effect::EFFECT_CLASS_MEPHISTO );
                }
            }
        } else if (pAttacker->isOusters()) {
            Ousters* pOustersAttacker = dynamic_cast<Ousters*>(pAttacker);
            Result2 = max(0, (int)pOustersAttacker->getHP() - (int)counterDamage);
            pOustersAttacker->setHP(Result2, ATTR_CURRENT);

            bBroadcastAttackerHP = true;
            bSendAttackerHP = true;
        } else if (pAttacker->isMonster()) {
            Monster* pMonsterAttacker = dynamic_cast<Monster*>(pAttacker);
            Result2 = max(0, (int)pMonsterAttacker->getHP() - (int)counterDamage);
            pMonsterAttacker->setHP(Result2, ATTR_CURRENT);
            pMonsterAttacker->setDamaged(true);

            
            pMonsterAttacker->addPrecedence(pTarget->getName(), pTarget->getPartyID(), counterDamage);
            pMonsterAttacker->setLastHitCreatureClass(pTarget->getCreatureClass());

            bBroadcastAttackerHP = true;
            if (pMonsterAttacker->getHP(ATTR_CURRENT) * 3 < pMonsterAttacker->getHP(ATTR_MAX)) {
                PrecedenceTable* pTable = pMonsterAttacker->getPrecedenceTable();

                
                
                
                
                if (pTable->getComputeFlag() == false) {
                    
                    pTable->compute();

                    
                    EffectPrecedence* pEffectPrecedence = new EffectPrecedence(pMonsterAttacker);
                    pEffectPrecedence->setDeadline(100);
                    pEffectPrecedence->setHostName(pTable->getHostName());
                    pEffectPrecedence->setHostPartyID(pTable->getHostPartyID());
                    pMonsterAttacker->setFlag(Effect::EFFECT_CLASS_PRECEDENCE);
                    pMonsterAttacker->addEffect(pEffectPrecedence);
                }
            }
        }
    }

    return Result2;
}

bool canBlockByWaterBarrier(SkillType_t SkillType) {
    switch (SkillType) {
    case SKILL_METEOR_STRIKE:
    case SKILL_POISON_STORM:
    case SKILL_GREEN_POISON:
    case SKILL_GREEN_STALKER:
    case SKILL_ACID_STORM:
    case SKILL_ACID_BOLT:
    case SKILL_ACID_STRIKE:
    case SKILL_ACID_SWAMP:
    case SKILL_BLOODY_KNIFE:
    case SKILL_BLOODY_WAVE:
    case SKILL_BLOODY_STRIKE:
    case SKILL_BLOODY_BALL:
    case SKILL_BLOODY_WALL:
    case SKILL_BLOODY_SPEAR:
    case SKILL_WIND_DIVIDER:
    case SKILL_SWORD_RAY:
    case SKILL_WIDE_LIGHTNING:
    case SKILL_EARTHQUAKE:
    case SKILL_POWER_OF_LAND:
    case SKILL_MULTI_AMPUTATE:
    case SKILL_WILD_TYPHOON:
    case SKILL_ATTACK_ARMS:
    case SKILL_DOUBLE_SHOT:
    case SKILL_TRIPLE_SHOT:
    case SKILL_MULTI_SHOT:
    case SKILL_HEAD_SHOT:
    case SKILL_QUICK_FIRE:
    case SKILL_ULTIMATE_BLOW:
    case SKILL_PIERCING:
    case SKILL_THROW_BOMB:
    case SKILL_BULLET_OF_LIGHT:
    case SKILL_GUN_SHOT_GUIDANCE:
    case SKILL_HOLY_ARROW:
    case SKILL_CAUSE_LIGHT_WOUNDS:
    case SKILL_CAUSE_SERIOUS_WOUNDS:
    case SKILL_CAUSE_CRITICAL_WOUNDS:
    case SKILL_VIGOR_DROP:
    case SKILL_TURN_UNDEAD:
    case SKILL_ILLENDUE:
    case SKILL_THROW_HOLY_WATER:
    case SKILL_LIGHT_BALL:
    case SKILL_AURA_BALL:
    case SKILL_AURA_RING:
    case SKILL_ENERGY_DROP:
    case SKILL_REBUKE:
    case SKILL_SPIRIT_GUARD:
    case SKILL_KASAS_ARROW:
    case SKILL_PROMINENCE:
    case SKILL_RING_OF_FLARE:
    case SKILL_BLAZE_BOLT:
    case SKILL_HANDS_OF_NIZIE:
    case SKILL_STONE_AUGER:
    case SKILL_EARTHS_TEETH:

        return true;
    default:
        return false;
    }
}
bool canBlockByGrayDarkness(SkillType_t skillType) {
    switch (skillType) {
    case SKILL_SWORD_WAVE:
    case SKILL_SWORD_RAY:
    case SKILL_WIND_DIVIDER:
    case SKILL_THUNDER_BOLT:
    case SKILL_THUNDER_STORM:
    case SKILL_WIDE_LIGHTNING:
    case SKILL_TORNADO_SEVER:
    case SKILL_EARTHQUAKE:
    case SKILL_POWER_OF_LAND:
    case SKILL_WILD_TYPHOON:
    case SKILL_MULTI_SHOT:
    case SKILL_MOLE_SHOT:
    case SKILL_GUN_SHOT_GUIDANCE:
    case SKILL_VIGOR_DROP:
    case SKILL_TURN_UNDEAD:
    case SKILL_ENERGY_DROP:
        return true;
    default:
        return false;
    }
}

bool canGiveSkillExp(Slayer* pSlayer, SkillDomainType_t SkillDomainType, SkillType_t UseSkillType) {
    if (pSlayer == NULL)
        return false;
    if (!pSlayer->isRealWearing(Slayer::WEAR_RIGHTHAND))
        return false;

    Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
    if (pWeapon == NULL)
        return false;

    switch (SkillDomainType) {
    case SKILL_DOMAIN_BLADE:
        if (pWeapon->getItemClass() != Item::ITEM_CLASS_BLADE)
            return false;
        break;
    case SKILL_DOMAIN_SWORD:
        if (pWeapon->getItemClass() != Item::ITEM_CLASS_SWORD)
            return false;
        break;
    case SKILL_DOMAIN_GUN:
        if (!isArmsWeapon(pWeapon))
            return false;
        break;
    case SKILL_DOMAIN_HEAL:
        if (pWeapon->getItemClass() != Item::ITEM_CLASS_CROSS)
            return false;
        break;
    case SKILL_DOMAIN_ENCHANT:
        if (pWeapon->getItemClass() != Item::ITEM_CLASS_MACE)
            return false;
        break;
    default:
        return false;
    }

    if (UseSkillType < SKILL_DOUBLE_IMPACT)
        return true;
    SkillInfo* pUseSkillInfo = g_pSkillInfoManager->getSkillInfo(UseSkillType);
    if (SkillDomainType != pUseSkillInfo->getDomainType())
        return false;

    return true;
}

void giveSkillExp(Slayer* pSlayer, SkillType_t SkillType, ModifyInfo& AttackerMI) {
    if (pSlayer == NULL)
        return;

    SkillSlot* pSkillSlot = pSlayer->getSkill(SkillType);
    SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
    if (pSkillSlot != NULL && pSkillInfo != NULL) {
        
        increaseSkillExp(pSlayer, pSkillInfo->getDomainType(), pSkillSlot, pSkillInfo, AttackerMI);
    }
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
HP_t setDamage(Creature* pTargetCreature, Damage_t Damage, Creature* pAttacker, SkillType_t SkillType, ModifyInfo* pMI,
               ModifyInfo* pAttackerMI, bool canKillTarget, bool canSteal) {
    Assert(pTargetCreature != NULL);

    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE) ||
        pTargetCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL) || pTargetCreature->isDead()) {
        // if (pTargetCreature->isVampire())
        
        
        //  by sigi. 2002.9.5
        return 0;
    }

    //	bool canKillTarget = (SkillType==SKILL_ACID_SWAMP)?false:true;

    Zone* pZone = pTargetCreature->getZone();
    Assert(pZone != NULL);

    
    if (pZone->getZoneID() == 1412 || pZone->getZoneID() == 1413) {
        if (pTargetCreature->isPC() && pAttacker != NULL && pAttacker->isPC())
            return 0;
    }

    HP_t Result = 0;
    HP_t Result2 = 0;
    HP_t hp = 0;
    MP_t mp = 0;
    ZoneCoord_t TX = 0;
    ZoneCoord_t TY = 0;
    ObjectID_t TOID = 0;
    ZoneCoord_t AX = 0;
    ZoneCoord_t AY = 0;
    ObjectID_t AOID = 0;

    Damage_t OriginalDamage = Damage;

    bool bBroadcastTargetHP = false;   
    bool bSendTargetHP = false;        
    bool bSendTargetMP = false;        
    bool bBroadcastAttackerHP = false; 
    bool bSendAttackerHP = false;      
    bool bSendAttackerMP = false;      

    GCStatusCurrentHP gcTargetHP;
    GCStatusCurrentHP gcAttackerHP;

    SkillProperty* pSkillProperty = g_pSkillPropertyManager->getSkillProperty(SkillType);
    bool bPhysicDamage = pSkillProperty->isPhysic();
    bool bMagicDamage = pSkillProperty->isMagic();

    SkillInfo* pSkillInfo = NULL;

    if (SkillType >= SKILL_DOUBLE_IMPACT) {
        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        Assert(pSkillInfo != NULL);

        BYTE domain = pSkillInfo->getDomainType();
        switch (domain) {
        case SKILL_DOMAIN_BLADE:
        case SKILL_DOMAIN_SWORD:
        case SKILL_DOMAIN_GUN:
            bPhysicDamage = true;
            bMagicDamage = false;
            break;
        case SKILL_DOMAIN_HEAL:
        case SKILL_DOMAIN_ENCHANT:
            bPhysicDamage = false;
            bMagicDamage = true;
            break;
        default:
            break;
        }
    }

    
    
    /*
    if( pTargetCreature != NULL && pTargetCreature->isFlag( Effect::EFFECT_CLASS_ARMAGEDDON ) && SkillType !=
    SKILL_ARMAGEDDON )
    {
        EffectArmageddon* pEffect =
    dynamic_cast<EffectArmageddon*>(pTargetCreature->findEffect(Effect::EFFECT_CLASS_ARMAGEDDON)); Assert( pEffect !=
    NULL );

        Damage = computePureDamage( pAttacker );

        pEffect->decreaseHP( Damage );

        if ( pTargetCreature->isSlayer() )
        {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
            Result = pSlayer->getHP( ATTR_CURRENT );
        }
        else if ( pTargetCreature->isVampire() )
        {
            Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
            Result = pVampire->getHP( ATTR_CURRENT );
        }
        else if ( pTargetCreature->isMonster() )
        {
            Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
            Result = pMonster->getHP( ATTR_CURRENT );
        }

        return Result;
    }
    */

    if (pTargetCreature != NULL && pTargetCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
        if (pMonster != NULL && pMonster->getMonsterType() == GROUND_ELEMENTAL_TYPE)
            if (pAttacker != NULL && pAttacker->isOusters())
                return 0;
    }

    
    if (pAttacker != NULL && pSkillProperty->isMagic() && pAttacker->isFlag(Effect::EFFECT_CLASS_STRIKING)) {
        EffectStriking* pEffect = dynamic_cast<EffectStriking*>(pAttacker->findEffect(Effect::EFFECT_CLASS_STRIKING));
        if (pEffect != NULL) {
            Damage += pEffect->getDamageBonus();
        }
    }

    if (pTargetCreature != NULL && pTargetCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET)) {
        EffectInstallTurret* pEffect =
            dynamic_cast<EffectInstallTurret*>(pTargetCreature->findEffect(Effect::EFFECT_CLASS_INSTALL_TURRET));
        if (pEffect != NULL) {
            Damage -= getPercentValue(Damage, pEffect->getDefense());
        }
    }

    
    if (pTargetCreature != NULL && pTargetCreature->isFlag(Effect::EFFECT_CLASS_DENIAL_MAGIC) &&
        pSkillProperty->isMagic()) {
        
        if (SkillType >= SKILL_DOUBLE_IMPACT) {
            
            if (pSkillInfo->getDomainType() == SKILL_DOMAIN_VAMPIRE ||
                pSkillInfo->getDomainType() == SKILL_DOMAIN_OUSTERS) {
                
                GCAddEffect gcAddEffect;
                gcAddEffect.setObjectID(pTargetCreature->getObjectID());
                gcAddEffect.setEffectID(Effect::EFFECT_CLASS_DENIAL_MAGIC_DAMAGED);
                gcAddEffect.setDuration(0);

                pTargetCreature->getZone()->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(),
                                                            &gcAddEffect);

                //				return 0;
                
                Damage = max(1, (int)(Damage * 0.4));
            }
        }
    }

    
    if (pTargetCreature != NULL && pTargetCreature->isFlag(Effect::EFFECT_CLASS_WATER_BARRIER) &&
        !pSkillProperty->isMelee()) {
        
        if (canBlockByWaterBarrier(SkillType)) {
            EffectWaterBarrier* pEWB =
                dynamic_cast<EffectWaterBarrier*>(pTargetCreature->findEffect(Effect::EFFECT_CLASS_WATER_BARRIER));
            if (pEWB != NULL) {
                int bonus = pEWB->getBonus();
                if (bonus > 100)
                    bonus = 100;

                Damage = (Damage_t)(Damage * ((float)(100 - bonus) / 100.0));
                Damage = max(1, (int)Damage);
            }
        }
    }

    
    if (pTargetCreature != NULL && pSkillProperty->isPhysic()) {
        if (pTargetCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);

            if (pOusters->isPassiveAvailable(SKILL_WATER_SHIELD)) {
                if ((rand() % 100) < min(20, (int)(pOusters->getPassiveRatio() * 3 / 2))) {
                    GCAddEffect gcAddEffect;
                    gcAddEffect.setObjectID(pOusters->getObjectID());
                    gcAddEffect.setEffectID(Effect::EFFECT_CLASS_WATER_SHIELD);
                    gcAddEffect.setDuration(0);

                    pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &gcAddEffect);
                    
                    return 0;
                }
            }
        }
    }

    if (pTargetCreature != NULL && pTargetCreature->isFlag(Effect::EFFECT_CLASS_STONE_SKIN) &&
        (SkillType >= SKILL_DOUBLE_IMPACT || SkillType == SKILL_ATTACK_ARMS)) {
        if (SkillType == SKILL_ATTACK_ARMS ||
            g_pSkillInfoManager->getSkillInfo(SkillType)->getDomainType() == SKILL_DOMAIN_GUN) {
            EffectStoneSkin* pStoneSkin =
                dynamic_cast<EffectStoneSkin*>(pTargetCreature->findEffect(Effect::EFFECT_CLASS_STONE_SKIN));
            if (pStoneSkin != NULL) {
                if ((rand() % 100) < pStoneSkin->getBonus()) {
                    Damage = 0;
                    GCAddEffect gcAddEffect;
                    gcAddEffect.setObjectID(pTargetCreature->getObjectID());
                    gcAddEffect.setEffectID(Effect::EFFECT_CLASS_STONE_SKIN_DAMAGED);
                    gcAddEffect.setDuration(0);

                    pTargetCreature->getZone()->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(),
                                                                &gcAddEffect);
                }
            }
        }
    }


    if (pAttacker != NULL && pAttacker->isOusters() && SkillType >= SKILL_DOUBLE_IMPACT) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);
        Assert(pOusters != NULL);
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        Assert(pSkillInfo != NULL);

        if (pSkillProperty->isMagic()) {
            switch (pSkillInfo->getElementalDomain()) {
            case ELEMENTAL_DOMAIN_FIRE:
                Damage += pOusters->getFireDamage();
                

                if (pOusters->isFlag(Effect::EFFECT_CLASS_HANDS_OF_FIRE)) {
                    EffectHandsOfFire* pEffect =
                        dynamic_cast<EffectHandsOfFire*>(pOusters->findEffect(Effect::EFFECT_CLASS_HANDS_OF_FIRE));
                    if (pEffect != NULL) {
                        Damage = (Damage_t)(Damage * (1.0 + (pEffect->getBonus() / 100.0)));
                    }
                }
                break;
            case ELEMENTAL_DOMAIN_WATER:
                Damage += pOusters->getWaterDamage();
                break;
            case ELEMENTAL_DOMAIN_EARTH:
                Damage += pOusters->getEarthDamage();
                break;
            default:
                break;
            }
        }
    }

    if (pTargetCreature != NULL) {
        TX = pTargetCreature->getX();
        TY = pTargetCreature->getY();
        TOID = pTargetCreature->getObjectID();

        gcTargetHP.setObjectID(TOID);
    }

    ////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////////
    if (pTargetCreature != NULL) {
        
        if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_SLEEP) && SkillType != SKILL_REBUKE) {
            EffectSleep* pEffect = dynamic_cast<EffectSleep*>(pTargetCreature->findEffect(Effect::EFFECT_CLASS_SLEEP));
            Assert(pEffect != NULL);

            pEffect->setDeadline(0);
        }
    }

    if (pAttacker != NULL) {
        AX = pAttacker->getX();
        AY = pAttacker->getY();
        AOID = pAttacker->getObjectID();

        gcAttackerHP.setObjectID(AOID);

        
        
        
        if (pAttacker->isFlag(Effect::EFFECT_CLASS_HYMN)) {
            EffectHymn* pHymn =
                dynamic_cast<EffectHymn*>(pAttacker->getEffectManager()->findEffect(Effect::EFFECT_CLASS_HYMN));

            Damage = Damage * (100 - pHymn->getDamagePenalty()) / 100;
        }

        if (pAttacker->isSlayer() && pAttackerMI != NULL && !pTargetCreature->isSlayer()) {
            Slayer* pAttackSlayer = dynamic_cast<Slayer*>(pAttacker);
            Assert(pAttackSlayer != NULL);

            
            if (canGiveSkillExp(pAttackSlayer, SKILL_DOMAIN_SWORD, SkillType)) {
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_DANCING_SWORD) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_DANCING_SWORD, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_REDIANCE) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_REDIANCE, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_EXPANSION) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_EXPANSION, *pAttackerMI);
            } else if (canGiveSkillExp(pAttackSlayer, SKILL_DOMAIN_BLADE, SkillType)) {
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_GHOST_BLADE) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_GHOST_BLADE, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_POTENTIAL_EXPLOSION) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_POTENTIAL_EXPLOSION, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_CHARGING_POWER) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_CHARGING_POWER, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_BERSERKER) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_BERSERKER, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_AIR_SHIELD_1) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_AIR_SHIELD, *pAttackerMI);
            } else if (canGiveSkillExp(pAttackSlayer, SKILL_DOMAIN_GUN, SkillType)) {
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_CONCEALMENT) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_CONCEALMENT, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_REVEALER) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_REVEALER, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_OBSERVING_EYE, *pAttackerMI);
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_HEART_CATALYST) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_HEART_CATALYST, *pAttackerMI);
            } else if (canGiveSkillExp(pAttackSlayer, SKILL_DOMAIN_ENCHANT, SkillType)) {
                if (pAttackSlayer->isFlag(Effect::EFFECT_CLASS_AURA_SHIELD) && (rand() % 2) != 0)
                    giveSkillExp(pAttackSlayer, SKILL_AURA_SHIELD, *pAttackerMI);
            }

            /*			if (pAttacker->isFlag(Effect::EFFECT_CLASS_REDIANCE) && pAttacker->isSlayer() && pAttackerMI !=
               NULL && !pTargetCreature->isSlayer())
                        {
                            EffectRediance* pEffect = dynamic_cast<EffectRediance*>(
               pAttacker->findEffect(Effect::EFFECT_CLASS_REDIANCE) ); if ( pEffect != NULL && pEffect->canGiveExp())
                            {
                                Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);
                                Assert( pSlayer != NULL );

                                SkillSlot* pSkillSlot = pSlayer->getSkill( SKILL_REDIANCE );
                                SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_REDIANCE );
                                if ( pSkillSlot != NULL && pSkillInfo != NULL )
                                {
                                    increaseSkillExp(pSlayer, SKILL_DOMAIN_SWORD,  pSkillSlot, pSkillInfo,
               *pAttackerMI);
                                }
                            }
                        }
                        if (pAttacker->isFlag(Effect::EFFECT_CLASS_EXPANSION) && pAttacker->isSlayer() && pAttackerMI !=
               NULL && !pTargetCreature->isSlayer())
                        {
                            Effect* pEffect = pAttacker->findEffect(Effect::EFFECT_CLASS_EXPANSION);
                            if ( pEffect != NULL && (rand()%2)==1 )
                            {
                                Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);
                                Assert( pSlayer != NULL );

                                SkillSlot* pSkillSlot = pSlayer->getSkill( SKILL_EXPANSION );
                                SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_EXPANSION );
                                if ( pSkillSlot != NULL && pSkillInfo != NULL )
                                {
                                    increaseSkillExp(pSlayer, SKILL_DOMAIN_SWORD,  pSkillSlot, pSkillInfo,
               *pAttackerMI);
                                }
                            }
                        }
                        if (pAttacker->isFlag(Effect::EFFECT_CLASS_GHOST_BLADE) && pAttacker->isSlayer() && pAttackerMI
               != NULL && !pTargetCreature->isSlayer())
                        {
                            Effect* pEffect = pAttacker->findEffect(Effect::EFFECT_CLASS_GHOST_BLADE);
                            if ( pEffect != NULL && (rand()%2)==1 )
                            {
                                Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);
                                Assert( pSlayer != NULL );

                                SkillSlot* pSkillSlot = pSlayer->getSkill( SKILL_GHOST_BLADE );
                                SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_GHOST_BLADE );
                                if ( pSkillSlot != NULL && pSkillInfo != NULL )
                                {
                                    increaseSkillExp(pSlayer, SKILL_DOMAIN_BLADE,  pSkillSlot, pSkillInfo,
               *pAttackerMI);
                                }
                            }
                        }*/
        }

        
        if (pAttacker->isPC()) {
            PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pAttacker);
            Damage_t MagicBonusDamage = pPC->getMagicBonusDamage();
            Damage_t PhysicBonusDamage = pPC->getPhysicBonusDamage();

            //			if ( MagicBonusDamage != 0 && pSkillProperty!= NULL && pSkillProperty->isMagic() )
            if (MagicBonusDamage != 0 && bMagicDamage) {
                
                Damage += MagicBonusDamage;
            }
            //			if ( PhysicBonusDamage != 0 && pSkillProperty!= NULL && pSkillProperty->isPhysic() )
            if (PhysicBonusDamage != 0 && bPhysicDamage) {
                
                Damage += PhysicBonusDamage;
            }
        }
    }

    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    if (pAttacker != NULL && canSteal) //(SkillType != SKILL_PROMINENCE && SkillType != SKILL_HELLFIRE)
    {
        Steal_t HPStealAmount = pAttacker->getHPStealAmount();
        Steal_t MPStealAmount = pAttacker->getMPStealAmount();

        
        if (HPStealAmount != 0 && (rand() % 100) < pAttacker->getHPStealRatio()) {
            if (pAttacker->isSlayer()) {
                if (pAttacker->isAlive()) {
                    Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);

                    
                    
                    hp = pSlayer->getHP(ATTR_CURRENT) + (int)HPStealAmount;
                    hp = min(hp, pSlayer->getHP(ATTR_MAX));

                    
                    pSlayer->setHP(hp, ATTR_CURRENT);
                    bBroadcastAttackerHP = true;
                    bSendAttackerHP = true;
                }
            } else if (pAttacker->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);

                
                
                hp = pVampire->getHP(ATTR_CURRENT) + (int)HPStealAmount;
                hp = min(hp, pVampire->getHP(ATTR_MAX));

                
                pVampire->setHP(hp, ATTR_CURRENT);
                bBroadcastAttackerHP = true;
                bSendAttackerHP = true;
            } else if (pAttacker->isOusters()) {
                
                if (pAttacker->isAlive()) {
                    Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);

                    
                    
                    // cout << "HP Steal!" << (int)HPStealAmount << endl;
                    hp = pOusters->getHP(ATTR_CURRENT) + (int)HPStealAmount;
                    hp = min(hp, pOusters->getHP(ATTR_MAX));

                    
                    pOusters->setHP(hp, ATTR_CURRENT);
                    bBroadcastAttackerHP = true;
                    bSendAttackerHP = true;
                }
            } else
                Assert(false);
        }

        
        if (MPStealAmount != 0 && (rand() % 100) < pAttacker->getMPStealRatio()) {
            
            if (pAttacker->isSlayer()) {
                Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);

                
                
                mp = pSlayer->getMP(ATTR_CURRENT) + (int)MPStealAmount;
                mp = min(mp, pSlayer->getMP(ATTR_MAX));

                pSlayer->setMP(mp, ATTR_CURRENT);

                bSendAttackerMP = true;
            } else if (pAttacker->isOusters()) {
                Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);

                if (pOusters->getMP(ATTR_CURRENT) < pOusters->getMP(ATTR_MAX)) {
                    
                    
                    mp = pOusters->getMP(ATTR_CURRENT) + (int)MPStealAmount;
                    mp = min(mp, pOusters->getMP(ATTR_MAX));

                    pOusters->setMP(mp, ATTR_CURRENT);

                    bSendAttackerMP = true;
                }
            }
        }
    }

    if (pTargetCreature != NULL && pTargetCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)) {
        EffectExplosionWater* pEffect =
            dynamic_cast<EffectExplosionWater*>(pTargetCreature->findEffect(Effect::EFFECT_CLASS_EXPLOSION_WATER));

        if (pEffect != NULL) {
            int ratio = 100;
            ratio -= pEffect->getDamageReduce();
            if (ratio < 0)
                ratio = 0;

            Damage = max(1, getPercentValue(Damage, ratio));
        }
    }

    if (pAttacker != NULL && pAttacker->isFlag(Effect::EFFECT_CLASS_REPUTO_FACTUM)) {
        Damage_t counterDamage = getPercentValue(Damage, 30);

        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pAttacker->getObjectID());
        gcAddEffect.setEffectID(Effect::EFFECT_CLASS_REPUTO_FACTUM);
        gcAddEffect.setDuration(0);
        pAttacker->getZone()->broadcastPacket(pAttacker->getX(), pAttacker->getY(), &gcAddEffect);
        setCounterDamage(pAttacker, pTargetCreature, counterDamage, bBroadcastAttackerHP, bSendAttackerHP);
    }

    if (pTargetCreature != NULL && !pTargetCreature->isSlayer()) {
        Effect* pEffect = pTargetCreature->getZone()
                              ->getTile(pTargetCreature->getX(), pTargetCreature->getY())
                              .getEffect(Effect::EFFECT_CLASS_SWORD_OF_THOR);
        if (pEffect != NULL) {
            EffectSwordOfThor* pThor = dynamic_cast<EffectSwordOfThor*>(pEffect);
            if (pThor != NULL) {
                cout << "before thor : " << Damage << endl;
                Damage = getPercentValue(Damage, 140 + (pThor->getLevel() / 10));
                cout << "after thor : " << Damage << endl;
            }
        }
    }

    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    if (pTargetCreature->isSlayer()) {
        
        Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
        bool bSetDamage = false;

        if (pSlayer->isFlag(Effect::EFFECT_CLASS_REQUITAL) && pAttacker != NULL && pSkillProperty != NULL &&
            pSkillProperty->isMelee()) {
            EffectRequital* pEffectRequital = (EffectRequital*)(pSlayer->findEffect(Effect::EFFECT_CLASS_REQUITAL));
            int refl = pEffectRequital->getReflection();
            Damage_t counterDamage = max(1, getPercentValue(Damage, refl));

            Result2 = setCounterDamage(pAttacker, pSlayer, counterDamage, bBroadcastAttackerHP, bSendAttackerHP);
        }

        
        
        // 2003. 1.16. Sequoia
        if (pSlayer->isFlag(Effect::EFFECT_CLASS_AURA_SHIELD) && SkillType != SKILL_GROUND_ATTACK) {
            
            if (pAttacker != NULL) {
                Damage_t counterDamage = 0;

                EffectAuraShield* pEffectAuraShield =
                    (EffectAuraShield*)(pSlayer->findEffect(Effect::EFFECT_CLASS_AURA_SHIELD));
                Assert(pEffectAuraShield != NULL);

                
                counterDamage = max(1, getPercentValue(Damage, 10));

                if (pAttacker->isVampire()) {
                    Vampire* pVampireAttacker = dynamic_cast<Vampire*>(pAttacker);
                    Result2 = max(0, (int)pVampireAttacker->getHP() - (int)counterDamage);
                    pVampireAttacker->setHP(Result2, ATTR_CURRENT);

                    bBroadcastAttackerHP = true;
                    bSendAttackerHP = true;
                } else if (pAttacker->isOusters()) {
                    Ousters* pOustersAttacker = dynamic_cast<Ousters*>(pAttacker);
                    Result2 = max(0, (int)pOustersAttacker->getHP() - (int)counterDamage);
                    pOustersAttacker->setHP(Result2, ATTR_CURRENT);

                    bBroadcastAttackerHP = true;
                    bSendAttackerHP = true;
                } else if (pAttacker->isMonster()) {
                    Monster* pMonsterAttacker = dynamic_cast<Monster*>(pAttacker);
                    Result2 = max(0, (int)pMonsterAttacker->getHP() - (int)counterDamage);
                    pMonsterAttacker->setHP(Result2, ATTR_CURRENT);
                    pMonsterAttacker->setDamaged(true);

                    bBroadcastAttackerHP = true;
                }
            }

#ifdef __CHINA_SERVER__
            Result = max(0, (int)pSlayer->getMP(ATTR_CURRENT) - (int)(Damage * 2.5));
#else
            Result = max(0, (int)pSlayer->getMP(ATTR_CURRENT) - (int)(Damage * 2));
#endif

            pSlayer->setMP(Result, ATTR_CURRENT);
            bSendTargetMP = true;

            
            
            if (Result == 0) {
                Effect* pEffect = pSlayer->findEffect(Effect::EFFECT_CLASS_AURA_SHIELD);
                if (pEffect != NULL)
                    pEffect->setDeadline(0);
                /*				pSlayer->deleteEffect(Effect::EFFECT_CLASS_AURA_SHIELD);
                                pSlayer->removeFlag(Effect::EFFECT_CLASS_AURA_SHIELD);

                                GCRemoveEffect removeEffect;
                                removeEffect.setObjectID(TOID);
                                removeEffect.addEffectList(Effect::EFFECT_CLASS_AURA_SHIELD);
                                pZone->broadcastPacket(TX, TY, &removeEffect);*/

                
                SkillSlot* pSkillSlot = pSlayer->hasSkill(SKILL_AURA_SHIELD);
                if (pSkillSlot != NULL) {
                    pSkillSlot->setRunTime(0, false);
                }
            }
            bSetDamage = true;
        }

        // by Sequoia 2002.12.26
        
        
        if (pSlayer->isFlag(Effect::EFFECT_CLASS_SHARP_SHIELD_1) && pSkillProperty != NULL &&
            pSkillProperty->isMelee() && pAttacker != NULL) {
            
            Damage = max(0, (int)Damage - ((int)OriginalDamage >> 1));

            EffectSharpShield* pEffect =
                dynamic_cast<EffectSharpShield*>(pSlayer->findEffect(Effect::EFFECT_CLASS_SHARP_SHIELD_1));
            Assert(pEffect != NULL);

            Damage_t counterDamage = pEffect->getDamage();

            Result2 = setCounterDamage(pAttacker, pSlayer, counterDamage, bBroadcastAttackerHP, bSendAttackerHP);

            if (pAttacker != NULL && !pAttacker->isSlayer())
                if (pMI != NULL && (rand() % 2) != 0)
                    giveSkillExp(pSlayer, SKILL_SHARP_SHIELD, *pMI);
            //			SkillSlot* pSkillSlot = pSlayer->getSkill( SKILL_SHARP_SHIELD );
            //			SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_SHARP_SHIELD );
            //
            //			if ( rand()%2 ) increaseSkillExp(pSlayer, SKILL_DOMAIN_SWORD, pSkillSlot, pSkillInfo,
            //*pAttackerMI);

             
        }

        if (pSlayer->isFlag(Effect::EFFECT_CLASS_AIR_SHIELD_1) && pSkillProperty != NULL && pSkillProperty->isMelee()) {
            bool isUnderworld = false;

#ifdef __UNDERWORLD__
            if (pAttacker != NULL && pAttacker->isMonster()) {
                Monster* pMonster = dynamic_cast<Monster*>(pAttacker);
                if (pMonster != NULL && (pMonster->isUnderworld() || pMonster->getMonsterType() == 599))
                    isUnderworld = true;
            }
#endif

            if (!isUnderworld) {
                EffectAirShield* pEffect =
                    dynamic_cast<EffectAirShield*>(pSlayer->findEffect(Effect::EFFECT_CLASS_AIR_SHIELD_1));
                Assert(pEffect != NULL);

                Damage = max(1, (int)Damage - getPercentValue(OriginalDamage, pEffect->getDamage()));
            }
        }

        Tile& rTile = pZone->getTile(TX, TY);

        if (rTile.getEffect(Effect::EFFECT_CLASS_MAGIC_ELUSION) != NULL
            
            && (!pSkillProperty->isMelee() && pSkillProperty->isMagic()) && pAttacker != NULL &&
            pAttacker->isVampire()) {
            Damage /= 2;
        }

        if (!bSetDamage) {
            if (pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE))
                Damage *= 1.5;

            //			cout << "before " << Damage << endl;
            //			if ( pSkillProperty->isMagic() )
            if (bMagicDamage) {
                Damage -= min(Damage - 1, (int)pSlayer->getMagicDamageReduce());
            }
            //			else if ( pSkillProperty->isPhysic() )
            else if (bPhysicDamage) {
                Damage -= min(Damage - 1, (int)pSlayer->getPhysicDamageReduce());
            }
            //			cout << "after " << Damage << endl;

            
            if (canKillTarget)
                Result = max(0, (int)pSlayer->getHP(ATTR_CURRENT) - (int)Damage);
            else
                Result = max(1, (int)pSlayer->getHP(ATTR_CURRENT) - (int)Damage);

            pSlayer->setHP(Result, ATTR_CURRENT);

            bBroadcastTargetHP = true;
            bSendTargetHP = true;
        }
    }
    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    else if (pTargetCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
        Silver_t silverDamage = 0;

        Tile& rTile = pZone->getTile(TX, TY);

        if (rTile.getEffect(Effect::EFFECT_CLASS_GRAY_DARKNESS) != NULL && canBlockByGrayDarkness(SkillType)) {
            
            Damage = (Damage_t)(Damage * 0.7);
        }

        if (pAttacker != NULL && pAttacker->isSlayer()) {
            
            silverDamage = computeSlayerSilverDamage(pAttacker, Damage, pAttackerMI);
        }

        
        Damage += silverDamage;
        
        if (SkillType == SKILL_SHINE_SWORD && silverDamage != 0) {
            silverDamage = Damage;
        }
        // end

        if (pVampire->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE))
            Damage *= 1.5;

        //		cout << "before " << Damage << endl;
        //		if ( pSkillProperty->isMagic() )
        if (bMagicDamage) {
            Damage -= min(Damage - 1, (int)pVampire->getMagicDamageReduce());
        }
        //		else if ( pSkillProperty->isPhysic() )
        else if (bPhysicDamage) {
            Damage -= min(Damage - 1, (int)pVampire->getPhysicDamageReduce());
        }
        //		cout << "after " << Damage << endl;

        HP_t currentHP = pVampire->getHP(ATTR_CURRENT);

        if (canKillTarget)
            Result = max(0, (int)currentHP - (int)Damage);
        else
            Result = max(1, (int)currentHP - (int)Damage);

        pVampire->setHP(Result, ATTR_CURRENT);

        
        if (pVampire->isFlag(Effect::EFFECT_CLASS_MEPHISTO)) {
            HP_t maxHP = pVampire->getHP(ATTR_MAX);

            
            if (currentHP * 3 < maxHP) {
                Effect* pEffect = pVampire->findEffect(Effect::EFFECT_CLASS_MEPHISTO);
                if (pEffect != NULL) {
                    pEffect->setDeadline(0);
                } else {
                    pVampire->removeFlag(Effect::EFFECT_CLASS_MEPHISTO);
                }
                //				pVampire->getEffectManager()->deleteEffect( Effect::EFFECT_CLASS_MEPHISTO );
            }
        }


        bBroadcastTargetHP = true;
        bSendTargetHP = true;

        if (silverDamage != 0 && pVampire->getSilverDamage() < (pVampire->getHP(ATTR_MAX) / 2)) {
            Silver_t newSilverDamage = pVampire->getSilverDamage() + silverDamage;
            pVampire->saveSilverDamage(newSilverDamage);
            if (pMI)
                pMI->addShortData(MODIFY_SILVER_DAMAGE, newSilverDamage);
        }
    }
    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    else if (pTargetCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
        Silver_t silverDamage = 0;

        if (pOusters->isFlag(Effect::EFFECT_CLASS_REACTIVE_ARMOR)) {
            EffectReactiveArmor* pEffect =
                dynamic_cast<EffectReactiveArmor*>(pOusters->findEffect(Effect::EFFECT_CLASS_REACTIVE_ARMOR));
            if (pEffect != NULL && pEffect->getDamageReduce() != 0) {
                cout << "before reactive : " << Damage << endl;
                Damage -= getPercentValue(Damage, pEffect->getDamageReduce());
                cout << "after  reactive : " << Damage << endl;
            }
        }

        
        if (pOusters->isFlag(Effect::EFFECT_CLASS_DIVINE_SPIRITS) && pSkillProperty->isMagic()) {
            EffectDivineSpirits* pEffect =
                dynamic_cast<EffectDivineSpirits*>(pOusters->findEffect(Effect::EFFECT_CLASS_DIVINE_SPIRITS));
            if (pEffect != NULL) {
                int Ratio = pEffect->getBonus();
                Damage_t mpDamage = (Damage_t)(((DWORD)Damage) * Ratio / 100);
                mpDamage = min(Damage, mpDamage);
                mpDamage = min(pOusters->getMP(), mpDamage);

                Damage -= mpDamage;

                MP_t resultMP = pOusters->getMP() - mpDamage;
                pOusters->setMP(resultMP, ATTR_CURRENT);
                bSendTargetMP = true;
            }
        }

        if (pOusters->isFlag(Effect::EFFECT_CLASS_FROZEN_ARMOR) && pSkillProperty->isMelee()) {
            EffectFrozenArmor* pFrozenArmor =
                dynamic_cast<EffectFrozenArmor*>(pOusters->findEffect(Effect::EFFECT_CLASS_FROZEN_ARMOR));
            if (pFrozenArmor != NULL) {
                Damage -= getPercentValue(Damage, pFrozenArmor->getBonus());
                if (pAttacker != NULL) {
                    
                    EffectIceFieldToCreature* pEffect = new EffectIceFieldToCreature(pAttacker, true);
                    pEffect->setDeadline(pFrozenArmor->getTargetDuration());
                    pAttacker->addEffect(pEffect);
                    pAttacker->setFlag(Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE);

                    GCAddEffect gcAddEffect;
                    gcAddEffect.setObjectID(pAttacker->getObjectID());
                    gcAddEffect.setEffectID(pEffect->getSendEffectClass());
                    gcAddEffect.setDuration(pFrozenArmor->getTargetDuration());

                    pAttacker->getZone()->broadcastPacket(pAttacker->getX(), pAttacker->getY(), &gcAddEffect);
                }
            }
        }

        if (pAttacker != NULL && pAttacker->isSlayer()) {
            
            
            silverDamage = (Silver_t)(computeSlayerSilverDamage(pAttacker, Damage, pAttackerMI) * 1.5);
            silverDamage = max(0, getPercentValue(silverDamage, 100 - pOusters->getSilverResist()));
        }

        
        Damage += silverDamage;
        
        if (SkillType == SKILL_SHINE_SWORD && silverDamage != 0) {
            silverDamage = Damage;
        }
        // end

        HP_t currentHP = pOusters->getHP(ATTR_CURRENT);

        //		cout << "before " << Damage << endl;
        //		if ( pSkillProperty->isMagic() )
        if (bMagicDamage) {
            Damage -= min(Damage - 1, (int)pOusters->getMagicDamageReduce());
        }
        //		else if ( pSkillProperty->isPhysic() )
        else if (bPhysicDamage) {
            Damage -= min(Damage - 1, (int)pOusters->getPhysicDamageReduce());
        }
        //		cout << "after " << Damage << endl;

        if (canKillTarget)
            Result = max(0, (int)currentHP - (int)Damage);
        else
            Result = max(1, (int)currentHP - (int)Damage);

        pOusters->setHP(Result, ATTR_CURRENT);

        bBroadcastTargetHP = true;
        bSendTargetHP = true;

        if (silverDamage != 0 && pOusters->getSilverDamage() < (pOusters->getHP(ATTR_MAX) / 2)) {
            Silver_t newSilverDamage = pOusters->getSilverDamage() + silverDamage;
            pOusters->saveSilverDamage(newSilverDamage);
            if (pMI)
                pMI->addShortData(MODIFY_SILVER_DAMAGE, newSilverDamage);
        }
    }
    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    else if (pTargetCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
        Silver_t silverDamage = 0;

        if (pAttacker != NULL && pAttacker->isSlayer()) {
            
            silverDamage = computeSlayerSilverDamage(pAttacker, Damage, pAttackerMI);
        }

        
        Damage += silverDamage;

        if (canKillTarget)
            Result = max(0, (int)pMonster->getHP(ATTR_CURRENT) - (int)Damage);
        else
            Result = max(1, (int)pMonster->getHP(ATTR_CURRENT) - (int)Damage);

        pMonster->setHP(Result, ATTR_CURRENT);

        if (pMonster->isFlag(Effect::EFFECT_CLASS_SHARE_HP)) {
            EffectShareHP* pEffect = dynamic_cast<EffectShareHP*>(pMonster->findEffect(Effect::EFFECT_CLASS_SHARE_HP));
            if (pEffect != NULL) {
                list<ObjectID_t>::iterator itr = pEffect->getSharingCreatures().begin();

                for (; itr != pEffect->getSharingCreatures().end(); ++itr) {
                    Monster* pMonster = dynamic_cast<Monster*>(pZone->getCreature(*itr));
                    if (pMonster != NULL) {
                        GCStatusCurrentHP gcHP;
                        pMonster->setHP(Result, ATTR_CURRENT);
                        gcHP.setObjectID(pMonster->getObjectID());
                        gcHP.setCurrentHP(Result);
                        pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &gcHP);
                    }
                }
            }
        }

        if (Result == 0 && pAttacker != NULL && pAttacker->isPC()) {
            increaseFame(pAttacker, Damage);

            PlayerCreature* pAttackerPC = dynamic_cast<PlayerCreature*>(pAttacker);
            Assert(pAttackerPC != NULL);

            GCModifyInformation gcFameMI;
            gcFameMI.addLongData(MODIFY_FAME, pAttackerPC->getFame());

            pAttackerPC->getPlayer()->sendPacket(&gcFameMI);

            //			if ( pAttackerMI != NULL ) pAttackerMI->addLongData(MODIFY_FAME, pAttackerPC->getFame());
        }

        bBroadcastTargetHP = true;

        if (silverDamage != 0 && pMonster->getSilverDamage() < (pMonster->getHP(ATTR_MAX) / 2)) {
            pMonster->setSilverDamage(pMonster->getSilverDamage() + silverDamage);
        }

        pMonster->setDamaged(true);

        if (pAttacker != NULL && pAttacker->isPC()) {
            
            
            pMonster->addPrecedence(pAttacker->getName(), pAttacker->getPartyID(), Damage);
            pMonster->setLastHitCreatureClass(pAttacker->getCreatureClass());
        }

         
        
        
        if (pMonster->getHP(ATTR_CURRENT) * 3 < pMonster->getHP(ATTR_MAX)) {
            PrecedenceTable* pTable = pMonster->getPrecedenceTable();

            
            
            
            
            if (pTable->getComputeFlag() == false) {
                
                pTable->compute();

                
                EffectPrecedence* pEffectPrecedence = new EffectPrecedence(pMonster);
                pEffectPrecedence->setDeadline(100);
                pEffectPrecedence->setHostName(pTable->getHostName());
                pEffectPrecedence->setHostPartyID(pTable->getHostPartyID());
                pMonster->setFlag(Effect::EFFECT_CLASS_PRECEDENCE);
                pMonster->addEffect(pEffectPrecedence);
            }
        }

        if (pMonster->getMonsterType() == 722 && pAttacker != NULL &&
            !pAttacker->isFlag(Effect::EFFECT_CLASS_BLINDNESS)) {
            if ((rand() % 100) < 30) {
                
                EffectBlindness* pEffect = new EffectBlindness(pAttacker);
                pEffect->setDamage(50);
                pEffect->setNextTime(0);
                pEffect->setDeadline(100);
                pAttacker->setFlag(Effect::EFFECT_CLASS_BLINDNESS);
                pAttacker->addEffect(pEffect);

                GCAddEffect gcAddEffect;
                gcAddEffect.setObjectID(pAttacker->getObjectID());
                gcAddEffect.setEffectID(pEffect->getSendEffectClass());
                gcAddEffect.setDuration(100);

                pZone->broadcastPacket(pAttacker->getX(), pAttacker->getY(), &gcAddEffect);
            }
        }
    }

    //	if ( pAttacker != NULL ) pAttacker->setLastTarget( pTargetCreature->getObjectID() );

    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    if (bBroadcastTargetHP && pTargetCreature != NULL) 
    {
        if (pTargetCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
            gcTargetHP.setCurrentHP(pSlayer->getHP(ATTR_CURRENT));
        } else if (pTargetCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
            gcTargetHP.setCurrentHP(pVampire->getHP(ATTR_CURRENT));
        } else if (pTargetCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
            gcTargetHP.setCurrentHP(pOusters->getHP(ATTR_CURRENT));
        } else if (pTargetCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
            gcTargetHP.setCurrentHP(pMonster->getHP(ATTR_CURRENT));
        } else
            Assert(false);

        pZone->broadcastPacket(TX, TY, &gcTargetHP, pTargetCreature);
    }

    if (bSendTargetHP && pTargetCreature != NULL) 
    {
        if (pTargetCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
            if (pMI != NULL)
                pMI->addShortData(MODIFY_CURRENT_HP, pSlayer->getHP(ATTR_CURRENT));
            // if (pSlayer->getHP(ATTR_CURRENT) == 0) increaseFame(pAttacker, Damage);
        } else if (pTargetCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
            if (pMI != NULL)
                pMI->addShortData(MODIFY_CURRENT_HP, pVampire->getHP(ATTR_CURRENT));
            // if (pVampire->getHP(ATTR_CURRENT) == 0) increaseFame(pAttacker, Damage);
        } else if (pTargetCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
            if (pMI != NULL)
                pMI->addShortData(MODIFY_CURRENT_HP, pOusters->getHP(ATTR_CURRENT));
        } else
            Assert(false);
    }

    if (bSendTargetMP && pTargetCreature != NULL) 
    {
        if (pTargetCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
            if (pMI != NULL)
                pMI->addShortData(MODIFY_CURRENT_MP, pSlayer->getMP(ATTR_CURRENT));
            else {
                GCModifyInformation gcMI;
                gcMI.addShortData(MODIFY_CURRENT_MP, pSlayer->getMP(ATTR_CURRENT));

                pSlayer->getPlayer()->sendPacket(&gcMI);
            }
        } else if (pTargetCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
            if (pMI != NULL)
                pMI->addShortData(MODIFY_CURRENT_MP, pOusters->getMP(ATTR_CURRENT));
            else {
                GCModifyInformation gcMI;
                gcMI.addShortData(MODIFY_CURRENT_MP, pOusters->getMP(ATTR_CURRENT));

                pOusters->getPlayer()->sendPacket(&gcMI);
            }
        } else
            Assert(false);
    }

    if (bBroadcastAttackerHP && pAttacker != NULL) 
    {
        if (pAttacker->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);
            gcAttackerHP.setCurrentHP(pSlayer->getHP(ATTR_CURRENT));
        } else if (pAttacker->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);
            gcAttackerHP.setCurrentHP(pVampire->getHP(ATTR_CURRENT));
        } else if (pAttacker->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);
            gcAttackerHP.setCurrentHP(pOusters->getHP(ATTR_CURRENT));
        } else if (pAttacker->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pAttacker);
            gcAttackerHP.setCurrentHP(pMonster->getHP(ATTR_CURRENT));
        } else
            Assert(false);

        pZone->broadcastPacket(AX, AY, &gcAttackerHP, pAttacker);
    }

    if (bSendAttackerHP && pAttacker != NULL) 
    {
        if (pAttacker->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);
            if (pAttackerMI != NULL)
                pAttackerMI->addShortData(MODIFY_CURRENT_HP, pSlayer->getHP(ATTR_CURRENT));
            // if (pSlayer->getHP(ATTR_CURRENT) == 0) increaseFame(pTargetCreature, Damage);
        } else if (pAttacker->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);
            if (pAttackerMI != NULL)
                pAttackerMI->addShortData(MODIFY_CURRENT_HP, pVampire->getHP(ATTR_CURRENT));
            // if (pVampire->getHP(ATTR_CURRENT) == 0) increaseFame(pTargetCreature, Damage);
        } else if (pAttacker->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);
            if (pAttackerMI != NULL)
                pAttackerMI->addShortData(MODIFY_CURRENT_HP, pOusters->getHP(ATTR_CURRENT));
        } else
            Assert(false);
    }

    if (bSendAttackerMP && pAttacker != NULL) 
    {
        if (pAttacker->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);
            if (pAttackerMI != NULL)
                pAttackerMI->addShortData(MODIFY_CURRENT_MP, pSlayer->getMP(ATTR_CURRENT));
        } else if (pAttacker->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);
            if (pAttackerMI != NULL)
                pAttackerMI->addShortData(MODIFY_CURRENT_MP, pOusters->getMP(ATTR_CURRENT));
        } else
            Assert(false);
    }

    
    if (pTargetCreature->isDead()) {
        affectKillCount(pAttacker, pTargetCreature);
    }

    return Result;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void decreaseDurability(Creature* pCreature, Creature* pTargetCreature, SkillInfo* pSkillInfo, ModifyInfo* pMI1,
                        ModifyInfo* pMI2) {
    // #ifdef __TEST_SERVER__
    WORD Point = (pSkillInfo) ? (pSkillInfo->getConsumeMP() / 3) : 1;
    // #else
    //	WORD Point = (pSkillInfo)?pSkillInfo->getPoint():1;
    // #endif

    
    if (Point == 0)
        return;

    Item* pWeapon = NULL;
    Item* pGear = NULL;
    int slot = 0;
    int durDiff = 0;
    int CurDur = 0;
    int Result = 0;
    ulong value = 0;

    ////////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////////
    if (pCreature != NULL) {
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

            slot = Slayer::WEAR_RIGHTHAND;
            pWeapon = pSlayer->getWearItem((Slayer::WearPart)slot);

            
            if (pWeapon != NULL && canDecreaseDurability(pWeapon))
            //				&& !pWeapon->isUnique())
            {
                CurDur = pWeapon->getDurability();
                durDiff = Point;
                Result = max(0, CurDur - durDiff);

                if (Result == 0) 
                {
                    GCRemoveFromGear gcRemoveFromGear;
                    gcRemoveFromGear.setSlotID(slot);
                    pSlayer->takeOffItem((Slayer::WearPart)slot, false, true);

                    Player* pPlayer = pSlayer->getPlayer();
                    pPlayer->sendPacket(&gcRemoveFromGear);

                    
                    log(LOG_DESTROY_ITEM, pCreature->getName(), "", pWeapon->toString());

                    
                    pWeapon->setDurability(Result);
                    pWeapon->save(pCreature->getName(), STORAGE_GEAR, 0, slot, 0);

                    
                    pWeapon->destroy();
                    SAFE_DELETE(pWeapon);
                } else {
                    if (pMI1 == NULL) {
                        return;
                    }

                    pWeapon->setDurability(Result);

                    //					value = MAKEDWORD(slot, Result);
                    value = (DWORD)(slot) << 24 | (DWORD)(Result);
                    pMI1->addLongData(MODIFY_DURABILITY, value);

                    
                    // pWeapon->save(pCreature->getName(), STORAGE_GEAR, 0, slot, 0);
                }
            } // if (pWeapon != NULL)
        } // if (pCreature->isSlayer())
        else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

            slot = Vampire::WEAR_RIGHTHAND;
            pWeapon = pVampire->getWearItem((Vampire::WearPart)slot);

            
            if (pWeapon != NULL && canDecreaseDurability(pWeapon)) {
                CurDur = pWeapon->getDurability();
                durDiff = Point;
                Result = max(0, CurDur - durDiff);

                if (Result == 0) 
                {
                    GCRemoveFromGear gcRemoveFromGear;
                    gcRemoveFromGear.setSlotID(slot);
                    pVampire->takeOffItem((Vampire::WearPart)slot, false, true);

                    Player* pPlayer = pVampire->getPlayer();
                    pPlayer->sendPacket(&gcRemoveFromGear);

                    
                    log(LOG_DESTROY_ITEM, pCreature->getName(), "", pWeapon->toString());

                    
                    pWeapon->setDurability(Result);
                    pWeapon->save(pCreature->getName(), STORAGE_GEAR, 0, slot, 0);

                    
                    pWeapon->destroy();
                    SAFE_DELETE(pWeapon);
                } else {
                    if (pMI1 == NULL) {
                        return;
                    }

                    pWeapon->setDurability(Result);

                    //					value = MAKEDWORD(slot, Result);
                    value = (DWORD)(slot) << 24 | (DWORD)(Result);
                    pMI1->addLongData(MODIFY_DURABILITY, value);
                }
            } // if (pWeapon != NULL)
        } // if (pCreature->isVampire())
        else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

            slot = Ousters::WEAR_RIGHTHAND;
            pWeapon = pOusters->getWearItem((Ousters::WearPart)slot);

            
            if (pWeapon != NULL && canDecreaseDurability(pWeapon)) {
                CurDur = pWeapon->getDurability();
                durDiff = Point;
                Result = max(0, CurDur - durDiff);

                if (Result == 0) 
                {
                    GCRemoveFromGear gcRemoveFromGear;
                    gcRemoveFromGear.setSlotID(slot);
                    pOusters->takeOffItem((Ousters::WearPart)slot, false, true);

                    Player* pPlayer = pOusters->getPlayer();
                    pPlayer->sendPacket(&gcRemoveFromGear);

                    
                    log(LOG_DESTROY_ITEM, pCreature->getName(), "", pWeapon->toString());

                    
                    pWeapon->setDurability(Result);
                    pWeapon->save(pCreature->getName(), STORAGE_GEAR, 0, slot, 0);

                    
                    pWeapon->destroy();
                    SAFE_DELETE(pWeapon);
                } else {
                    if (pMI1 == NULL) {
                        return;
                    }

                    pWeapon->setDurability(Result);

                    //					value = MAKEDWORD(slot, Result);
                    value = (DWORD)(slot) << 24 | (DWORD)(Result);
                    pMI1->addLongData(MODIFY_DURABILITY, value);
                }
            } // if (pWeapon != NULL)
        } // if (pCreature->isOusters())
    } // if (pCreature != NULL)

    ////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////
    if (pTargetCreature != NULL) {
        
        if (pTargetCreature->isSlayer()) {
            slot = Random(0, Slayer::WEAR_MAX - 1);
            pGear = dynamic_cast<Slayer*>(pTargetCreature)->getWearItem((Slayer::WearPart)slot);
        } else if (pTargetCreature->isVampire()) {
            slot = Random(0, Vampire::VAMPIRE_WEAR_MAX - 1);
            pGear = dynamic_cast<Vampire*>(pTargetCreature)->getWearItem((Vampire::WearPart)slot);
        } else if (pTargetCreature->isOusters()) {
            slot = Random(0, Ousters::OUSTERS_WEAR_MAX - 1);
            pGear = dynamic_cast<Ousters*>(pTargetCreature)->getWearItem((Ousters::WearPart)slot);
        }

        
        
        if (pGear != NULL && canDecreaseDurability(pGear))
        //			&& !pGear->isUnique()
        //			&& pGear->getItemClass()!=Item::ITEM_CLASS_VAMPIRE_AMULET)
        {
            
            
            if (isTwohandWeapon(pGear)) {
                if (pTargetCreature->isSlayer())
                    slot = Slayer::WEAR_RIGHTHAND;
                else if (pTargetCreature->isVampire())
                    slot = Vampire::WEAR_RIGHTHAND;
                else if (pTargetCreature->isOusters())
                    slot = Ousters::WEAR_RIGHTHAND;
            }

            CurDur = pGear->getDurability();
            durDiff = Point * Random(1, 3);
            Result = max(0, CurDur - durDiff);

            if (Result == 0) {
                GCRemoveFromGear gcRemoveFromGear;
                gcRemoveFromGear.setSlotID(slot);
                if (pTargetCreature->isSlayer()) {
                    Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
                    Player* pPlayer = pTargetSlayer->getPlayer();

                    Assert(pTargetSlayer != NULL);
                    Assert(pPlayer != NULL);

                    pTargetSlayer->takeOffItem((Slayer::WearPart)slot, false, true);
                    pPlayer->sendPacket(&gcRemoveFromGear);
                } else if (pTargetCreature->isVampire()) {
                    Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
                    Player* pPlayer = pTargetVampire->getPlayer();

                    Assert(pTargetVampire != NULL);
                    Assert(pPlayer != NULL);

                    pTargetVampire->takeOffItem((Vampire::WearPart)slot, false, true);
                    pPlayer->sendPacket(&gcRemoveFromGear);
                } else if (pTargetCreature->isOusters()) {
                    Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetCreature);
                    Player* pPlayer = pTargetOusters->getPlayer();

                    Assert(pTargetOusters != NULL);
                    Assert(pPlayer != NULL);

                    pTargetOusters->takeOffItem((Ousters::WearPart)slot, false, true);
                    pPlayer->sendPacket(&gcRemoveFromGear);
                }

                
                log(LOG_DESTROY_ITEM, pTargetCreature->getName(), "", pGear->toString());

                
                pGear->save(pTargetCreature->getName(), STORAGE_GEAR, 0, slot, 0);
                pGear->destroy();
                SAFE_DELETE(pGear);
            } else {
                pGear->setDurability(Result);

                //				value = MAKEDWORD(slot, Result);
                value = (DWORD)(slot) << 24 | (DWORD)(Result);

                if (pMI2 == NULL) {
                    return;
                }

                pMI2->addLongData(MODIFY_DURABILITY, value);

                
                // pGear->save(pTargetCreature->getName(), STORAGE_GEAR, 0, slot, 0);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool canHit(Creature* pAttacker, Creature* pDefender, SkillType_t SkillType, SkillLevel_t SkillLevel) {
    
    //	if (pDefender->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
    //	{
    //		return false;
    //	}

    
    if (pAttacker->isSlayer()) {
        
        

        
        EffectRevealer* pEffectRevealer = NULL;
        if (pAttacker->isFlag(Effect::EFFECT_CLASS_REVEALER)) {
            pEffectRevealer = dynamic_cast<EffectRevealer*>(pAttacker->findEffect(Effect::EFFECT_CLASS_REVEALER));
            Assert(pEffectRevealer);
        }

        
        if (pDefender->isFlag(Effect::EFFECT_CLASS_HIDE)) {
            if (!pAttacker->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) &&
                !(pEffectRevealer != NULL && pEffectRevealer->canSeeHide(pDefender)))
                return false;
        }
        
        if (pDefender->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)) {
            if (!pAttacker->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) &&
                !(pEffectRevealer != NULL && pEffectRevealer->canSeeInvisibility(pDefender)))
                return false;
        }
    }

    
    
    switch (SkillType) {
    
    case SKILL_ATTACK_MELEE:
    case SKILL_BLOOD_DRAIN:
        if (pDefender != NULL) {
            if (pDefender->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT))
                return false;
        }
        return true;

    
    case SKILL_ATTACK_ARMS:
        return true;

    default:
        break;
    }

    
    SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
    Assert(pSkillInfo != NULL);

    uint TType = pSkillInfo->getTarget();

    if ((TType & TARGET_GROUND) && (pDefender->getMoveMode() == Creature::MOVE_MODE_WALKING)) {
        return true;
    } else if ((TType & TARGET_UNDERGROUND) && (pDefender->getMoveMode() == Creature::MOVE_MODE_BURROWING)) {
        return true;
    } else if ((TType & TARGET_AIR) && (pDefender->getMoveMode() == Creature::MOVE_MODE_FLYING)) {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
MP_t decreaseConsumeMP(Vampire* pVampire, SkillInfo* pSkillInfo) {
    Assert(pVampire != NULL);
    Assert(pSkillInfo != NULL);

    int OriginalMP = pSkillInfo->getConsumeMP();
    int MagicLevel = pSkillInfo->getLevel();
    int INTE = max(0, pVampire->getINT() - 20);
    int DecreaseAmount = 0;

    if (INTE <= MagicLevel) {
    } else if (MagicLevel < INTE && INTE <= (MagicLevel * 1.5)) {
        DecreaseAmount = getPercentValue(OriginalMP, 10);
    } else if ((MagicLevel * 1.5) < INTE && INTE <= (MagicLevel * 2.0)) {
        DecreaseAmount = getPercentValue(OriginalMP, 25);
    } else if ((MagicLevel * 2.0) < INTE && INTE <= (MagicLevel * 3.0)) {
        DecreaseAmount = getPercentValue(OriginalMP, 50);
    } else if ((MagicLevel * 3.0) < INTE && INTE <= (MagicLevel * 4.0)) {
        DecreaseAmount = getPercentValue(OriginalMP, 60);
    } else if ((MagicLevel * 4.0) < INTE && INTE <= (MagicLevel * 5.0)) {
        DecreaseAmount = getPercentValue(OriginalMP, 75);
    } else if ((MagicLevel * 5.0) < INTE && INTE <= (MagicLevel * 6.0)) {
        DecreaseAmount = getPercentValue(OriginalMP, 85);
    } else if ((MagicLevel * 6.0) < INTE && INTE <= (MagicLevel * 7.0)) {
        DecreaseAmount = getPercentValue(OriginalMP, 90);
    } else if ((MagicLevel * 7.0) < INTE) {
        DecreaseAmount = getPercentValue(OriginalMP, 95);
    }

    return (OriginalMP - DecreaseAmount);
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool hasEnoughMana(Creature* pCaster, int RequiredMP) {
    if (pCaster->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCaster);

        int decreaseRatio = pSlayer->getConsumeMPRatio();
        if (decreaseRatio != 0) {
            RequiredMP += getPercentValue(RequiredMP, decreaseRatio);
        }

        
        if (pSlayer->isFlag(Effect::EFFECT_CLASS_SACRIFICE)) {
            // cout << "RequiredMP : " << (int)RequiredMP << endl;
            int margin = RequiredMP - pSlayer->getMP(ATTR_CURRENT);
            // cout << "margin: " << (int)margin<< endl;

            
            
            if (margin > 0) {
                margin = (int)pSlayer->getHP(ATTR_CURRENT) * 2 - (int)margin;
                // cout << "margin: " << (int)margin<< endl;
                if (margin > 0)
                    return true;
            } else {
                return true;
            }
        } else {
            if (pSlayer->getMP(ATTR_CURRENT) >= (MP_t)RequiredMP)
                return true;
        }
    } else if (pCaster->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCaster);

        
        
        

        int decreaseRatio = pVampire->getConsumeMPRatio();
        if (decreaseRatio != 0) {
            RequiredMP += getPercentValue(RequiredMP, decreaseRatio);
        }

        if (pVampire->getHP(ATTR_CURRENT) > (HP_t)RequiredMP)
            return true;
    } else if (pCaster->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCaster);

        int decreaseRatio = pOusters->getConsumeMPRatio();
        if (decreaseRatio != 0) {
            RequiredMP += getPercentValue(RequiredMP, decreaseRatio);
        }

        if (pOusters->getMP(ATTR_CURRENT) >= (MP_t)RequiredMP)
            return true;
    } else if (pCaster->isMonster()) {
        
        
        
        return true;
    } else {
        cerr << "hasEnoughMana() : Invalid Creature Class" << endl;
        Assert(false);
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
int decreaseMana(Creature* pCaster, int MP, ModifyInfo& info) {
    Assert(pCaster != NULL);

    int RemainHP = 0;
    int RemainMP = 0;

    if (pCaster->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
        return 0;

    if (pCaster->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCaster);

        
        if (pSlayer->hasRankBonus(RankBonus::RANK_BONUS_MAGIC_BRAIN)) {
            RankBonus* pRankBonus = pSlayer->getRankBonus(RankBonus::RANK_BONUS_MAGIC_BRAIN);
            Assert(pRankBonus != NULL);

            MP -= getPercentValue(MP, pRankBonus->getPoint());
        }

        
        int decreaseRatio = pSlayer->getConsumeMPRatio();
        if (decreaseRatio != 0) {
            
            MP += getPercentValue(MP, decreaseRatio);
        }

        
        if (pSlayer->isFlag(Effect::EFFECT_CLASS_SACRIFICE)) {
            int margin = (int)MP - (int)pSlayer->getMP(ATTR_CURRENT);

            
            if (margin > 0) {
                
                pSlayer->setMP(0, ATTR_CURRENT);
                
                RemainHP = max(0, (int)(pSlayer->getHP(ATTR_CURRENT) - margin / 2));
                pSlayer->setHP(RemainHP, ATTR_CURRENT);

                info.addShortData(MODIFY_CURRENT_MP, pSlayer->getMP(ATTR_CURRENT));
                info.addShortData(MODIFY_CURRENT_HP, pSlayer->getHP(ATTR_CURRENT));
                return CONSUME_BOTH;
            }

            
            RemainMP = max(0, ((int)pSlayer->getMP(ATTR_CURRENT) - (int)MP));
            pSlayer->setMP(RemainMP, ATTR_CURRENT);

            info.addShortData(MODIFY_CURRENT_MP, pSlayer->getMP(ATTR_CURRENT));
            return CONSUME_MP;
        } else 
        {
            RemainMP = max(0, ((int)pSlayer->getMP(ATTR_CURRENT) - (int)MP));
            pSlayer->setMP(RemainMP, ATTR_CURRENT);

            info.addShortData(MODIFY_CURRENT_MP, pSlayer->getMP(ATTR_CURRENT));
            return CONSUME_MP;
        }
    } else if (pCaster->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCaster);

        
        if (pVampire->hasRankBonus(RankBonus::RANK_BONUS_WISDOM_OF_BLOOD)) {
            RankBonus* pRankBonus = pVampire->getRankBonus(RankBonus::RANK_BONUS_WISDOM_OF_BLOOD);
            Assert(pRankBonus != NULL);

            MP -= getPercentValue(MP, pRankBonus->getPoint());
        }

        
        int decreaseRatio = pVampire->getConsumeMPRatio();
        if (decreaseRatio != 0) {
            
            MP += getPercentValue(MP, decreaseRatio);
        }

        HP_t currentHP = pVampire->getHP(ATTR_CURRENT);
        RemainHP = max(0, ((int)currentHP - (int)MP));
        pVampire->setHP(RemainHP, ATTR_CURRENT);

        
        if (pVampire->isFlag(Effect::EFFECT_CLASS_MEPHISTO)) {
            HP_t maxHP = pVampire->getHP(ATTR_MAX);

            
            if (currentHP * 3 < maxHP) {
                Effect* pEffect = pVampire->findEffect(Effect::EFFECT_CLASS_MEPHISTO);
                if (pEffect != NULL) {
                    pEffect->setDeadline(0);
                } else {
                    pVampire->removeFlag(Effect::EFFECT_CLASS_MEPHISTO);
                }
                //				pVampire->getEffectManager()->deleteEffect( Effect::EFFECT_CLASS_MEPHISTO );
            }
        }

        info.addShortData(MODIFY_CURRENT_HP, pVampire->getHP(ATTR_CURRENT));
        return CONSUME_HP;
    } else if (pCaster->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCaster);

        
        int decreaseRatio = pOusters->getConsumeMPRatio();
        if (decreaseRatio != 0) {
            
            MP += getPercentValue(MP, decreaseRatio);
        }

        RemainMP = max(0, ((int)pOusters->getMP(ATTR_CURRENT) - (int)MP));
        pOusters->setMP(RemainMP, ATTR_CURRENT);

        info.addShortData(MODIFY_CURRENT_MP, pOusters->getMP(ATTR_CURRENT));
        return CONSUME_MP;
    } else if (pCaster->isMonster()) {
        
        
        
        cerr << "decreaseMana() : Monster don't have Mana" << endl;
        Assert(false);
    } else {
        cerr << "hasEnoughMana() : Invalid Creature Class" << endl;
        Assert(false);
    }

    return CONSUME_MP;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Range_t computeSkillRange(SkillSlot* pSkillSlot, SkillInfo* pSkillInfo) {
    Assert(pSkillSlot != NULL);
    Assert(pSkillInfo != NULL);

    
    Range_t SkillMinPoint = pSkillInfo->getMinRange();
    Range_t SkillMaxPoint = pSkillInfo->getMaxRange();

    
    SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

    
    Range_t Range = (int)(SkillMinPoint + (SkillMaxPoint - SkillMinPoint) * (double)(SkillLevel * 0.01));

    return Range;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Range_t getDistance(ZoneCoord_t Ox, ZoneCoord_t Oy, ZoneCoord_t Tx, ZoneCoord_t Ty) {
    double OriginX = Ox;
    double OriginY = Oy;
    double TargetX = Tx;
    double TargetY = Ty;

    double XOffset = pow(OriginX - TargetX, 2.0);
    double YOffset = pow(OriginY - TargetY, 2.0);
    Range_t range = (Range_t)(sqrt(XOffset + YOffset));

    return range;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool verifyDistance(Creature* pCreature, ZoneCoord_t X, ZoneCoord_t Y, Range_t Dist) {
    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    ZoneCoord_t cx = pCreature->getX();
    ZoneCoord_t cy = pCreature->getY();

    ZoneLevel_t AttackerZoneLevel = pZone->getZoneLevel(cx, cy);
    // ZoneLevel_t DefenderZoneLevel = pZone->getZoneLevel(X, Y);

    
    if ((AttackerZoneLevel & SAFE_ZONE) && (g_pPKZoneInfoManager->isPKZone(pZone->getZoneID()) || pZone->isHolyLand()))
        return false;

    
    
    if ((AttackerZoneLevel & SLAYER_SAFE_ZONE) && !pCreature->isSlayer())
        return false;
    
    
    else if ((AttackerZoneLevel & VAMPIRE_SAFE_ZONE) && !pCreature->isVampire())
        return false;
    
    
    else if ((AttackerZoneLevel & OUSTERS_SAFE_ZONE) && !pCreature->isOusters())
        return false;
    
    else if (AttackerZoneLevel & COMPLETE_SAFE_ZONE)
        return false;

    
    //	if (DefenderZoneLevel & COMPLETE_SAFE_ZONE)
    //		return false;

    if ((abs(cx - X) <= Dist) && (abs(cy - Y) <= Dist))
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool verifyDistance(Creature* pCreature, Creature* pTargetCreature, Range_t Dist) {
    Assert(pCreature != NULL);
    Assert(pTargetCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    int ox = pCreature->getX();
    int oy = pCreature->getY();
    int tx = pTargetCreature->getX();
    int ty = pTargetCreature->getY();

    ZoneLevel_t AttackerZoneLevel = pZone->getZoneLevel(ox, oy);
    ZoneLevel_t DefenderZoneLevel = pZone->getZoneLevel(tx, ty);

    
    if ((AttackerZoneLevel & SAFE_ZONE) && (g_pPKZoneInfoManager->isPKZone(pZone->getZoneID()) || pZone->isHolyLand()))
        return false;

    
    
    if ((AttackerZoneLevel & SLAYER_SAFE_ZONE) && !pCreature->isSlayer())
        return false;
    
    
    else if ((AttackerZoneLevel & VAMPIRE_SAFE_ZONE) && !pCreature->isVampire())
        return false;
    
    
    else if ((AttackerZoneLevel & OUSTERS_SAFE_ZONE) && !pCreature->isOusters())
        return false;
    
    else if (AttackerZoneLevel & COMPLETE_SAFE_ZONE)
        return false;

    
    
    if ((DefenderZoneLevel & SLAYER_SAFE_ZONE) && pTargetCreature->isSlayer())
        return false;
    
    
    else if ((DefenderZoneLevel & VAMPIRE_SAFE_ZONE) && pTargetCreature->isVampire())
        return false;
    
    
    else if ((DefenderZoneLevel & OUSTERS_SAFE_ZONE) && pTargetCreature->isOusters())
        return false;
    
    else if (DefenderZoneLevel & COMPLETE_SAFE_ZONE)
        return false;

    if ((abs(tx - ox) <= Dist) && (abs(ty - oy) <= Dist))
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool verifyRunTime(SkillSlot* pSkillSlot) {
    Assert(pSkillSlot != NULL);

    Timeval CurrentTime;
    Timeval LastTime = pSkillSlot->getRunTime();

    getCurrentTime(CurrentTime);

    if (CurrentTime > LastTime)
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool verifyRunTime(VampireSkillSlot* pSkillSlot) {
    Assert(pSkillSlot != NULL);

    Timeval CurrentTime;
    Timeval LastTime = pSkillSlot->getRunTime();

    getCurrentTime(CurrentTime);

    if (CurrentTime > LastTime)
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool verifyRunTime(OustersSkillSlot* pSkillSlot) {
    Assert(pSkillSlot != NULL);

    Timeval CurrentTime;
    Timeval LastTime = pSkillSlot->getRunTime();

    getCurrentTime(CurrentTime);

    if (CurrentTime > LastTime)
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool verifyPK(Creature* pAttacker, Creature* pDefender) {
    Zone* pZone = pDefender->getZone();
    Assert(pZone != NULL);

    if (pDefender != NULL && pAttacker != NULL) {
        if (pZone->getZoneID() == 1412 || pZone->getZoneID() == 1413) {
            if (pDefender->isPC() && pAttacker->isPC())
                return false;
        }

        if (pDefender->getCreatureClass() == pAttacker->getCreatureClass() && pAttacker->isPC()) {
            
            if (pZone->getZoneLevel() == NO_PK_ZONE)
                return false;

            
            int PartyID1 = pAttacker->getPartyID();
            int PartyID2 = pDefender->getPartyID();
            if (PartyID1 != 0 && PartyID1 == PartyID2)
                return false;

            if (pDefender->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) ||
                pDefender->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER)) {
                if (pAttacker->isPC() && !dynamic_cast<PlayerCreature*>(pAttacker)->hasEnemy(pDefender->getName()))
                    return false;
            }
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
bool checkZoneLevelToUseSkill(Creature* pCaster) {
    Assert(pCaster != NULL);

    if (pCaster->isFlag(Effect::EFFECT_CLASS_PLEASURE_EXPLOSION))
        return false;

    Zone* pZone = pCaster->getZone();
    Assert(pZone != NULL);

    ZoneCoord_t cx = pCaster->getX();
    ZoneCoord_t cy = pCaster->getY();
    ZoneLevel_t ZoneLevel = pZone->getZoneLevel(cx, cy);

    
    if ((ZoneLevel & SAFE_ZONE)) // && pZone->isHolyLand() )
        return false;

    if (pCaster->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET))
        return false;

     
    return true;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool checkZoneLevelToHitTarget(Creature* pTargetCreature) {
    Assert(pTargetCreature != NULL);

    Zone* pZone = pTargetCreature->getZone();
    Assert(pZone != NULL);

    ZoneCoord_t tx = pTargetCreature->getX();
    ZoneCoord_t ty = pTargetCreature->getY();
    ZoneLevel_t ZoneLevel = pZone->getZoneLevel(tx, ty);

    
    if ((ZoneLevel & SLAYER_SAFE_ZONE) && pTargetCreature->isSlayer())
        return false;
    
    else if ((ZoneLevel & VAMPIRE_SAFE_ZONE) && pTargetCreature->isVampire())
        return false;
    
    else if ((ZoneLevel & OUSTERS_SAFE_ZONE) && pTargetCreature->isOusters())
        return false;
    
    else if (ZoneLevel & COMPLETE_SAFE_ZONE)
        return false;

    return true;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void computeAlignmentChange(Creature* pTargetCreature, Damage_t Damage, Creature* pAttacker, ModifyInfo* pMI,
                            ModifyInfo* pAttackerMI) {
    Assert(pTargetCreature != NULL);

    
    if (g_pPKZoneInfoManager->isPKZone(pAttacker->getZoneID()))
        return;

    Zone* pZone = pTargetCreature->getZone();
    Assert(pZone != NULL);

    bool bSameRace = false;

    
    if (pAttacker != NULL) {
        
        
        
        // 2002.8.21. by sigi
        // int zoneID = pAttacker->getZone()->getZoneID();

        // zoneID==1005 || zoneID==1006)

        bSameRace = isSameRace(pTargetCreature, pAttacker);

        bool bPKOlympic = false;

        switch (pAttacker->getZoneID()) {
        case 1122:
        case 1131:
        case 1132:
        case 1133:
        case 1134:
            bPKOlympic = true;
            break;
        default:
            break;
        }

        
        if (!bSameRace && bPKOlympic && pTargetCreature->isPC() && pTargetCreature->isDead() &&
            !GDRLairManager::Instance().isGDRLairZone(pTargetCreature->getZoneID())) {
            PlayerCreature* pAttackPC = dynamic_cast<PlayerCreature*>(pAttacker);
            if (pAttackPC->getLevel() - 10 <= pTargetCreature->getLevel() &&
                !pTargetCreature->isFlag(Effect::EFFECT_CLASS_PK_COUNTED)) {
                addSimpleCreatureEffect(pTargetCreature, Effect::EFFECT_CLASS_PK_COUNTED, 6000, false);
                addOlympicStat(pAttackPC, 2);
            }
        }

        if (pZone->isPKZone())
            return;
    }

    
    if (bSameRace) {
        PlayerCreature* pAttackPC = dynamic_cast<PlayerCreature*>(pAttacker);
        PlayerCreature* pTargetPC = dynamic_cast<PlayerCreature*>(pTargetCreature);

        string AttackName = pAttackPC->getName();
        string TargetName = pTargetPC->getName();

        Alignment_t AttackAlignment = pAttackPC->getAlignment();
        Alignment_t TargetAlignment = pTargetPC->getAlignment();

        Alignment_t ModifyAlignment = 0;

        
        bool bdecrease = false;
        if (pTargetPC->isDead()) {
            ModifyAlignment = g_pAlignmentManager->getMultiplier(AttackAlignment, TargetAlignment); // Damage* 2

            if (ModifyAlignment < 0) {
                ModifyAlignment = ModifyAlignment * 10;
                bdecrease = true;
            } else if (ModifyAlignment > 0) {
                

                if (pAttackPC->getLevel() - 10 <= pTargetPC->getLevel() &&
                    !pTargetPC->isFlag(Effect::EFFECT_CLASS_PUNISH_COUNTED)) {
                    addOlympicStat(pAttackPC, 7);
                    addSimpleCreatureEffect(pTargetPC, Effect::EFFECT_CLASS_PUNISH_COUNTED, 6000, false);
                }

                if (pAttackPC->isSlayer()) {
                    Slayer* pAttackSlayer = dynamic_cast<Slayer*>(pAttackPC);
                    Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetPC);

                    SkillLevel_t AttackLevel = pAttackSlayer->getHighestSkillDomainLevel();
                    SkillLevel_t TargetLevel = pTargetSlayer->getHighestSkillDomainLevel();

                    if (AttackLevel > TargetLevel && AttackLevel != 0) {
                        ModifyAlignment = (Alignment_t)(ModifyAlignment * ((float)TargetLevel / (float)AttackLevel));
                    }
                } else if (pAttackPC->isVampire()) {
                    Vampire* pAttackVampire = dynamic_cast<Vampire*>(pAttackPC);
                    Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetPC);

                    Level_t AttackLevel = pAttackVampire->getLevel();
                    Level_t TargetLevel = pTargetVampire->getLevel();

                    if (AttackLevel > TargetLevel && AttackLevel != 0) {
                        ModifyAlignment = (Alignment_t)(ModifyAlignment * ((float)TargetLevel / (float)AttackLevel));
                    }
                } else if (pAttackPC->isOusters()) {
                    Ousters* pAttackOusters = dynamic_cast<Ousters*>(pAttackPC);
                    Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetPC);

                    Level_t AttackLevel = pAttackOusters->getLevel();
                    Level_t TargetLevel = pTargetOusters->getLevel();

                    if (AttackLevel > TargetLevel && AttackLevel != 0) {
                        ModifyAlignment = (Alignment_t)(ModifyAlignment * ((float)TargetLevel / (float)AttackLevel));
                    }
                }
            }

            pTargetPC->setPK(true);
        }

        Alignment_t ResultAlignment = AttackAlignment + ModifyAlignment;

        ResultAlignment = max(-10000, ResultAlignment);
        ResultAlignment = min(10000, ResultAlignment);

        EffectManager* pAttackEffectManager = pAttackPC->getEffectManager();
        EffectManager* pTargetEffectManager = pTargetPC->getEffectManager();

        
        if (!pAttackPC->hasEnemy(TargetName) && g_pAlignmentManager->getAlignmentType(TargetAlignment) >= NEUTRAL) {
            GCAddInjuriousCreature gcAddInjuriousCreature;
            gcAddInjuriousCreature.setName(AttackName);
            pTargetPC->getPlayer()->sendPacket(&gcAddInjuriousCreature);

            
            
            pTargetPC->addEnemy(AttackName);

            EffectEnemyErase* pEffectEnemyErase = new EffectEnemyErase(pTargetPC);
            pEffectEnemyErase->setDeadline(3000);
            pEffectEnemyErase->setEnemyName(AttackName);
            pEffectEnemyErase->create(TargetName);
            pTargetEffectManager->addEffect(pEffectEnemyErase);
        }

        
        if (pAttackPC->hasEnemy(TargetName) && pTargetPC->isDead()) {
            EffectEnemyErase* pAttackerEffect =
                (EffectEnemyErase*)pAttackEffectManager->findEffect(Effect::EFFECT_CLASS_ENEMY_ERASE, TargetName);

            if (pAttackerEffect != NULL) {
                
                
                Assert(pAttackerEffect != NULL);
                Assert(pAttackerEffect->getEffectClass() == Effect::EFFECT_CLASS_ENEMY_ERASE);
                
                pAttackerEffect->setDeadline(0);
            }
        }

        
        
        if (!(bdecrease && pAttackPC->hasEnemy(TargetName) &&
              g_pAlignmentManager->getAlignmentType(AttackAlignment) >= NEUTRAL)) {
            
            
            if (pAttackerMI && ModifyAlignment != 0) {
                if (pAttackPC->isSlayer()) {
                    Slayer* pSlayer = dynamic_cast<Slayer*>(pAttackPC);

                    pAttackerMI->addShortData(MODIFY_ALIGNMENT, ResultAlignment);
                    pSlayer->setAlignment(ResultAlignment);

                    WORD AlignmentSaveCount = pSlayer->getAlignmentSaveCount();
                    if (AlignmentSaveCount > ALIGNMENT_SAVE_PERIOD) {
                        char pField[80];
                        sprintf(pField, "ALIGNMENT=%d", ResultAlignment);
                        pSlayer->tinysave(pField);

                        AlignmentSaveCount = 0;
                    } else
                        AlignmentSaveCount++;

                    pSlayer->setAlignmentSaveCount(AlignmentSaveCount);
                } else if (pAttackPC->isVampire()) {
                    Vampire* pVampire = dynamic_cast<Vampire*>(pAttackPC);

                    pAttackerMI->addShortData(MODIFY_ALIGNMENT, ResultAlignment);
                    pVampire->setAlignment(ResultAlignment);

                    WORD AlignmentSaveCount = pVampire->getAlignmentSaveCount();
                    if (AlignmentSaveCount > ALIGNMENT_SAVE_PERIOD) {
                        char pField[80];
                        sprintf(pField, "ALIGNMENT=%d", ResultAlignment);
                        pVampire->tinysave(pField);

                        AlignmentSaveCount = 0;
                    } else
                        AlignmentSaveCount++;

                    pVampire->setAlignmentSaveCount(AlignmentSaveCount);
                } else if (pAttackPC->isOusters()) {
                    Ousters* pOusters = dynamic_cast<Ousters*>(pAttackPC);

                    pAttackerMI->addShortData(MODIFY_ALIGNMENT, ResultAlignment);
                    pOusters->setAlignment(ResultAlignment);

                    WORD AlignmentSaveCount = pOusters->getAlignmentSaveCount();
                    if (AlignmentSaveCount > ALIGNMENT_SAVE_PERIOD) {
                        char pField[80];
                        sprintf(pField, "ALIGNMENT=%d", ResultAlignment);
                        pOusters->tinysave(pField);

                        AlignmentSaveCount = 0;
                    } else
                        AlignmentSaveCount++;

                    pOusters->setAlignmentSaveCount(AlignmentSaveCount);
                }
            }

            
            if (bdecrease) {
                
                
                
                EffectAlignmentRecovery* pAttackerEffect =
                    (EffectAlignmentRecovery*)pAttackEffectManager->findEffect(Effect::EFFECT_CLASS_ALIGNMENT_RECOVERY);
                
                
                

                if (pAttackerEffect != NULL) {
                    
                    Alignment_t Amount = abs(ModifyAlignment / 10 * 9);

                    
                    Alignment_t Quantity = 10;

                    
                    int DelayProvider = 300;

                    
                    double temp = (double)((double)Amount / (double)Quantity);
                    int Period = (uint)floor(temp);

                    
                    Turn_t Deadline = Period * DelayProvider;

                    pAttackerEffect->setQuantity(Quantity);
                    pAttackerEffect->setPeriod(Period);
                    pAttackerEffect->setDeadline(Deadline);
                    pAttackerEffect->setDelay(DelayProvider);
                } else {
                    
                    
                    
                    

                    
                    Alignment_t Amount = abs(ModifyAlignment / 10 * 9);

                    
                    Alignment_t Quantity = 10;

                    
                    int DelayProvider = 300;

                    
                    double temp = (double)((double)Amount / (double)Quantity);
                    int Period = (uint)floor(temp);

                    
                    Turn_t Deadline = Period * DelayProvider;

                    
                    EffectAlignmentRecovery* pEffectAlignmentRecovery = new EffectAlignmentRecovery();

                    pEffectAlignmentRecovery->setTarget(pAttackPC);
                    pEffectAlignmentRecovery->setDeadline(Deadline);
                    pEffectAlignmentRecovery->setDelay(DelayProvider);
                    pEffectAlignmentRecovery->setNextTime(DelayProvider);
                    pEffectAlignmentRecovery->setQuantity(Quantity);
                    pEffectAlignmentRecovery->setPeriod(Period);

                    pAttackEffectManager->addEffect(pEffectAlignmentRecovery);
                }

                
                EffectEnemyErase* pDefenderEffect =
                    (EffectEnemyErase*)pTargetEffectManager->findEffect(Effect::EFFECT_CLASS_ENEMY_ERASE, AttackName);

                if (pDefenderEffect != NULL) {
                    
                    
                    Assert(pDefenderEffect != NULL);
                    Assert(pDefenderEffect->getEffectClass() == Effect::EFFECT_CLASS_ENEMY_ERASE);
                    
                    pDefenderEffect->setDeadline(36000);
                    pDefenderEffect->save(TargetName);
                }
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void increaseAlignment(Creature* pCreature, Creature* pEnemy, ModifyInfo& mi) {
    Assert(pCreature != NULL);
    Assert(pEnemy != NULL);

    
    if (g_pPKZoneInfoManager->isPKZone(pCreature->getZoneID()))
        return;

    
    if (pCreature->getZone() != NULL && pCreature->getZone()->isDynamicZone())
        return;

    
    if (!pEnemy->isDead())
        return;

    
    if (pEnemy->isNPC())
        return;
    if (pCreature->isSlayer() && pEnemy->isSlayer())
        return;
    if (pCreature->isVampire() && pEnemy->isVampire())
        return;
    if (pCreature->isOusters() && pEnemy->isOusters())
        return;

    Alignment_t OldAlignValue = 0;
    Alignment_t NewAlignValue = 0;

    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        
        OldAlignValue = pSlayer->getAlignment();

        
        if (OldAlignValue > 0)
            return;

        
        if (pEnemy->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pEnemy);
            Assert(pMonster != NULL);
            NewAlignValue = max(0, (int)(pMonster->getLevel() / 10));
        } else if (pEnemy->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pEnemy);
            Assert(pVampire != NULL);
            NewAlignValue = max(0, (int)(pVampire->getLevel() / 5));
        } else if (pEnemy->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pEnemy);
            Assert(pOusters != NULL);
            NewAlignValue = max(0, (int)(pOusters->getLevel() / 5));
        }

        NewAlignValue = OldAlignValue + NewAlignValue;

        if (OldAlignValue != NewAlignValue) {
            
            mi.addShortData(MODIFY_ALIGNMENT, NewAlignValue);

            WORD AlignmentSaveCount = pSlayer->getAlignmentSaveCount();
            if (AlignmentSaveCount > ALIGNMENT_SAVE_PERIOD) {
                pSlayer->saveAlignment(NewAlignValue);
                AlignmentSaveCount = 0;
            } else {
                pSlayer->setAlignment(NewAlignValue);
                AlignmentSaveCount++;
            }

            pSlayer->setAlignmentSaveCount(AlignmentSaveCount);
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        
        OldAlignValue = pVampire->getAlignment();

        
        if (OldAlignValue > 0)
            return;

        
        NewAlignValue = 0;
        if (pEnemy->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pEnemy);
            Assert(pMonster != NULL);
            NewAlignValue = max(0, (int)(pMonster->getLevel() / 10));
        } else if (pEnemy->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pEnemy);
            Assert(pSlayer != NULL);
            NewAlignValue = max(0, (int)(pSlayer->getSTR(ATTR_BASIC) + pSlayer->getDEX(ATTR_BASIC) +
                                         pSlayer->getINT(ATTR_BASIC) + pSlayer->getSkillDomainLevelSum()) /
                                       5);
        } else if (pEnemy->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pEnemy);
            Assert(pOusters != NULL);
            NewAlignValue = max(0, (int)(pOusters->getLevel() / 5));
        }

        NewAlignValue = OldAlignValue + NewAlignValue;

        if (OldAlignValue != NewAlignValue) {
            
            mi.addShortData(MODIFY_ALIGNMENT, NewAlignValue);

            WORD AlignmentSaveCount = pVampire->getAlignmentSaveCount();
            if (AlignmentSaveCount > ALIGNMENT_SAVE_PERIOD) {
                pVampire->saveAlignment(NewAlignValue);
                AlignmentSaveCount = 0;
            } else {
                pVampire->setAlignment(NewAlignValue);
                AlignmentSaveCount++;
            }
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        
        OldAlignValue = pOusters->getAlignment();

        
        if (OldAlignValue > 0)
            return;

        
        NewAlignValue = 0;
        if (pEnemy->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pEnemy);
            Assert(pMonster != NULL);
            NewAlignValue = max(0, (int)(pMonster->getLevel() / 10));
        } else if (pEnemy->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pEnemy);
            Assert(pSlayer != NULL);
            NewAlignValue = max(0, (int)(pSlayer->getSTR(ATTR_BASIC) + pSlayer->getDEX(ATTR_BASIC) +
                                         pSlayer->getINT(ATTR_BASIC) + pSlayer->getSkillDomainLevelSum()) /
                                       5);
        } else if (pEnemy->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pEnemy);
            Assert(pVampire != NULL);
            NewAlignValue = max(0, (int)(pVampire->getLevel() / 5));
        }

        NewAlignValue = OldAlignValue + NewAlignValue;

        if (OldAlignValue != NewAlignValue) {
            
            mi.addShortData(MODIFY_ALIGNMENT, NewAlignValue);

            WORD AlignmentSaveCount = pOusters->getAlignmentSaveCount();
            if (AlignmentSaveCount > ALIGNMENT_SAVE_PERIOD) {
                pOusters->saveAlignment(NewAlignValue);
                AlignmentSaveCount = 0;
            } else {
                pOusters->setAlignment(NewAlignValue);
                AlignmentSaveCount++;
            }
        }
    }

    
    Alignment beforeAlignment = g_pAlignmentManager->getAlignmentType(OldAlignValue);
    Alignment afterAlignment = g_pAlignmentManager->getAlignmentType(NewAlignValue);

    if (beforeAlignment != afterAlignment) {
        GCOtherModifyInfo gcOtherModifyInfo;
        gcOtherModifyInfo.setObjectID(pCreature->getObjectID());
        gcOtherModifyInfo.addShortData(MODIFY_ALIGNMENT, NewAlignValue);

        Zone* pZone = pCreature->getZone();
        Assert(pZone != NULL);
        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcOtherModifyInfo, pCreature);
    }
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void shareAttrExp(Slayer* pSlayer, Damage_t Damage, BYTE STRMultiplier, BYTE DEXMultiplier, BYTE INTMultiplier,
                  ModifyInfo& _ModifyInfo) {
    Assert(pSlayer != NULL);

    
    if (g_pPKZoneInfoManager->isPKZone(pSlayer->getZoneID()))
        return;

    
    if (pSlayer->getZone() != NULL && pSlayer->getZone()->isDynamicZone())
        return;

    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pSlayer->getPlayer());
    Assert(pGamePlayer != NULL);

    if (pGamePlayer->isPremiumPlay() ||
        pGamePlayer->isFamilyFreePass()) // pZone->isPayPlay() || pZone->isPremiumZone() )
    {
        Damage = getPercentValue(Damage, g_pVariableManager->getPremiumExpBonusPercent());
        STRMultiplier = getPercentValue(STRMultiplier, g_pVariableManager->getPremiumExpBonusPercent());
        DEXMultiplier = getPercentValue(DEXMultiplier, g_pVariableManager->getPremiumExpBonusPercent());
        INTMultiplier = getPercentValue(INTMultiplier, g_pVariableManager->getPremiumExpBonusPercent());
    }

    if (pGamePlayer->isPCRoomPlay()) {
        Damage = getPercentValue(Damage, g_pVariableManager->getPCRoomExpBonusPercent());
        STRMultiplier = getPercentValue(STRMultiplier, g_pVariableManager->getPCRoomExpBonusPercent());
        DEXMultiplier = getPercentValue(DEXMultiplier, g_pVariableManager->getPCRoomExpBonusPercent());
        INTMultiplier = getPercentValue(INTMultiplier, g_pVariableManager->getPCRoomExpBonusPercent());
    }

    int PartyID = pSlayer->getPartyID();
    if (PartyID != 0) {
        
        
        LocalPartyManager* pLPM = pSlayer->getLocalPartyManager();
        Assert(pLPM != NULL);
        pLPM->shareAttrExp(PartyID, pSlayer, Damage, STRMultiplier, DEXMultiplier, INTMultiplier, _ModifyInfo);
    } else {
        
        divideAttrExp(pSlayer, Damage, STRMultiplier, DEXMultiplier, INTMultiplier, _ModifyInfo);
    }
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void shareVampExp(Vampire* pVampire, Exp_t Point, ModifyInfo& _ModifyInfo) {
    Assert(pVampire != NULL);
    if (Point <= 0)
        return;

    
    if (g_pPKZoneInfoManager->isPKZone(pVampire->getZoneID()))
        return;

    
    if (pVampire->getZone() != NULL && pVampire->getZone()->isDynamicZone())
        return;

    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pVampire->getPlayer());
    Assert(pGamePlayer != NULL);

    if (pGamePlayer->isPremiumPlay() ||
        pGamePlayer->isFamilyFreePass()) // pZone->isPayPlay() || pZone->isPremiumZone() )
    {
        Point = getPercentValue(Point, g_pVariableManager->getPremiumExpBonusPercent());
    }

    if (pGamePlayer->isPCRoomPlay()) // pZone->isPayPlay() || pZone->isPremiumZone() )
    {
        Point = getPercentValue(Point, g_pVariableManager->getPCRoomExpBonusPercent());
    }

    int PartyID = pVampire->getPartyID();
    if (PartyID != 0) {
        
        
        LocalPartyManager* pLPM = pVampire->getLocalPartyManager();
        Assert(pLPM != NULL);
        pLPM->shareVampireExp(PartyID, pVampire, Point, _ModifyInfo);
    } else {
        
        increaseVampExp(pVampire, Point, _ModifyInfo);
    }
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void shareOustersExp(Ousters* pOusters, Exp_t Point, ModifyInfo& _ModifyInfo) {
    Assert(pOusters != NULL);
    if (Point <= 0)
        return;

    
    if (g_pPKZoneInfoManager->isPKZone(pOusters->getZoneID()))
        return;

    
    if (pOusters->getZone() != NULL && pOusters->getZone()->isDynamicZone())
        return;

    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pOusters->getPlayer());
    Assert(pGamePlayer != NULL);

    if (pGamePlayer->isPremiumPlay() || pGamePlayer->isFamilyFreePass()) {
        Point = getPercentValue(Point, g_pVariableManager->getPremiumExpBonusPercent());
    }

    if (pGamePlayer->isPCRoomPlay()) {
        Point = getPercentValue(Point, g_pVariableManager->getPCRoomExpBonusPercent());
    }

    int PartyID = pOusters->getPartyID();
    if (PartyID != 0) {
        
        
        LocalPartyManager* pLPM = pOusters->getLocalPartyManager();
        Assert(pLPM != NULL);
        pLPM->shareOustersExp(PartyID, pOusters, Point, _ModifyInfo);
    } else {
        
        increaseOustersExp(pOusters, Point, _ModifyInfo);
    }
}

 

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void divideAttrExp(Slayer* pSlayer, Damage_t Damage, BYTE STRMultiplier, BYTE DEXMultiplier, BYTE INTMultiplier,
                   ModifyInfo& _ModifyInfo, int numPartyMember) {
    Assert(pSlayer != NULL);

    
    if (STRMultiplier > DEXMultiplier && STRMultiplier > INTMultiplier) {
        pSlayer->divideAttrExp(ATTR_KIND_STR, Damage, _ModifyInfo);
        
    } else if (DEXMultiplier > STRMultiplier && DEXMultiplier > INTMultiplier) {
        pSlayer->divideAttrExp(ATTR_KIND_DEX, Damage, _ModifyInfo);
        
    } else if (INTMultiplier > STRMultiplier && INTMultiplier > DEXMultiplier) {
        pSlayer->divideAttrExp(ATTR_KIND_INT, Damage, _ModifyInfo);
    }

    return;

     
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void increaseSkillExp(Slayer* pSlayer, SkillDomainType_t DomainType, SkillSlot* pSkillSlot, SkillInfo* pSkillInfo,
                      ModifyInfo& _ModifyInfo) {
    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);
    Assert(pSkillInfo != NULL);
    
    // if ( pSkillInfo->getLevel() >= 150 ) return;
    // end

    
    if (g_pPKZoneInfoManager->isPKZone(pSlayer->getZoneID()))
        return;

    
    if (pSlayer->getZone() != NULL && pSlayer->getZone()->isDynamicZone())
        return;

    
    Level_t CurrentLevel = pSkillSlot->getExpLevel();

    
    Level_t DomainLevel = pSlayer->getSkillDomainLevel(DomainType);

    
    SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(DomainLevel);

    
    Level_t LimitLevel = g_pSkillInfoManager->getLimitLevelByDomainGrade(SkillGrade(Grade + 1));

    if (CurrentLevel < LimitLevel) {
        
        Exp_t MaxExp = pSkillInfo->getSubSkill();
        Exp_t OldExp = pSkillSlot->getExp();
        Exp_t NewExp;

        Exp_t plusExp = 1;

        if (g_pVariableManager->getEventActivate() == 1) {
            plusExp = plusExp * g_pVariableManager->getExpRatio() / 100;
            plusExp = getPercentValue(plusExp, g_pVariableManager->getPremiumExpBonusPercent());
            if (pSlayer->isFlag(Effect::EFFECT_CLASS_BONUS_EXP))
                plusExp *= 2;
        }

        
        if (isAffectExp2X())
            plusExp *= 2;

        NewExp = min(MaxExp, OldExp + plusExp);

        pSkillSlot->setExp(NewExp);

        SkillLevel_t NewLevel = (NewExp * 100 / MaxExp) + 1;
        NewLevel = min((int)NewLevel, 100);

        ulong longData = (((ulong)pSkillSlot->getSkillType()) << 16) | (ulong)(NewExp / 10);
        _ModifyInfo.addLongData(MODIFY_SKILL_EXP, longData);

        
        if (CurrentLevel != NewLevel) {
            pSkillSlot->setExpLevel(NewLevel);
            pSkillSlot->save();

            longData = (((ulong)pSkillSlot->getSkillType()) << 16) | (ulong)NewLevel;
            _ModifyInfo.addLongData(MODIFY_SKILL_LEVEL, longData);

            pSlayer->getGQuestManager()->skillLevelUp(pSkillSlot);
        } else {
            WORD SkillExpSaveCount = pSlayer->getSkillExpSaveCount();
            if (SkillExpSaveCount > SKILL_EXP_SAVE_PERIOD) {
                pSkillSlot->save();
                SkillExpSaveCount = 0;
            } else
                SkillExpSaveCount++;
            pSlayer->setSkillExpSaveCount(SkillExpSaveCount);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool increaseDomainExp(Slayer* pSlayer, SkillDomainType_t Domain, Exp_t Point, ModifyInfo& _ModifyInfo,
                       Level_t EnemyLevel, int TargetNum) {
    if (pSlayer == NULL || Point == 0 || TargetNum == 0)
        return false;
    if (pSlayer->isAdvanced())
        return false;

    
    if (g_pPKZoneInfoManager->isPKZone(pSlayer->getZoneID()))
        return false;

    
    if (pSlayer->getZone() != NULL && pSlayer->getZone()->isDynamicZone())
        return false;

    int PartyID = pSlayer->getPartyID();

    if (EnemyLevel != 0) {
        int levelDiff = (int)pSlayer->getLevel() - (int)EnemyLevel;

        //		cout << "enemyLevel : " << (int)EnemyLevel << " , TargetNum : " << TargetNum << endl;

        //		cout << "Point : " << Point << endl;

        if (levelDiff > 50)
            Point = getPercentValue(Point, 30);
        else if (levelDiff > 40)
            Point = getPercentValue(Point, 50);
        else if (levelDiff < -40)
            Point = getPercentValue(Point, 140);
        else if (levelDiff < -30)
            Point = getPercentValue(Point, 130);
        else if (levelDiff < -20)
            Point = getPercentValue(Point, 120);

        //		cout << "after level Point : " << Point << endl;
    }

    if (TargetNum != -1)
        Point = Point * (TargetNum + 1) / 3;

    //	cout << "after target Point : " << Point << endl;

    
    
    // by sigi. 2002.10.30
    Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
    if (pWeapon != NULL) {
        SkillLevel_t DomainLevel = pSlayer->getSkillDomainLevel(Domain);

        Point = computeSkillPointBonus(Domain, DomainLevel, pWeapon, Point);
    }


    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pSlayer->getPlayer());
    Assert(pGamePlayer != NULL);

    if (pGamePlayer->isPremiumPlay() ||
        pGamePlayer->isFamilyFreePass()) // pZone->isPayPlay() || pZone->isPremiumZone() )
    {
        Point = getPercentValueEx(Point, g_pVariableManager->getPremiumExpBonusPercent());
    }

    if (pGamePlayer->isPCRoomPlay()) // pZone->isPayPlay() || pZone->isPremiumZone() )
    {
        Point = getPercentValueEx(Point, g_pVariableManager->getPCRoomExpBonusPercent());
    }

    if (PartyID != 0) {
        LocalPartyManager* pLPM = pSlayer->getLocalPartyManager();
        Assert(pLPM != NULL);

        int nMemberSize = pLPM->getAdjacentMemberSize(PartyID, pSlayer);
        switch (nMemberSize) {
        case 2:
            Point = getPercentValue(Point, 110);
            break;
        case 3:
            Point = getPercentValue(Point, 120);
            break;
        case 4:
            Point = getPercentValue(Point, 130);
            break;
        case 5:
            Point = getPercentValue(Point, 140);
            break;
        case 6:
            Point = getPercentValue(Point, 150);
            break;
        default:
            break;
        }
    }

    
    if (g_pVariableManager->getExpRatio() > 100 && g_pVariableManager->getEventActivate() == 1)
        Point = getPercentValue(Point, g_pVariableManager->getExpRatio());

    if (pSlayer->isFlag(Effect::EFFECT_CLASS_BONUS_EXP))
        Point *= 2;

    
    if (isAffectExp2X())
        Point *= 2;

    Level_t CurDomainLevel = pSlayer->getSkillDomainLevel(Domain);
    Level_t NewDomainLevel = CurDomainLevel;
    SkillType_t LearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(Domain, CurDomainLevel);
    Exp_t NewGoalExp = 0;
    bool availiable = false;

    
    if (LearnSkillType != 0) {
        
        if (pSlayer->hasSkill(LearnSkillType)) {
            availiable = true;
        }
    } else {
        availiable = true;
    }

    if (availiable) {
        bool isLevelUp = false;

        
        Point = (Exp_t)getPercentValue(Point, DomainExpTimebandFactor[getZoneTimeband(pSlayer->getZone())]);

        
        // endl;

        
        // Point = max(2, (int)getPercentValue(Point, 150));

        
        
        Exp_t GoalExp = pSlayer->getGoalExp(Domain);
        //		Exp_t CurrentExp = pSlayer->getSkillDomainExp(Domain);

        
        NewGoalExp = max(0, (int)(GoalExp - Point));

        
        
        //		Exp_t DiffExp = max(0, (int)(GoalExp - NewGoalExp));

        //		Exp_t NewExp = 0;

        
        //		if( DiffExp == 0 && CurDomainLevel >= SLAYER_MAX_DOMAIN_LEVEL ) {
        //			NewExp  = CurrentExp + Point;
        //		} else {
        //			NewExp  = CurrentExp + DiffExp;
        //		}

        
        
        pSlayer->setGoalExp(Domain, NewGoalExp);
        //		pSlayer->setSkillDomainExp(Domain, NewExp);

        

        
        if (NewGoalExp == 0 && CurDomainLevel != SLAYER_MAX_DOMAIN_LEVEL) {
            
            NewDomainLevel = CurDomainLevel + 1;

            
            NewGoalExp = g_pSkillDomainInfoManager->getDomainInfo((SkillDomain)Domain, NewDomainLevel)->getGoalExp();

            pSlayer->setGoalExp(Domain, NewGoalExp);
            pSlayer->setSkillDomainLevel(Domain, NewDomainLevel);

            

            SkillType_t NewLearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(Domain, NewDomainLevel);

            
            if (NewLearnSkillType != 0) {
                
                if (pSlayer->hasSkill(NewLearnSkillType) == NULL) {
                    
                    
                    GCLearnSkillReady readyPacket;
                    readyPacket.setSkillDomainType((SkillDomainType_t)Domain);
                    // send packet
                    pSlayer->getPlayer()->sendPacket(&readyPacket);
                }
            }

            isLevelUp = true;
            
        }

        /*		if (DiffExp != 0)
                {
                    switch (Domain)
                    {
                        case SKILL_DOMAIN_BLADE:   _ModifyInfo.addLongData(MODIFY_BLADE_DOMAIN_EXP, NewGoalExp); break;
                        case SKILL_DOMAIN_SWORD:   _ModifyInfo.addLongData(MODIFY_SWORD_DOMAIN_EXP, NewGoalExp); break;
                        case SKILL_DOMAIN_GUN:     _ModifyInfo.addLongData(MODIFY_GUN_DOMAIN_EXP, NewGoalExp); break;
                        case SKILL_DOMAIN_HEAL:    _ModifyInfo.addLongData(MODIFY_HEAL_DOMAIN_EXP, NewGoalExp); break;
                        case SKILL_DOMAIN_ENCHANT: _ModifyInfo.addLongData(MODIFY_ENCHANT_DOMAIN_EXP, NewGoalExp);
           break; default: break;
                    }
                }*/

        Level_t DomainLevelSum = pSlayer->getSkillDomainLevelSum();

        
        
        if (isLevelUp && DomainLevelSum > SLAYER_MAX_DOMAIN_LEVEL) {
            SDomain ds[SKILL_DOMAIN_VAMPIRE];

            for (int i = 0; i < SKILL_DOMAIN_VAMPIRE; i++) {
                ds[i].DomainType = i;
                ds[i].DomainLevel = pSlayer->getSkillDomainLevel((SkillDomain)i);
            }

            
            stable_sort(ds, ds + SKILL_DOMAIN_VAMPIRE, isBig());

            // Range test moved ahead of the read and corrected from `>` to
            // `>=`: ds holds exactly SKILL_DOMAIN_VAMPIRE entries and is
            // 0-based, so the old form read ds[j] before testing j at all, and
            // the test it did make would then have admitted
            // j == SKILL_DOMAIN_VAMPIRE. Same read-before-check shape 89b2892
            // fixed in SkillHandlerManager.
            //
            // Unreachable in practice -- ds carries one entry per domain, so at
            // most one of them can equal Domain and a real run stops at j <= 1.
            // If it ever did fire there would be no other domain to demote, so
            // fall back to the last entry (ds is sorted descending by level, so
            // that is the weakest one) rather than reading past the array. It
            // must not return: the level-up bookkeeping below this block still
            // has to run.
            int j = 0;
            while (j < SKILL_DOMAIN_VAMPIRE && ds[j].DomainType == Domain) {
                j++;
            }

            if (j >= SKILL_DOMAIN_VAMPIRE) {
                cerr << "Out of Skill Domain Range!!!" << endl;
                Assert(false);
                j = SKILL_DOMAIN_VAMPIRE - 1;
            }


            SkillDomainType_t DownDomainType = ds[j].DomainType;
            Level_t DownDomainLevel = ds[j].DomainLevel;

            

            
            SkillType_t eraseSkillType = g_pSkillInfoManager->getSkillTypeByLevel(DownDomainType, DownDomainLevel);
            SkillSlot* pESkillSlot = pSlayer->hasSkill(eraseSkillType);
            if (pESkillSlot != NULL) {
                pESkillSlot->setDisable();
            }

            
            DownDomainLevel--;

            
            pSlayer->setSkillDomainLevel(DownDomainType, DownDomainLevel);

            
            
            Exp_t DownDomainGoalExp =
                g_pSkillDomainInfoManager->getDomainInfo((SkillDomain)DownDomainType, DownDomainLevel)->getGoalExp();
            //			Exp_t DownDomainSumExp  = g_pSkillDomainInfoManager->getDomainInfo((SkillDomain)DownDomainType,
            // DownDomainLevel)->getAccumExp();

            
            
            pSlayer->setGoalExp(DownDomainType, DownDomainGoalExp);
            //			pSlayer->setSkillDomainExp(DownDomainType, DownDomainSumExp);
            
            

            StringStream DownSave;
            if (DownDomainType == SKILL_DOMAIN_BLADE) {
                DownSave << "BladeLevel = " << (int)DownDomainLevel << ",BladeGoalExp = " << (int)DownDomainGoalExp;
                _ModifyInfo.addShortData(MODIFY_BLADE_DOMAIN_LEVEL, DownDomainLevel);
                _ModifyInfo.addLongData(MODIFY_BLADE_DOMAIN_EXP, DownDomainGoalExp);
            } else if (DownDomainType == SKILL_DOMAIN_SWORD) {
                DownSave << "SwordLevel = " << (int)DownDomainLevel << ",SwordGoalExp = " << (int)DownDomainGoalExp;
                _ModifyInfo.addLongData(MODIFY_SWORD_DOMAIN_EXP, DownDomainGoalExp);
                _ModifyInfo.addShortData(MODIFY_SWORD_DOMAIN_LEVEL, DownDomainLevel);
            } else if (DownDomainType == SKILL_DOMAIN_GUN) {
                DownSave << "GunLevel = " << (int)DownDomainLevel << ",GunGoalExp = " << (int)DownDomainGoalExp;
                _ModifyInfo.addLongData(MODIFY_GUN_DOMAIN_EXP, DownDomainGoalExp);
                _ModifyInfo.addShortData(MODIFY_GUN_DOMAIN_LEVEL, DownDomainLevel);
            } else if (DownDomainType == SKILL_DOMAIN_ENCHANT) {
                DownSave << "EnchantLevel = " << (int)DownDomainLevel << ",EnchantGoalExp = " << (int)DownDomainGoalExp;
                _ModifyInfo.addShortData(MODIFY_ENCHANT_DOMAIN_LEVEL, DownDomainLevel);
                _ModifyInfo.addLongData(MODIFY_ENCHANT_DOMAIN_EXP, DownDomainGoalExp);
            } else if (DownDomainType == SKILL_DOMAIN_HEAL) {
                DownSave << "HealLevel = " << (int)DownDomainLevel << ",HealGoalExp = " << (int)DownDomainGoalExp;
                _ModifyInfo.addShortData(MODIFY_HEAL_DOMAIN_LEVEL, DownDomainLevel);
                _ModifyInfo.addLongData(MODIFY_HEAL_DOMAIN_EXP, DownDomainGoalExp);
            } else if (DownDomainType == SKILL_DOMAIN_ETC) {
                DownSave << "ETCLevel = " << (int)DownDomainLevel << ",ETCGoalExp = " << (int)DownDomainGoalExp;
                _ModifyInfo.addShortData(MODIFY_ETC_DOMAIN_LEVEL, DownDomainLevel);
            } else {
            }

            
            pSlayer->tinysave(DownSave.toString());
        }

        WORD DomainExpSaveCount = pSlayer->getDomainExpSaveCount();

        if (Domain == SKILL_DOMAIN_BLADE) {
            StringStream attrsave;
            if (isLevelUp) {
                attrsave << "BladeLevel = " << (int)NewDomainLevel << ",BladeGoalExp = " << (int)NewGoalExp;
                _ModifyInfo.addShortData(MODIFY_BLADE_DOMAIN_LEVEL, NewDomainLevel);
                pSlayer->tinysave(attrsave.toString());
            } else {
                attrsave << "BladeGoalExp = " << (int)NewGoalExp;

                if (DomainExpSaveCount > DOMAIN_EXP_SAVE_PERIOD) {
                    pSlayer->tinysave(attrsave.toString());
                    DomainExpSaveCount = 0;
                } else
                    DomainExpSaveCount++;
            }

            _ModifyInfo.addLongData(MODIFY_BLADE_DOMAIN_EXP, NewGoalExp);
        } else if (Domain == SKILL_DOMAIN_SWORD) {
            StringStream attrsave;
            if (isLevelUp) {
                attrsave << "SwordLevel = " << (int)NewDomainLevel << ",SwordGoalExp = " << (int)NewGoalExp;
                _ModifyInfo.addShortData(MODIFY_SWORD_DOMAIN_LEVEL, NewDomainLevel);
                pSlayer->tinysave(attrsave.toString());
            } else {
                attrsave << "SwordGoalExp = " << (int)NewGoalExp;

                if (DomainExpSaveCount > DOMAIN_EXP_SAVE_PERIOD) {
                    pSlayer->tinysave(attrsave.toString());
                    DomainExpSaveCount = 0;
                } else
                    DomainExpSaveCount++;
            }

            _ModifyInfo.addLongData(MODIFY_SWORD_DOMAIN_EXP, NewGoalExp);
        } else if (Domain == SKILL_DOMAIN_GUN) {
            StringStream attrsave;
            if (isLevelUp) {
                attrsave << "GunLevel = " << (int)NewDomainLevel << ",GunGoalExp = " << (int)NewGoalExp;
                _ModifyInfo.addShortData(MODIFY_GUN_DOMAIN_LEVEL, NewDomainLevel);
                pSlayer->tinysave(attrsave.toString());
            } else {
                attrsave << "GunGoalExp = " << (int)NewGoalExp;

                if (DomainExpSaveCount > DOMAIN_EXP_SAVE_PERIOD) {
                    pSlayer->tinysave(attrsave.toString());
                    DomainExpSaveCount = 0;
                } else
                    DomainExpSaveCount++;
            }

            _ModifyInfo.addLongData(MODIFY_GUN_DOMAIN_EXP, NewGoalExp);
        } else if (Domain == SKILL_DOMAIN_ENCHANT) {
            StringStream attrsave;
            if (isLevelUp) {
                attrsave << "EnchantLevel = " << (int)NewDomainLevel << ",EnchantGoalExp = " << (int)NewGoalExp;
                _ModifyInfo.addShortData(MODIFY_ENCHANT_DOMAIN_LEVEL, NewDomainLevel);
                pSlayer->tinysave(attrsave.toString());
            } else {
                attrsave << "EnchantGoalExp = " << (int)NewGoalExp;

                if (DomainExpSaveCount > DOMAIN_EXP_SAVE_PERIOD) {
                    pSlayer->tinysave(attrsave.toString());
                    DomainExpSaveCount = 0;
                } else
                    DomainExpSaveCount++;
            }

            _ModifyInfo.addLongData(MODIFY_ENCHANT_DOMAIN_EXP, NewGoalExp);
        } else if (Domain == SKILL_DOMAIN_HEAL) {
            StringStream attrsave;
            if (isLevelUp) {
                attrsave << "HealLevel = " << (int)NewDomainLevel << ",HealGoalExp = " << (int)NewGoalExp;
                _ModifyInfo.addShortData(MODIFY_HEAL_DOMAIN_LEVEL, NewDomainLevel);
                pSlayer->tinysave(attrsave.toString());
            } else {
                attrsave << "HealGoalExp = " << (int)NewGoalExp;

                if (DomainExpSaveCount > DOMAIN_EXP_SAVE_PERIOD) {
                    pSlayer->tinysave(attrsave.toString());
                    DomainExpSaveCount = 0;
                } else
                    DomainExpSaveCount++;
            }

            _ModifyInfo.addLongData(MODIFY_HEAL_DOMAIN_EXP, NewGoalExp);
        } else if (Domain == SKILL_DOMAIN_ETC) {
            StringStream attrsave;
            if (isLevelUp) {
                attrsave << "ETCLevel = " << (int)NewDomainLevel << ",ETCGoalExp = " << (int)NewGoalExp;
                _ModifyInfo.addShortData(MODIFY_ETC_DOMAIN_LEVEL, NewDomainLevel);
                pSlayer->tinysave(attrsave.toString());
            } else {
                attrsave << "ETCGoalExp = " << (int)NewGoalExp;

                if (DomainExpSaveCount > DOMAIN_EXP_SAVE_PERIOD) {
                    pSlayer->tinysave(attrsave.toString());
                    DomainExpSaveCount = 0;
                } else
                    DomainExpSaveCount++;
            }

            _ModifyInfo.addLongData(MODIFY_ETC_DOMAIN_EXP, NewGoalExp);
        } else {
        }

        
        // by sigi. 2002.11.8
        if (isLevelUp && DomainLevelSum >= GRADE_GRAND_MASTER_LIMIT_LEVEL) {
            
            if (pSlayer->getHighestSkillDomainLevel() >= GRADE_GRAND_MASTER_LIMIT_LEVEL &&
                !pSlayer->isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER)) {
                EffectGrandMasterSlayer* pEffect = new EffectGrandMasterSlayer(pSlayer);
                pEffect->setDeadline(999999);

                pSlayer->getEffectManager()->addEffect(pEffect);

                
                pEffect->affect();
            } else if (pSlayer->getHighestSkillDomainLevel() == 130 || pSlayer->getHighestSkillDomainLevel() == 150) {
                Effect* pEffect = pSlayer->findEffect(Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER);
                if (pEffect != NULL)
                    pEffect->affect();
            }
        }

        pSlayer->setDomainExpSaveCount(DomainExpSaveCount);

        
        if (isLevelUp) {
            SLAYER_RECORD prev;
            pSlayer->getSlayerRecord(prev);
            pSlayer->initAllStat();
            healCreatureForLevelUp(pSlayer, _ModifyInfo, &prev);

            
            sendEffectLevelUp(pSlayer);

            checkFreeLevelLimit(pSlayer);
            pSlayer->whenQuestLevelUpgrade();

            
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void increaseVampExp(Vampire* pVampire, Exp_t Point, ModifyInfo& _ModifyInfo) {
    Assert(pVampire != NULL);
    if (Point <= 0)
        return;
    if (pVampire->isAdvanced())
        return;

    
    if (pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT))
        return;

    
    if (pVampire->getZone() != NULL && pVampire->getZone()->isDynamicZone())
        return;

    Level_t curLevel = pVampire->getLevel();

    
    if (g_pVariableManager->getExpRatio() > 100 && g_pVariableManager->getEventActivate() == 1)
        Point = getPercentValue(Point, g_pVariableManager->getExpRatio());

    if (pVampire->isFlag(Effect::EFFECT_CLASS_BONUS_EXP))
        Point *= 2;

    
    if (isAffectExp2X())
        Point *= 2;

    

     

    //	Exp_t OldExp = pVampire->getExp();

    Exp_t OldGoalExp = pVampire->getGoalExp();
    Exp_t NewGoalExp = max(0, (int)(OldGoalExp - Point));

    
    //	Exp_t DiffGoalExp = max(0, (int)(OldGoalExp - NewGoalExp));
    //	Exp_t NewExp      = OldExp + DiffGoalExp;

    //	pVampire->setExp(NewExp);
    pVampire->setGoalExp(NewGoalExp);

    //	_ModifyInfo.addLongData(MODIFY_VAMP_GOAL_EXP, NewGoalExp);

    
    
     
    if (NewGoalExp > 0 || curLevel == VAMPIRE_MAX_LEVEL) {
        _ModifyInfo.addLongData(MODIFY_VAMP_GOAL_EXP, NewGoalExp);
        WORD ExpSaveCount = pVampire->getExpSaveCount();

        
        
        if (ExpSaveCount > VAMPIRE_EXP_SAVE_PERIOD) {
            

            StringStream attrsave;
            //			attrsave << "Exp = " << NewExp << ", GoalExp = " << NewGoalExp;
            attrsave << "GoalExp = " << NewGoalExp;
            pVampire->tinysave(attrsave.toString());

            ExpSaveCount = 0;
        } else
            ExpSaveCount++;

        pVampire->setExpSaveCount(ExpSaveCount);
    } else {
        
        
        VAMPIRE_RECORD prev;
        pVampire->getVampireRecord(prev);

        curLevel++;

        pVampire->setLevel(curLevel);
        _ModifyInfo.addShortData(MODIFY_LEVEL, curLevel);

        // add bonus point
        Bonus_t bonus = pVampire->getBonus();

        //		if ((pVampire->getSTR(ATTR_BASIC) + pVampire->getDEX(ATTR_BASIC) + pVampire->getINT(ATTR_BASIC) +
        // pVampire->getBonus() - 60) < ((pVampire->getLevel() - 1) * 3))
        {
            
            
            bonus += 3;
        }

        pVampire->setBonus(bonus);
        _ModifyInfo.addShortData(MODIFY_BONUS_POINT, bonus);

        //		VampEXPInfo* pBeforeExpInfo = g_pVampEXPInfoManager->getVampEXPInfo(curLevel-1);
        VampEXPInfo* pNextExpInfo = g_pVampEXPInfoManager->getVampEXPInfo(curLevel);
        Exp_t NextGoalExp = pNextExpInfo->getGoalExp();

        pVampire->setGoalExp(NextGoalExp);
        _ModifyInfo.addLongData(MODIFY_VAMP_GOAL_EXP, NextGoalExp);
        

        StringStream sav;
        sav << "Level = "
            << (int)curLevel
            //			<< ",Exp = " << (int)pBeforeExpInfo->getAccumExp()
            << ",GoalExp = " << (int)NextGoalExp << ",Bonus = " << (int)bonus;
        pVampire->tinysave(sav.toString());

        
        SkillType_t NewLearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(SKILL_DOMAIN_VAMPIRE, curLevel);
        if (NewLearnSkillType != 0) {
            
            if (pVampire->hasSkill(NewLearnSkillType) == NULL) {
                
                
                GCLearnSkillReady readyPacket;
                readyPacket.setSkillDomainType(SKILL_DOMAIN_VAMPIRE);
                pVampire->getPlayer()->sendPacket(&readyPacket);
            }
        }

        healCreatureForLevelUp(pVampire, _ModifyInfo, &prev);

        
        sendEffectLevelUp(pVampire);

        // by sigi. 2002.11.19
        
        
        checkFreeLevelLimit(pVampire);
        pVampire->whenQuestLevelUpgrade();

        
        
        // by sigi. 2002.11.9
        if (curLevel >= GRADE_GRAND_MASTER_LIMIT_LEVEL &&
            !pVampire->isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE)) {
            EffectGrandMasterVampire* pEffect = new EffectGrandMasterVampire(pVampire);
            pEffect->setDeadline(999999);

            pVampire->getEffectManager()->addEffect(pEffect);

            
            pEffect->affect();
        } else if (curLevel == 130 || curLevel == 150) {
            Effect* pEffect = pVampire->findEffect(Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE);
            if (pEffect != NULL)
                pEffect->affect();
        }
    }
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void increaseOustersExp(Ousters* pOusters, Exp_t Point, ModifyInfo& _ModifyInfo) {
    Assert(pOusters != NULL);
    if (Point <= 0)
        return;
    if (pOusters->isAdvanced())
        return;

    Level_t curLevel = pOusters->getLevel();

    
    if (pOusters->getZone() != NULL && pOusters->getZone()->isDynamicZone())
        return;

    
    if (g_pVariableManager->getExpRatio() > 100 && g_pVariableManager->getEventActivate() == 1)
        Point = getPercentValue(Point, g_pVariableManager->getExpRatio());

    if (pOusters->isFlag(Effect::EFFECT_CLASS_BONUS_EXP))
        Point *= 2;

    
    if (isAffectExp2X())
        Point *= 2;

    
    Point = (Exp_t)getPercentValue(Point, DomainExpTimebandFactor[getZoneTimeband(pOusters->getZone())]);

    

     
    Exp_t OldGoalExp = pOusters->getGoalExp();
    Exp_t NewGoalExp = max(0, (int)(OldGoalExp - Point));

    
    //	Exp_t DiffGoalExp = max(0, (int)(OldGoalExp - NewGoalExp));
    //	Exp_t NewExp      = OldExp + DiffGoalExp;

    //	pOusters->setExp(NewExp);
    pOusters->setGoalExp(NewGoalExp);

    //	_ModifyInfo.addLongData(MODIFY_OUSTERS_EXP, NewExp);

    //	if ( NewGoalExp > 0 )
    if (NewGoalExp > 0 || curLevel == OUSTERS_MAX_LEVEL) {
        WORD ExpSaveCount = pOusters->getExpSaveCount();
        _ModifyInfo.addLongData(MODIFY_OUSTERS_GOAL_EXP, NewGoalExp);

        
        
        if (ExpSaveCount > OUSTERS_EXP_SAVE_PERIOD) {
            StringStream attrsave;
            attrsave << "GoalExp = " << NewGoalExp;
            pOusters->tinysave(attrsave.toString());

            ExpSaveCount = 0;
        } else
            ExpSaveCount++;

        pOusters->setExpSaveCount(ExpSaveCount);
    } else {
        
        OUSTERS_RECORD prev;
        pOusters->getOustersRecord(prev);

        curLevel++;
        pOusters->setLevel(curLevel);

        //		OustersEXPInfo* pBeforeExpInfo = g_pOustersEXPInfoManager->getOustersEXPInfo(curLevel-1);
        OustersEXPInfo* pNextExpInfo = g_pOustersEXPInfoManager->getOustersEXPInfo(curLevel);
        Exp_t NextGoalExp = pNextExpInfo->getGoalExp();

        // add bonus point
        Bonus_t bonus = pOusters->getBonus();
        SkillBonus_t skillBonus = pOusters->getSkillBonus();

        bonus += 3;
        skillBonus += pNextExpInfo->getSkillPointBonus();

        pOusters->setBonus(bonus);
        pOusters->setSkillBonus(skillBonus);

        _ModifyInfo.addShortData(MODIFY_LEVEL, curLevel);
        _ModifyInfo.addShortData(MODIFY_BONUS_POINT, bonus);
        _ModifyInfo.addShortData(MODIFY_SKILL_BONUS_POINT, skillBonus);

        pOusters->setGoalExp(NextGoalExp);
        _ModifyInfo.addLongData(MODIFY_OUSTERS_GOAL_EXP, NextGoalExp);

        StringStream sav;
        sav << "Level = "
            << (int)curLevel
            //			<< ",Exp = " << (int)pBeforeExpInfo->getAccumExp()
            << ",GoalExp = " << (int)NextGoalExp << ",Bonus = " << (int)bonus << ",SkillBonus = " << (int)skillBonus;
        pOusters->tinysave(sav.toString());

        
        SkillType_t NewLearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(SKILL_DOMAIN_OUSTERS, curLevel);
        if (NewLearnSkillType != 0) {
            
            if (pOusters->hasSkill(NewLearnSkillType) == NULL) {
                
                
                GCLearnSkillReady readyPacket;
                readyPacket.setSkillDomainType(SKILL_DOMAIN_OUSTERS);
                pOusters->getPlayer()->sendPacket(&readyPacket);
            }
        }

        healCreatureForLevelUp(pOusters, _ModifyInfo, &prev);

        
        sendEffectLevelUp(pOusters);

        // by sigi. 2002.11.19
        
        
        checkFreeLevelLimit(pOusters);
        pOusters->whenQuestLevelUpgrade();

        
        
        // by sigi. 2002.11.9
        if (curLevel >= GRADE_GRAND_MASTER_LIMIT_LEVEL &&
            !pOusters->isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_OUSTERS)) {
            EffectGrandMasterOusters* pEffect = new EffectGrandMasterOusters(pOusters);
            pEffect->setDeadline(999999);

            pOusters->getEffectManager()->addEffect(pEffect);

            
            pEffect->affect();
        } else if (curLevel == 130 || curLevel == 150) {
            Effect* pEffect = pOusters->findEffect(Effect::EFFECT_CLASS_GRAND_MASTER_OUSTERS);
            if (pEffect != NULL)
                pEffect->affect();
        }
    }
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void increaseFame(Creature* pCreature, uint amount) {
    if (pCreature == NULL)
        return;

    
    if (g_pPKZoneInfoManager->isPKZone(pCreature->getZoneID()))
        return;

    
    if (pCreature->getZone() != NULL && pCreature->getZone()->isDynamicZone())
        return;

    
    int PartyID = pCreature->getPartyID();
    if (PartyID != 0) {
        LocalPartyManager* pLPM = pCreature->getLocalPartyManager();
        Assert(pLPM != NULL);

        int nMemberSize = pLPM->getAdjacentMemberSize(PartyID, pCreature);
        switch (nMemberSize) {
        case 2:
            amount = getPercentValue(amount, 120);
            break;
        case 3:
            amount = getPercentValue(amount, 140);
            break;
        case 4:
            amount = getPercentValue(amount, 160);
            break;
        case 5:
            amount = getPercentValue(amount, 180);
            break;
        case 6:
            amount = getPercentValue(amount, 200);
            break;
        default:
            break;
        }
    }

    StringStream attrsave;

    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        Fame_t OldFame = pSlayer->getFame();
        Fame_t NewFame = OldFame + amount;

        NewFame = min(2000000000, (int)NewFame);

        if (NewFame != OldFame) {
            WORD FameSaveCount = pSlayer->getFameSaveCount();
            if (FameSaveCount > FAME_SAVE_PERIOD) {
                attrsave << "Fame = " << (int)NewFame;
                pSlayer->tinysave(attrsave.toString());

                FameSaveCount = 0;
            } else
                FameSaveCount++;

            pSlayer->setFameSaveCount(FameSaveCount);

            
            pSlayer->setFame(NewFame);
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        Fame_t OldFame = pVampire->getFame();
        Fame_t NewFame = OldFame + amount;

        NewFame = min(2000000000, (int)NewFame);

        if (NewFame != OldFame) {
            WORD FameSaveCount = pVampire->getFameSaveCount();
            if (FameSaveCount > FAME_SAVE_PERIOD) {
                attrsave << "Fame = " << (int)NewFame;
                pVampire->tinysave(attrsave.toString());

                FameSaveCount = 0;
            } else
                FameSaveCount++;

            pVampire->setFameSaveCount(FameSaveCount);

            
            pVampire->setFame(NewFame);
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
        Fame_t OldFame = pOusters->getFame();
        Fame_t NewFame = OldFame + amount;

        NewFame = min(2000000000, (int)NewFame);

        if (NewFame != OldFame) {
            WORD FameSaveCount = pOusters->getFameSaveCount();
            if (FameSaveCount > FAME_SAVE_PERIOD) {
                attrsave << "Fame = " << (int)NewFame;
                pOusters->tinysave(attrsave.toString());

                FameSaveCount = 0;
            } else
                FameSaveCount++;

            pOusters->setFameSaveCount(FameSaveCount);

            
            pOusters->setFame(NewFame);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int computeArmsWeaponSplashSize(Item* pWeapon, int ox, int oy, int tx, int ty) {
    Assert(pWeapon != NULL);
    Item::ItemClass IClass = pWeapon->getItemClass();
    int Splash = 0;

    
    if (IClass == Item::ITEM_CLASS_SG) {
        switch (getDistance(ox, oy, tx, ty)) {
        case 1:
            Splash = 6;
            break;
        case 2:
            Splash = 5;
            break;
        case 3:
            Splash = 4;
            break;
        case 4:
            Splash = 3;
            break;
        case 5:
            Splash = 2;
            break;
        default:
            break;
        }
    }

    return Splash;
}

int computeArmsWeaponDamageBonus(Item* pWeapon, int ox, int oy, int tx, int ty) {
    Assert(pWeapon != NULL);

    Item::ItemClass IClass = pWeapon->getItemClass();
    int DamageBonus = 0;

    int range = getDistance(ox, oy, tx, ty);

    if (IClass == Item::ITEM_CLASS_SR) {
        // DamageBonus = max(0, range - 3);
        //  by sigi. 2002.12.3
        DamageBonus = range + 3;
    }
    // by sigi. 2002.12.3
    else if (IClass == Item::ITEM_CLASS_AR || IClass == Item::ITEM_CLASS_SMG) {
        switch (range) {
        case 6:
            DamageBonus = 5;
            break;
        case 5:
            DamageBonus = 3;
            break;
        case 4:
            DamageBonus = 1;
            break;
        case 3:
            DamageBonus = 1;
            break;
        case 2:
            DamageBonus = 3;
            break;
        case 1:
            DamageBonus = 5;
            break;

        default:
            break;
        }
    } else if (IClass == Item::ITEM_CLASS_SG) {
        // by sigi. 2002.12.3
        DamageBonus = max(0, 6 - range);
    }

    return DamageBonus;
}

int computeArmsWeaponToHitBonus(Item* pWeapon, int ox, int oy, int tx, int ty) {
    Assert(pWeapon != NULL);

    Item::ItemClass IClass = pWeapon->getItemClass();
    int ToHitBonus = 0;

    if (IClass == Item::ITEM_CLASS_SR) {
        int range = getDistance(ox, oy, tx, ty);
        ToHitBonus = max(0, range - 1);
    } else if (IClass == Item::ITEM_CLASS_SG) {
        switch (getDistance(ox, oy, tx, ty)) {
        case 1:
            ToHitBonus = 20;
            break;
        case 2:
            ToHitBonus = 15;
            break;
        case 3:
            ToHitBonus = 10;
            break;
        case 4:
            ToHitBonus = 0;
            break;
        case 5:
            ToHitBonus = 0;
            break;
        default:
            break;
        }
    }

    return ToHitBonus;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int getSplashVictims(Zone* pZone, int cx, int cy, Creature::CreatureClass CClass, list<Creature*>& creatureList,
                     int splash) {
    VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

    
    
    if (CClass == Creature::CREATURE_CLASS_SLAYER) {
        if (rect.ptInRect(cx, cy)) {
            Tile& rTile = pZone->getTile(cx, cy);

            if (rTile.hasCreature(Creature::MOVE_MODE_WALKING)) {
                Creature* pCreature = rTile.getCreature(Creature::MOVE_MODE_WALKING);
                if (pCreature->getCreatureClass() == CClass) {
                    creatureList.push_back(pCreature);
                }
            }
            
            if (rTile.hasCreature(Creature::MOVE_MODE_FLYING)) {
                Creature* pCreature = rTile.getCreature(Creature::MOVE_MODE_FLYING);
                if (pCreature->getCreatureClass() == CClass) {
                    creatureList.push_back(pCreature);
                }
            }
        }

        return (int)creatureList.size();
    }

    vector<Creature*> creatureVector;
    vector<int> pickedVector;

    for (int i = 0; i < 9; i++) {
        int tilex = cx + dirMoveMask[i].x;
        int tiley = cy + dirMoveMask[i].y;

        if (rect.ptInRect(tilex, tiley)) {
            Tile& rTile = pZone->getTile(tilex, tiley);

            if (rTile.hasCreature(Creature::MOVE_MODE_WALKING)) {
                Creature* pCreature = rTile.getCreature(Creature::MOVE_MODE_WALKING);

                if (CClass == Creature::CREATURE_CLASS_MAX) {
                    
                    creatureVector.push_back(pCreature);
                } else if (pCreature->getCreatureClass() == CClass) {
                    
                    creatureVector.push_back(pCreature);
                }
            }

            if (rTile.hasCreature(Creature::MOVE_MODE_FLYING)) {
                Creature* pCreature = rTile.getCreature(Creature::MOVE_MODE_FLYING);
                if (CClass == Creature::CREATURE_CLASS_MAX) {
                    
                    creatureVector.push_back(pCreature);
                } else if (pCreature->getCreatureClass() == CClass) {
                    
                    creatureVector.push_back(pCreature);
                }
            }
        }
    }

    
    
    if ((int)creatureVector.size() <= splash) {
        for (int i = 0; i < (int)creatureVector.size(); i++) {
            creatureList.push_back(creatureVector[i]);
        }
    }
    
    
    else {
        
        
        
        
        
        
        // (0, 1, 3, 4, 5, 5...)
        
        
        int Indexes[50] =
            {
                -1,
            },
            i;
        int size = creatureVector.size();
        for (i = 0; i < size; i++) {
            Indexes[i] = i;
        }

        for (i = 0; i < splash; i++) {
            int index = rand() % size;
            int realIndex = Indexes[index];
            creatureList.push_back(creatureVector[realIndex]);

            for (int m = index + 1; m < size; m++) {
                Indexes[m - 1] = Indexes[m];
            }

            size--;
        }
    }

    return (int)creatureList.size();
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int getSplashVictims(Zone* pZone, int cx, int cy, Creature::CreatureClass CClass, list<Creature*>& creatureList,
                     int splash, int range) {
    VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

    vector<Creature*> creatureVector;
    vector<int> pickedVector;

    for (int y = -range; y <= range; y++) {
        for (int x = -range; x <= range; x++) {
            int tilex = cx + x;
            int tiley = cy + y;
            ;

            if (rect.ptInRect(tilex, tiley)) {
                Tile& rTile = pZone->getTile(tilex, tiley);

                if (rTile.hasCreature(Creature::MOVE_MODE_WALKING)) {
                    Creature* pCreature = rTile.getCreature(Creature::MOVE_MODE_WALKING);

                    if (CClass == Creature::CREATURE_CLASS_MAX) {
                        
                        creatureVector.push_back(pCreature);
                    } else if (pCreature->getCreatureClass() == CClass) {
                        
                        creatureVector.push_back(pCreature);
                    }
                }

                if (rTile.hasCreature(Creature::MOVE_MODE_FLYING)) {
                    Creature* pCreature = rTile.getCreature(Creature::MOVE_MODE_FLYING);
                    if (CClass == Creature::CREATURE_CLASS_MAX) {
                        
                        creatureVector.push_back(pCreature);
                    } else if (pCreature->getCreatureClass() == CClass) {
                        
                        creatureVector.push_back(pCreature);
                    }
                }
            }
        }
    }

    
    
    if ((int)creatureVector.size() <= splash) {
        for (int i = 0; i < (int)creatureVector.size(); i++) {
            creatureList.push_back(creatureVector[i]);
        }
    }
    
    
    else {
        
        
        
        
        
        
        // (0, 1, 3, 4, 5, 5...)
        
        
        int Indexes[50] =
            {
                -1,
            },
            i;
        int size = creatureVector.size();
        for (i = 0; i < size; i++) {
            Indexes[i] = i;
        }

        for (i = 0; i < splash; i++) {
            int index = rand() % size;
            int realIndex = Indexes[index];
            creatureList.push_back(creatureVector[realIndex]);

            for (int m = index + 1; m < size; m++) {
                Indexes[m - 1] = Indexes[m];
            }

            size--;
        }
    }

    return (int)creatureList.size();
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void healCreatureForLevelUp(Slayer* pSlayer, ModifyInfo& _ModifyInfo, SLAYER_RECORD* prev) {
    
    pSlayer->initAllStat();

    
    pSlayer->sendRealWearingInfo();
    pSlayer->addModifyInfo(*prev, _ModifyInfo);

    if (pSlayer->isDead())
        return;

    
    HP_t OldHP = pSlayer->getHP(ATTR_CURRENT);
    HP_t OldMP = pSlayer->getMP(ATTR_CURRENT);

    
    pSlayer->setHP(pSlayer->getHP(ATTR_MAX), ATTR_CURRENT);
    pSlayer->setMP(pSlayer->getMP(ATTR_MAX), ATTR_CURRENT);

    HP_t NewHP = pSlayer->getHP(ATTR_CURRENT);
    HP_t NewMP = pSlayer->getMP(ATTR_CURRENT);

    
    if (OldHP != NewHP) {
        _ModifyInfo.addShortData(MODIFY_CURRENT_HP, NewHP);

        
        GCStatusCurrentHP gcStatusCurrentHP;
        gcStatusCurrentHP.setObjectID(pSlayer->getObjectID());
        gcStatusCurrentHP.setCurrentHP(NewHP);
        Zone* pZone = pSlayer->getZone();
        Assert(pZone != NULL);
        pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcStatusCurrentHP, pSlayer);
    }

    
    if (OldMP != NewMP) {
        _ModifyInfo.addShortData(MODIFY_CURRENT_MP, NewMP);
    }

    //	pSlayer->sendModifyInfo(*prev);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void healCreatureForLevelUp(Vampire* pVampire, ModifyInfo& _ModifyInfo, VAMPIRE_RECORD* prev) {
    
    pVampire->initAllStat();

    
    pVampire->sendRealWearingInfo();
    pVampire->addModifyInfo(*prev, _ModifyInfo);

    if (pVampire->isDead())
        return;

    HP_t OldHP = pVampire->getHP(ATTR_CURRENT);

    
    pVampire->setHP(pVampire->getHP(ATTR_MAX), ATTR_CURRENT);

    HP_t NewHP = pVampire->getHP(ATTR_CURRENT);

    
    if (OldHP != NewHP) {
        _ModifyInfo.addShortData(MODIFY_CURRENT_HP, NewHP);

        
        GCStatusCurrentHP gcStatusCurrentHP;
        gcStatusCurrentHP.setObjectID(pVampire->getObjectID());
        gcStatusCurrentHP.setCurrentHP(NewHP);
        Zone* pZone = pVampire->getZone();
        Assert(pZone != NULL);
        pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcStatusCurrentHP, pVampire);
    }

    //	pVampire->sendModifyInfo(*prev);
}

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
void executeSkillFailNormal(Creature* pCreature, SkillType_t SkillType, Creature* pTargetCreature, BYTE Grade) {
    Assert(pCreature != NULL);

    if (pCreature->isPC()) {
        GCSkillFailed1 gcSkillFailed1;
        gcSkillFailed1.setSkillType(SkillType);
        gcSkillFailed1.setGrade(Grade);
        (pCreature->getPlayer())->sendPacket(&gcSkillFailed1);
    }

    GCSkillFailed2 gcSkillFailed2;
    gcSkillFailed2.setSkillType(SkillType);
    gcSkillFailed2.setObjectID(pCreature->getObjectID());
    gcSkillFailed2.setGrade(Grade);

    
    
    
    
    if (pTargetCreature != NULL) {
        gcSkillFailed2.setTargetObjectID(pTargetCreature->getObjectID());
    } else {
        gcSkillFailed2.setTargetObjectID(0);
    }

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcSkillFailed2, pCreature);
}

//////////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////////
void executeAbsorbSoulSkillFail(Creature* pCreature, SkillType_t SkillType, ObjectID_t TargetObjectID, bool bBroadcast,
                                bool bSendTwice) {
    Assert(pCreature != NULL);

    
    if (pCreature->isPC()) {
        GCSkillFailed1 gcSkillFailed1;
        gcSkillFailed1.setSkillType(SkillType);
        (pCreature->getPlayer())->sendPacket(&gcSkillFailed1);
        if (bSendTwice)
            (pCreature->getPlayer())->sendPacket(&gcSkillFailed1);
    }

    if (bBroadcast) {
        GCSkillFailed2 gcSkillFailed2;
        gcSkillFailed2.setSkillType(SkillType);
        gcSkillFailed2.setObjectID(pCreature->getObjectID());
        gcSkillFailed2.setTargetObjectID(TargetObjectID);

        Zone* pZone = pCreature->getZone();
        Assert(pZone != NULL);

        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcSkillFailed2, pCreature);
    }
}

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
void executeSkillFailNormalWithGun(Creature* pCreature, SkillType_t SkillType, Creature* pTargetCreature,
                                   BYTE RemainBullet) {
    Assert(pCreature != NULL);

    if (pCreature->isPC()) {
        GCSkillFailed1 gcSkillFailed1;
        gcSkillFailed1.setSkillType(SkillType);
        gcSkillFailed1.addShortData(MODIFY_BULLET, RemainBullet);
        (pCreature->getPlayer())->sendPacket(&gcSkillFailed1);
    }

    GCSkillFailed2 gcSkillFailed2;
    gcSkillFailed2.setSkillType(SkillType);
    gcSkillFailed2.setObjectID(pCreature->getObjectID());

    
    
    
    
    if (pTargetCreature != NULL) {
        gcSkillFailed2.setTargetObjectID(pTargetCreature->getObjectID());
    } else {
        gcSkillFailed2.setTargetObjectID(0);
    }

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcSkillFailed2, pCreature);
}

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
void executeSkillFailException(Creature* pCreature, SkillType_t SkillType, BYTE Grade) {
    // by sigi. 2002.5.8
    //	Assert(pCreature != NULL);

    if (pCreature != NULL && pCreature->isPC()) {
        GCSkillFailed1 gcSkillFailed1;
        gcSkillFailed1.setSkillType(SkillType);
        gcSkillFailed1.setGrade(Grade);

        Player* pPlayer = pCreature->getPlayer();
        pPlayer->sendPacket(&gcSkillFailed1);
    }
}

//////////////////////////////////////////////////////////////////////////////

// -- 2003.04.19 by bezz
//////////////////////////////////////////////////////////////////////////////
void healCreatureForLevelUp(Ousters* pOusters, ModifyInfo& _ModifyInfo, OUSTERS_RECORD* prev) {
    
    pOusters->initAllStat();

    
    pOusters->sendRealWearingInfo();
    pOusters->addModifyInfo(*prev, _ModifyInfo);

    if (pOusters->isDead())
        return;

    HP_t OldHP = pOusters->getHP(ATTR_CURRENT);
    MP_t OldMP = pOusters->getMP(ATTR_CURRENT);

    
    pOusters->setHP(pOusters->getHP(ATTR_MAX), ATTR_CURRENT);
    pOusters->setMP(pOusters->getMP(ATTR_MAX), ATTR_CURRENT);

    HP_t NewHP = pOusters->getHP(ATTR_CURRENT);
    MP_t NewMP = pOusters->getMP(ATTR_CURRENT);

    
    if (OldHP != NewHP) {
        _ModifyInfo.addShortData(MODIFY_CURRENT_HP, NewHP);

        
        GCStatusCurrentHP gcStatusCurrentHP;
        gcStatusCurrentHP.setObjectID(pOusters->getObjectID());
        gcStatusCurrentHP.setCurrentHP(NewHP);
        Zone* pZone = pOusters->getZone();
        Assert(pZone != NULL);
        pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &gcStatusCurrentHP, pOusters);
    }

    if (OldMP != NewMP)
        _ModifyInfo.addShortData(MODIFY_CURRENT_MP, NewMP);

    //	pOusters->sendModifyInfo(*prev);
}



// by sigi. 2002.9.10
void decreaseHP(Zone* pZone, Creature* pCreature, int Damage, ObjectID_t attackerObjectID) {
    if (!(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE)
        
        && !pCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

            HP_t CurrentHP = pSlayer->getHP(ATTR_CURRENT);

            if (CurrentHP > 0) {
                HP_t RemainHP = max(0, CurrentHP - (int)Damage);

                pSlayer->setHP(RemainHP, ATTR_CURRENT);

                GCModifyInformation gcMI;
                gcMI.addShortData(MODIFY_CURRENT_HP, RemainHP);
                pSlayer->getPlayer()->sendPacket(&gcMI);

                
                GCStatusCurrentHP pkt;
                pkt.setObjectID(pSlayer->getObjectID());
                pkt.setCurrentHP(RemainHP);
                pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &pkt);
            }
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

            HP_t CurrentHP = pVampire->getHP(ATTR_CURRENT);

            if (CurrentHP > 0) {
                HP_t RemainHP = max(0, CurrentHP - (int)Damage);

                pVampire->setHP(RemainHP, ATTR_CURRENT);

                GCModifyInformation gcMI;
                gcMI.addShortData(MODIFY_CURRENT_HP, RemainHP);
                pVampire->getPlayer()->sendPacket(&gcMI);

                
                GCStatusCurrentHP pkt;
                pkt.setObjectID(pVampire->getObjectID());
                pkt.setCurrentHP(RemainHP);
                pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &pkt);
            }
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

            HP_t CurrentHP = pOusters->getHP(ATTR_CURRENT);

            if (CurrentHP > 0) {
                HP_t RemainHP = max(0, CurrentHP - (int)Damage);

                pOusters->setHP(RemainHP, ATTR_CURRENT);

                GCModifyInformation gcMI;
                gcMI.addShortData(MODIFY_CURRENT_HP, RemainHP);
                pOusters->getPlayer()->sendPacket(&gcMI);

                
                GCStatusCurrentHP pkt;
                pkt.setObjectID(pOusters->getObjectID());
                pkt.setCurrentHP(RemainHP);
                pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &pkt);
            }
        } else if (pCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pCreature);

            HP_t CurrentHP = pMonster->getHP(ATTR_CURRENT);

            if (CurrentHP > 0) {
                HP_t RemainHP = max(0, CurrentHP - (int)Damage);

                pMonster->setHP(RemainHP, ATTR_CURRENT);

                
                GCStatusCurrentHP pkt;
                pkt.setObjectID(pMonster->getObjectID());
                pkt.setCurrentHP(RemainHP);
                pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &pkt);
            }
        }

        
        // by sigi. 2002.9.9
        if (attackerObjectID != 0 && pCreature->isDead()) {
            Creature* pAttacker = pZone->getCreature(attackerObjectID);

            if (pAttacker != NULL) {
                affectKillCount(pAttacker, pCreature);
            }
        }
    }
}

//----------------------------------------------------------------------
// Set Direction To Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
const float BASIS_DIRECTION_LOW = 0.35f;
const float BASIS_DIRECTION_HIGH = 3.0f;

Dir_t getDirectionToPosition(int originX, int originY, int destX, int destY) {
    int stepX = destX - originX, stepY = destY - originY;

    
    float k = (stepX == 0) ? 0 : (float)(stepY) / stepX; 

    //--------------------------------------------------
    
    //--------------------------------------------------
    if (stepY == 0) {
        
        // - -;;
        if (stepX == 0)
            return DOWN;
        else if (stepX > 0)
            return RIGHT;
        else
            return LEFT;
    } else if (stepY < 0) 
    {
        
        if (stepX == 0) {
            return UP;
        }
        
        else if (stepX > 0) {
            if (k < -BASIS_DIRECTION_HIGH)
                return UP;
            else if (k <= -BASIS_DIRECTION_LOW)
                return RIGHTUP;
            else
                return RIGHT;
        }
        
        else {
            if (k > BASIS_DIRECTION_HIGH)
                return UP;
            else if (k >= BASIS_DIRECTION_LOW)
                return LEFTUP;
            else
                return LEFT;
        }
    }
    
    else {
        
        if (stepX == 0) {
            return DOWN;
        }
        
        else if (stepX > 0) {
            if (k > BASIS_DIRECTION_HIGH)
                return DOWN;
            else if (k >= BASIS_DIRECTION_LOW)
                return RIGHTDOWN;
            else
                return RIGHT;
        }
        
        else {
            if (k < -BASIS_DIRECTION_HIGH)
                return DOWN;
            else if (k <= -BASIS_DIRECTION_LOW)
                return LEFTDOWN;
            else
                return LEFT;
        }
    }
}


Exp_t computeSkillPointBonus(SkillDomainType_t Domain, SkillLevel_t DomainLevel, Item* pWeapon, Exp_t Point) {
    Assert(pWeapon != NULL);

    ItemType_t itemType = pWeapon->getItemType();
    ItemType_t bestItemType =
        g_pSkillDomainInfoManager->getDomainInfo((SkillDomain)Domain, DomainLevel)->getBestItemType();

    Exp_t newPoint;

    if (pWeapon->isUnique()) {
        newPoint = getPercentValue(Point, 120);
    } else if (itemType <= bestItemType) {
        newPoint = Point * (itemType + 1) / (bestItemType + 1);
    } else {
        Exp_t Point120 = getPercentValue(Point, 120);
        newPoint = Point * itemType / (bestItemType + 1);

        newPoint = min(Point120, newPoint);
    }

    // cout << "skillPoint: " << (int)itemType << " / " << (int)bestItemType
    //	 << ", " << (int)Point << " --> " << (int)newPoint << endl;

    // by sigi. 2002.11.5
    newPoint = max(1, (int)newPoint);

    return newPoint;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computePureSlayerDamage(Slayer* pSlayer) {
    Assert(pSlayer != NULL);

    Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);

    
    Damage_t MinDamage = pSlayer->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pSlayer->getDamage(ATTR_MAX);

    
    if (pItem != NULL && pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND)) {
        MinDamage += pItem->getMinDamage();
        MaxDamage += pItem->getMaxDamage();
    }

    
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));

    return RealDamage;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computePureVampireDamage(Vampire* pVampire) {
    Assert(pVampire != NULL);

    Damage_t MinDamage = pVampire->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pVampire->getDamage(ATTR_MAX);
    uint timeband = getZoneTimeband(pVampire->getZone());

    
    Item* pItem = pVampire->getWearItem(Vampire::WEAR_RIGHTHAND);

    
    if (pItem != NULL && pVampire->isRealWearingEx(Vampire::WEAR_RIGHTHAND)) {
        MinDamage += pItem->getMinDamage();
        MaxDamage += pItem->getMaxDamage();
    }

    
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));

    RealDamage = (Damage_t)getPercentValue(RealDamage, VampireTimebandFactor[timeband]);

    return RealDamage;
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computePureOustersDamage(Ousters* pOusters) {
    Assert(pOusters != NULL);

    Damage_t MinDamage = pOusters->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pOusters->getDamage(ATTR_MAX);

    
    Item* pItem = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);

    
    if (pItem != NULL && pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND)) {
        MinDamage += pItem->getMinDamage();
        MaxDamage += pItem->getMaxDamage();
    }

    
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));

    return RealDamage;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Damage_t computePureMonsterDamage(Monster* pMonster) {
    Assert(pMonster != NULL);

    Damage_t MinDamage = pMonster->getDamage(ATTR_CURRENT);
    Damage_t MaxDamage = pMonster->getDamage(ATTR_MAX);
    Damage_t RealDamage = max(1, Random(MinDamage, MaxDamage));
    uint timeband = getZoneTimeband(pMonster->getZone());

    RealDamage = (Damage_t)getPercentValue(RealDamage, MonsterTimebandFactor[timeband]);

    return RealDamage;
}


bool isPassLine(Zone* pZone, ZoneCoord_t sX, ZoneCoord_t sY, ZoneCoord_t eX, ZoneCoord_t eY, bool blockByCreature) {
    list<TPOINT> tpList;

    if (pZone == NULL)
        return false;

    
    getLinePoint(sX, sY, eX, eY, tpList);

    if (tpList.empty())
        return false;

    VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

    list<TPOINT>::const_iterator itr = tpList.begin();
    TPOINT prev = (*itr);
    for (; itr != tpList.end(); ++itr) {
        TPOINT tp = (*itr);
        if (!rect.ptInRect(tp.x, tp.y))
            return false;

        if (tp.x == sX && tp.y == sY) {
            
            continue;
        }

        Tile& tile = pZone->getTile(tp.x, tp.y);

        if (blockByCreature) {
            if (tile.isGroundBlocked())
                return false;
        } else if (tile.isFixedGroundBlocked()) {
            return false;
        }

        
        
        if (prev.x != tp.x && prev.y != tp.y) {
            if (!rect.ptInRect(tp.x, prev.y))
                return false;
            if (!rect.ptInRect(prev.x, tp.y))
                return false;

            Tile& tile1 = pZone->getTile(tp.x, prev.y);
            Tile& tile2 = pZone->getTile(prev.x, tp.y);

            if (tile1.isFixedGroundBlocked() && tile2.isFixedGroundBlocked()) {
                return false;
            }
        }

        prev = tp;
    }

    return true;
}


void getLinePoint(ZoneCoord_t sX, ZoneCoord_t sY, ZoneCoord_t eX, ZoneCoord_t eY, list<TPOINT>& tpList) {
    int xLength = abs(sX - eX);
    int yLength = abs(sY - eY);

    if (xLength == 0 && yLength == 0)
        return;

    if (xLength > yLength) {
        if (sX > eX) {
            int tmpX = sX;
            sX = eX;
            eX = tmpX;
            int tmpY = sY;
            sY = eY;
            eY = tmpY;
        }

        float yStep = (float)(eY - sY) / (float)(eX - sX);

        for (int i = sX; i <= eX; i++) {
            TPOINT pt;
            pt.x = i;
            pt.y = sY + (int)(yStep * (float)(i - sX));

            tpList.push_back(pt);
        }
    } else {
        if (sY > eY) {
            int tmpX = sX;
            sX = eX;
            eX = tmpX;
            int tmpY = sY;
            sY = eY;
            eY = tmpY;
        }

        float xStep = (float)(eX - sX) / (float)(eY - sY);

        for (int i = sY; i <= eY; i++) {
            TPOINT pt;
            pt.x = sX + (int)(xStep * (float)(i - sY));
            pt.y = i;

            tpList.push_back(pt);
        }
    }
}

ElementalType getElementalTypeFromString(const string& type) {
    if (type == "Fire")
        return ELEMENTAL_FIRE;
    else if (type == "Water")
        return ELEMENTAL_WATER;
    else if (type == "Earth")
        return ELEMENTAL_EARTH;
    else if (type == "Wind")
        return ELEMENTAL_WIND;

    return ELEMENTAL_MAX;
}

Damage_t computeElementalCombatSkill(Ousters* pOusters, Creature* pTargetCreature, ModifyInfo& AttackerMI) {
    Assert(pOusters != NULL);
    Assert(pTargetCreature != NULL);

    Zone* pZone = pTargetCreature->getZone();
    Assert(pZone != NULL);

    Damage_t ret = 0;
    int ratio = pOusters->getPassiveRatio();
    bool bMaster = false;

    if (pTargetCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
        Assert(pTargetCreature != NULL);

        if (pMonster->isMaster())
            bMaster = true;
    }

    if (pOusters->isPassiveAvailable(SKILL_FIRE_OF_SOUL_STONE)) {
        if ((rand() % 100) < min(30, ratio)) {
            ret += pOusters->getPassiveBonus(SKILL_FIRE_OF_SOUL_STONE);

            GCAddEffect gcAddEffect;
            gcAddEffect.setObjectID(pTargetCreature->getObjectID());
            gcAddEffect.setEffectID(Effect::EFFECT_CLASS_FIRE_OF_SOUL_STONE);
            gcAddEffect.setDuration(0);

            pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
        }
    }

    if (pOusters->isPassiveAvailable(SKILL_ICE_OF_SOUL_STONE)) {
        if (!bMaster && !pTargetCreature->isFlag(Effect::EFFECT_CLASS_ICE_OF_SOUL_STONE) &&
            (rand() % 100) < min(23, ratio * 2 / 3)) {
            Turn_t duration = pOusters->getPassiveBonus(SKILL_ICE_OF_SOUL_STONE);
            
            EffectIceOfSoulStone* pEffect = new EffectIceOfSoulStone(pTargetCreature);
            pEffect->setDeadline(duration);
            pTargetCreature->addEffect(pEffect);
            pTargetCreature->setFlag(Effect::EFFECT_CLASS_ICE_OF_SOUL_STONE);

            GCAddEffect gcAddEffect;
            gcAddEffect.setObjectID(pTargetCreature->getObjectID());
            gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ICE_OF_SOUL_STONE);
            gcAddEffect.setDuration(duration);

            pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
        }
    }

    if (pOusters->isPassiveAvailable(SKILL_SAND_OF_SOUL_STONE)) {
        if ((rand() % 100) < min(30, ratio)) {
            GCAddEffectToTile gcAddEffect;
            gcAddEffect.setObjectID(pTargetCreature->getObjectID());
            gcAddEffect.setEffectID(Effect::EFFECT_CLASS_SAND_OF_SOUL_STONE);
            gcAddEffect.setDuration(7);
            gcAddEffect.setXY(pTargetCreature->getX(), pTargetCreature->getY());

            pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);

            Damage_t bonusDamage = pOusters->getPassiveBonus(SKILL_SAND_OF_SOUL_STONE);
            VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

            GCSkillToObjectOK6 gcOK6;
            gcOK6.setXY(pOusters->getX(), pOusters->getY());
            gcOK6.setSkillType(SKILL_ATTACK_MELEE);
            gcOK6.setDuration(0);

            for (int i = pTargetCreature->getX() - 1; i <= pTargetCreature->getX() + 1; ++i)
                for (int j = pTargetCreature->getY() - 1; j <= pTargetCreature->getY() + 1; ++j) {
                    if (!rect.ptInRect(i, j))
                        continue;

                    Tile& rTile = pZone->getTile(i, j);
                    if (!rTile.hasCreature(Creature::MOVE_MODE_WALKING))
                        continue;
                    Creature* pTileCreature = rTile.getCreature(Creature::MOVE_MODE_WALKING);

                    if (pTileCreature == NULL || pTileCreature->getObjectID() == pTargetCreature->getObjectID() ||
                        pTileCreature->isOusters() || pTileCreature->isNPC())
                        continue;

                    GCSkillToObjectOK4 gcOK4;
                    gcOK4.setTargetObjectID(pTileCreature->getObjectID());
                    gcOK4.setSkillType(SKILL_ATTACK_MELEE);
                    gcOK4.setDuration(0);

                    if (pTileCreature->isPC()) {
                        gcOK6.clearList();
                        setDamage(pTileCreature, bonusDamage, pOusters, SKILL_SAND_OF_SOUL_STONE, &gcOK6, &AttackerMI);
                        pTileCreature->getPlayer()->sendPacket(&gcOK6);
                    } else if (pTileCreature->isMonster()) {
                        setDamage(pTileCreature, bonusDamage, pOusters, SKILL_SAND_OF_SOUL_STONE, NULL, &AttackerMI);
                    }

                    pZone->broadcastPacket(pTileCreature->getX(), pTileCreature->getY(), &gcOK4);

                    if (pTileCreature->isDead()) {
                        int exp = computeCreatureExp(pTileCreature, 100, pOusters);
                        shareOustersExp(pOusters, exp, AttackerMI);
                        increaseAlignment(pOusters, pTileCreature, AttackerMI);
                    }
                }

            ret += bonusDamage;
        }
    }

    if (pOusters->isPassiveAvailable(SKILL_BLOCK_HEAD)) {
        if (!bMaster && !pTargetCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD) &&
            (rand() % 100) < min(15, ratio / 2)) {
            Turn_t duration = pOusters->getPassiveBonus(SKILL_BLOCK_HEAD);
            
            EffectBlockHead* pEffect = new EffectBlockHead(pTargetCreature);
            pEffect->setDeadline(duration);
            pTargetCreature->addEffect(pEffect);
            pTargetCreature->setFlag(Effect::EFFECT_CLASS_BLOCK_HEAD);

            GCAddEffect gcAddEffect;
            gcAddEffect.setObjectID(pTargetCreature->getObjectID());
            gcAddEffect.setEffectID(Effect::EFFECT_CLASS_BLOCK_HEAD);
            gcAddEffect.setDuration(duration);

            pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
        }
    }

    if (pOusters->isPassiveAvailable(SKILL_BLESS_FIRE)) {
        if ((rand() % 100) < min(30, ratio)) {
            ret += pOusters->getPassiveBonus(SKILL_BLESS_FIRE);

            GCAddEffectToTile gcAddEffect;
            gcAddEffect.setObjectID(pTargetCreature->getObjectID());
            gcAddEffect.setXY(pTargetCreature->getX(), pTargetCreature->getY());
            gcAddEffect.setEffectID(Effect::EFFECT_CLASS_BLESS_FIRE);
            gcAddEffect.setDuration(0);

            pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
        }
    }

    /*	if ( pOusters->isPassiveAvailable(SKILL_WATER_SHIELD) )
        {
            if ( (rand()%100) < min(20, (int)(ratio * 3/2)) )
            {
                EffectWaterShield* pEffect = new EffectWaterShield(pOusters);
                pEffect->setDeadline(0);
                pOusters->addEffect(pEffect);
                pOusters->setFlag(Effect::EFFECT_CLASS_WATER_SHIELD);

                GCAddEffectToTile gcAddEffect;
                gcAddEffect.setObjectID( pOusters->getObjectID() );
                gcAddEffect.setXY( pOusters->getX(), pOusters->getY() );
                gcAddEffect.setEffectID( Effect::EFFECT_CLASS_WATER_SHIELD );
                gcAddEffect.setDuration( 0 );

                pZone->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddEffect );
            }
        }
    */
    if (pOusters->isPassiveAvailable(SKILL_SAND_CROSS)) {
        if ((rand() % 100) < min(30, ratio)) {
            GCAddEffectToTile gcAddEffect;
            gcAddEffect.setObjectID(pTargetCreature->getObjectID());
            gcAddEffect.setEffectID(Effect::EFFECT_CLASS_SAND_CROSS);
            gcAddEffect.setDuration(0);
            gcAddEffect.setXY(pTargetCreature->getX(), pTargetCreature->getY());

            pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);

            Damage_t bonusDamage = pOusters->getPassiveBonus(SKILL_SAND_CROSS);
            VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

            GCSkillToObjectOK6 gcOK6;
            gcOK6.setXY(pOusters->getX(), pOusters->getY());
            gcOK6.setSkillType(SKILL_ATTACK_MELEE);
            gcOK6.setDuration(0);

            for (int i = pTargetCreature->getX() - 3; i <= pTargetCreature->getX() + 3; ++i) {
                int j = pTargetCreature->getY();

                if (!rect.ptInRect(i, j))
                    continue;

                Tile& rTile = pZone->getTile(i, j);
                if (!rTile.hasCreature(Creature::MOVE_MODE_WALKING))
                    continue;
                Creature* pTileCreature = rTile.getCreature(Creature::MOVE_MODE_WALKING);

                if (pTileCreature == NULL || pTileCreature->getObjectID() == pTargetCreature->getObjectID() ||
                    pTileCreature->isOusters() || pTileCreature->isNPC())
                    continue;

                GCSkillToObjectOK4 gcOK4;
                gcOK4.setTargetObjectID(pTileCreature->getObjectID());
                gcOK4.setSkillType(SKILL_ATTACK_MELEE);
                gcOK4.setDuration(0);

                if (pTileCreature->isPC()) {
                    gcOK6.clearList();
                    setDamage(pTileCreature, bonusDamage, pOusters, SKILL_SAND_CROSS, &gcOK6, &AttackerMI);
                    pTileCreature->getPlayer()->sendPacket(&gcOK6);
                } else if (pTileCreature->isMonster()) {
                    setDamage(pTileCreature, bonusDamage, pOusters, SKILL_SAND_CROSS, NULL, &AttackerMI);
                }

                pZone->broadcastPacket(pTileCreature->getX(), pTileCreature->getY(), &gcOK4);

                if (pTileCreature->isDead()) {
                    int exp = computeCreatureExp(pTileCreature, 100, pOusters);
                    shareOustersExp(pOusters, exp, AttackerMI);
                    increaseAlignment(pOusters, pTileCreature, AttackerMI);
                }
            }

            for (int j = pTargetCreature->getY() - 3; j <= pTargetCreature->getY() + 3; ++j) {
                int i = pTargetCreature->getX();

                if (!rect.ptInRect(i, j))
                    continue;

                Tile& rTile = pZone->getTile(i, j);
                if (!rTile.hasCreature(Creature::MOVE_MODE_WALKING))
                    continue;
                Creature* pTileCreature = rTile.getCreature(Creature::MOVE_MODE_WALKING);

                if (pTileCreature == NULL || pTileCreature->getObjectID() == pTargetCreature->getObjectID() ||
                    pTileCreature->isOusters() || pTileCreature->isNPC())
                    continue;

                GCSkillToObjectOK4 gcOK4;
                gcOK4.setTargetObjectID(pTileCreature->getObjectID());
                gcOK4.setSkillType(SKILL_ATTACK_MELEE);
                gcOK4.setDuration(0);

                if (pTileCreature->isPC()) {
                    gcOK6.clearList();
                    setDamage(pTileCreature, bonusDamage, pOusters, SKILL_SAND_CROSS, &gcOK6, &AttackerMI);
                    pTileCreature->getPlayer()->sendPacket(&gcOK6);
                } else if (pTileCreature->isMonster()) {
                    setDamage(pTileCreature, bonusDamage, pOusters, SKILL_SAND_CROSS, NULL, &AttackerMI);
                }

                pZone->broadcastPacket(pTileCreature->getX(), pTileCreature->getY(), &gcOK4);

                if (pTileCreature->isDead()) {
                    int exp = computeCreatureExp(pTileCreature, 100, pOusters);
                    shareOustersExp(pOusters, exp, AttackerMI);
                    increaseAlignment(pOusters, pTileCreature, AttackerMI);
                }
            }

            ret += bonusDamage;
        }
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
bool canAttack(Creature* pAttacker, Creature* pDefender) {
    Assert(pDefender != NULL);

    
    if (pDefender->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
        return false;

    
    
    if (pAttacker == NULL)
        return true;

    
    static bool bNonPK =
        g_pGameServerInfoManager
            ->getGameServerInfo(1, g_pConfig->getPropertyInt("ServerID"), g_pConfig->getPropertyInt("WorldID"))
            ->isNonPKServer();
    // bool canPK = bNonPK || GDRLairManager::Instance().isGDRLairZone( pAttacker->getZoneID() );

    
    if (bNonPK && pAttacker->isPC() && pDefender->isPC())
        return false;

    return true;
}

//////////////////////////////////////////////////////////////////////////
// add by Coffee 2007-6-9

//////////////////////////////////////////////////////////////////////////
bool useSkillCrad(Creature* pCreature) {
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());

    CoordInven_t InvenX = 0;
    CoordInven_t InvenY = 0;
    ItemType_t fitItem = 0; 
    if (pCreature->isSlayer()) {
        fitItem = 5;
    } else if (pCreature->isVampire()) {
        fitItem = 6;
    } else if (pCreature->isOusters()) {
        fitItem = 7;
    }
    if (fitItem != 0) {
        Item* pItem = pPC->getInventory()->findItem(Item::ITEM_CLASS_MOON_CARD, fitItem, InvenX, InvenY);
        if (pItem == NULL) {
            GCSystemMessage gcSystemMessage1;
            gcSystemMessage1.setMessage("!");
            gcSystemMessage1.setType(SYSTEM_MESSAGE_OPERATOR);
            pGamePlayer->sendPacket(&gcSystemMessage1);
            return false;
        }
        ItemNum_t OldNum = pItem->getNum();
        if (OldNum == 1) {
            pPC->getInventory()->deleteItem(pItem->getObjectID());
            pItem->destroy();
            SAFE_DELETE(pItem);
        } else {
            OldNum--;
            pItem->setNum((pItem->getNum() - 1));
            pItem->save(pPC->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
        }

    } else {
        GCSystemMessage gcSystemMessage1;
        gcSystemMessage1.setMessage("!");
        gcSystemMessage1.setType(SYSTEM_MESSAGE_OPERATOR);
        pGamePlayer->sendPacket(&gcSystemMessage1);
        return false;
    }
    return true;
}
