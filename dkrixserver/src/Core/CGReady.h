//////////////////////////////////////////////////////////////////////
//
// Filename    : CGReady.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_READY_H__
#define __CG_READY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGReady;
//
//////////////////////////////////////////////////////////////////////

class CGReady : public Packet {
public:
    CGReady() {};
    ~CGReady() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_READY;
    }

    // get packet body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet's name
    string getPacketName() const {
        return "CGReady";
    }

    // get packet's debug string
    string toString() const {
        return "CGReady";
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGReadyFactory;
//
// Factory for CGReady
//
//////////////////////////////////////////////////////////////////////

class CGReadyFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGReady();
    }

    // get packet name
    string getPacketName() const {
        return "CGReady";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_READY;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGReadyHandler;
//
//////////////////////////////////////////////////////////////////////

class CGReadyHandler {
public:
    // execute packet's handler
    static void execute(CGReady* pPacket, Player* pPlayer);
};

#endif
