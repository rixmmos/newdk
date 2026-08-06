//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCPhoneConnected.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_PHONE_CONNECTED_H__
#define __GC_PHONE_CONNECTED_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnected;
//


//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnected : public Packet {

public :
	
	// constructor
	GCPhoneConnected () throw ();
	
	// destructor
	~GCPhoneConnected () throw ();

	
public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_PHONE_CONNECTED; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szPhoneNumber + szSlotID + szBYTE + m_Name.size(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCPhoneConnected"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get/set PhoneNumber
	PhoneNumber_t getPhoneNumber() const throw() { return m_PhoneNumber; }
	void setPhoneNumber( PhoneNumber_t PhoneNumber ) throw() { m_PhoneNumber = PhoneNumber; }

	// get/set SlotID
	SlotID_t getSlotID() const throw() { return m_SlotID; }
	void setSlotID( SlotID_t SlotID ) throw() { m_SlotID = SlotID; }

	// get/set Name
	std::string getName() const throw() { return m_Name; }
	void setName( const std::string & Name ) { m_Name = Name; }

private :

	// PhoneNumber
	PhoneNumber_t m_PhoneNumber;

	// SlotID
	SlotID_t m_SlotID;

	
	std::string m_Name;

};


//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectedFactory;
//
// Factory for GCPhoneConnected
//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectedFactory : public PacketFactory {

public :
	
	// constructor
	GCPhoneConnectedFactory () throw () {}
	
	// destructor
	virtual ~GCPhoneConnectedFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCPhoneConnected(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCPhoneConnected"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_PHONE_CONNECTED; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szPhoneNumber + szSlotID + szBYTE + 20; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectedHandler {

public :

	// execute packet's handler
	static void execute ( GCPhoneConnected * pGCPhoneConnected , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
