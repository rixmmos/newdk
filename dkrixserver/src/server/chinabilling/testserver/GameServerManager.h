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
    GameServerManager() throw(Error);
    ~GameServerManager() throw(Error);

public:
    
    const static uint nMaxGameServers = 100;

    // initialize
    void init() throw(Error);

    void run() throw(Error);

    // broadcast packet to all players
    void broadcast(Packet* pPacket) throw(Error);
    void broadcast(Packet* pPacket, Player* pPlayer) throw(Error);


    // select
    void select() throw(TimeoutException, InterruptedException, Error);

    // process all inputs
    void processInputs() throw(IOException, Error);

    // process all outputs
    void processOutputs() throw(IOException, Error);

    // process all exceptions
    void processExceptions() throw(IOException, Error);

    // process all commands
    void processCommands() throw(IOException, Error);

    // accept new connection
    void acceptNewConnection() throw(Error);

    // add/delete player
    void addGameServerPlayer(GameServerPlayer* pGameServerPlayer) throw(DuplicatedException, Error);
    void deleteGameServerPlayer(SOCKET fd) throw(OutOfBoundException, NoSuchElementException, Error);

    // lock/unlock
    void lock() throw(Error) {
        m_Mutex.lock();
    }
    void unlock() throw(Error) {
        m_Mutex.unlock();
    }

    void heartbeat() throw(Error);

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
