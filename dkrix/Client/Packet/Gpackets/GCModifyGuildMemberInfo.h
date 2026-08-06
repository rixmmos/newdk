//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCModifyGuildMemberInfo.h 
// Written By  : Reiot
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MODIFY_GUILD_MEMBER_INFO_H__
#define __GC_MODIFY_GUILD_MEMBER_INFO_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCModifyGuildMemberInfo;
//



//
//////////////////////////////////////////////////////////////////////

class GCModifyGuildMemberInfo : public Packet {

public :
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_MODIFY_GUILD_MEMBER_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szGuildID + szBYTE + m_GuildName.size() + szGuildMemberRank; }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCModifyGuildMemberInfo"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif

	// get/set Guild ID
	GuildID_t getGuildID() const throw() { return m_GuildID; }
	void setGuildID( GuildID_t guildID ) throw() { m_GuildID = guildID; }

	// get/set Guild Name
	std::string getGuildName() const throw() { return m_GuildName; }
	void setGuildName(const std::string& guildName) throw() { m_GuildName = guildName; }

	// get/set Guild ID
	GuildMemberRank_t getGuildMemberRank() const throw() { return m_GuildMemberRank; }
	void setGuildMemberRank( GuildMemberRank_t guildMemberRank ) throw() { m_GuildMemberRank = guildMemberRank; }

private :

	// Guild ID
	GuildID_t m_GuildID;

	// Guild Name
	std::string m_GuildName;

	// Guild Member Rank
	GuildMemberRank_t m_GuildMemberRank;
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCModifyGuildMemberInfoFactory;
//
// Factory for GCModifyGuildMemberInfo
//
//////////////////////////////////////////////////////////////////////

class GCModifyGuildMemberInfoFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCModifyGuildMemberInfo(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCModifyGuildMemberInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_MODIFY_GUILD_MEMBER_INFO; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szGuildID + szBYTE + 30 + szGuildMemberRank; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCModifyGuildMemberInfoHandler;
//
//////////////////////////////////////////////////////////////////////

class GCModifyGuildMemberInfoHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCModifyGuildMemberInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
