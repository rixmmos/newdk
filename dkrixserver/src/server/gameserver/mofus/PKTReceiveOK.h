/////////////////////////////////////////////////////////////////////////////
// Filename : PKTReceiveOK.h

/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_RECEIVE_OK_H__
#define __PKT_RECEIVE_OK_H__

// include files
#include "Assert.h"
#include "MPacket.h"


struct _PKT_RECEIVE_OK {
    int nSize; 
    int nCode; 
};

const int szPKTReceiveOK = sizeof(_PKT_RECEIVE_OK);

// class PKTReceiveOK
class PKTReceiveOK : public _PKT_RECEIVE_OK, public MPacket {
public:
    
    PKTReceiveOK();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTReceiveOK - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTReceiveOK;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;
};

#endif
