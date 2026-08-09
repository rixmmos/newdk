//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUsePowerPoint.h
// Written By  : bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_POWER_POINT_H__
#define __CG_USE_POWER_POINT_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGUsePowerPoint;
//////////////////////////////////////////////////////////////////////////////

class CGUsePowerPoint : public Packet {
public:
    CGUsePowerPoint();
    ~CGUsePowerPoint();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_USE_POWER_POINT;
    }
    PacketSize_t getPacketSize() const {
        return 0;
    }
    string getPacketName() const {
        return "CGUsePowerPoint";
    }
    string toString() const;
};

//////////////////////////////////////////////////////////////////////////////
// class CGUsePowerPointFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUsePowerPointFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGUsePowerPoint();
    }
    string getPacketName() const throw() {
        return "CGUsePowerPoint";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_USE_POWER_POINT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGUsePowerPointHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGUsePowerPointHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGUsePowerPointHandler {
public:
    static void execute(CGUsePowerPoint* pCGUsePowerPoint, Player* pPlayer);
};
#endif


#endif
