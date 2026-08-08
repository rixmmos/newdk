//////////////////////////////////////////////////////////////////////////////
// Filename    : IncomingPlayerManager.cpp
// Written by  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "IncomingPlayerManager.h"

#include <stdio.h>

#include <algorithm>

#include "Assert.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "Encrypter.h"
#include "GCUpdateInfo.h"
#include "GLIncomingConnection.h"
#include "GLKickVerify.h"
#include "GamePlayer.h"
#include "LogClient.h"
#include "LogDef.h"
#include "LoginServerManager.h"
#include "MasterLairManager.h"
#include "PKZoneInfoManager.h"
#include "PacketUtil.h"
#include "PaySystem.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "Socket.h"
#include "SocketAPI.h"
#include "ZoneGroup.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"

// #include "UserGateway.h"
#include "SystemAvailabilitiesManager.h"


//////////////////////////////////////////////////////////////////////////////
// constructor

//////////////////////////////////////////////////////////////////////////////

IncomingPlayerManager::IncomingPlayerManager()

    : m_pServerSocket(NULL), m_SocketID(INVALID_SOCKET), m_MinFD(-1), m_MaxFD(-1) {
    __BEGIN_TRY

    m_Mutex.setName("IncomingPlayerManager");
    m_MutexOut.setName("IncomingPlayerManagerOut");
    m_PlayerListQueue.clear();

    try {
        // create  server socket
        while (1) {
            try {
                m_pServerSocket = new ServerSocket(g_pConfig->getPropertyInt("TCPPort"));
                break;
            } catch (BindException& b) {
                SAFE_DELETE(m_pServerSocket);
                cout << "IncomingPlayerManager(" << g_pConfig->getPropertyInt("TCPPort") << ") : " << b.toString()
                     << endl;
                sleep(1);
            }
        }

        m_pServerSocket->setNonBlocking(true);


        m_SocketID = m_pServerSocket->getSOCKET();
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    g_pConnectionInfoManager = new ConnectionInfoManager();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////

IncomingPlayerManager::~IncomingPlayerManager()

{
    __BEGIN_TRY

    SAFE_DELETE(g_pConnectionInfoManager);

    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

void IncomingPlayerManager::init()

{
    __BEGIN_TRY


    FD_ZERO(&m_ReadFDs[0]);
    FD_ZERO(&m_WriteFDs[0]);
    FD_ZERO(&m_ExceptFDs[0]);


    FD_SET(m_SocketID, &m_ReadFDs[0]);
    FD_SET(m_SocketID, &m_ExceptFDs[0]);

    // set min/max fd
    m_MinFD = m_MaxFD = m_SocketID;


    m_Timeout[0].tv_sec = 0;
    m_Timeout[0].tv_usec = 0;


    string dist_host = g_pConfig->getProperty("UI_DB_HOST");
    string dist_db = "DARKEDEN";
    string dist_user = g_pConfig->getProperty("UI_DB_USER");
    string dist_password = g_pConfig->getProperty("UI_DB_PASSWORD");
    uint dist_port = 0;
    if (g_pConfig->hasKey("UI_DB_PORT"))
        dist_port = g_pConfig->getPropertyInt("UI_DB_PORT");

    Connection* pDistConnection = new Connection(dist_host, dist_db, dist_user, dist_password, dist_port);
    g_pDatabaseManager->addDistConnection(((int)(long)Thread::self()), pDistConnection);
    cout << "******************************************************" << endl;
    cout << " THREAD CONNECT UIIRIBUTION DB " << endl;
    cout << " TID Number = " << (int)(long)Thread::self() << endl;
    cout << "******************************************************" << endl;


    Statement* pStmt = NULL;
    BEGIN_DB {
        // pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        // pStmt = g_pDatabaseManager->getDistConnection( (int)(long)Thread::self() )->createStatement();
        // pStmt2 = g_pDatabaseManager->getDistConnection( (int)(long)Thread::self() )->createStatement();

        Connection* pConn = g_pDatabaseManager->getDistConnection("PLAYER_DB");

        PreparedStatement selectStmt(
            pConn, "SELECT PlayerID from Player WHERE LogOn='GAME' AND CurrentWorldID=? AND CurrentServerGroupID=?");
        selectStmt.bindInt(1, g_pConfig->getPropertyInt("WorldID"));
        selectStmt.bindInt(2, g_pConfig->getPropertyInt("ServerID"));
        Result* pResult = selectStmt.execute();


        while (pResult->next()) {
            string playerID = pResult->getString(1);

            PreparedStatement deleteStmt(pConn, "DELETE FROM PCRoomUserInfo WHERE PlayerID=?");
            deleteStmt.bindString(1, playerID);
            deleteStmt.execute();
        }

        PreparedStatement updateStmt(pConn,
                                     "UPDATE Player SET LogOn = 'LOGOFF' WHERE LogOn = 'GAME' AND CurrentWorldID=? "
                                     "AND CurrentServerGroupID=?");
        updateStmt.bindInt(1, g_pConfig->getPropertyInt("WorldID"));
        updateStmt.bindInt(2, g_pConfig->getPropertyInt("ServerID"));
        updateStmt.execute();
    }
    END_DB(pStmt)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement deleteStmt(pConn, "DELETE FROM UserIPInfo WHERE ServerID = ?");
        deleteStmt.bindInt(1, g_pConfig->getPropertyInt("ServerID"));
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::copyPlayers()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    PlayerManager::copyPlayers();

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::broadcast(Packet* pPacket)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (i != m_SocketID && m_pPlayers[i] != NULL)
            m_pPlayers[i]->sendPacket(pPacket);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// call select() system call

//////////////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::select() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)


    m_Timeout[1].tv_sec = m_Timeout[0].tv_sec;
    m_Timeout[1].tv_usec = m_Timeout[0].tv_usec;


    m_ReadFDs[1] = m_ReadFDs[0];
    m_WriteFDs[1] = m_WriteFDs[0];
    m_ExceptFDs[1] = m_ExceptFDs[0];

    try {
        SocketAPI::select_ex(m_MaxFD + 1, &m_ReadFDs[1], &m_WriteFDs[1], &m_ExceptFDs[1], &m_Timeout[1]);
    }

    catch (InterruptedException& ie) {
        log(LOG_GAMESERVER_ERROR, "", "", ie.toString());
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// process all players' inputs


//////////////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::processInputs() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    // copyPlayers();

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_ReadFDs[1])) {
            if (i == m_SocketID) {
                // by sigi. 2002.12.8
                for (int i = 0; i < 50; i++) {
                    if (!acceptNewConnection())
                        break;
                }
            } else {
                if (m_pPlayers[i] != NULL) {
                    GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
                    Assert(pTempPlayer != NULL);
                    Assert(m_pPlayers[i] != NULL);

                    if (pTempPlayer->getSocket()->getSockError()) {
                        FILELOG_INCOMING_CONNECTION("ICMPISocketErr.log", "[Input] PlayerID : %s, PlayerStatus : %d",
                                                    pTempPlayer->getID().c_str(), (int)pTempPlayer->getPlayerStatus());

                        try {
                            pTempPlayer->disconnect(DISCONNECTED);
                        } catch (Throwable& t) {
                            cerr << t.toString() << endl;
                        }


                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_INCOMING_INPUT_ERROR );


                        try {
                            deletePlayer(i);
                            deleteQueuePlayer(pTempPlayer);
                        } catch (Throwable& t) {
                            filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 389");
                            filelog("IncomingPlayerManager.txt", "Exception catched here: 0");
                        }

                        Creature* pCreature = pTempPlayer->getCreature();
                        if (pCreature != NULL)
                            pCreature->setValue(1);
                        SAFE_DELETE(pTempPlayer);
                    } else {
                        try {
                            pTempPlayer->processInput();
                        } catch (ConnectException& ce) {
                            FILELOG_INCOMING_CONNECTION("ICMPIConectionErr.log",
                                                        "[Input] %s, PlayerID : %s, PlayerStatus : %d",
                                                        ce.toString().c_str(), pTempPlayer->getID().c_str(),
                                                        (int)pTempPlayer->getPlayerStatus());


                            try {
                                pTempPlayer->disconnect();
                            } catch (Throwable& t) {
                                cerr << t.toString() << endl;
                            }

                            // by sigi. 2002.12.30
                            //							UserGateway::getInstance()->passUser(
                            // UserGateway::USER_OUT_INCOMING_INPUT_DISCONNECT );


                            try {
                                deletePlayer(i);
                                deleteQueuePlayer(pTempPlayer);
                            } catch (Throwable& t) {
                                filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 430");
                                filelog("IncomingPlayerManager.txt", "Exception catched here: 1");
                            }

                            Creature* pCreature = pTempPlayer->getCreature();
                            if (pCreature != NULL)
                                pCreature->setValue(2);
                            SAFE_DELETE(pTempPlayer);
                        }
                    } // else
                } // else
            } // if
        }
    }

    //	__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// process all players' commands
//////////////////////////////////////////////////////////////////////////////

void IncomingPlayerManager::processCommands() {
    __BEGIN_TRY
    __BEGIN_DEBUG

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    // copyPlayers();

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (i != m_SocketID && m_pPlayers[i] != NULL) {
            GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
            Assert(pTempPlayer != NULL);
            Assert(m_pPlayers[i] != NULL);

            if (pTempPlayer->getSocket()->getSockError()) {
                FILELOG_INCOMING_CONNECTION("ICMPCSocketErr.log", "[Command] PlayerID : %s, PlayerStatus : %d",
                                            pTempPlayer->getID().c_str(), (int)pTempPlayer->getPlayerStatus());
                try {
                    pTempPlayer->disconnect();
                } catch (Throwable& t) {
                    cerr << t.toString() << endl;
                }

                // by sigi. 2002.12.30
                //				UserGateway::getInstance()->passUser( UserGateway::USER_OUT_INCOMING_COMMAND_ERROR );


                try {
                    deletePlayer(i);
                    deleteQueuePlayer(pTempPlayer);
                } catch (Throwable& t) {
                    filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 503");
                    filelog("IncomingPlayerManager.txt", "Exception catched here: 2");
                }

                Creature* pCreature = pTempPlayer->getCreature();
                if (pCreature != NULL)
                    pCreature->setValue(3);
                try {
                    SAFE_DELETE(pTempPlayer);
                } catch (Throwable& t) {
                    cerr << t.toString() << endl;
                    filelog("Destructer.log", "IncommingPlayerManager.cpp +509 : %s", t.toString().c_str());
                }
            } else {
                try {
                    pTempPlayer->processCommand(false);
                } catch (ProtocolException& pe) {
                    try {
                        FILELOG_INCOMING_CONNECTION(
                            "ICMPCProtocolExcpt.log", "[Command] %s, PlayerID : %s, PlayerStatus : %d",
                            pe.toString().c_str(), pTempPlayer->getID().c_str(), (int)pTempPlayer->getPlayerStatus());
                        pTempPlayer->disconnect();
                    } catch (Throwable& t) {
                        cerr << t.toString() << endl;
                    }

                    // by sigi. 2002.12.30
                    //					UserGateway::getInstance()->passUser(
                    // UserGateway::USER_OUT_INCOMING_COMMAND_DISCONNECT );


                    try {
                        deletePlayer(i);
                        deleteQueuePlayer(pTempPlayer);
                    } catch (Throwable& t) {
                        filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 549");
                        filelog("IncomingPlayerManager.txt", "Exception catched here: 3");
                    }

                    Creature* pCreature = pTempPlayer->getCreature();
                    if (pCreature != NULL)
                        pCreature->setValue(4);
                    try {
                        SAFE_DELETE(pTempPlayer);
                    } catch (Throwable& t) {
                        cerr << t.toString() << endl;
                        filelog("Destructer.log", "IncommingPlayerManager.cpp +509 : %s", t.toString().c_str());
                    }
                }
            }
        }
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_DEBUG
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// process all players' outputs
//////////////////////////////////////////////////////////////////////////////

void IncomingPlayerManager::processOutputs() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    // copyPlayers();

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_WriteFDs[1])) {
            if (i == m_SocketID) {
                FILELOG_INCOMING_CONNECTION(
                    "ICMFD.txt",
                    "[ i == m_SocketID ] FD : %d, ServerSocket : %d, MinFD : %d, MaxFD : %d, nPlayers : %d:", i,
                    m_SocketID, m_MinFD, m_MaxFD, m_nPlayers);
                throw IOException("server socket's write bit is selected.");
            }

            if (m_pPlayers[i] != NULL) {
                GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);

                Assert(pTempPlayer != NULL);
                Assert(m_pPlayers[i] != NULL);

                if (pTempPlayer->getSocket()->getSockError()) {
                    FILELOG_INCOMING_CONNECTION("ICMPOSocketErr.log", "[Output] PlayerID : %s, PlayerStatus : %d",
                                                pTempPlayer->getID().c_str(), (int)pTempPlayer->getPlayerStatus());
                    try {
                        pTempPlayer->disconnect(DISCONNECTED);
                    } catch (Throwable& t) {
                        cerr << t.toString() << endl;
                    }

                    // by sigi. 2002.12.30
                    //					UserGateway::getInstance()->passUser(
                    // UserGateway::USER_OUT_INCOMING_OUTPUT_ERROR );


                    try {
                        deletePlayer(i);
                        deleteQueuePlayer(pTempPlayer);
                    } catch (Throwable& t) {
                        filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 637");
                        filelog("IncomingPlayerManager.txt", "Exception catched here: 4");
                    }

                    Creature* pCreature = pTempPlayer->getCreature();
                    if (pCreature != NULL)
                        pCreature->setValue(5);
                    SAFE_DELETE(pTempPlayer);
                } else {
                    try {
                        pTempPlayer->processOutput();
                    } catch (ConnectException& ce) {
                        FILELOG_INCOMING_CONNECTION(
                            "ICMPOConnectExcept.log", "[Output] %s, PlayerID : %s, PlayerStatus : %d",
                            ce.toString().c_str(), pTempPlayer->getID().c_str(), (int)pTempPlayer->getPlayerStatus());
                        StringStream msg;
                        msg << "DISCONNECT " << pTempPlayer->getID() << "(" << ce.toString() << ")";
                        log(LOG_GAMESERVER_ERROR, "", "", msg.toString());

                        try {
                            pTempPlayer->disconnect(DISCONNECTED);
                        } catch (Throwable& t) {
                            cerr << t.toString() << endl;
                        }

                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_INCOMING_OUTPUT_DISCONNECT );


                        try {
                            deletePlayer(i);
                            deleteQueuePlayer(pTempPlayer);
                        } catch (Throwable& t) {
                            filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 681");
                            filelog("IncomingPlayerManager.txt", "Exception catched here: 5");
                        }

                        Creature* pCreature = pTempPlayer->getCreature();
                        if (pCreature != NULL)
                            pCreature->setValue(6);
                        SAFE_DELETE(pTempPlayer);
                    } catch (ProtocolException& cp) {
                        FILELOG_INCOMING_CONNECTION(
                            "ICMPOProtocolExcept.log", "[Output] %s, PlayerID : %s, PlayerStatus : %d",
                            cp.toString().c_str(), pTempPlayer->getID().c_str(), (int)pTempPlayer->getPlayerStatus());
                        StringStream msg;
                        msg << "DISCONNECT " << pTempPlayer->getID() << "(" << cp.toString() << ")";
                        log(LOG_GAMESERVER_ERROR, "", "", cp.toString());


                        try {
                            pTempPlayer->disconnect(DISCONNECTED);
                        } catch (Throwable& t) {
                            cerr << t.toString() << endl;
                        }

                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_INCOMING_OUTPUT_DISCONNECT2 );


                        try {
                            deletePlayer(i);
                            deleteQueuePlayer(pTempPlayer);
                        } catch (Throwable& t) {
                            filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 720");
                            filelog("IncomingPlayerManager.txt", "Exception catched here: 6");
                        }

                        Creature* pCreature = pTempPlayer->getCreature();
                        if (pCreature != NULL)
                            pCreature->setValue(7);
                        SAFE_DELETE(pTempPlayer);
                    }
                }
            }

            // pTempPlayer->processOutput();
        }
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// process all players' exceptions


