////////////////////////////////////////////////////////////////////////////////
// Filename : CreatureUtil.cpp

// Description :


////////////////////////////////////////////////////////////////////////////////

#include "CreatureUtil.h"

#include <cstdio>

#include "Belt.h"
#include "BloodBible.h"
#include "CGSay.h"
#include "CastleInfoManager.h"
#include "CombatInfoManager.h"
#include "DB.h"
#include "Effect.h"
#include "EffectAftermath.h"
#include "EffectGnomesWhisper.h"
#include "EffectGrandMasterSlayer.h"
#include "EffectGrandMasterVampire.h"
#include "EffectKillAftermath.h"
#include "EffectObservingEye.h"
#include "EffectRelicLock.h"
#include "FlagSet.h"
#include "GCAddEffect.h"
#include "GCAddSlayer.h"
#include "GCAddVampire.h"
#include "GCDeleteInventoryItem.h"
#include "GCDeleteObject.h"
#include "GCNoticeEvent.h"
#include "GCNotifyWin.h"
#include "GCPetInfo.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "GameWorldInfoManager.h"
#include "Inventory.h"
#include "ItemUtil.h"
#include "LevelWarManager.h"
#include "Monster.h"
#include "MonsterInfo.h"
#include "Ousters.h"
#include "PacketUtil.h"
#include "PetInfo.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "Relic.h"
#include "RelicUtil.h"
#include "ShrineInfoManager.h"
#include "SimpleCreatureEffect.h"
#include "Slayer.h"
#include "Stash.h"
#include "Store.h"
#include "Tile.h"
#include "TimeChecker.h"
#include "Vampire.h"
#include "VariableManager.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "couple/PartnerWaitingManager.h"
#include "ctf/FlagManager.h"
#include "skill/Sniping.h"
#include "skill/SummonGroundElemental.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool isSameRace(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);

    return (pCreature1->getCreatureClass() == pCreature2->getCreatureClass() ? true : false);

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//

//
////////////////////////////////////////////////////////////////////////////////
Item* findItemOID(Creature* pCreature, ObjectID_t id)

{
    __BEGIN_TRY

    int storage, x, y;
    return findItemOID(pCreature, id, storage, x, y);

    __END_CATCH
}

Item* findItemOID(Creature* pCreature, ObjectID_t id, Item::ItemClass IClass)

{
    __BEGIN_TRY

    int storage, x, y;
    return findItemOID(pCreature, id, IClass, storage, x, y);

    __END_CATCH
}

Item* findItemOID(Creature* pCreature, ObjectID_t id, int& storage, int& x, int& y)

