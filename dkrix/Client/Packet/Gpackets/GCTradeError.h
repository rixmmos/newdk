////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeError.h 

// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_ERROR_H__
#define __GC_TRADE_ERROR_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum
{
	
	GC_TRADE_ERROR_CODE_TARGET_NOT_EXIST = 0,

	
	GC_TRADE_ERROR_CODE_RACE_DIFFER,

	
	GC_TRADE_ERROR_CODE_NOT_SAFE,

	
	GC_TRADE_ERROR_CODE_MOTORCYCLE,

	
	GC_TRADE_ERROR_CODE_BAT_OR_WOLF,

	
	GC_TRADE_ERROR_CODE_ALREADY_TRADING,

	
	GC_TRADE_ERROR_CODE_NOT_TRADING,

	
	GC_TRADE_ERROR_CODE_ADD_ITEM,

	
	GC_TRADE_ERROR_CODE_REMOVE_ITEM,

	
	GC_TRADE_ERROR_CODE_INCREASE_MONEY,

	
	GC_TRADE_ERROR_CODE_DECREASE_MONEY,

	
	GC_TRADE_ERROR_CODE_NOT_ENOUGH_SPACE,

 
    GC_TRADE_ERROR_CODE_EVENT_GIFT_BOX,
	
	
	GC_TRADE_ERROR_CODE_UNKNOWN,

	GC_TRADE_ERROR_CODE_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeError;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeError : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_GC_TRADE_ERROR; }
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBYTE ; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeError"; }
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
// class GCTradeErrorFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeErrorFactory : public PacketFactory 
{
public:
	Packet * createPacket () throw () { return new GCTradeError(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeError"; }
	#endif

	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_TRADE_ERROR; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szBYTE; }

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeErrorHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeErrorHandler 
{
public:
	static void execute ( GCTradeError * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
