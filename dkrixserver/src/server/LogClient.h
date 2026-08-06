//////////////////////////////////////////////////////////////////////////////
// Filename    : LogClient.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __LOGCLIENT_H__
#define __LOGCLIENT_H__

#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// LogType




//////////////////////////////////////////////////////////////////////////////

enum LogType {
    LOG_SYSTEM = 0,        
    LOG_SYSTEM_ERROR = 10, 

    LOG_GAMESERVER = 20,       
    LOG_GAMESERVER_ERROR = 30, 

    LOG_LOGINSERVER = 40,       
    LOG_LOGINSERVER_ERROR = 50, 

    LOG_SHAREDSERVER = 60,       
    LOG_SHAREDSERVER_ERROR = 70, 

    LOG_CGCONNECT = 80, 

    LOG_PICKUP_ITEM = 1010, 
    LOG_DROP_ITEM = 1020,   

    LOG_PICKUP_MONEY = 1030, 
    LOG_DROP_MONEY = 1040,   

    LOG_USE_ITEM = 1050,     
    LOG_CREATE_ITEM = 1060,  
    LOG_REPAIR_ITEM = 1065,  
    LOG_DESTROY_ITEM = 1070, 

    LOG_BUY_ITEM = 1080,          
    LOG_SELL_ITEM = 1090,         
    LOG_SHOP_CREATE_ITEM = 1100,  
    LOG_SHOP_DESTROY_ITEM = 1110, 

    LOG_DROP_ITEM_DIE = 1120,   
    LOG_DROP_ITEM_MORPH = 1130, 
    LOG_LOOT_CORPSE = 1140,     

    LOG_STASH_ADD_ITEM = 1150,     
    LOG_STASH_REMOVE_ITEM = 1160,  
    LOG_STASH_ADD_MONEY = 1170,    
    LOG_STASH_REMOVE_MONEY = 1180, 

    LOG_USE_BONUS_POINT = 1190, 

    LOG_TRADE = 1200, 

    LOG_KILL_PLAYER = 2000, 

    LOG_BLOODDRAIN = 2010,   
    LOG_BLOODDRAINED = 2020, 

    LOG_SLAYER_TO_VAMPIRE = 2030, 
    LOG_VAMPIRE_TO_SLAYER = 2040, 
    LOG_HEAL = 2050,              

    LOG_DEBUG_MSG = 3000, 

    LOGTYPE_MAX
};


//////////////////////////////////////////////////////////////////////////////
// class LogClient



//




//////////////////////////////////////////////////////////////////////////////

class LogClient {
    ///// member methods /////

public:
    LogClient(string ip, short port);
    virtual ~LogClient();

public:
    void connect(string ip, short port);
    void disconnect(void);

    void _log(short type, const string& source, const string& target);
    void _log(short type, const string& source, const string& target, const string& content);
    void _log(short type, const string& source, const string& target, const string& content, short ZoneID);

public:
    static int getLogLevel(void) {
        return m_LogLevel;
    }
    static void setLogLevel(int level) {
        m_LogLevel = level;
    }

    ///// member data /////

protected:
    int m_Socket;
    bool m_bConnected;
    long long m_Sent;

    static int m_LogLevel;
};

//////////////////////////////////////////////////////////////////////////////
// globals
//////////////////////////////////////////////////////////////////////////////

extern LogClient* g_pLogClient;

void log(short type, const string& source, const string& target);
void log(short type, const string& source, const string& target, const string& content);
void log(short type, const string& source, const string& target, const string& content, short ZoneID);


#endif