{
    __BEGIN_TRY

    if (pCreature == NULL)
        return NULL;

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Inventory* pInventory = pPC->getInventory();
    Stash* pStash = pPC->getStash();
    Item* pItem = NULL;
    CoordInven_t tx = 0;
    CoordInven_t ty = 0;


    pItem = pInventory->findItemOID(id, tx, ty);
    if (pItem != NULL) {
        storage = STORAGE_INVENTORY;
        x = tx;
        y = ty;
        return pItem;
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        for (int i = 0; i < Slayer::WEAR_MAX; i++) {
            pItem = pSlayer->getWearItem((Slayer::WearPart)i);
            if (pItem != NULL && pItem->getObjectID() == id) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        for (int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; i++) {
            pItem = pVampire->getWearItem((Vampire::WearPart)i);
            if (pItem != NULL && pItem->getObjectID() == id) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        for (int i = 0; i < Ousters::OUSTERS_WEAR_MAX; i++) {
            pItem = pOusters->getWearItem((Ousters::WearPart)i);
            if (pItem != NULL && pItem->getObjectID() == id) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    }


    pItem = pPC->getExtraInventorySlotItem();
    if (pItem != NULL && pItem->getObjectID() == id) {
        storage = STORAGE_EXTRASLOT;
        return pItem;
    }


    for (int r = 0; r < STASH_RACK_MAX; r++) {
        for (int i = 0; i < STASH_INDEX_MAX; i++) {
            pItem = pStash->get(r, i);
            if (pItem != NULL && pItem->getObjectID() == id) {
                storage = STORAGE_STASH;
                x = r;
                y = i;
                return pItem;
            }
        }
    }

    return NULL;

    __END_CATCH
}

Item* findItemOID(Creature* pCreature, ObjectID_t id, Item::ItemClass IClass, int& storage, int& x, int& y)

{
    __BEGIN_TRY

    if (pCreature == NULL)
        return NULL;

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Inventory* pInventory = pPC->getInventory();
    Stash* pStash = pPC->getStash();
    Item* pItem = NULL;
    CoordInven_t tx = 0;
    CoordInven_t ty = 0;


    pItem = pInventory->findItemOID(id, IClass, tx, ty);
    if (pItem != NULL) {
        storage = STORAGE_INVENTORY;
        x = tx;
        y = ty;
        return pItem;
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        for (int i = 0; i < Slayer::WEAR_MAX; i++) {
            pItem = pSlayer->getWearItem((Slayer::WearPart)i);
            if (pItem != NULL && pItem->getObjectID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        for (int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; i++) {
            pItem = pVampire->getWearItem((Vampire::WearPart)i);
            if (pItem != NULL && pItem->getObjectID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        for (int i = 0; i < Ousters::OUSTERS_WEAR_MAX; i++) {
            pItem = pOusters->getWearItem((Ousters::WearPart)i);
            if (pItem != NULL && pItem->getObjectID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    }


    pItem = pPC->getExtraInventorySlotItem();
    if (pItem != NULL && pItem->getObjectID() == id && pItem->getItemClass() == IClass) {
        storage = STORAGE_EXTRASLOT;
        return pItem;
    }


    for (int r = 0; r < STASH_RACK_MAX; r++) {
        for (int i = 0; i < STASH_INDEX_MAX; i++) {
            pItem = pStash->get(r, i);
            if (pItem != NULL && pItem->getObjectID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_STASH;
                x = r;
                y = i;
                return pItem;
            }
        }
    }

    return NULL;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//

//
////////////////////////////////////////////////////////////////////////////////

Item* findItemIID(Creature* pCreature, ItemID_t id)

{
    __BEGIN_TRY

    int storage, x, y;
    return findItemIID(pCreature, id, storage, x, y);

    __END_CATCH
}

Item* findItemIID(Creature* pCreature, ItemID_t id, Item::ItemClass IClass)

{
    __BEGIN_TRY

    int storage, x, y;
    return findItemIID(pCreature, id, IClass, storage, x, y);

    __END_CATCH
}

Item* findItemIID(Creature* pCreature, ItemID_t id, int& storage, int& x, int& y)

{
    __BEGIN_TRY

    if (pCreature == NULL)
        return NULL;

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Inventory* pInventory = pPC->getInventory();
    Stash* pStash = pPC->getStash();
    Item* pItem = NULL;
    CoordInven_t tx = 0;
    CoordInven_t ty = 0;


    pItem = pInventory->findItemIID(id, tx, ty);
    if (pItem != NULL) {
        storage = STORAGE_INVENTORY;
        x = tx;
        y = ty;
        return pItem;
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        for (int i = 0; i < Slayer::WEAR_MAX; i++) {
            pItem = pSlayer->getWearItem((Slayer::WearPart)i);
            if (pItem != NULL && pItem->getItemID() == id) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        for (int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; i++) {
            pItem = pVampire->getWearItem((Vampire::WearPart)i);
            if (pItem != NULL && pItem->getItemID() == id) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        for (int i = 0; i < Ousters::OUSTERS_WEAR_MAX; i++) {
            pItem = pOusters->getWearItem((Ousters::WearPart)i);
            if (pItem != NULL && pItem->getItemID() == id) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    }


    pItem = pPC->getExtraInventorySlotItem();
    if (pItem != NULL && pItem->getItemID() == id) {
        storage = STORAGE_EXTRASLOT;
        return pItem;
    }


    for (int r = 0; r < STASH_RACK_MAX; r++) {
        for (int i = 0; i < STASH_INDEX_MAX; i++) {
            pItem = pStash->get(r, i);
            if (pItem != NULL && pItem->getItemID() == id) {
                storage = STORAGE_STASH;
                x = r;
                y = i;
                return pItem;
            }
        }
    }

    return NULL;

    __END_CATCH
}

Item* findItemIID(Creature* pCreature, ItemID_t id, Item::ItemClass IClass, int& storage, int& x, int& y)

{
    __BEGIN_TRY

    if (pCreature == NULL)
        return NULL;

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Inventory* pInventory = pPC->getInventory();
    Stash* pStash = pPC->getStash();
    Item* pItem = NULL;
    CoordInven_t tx = 0;
    CoordInven_t ty = 0;


    pItem = pInventory->findItemIID(id, IClass, tx, ty);
    if (pItem != NULL) {
        storage = STORAGE_INVENTORY;
        x = tx;
        y = ty;
        return pItem;
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        for (int i = 0; i < Slayer::WEAR_MAX; i++) {
            pItem = pSlayer->getWearItem((Slayer::WearPart)i);
            if (pItem != NULL && pItem->getItemID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        for (int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; i++) {
            pItem = pVampire->getWearItem((Vampire::WearPart)i);
            if (pItem != NULL && pItem->getItemID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        for (int i = 0; i < Ousters::OUSTERS_WEAR_MAX; i++) {
            pItem = pOusters->getWearItem((Ousters::WearPart)i);
            if (pItem != NULL && pItem->getItemID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_GEAR;
                x = i;
                return pItem;
            }
        }
    }


    pItem = pPC->getExtraInventorySlotItem();
    if (pItem != NULL && pItem->getItemID() == id && pItem->getItemClass() == IClass) {
        storage = STORAGE_EXTRASLOT;
        return pItem;
    }


    for (int r = 0; r < STASH_RACK_MAX; r++) {
        for (int i = 0; i < STASH_INDEX_MAX; i++) {
            pItem = pStash->get(r, i);
            if (pItem != NULL && pItem->getItemID() == id && pItem->getItemClass() == IClass) {
                storage = STORAGE_STASH;
                x = r;
                y = i;
                return pItem;
            }
        }
    }

    return NULL;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
int computeCreatureExp(Creature* pCreature, int percent, Ousters* pOusters)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);
    Assert(percent >= 0 && percent <= 100);

    int exp = 0;

    if (pCreature->isSlayer()) {
        if ((pCreature->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH) == true && percent != BLOODDRAIN_EXP) ||
            (pCreature->isFlag(Effect::EFFECT_CLASS_AFTERMATH) == true && percent == BLOODDRAIN_EXP)) {
            exp = 0;
        } else {
            // change exp to 1,chengh modified 2005 11 06
            /*Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            exp += pSlayer->getSTR(ATTR_BASIC);
            exp += pSlayer->getDEX(ATTR_BASIC);
            exp += pSlayer->getINT(ATTR_BASIC);*/
            exp = 1;
        }


        if (g_pCombatInfoManager->isCombat()) {
            if (percent == BLOODDRAIN_EXP)
                percent = KILL_EXP;
            else if (percent == KILL_EXP)
                percent = BLOODDRAIN_EXP;
        }
    } else if (pCreature->isVampire()) {
        if (pCreature->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH))
            exp = 0;
        else if (pOusters != NULL) {
            // change exp to 1,chengh modified 2005 11 06
            /*Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            exp += pVampire->getSTR(ATTR_MAX);
            exp += pVampire->getDEX(ATTR_MAX);
            exp += pVampire->getINT(ATTR_MAX);*/
            exp = 1;
        }
    } else if (pCreature->isOusters()) {
        if ((pCreature->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH) == true && percent == KILL_EXP) ||
            (pCreature->isFlag(Effect::EFFECT_CLASS_AFTERMATH) == true && percent == BLOODDRAIN_EXP) ||
            pOusters != NULL) {
            exp = 0;
        } else {
            // change exp to 1,chengh modified 2005 11 06
            /*Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            exp += pOusters->getSTR(ATTR_BASIC);
            exp += pOusters->getDEX(ATTR_BASIC);
            exp += pOusters->getINT(ATTR_BASIC);*/
            exp = 1;
        }
    } else if (pCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);

        if (pMonster->getMonsterType() == 753) {
            return 0;
        }
        if (pMonster->getMonsterType() == GROUND_ELEMENTAL_TYPE) {
            return 0;
        }


        if (pOusters != NULL) {
            return getPercentValue(getPercentValue((int)(pMonster->getOustersExp(pOusters)),
                                                   g_pVariableManager->getVariable(MONSTER_EXP_RATIO)),
                                   percent);
        }

        exp += pMonster->getSTR();
        exp += pMonster->getDEX();
        exp += pMonster->getINT();
        exp = (int)(exp * (0.75 + (double)(pMonster->getLevel() / 200.0)));


        MonsterType_t MonsterType = pMonster->getMonsterType();
        if (MonsterType == 358 || MonsterType == 359)
            exp = 1144;
        else if (MonsterType == 360 || MonsterType == 361)
            exp = 1076;

        // cout << pMonster->getName() << " exp = " << exp << "  percent=" << percent << endl;

        /*
        const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(pMonster->getMonsterType());
        int HPBoost = pMonsterInfo->getEnhanceHP();
        int ToHitBoost = pMonsterInfo->getEnhanceToHit();
        int DefenseBoost = pMonsterInfo->getEnhanceDefense();
        int ProtectionBoost = pMonsterInfo->getEnhanceProtection();
        int DamageBoost = pMonsterInfo->getEnhanceDamage();
        int bonusPercent = (int)(HPBoost*0.4 + ToHitBoost*0.2 + DefenseBoost*0.4 + ProtectionBoost*0.3 +
        DamageBoost*0.5); exp += getPercentValue(exp, bonusPercent);
        */
    } else
        Assert(false);


    // exp = (int)((float)exp * 1.5);
    exp = getPercentValue(exp, percent);
    exp = getPercentValue(exp, g_pVariableManager->getVariable(MONSTER_EXP_RATIO));

    return exp;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
int computeBloodDrainHealPoint(Creature* pCreature, int percent)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);
    Assert(percent >= 0 && percent <= 100);

    int exp = 0;

    if (pCreature->isSlayer()) {
        // change exp to 1,chengh modified 2005 11 06
        /*Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        exp += pSlayer->getSTR(ATTR_BASIC);
        exp += pSlayer->getDEX(ATTR_BASIC);
        exp += pSlayer->getINT(ATTR_BASIC);*/
        exp = 1;


        if (g_pCombatInfoManager->isCombat()) {
            if (percent == BLOODDRAIN_EXP)
                percent = KILL_EXP;
            else if (percent == KILL_EXP)
                percent = BLOODDRAIN_EXP;
        }
    } else if (pCreature->isVampire()) {
        /*
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        exp += pVampire->getSTR(ATTR_MAX);
        exp += pVampire->getDEX(ATTR_MAX);
        exp += pVampire->getINT(ATTR_MAX);
        */
    } else if (pCreature->isOusters()) {
        // change exp to 1,chengh modified 2005 11 06
        /*Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
        exp += pOusters->getSTR(ATTR_BASIC);
        exp += pOusters->getDEX(ATTR_BASIC);
        exp += pOusters->getINT(ATTR_BASIC);*/
        exp = 1;
    } else if (pCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);


        if (pMonster->getMonsterType() == 753) {
            return 0;
        }

        exp += pMonster->getSTR();
        exp += pMonster->getDEX();
        exp += pMonster->getINT();
        exp = (int)(exp * (0.75 + (double)(pMonster->getLevel() / 200.0)));


        MonsterType_t MonsterType = pMonster->getMonsterType();
        if (MonsterType == 358 || MonsterType == 359)
            exp = 1144;
        else if (MonsterType == 360 || MonsterType == 361)
            exp = 1076;

        // cout << pMonster->getName() << " exp = " << exp << "  percent=" << percent << endl;

        /*
        const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(pMonster->getMonsterType());
        int HPBoost = pMonsterInfo->getEnhanceHP();
        int ToHitBoost = pMonsterInfo->getEnhanceToHit();
        int DefenseBoost = pMonsterInfo->getEnhanceDefense();
        int ProtectionBoost = pMonsterInfo->getEnhanceProtection();
        int DamageBoost = pMonsterInfo->getEnhanceDamage();
        int bonusPercent = (int)(HPBoost*0.4 + ToHitBoost*0.2 + DefenseBoost*0.4 + ProtectionBoost*0.3 +
        DamageBoost*0.5); exp += getPercentValue(exp, bonusPercent);
        */
    } else
        Assert(false);


    // exp = (int)((float)exp * 1.5);
    exp = getPercentValue(exp, percent);

    return exp;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool isEventMonster(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    if (!pCreature->isMonster())
        return false;

    Monster* pMonster = dynamic_cast<Monster*>(pCreature);

    switch (pMonster->getMonsterType()) {
    case 358:
    case 359:
    case 360:
    case 361:
        return true;
    default:
        break;
    }

    return false;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool isAbleToMove(Creature* pCreature) {
    Assert(pCreature != NULL);


    if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)) {
        if (pCreature->isVampire())
            return false;


        // if (pCreature->isMonster()) return false;

        if (pCreature->isSlayer())
            return false;
    }


    if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA) || pCreature->isDead()

        || pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
        //		|| pCreature->isFlag(Effect::EFFECT_CLASS_SANCTUARY)
        || pCreature->isFlag(Effect::EFFECT_CLASS_CASKET)

        || pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN) || pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP)
        // 2003.1.2 by Sequoia
        || pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
        // 2003.3.31 by Sequoia
        || pCreature->isFlag(Effect::EFFECT_CLASS_POISON_MESH) || pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED) || pCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER))
        return false;

    if (pCreature->isPC()) {
        if (dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen())
            return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool isAbleToUseSelfSkill(Creature* pCreature, SkillType_t SkillType) {
    Assert(pCreature != NULL);
    if (pCreature->isFlag(Effect::EFFECT_CLASS_PLEASURE_EXPLOSION))
        return false;


    if ((pCreature->isDead() || pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) && SkillType != SKILL_ETERNITY)
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE))
        return false;

    if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) || pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE) || pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN) || pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) && SkillType != SKILL_UN_TRANSFORM ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP) || pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL) || pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET) || pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)) {
        return false;
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_CASKET) && SkillType != SKILL_OPEN_CASKET) {
        return false;
    }


    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) {
        if (SkillType != SKILL_HOWL || SkillType != SKILL_EAT_CORPSE || SkillType != SKILL_UN_TRANSFORM)
        // end  edit
        // if (SkillType != SKILL_HOWL && SkillType != SKILL_EAT_CORPSE && SkillType != SKILL_UN_TRANSFORM)
        {
            return false;
        }
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
        if (SkillType != SKILL_BITE_OF_DEATH && SkillType != SKILL_UN_TRANSFORM && SkillType != SKILL_RAPID_GLIDING) {
            return false;
        }
    }


    if (pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)) {
        if (SkillType != SKILL_UN_TRANSFORM)
            return false;
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        if (pSlayer->hasRideMotorcycle()) {
            return false;
        }
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET)) {
        if (SkillType != SKILL_UN_TRANSFORM && SkillType != SKILL_TURRET_FIRE)
            return false;
    }

    if (pCreature->isPC()) {
        if (dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen())
            return false;
    }

    return true;
}

bool isAbleToUseObjectSkill(Creature* pCreature, SkillType_t SkillType) {
    Assert(pCreature != NULL);
    if (pCreature->isFlag(Effect::EFFECT_CLASS_PLEASURE_EXPLOSION))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) || pCreature->isFlag(Effect::EFFECT_CLASS_CASKET) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_COMA) || pCreature->isDead() ||
        pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT) || pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN) || pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP) || pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) || pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD) || pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED) || pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) {
        if (SkillType != SKILL_HOWL || SkillType != SKILL_EAT_CORPSE || SkillType != SKILL_ATTACK_MELEE)
        // end  edit
        // if (SkillType != SKILL_HOWL && SkillType != SKILL_EAT_CORPSE && SkillType != SKILL_ATTACK_MELEE)
        {
            return false;
        }
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
        if (SkillType != SKILL_BITE_OF_DEATH && SkillType != SKILL_UN_TRANSFORM && SkillType != SKILL_ATTACK_MELEE &&
            SkillType != SKILL_RAPID_GLIDING) {
            return false;
        }
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        if (pSlayer->hasRideMotorcycle())
            return false;
    }

    if (pCreature->isPC()) {
        if (dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen())
            return false;
    }

    return true;
}

bool isAbleToUseTileSkill(Creature* pCreature, SkillType_t SkillType) {
    Assert(pCreature != NULL);
    if (pCreature->isFlag(Effect::EFFECT_CLASS_PLEASURE_EXPLOSION))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) || pCreature->isFlag(Effect::EFFECT_CLASS_CASKET) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_COMA) || pCreature->isDead() ||
        pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT) || pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN) || pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP) || pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) || pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD) || pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED) || pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) {
        // if (SkillType != SKILL_HOWL && SkillType != SKILL_EAT_CORPSE)
        if (SkillType != SKILL_HOWL || SkillType != SKILL_EAT_CORPSE)
        // end  edit
        {
            return false;
        }
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
        if (SkillType != SKILL_BITE_OF_DEATH && SkillType != SKILL_UN_TRANSFORM && SkillType != SKILL_ATTACK_MELEE &&
            SkillType != SKILL_RAPID_GLIDING) {
            return false;
        }
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        if (pSlayer->hasRideMotorcycle())
            return false;
    }

    if (pCreature->isPC()) {
        if (dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen())
            return false;
    }

    return true;
}

