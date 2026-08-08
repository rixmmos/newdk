//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSelectGuild.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_GUILD_H__
#define __CG_SELECT_GUILD_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSelectGuild;
//
//////////////////////////////////////////////////////////////////////

class CGSelectGuild : public Packet {
public:
    CGSelectGuild() {};
    virtual ~CGSelectGuild() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_GUILD;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID;
    }

    // get packet name
    string getPacketName() const {
        return "CGSelectGuild";
    }

    // get packet's debug string
    string toString() const;

    // get/set GuildID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t GuildID) {
        m_GuildID = GuildID;
    }


private:
    // Guild ID
    GuildID_t m_GuildID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectGuildFactory;
//
// Factory for CGSelectGuild
//
//////////////////////////////////////////////////////////////////////

class CGSelectGuildFactory : public PacketFactory {
public:
    // constructor
    CGSelectGuildFactory() {}

    // destructor
    virtual ~CGSelectGuildFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGSelectGuild();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGSelectGuild";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SELECT_GUILD;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szGuildID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectGuildHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGSelectGuildHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGSelectGuildHandler {
public:
    // execute packet's handler
    static void execute(CGSelectGuild* pCGSelectGuild, Player* pPlayer);
};
#endif

#endif
