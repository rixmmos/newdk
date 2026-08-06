//--------------------------------------------------------------------------------
//
// Filename    : GCShopMarketCondition.h


//
//--------------------------------------------------------------------------------

#ifndef __GC_SHOP_MARKET_CONDITION_H__
#define __GC_SHOP_MARKET_CONDITION_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCShopMarketCondition;
//
//--------------------------------------------------------------------------------

class GCShopMarketCondition : public Packet {
public:
    GCShopMarketCondition();
    virtual ~GCShopMarketCondition();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_SHOP_MARKET_CONDITION;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID + szMarketCond * 2;
    }

    // get packet name
    string getPacketName() const {
        return "GCShopMarketCondition";
    }

    // get packet's debug string
    string toString() const;


public:
    // get/set NPC's object id
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t creatureID) {
        m_ObjectID = creatureID;
    }

    // get/set market condition buy
    MarketCond_t getMarketCondBuy(void) const {
        return m_MarketCondBuy;
    }
    void setMarketCondBuy(MarketCond_t cond) {
        m_MarketCondBuy = cond;
    }

    // get/set market condition sell
    MarketCond_t getMarketCondSell(void) const {
        return m_MarketCondSell;
    }
    void setMarketCondSell(MarketCond_t cond) {
        m_MarketCondSell = cond;
    }

private:
    // NPC's object id
    ObjectID_t m_ObjectID;

    // shop version
    MarketCond_t m_MarketCondBuy;
    MarketCond_t m_MarketCondSell;
};


//////////////////////////////////////////////////////////////////////
//
// class GCShopMarketConditionFactory;
//
// Factory for GCShopMarketCondition
//
//////////////////////////////////////////////////////////////////////

class GCShopMarketConditionFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCShopMarketCondition();
    }

    // get packet name
    string getPacketName() const {
        return "GCShopMarketCondition";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_SHOP_MARKET_CONDITION;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szMarketCond * 2;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCShopMarketConditionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCShopMarketConditionHandler {
public:
    // execute packet's handler
    static void execute(GCShopMarketCondition* pPacket, Player* pPlayer);
};

#endif
