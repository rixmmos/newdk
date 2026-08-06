//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCrossCounterOK3.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CROSS_COUNTER_OK_3_H__
#define __GC_CROSS_COUNTER_OK_3_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK3;
//

//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK3 : public Packet {
public:
    // constructor
    GCCrossCounterOK3();

    // destructor
    ~GCCrossCounterOK3();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_CROSS_COUNTER_OK_3;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szObjectID + szSkillType;
    }

    // get packet's name
    string getPacketName() const {
        return "GCCrossCounterOK3";
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

    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;

    // TargetObjectID
    ObjectID_t m_TargetObjectID;

    // CounterSkillType
    SkillType_t m_SkillType;
};


//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK3Factory;
//
// Factory for GCCrossCounterOK3
//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK3Factory : public PacketFactory {
public:
    // constructor
    GCCrossCounterOK3Factory() {}

    // destructor
    virtual ~GCCrossCounterOK3Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCCrossCounterOK3();
    }

    // get packet name
    string getPacketName() const {
        return "GCCrossCounterOK3";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_CROSS_COUNTER_OK_3;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szObjectID + szSkillType;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK3Handler;
//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK3Handler {
public:
    // execute packet's handler
    static void execute(GCCrossCounterOK3* pGCCrossCounterOK3, Player* pPlayer);
};

#endif
