//--------------------------------------------------------------------------------
//
// Filename    : ThreadManager.cc
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "ThreadManager.h"

#include "Assert.h"
#include "DB.h"
#include "LogClient.h"
#include "Properties.h"
#include "ThreadPool.h"
#include "ZoneGroupManager.h"
#include "ZoneGroupThread.h"


//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ThreadManager::ThreadManager() throw(Error) : m_pZoneGroupThreadPool(NULL) {
    __BEGIN_TRY

    
    m_pZoneGroupThreadPool = new ThreadPool();

    __END_CATCH
}


//--------------------------------------------------------------------------------
//
// destructor
//

//
//--------------------------------------------------------------------------------
ThreadManager::~ThreadManager() throw(Error) {
    __BEGIN_TRY

    SAFE_DELETE(m_pZoneGroupThreadPool);

    __END_CATCH
}


//--------------------------------------------------------------------------------
//

//

//
// *CAUTION*
//

//
//--------------------------------------------------------------------------------
void ThreadManager::init() throw(Error) {
    __BEGIN_TRY

    
    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT ZoneGroupID FROM ZoneGroupInfo");

        while (pResult->next()) {
            ZoneGroupID_t zoneGroupID = pResult->getInt(1);
            ZoneGroupThread* pZoneGroupThread = new ZoneGroupThread(g_pZoneGroupManager->getZoneGroup(zoneGroupID));
            m_pZoneGroupThreadPool->addThread(pZoneGroupThread);
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_CATCH
}


//--------------------------------------------------------------------------------
//
// activate sub thread pools
//

//
//--------------------------------------------------------------------------------
void ThreadManager::start() throw(Error) {
    __BEGIN_TRY

    
    m_pZoneGroupThreadPool->start();

    __END_CATCH
}


//--------------------------------------------------------------------------------
//
// deactivate sub thread pools
//

//
//--------------------------------------------------------------------------------
void ThreadManager::stop() throw(Error) {
    __BEGIN_TRY

    throw UnsupportedError();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// global variable definition
//--------------------------------------------------------------------------------
ThreadManager* g_pThreadManager = NULL;
