//////////////////////////////////////////////////////////////////////
// 
// Filename    : CLLogout.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CL_LOGOUT_H__
#define __CL_LOGOUT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLLogout;
//



//
//////////////////////////////////////////////////////////////////////

class CLLogout : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CL_LOGOUT; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return 0; }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName () const throw () { return "CLLogout"; }
	
	// get packet's debug string
	std::string toString () const throw () { return "CLLogout"; }
#endif

};


//////////////////////////////////////////////////////////////////////
//
// class CLLogoutFactory;
//
// Factory for CLLogout
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CLLogoutFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CLLogout(); }

	// get packet name
	std::string getPacketName () const throw () { return "CLLogout"; }

	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_LOGOUT; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return 0; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CLLogoutHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CLLogoutHandler {

	public :

		// execute packet's handler
		static void execute ( CLLogout * pPacket , Player * player ) throw ( ProtocolException , Error );

	};
#endif

#endif
