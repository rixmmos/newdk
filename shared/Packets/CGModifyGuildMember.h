//////////////////////////////////////////////////////////////////////
//
// Filename    : CGModifyGuildMember.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_MODIFY_GUILD_MEMBER_H__
#define __CG_MODIFY_GUILD_MEMBER_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildMember;
//
//////////////////////////////////////////////////////////////////////

class CGModifyGuildMember : public Packet {
public:
    CGModifyGuildMember() {};
    ~CGModifyGuildMember() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_MODIFY_GUILD_MEMBER;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID + szBYTE + m_Name.size() + szGuildMemberRank;
    }

    // get packet name
    string getPacketName() const {
        return "CGModifyGuildMember";
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

    // get/set name
    const string& getName() const {
        return m_Name;
    }
    void setName(const string& name) {
        m_Name = name;
    }

    // get/set Guild Member Rank
    GuildMemberRank_t getGuildMemberRank() const {
        return m_GuildMemberRank;
    }
    void setGuildMemberRank(GuildMemberRank_t GuildMemberRank) {
        m_GuildMemberRank = GuildMemberRank;
    }

private:
    // GuildID
    GuildID_t m_GuildID;

    // name
    string m_Name;

    // Guild Member Rank
    GuildMemberRank_t m_GuildMemberRank;
};


// Unconditional (matching the server's pre-migration file): the client
// Cpackets copy wrapped this whole class in #ifdef __DEBUG_OUTPUT__,
// but that macro is never defined by any target in either tree, so the
// class never actually compiled on the client either way (dead code
// pre-migration). The server's own PacketFactoryManager.cpp registers
// CGModifyGuildMemberFactory unconditionally, so it must stay
// unconditional here or the server build breaks.
//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildMemberFactory;
//
// Factory for CGModifyGuildMember
//
//////////////////////////////////////////////////////////////////////

class CGModifyGuildMemberFactory : public PacketFactory {
public:
    // constructor
    CGModifyGuildMemberFactory() {}

    // destructor
    virtual ~CGModifyGuildMemberFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGModifyGuildMember();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGModifyGuildMember";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_MODIFY_GUILD_MEMBER;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szGuildID + szBYTE + 20 + szGuildMemberRank;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildMemberHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGModifyGuildMemberHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGModifyGuildMemberHandler {
public:
    // execute packet's handler
    static void execute(CGModifyGuildMember* pCGModifyGuildMember, Player* pPlayer);
};
#endif

#endif
