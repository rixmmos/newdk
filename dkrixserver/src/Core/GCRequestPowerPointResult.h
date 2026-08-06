//--------------------------------------------------------------------------------
//
// Filename    : GCRequestPowerPointResult.h
// Written By  : bezz
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __GC_REQUEST_POWER_POINT_RESULT_H__
#define __GC_REQUEST_POWER_POINT_RESULT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCRequestPowerPointResult;
//
//--------------------------------------------------------------------------------

class GCRequestPowerPointResult : public Packet {
public:
    enum RESULT_CODE {
        NO_ERROR = 0,
        SERVER_ERROR,  
        PROCESS_ERROR, 
        NO_MEMBER,     
        NO_POINT,      
        NO_MATCHING,   
                       
                       
        CONNECT_ERROR, 
    };

public:
    GCRequestPowerPointResult();
    ~GCRequestPowerPointResult();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_REQUEST_POWER_POINT_RESULT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + szint + szint;
    }

    // get packet name
    string getPacketName() const {
        return "GCRequestPowerPointResult";
    }

    // get packet's debug string
    string toString() const;

    // get / set Error Code
    BYTE getErrorCode() const {
        return m_ErrorCode;
    }
    void setErrorCode(BYTE errorcode) {
        m_ErrorCode = errorcode;
    }

    // get / set SumPowerPoint
    int getSumPowerPoint() const {
        return m_SumPowerPoint;
    }
    void setSumPowerPoint(int powerpoint) {
        m_SumPowerPoint = powerpoint;
    }

    // get / set RequestPowerPoint
    int getRequestPowerPoint() const {
        return m_RequestPowerPoint;
    }
    void setRequestPowerPoint(int powerpoint) {
        m_RequestPowerPoint = powerpoint;
    }

    //--------------------------------------------------
    // data members
    //--------------------------------------------------
private:
    
    BYTE m_ErrorCode;

    
    int m_SumPowerPoint;

    
    int m_RequestPowerPoint;
};


//--------------------------------------------------------------------------------
//
// class GCRequestPowerPointResultFactory;
//
// Factory for GCRequestPowerPointResult
//
//--------------------------------------------------------------------------------

class GCRequestPowerPointResultFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCRequestPowerPointResult();
    }

    // get packet name
    string getPacketName() const {
        return "GCRequestPowerPointResult";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_REQUEST_POWER_POINT_RESULT;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szint + szint;
    }
};


//--------------------------------------------------------------------------------
//
// class GCRequestPowerPointResultHandler;
//
//--------------------------------------------------------------------------------

class GCRequestPowerPointResultHandler {
public:
    // execute packet's handler
    static void execute(GCRequestPowerPointResult* pPacket, Player* pPlayer);
};

#endif
