/////////////////////////////////////////////////////////////////////////////
// Filename : PKTResult.h


/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_RESULT_H__
#define __PKT_RESULT_H__

// include files
#include "Assert.h"
#include "MPacket.h"


struct _PKT_RESULT {
    int nSize; 
    int nCode; 
};

const int szPKTResult = sizeof(_PKT_RESULT);

// class PKTResult
class PKTResult : public _PKT_RESULT, public MPacket {
public:
    
    PKTResult();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTResult - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTResult;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;
};

#endif
