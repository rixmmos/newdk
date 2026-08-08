#include "Client_PCH.h"
#define __NPROTECT__
// EXECryptor include removed (SDL2) - Copy protection no longer needed
 
#include "APICheck.h"
APICheck _APICheck;
/* ***************************************** */
//#undef __NPROTECT__
//#define NO_GAMEGUARD
//#define __NPROTECT_OLD_VERSION__



//-----------------------------------------------------------------------------
// Client.cpp
//-----------------------------------------------------------------------------

#pragma warning(disable:4786)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 /* version 5.0 */
#endif /* !_WIN32_WINNT */

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#endif

#include "DebugInfo.h"
#ifndef _DEBUG
#include "CrashReport.h"
#endif

//add by zdj
#undef __NPROTECT__
#define NO_GAMEGUARD

#if defined(OUTPUT_DEBUG) || defined(_DEBUG) || defined(__DEBUG_OUTPUT__)
//#undef __NPROTECT__
//#define NO_GAMEGUARD
#endif

//
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <stdio.h>
#include <stdarg.h>
#ifdef PLATFORM_WINDOWS
#include <io.h>
#include <direct.h>
#include <crtdbg.h>
#include <process.h>
#else
#include <unistd.h>
#endif
#include <time.h>
#include <string>
#include <sys/stat.h>
#include "Client.h"
#include "GameObject.h"
#include "VS_UI.h"
#include "MMusic.h"
#include "UIDialog.h"
#include "Updater/UpdateManager.h"
#ifdef PLATFORM_WINDOWS
#include <Commctrl.h>
#endif
#include "AppendPatchInfo.h"
#include "EffectSpriteTypeDef.h"
//#include "MFileDef.h"
#include "Packet/Properties.h"
#include "ServerInfo.h"
#include "MTestDef.h"
#include "resource.h"
#include "UserInformation.h"
#ifdef SPRITELIB_BACKEND_SDL
#include <SDL.h>
#include "SpriteLib/SpriteLibBackend.h"
SDL_Window* g_pSDLWindow = NULL;
SDL_Renderer* g_pSDLRenderer = NULL;
#endif
#include "MFileDef.h"
#include "SoundSetting.h"

static void ResetStartupTrace()
{
}

static void ResetLoginFlowTrace()
{
}

static void TraceStartup(const char* step)
{
	(void)step;
}

void CSDLGraphics::Flip()
{
#ifdef SPRITELIB_BACKEND_SDL
	static const DWORD kTargetFrameMs = 1000 / 60;
	static DWORD s_lastPresentTime = 0;

	DWORD now = timeGetTime();
	if (s_lastPresentTime != 0)
	{
		DWORD elapsed = now - s_lastPresentTime;
		if (elapsed < kTargetFrameMs)
		{
			Sleep(kTargetFrameMs - elapsed);
			now = timeGetTime();
		}
	}

	if (g_pSDLRenderer == NULL || g_pBack == NULL)
	{
		return;
	}

	spritectl_surface_t backend_surface = g_pBack->GetBackendSurface();
	if (backend_surface == SPRITECTL_INVALID_SURFACE)
	{
		return;
	}

	SDL_RenderClear(g_pSDLRenderer);
	if (spritectl_present_surface(backend_surface, g_pSDLRenderer) == 0)
	{
		SDL_RenderPresent(g_pSDLRenderer);
		s_lastPresentTime = timeGetTime();
	}
#endif
}

#ifdef OUTPUT_DEBUG
#include "Packet/Gpackets/GCSystemMessage.h"
#endif

// REMOVED: nProtect anti-cheat code (SDL migration - no longer needed)

//yckou
#include "DebugKit.h"

//#define	new			DEBUG_NEW
//#define	UPDATER_FILENAME		"Updater.exe"
#define DIRECTORY_UPDATE		"Update"

#ifndef OUTPUT_DEBUG
	#define	USE_UPDATER
#endif

#if defined(OUTPUT_DEBUG) && defined(__METROTECH_TEST__)
//	#define	__WM_OUTPUT_DEBUG__
#endif

#ifdef OUTPUT_DEBUG
	extern bool					g_bSlideScreenShot;
#endif

#ifdef OUTPUT_DEBUG
//	int CLogFile = -1;
#endif

#ifdef DEBUG_INFO
BOOL g_bMsgOutPutFlag=FALSE;
BOOL g_bMsgDetailFlag=FALSE;
BOOL g_bMsgContentFlag=FALSE;
BYTE g_nKeyMapSelect = 0;
int  g_nGameVersion = 0;
CMessageStringTable g_MessageStringTable;
#endif
//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------

// REMOVED: nProtect anti-cheat code (SDL migration - no longer needed)
HWND				g_hWnd;
HINSTANCE			g_hInstance;
static const int kDefaultGameWidth = 1920;
static const int kDefaultGameHeight = 1080;
static const int kQhdGameWidth = 2560;
static const int kQhdGameHeight = 1440;

int					g_ResolutionX = kDefaultGameWidth;
int					g_ResolutionY = kDefaultGameHeight;

int					g_x=kDefaultGameWidth / 2;
int					g_y=kDefaultGameHeight / 2;
//bool				g_bHALAvailable = false;
DWORD				g_dwVideoMemory = 0;

char				g_CWD[_MAX_PATH];

int					g_Dimension = 0;
DWORD				g_TimerNPMON = 0;

// FPS
DWORD				g_CurrentTime		= 0;		
DWORD				g_CurrentFrame		= 0;		

int					g_FrameCount		= 0;
int					g_StartFrameCount	= 0;
DWORD				g_StartTime			= timeGetTime();
DWORD				g_EndTime			= g_StartTime;
int					g_FrameRate			= 0;
bool				g_bGoodFPS			= true;
const int			g_FrameGood			= 15;

// Execute Program --> bActiveApp
// minimize | anotherWnd click--> !ActiveGame
BOOL				g_bActiveApp			= FALSE; // Is application active?
BOOL				g_bActiveGame			= FALSE; // Is Game Active?
BOOL				g_bNeedUpdate			= FALSE; 

DWORD				g_double_click_time = 0;

bool				g_bTestMode = false;
//BOOL				g_WindowsNT = FALSE;
bool				g_bForceExitBynProtect = false;
bool				g_bForceExitBynProtect2 = false;
DWORD				g_nProtectErrorMessage = 0;
DWORD				g_nProtectErrorMessage2 = 0;
int					g_LeftPremiumDays = 0;
bool				g_bFamily = false;
bool				g_bSuperUser			= false;

DWORD g_dwSeqNumL = 0;
DWORD g_dwSeqNumG = 0;
//WORD g_wAuthKeyMap = 0x28AA;
//WORD g_wAuthKeyMap = 0x1455;
//edit by sonic 2006.4.12
//WORD g_wAuthKeyMap = 0xFFFFFFFF;
//end
WORD g_wAuthKeyMap = 0x5154;

//void SizeOfObjects();
//#define				__WEB_BROWSER__
#ifdef __WEB_BROWSER__
IWebBrowser2*			g_pWebBrowser = NULL;
#endif

char g_FutecIP[20] = { 0, };
unsigned int g_FutecPort = 0;
BYTE g_AdvanceVampireActionMaxCount[ ACTION_ADVANCEMENT_MAX - ACTION_ADVANCEMENT_STOP ];
BYTE g_AdvanceSlayerActionMaxCount[ ACTION_ADVANCEMENT_SLAYER_MAX - ACTION_ADVANCEMENT_SLAYER_STOP_SWORD ];
BYTE g_AdvanceOustersActionMaxCount[ ACTION_ADVANCEMENT_OUSTERS_MAX - ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP ];

enum DARKEDEN_LANGUAGE
{
	DARKEDEN_KOREAN = 0,
	DARKEDEN_CHINESE,
	DARKEDEN_JAPANESE,
	DARKEDEN_ENGLISH,
	DARKEDEN_TAIWAN,
	
	DARKEDEN_LANGUAGE_MAX
};

struct NETMARBLE_INFO
{
	MString ID;
	MString Password;
	int WorldID;
	int ServerID;
	bool bGore;
};

// 2004, 7, 14, sobeit add start
struct REALSERVER_INFO
{
	REALSERVER_INFO() { bMode = false;WorldID = 0;}
	bool bMode;		
	int WorldID;	
	MString ID;		// id
	MString Key;	// Key
};
// 2004, 7, 14, sobeit add end

BYTE g_macAddress[6];
extern void CheckMacScreenMode();
extern BOOL GetMacAddressFromSock();
extern BOOL InitDebugInfo();

BOOL g_MyFull=TRUE;
RECT g_GameRect={kDefaultGameWidth - 1,kDefaultGameHeight - 1,kDefaultGameWidth,kDefaultGameHeight};
LONG	g_SECTOR_WIDTH           =40 ;
LONG	g_SECTOR_HEIGHT          =45 ;
LONG	g_SECTOR_WIDTH_HALF      =21 ;
LONG	g_SECTOR_HEIGHT_HALF     =23 ;
LONG	g_SECTOR_SKIP_PLAYER_LEFT=-20 ;
LONG	g_SECTOR_SKIP_PLAYER_UP  =-22 ;

LONG	g_TILESURFACE_SECTOR_WIDTH         =46;
LONG	g_TILESURFACE_SECTOR_HEIGHT        =51;
LONG	g_TILESURFACE_SECTOR_OUTLINE_RIGHT =43;
LONG	g_TILESURFACE_SECTOR_OUTLINE_DOWN  =48;
LONG	g_TILESURFACE_WIDTH                =2208;
LONG	g_TILESURFACE_HEIGHT               =1224;
LONG	g_TILESURFACE_OUTLINE_RIGHT        =2064;
LONG	g_TILESURFACE_OUTLINE_DOWN         =1152;
LONG	g_TILE_X_HALF = 24;
LONG	g_TILE_Y_HALF = 12;
// end

static void EnableDpiAwareProcess()
{
#ifdef PLATFORM_WINDOWS
	HMODULE user32 = GetModuleHandleA("user32.dll");
	if (user32 == NULL)
	{
		user32 = LoadLibraryA("user32.dll");
	}

	if (user32 != NULL)
	{
		typedef BOOL (WINAPI *SetProcessDPIAwareProc)();
		SetProcessDPIAwareProc setProcessDPIAware =
			(SetProcessDPIAwareProc)GetProcAddress(user32, "SetProcessDPIAware");
		if (setProcessDPIAware != NULL)
		{
			setProcessDPIAware();
		}
	}
#endif
}

static void GetDesktopResolution(int& width, int& height)
{
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	if (width <= 0 || height <= 0)
	{
		width = kDefaultGameWidth;
		height = kDefaultGameHeight;
	}
}

static bool CanUseResolution(int width, int height)
{
	int desktopWidth = 0;
	int desktopHeight = 0;
	GetDesktopResolution(desktopWidth, desktopHeight);

	return desktopWidth >= width && desktopHeight >= height;
}

static bool ReadConfiguredGameResolution(int& width, int& height)
{
	std::ifstream file("DarkEdenResolution.cfg");
	if (!file.is_open())
	{
		return false;
	}

	std::string content;
	std::string line;
	while (std::getline(file, line))
	{
		content += line;
		content += '\n';
	}

	for (size_t i = 0; i < content.size(); ++i)
	{
		if (content[i] >= 'a' && content[i] <= 'z')
		{
			content[i] = (char)(content[i] - 'a' + 'A');
		}
	}

	if (content.find("AUTO") != std::string::npos)
	{
		if (CanUseResolution(kQhdGameWidth, kQhdGameHeight))
		{
			width = kQhdGameWidth;
			height = kQhdGameHeight;
		}
		else
		{
			width = kDefaultGameWidth;
			height = kDefaultGameHeight;
		}
		return true;
	}

	if (content.find("QHD") != std::string::npos ||
		content.find("2560") != std::string::npos ||
		content.find("1440") != std::string::npos)
	{
		width = kQhdGameWidth;
		height = kQhdGameHeight;
		return true;
	}

	if (content.find("FHD") != std::string::npos ||
		content.find("1920") != std::string::npos ||
		content.find("1080") != std::string::npos)
	{
		width = kDefaultGameWidth;
		height = kDefaultGameHeight;
		return true;
	}

	return false;
}

