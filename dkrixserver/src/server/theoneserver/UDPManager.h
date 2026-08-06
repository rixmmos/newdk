//////////////////////////////////////////////////////////////////////////////
// Filename    : UDPManager.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __UDP_MANAGER_H__
#define __UDP_MANAGER_H__

#include "DatagramSocket.h"
#include "Exception.h"
#include "Mutex.h"
#include "Thread.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class UDPManager;
//


//////////////////////////////////////////////////////////////////////////////

class UDPManager {
public:
    UDPManager() throw(Error);
    ~UDPManager() throw(Error);

public:
    void sendDatagram(Datagram* pDatagram) throw(ProtocolException, Error);

    void sendPacket(const string& host, uint port, DatagramPacket* pPacket) throw(ProtocolException, Error);


private:
    DatagramSocket* m_pDatagramSocket; 
};

// global variable declaration
extern UDPManager* g_pUDPManager;

#endif
