//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSystemMessage.h 
// Written By  : Reiot
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SYSTEM_MESSAGE_H__
#define __GC_SYSTEM_MESSAGE_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


enum SystemMessageType
{
	SYSTEM_MESSAGE_NORMAL = 0,
	SYSTEM_MESSAGE_OPERATOR,	
	SYSTEM_MESSAGE_MASTER_LAIR,	
	SYSTEM_MESSAGE_COMBAT,		
	SYSTEM_MESSAGE_INFO, 		
	SYSTEM_MESSAGE_HOLY_LAND,	
	SYSTEM_MESSAGE_RANGER_CHAT,	
	SYSTEM_MESSAGE_PLAYER,
	SYSTEM_MESSAGE_MAX
};

//////////////////////////////////////////////////////////////////////
//
// class GCSystemMessage;
//



//
//////////////////////////////////////////////////////////////////////

class GCSystemMessage : public Packet {

public :
	GCSystemMessage() 
	: m_Color(0x006040E8), m_Type(SYSTEM_MESSAGE_NORMAL)
	{
	}
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SYSTEM_MESSAGE; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE + m_Message.size() + szuint + szBYTE; }

	// get packet name
	std::string getPacketName() const throw() { return "GCSystemMessage"; }
	
	// get packet's debug std::string
	std::string toString() const throw();

	// get/set chatting message
	std::string getMessage() const throw() { return m_Message; }
	void setMessage(const std::string & msg) throw() { m_Message = msg; }

	// get/set text color
	uint getColor() const throw() { return m_Color; }
	void setColor( uint color ) throw() { m_Color = color; }

	SystemMessageType getType() const throw() { return m_Type; }
	void setType( SystemMessageType Type ) throw() { m_Type = Type; }


	

private :
	
	// chatting message
	std::string m_Message;

	// text color
	uint m_Color;
	
	SystemMessageType m_Type;

};


//////////////////////////////////////////////////////////////////////
//
// class GCSystemMessageFactory;
//
// Factory for GCSystemMessage
//
//////////////////////////////////////////////////////////////////////

class GCSystemMessageFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCSystemMessage(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCSystemMessage"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SYSTEM_MESSAGE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + 256 + szuint + szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCSystemMessageHandler;
//
//////////////////////////////////////////////////////////////////////

class GCSystemMessageHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCSystemMessage* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
