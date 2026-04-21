////////////////////////////////////////////////////////////////////////////////
//
// Filename    : CGStashRequestBuy.h
// Written By  : 김성민
// Description :
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_STASH_REQUEST_BUY_H__
#define __CG_STASH_REQUEST_BUY_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashRequestBuy;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashRequestBuy : public Packet {
public:
    CGStashRequestBuy() throw() {}
    virtual ~CGStashRequestBuy() throw() {}
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_STASH_REQUEST_BUY;
    }
    PacketSize_t getPacketSize() const throw() {
        return 0;
    }
    string getPacketName() const {
        return "CGStashRequestBuy";
    }
    string toString() const;
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashRequestBuyFactory;
//
// Factory for CGStashRequestBuy
//
////////////////////////////////////////////////////////////////////////////////

class CGStashRequestBuyFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGStashRequestBuy();
    }
    string getPacketName() const {
        return "CGStashRequestBuy";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_STASH_REQUEST_BUY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashRequestBuyHandler;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashRequestBuyHandler {
public:
    CGStashRequestBuyHandler() throw() {}
    virtual ~CGStashRequestBuyHandler() throw() {}
    static void execute(CGStashRequestBuy* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
