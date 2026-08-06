//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGSay.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SAY_H__
#define __CG_SAY_H__

// include files

//#ifdef __GAME_SERVER__
//#include "GamePlayer.h"
//#endif

#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSay;
//


//
//////////////////////////////////////////////////////////////////////

class Player;
class Creature;
class GamePlayer;

class CGSay : public Packet {

public:
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_SAY; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szuint + szBYTE + m_Message.size(); }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "CGSay"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif

	// get/set text color
	uint getColor() const throw() { return m_Color; }
	void setColor( uint color ) throw() { m_Color = color; }

	// get/set chatting message
	const std::string& getMessage() const throw() { return m_Message; }
	void setMessage(const std::string & msg) throw() { m_Message = msg; }
	

private :
	
	// text color
	uint m_Color;

	// chatting message
	std::string m_Message;
	
};


#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////
//
// class CGSayFactory;
//
// Factory for CGSay
//
//////////////////////////////////////////////////////////////////////

class CGSayFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new CGSay(); }

	// get packet name
	std::string getPacketName() const throw() { return "CGSay"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_SAY; }

	// get packet's max body size
	
	PacketSize_t getPacketMaxSize() const throw() { return szuint + szBYTE + 128; }

};
#endif


#ifndef __GAME_CLIENT__
//////////////////////////////////////////////////////////////////////
//
// class CGSayHandler;
//
//////////////////////////////////////////////////////////////////////

class CGSayHandler {

public:

	// execute packet's handler
	static void execute(CGSay* pPacket, Player* pPlayer) throw(ProtocolException, Error);

#ifdef __GAME_SERVER__

	static void opExecute( Creature* pCreature, GamePlayer* pPlayer, std::string msg, int i ) throw(ProtocolException, Error);

	// for guild test
	static void opzone( std::string msg, int i ) throw( ProtocolException, Error );
	static void opguild( std::string msg, int i ) throw(ProtocolException, Error);

	
	static void opcombat( GamePlayer* pPlayer, std::string msg, int i ) throw(ProtocolException, Error);

	
	static void opset( GamePlayer* pPlayer, std::string msg, int i ) throw(ProtocolException, Error);

	static void opview( GamePlayer* pPlayer, std::string msg, int i ) throw(ProtocolException, Error);

	// save
	static void opsave(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// wall
	static void opwall(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// Shutdown
	static void opshutdown(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// kick
	static void opkick(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// mute
	static void opmute(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// freezing
	static void opfreezing(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// deny
	static void opdeny(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// info
	static void opinfo(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// trace
	static void optrace(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// warp
	static void opwarp(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// create
	static void opcreate(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// grant
	static void opgrant(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// recall
	static void oprecall(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// mrecall
	static void opmrecall(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// user
	static void opuser(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// summon
	static void opsummon(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// notice 
	static void opnotice(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// pay 
	static void oppay(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);

	// world 
	static void opworld(GamePlayer* pPlayer, std::string msg, int i, bool bSameWorldOnly) throw(ProtocolException, Error);

	// command 
	static void opcommand(GamePlayer* pPlayer, std::string msg, int i) throw(ProtocolException, Error);
#endif

};
#endif

#endif
