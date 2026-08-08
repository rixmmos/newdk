//////////////////////////////////////////////////////////////////////
//
// Filename    : CGStoreOpen.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_STORE_OPEN_H__
#define __CG_STORE_OPEN_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGStoreOpen;
//
//////////////////////////////////////////////////////////////////////

class CGStoreOpen : public Packet {
public:
    CGStoreOpen() {};
    virtual ~CGStoreOpen() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_STORE_OPEN;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet name
    string getPacketName() const {
        return "CGStoreOpen";
    }

    // get packet's debug string
    string toString() const;
};


//////////////////////////////////////////////////////////////////////
//
// class CGStoreOpenFactory;
//
// Factory for CGStoreOpen
//
//////////////////////////////////////////////////////////////////////

class CGStoreOpenFactory : public PacketFactory {
public:
    // constructor
    CGStoreOpenFactory() {}

    // destructor
    virtual ~CGStoreOpenFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGStoreOpen();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGStoreOpen";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_STORE_OPEN;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGStoreOpenHandler;
//
//////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGStoreOpenHandler::execute (the "CGStoreOpen precedent" — this pair's
// own namesake). Keeping it unconditional here means CGHandlersStub.cpp
// needs no structural change — only its #include and exception spec
// were updated.
class CGStoreOpenHandler {
public:
    // execute packet's handler
    static void execute(CGStoreOpen* pCGStoreOpen, Player* pPlayer);
};

#endif
