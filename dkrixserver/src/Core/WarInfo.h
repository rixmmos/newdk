//--------------------------------------------------------------------------------
//
// Filename    : WarInfo.h
// Written By  :
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __WAR_LIST_H__
#define __WAR_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "ValueList.h"
#include "types/WarTypes.h"

//--------------------------------------------------------------------------------
//
// class WarInfo;
//

//
//--------------------------------------------------------------------------------

class WarInfo {
public:
    WarInfo() {
        m_StartTime = 0;
        m_RemainTime = 0;
    }
    virtual ~WarInfo() {}

    
    virtual void read(SocketInputStream& iStream);

    
    virtual void write(SocketOutputStream& oStream) const;

    virtual PacketSize_t getSize() const {
        return szDWORD + szDWORD;
    }

    static PacketSize_t getMaxSize() {
        return szDWORD + szDWORD;
    }

    // get packet's debug string
    virtual string toString() const = 0;

public:
    virtual WarType_t getWarType() const = 0;

    DWORD getRemainTime() const {
        return m_RemainTime;
    }
    void setRemainTime(DWORD rt) {
        m_RemainTime = rt;
    }

    DWORD getStartTime() const {
        return m_StartTime;
    }
    void setStartTime(DWORD st) {
        m_StartTime = st;
    }

protected:
    DWORD m_RemainTime; 
    DWORD m_StartTime;  
};

#endif
