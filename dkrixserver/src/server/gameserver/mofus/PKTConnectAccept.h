/////////////////////////////////////////////////////////////////////////////
// Filename : PKTConnectAccept.h

/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_CONNECT_ACCEPT_H__
#define __PKT_CONNECT_ACCEPT_H__

// include files
#include "Assert.h"
#include "MPacket.h"


struct _PKT_CONNECT_ACCEPT {
    int nSize; 
    int nCode; 
};

const int szPKTConnectAccept = sizeof(_PKT_CONNECT_ACCEPT);

// class PKTConnectAccept
class PKTConnectAccept : public _PKT_CONNECT_ACCEPT, public MPacket {
public:
    
    PKTConnectAccept();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTConnectAccept - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTConnectAccept;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;
};

#endif
