//--------------------------------------------------------------------------------
// RequestServerPlayerManager.h
//--------------------------------------------------------------------------------
//



//

//

//
//
//--------------------------------------------------------------------------------

#ifndef __REQUEST_SERVER_PLAYER_MANAGER_H__
#define __REQUEST_SERVER_PLAYER_MANAGER_H__

#pragma warning(disable:4786)

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <list>
#include <mutex>
#include "RequestServerPlayer.h"
#include "ServerSocket.h"


class RequestServerPlayerManager {
	public :
		typedef std::list<RequestServerPlayer*>	RequestServerPlayer_LIST;

	public :
		RequestServerPlayerManager();
		~RequestServerPlayerManager();

		//----------------------------------------------------------------------
		// Init / Wait
		//----------------------------------------------------------------------
		void		Init(int port=9650);
		void		Release();

		//----------------------------------------------------------------------
		// Wait Request
		//----------------------------------------------------------------------
		void		WaitRequest();

		//----------------------------------------------------------------------
		// Update
		//----------------------------------------------------------------------
		void		Update();

		//----------------------------------------------------------------------
		// Disconnect
		//----------------------------------------------------------------------
		void		Disconnect(const char* pName);

		//----------------------------------------------------------------------
		// broadcast
		//----------------------------------------------------------------------
		void		Broadcast(Packet* pPacket);


		int			GetSize() const			{ return m_listRequestServerPlayer.size(); }

		// Check if thread should continue running
		bool		IsThreadRunning() const		{ return m_bThreadRunning; }		

	protected :
		//----------------------------------------------------------------------
		// Lock / Unlock
		//----------------------------------------------------------------------
		void		Lock()					{ m_Lock.lock(); }
		void		Unlock()				{ m_Lock.unlock(); }

		//----------------------------------------------------------------------
		// RequestServerPlayer
		//----------------------------------------------------------------------
		bool		AddRequestServerPlayer(RequestServerPlayer* pRequestServerPlayer);

		//----------------------------------------------------------------------
		// Process Mode
		//----------------------------------------------------------------------
		void		ProcessMode(RequestServerPlayer* pPlayer);	

	
	protected :
		ServerSocket*			m_pServerSocket;

		HANDLE					m_hRequestThread;
		bool					m_bThreadRunning;

		std::mutex		m_Lock;
		RequestServerPlayer_LIST		m_listRequestServerPlayer;
};

extern RequestServerPlayerManager*	g_pRequestServerPlayerManager;

#endif

