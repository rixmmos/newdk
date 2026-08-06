//////////////////////////////////////////////////////////////////////
// 
// Filename    : GuildInfo.h 
// Written By  : 
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GUILD_INFO_H__
#define __GUILD_INFO_H__

// include files
#include "Packet/Types.h"
#include "Packet/Exception.h"
#include "Packet/Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class GuildInfo;
//

//
//////////////////////////////////////////////////////////////////////

class GuildInfo {

public :
	
	// constructor
	GuildInfo () throw ();
	
	// destructor
	~GuildInfo () throw ();

public :
	
	
    
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
		    
    
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	// get packet's body size
	
	PacketSize_t getSize () throw ();

	static uint getMaxSize() throw() {
		//return ( szGuildID + szBYTE + 30 + szBYTE + 20 + szBYTE + szBYTE + 11 ) * 256 + szBYTE;
		return szGuildID + szBYTE + 30 + szBYTE + 20 + szBYTE + szBYTE + 11;
	}

	// get packet's debug string
	std::string toString () const throw ();

	// get/set GuildID
	GuildID_t getGuildID() const throw() { return m_GuildID; }
	void setGuildID( GuildID_t GuildID ) throw() { m_GuildID = GuildID; }

	// get/set Guild Name
	std::string getGuildName() const throw() { return m_GuildName; }
	void setGuildName( const std::string& GuildName ) throw() { m_GuildName = GuildName; }

	// get/set Guild Master
	std::string getGuildMaster() const throw() { return m_GuildMaster; }
	void setGuildMaster( const std::string& GuildMaster ) throw() { m_GuildMaster = GuildMaster; }

	// get/set Guild Member Count
	BYTE getGuildMemberCount() const throw() { return m_GuildMemberCount; }
	void setGuildMemberCount( BYTE GuildMemberCount ) throw() { m_GuildMemberCount = GuildMemberCount; }

	// get/set Guild Expire Date
	std::string getGuildExpireDate() const throw() { return m_GuildExpireDate; }
	void setGuildExpireDate( const std::string& GuildExpireDate ) throw() { m_GuildExpireDate = GuildExpireDate; }


private :

	
	GuildID_t m_GuildID;

	
	std::string m_GuildName;

	
	std::string m_GuildMaster;

	
	BYTE m_GuildMemberCount;

	
	std::string m_GuildExpireDate;

};

#endif
