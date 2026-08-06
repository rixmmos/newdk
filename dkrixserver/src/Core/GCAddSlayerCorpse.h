//----------------------------------------------------------------------
//
// Filename    : GCAddSlayerCorpse.h
// Written By  : Reiot
//
//----------------------------------------------------------------------

#ifndef __GC_ADD_SLAYER_CORPSE_H__
#define __GC_ADD_SLAYER_CORPSE_H__

// include files
#include "PCSlayerInfo3.h"
#include "Packet.h"
#include "PacketFactory.h"

//----------------------------------------------------------------------
//
// class GCAddSlayerCorpse;
//

//
//----------------------------------------------------------------------

class GCAddSlayerCorpse : public Packet {
public:
    // constructor
    GCAddSlayerCorpse() {
        m_TreasureCount = 0;
    }
    GCAddSlayerCorpse(const PCSlayerInfo3& slayerInfo) : m_SlayerInfo(slayerInfo) {}
    ~GCAddSlayerCorpse() {};


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_ADD_SLAYER_CORPSE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return m_SlayerInfo.getSize() + szBYTE;
    }

    // get packet's name
    string getPacketName() const {
        return "GCAddSlayerCorpse";
    }

    // get packet's debug string
    string toString() const;


public:
    // get slayer info
    PCSlayerInfo3& getSlayerInfo() {
        return m_SlayerInfo;
    }
    const PCSlayerInfo3& getSlayerInfo() const {
        return m_SlayerInfo;
    }
    void setSlayerInfo(const PCSlayerInfo3& slayerInfo) {
        m_SlayerInfo = slayerInfo;
    }

    // get/set Treasure Count
    BYTE getTreasureCount() const {
        return m_TreasureCount;
    }
    void setTreasureCount(BYTE Count) {
        m_TreasureCount = Count;
    }

private:
    
    PCSlayerInfo3 m_SlayerInfo;

    
    BYTE m_TreasureCount;
};


//--------------------------------------------------------------------------------
//
// class GCAddSlayerCorpseFactory;
//
// Factory for GCAddSlayerCorpse
//
//--------------------------------------------------------------------------------

class GCAddSlayerCorpseFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCAddSlayerCorpse();
    }

    // get packet name
    string getPacketName() const {
        return "GCAddSlayerCorpse";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ADD_SLAYER_CORPSE;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return PCSlayerInfo3::getMaxSize() + szBYTE;
    }
};


//--------------------------------------------------------------------------------
//
// class GCAddSlayerCorpseHandler;
//
//--------------------------------------------------------------------------------

class GCAddSlayerCorpseHandler {
public:
    // execute packet's handler
    static void execute(GCAddSlayerCorpse* pPacket, Player* pPlayer);
};

#endif
