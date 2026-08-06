//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCDeletePCOK.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_DELETE_PC_OK_H__
#define __LC_DELETE_PC_OK_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCOK;
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCOK : public Packet {

public :

    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_LC_DELETE_PC_OK; }
	
	// get packet body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return 0; }
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "LCDeletePCOK"; }
		
		// get packet's debug std::string
		std::string toString () const throw () { return "LCDeletePCOK"; }
	#endif
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCOKFactory;
//
// Factory for LCDeletePCOK
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCOKFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new LCDeletePCOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LCDeletePCOK"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_LC_DELETE_PC_OK; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return 0; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCOKHandler {

public :

	// execute packet's handler
	static void execute ( LCDeletePCOK * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
