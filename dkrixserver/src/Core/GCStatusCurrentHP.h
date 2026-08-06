//////////////////////////////////////////////////////////////////////
//
// Filename    : GCStatusCurrentHP.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_STATUS_CURRENT_HP_H__
#define __GC_STATUS_CURRENT_HP_H__

// include files
#include "EffectInfo.h"
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCStatusCurrentHP;
//
////////////////////////////////////////////////////////////////////

class GCStatusCurrentHP : public Packet {
public:
    GCStatusCurrentHP();

    virtual ~GCStatusCurrentHP();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_STATUS_CURRENT_HP;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szHP;
    }

    // get packet's name
    string getPacketName() const {
        return "GCStatusCurrentHP";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set creature ID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t creatureID) {
        m_ObjectID = creatureID;
    }

    // get /set CurrentHP
    HP_t getCurrentHP() const {
        return m_CurrentHP;
    }
    void setCurrentHP(HP_t CurrentHP) {
        m_CurrentHP = CurrentHP;
    }

private:
    
    ObjectID_t m_ObjectID;

    
    HP_t m_CurrentHP;
};


//////////////////////////////////////////////////////////////////////
//
// class GCStatusCurrentHPFactory;
//
// Factory for GCStatusCurrentHP
//
//////////////////////////////////////////////////////////////////////

class GCStatusCurrentHPFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCStatusCurrentHP();
    }

    // get packet name
    string getPacketName() const {
        return "GCStatusCurrentHP";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_STATUS_CURRENT_HP;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szHP;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCStatusCurrentHPHandler;
//
//////////////////////////////////////////////////////////////////////

class GCStatusCurrentHPHandler {
public:
    // execute packet's handler
    static void execute(GCStatusCurrentHP* pPacket, Player* pPlayer);
};

#endif
