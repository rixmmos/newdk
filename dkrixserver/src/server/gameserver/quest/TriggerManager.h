//////////////////////////////////////////////////////////////////////////////
// Filename    : TriggerManager.h
// Written By  :
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __TRIGGER_MANAGER_H__
#define __TRIGGER_MANAGER_H__

#include "Exception.h"
#include "Trigger.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class TriggerManager;
//////////////////////////////////////////////////////////////////////////////

class TriggerManager {
public:
    TriggerManager();
    ~TriggerManager();

public:
    void load(const string& name);
    void load(ZoneID_t zoneid, int left, int top, int right, int bottom);

    void refresh();

    void addTrigger(Trigger* pTrigger);

    void deleteTrigger(TriggerID_t triggerID);

    Trigger* getTrigger(TriggerID_t triggerID);

    bool hasCondition(ConditionType_t conditionType) const {
        return m_ConditionSet.test(conditionType);
    }

    list<Trigger*>& getTriggers() {
        return m_Triggers;
    }
    const list<Trigger*>& getTriggers() const {
        return m_Triggers;
    }

    string toString() const;

private:
    // bitset of condition for fastest reference
    ConditionSet m_ConditionSet;

    // list of triggers
    
    list<Trigger*> m_Triggers;
};

#endif
