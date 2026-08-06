//----------------------------------------------------------------------
// 
// Filename    : LGIncomingConnection.h 
// Written By  : Reiot
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __LG_INCOMING_CONNECTION_H__
#define __LG_INCOMING_CONNECTION_H__

// include files
#include "../DatagramPacket.h"
#include "../PacketFactory.h"


//----------------------------------------------------------------------
//
// class LGIncomingConnection;
//



//
// *CAUTION*
//






//
//----------------------------------------------------------------------

class LGIncomingConnection : public DatagramPacket {

public :
	
    
    void read ( Datagram & iDatagram ) throw ( ProtocolException , Error );
		    
    
    void write ( Datagram & oDatagram ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_LG_INCOMING_CONNECTION; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return + szBYTE + m_PlayerID.size()	// Player ID
			+ szBYTE + m_PCName.size() 		// PC name
			+ szBYTE + m_ClientIP.size(); 	// client ip
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LGIncomingConnection"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set playerID
	std::string getPlayerID () const throw () { return m_PlayerID; }
	void setPlayerID ( std::string playerID ) throw () { m_PlayerID = playerID; }
	
	// get/set pcName
	std::string getPCName () const throw () { return m_PCName; }
	void setPCName ( std::string pcName ) throw () { m_PCName = pcName; }
	
	// get/set client ip
	std::string getClientIP () const throw () { return m_ClientIP; }
	void setClientIP ( std::string ip ) throw () { m_ClientIP = ip; }
	
private :

	// Player ID
	std::string m_PlayerID;

	// PC name
	std::string m_PCName;

	
	std::string m_ClientIP;

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionFactory;
//
// Factory for LGIncomingConnection
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new LGIncomingConnection(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LGIncomingConnection"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_LG_INCOMING_CONNECTION; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		return + szBYTE + 20 	// creature name
			+ szBYTE + 20 		// PC name
			+ szBYTE + 15; 		// client ip
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionHandler {
	
public :

	// execute packet's handler
	static void execute ( LGIncomingConnection * pPacket ) throw ( ProtocolException , Error );

};

#endif
