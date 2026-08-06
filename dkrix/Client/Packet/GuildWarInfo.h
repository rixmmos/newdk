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

public :
	typedef ValueList<GuildID_t>	GuildIDList;

public :
	GuildWarInfo() {}
	~GuildWarInfo() {}
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	PacketSize_t getSize() const throw() 
	{ 
		return WarInfo::getSize() 
				+ szZoneID 
				+ szBYTE + m_AttackGuildName.size() 
				+ szBYTE + m_DefenseGuildName.size() 
				+ m_GuildIDs.getPacketSize();
	}

	static PacketSize_t getMaxSize() throw() 
	{ 
		return WarInfo::getMaxSize() 
				+ szZoneID 
				+ szBYTE + 40
				+ szBYTE + 30
				+ GuildIDList::getPacketMaxSize();
	}

	// get packet's debug std::string
	std::string toString() const throw();

public :
	WarType_t 			getWarType() const 			{ return WAR_GUILD; }

	ZoneID_t			getCastleID() const			{ return m_CastleID; }
	void 				setCastleID(ZoneID_t zid) 	{ m_CastleID = zid; }

	const std::string&		getAttackGuildName() const						{ return m_AttackGuildName; }
	void 				setAttackGuildName(const std::string& guildName) 	{ m_AttackGuildName = guildName; }

	const std::string&		getDefenseGuildName() const						{ return m_DefenseGuildName; }
	void 				setDefenseGuildName(const std::string& guildName) 	{ m_DefenseGuildName = guildName; }

	GuildIDList& 		getJoinGuilds()				{ return m_GuildIDs; }
	void 				addJoinGuild(GuildID_t gid)	{ m_GuildIDs.addValue(gid); }

private :
	ZoneID_t				m_CastleID;				
	std::string					m_DefenseGuildName;		
	std::string					m_AttackGuildName;		
	GuildIDList				m_GuildIDs;				
};

#endif
