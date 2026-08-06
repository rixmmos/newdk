//////////////////////////////////////////////////////////////////////
//
// Filename    : PCSkillInfo.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __PC_SKILL_INFO_H__
#define __PC_SKILL_INFO_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class PCSkillInfo;
//

//
//////////////////////////////////////////////////////////////////////

class PCSkillInfo {
public:
    // destructor
    virtual ~PCSkillInfo() {}

public:
    
    virtual void read(SocketInputStream& iStream) = 0;

    
    virtual void write(SocketOutputStream& oStream) const = 0;

    // get packet's body size
    
    virtual PacketSize_t getSize() = 0;

    // get packet's debug string
    virtual string toString() const = 0;
};

#endif
