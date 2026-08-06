
//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGRequestGuildList.h 
// Written By  :
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUER_GUILD_LIST_H__
#define __CG_REQUER_GUILD_LIST_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

typedef BYTE GuildType_t;


//////////////////////////////////////////////////////////////////////
//
// class CGRequestGuildList;
//
//////////////////////////////////////////////////////////////////////

class CGRequestGuildList : public Packet
{
public:
	enum
	{
		GUILDTYPE_WAIT,             		
		GUILDTYPE_NORMAL,                   
		GUILDTYPE_MAX
	};

	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_REQUEST_GUILD_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return sizeof(GuildType_t); }
#ifdef __DEBUG_OUTPUT__
	// get packet name
	string getPacketName() const throw() { return "CGRequestGuildList"; }

	// get packet's debug string
	string toString() const throw();
#endif
	// get/set GuildType
	GuildType_t	getGuildType()	const throw() {	return m_GuildType; }
	void		setGuildType( GuildType_t GuildType )	throw()	{ m_GuildType = GuildType; }
	
	GuildType_t	m_GuildType;
	

};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestGuildListFactory;
//
// Factory for CGRequestGuildList
//
//////////////////////////////////////////////////////////////////////

class CGRequestGuildListFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestGuildListFactory() throw() {}
	
	// destructor
	virtual ~CGRequestGuildListFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGRequestGuildList(); }

	// get packet name
	string getPacketName() const throw() { return "CGRequestGuildList"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REQUEST_GUILD_LIST; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return sizeof(GuildType_t); }
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestGuildListHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGRequestGuildListHandler {

public:

	// execute packet's handler
	static void execute(CGRequestGuildList* pCGRequestGuildList, Player* pPlayer) throw ( ProtocolException , Error );

};
#endif
#endif
