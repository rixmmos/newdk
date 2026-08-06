//////////////////////////////////////////////////////////////////////
//
// Filename    : GameServerPlayer.cpp
// Written By  : Reiot
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GameServerPlayer.h"

#include "../Mofus.h"
#include "../PKTConnectAccept.h"
#include "../PKTConnectAsk.h"
#include "../PKTError.h"
#include "../PKTLogout.h"
#include "../PKTPowerPoint.h"
#include "../PKTReceiveOK.h"
#include "../PKTResult.h"
#include "../PKTUserInfo.h"
#include "Assert.h"
#include "MPacketManager.h"
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
GameServerPlayer::GameServerPlayer() throw(Error) {}


GameServerPlayer::GameServerPlayer(Socket* pSocket) throw(Error) {
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
GameServerPlayer::~GameServerPlayer() throw(Error) {
    __BEGIN_TRY
    __END_CATCH
}


void GameServerPlayer::processInput() throw(IOException, Error) {
    __BEGIN_TRY

    try {
        m_pInputStream->fill();
    } catch (NonBlockingIOException& nbie) {
    }

    __END_CATCH
}


void GameServerPlayer::processOutput() throw(IOException, Error) {
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
void GameServerPlayer::processCommand() throw(IOException, Error) {
    __BEGIN_TRY

    try {
        
        while (true) {
            
            char header[szMPacketHeader];
            MPacketSize_t packetSize;
            MPacketID_t packetID;

            if (!m_pInputStream->peek(&header[0], szMPacketHeader))
                return;

            memcpy(&packetSize, &header[0], szMPacketSize);
            memcpy(&packetID, &header[szMPacketSize], szMPacketID);

            // packetSize = ntohl( packetSize );
            // packetID = ntohl( packetID );

            
            if (!g_pMPacketManager->hasHandler(packetID)) {
                filelog(MOFUS_ERROR_FILE, "Invalid PacketID : %d", packetID);
                throw ProtocolException("Invalid PacketID");
            }

            
            if (g_pMPacketManager->getPacketSize(packetID) != packetSize) {
                filelog(MOFUS_ERROR_FILE, "Invalid PacketSize : %d, expected size : %d", packetSize,
                        g_pMPacketManager->getPacketSize(packetID));
                throw ProtocolException("Invalid PacketSize");
            }

            
            if (m_pInputStream->length() < (unsigned int)(packetSize + szMPacketSize))
                return;

            
            MPacket* pPacket = g_pMPacketManager->createPacket(packetID);

            
            pPacket->read(*m_pInputStream);

            
            g_pMPacketManager->execute(this, pPacket);
        }
    } catch (InsufficientDataException) {
        
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// send packet to player's output buffer
//
//////////////////////////////////////////////////////////////////////
void GameServerPlayer::sendPacket(MPacket* pPacket) throw(ProtocolException, Error) {
    __BEGIN_TRY

    Assert(pPacket != NULL);

    pPacket->write(*m_pOutputStream);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// disconnect ( close socket )
//
//////////////////////////////////////////////////////////////////////
void GameServerPlayer::disconnect(bool bDisconnected) throw(InvalidProtocolException, Error) {
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
void GameServerPlayer::setSocket(Socket* pSocket) throw() {
    __BEGIN_TRY

    m_pSocket = pSocket;

    if (m_pInputStream != NULL) {
        delete m_pInputStream;
        m_pInputStream = new SocketInputStream(m_pSocket);
    }

    if (m_pOutputStream != NULL) {
        delete m_pOutputStream;
        m_pOutputStream = new SocketOutputStream(m_pSocket);
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string GameServerPlayer::toString() const throw(Error) {
    __BEGIN_TRY

    StringStream msg;

    msg << "GameServerPlayer("
        << "SocketID:" << m_pSocket->getSOCKET() << ",Host:" << m_pSocket->getHost() << ",ID:" << m_ID << ")";

    return msg.toString();

    __END_CATCH
}
