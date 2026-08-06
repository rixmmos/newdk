//--------------------------------------------------------------------------------
// 
// Filename    : ClientPlyaer.cpp 
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "ClientPlayer.h"
#include "Assert.h"
#include "Packet.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "ClientConfig.h"
#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"
#include "DebugInfo.h"
//#include "MPlayer.h"
#include "MZone.h"
#include "UserInformation.h"
//#include "minTR.H" 
#include <fstream>

#include "DebugKit.h"
extern CMessageStringTable g_MessageStringTable;
extern BOOL g_bMsgOutPutFlag;
extern BOOL g_bMsgDetailFlag;
extern BOOL g_bMsgContentFlag;
extern void WriteCombatCrashMarker(const char* format, ...);

static void TraceGameEntryFlow(const char* step)
{
	(void)step;
}

extern void PrintMessageDetail(ofstream file, char *strMsg, int length);

void	SendBugReport(const char *bug, ...);
//--------------------------------------------------------------------------------
//
// constructor
//
//--------------------------------------------------------------------------------
ClientPlayer::ClientPlayer ( Socket * pSocket )
	 throw ( ProtocolException , Error )
: m_PlayerStatus(CPS_NONE)
{
	__BEGIN_TRY
		
	Assert( pSocket != NULL );
	m_pSocket = pSocket;

#ifdef __USE_ENCRYPTER__
	// create socket input stream
	m_pInputStream = new SocketEncryptInputStream( m_pSocket, 32768 );

	Assert( m_pInputStream != NULL );
	
	// create socket output stream
	m_pOutputStream = new SocketEncryptOutputStream( m_pSocket, 32768 );

	Assert( m_pOutputStream != NULL );
#else
	// create socket input stream
	m_pInputStream = new SocketInputStream( m_pSocket, 32768 );

	Assert( m_pInputStream != NULL );
	
	// create socket output stream
	m_pOutputStream = new SocketOutputStream( m_pSocket, 32768 );

	Assert( m_pOutputStream != NULL );
#endif
	__END_CATCH
}


//--------------------------------------------------------------------------------
//
// destructor
//
//--------------------------------------------------------------------------------
ClientPlayer::~ClientPlayer ()
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	
	Assert( m_PlayerStatus == CPS_END_SESSION );

	// delete all previous packets
	while ( ! m_PacketHistory.empty() ) {
		Packet * pPacket = m_PacketHistory.front();
		delete pPacket;
		m_PacketHistory.pop_front();
	}
	
	__END_CATCH
}

void ClientPlayer::setPlayerStatus ( PlayerStatus playerStatus ) throw () { 
	m_PlayerStatus = playerStatus; 
	DEBUG_ADD_FORMAT_ERR("[ClientPlayer::setPlayerStatus] %d", playerStatus);
}


