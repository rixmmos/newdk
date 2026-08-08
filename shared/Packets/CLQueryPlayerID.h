//--------------------------------------------------------------------------------
//
// Filename    : CLQueryPlayerID.h
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __CL_QUERY_PLAYER_ID_H__
#define __CL_QUERY_PLAYER_ID_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CLQueryPlayerID;
//


//
//--------------------------------------------------------------------------------

class CLQueryPlayerID : public Packet {
public:
    CLQueryPlayerID() {};
    virtual ~CLQueryPlayerID() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_QUERY_PLAYER_ID;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_PlayerID.size();
    }

    // get packet name
    string getPacketName() const {
        return "CLQueryPlayerID";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set player's id
    string getPlayerID() const {
        return m_PlayerID;
    }
    void setPlayerID(const string& playerID) {
        m_PlayerID = playerID;
    }

private:

    string m_PlayerID;
};


//--------------------------------------------------------------------------------
//
// class CLQueryPlayerIDFactory;
//
// Factory for CLQueryPlayerID
//
//--------------------------------------------------------------------------------

// Client Cpackets copy wrapped this whole class in
// #ifdef __DEBUG_OUTPUT__ (the client never needs a factory to decode
// its own outgoing CL packets, only to build debug tooling); adopting
// the server's unconditional canonical style here per Phase 12's
// reconciliation rules is behaviorally neutral -- the class has no
// side effects, it just becomes compilable in more configurations.
class CLQueryPlayerIDFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CLQueryPlayerID();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CLQueryPlayerID";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CL_QUERY_PLAYER_ID;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 20;
    }
};


//--------------------------------------------------------------------------------
//
// class CLQueryPlayerIDHandler;
//
//--------------------------------------------------------------------------------

// Server-only: CLQueryPlayerIDHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CLQueryPlayerIDHandler {
public:
    // execute packet's handler
    static void execute(CLQueryPlayerID* pPacket, Player* pPlayer);
};
#endif

#endif
