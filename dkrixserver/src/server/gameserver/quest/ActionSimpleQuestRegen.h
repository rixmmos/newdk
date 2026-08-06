//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionSimpleQuestRegen.h
// Written By  :
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_SIMPLE_QUEST_REGEN_H__
#define __ACTION_SIMPLE_QUEST_REGEN_H__

#include "Action.h"
#include "ActionFactory.h"
#include "Exception.h"
#include "Timeval.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionSimpleQuestRegen
//////////////////////////////////////////////////////////////////////////////

class ActionSimpleQuestRegen : public Action {
public:
    ActionSimpleQuestRegen();
    virtual ~ActionSimpleQuestRegen();

public:
    virtual ActionType_t getActionType() const {
        return ACTION_SIMPLE_QUEST_REGEN;
    }
    virtual void read(PropertyBuffer& propertyBuffer);
    virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
    virtual string toString() const;

private:
    Timeval m_Period;    
    Timeval m_NextRegen; 
};


////////////////////////////////////////////////////////////////////////////////
// class ActionSimpleQuestRegenFactory;
////////////////////////////////////////////////////////////////////////////////

class ActionSimpleQuestRegenFactory : public ActionFactory {
public:
    virtual ActionType_t getActionType() const {
        return Action::ACTION_SIMPLE_QUEST_REGEN;
    }
    virtual string getActionName() const {
        return "SimpleQuestRegen";
    }
    virtual Action* createAction() const {
        return new ActionSimpleQuestRegen();
    }
};
#endif
