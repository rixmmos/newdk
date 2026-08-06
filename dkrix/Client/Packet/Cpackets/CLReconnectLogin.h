//////////////////////////////////////////////////////////////////////
// 
// Filename    : CLReconnectLogin.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CL_RECONNECT_LOGIN_H__
#define __CL_RECONNECT_LOGIN_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLReconnectLogin;
//



//
//////////////////////////////////////////////////////////////////////

class CLReconnectLogin : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CL_RECONNECT_LOGIN; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szDWORD+szBYTE; 						// authentication key
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CLReconnectLogin"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set key
	DWORD getKey () const throw () { return m_Key; }
	void setKey ( DWORD key ) throw () { m_Key = key; }

	void SetLoginMode(BYTE n) { m_LoginMode = n;}
	BYTE GetLoginMode() { return m_LoginMode;}
private :
	
	// authentication key
	DWORD m_Key;
	// Login mode
	BYTE m_LoginMode;

};


//////////////////////////////////////////////////////////////////////
//
// class CLReconnectLoginFactory;
//
// Factory for CLReconnectLogin
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CLReconnectLoginFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CLReconnectLogin(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CLReconnectLogin"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_RECONNECT_LOGIN; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw ()
	{ 
		return szDWORD+szBYTE; 			// authentication key
	}

};

#endif

//////////////////////////////////////////////////////////////////////
//
// class CLReconnectLoginHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CLReconnectLoginHandler {

	public :

		// execute packet's handler
		static void execute ( CLReconnectLogin * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
