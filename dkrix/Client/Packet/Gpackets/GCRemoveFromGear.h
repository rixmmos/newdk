//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCRemoveFromGear.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_REMOVE_FROM_GEAR_H__
#define __GC_REMOVE_FROM_GEAR_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRemoveFromGear;
//


//
//////////////////////////////////////////////////////////////////////

class GCRemoveFromGear : public Packet {

public :
	
	// constructor
	GCRemoveFromGear () throw ();
	
	// destructor
	~GCRemoveFromGear () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_REMOVE_FROM_GEAR; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szSlotID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCRemoveFromGear"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get/set SlotID
	SlotID_t getSlotID() const throw() { return m_SlotID; }
	void setSlotID( SlotID_t SlotID ) throw() { m_SlotID = SlotID; }

private :

	// SlotID
	SlotID_t m_SlotID;

};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveFromGearFactory;
//
// Factory for GCRemoveFromGear
//
//////////////////////////////////////////////////////////////////////

class GCRemoveFromGearFactory : public PacketFactory {

public :
	
	// constructor
	GCRemoveFromGearFactory () throw () {}
	
	// destructor
	virtual ~GCRemoveFromGearFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCRemoveFromGear(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCRemoveFromGear"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_REMOVE_FROM_GEAR; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szSlotID; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveFromGearHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRemoveFromGearHandler {

public :

	// execute packet's handler
	static void execute ( GCRemoveFromGear * pGCRemoveFromGear , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
