//////////////////////////////////////////////////////////////////////////////
// Filename    : CGThrowBombHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGThrowBomb.h"

#ifdef __GAME_SERVER__
#include <list>

#include "CheckedCast.h"
#include "GCSkillFailed1.h"
#include "GCSkillFailed2.h"
#include "GCStatusCurrentHP.h"
#include "GCThrowBombOK1.h"
#include "GCThrowBombOK2.h"
#include "GCThrowBombOK3.h"
#include "GamePlayer.h"
#include "ItemUtil.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "ZoneUtil.h"
#include "item/Bomb.h"
#include "skill/Sniping.h"
#endif // __GAME_SERVER__

#ifdef __GAME_SERVER__




int BombMask[5][9][9] = {
    
    {{0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0}},
    
    {{0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0}},
    
    {{0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 100, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0}},
    
    {{0, 50, 0, 0, 100, 0, 0, 50, 0},
     {50, 0, 0, 0, 100, 0, 0, 0, 50},
     {0, 0, 0, 50, 100, 50, 0, 0, 0},
     {0, 0, 50, 0, 100, 0, 50, 0, 0},
     {0, 50, 0, 0, 100, 0, 0, 50, 0},
     {50, 0, 0, 0, 100, 0, 0, 0, 50},
     {0, 0, 0, 50, 100, 50, 0, 0, 0},
     {0, 0, 50, 0, 100, 0, 50, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0}},
    
    {{0, 50, 0, 50, 100, 50, 0, 50, 0},
     {50, 0, 50, 0, 100, 0, 50, 0, 50},
     {0, 50, 0, 50, 100, 50, 0, 50, 0},
     {50, 0, 50, 0, 100, 0, 50, 0, 50},
     {0, 50, 0, 50, 100, 50, 0, 50, 0},
     {50, 0, 50, 0, 100, 0, 50, 0, 50},
     {0, 50, 0, 50, 100, 50, 0, 50, 0},
     {50, 0, 50, 0, 100, 0, 50, 0, 50},
     {0, 0, 0, 0, 0, 0, 0, 0, 0}}};

