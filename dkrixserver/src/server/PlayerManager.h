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
    // Size of the descriptor-indexed player table. This is a descriptor bound,
    // not a headcount: m_pPlayers is indexed by the connection's file
    // descriptor, and every manager derived from this class multiplexes with
    // select(), whose fd_set is a fixed-width bitmap.
    //
    // On glibc that width is __FD_SETSIZE, hard-wired to 1024 in
    // bits/typesizes.h. Defining FD_SETSIZE before <sys/select.h> is silently
    // ignored -- measured on glibc 2.39 / g++ 13.3, sizeof(fd_set) is 128
    // bytes (1024 bits) with and without the define. Winsock does honour such
    // a define, which is likely why this constant used to read 2000.
    //
    // 2000 was therefore a capacity the process could not serve: descriptors
    // 1024..1999 fit the array but not the bitmap, and all three accept paths
    // now refuse them before any FD_SET (18-AK). Sizing the table at exactly
    // the fd_set width makes the bound honest and removes no capacity that
    // ever worked.
    //
    // The usable ceiling is a little lower again -- roughly 1000 concurrent
    // connections -- because the low descriptors go to stdio, the listening
    // socket, database connections and log files.
    const static uint nMaxPlayers = 1024;

#ifdef FD_SETSIZE
    // Reached via SocketAPI.h -> <sys/types.h> -> <sys/select.h>. Keeps the
    // array bound and the fd_set width from drifting apart again.
    static_assert(nMaxPlayers <= (uint)FD_SETSIZE, "player table must not exceed the fd_set bitmap width");
#endif

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
