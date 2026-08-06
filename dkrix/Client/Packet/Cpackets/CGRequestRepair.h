//-----------------------------------------------------------------------------
// 
// Filename    : CGRequestRepair.h 

// Description :
// 
//-----------------------------------------------------------------------------

#ifndef __CG_REQUEST_REPAIR_H__
#define __CG_REQUEST_REPAIR_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CGRequestRepair;
//
//--------------------------------------------------------------------------------

class CGRequestRepair : public Packet {

public :
	
	
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
	
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_REQUEST_REPAIR; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szObjectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGRequestRepair"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	// get/set ObjectID
	ObjectID_t getObjectID() throw () { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

private :
	
	// Item Object ID
	ObjectID_t m_ObjectID;

};


//-----------------------------------------------------------------------------
//
// class CGRequestRepairFactory;
//
// Factory for CGRequestRepair
//
//-----------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
class CGRequestRepairFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGRequestRepair(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGRequestRepair"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_REQUEST_REPAIR; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID; }

};

#endif


//--------------------------------------------------------------------------------
//
// class CGRequestRepairHandler;
//
//--------------------------------------------------------------------------------
#ifndef __GAME_CLIENT__
	class CGRequestRepairHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGRequestRepair * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeNormal ( CGRequestRepair * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeMotorcycle ( CGRequestRepair * pPacket , Player * player ) throw ( ProtocolException , Error );
	};
#endif

#endif
