////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradePrepare.h 

// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TRADE_PREPARE_H__
#define __CG_TRADE_PREPARE_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum
{
	
	CG_TRADE_PREPARE_CODE_REQUEST = 0,

	
	CG_TRADE_PREPARE_CODE_CANCEL,

	
	CG_TRADE_PREPARE_CODE_ACCEPT,

	
	CG_TRADE_PREPARE_CODE_REJECT,

	
	CG_TRADE_PREPARE_CODE_BUSY,

	CG_TRADE_PREPARE_CODE_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradePrepare;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradePrepare : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_CG_TRADE_PREPARE; }
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CGTradePrepare"; }
		std::string toString () const throw ();
	#endif
	
public:
	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id) throw() { m_TargetObjectID = id; }

	BYTE getCode(void) const throw() { return m_Code; }
	void setCode(BYTE code) throw() { m_Code = code; }

private:
	ObjectID_t m_TargetObjectID; 
	BYTE       m_Code;           

};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradePrepareFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGTradePrepareFactory : public PacketFactory 
{
public:
	Packet * createPacket () throw () { return new CGTradePrepare(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CGTradePrepare"; }
	#endif

	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_TRADE_PREPARE; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szBYTE; }
};
#endif


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradePrepareHandler;
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGTradePrepareHandler 
	{
	public:
		static void execute ( CGTradePrepare * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeSlayer ( CGTradePrepare * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeVampire ( CGTradePrepare * pPacket , Player * player ) throw ( ProtocolException , Error );
		static void executeError ( CGTradePrepare * pPacket , Player * player, BYTE ErrorCode ) throw ( ProtocolException , Error );
	};
#endif

#endif
