//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSilverCoating.h

// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SILVER_COATING_H__
#define __CG_SILVER_COATING_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSilverCoating
//////////////////////////////////////////////////////////////////////////////

class CGSilverCoating : public Packet {
public:
    CGSilverCoating() {};
    virtual ~CGSilverCoating() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);

    PacketID_t getPacketID() const {
        return PACKET_CG_SILVER_COATING;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }
    string getPacketName() const {
        return "CGSilverCoating";
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
    ObjectID_t m_ObjectID; // Item Object ID
};


//////////////////////////////////////////////////////////////////////////////
// class CGSilverCoatingFactory;
//////////////////////////////////////////////////////////////////////////////

// Client Cpackets copy wrapped this whole class in
// #ifdef __DEBUG_OUTPUT__ (the client never needs a factory to decode
// its own outgoing CG packets, only to build debug tooling); adopting
// the server's unconditional canonical style here per Phase 12's
// reconciliation rules is behaviorally neutral -- the class has no
// side effects, it just becomes compilable in more configurations.
class CGSilverCoatingFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGSilverCoating();
    }
    string getPacketName() const throw() {
        return "CGSilverCoating";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SILVER_COATING;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class CGSilverCoatingHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGSilverCoatingHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGSilverCoatingHandler {
public:
    static void execute(CGSilverCoating* pPacket, Player* player);
};
#endif

#endif
