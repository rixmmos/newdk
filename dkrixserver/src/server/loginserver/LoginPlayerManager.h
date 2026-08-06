//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginPlayerManager.h
// Written by  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __LOGIN_PLAYER_MANAGER_H__
#define __LOGIN_PLAYER_MANAGER_H__

// include files
#include "Exception.h"
#include "PlayerManager.h"
#include "ServerSocket.h"
#include "Types.h"

class LoginPlayer;

//////////////////////////////////////////////////////////////////////
//
// class LoginPlayerManager;
//

//
//////////////////////////////////////////////////////////////////////

class LoginPlayerManager : public PlayerManager {
public:
    // constructor
    LoginPlayerManager();

    // destructor
    ~LoginPlayerManager();

public:
    
    void init();

    // accept new connection
    void acceptNewConnection();

    
    void select();

    
    void processInputs();

    
    void processOutputs();

    
    void processCommands();

    
    void processExceptions();

public:
    
    void broadcastPacket(Packet* pPacket);

    
    void sendPacket(const string& id, Packet* pPacket);

    
    void addPlayer(Player* pPlayer);
    void addPlayer_NOLOCKED(Player* pPlayer);

    
    void deletePlayer(SOCKET fd);
    void deletePlayer_NOLOCKED(SOCKET fd);

    
    LoginPlayer* getPlayer(const string& PCName) const;
    LoginPlayer* getPlayer_NOLOCKED(const string& PCName) const;

    // lock/unlock
    void lock() {
        m_Mutex.lock();
    }
    void unlock() {
        m_Mutex.unlock();
    }

    // get debug string
    string toString() const;

private:
    
    ServerSocket* m_pServerSocket;

    
    SOCKET m_ServerFD;

    
    
    
    fd_set m_ReadFDs[2];
    fd_set m_WriteFDs[2];
    fd_set m_ExceptFDs[2];

    
    Timeval m_Timeout[2];

    // min_fd , max_fd
    
    
    SOCKET m_MinFD;
    SOCKET m_MaxFD;

    
    
    
    
    
    
    mutable Mutex m_Mutex;
};

// external variable declaration
extern LoginPlayerManager* g_pLoginPlayerManager;

#endif
