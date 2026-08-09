//////////////////////////////////////////////////////////////////////
//
// Filename    : CGDropMoney.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_DROP_MONEY_H__
#define __CG_DROP_MONEY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGDropMoney;
//
//////////////////////////////////////////////////////////////////////

class CGDropMoney : public Packet {
public:
    // constructor
    CGDropMoney();

    // destructor
    ~CGDropMoney();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_DROP_MONEY;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szGold;
    }

    // get packet name
    string getPacketName() const {
        return "CGDropMoney";
    }

    // get packet's debug string
    string toString() const;

public:
    Gold_t getAmount(void) const {
        return m_Amount;
    }
    void setAmount(Gold_t amount) {
        m_Amount = amount;
    }

private:
    Gold_t m_Amount;
};


//////////////////////////////////////////////////////////////////////
//
// class CGDropMoneyFactory;
//
// Factory for CGDropMoney
//
//////////////////////////////////////////////////////////////////////

class CGDropMoneyFactory : public PacketFactory {
public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGDropMoney();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGDropMoney";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DROP_MONEY;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const throw() {
        return szGold;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGDropMoneyHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGDropMoneyHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGDropMoneyHandler {
public:
    // execute packet's handler
    static void execute(CGDropMoney* pPacket, Player* player);
};
#endif

#endif
