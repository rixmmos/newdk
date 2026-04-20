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
    // Initialize the packet by reading data from the input stream (buffer).
    void read(SocketInputStream& iStream);

    // Write the packet's binary image to the output stream (buffer).
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
    // create packet
    // Base PacketFactory declares all four of these with throw() specs;
    // the override must be at least as strict or C++11 rejects it as a
    // looser exception specification. Keep throw() (not noexcept) to
    // match the rest of the codebase until a sweep bumps everything.
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

class CGStoreOpenHandler {
public:
    // execute packet's handler
    static void execute(CGStoreOpen* pCGStoreOpen, Player* pPlayer);
};

#endif
