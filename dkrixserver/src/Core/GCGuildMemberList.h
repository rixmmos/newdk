//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGuildMemberList.h
// Written By  :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_GUILD_MEMBER_LIST_H__
#define __GC_GUILD_MEMBER_LIST_H__

// include files
#include <list>

#include "GuildMemberInfo.h"
#include "Packet.h"
#include "PacketFactory.h"

typedef list<GuildMemberInfo*> GuildMemberInfoList;
typedef list<GuildMemberInfo*>::const_iterator GuildMemberInfoListConstItor;


//////////////////////////////////////////////////////////////////////
//
// class GCGuildMemberList;
//

//
//////////////////////////////////////////////////////////////////////

class GCGuildMemberList : public Packet {
public:
    // constructor
    GCGuildMemberList();

    // destructor
    ~GCGuildMemberList();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_GUILD_MEMBER_LIST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const;

    // get packet name
    string getPacketName() const {
        return "GCGuildMemberList";
    }

    // get packet's debug string
    string toString() const;

public:
    BYTE getListNum() const {
        return m_GuildMemberInfoList.size();
    }

    // add GuildMemberInfoList
    void addGuildMemberInfo(GuildMemberInfo* pGuildMemberInfo) {
        m_GuildMemberInfoList.push_front(pGuildMemberInfo);
    }

    // clear GuildMemberInfoList
    void clearGuildMemberInfoList();

    // pop front Element in GuildMemberInfoList
    GuildMemberInfo* popFrontGuildMemberInfoList() {
        if (!m_GuildMemberInfoList.empty()) {
            GuildMemberInfo* pGuildMemberInfo = m_GuildMemberInfoList.front();
            m_GuildMemberInfoList.pop_front();
            return pGuildMemberInfo;
        }
        return NULL;
    }

    BYTE getType() const {
        return m_Type;
    }
    void setType(BYTE type) {
        m_Type = type;
    }

private:
    BYTE m_Type;

    GuildMemberInfoList m_GuildMemberInfoList;
};


//////////////////////////////////////////////////////////////////////
//
// class GCGuildMemberListFactory;
//
// Factory for GCGuildMemberList
//
//////////////////////////////////////////////////////////////////////

class GCGuildMemberListFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCGuildMemberList();
    }

    // get packet name
    string getPacketName() const {
        return "GCGuildMemberList";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_GUILD_MEMBER_LIST;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + GuildMemberInfo::getMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCGuildMemberList;
//
//////////////////////////////////////////////////////////////////////

class GCGuildMemberListHandler {
public:
    // execute packet's handler
    static void execute(GCGuildMemberList* pPacket, Player* pPlayer);
};

#endif
