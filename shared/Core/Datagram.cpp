//////////////////////////////////////////////////////////////////////
//
// Filename    : Datagram.cpp
// Description : Shared canonical datagram wrapper
//
//////////////////////////////////////////////////////////////////////

#include "Datagram.h"

#include <cstring>
#include "DatagramPacket.h"
#include "PacketFactoryManager.h"

Datagram::Datagram() : m_Length(0), m_InputOffset(0), m_OutputOffset(0), m_Data(NULL) {
    __BEGIN_TRY

    memset(&m_SockAddr, 0, sizeof(m_SockAddr));
    m_SockAddr.sin_family = AF_INET;

    __END_CATCH
}

Datagram::~Datagram() noexcept {
    delete[] m_Data;
    m_Data = NULL;
}

bool Datagram::isDatagram(PacketID_t packetID) const {
    switch (packetID) {
    case Packet::PACKET_CG_PORT_CHECK:
    case Packet::PACKET_GG_COMMAND:
    case Packet::PACKET_GG_GUILD_CHAT:
    case Packet::PACKET_GG_SERVER_CHAT:
    case Packet::PACKET_GL_INCOMING_CONNECTION:
    case Packet::PACKET_GL_INCOMING_CONNECTION_ERROR:
    case Packet::PACKET_GL_INCOMING_CONNECTION_OK:
    case Packet::PACKET_GL_KICK_VERIFY:
    case Packet::PACKET_GM_SERVER_INFO:
    case Packet::PACKET_GTO_ACKNOWLEDGEMENT:
    case Packet::PACKET_LG_INCOMING_CONNECTION:
    case Packet::PACKET_LG_INCOMING_CONNECTION_ERROR:
    case Packet::PACKET_LG_INCOMING_CONNECTION_OK:
    case Packet::PACKET_LG_KICK_CHARACTER:
    case Packet::PACKET_RC_SAY:
        return true;
    default:
        return false;
    }
}

void Datagram::read(char* buf, uint len) {
    __BEGIN_TRY

    if (buf == NULL)
        throw Error("Datagram read buffer is null.");

    if (m_InputOffset + len > m_Length)
        throw InsufficientDataException("Datagram read");

    memcpy(buf, &m_Data[m_InputOffset], len);
    m_InputOffset += len;

    __END_CATCH
}

void Datagram::read(std::string& str, uint len) {
    __BEGIN_TRY

    if (m_InputOffset + len > m_Length)
        throw InsufficientDataException("Datagram read");

    str.reserve(len);
    str.assign(&m_Data[m_InputOffset], len);
    m_InputOffset += len;

    __END_CATCH
}

void Datagram::read(DatagramPacket*& pPacket) {
    __BEGIN_TRY

    if (pPacket != NULL)
        throw Error("Datagram target packet must be null before read().");

    PacketID_t packetID;
    PacketSize_t packetSize;

    read((char*)&packetID, szPacketID);
    read((char*)&packetSize, szPacketSize);

    if (packetID >= Packet::PACKET_MAX)
        throw InvalidProtocolException("invalid packet id(datagram)");

    if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID))
        throw InvalidProtocolException("too large packet size(datagram)");

    if (m_Length < szPacketHeader + packetSize)
        throw Error("Datagram payload shorter than packet header plus body.");

    if (m_Length > szPacketHeader + packetSize)
        throw Error("Datagram payload larger than packet header plus body.");

#if defined(__GAME_SERVER__) || defined(__LOGIN_SERVER__) || defined(__SHARED_SERVER__)
    if (!isDatagram(packetID)) {
        filelog("datagram.txt", "id:%u host:%s", packetID, getHost().c_str());
        throw InvalidProtocolException("packet is not UDP");
    }
#endif

    pPacket = (DatagramPacket*)g_pPacketFactoryManager->createPacket(packetID);
    if (pPacket == NULL)
        throw Error("PacketFactoryManager returned null datagram packet.");

    pPacket->read(*this);
    pPacket->setHost(getHost());
    pPacket->setPort(getPort());

    __END_CATCH
}

void Datagram::write(const char* buf, uint len) {
    __BEGIN_TRY

    if (buf == NULL)
        throw Error("Datagram write buffer is null.");

    if (m_OutputOffset + len > m_Length)
        throw Error("Datagram write exceeds payload length.");

    memcpy(&m_Data[m_OutputOffset], buf, len);
    m_OutputOffset += len;

    __END_CATCH
}

void Datagram::write(const std::string& str) {
    __BEGIN_TRY

    if (m_OutputOffset + str.size() > m_Length)
        throw Error("Datagram string write exceeds payload length.");
    write(str.c_str(), str.size());

    __END_CATCH
}

void Datagram::write(const DatagramPacket* pPacket) {
    __BEGIN_TRY

    if (pPacket == NULL)
        throw Error("Datagram packet write received null packet.");

    PacketID_t packetID = pPacket->getPacketID();
    PacketSize_t packetSize = pPacket->getPacketSize();

    setData(szPacketHeader + packetSize);

    write((char*)&packetID, szPacketID);
    write((char*)&packetSize, szPacketSize);
    pPacket->write(*this);

    __END_CATCH
}

void Datagram::setData(char* data, uint len) {
    __BEGIN_TRY

    if (data == NULL)
        throw Error("Datagram setData received null source buffer.");

    if (m_Data != NULL)
        throw Error("Datagram payload already initialized.");

    m_Length = len;
    m_Data = new char[m_Length];
    memcpy(m_Data, data, m_Length);

    __END_CATCH
}

void Datagram::setData(uint len) {
    __BEGIN_TRY

    if (m_Data != NULL)
        throw Error("Datagram payload already initialized.");

    m_Length = len;
    m_Data = new char[m_Length];

    __END_CATCH
}

void Datagram::setAddress(SOCKADDR_IN* pSockAddr) {
    __BEGIN_TRY

    if (pSockAddr == NULL)
        throw Error("Datagram setAddress received null socket address.");
    memcpy(&m_SockAddr, pSockAddr, szSOCKADDR_IN);

    __END_CATCH
}

std::string Datagram::toString() const {
    StringStream msg;
    msg << "Datagram("
        << "Length:" << m_Length << ",InputOffset:" << m_InputOffset << ",OutputOffset:" << m_OutputOffset << ")";
    return msg.toString();
}
