//////////////////////////////////////////////////////////////////////
//
// Filename    : ClientManager.cpp
// Written by  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#include "ClientManager.h"

#include "Assert.h"
#include "DatabaseManager.h"
#include "GameServerGroupInfoManager.h"
#include "GameWorldInfoManager.h"
#include "LoginPlayerManager.h"
#include "PacketProfile.h"
#include "Profile.h"
#include "Properties.h"
#include "ReconnectLoginInfoManager.h"
#include "Timeval.h"

#ifdef __THAILAND_SERVER__

#include "TimeChecker.h"

#endif

//////////////////////////////////////////////////////////////////////
//
// constructor
//

//
//////////////////////////////////////////////////////////////////////
ClientManager::ClientManager() throw(Error) {
    __BEGIN_TRY

    
    g_pLoginPlayerManager = new LoginPlayerManager();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//

//
//////////////////////////////////////////////////////////////////////
ClientManager::~ClientManager() throw(Error) {
    __BEGIN_TRY

    
    if (g_pLoginPlayerManager != NULL) {
        delete g_pLoginPlayerManager;
        g_pLoginPlayerManager = NULL;
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void ClientManager::init() throw(Error) {
    __BEGIN_TRY

    g_pLoginPlayerManager->init();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void ClientManager::start() throw(Error) {
    __BEGIN_TRY

    run(); 

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void ClientManager::stop() throw(Error) {
    __BEGIN_TRY

    throw UnsupportedError("stopping manager not supported.");

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void ClientManager::run() throw(Error) {
    __BEGIN_TRY

    Assert(g_pLoginPlayerManager != NULL);

    Timeval NextTime;
    getCurrentTime(NextTime);

    
    Timeval ReloadNextTime = NextTime;
    
    int ReloadGap = g_pConfig->getPropertyInt("ServerInfoReloadTime") * 60;
    ReloadNextTime.tv_sec += ReloadGap;

    NextTime.tv_sec += 10;

    Timeval dummyQueryTime;
    getCurrentTime(dummyQueryTime);

    while (true) {
        usleep(1000); 

        beginProfileEx("LS_MAIN");

        beginProfileEx("LPM_SELECT");
        g_pLoginPlayerManager->select();
        endProfileEx("LPM_SELECT");

        beginProfileEx("LPM_EXCEPTION");
        g_pLoginPlayerManager->processExceptions();
        endProfileEx("LPM_EXCEPTION");

        beginProfileEx("LPM_INPUT");
        g_pLoginPlayerManager->processInputs();
        endProfileEx("LPM_INPUT");

        beginProfileEx("LPM_COMMAND");
        g_pLoginPlayerManager->processCommands();
        endProfileEx("LPM_COMMAND");

        beginProfileEx("LPM_OUTPUT");
        g_pLoginPlayerManager->processOutputs();
        endProfileEx("LPM_OUTPUT");

        beginProfileEx("LPM_HEARTBEAT");
        g_pReconnectLoginInfoManager->heartbeat();
        endProfileEx("LPM_HEARTBEAT");

        endProfileEx("LS_MAIN");

        Timeval currentTime;
        getCurrentTime(currentTime);

        if (NextTime < currentTime) {
            // outputProfileEx(false, false);
            (g_ProfileSampleManager.getProfileSampleSet())->outputProfileToFile("Profile", false, false);

            NextTime.tv_sec = currentTime.tv_sec + 10;
            NextTime.tv_usec = currentTime.tv_usec;

            
            
            initProfileEx();

            // g_PacketProfileManager.outputResultToFile("PacketProfile.txt");
            g_PacketProfileManager.init();
        }

        if (ReloadNextTime < currentTime) {
            if (g_pGameWorldInfoManager != NULL) {
                g_pGameWorldInfoManager->load();
            }

            if (g_pGameServerGroupInfoManager != NULL) {
                g_pGameServerGroupInfoManager->load();
            }

            ReloadNextTime.tv_sec += ReloadGap;
        }

        
        // by bezz. 2003.04.21
        if (dummyQueryTime < currentTime) {
            g_pDatabaseManager->executeDummyQuery(g_pDatabaseManager->getConnection("DARKEDEN"));

            dummyQueryTime.tv_sec += (60 + rand() % 30) * 60;
        }
#ifdef __THAILAND_SERVER__

        g_pTimeChecker->heartbeat();
#endif
    }

    __END_CATCH
}


// global variable definition
ClientManager* g_pClientManager = NULL;