bool isAbleToUseInventorySkill(Creature* pCreature, BYTE X, BYTE Y, BYTE TX, BYTE TY, SkillType_t SkillType) {
    Assert(pCreature != NULL);
    if (pCreature->isFlag(Effect::EFFECT_CLASS_PLEASURE_EXPLOSION))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE))
        return false;


    // TX/TY are validated again. They are wire-controlled just like X/Y, and
    // the Create* skills (CreateBomb, CreateMine, CreateHolyWater,
    // CreateHolyPotion) index the inventory with the *target* coordinates --
    // both to read and, via Inventory::addItem, to write. The combined check
    // had been commented out in favour of an X/Y-only one, leaving TX/TY
    // unvalidated on every one of those paths.
    if (X >= 10 || Y >= 6 || TX >= 10 || TY >= 6)
        return false;
    if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) || pCreature->isFlag(Effect::EFFECT_CLASS_CASKET) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_COMA) || pCreature->isDead() ||
        pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT) || pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN) || pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP) || pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) || pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD) || pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED) || pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER))
        return false;


    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) {
        // if (SkillType != SKILL_HOWL && SkillType != SKILL_EAT_CORPSE)
        if (SkillType != SKILL_HOWL || SkillType != SKILL_EAT_CORPSE)
        // end  edit
        {
            return false;
        }
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
        if (SkillType != SKILL_BITE_OF_DEATH && SkillType != SKILL_UN_TRANSFORM && SkillType != SKILL_ATTACK_MELEE &&
            SkillType != SKILL_RAPID_GLIDING) {
            return false;
        }
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        if (pSlayer->hasRideMotorcycle())
            return false;
    }

    if (pCreature->isPC()) {
        if (dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen())
            return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool isAbleToPickupItem(Creature* pCreature, Item* pItem) {
    Assert(pCreature != NULL);


    if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)


        || pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
        //		|| pCreature->isFlag(Effect::EFFECT_CLASS_SANCTUARY)

        || pCreature->isFlag(Effect::EFFECT_CLASS_CASKET) || pCreature->isDead() ||
        pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) || pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD) || pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET))
        return false;


    Zone* pZone = pCreature->getZone();
    if (pZone != NULL) {
        Tile& rTile = pZone->getTile(pCreature->getX(), pCreature->getY());
        if (rTile.getEffect(Effect::EFFECT_CLASS_SANCTUARY) != NULL) {
            return false;
        }
    }


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        if (pSlayer->hasRideMotorcycle())
            return false;
    }


    Item::ItemClass itemClass = pItem->getItemClass();
    ItemType_t itemtype = pItem->getItemType();

    if (itemClass == Item::ITEM_CLASS_QUEST_ITEM && pCreature->isPC() && pItem->getItemType() < 4) {
        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
        Inventory* pInventory = pPC->getInventory();


        Item* pMouseItem = pPC->getExtraInventorySlotItem();

        if (pMouseItem != NULL && pMouseItem->getItemClass() == Item::ITEM_CLASS_QUEST_ITEM &&
            pMouseItem->getItemType() < 4)
            return false;


        Item* pInvenItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM);

        if (pInvenItem != NULL && pInvenItem->getItemClass() == Item::ITEM_CLASS_QUEST_ITEM &&
            pInvenItem->getItemType() < 4)
            return false;
    }

    if (itemClass == Item::ITEM_CLASS_EVENT_ITEM && pCreature->isPC() && pItem->getItemType() == 30) {
        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
        Inventory* pInventory = pPC->getInventory();

        Item* pMouseItem = pPC->getExtraInventorySlotItem();
        if (pMouseItem != NULL && pMouseItem->getItemClass() == Item::ITEM_CLASS_EVENT_ITEM &&
            pMouseItem->getItemType() == 30)
            return false;

        Item* pInvenItem = pInventory->findItem(Item::ITEM_CLASS_EVENT_ITEM, (ItemType_t)30);
        if (pInvenItem != NULL)
            return false;
    }


    if (pItem->isTimeLimitItem())
        return false;


    if (isRelicItem(itemClass)) {
        if (pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK) || pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) || pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE)) {
            return false;
        }

        switch (itemClass) {
        case Item::ITEM_CLASS_RELIC: {
            const RelicInfo* pRelicInfo = dynamic_cast<RelicInfo*>(g_pRelicInfoManager->getItemInfo(itemtype));

            if (pRelicInfo->relicType == RELIC_TYPE_SLAYER &&
                    !pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SLAYER_RELIC) ||
                pRelicInfo->relicType == RELIC_TYPE_VAMPIRE &&
                    !pCreature->isFlag(Effect::EFFECT_CLASS_HAS_VAMPIRE_RELIC)) {
                return true;
            }
        } break;

        case Item::ITEM_CLASS_BLOOD_BIBLE: {
            if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_VAMPIRE_RELIC) ||
                pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SLAYER_RELIC)) {
                return false;
            }

            BloodBible* pBloodBible = dynamic_cast<BloodBible*>(pItem);
            Assert(pBloodBible != NULL);

            if (!g_pShrineInfoManager->canPickupBloodBible(pCreature->getRace(), pBloodBible)) {
                return false;
            }


            ZoneLevel_t zoneLevel = pCreature->getZone()->getZoneLevel(pCreature->getX(), pCreature->getY());
            if (zoneLevel & SAFE_ZONE) {
                return false;
            }
        } break;

        case Item::ITEM_CLASS_CASTLE_SYMBOL: {
            if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_VAMPIRE_RELIC) ||
                pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SLAYER_RELIC)) {
                return false;
            }

            ZoneID_t castleZoneID = 0;
            if (g_pCastleInfoManager->getCastleZoneID(pCreature->getZone()->getZoneID(), castleZoneID)) {
                CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(castleZoneID);
                if (pCreature->getRace() != pCastleInfo->getRace())
                    return false;
            } else {
                if (pCreature->getZoneID() == 1500)
                    return true;
                return false;
            }
        } break;

        case Item::ITEM_CLASS_WAR_ITEM: {
            Level_t level = pCreature->getLevel();
            if (level < 150)
                return false;
        } break;

        default:
            return false;
        }
    }

    if (pItem->getItemClass() == Item::ITEM_CLASS_SWEEPER) {
        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER))
            return false;
        LevelWarManager* pLevelWarManager = NULL;
        if (pZone != NULL)
            pLevelWarManager = pZone->getLevelWarManager();

        if (pLevelWarManager == NULL || !pLevelWarManager->hasWar())
            return false;

        if (pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK) || pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) || pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL))
            return false;
    }


    if (pItem->isFlagItem()) {
        if (!g_pFlagManager->hasFlagWar())
            return false;
        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_FLAG))
            return false;
        if (pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK) || pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) || pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) ||
            pCreature->isFlag(Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL))
            return false;
    }

    ///*
    if (pCreature->isSlayer()) {
        if (pItem->isFlag(Effect::EFFECT_CLASS_VAMPIRE_ONLY))
            return false;
        if (pItem->isFlag(Effect::EFFECT_CLASS_OUSTERS_ONLY))
            return false;


        switch (itemClass) {
        case Item::ITEM_CLASS_VAMPIRE_RING:
        case Item::ITEM_CLASS_VAMPIRE_BRACELET:
        case Item::ITEM_CLASS_VAMPIRE_NECKLACE:
        case Item::ITEM_CLASS_VAMPIRE_COAT:
        case Item::ITEM_CLASS_VAMPIRE_ETC:
        case Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM:
        case Item::ITEM_CLASS_VAMPIRE_EARRING:
        case Item::ITEM_CLASS_VAMPIRE_WEAPON:
        case Item::ITEM_CLASS_VAMPIRE_AMULET:
        case Item::ITEM_CLASS_SERUM:
        case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
        case Item::ITEM_CLASS_OUSTERS_BOOTS:
        case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
        case Item::ITEM_CLASS_OUSTERS_CIRCLET:
        case Item::ITEM_CLASS_OUSTERS_COAT:
        case Item::ITEM_CLASS_OUSTERS_PENDENT:
        case Item::ITEM_CLASS_OUSTERS_RING:
        case Item::ITEM_CLASS_OUSTERS_STONE:
        case Item::ITEM_CLASS_OUSTERS_WRISTLET:
        case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM:
        case Item::ITEM_CLASS_CODE_SHEET:
        case Item::ITEM_CLASS_DERMIS:
        case Item::ITEM_CLASS_PERSONA:
        case Item::ITEM_CLASS_FASCIA:
        case Item::ITEM_CLASS_MITTEN:
            // case Item::ITEM_CLASS_MONEY :

            //  edit by sonic 2006.10.31
            // if (pItem->getItemType()==1) return false;
            return false;


            break;

        default:
            return true;
        }

        return true;
    } else if (pCreature->isVampire()) {
        if (pItem->isFlag(Effect::EFFECT_CLASS_SLAYER_ONLY))
            return false;
        if (pItem->isFlag(Effect::EFFECT_CLASS_OUSTERS_ONLY))
            return false;


        switch (pItem->getItemClass()) {
        case Item::ITEM_CLASS_VAMPIRE_RING:
        case Item::ITEM_CLASS_VAMPIRE_BRACELET:
        case Item::ITEM_CLASS_VAMPIRE_NECKLACE:
        case Item::ITEM_CLASS_VAMPIRE_COAT:
        case Item::ITEM_CLASS_VAMPIRE_ETC:
        case Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM:
        case Item::ITEM_CLASS_VAMPIRE_WEAPON:
        case Item::ITEM_CLASS_VAMPIRE_AMULET:
        case Item::ITEM_CLASS_VAMPIRE_EARRING:
        case Item::ITEM_CLASS_SERUM:

        case Item::ITEM_CLASS_SKULL:
        case Item::ITEM_CLASS_EVENT_GIFT_BOX:
        case Item::ITEM_CLASS_EVENT_STAR:
        case Item::ITEM_CLASS_RELIC:
        case Item::ITEM_CLASS_QUEST_ITEM:
        case Item::ITEM_CLASS_EVENT_TREE:
        case Item::ITEM_CLASS_EVENT_ETC:
        case Item::ITEM_CLASS_BLOOD_BIBLE:
        case Item::ITEM_CLASS_CASTLE_SYMBOL:
        case Item::ITEM_CLASS_EVENT_ITEM:
        case Item::ITEM_CLASS_RESURRECT_ITEM:
        case Item::ITEM_CLASS_MIXING_ITEM:
        case Item::ITEM_CLASS_EFFECT_ITEM:
        case Item::ITEM_CLASS_DYE_POTION:
        case Item::ITEM_CLASS_MOON_CARD:
        case Item::ITEM_CLASS_SWEEPER:
        case Item::ITEM_CLASS_PET_ITEM:
        case Item::ITEM_CLASS_PET_FOOD:
        case Item::ITEM_CLASS_PET_ENCHANT_ITEM:
        case Item::ITEM_CLASS_LUCKY_BAG:
        case Item::ITEM_CLASS_SMS_ITEM:
        case Item::ITEM_CLASS_CORE_ZAP:
        case Item::ITEM_CLASS_TRAP_ITEM:
        case Item::ITEM_CLASS_WAR_ITEM:
        case Item::ITEM_CLASS_DERMIS:
        case Item::ITEM_CLASS_PERSONA:
        case Item::ITEM_CLASS_MONEY:

            // edit by sonic 2006.10.31
            // if (pItem->getItemType()==1) return true;
            return true;


            break;

        default:
            return false;
        }


        return false;
    } else if (pCreature->isOusters()) {
        if (pItem->isFlag(Effect::EFFECT_CLASS_SLAYER_ONLY))
            return false;
        if (pItem->isFlag(Effect::EFFECT_CLASS_VAMPIRE_ONLY))
            return false;


        switch (pItem->getItemClass()) {
        case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
        case Item::ITEM_CLASS_OUSTERS_BOOTS:
        case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
        case Item::ITEM_CLASS_OUSTERS_CIRCLET:
        case Item::ITEM_CLASS_OUSTERS_COAT:
        case Item::ITEM_CLASS_OUSTERS_PENDENT:
        case Item::ITEM_CLASS_OUSTERS_RING:
        case Item::ITEM_CLASS_OUSTERS_STONE:
        case Item::ITEM_CLASS_OUSTERS_WRISTLET:
        case Item::ITEM_CLASS_SKULL:
        case Item::ITEM_CLASS_EVENT_GIFT_BOX:
        case Item::ITEM_CLASS_EVENT_STAR:
        case Item::ITEM_CLASS_RELIC:
        case Item::ITEM_CLASS_QUEST_ITEM:
        case Item::ITEM_CLASS_EVENT_TREE:
        case Item::ITEM_CLASS_EVENT_ETC:
        case Item::ITEM_CLASS_BLOOD_BIBLE:
        case Item::ITEM_CLASS_CASTLE_SYMBOL:
        case Item::ITEM_CLASS_EVENT_ITEM:
        case Item::ITEM_CLASS_RESURRECT_ITEM:
        case Item::ITEM_CLASS_MIXING_ITEM:
        case Item::ITEM_CLASS_LARVA:
        case Item::ITEM_CLASS_PUPA:
        case Item::ITEM_CLASS_COMPOS_MEI:
        case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM:
        case Item::ITEM_CLASS_EFFECT_ITEM:
        case Item::ITEM_CLASS_DYE_POTION:
        case Item::ITEM_CLASS_MOON_CARD:
        case Item::ITEM_CLASS_SWEEPER:
        case Item::ITEM_CLASS_PET_ITEM:
        case Item::ITEM_CLASS_PET_FOOD:
        case Item::ITEM_CLASS_PET_ENCHANT_ITEM:
        case Item::ITEM_CLASS_LUCKY_BAG:
        case Item::ITEM_CLASS_SMS_ITEM:
        case Item::ITEM_CLASS_CORE_ZAP:
        case Item::ITEM_CLASS_TRAP_ITEM:
        case Item::ITEM_CLASS_WAR_ITEM:
        case Item::ITEM_CLASS_FASCIA:
        case Item::ITEM_CLASS_MITTEN:
        case Item::ITEM_CLASS_MONEY:

            // edit by sonic 2006.10.31
            // if (pItem->getItemType()==1) return true;
            return true;


            break;

        default:
            return false;
        }

        return false;
    }
    //*/

    if (pCreature->isPC()) {
        if (dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen())
            return false;
    }

    return true;
}

