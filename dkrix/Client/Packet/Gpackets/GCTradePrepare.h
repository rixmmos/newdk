////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradePrepare.h 

// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_PREPARE_H__
#define __GC_TRADE_PREPARE_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum
{
	
	GC_TRADE_PREPARE_CODE_REQUEST = 0,

	GC_TRADE_PREPARE_CODE_CANCEL,

	
	GC_TRADE_PREPARE_CODE_ACCEPT,

	
	GC_TRADE_PREPARE_CODE_REJECT,

	
	GC_TRADE_PREPARE_CODE_BUSY,

	GC_TRADE_PREPARE_CODE_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradePrepare;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradePrepare : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_GC_TRADE_PREPARE; }
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradePrepare"; }
		std::string toString () const throw ();
	#endif

public:
	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id) throw() { m_TargetObjectID = id; }

	BYTE getCode() const throw() { return m_Code; }
	void setCode(BYTE code) { m_Code = code; }

private :
	ObjectID_t m_TargetObjectID; 
	BYTE       m_Code;           

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradePrepareFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradePrepareFactory : public PacketFactory 
{
public:
	Packet * createPacket () throw () { return new GCTradePrepare(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "GCTradePrepare"; }
	#endif

	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_TRADE_PREPARE; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szBYTE; }

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradePrepareHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradePrepareHandler 
{
public:
	static void execute ( GCTradePrepare * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
