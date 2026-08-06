#include "PetUtil.h"

#include <cstdio>

#include "GamePlayer.h"
#include "ModifyInfo.h"
#include "PetAttrInfo.h"
#include "PetExpInfo.h"
#include "PetInfo.h"
#include "PetTypeInfo.h"
#include "VariableManager.h"
#include "item/PetItem.h"

bool increasePetExp(PetInfo* pPetInfo, PetExp_t exp, ModifyInfo* pMI) {
    PetExp_t petExp = pPetInfo->getPetExp();
    petExp += exp;
    pPetInfo->setPetExp(petExp);

    PetLevel_t lastLevel, nextLevel;
    lastLevel = pPetInfo->getPetLevel();
    nextLevel = lastLevel + 1;

    

    if (pMI != NULL)
        pMI->addLongData(MODIFY_PET_EXP, petExp);

    if (PetExpInfoManager::Instance().canLevelUp(lastLevel, petExp)) {
        if (nextLevel > 10 && pPetInfo->getPetAttr() == 0xff) {
            petExp -= exp;
            pPetInfo->setPetExp(petExp);
            if (pMI != NULL)
                pMI->clearList();
            return false;
        }

        pPetInfo->setPetLevel(nextLevel);
        pPetInfo->setPetCreatureType(PetTypeInfoManager::getInstance()
                                         ->getPetTypeInfo(pPetInfo->getPetType())
                                         ->getPetCreatureType(pPetInfo->getPetLevel()));

        
        //(int)pPetInfo->getPetCreatureType() << endl;

        if (pPetInfo->getPetAttr() != 0xff) {
            PetAttrInfo* pPetAttrInfo = PetAttrInfoManager::Instance().getPetAttrInfo(pPetInfo->getPetAttr());
            if (pPetAttrInfo != NULL) {
                pPetInfo->setPetAttrLevel(pPetAttrInfo->getPetAttrLevel(pPetInfo->getPetLevel()));
            }
        }

        PetItem* pPetItem = pPetInfo->getPetItem();

        if (nextLevel > 10 && !pPetInfo->canCutHead())
            pPetInfo->setGamble(1);
        else if (nextLevel >= 30 && !pPetInfo->canAttack())
            pPetInfo->setGamble(1);
        else
            pPetInfo->setGamble(0);

        pPetItem->savePetInfo();

        return true;
    }

    return false;
}

PetExp_t computePetExp(Level_t myLevel, Level_t monsterLevel, PetInfo* pPetInfo, GamePlayer* pGamePlayer) {
    if (myLevel > 120)
        myLevel = 120;

    int x = monsterLevel - myLevel;
    PetExp_t min = 7, range = 7;
    if (pPetInfo->getPetType() > 2) {
        min = 5;
        range = 6;
    }

    PetExp_t ret = min + (rand() % range);

    if (pGamePlayer == NULL || !pGamePlayer->isPayPlaying()) {
        ret /= 2;
    }

    if (pPetInfo == NULL || pPetInfo->getPetType() > 2) {
        ret /= 2;
    }

    if (x > 20)
        ret = ret * 10 / 7;
    else if (x < -20)
        ret = max(1, (int)ret / 10);

    ret = getPercentValue(ret, g_pVariableManager->getPetExpRatio());

    return ret;
}

void refreshHP(PetInfo* pPetInfo, uint Ratio) {
    
    VSDateTime current = VSDateTime::currentDateTime();
    int lastSec = pPetInfo->getLastFeedTime().secsTo(current);
    
    if (lastSec <= 0)
        return;

    int lastMin = (lastSec / 60) + 1;
    
    int spendFood = getPercentValue(lastMin, Ratio);

    //	if ( pPetInfo->getPetLevel() == 50 ) spendFood = max(1, spendFood/10);
    if (pPetInfo->getPetHP() < spendFood)
        spendFood = pPetInfo->getPetHP();
    if (spendFood == 0) {
        
        return;
    }

    

    pPetInfo->setPetHP(pPetInfo->getPetHP() - spendFood);

    if (pPetInfo->getPetHP() == 0) {
        spendFood /= (Ratio / 100.0);
        
        pPetInfo->setFeedTime(pPetInfo->getLastFeedTime().addDays(spendFood / 1440).addSecs((spendFood % 1440) * 60));
    } else
        pPetInfo->setFeedTime(current);

    pPetInfo->getPetItem()->savePetInfo();
}
