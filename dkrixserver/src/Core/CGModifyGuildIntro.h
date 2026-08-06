//////////////////////////////////////////////////////////////////////
//
// Filename    : CGModifyGuildIntro.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_MODIFY_GUILD_INTRO_H__
#define __CG_MODIFY_GUILD_INTRO_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildIntro;
//
//////////////////////////////////////////////////////////////////////

class CGModifyGuildIntro : public Packet {
public:
    CGModifyGuildIntro() {};
    ~CGModifyGuildIntro() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_MODIFY_GUILD_INTRO;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID + szBYTE + m_GuildIntro.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGModifyGuildIntro";
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
// class CGModifyGuildIntroFactory;
//
// Factory for CGModifyGuildIntro
//
//////////////////////////////////////////////////////////////////////

class CGModifyGuildIntroFactory : public PacketFactory {
public:
    // constructor
    CGModifyGuildIntroFactory() {}

    // destructor
    virtual ~CGModifyGuildIntroFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new CGModifyGuildIntro();
    }

    // get packet name
    string getPacketName() const {
        return "CGModifyGuildIntro";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_MODIFY_GUILD_INTRO;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szGuildID + szBYTE + 255;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGModifyGuildIntroHandler;
//
//////////////////////////////////////////////////////////////////////

class CGModifyGuildIntroHandler {
public:
    // execute packet's handler
    static void execute(CGModifyGuildIntro* pCGModifyGuildIntro, Player* pPlayer);
};

#endif
