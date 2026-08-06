//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddZoneToInventory.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_ZONE_TO_INVENTORY_H__
#define __CG_ADD_ZONE_TO_INVENTORY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGAddZoneToInventory;
//
//////////////////////////////////////////////////////////////////////

class CGAddZoneToInventory : public Packet {
public :

	// constructor
	CGAddZoneToInventory() throw();

	// destructor
	~CGAddZoneToInventory() throw();

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_ADD_ZONE_TO_INVENTORY; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szCoord + szCoord + szCoordInven + szCoordInven; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddZoneToInventory"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	// get / set ObjectID
	ObjectID_t getObjectID() throw () { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get/set X Coordicate
	Coord_t getZoneX () const throw () { return m_ZoneX; }
	void setZoneX ( Coord_t ZoneX ) throw () { m_ZoneX = ZoneX; }

	// get/set Y Coordicate
	Coord_t getZoneY () const throw () { return m_ZoneY; }
	void setZoneY ( Coord_t ZoneY ) throw () { m_ZoneY = ZoneY; }

	// get / set Ivnentory X, Y Coordicate
	CoordInven_t getInvenX() const throw () { return m_InvenX; }
	void setInvenX( CoordInven_t InvenX ) throw () { m_InvenX = InvenX; }

	// get / set Inventory Y
	CoordInven_t getInvenY() const throw () { return m_InvenY; }
	void setInvenY( CoordInven_t InvenY ) { m_InvenY = InvenY; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	
	Coord_t m_ZoneX;
	Coord_t m_ZoneY;

	
	CoordInven_t m_InvenX;
	CoordInven_t m_InvenY;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAddZoneToInventoryFactory;
//
// Factory for CGAddZoneToInventory
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGAddZoneToInventoryFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGAddZoneToInventory(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddZoneToInventory"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_ADD_ZONE_TO_INVENTORY; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szCoord + szCoord + szCoordInven + szCoordInven; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGAddZoneToInventoryHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGAddZoneToInventoryHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGAddZoneToInventory * pPacket , Player * player ) throw ( ProtocolException , Error );
	};

#endif
#endif
