//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMPRecoveryEnd.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MP_RECOVERY_END_H__
#define __GC_MP_RECOVERY_END_H__

// include files
#include "EffectInfo.h"
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryEnd;
//
////////////////////////////////////////////////////////////////////

class GCMPRecoveryEnd : public Packet {
public:
    GCMPRecoveryEnd();

    virtual ~GCMPRecoveryEnd();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_MP_RECOVERY_END;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szMP;
    }

    // get packet's name
    string getPacketName() const {
        return "GCMPRecoveryEnd";
    }

    // get packet's debug string
    string toString() const;

public:
    // get /set CurrentMP
    MP_t getCurrentMP() const {
        return m_CurrentMP;
    }
    void setCurrentMP(MP_t CurrentMP) {
        m_CurrentMP = CurrentMP;
    }

private:
    
    MP_t m_CurrentMP;
};


//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryEndFactory;
//
// Factory for GCMPRecoveryEnd
//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryEndFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCMPRecoveryEnd();
    }

    // get packet name
    string getPacketName() const {
        return "GCMPRecoveryEnd";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_MP_RECOVERY_END;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szMP;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryEndHandler;
//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryEndHandler {
public:
    // execute packet's handler
    static void execute(GCMPRecoveryEnd* pPacket, Player* pPlayer);
};

#endif
