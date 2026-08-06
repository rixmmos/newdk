//////////////////////////////////////////////////////////////////////
//
// Filename    : SharedServer.cpp
// Written By  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

// include files
#include "SharedServer.h"

#include "Assert.h"
#include "GameServerGroupInfoManager.h"
#include "GameServerInfoManager.h"
#include "GameServerManager.h"
#include "GameWorldInfoManager.h"
#include "GuildManager.h"
#include "HeartbeatManager.h"
#include "LogClient.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "ResurrectLocationManager.h"
#include "StringPool.h"
#include "database/DatabaseManager.h"
#include "types/ServerType.h"
#ifdef __NETMARBLE_SERVER__
#include "NetmarbleGuildRegisterThread.h"
#endif

//////////////////////////////////////////////////////////////////////
//
// constructor
//

//
//////////////////////////////////////////////////////////////////////
SharedServer::SharedServer() throw(Error) {
    __BEGIN_TRY

    // create database manager
    g_pDatabaseManager = new DatabaseManager();

    // create guild manager
    g_pGuildManager = new GuildManager();

    // create some info managers
    g_pGameServerInfoManager = new GameServerInfoManager();
    g_pGameServerGroupInfoManager = new GameServerGroupInfoManager();

    // create packet factory manager, packet validator
    
    g_pPacketFactoryManager = new PacketFactoryManager();
    g_pPacketValidator = new PacketValidator();

    // create inter-server communication manager
    g_pGameServerManager = new GameServerManager();

    // create client manager
    g_pHeartbeatManager = new HeartbeatManager();

    // create GameWorldInfoManager
    g_pGameWorldInfoManager = new GameWorldInfoManager();

    // create ResurrectLocationManager
    g_pResurrectLocationManager = new ResurrectLocationManager();

    g_pStringPool = new StringPool();

    /*#ifdef __NETMARBLE_SERVER__
        g_pNetmarbleGuildRegisterThread = new NetmarbleGuildRegisterThread();
    #endif*/

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//

//
//////////////////////////////////////////////////////////////////////
SharedServer::~SharedServer() throw(Error) {
    __BEGIN_TRY

    SAFE_DELETE(g_pHeartbeatManager);
    SAFE_DELETE(g_pGameServerManager);
    SAFE_DELETE(g_pPacketValidator);
    SAFE_DELETE(g_pPacketFactoryManager);
    SAFE_DELETE(g_pGameServerInfoManager);
    SAFE_DELETE(g_pGameServerGroupInfoManager);
    SAFE_DELETE(g_pGuildManager);
    SAFE_DELETE(g_pDatabaseManager);
    SAFE_DELETE(g_pGameWorldInfoManager);
    SAFE_DELETE(g_pResurrectLocationManager);
    SAFE_DELETE(g_pStringPool);

    /*#ifdef __NETMARBLE_SERVER__
        SAFE_DELETE( g_pNetmarbleGuildRegisterThread );
    #endif*/

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// initialize game server
//
//////////////////////////////////////////////////////////////////////
void SharedServer::init() throw(Error) {
    __BEGIN_TRY

    cout << "SharedServer::init() start" << endl;

    
    g_pDatabaseManager->init();

    g_pStringPool->load();

    
    g_pGuildManager->init();

    // initialize some info managers
    g_pGameServerInfoManager->init();
    g_pGameServerGroupInfoManager->init();

    g_pGameWorldInfoManager->init();

    
    g_pPacketFactoryManager->init();
    g_pPacketValidator->init();

    
    g_pGameServerManager->init();

    
    g_pResurrectLocationManager->init();

    /*#ifdef __NETMARBLE_SERVER__
        g_pNetmarbleGuildRegisterThread->init();
    #endif*/

    
    
    g_pHeartbeatManager->init();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// start shared server
//
//////////////////////////////////////////////////////////////////////
void SharedServer::start() throw(Error) {
    __BEGIN_TRY

    cout << "---------- Start SharedServer ---------" << endl;
    
    g_pGameServerManager->start();

    
    /*#ifdef __NETMARBLE_SERVER__
        g_pNetmarbleGuildRegisterThread->start();
    #endif*/

    //
    
    //
    // *Reiot's Notes*
    //
    
    
    
    
    //
    g_pHeartbeatManager->start();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// stop shared server
//



//
//////////////////////////////////////////////////////////////////////
void SharedServer::stop() throw(Error) {
    __BEGIN_TRY

    
    throw UnsupportedError();

    //
    g_pHeartbeatManager->stop();

    //
    g_pGameServerManager->stop();

    __END_CATCH
}


//////////////////////////////////////////////////
// global variable declaration
//////////////////////////////////////////////////
SharedServer* g_pSharedServer = NULL;
