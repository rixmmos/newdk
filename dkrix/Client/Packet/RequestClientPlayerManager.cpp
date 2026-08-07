//--------------------------------------------------------------------------------
// RequestClientPlayerManager.cpp
//--------------------------------------------------------------------------------

//#include "RequestServerPlayerManager.h"
#include "Client_PCH.h"
#include "RequestClientPlayerManager.h"
#include "WhisperManager.h"
#include "ClientConfig.h"
#include "MPlayer.h"
#include "UserInformation.h"
#include "MGameStringTable.h"
#include "RequestUserManager.h"
#include "ProfileManager.h"
#include "DebugInfo.h"
#include "ServerInfo.h"
#include "ClientDef.h"

// Platform-specific threading includes
#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
	#include <process.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)
	#include <pthread.h>
	#include <unistd.h>
#endif

#include "Rpackets/CRConnect.h"
#include "Rpackets/CRWhisper.h"
#include "Rpackets/CRRequest.h"

// Platform-specific threading macros and stubs
#if defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)
	// Additional Windows type definitions
	typedef DWORD* LPDWORD;
	typedef void* (*LPTHREAD_START_ROUTINE)(void*);

	// Undefine macros that conflict with our stub functions
	#undef CloseHandle

	// Stub constants
	#define STILL_ACTIVE ((DWORD)-1)
	#define THREAD_PRIORITY_NORMAL 0
	#define THREAD_PRIORITY_LOWEST -2

	// Stub functions - simplified implementations for macOS/Linux
	static inline BOOL TerminateThread(HANDLE thread, DWORD exitCode) {
		return (pthread_cancel((pthread_t)(size_t)thread) == 0);
	}

	static inline BOOL CloseHandle(HANDLE handle) {
		// For pthread_t, no explicit close needed
		return TRUE;
	}

	static inline BOOL GetExitCodeThread(HANDLE thread, LPDWORD lpExitCode) {
		// Stub: just return FALSE to indicate not implemented
		return FALSE;
	}

	static inline HANDLE GetCurrentThread() {
		return (HANDLE)pthread_self();
	}

	static inline BOOL SetThreadPriority(HANDLE thread, int priority) {
		// Stub: pthread thread priority is complex, just return TRUE
		return TRUE;
	}

	// Stub thread creation
	static inline HANDLE _beginthreadex(void* security, unsigned stack_size,
		LPTHREAD_START_ROUTINE start_proc, void* arg,
		unsigned flags, DWORD* thread_id) {
		pthread_t thread;
		if (pthread_create(&thread, NULL, (void*(*)(void*))start_proc, arg) == 0) {
			if (thread_id) *thread_id = (unsigned long)thread;
			return (HANDLE)(size_t)thread;
		}
		return (HANDLE)0;
	}

// Note: CreateThread stub removed - use platform_thread_create from Platform.h
// #ifdef PLATFORM_WINDOWS... (removed)

#endif /* __APPLE__ || __linux__ || __EMSCRIPTEN__ */

#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
	extern CMessageArray*		g_pGameMessage;
#endif


//--------------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------------
RequestClientPlayerManager*	g_pRequestClientPlayerManager = NULL;

LONG					RequestConnectionThreadProc(LPVOID lpParameter);

//--------------------------------------------------------------------------------
//
// constructor / destructor
//
//--------------------------------------------------------------------------------
RequestClientPlayerManager::RequestClientPlayerManager()
{
}

RequestClientPlayerManager::~RequestClientPlayerManager()
{
	Release();

}