bool canDropToZone(Creature* pCreature, Item* pItem) {
    if (pItem->isTimeLimitItem())
        return false;

    if (!pCreature->isPC())
        return false;
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

    switch (pItem->getItemClass()) {
    case Item::ITEM_CLASS_MONEY: {
        // edit by sonic
        // return false;
    }
    case Item::ITEM_CLASS_COUPLE_RING:
    case Item::ITEM_CLASS_VAMPIRE_COUPLE_RING: {
        if (PartnerWaitInfo::getItemClass(pPC) == pItem->getItemClass() &&
            PartnerWaitInfo::getItemType(pPC) == pItem->getItemType()) {
            return false;
        }
    } break;

    case Item::ITEM_CLASS_MOON_CARD:

    {
        if (pItem->getItemType() == 0)
            return false;
    } break;
    case Item::ITEM_CLASS_LUCKY_BAG: {
        if (pItem->getItemType() == 3)
            return false;
    } break;
    case Item::ITEM_CLASS_EVENT_ITEM: {
        if (pItem->getItemType() == 28 || pItem->getItemType() == 30)
            return false;
    } break;
    case Item::ITEM_CLASS_EFFECT_ITEM: {
        if (pItem->getItemType() >= 4 && pItem->getItemType() <= 6)
            return false;
    } break;
    case Item::ITEM_CLASS_EVENT_STAR: {
        if (pItem->getItemType() >= 17 && pItem->getItemType() <= 21)
            return false;
    } break;
    case Item::ITEM_CLASS_MIXING_ITEM: {
        if (pItem->getItemType() == 18)
            return false;
    } break;

    default:
        break;
    }

    // Exchange System: Point-only items cannot be dropped
    if (isPointOnlyTradeItem(pItem))
        return false;

    return true;
}


// 2002. 1. 13. Sequoia

/*void confirmGrandMaster(Creature* pCreature)
{
    if( pCreature == NULL ) return;
    if( !pCreature->isPC() ) return;

    if( pCreature->isSlayer() )
    {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        Assert( pSlayer != NULL );

        if( !pSlayer->isFlag( Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER ) ) return;
        EffectGrandMasterSlayer* pEffect = dynamic_cast<EffectGrandMasterSlayer*>( pSlayer->findEffect(
Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER ) ); if( pEffect == NULL ) return;

        pEffect->affect();
    }
    else if( pCreature->isVampire() )
    {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        Assert( pVampire != NULL );

        if( !pVampire->isFlag( Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE ) ) return;
        EffectGrandMasterVampire* pEffect = dynamic_cast<EffectGrandMasterVampire*>( pVampire->findEffect(
Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE ) ); if( pEffect == NULL ) return;

        pEffect->affect();
    }
}*/

bool getRaceFromDB(const string& Name, Race_t& race)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectRaceStmt(pConn, "SELECT Race FROM Slayer where Name=?");
        selectRaceStmt.bindString(1, Name);
        Result* pResult = selectRaceStmt.execute();

        if (pResult->next()) {
            string Race = pResult->getString(1);

            if (Race == "SLAYER") {
                race = RACE_SLAYER;
            } else if (Race == "VAMPIRE") {
                race = RACE_VAMPIRE;
            } else
                race = RACE_OUSTERS;
        } else {
            return false;
        }
    }
    END_DB(pStmt)

    return true;

    __END_CATCH
}

bool getGuildIDFromDB(const string& Name, Race_t race, GuildID_t& guildID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        string table;
        if (race == RACE_SLAYER)
            table = "Slayer";
        else if (race == RACE_VAMPIRE)
            table = "Vampire";
        else
            table = "Ousters";

        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // table is one of three hardcoded literals selected above by race, never
        // packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement selectGuildIDStmt(pConn, "SELECT GuildID FROM " + table + " where Name=?");
        selectGuildIDStmt.bindString(1, Name);
        Result* pResult = selectGuildIDStmt.execute();

        if (pResult->next()) {
            guildID = (GuildID_t)pResult->getInt(1);

            if (guildID == 0 || guildID == 99 || guildID == 66) {
                return false;
            }
        } else {
            return false;
        }
    }
    END_DB(pStmt)

    return true;

    __END_CATCH
}

bool canSee(const Creature* pSource, Creature* pTarget, EffectObservingEye* pEffectObservingEye,
            EffectGnomesWhisper* pEffectGnomesWhisper) {
    if (pTarget->isFlag(Effect::EFFECT_CLASS_GHOST))
        return false;

    // ZoneCoord_t targetX = pTarget->getX(), targetY = pTarget->getY();


    if (pEffectObservingEye == NULL && pSource->isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE)) {
        pEffectObservingEye =
            dynamic_cast<EffectObservingEye*>(pSource->findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
    }


    if (pEffectGnomesWhisper == NULL && pSource->isFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER)) {
        pEffectGnomesWhisper =
            dynamic_cast<EffectGnomesWhisper*>(pSource->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
    }


    // if (pSource->getVisionState(targetX, targetY) >= IN_SIGHT)
    //{

    if (pSource->isVampire() && pTarget->isVampire())
        return true;

    if ((!pTarget->isFlag(Effect::EFFECT_CLASS_HIDE) || pSource->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN)

         || pSource->isFlag(Effect::EFFECT_CLASS_REVEALER) ||
         (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeHide())) &&
        (!pTarget->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) ||
         pSource->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) || pSource->isVampire() ||
         (pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility(pTarget)) ||
         (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeInvisibility())) &&
        (!pTarget->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) ||
         pSource->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ||
         (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeSniping()))) {
        return true;
    }
    //}

    return false;
}

int changeSexEx(PlayerCreature* pPC) {
    Zone* pZone = pPC->getZone();
    if (pZone == NULL)
        return 3;

    if (pPC->getFlagSet()->isOn(FLAGSET_IS_COUPLE))
        return 2;

    if (pPC->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
        Assert(pSlayer != NULL);

        if (pSlayer->isWear(Slayer::WEAR_BODY) || pSlayer->isWear(Slayer::WEAR_LEG))
            return 1;

        if (pSlayer->getSex() == MALE)
            pSlayer->setSex(FEMALE);
        else
            pSlayer->setSex(MALE);

        //		char query[50];
        //		sprintf( query, "SEX = '%s'", Sex2String[pPC->getSex()].c_str() );
        //		pSlayer->tinysave(query);
    } else if (pPC->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
        Assert(pVampire != NULL);

        if (pVampire->isWear(Vampire::WEAR_BODY))
            return 1;

        if (pVampire->getSex() == MALE)
            pVampire->setSex(FEMALE);
        else
            pVampire->setSex(MALE);

        //		char query[50];
        //		sprintf( query, "SEX = '%s'", Sex2String[pPC->getSex()].c_str() );
        //		pVampire->tinysave(query);
    } else
        return 3;


    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateSlayerSexStmt(pConn, "UPDATE Slayer SET SEX=? WHERE Name=?");
        updateSlayerSexStmt.bindString(1, Sex2String[pPC->getSex()]);
        updateSlayerSexStmt.bindString(2, pPC->getName());
        updateSlayerSexStmt.execute();

        PreparedStatement updateVampireSexStmt(pConn, "UPDATE Vampire SET SEX=? WHERE Name=?");
        updateVampireSexStmt.bindString(1, Sex2String[pPC->getSex()]);
        updateVampireSexStmt.bindString(2, pPC->getName());
        updateVampireSexStmt.execute();
    }
    END_DB(pStmt)

    return 0;
}

