//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRealWearingInfo.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_REAL_WEARING_INFO_H__
#define __GC_REAL_WEARING_INFO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCRealWearingInfo;
//
//////////////////////////////////////////////////////////////////////

class GCRealWearingInfo : public Packet {
public:
    GCRealWearingInfo() {};
    ~GCRealWearingInfo() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_REAL_WEARING_INFO;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szDWORD;
    }

    // get packet name
    string getPacketName() const {
        return "GCRealWearingInfo";
    }

    // get packet's debug string
    string toString() const;

public:
    DWORD getInfo(void) const {
        return m_Info;
    }
    void setInfo(DWORD info) {
        m_Info = info;
    }

private:
    DWORD m_Info;
};


//////////////////////////////////////////////////////////////////////
//
// class GCRealWearingInfoFactory;
//
// Factory for GCRealWearingInfo
//
//////////////////////////////////////////////////////////////////////

class GCRealWearingInfoFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCRealWearingInfo();
    }

    // get packet name
    string getPacketName() const {
        return "GCRealWearingInfo";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_REAL_WEARING_INFO;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szDWORD;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCRealWearingInfoHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRealWearingInfoHandler {
public:
    // execute packet's handler
    static void execute(GCRealWearingInfo* pPacket, Player* player);
};

#endif
