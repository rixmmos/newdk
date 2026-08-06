//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackMeleeOK2.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ATTACK_MELEE_OK_2_H__
#define __GC_ATTACK_MELEE_OK_2_H__

// include files
#include "Exception.h"
#include "ModifyInfo.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCAttackMeleeOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCAttackMeleeOK2 : public ModifyInfo {
public:
    // constructor
    GCAttackMeleeOK2();

    // destructor
    ~GCAttackMeleeOK2();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_ATTACK_MELEE_OK_2;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + ModifyInfo::getPacketSize();
    }

    // get packet's name
    string getPacketName() const {
        return "GCAttackMeleeOK2";
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
// class GCAttackMeleeOK2Factory;
//
// Factory for GCAttackMeleeOK2
//
//////////////////////////////////////////////////////////////////////

class GCAttackMeleeOK2Factory : public PacketFactory {
public:
    // constructor
    GCAttackMeleeOK2Factory() {}

    // destructor
    virtual ~GCAttackMeleeOK2Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCAttackMeleeOK2();
    }

    // get packet name
    string getPacketName() const {
        return "GCAttackMeleeOK2";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ATTACK_MELEE_OK_2;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + ModifyInfo::getPacketMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackMeleeOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCAttackMeleeOK2Handler {
public:
    // execute packet's handler
    static void execute(GCAttackMeleeOK2* pGCAttackMeleeOK2, Player* pPlayer);
};

#endif