//--------------------------------------------------------------------------------
//
// parse packet and execute handler for the packet
//
//--------------------------------------------------------------------------------
void ClientPlayer::processCommand () 
     throw ( IOException , Error )
{
	__BEGIN_TRY

	Packet * pPacket;

	try {
		try {

			
			char header[szPacketHeader];
			PacketID_t packetID = 0;
			PacketSize_t packetSize = 0;		
			pPacket = NULL;

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			
			
			
			//---------------------------------------------------------
			int maxProcessPacket = g_pClientConfig->MAX_PROCESS_PACKET;
			int processedPacket = 0;
 
			
			while ( true ) {
			
				
				
				
				if (m_pInputStream->peek( header , szPacketHeader ) == false) {
					break;
				}

				
				

				memcpy( &packetID   , &header[0] , szPacketID );
				memcpy( &packetSize , &header[szPacketID] , szPacketSize );
				BYTE seq = header[szPacketID+szPacketSize];
				if (getPlayerStatus() == CPS_AFTER_SENDING_CG_CONNECT ||
					getPlayerStatus() == CPS_WAITING_FOR_LOADING ||
					getPlayerStatus() == CPS_WAITING_FOR_GC_SET_POSITION)
				{
					char trace[160];
					sprintf(trace, "ClientPlayer recv packet id=%u size=%u seq=%u status=%d",
						packetID, packetSize, seq, (int)getPlayerStatus());
					TraceGameEntryFlow(trace);
				}

#ifdef __DEBUG_OUTPUT__
				ofstream file("packetID.log", ios::out | ios::app);
				file << "*** RECEIVED PacketID=" << packetID << ", PacketSize=" << packetSize << ", Seq=" << (int)seq << endl;
				if (packetID == 0) {
					file << " StreamLength " << m_pInputStream->length() << endl;
				}
				file.close();
#endif

/*
#ifdef DEBUG_INFO
				if(g_bMsgOutPutFlag)
				{
					SYSTEMTIME curTime;
					GetLocalTime(&curTime);
					char strTime[64];
					memset(strTime,0,sizeof(strTime));
					sprintf(strTime,"(%02d-%02d %02d:%02d:%02d)",
						(BYTE)curTime.wMonth,(BYTE)curTime.wDay,
						(BYTE)curTime.wHour,(BYTE)curTime.wMinute,(BYTE)curTime.wSecond);
					ofstream file("MessageLog.log", ios::out | ios::app);
					file << strTime << "<-- RCV[ " << g_MessageStringTable.GetMessageName(packetID) << " ]" << ", \tPacketID=" << packetID << ", PacketSize=" << packetSize << endl;
					if(g_bMsgContentFlag)
					{
						char strMsg[1024];
						int nMsgLength = szPacketHeader + packetSize;
						if(nMsgLength>64)
							nMsgLength = 64;
						m_pInputStream->peek(strMsg,nMsgLength);
						PrintMessageDetail(file,strMsg,nMsgLength);
					}
					file.close();
				}
#endif
*/
				//#ifdef	__DEBUG_OUTPUT__
					//	DEBUG_ADD_FORMAT("ID=%d (%s), size=%d", packetID, g_pPacketFactoryManager->getPacketName( packetID ), packetSize);
				//#else
					//	DEBUG_ADD_FORMAT("ID=%d, size=%d", packetID, packetSize);
				//#endif

				


				if ( packetID >= Packet::PACKET_MAX ) 
				{					
					DEBUG_ADD_FORMAT_ERR("[PacketError-ClientPlayer::processCommand] exceed MAX=%d. packetID=%u", Packet::PACKET_MAX, packetID);
					SendBugReport("Exceed PacketID:%d",packetID);
					
					throw InvalidProtocolException("[PacketError-ClientPlayer::processCommand] exceed MAX packetID");
				}

/*
				#ifdef __DEBUG_OUTPUT__
				if (packetID != 0) {
					DEBUG_ADD_FORMAT("[RECEIVE] [ID=%u] %s", packetID, g_pPacketFactoryManager->getPacketName(packetID).c_str());
				} else {
					DEBUG_ADD_FORMAT("[RECEIVE] wrong packet ID = 0 here?");
				}
				#endif
*/				
				BOOL bExecute = TRUE;

				try {
					if ( ! g_pPacketValidator->isValidPacketID( getPlayerStatus() , packetID ) )
					{
						//---------------------------------------------------------------
						
						//---------------------------------------------------------------
						if (getPlayerStatus()==CPS_WAITING_FOR_GC_RECONNECT_LOGIN)
						{
							DEBUG_ADD_ERR("[PacketError] ignore Packet when RECONNECT");

							
							bExecute = FALSE;
							
							
							maxProcessPacket = 0xFFFF;
						}
						//---------------------------------------------------------------
						
						//---------------------------------------------------------------
						else
						{
							DEBUG_ADD_FORMAT("[PacketError] invalid packet ORDER %d", getPlayerStatus());
							TraceGameEntryFlow("ClientPlayer invalid packet order after CGConnect");
							throw InvalidProtocolException("invalid packet ORDER");
						}				
					}
					
					
						
				//} catch (InvalidProtocolException& t) {
				} catch (IgnorePacketException& t)	{
					
					DEBUG_ADD_ERR(t.toString().c_str());

					bExecute = FALSE;

					//add by viva
					if(packetID == Packet::PACKET_GC_FRIEND_CHATTING)
							bExecute = TRUE;
				}

//				DEBUG_ADD_FORMAT_ERR("*** RECEIVED Read OK0 PacketID=%u, Size=%d",  packetID, packetSize);
				
				if ( packetSize > g_pPacketFactoryManager->getPacketMaxSize( packetID ) )
				{
					DEBUG_ADD_FORMAT_ERR("[PacketError] too large packet SIZE: %d/%d", packetSize, g_pPacketFactoryManager->getPacketMaxSize( packetID ));		
					SendBugReport("too large PacketSize ID)%d %d/%d", packetID, packetSize, g_pPacketFactoryManager->getPacketMaxSize( packetID ) );
					TraceGameEntryFlow("ClientPlayer packet too large after CGConnect");
					throw InvalidProtocolException("too large packet SIZE");
				}
				
				
				
				if ( m_pInputStream->length() < szPacketHeader + packetSize )
				{
					DEBUG_ADD_FORMAT_ERR("[PacketError] InsufficientDataException: %d/%d", m_pInputStream->length(), szPacketHeader + packetSize);
		
					throw InsufficientDataException();
				}
				
				
				
				
				pPacket = g_pPacketFactoryManager->createPacket( packetID );

				
				
				
				m_pInputStream->read( pPacket );

				
				

				if (bExecute)
				{
					#ifdef __DEBUG_OUTPUT__
						DEBUG_ADD(pPacket->toString().c_str());
					#endif

//					_MinTrace("Incomming Packet ID : %d\n",pPacket->getPacketID());

					WriteCombatCrashMarker("dispatch before id=%u size=%d status=%d",
						packetID, packetSize, getPlayerStatus());
					pPacket->execute( this );
					WriteCombatCrashMarker("dispatch after id=%u", packetID);

					//DEBUG_ADD_FORMAT("[Executed] %s", pPacket->toString().c_str());
					DEBUG_ADD("[PacketExecute OK1]");				
				}
				
				
				
				m_PacketHistory.push_back(pPacket);
				pPacket = NULL;

				
				while ( m_PacketHistory.size() > nPacketHistory ) {
					Packet * oldPacket = m_PacketHistory.front();
					delete oldPacket;
					m_PacketHistory.pop_front();
				}
				DEBUG_ADD("[PacketExecute OK2]");
				
				//---------------------------------------------------------	
				
				
				//---------------------------------------------------------
				if (++processedPacket > maxProcessPacket)
				{
					DEBUG_ADD_FORMAT("[PacketSkip] So many Packets. MaxProcessPacket:%d, CurrentPacket:%d, File:%s, Line:%s",maxProcessPacket,processedPacket, __FILE__,__LINE__);
					//DEBUG_ADD_FORMAT_WAR(szBuf);

					break;
				}			
			}

		} catch ( InsufficientDataException& ) {

			
		}

	} catch (Throwable&)	{
		
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
void ClientPlayer::disconnect ( bool bDisconnected )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	if ( bDisconnected == UNDISCONNECTED ) {

		
		//GCDisconnect gcDisconnect;
		//sendPacket( gcDisconnect );

		
		try {
			m_pOutputStream->flush();
		} catch (ConnectException& t)	{
			DEBUG_ADD_ERR(t.toString().c_str());
		} catch (Throwable& t) {
			DEBUG_ADD_ERR(t.toString().c_str());
		}

	}

	
	try {
		m_pSocket->close();
	} catch (Throwable& ) {
		
	}

	
	//cout<< "PlayerStatus : " << (int)m_PlayerStatus << endl;
	
	//Assert( m_PlayerStatus != CPS_END_SESSION );
	m_PlayerStatus = CPS_END_SESSION;
	// add by Coffee 2007-7-15
	m_pOutputStream->InitSeq();
	__END_CATCH
}


//--------------------------------------------------------------------------------
//

// 

//

//
//--------------------------------------------------------------------------------
Packet * ClientPlayer::getOldPacket ( uint prev )
	throw ( OutOfBoundException , NoSuchElementException )
{
	__BEGIN_TRY

	if ( prev >= nPacketHistory )
		throw OutOfBoundException();

	// if prev == 0 , return m_PacketHistory[9]
	// if prev == 9 , return m_PacketHistory[0]
	Packet * pPacket = m_PacketHistory[ nPacketHistory - prev - 1 ];

	if ( pPacket == NULL )
		throw NoSuchElementException("packet history is empty");

	return pPacket;

	__END_CATCH
}


//--------------------------------------------------------------------------------
//

//
//--------------------------------------------------------------------------------
Packet * ClientPlayer::getOldPacket ( PacketID_t packetID )
	throw ( NoSuchElementException )
{
	__BEGIN_TRY

	Packet * pPacket = NULL;

	for ( std::deque<Packet*>::reverse_iterator ritr = m_PacketHistory.rbegin() ;
			ritr != m_PacketHistory.rend() ;
			ritr ++ ) {
		if ( (*ritr)->getPacketID() == packetID ) {
			pPacket = (*ritr);
			break;
		}	
	}

	if ( pPacket == NULL )
		throw NoSuchElementException();

	return pPacket;

	__END_CATCH
}


//--------------------------------------------------------------------------------
//
// get debug std::string
//
//--------------------------------------------------------------------------------
std::string ClientPlayer::toString () const
       throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "ClientPlayer("
		<< "SocketID:" << (int)m_pSocket->getSOCKET() 
		<< ",Host:" << m_pSocket->getHost() 
		<< ")" ;

	return msg.toString();

	__END_CATCH
}





