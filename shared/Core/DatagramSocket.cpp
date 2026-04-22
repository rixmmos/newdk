//////////////////////////////////////////////////////////////////////
//
// Filename   : DatagramSocket.cpp
// Description: Shared canonical UDP socket wrapper
//
//////////////////////////////////////////////////////////////////////

#include "DatagramSocket.h"

#include <cstring>

DatagramSocket::DatagramSocket() : m_SocketID(INVALID_SOCKET) {
    __BEGIN_TRY

    m_SocketID = SocketAPI::socket_ex(AF_INET, SOCK_DGRAM, 0);

    __END_CATCH
}

DatagramSocket::DatagramSocket(uint port) : m_SocketID(INVALID_SOCKET) {
    __BEGIN_TRY

    m_SocketID = SocketAPI::socket_ex(AF_INET, SOCK_DGRAM, 0);

    memset(&m_SockAddr, 0, szSOCKADDR_IN);
    m_SockAddr.sin_family = AF_INET;
    m_SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_SockAddr.sin_port = htons(port);

    int opt = 1;
    SocketAPI::setsockopt_ex(m_SocketID, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    SocketAPI::bind_ex(m_SocketID, (SOCKADDR*)&m_SockAddr, szSOCKADDR_IN);

    __END_CATCH
}

DatagramSocket::~DatagramSocket() noexcept {
    if (m_SocketID != INVALID_SOCKET) {
        try {
            SocketAPI::closesocket_ex(m_SocketID);
        } catch (Throwable&) {
        }
    }
}

uint DatagramSocket::send(Datagram* pDatagram) {
    __BEGIN_TRY

    if (pDatagram == NULL)
        throw Error("DatagramSocket::send received null datagram.");

    int nSent = SocketAPI::sendto_ex(
        m_SocketID, pDatagram->getData(), pDatagram->getLength(), 0, pDatagram->getAddress(), szSOCKADDR_IN);

    return (uint)nSent;

    __END_CATCH
}

Datagram* DatagramSocket::receive() {
    __BEGIN_TRY

    Datagram* pDatagram = NULL;

    SOCKADDR_IN sockAddr;
    uint sockAddrSize = szSOCKADDR_IN;

    ulong available = SocketAPI::availablesocket_ex(m_SocketID);
    if (available > 0) {
        int nReceived = SocketAPI::recvfrom_ex(
            m_SocketID, m_Buffer, DATAGRAM_SOCKET_BUFFER_LEN, 0, (SOCKADDR*)&sockAddr, &sockAddrSize);

        if (nReceived > 0) {
            pDatagram = new Datagram();
            pDatagram->setData(m_Buffer, nReceived);
            pDatagram->setAddress(&sockAddr);
        }
    }

    return pDatagram;

    __END_CATCH
}
