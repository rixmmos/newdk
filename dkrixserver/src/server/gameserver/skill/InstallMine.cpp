//////////////////////////////////////////////////////////////////////////////
// Filename    : InstallMine.cpp
// Written by  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "InstallMine.h"

#include "Assert.h"
#include "GCSkillToInventoryOK1.h"
#include "ItemUtil.h"
#include "SkillInfo.h"
#include "item/Mine.h"
// #include "GCSkillToTileOK1.h"
// #include "GCSkillToTileOK5.h"
#include "GCDeleteEffectFromTile.h"
#include "GCDeleteObject.h"
#include "GCSkillFailed1.h"
#include "GCSkillFailed2.h"
#include "ItemInfoManager.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void InstallMine::execute(Slayer* pSlayer, ObjectID_t, CoordInven_t X, CoordInven_t Y, CoordInven_t TargetX,
                          CoordInven_t TargetY, SkillSlot* pSkillSlot)

{
    __BEGIN_TRY

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();

        Assert(pPlayer != NULL);
        Assert(pZone != NULL);

        GCSkillToInventoryOK1 _GCSkillToInventoryOK1;
        //		GCSkillToTileOK1 _GCSkillToTileOK1;
        //		GCSkillToTileOK5 _GCSkillToTileOK5;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

        
        // ToHit_t ToHit = pSlayer->getToHit();

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
        ZoneCoord_t slayerX = pSlayer->getX(), slayerY = pSlayer->getY();
        bool bInstallAction = false;

        Mine* pMine = NULL;

        Inventory* pInventory = pSlayer->getInventory();
        Assert(pInventory != NULL);

        if (bManaCheck && bTimeCheck && bRangeCheck) {
            
            Item* pItem = pInventory->getItem(X, Y);
            if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_MINE) {
                bInstallAction = true;
                pMine = dynamic_cast<Mine*>(pItem);
            }
        }


        
        if (bInstallAction) {
            // Range_t Range = 1;

            GCSkillToInventoryOK1 _GCSkillToInventoryOK1;
            //			GCSkillToInventoryOK5 _GCSkillToInventoryOK5;

            ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_MINE, pMine->getItemType());

            Damage_t MinDamage = pItemInfo->getMinDamage();
            Damage_t MaxDamage = pItemInfo->getMaxDamage();

            Damage_t RealDamage = MinDamage + (max(0, ((int)MaxDamage * (int)SkillLevel / 100) - MinDamage));

            Mine* pInstallMine = new Mine();
            ObjectRegistry& OR = pZone->getObjectRegistry();
            OR.registerObject(pInstallMine);

            Assert(pInstallMine != NULL);
            pInstallMine->setItemType(pMine->getItemType());
            pInstallMine->setDir(TargetX);
            pInstallMine->setDamage(RealDamage);
            pInstallMine->setInstallerName(pSlayer->getName());
            pInstallMine->setInstallerPartyID(pSlayer->getPartyID());
            pInstallMine->setFlag(Effect::EFFECT_CLASS_INSTALL);

            
            
            TPOINT pt = pZone->addItem(pInstallMine, slayerX, slayerY, true, 6000);

            // EXP up
            Exp_t Point = pSkillInfo->getPoint();

            shareAttrExp(pSlayer, 100, 1, 8, 1, _GCSkillToInventoryOK1);
            increaseDomainExp(pSlayer, SKILL_DOMAIN_GUN, Point, _GCSkillToInventoryOK1);
            increaseSkillExp(pSlayer, SKILL_DOMAIN_GUN, pSkillSlot, pSkillInfo, _GCSkillToInventoryOK1);

            decreaseMana(pSlayer, RequiredMP, _GCSkillToInventoryOK1);
            decreaseItemNum(pMine, pInventory, pSlayer->getName(), STORAGE_INVENTORY, 0, X, Y);


            _GCSkillToInventoryOK1.setObjectID(pInstallMine->getObjectID());
            _GCSkillToInventoryOK1.setSkillType(SkillType);
            _GCSkillToInventoryOK1.setCEffectID(0);
            _GCSkillToInventoryOK1.setX(X);
            _GCSkillToInventoryOK1.setY(Y);
            _GCSkillToInventoryOK1.setDuration(0);

            /*
            _GCSkillToInventoryOK5.setObjectID( pSlayer->getObjectID());
            _GCSkillToInventoryOK5.setSkillType( SkillType);
            _GCSkillToInventoryOK5.setX( X);
            _GCSkillToInventoryOK5.setY( Y);
            _GCSkillToInventoryOK5.setRange( Range);
            _GCSkillToInventoryOK5.setDuration( 0);
            */

            pPlayer->sendPacket(&_GCSkillToInventoryOK1);

            
            addInstalledMine(pZone, pInstallMine, pt.x, pt.y);

            //			pZone->broadcastPacket( slayerX, slayerY, &_GCSkillToInventoryOK5, pSlayer);

            //        cout << "Run Skill : " << (int)SkillType << endl;
            // Set NextTime
            pSkillSlot->setRunTime();

        } else {
            GCSkillFailed1 _GCSkillFailed1;
            GCSkillFailed2 _GCSkillFailed2;

            executeSkillFailException(pSlayer, getSkillType());
        }

    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, getSkillType());
    }

    __END_CATCH
}

 

InstallMine g_InstallMine;