void ApplyGameResolution(int width, int height)
{
	if (!((width == kDefaultGameWidth && height == kDefaultGameHeight) ||
		(width == kQhdGameWidth && height == kQhdGameHeight)))
	{
		width = kDefaultGameWidth;
		height = kDefaultGameHeight;
	}

	g_ResolutionX = width;
	g_ResolutionY = height;
	g_GameRect.left = width - 1;
	g_GameRect.top = height - 1;
	g_GameRect.right = width;
	g_GameRect.bottom = height;
	g_x = width / 2;
	g_y = height / 2;

	g_SECTOR_WIDTH = max(1, width / (g_TILE_X_HALF * 2));
	g_SECTOR_HEIGHT = max(1, height / (g_TILE_Y_HALF * 2));
	g_SECTOR_WIDTH_HALF = g_SECTOR_WIDTH / 2 + 1;
	g_SECTOR_HEIGHT_HALF = g_SECTOR_HEIGHT / 2 + 1;
	g_SECTOR_SKIP_PLAYER_LEFT = -max(1, g_SECTOR_WIDTH_HALF - 1);
	g_SECTOR_SKIP_PLAYER_UP = -max(1, g_SECTOR_HEIGHT_HALF - 1);

	const LONG kTileSurfaceSectorMargin = 3;
	g_TILESURFACE_SECTOR_WIDTH = g_SECTOR_WIDTH + kTileSurfaceSectorMargin * 2;
	g_TILESURFACE_SECTOR_HEIGHT = g_SECTOR_HEIGHT + kTileSurfaceSectorMargin * 2;
	g_TILESURFACE_SECTOR_OUTLINE_RIGHT = g_TILESURFACE_SECTOR_WIDTH - kTileSurfaceSectorMargin;
	g_TILESURFACE_SECTOR_OUTLINE_DOWN = g_TILESURFACE_SECTOR_HEIGHT - kTileSurfaceSectorMargin;
	g_TILESURFACE_WIDTH = g_TILESURFACE_SECTOR_WIDTH * (g_TILE_X_HALF * 2);
	g_TILESURFACE_HEIGHT = g_TILESURFACE_SECTOR_HEIGHT * (g_TILE_Y_HALF * 2);
	g_TILESURFACE_OUTLINE_RIGHT = g_TILESURFACE_SECTOR_OUTLINE_RIGHT * (g_TILE_X_HALF * 2);
	g_TILESURFACE_OUTLINE_DOWN = g_TILESURFACE_SECTOR_OUTLINE_DOWN * (g_TILE_Y_HALF * 2);
}

void ApplyConfiguredGameResolution()
{
	int width = kDefaultGameWidth;
	int height = kDefaultGameHeight;
	ReadConfiguredGameResolution(width, height);

	if (width == kQhdGameWidth && height == kQhdGameHeight &&
		!CanUseResolution(kQhdGameWidth, kQhdGameHeight))
	{
		width = kDefaultGameWidth;
		height = kDefaultGameHeight;
	}

	ApplyGameResolution(width, height);
}

//-----------------------------------------------------------------------------
// define function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------			
#define WRITE_FROM_FILE(originalFile, patchFile)		\
		{												\
			char buffer[4096];							\
			int n;										\
														\
			while (1)									\
			{											\
				patchFile.read(buffer, 4096);			\
				n = patchFile.gcount();					\
														\
				if (n > 0)								\
				{										\
					originalFile.write(buffer, n);		\
				}										\
				else									\
				{										\
					break;								\
				}										\
			}											\
		}

void get_rand_str(char s[],int number);
void get_rand_str(char s[],int number)
{
        char str[64] = "00123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; 
        int i;
        char ss[2];
        //printf("%c %c\n",str[1],str[62]);
        srand((unsigned int)time((time_t *)NULL));
        for(i=1;i<=number;i++){
                sprintf(ss,"%c",str[(rand()%62)+1]);
                //printf(ss);
                strcat(s,ss);
        }
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// DarkEden.exe Futec(IP:Port)
//              01234567890123
//-----------------------------------------------------------------------
bool
GetFutecAddress(const char* pStr)
{
	const char * pFutecStr = strstr(pStr, "Futec");
	const char * pLeftParen = strchr(pStr, '(');
	const char * pSeperator = strchr(pStr, ':');
	const char * pRightParen = strchr(pStr, ')');

	if (pFutecStr==NULL
		|| pLeftParen==NULL
		|| pSeperator==NULL
		|| pRightParen==NULL
		|| !(pFutecStr < pLeftParen 
			&& pLeftParen < pSeperator
			&& pSeperator < pRightParen))
		return false;

	char str[20];

	strncpy( g_FutecIP, pLeftParen+1, pSeperator-(pLeftParen+1) );
	strncpy( str, pSeperator+1, pRightParen-(pSeperator+1) );
	g_FutecPort = atoi(str);

	return true;
}

bool ParsingNetmarble(const char* pCommandLine, NETMARBLE_INFO &info);
//bool ParsingRealServer(const char* pCommandLine,  int Dimention, REALSERVER_INFO &info);

bool
ParsingRealServer(const char* pCommandLine, int Dimention, REALSERVER_INFO &info)
{
	if( pCommandLine == NULL )
		return false;
	
	char szTemp[4096];
	if(strlen(pCommandLine) < 16)
		return false;

	const char *pString = strchr(pCommandLine, ' ');
	if(pString == NULL)
	{
		return false;
	}
	
	strcpy(szTemp, pString+1);
	
	int argcnt = 0;
	char* token = NULL;
	char arg2[4][32];
	argcnt = 0;

	token = strtok(szTemp, "|");
	if(NULL == token)
		return false;
	while(token && argcnt < 4)
	{
		strcpy(arg2[argcnt], token);
		argcnt++;
		token = strtok(NULL, "|");
	//	if(NULL == token)
	//		return false;
	}
	// mode|demention|ID|key
	info.bMode = atoi(arg2[0])?1:0;
	info.WorldID = atoi(arg2[1]);
	info.ID = arg2[2];
	info.Key = arg2[3];
	
	return true;
} 
//-----------------------------------------------------------------------------
// Name: InitFail()
// Desc: This function is called if an initialization function fails
//-----------------------------------------------------------------------------
void InitFail(LPCTSTR szError,...)
{

	g_bActiveApp = FALSE;

	ShowCursor( TRUE );
	ShowWindow(g_hWnd, SW_HIDE);

    char		szBuff[1024];
    va_list		vl;

    va_start(vl, szError);
    vsprintf(szBuff, szError, vl);

    //ReleaseAllObjects();
	DEBUG_ADD( szBuff );
//add by sonic 2006.4.11
    //MessageBox(g_hWnd, szBuff, PROGRAM_TITLE, MB_OK);
#ifdef PLATFORM_WINDOWS
    DestroyWindow(g_hWnd);
#endif
    va_end(vl);
//end
}

//-----------------------------------------------------------------------------
// ProgressBar
//-----------------------------------------------------------------------------
bool g_bUseProgressBar = true;
HWND g_hWndProgress = NULL;
const int progressBarWidth = 300;
const int progressBarHeight = 40;
const int g_numAppendFiles = 16;		

std::map<DWORD,std::string> g_nProtectMessage;

void
RemoveProgressBar()
{
	if (g_hWndProgress!=NULL)
	{
#ifdef PLATFORM_WINDOWS
		DestroyWindow(g_hWndProgress);
#endif
		g_hWndProgress = NULL;
	}
}

DARKEDEN_LANGUAGE CheckDarkEdenLanguage()
{
	return DARKEDEN_ENGLISH;

	if( g_pFileDef == NULL )
		return DARKEDEN_KOREAN;

	int num = 0;
	const char* fileName = g_pFileDef->getProperty("FILE_LANGUAGE_INFO").c_str();
	FILE *f = fopen(fileName, "r");
	if (f == NULL) goto exit;

	char szLine[512];

	while( fgets( szLine, 512, f) != NULL)
	{
		if( szLine[0] == ';' || strlen( szLine ) <= 0 )
			continue;

		if( strncmp( szLine, "LANGUAGE",8 ) == 0 )
		{
			sscanf(szLine+8,"%d",&num);
			break;
		}
	}

	fclose(f);

exit:
	return (DARKEDEN_LANGUAGE)(DARKEDEN_KOREAN + num );
}

void		PrecalculateAdvancementClassCreatureFrames()
{
	memset( g_AdvanceVampireActionMaxCount, 0, sizeof( g_AdvanceVampireActionMaxCount ) );
	memset( g_AdvanceSlayerActionMaxCount, 0, sizeof( g_AdvanceSlayerActionMaxCount ) );
	memset( g_AdvanceOustersActionMaxCount, 0, sizeof( g_AdvanceOustersActionMaxCount ) );

	int i;

	
	for( i = ACTION_ADVANCEMENT_SLAYER_STOP_SWORD; i < ACTION_ADVANCEMENT_SLAYER_MAX; ++i )
	{
		int actionindex = i - ADVANCEMENT_ACTION_START;
		
		char part = 0;

		if( i == ACTION_ADVANCEMENT_SLAYER_BIKE_MOVE )
		{
			g_AdvanceSlayerActionMaxCount[ actionindex ] = 4;
			continue;
		}
			// ||
			//i == ACTION_ADVANCEMENT_SLAYER_BIKE_STOP )
			//part = AC_BIKE_1;

//		try{
		g_AdvanceSlayerActionMaxCount[ actionindex ] = 
			g_pTopView->m_AdvancementSlayerManFPK[ part ][ actionindex ][2].GetSize();
//		}catch(...){} //add by viva
	}
	

	for( i = ACTION_ADVANCEMENT_STOP; i < ACTION_ADVANCEMENT_MAX; ++i )
	{
		int actionindex = i - ADVANCEMENT_ACTION_START;
//		try{
		g_AdvanceVampireActionMaxCount[ actionindex ] = 
			g_pTopView->m_AdvancementVampireManFPK[0][ actionindex ][2].GetSize();
//		}catch(...){} //add by viva
	}

	for( i = ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP; i < ACTION_ADVANCEMENT_OUSTERS_MAX; ++i )
	{
		int actionindex = i - ADVANCEMENT_ACTION_START;
//		try{
		g_AdvanceOustersActionMaxCount[ actionindex ] = 
			g_pTopView->m_AdvancementOustersFPK[1][ actionindex ][2].GetSize();
//		}catch(...){} //add by viva
	}
}

int			
GetPersnalShopUpdateTime()
{
	CRarFile rarfile;

	rarfile.SetRAR(g_pFileDef->getProperty("FILE_INFO_DATA").c_str(),"darkeden");
	rarfile.Open(g_pFileDef->getProperty("FILE_UPDATE_PERSNALSHOPTIME").c_str() );

	if( !rarfile.IsSet() )
		return 60000;

	char szLine[512];
	int num = 0;

	while( rarfile.GetString( szLine, 512 ) )
	{
		if( strncmp( szLine, "UPDATETIME",10 ) == 0 )
		{
			sscanf(szLine+10,"%d",&num);
			break;
		}
	}

	rarfile.Release();

	return num;
}
void
CreateProgressBar(const char* str)
{
	if (g_bUseProgressBar)
	{
		RemoveProgressBar();

		InitCommonControls();

		int cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 

		g_hWndProgress = CreateWindowEx(WS_EX_TOPMOST, PROGRESS_CLASS, str, 
										WS_VISIBLE | PBS_SMOOTH, 
										GetSystemMetrics(SM_CXSCREEN)/2 - progressBarWidth/2, 
										GetSystemMetrics(SM_CYSCREEN)/2 - progressBarHeight/2, 
										progressBarWidth, progressBarHeight,
										NULL, (HMENU)0, g_hInstance, NULL); 
	}
}


void
SetProgressBarCount(int count)
{	
	if (!g_bUseProgressBar || g_hWndProgress==NULL)
	{
		return;
	}

	SendMessage(g_hWndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, count)); 
    SendMessage(g_hWndProgress, PBM_SETSTEP, (WPARAM) 1, 0); 
	SendMessage(g_hWndProgress, PBM_SETPOS, (WPARAM) 0, 0); 	
}

void
SetProgressBarText(const char* str)
{
	if (g_bUseProgressBar)
	{
		if (g_hWndProgress==NULL)
		{
			CreateProgressBar(str);
			SetProgressBarCount( g_numAppendFiles );
		}
		else
		{
			SetWindowText( g_hWndProgress, str	);
		}
	}
}



void
UpdateProgressBar()
{
	if (!g_bUseProgressBar || g_hWndProgress==NULL)
	{
		return;
	}

	SendMessage(g_hWndProgress, PBM_STEPIT, 0, 0); 
}


//-----------------------------------------------------------------------------
// Patch Log Window
//-----------------------------------------------------------------------------
HWND		g_hPatchLogWnd = NULL;
HWND		g_hPatchLogEdit = NULL;
char*		g_pPatchLogBuffer = NULL;	

