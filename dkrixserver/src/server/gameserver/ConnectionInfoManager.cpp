//////////////////////////////////////////////////////////////////////////////
// Filename    : ConnectionInfoManager.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ConnectionInfoManager.h"

#include <stdio.h>

#include "Assert.h"
#include "DB.h"
#include "GMServerInfo.h"
#include "LogDef.h"
#include "LoginServerManager.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "StringStream.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZonePlayerManager.h"

static string normalizeConnectionInfoIP(const string& clientIP) {
    if (clientIP == "localhost" || clientIP == "::1" || clientIP == "::ffff:127.0.0.1")
        return "127.0.0.1";

    return clientIP;
}

// global variable definition
ConnectionInfoManager* g_pConnectionInfoManager = NULL;

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
ConnectionInfoManager::ConnectionInfoManager()

{
    __BEGIN_TRY

    m_Mutex.setName("ConnectionInfoManager");


    getCurrentTime(m_NextHeartbeat);
    m_NextHeartbeat.tv_sec += 10;


    m_UpdateUserStatusTime.tv_sec = m_NextHeartbeat.tv_sec + 20;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
ConnectionInfoManager::~ConnectionInfoManager()

{
    __BEGIN_TRY


    HashMapConnectionInfo::iterator itr = m_ConnectionInfos.begin();
    for (; itr != m_ConnectionInfos.end(); itr++) {
        SAFE_DELETE(itr->second);
    }


    m_ConnectionInfos.clear();

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////////////
// add connection info to connection info manager
//////////////////////////////////////////////////////////////////////////////
void ConnectionInfoManager::addConnectionInfo(ConnectionInfo* pConnectionInfo) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    Assert(pConnectionInfo != NULL);

    const string normalizedIP = normalizeConnectionInfoIP(pConnectionInfo->getClientIP());
    pConnectionInfo->setClientIP(normalizedIP);

    HashMapConnectionInfo::iterator itr = m_ConnectionInfos.find(normalizedIP);

    if (itr != m_ConnectionInfos.end()) {
        // throw DuplicatedException("duplicated connection info id");


        // by sigi. 2002.12.7
        // throw DuplicatedException("duplicated connection info id");
        ConnectionInfo* pOldConnectionInfo = itr->second;

        FILELOG_INCOMING_CONNECTION("connectionInfo.log", "DupDelete [%s:%s] %s (%u)",
                                    pOldConnectionInfo->getPlayerID().c_str(), pOldConnectionInfo->getPCName().c_str(),
                                    pOldConnectionInfo->getClientIP().c_str(), pOldConnectionInfo->getKey());

        SAFE_DELETE(pOldConnectionInfo);

        itr->second = pConnectionInfo;

        m_Mutex.unlock();

        return;
    }

    m_ConnectionInfos[normalizedIP] = pConnectionInfo;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// Delete connection info from connection info manager
//////////////////////////////////////////////////////////////////////////////
void ConnectionInfoManager::deleteConnectionInfo(const string& clientIP) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    const string normalizedIP = normalizeConnectionInfoIP(clientIP);
    HashMapConnectionInfo::iterator itr = m_ConnectionInfos.find(normalizedIP);

    if (itr != m_ConnectionInfos.end()) {
        Assert(itr->second != NULL);


        SAFE_DELETE(itr->second);


        m_ConnectionInfos.erase(itr);
    } else {
        throw NoSuchElementException(normalizedIP);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get connection info from connection info manager
//////////////////////////////////////////////////////////////////////////////
ConnectionInfo* ConnectionInfoManager::getConnectionInfo(const string& clientIP) {
    __BEGIN_TRY

    ConnectionInfo* pConnectionInfo = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    const string normalizedIP = normalizeConnectionInfoIP(clientIP);
    HashMapConnectionInfo::iterator itr = m_ConnectionInfos.find(normalizedIP);

    if (itr != m_ConnectionInfos.end()) {
        pConnectionInfo = itr->second;
    } else {
        throw NoSuchElementException(normalizedIP);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pConnectionInfo;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void ConnectionInfoManager::heartbeat()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    Timeval currentTime;
    getCurrentTime(currentTime);

    if (m_NextHeartbeat < currentTime) {
        m_NextHeartbeat = currentTime;
        m_NextHeartbeat.tv_sec += 10;

        HashMapConnectionInfo::iterator before = m_ConnectionInfos.end();
        HashMapConnectionInfo::iterator current = m_ConnectionInfos.begin();

        while (current != m_ConnectionInfos.end()) {
            if (current->second->getExpireTime() < currentTime) {
                ConnectionInfo* pConnectionInfo = current->second;

                m_ConnectionInfos.erase(current);

                // by sigi. 2002.12.7
                FILELOG_INCOMING_CONNECTION("connectionInfo.log", "Expire [%s:%s] %s (%u)",
                                            pConnectionInfo->getPlayerID().c_str(),
                                            pConnectionInfo->getPCName().c_str(),
                                            pConnectionInfo->getClientIP().c_str(), pConnectionInfo->getKey());


                SAFE_DELETE(pConnectionInfo);

                if (before == m_ConnectionInfos.end()) // case of first
                {
                    current = m_ConnectionInfos.begin();
                } else // case of not first
                {
                    current = before;
                    current++;
                }
            } else {
                before = current++;
            }
        }


        Statement* pStmt = NULL;

        static int GroupCount = 0;

        BEGIN_DB {
            if (GroupCount == 0) {
                Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
                PreparedStatement selectStmt(pConn, "SELECT MAX(ZoneGroupID) FROM ZoneGroupInfo");
                Result* pResult = selectStmt.execute();

                pResult->next();

                GroupCount = pResult->getInt(1) + 1;
            }
        }
        END_DB(pStmt)

        GMServerInfo gmServerInfo;

        static int worldID = g_pConfig->getPropertyInt("WorldID");
        static int serverID = g_pConfig->getPropertyInt("ServerID");

        gmServerInfo.setWorldID(worldID);
        gmServerInfo.setServerID(serverID);

        // cout << "GroupCount: " << GroupCount << endl;
        uint numPC = 0;

        for (int i = 1; i < GroupCount; i++) {
            ZoneGroup* pZoneGroup = NULL;

            try {
                pZoneGroup = g_pZoneGroupManager->getZoneGroupByGroupID(i);
            } catch (NoSuchElementException& t) {
                throw Error("Critical Error : ZoneInfoManager    .");
            }

            pZoneGroup->makeZoneUserInfo(gmServerInfo);

            numPC += pZoneGroup->getZonePlayerManager()->size();
        }


        if (currentTime > m_UpdateUserStatusTime) {
            m_UpdateUserStatusTime.tv_sec = currentTime.tv_sec + 30;

            if (g_pConfig->getPropertyInt("IsNetMarble") == 1) {
                pStmt = NULL;
                BEGIN_DB {
                    Connection* pConn = g_pDatabaseManager->getUserInfoConnection();

                    PreparedStatement updateStmt(pConn,
                                                 "UPDATE UserStatus SET CurrentUser=? WHERE WorldID=? AND ServerID=?");
                    updateStmt.bindInt(1, numPC);
                    updateStmt.bindInt(2, worldID);
                    updateStmt.bindInt(3, serverID);
                    updateStmt.execute();


                    if (updateStmt.getAffectedRowCount() == 0) {
                        PreparedStatement insertStmt(
                            pConn, "INSERT IGNORE INTO UserStatus (WorldID, ServerID, CurrentUser) Values (?, ?, ?)");
                        insertStmt.bindInt(1, worldID);
                        insertStmt.bindInt(2, serverID);
                        insertStmt.bindInt(3, numPC);
                        insertStmt.execute();
                    }
                }
                END_DB(pStmt)
            }
        }


        // g_pLoginServerManager->sendPacket(g_pConfig->getProperty("MonitorClientIP1") ,
        // g_pConfig->getPropertyInt("MonitorClient1UDPORT"), &gmServerInfo);
        // g_pLoginServerManager->sendPacket(g_pConfig->getProperty("MonitorClientIP2") ,
        // g_pConfig->getPropertyInt("MonitorClient2UDPORT"), &gmServerInfo);

        static int portNum = g_pConfig->getPropertyInt("LoginServerUDPPortNum");
        static const string& loginServerIP = g_pConfig->getProperty("LoginServerIP");
        static int loginServerUDPPort = g_pConfig->getPropertyInt("LoginServerUDPPort");
        static int loginServerBaseUDPPort = g_pConfig->getPropertyInt("LoginServerBaseUDPPort");


        g_pLoginServerManager->sendPacket(loginServerIP, loginServerUDPPort, &gmServerInfo);


        if (portNum > 1) {
            for (int j = 0; j < portNum; j++) {
                g_pLoginServerManager->sendPacket(loginServerIP, loginServerBaseUDPPort + j, &gmServerInfo);
            }
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string ConnectionInfoManager::toString() const

{
    StringStream msg;

    msg << "ConnectionInfoManager(";

    for (unordered_map<string, ConnectionInfo*>::const_iterator itr = m_ConnectionInfos.begin();
         itr != m_ConnectionInfos.end(); itr++) {
        Assert(itr->second != NULL);
        msg << itr->second->toString();
    }

    msg << ")";

    return msg.toString();
}
