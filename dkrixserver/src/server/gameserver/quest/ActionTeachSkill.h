//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTeachSkill.h
// Written By  :
// Description :



//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_TEACH_SKILL_H__
#define __ACTION_TEACH_SKILL_H__

#include "Action.h"
#include "ActionFactory.h"
#include "Exception.h"
#include "Types.h"

#define TEACH_SKILL_SLAYER 0
#define TEACH_SKILL_VAMPIRE 1

//////////////////////////////////////////////////////////////////////////////
// class ActionTeachSkill;
//////////////////////////////////////////////////////////////////////////////

class ActionTeachSkill : public Action {
public:
    virtual ActionType_t getActionType() const {
        return ACTION_TEACH_SKILL;
    }
    virtual void read(PropertyBuffer& propertyBuffer);
    virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
    virtual string toString() const;

public:
    virtual void executeSlayer(Creature* pCreature1, Creature* pCreature2 = NULL);
    virtual void executeVampire(Creature* pCreature1, Creature* pCreature2 = NULL);

    SkillDomainType_t getDomainType(void) const {
        return m_DomainType;
    }
    void setDomainType(SkillDomainType_t domain) {
        m_DomainType = domain;
    }

private:
    SkillDomainType_t m_DomainType; 
};


////////////////////////////////////////////////////////////////////////////////
// class ActionTeachSkillFactory;
////////////////////////////////////////////////////////////////////////////////

class ActionTeachSkillFactory : public ActionFactory {
public:
    virtual ActionType_t getActionType() const {
        return Action::ACTION_TEACH_SKILL;
    }
    virtual string getActionName() const {
        return "TeachSkill";
    }
    virtual Action* createAction() const {
        return new ActionTeachSkill();
    }
};

#endif
