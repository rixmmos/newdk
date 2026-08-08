//////////////////////////////////////////////////////////////////////
//
// Filename    : CGStoreSign.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_STORE_SIGN_H__
#define __CG_STORE_SIGN_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGStoreSign;
//
//////////////////////////////////////////////////////////////////////

class CGStoreSign : public Packet {
public:
    CGStoreSign() {};
    virtual ~CGStoreSign() {};
    // Initialize packet by reading data from the incoming stream.
    void read(SocketInputStream& iStream);

    // Serialize packet data to the outgoing stream.
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_STORE_SIGN;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Sign.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGStoreSign";
    }

    // get packet's debug string
    string toString() const;

    string getSign() const {
        return m_Sign;
    }
    void setSign(const string& sign) {
        m_Sign = sign;
    }

private:
    string m_Sign;
};


//////////////////////////////////////////////////////////////////////
//
// class CGStoreSignFactory;
//
// Factory for CGStoreSign
//
//////////////////////////////////////////////////////////////////////

class CGStoreSignFactory : public PacketFactory {
public:
    // constructor
    CGStoreSignFactory() {}

    // destructor
    virtual ~CGStoreSignFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGStoreSign();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGStoreSign";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_STORE_SIGN;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 80;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGStoreSignHandler;
//
//////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGStoreSignHandler::execute (the "CGStoreOpen precedent"). Keeping
// it unconditional here means CGHandlersStub.cpp needs no structural
// change — only its #include and exception spec were updated.
class CGStoreSignHandler {
public:
    // execute packet's handler
    static void execute(CGStoreSign* pCGStoreSign, Player* pPlayer);
};

#endif
