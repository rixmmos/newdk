//-----------------------------------------------------------------------------
//
// Filename    : CGRequestRepair.h

// Description :
//
//-----------------------------------------------------------------------------

#ifndef __CG_REQUEST_REPAIR_H__
#define __CG_REQUEST_REPAIR_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CGRequestRepair;
//
//--------------------------------------------------------------------------------

class CGRequestRepair : public Packet {
public:
    CGRequestRepair() {};
    virtual ~CGRequestRepair() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_REQUEST_REPAIR;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet name
    string getPacketName() const {
        return "CGRequestRepair";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set ObjectID
    ObjectID_t getObjectID() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

private:
    // Item Object ID
    ObjectID_t m_ObjectID;
};


//-----------------------------------------------------------------------------
//
// class CGRequestRepairFactory;
//
// Factory for CGRequestRepair
//
//-----------------------------------------------------------------------------

class CGRequestRepairFactory : public PacketFactory {
public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGRequestRepair();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGRequestRepair";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_REQUEST_REPAIR;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};


//--------------------------------------------------------------------------------
//
// class CGRequestRepairHandler;
//
//--------------------------------------------------------------------------------

// Server-only: CGRequestRepairHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGRequestRepairHandler {
public:
    // execute packet's handler
    static void execute(CGRequestRepair* pPacket, Player* player);
    static void executeNormal(CGRequestRepair* pPacket, Player* player);
    static void executeMotorcycle(CGRequestRepair* pPacket, Player* player);
    static void executeAll(CGRequestRepair* pPacket, Player* player);
};
#endif

#endif
