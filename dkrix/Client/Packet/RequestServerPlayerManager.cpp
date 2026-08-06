//--------------------------------------------------------------------------------
// RequestServerPlayerManager.cpp
//--------------------------------------------------------------------------------
#include "Client_PCH.h"
#include "RequestServerPlayerManager.h"
//#include "RequestClientPlayerManager.h"
#include "ClientConfig.h"
#include "ClientDef.h"
#include "DebugInfo.h"
#include "ServerInfo.h"

// Platform-specific threading includes
#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
	#include <process.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)
	#include <pthread.h>
	#include <unistd.h>
	#include <SDL2/SDL.h>  // For SDL_Delay on non-Windows platforms

	// Additional Windows type definitions
	typedef DWORD* LPDWORD;
	typedef void* (*LPTHREAD_START_ROUTINE)(void*);
	typedef void* LPVOID;

	// Undefine macros that conflict with our stub functions
	#undef CloseHandle

	// Stub constants
	#define STILL_ACTIVE ((DWORD)-1)
	#define THREAD_PRIORITY_NORMAL 0
	#define THREAD_PRIORITY_LOWEST -2

	// Stub functions
	static inline BOOL TerminateThread(HANDLE thread, DWORD exitCode) {
		return (pthread_cancel((pthread_t)(size_t)thread) == 0);
	}

	static inline BOOL CloseHandle(HANDLE handle) {
		return TRUE;
	}

	static inline HANDLE GetCurrentThread() {
		return (HANDLE)pthread_self();
	}

	static inline BOOL SetThreadPriority(HANDLE thread, int priority) {
		return TRUE;
	}

	// Stub thread creation
	static inline HANDLE _beginthreadex(void* security, unsigned stack_size,
		LPTHREAD_START_ROUTINE start_proc, LPVOID arg,
		unsigned flags, DWORD* thread_id) {
		pthread_t thread;
		if (pthread_create(&thread, NULL, (void*(*)(void*))start_proc, arg) == 0) {
			if (thread_id) *thread_id = (unsigned long)thread;
			return (HANDLE)(size_t)thread;
		}
		return (HANDLE)0;
}
#endif

// Note: CreateThread stub removed - use platform_thread_create from Platform.h
// #ifdef PLATFORM_WINDOWS... (removed)

//#include "Rpackets/RCPositionInfo.h"

#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
	extern CMessageArray*		g_pGameMessage;
#endif

#ifdef OUTPUT_DEBUG
	extern CRITICAL_SECTION			g_Lock;
#endif


//--------------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------------
RequestServerPlayerManager*	g_pRequestServerPlayerManager = NULL;

LONG					WaitRequestThreadProc(LPVOID lpParameter);

//--------------------------------------------------------------------------------
//
// constructor / destructor
//
//--------------------------------------------------------------------------------
RequestServerPlayerManager::RequestServerPlayerManager()
{
	m_pServerSocket		= NULL;
	m_hRequestThread	= NULL;
	m_bThreadRunning	= false;

	InitializeCriticalSection(&m_Lock);
}

RequestServerPlayerManager::~RequestServerPlayerManager()
{
	Release();

	DeleteCriticalSection(&m_Lock);
}

//--------------------------------------------------------------------------------
// Release
//--------------------------------------------------------------------------------
void
RequestServerPlayerManager::Release()
{
	// Signal thread to stop gracefully
	m_bThreadRunning = false;

	// Wait for thread to exit (with timeout)
	if (m_hRequestThread != NULL)
	{
#ifdef PLATFORM_WINDOWS
		// Windows: Wait up to 2 seconds for thread to exit
		DWORD waitResult = WaitForSingleObject(m_hRequestThread, 2000);
		if (waitResult == WAIT_TIMEOUT)
		{
			// Thread didn't exit gracefully, force terminate
			TerminateThread(m_hRequestThread, 0);
		}
		CloseHandle(m_hRequestThread);
#else
		// macOS/Linux: pthread_join with timeout simulation
		// Note: We don't force terminate on POSIX as it can corrupt heap
		// Just close the handle and let the thread exit on its own
		CloseHandle(m_hRequestThread);
#endif
		m_hRequestThread = NULL;
	}

	try {
		if (m_pServerSocket!=NULL)
		{
			delete m_pServerSocket;
			m_pServerSocket = NULL;
		}
	} catch (Throwable&) {
	}

	Lock();

	RequestServerPlayer_LIST::iterator iPlayer = m_listRequestServerPlayer.begin();

	while (iPlayer != m_listRequestServerPlayer.end())
	{
		RequestServerPlayer* pPlayer = *iPlayer;

		try {
			delete pPlayer;
		} catch (Throwable&) {
		}

		iPlayer++;
	}

	m_listRequestServerPlayer.clear();

	Unlock();
}

