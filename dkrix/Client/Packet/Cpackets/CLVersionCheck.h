//--------------------------------------------------------------------------------
// 
// Filename    : CLVersionCheck.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __CL_VERSION_CHECK_H__
#define __CL_VERSION_CHECK_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CLVersionCheck;
//


//
//--------------------------------------------------------------------------------

class CLVersionCheck : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CL_VERSION_CHECK; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szDWORD; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CLVersionCheck"; }
		
		// get packet's debug string
		std::string toString () const throw ();
	#endif

public :

	// get/set Client Version
	DWORD getVersion() const throw() { return m_Version; }
	void setVersion( DWORD Version ) throw() { m_Version = Version; }

private :

	
	DWORD m_Version;

};


//--------------------------------------------------------------------------------
//
// class CLVersionCheckFactory;
//
// Factory for CLVersionCheck
//
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
class CLVersionCheckFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CLVersionCheck(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CLVersionCheck"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_VERSION_CHECK; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return szDWORD; }

};

#endif


//--------------------------------------------------------------------------------
//
// class CLVersionCheckHandler;
//
//--------------------------------------------------------------------------------
#ifndef __GAME_CLIENT__
	class CLVersionCheckHandler {

	public :

		// execute packet's handler
		static void execute ( CLVersionCheck * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
