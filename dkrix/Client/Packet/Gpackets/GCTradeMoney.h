////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeMoney.h 

// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_MONEY_H__
#define __GC_TRADE_MONEY_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum
{
	
	GC_TRADE_MONEY_INCREASE = 0,

	
	GC_TRADE_MONEY_DECREASE,

	
	GC_TRADE_MONEY_INCREASE_RESULT,

	
	GC_TRADE_MONEY_DECREASE_RESULT,
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeMoney;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeMoney : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_GC_TRADE_MONEY; }
	PacketSize_t getPacketSize () const throw () { return szObjectID + szGold + szBYTE ; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeMoney"; }
		std::string toString () const throw ();
	#endif

public:
	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id) throw() { m_TargetObjectID = id; }

	Gold_t getAmount() const throw() { return m_Gold; }
	void setAmount(Gold_t gold) throw() { m_Gold = gold; }

	BYTE getCode() const throw() { return m_Code; }
	void setCode(BYTE code) throw() { m_Code = code; }

private:
	ObjectID_t m_TargetObjectID; 
	Gold_t     m_Gold;           
	BYTE       m_Code;           

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeMoneyFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeMoneyFactory : public PacketFactory 
{
public:
	Packet * createPacket () throw () { return new GCTradeMoney(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeMoney"; }
	#endif

	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_TRADE_MONEY; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szGold + szBYTE; }

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeMoneyHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeMoneyHandler 
{
public:
	static void execute ( GCTradeMoney * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
