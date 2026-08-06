//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCReconnectLogin.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_RECONNECT_LOGIN_H__
#define __GC_RECONNECT_LOGIN_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCReconnectLogin;
//



//
//////////////////////////////////////////////////////////////////////

class GCReconnectLogin : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_RECONNECT_LOGIN; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szBYTE + m_LoginServerIP.size() 	
			+ szuint							
			+ szDWORD; 							
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCReconnectLogin"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set game server's ip
	std::string getLoginServerIP () const throw () { return m_LoginServerIP; }
	void setLoginServerIP ( const std::string & ip ) throw () { m_LoginServerIP = ip; }

	// get/set game server's port
	uint getLoginServerPort () const throw () { return m_LoginServerPort; }
	void setLoginServerPort ( uint port ) throw () { m_LoginServerPort = port; }

	// get/set key
	DWORD getKey () const throw () { return m_Key; }
	void setKey ( DWORD key ) throw () { m_Key = key; }

private :
	
	// New LoginServer's IP
	std::string m_LoginServerIP;

	// New LoginServer's TCP Port
	uint m_LoginServerPort;

	// authentication key
	DWORD m_Key;

};


//////////////////////////////////////////////////////////////////////
//
// class GCReconnectLoginFactory;
//
// Factory for GCReconnectLogin
//
//////////////////////////////////////////////////////////////////////

class GCReconnectLoginFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCReconnectLogin(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCReconnectLogin"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_RECONNECT_LOGIN; }

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
// class GCReconnectLoginHandler;
//
//////////////////////////////////////////////////////////////////////

class GCReconnectLoginHandler {

public :

	// execute packet's handler
	static void execute ( GCReconnectLogin * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