long FAR PASCAL PatchLogWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
    {
		case WM_DESTROY:
            PostQuitMessage(0);
        return 0L;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool
ReadPatchLogFromFile()
{
	char strBuffer[256];

	ifstream file("PatchLog.txt");	

	if (!file.is_open())
	{
		return false;
	}

	file.seekg( 0, ios::end );
	long fpEnd = file.tellg();

	if (g_pPatchLogBuffer!=NULL)
	{
		delete [] g_pPatchLogBuffer;
	}
	g_pPatchLogBuffer = new char [fpEnd*2];

	file.seekg( 0 );

	char* pBuffer = g_pPatchLogBuffer;

	while (!file.eof())
	{
		file.getline(strBuffer, 254);
		int n = file.gcount();		
		
		if (n==0)
		{
			break;
		}

		if (strBuffer[n-1]=='\0')
		{
			n -= 1;
		}

		
		strBuffer[n] = '\r';
		strBuffer[n+1] = '\n';
		
		strncpy(pBuffer, strBuffer, n+2);
		pBuffer += n+2;
	}
	*pBuffer = '\0';

	file.close();

	SetWindowText( g_hPatchLogEdit, g_pPatchLogBuffer );

	delete [] g_pPatchLogBuffer;
	g_pPatchLogBuffer = NULL;

	return true;
}

void OutPutClassSize()
{
//#ifdef __DEBUG_OUTPUT__
//	SizeOfObjects();
//#endif
}

void
ShowPatchLogWindow()
{
 
}

//-----------------------------------------------------------------------------
// Check DX Version
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
 
 
//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The Main Window Procedure
//-----------------------------------------------------------------------------
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//MINMAXINFO      *pMinMax;

    switch (message)
    {
// REMOVED: nProtect anti-cheat code (SDL migration - no longer needed)

		//---------------------------------------------------------------
		//
		//						MM_MCINOTIFY
		//
		//---------------------------------------------------------------
		case MM_MCINOTIFY :
		{
			
			if (wParam==MCI_NOTIFY_SUCCESSFUL)
			{
				
				if (g_pAvi!=NULL && (DWORD)lParam==g_pAvi->dwID) 
				{ 
					g_pAvi->bEndFlag=true; 
					g_pAvi->Close(); 

					
					SetMode( MODE_MAINMENU );
				}				
				
				else
				{
					 
					//else
					//{
						
					//	g_Music.RePlay();
					//}
//					if(g_pMP3->IsLoop())
//						g_pMP3->Play(true);
				}
			}
		}
		return 0L;

		//---------------------------------------------------------------
		//
		//	WM_SYSCOMMAND
		// 
		//---------------------------------------------------------------
		case WM_SYSCOMMAND:
            // Prevent moving/sizing and power loss in fullscreen mode
            switch( wParam )
            {
				case SC_HOTKEY:
				case SC_KEYMENU:
				case SC_TASKLIST:
#if !defined(OUTPUT_DEBUG) && !defined(__DEBUG_OUTPUT__)
				case SC_PREVWINDOW:
				case SC_NEXTWINDOW:
#endif
					return 1;

				case SC_CLOSE:
				case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_MONITORPOWER:
                    if( CSDLGraphics::IsFullscreen() )
                        return 1;
                    break;
            }
        break;//return 0L;


		//---------------------------------------------------------------
		//
		//							WM_ACTIVATE
		//
		//---------------------------------------------------------------
		 

		//---------------------------------------------------------------
		//
		//							WM_ACTIVATEAPP
		//
		//---------------------------------------------------------------
		///*
//		case WM_ACTIVATE :
		case WM_ACTIVATEAPP :
		{
			#ifdef __WM_OUTPUT_DEBUG__
					DEBUG_ADD("[WM_ACTIVATEAPP]");
			#endif

            // Pause if minimized or not the top window
			//WORD fActive = LOWORD(wParam);
			//WORD fMinimized = (BOOL) HIWORD(wParam); 
            //g_bActiveGame = (fActive == WA_ACTIVE) || (fActive == WA_CLICKACTIVE);

			g_double_click_time = GetDoubleClickTime();

			BOOL bActiveGame = (BOOL)wParam;

			//if (g_bActiveApp)
			{
				CheckActivate(bActiveGame);

				// 
				g_bActiveGame = bActiveGame;
			}
		}
        break;//return 0L;
		//*/

		/*
		case WM_SIZE :
		{
			BOOL bActive;

			// Check to see if we are losing our window...
            if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
                bActive = FALSE;
            else
                bActive = TRUE;

            g_double_click_time = GetDoubleClickTime();

			if (g_bActiveApp)
			{
				CheckActivate(bActive);
			
				g_bActiveGame = bActive;			
			}
		}
		break;
		*/
		
		//---------------------------------------------------------------
		//
		//							WM_SIZE
		//
		//---------------------------------------------------------------
		//case WM_SIZE:
            // Check to see if we are losing our window...
            //if (SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam)
              //  g_bActiveGame = FALSE;
            //else
              //  g_bActiveGame = TRUE;
        //return 0L;
		
		//---------------------------------------------------------------
		//
		//							WM_DESTROY
		//
		//---------------------------------------------------------------
        case WM_DESTROY:
            // Clean up and close the app            
#ifdef SPRITELIB_BACKEND_SDL
			if (g_pSDLRenderer != NULL)
			{
				SDL_DestroyRenderer(g_pSDLRenderer);
				g_pSDLRenderer = NULL;
			}

			if (g_pSDLWindow != NULL)
			{
				SDL_DestroyWindow(g_pSDLWindow);
				g_pSDLWindow = NULL;
			}

			spritectl_shutdown();
#endif
            PostQuitMessage(0);
        return 0L;

		case WM_GETMINMAXINFO:
		{
            // Fix the size of the window to 640x480 (client size)
            MINMAXINFO* pMinMax = (MINMAXINFO *)lParam;
			// add by Sonic 2006.9.26
			//if(g_MyFull)
			//{
			//	pMinMax->ptMinTrackSize.x = 1024+GetSystemMetrics(SM_CXSIZEFRAME)*2;
			//	pMinMax->ptMinTrackSize.y = 768+GetSystemMetrics(SM_CYSIZEFRAME)*2
            //                               +GetSystemMetrics(SM_CYMENU);
			//}
			//else
			//{
			
				pMinMax->ptMinTrackSize.x = g_GameRect.right+GetSystemMetrics(SM_CXSIZEFRAME)*2;
				pMinMax->ptMinTrackSize.y = g_GameRect.bottom+GetSystemMetrics(SM_CYSIZEFRAME)*2
											   +GetSystemMetrics(SM_CYMENU);
			//}
			// end
            pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
            pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
		}
        break;
		
		/*
		case WM_PAINT:
			if (g_bActiveApp)
				CSDLGraphics::OnPaint();
			return 0L;
		*/
	
		//---------------------------------------------------------------
		//
		//							WM_MOVE
		//
		//---------------------------------------------------------------
		case WM_MOVE :			
			if (g_bActiveApp && !CSDLGraphics::IsFullscreen())
				CSDLGraphics::OnMove();
		break;

		/*
		case WM_MOUSEMOVE :		
			//fwKeys = wParam;        // key flags 
			//g_x = LOWORD(lParam);  // horizontal position of cursor 
			//g_y = HIWORD(lParam);  // vertical position of cursor 
			
			return 0L;
		*/

		//---------------------------------------------------------------
		//
		//							WM_KEYDOWN
		//
		//---------------------------------------------------------------
        case WM_KEYDOWN:
			#ifdef __WM_OUTPUT_DEBUG__
				DEBUG_ADD("[WM_KEYDOWN]");
			#endif

			if (g_Mode==MODE_OPENING)
			{
				
				if (wParam==VK_ESCAPE || wParam==VK_RETURN || wParam==VK_SPACE)
				{	
					if (g_pAvi!=NULL)
					{
						g_pAvi->Close();
					}
					SetMode( MODE_MAINMENU );					
				}
				return 0L;
			}

			gC_vs_ui.KeyboardControl(message, wParam, lParam);
            // Handle any non-accelerated key commands

			switch (wParam)
			{
				//-----------------------------------------------
				// Screen Shot
				//-----------------------------------------------
				case VK_SCROLL :	
					#ifdef OUTPUT_DEBUG
						if (!g_bSlideScreenShot)
						{
							MakeScreenShot();
						}
					#else
						MakeScreenShot();
					#endif
					
				return 0L;
				//case VK_F11 : 
				//		g_UpdateDelay = (g_UpdateDelay==1)? DELAY_UPDATE_GAME : 1;
				//		return 0L;

				//-----------------------------------------------
				// Toggle Music
				//-----------------------------------------------
				/*
				case VK_F3 :
					//if (g_Music.IsPlay())
					if (g_SDLMusic.IsPlay())
					{
						//g_Music.Pause();
						g_SDLMusic.Pause();
					}
					else
					{						
						//g_Music.Resume();
						g_SDLMusic.Resume();
					}										
				return 0L;
				*/

				//-----------------------------------------------
				
				//-----------------------------------------------
				/*
				case VK_F5 :
					{
						g_pMasterVolume->SetCurrentVolume(
								g_pMasterVolume->GetCurrentVolume() - 0x1000
							);
					}
					//if (g_pPlayer->IsExistAddon(MCreatureWear::ADDON_COAT))
					//	g_pPlayer->SetAddonNULL(MCreatureWear::ADDON_COAT); 
					//else g_pPlayer->SetAddon(MCreatureWear::ADDON_COAT, 0);						

				
				return 0L;
	
				case VK_F6 :		
					{
						//g_SDLMusic.AddVolume( 0xF000 );
						g_pMasterVolume->SetCurrentVolume(
								g_pMasterVolume->GetCurrentVolume() + 0x1000
							);
					}
					//if (g_pPlayer->IsExistAddon(MCreatureWear::ADDON_TROUSER))
					//	g_pPlayer->SetAddonNULL(MCreatureWear::ADDON_TROUSER); 
					//else g_pPlayer->SetAddon(MCreatureWear::ADDON_TROUSER, 1);
				return 0L;
				*/

				//-----------------------------------------------
				//
				
				//
				//-----------------------------------------------
				#if defined(OUTPUT_DEBUG)
				
					case VK_PAUSE :
						g_bActiveApp = !g_bActiveApp;					
					return 0L;

					//-----------------------------------------------
					// Toggle ActiveMode
					//-----------------------------------------------
					//case VK_F1 :
					//	g_bActiveApp = !g_bActiveApp;
					//	return 0L;	

					//-----------------------------------------------
					
					//-----------------------------------------------
					 		

					
					 

					/*
					case VK_F8 :
					{
						//{
							////g_SDLMusic.AddVolume( 0xF000 );
							//pWaveVolume->SetCurrentVolume(
									//pWaveVolume->GetCurrentVolume() + 0x1000
								//);
						//}
						//if (g_pPlayer->IsExistAddon(MCreatureWear::ADDON_HELM))
						//	g_pPlayer->SetAddonNULL(MCreatureWear::ADDON_HELM); 
						//else g_pPlayer->SetAddon(MCreatureWear::ADDON_HELM, 3);
						static int a = 0;

						if (a==0)
						{
							gC_vs_ui.ChangeToVampireInterface();
							a = 1;
						}
						else
						{
							gC_vs_ui.ChangeToSlayerInterface();
							a = 0;
						}
					}
					return 0L;					
					*/

					//-----------------------------------------------
					// Frame Fixing
					//-----------------------------------------------
					case VK_F11 : 
						#if defined(_DEBUG)
							if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
							{
								
								//g_UpdateDelay = (g_UpdateDelay==1)? DELAY_UPDATE_GAME : 1;
								g_UpdateDelay = (g_UpdateDelay==1)? (54 ^ 8) : 1;
							}
						#endif
						return 0L;

					//-----------------------------------------------
					
					//-----------------------------------------------
					case VK_F12 : 
						g_bPutMessage = !g_bPutMessage;
						return 0L;				
					
					//-----------------------------------------------
					
					//-----------------------------------------------
					case VK_DELETE :
						{
							#ifdef	OUTPUT_DEBUG
								if (g_pDebugMessage!=NULL)
										g_pDebugMessage->Clear();
								GCSystemMessage _GCSystemMessage;
								_GCSystemMessage.setMessage("GM[Coffee]>123456789012345678901234567890123456789012345678901234567890");
								_GCSystemMessage.setType(SYSTEM_MESSAGE_PLAYER);
								_GCSystemMessage.execute(NULL);
								_GCSystemMessage.setMessage("GM[Coffee]>1234567890123456789012345678901234567890123456789012345678901");
								_GCSystemMessage.execute(NULL);
								_GCSystemMessage.setMessage("GM[Coffee]>12345678901234567890123456789012345678901234567890123456789012");
								_GCSystemMessage.execute(NULL);
							#endif
						}
						return 0L;						

				#endif
			}		
			
		return 0L; 
		case WM_TIMER :			
			//for (loop = 0; loop < STREAM_MAX; ++loop)
//					if (g_pOGG[loop] ) 
#ifndef __USE_MP3__
			if( g_SDLAudio.IsInit() && g_pOGG != NULL )
				g_pOGG->streamUpdate (g_pOGG);
#endif
			break;

		//---------------------------------------------------------------
		//
		//							WM_CHAR
		//
		//---------------------------------------------------------------
		case WM_CHAR :
		{
			#ifdef __WM_OUTPUT_DEBUG__
				DEBUG_ADD("[WM_CHAR]");
			#endif

			/*
			{
				std::ofstream file("inputtest.txt", ios::app);
				file << "input!" << (char)wParam << endl;
				file.close();
			}
			*/
			gC_vs_ui.KeyboardControl(message, wParam, lParam);

			#ifdef __WM_OUTPUT_DEBUG__
				DEBUG_ADD("UI KeyCtrl OK");
			#endif

			TCHAR ch = (TCHAR)wParam;    // character code 	
			
			//switch (ch)
			//{
			//	case '`' : g_bDrawMinimap = !g_bDrawMinimap;
			//	return 0L;
			//}

			#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
				if (g_Mode==MODE_GAME)
				switch (ch)
				{					
					///*
					case '5' :
					{
						/*
						for (int i=0; i<g_MaxNPC; i++)
						{
							MCreature* pCreature = g_pZone->GetCreature(1000+i);
							if (pCreature != NULL)
							{
								if (pCreature->IsDead())
								{
									pCreature->SetAlive();
								}
								else
								{
									pCreature->SetDead();
								}
							}
						}
						*/
						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL) )
						{
							if (g_pPlayer->IsDead())
							{						
								g_pPlayer->SetAlive();
								g_pTopView->SetFadeEnd();
							}
							else
							{
								
								g_pPlayer->SetDead();						
							}
						}
					}
					return 0L;
					//*/

					// [ TEST CODE ]
					case '/' :
					{
						
						 
					}
					return 0L;
					//*/

					
					case '6' :
					{
						g_pTopView->SetFadeStart(25, 31, 2, 31,0,0);					
					}
					break;

					
					case '7' :
					{
						SetLightning(rand()%4*500+500);					
					}
					break;
					

					case '.' :
					{
						do {
							int newActionInfo;
							
							if (g_pPlayer->GetSpecialActionInfo()==ACTIONINFO_NULL)
							{
								newActionInfo = MAGIC_LIGHT;
							}
							else
							{
								newActionInfo = g_pPlayer->GetSpecialActionInfo()+1;
							}

							if (newActionInfo >= (*g_pActionInfoTable).GetMinResultActionInfo())
								newActionInfo = 0;

							int maxPlayerAction = g_pPlayer->GetActionMax();
							int newActionInfoAction = (*g_pActionInfoTable)[newActionInfo].GetAction();
							
							g_pPlayer->SetSpecialActionInfo( newActionInfo );

							
							if (newActionInfoAction <= maxPlayerAction)
								break;
						} while (1);					
					}
					return 0L;

					case ',' :
					{
						do {
							int newActionInfo;
							
							if (g_pPlayer->GetSpecialActionInfo()==ACTIONINFO_NULL)
							{
								newActionInfo = MAGIC_LIGHT;
							}
							else
							{
								newActionInfo = g_pPlayer->GetSpecialActionInfo()-1;
							}

							if (newActionInfo < 0)
								newActionInfo = (*g_pActionInfoTable).GetMinResultActionInfo() - 1;

							int maxPlayerAction = g_pPlayer->GetActionMax();
							int newActionInfoAction = (*g_pActionInfoTable)[newActionInfo].GetAction();
							
							g_pPlayer->SetSpecialActionInfo( newActionInfo );

							
							if (newActionInfoAction <= maxPlayerAction)
								break;
						} while (1);
					}
					return 0L;
					case 'L' :
						g_pPlayer->AddEffectStatus(EFFECTSTATUS_SHRINE_SHIELD,0xffff);
						return 0L;			
						
				}					
			#endif
		}
		return 0L;	
				

		/*
        case WM_SETCURSOR:
            // Turn off the cursor since this is a full-screen app
			{
				//SetCursor(LoadCursor(NULL, IDC_ARROW));
				//SetCursor(LoadCursorFromFile("Cursor2.cur"));
				//SetCursor(LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_MYCROSS)));			
				
				//SetCursor(LoadCursorFromFile("Mono.cur"));
				//SetCursor( LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_TEST)) );

				//SetCursor( LoadIcon(NULL, IDI_ERROR) );

				SetCursor(NULL);
				
				SetCapture(g_hWnd);
			}
            return TRUE;
		*/		
		
		//

		//---------------------------------------------------------------
		//
		//				WM_KEYUP
		//				WM_IME_COMPOSITION
		//				WM_IME_STARTCOMPOSITION
		//				WM_IME_ENDCOMPOSITION
		//
		//---------------------------------------------------------------
		case WM_KEYUP:
		 case WM_IME_COMPOSITION:
		 case WM_IME_STARTCOMPOSITION:
		 case WM_IME_ENDCOMPOSITION:
			 
			#ifdef __WM_OUTPUT_DEBUG__
				switch (message)
				{
					case WM_KEYUP:
						DEBUG_ADD("[WM_KEYUP]");
					break;

					 case WM_IME_COMPOSITION:
						 DEBUG_ADD("[WM_IME_COMPOSITION]");
					break;

					 case WM_IME_STARTCOMPOSITION:
						 DEBUG_ADD("[WM_IME_STARTCOMPOSITION]");
					break;

					 case WM_IME_ENDCOMPOSITION:
						 DEBUG_ADD("[WM_IME_ENDCOMPOSITION]");
					break;
				}	
			#endif

			gC_vs_ui.KeyboardControl(message, wParam, lParam);

			#ifdef __WM_OUTPUT_DEBUG__
				DEBUG_ADD("[WM_IME_....] OK");
			#endif

			if (message == WM_IME_STARTCOMPOSITION)
				return 0;

			break; // ! break!!!!

    }

	#ifdef __WM_OUTPUT_DEBUG__
		DEBUG_ADD("[WM_....]");
	#endif

    return DefWindowProc(hWnd, message, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name: InitApp()
// Desc: Do work required for every instance of the application:
//          Create the window, initialize data
//-----------------------------------------------------------------------------
BOOL
InitApp(int nCmdShow)
{
	TraceStartup("InitApp begin");
	WNDCLASS                    wc;
	
	//char rnd_PROGRAM_NAME[50];
	//char rnd_PROGRAM_TITLE[50];
	//get_rand_str(rnd_PROGRAM_NAME,5);
	//get_rand_str(rnd_PROGRAM_TITLE,5);
    // Set up and register window class
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = g_hInstance;
    wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_DARKEDEN));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH )GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;//NAME;
	wc.lpszClassName = PROGRAM_NAME;
    //wc.lpszClassName = PROGRAM_NAME;
    RegisterClass(&wc);
	TraceStartup("window class registered");
	
	// Calculate the proper size for the window given a client of 640x480
    int cx,cy;
	int windowX = 0;
	int windowY = 0;
	DWORD style = 0;
	DWORD exStyle = 0;

