// u_window.cpp
#include "Client_PCH.h"
#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include "u_window.h"
#include <math.h>
#include "vs_ui.h"
#ifdef _WIN32
#include "CImm.h"
#endif
#include "UserOption.h"
#include <typeinfo>

#define STATCH_VALUE 10
#define HIDE_GAP	4
#define HIDE_SPEED	10
extern RECT g_GameRect;

static void TraceWindowManagerShow(const char* step, const Window* pWindow = NULL)
{
	(void)step;
	(void)pWindow;
}

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------
WindowManager *	gpC_window_manager; 

//-----------------------------------------------------------------------------
// Window
//
// 
//-----------------------------------------------------------------------------
Window::Window(int _x, int _y, int _w, int _h) : Rect(_x, _y, _w, _h)
{ 
	assert(gpC_window_manager != NULL);

	gpC_window_manager->Register(this);

	Init();

	//
	
	
	
	//
	//WindowEventReceiver(EVENT_WINDOW_CREATE);
}

Window::Window()
{
	//assert(gpC_window_manager != NULL);

	//gpC_window_manager->Register(this);

	Init();

	//WindowEventReceiver(EVENT_WINDOW_CREATE);
}

//-----------------------------------------------------------------------------
// ~Window
//
// 
//-----------------------------------------------------------------------------
Window::~Window()
{
	//assert(gpC_window_manager != NULL);

	//gpC_window_manager->Unregister(this);

	//WindowEventReceiver(EVENT_WINDOW_DESTROY);

//	gC_ci->.FinishImeRunning();
//	gC_ci->.ClearCurrentIMEComposition();

}

//-----------------------------------------------------------------------------
// ShowWidget
//

//

//-----------------------------------------------------------------------------
void Window::ShowWidget() const
{
	LineEditorVisual * data;
	for (int i=0; i < m_sdl_lev.Size(); i++)
		if (m_sdl_lev.Data(i, data))
			data->Show(); 
}

//-----------------------------------------------------------------------------
// Attach
//

//-----------------------------------------------------------------------------
void Window::Attach(LineEditorVisual * p_lev)
{
	if (p_lev == NULL)
		_Error(NULL_REF);

	m_sdl_lev.Add(p_lev); 
}

//-----------------------------------------------------------------------------
// InitAttributes
//
// 
//-----------------------------------------------------------------------------
void Window::InitAttributes()
{
	m_attributes.window_move = true;
	m_attributes.pin = false;
	m_attributes.topmost = false;
	m_attributes.keyboard_control = false;
	m_attributes.alpha = false;
	m_attributes.statch = true;
	m_attributes.autohide = ATTRIBUTES_HIDE_NOT;
}

//-----------------------------------------------------------------------------
// AttrTopmost
//

//-----------------------------------------------------------------------------
void Window::AttrTopmost(bool state)
{
	m_attributes.topmost = state;
}

//-----------------------------------------------------------------------------
// AttrKeyboardControl
//

//-----------------------------------------------------------------------------
void Window::AttrKeyboardControl(bool state)
{
	m_attributes.keyboard_control = state;
	gC_ci->FinishImeRunning();
	gC_ci->ClearCurrentIMEComposition();
//	gC_ci->.SetEngInput();
}

//-----------------------------------------------------------------------------
// AttrWindowMove
//

//-----------------------------------------------------------------------------
void Window::AttrWindowMove(bool new_state)
{ 
	m_attributes.window_move = new_state;

	
	m_bl_window_move_ready = false;
}

//-----------------------------------------------------------------------------
// AttrAlpha
//

//-----------------------------------------------------------------------------
void Window::AttrAlpha(bool new_state)
{ 
	m_attributes.alpha = new_state;
}

//-----------------------------------------------------------------------------
// AttrStatch
//

//-----------------------------------------------------------------------------
void Window::AttrStatch(bool new_state)
{ 
	m_attributes.statch = new_state;
}

//-----------------------------------------------------------------------------
// AttrAutoHide
//

//-----------------------------------------------------------------------------
void Window::AttrAutoHide(ATTRIBUTES_HIDE new_state)
{ 
	m_attributes.autohide = new_state;
}

//-----------------------------------------------------------------------------
// ProcessHide
//

