//////////////////////////////////////////////////////////////////////////////
// Filename    : Hide.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Hide.h"

#include "GCDeleteObject.h"
#include "GCSkillToSelfOK1.h"
#include "GCSkillToSelfOK3.h"
#include "RankBonus.h"
#include "ZoneUtil.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Hide::execute(Vampire* pVampire, VampireSkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    Assert(pVampire != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pVampire->getPlayer();
        Zone* pZone = pVampire->getZone();

        Assert(pPlayer != NULL);
        Assert(pZone != NULL);

        GCSkillToSelfOK1 _GCSkillToSelfOK1;
        GCSkillToSelfOK3 _GCSkillToSelfOK3;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

        ZoneCoord_t x = pVampire->getX();
        ZoneCoord_t y = pVampire->getY();

        
        int HitBonus = 0;
        if (pVampire->hasRankBonus(RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE)) {
            RankBonus* pRankBonus = pVampire->getRankBonus(RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE);
            Assert(pRankBonus != NULL);

            HitBonus = pRankBonus->getPoint();
        }

        Tile& rTile = pZone->getTile(pVampire->getX(), pVampire->getY());
        int RequiredMP = decreaseConsumeMP(pVampire, pSkillInfo);
        bool bManaCheck = hasEnoughMana(pVampire, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);
        bool bHitRoll = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pSkillSlot, HitBonus);
        bool bTileCheck = canBurrow(pZone, x, y);
        bool bMoveModeCheck = pVampire->isWalking();
        bool bEffected = pVampire->isFlag(Effect::EFFECT_CLASS_HIDE) || pVampire->hasRelicItem() ||
                         pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) ||
                         pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER) ||
                         rTile.getEffect(Effect::EFFECT_CLASS_TRYING_POSITION) != NULL;

        if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck && bMoveModeCheck && !bEffected) {
            decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

            
            _GCSkillToSelfOK1.setSkillType(SkillType);
            _GCSkillToSelfOK1.setCEffectID(CEffectID);
            _GCSkillToSelfOK1.setDuration(0);

            _GCSkillToSelfOK3.setXY(x, y);
            _GCSkillToSelfOK3.setSkillType(SkillType);
            _GCSkillToSelfOK3.setDuration(0);

            pPlayer->sendPacket(&_GCSkillToSelfOK1);
            pZone->broadcastPacket(x, y, &_GCSkillToSelfOK3, pVampire);

            
            GCDeleteObject gcDO;
            gcDO.setObjectID(pVampire->getObjectID());
            pZone->broadcastPacket(x, y, &gcDO, pVampire);

            
            addBurrowingCreature(pZone, pVampire, x, y);

            pSkillSlot->setRunTime();
        } else {
            executeSkillFailNormal(pVampire, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pVampire, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Hide::execute(Monster* pMonster)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    Assert(pMonster != NULL);

    try {
        Zone* pZone = pMonster->getZone();
        Assert(pZone != NULL);

        if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE)) {
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }
        if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)) {
            addVisibleCreature(pZone, pMonster, true);
        }

        GCSkillToSelfOK3 _GCSkillToSelfOK3;

        SkillType_t SkillType = SKILL_HIDE;
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

        ZoneCoord_t x = pMonster->getX();
        ZoneCoord_t y = pMonster->getY();

        bool bRangeCheck = checkZoneLevelToUseSkill(pMonster);
        bool bHitRoll = HitRoll::isSuccessMagic(pMonster, pSkillInfo);
        bool bTileCheck = canBurrow(pZone, x, y);
        bool bMoveModeCheck = pMonster->isWalking();
        bool bEffected = pMonster->isFlag(Effect::EFFECT_CLASS_HIDE);

        if (bRangeCheck && bHitRoll && bTileCheck && bMoveModeCheck && !bEffected) {
            
            _GCSkillToSelfOK3.setXY(x, y);
            _GCSkillToSelfOK3.setDuration(0);
            _GCSkillToSelfOK3.setSkillType(SkillType);

            pZone->broadcastPacket(x, y, &_GCSkillToSelfOK3);

            
            GCDeleteObject gcDO;
            gcDO.setObjectID(pMonster->getObjectID());
            pZone->broadcastPacket(x, y, &gcDO);

            
            addBurrowingCreature(pZone, pMonster, x, y);
        } else {
            executeSkillFailNormal(pMonster, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pMonster, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

Hide g_Hide;
