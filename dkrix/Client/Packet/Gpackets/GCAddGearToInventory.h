//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAddGearToInventory.h 
// Written By  : crazydog


// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_GEAR_TO_INVENTORY_H__
#define __GC_ADD_GEAR_TO_INVENTORY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCAddGearToInventory;
//
//////////////////////////////////////////////////////////////////////

class GCAddGearToInventory : public Packet {
public :

	// constructor
	GCAddGearToInventory() throw();

	// destructor
	~GCAddGearToInventory() throw();

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_ADD_GEAR_TO_INVENTORY; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szSlotID + szCoordInven + szCoordInven; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCAddGearToInventory"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	// get / set ObjectID
	SlotID_t getSlotID() throw () { return m_SlotID; }
	void setSlotID( SlotID_t SlotID ) throw() { m_SlotID = SlotID; }

	// get / set Ivnentory X, Y Coordicate
	CoordInven_t getInvenX() const throw () { return m_InvenX; }
	void setInvenX( CoordInven_t InvenX ) throw () { m_InvenX = InvenX; }

	// get / set Inventory Y
	CoordInven_t getInvenY() const throw () { return m_InvenY; }
	void setInvenY( CoordInven_t InvenY ) { m_InvenY = InvenY; }

private :
	
	// wear part ID
	SlotID_t m_SlotID;

	
	CoordInven_t m_InvenX;
	CoordInven_t m_InvenY;

};


//////////////////////////////////////////////////////////////////////
//
// class GCAddGearToInventoryFactory;
//
// Factory for GCAddGearToInventory
//
//////////////////////////////////////////////////////////////////////

class GCAddGearToInventoryFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCAddGearToInventory(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCAddGearToInventory"; }
	#endif	
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_ADD_GEAR_TO_INVENTORY; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szSlotID + szCoordInven + szCoordInven; }

};

//////////////////////////////////////////////////////////////////////
//
// class GCAddGearToInventoryHandler;
//
//////////////////////////////////////////////////////////////////////

class GCAddGearToInventoryHandler {
	
public :

	// execute packet's handler
	static void execute ( GCAddGearToInventory * pPacket , Player * player ) throw ( ProtocolException , Error );
};


#endif