//-----------------------------------------------------------------------------
void Window::ProcessHide(int gap, bool alpha_window)
{
	gap += HIDE_GAP;
	bool bl_move = false;
	if(GetAttributes()->autohide == ATTRIBUTES_HIDE_NOT)
	{
		if(x < 0)
		{
				x = 0;
				bl_move = true;
		}
		else if(x+w > g_GameRect.right)
		{
				x = g_GameRect.right-w;
				bl_move = true;
		}
		if(y < 0)
		{
				y = 0;
				bl_move = true;
		}
		else if(y+h > g_GameRect.bottom)
		{
				y = g_GameRect.bottom-h;
				bl_move = true;
		}
		
		if(bl_move)WindowEventReceiver(EVENT_WINDOW_MOVE);
		return;
	}

	if(IsPixel(gpC_mouse_pointer->GetX(), gpC_mouse_pointer->GetY()) && (gpC_window_manager->GetMouseFocusedWindow() == this || alpha_window))
	
	{
		if(x < 0)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				x = min(0, x+HIDE_SPEED);
			else
				x = 0;
			bl_move = true;
		}
		else if(x+w > g_GameRect.right)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				x = max(g_GameRect.right-w, x-HIDE_SPEED);
			else
				x = g_GameRect.right-w;
			bl_move = true;
		}
		if(y < 0)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				y = min(0, y+HIDE_SPEED);
			else
				y = 0;
			bl_move = true;
		}
		else if(y+h > g_GameRect.bottom)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				y = max(g_GameRect.bottom-h, y-HIDE_SPEED);
			else
				y = g_GameRect.bottom-h;
			bl_move = true;
		}
	}
	else
	
	{
		if(!(GetAttributes()->autohide == ATTRIBUTES_HIDE_HEIGHT
			&& (y <= 0 && y+h > gap || y+h >= g_GameRect.bottom && y < g_GameRect.bottom-gap))
			&& (x <= 0 && x+w > gap || x+w >= g_GameRect.right && x < g_GameRect.right-gap)
			&& y+h != gap && y != g_GameRect.bottom-gap)
		{
			if(x <= 0 && x+w > gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					x = max(-(w-gap), x-HIDE_SPEED);
				else
					x = -(w-gap);
			}
			else if(x+w >= g_GameRect.right && x < g_GameRect.right-gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					x = min(g_GameRect.right-gap, x+HIDE_SPEED);
				else
					x = g_GameRect.right-gap;
			}
			else if(x+w < gap) x = gap-w;
			else if(x > g_GameRect.right-gap)x = g_GameRect.right-gap;
			
			if(y < 0)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					y = min(0, y+HIDE_SPEED);
				else
					y = 0;
			}
			else if(y+h > g_GameRect.bottom)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					y = max(g_GameRect.bottom-h, y-HIDE_SPEED);
				else
					y = g_GameRect.bottom-h;
			}

			bl_move = true;
		}
		else if(x+w != gap && x != g_GameRect.right-gap)
		{
			if(y <= 0 && y+h > gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					y = max(-(h-gap), y-HIDE_SPEED);
				else
					y = -(h-gap);
			}
			else if(y+h >= g_GameRect.bottom && y < g_GameRect.bottom-gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					y = min(g_GameRect.bottom-gap, y+HIDE_SPEED);
				else
					y = g_GameRect.bottom-gap;
			}
			else if(y+h < gap) y = gap-h;
			else if(y > g_GameRect.bottom-gap)y = g_GameRect.bottom-gap;
			
			if(x < 0)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					x = min(0, x+HIDE_SPEED);
				else
					x = 0;
			}
			else if(x+w > g_GameRect.right)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					x = max(g_GameRect.right-w, x-HIDE_SPEED);
				else
					x = g_GameRect.right-w;
			}
			bl_move = true;
		}
	}
	if(bl_move)WindowEventReceiver(EVENT_WINDOW_MOVE);

}

//-----------------------------------------------------------------------------
// AttrPin
//




//-----------------------------------------------------------------------------
void Window::AttrPin(bool new_state)
{
	if (gpC_window_manager->GetShowState(this) == true)
	{
		
		
		gpC_window_manager->DisappearWindow(this);

		m_attributes.pin = new_state;
		gpC_window_manager->AppearWindow(this);
	}
	else
	{
		
		
		m_attributes.pin = new_state;
	}
}

//-----------------------------------------------------------------------------
// ClearInputState
//

//-----------------------------------------------------------------------------
void Window::ClearInputState()
{
	MoveOk();
	m_u_mouse_state = MOUSE_NOSTATE;
}

//-----------------------------------------------------------------------------
// Init
//
// 
//-----------------------------------------------------------------------------
void Window::Init()
{
	InitAttributes();

	m_bl_window_move_ready = false;
	m_u_mouse_state = MOUSE_NOSTATE;
}

//-----------------------------------------------------------------------------
// GetMouseInputState
//
// 
//-----------------------------------------------------------------------------
MOUSE_STATE Window::GetMouseInputState()
{
	MOUSE_STATE state = m_u_mouse_state;
	m_u_mouse_state = MOUSE_NOSTATE;

	return state;
}

