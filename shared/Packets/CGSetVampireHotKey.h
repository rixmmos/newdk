//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSetVampireHotKey.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SET_VAMPIRE_HOT_KEY_H__
#define __CG_SET_VAMPIRE_HOT_KEY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGSetVampireHotKey;
//
//////////////////////////////////////////////////////////////////////

class CGSetVampireHotKey : public Packet {
public:
    CGSetVampireHotKey() {};
    virtual ~CGSetVampireHotKey() {};
    // Initialize the packet by reading data from the input stream.
    void read(SocketInputStream& iStream);

    // Serialize the packet into the output stream.
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SET_VAMPIRE_HOT_KEY;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    // Use CGSetVampireHotKeyPacketSize if that constant is defined.
    PacketSize_t getPacketSize() const {
        return szSkillType * 8;
    }

    // get packet name
    string getPacketName() const {
        return "CGSetVampireHotKey";
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
    SkillType_t m_HotKey[8];
};


//////////////////////////////////////////////////////////////////////
//
// class CGSetVampireHotKeyFactory;
//
// Factory for CGSetVampireHotKey
//
//////////////////////////////////////////////////////////////////////

// Client Cpackets copy wrapped this whole class in
// #ifdef __DEBUG_OUTPUT__ (the client never needs a factory to decode
// its own outgoing CG packets, only to build debug tooling); adopting
// the server's unconditional canonical style here per Phase 12's
// reconciliation rules is behaviorally neutral -- the class has no
// side effects, it just becomes compilable in more configurations.
class CGSetVampireHotKeyFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGSetVampireHotKey();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGSetVampireHotKey";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SET_VAMPIRE_HOT_KEY;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    // Use CGSetVampireHotKeyPacketSize if that constant is defined.
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType * 8;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSetVampireHotKeyHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGSetVampireHotKeyHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGSetVampireHotKeyHandler {
public:
    // execute packet's handler
    static void execute(CGSetVampireHotKey* pPacket, Player* player);
};
#endif

#endif
