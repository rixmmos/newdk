////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeVerify.h 

// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_VERIFY_H__
#define __GC_TRADE_VERIFY_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum
{
	
	GC_TRADE_VERIFY_CODE_ADD_ITEM_WHEN_ACCEPT,

	
	GC_TRADE_VERIFY_CODE_REMOVE_ITEM,

	
	GC_TRADE_VERIFY_CODE_MONEY_INCREASE,

	
	GC_TRADE_VERIFY_CODE_MONEY_DECREASE,

	
	GC_TRADE_VERIFY_CODE_FINISH_ACCEPT,

	
	GC_TRADE_VERIFY_CODE_FINISH_REJECT,

	
	GC_TRADE_VERIFY_CODE_FINISH_RECONSIDER,

	
	GC_TRADE_VERIFY_CODE_MOUSE_TO_INVENTORY_OK,
	GC_TRADE_VERIFY_CODE_MOUSE_TO_INVENTORY_FAIL,

	
	GC_TRADE_VERIFY_CODE_INVENTORY_TO_MOUSE_OK,
	GC_TRADE_VERIFY_CODE_INVENTORY_TO_MOUSE_FAIL,

	
	GC_TRADE_VERIFY_CODE_ADD_ITEM_OK,
	GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL,

	
	GC_TRADE_VERIFY_CODE_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeVerify;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeVerify : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_GC_TRADE_VERIFY; }
	PacketSize_t getPacketSize () const throw () { return szBYTE ; }
	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeVerify"; }
		std::string toString () const throw ();
	#endif

public:
	BYTE getCode() const throw() { return m_Code; }
	void setCode(BYTE code) throw() { m_Code = code; }

private:
	BYTE       m_Code;           

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeVerifyFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeVerifyFactory : public PacketFactory 
{
public:
	Packet * createPacket () throw () { return new GCTradeVerify(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeVerify"; }
	#endif

	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_TRADE_VERIFY; }
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE; }

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeVerifyHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeVerifyHandler 
{
public:
	static void execute ( GCTradeVerify * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
