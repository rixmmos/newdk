//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCSelectPCError.h 
// Written By  : Reiot
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_SELECT_PC_ERROR_H__
#define __LC_SELECT_PC_ERROR_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

enum SELECT_PC_ERROR
{
	SELECT_PC_ERROR_NULL,

	SELECT_PC_CANNOT_PLAY,			
	SELECT_PC_NOT_BILLING_CHECK,	
	SELECT_PC_CANNOT_PLAY_BY_ATTR,  
};

//////////////////////////////////////////////////////////////////////
//
// class LCSelectPCError;
//



//
//////////////////////////////////////////////////////////////////////

class LCSelectPCError : public Packet {

public:
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_LC_SELECT_PC_ERROR; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE; }
	
	// get packet's name
	std::string getPacketName() const throw() { return "LCSelectPCError"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
	
	// get/set error message
	BYTE getCode() const throw() { return m_Code; }
	void setCode(BYTE code) throw() { m_Code = code; }

private : 

	
	BYTE  m_Code;

};


//////////////////////////////////////////////////////////////////////
//
// class LCSelectPCErrorFactory;
//
// Factory for LCSelectPCError
//
//////////////////////////////////////////////////////////////////////

class LCSelectPCErrorFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new LCSelectPCError(); }

	// get packet name
	std::string getPacketName() const throw() { return "LCSelectPCError"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_LC_SELECT_PC_ERROR; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCSelectPCErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class LCSelectPCErrorHandler {

public:

	// execute packet's handler
	static void execute(LCSelectPCError* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
