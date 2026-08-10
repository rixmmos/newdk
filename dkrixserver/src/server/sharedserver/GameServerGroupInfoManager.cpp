//----------------------------------------------------------------------
//
// Filename    : GameServerGroupInfoManager.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GameServerGroupInfoManager.h"

#include "database/Connection.h"
#include "database/DB.h"
#include "database/DatabaseManager.h"
#include "database/Result.h"
#include "database/Statement.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
GameServerGroupInfoManager::GameServerGroupInfoManager() throw() {
    // This constructor was empty, leaving BOTH members indeterminate. The
    // destructor below loops `i < m_MaxWorldID` indexing m_GameServerGroupInfos,
    // so uninitialised values are read as a loop bound and a base pointer.
    // See the gameserver copy for the ASan report on the same defect.
    m_GameServerGroupInfos = NULL;
    m_MaxWorldID = 0;
}

//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
GameServerGroupInfoManager::~GameServerGroupInfoManager() throw() {
    
    
    
    
    for (int i = 1; i < m_MaxWorldID; i++) {
        for (HashMapGameServerGroupInfo::iterator itr = m_GameServerGroupInfos[i].begin();
             itr != m_GameServerGroupInfos[i].end(); itr++) {
            delete itr->second;
            itr->second = NULL;
        }

        
        m_GameServerGroupInfos[i].clear();
    }

    SAFE_DELETE_ARRAY(m_GameServerGroupInfos);
}


//----------------------------------------------------------------------
// initialize GSIM
//----------------------------------------------------------------------
void GameServerGroupInfoManager::init() throw(Error) {
    __BEGIN_TRY

    // just load data from GameServerGroupInfo table
    load();

    // just print to cout
    cout << toString() << endl;

    __END_CATCH
}

//----------------------------------------------------------------------
// load data from database
//----------------------------------------------------------------------
void GameServerGroupInfoManager::load() throw(Error) {
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        Result* pResult = pStmt->executeQuery("SELECT MAX(WorldID) FROM GameServerGroupInfo");

        if (pResult->getRowCount() == 0) {
            throw Error("GameServerGroupInfo TABLE does not exist!");
        }

        pResult->next();
        m_MaxWorldID = pResult->getInt(1) + 2;

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    m_GameServerGroupInfos = new HashMapGameServerGroupInfo[m_MaxWorldID];

    try {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        Result* pResult = pStmt->executeQuery("SELECT WorldID, GroupID, GroupName FROM GameServerGroupInfo");

        while (pResult->next()) {
            GameServerGroupInfo* pGameServerGroupInfo = new GameServerGroupInfo();
            WorldID_t WorldID = pResult->getInt(1);
            pGameServerGroupInfo->setWorldID(WorldID);
            pGameServerGroupInfo->setGroupID(pResult->getInt(2));
            pGameServerGroupInfo->setGroupName(pResult->getString(3));
            addGameServerGroupInfo(pGameServerGroupInfo, WorldID);
        }

        
        delete pStmt;

    } catch (SQLQueryException& sqe) {
        
        delete pStmt;

        throw Error(sqe.toString());

    } catch (Throwable& t) {
        cout << t.toString() << endl;
    }

    __END_CATCH
}

//----------------------------------------------------------------------
// add info
//----------------------------------------------------------------------
void GameServerGroupInfoManager::addGameServerGroupInfo(GameServerGroupInfo* pGameServerGroupInfo,
                                                        WorldID_t WorldID) throw(DuplicatedException) {
    __BEGIN_TRY

    HashMapGameServerGroupInfo::iterator itr = m_GameServerGroupInfos[WorldID].find(pGameServerGroupInfo->getGroupID());

    if (itr != m_GameServerGroupInfos[WorldID].end())
        throw DuplicatedException("duplicated game-server nickname");

    m_GameServerGroupInfos[WorldID][pGameServerGroupInfo->getGroupID()] = pGameServerGroupInfo;

    __END_CATCH
}

//----------------------------------------------------------------------
// delete info
//----------------------------------------------------------------------
void GameServerGroupInfoManager::deleteGameServerGroupInfo(const ServerGroupID_t GroupID,
                                                           WorldID_t WorldID) throw(NoSuchElementException) {
    __BEGIN_TRY

    HashMapGameServerGroupInfo::iterator itr = m_GameServerGroupInfos[WorldID].find(GroupID);

    if (itr != m_GameServerGroupInfos[WorldID].end()) {
        
        delete itr->second;

        
        m_GameServerGroupInfos[WorldID].erase(itr);

    } else {
        
        throw NoSuchElementException();
    }

    __END_CATCH
}

//----------------------------------------------------------------------
// get GameServerGroupinfo by ServerGroupID
//----------------------------------------------------------------------
GameServerGroupInfo* GameServerGroupInfoManager::getGameServerGroupInfo(const ServerGroupID_t GroupID,
                                                                        WorldID_t WorldID) const
    throw(NoSuchElementException) {
    __BEGIN_TRY

    if (WorldID >= m_MaxWorldID) {
        
        throw NoSuchElementException();
    }

    GameServerGroupInfo* pGameServerGroupInfo = NULL;

    HashMapGameServerGroupInfo::const_iterator itr = m_GameServerGroupInfos[WorldID].find(GroupID);

    if (itr != m_GameServerGroupInfos[WorldID].end()) {
        pGameServerGroupInfo = itr->second;
    } else {
        
        throw NoSuchElementException();
    }

    return pGameServerGroupInfo;

    __END_CATCH
}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string GameServerGroupInfoManager::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;

    msg << "GameServerGroupInfoManager(\n";

    for (int i = 1; i < m_MaxWorldID; i++) {
        if (m_GameServerGroupInfos[i].empty()) {
            msg << "EMPTY";

        } else {
            //--------------------------------------------------
            // *OPTIMIZATION*
            //
            
            //--------------------------------------------------
            for (HashMapGameServerGroupInfo::const_iterator itr = m_GameServerGroupInfos[i].begin();
                 itr != m_GameServerGroupInfos[i].end(); itr++)
                msg << itr->second->toString() << '\n';
        }
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}

// global variable definition
GameServerGroupInfoManager* g_pGameServerGroupInfoManager = NULL;
