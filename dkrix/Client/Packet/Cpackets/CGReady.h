//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGReady.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_READY_H__
#define __CG_READY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGReady;
//
//////////////////////////////////////////////////////////////////////

class CGReady : public Packet {

public :

    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_READY; }
	
	// get packet body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return 0; }
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "CGReady"; }
		
		// get packet's debug std::string
		std::string toString () const throw () { return "CGReady"; }
	#endif
	
};


//////////////////////////////////////////////////////////////////////
//
// class CGReadyFactory;
//
// Factory for CGReady
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGReadyFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGReady(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGReady"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_READY; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return 0; }
	
};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGReadyHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGReadyHandler {

	public :

		// execute packet's handler
		static void execute ( CGReady * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
