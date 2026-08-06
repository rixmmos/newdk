//--------------------------------------------------------------------------------
//
// Filename    : GCChangeWeather.h
// Written By  : reiot
//
//--------------------------------------------------------------------------------

#ifndef __GC_CHANGE_WEATHER_H__
#define __GC_CHANGE_WEATHER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCChangeWeather;
//
//--------------------------------------------------------------------------------

class GCChangeWeather : public Packet {
public:
    GCChangeWeather() {};
    ~GCChangeWeather() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_CHANGE_WEATHER;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szWeather + szWeatherLevel;
    }

    // get packet's name
    string getPacketName() const {
        return "GCChangeWeather";
    }

    // get packet's debug string
    string toString() const;

public:
    Weather getWeather() const {
        return m_Weather;
    }
    void setWeather(Weather weather) {
        m_Weather = weather;
    }

    WeatherLevel_t getWeatherLevel() const {
        return m_WeatherLevel;
    }
    void setWeatherLevel(WeatherLevel_t weatherLevel) {
        m_WeatherLevel = weatherLevel;
    }

public:
    Weather m_Weather;

    WeatherLevel_t m_WeatherLevel;
};


//--------------------------------------------------------------------------------
//
// class GCChangeWeatherFactory;
//
// Factory for GCChangeWeather
//
//--------------------------------------------------------------------------------

class GCChangeWeatherFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCChangeWeather();
    }

    // get packet name
    string getPacketName() const {
        return "GCChangeWeather";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_CHANGE_WEATHER;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szWeather + szWeatherLevel;
    }
};


//--------------------------------------------------------------------------------
//
// class GCChangeWeatherHandler;
//
//--------------------------------------------------------------------------------

class GCChangeWeatherHandler {
public:
    // execute packet's handler
    static void execute(GCChangeWeather* pPacket, Player* pPlayer);
};

#endif
