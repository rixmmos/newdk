//////////////////////////////////////////////////////////////////////
//
// Filename    : GCBloodDrainOK3.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_BLOOD_DRAIN_OK_3_H__
#define __GC_BLOOD_DRAIN_OK_3_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK3;
//

//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK3 : public Packet {
public:
    // constructor
    GCBloodDrainOK3();

    // destructor
    ~GCBloodDrainOK3();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_BLOOD_DRAIN_OK_3;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szObjectID;
    }

    // get packet's name
    string getPacketName() const {
        return "GCBloodDrainOK3";
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

    // get / set ObjectID
    ObjectID_t getTargetObjectID() const {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t TargetObjectID) {
        m_TargetObjectID = TargetObjectID;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;

    // TargetObjectID
    ObjectID_t m_TargetObjectID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK3Factory;
//
// Factory for GCBloodDrainOK3
//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK3Factory : public PacketFactory {
public:
    // constructor
    GCBloodDrainOK3Factory() {}

    // destructor
    virtual ~GCBloodDrainOK3Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCBloodDrainOK3();
    }

    // get packet name
    string getPacketName() const {
        return "GCBloodDrainOK3";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_BLOOD_DRAIN_OK_3;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK3Handler;
//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK3Handler {
public:
    // execute packet's handler
    static void execute(GCBloodDrainOK3* pGCBloodDrainOK3, Player* pPlayer);
};

#endif