#ifdef __WEB_BROWSER__
	CoInitialize(NULL);
	if (HRESULT hr = FAILED(CoCreateInstance(CLSID_InternetExplorer,NULL,
			CLSCTX_LOCAL_SERVER, IID_IWebBrowser2,(LPVOID*)&g_pWebBrowser))) 
		{
			MessageBox(NULL, "IWebBrowser2 Create failed", "ERROR", MB_OK);
			return FALSE;
		}
#endif

	memset( g_macAddress, 0, 6*sizeof(BYTE) );
	//if(GetMacAddressFromSock())
	if(GetMacAddressFromNetBIOS(g_macAddress))
	{
#if !defined(OUTPUT_DEBUG) && !defined(_DEBUG) && !defined(__DEBUG_OUTPUT__)
		CheckMacScreenMode();
#endif
	}


	if (g_bFullScreen)
	{
		exStyle = WS_EX_TOPMOST;// | WS_EX_APPWINDOW;
		//style = WS_POPUP;
		style = WS_POPUP|WS_CLIPCHILDREN;
	//	style = WS_POPUP|WS_CLIPCHILDREN| WS_CLIPSIBLINGS;
		cx = GetSystemMetrics(SM_CXSCREEN);
		cy = GetSystemMetrics(SM_CYSCREEN);				  
	}
	else
	{
	//add by sonic 2006.4.10
	//DEVMODE  devmode;
	//	if(EnumDisplaySettings(0, 0, &devmode))
	//	{
	//		devmode.dmPelsWidth=1024;
	//		devmode.dmPelsHeight=768;
	//		devmode.dmBitsPerPel=16;
	//		devmode.dmDisplayFrequency=75;
			//ChangeDisplaySettings(&devmode,0);
	//	}
	//end
		exStyle = WS_EX_APPWINDOW;				//by viva : add WS_SYSMENU
		style = WS_POPUP | WS_CLIPCHILDREN;
//		style = WS_OVERLAPPEDWINDOW;
		// add by Sonic 2006.9.26
//		if(g_MyFull)
		//{
		//	cx = 1024 + GetSystemMetrics(SM_CXSIZEFRAME)*2;
		//	cy = 768 + GetSystemMetrics(SM_CYSIZEFRAME)*2+GetSystemMetrics(SM_CYMENU);	
		//}
		//else
		//{
			cx = g_GameRect.right;
			cy = g_GameRect.bottom;

			RECT workArea = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
			const int workWidth = workArea.right - workArea.left;
			const int workHeight = workArea.bottom - workArea.top;
			windowX = workArea.left + max(0, (workWidth - cx) / 2);
			windowY = workArea.top + max(0, (workHeight - cy) / 2);
		//}
	}
	
	// Create a window
	/*
    g_hWnd = CreateWindowEx(exStyle,//0,
                          PROGRAM_NAME,
                          PROGRAM_TITLE,
	rnd_PROGRAM_TITLE*/

    g_hWnd = CreateWindowEx(exStyle,//0,
                          PROGRAM_NAME,
                          PROGRAM_TITLE,
						  style,
                          windowX,
                          windowY,
						  cx,
						  cy,
                          NULL,
                          NULL,
                          g_hInstance,
                          NULL);

    if (!g_hWnd)
	{
        return FALSE;
	}
	TraceStartup("window created");

// REMOVED: nProtect anti-cheat code (SDL migration - no longer needed)

	//
    //ShowWindow(g_hWnd, nCmdShow);
    //UpdateWindow(g_hWnd);
    //SetFocus(g_hWnd);

	//---------------------------------------------------
	// DirectX Version check
	//---------------------------------------------------
	//if (!CheckDXVersion())
	//{
	//	return 0;
	//}

	
	SetCursor(NULL);
	ShowCursor( FALSE );

	
	ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);
    SetFocus(g_hWnd);	
	TraceStartup("window shown");

#ifdef SPRITELIB_BACKEND_SDL
	if (spritectl_init() != 0)
	{
		TraceStartup("SpriteLib backend init FAILED");
		return FALSE;
	}
	TraceStartup("SpriteLib backend init OK");

	g_pSDLWindow = SDL_CreateWindowFrom((const void*)g_hWnd);
	if (g_pSDLWindow == NULL)
	{
		TraceStartup("SDL window attached FAILED");
		return FALSE;
	}
	TraceStartup("SDL window attached");

	g_pSDLRenderer = SDL_CreateRenderer(g_pSDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (g_pSDLRenderer == NULL)
	{
		g_pSDLRenderer = SDL_CreateRenderer(g_pSDLWindow, -1, SDL_RENDERER_SOFTWARE);
	}

	if (g_pSDLRenderer == NULL)
	{
		TraceStartup("SDL renderer create FAILED");
		return FALSE;
	}

	SDL_SetRenderDrawColor(g_pSDLRenderer, 0, 0, 0, 255);
	TraceStartup("SDL renderer created");
#endif

	//---------------------------------------------------
	// Init Game
	//---------------------------------------------------
	TraceStartup("before InitGame");
	return InitGame();
}


//extern void OutputMemorySize();
//#include "MemoryCheck.cpp"
//#include "UIDialog.h"

