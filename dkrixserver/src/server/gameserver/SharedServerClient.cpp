//////////////////////////////////////////////////////////////////////////////
// Filename    : GamePlyaer.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SharedServerClient.h"

#include <fstream>

#include "Assert.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
// #define __PROFILE_PACKETS__


//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

SharedServerClient::SharedServerClient(Socket* pSocket)

    : Player(pSocket) {
    __BEGIN_TRY

    m_Mutex.setName("SharedServerClient");

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////

SharedServerClient::~SharedServerClient()

{
    __BEGIN_TRY

    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////
void SharedServerClient::processCommand() {
    __BEGIN_TRY

    
    char header[szPacketHeader];
    PacketID_t packetID;
    PacketSize_t packetSize;
    Packet* pPacket = NULL;

    try {
        
        while (true) {
            
            
            
            
            if (!m_pInputStream->peek(&header[0], szPacketHeader)) {
                break;
            }

            
            
            memcpy(&packetID, &header[0], szPacketID);
            memcpy(&packetSize, &header[szPacketID], szPacketSize);

            
            if (packetID >= (int)Packet::PACKET_MAX) {
                filelog("SharedServerClient.txt", "Packet ID exceed MAX, RECV [%d/%d]");

                throw InvalidProtocolException("too large packet id");
            }

            try {
                
                if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID)) {
                    filelog("SharedServerClient.txt", "Too Larget Packet Size, RECV [%d],PacketSize[%d]");

                    throw InvalidProtocolException("too large packet size");
                }

                
                if (m_pInputStream->length() < szPacketHeader + packetSize)
                    break;

                
                
                
                pPacket = g_pPacketFactoryManager->createPacket(packetID);

                
                
                
                m_pInputStream->readPacket(pPacket);



#ifdef __PROFILE_PACKETS__

                beginProfileEx(pPacket->getPacketName().c_str());
                pPacket->execute(this);
                endProfileEx(pPacket->getPacketName().c_str());

#else
                pPacket->execute(this);
#endif
            } catch (IgnorePacketException& igpe) {
                
                if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID))
                    throw InvalidProtocolException("too large packet size");

                
                if (m_pInputStream->length() < szPacketHeader + packetSize)
                    break;

                
                
                m_pInputStream->skip(szPacketHeader + packetSize);

                
                
                
            }
        }
    } catch (InsufficientDataException& ide) {
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// flush output buffer to socket's send buffer
//


//
//////////////////////////////////////////////////////////////////////
void SharedServerClient::processOutput() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    try {
        Player::processOutput();
    } catch (InvalidProtocolException& It) {
        throw DisconnectException("Pipe    ");
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// send packet to player's output buffer
//
//////////////////////////////////////////////////////////////////////
void SharedServerClient::sendPacket(Packet* pPacket) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    try {
        Player::sendPacket(pPacket);
    } catch (InvalidProtocolException& It) {
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string SharedServerClient::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    //////////////////////////////////////////////////
    // enter critical section
    //////////////////////////////////////////////////
    __ENTER_CRITICAL_SECTION(m_Mutex)

    msg << "SharedServerClient(SocketID:" << m_pSocket->getSOCKET() << ",Host:" << m_pSocket->getHost() << ")";

    //////////////////////////////////////////////////
    // leave critical section
    //////////////////////////////////////////////////
    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return msg.toString();

    __END_CATCH
}
