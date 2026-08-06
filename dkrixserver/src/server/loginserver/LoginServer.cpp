//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginServer.cpp
// Written By  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

// include files
#include "LoginServer.h"

#include "Assert.h"
#include "ClientManager.h"
#include "GameServerGroupInfoManager.h"
#include "GameServerInfoManager.h"
#include "GameServerManager.h"
#include "GameWorldInfoManager.h"
#include "ItemDestroyer.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "UserInfoManager.h"
#include "ZoneGroupInfoManager.h"
#include "ZoneInfoManager.h"
#include "database/DatabaseManager.h"
// #include "gameserver/billing/BillingPlayerManager.h"
#include "LogClient.h"
#include "chinabilling/CBillingPlayerManager.h"

#ifdef __THAILAND_SERVER__

#include "TimeChecker.h"

#endif

//////////////////////////////////////////////////////////////////////
//
// constructor
//

//
//////////////////////////////////////////////////////////////////////
LoginServer::LoginServer() throw(Error) {
    __BEGIN_TRY

    // create database manager
    g_pDatabaseManager = new DatabaseManager();

    // create some info managers
    g_pGameServerInfoManager = new GameServerInfoManager();
    g_pGameServerGroupInfoManager = new GameServerGroupInfoManager();

    g_pZoneInfoManager = new ZoneInfoManager();
    g_pZoneGroupInfoManager = new ZoneGroupInfoManager();

    // create packet factory manager, packet validator
    
    g_pPacketFactoryManager = new PacketFactoryManager();
    g_pPacketValidator = new PacketValidator();

    // create inter-server communication manager
    g_pGameServerManager = new GameServerManager();

    // create client manager
    g_pClientManager = new ClientManager();

    // create ItemDestroyer
    g_pItemDestroyer = new ItemDestroyer();

    // create ItemDestroyer
    g_pUserInfoManager = new UserInfoManager();

    // create GameWorldInfoManager
    g_pGameWorldInfoManager = new GameWorldInfoManager();

    
    
    // #ifdef __CONNECT_BILLING_SYSTEM__
    //  create GameWorldInfoManager
    // g_pBillingPlayerManager = new BillingPlayerManager();
    // #endif

    
#ifdef __CONNECT_CBILLING_SYSTEM__
    // create CBillingPlayerManager
    g_pCBillingPlayerManager = new CBillingPlayerManager();
#endif

#ifdef __THAILAND_SERVER__

    g_pTimeChecker = new TimeChecker();
#endif

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//

//
//////////////////////////////////////////////////////////////////////
LoginServer::~LoginServer() throw(Error) {
    __BEGIN_TRY

    if (g_pClientManager != NULL) {
        delete g_pClientManager;
        g_pClientManager = NULL;
    }

    if (g_pGameServerManager != NULL) {
        delete g_pGameServerManager;
        g_pGameServerManager = NULL;
    }

    if (g_pPacketValidator != NULL) {
        delete g_pPacketValidator;
        g_pPacketValidator = NULL;
    }

    if (g_pPacketFactoryManager != NULL) {
        delete g_pPacketFactoryManager;
        g_pPacketFactoryManager = NULL;
    }

    if (g_pZoneGroupInfoManager != NULL) {
        delete g_pZoneGroupInfoManager;
        g_pZoneGroupInfoManager = NULL;
    }

    if (g_pZoneInfoManager != NULL) {
        delete g_pZoneInfoManager;
        g_pZoneInfoManager = NULL;
    }

    if (g_pGameServerInfoManager != NULL) {
        delete g_pGameServerInfoManager;
        g_pGameServerInfoManager = NULL;
    }

    if (g_pGameServerGroupInfoManager != NULL) {
        delete g_pGameServerGroupInfoManager;
        g_pGameServerGroupInfoManager = NULL;
    }
    if (g_pDatabaseManager != NULL) {
        delete g_pDatabaseManager;
        g_pDatabaseManager = NULL;
    }
    if (g_pUserInfoManager != NULL) {
        delete g_pUserInfoManager;
        g_pUserInfoManager = NULL;
    }
    if (g_pGameWorldInfoManager != NULL) {
        delete g_pGameWorldInfoManager;
        g_pGameWorldInfoManager = NULL;
    }

    
    
    // #ifdef __CONNECT_BILLING_SYSTEM__
    // if ( g_pBillingPlayerManager != NULL ) {
    // delete g_pBillingPlayerManager;
    // g_pBillingPlayerManager = NULL;
    //}
    // #endif

    
#ifdef __CONNECT_CBILLING_SYSTEM__
    // destroy CBillingPlayerManager
    if (g_pCBillingPlayerManager != NULL) {
        delete g_pCBillingPlayerManager;
        g_pCBillingPlayerManager = NULL;
    }
#endif

#ifdef __THAILAND_SERVER__
    if (g_pTimeChecker != NULL) {
        delete g_pTimeChecker;
        g_pTimeChecker = NULL;
    }

#endif


    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// initialize game server
//
//////////////////////////////////////////////////////////////////////
void LoginServer::init() throw(Error) {
    __BEGIN_TRY

    
    g_pDatabaseManager->init();

    // initialize some info managers
    g_pGameServerInfoManager->init();
    g_pGameServerGroupInfoManager->init();
    g_pZoneInfoManager->init();
    g_pZoneGroupInfoManager->init();

    g_pGameWorldInfoManager->init();

    
    g_pPacketFactoryManager->init();
    g_pPacketValidator->init();

    g_pUserInfoManager->init();

    
    g_pGameServerManager->init();

    
    
    // #ifdef __CONNECT_BILLING_SYSTEM__
    
    // g_pBillingPlayerManager->init();
    // #endif

    
#ifdef __CONNECT_CBILLING_SYSTEM__
    
    g_pCBillingPlayerManager->init();
#endif

#ifdef __THAILAND_SERVER__
    // for Thailand ChildGuard System
    g_pTimeChecker->init();
#endif


    
    
    g_pClientManager->init();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// start login server
//
//////////////////////////////////////////////////////////////////////
void LoginServer::start() throw(Error) {
    __BEGIN_TRY

    
    g_pGameServerManager->start();

    
    
// #ifdef __CONNECT_BILLING_SYSTEM__
// g_pBillingPlayerManager->start();
// #endif

#ifdef __CONNECT_CBILLING_SYSTEM__
    
    g_pCBillingPlayerManager->start();
#endif

    //
    
    //
    // *Reiot's Notes*
    //
    
    
    
    
    //
    g_pClientManager->start();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// stop login server
//



//
//////////////////////////////////////////////////////////////////////
void LoginServer::stop() throw(Error) {
    __BEGIN_TRY

    
    throw UnsupportedError();

    
    g_pClientManager->stop();

    //
    g_pGameServerManager->stop();

    
    
    // #ifdef __CONNECT_BILLING_SYSTEM__
    // g_pBillingPlayerManager->stop();
    // #endif

    
#ifdef __CONNECT_CBILLING_SYSTEM__
    
    g_pCBillingPlayerManager->stop();
#endif

    __END_CATCH
}


//////////////////////////////////////////////////
// global variable declaration
//////////////////////////////////////////////////
LoginServer* g_pLoginServer = NULL;