//-----------------------------------------------------------------------------
// MoveReady
//

//-----------------------------------------------------------------------------
void Window::MoveReady()
{
	if (m_attributes.window_move == true)
	{
		m_bl_window_move_ready = true;
	}
}

//-----------------------------------------------------------------------------
// MoveOk
//

//-----------------------------------------------------------------------------
void Window::MoveOk()
{
	m_bl_window_move_ready = false;
}

//-----------------------------------------------------------------------------
// SetOrigin
//


//

//-----------------------------------------------------------------------------
void Window::SetOrigin(int ox, int oy)
{
	m_origin.Set(ox, oy);
}

//-----------------------------------------------------------------------------
// Move
//


//-----------------------------------------------------------------------------
bool Window::Move(int new_ox, int new_oy)
{
	if (m_bl_window_move_ready)
	{
		x = new_ox-m_origin.x;
		y = new_oy-m_origin.y;

		if(GetAttributes()->statch)
		{
			if(x < STATCH_VALUE && x > -STATCH_VALUE)
			{
				x = 0;
			}
			else if(x+w > g_GameRect.right-STATCH_VALUE && x+w < g_GameRect.right+STATCH_VALUE)
			{
				x = g_GameRect.right-w;
			}
			
			if(y < STATCH_VALUE && y > -STATCH_VALUE)
			{
				y = 0;
			}
			else if(y+h > g_GameRect.bottom-STATCH_VALUE && y+h < g_GameRect.bottom+STATCH_VALUE)
			{
				y = g_GameRect.bottom-h;
			}
		}
//		x += new_ox - m_origin.x;
//		y += new_oy - m_origin.y;

//		m_origin.Set(new_ox, new_oy);

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// MouseControl
//


//-----------------------------------------------------------------------------
bool Window::MouseControl(UINT message, int _x, int _y)
{
	switch (message)
	{
		case M_MOVING:
			if (Move(_x, _y) == true)
			{

				
				m_u_mouse_state = MOUSE_MOVE;
			}
			break;

		case M_LEFTBUTTON_UP:
			MoveOk();
			break;
	}

	return true;
}

//-----------------------------------------------------------------------------
// KeyboardControl
//

//-----------------------------------------------------------------------------
void Window::KeyboardControl(UINT message, UINT key, long extra)
{
#ifdef PLATFORM_WINDOWS
	gC_ci->IME_MessageProcessor(message, key, extra);
#else
	// On macOS/SDL2, the CI::IME_MessageProcessor is a stub
	// Windows that need keyboard input should override this method
	(void)message;
	(void)key;
	(void)extra;
#endif
}

//-----------------------------------------------------------------------------
// WindowManager
//
// 
//-----------------------------------------------------------------------------
WindowManager::WindowManager()
{
	m_show_list.clear();
	m_show_list_pinned_window.clear();

	m_pC_mouse_click_window = NULL;
	m_pC_topmost_window = NULL;
	m_pC_keydown_window = NULL;
	m_pC_keyboard_control_window = NULL;
	m_pC_mouse_focused_window = NULL;
	m_pC_pushed_window = NULL;
	m_old_origin_x = 0;
	m_old_origin_y = 0;
	m_res_x = 0;
	m_res_y = 0;
	m_fp_handler_acquire_mousefocus = NULL;
	m_fp_handler_unacquire_mousefocus = NULL;
	m_fp_handler_acquire_disappear = NULL;
}

//-----------------------------------------------------------------------------
// ~WindowManager
//
// 
//-----------------------------------------------------------------------------
WindowManager::~WindowManager()
{

}

//-----------------------------------------------------------------------------
// SetKeyboardControlWindow
//

//-----------------------------------------------------------------------------
void WindowManager::SetKeyboardControlWindow(Window * p_window)
{
	if (p_window != NULL)
	{
		if (p_window->GetAttributes()->keyboard_control == true)
			m_pC_keyboard_control_window = p_window;
	}
	else
		m_pC_keyboard_control_window = NULL;
}

//-----------------------------------------------------------------------------
// SetNextKeyboardControlWindow
//
// 
//-----------------------------------------------------------------------------
void WindowManager::SetNextKeyboardControlWindow()
{
	m_pC_keyboard_control_window = NULL;

	if (m_pC_topmost_window == NULL)
	{
		List::iterator itr;

		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if ((*itr)->GetAttributes()->keyboard_control == true)
			{
				m_pC_keyboard_control_window = *itr;
				return;
			}

			itr++;
		}

		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if ((*itr)->GetAttributes()->keyboard_control == true)
			{
				m_pC_keyboard_control_window = *itr;
				return;
			}

			itr++;
		}
	}
}