#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGThrowBombHandler::execute(CGThrowBomb* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    if (pGamePlayer->getPlayerStatus() != GPS_NORMAL)
        return;

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    
    if (!pCreature->isSlayer())
        return;

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    int ZoneX = pPacket->getZoneX();
    int ZoneY = pPacket->getZoneY();
    int BombX = pPacket->getBombX();
    int BombY = pPacket->getBombY();

    Inventory* pInventory = pSlayer->getInventory();
    Assert(pInventory != NULL);


    
    ZoneLevel_t ZoneLevel = pZone->getZoneLevel(pSlayer->getX(), pSlayer->getY());
    if ((ZoneLevel & COMPLETE_SAFE_ZONE) || (BombX >= pInventory->getWidth() || BombY >= pInventory->getHeight())) {
        GCSkillFailed1 _GCSkillFailed1;
        _GCSkillFailed1.setSkillType(SKILL_THROW_BOMB);
        pPlayer->sendPacket(&_GCSkillFailed1);
        return;
    }

    ObjectID_t slayerObjectID = pCreature->getObjectID();

    bool bAttackSlayer = (pPacket->getAttackSlayerFlag() != 0) ? true : false;

    try {
        if (pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)) {
            g_Sniping.checkRevealRatio(pCreature, 20, 10);
        }

        Item* pItem = pInventory->getItem(BombX, BombY);

        
        if (pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_BOMB) {
            GCSkillFailed1 _GCSkillFailed1;
            //_GCSkillFailed1.setSkillType(SKILL_THROW_BOMB);
            pPlayer->sendPacket(&_GCSkillFailed1);
            return;
        }


        Bomb* pBomb = checkedCast<Bomb*>(pItem);
        ItemType_t BombType = pBomb->getItemType();
        Damage_t MinDamage = pBomb->getMinDamage();
        Damage_t MaxDamage = pBomb->getMaxDamage();
        SkillSlot* pSkillSlot = pSlayer->hasSkill(SKILL_THROW_BOMB);
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SKILL_THROW_BOMB);

        // hasSkill() returns NULL when the skill was never learned, and holding
        // a bomb does not imply holding the skill. A NULL dereference here is a
        // SIGSEGV, not a Throwable the catch below could absorb, so the process
        // would die rather than log.
        if (pSkillSlot == NULL || pSkillInfo == NULL) {
            GCSkillFailed1 _GCSkillFailed1;
            pPlayer->sendPacket(&_GCSkillFailed1);
            return;
        }

        Level_t SkillLevel = pSkillSlot->getExpLevel();
        Damage_t RealDamage = MinDamage + (max(0, ((int)MaxDamage * (int)SkillLevel / 100) - MinDamage));

        
        ZoneCoord_t myX = pSlayer->getX();
        ZoneCoord_t myY = pSlayer->getY();
        Dir_t dir = calcDirection(myX, myY, ZoneX, ZoneY);
        int count = 0;
        bool bHit = false;
        bool bRaceCheck = true;
        ;

        list<Creature*> cList;
        GCThrowBombOK1 _GCThrowBombOK1;
        GCThrowBombOK2 _GCThrowBombOK2;
        GCThrowBombOK3 _GCThrowBombOK3;

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = verifyDistance(pSlayer, ZoneX, ZoneY, 6);

        
        decreaseItemNum(pBomb, pInventory, pSlayer->getName(), STORAGE_INVENTORY, 0, BombX, BombY);

        if (bManaCheck && bTimeCheck && bRangeCheck) {
        } else {
            GCSkillFailed1 _GCSkillFailed1;
            //_GCSkillFailed1.setSkillType(SKILL_THROW_BOMB);
            pPlayer->sendPacket(&_GCSkillFailed1);
            return;
        }

        decreaseMana(pSlayer, RequiredMP, _GCThrowBombOK1);

        
        for (int tileY = ZoneY - 1; tileY <= ZoneY + 1; tileY++) {
            for (int tileX = ZoneX - 1; tileX <= ZoneX + 1; tileX++, count++) {
                
                if (!isValidZoneCoord(pZone, tileX, tileY))
                    continue;

                
                int Mask = BombMask[BombType][dir][count];
                if (Mask == 0)
                    continue;

                Tile& tile = pZone->getTile(tileX, tileY);

                checkMine(pZone, tileX, tileY);

                list<Creature*> targetList;
                if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) {
                    Creature* pCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

                    
                    if (pCreature != NULL && !pCreature->isFlag(Effect::EFFECT_CLASS_COMA))
                        targetList.push_back(pCreature);
                }
                if (tile.hasCreature(Creature::MOVE_MODE_BURROWING)) {
                    Creature* pCreature = tile.getCreature(Creature::MOVE_MODE_BURROWING);

                    
                    if (pCreature != NULL && !pCreature->isFlag(Effect::EFFECT_CLASS_COMA))
                        targetList.push_back(pCreature);
                }

                list<Creature*>::iterator itr = targetList.begin();
                for (; itr != targetList.end(); itr++) {
                    Creature* pTargetCreature = (*itr);
                    Assert(pTargetCreature != NULL);

                    bool bHitRoll = true;
                    if (pTargetCreature->isSlayer() && !bAttackSlayer)
                        bHitRoll = false;
                    else if (pTargetCreature->isNPC())
                        bHitRoll = false;

                    if (!canAttack(pSlayer, pTargetCreature))
                        continue;

                    if (!bHitRoll)
                        continue;
                    bool bPK = verifyPK(pSlayer, pTargetCreature);
                    bool bZoneLevelCheck = checkZoneLevelToHitTarget(pTargetCreature);

                    
                    
                    if (tileX == ZoneX && tileY == ZoneY) {
                        if (pTargetCreature->isSlayer()) {
                            bRaceCheck = false;
                        }
                    }

                    if (HitRoll::isSuccess(pSlayer, pTargetCreature, 50) && bPK && bZoneLevelCheck) {
                        Damage_t Damage = getPercentValue(RealDamage, Mask);
                        setDamage(pTargetCreature, Damage, pSlayer, 0, NULL, &_GCThrowBombOK1);
                        computeAlignmentChange(pTargetCreature, Damage, pSlayer, NULL, &_GCThrowBombOK1);
                        increaseAlignment(pSlayer, pTargetCreature, _GCThrowBombOK1);

                        bHit = true;

                        cList.push_back(pTargetCreature);
                        ObjectID_t targetObjectID = pTargetCreature->getObjectID();
                        _GCThrowBombOK1.addCListElement(targetObjectID);
                        _GCThrowBombOK2.addCListElement(targetObjectID);
                        _GCThrowBombOK3.addCListElement(targetObjectID);
                    }
                }
            }
        } // for (int tileY = ZoneY-1; tileY <= ZoneY+1; tileY++)

        
        if (bHit && bRaceCheck) {
            shareAttrExp(pSlayer, RealDamage, 1, 8, 1, _GCThrowBombOK1);
            increaseDomainExp(pSlayer, SKILL_DOMAIN_GUN, pSkillInfo->getPoint(), _GCThrowBombOK1);

            
            // 2003. 1. 12 by bezz
            if (pSkillSlot->canUse())
                increaseSkillExp(pSlayer, SKILL_DOMAIN_GUN, pSkillSlot, pSkillInfo, _GCThrowBombOK1);
            //			increaseAlignment(pSlayer, SKILL_DOMAIN_GUN, _GCThrowBombOK1);
            // shareAttrExp(pSlayer, RealDamage, 1, 8, 1, _GCSkillToTileOK1);
            
        }

        _GCThrowBombOK1.setXYDir(ZoneX, ZoneY, dir);
        _GCThrowBombOK1.setItemType(BombType);
        _GCThrowBombOK2.setXYDir(ZoneX, ZoneY, dir);
        _GCThrowBombOK2.setObjectID(slayerObjectID);
        _GCThrowBombOK2.setItemType(BombType);
        _GCThrowBombOK3.setXYDir(ZoneX, ZoneY, dir);
        _GCThrowBombOK3.setObjectID(slayerObjectID);
        _GCThrowBombOK3.setItemType(BombType);

        for (list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++) {
            Creature* pTargetCreature = *itr;
            _GCThrowBombOK2.clearList();

            HP_t targetHP = 0;
            if (pTargetCreature->isSlayer()) {
                targetHP = (dynamic_cast<Slayer*>(pTargetCreature))->getHP();
            } else if (pTargetCreature->isVampire()) {
                targetHP = (dynamic_cast<Vampire*>(pTargetCreature))->getHP();
            } else if (pTargetCreature->isOusters()) {
                targetHP = (dynamic_cast<Ousters*>(pTargetCreature))->getHP();
            }

            _GCThrowBombOK2.addShortData(MODIFY_CURRENT_HP, targetHP);

            
            decreaseDurability(NULL, pTargetCreature, pSkillInfo, NULL, &_GCThrowBombOK2);


            if (pTargetCreature->isPC()) {
                pTargetCreature->getPlayer()->sendPacket(&_GCThrowBombOK2);
            }
        }

        pPlayer->sendPacket(&_GCThrowBombOK1);

        cList.push_back(pSlayer);

        pZone->broadcastPacket(ZoneX, ZoneY, &_GCThrowBombOK3, cList);
    } catch (Throwable& t) {
        GCSkillFailed1 _GCSkillFailed1;
        GCSkillFailed2 failpkt2;

        failpkt2.setObjectID(slayerObjectID);
        failpkt2.setSkillType(SKILL_THROW_BOMB);
        pZone->broadcastPacket(ZoneX, ZoneY, &failpkt2, pSlayer);

        pPlayer->sendPacket(&_GCSkillFailed1);
    }


#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
