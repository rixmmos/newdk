//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCReconnect.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_RECONNECT_H__
#define __LC_RECONNECT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCReconnect;
//



//
//////////////////////////////////////////////////////////////////////

class LCReconnect : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_LC_RECONNECT; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szBYTE + m_GameServerIP.size() 	
			+ szuint							
			+ szDWORD; 							
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LCReconnect"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set game server's ip
	std::string getGameServerIP () const throw () { return m_GameServerIP; }
	void setGameServerIP ( const std::string & ip ) throw () { m_GameServerIP = ip; }

	// get/set game server's port
	uint getGameServerPort () const throw () { return m_GameServerPort; }
	void setGameServerPort ( uint port ) throw () { m_GameServerPort = port; }

	// get/set key
	DWORD getKey () const throw () { return m_Key; }
	void setKey ( DWORD key ) throw () { m_Key = key; }

private :
	
	// New GameServer's IP
	std::string m_GameServerIP;

	// New GameServer's TCP Port
	uint m_GameServerPort;

	// authentication key
	DWORD m_Key;

};


//////////////////////////////////////////////////////////////////////
//
// class LCReconnectFactory;
//
// Factory for LCReconnect
//
//////////////////////////////////////////////////////////////////////

class LCReconnectFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new LCReconnect(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LCReconnect"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_LC_RECONNECT; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		return szBYTE + 15 	
			+ szuint		
			+ szDWORD; 		
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LCReconnectHandler;
//
//////////////////////////////////////////////////////////////////////

class LCReconnectHandler {

public :

	// execute packet's handler
	static void execute ( LCReconnect * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
