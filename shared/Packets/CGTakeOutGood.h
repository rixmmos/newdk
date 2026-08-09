//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTakeOutGood.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TAKE_OUT_GOOD_H__
#define __CG_TAKE_OUT_GOOD_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGTakeOutGood;
//////////////////////////////////////////////////////////////////////////////

class CGTakeOutGood : public Packet {
public:
    CGTakeOutGood();
    ~CGTakeOutGood();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_TAKE_OUT_GOOD;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }
    string getPacketName() const {
        return "CGTakeOutGood";
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
// class CGTakeOutGoodFactory;
//////////////////////////////////////////////////////////////////////////////

class CGTakeOutGoodFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGTakeOutGood();
    }
    string getPacketName() const throw() {
        return "CGTakeOutGood";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_TAKE_OUT_GOOD;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGTakeOutGoodHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGTakeOutGoodHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGTakeOutGoodHandler {
public:
    static void execute(CGTakeOutGood* pPacket, Player* player);
};
#endif

#endif
