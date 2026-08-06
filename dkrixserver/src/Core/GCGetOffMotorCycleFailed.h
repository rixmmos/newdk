//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGetOffMotorCycleFailed.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_GET_OFF_MOTORCYCLE_FAILED_H__
#define __GC_GET_OFF_MOTORCYCLE_FAILED_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleFailed;
//

//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleFailed : public Packet {
public:
    // constructor
    GCGetOffMotorCycleFailed();

    // destructor
    ~GCGetOffMotorCycleFailed();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_GET_OFF_MOTORCYCLE_FAILED;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet's name
    string getPacketName() const {
        return "GCGetOffMotorCycleFailed";
    }

    // get packet's debug string
    string toString() const;

private:
};


//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleFailedFactory;
//
// Factory for GCGetOffMotorCycleFailed
//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleFailedFactory : public PacketFactory {
public:
    // constructor
    GCGetOffMotorCycleFailedFactory() {}

    // destructor
    virtual ~GCGetOffMotorCycleFailedFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCGetOffMotorCycleFailed();
    }

    // get packet name
    string getPacketName() const {
        return "GCGetOffMotorCycleFailed";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_GET_OFF_MOTORCYCLE_FAILED;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleFailedHandler;
//
//////////////////////////////////////////////////////////////////////
class GCGetOffMotorCycleFailedHandler {
public:
    // execute packet's handler
    static void execute(GCGetOffMotorCycleFailed* pGCGetOffMotorCycleFailed, Player* pPlayer);
};

#endif
