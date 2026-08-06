//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHPRecoveryStartToOthers.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_HP_RECOVERY_START_TO_OTHERS_H__
#define __GC_HP_RECOVERY_START_TO_OTHERS_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToOthers;
//

//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToOthers : public Packet {
public:
    // constructor
    GCHPRecoveryStartToOthers();

    // destructor
    ~GCHPRecoveryStartToOthers();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_HP_RECOVERY_START_TO_OTHERS;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE + szHP + szHP;
    }

    // get packet's name
    string getPacketName() const {
        return "GCHPRecoveryStartToOthers";
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
    // ObjectID
    ObjectID_t m_ObjectID;

    
    BYTE m_Delay;

    
    HP_t m_Period;

    
    HP_t m_Quantity;
};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToOthersFactory;
//
// Factory for GCHPRecoveryStartToOthers
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToOthersFactory : public PacketFactory {
public:
    // constructor
    GCHPRecoveryStartToOthersFactory() {}

    // destructor
    virtual ~GCHPRecoveryStartToOthersFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCHPRecoveryStartToOthers();
    }

    // get packet name
    string getPacketName() const {
        return "GCHPRecoveryStartToOthers";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_HP_RECOVERY_START_TO_OTHERS;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szBYTE + szHP + szHP;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToOthersHandler;
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToOthersHandler {
public:
    // execute packet's handler
    static void execute(GCHPRecoveryStartToOthers* pGCHPRecoveryStartToOthers, Player* pPlayer);
};

#endif
