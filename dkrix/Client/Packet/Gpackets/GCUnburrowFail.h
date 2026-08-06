//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCUnburrowFail.h 
// Written By  : crazydog
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_UNBURROW_FAIL_H__
#define __GC_UNBURROW_FAIL_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCUnburrowFail;
//
//////////////////////////////////////////////////////////////////////

class GCUnburrowFail : public Packet {

public :

	// constructor
	GCUnburrowFail () throw () {}
	

public :

    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_UNBURROW_FAIL; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return 0; }
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCUnburrowFail"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	

public :


private : 


};


//////////////////////////////////////////////////////////////////////
//
// class  GCUnburrowFailFactory;
//
// Factory for  GCUnburrowFail
//
//////////////////////////////////////////////////////////////////////

class  GCUnburrowFailFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCUnburrowFail(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCUnburrowFail"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_UNBURROW_FAIL; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return 0; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class  GCUnburrowFailHandler;
//
//////////////////////////////////////////////////////////////////////

class  GCUnburrowFailHandler {

public :

	// execute packet's handler
	static void execute ( GCUnburrowFail * pPacket , Player * pPlayer ) throw ( Error);

};

#endif
