//////////////////////////////////////////////////////////////////////////////
// Filename    : GameServer.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GameServer.h"

#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "Assert.h"
#include "BillingPlayerManager.h"
#include "ClientManager.h"
#include "DatabaseManager.h"
#include "GameServerInfoManager.h"
#include "GameServerTester.h"
#include "LogClient.h"
#include "LoginServerManager.h"
#include "ObjectManager.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "SharedServerManager.h"
#include "SystemAPI.h"
#include "ThreadManager.h"
#include "chinabilling/CBillingInfo.h"
#ifdef __CONNECT_CBILLING_SYSTEM__
#include "chinabilling/CBillingPlayerManager.h"
#endif

#include "mofus/Mofus.h"
#ifdef __MOFUS__
#include "mofus/MPacketManager.h"
#include "mofus/MPlayerManager.h"
#endif

#include "GDRLairManager.h"
#include "SMSServiceThread.h"


////////////////////////////////////////////////////////////////////////////////
// constructor
////////////////////////////////////////////////////////////////////////////////

GameServer::GameServer()

{
    __BEGIN_TRY

    try {
        // create database manager
        g_pDatabaseManager = new DatabaseManager();

        // create object manager
        g_pObjectManager = new ObjectManager();

        // create packet factory manager , packet validator
        g_pPacketFactoryManager = new PacketFactoryManager();
        g_pPacketValidator = new PacketValidator();

        // create thread manager
        g_pThreadManager = new ThreadManager();

        // create login server manager
        g_pLoginServerManager = new LoginServerManager();

        // create shared server manager
        g_pSharedServerManager = new SharedServerManager();

        // create billing player manager
#ifdef __CONNECT_BILLING_SYSTEM__
        g_pBillingPlayerManager = new BillingPlayerManager();
#endif

        // create china billing player manager
#ifdef __CONNECT_CBILLING_SYSTEM__
        g_pCBillingPlayerManager = new CBillingPlayerManager();
#endif

#ifdef __MOFUS__
        g_pMPlayerManager = new MPlayerManager();
        g_pMPacketManager = new MPacketManager();
#endif

        // create client manager
        g_pClientManager = new ClientManager();

        // create login server manager
        g_pGameServerInfoManager = new GameServerInfoManager();

    } catch (Throwable& t) {
        // cout << t.toString() << endl;
        throw;
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor

//////////////////////////////////////////////////////////////////////////////

GameServer::~GameServer()

{
    __BEGIN_TRY

    SAFE_DELETE(g_pClientManager);
    SAFE_DELETE(g_pObjectManager);
    SAFE_DELETE(g_pPacketValidator);
    SAFE_DELETE(g_pPacketFactoryManager);
    SAFE_DELETE(g_pLoginServerManager);
    SAFE_DELETE(g_pSharedServerManager);
#ifdef __CONNECT_BILLING_SYSTEM__
    SAFE_DELETE(g_pBillingPlayerManager);
#endif
#ifdef __CONNECT_CBILLING_SYSTEM__
    SAFE_DELETE(g_pCBillingPlayerManager);
#endif
#ifdef __MOFUS__
    SAFE_DELETE(g_pMPlayerManager);
    SAFE_DELETE(g_pMPacketManager);
#endif
    SAFE_DELETE(g_pGameServerInfoManager);
    SAFE_DELETE(g_pThreadManager);
    SAFE_DELETE(g_pDatabaseManager);

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////////////
// initialize game server
//////////////////////////////////////////////////////////////////////////////

void GameServer::init()

{
    __BEGIN_TRY

    sysinit();
    cout << "GameServer::init() : System Initialization Success..." << endl;

    
    setCurrentTime();

    
    g_pDatabaseManager->init();
    cout << "GameServer::init() : DatabaseManager Initialization Success..." << endl;

    
    g_pObjectManager->init();
    g_pObjectManager->load();
    cout << "GameServer::init() : ObjectManager Initialization Success..." << endl;

    
    
    g_pThreadManager->init();
    cout << "GameServer::init() : ThreadManager Initialization Success..." << endl;

    
    g_pPacketFactoryManager->init();
    cout << "GameServer::init() : PacketFactoryManager Initialization Success..." << endl;

    g_pPacketValidator->init();
    cout << "GameServer::init() : PacketValidator Initialization Success..." << endl;

    
    g_pLoginServerManager->init();
    cout << "GameServer::init() : LoginServerManager Initialization Success..." << endl;

    
    g_pSharedServerManager->init();
    cout << "GameServer::init() : SharedServerManager Initialization Success..." << endl;

#ifdef __CONNECT_BILLING_SYSTEM__
    
    g_pBillingPlayerManager->init();
    cout << "GameServer::init() : BillingPlayerManager Initialization Success..." << endl;
#endif

#ifdef __CONNECT_CBILLING_SYSTEM__
    
    g_pCBillingPlayerManager->init();
    cout << "GameServer::init() : CBillingPlayerManager Initialization Success..." << endl;
#endif

#ifdef __MOFUS__
    g_pMPacketManager->init();
    cout << "GameServer::init() : MPacketManager Initialization Success..." << endl;

    g_pMPlayerManager->init();
    cout << "GameServer::init() : MPlayerManager Initialization Success..." << endl;
#endif

    g_pGameServerInfoManager->init();
    cout << "GameServer::init() : GameServerInfoManager Initialization Success..." << endl;

    
    
    g_pClientManager->init();
    cout << "GameServer::init() : ClientManager Initialization Success..." << endl;

    
    // goBackground();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// start game server
//////////////////////////////////////////////////////////////////////////////

void GameServer::start()

{
    __BEGIN_TRY

    cout << ">>> STARTING THREAD MANAGER..." << endl;
    g_pThreadManager->start();

    cout << ">>> STARTING LOGIN SERVER MANAGER..." << endl;
    g_pLoginServerManager->start();

    cout << ">>> STARTING SHARED SERVER MANAGER..." << endl;
    g_pSharedServerManager->start();

#ifdef __CONNECT_BILLING_SYSTEM__
    cout << ">>> STARTING BILLING PLAYER MANAGER..." << endl;
    g_pBillingPlayerManager->start();
#endif

#ifdef __CONNECT_CBILLING_SYSTEM__
    cout << ">>> STARTING CBILLING PLAYER MANAGER..." << endl;
    g_pCBillingPlayerManager->start();
#endif

#ifdef __MOFUS__
    g_pMPlayerManager->start();
    cout << ">>> STARTING MOFUS PLAYER MANAGER..." << endl;
#endif

    // add by zdj
    // cout << ">>> STARTING SMS SERVICE THREAD..." << endl;
    // SMSServiceThread::Instance().start();

    //	cout << ">>> STARTING Gilles De Rais Lair Manager THREAD..." << endl;
    GDRLairManager::Instance().init();
    GDRLairManager::Instance().start();

    
    // *Reiot's Notes*
    
    
    
    
    cout << ">>> ALL INITIALIZATIONS ARE COMPLETED SUCCESSFULLY." << endl;
    cout << ">>> STARTING ClientManager->start() INFINITE LOOP..." << endl;

    log(LOG_SYSTEM, "", "", "Game Server Started");

    
    if (g_pTestConfig != NULL) {
        // testGameServer();
    }

    try {
        g_pClientManager->start();

    } catch (Throwable& t) {
        filelog("GameServerError.txt", "%s", t.toString().c_str());
        throw;
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// stop game server
//



//

//////////////////////////////////////////////////////////////////////////////

void GameServer::stop()

{
    __BEGIN_TRY

    //
    // stop client manager
    //
    
    
    //
    g_pClientManager->stop();

    //
    // stop thread manager
    //
    
    
    
    //
    g_pThreadManager->stop();

    //
    // stop object manager
    //
    
    
    //
    // g_pObjectManager->save();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

void GameServer::sysinit()

{
    __BEGIN_TRY

    
    srand(time(0));

    signal(SIGPIPE, SIG_IGN); 
    signal(SIGALRM, SIG_IGN); 
    signal(SIGCHLD, SIG_IGN); 

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void GameServer::goBackground()

{
    __BEGIN_TRY

    int forkres = SystemAPI::fork_ex();

    if (forkres == 0) {
        // case of child process
        close(0);
        close(1);
        close(2);
    } else {
        // case of parent process
        exit(0);
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// global variable declaration
//////////////////////////////////////////////////////////////////////////////
GameServer* g_pGameServer = NULL;
