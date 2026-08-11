//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginPlayerManager.cpp
// Written by  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

// include files
#include "LoginPlayerManager.h"

#include <stdio.h>
#include <time.h>

#include <algorithm>

#include "Assert.h"
#include "DB.h"
#include "LogClient.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "ReconnectLoginInfoManager.h"
#include "Socket.h"
#include "SocketAPI.h"
#include "Utility.h"
#include "gameserver/billing/BillingInfo.h"

namespace {

// Rate-limit the "descriptor is not selectable" rejection log.
//
// m_ReadFDs / m_WriteFDs / m_ExceptFDs are plain fd_set, which glibc lays out
// as a fixed FD_SETSIZE-bit bitmap. FD_SETSIZE is not overridden anywhere in
// this tree, so that is 1024 bits -- but nMaxPlayers is 2000, and nothing on
// the way in used to compare the accepted descriptor against either number.
// FD_SET() on descriptor 1024 or above therefore wrote past the end of
// m_ReadFDs and into the adjacent m_WriteFDs member. Refusing those
// descriptors removes the out-of-bounds write without removing any capacity
// that ever worked: everything above 1023 was already corrupting memory
// rather than serving a player.
//
// TCP 9999 is internet-exposed and accepts before it authenticates, so the
// rejection path is reachable by anyone who can open sockets. It must not
// cost a file open per attempt -- that is the same amplifier
// Datagram.cpp's logNonDatagramPacket() was written to close. Up to
// kMaxLinesPerWindow lines are still written each window, and a window that
// closes with drops hidden records how many, which bounds the path to ~9 file
// writes per minute regardless of inbound rate.
//
// acceptNewConnection() is reached only from processInputs(), which holds
// m_Mutex, and only one thread runs that loop, so these statics need no
// locking of their own.
void logRejectedDescriptor(int fd, int limit, const string& host) {
    const int kMaxLinesPerWindow = 8;
    const time_t kWindowSeconds = 60;

    static time_t windowStart = 0;
    static int loggedThisWindow = 0;
    static unsigned long suppressedThisWindow = 0;

    const time_t now = time(NULL);

    if (now - windowStart >= kWindowSeconds) {
        if (suppressedThisWindow > 0)
            filelog("loginserver_fdlimit.txt",
                    "unselectable descriptors: %lu further rejections suppressed in the last %ld seconds",
                    suppressedThisWindow, (long)kWindowSeconds);

        windowStart = now;
        loggedThisWindow = 0;
        suppressedThisWindow = 0;
    }

    if (loggedThisWindow >= kMaxLinesPerWindow) {
        suppressedThisWindow++;
        return;
    }

    loggedThisWindow++;
    filelog("loginserver_fdlimit.txt", "rejected fd:%d (limit %d) host:%s", fd, limit, host.c_str());
}

} // namespace


//////////////////////////////////////////////////////////////////////
//
// constructor
//

