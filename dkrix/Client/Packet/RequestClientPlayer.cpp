//--------------------------------------------------------------------------------
// 
// Filename    : RequestClientPlayer.cpp 
// Written By  : sigi
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "RequestClientPlayer.h"
#include "Assert.h"
#include "Packet.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "ClientDef.h"
#include "RequestFileManager.h"
#include "ClientConfig.h"
#include "DebugInfo.h"

#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
	extern CMessageArray*		g_pGameMessage;
#endif

#define	EXPIRE_DELAY	60000		


	void	SendBugReport(const char *bug, ...);

//--------------------------------------------------------------------------------
//
// constructor
//
//--------------------------------------------------------------------------------
RequestClientPlayer::RequestClientPlayer ( Socket * pSocket )
	 throw ( ProtocolException , Error )
: Player(pSocket), m_PlayerStatus(CPS_NONE)
{
	m_RequestMode = REQUEST_CLIENT_MODE_NULL;

	m_ExpireTime = g_CurrentTime + EXPIRE_DELAY;
}

//--------------------------------------------------------------------------------
// get InputStream Length
//--------------------------------------------------------------------------------
uint 
RequestClientPlayer::getInputStreamLength () const
	throw()
{
	return m_pInputStream->length();
}

//--------------------------------------------------------------------------------
// read InputStream
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
uint 
RequestClientPlayer::readInputStream ( char * buf , uint len )
	throw ( ProtocolException , Error )
{
	uint nRead = m_pInputStream->read( buf, len );

	return nRead;
}

//--------------------------------------------------------------------------------
//
// destructor
//
//--------------------------------------------------------------------------------
RequestClientPlayer::~RequestClientPlayer ()
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
		if (g_pGameMessage!=NULL)
			g_pGameMessage->AddFormat("Disonnected From %s", m_RequestServerName.c_str());
	#endif

	
	
	Assert( m_PlayerStatus == CPS_END_SESSION );

	__END_CATCH
}

