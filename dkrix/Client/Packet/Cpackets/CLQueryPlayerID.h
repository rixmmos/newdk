//--------------------------------------------------------------------------------
// 
// Filename    : CLQueryPlayerID.h 
// Written By  : Reiot
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __CL_QUERY_PLAYER_ID_H__
#define __CL_QUERY_PLAYER_ID_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CLQueryPlayerID;
//


//
//--------------------------------------------------------------------------------

class CLQueryPlayerID : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_QUERY_PLAYER_ID; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szBYTE + m_PlayerID.size(); 
	}

#ifdef __DEBUG_OUTPUT__	
	// get packet name
	std::string getPacketName () const throw () { return "CLQueryPlayerID"; }
	
	// get packet's debug std::string
	std::string toString () const throw ();
#endif

public :

	// get/set player's id
	std::string getPlayerID () const throw () { return m_PlayerID; }
	void setPlayerID ( const std::string & playerID ) throw () { m_PlayerID = playerID; }

private :

	
	std::string m_PlayerID;

};


//--------------------------------------------------------------------------------
//
// class CLQueryPlayerIDFactory;
//
// Factory for CLQueryPlayerID
//
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
class CLQueryPlayerIDFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CLQueryPlayerID(); }

	// get packet name
	std::string getPacketName () const throw () { return "CLQueryPlayerID"; }

	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_QUERY_PLAYER_ID; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		return szBYTE + 20; 
	}

};

#endif


//--------------------------------------------------------------------------------
//
// class CLQueryPlayerIDHandler;
//
//--------------------------------------------------------------------------------
#ifndef __GAME_CLIENT__
	class CLQueryPlayerIDHandler {

	public :

		// execute packet's handler
		static void execute ( CLQueryPlayerID * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
