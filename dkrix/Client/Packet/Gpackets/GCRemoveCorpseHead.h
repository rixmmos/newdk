//--------------------------------------------------------------------------------
// 
// Filename    : GCRemoveCorpseHead.h 
// Written By  : Reiot
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __GC_REMOVE_CORPSE_HEAD_H__
#define __GC_REMOVE_CORPSE_HEAD_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCRemoveCorpseHead;
//


//

//




//
// *CAUTION*
//


//
//--------------------------------------------------------------------------------

class GCRemoveCorpseHead : public Packet {

public :

	// constructor
	GCRemoveCorpseHead () throw ()
	{
	}

	GCRemoveCorpseHead ( ObjectID_t objectID ) throw ()
		: m_ObjectID(objectID)
	{
	}


public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_REMOVE_CORPSE_HEAD; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szObjectID; 
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCRemoveCorpseHead"; }
		
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
// class GCRemoveCorpseHeadFactory;
//
// Factory for GCRemoveCorpseHead
//
//////////////////////////////////////////////////////////////////////

class GCRemoveCorpseHeadFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCRemoveCorpseHead(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCRemoveCorpseHead"; }
	#endif	

	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_REMOVE_CORPSE_HEAD; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		return szObjectID; 
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveCorpseHeadHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRemoveCorpseHeadHandler {

public :

	// execute packet's handler
	static void execute ( GCRemoveCorpseHead * pPacket , Player * player ) throw ( ProtocolException , Error );

};

#endif