//-----------------------------------------------------------------------------
// SetNextTopmostWindow
//

//-----------------------------------------------------------------------------
void WindowManager::SetNextTopmostWindow()
{
	

	List::iterator itr;

	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		if ((*itr)->GetAttributes()->topmost == true)
		{
			m_pC_topmost_window = *itr;
			return;
		}

		itr++;
	}

	m_pC_topmost_window = NULL;
}

//-----------------------------------------------------------------------------
// CancelPushStateOfCurrentPushedWindow
//


//

//-----------------------------------------------------------------------------
bool WindowManager::CancelPushStateOfCurrentPushedWindow()
{
	if (m_pC_pushed_window != NULL)
	{
		m_pC_pushed_window->CancelPushState();
		m_pC_pushed_window = NULL;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// ShowedWindowSize
//
// 
//-----------------------------------------------------------------------------
int WindowManager::ShowedWindowSize() const 
{ 
	return m_show_list.size()+m_show_list_pinned_window.size(); 
}

//-----------------------------------------------------------------------------
// GetSequence
//


//-----------------------------------------------------------------------------
int WindowManager::GetSequence(Window * p_this_window)
{
	if (m_pC_topmost_window != NULL && m_pC_topmost_window == p_this_window)
		return 0;

	int sequence = 0;
	List::iterator itr;

	itr = m_show_list_pinned_window.begin();
	while (itr != m_show_list_pinned_window.end())
	{
		if (*itr == p_this_window)
		{
			return sequence;
		}

		itr++;
		sequence++;
	}
	
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		if (*itr == p_this_window)
		{
			return sequence;
		}

		itr++;
		sequence++;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// SendMouseMoveMessageToMouseFocusedWindow
//
// 
//-----------------------------------------------------------------------------
void WindowManager::SendMouseMoveMessageToMouseFocusedWindow()
{
	if (m_pC_mouse_focused_window != NULL)
		m_pC_mouse_focused_window->MouseControl(M_MOVING, m_current_mouse_x, m_current_mouse_y);
}

//-----------------------------------------------------------------------------
// GetFirstPriorityWindow
//


//-----------------------------------------------------------------------------
Window * WindowManager::GetFirstPriorityWindow() const
{
	if (m_pC_topmost_window != NULL)
		return m_pC_topmost_window;

	if (m_show_list_pinned_window.empty() == false)
	{
		return m_show_list_pinned_window.front();
	}
	else
	{
		if (m_show_list.empty() == false)
			return m_show_list.front();
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// GetMovingWindow
//


//


//-----------------------------------------------------------------------------
Window * WindowManager::GetMovingWindow() const
{
	Window * p_window;

	if (m_show_list_pinned_window.empty() == false)
	{
		p_window = m_show_list_pinned_window.front();
		if (p_window->Moving() == true)
			return p_window;
	}

	if (m_show_list.empty() == false)
	{
		p_window = m_show_list.front();
		if (p_window->Moving() == true)
			return p_window;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// FirstPriority
//


//-----------------------------------------------------------------------------
void WindowManager::FirstPriority(Window * p_this_window)
{
	assert(p_this_window != NULL);

	// failed?
	if (p_this_window == NULL)
		 return;

	List::iterator itr;
	
	if (p_this_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list_pinned_window.erase(itr);
				m_show_list_pinned_window.push_front(p_this_window);
				break;
			}

			itr++;
		}
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list.erase(itr);
				m_show_list.push_front(p_this_window);
				break;
			}

			itr++;
		}	
	}
}

//-----------------------------------------------------------------------------
// Show
//

//-----------------------------------------------------------------------------
void WindowManager::Show()
{
	TraceWindowManagerShow("WindowManager::Show begin");
	List::reverse_iterator itr;

	itr = m_show_list.rbegin();
	while (itr != m_show_list.rend())
	{
		Window *pWindow = *itr;
		if (pWindow == NULL)
		{
			itr++;
			continue;
		}

		if (pWindow->GetAttributes()->topmost == false)
		{
			TraceWindowManagerShow("WindowManager::Show before normal window", pWindow);
			pWindow->Show();
			TraceWindowManagerShow("WindowManager::Show after normal window", pWindow);
		}

		itr++;
	}

	
	itr = m_show_list_pinned_window.rbegin();
	while (itr != m_show_list_pinned_window.rend())
	{
		Window *pWindow = *itr;
		if (pWindow == NULL)
		{
			itr++;
			continue;
		}
//		(*itr)->Show();
		TraceWindowManagerShow("WindowManager::Show before pinned window", pWindow);
		pWindow->Show();
		TraceWindowManagerShow("WindowManager::Show after pinned window", pWindow);

		itr++;
	}

	// show topmost
	itr = m_show_list.rbegin();
	while (itr != m_show_list.rend())
	{
		Window *pWindow = *itr;
		if (pWindow == NULL)
		{
			itr++;
			continue;
		}

		if (pWindow->GetAttributes()->topmost == true)
		{
			TraceWindowManagerShow("WindowManager::Show before topmost window", pWindow);
			pWindow->Show();
			TraceWindowManagerShow("WindowManager::Show after topmost window", pWindow);
		}

		itr++;
	}
	TraceWindowManagerShow("WindowManager::Show end");
}

//-----------------------------------------------------------------------------
// AppearWindow
//


//

//-----------------------------------------------------------------------------
void WindowManager::AppearWindow(Window * p_this_window)
{
	assert(p_this_window != NULL);

	// failed?
	if (p_this_window == NULL ||
		 AlreadyRegistered(p_this_window) == false)
		 return;

	Window * p_prev_first_window = GetFirstPriorityWindow();

	List::iterator itr;

	//
	
	//

	if (p_this_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list_pinned_window.erase(itr);
				break;
			}

			itr++;
		}

		m_show_list_pinned_window.push_front(p_this_window);
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list.erase(itr);
				break;
			}

			itr++;
		}

		m_show_list.push_front(p_this_window);
	}

	//
	// cancel previous focused Window focus & focus new Window
	//
	Window * pC_prev_focused_window = m_pC_mouse_focused_window;
	SetMouseMoveFocusedWindow();
	if (pC_prev_focused_window != m_pC_mouse_focused_window)
	{
		if (pC_prev_focused_window != NULL)
		{
			pC_prev_focused_window->UnacquireMouseFocus();
			RunUnacquireMouseFocusHandler();
		}
		if (m_pC_mouse_focused_window != NULL)
		{
			m_pC_mouse_focused_window->AcquireMouseFocus();
			RunAcquireMouseFocusHandler();
		}
	}

	SendMouseMoveMessageToMouseFocusedWindow();

	SetNextTopmostWindow();

	// new first sequence Window detection?
	Window * p_first_window = GetFirstPriorityWindow();
	if (p_first_window != NULL && p_first_window != p_prev_first_window)
	{
		p_first_window->AcquireFirstSequence();
		SetKeyboardControlWindow(p_first_window);
	}
}

