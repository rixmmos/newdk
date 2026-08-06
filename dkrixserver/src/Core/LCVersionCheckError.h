//////////////////////////////////////////////////////////////////////
//
// Filename    : LCVersionCheckError.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_VERSION_CHECK_ERROR_H__
#define __LC_VERSION_CHECK_ERROR_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCVersionCheckError;
//
//
//////////////////////////////////////////////////////////////////////

class LCVersionCheckError : public Packet {
public:
    LCVersionCheckError() {};
    virtual ~LCVersionCheckError() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_VERSION_CHECK_ERROR;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet's name
    string getPacketName() const {
        return "LCVersionCheckError";
    }

    // get packet's debug string
    string toString() const;
};


//////////////////////////////////////////////////////////////////////
//
// class LCVersionCheckErrorFactory;
//
// Factory for LCVersionCheckError
//
//////////////////////////////////////////////////////////////////////

class LCVersionCheckErrorFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCVersionCheckError();
    }

    // get packet name
    string getPacketName() const {
        return "LCVersionCheckError";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_VERSION_CHECK_ERROR;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCVersionCheckErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class LCVersionCheckErrorHandler {
public:
    // execute packet's handler
    static void execute(LCVersionCheckError* pPacket, Player* pPlayer);
};

#endif
