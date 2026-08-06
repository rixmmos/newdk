//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCDeleteEffectFromTile.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_DELETE_EFFECT_FROM_TILE_H__
#define __GC_DELETE_EFFECT_FROM_TILE_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCDeleteEffectFromTile;
//

//
//////////////////////////////////////////////////////////////////////

class GCDeleteEffectFromTile : public Packet {

public :
	
	// constructor
	GCDeleteEffectFromTile () throw ();
	
	// destructor
	~GCDeleteEffectFromTile () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_DELETE_EFFECT_FROM_TILE; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szCoord*2 + szEffectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCDeleteEffectFromTile"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set EffectID 
	EffectID_t getEffectID() const throw() { return m_EffectID; }
	void setEffectID( EffectID_t e ) throw() { m_EffectID = e; }
	

	// get / set ObjectID 
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t d ) throw() { m_ObjectID = d; }

	// get & set X, Y
	Coord_t getX() const throw() { return m_X;}
	Coord_t getY() const throw() { return m_Y;}
	void setXY( Coord_t x, Coord_t y) throw() { m_X = x; m_Y = y;}
	
private :
	
	Coord_t  m_X, m_Y;
	ObjectID_t m_ObjectID;

	EffectID_t	m_EffectID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteEffectFromTileFactory;
//
// Factory for GCDeleteEffectFromTile
//
//////////////////////////////////////////////////////////////////////

class GCDeleteEffectFromTileFactory : public PacketFactory {

public :
	
	// constructor
	GCDeleteEffectFromTileFactory () throw () {}
	
	// destructor
	virtual ~GCDeleteEffectFromTileFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCDeleteEffectFromTile(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCDeleteEffectFromTile"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_DELETE_EFFECT_FROM_TILE; }

	// get Packet Max Size
	// PacketSize_t getPacketMaxSize() const throw() { return szSkillType + szCEffectID + szDuration + szBYTE + szBYTE * m_ListNum * 2 ; }
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szCoord*2 + szEffectID ; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteEffectFromTileHandler;
//
//////////////////////////////////////////////////////////////////////

class GCDeleteEffectFromTileHandler {

public :

	// execute packet's handler
	static void execute ( GCDeleteEffectFromTile * pGCDeleteEffectFromTile , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
