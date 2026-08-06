//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCThrowItemOK3.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_THROW_ITEM_OK_3_H__
#define __GC_THROW_ITEM_OK_3_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK3;
//

//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK3 : public Packet {

public :
	
	// constructor
	GCThrowItemOK3 () throw ();
	
	// destructor
	~GCThrowItemOK3 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_THROW_ITEM_OK_3; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szObjectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCThrowItemOK3"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set ObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get / set TargetObjectID
	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID( ObjectID_t TargetObjectID ) throw() { m_TargetObjectID = TargetObjectID; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	// TargetObjectID
	ObjectID_t m_TargetObjectID;

};


//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK3Factory;
//
// Factory for GCThrowItemOK3
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK3Factory : public PacketFactory {

public :
	
	// constructor
	GCThrowItemOK3Factory () throw () {}
	
	// destructor
	virtual ~GCThrowItemOK3Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCThrowItemOK3(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCThrowItemOK3"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_THROW_ITEM_OK_3; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szObjectID; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK3Handler;
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK3Handler {

public :

	// execute packet's handler
	static void execute ( GCThrowItemOK3 * pGCThrowItemOK3 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
