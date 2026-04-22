//////////////////////////////////////////////////////////////////////
//
// Filename   : DatagramSocket.h
// Description: Shared canonical UDP socket wrapper
//
//////////////////////////////////////////////////////////////////////

#ifndef __DATAGRAM_SOCKET_H__
#define __DATAGRAM_SOCKET_H__

#include "Datagram.h"
#include "Exception.h"
#include "SocketAPI.h"
#include "Types.h"

const uint DATAGRAM_SOCKET_BUFFER_LEN = 65536;

class DatagramSocket {
public:
    DatagramSocket();
    DatagramSocket(uint port);
    ~DatagramSocket() noexcept;

    uint send(Datagram* pDatagram);
    Datagram* receive();

    SOCKET getSOCKET() const { return m_SocketID; }

private:
    SOCKET m_SocketID;
    SOCKADDR_IN m_SockAddr;
    char m_Buffer[DATAGRAM_SOCKET_BUFFER_LEN];
};

#endif