//-----------------------------------------------------------------------------
// Check TerriblePatch
//-----------------------------------------------------------------------------
bool
CheckTerriblePatch()
{
	bool bCrash = false;
	int errorCode = 0;

	//-----------------------------------------------------------------------------
	// ImageObject Patch
	//-----------------------------------------------------------------------------
	AppendPatchTable apt;

/*
#ifdef _DEBUG
	apt.Init( 2 );	
	
	//-----------------------------------------------------------------------------
	// ImageObject Patch
	//-----------------------------------------------------------------------------	
	apt[0].SetOriginalInfo( FILE_SPRITE_IMAGEOBJECT, 47564158, 0x04CA );
	apt[0].AddAppendInfo( "Data\\Image\\New18ImageObject.spk", 1968462, 0x82 );
	apt[0].AddAppendInfo( "Data\\Image\\New22ImageObject.spk", 5706400, 0x8A );
	apt[0].SetFinalInfo( 55239016, 0x05D6 );

	//-----------------------------------------------------------------------------
	// Creature Patch
	//-----------------------------------------------------------------------------
	apt[1].SetOriginalInfo( FILE_ISPRITE_CREATURE, 47722828, 0x2A4E );
	apt[1].AddAppendInfo( "Data\\Image\\New18Creature.ispk", 4343846, 0x0418 );
	apt[1].AddAppendInfo( "Data\\Image\\New22Creature.ispk", 9813886, 0x0798 );
	apt[1].SetFinalInfo( 61880556, 0x35FE );

	
	//-----------------------------------------------------------------------------
	// Save
	//-----------------------------------------------------------------------------
	std::ofstream fileAppendInfo2(FILE_INFO_APPENDPATCH, ios::binary);
	apt.SaveToFile( fileAppendInfo2 );
	fileAppendInfo2.close();
#endif
*/

	//-----------------------------------------------------------------------------
	// Load
	//-----------------------------------------------------------------------------
	std::ifstream fileAppendInfo;
	if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_APPENDPATCH").c_str(), fileAppendInfo))
		return false;
	apt.LoadFromFile( fileAppendInfo );
	fileAppendInfo.close();

	

	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	errorCode = 0;
	for (int i=0; i<apt.GetSize(); i++)
	{
		errorCode ++;
		if (!apt[i].ExecutePatch())
		{
			//bCrash = true;
			break;
		}

		errorCode ++;
		if (!apt[i].CheckFinalInfo())
		{
			#ifdef OUTPUT_DEBUG				
			#else
				//bCrash = true;
				break;
			#endif
		}
		
	}
	
	//-----------------------------------------------------------------------
	
	//-----------------------------------------------------------------------
	if (bCrash)
	{
		//char str[256];
		//add by sonic 2006.4.11
		
		//MessageBox(NULL, str, PROGRAM_TITLE, MB_OK);
		//end 
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Check TerriblePatch
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
// ConvertScreenEffect
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
 
//------------------------------------------------------------------------
// ApplyPatch 
//------------------------------------------------------------------------

//------------------------------------------------------------------------
bool
ApplyPatch()
{	
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
//	std::ifstream versionFile(FILE_INFO_VERSION, ios::binary);
//	int version;
//	versionFile.read((char*)&version, 4);
//	versionFile.close();

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
//	char packFilename[256];
//	char infoFilename[256];

//	struct _finddata_t	FileData;
//	long				hFile;

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
//	if ( (hFile = _findfirst( "*.mpk", &FileData )) != -1L )
//	{		
//		int files = 1;
//		do
//		{
//			strcpy(packFilename, FileData.name);
//			files ++;
//		}
//		while (_findnext( hFile, &FileData ) == 0);
//
//		_findclose( hFile );			

		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
//		_mkdir( "Update" );
//

//
//		MZLib mzlib;
//

//		mzlib.Uncompress( packFilename );	
//
//		//-----------------------------------------------------------------

//		//-----------------------------------------------------------------
//		int len = strlen(packFilename);
//		char str[256];

//		str[len-4] = '\0';
//		sprintf(infoFilename, "Update\\%s.inf", str);
//
//	}
	//-----------------------------------------------------------------
	
	
	//-----------------------------------------------------------------
//	else
//	{
//		/*
//		char* pUnder = strchr(packFilename+5, "_");
//		if (pUnder!=NULL)
//		{   
//			char strVersion[10];
//			strncpy(strVersion, packFilename+5, pUnder - packFilename+5);
//			int versionFrom = atoi(strVersion);
//		}
//		*/
//		

//		sprintf(infoFilename, "Update\\Patch%d_*.inf", version);
//
//		if ( (hFile = _findfirst( infoFilename, &FileData )) != -1L )
//		{	
//			int files = 1;
//			do
//			{
//				sprintf(infoFilename, "Update\\%s", FileData.name);
//				files ++;
//			}
//			while (_findnext( hFile, &FileData ) == 0);
//			
//			_findclose( hFile );
//		}
//		else
//		{

//			return false;
//		}
//	}
//
//	//-----------------------------------------------------------------

//	//-----------------------------------------------------------------	

//	
//	UpdateManager	UM;
//	UM.load( infoFilename );
//
//	SetProgressBarCount( UM.getNum()+g_numAppendFiles );	
//

//	
//	//-----------------------------------------------------------------

//	//-----------------------------------------------------------------
//	int newVersion = 0;
//	while (!UM.empty())
//	{
//		Update* pUpdate = UM.front();
//		UM.pop_front();
//		
//		if (!pUpdate->execute())
//		{
//			// -_-;;
//		}	
//

//		if (pUpdate->getVersion() > newVersion)
//		{
//			newVersion = pUpdate->getVersion();
//		}
//
//		UpdateProgressBar();
//	}
//
//	//-----------------------------------------------------------------

//	//-----------------------------------------------------------------
//	std::ofstream versionFile2(FILE_INFO_VERSION, ios::binary);
//	versionFile2.write((const char*)&newVersion, 4);
//	versionFile2.close();
//	
//	//-----------------------------------------------------------------

//	//-----------------------------------------------------------------
//	remove( packFilename );
//
//	//-----------------------------------------------------------------

//	//-----------------------------------------------------------------
//
	return true;
}


#ifdef OUTPUT_DEBUG
	//-----------------------------------------------------------------------------
	// Check LogFile
	//-----------------------------------------------------------------------------
//	void
//	CheckCLogFile()
//	{
//		_chdir( g_CWD );
//
//		char filename[256] = { 0, };
//		char buffer[256] = { 0, };
//		char computerName[256] = { 0, };
//
//		struct _finddata_t	FileData;
//		long				hFile;
//
//		//-----------------------------------------------------------------

//		//-----------------------------------------------------------------
//		if ( (hFile = _findfirst( "Data\\UI\\txt\\Log*.txt", &FileData )) != -1L )
//		{
//			do
//			{
//				sprintf(filename, "Data\\UI\\txt\\%s", FileData.name);
//				
//				std::ifstream file( filename, ios::binary );
//				file.seekg( 0, ios::end );
//				
//				long fp = file.tellg();
//				
//				file.close();
//				if (fp == 0)
//				{
//					_chmod( filename, _S_IREAD | _S_IWRITE );
//					remove(filename);	
//				}
//				else
//				{
//					//copy
//					if (computerName[0]=='\0')
//					{
//						unsigned long numSize = 255;
//						GetComputerName( computerName,  &numSize );
//					}
//					

//					
//					_chmod( filename, _S_IREAD | _S_IWRITE );
//					rename(filename, buffer);						
//				}
//			}
//			
//			
//			while (_findnext( hFile, &FileData ) == 0);
//			
//			_findclose( hFile );			
//		}
//	}

	
	void
	CheckLogFile()
	{
		_chdir( g_CWD );

		char filename[256] = { 0, };
		char buffer[256] = { 0, };
		char computerName[256] = { 0, };

		struct _finddata_t	FileData;
		long				hFile;

		_mkdir( "Log" );
		
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		if ( (hFile = _findfirst( "Log\\Log*.txt", &FileData )) != -1L )
		{
			do
			{
				sprintf(filename, "Log\\%s", FileData.name);
				
				std::ifstream file( filename, ios::binary );
				file.seekg( 0, ios::end );
				
				long fp = file.tellg();
				
				if (fp < 39)
				{
					_chmod( filename, _S_IREAD | _S_IWRITE );
					remove(filename);	
				}
				else
				{
					file.seekg( -41, ios::end );
					file.read((char*)buffer, 24);
					buffer[24] = '\0';
					file.close();
					
					if (strcmp(buffer, "Delete DebugMessageArray")==0)
					{
						_chmod( filename, _S_IREAD | _S_IWRITE );
						remove(filename);
					}
					else
					{
						//copy
//						if (computerName[0]=='\0')
//						{
//							unsigned long numSize = 255;
//							GetComputerName( computerName,  &numSize );
//						}
//						

//						
//						_chmod( filename, _S_IREAD | _S_IWRITE );
//						rename(filename, buffer);						
					}
				}
			}
			
			
			while (_findnext( hFile, &FileData ) == 0);
			
			_findclose( hFile );			
		}
	}
	
	//-----------------------------------------------------------------------------
	// Check Flush LogFile
	//-----------------------------------------------------------------------------
	void
	CheckFlushLogFile()
	{
		//#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		const DWORD flushDelay = 3*60*1000;
		static DWORD flushTime = g_CurrentTime + flushDelay;

			if (g_CurrentTime > flushTime)
			{
				if (g_pDebugMessage!=NULL)
				{
					char logFile[128];
					strcpy(logFile, g_pDebugMessage->GetFilename());
				
					if (g_pDebugMessage!=NULL)
					{
						DEBUG_ADD("--------------- Delete DebugMessageArray --------------");		
						delete g_pDebugMessage;	
						g_pDebugMessage = NULL;
					}
					_chmod( logFile, _S_IREAD | _S_IWRITE );
					remove( logFile );

					sprintf(logFile, "%s\\Log\\Log%d.txt", g_CWD, timeGetTime());
					g_pDebugMessage = new CMessageArray;
					g_pDebugMessage->Init(MAX_DEBUGMESSAGE, 256, logFile);

					DEBUG_ADD_FORMAT("[Time = %d]", g_CurrentTime);
				}

				
				flushTime = g_CurrentTime + flushDelay;
			}
		//#endif
	}
#endif

// HHOOK g_hHook = NULL;

//LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
//{
//    BOOL bSysKeyDown = FALSE;
//    BOOL bWinKeyDown = FALSE;
//    
//    if(nCode == HC_ACTION)
//    {
//        KBDLLHOOKSTRUCT* kbhook = (KBDLLHOOKSTRUCT*)lParam;
//

//        if(kbhook->flags & LLKHF_ALTDOWN)
//        {
//            switch(kbhook->vkCode)
//            {
//            case VK_TAB:
//            case VK_F4:
//			case VK_ESCAPE:
//                bSysKeyDown = TRUE;
//                break;
//            }
//        }

//		else if(((GetAsyncKeyState( VK_CONTROL ) & 0x8000) || g_pSDLInput != NULL && (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))) && kbhook->vkCode == VK_ESCAPE)
//		{
//			SHORT control = GetAsyncKeyState( VK_CONTROL );
//			bSysKeyDown = TRUE;
//		}
//        else
//        {
//            switch(kbhook->vkCode)
//            {
//            case VK_LWIN:
//            case VK_RWIN:
//                bWinKeyDown = TRUE;
//                break;
//            }
//        }
//        
//        if(bSysKeyDown || bWinKeyDown)
//            return 1;
//    }
//
//    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
//}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Initialization, message loop
//-----------------------------------------------------------------------------
int PASCAL
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{	
	ResetStartupTrace();
	ResetLoginFlowTrace();
	TraceStartup("WinMain begin");
	EnableDpiAwareProcess();
	timeBeginPeriod(1);
	ApplyConfiguredGameResolution();
// 	char tttt[] = "0000000011";
// 	memcpy(lpCmdLine,tttt,strlen(tttt));
	//tttt += lpCmdLine;
#ifdef OUTPUT_DEBUG
	//lpCmdLine="";
	//MessageBox(0,"Winmain","",MB_OK);
	std::string tttt = "0000000031";
	tttt += lpCmdLine;
#endif

	//add by Soargon
	DEVMODE   DevMode; 
	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &DevMode );
	TraceStartup("display settings read");
	if(DevMode.dmBitsPerPel != 16)
	{
		DEVMODE	tempDevMode = DevMode;
		tempDevMode.dmBitsPerPel = 16;
		ChangeDisplaySettings( &tempDevMode, CDS_RESET );
	}
	//end
	//add by zdj
	bool cmpFullScreen = false;
	size_t cmdLineLength = (lpCmdLine != NULL) ? strlen(lpCmdLine) : 0;
	char launchMode = (cmdLineLength > 0) ? lpCmdLine[cmdLineLength - 1] : '1';
	TraceStartup("command line parsed");
	if (launchMode == '1')
	{
		g_MyFull=false;
		cmpFullScreen = false;
	}
	else if (launchMode == '2')
	{
		g_MyFull=false;
		cmpFullScreen=true;
	}
	// add by Sonic 2006.9.26
	else if (launchMode == '3')
	{
		g_MyFull=true;
		cmpFullScreen=false;
		ApplyConfiguredGameResolution();
	}
	else if (launchMode == '4')
	{
		g_MyFull=true;
		cmpFullScreen=true;
		ApplyConfiguredGameResolution();
	}
	if (cmdLineLength > 0 && launchMode >= '1' && launchMode <= '4')
	{
		lpCmdLine[cmdLineLength - 1] = 0x0;
	}

	//std::string tttt = "NEWSTART0";
	//tttt = lpCmdLine + tttt;
	//lpCmdLine = (char *)tttt.c_str();

	/*
	CIndexSpritePack ISPK;
	std::ifstream ispkFile("f:\\vcpp\\clientinfo\\Data\\AppendPatch\\New18Creature.ispk", ios::binary);
	ISPK.LoadFromFile(ispkFile);
	ispkFile.close();

	std::ofstream ispkFile2("f:\\vcpp\\clientinfo\\Data\\AppendPatch\\New18Creature.ispk", ios::binary);
	std::ofstream ispkFile3("f:\\vcpp\\clientinfo\\Data\\AppendPatch\\New18Creature.ispki", ios::binary);
	ISPK.SaveToFile(ispkFile2, ispkFile3);
	ispkFile2.close();
	ispkFile3.close();
	*/
		
	

	
	//----------------------------------------------------------
	
	//----------------------------------------------------------
	
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = FALSE;


#ifndef OUTPUT_DEBUG
	
	HANDLE hMutex = CreateMutex(&sa, FALSE, "<<<DarkEden>>>");  
	TraceStartup("mutex created");
	/*
	HANDLE hMutex = CreateMutex(&sa, FALSE, "<<<DarkEden>>>");   
	

	if (GetLastError()==ERROR_ALREADY_EXISTS)
	{
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage!=NULL)
			{
				DEBUG_ADD("DarkEden is Already Running..."); 
				DEBUG_ADD("--------------- Delete DebugMessageArray --------------");		
				delete g_pDebugMessage;	
				g_pDebugMessage = NULL;
			}
		#endif

		ReleaseMutex( hMutex );


		#ifndef _DEBUG
			return -1;
		#endif
	}
	
	
	if( FindWindow( "DK2", "DK2" ) != NULL )
	{
		return -1;
	}
	*/
	// end Coffee 2006.11.07  
	if( FindWindow( "PROCEXPL", "" ) != NULL )
	{
		return -1;
	}

/*	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);
	if((CurTime.wMonth == 1) || (CurTime.wMonth == 3) || (CurTime.wMonth == 5))
	{
		return -1;
	}*/

#endif
	
		
//	strcpy(g_CWD, __argv[0]);
 	GetModuleFileName(NULL, g_CWD, _MAX_PATH);
	char *tempCut = strrchr(g_CWD, '\\');
	if(tempCut == NULL)
		return FALSE;

	*tempCut = '\0';

//	GetCurrentDirectory( _MAX_PATH, g_CWD );

	
	SetCurrentDirectory(g_CWD);
	TraceStartup("current directory set");
	

//	if (_access(UPDATER_NEW_FILENAME, 0) == 0/* && _access(UPDATER_FILENAME, 0)*/)//updaterNewFile)
//	{	

//		if (remove( UPDATER_FILENAME )==0)
//		{

//			if (rename( UPDATER_NEW_FILENAME, UPDATER_FILENAME )==0)
//			{
//				DEBUG_ADD("Update Updater.exe OK"); 
//			}
//		}

////		//remove( UPDATER_FILENAME );

////		if (rename( UPDATER_NEW_FILENAME, UPDATER_FILENAME )==0)
////		{
////			DEBUG_ADD("Update Updater.exe OK"); 
////		}
//	}	

	

#ifdef __METROTECH_TEST__
	CheckLogFile();
#endif

#ifdef DEBUG_INFO
	InitDebugInfo();
#endif
	
#ifdef DEBUG_INFO
	switch(g_nKeyMapSelect)
	{
	case 0:
		g_wAuthKeyMap = 0x5154;
		break;
	case 1:
		break;
	default:
		break;
	}
#else
//yckou:update the update.exe program
/*
	CFileFind finder;
	BOOL bWorking = finder.FindFile("Updater2.exe");
*/
	WIN32_FIND_DATA FileData1; 
	HANDLE hSearch1; 
	DWORD pid;
	HANDLE hProcess=NULL,hProcessToken=NULL;
	HWND hwndUpdate = FindWindow("#32770", "FsDarkedenLaunch");
		if(hwndUpdate != NULL)
		{
				pid =0;
				GetWindowThreadProcessId(hwndUpdate, &pid);
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				TerminateProcess(hProcess, 0);
				int a = SendMessage(hwndUpdate, WM_CLOSE, 0 , 0);
		}
		Sleep(1000);
	hSearch1 = FindFirstFile("Updater2.exe", &FileData1); 
	if(hSearch1 != INVALID_HANDLE_VALUE)
	{
		DeleteFile("Updater.exe");
		CopyFile("Updater2.exe","Updater.exe",FALSE);
		DeleteFile("Updater2.exe");
	}
//yckou end
//add by sonic Check *.dll have Bug. 2006.4.13
//yckou begin: check invalid *.dll
#ifndef PLATFORM_WINDOWS
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 
	bool fFinished = false; 

	std::string InvalidDll;
	hSearch = FindFirstFile("*.dll", &FileData); 
	if (hSearch != INVALID_HANDLE_VALUE) 
	{
		while (!fFinished) 
		{
			int iLen = strlen(FileData.cFileName);
			for (int j=0;j<iLen;j++)
			{
				if(isupper(FileData.cFileName[j]) != 0)
					FileData.cFileName[j] = tolower(FileData.cFileName[j]);
			}
			InvalidDll = FileData.cFileName;

			if(InvalidDll != "timer.dll" &&
				InvalidDll != "msvcrtd.dll" &&
				InvalidDll != "msvcrt.dll" &&
				InvalidDll != "msvcp60.dll" &&
				InvalidDll != "msvcirt.dll" &&
				InvalidDll != "ifc22.dll" &&
				InvalidDll != "gl.dll" &&
				InvalidDll != "dsetup32.dll" &&
				InvalidDll != "dsetup.dll" &&
				InvalidDll != "deutil.dll" &&
				InvalidDll != "basics.dll" &&
				InvalidDll != "npx.dll" &&
				InvalidDll != "npupdate0.dll" &&
				InvalidDll != "nppsk.dll" &&
				InvalidDll != "npgmup.dll" &&
				InvalidDll != "npfgm9x.dll" &&
				InvalidDll != "npcipher.dll" &&
				InvalidDll != "mother.dll" &&
				InvalidDll != "fl.dll" &&
				InvalidDll != "dbghelp.dll" &&
				InvalidDll != "darkedenweb.dll" &&
				InvalidDll != "npchk.dll" &&
				InvalidDll != "xerces-c_2_4_0.dll")
				//MessageBox(0,(LPCTSTR)InvalidDll.c_str(),"ERROR",MB_OK);
				//MessageBox(0,
				return -1;
				
		
			if (!FindNextFile(hSearch, &FileData)) 
			{
				fFinished = true; 
			}
		} 
		// Close the search handle. 
		FindClose(hSearch);
	}
#endif
//yckou end
//end sonic

#endif

	 

	/*
	std::ifstream file("ImageObject.spk", ios::binary);
	std::ofstream file2("ImageObject-2.spk", ios::binary);
	std::ofstream file2index("ImageObject-2.spki", ios::binary);
	CSpritePack SPK;
	SPK.LoadFromFile(file);
	SPK.SaveToFile(file2, file2index);
	file.close();
	file2.close();
	file2index.close();

	return 0;
	*/

	/*
	OSVERSIONINFO        osVer;
    
    // First get the windows platform
    osVer.dwOSVersionInfoSize = sizeof(osVer);

    if( !GetVersionEx( &osVer ) )
    {
        return 0;
    }

    if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
		g_WindowsNT = TRUE;
	}
	else 
	{
		g_WindowsNT = FALSE;
	}
	*/
	
	//OutputMemorySize();

//	return 0;

	/*
	std::ifstream file("f:\\vcpp\\clientexpo\\data\\image\\creatureshadow.cfpk", ios::binary);
	CCreatureFramePack CFPK;
	CFPK.LoadFromFile(file);
	file.close();

	CCreatureFramePack CFPK2;
	CFPK2.Init( CFPK.GetSize()-3 );

	for (int i=0; i<CFPK2.GetSize(); i++)
	{
		CFPK2[i] = CFPK[i];
	}

	std::ofstream file2("f:\\CreatureShadow.cfpk", ios::binary);
	std::ofstream fileindex2("f:\\CreatureShadow.cfpki", ios::binary);
	CFPK2.SaveToFile(file2, fileindex2);
	file2.close();
	fileindex2.close();
	*/
	
	//-----------------------------------------------------------------------
	
	//-----------------------------------------------------------------------
	g_bNeedUpdate = FALSE;
//	OutPutClassSize();
//	return 0;

	#ifdef OUTPUT_DEBUG
	
	OutPutClassSize();
		if (g_pDebugMessage!=NULL)
		{
			delete g_pDebugMessage;		
		}
		g_pDebugMessage = new CMessageArray;

		//---------------------------------------------------
		// Debug Log File Init...
		//---------------------------------------------------
		if (g_pDebugMessage!=NULL)
		{
			char logFile[128];
			sprintf(logFile, "%s\\Log\\Log%d.txt", g_CWD, timeGetTime());			

//			#include <fcntl.h>
//			char clogFile[128];
//			sprintf(clogFile, "%s\\Data\\UI\\txt\\Logc%d.txt", g_CWD, timeGetTime());			
//			CLogFile = _open(clogFile, _O_WRONLY | _O_TEXT | _O_CREAT | _O_TRUNC);


			g_pDebugMessage->Init(MAX_DEBUGMESSAGE, 256, logFile);

			DEBUG_ADD_FORMAT("[ Compile Time : %s ]", __TIMESTAMP__);
		}
	#endif
	

	
	//return 0;

	//g_MemLogFile = _open("memlog.txt", _O_WRONLY | _O_TEXT | _O_CREAT | _O_TRUNC);

	#ifdef _DEBUG
		int tmpDbgFlag;
		tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
		tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
		//_CrtSetDbgFlag(tmpDbgFlag);
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | tmpDbgFlag);
		

		_CrtSetReportMode ( _CRT_WARN, _CRTDBG_MODE_DEBUG );
		_CrtSetReportMode ( _CRT_ERROR,	_CRTDBG_MODE_DEBUG );
		_CrtSetReportMode ( _CRT_ASSERT,_CRTDBG_MODE_DEBUG );
	#endif	
	

	g_hInstance = hInstance;
	// random
	srand(time(NULL));
	TraceStartup("instance initialized");

	
	

	//---------------------------------------------------
	// SpritePack Loading Test
	//---------------------------------------------------
	/*
	CSpritePack tileSPK;
	std::ifstream	fileSPK;//(FILE_ISPRITE_ADDON, ios::binary);
	if (!FileOpenBinary(FILE_SPRITE_TILE, fileSPK))
		return false;
	tileSPK.LoadFromFile(fileSPK);
	fileSPK.close();
	*/

	g_pFileDef = new Properties;
	g_pFileDef->load(FILE_INFO_FILEDEF);
	TraceStartup("file definitions loaded");

	
	bool bNetmarble = false;
	Properties NetmarbleConfig;

	NetmarbleConfig.load(g_pFileDef->getProperty("FILE_INFO_NETMARBLE").c_str());
	bNetmarble = NetmarbleConfig.getPropertyInt("Netmarble") != 0;
	TraceStartup("netmarble config loaded");

	if(bNetmarble)
	{
		// Registry access removed - use configuration file instead
		// Netmarble-specific registry key: HKEY_LOCAL_MACHINE\SOFTWARE\Netmarble\NetmarbleDarkEden
		// This was used to set "RunFileName" for auto-updater
		// TODO: Implement config file-based approach if needed
	}
//	if( bNetmarble == false )
//		return -1;


	

	//MessageBox(g_hWnd, lpCmdLine, NULL, MB_OK);
		
	//------------------------------------------------------
	
	//------------------------------------------------------
	if (strlen(lpCmdLine)==0)
	{
		#if defined(USE_UPDATER) && !defined(PLATFORM_WINDOWS)
			
#ifndef OUTPUT_DEBUG
			if (!ReleaseMutex( hMutex ))
			{
				DEBUG_ADD_FORMAT("Failed to release mutex : %d", GetLastError()); 
			}
#endif

			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage!=NULL)
				{
					//MessageBox(0,"Error:[g_pDebugMessage!=NULL]","Error",MB_OK);
					DEBUG_ADD("No CommandLine --> Execute Updater.exe"); 
					DEBUG_ADD("--------------- Delete DebugMessageArray --------------");		
					delete g_pDebugMessage;	
					g_pDebugMessage = NULL;
				}
			#endif


			
			ReleaseAllObjects();			
			



			//_spawnl(_P_NOWAIT, "Updater.exe", "Updater.exe", NULL);
			_chdir( g_CWD );
			
			_spawnl(_P_OVERLAY, UPDATER_FILENAME, UPDATER_FILENAME, lpCmdLine, NULL);

//			ShellExecute(g_hWnd, NULL, UPDATER_FILENAME, lpCmdLine, NULL, SW_SHOW);
			return FALSE;

		#else

			//strcpy(g_ServerIP, SERVER_IP);
			g_MaxNPC		= 0;//MAX_NPC;
			
			#ifdef _DEBUG
				g_bFullScreen	= false;
			#else
			#ifdef WINDOWS_SCREEN_DISPLAY
				g_bFullScreen	= false;
			#else
				g_bFullScreen	= true;
			#endif
			#endif
			g_bHAL			= true;
			g_bMusicSW		= true;		
			TraceStartup("empty command line accepted");
			
		#endif
	}
	//------------------------------------------------------
	
	//------------------------------------------------------
	else
	{
		char* str = lpCmdLine;
		if (strlen(str) < 8)
		{
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage!=NULL)
				{
					//MessageBox(0,"Error [strlen(str) < 5]","Error",MBOK);
					DEBUG_ADD("Too short Commandline --> Run Updater.exe"); 
					DEBUG_ADD("--------------- Delete DebugMessageArray --------------");		
					delete g_pDebugMessage;	
					g_pDebugMessage = NULL;
				}
			#endif

#ifndef OUTPUT_DEBUG
			ReleaseMutex( hMutex );
#endif

			ReleaseAllObjects();

			

			//_spawnl(_P_NOWAIT, "Updater.exe", "Updater.exe", NULL);	
			_chdir( g_CWD );
			
			_spawnl(_P_OVERLAY, UPDATER_FILENAME, UPDATER_FILENAME, lpCmdLine, NULL);
//			ShellExecute(g_hWnd, NULL, UPDATER_FILENAME, lpCmdLine, NULL, SW_SHOW);

			return FALSE;
		}

		bool bCheck = true;
		bool bRunUpdater = false;
		//char checkStr[9] = "NEWSTART";
		//char checkStr[9] = "START";

		
		DWORD GetCommand;
		// EXECryptor_GetHardwareID() removed (SDL2) - Copy protection no longer needed
		GetCommand = 0x00000000;  
		char checkStr[9];
		memset(checkStr,0,9);
		sprintf(checkStr, "%X" ,GetCommand);
		char T_checkStr[9];
		memset(T_checkStr,0,9);
		if (8 -strlen(checkStr) !=0)
		{
			memcpy(T_checkStr,checkStr,strlen(checkStr));
			int n = 8-strlen(checkStr);
			char* p=checkStr;
			for (int i =0;i<n;i++)
			{
				memcpy(p,"0",1);
				p++;
			}
			memcpy(p,T_checkStr,strlen(T_checkStr));
		}
		memcpy(checkStr,"00000000",8);
		memcpy(T_checkStr,str,8);
		//strcpy(checkStr,str);
		int i = 0;

