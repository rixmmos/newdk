//--------------------------------------------------------------------------------
//
// Filename    : WarInfo.h
// Written By  :
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __GUILD_WAR_LIST_H__
#define __GUILD_WAR_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "WarInfo.h"

//--------------------------------------------------------------------------------
//
// class WarInfo;
//

//
//--------------------------------------------------------------------------------

class GuildWarInfo : public WarInfo {
public:
    typedef ValueList<GuildID_t> GuildIDList;

public:
    GuildWarInfo() {}
    ~GuildWarInfo() {}

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    PacketSize_t getSize() const {
        return WarInfo::getSize() + szZoneID + szBYTE + m_AttackGuildName.size() + szBYTE + m_DefenseGuildName.size() +
               m_GuildIDs.getPacketSize();
    }

    static PacketSize_t getMaxSize() {
        return WarInfo::getMaxSize() + szZoneID + szBYTE + 40 + szBYTE + 30 + GuildIDList::getPacketMaxSize();
    }

    // get packet's debug string
    string toString() const;

    void operator=(const GuildWarInfo& GWI) {
        m_StartTime = GWI.m_StartTime;
        m_RemainTime = GWI.m_RemainTime;
        m_CastleID = GWI.m_CastleID;
        m_DefenseGuildName = GWI.m_DefenseGuildName;
        m_AttackGuildName = GWI.m_AttackGuildName;
        m_GuildIDs = GWI.m_GuildIDs;
    }

public:
    WarType_t getWarType() const {
        return WAR_GUILD;
    }

    ZoneID_t getCastleID() const {
        return m_CastleID;
    }
    void setCastleID(ZoneID_t zid) {
        m_CastleID = zid;
    }

    const string& getAttackGuildName() const {
        return m_AttackGuildName;
    }
    void setAttackGuildName(const string& guildName) {
        m_AttackGuildName = guildName;
    }

    const string& getDefenseGuildName() const {
        return m_DefenseGuildName;
    }
    void setDefenseGuildName(const string& guildName) {
        m_DefenseGuildName = guildName;
    }

    GuildIDList& getJoinGuilds() {
        return m_GuildIDs;
    }
    void addJoinGuild(GuildID_t gid) {
        m_GuildIDs.addValue(gid);
    }

private:
    ZoneID_t m_CastleID;       
    string m_DefenseGuildName; 
    string m_AttackGuildName;  
    GuildIDList m_GuildIDs;    
};

#endif
