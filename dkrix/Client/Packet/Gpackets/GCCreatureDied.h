//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCCreatureDied.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CREATURE_DIED_H__
#define __GC_CREATURE_DIED_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCCreatureDied;
//



//
//////////////////////////////////////////////////////////////////////

class GCCreatureDied : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_CREATURE_DIED; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szObjectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCCreatureDied"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get/set dead creature's creature id
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( const ObjectID_t & creatureID ) throw () { m_ObjectID = creatureID; }
	

private :
	
	// dead creature's creature id
	ObjectID_t m_ObjectID;
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCCreatureDiedFactory;
//
// Factory for GCCreatureDied
//
//////////////////////////////////////////////////////////////////////

class GCCreatureDiedFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCCreatureDied(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCCreatureDied"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_CREATURE_DIED; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCCreatureDiedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCCreatureDiedHandler {
	
public :
	
	// execute packet's handler
	static void execute ( GCCreatureDied * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
