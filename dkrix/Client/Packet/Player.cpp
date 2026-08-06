//////////////////////////////////////////////////////////////////////
// 
// Filename    : Plyaer.cpp 
// Written By  : reiot@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "Player.h"
#include "Socket.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Assert.h"
#include "Packet.h"
#include "PacketFactoryManager.h"
#include "DebugInfo.h"
#include <cstdio>

void	SendBugReport(const char *bug, ...);

static void TraceNetworkFlow(const char* step)
{
	(void)step;
}

static void TraceNetworkPacket(const char* step, PacketID_t packetID, PacketSize_t packetSize, SequenceSize_t packetSequence)
{
	static int s_packetTraceBudget = 256;
	if (s_packetTraceBudget <= 0)
	{
		return;
	}
	--s_packetTraceBudget;

	char buffer[160];
	sprintf(buffer,
		"Player::processCommand %s id=%u size=%u seq=%u",
		step,
		(unsigned int)packetID,
		(unsigned int)packetSize,
		(unsigned int)packetSequence);
	TraceNetworkFlow(buffer);
}

//////////////////////////////////////////////////////////////////////
//
// constructor
//
//////////////////////////////////////////////////////////////////////
Player::Player ()
	 throw ( Error )
: m_pSocket(NULL), m_pInputStream(NULL), m_pOutputStream(NULL)
{
	__BEGIN_TRY

	Assert( m_pSocket == NULL );

	// create socket input stream
	m_pInputStream = NULL;

	Assert( m_pInputStream == NULL );
	
	// create socket output stream
	m_pOutputStream = NULL;

	Assert( m_pOutputStream == NULL );

	//add by viva
	pHashTable = NULL;
	//end

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// constructor
//
//////////////////////////////////////////////////////////////////////
Player::Player ( Socket * pSocket )
	 throw ( ProtocolException , Error )
: m_pSocket(pSocket), m_pInputStream(NULL), m_pOutputStream(NULL)
{
	__BEGIN_TRY
		
	Assert( m_pSocket != NULL );

	// create socket input stream
	m_pInputStream = new SocketInputStream( m_pSocket, 32768 );

	Assert( m_pInputStream != NULL );
	
	// create socket output stream
	m_pOutputStream = new SocketOutputStream( m_pSocket, 32768 );

	Assert( m_pOutputStream != NULL );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//
//////////////////////////////////////////////////////////////////////
Player::~Player ()
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	// delete socket input stream
	if ( m_pInputStream != NULL ) {
		delete m_pInputStream;
		m_pInputStream = NULL;
	}

	// delete socket output stream
	if ( m_pOutputStream != NULL ) {
		delete m_pOutputStream;
		m_pOutputStream = NULL;
	}

	// delete socket
	if ( m_pSocket != NULL ) {
		m_pSocket->close();
		delete m_pSocket;
		m_pSocket = NULL;
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////
void Player::processInput ()
	throw ( IOException , Error )
{
	__BEGIN_TRY

	try {
		m_pInputStream->fill();
	} catch ( NonBlockingIOException ) {
	} catch ( ConnectException& t ) {
		TraceNetworkFlow(t.toString().c_str());
		throw;
	} catch ( Throwable& t ) {
		TraceNetworkFlow(t.toString().c_str());
		throw;
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////
void Player::processCommand () 
     throw ( IOException , Error )
{
	__BEGIN_TRY

	try {

		
		char header[szPacketHeader];
		PacketID_t packetID;
		PacketSize_t packetSize;
		
		SequenceSize_t packetSequence;

		Packet * pPacket;

		
		while ( true ) {
		
			
			
			
			m_pInputStream->peek( header , szPacketHeader );

			
			
			memcpy( &packetID   , &header[0] , szPacketID );	
			memcpy( &packetSize , &header[szPacketID] , szPacketSize );
			memcpy( &packetSequence , &header[szPacketID+szPacketSize] , szSequenceSize );

			
			if ( packetID >= Packet::PACKET_MAX )
				throw InvalidProtocolException("[Player::processCommand]invalid packet id");
			
			
			if ( packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID) )
			{
				SendBugReport("too large PacketSize ID)%d %d/%d", packetID, packetSize, g_pPacketFactoryManager->getPacketMaxSize( packetID ) );
				throw InvalidProtocolException("too large packet size");
			}
			
			
			
			if ( m_pInputStream->length() < szPacketHeader + packetSize )
				throw InsufficientDataException();
			
			
			
			
			pPacket = g_pPacketFactoryManager->createPacket( packetID );
			TraceNetworkPacket("before read", packetID, packetSize, packetSequence);

			
			
			
			m_pInputStream->read( pPacket );
			TraceNetworkPacket("after read", packetID, packetSize, packetSequence);
/*
	#ifdef __DEBUG_OUTPUT__
			FILE* fp = fopen("packet.log", "a");
			fprintf (fp, "Read: %s\n", pPacket->toString());
			fclose(fp);
	#endif
*/		
			
			
			TraceNetworkPacket("before execute", packetID, packetSize, packetSequence);
			pPacket->execute( this );
			TraceNetworkPacket("after execute", packetID, packetSize, packetSequence);
			
			
			delete pPacket;

		}

	} catch ( NoSuchElementException & nsee ) {

		// PacketFactoryManager::createPacket(PacketID_t)
		// PacketFactoryManager::getPacketMaxSize(PacketID_t)
		
		throw Error( nsee.toString() );

	} catch ( InsufficientDataException ) {

		// do nothing

	} 

	__END_CATCH
}
		    

//////////////////////////////////////////////////////////////////////
//
// flush output buffer to socket's send buffer
//
//////////////////////////////////////////////////////////////////////
void Player::processOutput ()
	throw ( IOException , Error )
{
	__BEGIN_TRY

	m_pOutputStream->flush();

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// send packet to player's output buffer
//
//////////////////////////////////////////////////////////////////////
void Player::sendPacket ( Packet * pPacket )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	#ifdef __DEBUG_OUTPUT__
		DEBUG_ADD_FORMAT("[Send] %s", pPacket->toString().c_str() );							
	#endif

	m_pOutputStream->write( pPacket );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// disconnect ( close socket )
//
//////////////////////////////////////////////////////////////////////
void Player::disconnect ( bool bDisconnected )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	
	
	
	if ( bDisconnected == UNDISCONNECTED ) {

		//
		m_pOutputStream->flush();

	}

	m_pSocket->close();

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// set socket
//////////////////////////////////////////////////////////////////////
void Player::setSocket ( Socket * pSocket )
	throw ()
{
	__BEGIN_TRY

	m_pSocket = pSocket;

	if ( m_pInputStream != NULL ) {
		delete m_pInputStream;
		m_pInputStream = new SocketInputStream( m_pSocket );
	}

	if ( m_pOutputStream != NULL ) {
		delete m_pOutputStream;
		m_pOutputStream = new SocketOutputStream( m_pSocket );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
std::string Player::toString () const
       throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "Player("
		<< "SocketID:" << (int)m_pSocket->getSOCKET() 
		<< ",Host:" << m_pSocket->getHost() 
		<< ",ID:" << m_ID
		<< ")" ;

	return msg.toString();

	__END_CATCH
}
	//add by viva
void Player::setKey(WORD EncryptKey, WORD HashKey) 
	throw()
{
	pHashTable = new BYTE[512];
	BYTE key = (HashKey + 4658)&0x00FF;
	for(int i = 0; i<512; i++)
	{
		key = (key+0xCC)^(key * 0x3)^key;
		pHashTable[i] = key;
	}

	EncryptKey = EncryptKey % 512;
	if( m_pInputStream != NULL)
		m_pInputStream->setKey(EncryptKey, pHashTable);
	if( m_pOutputStream != NULL)
		m_pOutputStream->setKey(EncryptKey, pHashTable);
}
void Player::delKey() 
	throw()
{
	if(pHashTable)
	{
		delete[] pHashTable;
		pHashTable = NULL;
	}
	if( m_pInputStream != NULL)
		m_pInputStream->setKey(0, NULL);
	if( m_pOutputStream != NULL)
		m_pOutputStream->setKey(0, NULL);
}
	//end

void Player::resetStreams()
	throw()
{
	if (m_pInputStream != NULL)
	{
		m_pInputStream->resetState();
	}

	if (m_pOutputStream != NULL)
	{
		m_pOutputStream->resetState();
	}
}
