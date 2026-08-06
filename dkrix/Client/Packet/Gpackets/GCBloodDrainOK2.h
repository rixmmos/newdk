//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCBloodDrainOK2.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_BLOOD_DRAIN_OK_2_H__
#define __GC_BLOOD_DRAIN_OK_2_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../ModifyInfo.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK2 : public ModifyInfo {

public :
	
	// constructor
	GCBloodDrainOK2 () throw ();
	
	// destructor
	~GCBloodDrainOK2 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_BLOOD_DRAIN_OK_2; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + ModifyInfo::getPacketSize(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCBloodDrainOK2"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set CEffectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;



};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK2Factory;
//
// Factory for GCBloodDrainOK2
//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK2Factory : public PacketFactory {

public :
	
	// constructor
	GCBloodDrainOK2Factory () throw () {}
	
	// destructor
	virtual ~GCBloodDrainOK2Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCBloodDrainOK2(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCBloodDrainOK2"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_BLOOD_DRAIN_OK_2; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + ModifyInfo::getPacketMaxSize(); }

};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK2Handler {

public :

	// execute packet's handler
	static void execute ( GCBloodDrainOK2 * pGCBloodDrainOK2 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
