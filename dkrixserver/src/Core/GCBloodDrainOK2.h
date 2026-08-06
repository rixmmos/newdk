//////////////////////////////////////////////////////////////////////
//
// Filename    : GCBloodDrainOK2.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_BLOOD_DRAIN_OK_2_H__
#define __GC_BLOOD_DRAIN_OK_2_H__

// include files
#include "Exception.h"
#include "ModifyInfo.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK2 : public ModifyInfo {
public:
    // constructor
    GCBloodDrainOK2();

    // destructor
    ~GCBloodDrainOK2();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_BLOOD_DRAIN_OK_2;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + ModifyInfo::getPacketSize();
    }

    // get packet's name
    string getPacketName() const {
        return "GCBloodDrainOK2";
    }

    // get packet's debug string
    string toString() const;

    // get / set CEffectID
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
// class GCBloodDrainOK2Factory;
//
// Factory for GCBloodDrainOK2
//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK2Factory : public PacketFactory {
public:
    // constructor
    GCBloodDrainOK2Factory() {}

    // destructor
    virtual ~GCBloodDrainOK2Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCBloodDrainOK2();
    }

    // get packet name
    string getPacketName() const {
        return "GCBloodDrainOK2";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_BLOOD_DRAIN_OK_2;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + ModifyInfo::getPacketMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodDrainOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCBloodDrainOK2Handler {
public:
    // execute packet's handler
    static void execute(GCBloodDrainOK2* pGCBloodDrainOK2, Player* pPlayer);
};

#endif
