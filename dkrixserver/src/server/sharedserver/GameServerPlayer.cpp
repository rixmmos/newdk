//////////////////////////////////////////////////////////////////////
//
// Filename    : GameServerPlayer.cpp
// Written By  : Reiot
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GameServerPlayer.h"

#include "Assert.h"
#include "Packet.h"
#include "PacketFactoryManager.h"
#include "Socket.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

// by sigi. 2002.11.12
const int defaultGameServerPlayerInputStreamSize = 10240;
const int defaultGameServerPlayerOutputStreamSize = 163840;


//////////////////////////////////////////////////////////////////////
//
// constructor
//
//////////////////////////////////////////////////////////////////////
GameServerPlayer::GameServerPlayer() noexcept(false)
//: m_pSocket(NULL), m_pInputStream(NULL), m_pOutputStream(NULL)
{}


GameServerPlayer::GameServerPlayer(Socket* pSocket)
//: Player( pSocket )//m_pSocket(pSocket), m_pInputStream(NULL), m_pOutputStream(NULL)
{
    __BEGIN_TRY

    Assert(pSocket != NULL);
    m_pSocket = pSocket;

    // create socket input stream
    m_pInputStream = new SocketInputStream(m_pSocket, defaultGameServerPlayerInputStreamSize);

    Assert(m_pInputStream != NULL);

    // create socket output stream
    m_pOutputStream = new SocketOutputStream(m_pSocket, defaultGameServerPlayerOutputStreamSize);

    Assert(m_pOutputStream != NULL);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//
//////////////////////////////////////////////////////////////////////
GameServerPlayer::~GameServerPlayer() noexcept {
    // destructor should never throw; guard any future cleanup
    try {
        /*
    // delete socket input stream
    SAFE_DELETE(m_pInputStream);

    // delete socket output stream
    SAFE_DELETE(m_pOutputStream);

    // delete socket
    if ( m_pSocket != NULL )
    {
        m_pSocket->close();
        delete m_pSocket;
        m_pSocket = NULL;
    }
    */
    } catch (...) {
        // swallow all exceptions to honor noexcept
    }
}


void GameServerPlayer::processInput() noexcept(false) {
    __BEGIN_TRY

    try {
        m_pInputStream->fill();
    } catch (NonBlockingIOException& nbie) {
    }

    __END_CATCH
}


void GameServerPlayer::processOutput() noexcept(false) {
    __BEGIN_TRY

    try {
        m_pOutputStream->flush();
    } catch (InvalidProtocolException&) {
        throw DisconnectException(" ");
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////
void GameServerPlayer::processCommand() noexcept(false) {
    __BEGIN_TRY

    try {
        
        char header[szPacketHeader];
        PacketID_t packetID;
        PacketSize_t packetSize;
        Packet* pPacket;

        
        while (true) {
            
            
            
            if (!m_pInputStream->peek(header, szPacketHeader))
                break;

            
            
            memcpy(&packetID, &header[0], szPacketID);
            memcpy(&packetSize, &header[szPacketID], szPacketSize);

            
            if (packetID >= Packet::PACKET_MAX)
                throw InvalidProtocolException("invalid packet id");

            
            if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID))
                throw InvalidProtocolException("too large packet size");

            
            
            if (m_pInputStream->length() < szPacketHeader + packetSize)
                throw InsufficientDataException();

            
            
            
            pPacket = g_pPacketFactoryManager->createPacket(packetID);

            
            
            
            m_pInputStream->readPacket(pPacket);

            
            
            pPacket->execute(this);

            
            delete pPacket;
        }

    } catch (NoSuchElementException& nsee) {
        // PacketFactoryManager::createPacket(PacketID_t)
        // PacketFactoryManager::getPacketMaxSize(PacketID_t)
        
        throw Error(nsee.toString());

    } catch (const InsufficientDataException&) {
        // do nothing
    }
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// send packet to player's output buffer
//
//////////////////////////////////////////////////////////////////////
void GameServerPlayer::sendPacket(Packet* pPacket) noexcept(false) {
    __BEGIN_TRY

    m_pOutputStream->writePacket(pPacket);

    /*
    cout << endl;
    cout << "=== GameServerPlayer::sendPacket() ===" << endl;
    cout << pPacket->toString() << endl;
    cout << "============================" << endl;
    */

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// disconnect ( close socket )
//
//////////////////////////////////////////////////////////////////////
void GameServerPlayer::disconnect(bool bDisconnected) noexcept(false) {
    __BEGIN_TRY

    try {
        
        
        
        if (bDisconnected == UNDISCONNECTED) {
            m_pOutputStream->flush();
        }

        m_pSocket->close();
    } catch (InvalidProtocolException& t) {
        cerr << "GameServerPlayer::disconnect Exception Check!!" << endl;
        cerr << t.toString() << endl;
        m_pSocket->close();
        
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// set socket
//////////////////////////////////////////////////////////////////////
void GameServerPlayer::setSocket(Socket* pSocket) noexcept {
    m_pSocket = pSocket;

    if (m_pInputStream != NULL) {
        delete m_pInputStream;
        m_pInputStream = new SocketInputStream(m_pSocket);
    }

    if (m_pOutputStream != NULL) {
        delete m_pOutputStream;
        m_pOutputStream = new SocketOutputStream(m_pSocket);
    }
}


//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string GameServerPlayer::toString() const noexcept(false) {
    __BEGIN_TRY

    StringStream msg;

    msg << "GameServerPlayer("
        << "SocketID:" << m_pSocket->getSOCKET() << ",Host:" << m_pSocket->getHost() << ",ID:" << m_ID << ")";

    return msg.toString();

    __END_CATCH
}
