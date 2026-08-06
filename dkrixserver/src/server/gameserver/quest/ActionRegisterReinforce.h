//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionRegisterReinforce.h
// Written By  : excel96
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_REGISTER_REINFORCE_H__
#define __ACTION_REGISTER_REINFORCE_H__

#include "Action.h"
#include "ActionFactory.h"
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionRegisterReinforce
//////////////////////////////////////////////////////////////////////////////

class ActionRegisterReinforce : public Action {
public:
    virtual ActionType_t getActionType() const {
        return ACTION_REGISTER_REINFORCE;
    }
    virtual void read(PropertyBuffer& propertyBuffer);
    virtual void execute(Creature* pCreature1, Creature* pCreature2);
    virtual string toString() const;

public:
    ZoneID_t getZoneID() const {
        return m_ZoneID;
    }
    void setZoneID(ZoneID_t ZoneID) {
        m_ZoneID = ZoneID;
    }

private:
    ZoneID_t m_ZoneID; 
};


//////////////////////////////////////////////////////////////////////////////
// class ActionRegisterReinforceFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionRegisterReinforceFactory : public ActionFactory {
public:
    virtual ActionType_t getActionType() const {
        return Action::ACTION_REGISTER_REINFORCE;
    }
    virtual string getActionName() const {
        return "RegisterReinforce";
    }
    virtual Action* createAction() const {
        return new ActionRegisterReinforce();
    }
};
#endif