void ClientPlayer::setEncryptCode()
	throw( Error )
{
#ifdef __USE_ENCRYPTER__
    __BEGIN_TRY

//	Assert(g_pPlayer!=NULL);
	Assert(g_pZone!=NULL);

	
//	ObjectID_t objectID = g_pPlayer->GetID();
	ZoneID_t zoneID = g_pZone->GetID();
	int serverID = g_pUserInformation->ServerID;

//	if (objectID!=0)
	{
//		uchar code = (uchar)(objectID / zoneID + objectID);		
		uchar code;

		if( g_pUserInformation->IsNetmarble )
			code = (uchar)( ( ( ( zoneID ) >> 8 ) ^ ( zoneID ) ) ^ ( ( ( serverID ) + 1 ) << 4 ) );
		else if ( g_pUserInformation->bChinese )
//			code = (uchar) ( ( ( ( ( serverID ) + 1 ) << 4 ) | ( zoneID ) ) ^ ( ( zoneID ) >> 8 ) );
			code = (uchar)( ( ( ( zoneID ) >> 8 ) ^ ( zoneID ) ) ^ ( ( ( serverID ) + 1 ) << 4 ) );
		else if ( g_pUserInformation->bEnglish )
			code = (uchar)( ( ( ( zoneID ) >> 8 ) ^ ( zoneID ) ) ^ ( ( ( serverID ) + 1 ) * 51 ) );
		else
			code = (uchar)( ( ( ( zoneID ) >> 8 ) ^ ( zoneID ) ) ^ ( ( ( serverID ) + 1 ) << 4 ) );

		SocketEncryptOutputStream* pEOS = dynamic_cast<SocketEncryptOutputStream*>(m_pOutputStream);
		Assert(pEOS!=NULL);

		SocketEncryptInputStream* pEIS = dynamic_cast<SocketEncryptInputStream*>(m_pInputStream);
		Assert(pEIS!=NULL);

		pEOS->setEncryptCode(code);
		pEIS->setEncryptCode(code);
	}

	__END_CATCH
#endif
}
