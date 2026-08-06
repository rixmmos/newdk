//----------------------------------------------------------------------
//
// Filename    : SGDeleteGuildOK.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __SG_DELETE_GUILD_OK_H__
#define __SG_DELETE_GUILD_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class SGDeleteGuildOK;
//

//
// *CAUTION*
//
//----------------------------------------------------------------------

class SGDeleteGuildOK : public Packet {
public:
    SGDeleteGuildOK() {};
    ~SGDeleteGuildOK() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_SG_DELETE_GUILD_OK;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID; // guild ID
    }

    // get packet name
    string getPacketName() const {
        return "SGDeleteGuildOK";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set guildID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t guildID) {
        m_GuildID = guildID;
    }

private:
    // GuildID
    GuildID_t m_GuildID;
};


//////////////////////////////////////////////////////////////////////
//
// class SGDeleteGuildOKFactory;
//
// Factory for SGDeleteGuildOK
//
//////////////////////////////////////////////////////////////////////

class SGDeleteGuildOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new SGDeleteGuildOK();
    }

    // get packet name
    string getPacketName() const {
        return "SGDeleteGuildOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_SG_DELETE_GUILD_OK;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szGuildID; // guild ID
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGDeleteGuildOKHandler {
public:
    // execute packet's handler
    static void execute(SGDeleteGuildOK* pPacket);
};

#endif
