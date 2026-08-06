//--------------------------------------------------------------------------------
//
// Filename    : GCMorph1.h
// Written By  : crazydog
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __GC_MORPH1_H__
#define __GC_MORPH1_H__

// include files
#include "Assert1.h"
#include "ExtraInfo.h"
#include "GameTime.h"
#include "GearInfo.h"
#include "InventoryInfo.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCMorph1;
//

//--------------------------------------------------------------------------------

class GCMorph1 : public Packet {
public:
    // constructor
    GCMorph1();

    // destructor
    ~GCMorph1();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_MORPH_1;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        // [PCType][PCInfo]
        // [GameTime][Weather][WeatherLevel][DarkLevel][LightLevel]
        // [#NPCTypes][NPCType1]...[NPCTypeN]
        // [#MonsterTypes][MonsterType1]...[MonsterTypeM]
        return m_pPCInfo->getSize() + m_pInventoryInfo->getSize() + m_pGearInfo->getSize() + m_pExtraInfo->getSize();
    }

    // get packet name
    string getPacketName() const {
        return "GCMorph1";
    }

    // get packet's debug string
    string toString() const;


    //--------------------------------------------------
    // methods
    //--------------------------------------------------
public:
    // get/set PC info
    PCInfo* getPCInfo2() const {
        return m_pPCInfo;
    }
    void setPCInfo2(PCInfo* pPCInfo) {
        m_pPCInfo = pPCInfo;
    }

    // get/set Inventory Info
    InventoryInfo* getInventoryInfo() const {
        return m_pInventoryInfo;
    }
    void setInventoryInfo(InventoryInfo* pInventoryInfo) {
        m_pInventoryInfo = pInventoryInfo;
    }

    // get/set Gear Info
    GearInfo* getGearInfo() const {
        return m_pGearInfo;
    }
    void setGearInfo(GearInfo* pGearInfo) {
        m_pGearInfo = pGearInfo;
    }

    // get/set ExtraInfo
    ExtraInfo* getExtraInfo() const {
        return m_pExtraInfo;
    }
    void setExtraInfo(ExtraInfo* pExtraInfo) {
        m_pExtraInfo = pExtraInfo;
    }

    //--------------------------------------------------
    // data members
    //--------------------------------------------------
private:
    //--------------------------------------------------------------------------------
    // PC Information
    //--------------------------------------------------------------------------------
    
    PCInfo* m_pPCInfo;

    //--------------------------------------------------------------------------------
    // Inventory Information
    //--------------------------------------------------------------------------------
    InventoryInfo* m_pInventoryInfo;

    //--------------------------------------------------------------------------------
    // Gear Information
    //--------------------------------------------------------------------------------
    GearInfo* m_pGearInfo;

    //--------------------------------------------------------------------------------
    // Extra Information
    //--------------------------------------------------------------------------------
    ExtraInfo* m_pExtraInfo;

    // inventory
    // quick item slot
    // gear

    
    
    
    
};


//--------------------------------------------------------------------------------
//
// class GCMorph1Factory;
//
// Factory for GCMorph1
//
//--------------------------------------------------------------------------------

class GCMorph1Factory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCMorph1();
    }

    // get packet name
    string getPacketName() const {
        return "GCMorph1";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_MORPH_1;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + PCSlayerInfo2::getMaxSize() + InventoryInfo::getMaxSize() + GearInfo::getMaxSize() +
               ExtraInfo::getMaxSize();
    }
};


//--------------------------------------------------------------------------------
//
// class GCMorph1Handler;
//
//--------------------------------------------------------------------------------

class GCMorph1Handler {
public:
    // execute packet's handler
    static void execute(GCMorph1* pPacket, Player* pPlayer);
};

#endif
