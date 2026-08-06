/*-----------------------------------------------------------------------------

	CDirectInput.h

	Direct Input Class library.

	2000.3.2. KJTINC

-----------------------------------------------------------------------------*/

/* Include Platform.h first for type definitions (outside header guard) */
#include "../basic/Platform.h"

#ifndef __CSDLINPUT_H__
#define __CSDLINPUT_H__

/* Include platform-independent key codes (SDL-based) */
#include "../../basic/InputCodes.h"

/* Forward declarations for DirectInput types (opaque pointers) */
/* The actual implementation uses SDL2 for all platforms */
struct IDirectInput;
struct IDirectInputDevice;

#define BUFFER_SIZE           32

/*-----------------------------------------------------------------------------
  Class Direct Input
-----------------------------------------------------------------------------*/
class CSDLInput
{
private:
	IDirectInput *          m_pDI;
	IDirectInputDevice *    m_pMouse;
	IDirectInputDevice *		m_pKeyboard;
	BOOL                    m_bSwapMouseButtons;
	int							m_mouse_info[3]; 
	BOOL						m_lb_held;
	BOOL						m_rb_held;
	BOOL						m_cb_held;

	BOOL					m_key[256];				
	static const char*		s_KeyName[256];

public:
	int							m_mouse_x;
	int							m_mouse_y;
	int							m_mouse_z;	// wheel action
	BOOL						m_lb_down;	// left button - down
	BOOL						m_rb_down;	// right button - down
	BOOL						m_cb_down;	// center button - down
	BOOL						m_lb_up;	// left button - up
	BOOL						m_rb_up;	// right button - up
	BOOL						m_cb_up;	// center button - up

	enum E_EXCLUSIVE 
	{
		EXCLUSIVE,
		NONEXCLUSIVE,
	};

	enum E_MOUSE_EVENT 
	{
		MOVE = 0,
		LEFTDOWN,
		LEFTUP,
		RIGHTDOWN,
		RIGHTUP,
		CENTERDOWN,
		CENTERUP,
		WHEELDOWN,
		WHEELUP,
	};

	enum E_KEYBOARD_EVENT
	{
		KEYUP,
		KEYDOWN,
	};

private:
	enum
	{
		FIRST_THRESHOLD,
		SECOND_THRESHOLD,
		ACCEL_LEVEL,
	};

	int							m_limit_x; 
	int							m_limit_y; 

	HRESULT	InitDI(HWND hWnd, HINSTANCE hInst, E_EXCLUSIVE ex);
	void		FreeDirectInput();
	void		OnMouseInput();
	void		OnKeyboardInput();
	void		SetMouseSpeed();
	int			GetMouseAcceleration(int value);

	void		(*m_fp_mouse_event_receiver)(E_MOUSE_EVENT event, int x, int y, int z);
	void		(*m_fp_keyboard_event_receiver)(E_KEYBOARD_EVENT event, DWORD scan_code);

public:
	CSDLInput();
	~CSDLInput();

	void		Clear();
	BOOL		Init(HWND hWnd, HINSTANCE hInst, E_EXCLUSIVE ex=EXCLUSIVE);
	HRESULT		SetAcquire(bool active_app);
	void		SetMouseEventReceiver(void (*fp_receiver)(E_MOUSE_EVENT, int, int, int));
	void		SetKeyboardEventReceiver(void (*fp_receiver)(E_KEYBOARD_EVENT, DWORD));
	void		SetMouseMoveLimit(int x, int y);
	void		SetMousePosition(int x, int y);
	void		UpdateInput();

	
	BOOL		KeyDown(DWORD dik)		{ return m_key[dik]; }

	//int 		GetMouseInfo1() const { return m_mouse_info[0]; }
	//int 		GetMouseInfo2() const { return m_mouse_info[1]; }
	//int 		GetMouseInfo3() const { return m_mouse_info[2]; }
	static const char*	GetKeyName(DWORD dik)	{ return s_KeyName[dik]; }
};


extern	CSDLInput*	g_pSDLInput;

#endif
