//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShowGuildJoin.h
// Written By  :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SHOW_GUILD_JOIN_H__
#define __GC_SHOW_GUILD_JOIN_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCShowGuildJoin;
//

//
//////////////////////////////////////////////////////////////////////

class GCShowGuildJoin : public Packet {
public:
    GCShowGuildJoin() {};
    ~GCShowGuildJoin() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_SHOW_GUILD_JOIN;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGuildID + szBYTE + m_GuildName.size() + szGuildMemberRank + szGold;
    }

    // get packet name
    string getPacketName() const {
        return "GCShowGuildJoin";
    }

    // get packet's debug string
    string toString() const;

    // get/set Guild ID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t GuildID) {
        m_GuildID = GuildID;
    }

    // get/set Guild Name
    const string& getGuildName() const {
        return m_GuildName;
    }
    void setGuildName(const string& GuildName) {
        m_GuildName = GuildName;
    }

    // get/set Guild Member Rak
    GuildMemberRank_t getGuildMemberRank() const {
        return m_GuildMemberRank;
    }
    void setGuildMemberRank(GuildMemberRank_t GuildMemberRank) {
        m_GuildMemberRank = GuildMemberRank;
    }

    // get/set Join Fee
    Gold_t getJoinFee() const {
        return m_JoinFee;
    }
    void setJoinFee(Gold_t JoinFee) {
        m_JoinFee = JoinFee;
    }

private:
    // Guild ID
    GuildID_t m_GuildID;

    // Guild Name
    string m_GuildName;

    // Guild Member Rank
    GuildMemberRank_t m_GuildMemberRank;

    // Join Fee
    Gold_t m_JoinFee;
};


//////////////////////////////////////////////////////////////////////
//
// class GCShowGuildJoinFactory;
//
// Factory for GCShowGuildJoin
//
//////////////////////////////////////////////////////////////////////

class GCShowGuildJoinFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCShowGuildJoin();
    }

    // get packet name
    string getPacketName() const {
        return "GCShowGuildJoin";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_SHOW_GUILD_JOIN;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szGuildID + szBYTE + 30 + szGuildMemberRank + szGold;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCShowGuildJoin;
//
//////////////////////////////////////////////////////////////////////

class GCShowGuildJoinHandler {
public:
    // execute packet's handler
    static void execute(GCShowGuildJoin* pPacket, Player* pPlayer);
};

#endif
