//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionSimpleQuestList.h
// Written By  : excel96
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_SIMPLE_QUEST_LIST_H__
#define __ACTION_SIMPLE_QUEST_LIST_H__

#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionSimpleQuestList
//////////////////////////////////////////////////////////////////////////////

class ActionSimpleQuestList : public Action {
public:
    virtual ActionType_t getActionType() const {
        return ACTION_SIMPLE_QUEST_LIST;
    }
    virtual void read(PropertyBuffer& propertyBuffer);
    virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
    virtual string toString() const;

public:
    ScriptID_t getScriptID() const {
        return m_ScriptID;
    }
    void setScriptID(ScriptID_t scriptID) {
        m_ScriptID = scriptID;
    }

private:
    ScriptID_t m_ScriptID; 
};


//////////////////////////////////////////////////////////////////////////////
// class ActionSimpleQuestListFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionSimpleQuestListFactory : public ActionFactory {
public:
    virtual ActionType_t getActionType() const {
        return Action::ACTION_SIMPLE_QUEST_LIST;
    }
    virtual string getActionName() const {
        return "SimpleQuestList";
    }
    virtual Action* createAction() const {
        return new ActionSimpleQuestList();
    }
};

#endif
