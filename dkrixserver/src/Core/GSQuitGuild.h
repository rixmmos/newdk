//----------------------------------------------------------------------
//
// Filename    : GSQuitGuild.h
// Written By  :
// Description :
//
//----------------------------------------------------------------------

#ifndef __GS_QUIT_GUILD_H__
#define __GS_QUIT_GUILD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class GSQuitGuild;
//

//
// *CAUTION*
//
//----------------------------------------------------------------------

class GSQuitGuild : public Packet {
public:
    GSQuitGuild() {};
    ~GSQuitGuild() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GS_QUIT_GUILD;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID +    // Guild ID
               szBYTE +       // name length
               m_Name.size(); // name
    }

    // get packet name
    string getPacketName() const {
        return "GSQuitGuild";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set Guild ID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t guildID) {
        m_GuildID = guildID;
    }

    // get/set Name
    const string& getName() const {
        return m_Name;
    }
    void setName(const string& name) {
        m_Name = name;
    }

private:
    // Guild ID
    GuildID_t m_GuildID;

    // name
    string m_Name;
};


//////////////////////////////////////////////////////////////////////
//
// class GSQuitGuildFactory;
//
// Factory for GSQuitGuild
//
//////////////////////////////////////////////////////////////////////

class GSQuitGuildFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GSQuitGuild();
    }

    // get packet name
    string getPacketName() const {
        return "GSQuitGuild";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GS_QUIT_GUILD;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szGuildID + // guild ID
               szBYTE +    // name length
               20;         // name max length
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GSQuitGuildHandler {
public:
    // execute packet's handler
    static void execute(GSQuitGuild* pPacket, Player* pPlayer);
};

#endif
