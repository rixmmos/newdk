//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCRideMotorCycleOK.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_RIDE_MOTORCYCLE_OK_H__
#define __GC_RIDE_MOTORCYCLE_OK_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleOK;
//

//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleOK : public Packet {

public :
	
	// constructor
	GCRideMotorCycleOK () throw ();
	
	// destructor
	~GCRideMotorCycleOK () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_RIDE_MOTORCYCLE_OK; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCRideMotorCycleOK"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set ObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

};


//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleOKFactory;
//
// Factory for GCRideMotorCycleOK
//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleOKFactory : public PacketFactory {

public :
	
	// constructor
	GCRideMotorCycleOKFactory () throw () {}
	
	// destructor
	virtual ~GCRideMotorCycleOKFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCRideMotorCycleOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCRideMotorCycleOK"; }	
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_RIDE_MOTORCYCLE_OK; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleOKHandler;
//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleOKHandler {

public :

	// execute packet's handler
	static void execute ( GCRideMotorCycleOK * pGCRideMotorCycleOK , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
