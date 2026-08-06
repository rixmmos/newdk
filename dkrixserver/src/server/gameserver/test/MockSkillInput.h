#ifndef __MOCK_SKILL_INPUT_H__
#define __MOCK_SKILL_INPUT_H__

#include "Item.h"
#include "skill/SkillHandler.h"

class MockSkillInput : public SkillInput {
public:
    MockSkillInput() {
        TargetType = TARGET_MAX;
        Range = 0;
        PartySize = 1;

        
        /*
        SkillLevel

        STR
        DEX
        INTE
        IClass
        DomainLevel
        */
    }

    void setSkillLevel(int skillLevel) {
        SkillLevel = skillLevel;
    }

    void setATTR(int str, int dex, int inte) {
        STR = str;
        DEX = dex;
        INTE = inte;
    }

    void setWeaponClass(Item::ItemClass iClass) {
        IClass = iClass;
    }

    void setDomainLevel(int domainLevel) {
        DomainLevel = domainLevel;
    }
};

#endif
