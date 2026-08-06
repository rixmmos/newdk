//----------------------------------------------------------------------
//
// Filename    : SubVampireSkillInfo.h
// Written By  : elca
// Description :
//
//----------------------------------------------------------------------

#ifndef __SUB_VAMPIRE_SKILL_INFO_H__
#define __SUB_VAMPIRE_SKILL_INFO_H__

// include files
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Types.h"

//----------------------------------------------------------------------
//

//


//
//----------------------------------------------------------------------

class SubVampireSkillInfo {
public:
    // read data from socket input stream
    void read(SocketInputStream& iStream);

    // write data to socket output stream
    void write(SocketOutputStream& oStream) const;

    // get size of object
    uint getSize() const {
        return szSkillType + szTurn + szTurn;
    }
    // get max size of object
    static uint getMaxSize() {
        return szSkillType + szTurn + szTurn;
    }

    // get debug string
    string toString() const;

public:
    // get / set SkillType
    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

    // get / set Turn
    Turn_t getSkillTurn() const {
        return m_Interval;
    }
    void setSkillTurn(Turn_t SkillTurn) {
        m_Interval = SkillTurn;
    }

    // get / set CastingTime
    Turn_t getCastingTime() const {
        return m_CastingTime;
    }
    void setCastingTime(Turn_t CastingTime) {
        m_CastingTime = CastingTime;
    }

private:
    
    SkillType_t m_SkillType;

    
    Turn_t m_Interval;

    
    Turn_t m_CastingTime;
};

#endif
