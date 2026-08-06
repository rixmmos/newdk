//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCDeleteandPickUpOK.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_DELETE_AND_PICKUP_OK_H__
#define __GC_DELETE_AND_PICKUP_OK_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteandPickUpOK;
//
//////////////////////////////////////////////////////////////////////

class GCDeleteandPickUpOK : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_DELETE_AND_PICKUP_OK; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szObjectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCDeleteandPickUpOK"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	// get / set ObjectID
	ObjectID_t getObjectID() throw () { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

};


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteandPickUpOKFactory;
//
// Factory for GCDeleteandPickUpOK
//
//////////////////////////////////////////////////////////////////////

class GCDeleteandPickUpOKFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCDeleteandPickUpOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCDeleteandPickUpOK"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_DELETE_AND_PICKUP_OK; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteandPickUpOKHandler;
//
//////////////////////////////////////////////////////////////////////

class GCDeleteandPickUpOKHandler {
	
public :

	// execute packet's handler
	static void execute ( GCDeleteandPickUpOK * pPacket , Player * player ) throw ( ProtocolException , Error );
};

#endif