//-----------------------------------------------------------------------------
// DisappearWindow
//


//-----------------------------------------------------------------------------
bool WindowManager::DisappearWindow(Window * p_this_window)
{
	assert(p_this_window != NULL);

	if (p_this_window == NULL)
		return false;

	if (m_pC_keydown_window==p_this_window)
	{
		m_pC_keydown_window = NULL;
	}
	if (m_pC_topmost_window == p_this_window)
	{
		m_pC_topmost_window = NULL;
	}

	Window * p_prev_first_window = GetFirstPriorityWindow();

	bool disappear_result = false;
	List::iterator itr;

	if (p_this_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_this_window)
			{
				p_this_window->ClearInputState();

				m_show_list_pinned_window.erase(itr);
				disappear_result = true;
				break;
			}

			itr++;
		}	
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_this_window)
			{
				p_this_window->ClearInputState();

				m_show_list.erase(itr);
				disappear_result = true;
				break;
			}

			itr++;
		}	
	}

	if (disappear_result == true)
	{
		// cancel previous focused Window focus & focus new Window
		Window * pC_prev_focused_window = m_pC_mouse_focused_window;
		SetMouseMoveFocusedWindow();
		if (pC_prev_focused_window != m_pC_mouse_focused_window)
		{
			if (pC_prev_focused_window != NULL)
			{
				pC_prev_focused_window->UnacquireMouseFocus();
				RunUnacquireMouseFocusHandler();
			}
			if (m_pC_mouse_focused_window != NULL)
			{
				m_pC_mouse_focused_window->AcquireMouseFocus();
				RunAcquireMouseFocusHandler();
			}
		}

		p_this_window->AcquireDisappear();
		RunAcquireDisappearHandler();

		SendMouseMoveMessageToMouseFocusedWindow();

		if (p_this_window == m_pC_pushed_window)
			m_pC_pushed_window = NULL;

		SetNextTopmostWindow();

		// new first sequence Window detection?
		Window * p_first_window = GetFirstPriorityWindow();
		if (p_first_window != NULL && p_first_window != p_prev_first_window)
			p_first_window->AcquireFirstSequence();

		//
		
		
		//
		SetNextKeyboardControlWindow();
	}

	return disappear_result;
}

//-----------------------------------------------------------------------------
// SetMouseMoveFocusedWindow
//