//////////////////////////////////////////////////////////////////////////////

void IncomingPlayerManager::processExceptions() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    // copyPlayers();

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_ExceptFDs[1])) {
            if (i != m_SocketID) {
                if (m_pPlayers[i] != NULL) {
                    GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
                    Assert(pTempPlayer != NULL);
                    Assert(i != m_SocketID);
                    Assert(m_pPlayers[i] != NULL);
                    StringStream msg;
                    msg << "OOB from " << pTempPlayer->toString();

                    FILELOG_INCOMING_CONNECTION("ICMPEOOB.log", "PlayerID : %s, PlayerStatus : %d",
                                                pTempPlayer->getID().c_str(), (int)pTempPlayer->getPlayerStatus());
                    try {
                        pTempPlayer->disconnect();
                    } catch (Throwable& t) {
                        // cerr << t.toString() << endl;
                    }

                    // by sigi. 2002.12.30
                    //					UserGateway::getInstance()->passUser( UserGateway::USER_OUT_INCOMING_EXCEPTION
                    //);


                    try {
                        deletePlayer(i);
                        deleteQueuePlayer(pTempPlayer);
                    } catch (Throwable& t) {
                        filelog("deletePlayer.log", "called in IncomingPlayerManager.cpp line 799");
                        filelog("IncomingPlayerManager.txt", "Exception catched here: 7");
                    }

                    Creature* pCreature = pTempPlayer->getCreature();
                    if (pCreature != NULL)
                        pCreature->setValue(8);
                    SAFE_DELETE(pTempPlayer);
                }
            } else {
                // cerr << "Exception in Loginserver to Gameserver" << endl;
            }
        }
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool IncomingPlayerManager::acceptNewConnection()

