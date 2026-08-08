//////////////////////////////////////////////////////////////////////
//
// Filename    : CGModifyGuildMemberIntro.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_MODIFY_GUILD_MEMBER_INTRO_H__
#define __CG_MODIFY_GUILD_MEMBER_INTRO_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildMemberIntro;
//
//////////////////////////////////////////////////////////////////////

class CGModifyGuildMemberIntro : public Packet {
public:
    CGModifyGuildMemberIntro() {};
    ~CGModifyGuildMemberIntro() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_MODIFY_GUILDMEMBER_INTRO;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID + szBYTE + m_GuildMemberIntro.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGModifyGuildMemberIntro";
    }

    // get packet's debug string
    string toString() const;

    // get/set GuildID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t guildID) {
        m_GuildID = guildID;
    }

    // get/set intro
    const string& getGuildMemberIntro() const {
        return m_GuildMemberIntro;
    }
    void setGuildMemberIntro(const string& intro) {
        m_GuildMemberIntro = intro;
    }

private:
    // GuildID
    GuildID_t m_GuildID;

    // Guild Member Intro
    string m_GuildMemberIntro;
};


//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildMemberIntroFactory;
//
// Factory for CGModifyGuildMemberIntro
//
//////////////////////////////////////////////////////////////////////

// Unconditional (matching the server's pre-migration file): the client
// Cpackets copy wrapped this whole class in #ifdef __DEBUG_OUTPUT__,
// but that macro is never defined by any target in either tree, so the
// class never actually compiled on the client either way (dead code
// pre-migration). The server's own PacketFactoryManager.cpp registers
// CGModifyGuildMemberIntroFactory unconditionally, so it must stay
// unconditional here or the server build breaks.
class CGModifyGuildMemberIntroFactory : public PacketFactory {
public:
    // constructor
    CGModifyGuildMemberIntroFactory() {}

    // destructor
    virtual ~CGModifyGuildMemberIntroFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGModifyGuildMemberIntro();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGModifyGuildMemberIntro";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_MODIFY_GUILDMEMBER_INTRO;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szGuildID + szBYTE + 255;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildMemberIntroHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGModifyGuildMemberIntroHandler::execute has no
// client-side definition or use. Guarded (matching the client Cpackets
// copy's existing guard) since no CGHandlersStub.cpp-style client stub
// exists for this family.
#ifndef __GAME_CLIENT__
class CGModifyGuildMemberIntroHandler {
public:
    // execute packet's handler
    static void execute(CGModifyGuildMemberIntro* pCGModifyGuildMemberIntro, Player* pPlayer);
};
#endif

#endif