//--------------------------------------------------------------------------------
// Add RequestServerPlayer
//--------------------------------------------------------------------------------
bool
RequestServerPlayerManager::AddRequestServerPlayer(RequestServerPlayer* pRequestServerPlayer)
{
	if (1)//g_Mode==MODE_GAME)
	{
		Lock();

		
		if (m_listRequestServerPlayer.size() < g_pClientConfig->MAX_REQUEST_SERVICE)
		{
			
			m_listRequestServerPlayer.push_back( pRequestServerPlayer );

			#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
			
				EnterCriticalSection(&g_Lock);
	
				if (g_pGameMessage!=NULL)
				{
					Socket* pSocket = pRequestServerPlayer->getSocket();

					if (pSocket!=NULL)
					{
						const char* addrIP = pSocket->getHost().c_str();
						g_pGameMessage->AddFormat("New Connection From %s", addrIP);
					}
					else
					{
						g_pGameMessage->AddFormat("New Connection From where?");
					}
				}

				LeaveCriticalSection(&g_Lock);
			#endif

			Unlock();
			return true;
		}
		else
		{
			pRequestServerPlayer->disconnect( UNDISCONNECTED );
			delete pRequestServerPlayer;
		}
		
		Unlock();		
	}
	/*
	else
	{
		pRequestServerPlayer->disconnect( UNDISCONNECTED );
		delete pRequestServerPlayer;
	}
	*/

	return false;	
}

//--------------------------------------------------------------------------------
// Disconnect
//--------------------------------------------------------------------------------
void		
RequestServerPlayerManager::Disconnect(const char* pName)
{
	Lock();


	RequestServerPlayer_LIST::iterator iPlayer = m_listRequestServerPlayer.begin();
		
	while (iPlayer != m_listRequestServerPlayer.end())
	{
		RequestServerPlayer* pPlayer = *iPlayer;

		
		if (pPlayer->getName()==pName)
		{
			pPlayer->disconnect(UNDISCONNECTED);
			delete pPlayer;

			m_listRequestServerPlayer.erase( iPlayer );

			Unlock();
			return;
		}
		
		iPlayer++;
	}


	Unlock();
}

//--------------------------------------------------------------------------------
// Broadcast
//--------------------------------------------------------------------------------
void		
RequestServerPlayerManager::Broadcast(Packet* pPacket)
{
	Lock();

	
	RequestServerPlayer_LIST::iterator iPlayer = m_listRequestServerPlayer.begin();
		
	while (iPlayer != m_listRequestServerPlayer.end())
	{
		RequestServerPlayer* pPlayer = *iPlayer;

		pPlayer->sendPacket( pPacket );
		
		iPlayer++;
	}

	Unlock();
}


//--------------------------------------------------------------------------------
// Update
//--------------------------------------------------------------------------------
void		
RequestServerPlayerManager::ProcessMode(RequestServerPlayer* pPlayer)
{
	 
}


