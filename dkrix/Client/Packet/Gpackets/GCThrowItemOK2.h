//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCThrowItemOK2.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_THROW_ITEM_OK_2_H__
#define __GC_THROW_ITEM_OK_2_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../ModifyInfo.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK2 : public ModifyInfo {

public :
	
	// constructor
	GCThrowItemOK2 () throw ();
	
	// destructor
	~GCThrowItemOK2 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_THROW_ITEM_OK_2; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + ModifyInfo::getPacketSize(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCThrowItemOK2"; }
		
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
// class GCThrowItemOK2Factory;
//
// Factory for GCThrowItemOK2
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK2Factory : public PacketFactory {

public :
	
	// constructor
	GCThrowItemOK2Factory () throw () {}
	
	// destructor
	virtual ~GCThrowItemOK2Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCThrowItemOK2(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCThrowItemOK2"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_THROW_ITEM_OK_2; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + ModifyInfo::getPacketMaxSize(); }

};


//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK2Handler {

public :

	// execute packet's handler
	static void execute ( GCThrowItemOK2 * pGCThrowItemOK2 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
