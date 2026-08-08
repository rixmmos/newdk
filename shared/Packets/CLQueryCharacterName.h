//--------------------------------------------------------------------------------
//
// Filename    : CLQueryCharacterName.h
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __CL_QUERY_CHARACTER_NAME_H__
#define __CL_QUERY_CHARACTER_NAME_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CLQueryCharacterName;
//
// Packet sent by the client on login when requesting character name lookup.
// ID and password are encrypted.
//
//--------------------------------------------------------------------------------

class CLQueryCharacterName : public Packet {
public:
    CLQueryCharacterName() {};
    virtual ~CLQueryCharacterName() {};
    // Initialize packet by reading data from the incoming stream.
    void read(SocketInputStream& iStream);

    // Serialize packet data to the outgoing stream.
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_QUERY_CHARACTER_NAME;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_CharacterName.size();
    }

    // get packet name
    string getPacketName() const {
        return "CLQueryCharacterName";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set player's id
    string getCharacterName() const {
        return m_CharacterName;
    }
    void setCharacterName(const string& playerID) {
        m_CharacterName = playerID;
    }

private:
    // Player ID (character name)
    string m_CharacterName;
};


//--------------------------------------------------------------------------------
//
// class CLQueryCharacterNameFactory;
//
// Factory for CLQueryCharacterName
//
//--------------------------------------------------------------------------------

// Client Cpackets copy wrapped getPacketName()/toString() (only) in
// #ifdef __DEBUG_OUTPUT__; adopting the server's unconditional canonical
// style per Phase 12's reconciliation rules.
class CLQueryCharacterNameFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CLQueryCharacterName();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CLQueryCharacterName";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CL_QUERY_CHARACTER_NAME;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 20;
    }
};


//--------------------------------------------------------------------------------
//
// class CLQueryCharacterNameHandler;
//
//--------------------------------------------------------------------------------

// Server-only: CLQueryCharacterNameHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CLQueryCharacterNameHandler {
public:
    // execute packet's handler
    static void execute(CLQueryCharacterName* pPacket, Player* pPlayer);
};
#endif

#endif
