/////////////////////////////////////////////////////////////////////////////
// Filename : PKTSError.h

/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_SERROR_H__
#define __PKT_SERROR_H__

// include files
#include "Assert.h"
#include "MPacket.h"


enum MSERR_CODE {
    MSERR_MATCH = 0x01, 
                        
                        
};


struct _PKT_SERROR {
    int nSize;  
    int nCode;  
    int nError; 
};

const int szPKTSError = sizeof(_PKT_SERROR);

// class PKTSError
class PKTSError : public _PKT_SERROR, public MPacket {
public:
    
    PKTSError();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTSError - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTSError;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;

public:
    
    void setErrorCode(int errorCode) {
        nError = errorCode;
    }
};

#endif