void initAllStatAndSendChange(PlayerCreature* pPC) {
    if (pPC->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
        Assert(pSlayer != NULL);

        SLAYER_RECORD prev;
        pSlayer->getSlayerRecord(prev);
        pSlayer->initAllStat();
        pSlayer->sendModifyInfo(prev);
        pSlayer->sendRealWearingInfo();
    } else if (pPC->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
        Assert(pVampire != NULL);

        VAMPIRE_RECORD prev;
        pVampire->getVampireRecord(prev);
        pVampire->initAllStat();
        pVampire->sendModifyInfo(prev);
        pVampire->sendRealWearingInfo();
    } else if (pPC->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
        Assert(pOusters != NULL);

        OUSTERS_RECORD prev;
        pOusters->getOustersRecord(prev);
        pOusters->initAllStat();
        pOusters->sendModifyInfo(prev);
        pOusters->sendRealWearingInfo();
    }
}

void addSimpleCreatureEffect(Creature* pCreature, Effect::EffectClass eClass, int time /* = -1 */,
                             bool isSend /* = true */) {
    SimpleCreatureEffect* pEffect = new SimpleCreatureEffect(eClass, pCreature);
    Assert(pEffect != NULL);

    if (time != -1)
        pEffect->setDeadline(time);
    pEffect->setBroadcastingEffect(isSend);

    pCreature->addEffect(pEffect);
    pCreature->setFlag(eClass);

    if (isSend) {
        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pCreature->getObjectID());
        gcAddEffect.setEffectID(pEffect->getSendEffectClass());

        if (time == -1)
            gcAddEffect.setDuration(65535);
        else {
            if (pEffect->getSendEffectClass() == Effect::EFFECT_CLASS_BLOOD_DRAIN ||
                pEffect->getSendEffectClass() == Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR)
                gcAddEffect.setDuration(time / 10);
            else
                gcAddEffect.setDuration(time);
        }

        pCreature->getZone()->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);
    }
}

void deleteCreatureEffect(Creature* pCreature, Effect::EffectClass eClass) {
    Effect* pEffect = pCreature->findEffect(eClass);
    if (pEffect != NULL)
        pEffect->setDeadline(0);
}

bool dropFlagToZone(PlayerCreature* pPC, Item* pItem)

{
    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);


    //	TPOINT pt = pZone->addItem( pItem, pPC->getX(), pPC->getY(), false );
    pZone->addItemDelayed(pItem, pPC->getX(), pPC->getY(), false);

    Effect* pEffect = pPC->findEffect(Effect::EFFECT_CLASS_HAS_FLAG);
    if (pEffect != NULL)
        pEffect->setDeadline(0);

    if (!pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK)) {
        EffectRelicLock* pLock = new EffectRelicLock(pItem);
        pLock->setDeadline(10 * 10);
        pItem->setFlag(Effect::EFFECT_CLASS_RELIC_LOCK);
        pItem->getEffectManager().addEffect(pLock);
    }


    return true;
}

bool dropFlagToZone(Creature* pCreature, bool bSendPacket) {
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    if (!pPC->isFlag(Effect::EFFECT_CLASS_HAS_FLAG))
        return false;

    bool bDrop = false;


    Item* pSlotItem = pPC->getExtraInventorySlotItem();

    if (pSlotItem != NULL && pSlotItem->isFlagItem()) {
        if (dropFlagToZone(pPC, pSlotItem)) {
            pPC->deleteItemFromExtraInventorySlot();


            if (bSendPacket) {
                GCDeleteInventoryItem gcDeleteInventoryItem;
                gcDeleteInventoryItem.setObjectID(pSlotItem->getObjectID());

                pPC->getPlayer()->sendPacket(&gcDeleteInventoryItem);
            }

            bDrop = true;
        }
    }

    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);

    Inventory* pInventory = pPC->getInventory();
    Assert(pInventory != NULL);


    for (CoordInven_t y = 0; y < pInventory->getHeight(); y++) {
        for (CoordInven_t x = 0; x < pInventory->getWidth(); x++) {
            Item* pItem = pInventory->getItem(x, y);
            if (pItem != NULL && pItem->isFlagItem()) {
                if (dropFlagToZone(pPC, pItem)) {
                    pInventory->deleteItem(pItem->getObjectID());


                    if (bSendPacket) {
                        GCDeleteInventoryItem gcDeleteInventoryItem;
                        gcDeleteInventoryItem.setObjectID(pItem->getObjectID());

                        pPC->getPlayer()->sendPacket(&gcDeleteInventoryItem);
                    }

                    bDrop = true;
                }
            }
        }
    }

    return bDrop;
}

void disableFlags(Creature* pCreature, Zone* pZone, SkillType_t SkillType) {
    if (pCreature->isSlayer()) {
        if (pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)) {
            g_Sniping.checkRevealRatio(pCreature, 20, 10);
        }
    } else if (pCreature->isVampire() && pZone != NULL) {
        if (pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)) {
            addVisibleCreature(pZone, pCreature, true);
        }

        /*		if (pCreature->isFlag(Effect::EFFECT_CLASS_EXTREME))
                {
                    if(!(SkillType == SKILL_ACID_TOUCH) && !(SkillType == SKILL_POISONOUS_HANDS) && !(SkillType ==
           SKILL_BLOODY_NAIL))
                    {
                        EffectManager * pEffectManager = pCreature->getEffectManager();
                        Assert( pEffectManager != NULL );
                        Effect * pEffect = pEffectManager->findEffect( Effect::EFFECT_CLASS_EXTREME );
                        if ( pEffect != NULL )
                        {
                            pEffect->setDeadline(0);
                        }
                    }
                } */

    } else if (pCreature->isOusters()) {
    }
}

bool canEnterBeginnerZone(Creature* pCreature) {
    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        Assert(pSlayer != NULL);

        return pSlayer->getTotalAttr(ATTR_BASIC) <= 150;
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        Assert(pVampire != NULL);

        return pVampire->getLevel() <= 30;
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
        Assert(pOusters != NULL);

        return pOusters->getLevel() <= 30;
    }

    return false;
}

#ifdef __UNDERWORLD__
void giveUnderworldGift(Creature* pCreature) {
    if (!pCreature->isPC())
        return;

    string PlayerName;

    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        Assert(pSlayer != NULL);
        PlayerName = pSlayer->getName();
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        Assert(pVampire != NULL);
        PlayerName = pVampire->getName();
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
        Assert(pOusters != NULL);
        PlayerName = pOusters->getName();
    } else
        return;

    Player* pPlayer = pCreature->getPlayer();
    Assert(pPlayer != NULL);

    string PlayerID = pPlayer->getID();

    Statement* pStmt = NULL;

    filelog("Underworld.log", "[%s:%s]   .", PlayerID.c_str(), PlayerName.c_str());

    try {
        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getDistConnection("PLAYER_DB");
            PreparedStatement insertUnderworldEventStmt(
                pConn, "INSERT INTO UnderworldEvent (WorldID, ServerID, PlayerID, CharacterID, KillTime) "
                       "VALUES (?, ?, ?, ?, now())");
            insertUnderworldEventStmt.bindUInt(1, g_pConfig->getPropertyInt("WorldID"));
            insertUnderworldEventStmt.bindUInt(2, g_pConfig->getPropertyInt("ServerID"));
            insertUnderworldEventStmt.bindString(3, PlayerID);
            insertUnderworldEventStmt.bindString(4, PlayerName);
            insertUnderworldEventStmt.execute();
        }
        END_DB(pStmt)
    } catch (Throwable& t) {
        filelog("Underworld.log", "DB  . : %s", t.toString().c_str());
    }

    GCNotifyWin gcNW;
    gcNW.setGiftID(101);
    gcNW.setName(PlayerName);

    g_pZoneGroupManager->broadcast(&gcNW);

    char ggCommand[200];
    string worldName = g_pGameWorldInfoManager->getGameWorldInfo(g_pConfig->getPropertyInt("WorldID"))->getName();
    sprintf(ggCommand, "*allworld *command NotifyWin %s(%s) %u", PlayerName.c_str(), worldName.c_str(), 101);
    CGSayHandler::opworld(NULL, ggCommand, 0, false);
}
#endif

bool dropSweeperToZone(PlayerCreature* pPC, Item* pItem)

{
    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);

    pZone->addItemDelayed(pItem, pPC->getX(), pPC->getY(), false);

    Effect* pEffect = pPC->findEffect(Effect::EFFECT_CLASS_HAS_SWEEPER);
    if (pEffect != NULL)
        pEffect->setDeadline(0);

    if (!pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK)) {
        EffectRelicLock* pLock = new EffectRelicLock(pItem);
        pLock->setDeadline(10 * 10);
        pItem->setFlag(Effect::EFFECT_CLASS_RELIC_LOCK);
        pItem->getEffectManager().addEffect(pLock);
    }

    return true;
}

bool dropSweeperToZone(Creature* pCreature, bool bSendPacket) {
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    if (!pPC->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER))
        return false;

    bool bDrop = false;


    Item* pSlotItem = pPC->getExtraInventorySlotItem();

    if (pSlotItem != NULL && pSlotItem->getItemClass() == Item::ITEM_CLASS_SWEEPER) {
        if (dropSweeperToZone(pPC, pSlotItem)) {
            pPC->deleteItemFromExtraInventorySlot();


            if (bSendPacket) {
                GCDeleteInventoryItem gcDeleteInventoryItem;
                gcDeleteInventoryItem.setObjectID(pSlotItem->getObjectID());

                pPC->getPlayer()->sendPacket(&gcDeleteInventoryItem);
            }

            bDrop = true;
        }
    }

    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);

    Inventory* pInventory = pPC->getInventory();
    Assert(pInventory != NULL);


    for (CoordInven_t y = 0; y < pInventory->getHeight(); y++) {
        for (CoordInven_t x = 0; x < pInventory->getWidth(); x++) {
            Item* pItem = pInventory->getItem(x, y);
            if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_SWEEPER) {
                if (dropSweeperToZone(pPC, pItem)) {
                    pInventory->deleteItem(pItem->getObjectID());


                    if (bSendPacket) {
                        GCDeleteInventoryItem gcDeleteInventoryItem;
                        gcDeleteInventoryItem.setObjectID(pItem->getObjectID());

                        pPC->getPlayer()->sendPacket(&gcDeleteInventoryItem);
                    }

                    bDrop = true;
                }
            }
        }
    }

    return bDrop;
}

