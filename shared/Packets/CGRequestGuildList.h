
//////////////////////////////////////////////////////////////////////
//
// Filename    : CGRequestGuildList.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUER_GUILD_LIST_H__
#define __CG_REQUER_GUILD_LIST_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

typedef BYTE GuildType_t;


//////////////////////////////////////////////////////////////////////
//
// class CGRequestGuildList;
//
//////////////////////////////////////////////////////////////////////

class CGRequestGuildList : public Packet {
public:
    enum {
        GUILDTYPE_WAIT,
        GUILDTYPE_NORMAL,
        GUILDTYPE_MAX
    };

    CGRequestGuildList() {};
    virtual ~CGRequestGuildList() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_REQUEST_GUILD_LIST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return sizeof(GuildType_t);
    }

    // get packet name
    string getPacketName() const {
        return "CGRequestGuildList";
    }

    // get packet's debug string
    string toString() const;

    // get/set GuildType
    GuildType_t getGuildType() const {
        return m_GuildType;
    }
    void setGuildType(GuildType_t GuildType) {
        m_GuildType = GuildType;
    }

    GuildType_t m_GuildType;
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestGuildListFactory;
//
// Factory for CGRequestGuildList
//
//////////////////////////////////////////////////////////////////////

class CGRequestGuildListFactory : public PacketFactory {
public:
    // constructor
    CGRequestGuildListFactory() {}

    // destructor
    virtual ~CGRequestGuildListFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGRequestGuildList();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGRequestGuildList";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_REQUEST_GUILD_LIST;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return sizeof(GuildType_t);
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestGuildListHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGRequestGuildListHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGRequestGuildListHandler {
public:
    // execute packet's handler
    static void execute(CGRequestGuildList* pCGRequestGuildList, Player* pPlayer);
};
#endif

#endif
