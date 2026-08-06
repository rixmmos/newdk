//----------------------------------------------------------------------
//
// Filename    : SGModifyGuildIntroOK.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __SG_MODIFY_GUILD_INTRO_OK_H__
#define __SG_MODIFY_GUILD_INTRO_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class SGModifyGuildIntroOK;
//

//
// *CAUTION*
//
//----------------------------------------------------------------------

class SGModifyGuildIntroOK : public Packet {
public:
    SGModifyGuildIntroOK() {};
    ~SGModifyGuildIntroOK() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_SG_MODIFY_GUILD_INTRO_OK;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID +          // guild ID
               szBYTE +             // Guild Intro length
               m_GuildIntro.size(); // Guild Intro
    }

    // get packet name
    string getPacketName() const {
        return "SGModifyGuildIntroOK";
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

    // get/set guild intro
    const string& getGuildIntro() const {
        return m_GuildIntro;
    }
    void setGuildIntro(const string& intro) {
        m_GuildIntro = intro;
    }

private:
    // GuildID
    GuildID_t m_GuildID;

    // Guild Intro
    string m_GuildIntro;
};


//////////////////////////////////////////////////////////////////////
//
// class SGModifyGuildIntroOKFactory;
//
// Factory for SGModifyGuildIntroOK
//
//////////////////////////////////////////////////////////////////////

class SGModifyGuildIntroOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new SGModifyGuildIntroOK();
    }

    // get packet name
    string getPacketName() const {
        return "SGModifyGuildIntroOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_SG_MODIFY_GUILD_INTRO_OK;
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

class SGModifyGuildIntroOKHandler {
public:
    // execute packet's handler
    static void execute(SGModifyGuildIntroOK* pPacket);
};

#endif