//-----------------------------------------------------------------------------
void WindowManager::SetMouseMoveFocusedWindow()
{
	List::iterator itr, endItr;

	// topmost first
	
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == true)
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				m_pC_mouse_focused_window = p_searched_window;
				return;
			}

		itr++;
	}

	itr = m_show_list_pinned_window.begin();
	endItr = m_show_list_pinned_window.end();
	while (itr != endItr)
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
		{
			m_pC_mouse_focused_window = p_searched_window;
			return;
		}

		itr++;
	}
	
	itr = m_show_list.begin();

	endItr = m_show_list.end();
	while (itr != endItr)
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == false)
		{
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				m_pC_mouse_focused_window = p_searched_window;
				return;
			}
		}			
		itr++;
	}

	m_pC_mouse_focused_window = NULL;
}

//-----------------------------------------------------------------------------
// GetFocusedWindow
//


//-----------------------------------------------------------------------------
Window* WindowManager::GetFocusedWindow(int x, int y)
{
	List::iterator itr;

	// topmost first
	
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == true)
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				return p_searched_window;
			}

		itr++;
	}

	itr = m_show_list_pinned_window.begin();
	while (itr != m_show_list_pinned_window.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
		{
			return p_searched_window;
		}

		itr++;
	}
	
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == false)
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				return p_searched_window;
			}

		itr++;
	}

	return NULL;
}
//-----------------------------------------------------------------------------
// MouseControl
//


//



