//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCPhoneConnectionFailed.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_PHONE_CONNECTION_FAILED_H__
#define __GC_PHONE_CONNECTION_FAILED_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectionFailed;
//


//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectionFailed : public Packet {

public :
	
	// constructor
	GCPhoneConnectionFailed () throw ();
	
	// destructor
	~GCPhoneConnectionFailed () throw ();

	
public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_PHONE_CONNECTION_FAILED; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return 0; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCPhoneConnectionFailed"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

private :

};

//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectionFailedFactory;
//
// Factory for GCPhoneConnectionFailed
//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectionFailedFactory : public PacketFactory {

public :
	
	// constructor
	GCPhoneConnectionFailedFactory () throw () {}
	
	// destructor
	virtual ~GCPhoneConnectionFailedFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCPhoneConnectionFailed(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCPhoneConnectionFailed"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_PHONE_CONNECTION_FAILED; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return 0; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectionFailedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectionFailedHandler {

public :

	// execute packet's handler
	static void execute ( GCPhoneConnectionFailed * pGCPhoneConnectionFailed , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
