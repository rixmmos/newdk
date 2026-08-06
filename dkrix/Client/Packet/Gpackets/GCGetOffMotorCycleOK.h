//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCGetOffMotorCycleOK.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_GET_OFF_MOTORCYCLE_OK_H__
#define __GC_GET_OFF_MOTORCYCLE_OK_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleOK;
//

//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleOK : public Packet {

public :
	
	// constructor
	GCGetOffMotorCycleOK () throw ();
	
	// destructor
	~GCGetOffMotorCycleOK () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_GET_OFF_MOTORCYCLE_OK; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return 0; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCGetOffMotorCycleOK"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

private :
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleOKFactory;
//
// Factory for GCGetOffMotorCycleOK
//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleOKFactory : public PacketFactory {

public :
	
	// constructor
	GCGetOffMotorCycleOKFactory () throw () {}
	
	// destructor
	virtual ~GCGetOffMotorCycleOKFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCGetOffMotorCycleOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCGetOffMotorCycleOK"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_GET_OFF_MOTORCYCLE_OK; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return 0; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleOKHandler;
//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleOKHandler {

public :

	// execute packet's handler
	static void execute ( GCGetOffMotorCycleOK * pGCGetOffMotorCycleOK , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
