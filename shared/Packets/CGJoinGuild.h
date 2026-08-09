//////////////////////////////////////////////////////////////////////
//
// Filename    : CGJoinGuild.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_JOIN_GUILD_H__
#define __CG_JOIN_GUILD_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGJoinGuild;
//
//////////////////////////////////////////////////////////////////////

class CGJoinGuild : public Packet {
public:
    CGJoinGuild() {};
    ~CGJoinGuild() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_JOIN_GUILD;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID + szGuildMemberRank + szBYTE + m_GuildMemberIntro.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGJoinGuild";
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

    // get/set Guild Member Intro
    string getGuildMemberIntro() const {
        return m_GuildMemberIntro;
    }
    void setGuildMemberIntro(const string& GuildMemberIntro) {
        m_GuildMemberIntro = GuildMemberIntro;
    }


private:
    // Guild ID
    GuildID_t m_GuildID;

    // Guild Member Rank
    GuildMemberRank_t m_GuildMemberRank;

    // Guild Member Intro
    string m_GuildMemberIntro;
};


//////////////////////////////////////////////////////////////////////
//
// class CGJoinGuildFactory;
//
// Factory for CGJoinGuild
//
//////////////////////////////////////////////////////////////////////

class CGJoinGuildFactory : public PacketFactory {
public:
    // constructor
    CGJoinGuildFactory() {}

    // destructor
    virtual ~CGJoinGuildFactory() {}


public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGJoinGuild();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGJoinGuild";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_JOIN_GUILD;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szGuildID + szGuildMemberRank + szBYTE + 256;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGJoinGuildHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGJoinGuildHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGJoinGuildHandler {
public:
    // execute packet's handler
    static void execute(CGJoinGuild* pCGJoinGuild, Player* pPlayer);
};
#endif

#endif
