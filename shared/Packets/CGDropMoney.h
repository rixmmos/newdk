//////////////////////////////////////////////////////////////////////
// Filename    : CGDropMoney.h
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_DROP_MONEY_H__
#define __CG_DROP_MONEY_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGDropMoney : public Packet {
public:
    CGDropMoney() throw();
    ~CGDropMoney() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_DROP_MONEY;
    }
    PacketSize_t getPacketSize() const throw() {
        return szGold;
    }
    string getPacketName() const throw() {
        return "CGDropMoney";
    }
    string toString() const throw();

    Gold_t getAmount() const throw() {
        return m_Amount;
    }
    void setAmount(Gold_t amount) throw() {
        m_Amount = amount;
    }

private:
    Gold_t m_Amount;
};

class CGDropMoneyFactory : public PacketFactory {
public:
    CGDropMoneyFactory() throw() {}
    virtual ~CGDropMoneyFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGDropMoney();
    }
    string getPacketName() const throw() {
        return "CGDropMoney";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DROP_MONEY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szGold;
    }
};

class CGDropMoneyHandler {
public:
    static void execute(CGDropMoney* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
