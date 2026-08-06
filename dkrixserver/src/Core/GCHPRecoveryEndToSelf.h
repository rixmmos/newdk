//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHPRecoveryEndToSelf.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_HP_RECOVERY_END_TO_SELF_H__
#define __GC_HP_RECOVERY_END_TO_SELF_H__

// include files
#include "EffectInfo.h"
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToSelf;
//
////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToSelf : public Packet {
public:
    GCHPRecoveryEndToSelf();

    virtual ~GCHPRecoveryEndToSelf();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_HP_RECOVERY_END_TO_SELF;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szHP;
    }

    // get packet's name
    string getPacketName() const {
        return "GCHPRecoveryEndToSelf";
    }

    // get packet's debug string
    string toString() const;

public:
    // get /set CurrentHP
    HP_t getCurrentHP() const {
        return m_CurrentHP;
    }
    void setCurrentHP(HP_t CurrentHP) {
        m_CurrentHP = CurrentHP;
    }

private:
    
    HP_t m_CurrentHP;
};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToSelfFactory;
//
// Factory for GCHPRecoveryEndToSelf
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToSelfFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCHPRecoveryEndToSelf();
    }

    // get packet name
    string getPacketName() const {
        return "GCHPRecoveryEndToSelf";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_HP_RECOVERY_END_TO_SELF;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szHP;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToSelfHandler;
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToSelfHandler {
public:
    // execute packet's handler
    static void execute(GCHPRecoveryEndToSelf* pPacket, Player* pPlayer);
};

#endif
