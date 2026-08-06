//////////////////////////////////////////////////////////////////////
//
// Filename    : GameServerManager.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GAME_SERVER_MANANGER_H__
#define __GAME_SERVER_MANANGER_H__

// include files
#include "DatagramSocket.h"
#include "Exception.h"
#include "Thread.h"
#include "Timeval.h"
#include "Types.h"

class Datagram;
class DatagramPacket;

//////////////////////////////////////////////////////////////////////
//
// class GameServerManager;
//

//

//
//////////////////////////////////////////////////////////////////////

class GameServerManager {
public:
    GameServerManager() throw(Error);
    ~GameServerManager() throw(Error);

    void init() throw(Error) {}

    void start() throw(Error) {
        m_bRunning = true;
        run();
    }
    void stop() throw(Error) {
        m_bRunning = false;
    }

    // main method
    void run() throw(Error);

    void sendDatagram(Datagram* pDatagram) throw(ConnectException, Error);
    void sendPacket(string host, uint port, DatagramPacket* pPacket) throw(ConnectException, Error);

private:
    
    DatagramSocket* m_pDatagramSocket;
    bool m_bRunning;
};

// global variable declaration
extern GameServerManager* g_pGameServerManager;

#endif
