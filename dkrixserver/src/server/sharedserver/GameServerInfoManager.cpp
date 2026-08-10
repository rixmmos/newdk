//////////////////////////////////////////////////////////////////////////////
// Filename    : GameServerInfoManager.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GameServerInfoManager.h"

#include "Properties.h"
#include "database/DB.h"

//////////////////////////////////////////////////////////////////////////////
// class GameServerInfoManager member methods
//////////////////////////////////////////////////////////////////////////////

GameServerInfoManager::GameServerInfoManager() throw() {
    // This constructor was empty, leaving every member indeterminate -- the
    // destructor and clear() both use m_MaxWorldID/m_MaxServerGroupID as loop
    // bounds and delete[] m_pGameServerInfos. Same defect as the gameserver
    // copy and as GameServerGroupInfoManager.
    m_pGameServerInfos = NULL;
    m_MaxServerGroupID = 0;
}


GameServerInfoManager::~GameServerInfoManager() throw() {
    
    
    
    
    for (int i = 0; i < m_MaxServerGroupID; i++) {
        HashMapGameServerInfoItor itr = m_pGameServerInfos[i].begin();
        for (; itr != m_pGameServerInfos[i].end(); itr++) {
            SAFE_DELETE(itr->second);
        }

        
        m_pGameServerInfos[i].clear();
    }

    if (m_pGameServerInfos != NULL) {
        SAFE_DELETE_ARRAY(m_pGameServerInfos);
    }
}


void GameServerInfoManager::init() throw(Error) {
    __BEGIN_TRY

    // just load data from GameServerInfo table
    load();

    // just print to cout
    cout << toString() << endl;

    __END_CATCH
}

void GameServerInfoManager::load() throw(Error) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    WorldID_t WorldID = g_pConfig->getPropertyInt("WorldID");

    
    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        Result* pResult = pStmt->executeQuery("SELECT MAX(GroupID) FROM GameServerInfo WHERE WorldID = %d", WorldID);

        if (pResult->getRowCount() == 0) {
            throw Error("GameServerInfo TABLE does not exist!");
        }

        pResult->next();
        m_MaxServerGroupID = pResult->getInt(1) + 1;

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    m_pGameServerInfos = new HashMapGameServerInfo[m_MaxServerGroupID];

    cout << "MAX SERVER GROUP = " << m_MaxServerGroupID << endl;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        Result* pResult = pStmt->executeQuery(
            "SELECT ServerID, Nickname , IP , TCPPort , UDPPort, GroupID, Stat, WorldID FROM GameServerInfo");

        while (pResult->next()) {
            int i = 0;
            if (pResult->getInt(8) == WorldID) {
                GameServerInfo* pGameServerInfo = new GameServerInfo();

                pGameServerInfo->setServerID(pResult->getInt(++i));
                pGameServerInfo->setNickname(pResult->getString(++i));
                pGameServerInfo->setIP(pResult->getString(++i));
                pGameServerInfo->setTCPPort(pResult->getInt(++i));
                pGameServerInfo->setUDPPort(pResult->getInt(++i));
                pGameServerInfo->setWorldID(WorldID);

                ServerGroupID_t ServerGroupID = pResult->getInt(++i);
                pGameServerInfo->setGroupID(ServerGroupID);

                pGameServerInfo->setServerStat((ServerStatus)pResult->getInt(++i));

                addGameServerInfo(pGameServerInfo, ServerGroupID);
            }
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_CATCH
}

void GameServerInfoManager::addGameServerInfo(GameServerInfo* pGameServerInfo,
                                              const ServerGroupID_t ServerGroupID) throw(DuplicatedException) {
    __BEGIN_TRY

    if (ServerGroupID >= m_MaxServerGroupID) {
        throw DuplicatedException("ServerGroupID over Bounce");
    }

    HashMapGameServerInfoItor itr = m_pGameServerInfos[ServerGroupID].find(pGameServerInfo->getServerID());

    if (itr != m_pGameServerInfos[ServerGroupID].end()) {
        throw DuplicatedException("duplicated game-server ServerID");
    }

    m_pGameServerInfos[ServerGroupID][pGameServerInfo->getServerID()] = pGameServerInfo;

    __END_CATCH
}

void GameServerInfoManager::deleteGameServerInfo(const ServerID_t ServerID,
                                                 const ServerGroupID_t ServerGroupID) throw(NoSuchElementException) {
    __BEGIN_TRY

    if (ServerGroupID >= m_MaxServerGroupID) {
        throw DuplicatedException("ServerGroupID over Bounce");
    }

    HashMapGameServerInfoItor itr = m_pGameServerInfos[ServerGroupID].find(ServerID);

    if (itr != m_pGameServerInfos[ServerGroupID].end()) {
        
        delete itr->second;

        
        m_pGameServerInfos[ServerGroupID].erase(itr);
    } else {
        
        throw NoSuchElementException();
    }

    __END_CATCH
}


GameServerInfo* GameServerInfoManager::getGameServerInfo(const ServerID_t ServerID,
                                                         const ServerGroupID_t ServerGroupID) const
    throw(NoSuchElementException) {
    __BEGIN_TRY

    GameServerInfo* pGameServerInfo = NULL;

    if (ServerGroupID >= m_MaxServerGroupID) {
        
        throw NoSuchElementException();
    }

    HashMapGameServerInfoItor itr = m_pGameServerInfos[ServerGroupID].find(ServerID);

    if (itr != m_pGameServerInfos[ServerGroupID].end()) {
        pGameServerInfo = itr->second;
    } else {
        
        throw NoSuchElementException();
    }

    return pGameServerInfo;

    __END_CATCH
}

string GameServerInfoManager::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "GameServerInfoManager(\n";

    for (int i = 0; i < m_MaxServerGroupID; i++) {
        if (m_pGameServerInfos[i].empty()) {
            msg << "EMPTY";
        } else {
            HashMapGameServerInfoItor itr = m_pGameServerInfos[i].begin();
            for (; itr != m_pGameServerInfos[i].end(); itr++) {
                msg << itr->second->toString() << '\n';
            }
        }

        msg << ")";
    }

    return msg.toString();

    __END_CATCH
}

// global variable definition
GameServerInfoManager* g_pGameServerInfoManager = NULL;
