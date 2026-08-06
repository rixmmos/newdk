//////////////////////////////////////////////////////////////////////////////
// Filename    : LoginServerManager.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __LOGIN_SERVER_MANANGER_H__
#define __LOGIN_SERVER_MANANGER_H__

#include "DatagramSocket.h"
#include "Exception.h"
#include "Mutex.h"
#include "Thread.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class LoginServerManager;
//


//////////////////////////////////////////////////////////////////////////////

class LoginServerManager : public Thread {
public:
    LoginServerManager();
    ~LoginServerManager();

public:
    void init() {}

    void stop();

    void run();

    void sendDatagram(Datagram* pDatagram);

    void sendPacket(const string& host, uint port, DatagramPacket* pPacket);

    void lock() const {
        m_Mutex.lock();
    }
    void unlock() const {
        m_Mutex.unlock();
    }

private:
    DatagramSocket* m_pDatagramSocket; 

    mutable Mutex m_Mutex;
};

// global variable declaration
extern LoginServerManager* g_pLoginServerManager;

#endif
