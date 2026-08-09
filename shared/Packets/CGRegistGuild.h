//////////////////////////////////////////////////////////////////////
//
// Filename    : CGRegistGuild.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REGIST_GUILD_H__
#define __CG_REGIST_GUILD_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGRegistGuild;
//
//////////////////////////////////////////////////////////////////////

class CGRegistGuild : public Packet {
public:
    CGRegistGuild() {};
    ~CGRegistGuild() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_REGIST_GUILD;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_GuildName.size() + szBYTE + m_GuildIntro.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGRegistGuild";
    }

    // get packet's debug string
    string toString() const;

    // get/set GuildName
    string getGuildName() const {
        return m_GuildName;
    }
    void setGuildName(const string& guildName) {
        m_GuildName = guildName;
    }

    // get/set GuildIntro
    string getGuildIntro() const {
        return m_GuildIntro;
    }
    void setGuildIntro(const string& guildIntro) {
        m_GuildIntro = guildIntro;
    }


private:
    // Guild Name
    string m_GuildName;

    // Guild Intro
    string m_GuildIntro;
};


//////////////////////////////////////////////////////////////////////
//
// class CGRegistGuildFactory;
//
// Factory for CGRegistGuild
//
//////////////////////////////////////////////////////////////////////

class CGRegistGuildFactory : public PacketFactory {
public:
    // constructor
    CGRegistGuildFactory() {}

    // destructor
    virtual ~CGRegistGuildFactory() {}


public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGRegistGuild();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGRegistGuild";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_REGIST_GUILD;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 30 + szBYTE + 256;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGRegistGuildHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGRegistGuildHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGRegistGuildHandler {
public:
    // execute packet's handler
    static void execute(CGRegistGuild* pCGRegistGuild, Player* pPlayer);
};
#endif

#endif
