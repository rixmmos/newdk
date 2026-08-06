//---------------------------------------------------------------------------
// MHelpDisplayer.h
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#ifndef __MHELPDISPLAYER_H__
#define __MHELPDISPLAYER_H__

#include "MHelpDef.h"

class MHelpDisplayer {
	public :
		MHelpDisplayer();
		~MHelpDisplayer();

		//-----------------------------------------------------
		// Output Help
		//-----------------------------------------------------
		void	OutputHelp(HELP_OUTPUT ho);
		
	protected :
		DWORD	m_DelayTime;		
};

extern MHelpDisplayer*		g_pHelpDisplayer;

#endif

