//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCrossCounterOK1.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CROSS_COUNTER_OK_1_H__
#define __GC_CROSS_COUNTER_OK_1_H__

// include files
#include "Exception.h"
#include "ModifyInfo.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK1;
//

//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK1 : public ModifyInfo {
public:
    // constructor
    GCCrossCounterOK1();

    // destructor
    ~GCCrossCounterOK1();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_CROSS_COUNTER_OK_1;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + ModifyInfo::getPacketSize() + szSkillType;
    }

    // get packet's name
    string getPacketName() const {
        return "GCCrossCounterOK1";
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

    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;

    // Counter SkillType
    SkillType_t m_SkillType;
};


//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK1Factory;
//
// Factory for GCCrossCounterOK1
//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK1Factory : public PacketFactory {
public:
    // constructor
    GCCrossCounterOK1Factory() {}

    // destructor
    virtual ~GCCrossCounterOK1Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCCrossCounterOK1();
    }

    // get packet name
    string getPacketName() const {
        return "GCCrossCounterOK1";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_CROSS_COUNTER_OK_1;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + ModifyInfo::getPacketMaxSize() + szSkillType;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK1Handler;
//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK1Handler {
public:
    // execute packet's handler
    static void execute(GCCrossCounterOK1* pGCCrossCounterOK1, Player* pPlayer);
};

#endif
