//////////////////////////////////////////////////////////////////////
//
// Filename    : GCModifyMoney.h
// Written By  :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MODIFY_MONEY_H__
#define __GC_MODIFY_MONEY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCModifyMoney;
//

//
//////////////////////////////////////////////////////////////////////

class GCModifyMoney : public Packet {
public:
    GCModifyMoney() {};
    ~GCModifyMoney() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_MODIFY_MONEY;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGold;
    }

    // get packet name
    string getPacketName() const {
        return "GCModifyMoney";
    }

    // get packet's debug string
    string toString() const;

    // get/set amount
    Gold_t getAmount() const {
        return m_Amount;
    }
    void setAmount(Gold_t amount) {
        m_Amount = amount;
    }


private:
    // Amount
    Gold_t m_Amount;
};


//////////////////////////////////////////////////////////////////////
//
// class GCModifyMoneyFactory;
//
// Factory for GCModifyMoney
//
//////////////////////////////////////////////////////////////////////

class GCModifyMoneyFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCModifyMoney();
    }

    // get packet name
    string getPacketName() const {
        return "GCModifyMoney";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_MODIFY_MONEY;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szGold;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCModifyMoney;
//
//////////////////////////////////////////////////////////////////////

class GCModifyMoneyHandler {
public:
    // execute packet's handler
    static void execute(GCModifyMoney* pPacket, Player* pPlayer);
};

#endif
