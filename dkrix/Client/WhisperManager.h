//----------------------------------------------------------------------
// WhisperManager.h
//----------------------------------------------------------------------

// 

 

#ifndef __WHISPER_MANAGER_H__
#define __WHISPER_MANAGER_H__

#pragma warning(disable:4786)

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <map>
#include <string>
#include <list>

struct WHISPER_MESSAGE
{
	std::string msg;
	DWORD color;
};

//----------------------------------------------------------------------
// WhisperInfo
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class WhisperInfo {
	public :
		std::string					Name;
		std::list<WHISPER_MESSAGE>	Messages;

		int							TryingCount;	

	public :
		WhisperInfo()
		{
			TryingCount = 0;
		}

		void		SendToGameServer() const;
};

//----------------------------------------------------------------------
// WhisperManager
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class WhisperManager {
	public :
		typedef std::map<std::string, WhisperInfo*>		WHISPER_INFO_MAP;

	public :
		WhisperManager();
		~WhisperManager();

		//--------------------------------------------------------------
		// Release
		//--------------------------------------------------------------
		void		Release();

		//--------------------------------------------------------------
		// Message
		//--------------------------------------------------------------
		bool		HasWhisperMessage(const char* pName) const;
		const std::list<WHISPER_MESSAGE>*	GetWhisperMessages(const char* pName) const;

		void		SendWhisperMessage(const char* pName, const char* pMessage, DWORD color);

		void		AddWhisperMessage(const char* pName, const char* pMessage, DWORD color);		
		bool		RemoveWhisperMessage(const char* pName);
		void		TryToSendWhisperMessage(const char* pName);

		//--------------------------------------------------------------
		// Update
		//--------------------------------------------------------------
		void		Update();

	protected :
		//----------------------------------------------------------------------
		// Lock / Unlock
		//----------------------------------------------------------------------
		void		Lock()					{ EnterCriticalSection(&m_Lock); }
		void		Unlock()				{ LeaveCriticalSection(&m_Lock); }

	private :
		CRITICAL_SECTION		m_Lock;

		WHISPER_INFO_MAP		m_WhisperInfos;
};


extern WhisperManager*		g_pWhisperManager;


#endif

