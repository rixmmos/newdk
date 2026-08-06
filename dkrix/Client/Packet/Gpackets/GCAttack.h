//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAttack.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ATTACK_H__
#define __GC_ATTACK_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCAttack;
//


//
//////////////////////////////////////////////////////////////////////

class GCAttack : public Packet {

public :
	
	// constructor
	GCAttack () throw ();
	
	// destructor
	~GCAttack () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_ATTACK; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szCoord + szCoord + szDir ; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCAttack"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get Object ID 
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( ObjectID_t objectID ) throw () { m_ObjectID = objectID; }

	// get/set X
	Coord_t getX () const throw () { return m_X; }
	void setX ( Coord_t x ) throw () { m_X = x; }
	
	// get/set Y
	Coord_t getY () const throw () { return m_Y; }
	void setY ( Coord_t y ) throw () { m_Y = y; }

	// get/set Dir
	Dir_t getDir () const throw () { return m_Dir; }
	void setDir ( Dir_t dir ) throw () { m_Dir = dir; }


private :

	ObjectID_t m_ObjectID;		
	Coord_t m_X;				
	Coord_t m_Y;				
	Dir_t m_Dir;				

};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackFactory;
//
// Factory for GCAttack
//
//////////////////////////////////////////////////////////////////////

class GCAttackFactory : public PacketFactory {

public :
	
	// constructor
	GCAttackFactory () throw () {}
	
	// destructor
	virtual ~GCAttackFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCAttack(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCAttack"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_ATTACK; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szCoord + szCoord + szDir ; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackHandler;
//
//////////////////////////////////////////////////////////////////////

class GCAttackHandler {

public :

	// execute packet's handler
	static void execute ( GCAttack * pGCAttack , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
