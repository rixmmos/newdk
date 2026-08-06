//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillHandlerManager.h
// Wrrtten by  : elca@ewestsoft.com

//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HANDLER_MANAGER_H__
#define __SKILL_HANDLER_MANAGER_H__

#include "Skill.h"
#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SkillHandlerManager
//////////////////////////////////////////////////////////////////////////////

class SkillHandlerManager {
public:
    SkillHandlerManager() throw();
    ~SkillHandlerManager() throw();

public:
    
    void init();

    
    void addSkillHandler(SkillHandler* pSkillHandler);

    
    SkillHandler* getSkillHandler(SkillType_t SkillType);

    // toString
    string toString() const throw();

private:
    SkillHandler** m_SkillHandlers; 
    ushort m_Size;                  
};

extern SkillHandlerManager* g_pSkillHandlerManager;

#endif
