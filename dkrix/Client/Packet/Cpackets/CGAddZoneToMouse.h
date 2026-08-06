//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddZoneToMouse.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_ZONE_TO_MOUSE_H__
#define __CG_ADD_ZONE_TO_MOUSE_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGAddZoneToMouse;
//
//////////////////////////////////////////////////////////////////////

class CGAddZoneToMouse : public Packet {
public :

	// constructor
	CGAddZoneToMouse() throw();

	// destructor
	~CGAddZoneToMouse() throw();

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_ADD_ZONE_TO_MOUSE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szCoord + szCoord; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddZoneToMouse"; }
		
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

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	
	Coord_t m_ZoneX;
	Coord_t m_ZoneY;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAddZoneToMouseFactory;
//
// Factory for CGAddZoneToMouse
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGAddZoneToMouseFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGAddZoneToMouse(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddZoneToMouse"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_ADD_ZONE_TO_MOUSE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szCoord + szCoord; }

};

#endif

//////////////////////////////////////////////////////////////////////
//
// class CGAddZoneToMouseHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGAddZoneToMouseHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGAddZoneToMouse * pPacket , Player * player ) throw ( ProtocolException , Error );
	};

#endif
#endif
