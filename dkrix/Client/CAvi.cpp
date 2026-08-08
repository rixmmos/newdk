#include "Client_PCH.h"
#include "CAvi.h"

CAVI::CAVI()
{
	bEndFlag=0;
}

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#pragma comment(lib, "winmm.lib")

int CAVI::OpenMPG(HWND hwnd, LPSTR szName, int w, int h)
{
	this->hwnd=hwnd;

	MCI_DGV_OPEN_PARMS MciDgvOpenParms;
	//MciDgvOpenParms.lpstrDeviceType="AVIVideo";
	MciDgvOpenParms.lpstrDeviceType="MPEGVideo";
	MciDgvOpenParms.lpstrElementName=szName;

	if(mciSendCommand(NULL, MCI_OPEN,
					  MCI_OPEN_ELEMENT |
					  MCI_OPEN_TYPE |
					  MCI_DGV_OPEN_WS,
					  (DWORD_PTR)&MciDgvOpenParms))
	{
		dwID=NULL;
		return FALSE;
	}
	dwID = MciDgvOpenParms.wDeviceID;

	
	MCI_DGV_RECT_PARMS MciDgvRectParms;
	
	RECT Rect;
	GetClientRect(hwnd, &Rect);
	int x = (Rect.right - w) / 2;
	int y = (Rect.bottom - h) / 2;

	MciDgvRectParms.rc.left		= x;
	MciDgvRectParms.rc.top		= y;
	MciDgvRectParms.rc.right	= w;
	MciDgvRectParms.rc.bottom	= h;

	mciSendCommand(dwID, MCI_PUT,
				   MCI_ANIM_PUT_DESTINATION | MCI_DGV_RECT,
				   (DWORD_PTR)&MciDgvRectParms);

	MCI_DGV_WINDOW_PARMS MciDgvWindowParms;
	MciDgvWindowParms.hWnd = hwnd;

	mciSendCommand(dwID, MCI_WINDOW, MCI_ANIM_WINDOW_HWND,
				   (DWORD_PTR)&MciDgvWindowParms);

	return TRUE;
}

int CAVI::OpenAVI(HWND hwnd, LPSTR szName, int w, int h)
{
	this->hwnd=hwnd;

	MCI_DGV_OPEN_PARMS MciDgvOpenParms;
	MciDgvOpenParms.lpstrDeviceType="AVIVideo";
	//MciDgvOpenParms.lpstrDeviceType="MPEGVideo";
	MciDgvOpenParms.lpstrElementName=szName;

	if(mciSendCommand(NULL, MCI_OPEN,
					  MCI_OPEN_ELEMENT |
					  MCI_OPEN_TYPE |
					  MCI_DGV_OPEN_WS,
					  (DWORD_PTR)&MciDgvOpenParms))
	{
		dwID=NULL;
		return FALSE;
	}
	dwID = MciDgvOpenParms.wDeviceID;

	
	MCI_DGV_RECT_PARMS MciDgvRectParms;
	
	RECT Rect;
	GetClientRect(hwnd, &Rect);
	int x = (Rect.right - w) / 2;
	int y = (Rect.bottom - h) / 2;

	MciDgvRectParms.rc.left		= x;
	MciDgvRectParms.rc.top		= y;
	MciDgvRectParms.rc.right	= w;
	MciDgvRectParms.rc.bottom	= h;

	mciSendCommand(dwID, MCI_PUT,
				   MCI_ANIM_PUT_DESTINATION | MCI_DGV_RECT,
				   (DWORD_PTR)&MciDgvRectParms);

	MCI_DGV_WINDOW_PARMS MciDgvWindowParms;
	MciDgvWindowParms.hWnd = hwnd;

	mciSendCommand(dwID, MCI_WINDOW, MCI_ANIM_WINDOW_HWND,
				   (DWORD_PTR)&MciDgvWindowParms);

	return TRUE;
}

void CAVI::Close()
{
	if(dwID!=NULL)
	{
		MCI_PLAY_PARMS MciPlayParms;
		mciSendCommand(dwID, MCI_STOP, MCI_NOTIFY, (DWORD_PTR)&MciPlayParms);
		mciSendCommand(dwID, MCI_CLOSE, 0, NULL);
		dwID=NULL;
	}
}

int CAVI::Play()
{
	if(dwID==NULL) return FALSE;

	MCI_PLAY_PARMS MciPlayParms;
	MciPlayParms.dwCallback=(DWORD_PTR)hwnd;
	mciSendCommand(dwID, MCI_PLAY, MCI_NOTIFY,
				   (DWORD_PTR)&MciPlayParms);

	return TRUE;
}

void CAVI::Stop()
{
	if(dwID!=NULL)
	{
		MCI_PLAY_PARMS MciPlayParms;
		mciSendCommand(dwID, MCI_STOP, MCI_WAIT,(DWORD_PTR)&MciPlayParms);
	}
}

#else
// Non-Windows platforms: Video playback not implemented
int CAVI::OpenMPG(HWND hwnd, LPSTR szName, int w, int h) { return FALSE; }
int CAVI::OpenAVI(HWND hwnd, LPSTR szName, int w, int h) { return FALSE; }
void CAVI::Close() {}
int CAVI::Play() { return FALSE; }
void CAVI::Stop() {}
#endif
