//////////////////////////////////////////////////////////////////////////////
// Filename    : CreateHolyPotion.cpp
// Written by  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CreateHolyPotion.h"

#include "GCSkillToInventoryOK1.h"
#include "GCSkillToInventoryOK2.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "item/Potion.h"


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CreateHolyPotion::execute(Slayer* pSlayer, ObjectID_t InvenObjectID, CoordInven_t X, CoordInven_t Y,
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

        
        
        Item* pPotion = pInventory->getItem(X, Y);
        if (pPotion == NULL || pPotion->getItemClass() != Item::ITEM_CLASS_WATER ||
            pPotion->getObjectID() != InvenObjectID) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" << endl;
            return;
        }

        bool bSamePosition = false;
        if (X == TargetX && Y == TargetY)
            bSamePosition = true;

        
        
        
        
        
        if (bSamePosition && pPotion->getNum() != 1) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" << endl;
            return;
        }

        GCSkillToInventoryOK1 _GCSkillToInventoryOK1;
        GCSkillToInventoryOK2 _GCSkillToInventoryOK2;

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();
        SkillDomainType_t DomainType = pSkillInfo->getDomainType();
        SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));

        
        ItemType_t waterType = pPotion->getItemType() + 11;

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
        bool bCanMake = canMake(pPotion->getItemType(), pSlayer->getSkillDomainLevel(DomainType), SkillLevel);

        if (bManaCheck && bTimeCheck && bRangeCheck && bCanMake) {
            decreaseMana(pSlayer, RequiredMP, _GCSkillToInventoryOK1);

            SkillInput input(pSlayer, pSkillSlot);
            SkillOutput output;
            computeOutput(input, output);

            
            
            
            list<OptionType_t> optionNULL;
            Item* pHolyPotion = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_POTION, waterType, optionNULL);

            
            
            
            decreaseItemNum(pPotion, pInventory, pSlayer->getName(), STORAGE_INVENTORY, 0, X, Y);
            Item* pPrevHolyPotion = pInventory->getItem(TargetX, TargetY);

            
            if (pPrevHolyPotion != NULL) {
                if (canStack(pPrevHolyPotion, pHolyPotion) == false) {
                    
                    SAFE_DELETE(pHolyPotion);

                    executeSkillFailException(pSlayer, getSkillType());

                    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" <<
                    // endl;
                    return;
                }

                
                pPrevHolyPotion->setNum(pPrevHolyPotion->getNum() + pHolyPotion->getNum());
                pPrevHolyPotion->save(pSlayer->getName(), STORAGE_INVENTORY, 0, TargetX, TargetY);

                
                
                pInventory->increaseNum(pHolyPotion->getNum());

                
                SAFE_DELETE(pHolyPotion);

                _GCSkillToInventoryOK1.setObjectID(pPrevHolyPotion->getObjectID());
            }
            
            else {
                ObjectRegistry& OR = pZone->getObjectRegistry();
                OR.registerObject(pHolyPotion);

                
                pInventory->addItem(TargetX, TargetY, pHolyPotion);
                pHolyPotion->create(pSlayer->getName(), STORAGE_INVENTORY, 0, TargetX, TargetY);

                _GCSkillToInventoryOK1.setObjectID(pHolyPotion->getObjectID());
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
            
            //			increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToInventoryOK1);
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

bool CreateHolyPotion::canMake(ItemType_t PotionType, int DomainLevel, int SkillLevel) throw() {
    __BEGIN_TRY

    bool rvalue = false;
    int ratio = 60 + SkillLevel;

    
    
    
    if (PotionType == 6 && DomainLevel >= 101) {
        rvalue = true;
    } else if (PotionType == 5 && DomainLevel >= 81) {
        rvalue = true;
    } else if (PotionType == 4 && DomainLevel >= 61) {
        
        rvalue = true;
    } else if (PotionType == 3) {
        rvalue = true;
    }

    
    if (rvalue) {
        if ((rand() % 100) < ratio)
            return true;
    }

    return false;

    __END_CATCH
}

CreateHolyPotion g_CreateHolyPotion;
