/////////////////////////////////////////////////////////////////////////////
// Filename : PKTConnectAsk.h


/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_CONNECT_ASK_H__
#define __PKT_CONNECT_ASK_H__

// include files
#include "Assert.h"
#include "MPacket.h"


struct _PKT_CONNECT_ASK {
    int nSize;       
    int nCode;       
    int nOnGameCode; 
};

const int szPKTConnectAsk = sizeof(_PKT_CONNECT_ASK);

// class PKTConnectASK
class PKTConnectAsk : public _PKT_CONNECT_ASK, public MPacket {
public:
    
    PKTConnectAsk();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTConnectAsk - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTConnectAsk;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;

public:
    // get/set OnGameCode
    int getOnGameCode() const {
        return nOnGameCode;
    }
    void setOnGameCode(int onGameCode) {
        nOnGameCode = onGameCode;
    }
};

#endif
