//////////////////////////////////////////////////////////////////////
// 
// Filename    : CRDisconnect.h 
// Written By  : crazydog

// 
//////////////////////////////////////////////////////////////////////

#ifndef __CR_DISCONNECT_H__
#define __CR_DISCONNECT_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CRDisconnect;
//

//
//////////////////////////////////////////////////////////////////////

class CRDisconnect : public Packet
{

public :
	
	// constructor
	CRDisconnect () throw ();
	
	// destructor
	~CRDisconnect () throw ();
	
public :
    PacketID_t getPacketID () const throw () { return PACKET_CR_DISCONNECT; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CRDisconnect"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return 0; }
	static PacketSize_t getPacketMaxSize() throw() { return 0;}

protected :

};

//////////////////////////////////////////////////////////////////////
//
// class CRDisconnectFactory;
//
// Factory for CRDisconnect
//
//////////////////////////////////////////////////////////////////////

class CRDisconnectFactory : public PacketFactory {

public :
	
	// constructor
	CRDisconnectFactory () throw () {}
	
	// destructor
	virtual ~CRDisconnectFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new CRDisconnect(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CRDisconnect"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CR_DISCONNECT; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return 0;}

};


//////////////////////////////////////////////////////////////////////
//
// class CRDisconnectHandler;
//
//////////////////////////////////////////////////////////////////////

class CRDisconnectHandler {

public :

	// execute packet's handler
	static void execute ( CRDisconnect * pCRDisconnect , Player * pPlayer ) throw ( ProtocolException , Error );

};


#endif
