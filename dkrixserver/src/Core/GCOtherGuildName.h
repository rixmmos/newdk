//////////////////////////////////////////////////////////////////////
//
// Filename    : GCOtherGuildName.h
// Written By  : Reiot
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_OTHER_GUILD_NAME_H__
#define __GC_OTHER_GUILD_NAME_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCOtherGuildName;
//



//
//////////////////////////////////////////////////////////////////////

class GCOtherGuildName : public Packet {
public:
    GCOtherGuildName() {};
    ~GCOtherGuildName() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_OTHER_GUILD_NAME;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID + szGuildID + szBYTE + m_GuildName.size();
    }

    // get packet name
    string getPacketName() const {
        return "GCOtherGuildName";
    }

    // get packet's debug string
    string toString() const;

    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) {
        m_ObjectID = objectID;
    }

    // get/set Guild ID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t guildID) {
        m_GuildID = guildID;
    }

    // get/set Guild Name
    string getGuildName() const {
        return m_GuildName;
    }
    void setGuildName(const string& guildName) {
        m_GuildName = guildName;
    }

private:
    ObjectID_t m_ObjectID;

    // Guild ID
    GuildID_t m_GuildID;

    // Guild Name
    string m_GuildName;
};


//////////////////////////////////////////////////////////////////////
//
// class GCOtherGuildNameFactory;
//
// Factory for GCOtherGuildName
//
//////////////////////////////////////////////////////////////////////

class GCOtherGuildNameFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCOtherGuildName();
    }

    // get packet name
    string getPacketName() const {
        return "GCOtherGuildName";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_OTHER_GUILD_NAME;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szGuildID + szBYTE + 30;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCOtherGuildNameHandler;
//
//////////////////////////////////////////////////////////////////////

class GCOtherGuildNameHandler {
public:
    // execute packet's handler
    static void execute(GCOtherGuildName* pPacket, Player* pPlayer);
};

#endif
