////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeFinish.h 

// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_FINISH_H__
#define __GC_TRADE_FINISH_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum
{
	
	GC_TRADE_FINISH_ACCEPT = 0,

	
	GC_TRADE_FINISH_REJECT,

	
	GC_TRADE_FINISH_RECONSIDER,

	
	GC_TRADE_FINISH_EXECUTE,

	GC_TRADE_FINISH_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeFinish;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeFinish : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_GC_TRADE_FINISH; }
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeFinish"; }
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
// class GCTradeFinishFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeFinishFactory : public PacketFactory 
{
public:
	Packet * createPacket () throw () { return new GCTradeFinish(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradeFinish"; }
	#endif

	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_TRADE_FINISH; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szBYTE; }

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeFinishHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeFinishHandler 
{
public:
	static void execute ( GCTradeFinish * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