//--------------------------------------------------------------------------------
// Release
//--------------------------------------------------------------------------------
void
RequestClientPlayerManager::Release()
{
	Lock();

	//------------------------------------------------------------------------
	// thread
	//------------------------------------------------------------------------
	HANDLE_LIST::iterator iHandle = m_listConnectionThread.begin();

	while (iHandle != m_listConnectionThread.end())
	{
		HANDLE handle = *iHandle;

		TerminateThread( handle, 0 );
		CloseHandle( handle );

		iHandle ++;
	}

	m_listConnectionThread.clear();

	//------------------------------------------------------------------------
	// m_mapRequestClientPlayer
	//------------------------------------------------------------------------
	REQUESTCLIENTPLAYER_MAP::iterator iPlayer = m_mapRequestClientPlayer.begin();
		
	while (iPlayer != m_mapRequestClientPlayer.end())
	{
		RequestClientPlayer* pPlayer = iPlayer->second;

		try {
			delete pPlayer;
		} catch (Throwable&) {
		}
		
		iPlayer++;
	}

	m_mapRequestClientPlayer.clear();

	//------------------------------------------------------------------------
	// m_mapConnectionInfo
	//------------------------------------------------------------------------
	CONNECTION_INFO_MAP::iterator iInfo = m_mapConnectionInfo.begin();
		
	while (iInfo != m_mapConnectionInfo.end())
	{	
		CONNECTION_INFO* pInfo = iInfo->second;

		delete pInfo;

		iInfo++;
	}

	m_mapConnectionInfo.clear();
		

	Unlock();
}

//----------------------------------------------------------------------
// Remove Terminated Thread
//----------------------------------------------------------------------
void		
RequestClientPlayerManager::RemoveTerminatedThread()
{
	HANDLE_LIST::iterator iHandle = m_listConnectionThread.begin();

	DWORD exitCode;
		
	while (iHandle != m_listConnectionThread.end())
	{
		HANDLE handle = *iHandle;

		if (GetExitCodeThread(handle, &exitCode))
		{
			if (exitCode!=STILL_ACTIVE)
			{
				CloseHandle( handle );

				HANDLE_LIST::iterator iTemp = iHandle ++;

				m_listConnectionThread.erase( iTemp );

				continue;
			}
		}

		iHandle ++;
	}
}

//----------------------------------------------------------------------
// Connect
//----------------------------------------------------------------------

//


// 

//----------------------------------------------------------------------
void		
RequestClientPlayerManager::Connect(const char* pIP, const char* pRequestName, REQUEST_CLIENT_MODE requestMode)
{
	
	if (m_mapRequestClientPlayer.size() >= g_pClientConfig->MAX_REQUEST_SERVICE)
	{
		return;
	}

	if (HasConnection(pRequestName)
		|| HasTryingConnection(pRequestName))
	{
		return;
	}

	int port = 9650;
	
	//RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo(pRequestName);

	//if (pUserInfo!=NULL && pUserInfo->TCPPort!=0)
	{
	//	port = pUserInfo->TCPPort;
	}

	
	CONNECTION_INFO* pInfo = new CONNECTION_INFO;
	pInfo->name			= pRequestName;
	pInfo->ip			= pIP;
	pInfo->port			= port;
	pInfo->requestMode	= requestMode;

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	Lock();		

	m_mapConnectionInfo[pInfo->name] = pInfo;
	
#ifdef PLATFORM_WINDOWS
	DWORD dwChildThreadID;
	HANDLE hConnectionThread = CreateThread(NULL, 
									0,	// default stack size
									(LPTHREAD_START_ROUTINE)RequestConnectionThreadProc,
									pInfo,
									NULL,
									&dwChildThreadID);

	
	SetThreadPriority(hConnectionThread, THREAD_PRIORITY_LOWEST);	
#else
	// Non-Windows: Use platform_thread_create
	HANDLE hConnectionThread = (HANDLE)platform_thread_create(
		(platform_thread_func_t)RequestConnectionThreadProc,
		pInfo
	);
	(void)dwChildThreadID; // unused on non-Windows
#endif

	
	m_listConnectionThread.push_back( hConnectionThread );

	Unlock();
}

//----------------------------------------------------------------------
// Send Packet
//----------------------------------------------------------------------
bool		
RequestClientPlayerManager::SendPacket(const char* pName, Packet* pPacket)
{
	Lock();

	REQUESTCLIENTPLAYER_MAP::const_iterator iPlayer = m_mapRequestClientPlayer.find( std::string(pName) );
		
	if (iPlayer != m_mapRequestClientPlayer.end())
	{
		RequestClientPlayer* pPlayer = iPlayer->second;

		pPlayer->sendPacket( pPacket );

		Unlock();
		return true;
	}

	Unlock();
	return false;
	
}