//--------------------------------------------------------------------------------
// Update
//--------------------------------------------------------------------------------
void
RequestServerPlayerManager::Update()
{
	Lock();

	if (m_listRequestServerPlayer.empty())
	{
		Unlock();
		return;
	}

	try {

		RequestServerPlayer_LIST::iterator iPlayer = m_listRequestServerPlayer.begin();

		while (iPlayer != m_listRequestServerPlayer.end())
		{
			RequestServerPlayer* pPlayer = *iPlayer;

			try {

				if (!pPlayer->getSocket()->isValid())
				{
					throw SocketException("sock error");
				}

				ProcessMode( pPlayer );

				pPlayer->processInput();
				pPlayer->processCommand();
				pPlayer->processOutput();

			} catch (NonBlockingIOException& t) {

				
				DEBUG_ADD_ERR( t.toString().c_str() );

			} catch (Throwable &t) 	{

				DEBUG_ADD_ERR( t.toString().c_str() );

				
				//if (g_pRequestClientPlayerManager!=NULL)
				{
				//	g_pRequestClientPlayerManager->Disconnect( pPlayer->getName().c_str() );
				}

				
				pPlayer->disconnect(UNDISCONNECTED);
				delete pPlayer;

				RequestServerPlayer_LIST::iterator iTemp = iPlayer;
				iPlayer++;

				m_listRequestServerPlayer.erase( iTemp );

				continue;
			}

			iPlayer++;
		}

	} catch (Throwable&t) {
		DEBUG_ADD_ERR( t.toString().c_str() );
	}

	Unlock();
}

//--------------------------------------------------------------------------------
// Init
//--------------------------------------------------------------------------------
void
RequestServerPlayerManager::Init(int port)
{
	if (m_pServerSocket!=NULL)
	{
		delete m_pServerSocket;
	}

	m_pServerSocket = new ServerSocket( port );

	// Set running flag before creating thread
	m_bThreadRunning = true;

	DWORD dwChildThreadID;	

#ifdef PLATFORM_WINDOWS
	m_hRequestThread = CreateThread(NULL,
								0,	// default stack size
								(LPTHREAD_START_ROUTINE)WaitRequestThreadProc,
								this,
								NULL,
								&dwChildThreadID);

	
	SetThreadPriority(m_hRequestThread, THREAD_PRIORITY_LOWEST);
#else
	// Non-Windows: Use platform_thread_create
	m_hRequestThread = (HANDLE)platform_thread_create(
		(platform_thread_func_t)WaitRequestThreadProc,
		this
	);
	(void)dwChildThreadID; // unused on non-Windows
#endif
}

//--------------------------------------------------------------------------------
// Wait Request
//--------------------------------------------------------------------------------
void
RequestServerPlayerManager::WaitRequest()
{
	// Check if socket is still valid (may be NULL during shutdown)
	if (m_pServerSocket == NULL || !m_bThreadRunning)
	{
		return;
	}

	Socket* pSocket = m_pServerSocket->accept();

	// accept() returns NULL if no connection or socket closed
	if (pSocket == NULL)
	{
		return;
	}

	
	RequestServerPlayer* pRequestServerPlayer = new RequestServerPlayer( pSocket );

	pRequestServerPlayer->setPlayerStatus( CPS_REQUEST_SERVER_BEGIN_SESSION );

	pSocket->setNonBlocking();

#ifdef PLATFORM_WINDOWS
	SetThreadPriority(m_hRequestThread, THREAD_PRIORITY_NORMAL);
#else
	// Non-Windows: Thread priority not supported via platform_thread_create
#endif

	if (AddRequestServerPlayer( pRequestServerPlayer ))
	{
		
		//DEBUG_ADD_FORMAT("[Request] New Connection from %s:%d", pSocket->getHost().c_str(), pSocket->getPort());
	}

	SetThreadPriority(m_hRequestThread, THREAD_PRIORITY_LOWEST);
}

//--------------------------------------------------------------------------------
// WaitRequest
//--------------------------------------------------------------------------------
LONG
WaitRequestThreadProc(LPVOID lpParameter)
{
	RequestServerPlayerManager* pRequestServerPlayerManager = (RequestServerPlayerManager*)lpParameter;

	while (pRequestServerPlayerManager != NULL && pRequestServerPlayerManager->IsThreadRunning())
	{
		pRequestServerPlayerManager->WaitRequest();

		// Add delay to prevent CPU busy-wait
		// On non-Windows platforms, sleep for 10ms to reduce CPU usage
#ifndef PLATFORM_WINDOWS
		SDL_Delay(10);  // 10ms delay to reduce CPU from busy-wait loop
#endif
	}

	return 0L;
}
