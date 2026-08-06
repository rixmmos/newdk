//////////////////////////////////////////////////////////////////////
// 
// Filename    :  GCSelectRankBonusFailed.h 
// Written By  :  elca@ewestsoft.com

//                
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SELECT_RANK_BONUS_FAILED_H__
#define __GC_SELECT_RANK_BONUS_FAILED_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class  GCSelectRankBonusFailed;
//
//////////////////////////////////////////////////////////////////////

class GCSelectRankBonusFailed : public Packet 
{

public: 

	GCSelectRankBonusFailed() throw();
	virtual ~GCSelectRankBonusFailed() throw();

	
public:
	
	
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
			
	
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SELECT_RANK_BONUS_FAILED; }
	
	// get packet size
	PacketSize_t getPacketSize() const throw() { return szDWORD + szBYTE; }

#ifdef __DEBUG_OUTPUT__
	// get packet's name
	std::string getPacketName() const throw() { return "GCSelectRankBonusFailed"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif

	// get/set skill type
	DWORD getRankBonusType() const throw() { return m_RankBonusType; }
	void setRankBonusType(DWORD rankBonusType) throw() { m_RankBonusType = rankBonusType; }

	// get/set description
	BYTE getDesc(void) const throw() { return m_Desc;}
	void setDesc(BYTE desc) throw() { m_Desc = desc;}

private:

	DWORD 		m_RankBonusType;
	BYTE        m_Desc;       

};


//////////////////////////////////////////////////////////////////////
//
// class  GCSelectRankBonusFailedFactory;
//
// Factory for  GCSelectRankBonusFailed
//
//////////////////////////////////////////////////////////////////////

class  GCSelectRankBonusFailedFactory : public PacketFactory {

public :
	
	// constructor
	 GCSelectRankBonusFailedFactory() throw() {}
	
	// destructor
	virtual ~GCSelectRankBonusFailedFactory() throw() {}

	
public :
	
	// create packet
	Packet* createPacket() throw() { return new GCSelectRankBonusFailed(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCSelectRankBonusFailed"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SELECT_RANK_BONUS_FAILED; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szDWORD + szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class  GCSelectRankBonusFailedHandler;
//
//////////////////////////////////////////////////////////////////////

class  GCSelectRankBonusFailedHandler {

public :

	// execute packet's handler
	static void execute( GCSelectRankBonusFailed* pGCSelectRankBonusFailed, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif	// __GC_LEARN_SKILL_FAILED_H__
