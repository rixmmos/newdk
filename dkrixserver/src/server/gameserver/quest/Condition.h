//////////////////////////////////////////////////////////////////////////////
// Filename    : Condition.h
// Written By  :
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <bitset>

#include "Exception.h"
#include "PropertyBuffer.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

typedef struct {
    ScriptID_t ScriptID;
    AnswerID_t AnswerID;
} COND_ANSWERED_BY;


////////////////////////////////////////////////////////////////////////////////
//
// class Condition;
//
////////////////////////////////////////////////////////////////////////////////
//
//* isActive() | isPassive() | isNeutral()
//





//


//

//(ex: PC_HAS_SKILL, PC_HAS_ITEM ...)
//
////////////////////////////////////////////////////////////////////////////////

class Creature;

class Condition {
public:
    enum ConditionTypes {
        // active conditions
        CONDITION_AT_FIRST,
        CONDITION_AT_TIME,
        CONDITION_FROM_TIME_TO_TIME,
        CONDITION_IDLE,
        CONDITION_EVERY_TIME,

        // passive conditions
        CONDITION_TALKED_BY,
        CONDITION_ANSWERED_BY, 
        CONDITION_BLOOD_DRAINED,
        CONDITION_FLAG_ON,
        CONDITION_FLAG_OFF,
        CONDITION_ATTR_COMP,
        CONDITION_RACE_COMP,
        CONDITION_SAME_CLAN,

        CONDITION_ATTACKED_BY,
        CONDITION_DIED_BY,
        CONDITION_RESCUED_BY,
        CONDITION_PC_ATTRIBUTE_COMP,
        CONDITION_PC_HAS_SKILL,
        CONDITION_PC_SKILL_COMP,
        CONDITION_PC_HAS_ITEM,
        CONDITION_PC_DONE_QUEST,
        CONDITION_PC_UNDER_QUEST,
        CONDITION_QUEST_COMPLETED,
        CONDITION_QUEST_STATE,

        // 2002.6.3 by sigi
        CONDITION_PAY_PLAY,

        // 2002.9.2 by sigi
        CONDITION_ENTER_MASTER_LAIR, 

        // 2003.1.20 by bezz, Sequoia
        CONDITION_ENTER_CASTLE,                   
        CONDITION_HAS_ENOUGH_CASTLE_ENTRANCE_FEE, 

        // 2003.2.18 by sigi
        CONDITION_ENTER_HOLY_LAND,      
        CONDITION_ENTER_CASTLE_DUNGEON, 

        CONDITION_HAS_QUEST,
        CONDITION_HAS_INVEN_SPACE,

        CONDITION_CAN_ENTER_PAY_ZONE, 

        CONDITION_CAN_ENTER_BEGINNER_ZONE,  
        CONDITION_CAN_WARP_GATE,            
        CONDITION_CAN_ENTER_LEVEL_WAR_ZONE, 

        CONDITION_CAN_PET_QUEST,
        CONDITION_CAN_ENTER_EVENT_ZONE,

        CONDITION_EFFECT_FLAG,        
        CONDITION_CAN_ENTER_GDR_LAIR, 

        CONDITION_EXIST_REINFORCE, 

        CONDITION_SIEGE_DEFENDER_SIDE, 
        CONDITION_SIEGE_ATTACKER_SIDE, 

        CONDITION_NOT_GUILD_MEMBER, 
        CONDITION_IS_GUILD_MEMBER,  

        CONDITION_CAN_ENTER_QUEST_ZONE, 

        CONDITION_MAX
    };

public:
    virtual ~Condition() {}
    virtual ConditionType_t getConditionType() const = 0;

    virtual bool isActive() const {
        return false;
    }
    virtual bool isPassive() const {
        return false;
    }
    virtual bool isNeutral() const {
        return false;
    }

    virtual bool isSatisfied(Creature* pCreature1, Creature* pCreature2 = NULL, void* pParam = NULL) const = 0;

    virtual void read(PropertyBuffer& propertyBuffer) = 0;

    virtual string toString() const = 0;
};

// Condition Set
#define ConditionSet bitset<Condition::CONDITION_MAX>

#endif
