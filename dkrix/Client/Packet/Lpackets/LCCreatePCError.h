//----------------------------------------------------------------------
// 
// Filename    : LCCreatePCError.h 
// Written By  : Reiot
// Description :
// 
//----------------------------------------------------------------------

#ifndef __LC_CREATE_PC_ERROR_H__
#define __LC_CREATE_PC_ERROR_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//----------------------------------------------------------------------
//
// class LCCreatePCError;
//


//
//----------------------------------------------------------------------

class LCCreatePCError : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_LC_CREATE_PC_ERROR; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szBYTE; }
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "LCCreatePCError"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
	// get/set error message
//	std::string getMessage () const throw () { return m_Message; }
//  void setMessage ( std::string message ) throw () { m_Message = message; }

	// get /set ErrorID
	BYTE getErrorID() const throw() { return m_ErrorID; }
	void setErrorID( BYTE ErrorID ) throw() { m_ErrorID = ErrorID; }

private : 

	
	BYTE m_ErrorID;

};


//////////////////////////////////////////////////////////////////////
//
// class LCCreatePCErrorFactory;
//
// Factory for LCCreatePCError
//
//////////////////////////////////////////////////////////////////////

class LCCreatePCErrorFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new LCCreatePCError(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LCCreatePCError"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_LC_CREATE_PC_ERROR; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCCreatePCErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class LCCreatePCErrorHandler {

public :

	// execute packet's handler
	static void execute ( LCCreatePCError * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
