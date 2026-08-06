/////////////////////////////////////////////////////////////////////////////
// Filename    : InstallMine.h
// Written By  : elca
// Description :
/////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_INSTALL_MINE_HANDLER_H__
#define __SKILL_INSTALL_MINE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class InstallMine;
//////////////////////////////////////////////////////////////////////////////

class InstallMine : public SkillHandler {
public:
    InstallMine() throw() {}
    ~InstallMine() throw() {}

public:
    string getSkillHandlerName() const throw() {
        return "InstallMine";
    }
    SkillType_t getSkillType() const throw() {
        return SKILL_INSTALL_MINE;
    }

    
    void execute(Slayer* pSlayer, ObjectID_t, CoordInven_t X, CoordInven_t Y, CoordInven_t TargetX,
                 CoordInven_t TargetY, SkillSlot* pSkillSlot);

    void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern InstallMine g_InstallMine;

#endif
