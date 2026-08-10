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
#include <time.h>

#include <exception>

#include "Assert.h"
#include "DatagramPacket.h"
#include "Packet.h"
#include "PacketFactoryManager.h"

namespace {

// Rate-limit the "packet is not UDP" drop log.
//
// The check below is reached by any datagram whose header parses but whose
// packet ID is not in Datagram::isDatagram(). That is unauthenticated,
// connectionless and source-spoofable: UDP 9997 is port-forwarded, so a ~10
// byte datagram carrying, say, a CG_* packet ID cost one full
// open()/write()/close() on datagram.txt per packet, plus the exception the
// throw below raises, which the receive loops log again. Roughly 30x
// amplification by volume and, more importantly, two file opens per attacker
// packet and a log file that grows without bound until the disk fills.
//
// The information is kept, not dropped: up to kMaxLinesPerWindow lines are
// still written each window, and when the window closes with drops suppressed
// a single summary line records how many were hidden. That bounds the path to
// ~17 file writes per minute regardless of inbound rate.
//
// Datagram::read() is called only from LoginServerManager::run() (gameserver)
// and GameServerManager::run() (loginserver), one dedicated thread per
// process, so these statics need no locking -- the same invariant
// LoginServerManager's logRejectedDatagram() relies on.
void logNonDatagramPacket(PacketID_t packetID, const string& host) {
    const int kMaxLinesPerWindow = 16;
    const time_t kWindowSeconds = 60;

    static time_t windowStart = 0;
    static int loggedThisWindow = 0;
    static unsigned long suppressedThisWindow = 0;

    const time_t now = time(NULL);

    if (now - windowStart >= kWindowSeconds) {
        if (suppressedThisWindow > 0)
            filelog("datagram.txt", "non-UDP packet ids: %lu further drops suppressed in the last %ld seconds",
                    suppressedThisWindow, (long)kWindowSeconds);

        windowStart = now;
        loggedThisWindow = 0;
        suppressedThisWindow = 0;
    }

    if (loggedThisWindow >= kMaxLinesPerWindow) {
        suppressedThisWindow++;
        return;
    }

    loggedThisWindow++;
    filelog("datagram.txt", "id:%u host:%s", (uint)packetID, host.c_str());
}

} // namespace

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

    // Real bounds check, not just the Assert: readPacket() validates only the
    // 5-byte header against m_Length, then hands the datagram to
    // DatagramPacket::read(), where GG/GL/LG packets read a wire-supplied BYTE
    // length and pass it straight back in here (GGGuildChat, GGServerChat,
    // GGCommand, GLIncomingConnection, ...). Under NDEBUG the Assert is gone
    // and this memcpy reads off the end of m_Data. Written subtractively so
    // m_InputOffset + len cannot itself wrap. InsufficientDataException matches
    // SocketInputStream::read on the TCP side and derives from
    // ProtocolException, which the datagram receive loops already catch and
    // treat as "drop this datagram".
    if (m_InputOffset > m_Length || len > m_Length - m_InputOffset)
        throw InsufficientDataException("Datagram::read: read past end of datagram");

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

    // See Datagram::read(char*, uint) -- same guard, and this is the overload
    // the wire-supplied string lengths actually reach.
    if (m_InputOffset > m_Length || len > m_Length - m_InputOffset)
        throw InsufficientDataException("Datagram::read: read past end of datagram");

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
        logNonDatagramPacket(packetID, getHost());
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

    // Real bounds check on the send side too -- overflowing m_Data here is a
    // heap *write*. This replaces a commented-out `if` with the same condition
    // that the original author left unfinished.
    if (m_OutputOffset > m_Length || len > m_Length - m_OutputOffset)
        throw OutOfBoundException("Datagram::write: write past end of datagram");

    // boundary check
    Assert(m_OutputOffset + len <= m_Length);

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
