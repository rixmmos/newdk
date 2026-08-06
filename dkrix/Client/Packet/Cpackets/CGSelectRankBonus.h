//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectRankBonus.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_RANK_BONUS_H__
#define __CG_SELECT_RANK_BONUS_H__

#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectRankBonus;
//////////////////////////////////////////////////////////////////////////////

class CGSelectRankBonus : public Packet 
{
public:

    
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);

    
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_SELECT_RANK_BONUS; }

	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szDWORD; }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "CGSelectRankBonus"; }

	// get packet's debug std::string
	std::string toString() const throw();
#endif

public:
	DWORD getRankBonusType() const throw() { return m_RankBonusType; }
	void setRankBonusType( DWORD rankBonusType ) throw() { m_RankBonusType = rankBonusType; }

private:
	DWORD	m_RankBonusType;		// Rank Bonus Type
};

//////////////////////////////////////////////////////////////////////
// class CGSelectRankBonusFactory;
//////////////////////////////////////////////////////////////////////
class CGSelectRankBonusFactory : public PacketFactory 
{
public:
	// create packet
	Packet* createPacket() throw() { return new CGSelectRankBonus(); }

	// get packet name
	std::string getPacketName() const throw() { return "CGSelectRankBonus"; }

	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_SELECT_RANK_BONUS; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szDWORD; }
};

//////////////////////////////////////////////////////////////////////
// class CGSelectRankBonusHandler;
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGSelectRankBonusHandler 
{
public:
	// execute packet's handler
	static void execute(CGSelectRankBonus* pCGSelectRankBonus, Player* pPlayer) throw(ProtocolException, Error);
	static void executeSlayerSkill(CGSelectRankBonus* pCGSelectRankBonus, Player* pPlayer) throw(ProtocolException, Error);
	static void executeVampireSkill(CGSelectRankBonus* pCGSelectRankBonus, Player* pPlayer) throw(ProtocolException, Error);
};
#endif
#endif