//
//////////////////////////////////////////////////////////////////////
LoginPlayerManager::LoginPlayerManager() : m_pServerSocket(NULL), m_ServerFD(INVALID_SOCKET), m_MinFD(-1), m_MaxFD(-1) {
    __BEGIN_TRY

    m_Mutex.setName("LoginPlayerManager");

    g_pReconnectLoginInfoManager = new ReconnectLoginInfoManager();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//

//
//////////////////////////////////////////////////////////////////////
LoginPlayerManager::~LoginPlayerManager() {
    __BEGIN_TRY

    if (m_pServerSocket != NULL) {
        m_pServerSocket->close();
        delete m_pServerSocket;
        m_pServerSocket = NULL;
    }


    //
    if (g_pReconnectLoginInfoManager != NULL) {
        delete g_pReconnectLoginInfoManager;
        g_pReconnectLoginInfoManager = NULL;
    }

    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::init() {
    __BEGIN_TRY


    while (1) {
        try {
            m_pServerSocket = new ServerSocket(g_pConfig->getPropertyInt("LoginServerPort"));

            break;
        } catch (BindException& be) {
            SAFE_DELETE(m_pServerSocket);
            cout << be.toString() << endl;
            usleep(1000);
        }
    }


    m_ServerFD = m_pServerSocket->getSOCKET();


    FD_ZERO(&m_ReadFDs[0]);
    FD_ZERO(&m_WriteFDs[0]);
    FD_ZERO(&m_ExceptFDs[0]);


    FD_SET(m_ServerFD, &m_ReadFDs[0]);

    // set min/max fd
    m_MaxFD = m_MinFD = m_ServerFD;


    m_Timeout[0].tv_sec = 0;
    m_Timeout[0].tv_usec = 0;

    //--------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------
    Statement* pStmt = NULL;
    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectLoggedOnPlayersStmt(
            pConn, "SELECT PlayerID from Player WHERE LogOn = 'LOGON' AND CurrentLoginServerID=?");
        selectLoggedOnPlayersStmt.bindInt(1, g_pConfig->getPropertyInt("LoginServerID"));
        Result* pResult = selectLoggedOnPlayersStmt.execute();


        while (pResult->next()) {
            string playerID = pResult->getString(1);

            // cout << "delete from PCRoomUserInfo " << playerID.c_str() << endl;

            PreparedStatement deletePCRoomUserInfoStmt(pConn, "DELETE FROM PCRoomUserInfo WHERE PlayerID=?");
            deletePCRoomUserInfoStmt.bindString(1, playerID);
            deletePCRoomUserInfoStmt.execute();

            // cout << "delete ok" << endl;
        }

        PreparedStatement updateLogOffStmt(
            pConn, "UPDATE Player SET LogOn = 'LOGOFF' WHERE LogOn = 'LOGON' AND CurrentLoginServerID=?");
        updateLogOffStmt.bindInt(1, g_pConfig->getPropertyInt("LoginServerID"));
        updateLogOffStmt.execute();
    } catch (SQLQueryException& sqe) {
        throw Error(sqe.toString());
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::select() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)


    m_Timeout[1].tv_sec = m_Timeout[0].tv_sec;
    m_Timeout[1].tv_usec = m_Timeout[0].tv_usec;


    m_ReadFDs[1] = m_ReadFDs[0];
    m_WriteFDs[1] = m_WriteFDs[0];
    m_ExceptFDs[1] = m_ExceptFDs[0];

    try {
        SocketAPI::select_ex(m_MaxFD + 1, &m_ReadFDs[1], &m_WriteFDs[1], &m_ExceptFDs[1], &m_Timeout[1]);

    } catch (TimeoutException) {
        // do nothing
    } catch (InterruptedException& ie) {
        log(LOG_LOGINSERVER, "", "", ie.toString());
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::processExceptions() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_ExceptFDs[1]) && i != m_ServerFD) {
            Assert(m_pPlayers[i] != NULL);

            StringStream msg;
            msg << "OOB from " << m_pPlayers[i]->toString();
            log(LOG_LOGINSERVER_ERROR, "", "", msg.toString());
            cout << msg.toString() << endl;


            m_pPlayers[i]->disconnect(UNDISCONNECTED);


            delete m_pPlayers[i];


            deletePlayer_NOLOCKED(i);
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::processInputs() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_ReadFDs[1])) {
            if (i == m_ServerFD) {
                acceptNewConnection();

            } else {
                Assert(m_pPlayers[i] != NULL);

                try {
                    if (m_pPlayers[i]->getSocket()->getSockError()) {
                        m_pPlayers[i]->disconnect(DISCONNECTED);


                        delete m_pPlayers[i];


                        deletePlayer_NOLOCKED(i);
                    } else {
                        m_pPlayers[i]->processInput();
                    }

                } catch (ConnectException& ce) {
                    cout << ce.toString() << endl;
                    log(LOG_LOGINSERVER_ERROR, "", ce.toString());


                    m_pPlayers[i]->disconnect(DISCONNECTED);


                    delete m_pPlayers[i];


                    deletePlayer_NOLOCKED(i);
                }
            }
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::processCommands() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)


    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (m_pPlayers[i] != NULL && i != m_ServerFD) {
            try {
#ifdef __CONNECT_BILLING_SYSTEM__

                // LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(m_pPlayers[i]);


                // if (!pLoginPlayer->isBillingLoginVerified())
                //{
                // pLoginPlayer->sendBillingLogin();
                //}
#endif

                m_pPlayers[i]->processCommand();
            } catch (ProtocolException& pe) {
                //--------------------------------------------------

                //


                //


                //
                //--------------------------------------------------

                cout << pe.toString() << endl;
                log(LOG_LOGINSERVER_ERROR, "", "", pe.toString());

                m_pPlayers[i]->disconnect(UNDISCONNECTED);


                delete m_pPlayers[i];


                deletePlayer_NOLOCKED(i);
            }

            catch (ConnectException& ce) {
                cout << ce.toString() << endl;
                log(LOG_LOGINSERVER_ERROR, "", "", ce.toString());

                m_pPlayers[i]->disconnect(DISCONNECTED);


                delete m_pPlayers[i];


                deletePlayer_NOLOCKED(i);
            }
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::processOutputs() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_WriteFDs[1]) && i != m_ServerFD) {
            Assert(m_pPlayers[i] != NULL);

            try {
                m_pPlayers[i]->processOutput();

            } catch (ConnectException& ce) {
                cout << ce.toString() << endl;
                log(LOG_LOGINSERVER_ERROR, "", "", ce.toString());


                m_pPlayers[i]->disconnect(DISCONNECTED);


                delete m_pPlayers[i];


                deletePlayer_NOLOCKED(i);

            } catch (ProtocolException& pe) {
                cout << pe.toString() << endl;
                log(LOG_LOGINSERVER_ERROR, "", "", pe.toString());


                m_pPlayers[i]->disconnect(DISCONNECTED);


                delete m_pPlayers[i];


                deletePlayer_NOLOCKED(i);
            }
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// accept new connection
//

//


//
//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::acceptNewConnection() {
    __BEGIN_TRY


    Socket* client = NULL;

    try {
        client = m_pServerSocket->accept();
    } catch (Throwable& t) {
    }


    if (client == NULL)
        return;

    // Refuse descriptors select() cannot represent, before anything touches
    // FD_SET. The ceiling is the smaller of FD_SETSIZE (the fd_set bitmap
    // width) and nMaxPlayers (the m_pPlayers array bound); see the note on
    // logRejectedDescriptor() above. ~Socket closes the descriptor.
    const int kMaxSelectableFD = (FD_SETSIZE < (int)nMaxPlayers) ? FD_SETSIZE : (int)nMaxPlayers;
    const int acceptedFD = (int)client->getSOCKET();

    if (acceptedFD < 0 || acceptedFD >= kMaxSelectableFD) {
        logRejectedDescriptor(acceptedFD, kMaxSelectableFD, client->getHost());
        delete client;
        return;
    }

    if (client->getSockError()) {
        delete client;
        return;
    }

    client->setNonBlocking(true);

    if (client->getSockError()) {
        delete client;
        return;
    }

    StringStream msg;
    msg << "NEW CONNECTION FROM [" << client->getHost().c_str() << ":" << client->getPort() << "]";
    log(LOG_LOGINSERVER, "", "", msg.toString());
    cout << "NEW CONNECTION FROM " << client->getHost() << ":" << client->getPort() << endl;
    cerr << "NEW CONNECTION FROM " << client->getHost() << ":" << client->getPort() << endl;

    //--------------------------------------------------

    //--------------------------------------------------
    /*
    if ( g_pBanManager->isBanned( client->getHost() ) ) {
        client->send("You are banned. Bye~\n",23);
        client->close();
        delete client;
    }

    */

    // set socket option ( !NonBlocking, NoLinger )
    client->setLinger(0);


    LoginPlayer* pPlayer = new LoginPlayer(client);

    // set player status to PLAYER_LOGON
    Assert(pPlayer->getPlayerStatus() == LPS_NONE);
    pPlayer->setPlayerStatus(LPS_BEGIN_SESSION);


    addPlayer_NOLOCKED(pPlayer);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//

//


//
//////////////////////////////////////////////////////////////////////
void LoginPlayerManager::addPlayer_NOLOCKED(Player* pPlayer) {
    __BEGIN_TRY

    PlayerManager::addPlayer(pPlayer);

    SOCKET fd = pPlayer->getSocket()->getSOCKET();


    m_MinFD = min(fd, m_MinFD);
    m_MaxFD = max(fd, m_MaxFD);


    FD_SET(fd, &m_ReadFDs[0]);
    FD_SET(fd, &m_WriteFDs[0]);
    FD_SET(fd, &m_ExceptFDs[0]);

    __END_CATCH
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void LoginPlayerManager::addPlayer(Player* pPlayer) {
    __ENTER_CRITICAL_SECTION(m_Mutex)

    addPlayer_NOLOCKED(pPlayer);

    __LEAVE_CRITICAL_SECTION(m_Mutex)
}


//--------------------------------------------------------------------------------
//

//


//
//--------------------------------------------------------------------------------
void LoginPlayerManager::deletePlayer_NOLOCKED(SOCKET fd) {
    __BEGIN_TRY

    PlayerManager::deletePlayer(fd);

    Assert(m_pPlayers[fd] == NULL);


    if (fd == m_MinFD) {
        int i = m_MinFD;
        for (; i <= m_MaxFD; i++) {
            if (m_pPlayers[i] != NULL || i == m_ServerFD) {
                m_MinFD = i;
                break;
            }
        }


        if (i > m_MaxFD)
            m_MinFD = m_MaxFD = -1;

    } else if (fd == m_MaxFD) {
        int i = m_MaxFD;
        for (; i >= m_MinFD; i--) {
            if (m_pPlayers[i] != NULL || i == m_ServerFD) {
                m_MaxFD = i;
                break;
            }
        }


        if (i < m_MinFD) {
            throw UnknownError("m_MinFD & m_MaxFD problem.");
        }
    }


    FD_CLR(fd, &m_ReadFDs[0]);
    FD_CLR(fd, &m_ReadFDs[1]);
    FD_CLR(fd, &m_WriteFDs[0]);
    FD_CLR(fd, &m_WriteFDs[1]);
    FD_CLR(fd, &m_ExceptFDs[0]);
    FD_CLR(fd, &m_ExceptFDs[1]);

    __END_CATCH
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void LoginPlayerManager::deletePlayer(SOCKET fd) {
    __ENTER_CRITICAL_SECTION(m_Mutex)

    deletePlayer_NOLOCKED(fd);

    __LEAVE_CRITICAL_SECTION(m_Mutex)
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
LoginPlayer* LoginPlayerManager::getPlayer_NOLOCKED(const string& id) const {
    __BEGIN_TRY

    LoginPlayer* pLoginPlayer = NULL;

    // cout << "m_MinFD : " << m_MinFD << endl;
    // cout << "m_MaxFD : " << m_MaxFD << endl;

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (m_pPlayers[i] != NULL) {
            // cout << "[" << i << "] : " << m_pPlayers[i]->toString() << endl;
            // cout << "[" << i << "] : " << m_pPlayers[i]->getID().c_str() << endl;

            if (m_pPlayers[i]->getID() == id) {
                pLoginPlayer = dynamic_cast<LoginPlayer*>(m_pPlayers[i]);
                // cout << "Found OK" << endl;
                break;
            }
        }
    }

    if (pLoginPlayer == NULL)
        throw NoSuchElementException("     .");

    return pLoginPlayer;

    __END_CATCH
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
LoginPlayer* LoginPlayerManager::getPlayer(const string& id) const {
    LoginPlayer* pPlayer;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    pPlayer = getPlayer_NOLOCKED(id);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pPlayer;
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void LoginPlayerManager::broadcastPacket(Packet* pPacket) {
    __BEGIN_TRY

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (m_pPlayers[i] != NULL) {
            m_pPlayers[i]->sendPacket(pPacket);
        }
    }

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void LoginPlayerManager::sendPacket(const string& id, Packet* pPacket) {
    __BEGIN_TRY

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (m_pPlayers[i] != NULL) {
            // cout << "[" << i << "] ID : " << m_pPlayers[i]->getID() << endl;
            if (m_pPlayers[i]->getID() == id) {
                m_pPlayers[i]->sendPacket(pPacket);
                break;
            }
        }
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////
string LoginPlayerManager::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "LoginPlayerManager(nPlayers:" << size() << ")";
    return msg.toString();

    __END_CATCH
}


// global variable definition
LoginPlayerManager* g_pLoginPlayerManager = NULL;
