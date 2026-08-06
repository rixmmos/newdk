//----------------------------------------------------------------------
//
// Filename    : GSModifyGuildIntro.h
// Written By  :
// Description :
//
//----------------------------------------------------------------------

#ifndef __GS_MODIFY_GUILD_INTRO_H__
#define __GS_MODIFY_GUILD_INTRO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class GSModifyGuildIntro;
//

//
// *CAUTION*
//
//----------------------------------------------------------------------

class GSModifyGuildIntro : public Packet {
public:
    GSModifyGuildIntro() {};
    ~GSModifyGuildIntro() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GS_MODIFY_GUILD_INTRO;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID +          // Guild ID
               szBYTE +             // Guild Intro length
               m_GuildIntro.size(); // Guild Intro
    }

    // get packet name
    string getPacketName() const {
        return "GSModifyGuildIntro";
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

    // get/set Guild Intro
    const string& getGuildIntro() const {
        return m_GuildIntro;
    }
    void setGuildIntro(const string& intro) {
        m_GuildIntro = intro;
    }

private:
    // Guild ID
    GuildID_t m_GuildID;

    // Guild Intro
    string m_GuildIntro;
};


//////////////////////////////////////////////////////////////////////
//
// class GSModifyGuildIntroFactory;
//
// Factory for GSModifyGuildIntro
//
//////////////////////////////////////////////////////////////////////

class GSModifyGuildIntroFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GSModifyGuildIntro();
    }

    // get packet name
    string getPacketName() const {
        return "GSModifyGuildIntro";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GS_MODIFY_GUILD_INTRO;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szGuildID + // guild ID
               szBYTE +    // Guild Intro length
               255;        // Guild Intro max length
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GSModifyGuildIntroHandler {
public:
    // execute packet's handler
    static void execute(GSModifyGuildIntro* pPacket, Player* pPlayer);
};

#endif