#ifdef OUTPUT_DEBUG
		bRunUpdater = false;
#else
		////////////////////Add by Sonic 2006-6-21////////////////////////////////
		if (memcmp(checkStr,T_checkStr,strlen(checkStr)))
		{
			//MessageBox(0,checkStr,str,MB_OK);
			
			return false;
			bRunUpdater = true;
			//break;
		}
		//////////////////////////////////////////////////////////////////////////
		/* Delete by Sonic 2006-6-21
		for (int i=0; i<strlen(checkStr); i++)
		{
			if (checkStr[i]!=str[i])
			{
				bRunUpdater = true;

				break;
			}
		}
		*/
		if(bRunUpdater == false && !bNetmarble && lpCmdLine[8] >= '0' && lpCmdLine[8] <= '9')
		{
			for( int i = 9; ; i++ )
			{
				//g_Dimension = lpCmdLine[8]-'0';
				if ( lpCmdLine[i] == NULL )
					break;
				
				if ( lpCmdLine[i] >= '0' && lpCmdLine[i] <= '9' )
				{
					continue;
				}
				else
					break;
			}
			if (memcmp(checkStr,str,strlen(checkStr)))
			{
				char szDimen[10];
				memset( szDimen, 0, 10 );
				memcpy( szDimen, "8", 1 );
				g_Dimension = atoi( szDimen );
			}
			else
			{
				char szDimen[10];
				memset( szDimen, 0, 10 );
				memcpy( szDimen, lpCmdLine+8, 1 );
				g_Dimension = atoi( szDimen );
			}
		//	g_Dimension = lpCmdLine[8]-'0';
		}
