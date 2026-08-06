//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCGetOffMotorCycleFailed.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_GET_OFF_MOTORCYCLE_FAILED_H__
#define __GC_GET_OFF_MOTORCYCLE_FAILED_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleFailed;
//

//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleFailed : public Packet {

public :
	
	// constructor
	GCGetOffMotorCycleFailed () throw ();
	
	// destructor
	~GCGetOffMotorCycleFailed () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_GET_OFF_MOTORCYCLE_FAILED; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return 0; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCGetOffMotorCycleFailed"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

private :
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleFailedFactory;
//
// Factory for GCGetOffMotorCycleFailed
//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleFailedFactory : public PacketFactory {

public :
	
	// constructor
	GCGetOffMotorCycleFailedFactory () throw () {}
	
	// destructor
	virtual ~GCGetOffMotorCycleFailedFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCGetOffMotorCycleFailed(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCGetOffMotorCycleFailed"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_GET_OFF_MOTORCYCLE_FAILED; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return 0; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleFailedHandler;
//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleFailedHandler {

public :

	// execute packet's handler
	static void execute ( GCGetOffMotorCycleFailed * pGCGetOffMotorCycleFailed , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
