/////////////////////////////////////////////////////////////////////////////
// Filename : PKTLogout.h

/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_LOGOUT_H__
#define __PKT_LOGOUT_H__

// include files
#include "Assert.h"
#include "MPacket.h"


struct _PKT_LOGOUT {
    int nSize; 
    int nCode; 
};

const int szPKTLogout = sizeof(_PKT_LOGOUT);

// class PKTLogout
class PKTLogout : public _PKT_LOGOUT, public MPacket {
public:
    
    PKTLogout();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTLogout - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTLogout;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;
};

#endif
