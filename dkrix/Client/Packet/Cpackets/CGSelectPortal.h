//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGSelectPortal.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_PORTAL_H__
#define __CG_SELECT_PORTAL_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSelectPortal;
//
//////////////////////////////////////////////////////////////////////

class CGSelectPortal : public Packet {

public :
	
	// constructor
	CGSelectPortal () throw ();
	
	// destructor
	~CGSelectPortal () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_SELECT_PORTAL; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szZoneID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGSelectPortal"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set ZoneID
	ObjectID_t getZoneID() const throw()  { return m_ZoneID; }
	void setZoneID( ZoneID_t ZoneID ) throw() { m_ZoneID = ZoneID; }

	
private :

	ZoneID_t m_ZoneID;

};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectPortalFactory;
//
// Factory for CGSelectPortal
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGSelectPortalFactory : public PacketFactory {

public :
	
	// constructor
	CGSelectPortalFactory () throw () {}
	
	// destructor
	virtual ~CGSelectPortalFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new CGSelectPortal(); }

	// get packet name
	std::string getPacketName () const throw () { return "CGSelectPortal"; }
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_SELECT_PORTAL; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szZoneID; }
};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGSelectPortalHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGSelectPortalHandler {

public :

	// execute packet's handler
	static void execute ( CGSelectPortal * pCGSelectPortal , Player * pPlayer ) throw ( ProtocolException , Error );

};
#endif

#endif
