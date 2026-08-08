//----------------------------------------------------------------------
// 
// Filename    : RCCharacterInfo.h 
// Written By  : Reiot
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __RC_CHARACTER_INFO_H__
#define __RC_CHARACTER_INFO_H__

// include files
#include "../DatagramPacket.h"
#include "../PacketFactory.h"
#include "../Types/GuildTypes.h"


//----------------------------------------------------------------------
//
// class RCCharacterInfo;
//

//
//----------------------------------------------------------------------

class RCCharacterInfo : public DatagramPacket {

public :
	RCCharacterInfo();
	
    
    void read ( Datagram & iDatagram ) throw ( ProtocolException , Error );
		    
    
    void write ( Datagram & oDatagram ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_RC_CHARACTER_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szBYTE + m_Name.size() + szGuildID;
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "RCCharacterInfo"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get /set GuildID
	GuildID_t getGuildID() const throw() { return m_GuildID; }
	void setGuildID( GuildID_t guildID ) throw() { m_GuildID = guildID; }


	// get/set chatting Name
	std::string getName () const throw () { return m_Name; }
	void setName ( const std::string & name ) throw () { m_Name = name; }

protected :
	std::string		m_Name;

	GuildID_t	m_GuildID;
};


//////////////////////////////////////////////////////////////////////
//
// class RCCharacterInfoFactory;
//
// Factory for RCCharacterInfo
//
//////////////////////////////////////////////////////////////////////

class RCCharacterInfoFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new RCCharacterInfo(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "RCCharacterInfo"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_RC_CHARACTER_INFO; }

	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		return szBYTE + 20 + szGuildID;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class RCCharacterInfoHandler;
//
//////////////////////////////////////////////////////////////////////

class RCCharacterInfoHandler {
	
public :

	// execute packet's handler
	static void execute ( RCCharacterInfo * pPacket ) throw ( ProtocolException , Error );

};

#endif
