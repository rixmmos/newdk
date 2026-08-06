//////////////////////////////////////////////////////////////////////////////
// Filename    : GameServerManager.cpp
// Written by  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GameServerManager.h"

#include <stdio.h>

#include <algorithm>

#include "Assert.h"
#include "DB.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Packet.h"
#include "Properties.h"
#include "Socket.h"
#include "SocketAPI.h"


//////////////////////////////////////////////////////////////////////////////
// constructor

//////////////////////////////////////////////////////////////////////////////

GameServerManager::GameServerManager() : m_pServerSocket(NULL), m_SocketID(INVALID_SOCKET), m_MinFD(-1), m_MaxFD(-1) {
    __BEGIN_TRY

    m_Mutex.setName("GameServerManager");

    try {
        // create  server socket
        while (true) {
            try {
                m_pServerSocket = new ServerSocket(g_pConfig->getPropertyInt("TCPPort"));
                break;
            } catch (BindException& b) {
                SAFE_DELETE(m_pServerSocket);
                cout << "GameServerManager(" << g_pConfig->getPropertyInt("TCPPort") << ") : " << b.toString() << endl;
                sleep(1);
            }
        }

        m_pServerSocket->setNonBlocking();

        
        m_SocketID = m_pServerSocket->getSOCKET();
    } catch (NoSuchElementException& nsee) {
        
        throw Error(nsee.toString());
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////

GameServerManager::~GameServerManager() noexcept {}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

void GameServerManager::init() {
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

    __END_CATCH
}


void GameServerManager::run() {
    __BEGIN_TRY
    __BEGIN_DEBUG

    try {
        Timeval dummyQueryTime;
        getCurrentTime(dummyQueryTime);

        while (true) {
            try {
                usleep(1000); 

                select();

                processInputs();

                processOutputs();
            } catch (Throwable& t) {
                filelog("SSGSManager.txt", "%s", t.toString().c_str());
            }

            processCommands();

            g_pGuildManager->heartbeat();

            Timeval currentTime;
            getCurrentTime(currentTime);

            if (dummyQueryTime < currentTime) {
                g_pDatabaseManager->executeDummyQuery(g_pDatabaseManager->getConnection("DARKEDEN"));

                dummyQueryTime.tv_sec = (60 + rand() % 30) * 60;
            }
        }

    } catch (Throwable& t) {
        filelog("sharedserverBug.txt", "%s", t.toString().c_str());
        throw;
    }

    __END_DEBUG
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void GameServerManager::broadcast(Packet* pPacket) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    try {
        for (int i = m_MinFD; i <= m_MaxFD; i++) {
            if (i != m_SocketID && m_pGameServerPlayers[i] != NULL)
                m_pGameServerPlayers[i]->sendPacket(pPacket);
        }
    } catch (const ProtocolException& e) {
        filelog("SSException.log", "%s\n%s", e.toString().c_str(), pPacket->toString().c_str());
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void GameServerManager::broadcast(Packet* pPacket, Player* pPlayer) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (i != m_SocketID && m_pGameServerPlayers[i] != NULL && m_pGameServerPlayers[i] != pPlayer)
            m_pGameServerPlayers[i]->sendPacket(pPacket);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// call select() system call

//////////////////////////////////////////////////////////////////////////////
void GameServerManager::select() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    
    m_Timeout[1].tv_sec = m_Timeout[0].tv_sec;
    m_Timeout[1].tv_usec = m_Timeout[0].tv_usec;

    
    m_ReadFDs[1] = m_ReadFDs[0];
    m_WriteFDs[1] = m_WriteFDs[0];
    m_ExceptFDs[1] = m_ExceptFDs[0];

    try {
        
        SocketAPI::select_ex(m_MaxFD + 1, &m_ReadFDs[1], &m_WriteFDs[1], &m_ExceptFDs[1], &m_Timeout[1]);
    } catch (InterruptedException& ie) {
        
        // log(LOG_GAMESERVER_ERROR, "", "", ie.toString());
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// process all players' inputs



//////////////////////////////////////////////////////////////////////////////
void GameServerManager::processInputs() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_ReadFDs[1])) {
            if (i == m_SocketID) {
                
                acceptNewConnection();
            } else {
                if (m_pGameServerPlayers[i] != NULL) {
                    GameServerPlayer* pGameServerPlayer = m_pGameServerPlayers[i];
                    Assert(pGameServerPlayer != NULL);
                    Assert(m_pGameServerPlayers[i] != NULL);

                    if (pGameServerPlayer->getSocket()->getSockError()) {
                        try {
                            
                            pGameServerPlayer->disconnect(DISCONNECTED);
                        } catch (Throwable& t) {
                            cerr << t.toString() << endl;
                        }

                        deleteGameServerPlayer(i);

                        delete pGameServerPlayer;
                    } else {
                        try {
                            pGameServerPlayer->processInput();
                        } catch (ConnectException& ce) {
                            
                            
                            
                            
                            try {
                                pGameServerPlayer->disconnect();
                            } catch (Throwable& t) {
                                cerr << t.toString() << endl;
                            }

                            deleteGameServerPlayer(i);

                            delete pGameServerPlayer;
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

void GameServerManager::processCommands() {
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
        if (i != m_SocketID && m_pGameServerPlayers[i] != NULL) {
            GameServerPlayer* pGameServerPlayer = m_pGameServerPlayers[i];
            Assert(pGameServerPlayer != NULL);
            Assert(m_pGameServerPlayers[i] != NULL);

            if (pGameServerPlayer->getSocket()->getSockError()) {
                try {
                    
                    pGameServerPlayer->disconnect();
                } catch (Throwable& t) {
                    cerr << t.toString() << endl;
                }

                deleteGameServerPlayer(i);

                delete pGameServerPlayer;
            } else {
                try {
                    pGameServerPlayer->processCommand();
                } catch (ProtocolException& pe) {
                    try {
                        pGameServerPlayer->disconnect();
                        cout << pe.toString().c_str() << endl;
                    } catch (Throwable& t) {
                        cerr << t.toString() << endl;
                    }

                    deleteGameServerPlayer(i);

                    delete pGameServerPlayer;
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

void GameServerManager::processOutputs() {
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
            if (i == m_SocketID)
                throw IOException("server socket's write bit is selected.");

            if (m_pGameServerPlayers[i] != NULL) {
                GameServerPlayer* pGameServerPlayer = m_pGameServerPlayers[i];

                Assert(pGameServerPlayer != NULL);
                Assert(m_pGameServerPlayers[i] != NULL);

                if (pGameServerPlayer->getSocket()->getSockError()) {
                    try {
                        
                        pGameServerPlayer->disconnect(DISCONNECTED);
                    } catch (Throwable& t) {
                        cerr << t.toString() << endl;
                    }

                    GameServerPlayer* pGameServerPlayer = pGameServerPlayer;

                    deleteGameServerPlayer(i);

                    delete pGameServerPlayer;
                } else {
                    try {
                        pGameServerPlayer->processOutput();
                    } catch (ConnectException& ce) {
                        StringStream msg;
                        msg << "DISCONNECT " << pGameServerPlayer->getID() << "(" << ce.toString() << ")";
                        // log(LOG_GAMESERVER_ERROR, "", "", msg.toString());

                        try {
                            
                            pGameServerPlayer->disconnect(DISCONNECTED);
                        } catch (Throwable& t) {
                            cerr << t.toString() << endl;
                        }

                        deleteGameServerPlayer(i);

                        delete pGameServerPlayer;
                    } catch (ProtocolException& cp) {
                        StringStream msg;
                        msg << "DISCONNECT " << pGameServerPlayer->getID() << "(" << cp.toString() << ")";
                        // log(LOG_GAMESERVER_ERROR, "", "", cp.toString());

                        

                        try {
                            pGameServerPlayer->disconnect(DISCONNECTED);
                        } catch (Throwable& t) {
                            cerr << t.toString() << endl;
                        }

                        deleteGameServerPlayer(i);

                        delete pGameServerPlayer;
                    }
                }
            }
        }
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// process all players' exceptions


//////////////////////////////////////////////////////////////////////////////

void GameServerManager::processExceptions() {
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
                if (m_pGameServerPlayers[i] != NULL) {
                    GameServerPlayer* pGameServerPlayer = m_pGameServerPlayers[i];
                    Assert(pGameServerPlayer != NULL);
                    Assert(i != m_SocketID);
                    Assert(m_pGameServerPlayers[i] != NULL);
                    StringStream msg;
                    msg << "OOB from " << pGameServerPlayer->toString();

                    try {
                        pGameServerPlayer->disconnect();
                    } catch (Throwable& t) {
                        // cerr << t.toString() << endl;
                    }

                    deleteGameServerPlayer(i);

                    delete pGameServerPlayer;
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
void GameServerManager::acceptNewConnection() {
    __BEGIN_TRY

    
    
    
    Socket* client = NULL;

    try {
        client = m_pServerSocket->accept();
    } catch (Throwable& t) {
    }

    if (client == NULL) {
        return;
    }

    try {
        
        
        
        if (client->getSockError())
            throw Error();
        client->setNonBlocking(true);

        
        
        
        if (client->getSockError())
            throw Error();
        // set socket option (!NonBlocking, NoLinger)
        client->setLinger(0);

        // StringStream msg;
        // cout << "NEW CONNECTION FROM " << client->getHost() << ":" << client->getPort();

        
        GameServerPlayer* pGameServerPlayer = new GameServerPlayer(client);

        
        try {
            addGameServerPlayer(pGameServerPlayer);
        } catch (DuplicatedException&) {
            client->close();
            SAFE_DELETE(client);
            SAFE_DELETE(pGameServerPlayer);
            return;
        }
    } catch (NoSuchElementException&) {
        StringStream msg2;
        msg2 << "ILLEGAL ACCESS FROM " << client->getHost() << ":" << client->getPort();
        // log(LOG_GAMESERVER, "", "", msg2.toString());

        
        client->send("Error : Unauthorized access", 27);
        client->close();
        SAFE_DELETE(client);
    } catch (Throwable& t) {
        try {
            if (client != NULL) {
                //				client->close();
                SAFE_DELETE(client);
            }
        } catch (Throwable& t) {
        } catch (...) {
        }
    } catch (exception& e) {
    } catch (...) {
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GameServerManager::addGameServerPlayer(GameServerPlayer* pGameServerPlayer) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    SOCKET fd = pGameServerPlayer->getSocket()->getSOCKET();

    
    m_MinFD = min(fd, m_MinFD);
    m_MaxFD = max(fd, m_MaxFD);

    
    
    FD_SET(fd, &m_ReadFDs[0]);
    FD_SET(fd, &m_WriteFDs[0]);
    FD_SET(fd, &m_ExceptFDs[0]);

    m_pGameServerPlayers[fd] = pGameServerPlayer;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GameServerManager::deleteGameServerPlayer(SOCKET fd) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    m_pGameServerPlayers[fd] = NULL;

    
    
    if (fd == m_MinFD) {
        
        
        int i = m_MinFD;
        for (i = m_MinFD; i <= m_MaxFD; i++) {
            if (m_pGameServerPlayers[i] != NULL || i == m_SocketID) {
                m_MinFD = i;
                break;
            }
        }

        
        
        
        if (i > m_MaxFD)
            m_MinFD = m_MaxFD = -1;
    } else if (fd == m_MaxFD) {
        
        
        int i = m_MaxFD;
        for (i = m_MaxFD; i >= m_MinFD; i--) {
            if (m_pGameServerPlayers[i] != NULL || i == m_SocketID) {
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

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

void GameServerManager::heartbeat(){__BEGIN_TRY

                                        __ENTER_CRITICAL_SECTION(m_Mutex)

                                            __LEAVE_CRITICAL_SECTION(m_Mutex)

                                                __END_CATCH}

// external variable definition
GameServerManager* g_pGameServerManager = NULL;