Level_t getPCLevel(PlayerCreature* pPC) {
    if (pPC->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
        return pSlayer->getHighestSkillDomainLevel();
    } else if (pPC->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
        return pVampire->getLevel();
    } else if (pPC->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
        return pOusters->getLevel();
    }

    return 0;
}

void sendPetInfo(GamePlayer* pGamePlayer, bool bBroadcast, bool bSummon) {
    if (pGamePlayer == NULL)
        return;

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
    if (pPC == NULL)
        return;

    PetInfo* pPetInfo = pPC->getPetInfo();

    GCPetInfo gcPetInfo;
    gcPetInfo.setPetInfo(pPetInfo);
    gcPetInfo.setSummonInfo((bSummon) ? 1 : 0);
    gcPetInfo.setObjectID(pPC->getObjectID());
    pGamePlayer->sendPacket(&gcPetInfo);

    if (bBroadcast) {
        pPC->getZone()->broadcastPacket(pPC->getX(), pPC->getY(), &gcPetInfo, pPC);
    }
}

void giveGoldMedal(PlayerCreature* pPC) {
    __BEGIN_TRY

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
    Assert(pGamePlayer != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getDistConnection("USERINFO");
        PreparedStatement insertGoldMedalCountStmt(pConn,
                                                   "INSERT INTO GoldMedalCount (PlayerID, getTime) VALUES (?, now())");
        insertGoldMedalCountStmt.bindString(1, pGamePlayer->getID());
        insertGoldMedalCountStmt.execute();
        addSimpleCreatureEffect(pPC, Effect::EFFECT_CLASS_GOLD_MEDAL, 10, true);

        GCSystemMessage gcSM;
        gcSM.setMessage(".");
        pGamePlayer->sendPacket(&gcSM);
    }
    END_DB(pStmt);

    __END_CATCH
}

void giveLotto(PlayerCreature* pPC, BYTE type, uint num) {
    __BEGIN_TRY

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
    Assert(pGamePlayer != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getDistConnection("USERINFO");

        PreparedStatement updateEventLottoStmt(pConn,
                                               "UPDATE EventLotto SET count=count+? WHERE PlayerID=? AND Type=?");
        updateEventLottoStmt.bindUInt(1, num);
        updateEventLottoStmt.bindString(2, pGamePlayer->getID());
        updateEventLottoStmt.bindUInt(3, type);
        updateEventLottoStmt.execute();

        if (updateEventLottoStmt.getAffectedRowCount() < 1) {
            PreparedStatement replaceEventLottoStmt(pConn,
                                                    "REPLACE INTO EventLotto (PlayerID,Type,count) VALUES (?,?,?)");
            replaceEventLottoStmt.bindString(1, pGamePlayer->getID());
            replaceEventLottoStmt.bindUInt(2, type);
            replaceEventLottoStmt.bindUInt(3, num);
            replaceEventLottoStmt.execute();
        }

        PreparedStatement selectEventLottoStmt(pConn, "SELECT count FROM EventLotto WHERE PlayerID=? AND Type=?");
        selectEventLottoStmt.bindString(1, pGamePlayer->getID());
        selectEventLottoStmt.bindUInt(2, type);
        Result* pResult = selectEventLottoStmt.execute();

        if (pResult->next()) {
            char buffer[256];
            sprintf(buffer, "%d..", pResult->getInt(1));
            GCSystemMessage gcSM;
            gcSM.setMessage(buffer);
            pGamePlayer->sendPacket(&gcSM);
        }
    }
    END_DB(pStmt);

    __END_CATCH
}

void addOlympicStat(PlayerCreature* pPC, BYTE type, uint num) {
    __BEGIN_TRY

    __END_CATCH
}

