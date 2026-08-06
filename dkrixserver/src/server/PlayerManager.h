//--------------------------------------------------------------------------------
//
// Filename    : PlayerManager.h
// Written by  : reiot@ewestsoft.com
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __PLAYER_MANAGER_H__
#define __PLAYER_MANAGER_H__

// include files
#include "Exception.h"
#include "Mutex.h"
#include "SocketAPI.h"
#include "Timeval.h"
#include "Types.h"

// forward declaration
class Player;
class Packet;

//--------------------------------------------------------------------------------
//
// class PlayerManager;
//




//

//
// 		900 x 4(byte) x 10(#ZoneGroup) = 36k
//

//
//--------------------------------------------------------------------------------

class PlayerManager {
public:
    
    const static uint nMaxPlayers = 2000;

public:
    // constructor
    PlayerManager();

    // destructor
    virtual ~PlayerManager();

    // broadcast message
    virtual void broadcastPacket(Packet* pPacket);

    
    virtual void addPlayer(Player* pPlayer);

    
    virtual void deletePlayer(SOCKET fd);

    
    virtual Player* getPlayer(SOCKET fd);

    
    virtual Player* getPlayerByPhoneNumber(PhoneNumber_t PhoneNumber) {
        return NULL;
    }

    
    uint size() const {
        return m_nPlayers;
    }

    // Copy Player
    void copyPlayers();

protected:
    
    Player* m_pPlayers[nMaxPlayers];

    // number of Players
    uint m_nPlayers;

    
    Player* m_pCopyPlayers[nMaxPlayers];
};

#endif
