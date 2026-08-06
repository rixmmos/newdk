//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRideMotorCycleFailed.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_RIDE_MOTORCYCLE_FAILED_H__
#define __GC_RIDE_MOTORCYCLE_FAILED_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleFailed;
//

//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleFailed : public Packet {
public:
    // constructor
    GCRideMotorCycleFailed();

    // destructor
    ~GCRideMotorCycleFailed();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_RIDE_MOTORCYCLE_FAILED;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet's name
    string getPacketName() const {
        return "GCRideMotorCycleFailed";
    }

    // get packet's debug string
    string toString() const;

    // get / set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleFailedFactory;
//
// Factory for GCRideMotorCycleFailed
//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleFailedFactory : public PacketFactory {
public:
    // constructor
    GCRideMotorCycleFailedFactory() {}

    // destructor
    virtual ~GCRideMotorCycleFailedFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCRideMotorCycleFailed();
    }

    // get packet name
    string getPacketName() const {
        return "GCRideMotorCycleFailed";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_RIDE_MOTORCYCLE_FAILED;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleFailedHandler;
//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleFailedHandler {
public:
    // execute packet's handler
    static void execute(GCRideMotorCycleFailed* pGCRideMotorCycleFailed, Player* pPlayer);
};

#endif
