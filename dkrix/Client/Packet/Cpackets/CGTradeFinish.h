////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeFinish.h 

// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TRADE_FINISH_H__
#define __CG_TRADE_FINISH_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum
{
	
	CG_TRADE_FINISH_ACCEPT = 0,
	
	
	CG_TRADE_FINISH_REJECT,

	
	CG_TRADE_FINISH_RECONSIDER,


	CG_TRADE_FINISH_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinish;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeFinish : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_CG_TRADE_FINISH; }
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CGTradeFinish"; }
		std::string toString () const throw ();
	#endif
	
public:
	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id) throw() { m_TargetObjectID = id; }

	BYTE getCode() const throw() { return m_Code; }
	void setCode(BYTE code) throw() { m_Code = code; } 

private:
	ObjectID_t m_TargetObjectID; 
	BYTE       m_Code;           
	

};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinishFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGTradeFinishFactory : public PacketFactory 
{
public:
	Packet * createPacket () throw () { return new CGTradeFinish(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CGTradeFinish"; }
	#endif

	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_TRADE_FINISH; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szBYTE; }
};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinishHandler;
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGTradeFinishHandler 
	{
	public:
		static void execute ( CGTradeFinish * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeSlayer ( CGTradeFinish * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeVampire ( CGTradeFinish * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeError ( CGTradeFinish * pPacket , Player * player, BYTE ErrorCode ) throw ( ProtocolException , Error );
	};
#endif

#endif
