//////////////////////////////////////////////////////////////////////////////
// Filename    : CreateMine.cpp
// Written by  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CreateMine.h"

#include "GCSkillToInventoryOK1.h"
#include "GCSkillToInventoryOK2.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "item/Mine.h"

int MaterialType2MineTypeMap[] = {
    -1, // 0
    -1, // 1
    -1, // 2
    -1, // 3
    -1, // 4
    0,  // 5
    1,  // 6
    2,  // 7
    3,  // 8
    4   // 9
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CreateMine::execute(Slayer* pSlayer, ObjectID_t InvenObjectID, CoordInven_t X, CoordInven_t Y,
                         CoordInven_t TargetX, CoordInven_t TargetY, SkillSlot* pSkillSlot)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(Create Mine)" << endl;

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();
        Inventory* pInventory = pSlayer->getInventory();

        Assert(pPlayer != NULL);
        Assert(pZone != NULL);
        Assert(pInventory != NULL);

        
        
        Item* pBombMaterial = pInventory->getItem(X, Y);
        if (pBombMaterial == NULL || pBombMaterial->getItemClass() != Item::ITEM_CLASS_BOMB_MATERIAL ||
            pBombMaterial->getObjectID() != InvenObjectID) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" << endl;
            return;
        }

        bool bSamePosition = false;
        if (X == TargetX && Y == TargetY)
            bSamePosition = true;

        
        
        
        
        if (bSamePosition && pBombMaterial->getNum() != 1) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" << endl;
            return;
        }

        
        ItemType_t MaterialType = pBombMaterial->getItemType();
        int MineType = MaterialType2MineTypeMap[MaterialType];
        if (MineType == -1) {
            
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

        int RequiredMP = (int)pSkillInfo->getConsumeMP();
        bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
        bool bTimeCheck = verifyRunTime(pSkillSlot);
        bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
        bool bCanMake = canMake(MineType, pSlayer->getSkillDomainLevel(DomainType), SkillLevel);

        if (bManaCheck && bTimeCheck && bRangeCheck && bCanMake) {
            decreaseMana(pSlayer, RequiredMP, _GCSkillToInventoryOK1);

            SkillInput input(pSlayer, pSkillSlot);
            SkillOutput output;
            computeOutput(input, output);

            list<OptionType_t> optionNULL;
            Item* pMine = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MINE, MineType, optionNULL);

            
            
            
            decreaseItemNum(pBombMaterial, pInventory, pSlayer->getName(), STORAGE_INVENTORY, 0, X, Y);

            Item* pPrevMine = pInventory->getItem(TargetX, TargetY);

            
            if (pPrevMine != NULL) {
                if (canStack(pPrevMine, pMine) == false) {
                    
                    SAFE_DELETE(pMine);

                    executeSkillFailException(pSlayer, getSkillType());

                    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerinventory)" <<
                    // endl;
                    return;
                }

                
                pPrevMine->setNum(pPrevMine->getNum() + 1);
                pPrevMine->save(pSlayer->getName(), STORAGE_INVENTORY, 0, TargetX, TargetY);

                
                
                pInventory->increaseNum();

                
                SAFE_DELETE(pMine);

                _GCSkillToInventoryOK1.setObjectID(pPrevMine->getObjectID());
            }
            
            else {
                ObjectRegistry& OR = pZone->getObjectRegistry();
                OR.registerObject(pMine);

                
                pInventory->addItem(TargetX, TargetY, pMine);
                pMine->setNum(1);
                pMine->create(pSlayer->getName(), STORAGE_INVENTORY, 0, TargetX, TargetY);

                _GCSkillToInventoryOK1.setObjectID(pMine->getObjectID());
            }

            
            _GCSkillToInventoryOK1.setSkillType(SkillType);
            _GCSkillToInventoryOK1.setItemType(MineType);
            _GCSkillToInventoryOK1.setCEffectID(0);
            _GCSkillToInventoryOK1.setX(TargetX);
            _GCSkillToInventoryOK1.setY(TargetY);

            _GCSkillToInventoryOK2.setObjectID(pSlayer->getObjectID());
            _GCSkillToInventoryOK2.setSkillType(SkillType);

            // EXP UP!
            Exp_t ExpUp = 10 * (Grade + 1);
            shareAttrExp(pSlayer, ExpUp, 1, 8, 1, _GCSkillToInventoryOK1);
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

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(Create Mine)" << endl;

    __END_CATCH
}

bool CreateMine::canMake(ItemType_t BombType, int DomainLevel, int SkillLevel) throw() {
    __BEGIN_TRY

    int ratio = 10 * ((SkillLevel / 10) - BombType) + 40;
    if ((rand() % 100) < ratio)
        return true;
    return false;

    __END_CATCH
}

CreateMine g_CreateMine;
