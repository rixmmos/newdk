//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGConnect.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_CONNECT_H__
#define __CG_CONNECT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGConnect;
//



//
//////////////////////////////////////////////////////////////////////

class CGConnect : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_CONNECT; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szDWORD 						// authentication key
			+ szPCType 						// Slayer or Vampire?
			+ szBYTE + m_PCName.size()  	// name
			+ 6;
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGConnect"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set key
	DWORD getKey () const throw () { return m_Key; }
	void setKey ( DWORD key ) throw () { m_Key = key; }

	// get/set PCType
	PCType getPCType () const throw () { return m_PCType; }
	void setPCType ( PCType pcType ) throw () { m_PCType = pcType; }
	
	// get/set pc name
	std::string getPCName () const throw () { return m_PCName; }
	void setPCName ( std::string pcName ) throw () { m_PCName = pcName; }

	// get/set mac address
	const BYTE* getMacAddress() const throw () { return m_MacAddress; }
	void setMacAddress( const BYTE* macAddress ) throw () { memcpy( m_MacAddress, macAddress, 6 * sizeof(BYTE) ); }

private :
	
	// authentication key
	DWORD m_Key;

	// Slayer or Vampire?
	PCType m_PCType;

	
	std::string m_PCName;

	// Mac address
	BYTE m_MacAddress[6];
	
};


//////////////////////////////////////////////////////////////////////
//
// class CGConnectFactory;
//
// Factory for CGConnect
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGConnectFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGConnect(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGConnect"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_CONNECT; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw ()
	{ 
		return szDWORD 			// authentication key
			+ szPCType 			// Slayer or Vampire
			+ szBYTE + 20 		// name
			+6;				// MacAddress
	}

};

#endif

//////////////////////////////////////////////////////////////////////
//
// class CGConnectHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGConnectHandler {

	public :

		// execute packet's handler
		static void execute ( CGConnect * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
