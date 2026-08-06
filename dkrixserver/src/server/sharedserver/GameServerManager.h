//////////////////////////////////////////////////////////////////////////////
// Filename    : GameServerManager.h
// Written by  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_SERVER_MANAGER_H__
#define __GAME_SERVER_MANAGER_H__

#include "Exception.h"
#include "GameServerPlayer.h"
#include "Mutex.h"
#include "ServerSocket.h"
#include "Thread.h"
#include "Timeval.h"
#include "Types.h"


//////////////////////////////////////////////////////////////////////////////
//
// class GameServerManager;
//
//////////////////////////////////////////////////////////////////////////////

class GameServerManager : public Thread {
public:
    GameServerManager();
    ~GameServerManager();

public:
    
    const static uint nMaxGameServers = 100;

    // initialize
    void init();

    void run();

    // broadcast packet to all players
    void broadcast(Packet* pPacket);
    void broadcast(Packet* pPacket, Player* pPlayer);


    // select
    void select();

    // process all inputs
    void processInputs();

    // process all outputs
    void processOutputs();

    // process all exceptions
    void processExceptions();

    // process all commands
    void processCommands();

    // accept new connection
    void acceptNewConnection();

    // add/delete player
    void addGameServerPlayer(GameServerPlayer* pGameServerPlayer);
    void deleteGameServerPlayer(SOCKET fd);

    // lock/unlock
    void lock() {
        m_Mutex.lock();
    }
    void unlock() {
        m_Mutex.unlock();
    }

    void heartbeat();

private:
    
    ServerSocket* m_pServerSocket;
    SOCKET m_SocketID;

    
    
    
    fd_set m_ReadFDs[2];
    fd_set m_WriteFDs[2];
    fd_set m_ExceptFDs[2];

    
    Timeval m_Timeout[2];

    // min_fd, max_fd
    
    
    SOCKET m_MinFD;
    SOCKET m_MaxFD;

    // mutex
    mutable Mutex m_Mutex;


    
    GameServerPlayer* m_pGameServerPlayers[nMaxGameServers];
};

// external variable declaration
extern GameServerManager* g_pGameServerManager;

#endif
