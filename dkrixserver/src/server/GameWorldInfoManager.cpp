//----------------------------------------------------------------------
//
// Filename    : GameWorldInfoManager.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GameWorldInfoManager.h"

#include "database/Connection.h"
#include "database/DatabaseManager.h"
#include "database/Result.h"
#include "database/Statement.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
GameWorldInfoManager::GameWorldInfoManager() {}

//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
GameWorldInfoManager::~GameWorldInfoManager() throw() {
    
    
    
    
    for (HashMapGameWorldInfo::iterator itr = m_GameWorldInfos.begin(); itr != m_GameWorldInfos.end(); itr++) {
        delete itr->second;
        itr->second = NULL;
    }

    
    m_GameWorldInfos.clear();
}


//----------------------------------------------------------------------
// initialize GSIM
//----------------------------------------------------------------------
void GameWorldInfoManager::init() {
    __BEGIN_TRY

    // just load data from GameWorldInfo table
    load();

    // just print to cout
    cout << toString() << endl;

    __END_CATCH
}

//----------------------------------------------------------------------
// load data from database
//----------------------------------------------------------------------
void GameWorldInfoManager::load() {
    __BEGIN_TRY

    // clear GameWorldInfos
    clear();

    Statement* pStmt = NULL;

    try {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        Result* pResult = pStmt->executeQueryString("SELECT ID, Name, Stat FROM WorldInfo");

        cout << "Loading GameWorldInfoManager...." << endl;

        while (pResult->next()) {
            // cout << "TICK" << endl;
            GameWorldInfo* pGameWorldInfo = new GameWorldInfo();
            pGameWorldInfo->setID(pResult->getInt(1));
            pGameWorldInfo->setName(pResult->getString(2));
            pGameWorldInfo->setStatus((WorldStatus)pResult->getInt(3));
            addGameWorldInfo(pGameWorldInfo);
        }
        cout << "End GameWorldInfoManager Load" << endl;

        
        SAFE_DELETE(pStmt);

    } catch (SQLQueryException& sqe) {
        
        SAFE_DELETE(pStmt);

        throw Error(sqe.toString());

    } catch (Throwable& t) {
        SAFE_DELETE(pStmt);
        cout << t.toString() << endl;
    }

    __END_CATCH
}
//----------------------------------------------------------------------
// clear info
//----------------------------------------------------------------------
void GameWorldInfoManager::clear() {
    __BEGIN_TRY

    HashMapGameWorldInfo::iterator itr = m_GameWorldInfos.begin();
    for (; itr != m_GameWorldInfos.end(); itr++) {
        GameWorldInfo* pGameWorldInfo = itr->second;
        SAFE_DELETE(pGameWorldInfo);
    }

    m_GameWorldInfos.clear();

    __END_CATCH
}

//----------------------------------------------------------------------
// add info
//----------------------------------------------------------------------
void GameWorldInfoManager::addGameWorldInfo(GameWorldInfo* pGameWorldInfo) {
    __BEGIN_TRY

    cout << pGameWorldInfo->toString() << endl;
    cout << "Size : " << m_GameWorldInfos.size() << endl;

    HashMapGameWorldInfo::iterator itr = m_GameWorldInfos.find(pGameWorldInfo->getID());

    if (itr != m_GameWorldInfos.end())
        throw DuplicatedException("duplicated game-server nickname");

    m_GameWorldInfos[pGameWorldInfo->getID()] = pGameWorldInfo;

    __END_CATCH
}

//----------------------------------------------------------------------
// delete info
//----------------------------------------------------------------------
void GameWorldInfoManager::deleteGameWorldInfo(const WorldID_t ID) {
    __BEGIN_TRY

    HashMapGameWorldInfo::iterator itr = m_GameWorldInfos.find(ID);

    if (itr != m_GameWorldInfos.end()) {
        
        delete itr->second;

        
        m_GameWorldInfos.erase(itr);

    } else {
        
        throw NoSuchElementException();
    }

    __END_CATCH
}

//----------------------------------------------------------------------
// get Worldinfo by WorldID
//----------------------------------------------------------------------
GameWorldInfo* GameWorldInfoManager::getGameWorldInfo(const WorldID_t ID) const {
    __BEGIN_TRY

    GameWorldInfo* pGameWorldInfo = NULL;

    HashMapGameWorldInfo::const_iterator itr = m_GameWorldInfos.find(ID);

    if (itr != m_GameWorldInfos.end()) {
        pGameWorldInfo = itr->second;
    } else {
        
        throw NoSuchElementException();
    }

    return pGameWorldInfo;

    __END_CATCH
}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string GameWorldInfoManager::toString() const {
    __BEGIN_TRY

    StringStream msg;

    msg << "GameWorldInfoManager(\n";

    if (m_GameWorldInfos.empty()) {
        msg << "EMPTY";

    } else {
        //--------------------------------------------------
        // *OPTIMIZATION*
        //
        
        //--------------------------------------------------
        for (HashMapGameWorldInfo::const_iterator itr = m_GameWorldInfos.begin(); itr != m_GameWorldInfos.end(); itr++)
            msg << itr->second->toString() << '\n';
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}

// global variable definition
GameWorldInfoManager* g_pGameWorldInfoManager = NULL;
