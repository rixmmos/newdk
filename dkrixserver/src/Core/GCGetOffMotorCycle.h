//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGetOffMotorCycle.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_GET_OFF_MOTORCYCLE_H__
#define __GC_GET_OFF_MOTORCYCLE_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycle;
//

//
//////////////////////////////////////////////////////////////////////

class GCGetOffMotorCycle : public Packet {
public:
    // constructor
    GCGetOffMotorCycle();

    // destructor
    ~GCGetOffMotorCycle();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_GET_OFF_MOTORCYCLE;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet's name
    string getPacketName() const {
        return "GCGetOffMotorCycle";
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
// class GCGetOffMotorCycleFactory;
//
// Factory for GCGetOffMotorCycle
//
//////////////////////////////////////////////////////////////////////

class GCGetOffMotorCycleFactory : public PacketFactory {
public:
    // constructor
    GCGetOffMotorCycleFactory() {}

    // destructor
    virtual ~GCGetOffMotorCycleFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCGetOffMotorCycle();
    }

    // get packet name
    string getPacketName() const {
        return "GCGetOffMotorCycle";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_GET_OFF_MOTORCYCLE;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCGetOffMotorCycleHandler;
//
//////////////////////////////////////////////////////////////////////

class GCGetOffMotorCycleHandler {
public:
    // execute packet's handler
    static void execute(GCGetOffMotorCycle* pGCGetOffMotorCycle, Player* pPlayer);
};

#endif
