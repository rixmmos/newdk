//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMPRecoveryStart.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MP_RECOVERY_START_H__
#define __GC_MP_RECOVERY_START_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryStart;
//

//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryStart : public Packet {
public:
    // constructor
    GCMPRecoveryStart();

    // destructor
    ~GCMPRecoveryStart();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_MP_RECOVERY_START;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szBYTE + szMP + szMP;
    }

    // get packet's name
    string getPacketName() const {
        return "GCMPRecoveryStart";
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
    MP_t getPeriod() const {
        return m_Period;
    }
    void setPeriod(MP_t Period) {
        m_Period = Period;
    }

    // get / set Quantity
    MP_t getQuantity() const {
        return m_Quantity;
    }
    void setQuantity(MP_t Quantity) {
        m_Quantity = Quantity;
    }

private:
    
    BYTE m_Delay;

    
    MP_t m_Period;

    
    MP_t m_Quantity;
};


//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryStartFactory;
//
// Factory for GCMPRecoveryStart
//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryStartFactory : public PacketFactory {
public:
    // constructor
    GCMPRecoveryStartFactory() {}

    // destructor
    virtual ~GCMPRecoveryStartFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCMPRecoveryStart();
    }

    // get packet name
    string getPacketName() const {
        return "GCMPRecoveryStart";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_MP_RECOVERY_START;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szMP + szMP;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryStartHandler;
//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryStartHandler {
public:
    // execute packet's handler
    static void execute(GCMPRecoveryStart* pGCMPRecoveryStart, Player* pPlayer);
};

#endif
