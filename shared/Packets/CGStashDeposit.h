////////////////////////////////////////////////////////////////////////////////
// Filename    : CGStashDeposit.h
// Written By  : 김성민
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_STASH_DEPOSIT_H__
#define __CG_STASH_DEPOSIT_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashDeposit;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashDeposit : public Packet {
public:
    CGStashDeposit() throw() {}
    virtual ~CGStashDeposit() throw() {}
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_STASH_DEPOSIT;
    }
    PacketSize_t getPacketSize() const throw() {
        return szGold;
    }
    string getPacketName() const {
        return "CGStashDeposit";
    }
    string toString() const;

public:
    Gold_t getAmount(void) const throw() {
        return m_Amount;
    }
    void setAmount(Gold_t amount) throw() {
        m_Amount = amount;
    }

private:
    Gold_t m_Amount;
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashDepositFactory;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashDepositFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGStashDeposit();
    }
    string getPacketName() const {
        return "CGStashDeposit";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_STASH_DEPOSIT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szGold;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashDepositHandler;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashDepositHandler {
public:
    // execute packet's handler
    static void execute(CGStashDeposit* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
