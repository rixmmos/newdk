//--------------------------------------------------------------------------------
//
// Filename    : GCChangeDarkLight.h
// Written By  : reiot
//
//--------------------------------------------------------------------------------

#ifndef __GC_CHANGE_DARK_LIGHT_H__
#define __GC_CHANGE_DARK_LIGHT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCChangeDarkLight;
//
//--------------------------------------------------------------------------------

class GCChangeDarkLight : public Packet {
public:
    GCChangeDarkLight() {};
    ~GCChangeDarkLight() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_CHANGE_DARK_LIGHT;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szDarkLevel + szLightLevel;
    }

    // get packet's name
    string getPacketName() const {
        return "GCChangeDarkLight";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set dark level
    DarkLevel_t getDarkLevel() const {
        return m_DarkLevel;
    }
    void setDarkLevel(DarkLevel_t darkLevel) {
        m_DarkLevel = darkLevel;
    }

    // get/set light level
    LightLevel_t getLightLevel() const {
        return m_LightLevel;
    }
    void setLightLevel(LightLevel_t lightLevel) {
        m_LightLevel = lightLevel;
    }


public:
    
    DarkLevel_t m_DarkLevel;

    
    LightLevel_t m_LightLevel;
};


//--------------------------------------------------------------------------------
//
// class GCChangeDarkLightFactory;
//
// Factory for GCChangeDarkLight
//
//--------------------------------------------------------------------------------

class GCChangeDarkLightFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCChangeDarkLight();
    }

    // get packet name
    string getPacketName() const {
        return "GCChangeDarkLight";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_CHANGE_DARK_LIGHT;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szDarkLevel + szLightLevel;
    }
};


//--------------------------------------------------------------------------------
//
// class GCChangeDarkLightHandler;
//
//--------------------------------------------------------------------------------

class GCChangeDarkLightHandler {
public:
    // execute packet's handler
    static void execute(GCChangeDarkLight* pPacket, Player* pPlayer);
};

#endif
