//////////////////////////////////////////////////////////////////////
//
// Filename    : CGUntransform.h
// Written By  : crazydog

//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_UNTRANSFORM_H__
#define __CG_UNTRANSFORM_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGUntransform;
//
//////////////////////////////////////////////////////////////////////

class CGUntransform : public Packet {
public:
    CGUntransform() {};
    virtual ~CGUntransform() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_UNTRANSFORM;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet name
    string getPacketName() const {
        return "CGUntransform";
    }

    // get packet's debug string
    string toString() const;
};


//////////////////////////////////////////////////////////////////////
//
// class CGUntransformFactory;
//
// Factory for CGUntransform
//
//////////////////////////////////////////////////////////////////////

class CGUntransformFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGUntransform();
    }

    // get packet name
    string getPacketName() const {
        return "CGUntransform";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_UNTRANSFORM;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGUntransformHandler;
//
//////////////////////////////////////////////////////////////////////

class CGUntransformHandler {
public:
    // execute packet's handler
    static void execute(CGUntransform* pPacket, Player* player);
};

#endif