void deletePC(PlayerCreature* pPC) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement inactivateSlayerStmt(pConn, "UPDATE Slayer SET Active='INACTIVE' WHERE Name = ?");
        inactivateSlayerStmt.bindString(1, pPC->getName());
        inactivateSlayerStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement inactivateVampireStmt(pConn, "UPDATE Vampire SET Active='INACTIVE' WHERE Name = ?");
        inactivateVampireStmt.bindString(1, pPC->getName());
        inactivateVampireStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement inactivateOustersStmt(pConn, "UPDATE Ousters SET Active='INACTIVE' WHERE Name = ?");
        inactivateOustersStmt.bindString(1, pPC->getName());
        inactivateOustersStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteSkillSaveStmt(pConn, "DELETE FROM SkillSave WHERE OwnerID = ?");
        deleteSkillSaveStmt.bindString(1, pPC->getName());
        deleteSkillSaveStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteVampireSkillSaveStmt(pConn, "DELETE FROM VampireSkillSave WHERE OwnerID = ?");
        deleteVampireSkillSaveStmt.bindString(1, pPC->getName());
        deleteVampireSkillSaveStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteOustersSkillSaveStmt(pConn, "DELETE FROM OustersSkillSave WHERE OwnerID = ?");
        deleteOustersSkillSaveStmt.bindString(1, pPC->getName());
        deleteOustersSkillSaveStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteRankBonusDataStmt(pConn, "DELETE FROM RankBonusData WHERE OwnerID = ?");
        deleteRankBonusDataStmt.bindString(1, pPC->getName());
        deleteRankBonusDataStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        string ownerID = pPC->getName();
        PreparedStatement deleteARObjectStmt(pConn, "DELETE FROM ARObject WHERE OwnerID = ?");
        deleteARObjectStmt.bindString(1, ownerID);
        deleteARObjectStmt.execute();
        PreparedStatement deleteBeltObjectStmt(pConn, "DELETE FROM BeltObject WHERE OwnerID = ?");
        deleteBeltObjectStmt.bindString(1, ownerID);
        deleteBeltObjectStmt.execute();
        PreparedStatement deleteBladeObjectStmt(pConn, "DELETE FROM BladeObject WHERE OwnerID = ?");
        deleteBladeObjectStmt.bindString(1, ownerID);
        deleteBladeObjectStmt.execute();
        PreparedStatement deleteBloodBibleObjectStmt(pConn, "DELETE FROM BloodBibleObject WHERE OwnerID = ?");
        deleteBloodBibleObjectStmt.bindString(1, ownerID);
        deleteBloodBibleObjectStmt.execute();
        PreparedStatement deleteBombMaterialObjectStmt(pConn, "DELETE FROM BombMaterialObject WHERE OwnerID = ?");
        deleteBombMaterialObjectStmt.bindString(1, ownerID);
        deleteBombMaterialObjectStmt.execute();
        PreparedStatement deleteBombObjectStmt(pConn, "DELETE FROM BombObject WHERE OwnerID = ?");
        deleteBombObjectStmt.bindString(1, ownerID);
        deleteBombObjectStmt.execute();
        PreparedStatement deleteBraceletObjectStmt(pConn, "DELETE FROM BraceletObject WHERE OwnerID = ?");
        deleteBraceletObjectStmt.bindString(1, ownerID);
        deleteBraceletObjectStmt.execute();
        PreparedStatement deleteCastleSymbolObjectStmt(pConn, "DELETE FROM CastleSymbolObject WHERE OwnerID = ?");
        deleteCastleSymbolObjectStmt.bindString(1, ownerID);
        deleteCastleSymbolObjectStmt.execute();
        PreparedStatement deleteCoatObjectStmt(pConn, "DELETE FROM CoatObject WHERE OwnerID = ?");
        deleteCoatObjectStmt.bindString(1, ownerID);
        deleteCoatObjectStmt.execute();
        PreparedStatement deleteCrossObjectStmt(pConn, "DELETE FROM CrossObject WHERE OwnerID = ?");
        deleteCrossObjectStmt.bindString(1, ownerID);
        deleteCrossObjectStmt.execute();
        PreparedStatement deleteETCObjectStmt(pConn, "DELETE FROM ETCObject WHERE OwnerID = ?");
        deleteETCObjectStmt.bindString(1, ownerID);
        deleteETCObjectStmt.execute();
        PreparedStatement deleteEventETCObjectStmt(pConn, "DELETE FROM EventETCObject WHERE OwnerID = ?");
        deleteEventETCObjectStmt.bindString(1, ownerID);
        deleteEventETCObjectStmt.execute();
        PreparedStatement deleteEventGiftBoxObjectStmt(pConn, "DELETE FROM EventGiftBoxObject WHERE OwnerID = ?");
        deleteEventGiftBoxObjectStmt.bindString(1, ownerID);
        deleteEventGiftBoxObjectStmt.execute();
        PreparedStatement deleteEventStarObjectStmt(pConn, "DELETE FROM EventStarObject WHERE OwnerID = ?");
        deleteEventStarObjectStmt.bindString(1, ownerID);
        deleteEventStarObjectStmt.execute();
        PreparedStatement deleteEventTreeObjectStmt(pConn, "DELETE FROM EventTreeObject WHERE OwnerID = ?");
        deleteEventTreeObjectStmt.bindString(1, ownerID);
        deleteEventTreeObjectStmt.execute();
        PreparedStatement deleteGloveObjectStmt(pConn, "DELETE FROM GloveObject WHERE OwnerID = ?");
        deleteGloveObjectStmt.bindString(1, ownerID);
        deleteGloveObjectStmt.execute();
        PreparedStatement deleteHelmObjectStmt(pConn, "DELETE FROM HelmObject WHERE OwnerID = ?");
        deleteHelmObjectStmt.bindString(1, ownerID);
        deleteHelmObjectStmt.execute();
        PreparedStatement deleteHolyWaterObjectStmt(pConn, "DELETE FROM HolyWaterObject WHERE OwnerID = ?");
        deleteHolyWaterObjectStmt.bindString(1, ownerID);
        deleteHolyWaterObjectStmt.execute();
        PreparedStatement deleteKeyObjectStmt(pConn, "DELETE FROM KeyObject WHERE OwnerID = ?");
        deleteKeyObjectStmt.bindString(1, ownerID);
        deleteKeyObjectStmt.execute();
        PreparedStatement deleteLearningItemObjectStmt(pConn, "DELETE FROM LearningItemObject WHERE OwnerID = ?");
        deleteLearningItemObjectStmt.bindString(1, ownerID);
        deleteLearningItemObjectStmt.execute();
        PreparedStatement deleteMaceObjectStmt(pConn, "DELETE FROM MaceObject WHERE OwnerID = ?");
        deleteMaceObjectStmt.bindString(1, ownerID);
        deleteMaceObjectStmt.execute();
        PreparedStatement deleteMagazineObjectStmt(pConn, "DELETE FROM MagazineObject WHERE OwnerID = ?");
        deleteMagazineObjectStmt.bindString(1, ownerID);
        deleteMagazineObjectStmt.execute();
        PreparedStatement deleteMineObjectStmt(pConn, "DELETE FROM MineObject WHERE OwnerID = ?");
        deleteMineObjectStmt.bindString(1, ownerID);
        deleteMineObjectStmt.execute();
        PreparedStatement deleteMoneyObjectStmt(pConn, "DELETE FROM MoneyObject WHERE OwnerID = ?");
        deleteMoneyObjectStmt.bindString(1, ownerID);
        deleteMoneyObjectStmt.execute();
        PreparedStatement deleteMotorcycleObjectStmt(pConn, "DELETE FROM MotorcycleObject WHERE OwnerID = ?");
        deleteMotorcycleObjectStmt.bindString(1, ownerID);
        deleteMotorcycleObjectStmt.execute();
        PreparedStatement deleteNecklaceObjectStmt(pConn, "DELETE FROM NecklaceObject WHERE OwnerID = ?");
        deleteNecklaceObjectStmt.bindString(1, ownerID);
        deleteNecklaceObjectStmt.execute();
        PreparedStatement deletePotionObjectStmt(pConn, "DELETE FROM PotionObject WHERE OwnerID = ?");
        deletePotionObjectStmt.bindString(1, ownerID);
        deletePotionObjectStmt.execute();
        PreparedStatement deleteQuestItemObjectStmt(pConn, "DELETE FROM QuestItemObject WHERE OwnerID = ?");
        deleteQuestItemObjectStmt.bindString(1, ownerID);
        deleteQuestItemObjectStmt.execute();
        PreparedStatement deleteRelicObjectStmt(pConn, "DELETE FROM RelicObject WHERE OwnerID = ?");
        deleteRelicObjectStmt.bindString(1, ownerID);
        deleteRelicObjectStmt.execute();
        PreparedStatement deleteSGObjectStmt(pConn, "DELETE FROM SGObject WHERE OwnerID = ?");
        deleteSGObjectStmt.bindString(1, ownerID);
        deleteSGObjectStmt.execute();
        PreparedStatement deleteSMGObjectStmt(pConn, "DELETE FROM SMGObject WHERE OwnerID = ?");
        deleteSMGObjectStmt.bindString(1, ownerID);
        deleteSMGObjectStmt.execute();
        PreparedStatement deleteSRObjectStmt(pConn, "DELETE FROM SRObject WHERE OwnerID = ?");
        deleteSRObjectStmt.bindString(1, ownerID);
        deleteSRObjectStmt.execute();
        PreparedStatement deleteSerumObjectStmt(pConn, "DELETE FROM SerumObject WHERE OwnerID = ?");
        deleteSerumObjectStmt.bindString(1, ownerID);
        deleteSerumObjectStmt.execute();
        PreparedStatement deleteShieldObjectStmt(pConn, "DELETE FROM ShieldObject WHERE OwnerID = ?");
        deleteShieldObjectStmt.bindString(1, ownerID);
        deleteShieldObjectStmt.execute();
        PreparedStatement deleteShoesObjectStmt(pConn, "DELETE FROM ShoesObject WHERE OwnerID = ?");
        deleteShoesObjectStmt.bindString(1, ownerID);
        deleteShoesObjectStmt.execute();
        PreparedStatement deleteSkullObjectStmt(pConn, "DELETE FROM SkullObject WHERE OwnerID = ?");
        deleteSkullObjectStmt.bindString(1, ownerID);
        deleteSkullObjectStmt.execute();
        PreparedStatement deleteSlayerPortalItemObjectStmt(pConn,
                                                           "DELETE FROM SlayerPortalItemObject WHERE OwnerID = ?");
        deleteSlayerPortalItemObjectStmt.bindString(1, ownerID);
        deleteSlayerPortalItemObjectStmt.execute();
        PreparedStatement deleteSwordObjectStmt(pConn, "DELETE FROM SwordObject WHERE OwnerID = ?");
        deleteSwordObjectStmt.bindString(1, ownerID);
        deleteSwordObjectStmt.execute();
        PreparedStatement deleteTrouserObjectStmt(pConn, "DELETE FROM TrouserObject WHERE OwnerID = ?");
        deleteTrouserObjectStmt.bindString(1, ownerID);
        deleteTrouserObjectStmt.execute();
        PreparedStatement deleteRingObjectStmt(pConn, "DELETE FROM RingObject WHERE OwnerID = ?");
        deleteRingObjectStmt.bindString(1, ownerID);
        deleteRingObjectStmt.execute();
        PreparedStatement deleteCoupleRingObjectStmt(pConn, "DELETE FROM CoupleRingObject WHERE OwnerID = ?");
        deleteCoupleRingObjectStmt.bindString(1, ownerID);
        deleteCoupleRingObjectStmt.execute();
        PreparedStatement deleteVampireAmuletObjectStmt(pConn, "DELETE FROM VampireAmuletObject WHERE OwnerID = ?");
        deleteVampireAmuletObjectStmt.bindString(1, ownerID);
        deleteVampireAmuletObjectStmt.execute();
        PreparedStatement deleteVampireBraceletObjectStmt(pConn, "DELETE FROM VampireBraceletObject WHERE OwnerID = ?");
        deleteVampireBraceletObjectStmt.bindString(1, ownerID);
        deleteVampireBraceletObjectStmt.execute();
        PreparedStatement deleteVampireCoatObjectStmt(pConn, "DELETE FROM VampireCoatObject WHERE OwnerID = ?");
        deleteVampireCoatObjectStmt.bindString(1, ownerID);
        deleteVampireCoatObjectStmt.execute();
        PreparedStatement deleteVampireETCObjectStmt(pConn, "DELETE FROM VampireETCObject WHERE OwnerID = ?");
        deleteVampireETCObjectStmt.bindString(1, ownerID);
        deleteVampireETCObjectStmt.execute();
        PreparedStatement deleteVampireEarringObjectStmt(pConn, "DELETE FROM VampireEarringObject WHERE OwnerID = ?");
        deleteVampireEarringObjectStmt.bindString(1, ownerID);
        deleteVampireEarringObjectStmt.execute();
        PreparedStatement deleteVampireNecklaceObjectStmt(pConn, "DELETE FROM VampireNecklaceObject WHERE OwnerID = ?");
        deleteVampireNecklaceObjectStmt.bindString(1, ownerID);
        deleteVampireNecklaceObjectStmt.execute();
        PreparedStatement deleteVampirePortalItemObjectStmt(pConn,
                                                            "DELETE FROM VampirePortalItemObject WHERE OwnerID = ?");
        deleteVampirePortalItemObjectStmt.bindString(1, ownerID);
        deleteVampirePortalItemObjectStmt.execute();
        PreparedStatement deleteVampireRingObjectStmt(pConn, "DELETE FROM VampireRingObject WHERE OwnerID = ?");
        deleteVampireRingObjectStmt.bindString(1, ownerID);
        deleteVampireRingObjectStmt.execute();
        PreparedStatement deleteVampireWeaponObjectStmt(pConn, "DELETE FROM VampireWeaponObject WHERE OwnerID = ?");
        deleteVampireWeaponObjectStmt.bindString(1, ownerID);
        deleteVampireWeaponObjectStmt.execute();
        PreparedStatement deleteVampireCoupleRingObjectStmt(pConn,
                                                            "DELETE FROM VampireCoupleRingObject WHERE OwnerID = ?");
        deleteVampireCoupleRingObjectStmt.bindString(1, ownerID);
        deleteVampireCoupleRingObjectStmt.execute();
        PreparedStatement deleteWaterObjectStmt(pConn, "DELETE FROM WaterObject WHERE OwnerID = ?");
        deleteWaterObjectStmt.bindString(1, ownerID);
        deleteWaterObjectStmt.execute();
        PreparedStatement deleteEventItemObjectStmt(pConn, "DELETE FROM EventItemObject WHERE OwnerID = ?");
        deleteEventItemObjectStmt.bindString(1, ownerID);
        deleteEventItemObjectStmt.execute();
        PreparedStatement deleteDyePotionObjectStmt(pConn, "DELETE FROM DyePotionObject WHERE OwnerID = ?");
        deleteDyePotionObjectStmt.bindString(1, ownerID);
        deleteDyePotionObjectStmt.execute();
        PreparedStatement deleteResurrectItemObjectStmt(pConn, "DELETE FROM ResurrectItemObject WHERE OwnerID = ?");
        deleteResurrectItemObjectStmt.bindString(1, ownerID);
        deleteResurrectItemObjectStmt.execute();
        PreparedStatement deleteMixingItemObjectStmt(pConn, "DELETE FROM MixingItemObject WHERE OwnerID = ?");
        deleteMixingItemObjectStmt.bindString(1, ownerID);
        deleteMixingItemObjectStmt.execute();
        PreparedStatement deleteOustersArmsbandObjectStmt(pConn, "DELETE FROM OustersArmsbandObject WHERE OwnerID = ?");
        deleteOustersArmsbandObjectStmt.bindString(1, ownerID);
        deleteOustersArmsbandObjectStmt.execute();
        PreparedStatement deleteOustersBootsObjectStmt(pConn, "DELETE FROM OustersBootsObject WHERE OwnerID = ?");
        deleteOustersBootsObjectStmt.bindString(1, ownerID);
        deleteOustersBootsObjectStmt.execute();
        PreparedStatement deleteOustersChakramObjectStmt(pConn, "DELETE FROM OustersChakramObject WHERE OwnerID = ?");
        deleteOustersChakramObjectStmt.bindString(1, ownerID);
        deleteOustersChakramObjectStmt.execute();
        PreparedStatement deleteOustersCircletObjectStmt(pConn, "DELETE FROM OustersCircletObject WHERE OwnerID = ?");
        deleteOustersCircletObjectStmt.bindString(1, ownerID);
        deleteOustersCircletObjectStmt.execute();
        PreparedStatement deleteOustersCoatObjectStmt(pConn, "DELETE FROM OustersCoatObject WHERE OwnerID = ?");
        deleteOustersCoatObjectStmt.bindString(1, ownerID);
        deleteOustersCoatObjectStmt.execute();
        PreparedStatement deleteOustersPendentObjectStmt(pConn, "DELETE FROM OustersPendentObject WHERE OwnerID = ?");
        deleteOustersPendentObjectStmt.bindString(1, ownerID);
        deleteOustersPendentObjectStmt.execute();
        PreparedStatement deleteOustersRingObjectStmt(pConn, "DELETE FROM OustersRingObject WHERE OwnerID = ?");
        deleteOustersRingObjectStmt.bindString(1, ownerID);
        deleteOustersRingObjectStmt.execute();
        PreparedStatement deleteOustersStoneObjectStmt(pConn, "DELETE FROM OustersStoneObject WHERE OwnerID = ?");
        deleteOustersStoneObjectStmt.bindString(1, ownerID);
        deleteOustersStoneObjectStmt.execute();
        PreparedStatement deleteOustersWristletObjectStmt(pConn, "DELETE FROM OustersWristletObject WHERE OwnerID = ?");
        deleteOustersWristletObjectStmt.bindString(1, ownerID);
        deleteOustersWristletObjectStmt.execute();
        PreparedStatement deleteLarvaObjectStmt(pConn, "DELETE FROM LarvaObject WHERE OwnerID = ?");
        deleteLarvaObjectStmt.bindString(1, ownerID);
        deleteLarvaObjectStmt.execute();
        PreparedStatement deletePupaObjectStmt(pConn, "DELETE FROM PupaObject WHERE OwnerID = ?");
        deletePupaObjectStmt.bindString(1, ownerID);
        deletePupaObjectStmt.execute();
        PreparedStatement deleteComposMeiObjectStmt(pConn, "DELETE FROM ComposMeiObject WHERE OwnerID = ?");
        deleteComposMeiObjectStmt.bindString(1, ownerID);
        deleteComposMeiObjectStmt.execute();
        PreparedStatement deleteOustersSummonItemObjectStmt(pConn,
                                                            "DELETE FROM OustersSummonItemObject WHERE OwnerID = ?");
        deleteOustersSummonItemObjectStmt.bindString(1, ownerID);
        deleteOustersSummonItemObjectStmt.execute();
        PreparedStatement deleteEffectItemObjectStmt(pConn, "DELETE FROM EffectItemObject WHERE OwnerID = ?");
        deleteEffectItemObjectStmt.bindString(1, ownerID);
        deleteEffectItemObjectStmt.execute();
        PreparedStatement deleteCodeSheetObjectStmt(pConn, "DELETE FROM CodeSheetObject WHERE OwnerID = ?");
        deleteCodeSheetObjectStmt.bindString(1, ownerID);
        deleteCodeSheetObjectStmt.execute();
        PreparedStatement deleteMoonCardObjectStmt(pConn, "DELETE FROM MoonCardObject WHERE OwnerID = ?");
        deleteMoonCardObjectStmt.bindString(1, ownerID);
        deleteMoonCardObjectStmt.execute();
        PreparedStatement deleteSweeperObjectStmt(pConn, "DELETE FROM SweeperObject WHERE OwnerID = ?");
        deleteSweeperObjectStmt.bindString(1, ownerID);
        deleteSweeperObjectStmt.execute();
        PreparedStatement deletePetItemObjectStmt(pConn, "DELETE FROM PetItemObject WHERE OwnerID = ?");
        deletePetItemObjectStmt.bindString(1, ownerID);
        deletePetItemObjectStmt.execute();
        PreparedStatement deletePetFoodObjectStmt(pConn, "DELETE FROM PetFoodObject WHERE OwnerID = ?");
        deletePetFoodObjectStmt.bindString(1, ownerID);
        deletePetFoodObjectStmt.execute();
        PreparedStatement deletePetEnchantItemObjectStmt(pConn, "DELETE FROM PetEnchantItemObject WHERE OwnerID = ?");
        deletePetEnchantItemObjectStmt.bindString(1, ownerID);
        deletePetEnchantItemObjectStmt.execute();
        PreparedStatement deleteLuckyBagObjectStmt(pConn, "DELETE FROM LuckyBagObject WHERE OwnerID = ?");
        deleteLuckyBagObjectStmt.bindString(1, ownerID);
        deleteLuckyBagObjectStmt.execute();
        PreparedStatement deleteSMSItemObjectStmt(pConn, "DELETE FROM SMSItemObject WHERE OwnerID = ?");
        deleteSMSItemObjectStmt.bindString(1, ownerID);
        deleteSMSItemObjectStmt.execute();
        PreparedStatement deleteCoreZapObjectStmt(pConn, "DELETE FROM CoreZapObject WHERE OwnerID = ?");
        deleteCoreZapObjectStmt.bindString(1, ownerID);
        deleteCoreZapObjectStmt.execute();
        PreparedStatement deleteGQuestItemObjectStmt(pConn, "DELETE FROM GQuestItemObject WHERE OwnerID = ?");
        deleteGQuestItemObjectStmt.bindString(1, ownerID);
        deleteGQuestItemObjectStmt.execute();
        PreparedStatement deleteGQuestSaveStmt(pConn, "DELETE FROM GQuestSave WHERE OwnerID = ?");
        deleteGQuestSaveStmt.bindString(1, ownerID);
        deleteGQuestSaveStmt.execute();
        PreparedStatement deleteTrapItemObjectStmt(pConn, "DELETE FROM TrapItemObject WHERE OwnerID = ?");
        deleteTrapItemObjectStmt.bindString(1, ownerID);
        deleteTrapItemObjectStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteCoupleInfoFemalePartnerNameStmt(pConn,
                                                                "DELETE FROM CoupleInfo WHERE FemalePartnerName=?");
        deleteCoupleInfoFemalePartnerNameStmt.bindString(1, ownerID);
        deleteCoupleInfoFemalePartnerNameStmt.execute();
        PreparedStatement deleteCoupleInfoMalePartnerNameStmt(pConn, "DELETE FROM CoupleInfo WHERE MalePartnerName=?");
        deleteCoupleInfoMalePartnerNameStmt.bindString(1, ownerID);
        deleteCoupleInfoMalePartnerNameStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteEffectAcidTouchStmt(pConn, "DELETE FROM EffectAcidTouch where OwnerID=?");
        deleteEffectAcidTouchStmt.bindString(1, pPC->getName());
        deleteEffectAcidTouchStmt.execute();
        PreparedStatement deleteEffectAftermathStmt(pConn, "DELETE FROM EffectAftermath where OwnerID=?");
        deleteEffectAftermathStmt.bindString(1, pPC->getName());
        deleteEffectAftermathStmt.execute();
        PreparedStatement deleteEffectBloodDrainStmt(pConn, "DELETE FROM EffectBloodDrain where OwnerID=?");
        deleteEffectBloodDrainStmt.bindString(1, pPC->getName());
        deleteEffectBloodDrainStmt.execute();
        PreparedStatement deleteEffectDetectHiddenStmt(pConn, "DELETE FROM EffectDetectHidden where OwnerID=?");
        deleteEffectDetectHiddenStmt.bindString(1, pPC->getName());
        deleteEffectDetectHiddenStmt.execute();
        PreparedStatement deleteEffectFlareStmt(pConn, "DELETE FROM EffectFlare where OwnerID=?");
        deleteEffectFlareStmt.bindString(1, pPC->getName());
        deleteEffectFlareStmt.execute();
        PreparedStatement deleteEffectLightStmt(pConn, "DELETE FROM EffectLight where OwnerID=?");
        deleteEffectLightStmt.bindString(1, pPC->getName());
        deleteEffectLightStmt.execute();
        PreparedStatement deleteEffectParalysisStmt(pConn, "DELETE FROM EffectParalysis where OwnerID=?");
        deleteEffectParalysisStmt.bindString(1, pPC->getName());
        deleteEffectParalysisStmt.execute();
        PreparedStatement deleteEffectPoisonStmt(pConn, "DELETE FROM EffectPoison where OwnerID=?");
        deleteEffectPoisonStmt.bindString(1, pPC->getName());
        deleteEffectPoisonStmt.execute();
        PreparedStatement deleteEffectPoisonousHandsStmt(pConn, "DELETE FROM EffectPoisonousHands where OwnerID=?");
        deleteEffectPoisonousHandsStmt.bindString(1, pPC->getName());
        deleteEffectPoisonousHandsStmt.execute();
        PreparedStatement deleteEffectProtectionFromParalysisStmt(
            pConn, "DELETE FROM EffectProtectionFromParalysis where OwnerID=?");
        deleteEffectProtectionFromParalysisStmt.bindString(1, pPC->getName());
        deleteEffectProtectionFromParalysisStmt.execute();
        PreparedStatement deleteEffectProtectionFromPoisonStmt(
            pConn, "DELETE FROM EffectProtectionFromPoison where OwnerID=?");
        deleteEffectProtectionFromPoisonStmt.bindString(1, pPC->getName());
        deleteEffectProtectionFromPoisonStmt.execute();
        PreparedStatement deleteEffectRestoreStmt(pConn, "DELETE FROM EffectRestore where OwnerID=?");
        deleteEffectRestoreStmt.bindString(1, pPC->getName());
        deleteEffectRestoreStmt.execute();
        PreparedStatement deleteEffectYellowPoisonToCreatureStmt(
            pConn, "DELETE FROM EffectYellowPoisonToCreature where OwnerID=?");
        deleteEffectYellowPoisonToCreatureStmt.bindString(1, pPC->getName());
        deleteEffectYellowPoisonToCreatureStmt.execute();
        PreparedStatement deleteEffectMuteStmt(pConn, "DELETE FROM EffectMute where OwnerID=?");
        deleteEffectMuteStmt.bindString(1, pPC->getName());
        deleteEffectMuteStmt.execute();
        PreparedStatement deleteEnemyEraseStmt(pConn, "DELETE FROM EnemyErase where OwnerID=?");
        deleteEnemyEraseStmt.bindString(1, pPC->getName());
        deleteEnemyEraseStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteFlagSetStmt(pConn, "DELETE FROM FlagSet WHERE OwnerID=?");
        deleteFlagSetStmt.bindString(1, pPC->getName());
        deleteFlagSetStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteTimeLimitItemsStmt(pConn, "DELETE FROM TimeLimitItems WHERE OwnerID=?");
        deleteTimeLimitItemsStmt.bindString(1, pPC->getName());
        deleteTimeLimitItemsStmt.execute();

        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        PreparedStatement deleteEventQuestAdvanceStmt(pConn, "DELETE FROM EventQuestAdvance WHERE OwnerID=?");
        deleteEventQuestAdvanceStmt.bindString(1, pPC->getName());
        deleteEventQuestAdvanceStmt.execute();
    }
    END_DB(pStmt);

    __END_CATCH
}

bool isAffectExp2X() {
    if (g_pVariableManager->getVariable(TIME_PERIOD_EXP_2X) != 0) {
        if (g_pTimeChecker->isInPeriod(TIME_PERIOD_AFTER_SCHOOL) ||
            g_pTimeChecker->isInPeriod(TIME_PERIOD_AFTER_WORK) || g_pTimeChecker->isInPeriod(TIME_PERIOD_MIDNIGHT)) {
            return true;
        }
    }

    return false;
}
