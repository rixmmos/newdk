//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCOtherGuildName.h 
// Written By  : Reiot
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_OTHER_GUILD_NAME_H__
#define __GC_OTHER_GUILD_NAME_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCOtherGuildName;
//



//
//////////////////////////////////////////////////////////////////////

class GCOtherGuildName : public Packet {

public :
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_OTHER_GUILD_NAME; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szObjectID + szGuildID + szBYTE + m_GuildName.size(); }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCOtherGuildName"; }
	
	// get packet's debug string
	std::string toString() const throw();
#endif

	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t objectID ) throw() { m_ObjectID = objectID; }

	// get/set Guild ID
	GuildID_t getGuildID() const throw() { return m_GuildID; }
	void setGuildID( GuildID_t guildID ) throw() { m_GuildID = guildID; }

	// get/set Guild Name
	std::string getGuildName() const throw() { return m_GuildName; }
	void setGuildName(const std::string& guildName) throw() { m_GuildName = guildName; }

private :

	ObjectID_t m_ObjectID;

	// Guild ID
	GuildID_t m_GuildID;

	// Guild Name
	std::string m_GuildName;

};


//////////////////////////////////////////////////////////////////////
//
// class GCOtherGuildNameFactory;
//
// Factory for GCOtherGuildName
//
//////////////////////////////////////////////////////////////////////

class GCOtherGuildNameFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCOtherGuildName(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCOtherGuildName"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_OTHER_GUILD_NAME; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szGuildID + szBYTE + 30; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCOtherGuildNameHandler;
//
//////////////////////////////////////////////////////////////////////

class GCOtherGuildNameHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCOtherGuildName* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
