//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionAskDynamic.h
// Written By  : excel96
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_ASK_DYNAMIC_H__
#define __ACTION_ASK_DYNAMIC_H__

#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionAskDynamic
//////////////////////////////////////////////////////////////////////////////

class ActionAskDynamic : public Action {
public:
    virtual ActionType_t getActionType() const {
        return ACTION_ASK_DYNAMIC;
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
// class ActionAskDynamicFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionAskDynamicFactory : public ActionFactory {
public:
    virtual ActionType_t getActionType() const {
        return Action::ACTION_ASK_DYNAMIC;
    }
    virtual string getActionName() const {
        return "AskDynamic";
    }
    virtual Action* createAction() const {
        return new ActionAskDynamic();
    }
};

#endif
