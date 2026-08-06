/////////////////////////////////////////////////////////////////////////////
// Filename : PKTUserInfo.h


/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_USERINFO_H__
#define __PKT_USERINFO_H__

// include files
#include "Assert.h"
#include "MPacket.h"


struct _PKT_USERINFO {
    int nSize;
    int nCode;
    char sJuminNo[20];   
    char sHandPhone[12]; 
    int nIndex;          
};

const int szPKTUserInfo = sizeof(_PKT_USERINFO);

// class PKTUserInfo
class PKTUserInfo : public _PKT_USERINFO, public MPacket {
public:
    
    PKTUserInfo();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTUserInfo - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTUserInfo;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;

public:
    // set ssn & cellnum
    void setSSN(const string& ssn);
    void setCellNum(const string& cellnum);
};

#endif
