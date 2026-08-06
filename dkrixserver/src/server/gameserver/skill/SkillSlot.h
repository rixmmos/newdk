//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillSlot.h
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SLOT_H__
#define __SKILL_SLOT_H__

#include "Exception.h"
#include "Skill.h"
#include "Timeval.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// Class SkillSlot;
//////////////////////////////////////////////////////////////////////////////

class SkillSlot {
public:
    SkillSlot() throw();
    SkillSlot(SkillType_t SkillType, DWORD Exp, ulong Interval) throw();
    virtual ~SkillSlot() throw();

public:
    virtual void save(const string& ownerID);
    virtual void save();

    virtual void create(const string& ownerID);

    SkillType_t getSkillType() const throw() {
        return m_SkillType;
    }
    void setSkillType(SkillType_t Type) throw() {
        m_SkillType = Type;
    }

    void setExp(Exp_t Exp) throw() {
        m_Exp = Exp;
    }
    Exp_t getExp() throw() {
        return m_Exp;
    }

    void setExpLevel(ExpLevel_t ExpLevel) throw() {
        m_ExpLevel = ExpLevel;
    }
    ExpLevel_t getExpLevel() throw() {
        return m_ExpLevel;
    }

    Turn_t getInterval() throw() {
        return m_Interval;
    }
    void setInterval(Turn_t Interval) throw() {
        m_Interval = Interval;
    }

    Turn_t getCastingTime() throw() {
        return m_CastingTime;
    }
    void setCastingTime(Turn_t CastingTime) throw() {
        m_CastingTime = CastingTime;
    }

    Timeval getRunTime() throw() {
        return m_runTime;
    }
    void setRunTime() throw();
    void setRunTime(Turn_t delay, bool bSave = true) throw();

    string getName() const throw() {
        return m_Name;
    }
    void setName(const string& Name) {
        m_Name = Name;
    }

    
    
    void setDisable() throw() {
        m_Enable = false;
    }
    void setEnable() throw() {
        m_Enable = true;
    }
    bool canUse() const throw() {
        return m_Enable;
    }

    
    Turn_t getRemainTurn(Timeval currentTime) const throw();

protected:
    string m_Name;
    SkillType_t m_SkillType; 
    Exp_t m_Exp;             
    ExpLevel_t m_ExpLevel;   
    Turn_t m_Interval;       
    Turn_t m_CastingTime;    
    Timeval m_runTime;       
    bool m_Enable;           
};

#endif // __SKILL_SLOT_H__