{
    __BEGIN_TRY

    m_CheckValue = 0;

    int fd = -9999;
    int MinFD = (int)m_MinFD;
    int MaxFD = (int)m_MaxFD;


    Socket* client = NULL;

    try {
        m_CheckValue = 1;
        client = m_pServerSocket->accept();
        m_CheckValue = 2;
    } catch (Throwable& t) {
        m_CheckValue += 10000;
    }

    if (client == NULL) {
        m_CheckValue = 50;
        return false;
    }

    try {
        fd = (int)client->getSOCKET();
        FILELOG_INCOMING_CONNECTION("acceptNewConnection.log", "Accept FD : %d ( MinFD : %d , MaxFD : %d ) %s", fd,
                                    MinFD, MaxFD, client->getHost().c_str());

        if (fd <= 0 || fd >= nMaxPlayers) {
            FILELOG_INCOMING_CONNECTION("acceptNewConnectionError.log", "Accept FD : %d ( MinFD : %d , MaxFD : %d ) %s",
                                        fd, MinFD, MaxFD, client->getHost().c_str());

            throw Error();
        }


        if (client->getSockError()) {
            m_CheckValue = 4;
            throw Error();
        }

        m_CheckValue = 5;
        client->setNonBlocking(true);
        m_CheckValue = 6;


        if (client->getSockError()) {
            m_CheckValue = 7;
            throw Error();
        }
        // set socket option (!NonBlocking, NoLinger)
        m_CheckValue = 8;
        client->setLinger(0);
        m_CheckValue = 9;

        StringStream msg;
        msg << "NEW CONNECTION FROM " << client->getHost() << ":" << client->getPort();
        log(LOG_GAMESERVER, "", "", msg.toString());
        FILELOG_INCOMING_CONNECTION("accept_trace.log", "accepted host=%s port=%u fd=%d", client->getHost().c_str(),
                                    client->getPort(), fd);
        m_CheckValue = 10;

        //----------------------------------------------------------------------

        //----------------------------------------------------------------------


        g_pConnectionInfoManager->getConnectionInfo(client->getHost());
        FILELOG_INCOMING_CONNECTION("accept_trace.log", "connection info exists for host=%s fd=%d",
                                    client->getHost().c_str(), fd);
        m_CheckValue = 11;


        GamePlayer* pGamePlayer = new GamePlayer(client);
        m_CheckValue = 12;

        // set player status to GPS_BEGIN_SESSION
        pGamePlayer->setPlayerStatus(GPS_BEGIN_SESSION);
        m_CheckValue = 13;


        // addPlayer_NOBLOCKED(pGamePlayer);
        try {
            m_CheckValue = 14;
            addPlayer(pGamePlayer);
            m_CheckValue = 15;

            // by sigi. 2002.12.30
            //			UserGateway::getInstance()->passUser( UserGateway::USER_IN_NORMAL );
        } catch (DuplicatedException& de) {
            FILELOG_INCOMING_CONNECTION("ancDupExcept.log", "[Output] %s, FD : %d ( MinFD : %d , MaxFD : %d ) %s",
                                        de.toString().c_str(), fd, MinFD, MaxFD, client->getHost().c_str());

            m_CheckValue += 1000;
            client->close();
            m_CheckValue += 1000;
            SAFE_DELETE(client);
            m_CheckValue += 1000;
            SAFE_DELETE(pGamePlayer);
            m_CheckValue += 1000;
            // return true;
        }
    } catch (NoSuchElementException&) {
        FILELOG_INCOMING_CONNECTION("accept_trace.log", "missing connection info for host=%s fd=%d",
                                    client->getHost().c_str(), fd);
        FILELOG_INCOMING_CONNECTION("ancNoSuch.log", "FD : %d ( MinFD : %d , MaxFD : %d ) %s", fd, MinFD, MaxFD,
                                    client->getHost().c_str());

        m_CheckValue += 20000;
        m_CheckValue += 1000;
        StringStream msg2;
        msg2 << "ILLEGAL ACCESS FROM " << client->getHost() << ":" << client->getPort();
        log(LOG_GAMESERVER, "", "", msg2.toString());

        m_CheckValue += 1000;

        //----------------------------------------acceptNewConnection core!!!

        // client->send("Error : Unauthorized access",27);

        m_CheckValue += 1000;
        client->close();
        m_CheckValue += 1000;
        SAFE_DELETE(client);
        m_CheckValue += 1000;
    } catch (Throwable& t) {
        FILELOG_INCOMING_CONNECTION("ancThrowable.log", "FD : %d ( MinFD : %d , MaxFD : %d ) %s checkValue : %d", fd,
                                    MinFD, MaxFD, client->getHost().c_str(), m_CheckValue);
        m_CheckValue += 30000;
        try {
            m_CheckValue = 25;
            if (client != NULL) {
                client->close();
                m_CheckValue = 26;
                SAFE_DELETE(client);
                m_CheckValue = 27;
            }
            m_CheckValue = 28;
        } catch (Throwable& t) {
            m_CheckValue += 1000;
        } catch (...) {
            m_CheckValue += 2000;
        }
    } catch (exception& e) {
        m_CheckValue += 40000;
        FILELOG_INCOMING_CONNECTION("ancException.log", "FD : %d ( MinFD : %d , MaxFD : %d ) %s checkValue : %d", fd,
                                    MinFD, MaxFD, client->getHost().c_str(), m_CheckValue);
    } catch (...) {
        m_CheckValue += 50000;
        FILELOG_INCOMING_CONNECTION("ancEtc.log", "FD : %d ( MinFD : %d , MaxFD : %d ) %s checkValue : %d", fd, MinFD,
                                    MaxFD, client->getHost().c_str(), m_CheckValue);
    }
    m_CheckValue = 33;

    return true;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::addPlayer(Player* pGamePlayer) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    // call base class's method
    PlayerManager::addPlayer(pGamePlayer);

    SOCKET fd = pGamePlayer->getSocket()->getSOCKET();


    m_MinFD = min(fd, m_MinFD);
    m_MaxFD = max(fd, m_MaxFD);


    FD_SET(fd, &m_ReadFDs[0]);
    FD_SET(fd, &m_WriteFDs[0]);
    FD_SET(fd, &m_ExceptFDs[0]);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::addPlayer_NOBLOCKED(Player* pGamePlayer) {
    __BEGIN_TRY

    // call base class's method
    PlayerManager::addPlayer(pGamePlayer);

    SOCKET fd = pGamePlayer->getSocket()->getSOCKET();


    m_MinFD = min(fd, m_MinFD);
    m_MaxFD = max(fd, m_MaxFD);


    FD_SET(fd, &m_ReadFDs[0]);
    FD_SET(fd, &m_WriteFDs[0]);
    FD_SET(fd, &m_ExceptFDs[0]);

    __END_CATCH
}

void IncomingPlayerManager::deletePlayer_NOBLOCKED(SOCKET fd) {
    __BEGIN_TRY

    // call base class's method
    // filelog("deletePlayer.log", "Call in deletePlayer_NOBLOCKED");
    PlayerManager::deletePlayer(fd);

    Assert(m_pPlayers[fd] == NULL);


    if (fd == m_MinFD) {
        int i = m_MinFD;
        for (i = m_MinFD; i <= m_MaxFD; i++) {
            if (m_pPlayers[i] != NULL || i == m_SocketID) {
                m_MinFD = i;
                break;
            }
        }


        if (i > m_MaxFD)
            m_MinFD = m_MaxFD = -1;
    } else if (fd == m_MaxFD) {
        int i = m_MaxFD;
        for (i = m_MaxFD; i >= m_MinFD; i--) {
            if (m_pPlayers[i] != NULL || i == m_SocketID) {
                m_MaxFD = i;
                break;
            }
        }


        if (i < m_MinFD) {
            FILELOG_INCOMING_CONNECTION("ICMFD.txt",
                                        "[ i < m_MinFD nbl] nPlayers : %d, MinFD : %d, MaxFD : %d, ServerSocket : %d",
                                        m_nPlayers, (int)m_MinFD, (int)m_MaxFD, (int)m_SocketID);
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


//////////////////////////////////////////////////////////////////////
//

//

//


//

//
//////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::deletePlayer(SOCKET fd) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    // call base class's method
    // filelog("deletePlayer.log", "Call in deletePlayer(...) in IncomingPlayerManager.cpp");
    PlayerManager::deletePlayer(fd);

    Assert(m_pPlayers[fd] == NULL);


    if (fd == m_MinFD) {
        int i = m_MinFD;
        for (i = m_MinFD; i <= m_MaxFD; i++) {
            if (m_pPlayers[i] != NULL || i == m_SocketID) {
                m_MinFD = i;
                break;
            }
        }


        if (i > m_MaxFD)
            m_MinFD = m_MaxFD = -1;
    } else if (fd == m_MaxFD) {
        int i = m_MaxFD;
        for (i = m_MaxFD; i >= m_MinFD; i--) {
            if (m_pPlayers[i] != NULL || i == m_SocketID) {
                m_MaxFD = i;
                break;
            }
        }


        if (i < m_MinFD) {
            FILELOG_INCOMING_CONNECTION("ICMFD.txt",
                                        "[ i < m_MinFD ] nPlayers : %d, MinFD : %d, MaxFD : %d, ServerSocket : %d",
                                        m_nPlayers, (int)m_MinFD, (int)m_MaxFD, (int)m_SocketID);
            throw UnknownError("m_MinFD & m_MaxFD problem.");
        }
    }


    FD_CLR(fd, &m_ReadFDs[0]);
    FD_CLR(fd, &m_ReadFDs[1]);
    FD_CLR(fd, &m_WriteFDs[0]);
    FD_CLR(fd, &m_WriteFDs[1]);
    FD_CLR(fd, &m_ExceptFDs[0]);
    FD_CLR(fd, &m_ExceptFDs[1]);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

GamePlayer* IncomingPlayerManager::getPlayer_NOBLOCKED(const string& id) {
    __BEGIN_TRY

    GamePlayer* pGamePlayer = NULL;

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (m_pPlayers[i] != NULL) {
            if (m_pPlayers[i]->getID() == id) {
                pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
                break;
            }
        }
    }

    if (pGamePlayer == NULL)
        throw NoSuchElementException("     .");

    return pGamePlayer;

    __END_CATCH
}

GamePlayer* IncomingPlayerManager::getPlayer(const string& id) {
    __BEGIN_TRY

    GamePlayer* pGamePlayer = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    pGamePlayer = getPlayer_NOBLOCKED(id);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pGamePlayer;

    __END_CATCH
}

GamePlayer* IncomingPlayerManager::getReadyPlayer(const string& id) {
    __BEGIN_TRY

    GamePlayer* pGamePlayer = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    list<GamePlayer*>::iterator itr = find_if(m_PlayerListQueue.begin(), m_PlayerListQueue.end(), isSamePlayerbyID(id));

    if (itr == m_PlayerListQueue.end()) {
        throw NoSuchElementException();
    } else {
        pGamePlayer = (*itr);
        Assert(pGamePlayer != NULL);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pGamePlayer;

    __END_CATCH
}

void IncomingPlayerManager::pushPlayer(GamePlayer* pGamePlayer)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    m_PlayerListQueue.push_back(pGamePlayer);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

void IncomingPlayerManager::pushOutPlayer(GamePlayer* pGamePlayer)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_MutexOut)

    m_PlayerOutListQueue.push_back(pGamePlayer);

    __LEAVE_CRITICAL_SECTION(m_MutexOut)

    __END_CATCH
}

void IncomingPlayerManager::heartbeat()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    //--------------------------------------------------

    //--------------------------------------------------


    while (!m_PlayerListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerListQueue.front();

        if (pGamePlayer == NULL) {
            filelog("ZoneBug.txt", "%s : %s", "Zone::heartbeat(1)", "pGamePlayer NULL.");
            continue;
        }

        m_PlayerListQueue.pop_front();

        //-----------------------------------------------------------------------------
        // * elcastle 's Note
        //-----------------------------------------------------------------------------


        //-----------------------------------------------------------------------------
        if (pGamePlayer->isPenaltyFlag(PENALTY_TYPE_KICKED)) {
            int fd = -1;
            Socket* pSocket = pGamePlayer->getSocket();
            if (pSocket != NULL)
                fd = (int)pSocket->getSOCKET();

            FILELOG_INCOMING_CONNECTION("incomingDisconnect.log", "FD : %d, %s", fd,
                                        (pSocket == NULL ? "NULL" : pSocket->getHost().c_str()));

            // by sigi. 2002.12.30
            if (pGamePlayer->getReconnectPacket() != NULL) {
                //				UserGateway::getInstance()->passUser( UserGateway::USER_OUT_NORMAL );
            } else {
                //				UserGateway::getInstance()->passUser( UserGateway::USER_OUT_KICKED );
            }

            try {
                pGamePlayer->disconnect(DISCONNECTED);


                // by sigi. 2002.5.4
                if (pGamePlayer->isKickForLogin()) {
                    // send GLKickVerify to LoginServer. 2002.5.6
                    GLKickVerify glKickVerify;
                    glKickVerify.setKicked(true);
                    glKickVerify.setID(pGamePlayer->getSocket()->getSOCKET());
                    glKickVerify.setPCName(pGamePlayer->getCreature()->getName());

                    g_pLoginServerManager->sendPacket(pGamePlayer->getKickRequestHost(),
                                                      pGamePlayer->getKickRequestPort(), &glKickVerify);

                    cout << "LGKickVerify Send Packet to ServerIP : " << pGamePlayer->getKickRequestHost() << endl;
                    cout << "LGKickVerify Send Packet to ServerPort : " << pGamePlayer->getKickRequestPort() << endl;
                }

            } catch (Throwable& t) {
                cerr << t.toString() << endl;
            }

            Creature* pCreature = pGamePlayer->getCreature();
            if (pCreature != NULL)
                pCreature->setValue(9);
            SAFE_DELETE(pGamePlayer);

            continue;
        }


        addPlayer_NOBLOCKED(pGamePlayer);

        // filelog("ZoneHeartbeatTrace.txt", "Added Player[%s]", pGamePlayer->getID().c_str());


        if (pGamePlayer->getPlayerStatus() == GPS_WAITING_FOR_CG_READY) {
            Creature* pCreature = pGamePlayer->getCreature();
            Assert(pCreature != NULL);

            Zone* pOldZone = pCreature->getZone();

            // by sigi. 2002.5.15
            Zone* pZone = pCreature->getNewZone();
            // Assert(pZone != NULL);

            if (pOldZone != NULL) {
                if (pOldZone->isMasterLair()) {
                    MasterLairManager* pMasterLairManager = pOldZone->getMasterLairManager();
                    Assert(pMasterLairManager != NULL);
                    pMasterLairManager->leaveCreature(pCreature);
                }


                if (pCreature->isPLAYER() && pZone != NULL && pOldZone->getZoneID() != pZone->getZoneID()) {
                    if (g_pPKZoneInfoManager->isPKZone(pOldZone->getZoneID()))
                        g_pPKZoneInfoManager->leavePKZone(pOldZone->getZoneID());
                }
            }

            if (pZone == NULL) {
                pZone = pCreature->getZone();
                Assert(pZone != NULL);
            } else {
                pCreature->setZone(pZone);
                pCreature->setNewZone(NULL);

                pCreature->setXY(pCreature->getNewX(), pCreature->getNewY());


                pCreature->registerObject();
            }


#ifdef __USE_ENCRYPTER__
            pGamePlayer->setEncryptCode();
#endif

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)


            if ((pGamePlayer->isPayPlaying() || pGamePlayer->isPremiumPlay()) &&
                pGamePlayer->getPayType() == PAY_TYPE_TIME) {
                Assert(pCreature->isPC());
                PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                Assert(pPC != NULL);

                if (pPC->canPlayFree() && !pZone->isPayPlay() && !pZone->isPremiumZone()) {
                    // cout << "LogoutPayPlay:[" << (int)pZone->getZoneID() << "] "
                    //		<< (int)pZone->isPayPlay() << " , "
                    //		<< (int)pZone->isPremiumZone() << endl;
                    pGamePlayer->logoutPayPlay(pGamePlayer->getID());
                }
            }
#endif


            SEND_SYSTEM_AVAILABILITIES(pGamePlayer);

            //--------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------
            GCUpdateInfo gcUpdateInfo;

            makeGCUpdateInfo(&gcUpdateInfo, pCreature);

            pGamePlayer->sendPacket(&gcUpdateInfo);


        } else if (pGamePlayer->getPlayerStatus() == GPS_AFTER_SENDING_GL_INCOMING_CONNECTION) {
            //			cout << "Logout..." << pGamePlayer->getID() << endl;


            GLIncomingConnection glIncomingConnection;
            glIncomingConnection.setPlayerID(pGamePlayer->getID());
            glIncomingConnection.setClientIP(pGamePlayer->getSocket()->getHost());

            static int portNum = g_pConfig->getPropertyInt("LoginServerUDPPortNum");

            int port;

            if (portNum > 1) {
                port = g_pConfig->getPropertyInt("LoginServerBaseUDPPort") + rand() % portNum;
            } else {
                port = g_pConfig->getPropertyInt("LoginServerUDPPort");
            }

            // cout << "ReconnectAddress = " << g_pConfig->getProperty("LoginServerIP").c_str() << ":" << port << endl;

            /*
            if (g_pConfig->getProperty("User") == "excel96")
                g_pLoginServerManager->sendPacket(g_pConfig->getProperty("LoginServerIP") , port,
            &glIncomingConnection); else if (g_pConfig->getProperty("User") == "elcastle")
                g_pLoginServerManager->sendPacket(g_pConfig->getProperty("LoginServerIP") , port,
            &glIncomingConnection); else if (g_pConfig->getProperty("User") == "elca")
                g_pLoginServerManager->sendPacket(g_pConfig->getProperty("LoginServerIP") , port,
            &glIncomingConnection);
            */


            g_pLoginServerManager->sendPacket(g_pConfig->getProperty("LoginServerIP"), port, &glIncomingConnection);
        }

        // filelog("ZoneHeartbeatTrace.txt", "After pop front");
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)


    // by sigi. 2002.12.10
    __ENTER_CRITICAL_SECTION(m_MutexOut)

    while (!m_PlayerOutListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerOutListQueue.front();

        m_PlayerOutListQueue.pop_front();

        try {
            Assert(pGamePlayer != NULL);

            Creature* pCreature = pGamePlayer->getCreature();
            Assert(pCreature != NULL);


            // Zone * pZone = pCreature->getZone();
            // Assert(pZone != NULL);
            Zone* pZone = pCreature->getNewZone();
            // Assert(pZone != NULL);


            if (pZone == NULL) {
                pZone = pCreature->getZone();
                Assert(pZone != NULL);
            }


            ZoneGroup* pZoneGroup = pZone->getZoneGroup();
            Assert(pZoneGroup != NULL);
            ZonePlayerManager* pZonePlayerManager = pZoneGroup->getZonePlayerManager();
            Assert(pZonePlayerManager != NULL);


            pZonePlayerManager->pushPlayer(pGamePlayer);
        } catch (...) {
            filelog("IncomingPlayerManager.txt", "AssertionError! IncomingPlayManager.cpp line 1594");
        }
    }

    __LEAVE_CRITICAL_SECTION(m_MutexOut)

    __END_CATCH
}