//----------------------------------------------------------------------
// Has Connection
//----------------------------------------------------------------------
bool	
RequestClientPlayerManager::HasConnection(const char* pRequestName)
{
	Lock();

	std::string name = pRequestName;

	
	REQUESTCLIENTPLAYER_MAP::const_iterator iPlayer = m_mapRequestClientPlayer.find( name );
		
	if (iPlayer != m_mapRequestClientPlayer.end())
	{
		Unlock();		
		return true;		
	}
	
	Unlock();
	return false;	
}

//----------------------------------------------------------------------
// Has TryingConnection
//----------------------------------------------------------------------
bool	
RequestClientPlayerManager::HasTryingConnection(const char* pRequestName)
{
	Lock();

	std::string name = pRequestName;

	
	CONNECTION_INFO_MAP::const_iterator iInfo = m_mapConnectionInfo.find( name );
		
	if (iInfo != m_mapConnectionInfo.end())
	{	
		Unlock();
		return true;
	}

	Unlock();
	return false;	
}

//----------------------------------------------------------------------
// Disconnect
//----------------------------------------------------------------------
void		
RequestClientPlayerManager::Disconnect(const char* pRequestName)
{
	Lock();

	REQUESTCLIENTPLAYER_MAP::iterator iPlayer = m_mapRequestClientPlayer.find( std::string(pRequestName) );
		
	if (iPlayer != m_mapRequestClientPlayer.end())
	{
		RequestClientPlayer* pPlayer = iPlayer->second;

		try {

			m_mapRequestClientPlayer.erase( iPlayer );
			delete pPlayer;				

		} catch (Throwable& t)	{
			DEBUG_ADD_ERR( t.toString().c_str() );
		}

		Unlock();
		return;		
	}

	Unlock();
}

//--------------------------------------------------------------------------------
// Add RequestClientPlayer
//--------------------------------------------------------------------------------
bool
RequestClientPlayerManager::AddRequestClientPlayer(RequestClientPlayer* pRequestClientPlayer)
{
	Lock();	

	bool bAdd = false;

	std::string serverName = pRequestClientPlayer->getRequestServerName();

	if (g_Mode==MODE_GAME)
	{		
		
		//if (m_mapRequestClientPlayer.size() < g_pClientConfig->MAX_REQUEST_SERVICE)
		{
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			m_mapRequestClientPlayer[pRequestClientPlayer->getRequestServerName()] = pRequestClientPlayer;

			pRequestClientPlayer->setPlayerStatus( CPS_REQUEST_CLIENT_BEGIN_SESSION );

			#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
				if (g_pGameMessage!=NULL)
					g_pGameMessage->AddFormat("Connected To %s", pRequestClientPlayer->getRequestServerName().c_str());
			#endif

			bAdd = true;
		}
	}
	else
	{
		pRequestClientPlayer->disconnect( UNDISCONNECTED );
		delete pRequestClientPlayer;
	}
	
	
	CONNECTION_INFO_MAP::iterator iInfo = m_mapConnectionInfo.find( serverName );
		
	if (iInfo != m_mapConnectionInfo.end())
	{	
		CONNECTION_INFO* pInfo = iInfo->second;

		delete pInfo;
		m_mapConnectionInfo.erase( iInfo );
	}

	Unlock();

	return bAdd;	
}

//--------------------------------------------------------------------------------
// Add RequestClientPlayer
//--------------------------------------------------------------------------------
bool
RequestClientPlayerManager::RemoveRequestClientPlayer(const char* pRequestName)
{
	Lock();

	REQUESTCLIENTPLAYER_MAP::iterator iPlayer = m_mapRequestClientPlayer.find( std::string(pRequestName) );
		
	if (iPlayer != m_mapRequestClientPlayer.end())
	{
		RequestClientPlayer* pPlayer = iPlayer->second;

		try {

			pPlayer->disconnect(UNDISCONNECTED);
			delete pPlayer;

		} catch (Throwable& t) {
			DEBUG_ADD_ERR( t.toString().c_str() );
		}

		Unlock();
		return true;
	}	

	Unlock();

	return false;
}

