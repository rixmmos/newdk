/////////////////////////////////////////////////////////////////////////////
// Filename : PKTError.h



/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_ERROR_H__
#define __PKT_ERROR_H__

// include files
#include "Assert.h"
#include "MPacket.h"


enum MERR_CODE {
    MERR_SERVER = 0x01,    
                           
    MERR_CONFIRM = 0x02,   
    MERR_PACKET = 0x03,    
    MERR_PROCESS = 0x04,   
    MERR_SEARCH = 0x05,    
    MERR_NULLPOINT = 0x06, 
                           
    MERR_MATCHING = 0x07,  
                           
};


struct _PKT_ERROR {
    int nSize;  
    int nCode;  
    int nError; 
};

const int szPKTError = sizeof(_PKT_ERROR);

// class PKTError
class PKTError : public _PKT_ERROR, public MPacket {
public:
    
    PKTError();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTError - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTError;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;

public:
    // get error code
    int getErrorCode() const {
        return nError;
    }
};

#endif