#endif
//add by Sonic 2006.7.26
	
		 
		#ifdef OUTPUT_DEBUG
			
			g_bTestMode = true;
			char checkTestMode[] = "TestMode";
			for (int i=0; i<strlen(checkTestMode); i++)
			{
				if (checkTestMode[i]!=str[i])
				{
					g_bTestMode = false;

					break;
				}
			}

			if (g_bTestMode)
			{
				bRunUpdater = false;
			}
		#endif
			 
/////////////////////////////edit by sonic 2006.6.20///////////////////////
		if (bRunUpdater)
		{
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage!=NULL)
				{
					//MessageBox(0,"Error [g_pDebugMessage!=NULL]","Error",MBOK);
					DEBUG_ADD("Wrong Commandline --> Run Updater.exe"); 
					DEBUG_ADD("--------------- Delete DebugMessageArray --------------");		
					delete g_pDebugMessage;	
					g_pDebugMessage = NULL;
				}
			#endif			
				
#ifndef OUTPUT_DEBUG
			ReleaseMutex( hMutex );
#endif

			ReleaseAllObjects();

			//bCheck = false;
			

			//_spawnl(_P_NOWAIT, "Updater.exe", "Updater.exe", NULL);	
			_chdir( g_CWD );

			
			
//			char szTemp[512];
//			sprintf(szTemp, "%s %s", UPDATER_FILENAME, lpCmdLine);
//			DWORD error = WinExec(szTemp, SW_SHOW);
//			switch(error)
//			{
//			case 0:
//				MessageBox(NULL, "The system is out of memory or resources. ", PROGRAM_TITLE, MB_OK);
//				break;
//
//			case ERROR_BAD_FORMAT:
//				MessageBox(NULL, "The .exe file is invalid (non-Win32 .exe or error in .exe image). ", PROGRAM_TITLE, MB_OK);
//				break;
//
//			case ERROR_FILE_NOT_FOUND:
//				MessageBox(NULL, "The specified file was not found. ", PROGRAM_TITLE, MB_OK);
//				break;
//
//			case ERROR_PATH_NOT_FOUND:
//				MessageBox(NULL, "The specified path was not found. ", PROGRAM_TITLE, MB_OK);
//				break;
//			}
			
			_spawnl(_P_OVERLAY, UPDATER_FILENAME, UPDATER_FILENAME, lpCmdLine, NULL);
//			ShellExecute(g_hWnd, NULL, UPDATER_FILENAME, lpCmdLine, NULL, SW_SHOW);

			return FALSE;
		}
///////////////////////////////////end edit by sonic 2006.6.20///////////////////////
		#ifdef WINDOWS_SCREEN_DISPLAY
		g_bFullScreen	= false;//false;
		#else
		g_bFullScreen	= true;//false;  Z
		#endif
		
		//strcpy(g_ServerIP, SERVER_IP);
		g_MaxNPC		= 0;//MAX_NPC;

		g_bHAL			= true;
		g_bMusicSW		= true;

		#ifdef OUTPUT_DEBUG
			if (g_bTestMode)
			{
				g_bFullScreen	= false;
			}
		#endif
	}
	

	#ifdef OUTPUT_DEBUG
		//g_bFullScreen	= true;
	#endif

	
	GetFutecAddress(lpCmdLine);



	//------------------------------------------------------------------------
	
	
	//------------------------------------------------------------------------
//	bool bPatched = ApplyPatch();

	//------------------------------------------------------------------------
	
	//------------------------------------------------------------------------
//	ConvertScreenEffect();

	//------------------------------------------------------------------------
	
	
	
	//------------------------------------------------------------------------
	//AppendScreenEffect();

	if ( cmpFullScreen )
		{
			g_bFullScreen = true;
		}
		else
			g_bFullScreen = false;
	#ifdef _DEBUG
		g_bFullScreen = false;
	#endif
	
//end 
	if (!CheckTerriblePatch())
	{
		ReleaseAllObjects();

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage!=NULL)
			{
				delete g_pDebugMessage;	
				g_pDebugMessage = NULL;
			}
		#endif
	
#ifndef OUTPUT_DEBUG
		ReleaseMutex( hMutex );
#endif


		return 0;
	}
	

	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
//	std::ifstream updaterNewFile(UPDATER_NEW_FILENAME, ios::binary | );

	
	if (!_access(UPDATER_NEW_FILENAME, 0))//updaterNewFile)
	{	
//		updaterNewFile.close();

		
		if (remove( UPDATER_FILENAME )==0)
		{
			
			if (rename( UPDATER_NEW_FILENAME, UPDATER_FILENAME )==0)
			{
				//MessageBox(0,"Error:[rename( UPDATER_NEW_FILENAME, UPDATER_FILENAME )==0]","Error",MB_OK);
				DEBUG_ADD("Update Updater.exe OK"); 
			}
		}
	}	

	//strcpy(g_ServerIP, SERVER_IP);
	//---------------------------------------------------
	
	//---------------------------------------------------
	
	//---------------------------------------------------
	char CWD[_MAX_PATH];

	
	if (_getcwd( CWD, _MAX_PATH )!=NULL)
	{	
		char UpdateDir[_MAX_PATH];

		int lenCWD = strlen(CWD);
		if (lenCWD > 1)
		{
			if (CWD[lenCWD-1]=='\\')
			{
				sprintf(UpdateDir, "%s%s", CWD, DIRECTORY_UPDATE);
			}
			else
			{
				sprintf(UpdateDir, "%s\\%s", CWD, DIRECTORY_UPDATE);
			}
		}
		
		if (_rmdir( UpdateDir )!=0)
		{			
			
			if (errno==ENOENT)
			{				
			}
			
			else //if (errno==ENOTEMPTY)
			{
				
				if (_chdir( UpdateDir ) == 0)
				{
					//---------------------------------------------------
					
					//---------------------------------------------------
					struct _finddata_t	FileData;
					long				hFile;

					
					if( (hFile = _findfirst( "*.*", &FileData )) != -1L )					
					{
						while (_findnext( hFile, &FileData ) == 0)
						{
							
							if (FileData.name[0] != '.')
							{
								remove( FileData.name );
							}
						}

						
						_findclose( hFile );			
					}
					
					_chdir( CWD );

					if (_rmdir( UpdateDir )==0)
					{
						
					}
				}				
			}
		}
	}

	
	RemoveProgressBar();
	TraceStartup("progress bar removed");

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
//	if (bPatched)
//	{
//		ShowPatchLogWindow();
//	}	


	BOOL bBadTimer = FALSE;

    MSG                         msg;

	NETMARBLE_INFO NetmarbleInfo;
	REALSERVER_INFO RealServerInfo;
	if(bNetmarble)
	{
		if(ParsingNetmarble(lpCmdLine, NetmarbleInfo) == false)
		{
			if(NetmarbleConfig.getPropertyInt("Netmarble") == 1)
			{
				//add by sonic 2006.4.11
				
				//end

				return -1;
			}

			bNetmarble = false;
		}
	} else
	{
//		 //test  mode|demention|ID|key
//		char szBuf[1024]; 
//		sprintf(szBuf, "%s", lpCmdLine); 
//		MessageBox(NULL, szBuf, PROGRAM_TITLE, MB_OK);// ??
		if(ParsingRealServer(lpCmdLine, g_Dimension, RealServerInfo) == false)
		{
//#ifndef OUTPUT_DEBUG

//			ShellExecute(NULL, NULL, "www.darkeden.com", NULL, NULL, SW_SHOW);
//			return -1;
//
//#endif
		}
	}
	
 
