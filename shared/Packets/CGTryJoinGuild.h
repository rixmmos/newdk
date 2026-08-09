//////////////////////////////////////////////////////////////////////
//
// Filename    : CGTryJoinGuild.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_TRY_JOIN_GUILD_H__
#define __CG_TRY_JOIN_GUILD_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGTryJoinGuild;
//
//////////////////////////////////////////////////////////////////////

class CGTryJoinGuild : public Packet {
public:
    CGTryJoinGuild() {};
    virtual ~CGTryJoinGuild() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_TRY_JOIN_GUILD;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID + szGuildMemberRank;
    }

    // get packet name
    string getPacketName() const {
        return "CGTryJoinGuild";
    }

    // get packet's debug string
    string toString() const;

    // get/set Guild ID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t GuildID) {
        m_GuildID = GuildID;
    }

    // get/set Guild Member Rank
    GuildMemberRank_t getGuildMemberRank() const {
        return m_GuildMemberRank;
    }
    void setGuildMemberRank(GuildMemberRank_t GuildMemberRank) {
        m_GuildMemberRank = GuildMemberRank;
    }


private:
    // Guild ID
    GuildID_t m_GuildID;

    // Guild Member Rank
    GuildMemberRank_t m_GuildMemberRank;
};


//////////////////////////////////////////////////////////////////////
//
// class CGTryJoinGuildFactory;
//
// Factory for CGTryJoinGuild
//
//////////////////////////////////////////////////////////////////////

class CGTryJoinGuildFactory : public PacketFactory {
public:
    // constructor
    CGTryJoinGuildFactory() {}

    // destructor
    virtual ~CGTryJoinGuildFactory() {}


public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGTryJoinGuild();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGTryJoinGuild";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_TRY_JOIN_GUILD;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szGuildID + szGuildMemberRank;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGTryJoinGuildHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGTryJoinGuildHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGTryJoinGuildHandler {
public:
    // execute packet's handler
    static void execute(CGTryJoinGuild* pCGTryJoinGuild, Player* pPlayer);
};
#endif

#endif