//-----------------------------------------------------------------------------
bool WindowManager::MouseControl(UINT message, int x, int y)
{
	m_current_mouse_x = x;
	m_current_mouse_y = y;

	//
	// change move focused Window
	//
	if (message == M_MOVING)
	{
		
#ifdef _WIN32
		if (gpC_Imm && m_pC_mouse_focused_window != NULL && m_pC_mouse_focused_window->Moving())
			gpC_Imm->ForceUI(CImm::FORCE_UI_DRAG);
#endif

		if (m_pC_mouse_focused_window == NULL || m_pC_mouse_focused_window->Moving() == false)
		{
			//
			// Acquire/Unacquire mouse focus
			//
			Window * pC_prev_focused_window = m_pC_mouse_focused_window;

			SetMouseMoveFocusedWindow();

			if (pC_prev_focused_window != m_pC_mouse_focused_window)
			{
#ifdef _WIN32
				if(gpC_Imm)
					gpC_Imm->ForceUI(CImm::FORCE_UI_WINDOW);
#endif
				if (pC_prev_focused_window != NULL)
				{
					pC_prev_focused_window->UnacquireMouseFocus();
					RunUnacquireMouseFocusHandler();
				}
				if (m_pC_mouse_focused_window != NULL)
				{
					m_pC_mouse_focused_window->AcquireMouseFocus();
					RunAcquireMouseFocusHandler();
				}
			}
		}
	}

	//
	// change Window priority
	
	//
	if (message == M_LEFTBUTTON_DOWN || message == M_RIGHTBUTTON_DOWN)
	{
		if (m_pC_mouse_focused_window != NULL)
		{
			if (m_pC_topmost_window == NULL)
			{
				Window * p_prev_first_window = GetFirstPriorityWindow();

				FirstPriority(m_pC_mouse_focused_window);
				
				Window * p_first_window = GetFirstPriorityWindow();
				if (p_prev_first_window != p_first_window)
				{
					p_first_window->AcquireFirstSequence();
					SetKeyboardControlWindow(p_first_window);
				}

				// acquire mouse click
				if (m_pC_mouse_click_window != m_pC_mouse_focused_window)
				{
					m_pC_mouse_click_window = m_pC_mouse_focused_window;
					m_pC_mouse_click_window->AcquireMouseClick();
					SetKeyboardControlWindow(m_pC_mouse_click_window);
				}
			}

			// set pushed Window
			m_pC_pushed_window = m_pC_mouse_focused_window;
		}
	}

	
	if (m_pC_mouse_focused_window != NULL)
	{
		if (m_pC_topmost_window != NULL)
		{
			if (m_pC_topmost_window == m_pC_mouse_focused_window)
				m_pC_topmost_window->MouseControl(message, x, y);
		}
		else
		{
			m_pC_mouse_focused_window->MouseControl(message, x, y);

			
			if(message == M_MOVING && m_pC_mouse_focused_window->Moving())
			{
				if(m_pC_mouse_focused_window->GetAttributes()->statch)
				{
					List::const_iterator itr;
					
					for(int k = 0; k < 2; k++)
					{
						if(k == 0)
							itr = m_show_list_pinned_window.begin();
						else if(k == 1)
							itr = m_show_list.begin();
						
						while (k ==0 && itr != m_show_list_pinned_window.end() || k ==1 && itr != m_show_list.end())
						{
							//						m_pC_mouse_focused_window->UnStatch();
							Window * p_searched_window = (*itr);
							
							bool bl_statch = false;
							
							
							if(m_pC_mouse_focused_window->y < p_searched_window->y+p_searched_window->h &&
								m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h > p_searched_window->y)
							{
								
								if (m_pC_mouse_focused_window->x > p_searched_window->x+p_searched_window->w-STATCH_VALUE &&
									m_pC_mouse_focused_window->x < p_searched_window->x+p_searched_window->w+STATCH_VALUE)
								{
									m_pC_mouse_focused_window->x = p_searched_window->x+p_searched_window->w;
									bl_statch = true;
								}
								else 
									if (m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w > p_searched_window->x-STATCH_VALUE &&
										m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w < p_searched_window->x+STATCH_VALUE)
									{
										m_pC_mouse_focused_window->x = p_searched_window->x-m_pC_mouse_focused_window->w;
										bl_statch = true;
									}
									
									if(bl_statch)	
									{
										
										if(m_pC_mouse_focused_window->y > p_searched_window->y-STATCH_VALUE &&
											m_pC_mouse_focused_window->y < p_searched_window->y+STATCH_VALUE)
										{
											m_pC_mouse_focused_window->y = p_searched_window->y;
										}
										else	
											if(m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h > p_searched_window->y+p_searched_window->h-STATCH_VALUE &&
												m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h < p_searched_window->y+p_searched_window->h+STATCH_VALUE)
											{
												m_pC_mouse_focused_window->y = p_searched_window->y+p_searched_window->h-m_pC_mouse_focused_window->h;
											}
									}
									
							}
							
							bl_statch = false;
							
							if (m_pC_mouse_focused_window->x < p_searched_window->x+p_searched_window->w &&
								m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w > p_searched_window->x)
							{
								
								if (m_pC_mouse_focused_window->y > p_searched_window->y+p_searched_window->h-STATCH_VALUE &&
									m_pC_mouse_focused_window->y < p_searched_window->y+p_searched_window->h+STATCH_VALUE)
								{
									m_pC_mouse_focused_window->y = p_searched_window->y+p_searched_window->h;
									bl_statch = true;
								}
								else 
									if (m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h > p_searched_window->y-STATCH_VALUE &&
										m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h < p_searched_window->y+STATCH_VALUE)
									{
										m_pC_mouse_focused_window->y = p_searched_window->y-m_pC_mouse_focused_window->h;
										bl_statch = true;
									}
							}
							if(bl_statch == true)	
							{
								
								if(m_pC_mouse_focused_window->x > p_searched_window->x-STATCH_VALUE &&
									m_pC_mouse_focused_window->x < p_searched_window->x+STATCH_VALUE)
								{
									m_pC_mouse_focused_window->x = p_searched_window->x;
								}
								else	
									if(m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w > p_searched_window->x+p_searched_window->w-STATCH_VALUE &&
										m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w < p_searched_window->x+p_searched_window->w+STATCH_VALUE)
									{
										m_pC_mouse_focused_window->x = p_searched_window->x+p_searched_window->w-m_pC_mouse_focused_window->w;
									}
							}
							
							itr++;
						}
					}
				}

				// event
				m_pC_mouse_focused_window->WindowEventReceiver(Window::EVENT_WINDOW_MOVE);
				
			
			}
		}
	}

	//
	// clear mouse push state.
	//
	if (message == M_LEFTBUTTON_UP ||
		 message == M_RIGHTBUTTON_UP)
	{
		//
		
		
		//
		
		

		
		//if (mouse_move_focus_change_window_priority?)
		//{
		//
		//}
		//else
		if (m_pC_pushed_window != NULL)
		{
			if (m_pC_pushed_window != m_pC_mouse_focused_window)
				m_pC_pushed_window->MouseControl(message, x, y);

			m_pC_pushed_window = NULL;
		}

	}

	return (m_pC_mouse_focused_window != NULL);
}

#include "VS_UI.h"

#ifndef WM_TEXTINPUT
#define WM_TEXTINPUT (WM_USER + 0x500)
#endif

#ifndef WM_TEXTEDITING
#define WM_TEXTEDITING (WM_USER + 0x501)
#endif

//-----------------------------------------------------------------------------
// KeyboardControl
//


