//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGRangerSay.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_RANGER_SAY_H__
#define __CG_RANGER_SAY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGRangerSay;
//

//
//////////////////////////////////////////////////////////////////////

class CGRangerSay : public Packet
{
public:
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_RANGER_SAY; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE + m_Message.size(); }
#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "CGRangerSay"; }
	
	// get packet's debug string
	std::string toString() const throw();
#endif
	// get/set chatting message
	std::string getMessage() const throw() { return m_Message; }
	void setMessage(const std::string & msg) throw() { m_Message = msg; }
	

private :
	// chatting message
	std::string m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class CGRangerSayFactory;
//
// Factory for CGRangerSay
//
//////////////////////////////////////////////////////////////////////

class CGRangerSayFactory : public PacketFactory
{
public:
	// create packet
	Packet* createPacket() throw() { return new CGRangerSay(); }

	// get packet name
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "CGRangerSay"; }
#endif	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_RANGER_SAY; }

	// get packet's max body size
	
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + 128; }

};


//////////////////////////////////////////////////////////////////////
//
// class CGRangerSayHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGRangerSayHandler
{
public:

	// execute packet's handler
	static void execute(CGRangerSay* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};
#endif
#endif