void IncomingPlayerManager::deleteQueuePlayer(GamePlayer* pGamePlayer) {
    __BEGIN_TRY


    // by sigi. 2002.5.9

    __ENTER_CRITICAL_SECTION(m_MutexOut)

    Assert(pGamePlayer != NULL);

    list<GamePlayer*>::iterator itr =
        find_if(m_PlayerOutListQueue.begin(), m_PlayerOutListQueue.end(), isSamePlayer(pGamePlayer));

    if (itr != m_PlayerOutListQueue.end()) {
        m_PlayerOutListQueue.erase(itr);
    }

    __LEAVE_CRITICAL_SECTION(m_MutexOut)

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void IncomingPlayerManager::clearPlayers()

{
    __BEGIN_TRY


    while (!m_PlayerListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerListQueue.front();

        m_PlayerListQueue.pop_front();

        if (pGamePlayer != NULL) {
            try {
                pGamePlayer->disconnect();
            } catch (Throwable& t) {
            }

            SAFE_DELETE(pGamePlayer);
        }
    }


    while (!m_PlayerOutListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerOutListQueue.front();

        m_PlayerOutListQueue.pop_front();

        if (pGamePlayer != NULL) {
            try {
                pGamePlayer->disconnect();
            } catch (Throwable& t) {
            }

            SAFE_DELETE(pGamePlayer);
        }
    }


    if (m_MinFD == -1 && m_MaxFD == -1)
        return;


    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (i != m_SocketID && m_pPlayers[i] != NULL) {
            GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);

            if (pGamePlayer != NULL) {
                try {
                    pGamePlayer->disconnect();
                } catch (Throwable& t) {
                }

                SAFE_DELETE(pGamePlayer);
            }
        }
    }

    __END_CATCH
}


// external variable definition
IncomingPlayerManager* g_pIncomingPlayerManager = NULL;
