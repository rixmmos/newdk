//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHPRecoveryEndToOthers.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_HP_RECOVERY_END_TO_OTHERS_H__
#define __GC_HP_RECOVERY_END_TO_OTHERS_H__

// include files
#include "EffectInfo.h"
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToOthers;
//
////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToOthers : public Packet {
public:
    GCHPRecoveryEndToOthers();

    virtual ~GCHPRecoveryEndToOthers();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_HP_RECOVERY_END_TO_OTHERS;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szHP;
    }

    // get packet's name
    string getPacketName() const {
        return "GCHPRecoveryEndToOthers";
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
// class GCHPRecoveryEndToOthersFactory;
//
// Factory for GCHPRecoveryEndToOthers
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToOthersFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCHPRecoveryEndToOthers();
    }

    // get packet name
    string getPacketName() const {
        return "GCHPRecoveryEndToOthers";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_HP_RECOVERY_END_TO_OTHERS;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szHP;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToOthersHandler;
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToOthersHandler {
public:
    // execute packet's handler
    static void execute(GCHPRecoveryEndToOthers* pPacket, Player* pPlayer);
};

#endif
