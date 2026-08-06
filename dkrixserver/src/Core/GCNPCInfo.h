//--------------------------------------------------------------------------------
//
// Filename    : GCNPCInfo.h
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __GC_NPC_INFO_H__
#define __GC_NPC_INFO_H__

// include files
#include "Assert1.h"
#include "EffectInfo.h"
#include "ExtraInfo.h"
#include "GameTime.h"
#include "GearInfo.h"
#include "InventoryInfo.h"
#include "NPCInfo.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "RideMotorcycleInfo.h"

#define FLAG_PREMIUM_ZONE 0x10 
#define FLAG_PREMIUM_PLAY 0x01 

//--------------------------------------------------------------------------------
//
// class GCNPCInfo;
//



//
//--------------------------------------------------------------------------------

class GCNPCInfo : public Packet {
public:
    // constructor
    GCNPCInfo();

    // destructor
    ~GCNPCInfo();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_NPC_INFO;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        PacketSize_t size = 0;

        size += szBYTE;
        list<NPCInfo*>::const_iterator itr = m_NPCInfos.begin();
        for (; itr != m_NPCInfos.end(); itr++) {
            NPCInfo* pInfo = *itr;
            size += pInfo->getSize();
        }

        return size;
    }

    // get packet name
    string getPacketName() const {
        return "GCNPCInfo";
    }

    // get packet's debug string
    string toString() const;


    //--------------------------------------------------
    // methods
    //--------------------------------------------------
public:
    // get/set npc info
    void addNPCInfo(NPCInfo* pInfo) {
        m_NPCInfos.push_back(pInfo);
    }
    NPCInfo* popNPCInfo(void) {
        if (m_NPCInfos.empty())
            return NULL;
        NPCInfo* pInfo = m_NPCInfos.front();
        m_NPCInfos.pop_front();
        return pInfo;
    }

    //--------------------------------------------------
    // data members
    //--------------------------------------------------
private:
    
    list<NPCInfo*> m_NPCInfos;
};


//--------------------------------------------------------------------------------
//
// class GCNPCInfoFactory;
//
// Factory for GCNPCInfo
//
//--------------------------------------------------------------------------------

class GCNPCInfoFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCNPCInfo();
    }

    // get packet name
    string getPacketName() const {
        return "GCNPCInfo";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_NPC_INFO;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        PacketSize_t size = 0;

        size += szBYTE;
        size += NPCInfo::getMaxSize() * 255;

        return size;
    }
};


//--------------------------------------------------------------------------------
//
// class GCNPCInfoHandler;
//
//--------------------------------------------------------------------------------

class GCNPCInfoHandler {
public:
    // execute packet's handler
    static void execute(GCNPCInfo* pPacket, Player* pPlayer);
};

#endif
