//////////////////////////////////////////////////////////////////////////////
// Filename    : IncomingPlayerManager.h
// Written by  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __INCOMING_PLAYER_MANAGER_H__
#define __INCOMING_PLAYER_MANAGER_H__

#include "ConnectionInfoManager.h"
#include "DatagramSocket.h"
#include "Exception.h"
#include "GamePlayer.h"
#include "Mutex.h"
#include "PlayerManager.h"
#include "ServerSocket.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class IncomingPlayerManager;
//




//


//////////////////////////////////////////////////////////////////////////////

class IncomingPlayerManager : public PlayerManager {
public:
    IncomingPlayerManager();
    ~IncomingPlayerManager();

public:
    // initialize
    void init();

    // broadcast packet to all players
    void broadcast(Packet* pPacket);

    

    // select
    void select();

    // process all players' inputs
    void processInputs();

    // process all players' outputs
    void processOutputs();

    // process all players' exceptions
    void processExceptions();

    // process all players' commands
    void processCommands();

    // accept new connection
    bool acceptNewConnection();

    void copyPlayers();

    // add/delete player
    void addPlayer(Player* pGamePlayer);
    void addPlayer_NOBLOCKED(Player* pGamePlayer);
    void deletePlayer(SOCKET fd);
    void deletePlayer_NOBLOCKED(SOCKET fd);

    // get Player by string
    GamePlayer* getPlayer_NOBLOCKED(const string& id);
    GamePlayer* getPlayer(const string& id);
    GamePlayer* getReadyPlayer(const string& id);

    // lock/unlock
    void lock() {
        m_Mutex.lock();
    }
    void unlock() {
        m_Mutex.unlock();
    }

    // push Player to queue
    void pushPlayer(GamePlayer* pGamePlayer);

    void pushOutPlayer(GamePlayer* pGamePlayer);

    // Queue's Player Add Manager
    void heartbeat();

    void deleteQueuePlayer(GamePlayer* pGamePlayer);

    
    void clearPlayers();

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

    list<GamePlayer*> m_PlayerListQueue;
    list<GamePlayer*> m_PlayerOutListQueue;

    int m_CheckValue; // by sigi. for debugging. 2002.11.11

    mutable Mutex m_MutexOut;
};

// external variable declaration
extern IncomingPlayerManager* g_pIncomingPlayerManager;

#endif
