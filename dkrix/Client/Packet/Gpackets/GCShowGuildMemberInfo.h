//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCShowGuildMemberInfo.h 
// Written By  : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SHOW_GUILD_MEMBER_INFO_H__
#define __GC_SHOW_GUILD_MEMBER_INFO_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCShowGuildMemberInfo;
//

//
//////////////////////////////////////////////////////////////////////

class GCShowGuildMemberInfo : public Packet {

public :
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SHOW_GUILD_MEMBER_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw()
	{ 
		return szGuildID +					// Guild ID
			   szBYTE +						// Name length
			   m_Name.size() +				// Name
			   szGuildMemberRank +			// Guild Member Rank
			   szBYTE +						// Guild Member Intro length
			   m_GuildMemberIntro.size();	// Guild Member Intro
	}

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCShowGuildMemberInfo"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif
	// get/set Guild ID
	GuildID_t getGuildID() const throw() { return m_GuildID; }
	void setGuildID( GuildID_t GuildID ) throw() { m_GuildID = GuildID; }

	// get/set Name
	const std::string& getName() const throw() { return m_Name; }
	void setName( const std::string& name ) throw() { m_Name = name; }

	// get/set Guild Member Rank
	GuildMemberRank_t getGuildMemberRank() const throw() { return m_GuildMemberRank; }
	void setGuildMemberRank( GuildMemberRank_t guildMemberRank ) throw() { m_GuildMemberRank = guildMemberRank; }

	// get/set Guild Member Intro
	const std::string& getGuildMemberIntro() const throw() { return m_GuildMemberIntro; }
	void setGuildMemberIntro( const std::string& intro ) throw() { m_GuildMemberIntro = intro; }

private :
	
	// Guild ID
	GuildID_t m_GuildID;

	// Name
	std::string m_Name;

	// Guild Member Rank
	GuildMemberRank_t m_GuildMemberRank;

	// Guild Member Intro
	std::string m_GuildMemberIntro;

};

//////////////////////////////////////////////////////////////////////
//
// class GCShowGuildMemberInfoFactory;
//
// Factory for GCShowGuildMemberInfo
//
//////////////////////////////////////////////////////////////////////

class GCShowGuildMemberInfoFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCShowGuildMemberInfo(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCShowGuildMemberInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SHOW_GUILD_MEMBER_INFO; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw()
	{
		return szGuildID +					// Guild ID
			   szBYTE +						// Name length
			   20 +							// Name max size
			   szGuildMemberRank +			// Guild Member Rank
			   szBYTE +						// Guild Member Intro length
			   256;							// Guild Member Intro max size
	}

};

//////////////////////////////////////////////////////////////////////
//
// class GCShowGuildMemberInfo;
//
//////////////////////////////////////////////////////////////////////

class GCShowGuildMemberInfoHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCShowGuildMemberInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
