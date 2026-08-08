//////////////////////////////////////////////////////////////////////
//
// Filename    : CGWithdrawTax.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_WITHDRAW_TAX_H__
#define __CG_WITHDRAW_TAX_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGWithdrawTax;
//
//////////////////////////////////////////////////////////////////////

class CGWithdrawTax : public Packet {
public:
    CGWithdrawTax() {};
    virtual ~CGWithdrawTax() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_WITHDRAW_TAX;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szGold;
    }

    // get packet name
    string getPacketName() const {
        return "CGWithdrawTax";
    }

    // get packet's debug string
    string toString() const;

    // get/set Gold
    Gold_t getGold() const {
        return m_Gold;
    }
    void setGold(Gold_t gold) {
        m_Gold = gold;
    }


private:
    Gold_t m_Gold;
};


//////////////////////////////////////////////////////////////////////
//
// class CGWithdrawTaxFactory;
//
// Factory for CGWithdrawTax
//
//////////////////////////////////////////////////////////////////////

// Client Cpackets copy wrapped getPacketName()/toString() (only) in
// #ifdef __DEBUG_OUTPUT__; adopting the server's unconditional canonical
// style per Phase 12's reconciliation rules.
class CGWithdrawTaxFactory : public PacketFactory {
public:
    // constructor
    // Base PacketFactory declares these with throw() specs on the client
    // tree; narrowing to throw() here also satisfies the server tree's
    // unconstrained base. See CLGetWorldList.h (Phase 12 pilot) for the
    // precedent.
    CGWithdrawTaxFactory() throw() {}

    // destructor
    virtual ~CGWithdrawTaxFactory() throw() {}


public:
    // create packet
    Packet* createPacket() throw() {
        return new CGWithdrawTax();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGWithdrawTax";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_WITHDRAW_TAX;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szGold;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGWithdrawTaxHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGWithdrawTaxHandler::execute has no client-side definition
// or use. The client Cpackets copy left the class declaration itself
// unguarded and wrapped only the method declaration in
// #ifndef __GAME_CLIENT__ -- replicated exactly (a declared-but-never-
// ODR-used static method needs no definition, same as the
// CGSMSAddressList/CGGQuestAccept precedent, just with the guard drawn one
// line tighter).
class CGWithdrawTaxHandler {
public:
#ifndef __GAME_CLIENT__

    // execute packet's handler
    static void execute(CGWithdrawTax* pCGWithdrawTax, Player* pPlayer);
#endif
};

#endif
