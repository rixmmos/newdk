//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackMeleeOK1.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ATTACK_MELEE_OK_1_H__
#define __GC_ATTACK_MELEE_OK_1_H__

// include files
#include "Exception.h"
#include "ModifyInfo.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCAttackMeleeOK1;
//

//
//////////////////////////////////////////////////////////////////////

class GCAttackMeleeOK1 : public ModifyInfo {
public:
    // constructor
    GCAttackMeleeOK1();

    // destructor
    ~GCAttackMeleeOK1();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_ATTACK_MELEE_OK_1;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + ModifyInfo::getPacketSize();
    }

    // get packet's name
    string getPacketName() const {
        return "GCAttackMeleeOK1";
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
// class GCAttackMeleeOK1Factory;
//
// Factory for GCAttackMeleeOK1
//
//////////////////////////////////////////////////////////////////////

class GCAttackMeleeOK1Factory : public PacketFactory {
public:
    // constructor
    GCAttackMeleeOK1Factory() {}

    // destructor
    virtual ~GCAttackMeleeOK1Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCAttackMeleeOK1();
    }

    // get packet name
    string getPacketName() const {
        return "GCAttackMeleeOK1";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ATTACK_MELEE_OK_1;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + ModifyInfo::getPacketMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackMeleeOK1Handler;
//
//////////////////////////////////////////////////////////////////////

class GCAttackMeleeOK1Handler {
public:
    // execute packet's handler
    static void execute(GCAttackMeleeOK1* pGCAttackMeleeOK1, Player* pPlayer);
};

#endif
