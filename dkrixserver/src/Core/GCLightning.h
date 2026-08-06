//--------------------------------------------------------------------------------
//
// Filename    : GCLightning.h
// Written By  : reiot
//
//--------------------------------------------------------------------------------

#ifndef __GC_LIGHTNING_H__
#define __GC_LIGHTNING_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCLightning;
//

//
//
//////////////////////////////////////////////////////////////////////

class GCLightning : public Packet {
public:
    GCLightning() {};
    ~GCLightning() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_LIGHTNING;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szBYTE;
    }

    // get packet's name
    string getPacketName() const {
        return "GCLightning";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set delay
    BYTE getDelay() const {
        return m_Delay;
    }
    void setDelay(BYTE delay) {
        m_Delay = delay;
    }


private:
    
    
    BYTE m_Delay;
};


//////////////////////////////////////////////////////////////////////
//
// class GCLightningFactory;
//
// Factory for GCLightning
//
//////////////////////////////////////////////////////////////////////

class GCLightningFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCLightning();
    }

    // get packet name
    string getPacketName() const {
        return "GCLightning";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_LIGHTNING;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCLightningHandler;
//
//////////////////////////////////////////////////////////////////////

class GCLightningHandler {
public:
    // execute packet's handler
    static void execute(GCLightning* pPacket, Player* pPlayer);
};

#endif