//--------------------------------------------------------------------------------
// Remove ConnectionInfo
//--------------------------------------------------------------------------------
void
RequestClientPlayerManager::RemoveConnectionInfo(const char* pName)
{
	Lock();

	CONNECTION_INFO_MAP::iterator iInfo = m_mapConnectionInfo.find( pName );

	if (iInfo != m_mapConnectionInfo.end())
	{	
		CONNECTION_INFO* pInfo = iInfo->second;

		delete pInfo;

		m_mapConnectionInfo.erase( iInfo );
	}

	Unlock();
}

//--------------------------------------------------------------------------------
// Process Mode
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void
RequestClientPlayerManager::ProcessMode(RequestClientPlayer* pRequestClientPlayer)
{
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	switch (pRequestClientPlayer->getRequestMode())
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		case REQUEST_CLIENT_MODE_NULL :
		{
			if (pRequestClientPlayer->getPlayerStatus()==CPS_REQUEST_CLIENT_BEGIN_SESSION)
			{
				CRConnect _CRConnect;
				_CRConnect.setRequestServerName( pRequestClientPlayer->getRequestServerName().c_str() );
				_CRConnect.setRequestClientName( g_pUserInformation->CharacterID.GetString() );

				
				pRequestClientPlayer->setPlayerStatus( CPS_REQUEST_CLIENT_AFTER_SENDING_CONNECT );			

				pRequestClientPlayer->sendPacket( &_CRConnect );
			}
		}
		break;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		case REQUEST_CLIENT_MODE_WHISPER :
		{
			if (pRequestClientPlayer->getPlayerStatus()==CPS_REQUEST_CLIENT_BEGIN_SESSION)
			{
				const char* pRequestServerName = pRequestClientPlayer->getRequestServerName().c_str();

				
				if (g_pWhisperManager->HasWhisperMessage( pRequestServerName ))
				{
					const std::list<WHISPER_MESSAGE>* pMessageList = g_pWhisperManager->GetWhisperMessages( pRequestServerName );

					if (pMessageList)
					{
						
						CRWhisper _CRWhisper;

						_CRWhisper.setName( g_pUserInformation->CharacterID.GetString() );
						_CRWhisper.setTargetName( pRequestServerName );
						
//						if (g_pPlayer->IsSlayer())
//						{
//							_CRWhisper.setSlayer();
//						}
//						else
//						{
//							_CRWhisper.setVampire();
//						}
						_CRWhisper.setRace(g_pPlayer->GetRace());

						_CRWhisper.setWorldID(g_pUserInformation->WorldID);

						std::list<WHISPER_MESSAGE>::const_iterator iMessage = pMessageList->begin();

						
						while (iMessage != pMessageList->end())
						{
							_CRWhisper.addMessage ( *iMessage );

							iMessage ++;
						}

						pRequestClientPlayer->sendPacket( &_CRWhisper );

						pRequestClientPlayer->setPlayerStatus( CPS_REQUEST_CLIENT_NORMAL );
					}

					g_pWhisperManager->RemoveWhisperMessage( pRequestServerName );
				}					
			}
		}
		break;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		case REQUEST_CLIENT_MODE_PROFILE :
		{
			if (pRequestClientPlayer->getPlayerStatus()==CPS_REQUEST_CLIENT_BEGIN_SESSION)
			{
				const char* pRequestServerName = pRequestClientPlayer->getRequestServerName().c_str();

				CRRequest crRequest;
				crRequest.setCode( CR_REQUEST_FILE_PROFILE );
				crRequest.setRequestName( pRequestServerName );

				pRequestClientPlayer->sendPacket( &crRequest );

				pRequestClientPlayer->setPlayerStatus( CPS_REQUEST_CLIENT_NORMAL );
			}
		}
		break;
	}
}

