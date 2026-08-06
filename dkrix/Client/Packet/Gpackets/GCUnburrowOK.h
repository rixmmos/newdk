//////////////////////////////////////////////////////////////////////
// 
// Filename    :  GCUnburrowOK.h 
// Written By  :  Elca
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_UNBURROW_OK_H__
#define __GC_UNBURROW_OK_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class  GCUnburrowOK;
//


//
//////////////////////////////////////////////////////////////////////

class GCUnburrowOK : public Packet {

public :

	// constructor
	GCUnburrowOK () throw () {}
	GCUnburrowOK ( Coord_t x , Coord_t y , Dir_t dir ) throw () : m_X(x), m_Y(y), m_Dir(dir) {}


public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_UNBURROW_OK; }
	
	// get packet body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szCoord + szCoord + szDir; }
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCUnburrowOK"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	

public :

	// get/set X
	Coord_t getX() const throw () { return m_X; }
	void setX( Coord_t x) throw () { m_X = x; }
	
	// get/set Y
	Coord_t getY() const throw () { return m_Y; }
	void setY( Coord_t y) throw () { m_Y = y ; }

	// get/set Dir
	Dir_t getDir() const throw () { return m_Dir; }
	void setDir( Dir_t dir) throw () { m_Dir = dir; }
	

private : 

	Coord_t m_X;   
	Coord_t m_Y;   
	Dir_t m_Dir;   

};


//////////////////////////////////////////////////////////////////////
//
// class GCUnburrowOKFactory;
//
// Factory for GCUnburrowOK
//
//////////////////////////////////////////////////////////////////////

class  GCUnburrowOKFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCUnburrowOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCUnburrowOK"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_UNBURROW_OK; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szCoord + szCoord + szDir; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class  GCUnburrowOKHandler;
//
//////////////////////////////////////////////////////////////////////

class  GCUnburrowOKHandler {

public :

	// execute packet's handler
	static void execute (  GCUnburrowOK * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
