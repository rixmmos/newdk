//////////////////////////////////////////////////////////////////////////////
// Filename    : SharedServerManager.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __SHARED_SERVER_MANANGER_H__
#define __SHARED_SERVER_MANANGER_H__

#include "Exception.h"
#include "Mutex.h"
#include "Packet.h"
#include "Socket.h"
#include "Thread.h"
#include "Types.h"

class SharedServerClient;

//////////////////////////////////////////////////////////////////////////////
// class SharedServerManager;
//


//////////////////////////////////////////////////////////////////////////////

class SharedServerManager : public Thread {
public:
    SharedServerManager();
    ~SharedServerManager();

public:
    void init() {}

    void stop();

    void run();

    void sendPacket(Packet* pPacket);

private:
    SharedServerClient* m_pSharedServerClient;

    mutable Mutex m_Mutex;
};

// global variable declaration
extern SharedServerManager* g_pSharedServerManager;

#endif
