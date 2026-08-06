//--------------------------------------------------------------------------------
//
// Filename   : WeatherManager.h
// Written By : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __WEATHER_MANAGER_H__
#define __WEATHER_MANAGER_H__

// include files
#include <time.h>

#include "Assert.h"
#include "Exception.h"
#include "Types.h"

class Zone;

//--------------------------------------------------------------------------------
//
// class WeatherManager;
//

//




//

//




//

//
//--------------------------------------------------------------------------------
class WeatherManager {
public:
    // constructor
    WeatherManager(Zone* pZone) : m_pZone(pZone) {
        Assert(m_pZone != NULL);
    }

    // destructor
    virtual ~WeatherManager();

    // initialize
    void init();

    
    void heartbeat();

    
    Weather getTodayWeather() const {
        return m_TodayWeather;
    }

    
    uint getProbability() const {
        return m_Probability;
    }

    
    Weather getCurrentWeather() const {
        return m_CurrentWeather;
    }

    
    WeatherLevel_t getWeatherLevel() const {
        return m_WeatherLevel;
    }

    void resetDarkLightInfo() {
        m_Next10Min = time(0);
    }

    // get debug string
    string toString() const;

private:
    
    Zone* m_pZone;

    
    Weather m_TodayWeather;

    
    uint m_Probability;

    
    Weather m_CurrentWeather;

    
    WeatherLevel_t m_WeatherLevel;


    //--------------------------------------------------
    
    //--------------------------------------------------
private:
    
    time_t m_Tomorrow;

    
    time_t m_NextWeatherChangingTime;

    
    time_t m_NextLightning;

    
    time_t m_Next10Min;
};

#endif