//-----------------------------------------------------------------------------
void WindowManager::KeyboardControl(UINT message, UINT key, long extra)
{
//	static Window * m_pC_keydown_window;

	if (message == WM_KEYDOWN || message == WM_CHAR
#if defined(USE_SDL_BACKEND) || defined(SPRITELIB_BACKEND_SDL) || defined(PLATFORM_MACOS)
	    || message == WM_TEXTINPUT || message == WM_TEXTEDITING
#endif
		)// || message == WM_IME_STARTCOMPOSITION)
	{
		m_pC_keydown_window = NULL;
		if (m_pC_topmost_window != NULL)// && m_pC_topmost_window->GetAttributes()->keyboard_control == true)
			m_pC_keydown_window = m_pC_topmost_window;
		else if (m_pC_keyboard_control_window != NULL)
			m_pC_keydown_window = m_pC_keyboard_control_window;
	}

	//
	
	//			 	 (2) keyboard control Window
	//
	if (m_pC_topmost_window != NULL)
	{
//		if (message == WM_KEYDOWN)
//		{
//			if (m_pC_topmost_window != m_pC_keydown_window)
//				return;
//		}

		if (m_pC_topmost_window == m_pC_keydown_window)
		{

		//if(key != VK_ESCAPE && gC_vs_ui.IsGameMode() || !gC_vs_ui.IsGameMode())
		if(m_pC_topmost_window->GetAttributes()->keyboard_control == true || gC_vs_ui.IsEmptyChatting() && key == VK_RETURN || !gC_vs_ui.IsGameMode() && key == VK_ESCAPE)
		{
			m_pC_topmost_window->KeyboardControl(message, key, extra);
			return;
		}

		
		if(!gC_vs_ui.IsGameMode())
			return;
		}
	}

	
	if (m_pC_keyboard_control_window != NULL && m_pC_keyboard_control_window != m_pC_topmost_window)
		m_pC_keyboard_control_window->KeyboardControl(message, key, extra);

//	if (m_pC_keydown_window != NULL)
//	{
//		if (message == WM_KEYDOWN)
//		{
//			if(m_pC_keyboard_control_window != m_pC_keydown_window && 
//				m_pC_keydown_window->GetAttributes()->keyboard_control == true ||
//				m_pC_keyboard_control_window == m_pC_topmost_window ||
//				m_pC_keyboard_control_window == NULL)
//				return;
//			if (m_pC_keyboard_control_window != m_pC_keydown_window)
//				return;
//		}

//		if(
//			m_pC_topmost_window != NULL && 
//			m_pC_topmost_window->GetAttributes()->keyboard_control == true ||
//			m_pC_keyboard_control_window == m_pC_topmost_window ||
//			m_pC_keyboard_control_window == NULL
//			)
//			return;

//		if (m_pC_keyboard_control_window != m_pC_keydown_window)
//			return;

//	}
}

//-----------------------------------------------------------------------------
// AlreadyRegistered
//

//-----------------------------------------------------------------------------
bool WindowManager::AlreadyRegistered(Window * p_window) const
{
	return Find(p_window);
}

//-----------------------------------------------------------------------------
// GetShowState
//
// 
//-----------------------------------------------------------------------------
bool WindowManager::GetShowState(Window * p_window) const
{
	if (p_window == NULL)
		return false;

	List::const_iterator itr;

	if (p_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_window)
				return true;

			itr++;
		}
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_window)
				return true;

			itr++;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Register
//


//-----------------------------------------------------------------------------
void WindowManager::Register(Window * p_window)
{
	if (p_window == NULL)
		_Error(NULL_REF);

	Add(p_window);
}

//-----------------------------------------------------------------------------
// Unregister
//

//-----------------------------------------------------------------------------
void WindowManager::Unregister(Window * p_window)
{
	if (p_window == NULL)
		_Error(NULL_REF);

	// Keep the visible lists clean even if the registration list is already
	// out of sync. Stale windows here can survive into MODE_GAME and hang draw.
	DisappearWindow(p_window);
	Delete(p_window);

	if (p_window == m_pC_pushed_window)
		m_pC_pushed_window = NULL;
}

//-----------------------------------------------------------------------------
// Process
//
//
//-----------------------------------------------------------------------------
void WindowManager::Process()
{
}

//-----------------------------------------------------------------------------
// g_RegisterWindow
//

//-----------------------------------------------------------------------------
void g_RegisterWindow(Window * p_window)
{
	if (gpC_window_manager == NULL)
		_ErrorStr("WindowManager not initialized.");
	if (p_window == NULL)
		_Error(NULL_REF);

	gpC_window_manager->Register(p_window);
}

//-----------------------------------------------------------------------------
// g_UnregisterWindow
//

//-----------------------------------------------------------------------------
void g_UnregisterWindow(Window * p_window)
{
	if (gpC_window_manager == NULL)
		_ErrorStr("WindowManager not initialized.");
	if (p_window == NULL)
		_Error(NULL_REF);

	gpC_window_manager->Unregister(p_window);
}	
