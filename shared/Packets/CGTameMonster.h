//////////////////////////////////////////////////////////////////////
//
// Filename    : CGTameMonster
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_TAME_MONSTER_H__
#define __CG_TAME_MONSTER_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGTameMonster;
//
//////////////////////////////////////////////////////////////////////

class CGTameMonster : public Packet {
public:
    CGTameMonster();
    ~CGTameMonster();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_TAME_MONSTER;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }
    string getPacketName() const {
        return "CGTameMonster";
    }
    string toString() const;

public:
    // get/set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

private:
    ObjectID_t m_ObjectID; // ObjectID
};


//////////////////////////////////////////////////////////////////////
//
// class CGTameMonsterFactory;
//
// Factory for CGTameMonster
//
//////////////////////////////////////////////////////////////////////

class CGTameMonsterFactory : public PacketFactory {
public:
    CGTameMonsterFactory() {}
    virtual ~CGTameMonsterFactory() {}

public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGTameMonster();
    }
    string getPacketName() const throw() {
        return "CGTameMonster";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_TAME_MONSTER;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};

//////////////////////////////////////////////////////////////////////
//
// class CGTameMonsterHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGTameMonsterHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGTameMonsterHandler {
public:
    static void execute(CGTameMonster* pCGTameMonster, Player* player);
};
#endif

#endif
