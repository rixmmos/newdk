//////////////////////////////////////////////////////////////////////////////
// Filename    : CGResurrect.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_RESURRECT_H__
#define __CG_RESURRECT_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGResurrect;
//////////////////////////////////////////////////////////////////////////////

class CGResurrect : public Packet {
public:
    CGResurrect() {};
    virtual ~CGResurrect() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_RESURRECT;
    }
    PacketSize_t getPacketSize() const {
        return 0;
    }
    string getPacketName() const {
        return "CGResurrect";
    }
    string toString() const;
};

//////////////////////////////////////////////////////////////////////////////
// class CGResurrectFactory;
//////////////////////////////////////////////////////////////////////////////

class CGResurrectFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGResurrect();
    }
    string getPacketName() const throw() {
        return "CGResurrect";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_RESURRECT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGResurrectHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGResurrectHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGResurrectHandler {
public:
    static void execute(CGResurrect* pPacket, Player* player);
};
#endif

#endif
