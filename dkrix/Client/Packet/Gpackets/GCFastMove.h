//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFastMove.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_FAST_MOVE_H__
#define __GC_FAST_MOVE_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCFastMove;
//


//
//////////////////////////////////////////////////////////////////////

class GCFastMove : public Packet {

public :

	// constructor
	GCFastMove () throw ()
	{
	}

	GCFastMove ( ObjectID_t objectID , Coord_t x , Coord_t y , Coord_t x2, Coord_t y2) throw ()
		: m_ObjectID(objectID), m_FromX(x), m_FromY(y), m_ToX(x2), m_ToY(y2)
	{
	}


public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_FAST_MOVE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + 4*szCoord + szSkillType; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCFastMove"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


public :

	// get/set Creature ID 
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( ObjectID_t objectID ) throw () { m_ObjectID = objectID; }

	void setXY( Coord_t x, Coord_t y, Coord_t x2, Coord_t y2)
	{
		m_FromX = x; m_FromY = y; m_ToX = x2; m_ToY = y2;
	}

	// get
	Coord_t getFromX () const throw () { return m_FromX; }
	Coord_t getFromY () const throw () { return m_FromY; }
	Coord_t getToX () const throw () { return m_ToX; }
	Coord_t getToY () const throw () { return m_ToY; }
	
	void setSkillType( SkillType_t skillType ) { m_SkillType = skillType; }
	SkillType_t getSkillType() const { return m_SkillType; }


private :
	
	ObjectID_t m_ObjectID;		
	Coord_t m_FromX,m_FromY, m_ToX, m_ToY;				
	SkillType_t m_SkillType;	
};


//////////////////////////////////////////////////////////////////////
//
// class GCFastMoveFactory;
//
// Factory for GCFastMove
//
//////////////////////////////////////////////////////////////////////

class GCFastMoveFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCFastMove(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCFastMove"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_FAST_MOVE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + 4*szCoord + szSkillType; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCFastMoveHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFastMoveHandler {

public :

	// execute packet's handler
	static void execute ( GCFastMove * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
