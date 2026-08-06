//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPhoneConnectionFailed.h
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_PHONE_CONNECTION_FAILED_H__
#define __GC_PHONE_CONNECTION_FAILED_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectionFailed;
//


//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectionFailed : public Packet {
public:
    // constructor
    GCPhoneConnectionFailed();

    // destructor
    ~GCPhoneConnectionFailed();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_PHONE_CONNECTION_FAILED;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet's name
    string getPacketName() const {
        return "GCPhoneConnectionFailed";
    }

    // get packet's debug string
    string toString() const;

private:
};

//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectionFailedFactory;
//
// Factory for GCPhoneConnectionFailed
//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectionFailedFactory : public PacketFactory {
public:
    // constructor
    GCPhoneConnectionFailedFactory() {}

    // destructor
    virtual ~GCPhoneConnectionFailedFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCPhoneConnectionFailed();
    }

    // get packet name
    string getPacketName() const {
        return "GCPhoneConnectionFailed";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_PHONE_CONNECTION_FAILED;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCPhoneConnectionFailedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCPhoneConnectionFailedHandler {
public:
    // execute packet's handler
    static void execute(GCPhoneConnectionFailed* pGCPhoneConnectionFailed, Player* pPlayer);
};

#endif