//--------------------------------------------------------------------------------
//
// parse packet and execute handler for the packet
//
//--------------------------------------------------------------------------------
void RequestClientPlayer::processCommand () 
     throw ( IOException , Error )
{
	__BEGIN_TRY

	//-----------------------------------------------------------------------
	
	//-----------------------------------------------------------------------
	switch (m_RequestMode)
	{
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		case REQUEST_CLIENT_MODE_PROFILE :
			if (g_pRequestFileManager->ReceiveMyRequest(m_RequestServerName, this))
			{
				
				m_ExpireTime = g_CurrentTime + EXPIRE_DELAY;
				return;
			}			
		break;
	}	

	Packet * pPacket;

	try {
		try {

			
			char header[szPacketHeader];
			PacketID_t packetID;
			PacketSize_t packetSize;
			pPacket = NULL;

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			
			
			
			//---------------------------------------------------------
			int maxProcessPacket = g_pClientConfig->MAX_PROCESS_PACKET;
			int processedPacket = 0;

			
			while ( true ) {
			
				
				
				
				m_pInputStream->peek( header , szPacketHeader );

				
				
				memcpy( &packetID   , &header[0] , szPacketID );	
				memcpy( &packetSize , &header[szPacketID] , szPacketSize );

				//#ifdef	__DEBUG_OUTPUT__
					//	DEBUG_ADD_FORMAT("ID=%d (%s), size=%d", packetID, g_pPacketFactoryManager->getPacketName( packetID ), packetSize);
				//#else
					//	DEBUG_ADD_FORMAT("ID=%d, size=%d", packetID, packetSize);
				//#endif
				
				
				if ( packetID >= Packet::PACKET_MAX )
				{
					DEBUG_ADD_FORMAT("[PacketError-RequestClientPlayer::processCommand] exceed MAX=%d. packetID=%d", Packet::PACKET_MAX, packetID);		
					SendBugReport("RCP,Exceed PacketID:%d",packetID);
					
					throw InvalidProtocolException("[PacketError-RequestClientPlayer::processCommand] exceed MAX packetID");
				}

				#ifdef __DEBUG_OUTPUT__
					DEBUG_ADD_FORMAT("[RECEIVE] [ID=%d] %s", packetID, g_pPacketFactoryManager->getPacketName(packetID).c_str());
				#endif
				
				BOOL bExecute = TRUE;

				if ( ! g_pPacketValidator->isValidPacketID( getPlayerStatus() , packetID ) )
				{
					DEBUG_ADD("[PacketError] invalid packet ORDER");

					//---------------------------------------------------------------
					
					//---------------------------------------------------------------
					if (getPlayerStatus()==CPS_WAITING_FOR_GC_RECONNECT_LOGIN)
					{
						
						bExecute = FALSE;
						
						
						maxProcessPacket = 0xFFFF;
					}
					//---------------------------------------------------------------
					
					//---------------------------------------------------------------
					else
					{
						throw InvalidProtocolException("invalid packet ORDER");
					}				
				}

				
				if ( packetSize > g_pPacketFactoryManager->getPacketMaxSize( packetID ) )
				{
					DEBUG_ADD_FORMAT("[PacketError] too large packet SIZE: %d/%d", (int)packetSize, (int)g_pPacketFactoryManager->getPacketMaxSize( packetID ));		
					throw InvalidProtocolException("too large packet SIZE");
				}
				
				
				
				if ( m_pInputStream->length() < szPacketHeader + packetSize )
				{
					DEBUG_ADD_FORMAT("[PacketError] InsufficientDataException: %d/%d", m_pInputStream->length(), szPacketHeader + packetSize);
		
					throw InsufficientDataException();
				}

				if (g_Mode!=MODE_GAME)	
					throw InvalidProtocolException("not MODE_GAME");

				
				
				
				
				pPacket = g_pPacketFactoryManager->createPacket( packetID );

				
				
				
				m_pInputStream->read( pPacket );
				
				
				

				if (bExecute)
				{
					#ifdef __DEBUG_OUTPUT__
						DEBUG_ADD(pPacket->toString().c_str());
					#endif

					pPacket->execute( this );

					//DEBUG_ADD_FORMAT("[Executed] %s", pPacket->toString().c_str());
					DEBUG_ADD("[PacketExecute OK]");				
				}				
				
				delete pPacket;
				pPacket = NULL;


				m_ExpireTime = g_CurrentTime + EXPIRE_DELAY;

				//---------------------------------------------------------	
				
				
				//---------------------------------------------------------
				if (++processedPacket > maxProcessPacket)
				{
					DEBUG_ADD_FORMAT("[PacketSkip] So many Packets. at RequestClientPlayer");

					break;
				}
				
				//---------------------------------------------------------
				
				
				//---------------------------------------------------------
				if (packetID==Packet::PACKET_RC_REQUESTED_FILE)
				{
					break;
				}
			}

		} catch ( InsufficientDataException ) {

			
			if (g_CurrentTime > m_ExpireTime)
			{
				throw InvalidProtocolException("timeout - -;;");			
			}
		}
	} catch (Throwable)	{
		if (pPacket!=NULL)
		{
			delete pPacket;
			pPacket = NULL;
		}

		throw;
	}

	__END_CATCH
}
		    

//--------------------------------------------------------------------------------
// disconnect player
//--------------------------------------------------------------------------------
void RequestClientPlayer::disconnect ( bool bDisconnected )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	
	if (g_pRequestFileManager!=NULL
		&& g_pRequestFileManager->HasMyRequest(m_RequestServerName.c_str()))
	{
		g_pRequestFileManager->RemoveMyRequest(m_RequestServerName.c_str());
	}

	if ( bDisconnected == UNDISCONNECTED ) {

		
		//GCDisconnect gcDisconnect;
		//sendPacket( gcDisconnect );

		
		m_pOutputStream->flush();		
	}

	
	m_pSocket->close();

	
	//cout<< "PlayerStatus : " << (int)m_PlayerStatus << endl;
	
	//Assert( m_PlayerStatus != CPS_END_SESSION );
	m_PlayerStatus = CPS_END_SESSION;

	__END_CATCH
}


//--------------------------------------------------------------------------------
//
// get debug string
//
//--------------------------------------------------------------------------------
std::string RequestClientPlayer::toString () const
       throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "RequestClientPlayer("
		<< "SocketID:" << (int)m_pSocket->getSOCKET() 
		<< ",Host:" << m_pSocket->getHost() 
		<< ")" ;

	return msg.toString();

	__END_CATCH
}
