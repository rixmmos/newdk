//////////////////////////////////////////////////////////////////////
// 
// Filename    : CLLogin.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CL_LOGIN_H__
#define __CL_LOGIN_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLLogin;
//


//
//////////////////////////////////////////////////////////////////////

class CLLogin : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CL_LOGIN; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw ();// { return szBYTE + m_ID.size() + szBYTE + m_Password.size(); }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName () const throw () { return "CLLogin"; }
	
	// get packet's debug string
	std::string toString () const throw ();
#endif

public :

	// get/set player's id
	std::string getID () const throw () { return m_ID; }
	void setID ( std::string id ) throw () { m_ID = id; }

	// get/set player's password
	std::string getPassword () const throw () { return m_Password; }
	void setPassword ( std::string password ) throw () { m_Password = password; }

	const BYTE* getMacAddress() const throw () { return m_MacAddress; }
	void setMacAddress( const BYTE* macAddress ) throw () { memcpy( m_MacAddress, macAddress, 6 * sizeof(BYTE) ); }

	void SetLoginMode(BYTE n) { m_LoginMode = n;}
private :

	
	std::string m_ID;

	
	std::string m_Password;

	// Mac address
	BYTE m_MacAddress[6];

	// Login mode
	BYTE m_LoginMode;

};


//////////////////////////////////////////////////////////////////////
//
// class CLLoginFactory;
//
// Factory for CLLogin
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CLLoginFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CLLogin(); }

	// get packet name
	std::string getPacketName () const throw () { return "CLLogin"; }

	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_LOGIN; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE + 30 + szBYTE + 20 + 6 + szBYTE; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CLLoginHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CLLoginHandler {

	public :

		// execute packet's handler
		static void execute ( CLLogin * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
