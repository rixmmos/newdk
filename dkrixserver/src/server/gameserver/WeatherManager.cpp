//--------------------------------------------------------------------------------
//
// Filename   : WeatherManager.cpp
// Written By : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "WeatherManager.h"

#include "DarkLightInfo.h"
#include "GCChangeDarkLight.h"
#include "GCChangeWeather.h"
#include "GCLightning.h"
#include "GameTime.h"
#include "LogClient.h"
#include "PKZoneInfoManager.h"
#include "TimeManager.h"
#include "WeatherInfo.h"
#include "Zone.h"

//--------------------------------------------------------------------------------
//
// destructor
//

//
//--------------------------------------------------------------------------------
WeatherManager::~WeatherManager()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}


//--------------------------------------------------------------------------------
// initialize current zone's weather, darklevel, lightlevel
//--------------------------------------------------------------------------------
void WeatherManager::init()

{
    __BEGIN_TRY

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------

    
    GameTime gametime = g_pTimeManager->getGameTime();

    
    // const WeatherInfo & weatherInfo = g_pWeatherInfoManager->getWeatherInfo(gametime.getMonth());

    
    // m_TodayWeather = weatherInfo.getWeather(Dice(1,100));

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------

    
    time_t gmtime = g_pTimeManager->getgametime();

    
    tm ltm;
    localtime_r(&gmtime, &ltm);
    // struct tm* ptm = localtime(&gmtime);

    
    int dSec = (23 - ltm.tm_hour) * 3600 + (59 - ltm.tm_min) * 60 + (60 - ltm.tm_sec);

    
    time_t currentTime = time(0);
    m_Tomorrow = currentTime + dSec / 5;

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    

    m_TodayWeather = WEATHER_CLEAR;
    m_Probability = 0;
    m_CurrentWeather = WEATHER_CLEAR;
    m_WeatherLevel = 0;

    m_NextWeatherChangingTime = m_Tomorrow;
    m_NextLightning = m_Tomorrow;

     

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    DarkLightInfo* pDIInfo = g_pDarkLightInfoManager->getCurrentDarkLightInfo(m_pZone);
    m_pZone->setDarkLevel(pDIInfo->getDarkLevel());
    m_pZone->setLightLevel(pDIInfo->getLightLevel());

    
    dSec = (9 - ltm.tm_min / 10) * 60 + (60 - ltm.tm_sec);
    m_Next10Min = currentTime + dSec / 5;

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void WeatherManager::heartbeat()

{
    
    if (m_pZone->getZoneType() != ZONE_NORMAL_FIELD)
        return;

    
    if (g_pPKZoneInfoManager->isPKZone(m_pZone->getZoneID()))
        return;

    time_t currentTime = time(0);

    //--------------------------------------------------------------------------------
    
    
    
    //--------------------------------------------------------------------------------
    if (currentTime > m_Tomorrow) {
        
        GameTime gametime = g_pTimeManager->getGameTime();

        
        const WeatherInfo& weatherInfo = g_pWeatherInfoManager->getWeatherInfo(gametime.getMonth());

        
        m_TodayWeather = weatherInfo.getWeather(Dice(1, 100));

        
        
        
        if (m_TodayWeather == WEATHER_CLEAR) {
            
            m_Probability = 0;
        } else {
            m_Probability = Dice(3, 100) / 3;
        }

        
        time_t gmtime = g_pTimeManager->getgametime();

        
        tm ltm;
        localtime_r(&gmtime, &ltm);
        // struct tm* ptm = localtime(&gmtime);

        
        int dSec = (23 - ltm.tm_hour) * 3600 + (59 - ltm.tm_min) * 60 + (59 - ltm.tm_sec);

        
        m_Tomorrow = currentTime + dSec / 5;
    }

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    if (currentTime > m_NextWeatherChangingTime) {
        if (m_TodayWeather == WEATHER_CLEAR) {
            
            m_CurrentWeather = WEATHER_CLEAR;
            m_WeatherLevel = 0;
            m_NextWeatherChangingTime = m_Tomorrow;
            m_NextLightning = m_Tomorrow / 2;
        } else {
            m_CurrentWeather = (Dice(1, 100) < m_Probability) ? m_TodayWeather : WEATHER_CLEAR;
            m_WeatherLevel = (m_CurrentWeather != WEATHER_CLEAR) ? Dice(3, 20) / 3 : 0;

            
            // m_NextWeatherChangingTime = time(0) + Dice(1,20)* 60;
            m_NextWeatherChangingTime = time(0) + 60;

            
            // m_NextLightning = time(0) + 60;
            m_NextLightning = time(0) + 20;
        }

        GCChangeWeather gcChangeWeather;
        gcChangeWeather.setWeather(m_CurrentWeather);
        gcChangeWeather.setWeatherLevel(m_WeatherLevel);

        StringStream msg;
        msg << "ZONE[" << m_pZone->getZoneID() << "] : " << gcChangeWeather.toString();
        log(LOG_DEBUG_MSG, "", "", msg.toString());

        m_pZone->broadcastPacket(&gcChangeWeather, NULL);
    }

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    if (m_CurrentWeather == WEATHER_RAINY && currentTime > m_NextLightning) {
        
        
        
        
        if (Dice(1, 100) < (uint)max(0, m_WeatherLevel * 5 - 30)) {
            GCLightning gcLightning;
            gcLightning.setDelay(Dice(1, 5));
            m_pZone->broadcastPacket(&gcLightning, NULL);
        }

        
        
        m_NextLightning += 60;
    }

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    if (currentTime > m_Next10Min) {
        DarkLightInfo* pDIInfo = g_pDarkLightInfoManager->getCurrentDarkLightInfo(m_pZone);

        DarkLevel_t darkLevel = pDIInfo->getDarkLevel();
        LightLevel_t lightLevel = pDIInfo->getLightLevel();

        
        if (darkLevel != m_pZone->getDarkLevel() || lightLevel != m_pZone->getLightLevel()) {
            m_pZone->setDarkLevel(darkLevel);
            m_pZone->setLightLevel(lightLevel);

            GCChangeDarkLight gcChangeDarkLight;
            gcChangeDarkLight.setDarkLevel(darkLevel);
            gcChangeDarkLight.setLightLevel(lightLevel);

            GCChangeDarkLight gcChangeDarkLight2;
            gcChangeDarkLight2.setDarkLevel(DARK_MAX - darkLevel);
            gcChangeDarkLight2.setLightLevel(LIGHT_MAX - lightLevel);

            m_pZone->broadcastDarkLightPacket(&gcChangeDarkLight, &gcChangeDarkLight2, NULL);

            // cout << "(DarkLevel/LightLevel) : (" << (int)darkLevel << "," << (int)lightLevel << ") at " <<
            // g_pTimeManager->getGameTime().toString() << endl;
        } else {
            // cout << "(DarkLevel/LightLevel) : (" << (int)darkLevel << "," << (int)lightLevel << ") at " <<
            // g_pTimeManager->getGameTime().toString() << endl;
        }

        m_Next10Min += 120;
    }
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string WeatherManager::toString() const

{
    StringStream msg;

    msg << "WeatherManager(";
    msg << "      : " << Weather2String[m_TodayWeather];
    msg << "        : " << Weather2String[m_CurrentWeather];
    msg << "/   : " << (int)m_Probability << "%";
    msg << "        : " << (int)m_WeatherLevel;

    time_t currentTime = time(0);

    msg << "        : " << ctime(&currentTime);
    msg << "    : " << ctime(&m_Tomorrow);
    msg << ": " << ctime(&m_NextWeatherChangingTime);
    msg << ": " << ctime(&m_NextLightning);

    return msg.toString();
}
