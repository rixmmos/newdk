////////////////////////////////////////////////////////////////////////////////
// Filename    : CGDonationMoney.h

// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_DONATION_MONEY_H__
#define __CG_DONATION_MONEY_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
enum DonationType {
    DONATION_TYPE_200501_PERSONAL = 0,
    DONATION_TYPE_200501_GUILD,   // 1
    DONATION_TYPE_200505_WEDDING, // 2

    DONATION_TYPE_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGDonationMoney
//
////////////////////////////////////////////////////////////////////////////////
class CGDonationMoney : public Packet {
public:
    CGDonationMoney() {};
    virtual ~CGDonationMoney() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_DONATION_MONEY;
    }
    PacketSize_t getPacketSize() const {
        return szGold + szBYTE;
    }
    string getPacketName() const {
        return "CGDonationMoney";
    }
    string toString() const;

public:
    // get / set gold
    Gold_t getGold() const {
        return m_Gold;
    }
    void setGold(Gold_t gold) {
        m_Gold = gold;
    }

    // get / set donation Type
    BYTE getDonationType() const {
        return m_DonationType;
    }
    void setDonationType(BYTE donationType) {
        m_DonationType = donationType;
    }

private:
    Gold_t m_Gold;       
    BYTE m_DonationType; 
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGDonationMoneyFactory
//
////////////////////////////////////////////////////////////////////////////////

class CGDonationMoneyFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGDonationMoney();
    }
    string getPacketName() const throw() {
        return "CGDonationMoney";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DONATION_MONEY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szGold + szBYTE;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGDonationMoneyHandler
//
////////////////////////////////////////////////////////////////////////////////
// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGDonationMoneyHandler::execute (the "CGStoreOpen precedent" from the
// parked line, rather than the CLGetWorldList pilot's guard-and-omit
// approach). Keeping it unconditional here, matching the server's
// pre-migration style exactly, means CGHandlersStub.cpp needs no
// structural change — only its exception spec was updated to match this
// declaration (see CGHandlersStub.cpp).
class CGDonationMoneyHandler {
public:
    static void execute(CGDonationMoney* pPacket, Player* player);
};

#endif
