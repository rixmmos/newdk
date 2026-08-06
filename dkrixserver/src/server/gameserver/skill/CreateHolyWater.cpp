//////////////////////////////////////////////////////////////////////////////
// Filename    : CreateHolyWater.cpp
// Written by  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CreateHolyWater.h"

#include "GCSkillToInventoryOK1.h"
#include "GCSkillToInventoryOK2.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "item/HolyWater.h"


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CreateHolyWater::execute(Slayer* pSlayer, ObjectID_t InvenObjectID, CoordInven_t X, CoordInven_t Y,
                              CoordInven_t TargetX, CoordInven_t TargetY, SkillSlot* pSkillSlot)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayerinventory)" << endl;

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();
        Inventory* pInventory = pSlayer->getInventory();

        Assert(pPlayer != NULL);
        Assert(pZone != NULL);
        Assert(pInventory != NULL);

        
        
        Item* pWater = pInventory->getItem(X, Y);
        if (pWater == NULL || pWater->getItemClass() != Item::ITEM_CLASS_WATER ||
            pWater->getObjectID() != InvenObjectID) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" << endl;
            return;
        }

        bool bSamePosition = false;
        if (X == TargetX && Y == TargetY)
            bSamePosition = true;

        
        
        
        
        
        /*		if (bSamePosition && pWater->getNum() != 1)
                {
                    executeSkillFailException(pSlayer, getSkillType());
                    //cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" <<
           endl; return;
                }*/

        GCSkillToInventoryOK1 _GCSkillToInventoryOK1;
        GCSkillToInventoryOK2 _GCSkillToInventoryOK2;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();
        SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));

        ItemType_t waterType = pWater->getItemType();

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
        bool bCanMake = canMake(waterType, pSlayer->getSkillDomainLevel(DomainType), SkillLevel);

        if (bManaCheck && bTimeCheck && bRangeCheck && bCanMake) {
            decreaseMana(pSlayer, RequiredMP, _GCSkillToInventoryOK1);

            SkillInput input(pSlayer, pSkillSlot);
            SkillOutput output;
            computeOutput(input, output);

            
            
            
            list<OptionType_t> optionNULL;
            Item* pHolyWater = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_HOLYWATER, waterType, optionNULL);
            
            pHolyWater->setNum(pWater->getNum());

            
            
            
            //			decreaseItemNum(pWater, pInventory, pSlayer->getName(), STORAGE_INVENTORY, 0, X, Y);
            
            pInventory->deleteItem(X, Y);
            pWater->destroy();
            SAFE_DELETE(pWater);

            Item* pPrevHolyWater = pInventory->getItem(TargetX, TargetY);

            
            if (pPrevHolyWater != NULL) {
                if (canStack(pPrevHolyWater, pHolyWater) == false) {
                    
                    SAFE_DELETE(pHolyWater);

                    executeSkillFailException(pSlayer, getSkillType());

                    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" <<
                    // endl;
                    return;
                }

                
                pPrevHolyWater->setNum(pPrevHolyWater->getNum() + pHolyWater->getNum());
                pPrevHolyWater->save(pSlayer->getName(), STORAGE_INVENTORY, 0, TargetX, TargetY);

                
                
                pInventory->increaseNum(pHolyWater->getNum());

                
                SAFE_DELETE(pHolyWater);

                _GCSkillToInventoryOK1.setObjectID(pPrevHolyWater->getObjectID());
            }
            
            else {
                ObjectRegistry& OR = pZone->getObjectRegistry();
                OR.registerObject(pHolyWater);

                
                pInventory->addItem(TargetX, TargetY, pHolyWater);
                pHolyWater->create(pSlayer->getName(), STORAGE_INVENTORY, 0, TargetX, TargetY);

                _GCSkillToInventoryOK1.setObjectID(pHolyWater->getObjectID());
            }

            
            _GCSkillToInventoryOK1.setSkillType(SkillType);
            _GCSkillToInventoryOK1.setItemType(waterType);
            _GCSkillToInventoryOK1.setCEffectID(0);
            _GCSkillToInventoryOK1.setX(TargetX);
            _GCSkillToInventoryOK1.setY(TargetY);

            _GCSkillToInventoryOK2.setObjectID(pSlayer->getObjectID());
            _GCSkillToInventoryOK2.setSkillType(SkillType);

            // EXP UP!
            Exp_t ExpUp = 10 * (Grade + 1);
            shareAttrExp(pSlayer, ExpUp, 1, 1, 8, _GCSkillToInventoryOK1);
            increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToInventoryOK1);
            increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToInventoryOK1);

            pPlayer->sendPacket(&_GCSkillToInventoryOK1);

            pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCSkillToInventoryOK2, pSlayer);

            pSkillSlot->setRunTime(output.Delay);
        } else {
            // executeSkillFailNormal(pSlayer, getSkillType(), NULL);
            
            
            
            
            
            
            executeSkillFailException(pSlayer, getSkillType());
        }
    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" << endl;

    __END_CATCH
}

bool CreateHolyWater::canMake(ItemType_t WaterType, int DomainLevel, int SkillLevel) throw() {
    __BEGIN_TRY

    bool rvalue = false;
    SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(DomainLevel);
    int ratio = 0;

    
    
    
    if (WaterType == 2 && Grade >= SKILL_GRADE_EXPERT) {
        
        rvalue = true;
        ratio = 50 + SkillLevel - 50;
    } else if (WaterType == 1 && Grade >= SKILL_GRADE_ADEPT) {
        
        rvalue = true;
        ratio = 50 + SkillLevel - 30;
    } else if (WaterType == 0 && Grade >= SKILL_GRADE_APPRENTICE) {
        
        rvalue = true;
        ratio = 50 + SkillLevel - 10;
    }

    
    if (rvalue) {
        if ((rand() % 100) < ratio)
            return true;
    }

    return false;

    __END_CATCH
}

CreateHolyWater g_CreateHolyWater;
