//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCThrowItemOK1.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_THROW_ITEM_OK_1_H__
#define __GC_THROW_ITEM_OK_1_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../ModifyInfo.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK1;
//

//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK1 : public Packet {

public :
	
	// constructor
	GCThrowItemOK1 () throw ();
	
	// destructor
	~GCThrowItemOK1 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_THROW_ITEM_OK_1; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID ; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCThrowItemOK1"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set TargetObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

private :
	
	// TargetObjectID
	ObjectID_t m_ObjectID;

};


//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK1Factory;
//
// Factory for GCThrowItemOK1
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK1Factory : public PacketFactory {

public :
	
	// constructor
	GCThrowItemOK1Factory () throw () {}
	
	// destructor
	virtual ~GCThrowItemOK1Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCThrowItemOK1(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCThrowItemOK1"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_THROW_ITEM_OK_1; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK1Handler;
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK1Handler {

public :

	// execute packet's handler
	static void execute ( GCThrowItemOK1 * pGCThrowItemOK1 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
