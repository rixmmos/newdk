//--------------------------------------------------------------------------------
// RequestClientPlayerManager.h
//--------------------------------------------------------------------------------
//



//

//

//
//
//--------------------------------------------------------------------------------

#ifndef __REQUEST_CLIENT_PLAYER_MANAGER_H__
#define __REQUEST_CLIENT_PLAYER_MANAGER_H__

#pragma warning(disable:4786)

#if defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>
#else
	#include "../../basic/Platform.h"
	#include <pthread.h>
#endif
#include <list>
#include <map>
#include <string>
#include "RequestClientPlayer.h"
#include "Packet.h"

//--------------------------------------------------------------------------------
// CONNCETION_INFO
//--------------------------------------------------------------------------------
class CONNECTION_INFO
{
	public :
		std::string				name;
		std::string				ip;
		int					port;
		REQUEST_CLIENT_MODE	requestMode;

	public :
		CONNECTION_INFO()
		{
			requestMode = REQUEST_CLIENT_MODE_NULL;
			port = 0;
		}
};

//--------------------------------------------------------------------------------
// RequestClientPlayerManager
//--------------------------------------------------------------------------------
class RequestClientPlayerManager {
	public :
		typedef std::map<std::string, RequestClientPlayer*>	REQUESTCLIENTPLAYER_MAP;
		typedef std::list<HANDLE>							HANDLE_LIST;
		typedef std::map<std::string, CONNECTION_INFO*>		CONNECTION_INFO_MAP;

	public :
		RequestClientPlayerManager();
		~RequestClientPlayerManager();

		//----------------------------------------------------------------------
		// Release
		//----------------------------------------------------------------------
		void		Release();

		//----------------------------------------------------------------------
		// Connect / Disconnect
		//----------------------------------------------------------------------
		void		Connect(const char* pIP, const char* pRequestName, REQUEST_CLIENT_MODE requestMode=REQUEST_CLIENT_MODE_NULL);
		void		Disconnect(const char* pRequestName);
		bool		HasConnection(const char* pRequesName);
		bool		HasTryingConnection(const char* pRequesName);

		bool		AddRequestClientPlayer(RequestClientPlayer* pRequestClientPlayer);
		
		//----------------------------------------------------------------------
		// send Packet
		//----------------------------------------------------------------------
		bool		SendPacket(const char* pName, Packet* pPacket);
		
		//----------------------------------------------------------------------
		// Update
		//----------------------------------------------------------------------
		void		Update();

		void		ProcessMode(RequestClientPlayer* pRequestClientPlayer);

		int			GetSize() const			{ return m_mapRequestClientPlayer.size(); }		

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		void		RemoveConnectionInfo(const char* pName);

	protected :
		//----------------------------------------------------------------------
		// Lock / Unlock
		//----------------------------------------------------------------------
		void		Lock()					{ EnterCriticalSection(&m_Lock); }
		void		Unlock()				{ LeaveCriticalSection(&m_Lock); }

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		void		RemoveTerminatedThread();

		//----------------------------------------------------------------------
		// Add / Remove
		//----------------------------------------------------------------------
		bool		RemoveRequestClientPlayer(const char* pRequestName);		
	
	protected :
		HANDLE_LIST						m_listConnectionThread;
		
		CRITICAL_SECTION				m_Lock;

		REQUESTCLIENTPLAYER_MAP			m_mapRequestClientPlayer;

		CONNECTION_INFO_MAP				m_mapConnectionInfo;
};

extern RequestClientPlayerManager*	g_pRequestClientPlayerManager;

#endif

