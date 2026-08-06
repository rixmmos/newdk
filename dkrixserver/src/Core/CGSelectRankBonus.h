//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectRankBonus.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_RANK_BONUS_H__
#define __CG_SELECT_RANK_BONUS_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectRankBonus;
//////////////////////////////////////////////////////////////////////////////

class CGSelectRankBonus : public Packet {
public:
    CGSelectRankBonus() {};
    virtual ~CGSelectRankBonus() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_RANK_BONUS;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szDWORD;
    }

    // get packet name
    string getPacketName() const {
        return "CGSelectRankBonus";
    }

    // get packet's debug string
    string toString() const;

public:
    DWORD getRankBonusType() const {
        return m_RankBonusType;
    }
    void setRankBonusType(DWORD rankBonusType) {
        m_RankBonusType = rankBonusType;
    }

private:
    DWORD m_RankBonusType; // Rank Bonus Type
};

//////////////////////////////////////////////////////////////////////
// class CGSelectRankBonusFactory;
//////////////////////////////////////////////////////////////////////

class CGSelectRankBonusFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGSelectRankBonus();
    }

    // get packet name
    string getPacketName() const {
        return "CGSelectRankBonus";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_SELECT_RANK_BONUS;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szDWORD;
    }
};


//////////////////////////////////////////////////////////////////////
// class CGSelectRankBonusHandler;
//////////////////////////////////////////////////////////////////////

class CGSelectRankBonusHandler {
public:
    // execute packet's handler
    static void execute(CGSelectRankBonus* pCGSelectRankBonus, Player* pPlayer);
};

#endif