_APICheck.init();
	TraceStartup("api check initialized");
/* ************************************* */
//	FILE *fp = fopen("Data\\Info\\SuperUser.inf","rt");	
//	MessageBox(NULL,__TIME__,__TIME__,MB_OK);
//	if(fp != NULL)
//	{
//		char szLine[512];
//		fgets(szLine,512,fp);
//		char szCompare[512];
//		bool	superUser = true;
//		wsprintf(szCompare,"%d%s",__LINE__,__TIME__);
//		MessageBox(NULL,szCompare,szCompare,MB_OK);
//		if(strcmp(szLine,szCompare))
//			superUser = false;
//		fgets(szLine,512,fp);
//		fclose(fp);
//		
//		int Code=0;
//		char *tempCode = new char[strlen(szCompare)];
//		memset(tempCode, 0, strlen(szCompare) );
//		for(int i=0;i<strlen(szCompare);i++)
//		{
//			if( i != 0 )
//				tempCode[i] = szCompare[1] + szCompare[i];
//			else
//				tempCode[i] = szCompare[0] + szCompare[i];			
//		}
//		MessageBox(NULL, tempCode, tempCode, MB_OK);
//		if(strcmp(tempCode,szLine))
//			superUser = false;
//		delete []tempCode;	
//
//		std::string myIP;
//		
//		WORD wVersionRequested; 
//		WSADATA wsaData; 
//		char name[255]; 
//		PHOSTENT hostinfo; 
//		wVersionRequested = MAKEWORD( 2, 0 ); 
//		if ( WSAStartup( wVersionRequested, &wsaData ) == 0 ) 
//		{
//			if( gethostname ( name, sizeof(name)) == 0) 
//			{
//				if((hostinfo = gethostbyname(name)) != NULL) 
//					myIP= inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list); 
//				else
//					superUser = false;
//			} else superUser = false;
//			WSACleanup( ); 
//		} else
//			superUser = false;
//		if(strstr(myIP.c_str(),"220.117.154")==NULL)
//			superUser = false;
//
//		MessageBox(NULL, myIP.c_str(), myIP.c_str(), MB_OK);
//		if(superUser)
//		{
//			g_bSuperUser = true;			
//		} else
//			g_bSuperUser = false;
//	}

	DARKEDEN_LANGUAGE Language = CheckDarkEdenLanguage();
	TraceStartup("language checked");
	
	switch( Language )
	{
	default :
		gC_ci = new CI_ENGLISH;
		break;
	}
	TraceStartup("language interface created");
	
#ifndef _DEBUG
	if( gC_ci->IsKorean() == true )
		InitCrashReport();
#endif
	TraceStartup("before InitApp");
	if (InitApp(nCmdShow))
	{
		TraceStartup("InitApp succeeded");
		// SDL2 doesn't provide video memory queries like DirectX
		// Use a reasonable default value for all platforms
		g_dwVideoMemory = 256 * 1024 * 1024;  // 256 MB default

		
		if(bNetmarble)
		{
			g_pUserInformation->NetmarbleID = NetmarbleInfo.ID;
			g_pUserInformation->NetmarblePassword = NetmarbleInfo.Password;
			g_pUserInformation->WorldID = NetmarbleInfo.WorldID;
			g_pUserInformation->ServerID = NetmarbleInfo.ServerID;
//			g_pUserInformation->GoreLevel = NetmarbleInfo.bGore;
			g_pUserInformation->bNetmarbleGoreLevel = NetmarbleInfo.bGore;
			g_pUserInformation->IsNetmarble = bNetmarble;
		}
		else
		{
			// mode|demention|ID|key
			g_pUserInformation->IsAutoLogIn			= RealServerInfo.bMode;
			if(true == g_pUserInformation->IsAutoLogIn)
			{
				g_pUserInformation->UserID				= RealServerInfo.ID;
				g_pUserInformation->AutoLogInKeyValue	= RealServerInfo.Key;
			
				g_Dimension = RealServerInfo.WorldID;

#ifdef OUTPUT_DEBUG
				char szBuf[1024];
				sprintf(szBuf, "mode = %d, id = %d,world = %s,key = %s", g_pUserInformation->IsAutoLogIn,
																		

	g_Dimension,
																		

	g_pUserInformation->UserID.GetString(),
																		

	g_pUserInformation->AutoLogInKeyValue.GetString());
				DEBUG_ADD(szBuf);
#endif
			}			
			
		
		}

		// Keep the legacy Korean protocol/config path for server compatibility.
		// UI/input language is forced independently via CI_ENGLISH.
		g_pUserInformation->SetKorean();

//		#if !defined(OUTPUT_DEBUG) && !defined(__DEBUG_OUTPUT__)
//		g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyboardHook, hInstance, 0);
//
//		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, NULL, NULL); 
//		#endif

		//static DWORD lastTime=1;
		//------------------------------
		
		//------------------------------
		if (!g_pTopView->IsInit())
		{
			TraceStartup("before top view init");
			g_pTopView->Init();			
			TraceStartup("top view initialized");
		
		}

		g_bActiveApp = TRUE;
		TraceStartup("entering message loop");
		//g_bActiveGame = TRUE;
		DEBUG_CMD(MIN_CLRSCR, "");
		DEBUG_CMD(MIN_SHOWWND, "------------------------------");
		

		while (TRUE)
		{
//			Sleep(1);	//add by viva
			 
				_APICheck.CheckApi();
			/* *************************** */
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			//if (GetMessage(&msg, NULL, 0, 0))
			{	
				if (!GetMessage(&msg, NULL, 0, 0))
					break;
					//return msg.wParam;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else if (g_bActiveApp
#ifdef OUTPUT_DEBUG
				|| g_bTestMode
#endif
				)
			{
	
				{						

					g_CurrentTime = timeGetTime();

					//if (g_CurrentTime - lastTime > g_UpdateDelay)
					{
#ifdef PLATFORM_WINDOWS
						if (g_pUpdate!=NULL)
						{
							
							CWinUpdate*	pCurrentUpdate = g_pUpdate;

							pCurrentUpdate->Update();

							#ifdef __METROTECH_TEST__
								CheckFlushLogFile();
							#endif

						}
#else
						// On non-Windows platforms, game update is handled differently
						// TODO: Implement SDL2-based game loop
#endif
						//lastTime = g_CurrentTime;
					}
// REMOVED: nProtect anti-cheat code (SDL migration - no longer needed)
					
					if( g_bForceExitBynProtect )
					{
						//MessageBox(0,"#3","#3",MB_OK);
						bBadTimer = TRUE;
						
						break;
					}

					//#ifdef OUTPUT_DEBUG
						DWORD timeGap = g_CurrentTime - g_StartTime;
							
						if (timeGap > 1000)
						{
							//UpdateFrame();
					
							g_FrameRate = (g_FrameCount - g_StartFrameCount) * 1000 / timeGap;

							
							g_bGoodFPS = (g_FrameRate >= g_FrameGood);
							
							g_StartTime = g_CurrentTime;
							g_StartFrameCount = g_FrameCount;
						}								
				}
			}
			else
			{
				// Make sure we go to sleep if we have nothing else to do
				WaitMessage();
			}
		}	

		#ifndef __OUTPUT_DEBUG__
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, FALSE, NULL, NULL); 
		#endif
	}
	else
	{		
	//	InitFail("Initialize Failed!");        
	}
	//add by Soargon
	ChangeDisplaySettings( &DevMode, CDS_RESET );
	timeEndPeriod(1);
	//end
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	if (bBadTimer)
	{
		if (g_pUIDialog!=NULL)
		{
#if defined(__NPROTECT_OLD_VERSION__)&&defined(__NPROTECT__)
			if( g_nProtectSTATUS != NPROTECT_NORMAL && g_nProtectSTATUS != NPROTECT_MAX )
			{
				g_pUIDialog->PopupFreeMessageDlg( g_nProtectMessage[g_nProtectSTATUS].c_str() );
			} else
				g_pUIDialog->PopupFreeMessageDlg( "    .   ,         bug@darkeden.com  .", -1, -1, 0 );
#endif

#if defined(__NPROTECT__)&&!defined(__NPROTECT_OLD_VERSION__)
			if( g_bForceExitBynProtect )
			{
				if( g_nProtectMessage.find( g_nProtectErrorMessage ) != g_nProtectMessage.end() )
				{
					char szTemp[256];
					wsprintf(szTemp,"%s [%d]", g_nProtectMessage[g_nProtectErrorMessage].c_str(), g_nProtectErrorMessage2 );
					g_pUIDialog->PopupFreeMessageDlg( szTemp );
				}
				else
				{
					char szTemp[256];
					wsprintf(szTemp,"nProtect     [%d][%d].  .",g_nProtectErrorMessage, g_nProtectErrorMessage2);
					g_pUIDialog->PopupFreeMessageDlg( szTemp );
				}
			}
#endif
				
//			if(g_bForceExitBynProtect2)
//			{

//			}else
//			{
//				if(g_bForceExitBynProtect)

//				else

//			}
//			//MessageBox(NULL, "Timer Error!", PROGRAM_TITLE, MB_OK | MB_TOPMOST);
			
			while (1)
			{
				UpdateInput();
				UpdateMouse();

//				if (true)
//				{
//					gC_vs_ui.Show();
//				}
//				else
				{	
					gC_vs_ui.Show();

					POINT point = { 0, 0 };
					RECT rect = { 0, 0, g_GameRect.right, g_GameRect.bottom };
					
					g_pBack->BltNoColorkey( &point, g_pLast, &rect );	
				}
				
				CSDLGraphics::Flip();

				if (g_pSDLInput->KeyDown(DIK_ESCAPE)
					|| g_pSDLInput->KeyDown(DIK_NUMPADENTER)
					|| g_pSDLInput->KeyDown(DIK_RETURN)
					|| g_pSDLInput->KeyDown(DIK_SPACE))
				{
					break;
				}
			}
		}
	}

	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------	
	ReleaseAllObjects();
#ifdef __WEB_BROWSER__
	if(NULL != g_pWebBrowser)
	{
		g_pWebBrowser->Quit();
		g_pWebBrowser->Release();
		DeleteObject(g_pWebBrowser);
		g_pWebBrowser= NULL;
		CoUninitialize();
	}
#endif
//#if defined(OUTPUT_DEBUG) && !defined(_DEBUG)
	delete gC_ci;
#ifdef OUTPUT_DEBUG
//	_close(CLogFile);
//	CheckCLogFile();
#endif

	#ifdef OUTPUT_DEBUG	
		if (g_bNeedUpdate)	
		{
			//MessageBox(0,"Error:[g_bNeedUpdate]","Error",MB_OK);
			DEBUG_ADD("Need to Update! Run Updater.exe"); 
		}

		DEBUG_ADD("---------------[  End ReleaseAllObjects  ]---------------");
	
		if (g_pDebugMessage!=NULL)
		{
			DEBUG_ADD("--------------- Delete DebugMessageArray --------------");		
			delete g_pDebugMessage;	
			g_pDebugMessage = NULL;
		}
	#endif
	DEBUG_CMD(MIN_HIDEWND, "------------------------------");


	if (g_pClientConfig!=NULL)
	{
		delete g_pClientConfig;
		g_pClientConfig = NULL;
	}

// REMOVED: nProtect anti-cheat code (SDL migration - no longer needed)


#ifndef OUTPUT_DEBUG
	ReleaseMutex( hMutex );
#endif

	
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	if (g_bNeedUpdate)
	{
		//_spawnl(_P_NOWAIT, UPDATER_FILENAME, UPDATER_FILENAME, "UPDATE", NULL);
		char szTemp[512];
		sprintf(szTemp, "UPDATE %s", lpCmdLine);
		_chdir( g_CWD );
		
		_spawnl(_P_OVERLAY, UPDATER_FILENAME, UPDATER_FILENAME, szTemp, NULL);		
//		ShellExecute(g_hWnd, NULL, UPDATER_FILENAME, szTemp, NULL, SW_SHOW);
	}	

	return 0;
}
BOOL GetSystem()
{
	OSVERSIONINFO OsInfo;
	OsInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&OsInfo))
	{
		// platform
		switch (OsInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_WINDOWS:
			if(OsInfo.dwMajorVersion == 3)
				
				
				return FALSE;
			else if(OsInfo.dwMajorVersion == 4)
				
				return FALSE;
				
			break;
		case VER_PLATFORM_WIN32_NT:
			if(OsInfo.dwMajorVersion == 5)
				
				return TRUE;
				
			else
				return TRUE;
				
				
			break;
		default:
			return FALSE;
			
			break;
		}
		// version and language
	}
	return FALSE;
}
