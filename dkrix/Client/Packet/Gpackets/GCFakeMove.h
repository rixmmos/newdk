//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFakeMove.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_FAKE_MOVE_H__
#define __GC_FAKE_MOVE_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCFakeMove;
//


//
//////////////////////////////////////////////////////////////////////

class GCFakeMove : public Packet {

public :

	// constructor
	GCFakeMove() throw()
	{
	}

	GCFakeMove(ObjectID_t objectID, Coord_t x, Coord_t y, Coord_t x2, Coord_t y2) throw()
		: m_ObjectID(objectID), m_ToX(x2), m_ToY(y2)
	{
	}


public :
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_FAKE_MOVE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize() const throw() { return szObjectID + 2*szCoord; }

#ifdef __DEBUG_OUTPUT__
	// get packet's name
	std::string getPacketName() const throw() { return "GCFakeMove"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif


public :

	// get/set Creature ID 
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t objectID) throw() { m_ObjectID = objectID; }

	void setXY(Coord_t x2, Coord_t y2)
	{
		m_ToX = x2; 
		m_ToY = y2;
	}

	// get
	Coord_t getToX() const throw() { return m_ToX; }
	Coord_t getToY() const throw() { return m_ToY; }
	


private :
	
	ObjectID_t m_ObjectID;		
	Coord_t m_ToX, m_ToY;		
};


//////////////////////////////////////////////////////////////////////
//
// class GCFakeMoveFactory;
//
// Factory for GCFakeMove
//
//////////////////////////////////////////////////////////////////////

class GCFakeMoveFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCFakeMove(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCFakeMove"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FAKE_MOVE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + 2*szCoord ; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCFakeMoveHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFakeMoveHandler {

public :

	// execute packet's handler
	static void execute(GCFakeMove* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
