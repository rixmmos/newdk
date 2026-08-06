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
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class  GCSelectRankBonusFailed;
//
//////////////////////////////////////////////////////////////////////

class GCSelectRankBonusFailed : public Packet {
public:
    GCSelectRankBonusFailed();
    virtual ~GCSelectRankBonusFailed();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_SELECT_RANK_BONUS_FAILED;
    }

    // get packet size
    PacketSize_t getPacketSize() const {
        return szDWORD + szBYTE;
    }

    // get packet's name
    string getPacketName() const {
        return "GCSelectRankBonusFailed";
    }

    // get packet's debug string
    string toString() const;

    // get/set skill type
    DWORD getRankBonusType() const {
        return m_RankBonusType;
    }
    void setRankBonusType(DWORD rankBonusType) {
        m_RankBonusType = rankBonusType;
    }

    // get/set description
    BYTE getDesc(void) const {
        return m_Desc;
    }
    void setDesc(BYTE desc) {
        m_Desc = desc;
    }

private:
    DWORD m_RankBonusType;
    BYTE m_Desc; 
};


//////////////////////////////////////////////////////////////////////
//
// class  GCSelectRankBonusFailedFactory;
//
// Factory for  GCSelectRankBonusFailed
//
//////////////////////////////////////////////////////////////////////

class GCSelectRankBonusFailedFactory : public PacketFactory {
public:
    // constructor
    GCSelectRankBonusFailedFactory() {}

    // destructor
    virtual ~GCSelectRankBonusFailedFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCSelectRankBonusFailed();
    }

    // get packet name
    string getPacketName() const {
        return "GCSelectRankBonusFailed";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_SELECT_RANK_BONUS_FAILED;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szDWORD + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class  GCSelectRankBonusFailedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCSelectRankBonusFailedHandler {
public:
    // execute packet's handler
    static void execute(GCSelectRankBonusFailed* pGCSelectRankBonusFailed, Player* pPlayer);
};

#endif // __GC_LEARN_SKILL_FAILED_H__
