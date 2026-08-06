//-----------------------------------------------------------------------------
// CWinUpdate.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#ifndef	__CWINUPDATE_H__
#define	__CWINUPDATE_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <MMSystem.h>
#else
#include "../../basic/Platform.h"
#endif

class CWinUpdate {
	public :
		CWinUpdate();
		virtual ~CWinUpdate();

		//-------------------------------------------------------
		// Init
		//-------------------------------------------------------
		virtual void	Init() {}

		//-------------------------------------------------------
		// Update
		//-------------------------------------------------------
		virtual void	Update();

		//-------------------------------------------------------
		// Current Time
		//-------------------------------------------------------
		static void		SetCurrentTime()	{ m_CurrentTime	= timeGetTime(); }
		static DWORD	GetCurrentTime()	{ return m_CurrentTime; }

		//-------------------------------------------------------
		// Update Delay
		//-------------------------------------------------------
		void			SetDelay(DWORD delay)	{ m_UpdateDelay = delay; }
		DWORD			GetDelay() const		{ return m_UpdateDelay; }

		//-------------------------------------------------------
		// Last Time
		//-------------------------------------------------------
		DWORD			GetLastTime()		{ return m_LastTime; }
		

	private :
		static DWORD	m_CurrentTime;		

	protected :
		DWORD			m_LastTime;			
		DWORD			m_UpdateDelay;		
};

#endif		
