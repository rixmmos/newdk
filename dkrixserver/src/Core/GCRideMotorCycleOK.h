//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRideMotorCycleOK.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_RIDE_MOTORCYCLE_OK_H__
#define __GC_RIDE_MOTORCYCLE_OK_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleOK;
//

//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleOK : public Packet {
public:
    // constructor
    GCRideMotorCycleOK();

    // destructor
    ~GCRideMotorCycleOK();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_RIDE_MOTORCYCLE_OK;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet's name
    string getPacketName() const {
        return "GCRideMotorCycleOK";
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
// class GCRideMotorCycleOKFactory;
//
// Factory for GCRideMotorCycleOK
//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleOKFactory : public PacketFactory {
public:
    // constructor
    GCRideMotorCycleOKFactory() {}

    // destructor
    virtual ~GCRideMotorCycleOKFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCRideMotorCycleOK();
    }

    // get packet name
    string getPacketName() const {
        return "GCRideMotorCycleOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_RIDE_MOTORCYCLE_OK;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCRideMotorCycleOKHandler;
//
//////////////////////////////////////////////////////////////////////
class GCRideMotorCycleOKHandler {
public:
    // execute packet's handler
    static void execute(GCRideMotorCycleOK* pGCRideMotorCycleOK, Player* pPlayer);
};

#endif
