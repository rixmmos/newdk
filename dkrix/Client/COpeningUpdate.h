//-----------------------------------------------------------------------------
// COpeningUpdate.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#ifndef	__COPENINGUPDATE_H__
#define	__COPENINGUPDATE_H__

#include "WinLib/CWinUpdate.h"

class COpeningUpdate : public CWinUpdate {
	public :
		COpeningUpdate() {}
		~COpeningUpdate() {}

		void		Init();

		void		PlayMPG(char* filename);				
		void		Update();		

	protected :
		void		ProcessInput();		
};

extern COpeningUpdate*	g_pCOpeningUpdate;

#endif
