//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddInventoryToMouse.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_INVENTORY_TO_MOUSE_H__
#define __CG_ADD_INVENTORY_TO_MOUSE_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGAddInventoryToMouse;
//
//////////////////////////////////////////////////////////////////////

class CGAddInventoryToMouse : public Packet {
public :

	// constructor
	CGAddInventoryToMouse() throw();

	// destructor
	~CGAddInventoryToMouse() throw();

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_ADD_INVENTORY_TO_MOUSE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	//modify by viva
	//PacketSize_t getPacketSize () const throw () { return szObjectID + szObjectID + szCoordInven + szCoordInven; }
	PacketSize_t getPacketSize () const throw () { return szObjectID + szCoordInven + szCoordInven; }
	//end
	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddInventoryToMouse"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	// get / set ObjectID
	ObjectID_t getObjectID() throw () { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	ObjectID_t getInventoryItemObjectID() throw() { return m_InventoryItemObjectID; }
	void setInventoryItemObjectID(ObjectID_t InventoryItemObjectID) throw() { m_InventoryItemObjectID = InventoryItemObjectID; }

	// get / set Ivnentory X Coordicate
	CoordInven_t getX() const throw () { return m_InvenX; }
	void setX( CoordInven_t InvenX ) throw () { m_InvenX = InvenX; }

	// get / set Inventory Y Coordicate
	CoordInven_t getY() const throw () { return m_InvenY; }
	void setY( CoordInven_t InvenY ) { m_InvenY = InvenY; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	
	ObjectID_t m_InventoryItemObjectID;

	
	CoordInven_t m_InvenX;
	CoordInven_t m_InvenY;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAddInventoryToMouseFactory;
//
// Factory for CGAddInventoryToMouse
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGAddInventoryToMouseFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGAddInventoryToMouse(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddInventoryToMouse"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_ADD_INVENTORY_TO_MOUSE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	//modify by viva
	//PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szObjectID + szCoordInven + szCoordInven; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szCoordInven + szCoordInven; }
	//end
};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGAddInventoryToMouseHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGAddInventoryToMouseHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGAddInventoryToMouse * pPacket , Player * player ) throw ( ProtocolException , Error );
	};

#endif
#endif
