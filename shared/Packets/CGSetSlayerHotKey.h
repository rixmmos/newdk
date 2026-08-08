//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSetSlayerHotKey.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SET_SLAYER_HOT_KEY_H__
#define __CG_SET_SLAYER_HOT_KEY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKey;
//
//////////////////////////////////////////////////////////////////////

class CGSetSlayerHotKey : public Packet {
public:
    CGSetSlayerHotKey() {};
    virtual ~CGSetSlayerHotKey() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SET_SLAYER_HOT_KEY;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*

    PacketSize_t getPacketSize() const {
        return szSkillType * 4;
    }

    // get packet name
    string getPacketName() const {
        return "CGSetSlayerHotKey";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set X Coordicate
    SkillType_t getHotKey(BYTE pos) const {
        return m_HotKey[pos];
    }
    void setHotKey(BYTE pos, SkillType_t SkillType) {
        m_HotKey[pos] = SkillType;
    }

private:
    SkillType_t m_HotKey[4];
};


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKeyFactory;
//
// Factory for CGSetSlayerHotKey
//
//////////////////////////////////////////////////////////////////////

// Client Cpackets copy wrapped this whole class in
// #ifdef __DEBUG_OUTPUT__ (the client never needs a factory to decode
// its own outgoing CG packets, only to build debug tooling); adopting
// the server's unconditional canonical style here per Phase 12's
// reconciliation rules is behaviorally neutral -- the class has no
// side effects, it just becomes compilable in more configurations.
class CGSetSlayerHotKeyFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGSetSlayerHotKey();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGSetSlayerHotKey";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SET_SLAYER_HOT_KEY;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*

    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType * 4;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKeyHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGSetSlayerHotKeyHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGSetSlayerHotKeyHandler {
public:
    // execute packet's handler
    static void execute(CGSetSlayerHotKey* pPacket, Player* player);
};
#endif

#endif
