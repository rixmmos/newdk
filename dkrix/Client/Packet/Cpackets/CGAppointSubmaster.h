//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAppointSubmaster.h 
// Written By  :
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_APPOINT_SUBMASTER_H__
#define __CG_APPOINT_SUBMASTER_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGAppointSubmaster;
//
//////////////////////////////////////////////////////////////////////

class CGAppointSubmaster : public Packet
{
public:
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_APPOINT_SUBMASTER; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szGuildID + szBYTE + m_Name.size(); }
#ifdef __DEBUG_OUTPUT__
	// get packet name
	string getPacketName() const throw() { return "CGAppointSubmaster"; }

	// get packet's debug string
	string toString() const throw();
#endif
	// get/set GuildID
	GuildID_t getGuildID() const throw() { return m_GuildID; }
	void setGuildID( GuildID_t guildID ) throw() { m_GuildID = guildID; }

	// get/set name
	const string& getName() const throw() { return m_Name; }
	void setName( const string& name ) throw() { m_Name = name; }


private :

	// GuildID
	GuildID_t m_GuildID;

	// name
	string m_Name;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAppointSubmasterFactory;
//
// Factory for CGAppointSubmaster
//
//////////////////////////////////////////////////////////////////////

class CGAppointSubmasterFactory : public PacketFactory {

public:
	
	// constructor
	CGAppointSubmasterFactory() throw() {}
	
	// destructor
	virtual ~CGAppointSubmasterFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGAppointSubmaster(); }

	// get packet name
	string getPacketName() const throw() { return "CGAppointSubmaster"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_APPOINT_SUBMASTER; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szGuildID + szBYTE + 20; }
};


//////////////////////////////////////////////////////////////////////
//
// class CGAppointSubmasterHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGAppointSubmasterHandler {

public:

	// execute packet's handler
	static void execute(CGAppointSubmaster* pCGAppointSubmaster, Player* pPlayer) throw ( ProtocolException , Error );

};
#endif
#endif
