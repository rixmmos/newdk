//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyMarker.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "BloodyMarker.h"

#include "GCDeleteObject.h"
#include "GCSkillToInventoryOK1.h"
#include "GCSkillToSelfOK1.h"
#include "GCSkillToSelfOK3.h"
#include "ZoneUtil.h"
#include "item/VampirePortalItem.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void BloodyMarker::execute(Vampire* pVampire, ObjectID_t InvenObjectID, CoordInven_t X, CoordInven_t Y,
                           CoordInven_t TargetX, CoordInven_t TargetY, VampireSkillSlot* pSkillSlot)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    Assert(pVampire != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pVampire->getPlayer();
        Zone* pZone = pVampire->getZone();
        Inventory* pInventory = pVampire->getInventory();

        Assert(pPlayer != NULL);
        Assert(pInventory != NULL);
        Assert(pZone != NULL);

        
        
        ///*
        // int zoneID = pZone->getZoneID();
        // if (pZone->getZoneID()==1122 || pZone->getZoneID()==1123)

        
        // if (zoneID==1005 || zoneID==1006)
        if (pZone->isNoPortalZone() ||
            pZone->isMasterLair()
            
            || pZone->isCastle() || pZone->isHolyLand()) {
            executeSkillFailException(pVampire, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }
        //*/

        Item* pItem = pInventory->getItem(X, Y);
        
        if (pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM ||
            pItem->getObjectID() != InvenObjectID) {
            executeSkillFailException(pVampire, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }

        VampirePortalItem* pVampirePortalItem = dynamic_cast<VampirePortalItem*>(pItem);
        Assert(pVampirePortalItem != NULL);

        
        if (pVampirePortalItem->getZoneID() != 0 || pVampirePortalItem->getX() != 0 ||
            pVampirePortalItem->getY() != 0) {
            executeSkillFailException(pVampire, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }

        GCSkillToInventoryOK1 _GCSkillToInventoryOK1;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

        int RequiredMP = decreaseConsumeMP(pVampire, pSkillInfo);
        bool bManaCheck = hasEnoughMana(pVampire, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);

        if (bManaCheck && bTimeCheck && bRangeCheck) {
            
            decreaseMana(pVampire, RequiredMP, _GCSkillToInventoryOK1);

            SkillInput input(pVampire);
            SkillOutput output;
            computeOutput(input, output);

            
            pVampirePortalItem->setZoneID(pZone->getZoneID());
            pVampirePortalItem->setX(pVampire->getX());
            pVampirePortalItem->setY(pVampire->getY());
            pVampirePortalItem->save(pVampire->getName(), STORAGE_INVENTORY, 0, X, Y);

            _GCSkillToInventoryOK1.setSkillType(SkillType);
            _GCSkillToInventoryOK1.setObjectID(InvenObjectID);
            _GCSkillToInventoryOK1.setCEffectID(0);
            _GCSkillToInventoryOK1.setDuration(0);

            pPlayer->sendPacket(&_GCSkillToInventoryOK1);

            pSkillSlot->setRunTime(output.Delay);
        } else {
            executeSkillFailNormal(pVampire, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pVampire, getSkillType());
        // cout << t.toString() << endl;
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

BloodyMarker g_BloodyMarker;
