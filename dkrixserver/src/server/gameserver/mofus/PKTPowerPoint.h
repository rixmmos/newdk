/////////////////////////////////////////////////////////////////////////////
// Filename : PKTPowerPoint.h


/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_POWERPOINT_H__
#define __PKT_POWERPOINT_H__

// include files
#include "Assert.h"
#include "MPacket.h"


struct _PKT_POWERPOINT {
    int nSize;
    int nCode;
    int nMoDataCode;         
    char sPhoneNo[12];       
    char sMemID[20];         
    int nMatchingCode;       
    char sMoGameName[20];    
    int nMoGameCode;         
    int nOnGameCode;         
    char sOnGameName[20];    
    int nOnGameSerCode;      
    char sOnGameSerName[20]; 
    char sOnGameID[20];      
    char sCharName[40];      
    int nOnAbilityCode;      
    char sOnAbilityName[20]; 
    int nPowerPoint;         
    char sInputDate[20];     
    int nIndex;              
    int nContinue;           
};

const int szPKTPowerPoint = sizeof(_PKT_POWERPOINT);

// class PKTPowerPoint
class PKTPowerPoint : public _PKT_POWERPOINT, public MPacket {
public:
    
    PKTPowerPoint();

public:
    
    MPacketID_t getID() const;

    
    MPacketSize_t getSize() const {
        return szPKTPowerPoint - szMPacketSize;
    }

    
    MPacket* create() {
        MPacket* pPacket = new PKTPowerPoint;
        Assert(pPacket != NULL);
        return pPacket;
    }

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream);

    // debug message
    string toString() const;

public:
    // get methods
    bool isContinue() const {
        return nContinue == 1;
    }

    // get PowerPoint
    int getPowerPoint() const {
        return nPowerPoint;
    }

    // get GameCode
    int getGameCode() const {
        return nOnGameCode;
    }

    // get GameServerCode
    int getGameServerCode() const {
        return nOnGameSerCode;
    }

    // get Character Name
    const char* getCharacterName() const {
        return (const char*)sCharName;
    }
};

#endif