//--------------------------------------------------------------------------------
// Update
//--------------------------------------------------------------------------------
void
RequestClientPlayerManager::Update()
{
	Lock();

	if (m_mapRequestClientPlayer.empty())
	{
		Unlock();
		return;
	}

	RemoveTerminatedThread();

	try {

		REQUESTCLIENTPLAYER_MAP::iterator iPlayer = m_mapRequestClientPlayer.begin();

		while (iPlayer != m_mapRequestClientPlayer.end())
		{
			RequestClientPlayer* pPlayer = iPlayer->second;

			if(pPlayer != NULL)
			{
				try {

					if (!pPlayer->getSocket()->isValid())
					{
						throw SocketException("sock error");
					}

					ProcessMode(pPlayer);

					pPlayer->processInput();
					pPlayer->processCommand();
					pPlayer->processOutput();

				} catch (Throwable &t) 	{

					DEBUG_ADD_ERR( t.toString().c_str() );

					
					//if (g_pRequestServerPlayerManager!=NULL)
					{
					//	g_pRequestServerPlayerManager->Disconnect( pPlayer->getRequestServerName().c_str() );
					}

					
					pPlayer->disconnect(UNDISCONNECTED);
					delete pPlayer;

					REQUESTCLIENTPLAYER_MAP::iterator iTemp = iPlayer;
					iPlayer++;

					m_mapRequestClientPlayer.erase( iTemp );

					continue;
				}
			}

			iPlayer++;
		}

	} catch (Throwable&t) {
		DEBUG_ADD_ERR( t.toString().c_str() );
	}

	Unlock();
}

//--------------------------------------------------------------------------------
// RequestConnectionThreadProc
//--------------------------------------------------------------------------------
LONG					
RequestConnectionThreadProc(LPVOID lpParameter)
{
	CONNECTION_INFO* pInfo = (CONNECTION_INFO*)lpParameter;
	
	HANDLE hConnectionThread = GetCurrentThread();

	Socket * pSocket = NULL;

	//for (int i=0; i<5; i++)
	{
		try {
			pSocket = new Socket( pInfo->ip, pInfo->port );

			// try to connect to server
			pSocket->connect();

			// make nonblocking socket
			pSocket->setNonBlocking();

			// make no-linger socket
			pSocket->setLinger(0);

			// create player
			RequestClientPlayer* pPlayer = new RequestClientPlayer(pSocket);
			pSocket = NULL;

			pPlayer->setRequestServerName( pInfo->name.c_str() );
			pPlayer->setRequestServerIP( pInfo->ip.c_str() );
			pPlayer->setRequestMode( pInfo->requestMode );

			if (g_pRequestClientPlayerManager!=NULL)
			{
				SetThreadPriority(hConnectionThread, THREAD_PRIORITY_NORMAL);

				g_pRequestClientPlayerManager->AddRequestClientPlayer( pPlayer );

				SetThreadPriority(hConnectionThread, THREAD_PRIORITY_LOWEST);
			}
			else
			{
			}

		} catch (Throwable& t) {

			DEBUG_ADD_FORMAT_ERR("[RequestClientPlayerManager] Can't Connect %s:%s(%d)", pInfo->name.c_str(), pInfo->ip.c_str(), pInfo->port);
			DEBUG_ADD(t.toString().c_str());

			if (pSocket!=NULL)
			{
				delete pSocket;
			}

			//------------------------------------------------------
			
			//------------------------------------------------------
			switch (pInfo->requestMode)
			{
				//------------------------------------------------------
				
				//------------------------------------------------------
				case REQUEST_CLIENT_MODE_WHISPER :

					SetThreadPriority(hConnectionThread, THREAD_PRIORITY_NORMAL);

					
					
//					UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_WHISPER_FAILED].GetString(), NULL, 5 );
					g_pRequestUserManager->RemoveRequestUserLater( pInfo->name.c_str() );
//					g_pWhisperManager->RemoveWhisperMessage( pInfo->name.c_str() );
					g_pWhisperManager->TryToSendWhisperMessage( pInfo->name.c_str() );					

					SetThreadPriority(hConnectionThread, THREAD_PRIORITY_LOWEST);
				break;

				//------------------------------------------------------
				
				//------------------------------------------------------
				case REQUEST_CLIENT_MODE_PROFILE :
					g_pProfileManager->RemoveRequire(pInfo->name.c_str());
				break;
			}

			
			g_pRequestClientPlayerManager->RemoveConnectionInfo(pInfo->name.c_str());
		}
	}

	//delete pInfo;

	return 0L;
}
