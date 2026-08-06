//-----------------------------------------------------------------------------
// CWaitPacketUpdate.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#ifndef	__CWAITPACKETUPDATE_H__
#define	__CWAITPACKETUPDATE_H__

#include "WinLib/CWinUpdate.h"

class CWaitPacketUpdate : public CWinUpdate {
	public :
		CWaitPacketUpdate()	{ m_DelayLimit = 0xFFFFFFFF; }  // Will be set by SetDelay()
		~CWaitPacketUpdate() {}

		
		void		Init();

		
		void		SetDelay(DWORD delay);		

		// update
		void		Update();		

	protected :				
		DWORD		m_DelayLimit;	
};


extern CWaitPacketUpdate*	g_pCWaitPacketUpdate;

#endif
