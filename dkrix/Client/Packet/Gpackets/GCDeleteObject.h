//--------------------------------------------------------------------------------
// 
// Filename    : GCDeleteObject.h 
// Written By  : Reiot
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __GC_DELETE_OBJECT_H__
#define __GC_DELETE_OBJECT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCDeleteObject;
//


//

//




//
// *CAUTION*
//


//
//--------------------------------------------------------------------------------

class GCDeleteObject : public Packet {

public :

	// constructor
	GCDeleteObject () throw ()
	{
	}

	GCDeleteObject ( ObjectID_t objectID ) throw ()
		: m_ObjectID(objectID)
	{
	}


public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_DELETE_OBJECT; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szObjectID; 
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCDeleteObject"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set object id
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( ObjectID_t objectID ) throw () { m_ObjectID = objectID; }

private :

	// object id
	ObjectID_t m_ObjectID;

};


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteObjectFactory;
//
// Factory for GCDeleteObject
//
//////////////////////////////////////////////////////////////////////

class GCDeleteObjectFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCDeleteObject(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCDeleteObject"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_DELETE_OBJECT; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		return szObjectID; 
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteObjectHandler;
//
//////////////////////////////////////////////////////////////////////

class GCDeleteObjectHandler {

public :

	// execute packet's handler
	static void execute ( GCDeleteObject * pPacket , Player * player ) throw ( ProtocolException , Error );

};

#endif
