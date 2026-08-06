////////////////////////////////////////////////////////////////////////////////
// Filename    : WeatherInfo.h
// Written By  : Reiot
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __WEATHER_INFO_H__
#define __WEATHER_INFO_H__

#include "Exception.h"
#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
// class WeatherInfo;
////////////////////////////////////////////////////////////////////////////////

class WeatherInfo {
public:
    
    Weather getWeather(uint probability) const;

    
    uint getProbability(Weather weather) const {
        return m_Probabilities[weather];
    }

    
    void setProbability(Weather weather, uint prob) {
        m_Probabilities[weather] = prob;
    }

    // get debug string
    string toString() const;

private:
    uint m_Probabilities[WEATHER_MAX];
};


////////////////////////////////////////////////////////////////////////////////
// class WeatherInfoManager;
//

////////////////////////////////////////////////////////////////////////////////

class WeatherInfoManager {
public:
    // init vision info
    void init() {
        load();
    }

    // load from database
    void load();

    // save to database
    void save() {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }

    // get vision info
    const WeatherInfo& getWeatherInfo(int month) const {
        if (month > 12)
            throw OutOfBoundException("too large month value");
        return m_WeatherInfos[month - 1];
    }

    // get debug string
    string toString() const;

private:
    
    WeatherInfo m_WeatherInfos[12];
};

// global variable declaration
extern WeatherInfoManager* g_pWeatherInfoManager;

#endif
