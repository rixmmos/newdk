//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddMouseToZone.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_MOUSE_TO_ZONE_H__
#define __CG_ADD_MOUSE_TO_ZONE_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGAddMouseToZone;
//////////////////////////////////////////////////////////////////////////////

class CGAddMouseToZone : public Packet {
public:
    CGAddMouseToZone();
    ~CGAddMouseToZone();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_ADD_MOUSE_TO_ZONE;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }
    string getPacketName() const {
        return "CGAddMouseToZone";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

private:
    ObjectID_t m_ObjectID;
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddMouseToZoneFactory;
//////////////////////////////////////////////////////////////////////////////

class CGAddMouseToZoneFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGAddMouseToZone();
    }
    string getPacketName() const throw() {
        return "CGAddMouseToZone";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_ADD_MOUSE_TO_ZONE;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddMouseToZoneHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGAddMouseToZoneHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGAddMouseToZoneHandler {
public:
    static void execute(CGAddMouseToZone* pPacket, Player* player);
};
#endif

#endif
