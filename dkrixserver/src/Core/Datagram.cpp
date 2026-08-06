//////////////////////////////////////////////////////////////////////
//
// Filename    : Datagram.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Datagram.h"

#include <stdio.h>

#include <exception>

#include "Assert.h"
#include "DatagramPacket.h"
#include "Packet.h"
#include "PacketFactoryManager.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
Datagram::Datagram() : m_Length(0), m_InputOffset(0), m_OutputOffset(0), m_Data(NULL) {
    __BEGIN_TRY

    memset(&m_SockAddr, 0, sizeof(m_SockAddr));
    m_SockAddr.sin_family = AF_INET;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
Datagram::~Datagram() noexcept {
    try {
        if (m_Data != NULL) {
            SAFE_DELETE_ARRAY(m_Data);
            m_Data = NULL;
        }
    } catch (const std::exception&) {
        // ignore during teardown
    }
}


bool Datagram::isDatagram(PacketID_t packetID) {
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
    case Packet::PACKET_LG_KICK_CHARACTER:
    case Packet::PACKET_RC_SAY:
    case Packet::PACKET_GTO_ACKNOWLEDGEMENT:
    case Packet::PACKET_LG_INCOMING_CONNECTION:
    case Packet::PACKET_LG_INCOMING_CONNECTION_ERROR:
    case Packet::PACKET_LG_INCOMING_CONNECTION_OK:
        return true;
    default:
        return false;
    }
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void Datagram::read(char* buf, uint len) {
    __BEGIN_TRY

    // boundary check
    Assert(m_InputOffset + len <= m_Length);

    memcpy(buf, &m_Data[m_InputOffset], len);

    m_InputOffset += len;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void Datagram::read(string& str, uint len) {
    __BEGIN_TRY

    // boundary check
    Assert(m_InputOffset + len <= m_Length);

    str.reserve(len);
    str.assign(&m_Data[m_InputOffset], len);

    m_InputOffset += len;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//



//


//
// *CAUTION*
//



//
//////////////////////////////////////////////////////////////////////
void Datagram::read(DatagramPacket*& pPacket) {
    __BEGIN_TRY

    Assert(pPacket == NULL);

    PacketID_t packetID;
    PacketSize_t packetSize;

    // initialize packet header
    read((char*)&packetID, szPacketID);
    read((char*)&packetSize, szPacketSize);

    // cout << "DatagramPacket I  D : " << packetID << endl;

    
    if (packetID >= Packet::PACKET_MAX)
        throw InvalidProtocolException("invalid packet id");

    
    if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID))
        throw InvalidProtocolException("too large packet size");

    
    if (m_Length < szPacketHeader + packetSize)
        throw Error(
            "    .");

    
    if (m_Length > szPacketHeader + packetSize)
        throw Error("     "
                    ".");

    
    if (!isDatagram(packetID)) {
        filelog("datagram.txt", "id:%u host:%s", packetID, getHost().c_str());
        throw InvalidProtocolException("packet is not UDP");
    }

    pPacket = (DatagramPacket*)g_pPacketFactoryManager->createPacket(packetID);

    Assert(pPacket != NULL);

    
    // filelog("datagram.txt","id:%u host:%s",packetID,getHost().c_str());
    pPacket->read(*this);

    
    pPacket->setHost(getHost());
    pPacket->setPort(getPort());

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void Datagram::write(const char* buf, uint len) {
    __BEGIN_TRY

    // boundary check
    Assert(m_OutputOffset + len <= m_Length);
    //	if (m_OutputOffset + len > m_Length)
    //	{
    
    //	}

    memcpy(&m_Data[m_OutputOffset], buf, len);

    m_OutputOffset += len;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//
// *CAUTION*
//


//
//////////////////////////////////////////////////////////////////////
void Datagram::write(const string& str) {
    __BEGIN_TRY

    // boundary check
    Assert(m_OutputOffset + str.size() <= m_Length);

    // write string body
    write(str.c_str(), str.size());

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// write packet
//





//
//////////////////////////////////////////////////////////////////////
void Datagram::write(const DatagramPacket* pPacket) {
    __BEGIN_TRY

    Assert(pPacket != NULL);

    PacketID_t packetID = pPacket->getPacketID();
    PacketSize_t packetSize = pPacket->getPacketSize();

    
    setData(szPacketHeader + packetSize);

    
    write((char*)&packetID, szPacketID);
    write((char*)&packetSize, szPacketSize);

    
    pPacket->write(*this);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// set data
//


//
//////////////////////////////////////////////////////////////////////
void Datagram::setData(char* data, uint len) {
    __BEGIN_TRY

    Assert(data != NULL && m_Data == NULL);

    m_Length = len;
    m_Data = new char[m_Length];
    memcpy(m_Data, data, m_Length);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void Datagram::setData(uint len) {
    __BEGIN_TRY

    Assert(m_Data == NULL);

    m_Length = len;
    m_Data = new char[m_Length];

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// set address
//////////////////////////////////////////////////////////////////////
void Datagram::setAddress(SOCKADDR_IN* pSockAddr) {
    __BEGIN_TRY

    Assert(pSockAddr != NULL);

    memcpy(&m_SockAddr, pSockAddr, szSOCKADDR_IN);

    // char str[80];
    // sprintf(str, "0x%X - 0x%X", m_SockAddr.sin_port, ntohs(m_SockAddr.sin_port));
    // cout << "[Datagram::setAddress] " << inet_ntoa(m_SockAddr.sin_addr) << ":" << ntohs(m_SockAddr.sin_port) << " - "
    // << str << endl;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////
string Datagram::toString() const {
    StringStream msg;
    msg << "Datagram("
        << "Length:" << m_Length << ",InputOffset:" << m_InputOffset << ",OutputOffset:" << m_OutputOffset << ")";
    return msg.toString();
}
