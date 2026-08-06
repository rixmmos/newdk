//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHPRecoveryStartToSelf.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_HP_RECOVERY_START_TO_SELF_H__
#define __GC_HP_RECOVERY_START_TO_SELF_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToSelf;
//

//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToSelf : public Packet {
public:
    // constructor
    GCHPRecoveryStartToSelf();

    // destructor
    ~GCHPRecoveryStartToSelf();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_HP_RECOVERY_START_TO_SELF;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szBYTE + szHP + szHP;
    }

    // get packet's name
    string getPacketName() const {
        return "GCHPRecoveryStartToSelf";
    }

    // get packet's debug string
    string toString() const;

    // get / set Delay
    BYTE getDelay() const {
        return m_Delay;
    }
    void setDelay(BYTE Delay) {
        m_Delay = Delay;
    }

    // get / set Period
    HP_t getPeriod() const {
        return m_Period;
    }
    void setPeriod(HP_t Period) {
        m_Period = Period;
    }

    // get / set Quantity
    HP_t getQuantity() const {
        return m_Quantity;
    }
    void setQuantity(HP_t Quantity) {
        m_Quantity = Quantity;
    }

private:
    
    BYTE m_Delay;

    
    HP_t m_Period;

    
    HP_t m_Quantity;
};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToSelfFactory;
//
// Factory for GCHPRecoveryStartToSelf
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToSelfFactory : public PacketFactory {
public:
    // constructor
    GCHPRecoveryStartToSelfFactory() {}

    // destructor
    virtual ~GCHPRecoveryStartToSelfFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCHPRecoveryStartToSelf();
    }

    // get packet name
    string getPacketName() const {
        return "GCHPRecoveryStartToSelf";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_HP_RECOVERY_START_TO_SELF;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szHP + szHP;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToSelfHandler;
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToSelfHandler {
public:
    // execute packet's handler
    static void execute(GCHPRecoveryStartToSelf* pGCHPRecoveryStartToSelf, Player* pPlayer);
};

#endif
