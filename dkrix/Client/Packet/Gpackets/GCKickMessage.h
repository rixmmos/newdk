//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCKickMessage.h 
// Written By  : Reiot
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_KICK_MESSAGE_H__
#define __GC_KICK_MESSAGE_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


enum KickMessageType
{
	KICK_MESSAGE_PAY_TIMEOUT = 0,
	KICK_MESSAGE_EXPIRE_FREEPLAY,

	KICK_MESSAGE_MAX
};

//////////////////////////////////////////////////////////////////////
//
// class GCKickMessage;
//



//
//////////////////////////////////////////////////////////////////////

class GCKickMessage : public Packet {

public :
	GCKickMessage() 
	: m_Type(KICK_MESSAGE_PAY_TIMEOUT)
	{
	}
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_KICK_MESSAGE; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE + szuint; }
#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCKickMessage"; }

	// get packet's debug std::string
	std::string toString() const throw();
#endif

	// get/set Kick Message Type
	BYTE getType() const throw() { return m_Type; }
	void setType( BYTE type ) throw() { m_Type = (KickMessageType)type; }

	// get/set seconds
	uint getSeconds() const throw() { return m_Seconds; }
	void setSeconds( uint seconds ) throw() { m_Seconds = seconds; }

private :
	
	KickMessageType m_Type;

	// seconds
	uint m_Seconds;
};


//////////////////////////////////////////////////////////////////////
//
// class GCKickMessageFactory;
//
// Factory for GCKickMessage
//
//////////////////////////////////////////////////////////////////////

class GCKickMessageFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCKickMessage(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCKickMessage"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_KICK_MESSAGE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szuint; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCKickMessageHandler;
//
//////////////////////////////////////////////////////////////////////

class GCKickMessageHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCKickMessage* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
