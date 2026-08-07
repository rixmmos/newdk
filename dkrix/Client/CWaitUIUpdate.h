//-----------------------------------------------------------------------------
// CWaitUIUpdate.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#ifndef	__CWAITUIUPDATE_H__
#define	__CWaitUIUPDATE_H__

#include "WinLib/CWinUpdate.h"
#include "InputService.h"

class CWaitUIUpdate : public CWinUpdate {
	public :
		CWaitUIUpdate() {}
		~CWaitUIUpdate() {}

		void		Init();

		void		Update();		

	protected :
		void		ProcessInput();
		void		UpdateDraw();
		static void DXMouseEvent(InputManager::E_MOUSE_EVENT event, int x, int y, int z);
		static void	DXKeyboardEvent(InputManager::E_KEYBOARD_EVENT, DWORD);
		static void	SDLTextInputEvent(const char* text, int* window_coords);
		static void	SDLTextEditingEvent(const char* text, int start, int length, int* window_coords);
};

extern CWaitUIUpdate*		g_pCWaitUIUpdate;

#endif
