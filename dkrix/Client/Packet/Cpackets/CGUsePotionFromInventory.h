//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGUsePotionFromInventory.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_POTION_FROM_INVENTORY_H__
#define __CG_USE_POTION_FROM_INVENTORY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGUsePotionFromInventory;
//
//////////////////////////////////////////////////////////////////////

class CGUsePotionFromInventory : public Packet {
public :

	// constructor
	CGUsePotionFromInventory() throw()		{}

	// destructor
	~CGUsePotionFromInventory() throw()		{}

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_USE_POTION_FROM_INVENTORY; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szCoordInven + szCoordInven; }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName () const throw () { return "CGUsePotionFromInventory"; }
	
	// get packet's debug std::string
	std::string toString () const throw ();
#endif
	
public :

	// get / set ObjectID
	ObjectID_t getObjectID() const throw () { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get / set Inventory X
	CoordInven_t getX() const throw() { return m_InvenX; }
	void setX( CoordInven_t InvenX ) throw() { m_InvenX = InvenX; }

	// get / set Inventory Y
	CoordInven_t getY() const throw() { return m_InvenY; }
	void setY( CoordInven_t InvenY ) throw() { m_InvenY = InvenY; }


private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	
	CoordInven_t m_InvenX;
	CoordInven_t m_InvenY;

};


//////////////////////////////////////////////////////////////////////
//
// class CGUsePotionFromInventoryFactory;
//
// Factory for CGUsePotionFromInventory
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGUsePotionFromInventoryFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGUsePotionFromInventory(); }

	// get packet name
	std::string getPacketName () const throw () { return "CGUsePotionFromInventory"; }
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_USE_POTION_FROM_INVENTORY; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szCoordInven + szCoordInven; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGUsePotionFromInventoryHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGUsePotionFromInventoryHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGUsePotionFromInventory * pPacket , Player * player ) throw ( ProtocolException , Error );
	};

#endif
#endif
