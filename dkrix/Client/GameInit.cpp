//---------------------------------------------------------------------------
// GameInit.cpp
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#ifdef PLATFORM_WINDOWS
#include <MMSystem.h>
#include <WinSock.h>
#include <SDL.h>
#else
// macOS: BSD sockets headers for network functions
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include "DebugLog.h"
#include "Client.h"
#include "GameObject.h"
#include "AddonDef.h"
#include "ServerInfo.h"
#include "PacketDef.h"
#include "VS_UI.h"
#include "COpeningUpdate.h"
#include "CGameUpdate.h"
#include "CWaitPacketUpdate.h"
#include "UserInformation.h"
#include "TempInformation.h"
#include "CWaitUIUpdate.h"
#include "UIDialog.h"
#include "MMusic.h"
#include "MGameStringTable.h"
#include "MMoneyManager.h"
#include "MPriceManager.h"
#include "MChatManager.h"
#include "MTradeManager.h"
#include "UIMessageManager.h"
#include "MStorage.h"
#include "MHelpManager.h"
#include "MHelpStringTable.h"
#include "MCompareManager.h"
#include "MHelpDisplayer.h"
#include "MObjectSelector.h"
#include "MLevelNameTable.h"
#include "MonsterNameTable.h"
#include "ExperienceTable.h"
#include "ServerInformation.h"
#include "GameObject.h"
#include "MZoneSoundManager.h"
#include "MNPCScriptTable.h"
#include "MParty.h"
#include "SpriteIDDef.h"
#include "MathTable.h"
#include "ModifyStatusManager.h"
#include "Packet/RequestServerPlayerManager.h"
#include "Packet/RequestClientPlayerManager.h"
#include "Packet/ClientCommunicationManager.h"
#include "KeyAccelerator.h"
#include "AcceleratorManager.h"
#include "WhisperManager.h"
#include "ProfileManager.h"
#include "MGuildMarkManager.h"
#include "MEventManager.h"
#include "RequestFileManager.h"
#include "RequestUserManager.h"
#include "MJusticeAttackManager.h"
#include "Profiler.h"
#include "WavePackFileManager.h"
//#include "MFileDef.h"
#include "MGuildInfoMapper.h"
#include "PCConfigTable.h"
#include "MAttachOrbitEffect.h"
#include "DebugInfo.h"
#include "MTestDef.h"
#include "CMP3.h"
#include "RankBonusTable.h"
#include "MMonsterKillQuestInfo.h"

static void TraceGameStartup(const char* step)
{
	(void)step;
}
#include "MTimeItemManager.h"
#include "FameInfo.h"
#include "MWarManager.H"
#include "CSprite555.h"
#include "CSprite565.h"
#include "Packet/Properties.h"
#include "UIFunction.h"
#include "SoundSetting.h"
#include "SystemAvailabilities.h"
#include "ShrineInfoManager.h"
#include "ServerInfoFileParser.h"

//yckou
#include "DebugKit.h"

extern BYTE g_macAddress[6];
extern BOOL GetMacAddressFromSock();
extern DWORD g_dwSeqNumL;
extern DWORD g_dwSeqNumG;
extern BOOL g_MyFull;
extern RECT g_GameRect;
//#include "CJpeg.h"
//#define	new			DEBUG_NEW



extern int					g_Dimension ;
// g_bEnable3DHAL was originally defined in VS_UI_Title.cpp when _LIB is not defined
// Now that VS_UI is built with _LIB defined, we need to define it here
BOOL g_bEnable3DHAL = FALSE;

//----------------------------------------------------------------------

//----------------------------------------------------------------------
//CSpritePack		g_TitleSPK;
//CSpritePack		g_TitleAniSPK;
//int				g_TitleSpriteID = 0;
//int				g_TitleSpriteDarkness = 4;
bool			g_bTitleLoading = false;
CSprite*		g_pTitleLoadingSprite = NULL;
CSpriteSurface*	g_pTitleLoadingSurface = NULL;
int				g_TitleSpriteAlpha = 32;
DWORD			g_TitleLoadingStartTime = 0;
//CDirectDrawSurface	g_TitleSurface;

static bool LoadStartupSplashSurface()
{
	const char* filename = "data\\image\\StartupSplash.bmp";
	SDL_Surface* loaded = SDL_LoadBMP(filename);
	if (loaded == NULL)
	{
		return false;
	}

	SDL_Surface* converted = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_RGB565, 0);
	SDL_FreeSurface(loaded);
	if (converted == NULL)
	{
		return false;
	}

	SDL_Surface* scaled = SDL_CreateRGBSurfaceWithFormat(0, g_GameRect.right, g_GameRect.bottom, 16, SDL_PIXELFORMAT_RGB565);
	if (scaled == NULL)
	{
		SDL_FreeSurface(converted);
		return false;
	}

	SDL_FillRect(scaled, NULL, 0);
	SDL_Rect splashRect = { 0, 0, g_GameRect.right, g_GameRect.bottom };
	if (SDL_BlitScaled(converted, NULL, scaled, &splashRect) != 0)
	{
		SDL_FreeSurface(scaled);
		SDL_FreeSurface(converted);
		return false;
	}
	SDL_FreeSurface(converted);

	CSpriteSurface* surface = new CSpriteSurface;
	if (!surface->InitOffsurface(g_GameRect.right, g_GameRect.bottom))
	{
		SDL_FreeSurface(scaled);
		delete surface;
		return false;
	}

	surface->FillSurface(CSDLGraphics::Color(0, 0, 0));

	S_SURFACEINFO info;
	surface->GetSurfaceInfo(&info);
	if (info.p_surface == NULL)
	{
		SDL_FreeSurface(scaled);
		delete surface;
		return false;
	}

	const int copyWidth = (scaled->w < info.width) ? scaled->w : info.width;
	const int copyHeight = (scaled->h < info.height) ? scaled->h : info.height;
	const int destX = (info.width - copyWidth) / 2;
	const int destY = (info.height - copyHeight) / 2;

	WORD* dest = (WORD*)info.p_surface;
	for (int y = 0; y < copyHeight; ++y)
	{
		const WORD* srcRow = (const WORD*)((const BYTE*)scaled->pixels + y * scaled->pitch);
		WORD* destRow = (WORD*)((BYTE*)dest + (destY + y) * info.pitch) + destX;
		memcpy(destRow, srcRow, copyWidth * sizeof(WORD));
	}

	SDL_FreeSurface(scaled);

	if (g_pTitleLoadingSurface != NULL)
	{
		delete g_pTitleLoadingSurface;
	}
	g_pTitleLoadingSurface = surface;
	return true;
}

static WORD Blend565(WORD dest, WORD src, int alpha)
{
	if (alpha >= 32)
	{
		return src;
	}
	if (alpha <= 0)
	{
		return dest;
	}

	int srcR = (src >> 11) & 0x1F;
	int srcG = (src >> 5) & 0x3F;
	int srcB = src & 0x1F;
	int destR = (dest >> 11) & 0x1F;
	int destG = (dest >> 5) & 0x3F;
	int destB = dest & 0x1F;

	int outR = (srcR * alpha + destR * (32 - alpha)) >> 5;
	int outG = (srcG * alpha + destG * (32 - alpha)) >> 5;
	int outB = (srcB * alpha + destB * (32 - alpha)) >> 5;

	return (WORD)((outR << 11) | (outG << 5) | outB);
}

static void DrawStartupSplashSurface(CSpriteSurface* target, int alpha)
{
	if (target == NULL || g_pTitleLoadingSurface == NULL || alpha <= 0)
	{
		return;
	}

	S_SURFACEINFO destInfo;
	S_SURFACEINFO srcInfo;
	target->GetSurfaceInfo(&destInfo);
	g_pTitleLoadingSurface->GetSurfaceInfo(&srcInfo);
	if (destInfo.p_surface == NULL || srcInfo.p_surface == NULL)
	{
		return;
	}

	const int copyWidth = (srcInfo.width < destInfo.width) ? srcInfo.width : destInfo.width;
	const int copyHeight = (srcInfo.height < destInfo.height) ? srcInfo.height : destInfo.height;
	WORD* dest = (WORD*)destInfo.p_surface;
	WORD* src = (WORD*)srcInfo.p_surface;

	for (int y = 0; y < copyHeight; ++y)
	{
		WORD* destRow = (WORD*)((BYTE*)dest + y * destInfo.pitch);
		WORD* srcRow = (WORD*)((BYTE*)src + y * srcInfo.pitch);
		for (int x = 0; x < copyWidth; ++x)
		{
			destRow[x] = Blend565(destRow[x], srcRow[x], alpha);
		}
	}
}



extern char g_FutecIP[20];
extern unsigned int g_FutecPort;

#ifdef OUTPUT_DEBUG
	extern CRITICAL_SECTION		g_Lock;
#endif

//----------------------------------------------------------------------

//----------------------------------------------------------------------
TYPE_SPRITEID		g_AddonSPKNum = 0;
const int			g_AddonSPKLoadingTimes = 20;	
bool				g_AddonSPKLoaded[g_AddonSPKLoadingTimes] = { false, };
int					g_AddonSPKIndexFirst[g_AddonSPKLoadingTimes] = { 0, };
int					g_AddonSPKIndexLast[g_AddonSPKLoadingTimes] = { 0, };
long				g_AddonSPKIndexFP[g_AddonSPKLoadingTimes] = { 0, };
bool				g_AddonSPKAllLoaded = false;

bool				g_bHALAvailable = false;
extern bool				g_bHALAvailable;
extern WORD g_wAuthKeyMap;

#ifdef PLATFORM_WINDOWS
static bool EnsureWinsockStarted()
{
	static bool initialized = false;
	if (initialized)
	{
		return true;
	}

	WORD versionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
	if (WSAStartup(versionRequested, &wsaData) != 0)
	{
		return false;
	}

	initialized = true;
	return true;
}
#endif

void	StartTitleLoading();
void	DrawTitleLoading();
void	EndTitleLoading(bool SendLogin = false);
bool	PrepareLoadingAddonSPK();
bool	LoadingAddonSPK(bool bLoadingAll);
extern bool CheckInvalidProcess();
extern void ClearDebugInfo();

extern int  g_nGameVersion;
//#define	CLIENT_VERSION		"[TestVersion] : 2000.12.05.09"
//---------------------------------------------------------------------------
// Start TitleLoading
//---------------------------------------------------------------------------
// 2001.8.20
//---------------------------------------------------------------------------
void
StartTitleLoading()
{
//	std::ifstream	titleFile;//(FILE_SSPRITE_IMAGEOBJECT, ios::binary);
//	if (!FileOpenBinary("data\\ui\\spk\\Title.spk", titleFile))
//		return;
//
//	g_TitleSPK.LoadFromFile(titleFile);
//	titleFile.close();
//
//	std::ifstream	titleAniFile;//(FILE_SSPRITE_IMAGEOBJECT, ios::binary);
//	if (!FileOpenBinary("data\\ui\\spk\\TitleAni.spk", titleAniFile))
//		return;
//
//	g_TitleAniSPK.LoadFromFile(titleAniFile);
//	titleAniFile.close();
//	
//
//	g_TitleSpriteID = 0;
//	g_TitleSpriteDarkness = 4;
	g_bTitleLoading = true;
	g_TitleLoadingStartTime = timeGetTime();
	g_TitleSpriteAlpha = 32;

	if (LoadStartupSplashSurface())
	{
		return;
	}

//	CJpeg jpg;
//	bool bOpen = jpg.Open(g_pFileDef->getProperty("FILE_JPG_SOFTON_CI").c_str());
//	if(bOpen == true && jpg.GetWidth() > 0 && jpg.GetHeight() > 0 && jpg.GetHeight() > 0)
//	{
//		g_bTitleLoading = true;
//		CDirectDrawSurface &surface = g_TitleSurface;
//		const int bpp = jpg.GetBpp(), width = jpg.GetWidth(), height = jpg.GetHeight(), pitch = width*bpp;
//
//		if (surface.InitOffsurface(width, height, DDSCAPS_SYSTEMMEMORY))
//		{
//			if (surface.Lock())
//			{
//				WORD *pSurface = (WORD *)surface.GetSurfacePointer();
//				unsigned char *pData = jpg.GetImage(), *pDataTemp;
//				WORD *pSurfaceTemp;
//				
//				int surfacePitch = surface.GetSurfacePitch();
//				
//				if (pSurface)
//				{
//					if (bpp == 1)
//					{
//						for (register int y = 0; y < height; y++)
//						{
//							pDataTemp = pData;
//							pSurfaceTemp = pSurface;								
//							
//							for (register int x = 0; x < width; x++)
//							{
//								BYTE temp_data = *pDataTemp++;	//p_data[y*(pitch)+x];
//								BYTE r = temp_data>>3;
//								BYTE g = r;
//								BYTE b = r;
//								
//								*pSurfaceTemp++ = CSDLGraphics::Color(r, g, b);
//							}
//							
//							pData = pData + pitch;
//							pSurface = (WORD*)((BYTE*)pSurface + surfacePitch);
//						}
//					}
//					else if (bpp == 3)
//					{
//						for (register int y = 0; y < height; y++)
//						{
//							pDataTemp = pData;
//							pSurfaceTemp = pSurface;	
//							
//							for (register int x = 0; x < width; x++)
//							{
//								//char *temp_data = &p_data[y*pitch+x*bpp];
//								BYTE r = *(pDataTemp+2) >> 3;		//temp_data[2]>>3;
//								BYTE g = *(pDataTemp+1) >> 3;	//temp_data[1]>>3;
//								BYTE b = *pDataTemp >> 3;	//temp_data[0]>>3;
//								
//								pDataTemp += bpp;
//								
//								*pSurfaceTemp++ = CSDLGraphics::Color(r, g, b);
//							}
//							
//							pData = pData + pitch;
//							pSurface = (WORD*)((BYTE*)pSurface + surfacePitch);
//						}
//					}
//					
//				}
//				
//				surface.Unlock();
//			}
//		}
//	}
	
	if (g_pTitleLoadingSprite==NULL)
	{
		if (CSDLGraphics::Is565())
		{
			g_pTitleLoadingSprite = new CSprite565;
		}
		else
		{
			g_pTitleLoadingSprite = new CSprite555;
		}

		CFileIndexTable	FIT;
		std::ifstream indexFile(g_pFileDef->getProperty("FILE_SPRITEINDEX_UI").c_str(), ios::binary);
		FIT.LoadFromFile( indexFile );
		indexFile.close();

		std::ifstream spkFile(g_pFileDef->getProperty("FILE_SPRITE_UI").c_str(), ios::binary);
		if(g_MyFull)
			spkFile.seekg( FIT[5] );
		else
			spkFile.seekg( FIT[SPRITEID_CI] );

		g_pTitleLoadingSprite->LoadFromFile( spkFile );
		spkFile.close();
	}
}
	
//---------------------------------------------------------------------------
// Draw TitleLoading
//---------------------------------------------------------------------------
// 2001.8.20
//---------------------------------------------------------------------------
void
DrawTitleLoading()
{
	if (g_bTitleLoading)
	{
		//--------------------------------------------------------
		// 2001.8.20
		//--------------------------------------------------------
		POINT pointZero = { 0, 0 };
		POINT point = { 263, 93 };
		// add by Sonic 2006.9.26
//		if(g_MyFull)
//		{
//			RECT rect = {0, 0, 1024, 768};
//		}
//		else
//		{
			RECT rect = {0, 0, g_GameRect.right, g_GameRect.bottom};
//		}
		// end

//		g_pBack->FillSurface( CSDLGraphics::Color(0,0,0) );
		
//		g_pBack->Lock();

//		g_pBack->Blt(&pointZero, &g_TitleSurface, &rect);
//		g_pBack->BltSprite( &pointZero, &g_TitleSPK[ 0 ] );

//		if (g_TitleSpriteDarkness==0)
//		{
//			g_pBack->BltSprite( &point, &g_TitleAniSPK[g_TitleSpriteID] );
//		}
//		else
//		{
//			g_pBack->BltSpriteDarkness( &point, &g_TitleAniSPK[g_TitleSpriteID], g_TitleSpriteDarkness );
//		}

//		g_pBack->Unlock();

		
		 
		
		if (g_pTitleLoadingSurface != NULL)
		{
			CSpriteSurface* pSurface = g_pBack;
			if (pSurface != NULL)
			{
				DrawStartupSplashSurface(pSurface, g_TitleSpriteAlpha);
			}
		}
		else if (g_pTitleLoadingSprite!=NULL)
		{
//			point.x = 400 - (g_pTitleLoadingSprite->GetWidth()>>1);
//			point.y = 460 - (g_pTitleLoadingSprite->GetHeight()>>1);
//			
			CSpriteSurface *pSurface = NULL;
//			if (!true)
			if(g_TitleSpriteAlpha != 32)
			{
				pSurface = g_pLast;
			}
			else
			{
				pSurface = g_pBack;
			}

			pSurface->Lock();
			if(g_TitleSpriteAlpha == 32)
			{
				pSurface->BltSprite( &pointZero, g_pTitleLoadingSprite );
			}
			else
			{
				pSurface->BltSpriteAlpha( &pointZero, g_pTitleLoadingSprite, g_TitleSpriteAlpha );
			}
			pSurface->Unlock();
		}

//		if (++g_TitleSpriteID>=g_TitleAniSPK.GetSize())
//		{
//			g_TitleSpriteID = 0;			
//		}
//			

//		if (g_TitleSpriteID%3==2)
//		{
//			if (--g_TitleSpriteDarkness < 0)
//			{
//				g_TitleSpriteDarkness = 0;
//			}
//		}			
	}
}

//---------------------------------------------------------------------------
// End TitleLoading
//---------------------------------------------------------------------------
void
EndTitleLoading(bool SendLogin)
{
	TraceGameStartup("EndTitleLoading begin");
//	g_TitleAniSPK.Release();
//	g_TitleSPK.Release();
//	g_TitleSpriteID = 0;
//	g_TitleSpriteDarkness = 4;
	g_bTitleLoading = false;

	if (g_pTitleLoadingSprite!=NULL)
	{
		delete g_pTitleLoadingSprite;
		g_pTitleLoadingSprite = NULL;
	}
	if (g_pTitleLoadingSurface!=NULL)
	{
		delete g_pTitleLoadingSurface;
		g_pTitleLoadingSurface = NULL;
	}
	TraceGameStartup("EndTitleLoading sprite released");
	// 2004, 7, 15, sobeit modify start
	
	
	if(SendLogin && true == g_pUserInformation->IsAutoLogIn) 
	{
		static LOGIN	login;
		login.sz_id = new char[  g_pUserInformation->UserID.GetLength() + 1 ];
		login.sz_password = new char[ 20 ];
		
		strcpy( login.sz_id, g_pUserInformation->UserID.GetString() );		
		strcpy( login.sz_password, g_pUserInformation->AutoLogInKeyValue.GetString() );
		
		gpC_base->SendMessage(UI_LOGIN, 0, 0, (void *)&login);
		TraceGameStartup("EndTitleLoading auto login sent");
	}
	else
	{
		TraceGameStartup("EndTitleLoading no auto login");
	}
	
	// 2006.11.07  Coffee Edit
	// 2004, 7, 15, sobeit modify end
}

//---------------------------------------------------------------------------
// PrepareLoadingAddonSPK
//---------------------------------------------------------------------------
bool
PrepareLoadingAddonSPK()
{
	return true;

	#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			DEBUG_ADD("Prepare LoadingAddonSPK");
		}
	#endif

	g_AddonSPKAllLoaded = false;

	if (g_pTopView==NULL)
	{
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("[Error] g_pTopView is NULL");
			}
		#endif

		return false;
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (g_AddonSPKNum==0)
	{
		std::ifstream	AddonFileIndex2;//(FILE_ISPRITE_ADDON, ios::binary);
		if (!FileOpenBinary(g_pFileDef->getProperty("FILE_ISPRITEINDEX_ADDON").c_str(), AddonFileIndex2))
			return false;

		AddonFileIndex2.read((char*)&g_AddonSPKNum, SIZE_SPRITEID);

		
		g_pTopView->m_AddonSPK.Init( g_AddonSPKNum);		

		bool				g_AddonSPKLoaded[g_AddonSPKLoadingTimes] = { false, };
		bool				g_AddonSPKIndex[g_AddonSPKLoadingTimes] = { 0 };

		
		int first = 0;
		for (int i=0; i<g_AddonSPKLoadingTimes; i++)
		{
			// 1/3
			int last = (int)g_AddonSPKNum * (i+1) / g_AddonSPKLoadingTimes;
			
			
			if (last >= g_AddonSPKNum)
			{
				last = g_AddonSPKNum - 1;
			}

			long fp;
			
			AddonFileIndex2.seekg( 2 + first*4 , ios::beg );
			AddonFileIndex2.read((char*)&fp, 4);

			
			g_AddonSPKLoaded[i] = false;

			
			g_AddonSPKIndexFirst[i] = first;
			g_AddonSPKIndexLast[i] = last;
			g_AddonSPKIndexFP[i] = fp;
			

			
			first = last+1;
		}

		AddonFileIndex2.close();
	}

	return true;
}

//---------------------------------------------------------------------------
// LoadingAddonSPK
//---------------------------------------------------------------------------
bool
LoadingAddonSPK(bool bLoadingAll)
{
	return true;

	#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			if (bLoadingAll)
			{
				DEBUG_ADD("LoadingAddonSPK - LoadingAll");
			}
			else
			{
				DEBUG_ADD("LoadingAddonSPK - LoadingPart");
			}
		}
	#endif

	if (g_pTopView==NULL)
	{
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("[Error] g_pTopView is NULL");
			}
		#endif

		return false;
	}

	for (int i=0; i<g_AddonSPKLoadingTimes; i++)
	{
		
		if (!g_AddonSPKLoaded[i])
		{
//			std::ifstream	addonFile;
//
//			if (!FileOpenBinary(FILE_ISPRITE_ADDON, addonFile))
//				return false;	

			TYPE_SPRITEID first = g_AddonSPKIndexFirst[i];
			TYPE_SPRITEID last	= g_AddonSPKIndexLast[i];
			long			fp	= g_AddonSPKIndexFP[i];

			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage)
				{
					DEBUG_ADD_FORMAT("LoadingAddonSPK: [%d ~ %d](fp=%d). num=%d", first, last, fp, g_AddonSPKNum);
				}
			#endif

			g_pTopView->m_AddonSPK.LoadFromFilePart(first, last);
		
			g_AddonSPKLoaded[i] = true;

			
			if (!bLoadingAll)
			{
				#ifdef OUTPUT_DEBUG
					if (g_pDebugMessage)
					{
						DEBUG_ADD("LoadingAddonSPK Part OK");
					}
				#endif

				return true;
			}
		}
	}

	g_AddonSPKAllLoaded = true;

	#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			DEBUG_ADD("All AddonSPK is Loaded");
		}
	#endif

	return true;
}

//---------------------------------------------------------------------------
// Init Volume
//---------------------------------------------------------------------------
BOOL
InitVolume()
{
	// Debug Message
	DEBUG_ADD("[ InitGame ]  Volume");

	// Volume control is now handled by SDL_mixer
	// No Windows mixer support needed

	return TRUE;
}

//---------------------------------------------------------------------------
// Init Input
//---------------------------------------------------------------------------
BOOL
InitInput()
{
	// Debug Message
	DEBUG_ADD("[ InitGame ]  Input");

	if (g_pSDLInput==NULL)
	{
		g_pSDLInput = new CSDLInput;
	}

	//----------------------------
	// DirectInput
	//----------------------------
	//g_pSDLInput->Init(g_hInstance);
	//g_pSDLInput->InitKeyDevice(g_hWnd);
//	g_pSDLInput->InitJoyDevice(g_hWnd);
	//g_pSDLInput->InitMouseDevice(g_hWnd);

	if (!g_pSDLInput->Init( g_hWnd, g_hInstance, CSDLInput::NONEXCLUSIVE ))
	{
		InitFail("DirectInput Error!");
		return false;
	}

	//g_pSDLInput->SetMouseMoveLimit(SURFACE_WIDTH, SURFACE_HEIGHT);

	g_pSDLInput->SetAcquire(true);

	return TRUE;
}


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
BOOL
InitSurface()
{
	TraceGameStartup("InitSurface begin");
	DEBUG_ADD("[ InitGame ]  Surface");

	//--------------------------------------------------------
	// Back Surface
	//--------------------------------------------------------
	if (g_pBack!=NULL) 
	{
		delete g_pBack;
	}
	g_pBack = new CSpriteSurface;
	// SDL2: Unified surface initialization
	g_pBack->Init(g_GameRect.right, g_GameRect.bottom);
	TraceGameStartup("back surface initialized");

	//--------------------------------------------------------
	
	// 2001.8.20
	//--------------------------------------------------------

	StartTitleLoading();
	TraceGameStartup("title loading started");
	DrawTitleLoading();
	TraceGameStartup("title loading drawn first");


	g_pBack->SetTransparency( 0 );
	g_pBack->FillSurface( CSDLGraphics::Color(0,0,0) );
	TraceGameStartup("back surface cleared");

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	DEBUG_ADD("[ InitGame ]  Surface - new g_pTopView");

		
	if (g_pTopView==NULL)
	{		
		g_pTopView = new MTopView;
		TraceGameStartup("top view allocated");

		
		if (!PrepareLoadingAddonSPK())
		{
			return FALSE;
		}
		TraceGameStartup("addon spk prepared");
	}


	
//	if (true)
//	{
//		g_pTopView->SetSurface( g_pBack );
//	}

//	else
	{		
		if (g_pLast!=NULL) 
		{
			delete g_pLast;
		}
		g_pLast = new CSpriteSurface;
		// SDL2: Unified offscreen surface initialization (always system memory)
		g_pLast->InitOffsurface(g_GameRect.right, g_GameRect.bottom);
		g_pLast->SetTransparency( 0 );
		g_pLast->FillSurface( CSDLGraphics::Color(30,30,30) );
		TraceGameStartup("last surface initialized");

		g_pTopView->SetSurface( g_pLast );
		TraceGameStartup("top view surface set");
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (g_pUserOption->UseGammaControl
		&& g_pUserOption->GammaValue!=100)
	{
		CSDLGraphics::SetGammaRamp( g_pUserOption->GammaValue );
	}

	//--------------------------------------------------------
	//                UI
	//--------------------------------------------------------	
	DEBUG_ADD("[ InitGame ]  Surface - InitializeGL");


	InitializeGL(CSDLGraphics::Get_BPP(), 
					 CSDLGraphics::Get_Count_Rbit(), 
					 CSDLGraphics::Get_Count_Gbit(), 
					 CSDLGraphics::Get_Count_Bbit());
	TraceGameStartup("InitializeGL OK");

	DEBUG_ADD("[ InitGame ]  Surface - Initialize Font");
	DEBUG_ADD("[ InitGame ]  Surface - UI");

	
	LoadingAddonSPK( false );
	TraceGameStartup("addon spk loaded");
	DrawTitleLoading();
	TraceGameStartup("title loading drawn second");


	
	gC_vs_ui.Release();
	TraceGameStartup("vs ui released");

	DEBUG_ADD("[ InitGame ]  Surface - InitD3D");
//	if (true)
//	{
//		gC_vs_ui.Init(g_pBack, UI_ResultReceiver);		
//	} 
//	else
	{
		gC_vs_ui.Init(g_pLast, UI_ResultReceiver);
	}
	TraceGameStartup("vs ui initialized");

	//--------------------------------------------------------
	// 2001.8.20
	//--------------------------------------------------------	
	DrawTitleLoading();	
	TraceGameStartup("title loading drawn third");

//	gC_vs_ui.SetVersion(CLIENT_VERSION);




	///*
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	
	
	CIndexSprite::SetColorSet();
	TraceGameStartup("index sprite color set initialized");

	//#ifdef OUTPUT_DEBUG
	//	std::ofstream indexTable(FILE_INDEXTABLE, ios::binary);
	//	CIndexSprite::SaveIndexTableToFile( indexTable );
	///	indexTable.close();
	//#endif
	
	//std::ifstream indexTable2;//(FILE_INFO_USINGCOLORSET, ios::binary);
	//if (!FileOpenBinary(FILE_INDEXTABLE, indexTable2))
	//	return FALSE;
	//CIndexSprite::LoadIndexTableFromFile( indexTable2 );
	//indexTable2.close();

	/*
	CSpriteSurface imageSurface;
	CSpriteSurface indexSurface1;
	//CSpriteSurface indexSurface2;
	
	WORD *lpImage, imagePitch;
	WORD *lpIndex1, indexPitch1;
	//WORD *lpIndex2, indexPitch2;

	imageSurface.InitFromBMP("test\\0.bmp", DDSCAPS_SYSTEMMEMORY);	
	indexSurface1.InitFromBMP("test\\0i.bmp", DDSCAPS_SYSTEMMEMORY);	
	//indexSurface2.InitFromBMP("Gauntlet2.bmp", DDSCAPS_SYSTEMMEMORY);	

	imageSurface.LockW(lpImage, imagePitch);
	indexSurface1.LockW(lpIndex1, indexPitch1);
	//indexSurface2.LockW(lpIndex2, indexPitch2);

	CIndexSprite565 is;

	RECT rect;

	CSprite::GetTightColorRect(lpImage, imagePitch, 
								imageSurface.GetWidth(), imageSurface.GetHeight(),
								0x001F, 
								rect);


	lpImage = (WORD*)((BYTE*)lpImage + imagePitch*rect.top + (rect.left << 1));
	lpIndex1 = (WORD*)((BYTE*)lpIndex1 + indexPitch1*rect.top + (rect.left << 1));
	//lpIndex2 = (WORD*)((BYTE*)lpIndex2 + indexPitch2*rect.top + (rect.left << 1));

	CIndexSprite::SetColorkey( 0x001F );
	//CIndexSprite::SetColorkey( ((32>>3) << 11) | ((32>>3) << 6) | (32>>3) );
	is.SetPixel(lpImage, imagePitch,
				lpIndex1, indexPitch1,
				NULL, 0, //lpIndex2, indexPitch2,
				rect.right-rect.left, rect.bottom-rect.top);
	
	imageSurface.Unlock();
	indexSurface1.Unlock();
	//indexSurface2.Unlock();

	std::ofstream file("test.ispr", ios::binary);
	is.SaveToFile( file );
	file.close();

	POINT point;
	point.x = 100;
	point.y = 100;
	WORD a=40, b=150;
	while (1)
	{
		UpdateInput();

		if (g_pSDLInput->KeyDown(DIK_ESCAPE))
			break;

		if (g_pSDLInput->KeyDown(DIK_SPACE))
		{
			a = rand()%MAX_COLORSET;
			b = rand()%MAX_COLORSET;
		}

		g_pBack->FillSurface( 0 );	

		g_pBack->Lock();
		CIndexSprite::SetUsingColorSet( a, b );
		g_pBack->BltIndexSprite( &point, &is );
		g_pBack->Unlock();

		CSDLGraphics::Flip();
	}

	return FALSE;
	*/	

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	//g_pTopView->SetSurface(g_pLast);	
	if (g_Mode==MODE_CHANGE_OPTION	
		&& !g_pTopView->Init())
	{
		return FALSE;
	}			

	DrawTitleLoading();	
//	EndTitleLoading();
	
	//g_pTopView->SetDarkBits(5);
		

	
	CIndexSprite::SetColorSet();
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	
	if (CSDLGraphics::IsFullscreen())
	{
		if (g_pCursorSurface!=NULL)
		{
			delete g_pCursorSurface;
			g_pCursorSurface = NULL;
		}

		//g_pCursorSurface = new CStorageSurface;
	}
	//g_pCursorSurface.Init(2, CURSOR_SURFACE_WIDTH, CURSOR_SURFACE_HEIGHT);
	DrawTitleLoading();	
	TraceGameStartup("title loading drawn fourth");

	TraceGameStartup("InitSurface OK");
	return TRUE;
}

//---------------------------------------------------------------------------
// Init Sound
//---------------------------------------------------------------------------
BOOL
InitSound()
{
	// Debug Message
	DEBUG_ADD("[ InitGame ]  InitSound");

	//--------------------------------------------------------
	//
	//                 DirectSound
	//
	//--------------------------------------------------------
	/*
	if (g_pWavePackFileManager==NULL)
	{
		g_pWavePackFileManager = new WavePackFileManager;

		g_pWavePackFileManager->LoadFromFileInfo(FILE_WAVE_PACK_SOUND_INDEX);
		g_pWavePackFileManager->SetDataFilename(FILE_WAVE_PACK_SOUND);
	}
	*/

	if (g_SDLAudio.IsInit())
	{
		DEBUG_ADD("[ InitGame ]  Already Init");

		return TRUE;
	}
	else
	{
		if (g_SDLAudio.Init(g_hWnd))
		{	
			
			//for (int i=0; i<(*g_pSoundTable).GetSize(); i++)
			//{			
			//	(*g_pSoundTable)[i].pDSBuffer = g_Sound.LoadWav( (*g_pSoundTable)[i].Filename );
			//}
			int value = g_pUserOption->VolumeSound;	

			LONG volume = value*SOUND_DEGREE + SOUND_MIN;

			g_SDLAudio.SetVolumeLimit( volume );	
		}
		else
		{
			DEBUG_ADD("[Error] Init Sound Error");

			return FALSE;
		}

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		// SDL2: Unified - use default sound part count for all platforms
		// DirectDraw memory status not available in SDL2
		g_pClientConfig->MAX_SOUNDPART = 100;

		
		g_pSoundManager->Init( g_pSoundTable->GetSize(), g_pClientConfig->MAX_SOUNDPART );

		 

		return TRUE;
	}
	
}

//---------------------------------------------------------------------------
// Init Music
//---------------------------------------------------------------------------
BOOL
InitMusic()
{
	// Debug Message
	DEBUG_ADD("[ InitGame ]  Music");
	
	//--------------------------------------------------------
	//
	//                 Music
	//
	//--------------------------------------------------------
	//g_Music.Init(g_hWnd);	
		/*
	if (g_bMusicSW)
	{
		g_SDLMusic.Init(g_hWnd, DIRECTMUSIC_TYPE_SW);
	}
	else
	{
		g_SDLMusic.Init(g_hWnd, DIRECTMUSIC_TYPE_HW);
	}
	*/
	g_Music.Init( g_hWnd );

	//int volume = g_Music.GetVolume();
	//			DEBUG_ADD_FORMAT("[MIDI] Volume in Init = %x", volume);

	int value = g_pUserOption->VolumeMusic;
				
	WORD volume = value*0x1000 + 0x0FFF;

	g_Music.SetVolume( volume );

	
//	if (g_pSDLStream==NULL)
//	{
//		g_pSDLStream = new CDirectSoundStream;
//
//		int value = g_pUserOption->VolumeMusic;	
//
//		LONG volume = value*SOUND_DEGREE + SOUND_MIN;
//
//		g_pSDLStream->SetVolumeLimit( volume );	
//	}

	DEBUG_ADD("MP3 new");
#ifdef __USE_MP3__
	if (g_pMP3 == NULL)
	{
		g_pMP3 = new CMP3;

	DEBUG_ADD("MP3 new OK");
		int value = g_pUserOption->VolumeMusic;	

		LONG volume = value*16*257;// value*SOUND_DEGREE + SOUND_MIN;

	DEBUG_ADD("MP3 SetVolume1");
		g_pMP3->SetVolume( volume );	
	DEBUG_ADD("MP3 SetVolume1 OK");
	}
#else
	if( g_SDLAudio.IsInit() )
	{
		// SDL backend stub - OGG streaming not implemented
		if( g_pSoundBufferForOGG == NULL )
		{
			// g_pSoundBufferForOGG = new CDirectSoundBuffer(g_hWnd, SOUND_STEREO, SOUND_44K, SOUND_16BIT);
			g_pSoundBufferForOGG = NULL; // Stub: not implemented on SDL
		}
		if( g_pOGG == NULL )
		{
#ifdef _MT
			g_pOGG = new COGGSTREAM(NULL, g_pSoundBufferForOGG, 44100, 11025, 8800, 1);
#else
			g_pOGG = new COGGSTREAM(NULL, g_pSoundBufferForOGG, 44100, 11025, 8800,1);
#endif
			int volume = (g_pUserOption->VolumeMusic - 15) * 250;

			g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
		}
	}
#endif

	return TRUE;
}

//---------------------------------------------------------------------------
// Init Draw
//---------------------------------------------------------------------------
BOOL
InitDraw()
{
	TraceGameStartup("InitDraw begin");
	// Debug Message
	DEBUG_ADD("[ InitGame ]  Draw");
	TraceGameStartup("InitDraw after DEBUG_ADD");

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	TraceGameStartup(g_pUserOption != NULL ? "InitDraw user option present" : "InitDraw user option NULL");
	g_bHAL = (g_pUserOption->Use3DHAL)? true : false;
	TraceGameStartup("InitDraw HAL flag read");

//	bool g_bUseIMEWindow = (g_pUserOption->Chinese) ? true : false;

	//bool g_bUseIMEWindow = gC_ci->IsChinese() == true;
	bool g_bUseIMEWindow = true;
	TraceGameStartup("InitDraw IME flag set");

	//--------------------------------------------------------
	//
	//                 DirectDraw
	//
	//--------------------------------------------------------
#ifdef PLATFORM_WINDOWS
	//if (g_bHAL)
	if( g_bHAL )
	{
		if (g_bFullScreen)
		{
			// add by Sonic 2006.9.26
			if(g_MyFull)
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::FULLSCREEN, true, g_bUseIMEWindow);
			}
			else
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::FULLSCREEN, true, g_bUseIMEWindow);
			}
		}
		else
		{
			if(g_MyFull)
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::WINDOWMODE, true, g_bUseIMEWindow);
			}
			else
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::WINDOWMODE, true, g_bUseIMEWindow);
			}
		}
	}
#endif // PLATFORM_WINDOWS

#if defined(PLATFORM_WINDOWS) && !defined(USE_SDL_BACKEND)
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		DDSCAPS2 ddsCaps2;
		DWORD dwTotal;
		DWORD dwFree;
		ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
		ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY;//DDSCAPS_TEXTURE;
		HRESULT hr = CSDLGraphics::GetDD()->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree);

		DEBUG_ADD_FORMAT("[VidemoMemory from GetAvailableVidMem()] Before Init Draw = %d/%d", dwFree, dwTotal);


		//--------------------------------------------------------
		
		//--------------------------------------------------------
		
		
		DDCAPS	driverCaps;
		ZeroMemory( &driverCaps, sizeof(driverCaps) );
		driverCaps.dwSize = sizeof(driverCaps);

		hr = CSDLGraphics::GetDD()->GetCaps( &driverCaps, NULL );
		if (hr!=DD_OK)
		{
			InitFail("[Error] GetCaps to Get VidMem");
			return FALSE;
		}

		#ifdef	OUTPUT_DEBUG
			if (driverCaps.dwCaps2 & DDCAPS2_NONLOCALVIDMEMCAPS)
			{
				DEBUG_ADD_FORMAT("DDCAPS2_NONLOCALVIDMEMCAPS Enable");
			}
			else
			{
				DEBUG_ADD_FORMAT("DDCAPS2_NONLOCALVIDMEMCAPS Disable");
			}
		#endif

		dwTotal = driverCaps.dwVidMemTotal;
		dwFree = driverCaps.dwVidMemFree;

		DEBUG_ADD_FORMAT("[VidemoMemory from GetDDCaps()] Before Init Draw = %d/%d", dwFree, dwTotal);

		DEBUG_ADD("[ InitGame ]  Init Draw OK");

		BOOL enoughMemory = TRUE;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (dwFree < 8388608)
		{
			enoughMemory = FALSE;
		}

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		bool bUse3D = ( enoughMemory && g_bHAL && ( true ));

		if (bUse3D)
		{
			DEBUG_ADD("[ InitGame ]  Init 3D");

			// CDirect3D::Init() removed (SDL2) - SDL2 always uses hardware acceleration
			g_bEnable3DHAL = TRUE;
			g_bHALAvailable = true;

			/*
			D3DDEVICEDESC7 devDesc;
			CDirect3D::GetDevice()->GetCaps( &devDesc );

			#ifdef	OUTPUT_DEBUG
				if (devDesc.dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM)
				{
					DEBUG_ADD_FORMAT("D3DDEVCAPS_TEXTURENONLOCALVIDMEM Enable");
				}
				else
				{
					DEBUG_ADD_FORMAT("D3DDEVCAPS_TEXTURENONLOCALVIDMEM Disable");
				}
			#endif
			*/
		}
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (!bUse3D)
		{
			DEBUG_ADD("[ InitGame ]  3D Failed. Release Draw and Re-Init 2D");

			CSDLGraphics::ReleaseAll();

			if (g_bFullScreen)
			{
				// add by Sonic 2006.9.26
				if(g_MyFull)
				{
					CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::FULLSCREEN, false, g_bUseIMEWindow);
				}
				else
				{
					CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::FULLSCREEN, false, g_bUseIMEWindow);
				}
			}
			else
			{
				if(g_MyFull)
				{
					CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::WINDOWMODE, true, g_bUseIMEWindow);
				}
				else
				{
					CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::WINDOWMODE, true, g_bUseIMEWindow);
				}
			}

			g_bEnable3DHAL = FALSE;
		}
#endif // PLATFORM_WINDOWS

#ifndef PLATFORM_WINDOWS
	//--------------------------------------------------------
	//
	// SDL Backend - DirectX not available
	//
	//--------------------------------------------------------
	DEBUG_ADD("[ InitGame ]  SDL backend - no DirectX initialization");
#endif // PLATFORM_WINDOWS

#if defined(PLATFORM_WINDOWS) && !defined(USE_SDL_BACKEND)
	//--------------------------------------------------------
	//
	
	//
	//--------------------------------------------------------
	else
	{
		DEBUG_ADD("[ InitGame ]  Init 2D");

		if (g_bFullScreen)
		{
			if(g_MyFull)
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::FULLSCREEN, false, g_bUseIMEWindow);
			}
			else
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::FULLSCREEN, false, g_bUseIMEWindow);
			}
		}
		else
		{
			if(g_MyFull)
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::WINDOWMODE, true, g_bUseIMEWindow);
			}
			else
			{
				CSDLGraphics::Init(g_hWnd, g_GameRect.right, g_GameRect.bottom, CSDLGraphics::WINDOWMODE, true, g_bUseIMEWindow);
			}
		}
	}
#endif // PLATFORM_WINDOWS

	TraceGameStartup("before CSpriteSurface::InitEffectTable");
	CSpriteSurface::InitEffectTable();
	TraceGameStartup("CSpriteSurface::InitEffectTable OK");

#ifdef PLATFORM_WINDOWS
	TraceGameStartup("before CDirectDrawSurface::SetGammaFunction");
	CDirectDrawSurface::SetGammaFunction();
	TraceGameStartup("CDirectDrawSurface::SetGammaFunction OK");
#endif

	TraceGameStartup("InitDraw OK");
	return TRUE;
}

//-----------------------------------------------------------------------------
// LoadingThreadProc
//-----------------------------------------------------------------------------
LONG
LoadingThreadProc(LPVOID lpParameter)
{
	if (g_pLoadingThread!=NULL)
	{
		g_pLoadingThread->Execute();
	}

	return 0L;
}

//-----------------------------------------------------------------------------
// Init Thread
//-----------------------------------------------------------------------------
BOOL
InitThread()
{
	// Debug Message
	DEBUG_ADD("[ InitGame ]  Thread");

	/*
	DWORD	dwChildThreadID;

	// init event for nonsignaled
	g_hFileEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	

	g_hFileThread = CreateThread(NULL, 
								0,	// default stack size
								(LPTHREAD_START_ROUTINE)FileThreadProc,
								NULL,
								NULL,
								&dwChildThreadID);

	SetThreadPriority(g_hFileThread, 
						//THREAD_PRIORITY_NORMAL
						THREAD_PRIORITY_BELOW_NORMAL
						);
	*/

	if (g_pLoadingThread!=NULL)
	{
		delete g_pLoadingThread;
	}
	g_pLoadingThread = new MWorkThread;
	g_pLoadingThread->Init( (LPTHREAD_START_ROUTINE)LoadingThreadProc );

	g_pLoadingThread->SetPriority( THREAD_PRIORITY_LOWEST );

	return TRUE;
}

//---------------------------------------------------------------------------
// Stop LoadingThread
//---------------------------------------------------------------------------
void
StopLoadingThread()
{
	//-------------------------------------------------------------
	
	//-------------------------------------------------------------
	if (g_pLoadingThread!=NULL)
	{
		DEBUG_ADD_FORMAT("Thread-ReleaseWork: size=%d, working=%d, finish=%d, stop=%d", 
									g_pLoadingThread->GetSize(),
									(int)g_pLoadingThread->IsWorking(),
									(int)g_pLoadingThread->IsFinishCurrentWork(),
									(int)g_pLoadingThread->IsStopWork());
		
		g_pLoadingThread->SetPriority( THREAD_PRIORITY_HIGHEST );
		g_pLoadingThread->ReleaseWork();
		g_pLoadingThread->SetPriority( THREAD_PRIORITY_LOWEST );

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		DEBUG_ADD_FORMAT("Thread-WaitingForStop: size=%d, working=%d, finish=%d, stop=%d", 
									g_pLoadingThread->GetSize(),
									(int)g_pLoadingThread->IsWorking(),
									(int)g_pLoadingThread->IsFinishCurrentWork(),
									(int)g_pLoadingThread->IsStopWork());
		
		while (g_pLoadingThread->IsStopWork());

		DEBUG_ADD("Thread-End Waiting");
	}
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
BOOL
InitGameUpdate()
{
	TraceGameStartup("InitGameUpdate begin");
	if (g_pCOpeningUpdate!=NULL)
	{
		TraceGameStartup("InitGameUpdate delete opening");
		delete	g_pCOpeningUpdate;
		g_pCOpeningUpdate = NULL;
	}
	TraceGameStartup("InitGameUpdate opening clear");

	if (g_pCGameUpdate!=NULL)
	{
		TraceGameStartup("InitGameUpdate delete game");
		delete	g_pCGameUpdate;
		g_pCGameUpdate = NULL;
	}
	TraceGameStartup("InitGameUpdate game clear");

	if (g_pCWaitPacketUpdate!=NULL)
	{
		TraceGameStartup("InitGameUpdate delete wait packet");
		delete	g_pCWaitPacketUpdate;
		g_pCWaitPacketUpdate = NULL;
	}
	TraceGameStartup("InitGameUpdate wait packet clear");

	if (g_pCWaitUIUpdate!=NULL)
	{
		TraceGameStartup("InitGameUpdate delete wait ui");
		delete	g_pCWaitUIUpdate;
		g_pCWaitUIUpdate = NULL;
	}
	TraceGameStartup("InitGameUpdate wait ui clear");

	g_pCOpeningUpdate		= new COpeningUpdate;
	TraceGameStartup("InitGameUpdate opening allocated");
	g_pCGameUpdate			= new CGameUpdate;
	TraceGameStartup("InitGameUpdate game allocated");
	g_pCWaitPacketUpdate	= new CWaitPacketUpdate;
	TraceGameStartup("InitGameUpdate wait packet allocated");
	g_pCWaitUIUpdate		= new CWaitUIUpdate;
	TraceGameStartup("InitGameUpdate wait ui allocated");

	TraceGameStartup("InitGameUpdate OK");
	return TRUE;
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
BOOL
InitGame()
{
	TraceGameStartup("InitGame begin");
	//---------------------------------------------------------------------
	// Initialize logging system first (before everything else)
	//---------------------------------------------------------------------
	log_init();
	TraceGameStartup("logging initialized");

	// Always enable console output and INFO level logging
	// This ensures DEBUG_ADD macros and panic messages are visible
	log_set_console_output(true);

// Conditional compilation: Remove DEBUG logs in Release builds
#ifdef _DEBUG
	log_set_level(LOG_LEVEL_DEBUG);
#else
	log_set_level(LOG_LEVEL_INFO);
#endif

	g_pFileDef = new Properties;
	g_pFileDef->load("Data/Info/FileDef.inf");
	TraceGameStartup("InitGame file definitions loaded");

	//---------------------------------------------------------------------
	// Profiler
	//---------------------------------------------------------------------
	#ifdef OUTPUT_DEBUG
		if (g_pProfiler!=NULL)
		{
			g_pProfiler->Release();
		}
		else
		{
			g_pProfiler = new Profiler;
		}
	#endif

	//---------------------------------------------------------------------
	// MathTable
	//---------------------------------------------------------------------
	MathTable::FCreateSines();
	TraceGameStartup("math table initialized");

	//---------------------------------------------------------------------
	// Effect Orbit Table
	//---------------------------------------------------------------------
	MAttachOrbitEffect::InitOrbitPosition();
	TraceGameStartup("orbit table initialized");

	//---------------------------------------------------------------------
	// Init 
	//---------------------------------------------------------------------
	if (g_pGameStringTable==NULL)
	{
		g_pGameStringTable = new MStringArray;
	}

	if (g_pSystemMessage==NULL)
	{
		g_pSystemMessage = new CMessageArray;
	}

	if (g_pPlayerMessage==NULL)
	{
		g_pPlayerMessage = new CMessageArray;
	}
	
	if (g_pGameMessage==NULL)
	{
		g_pGameMessage = new CMessageArray;
	}

	if (g_pHelpMessage==NULL)
	{
		g_pHelpMessage = new CMessageArray;
	}
	
	if (g_pNoticeMessage == NULL )
	{
		g_pNoticeMessage = new CMessageArray;
	}

	// 2004, 6, 18 sobeit add start - nick name table 
	if (g_pNickNameStringTable==NULL)
	{
		g_pNickNameStringTable = new MStringArray;
	}
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
				break;
			
			if (!FindNextFile(hSearch, &FileData)) 
			{
				fFinished = true; 
				g_wAuthKeyMap = 0x5154;
			}
		} 
		// Close the search handle. 
		FindClose(hSearch);
	}
#endif
	//yckou end
	TraceGameStartup("message tables allocated");
	//---------------------------------------------------------------------
	// nick name string table Loading
	//---------------------------------------------------------------------
	std::ifstream gameStringTableTable;//(FILE_INFO_gameStringTable, ios::binary);
	if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_NICKNAME_STRING").c_str(), gameStringTableTable))
		return FALSE;
	(*g_pNickNameStringTable).LoadFromFile_NickNameString(gameStringTableTable);
	gameStringTableTable.close();
	TraceGameStartup("nickname table loaded");


	// 2004, 6, 18 sobeit add end - nick name table 
	//---------------------------------------------------------------------
	// GameStringTable Loading
	//---------------------------------------------------------------------
	std::ifstream gameStringTableTable2;//(FILE_INFO_gameStringTable, ios::binary);
	if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_STRING").c_str(), gameStringTableTable2))
		return FALSE;
	(*g_pGameStringTable).LoadFromFile(gameStringTableTable2);
	gameStringTableTable2.close();
	TraceGameStartup("game string table loaded");


	//---------------------------------------------------
	// System Message
	//---------------------------------------------------
	g_pSystemMessage->Init(MAX_SYSTEMMESSAGE, 256);

	//---------------------------------------------------
	// Player Message
	//---------------------------------------------------
	g_pPlayerMessage->Init(MAX_PLAYERMESSAGE, 256);

	g_pNoticeMessage->Init(7, 256);

	//---------------------------------------------------
	// Game Message
	//---------------------------------------------------
	g_pGameMessage->Init(MAX_GAMEMESSAGE, 256);

	//---------------------------------------------------
	// Help Message
	//---------------------------------------------------
	g_pHelpMessage->Init(MAX_HELPMESSAGE, 256);
	
	// Debug Message
	DEBUG_ADD("---------------[   InitGame   ]---------------");

	//----------------------------------------------------------------------
	// Socket initialization (mingw socket on Windows, BSD sockets on Unix)
	//----------------------------------------------------------------------
	DEBUG_ADD("[ InitGame ]  Socket - Socket initialized");

	// Note: mingw socket (POSIX-compatible) doesn't need WSAStartup like WinSock

//	#ifdef _DEBUG
//		bool bMerge = false;
//
//		if (bMerge)
//		{
//			MGuildMarkManager::MergeGuildMark( FILE_SPRITE_GUILD_MARK, 
//												FILE_INFO_GUILD_INFO_MAPPER,
//												"data\\GuildMark.spk",
//												"data\\GuildMark.inf");
//		}
//	#endif

	//--------------------------------------------------------
	//
	//                 Init...
	//
	//--------------------------------------------------------
	if ((TraceGameStartup("before InitClientConfig"), InitClientConfig()) &&
		(TraceGameStartup("before InitInfomation"), InitInfomation()) &&
		(TraceGameStartup("before InitUserOption"), InitUserOption()) &&

		(TraceGameStartup("before InitVolume"), InitVolume()) &&
		(TraceGameStartup("before InitInput"), InitInput()) &&
		//InitSound()  &&
		(TraceGameStartup("before InitDraw"), InitDraw()) &&

		
		(TraceGameStartup("before InitGameObject"), InitGameObject()) &&    // GameObject must be initialized before Surface
		                        // because gC_vs_ui.Init() in InitSurface() depends on g_pMoneyManager
		(TraceGameStartup("before InitSurface"), InitSurface()) &&

		(TraceGameStartup("before InitGameUpdate"), InitGameUpdate())
		)		
	{	
		//yckou
#ifdef SPRITELIB_BACKEND_SDL
		TraceGameStartup("CheckInvalidProcess skipped for SDL backend");
#else
		TraceGameStartup("before CheckInvalidProcess");
		CheckInvalidProcess();
		TraceGameStartup("CheckInvalidProcess OK");
#endif
		TraceGameStartup("before UI_AffectUserOption");
		UI_AffectUserOption();
		TraceGameStartup("UI_AffectUserOption OK");
		//----------------------------------------
		
		//----------------------------------------
		/*
		#ifdef _DEBUG
			g_pGuildMarkManager->CreateGuildMark( 1, "Data\\Guild\\Guild1.bmp" );
			g_pGuildMarkManager->CreateGuildMark( 2, "Data\\Guild\\Guild2.bmp" );
			g_pGuildMarkManager->CreateGuildMark( 3, "Data\\Guild\\Guild3.bmp" );
			g_pGuildMarkManager->CreateGuildMark( 4, "Data\\Guild\\Guild4.bmp" );
			g_pGuildMarkManager->CreateGuildMark( 5, "Data\\Guild\\Guild5.bmp" );
			g_pGuildMarkManager->CreateGuildMark( 6, "Data\\Guild\\Guild6.bmp" );
			g_pGuildMarkManager->CreateGuildMark( 7, "Data\\Guild\\Guild7.bmp" );
		#endif
		*/

		 

		//----------------------------------------
		
		//----------------------------------------
		TraceGameStartup("before ProfileManager::DeleteProfiles");
		ProfileManager::DeleteProfiles();		
		TraceGameStartup("ProfileManager::DeleteProfiles OK");
		TraceGameStartup("before ProfileManager::InitProfiles");
		ProfileManager::InitProfiles();			
		TraceGameStartup("ProfileManager::InitProfiles OK");


			//----------------------------------------
			
			//----------------------------------------
			//UI_DrawProgress(70);

			//LoadCreature( 0 );
			//UI_DrawProgress(80);

			//LoadCreature( 1 );
			//UI_DrawProgress(90);

			//LoadCreature( 2 );
			//UI_DrawProgress(95);

			//LoadCreature( 3 );
			//UI_DrawProgress(100);
		
			//gC_vs_ui.EndProgress();

			
			//SetMode( MODE_OPENING );

			// Debug: Check g_Mode before and after SetMode
			extern enum CLIENT_MODE g_Mode;
			TraceGameStartup("before SetMode MODE_MAINMENU");
			printf("DEBUG: Before SetMode, g_Mode = %d\n", g_Mode);
			SetMode( MODE_MAINMENU );
			printf("DEBUG: After SetMode, g_Mode = %d\n", g_Mode);
			TraceGameStartup("SetMode MODE_MAINMENU OK");
			//SetMode( MODE_WAIT_POSITION );
		///*
		
		//*/

		// Debug Message
		#ifdef	OUTPUT_DEBUG			
			// Fullscreen?
			if (CSDLGraphics::IsFullscreen())	
			{
				DEBUG_ADD("# Fullscreen.");
			}
			else
			{
				DEBUG_ADD("# Windowed Mode.");
			}

			// 565?
			if (CSDLGraphics::Is565())	
			{
				DEBUG_ADD("# 5:6:5 mode");
			}
			else
			{
				DEBUG_ADD("# 5:5:5 mode or ???.");
			}

			// 3D HAL?
			if (true)	
			{
				DEBUG_ADD("# 3D Hardware Accel.");
			}
			
			DEBUG_ADD("---------------[  InitGame OK  ]---------------");
		#endif
	
		TraceGameStartup("InitGame OK returning TRUE");
		return TRUE;
	}

	
	return FALSE;	
}

//-----------------------------------------------------------------------------
// Init Socket
//-----------------------------------------------------------------------------
BOOL
InitSocket()
{
	
	auto TraceInitSocket = [](const char* step)
	{
		(void)step;
	};

	TraceInitSocket("InitSocket begin");
	ReleaseSocket();

	// Debug Message
	DEBUG_ADD("[ InitGame ]  Socket");

#ifdef PLATFORM_WINDOWS
	if (!EnsureWinsockStarted())
	{
		TraceInitSocket("InitSocket WSAStartup failed");
		DEBUG_ADD("[ InitGame ]  Socket - WSAStartup failed");
		SetMode( MODE_MAINMENU );
		UpdateDisconnected();
		return FALSE;
	}
	TraceInitSocket("InitSocket WSAStartup OK");
#endif


	//----------------------------------------------------------------------
	
	
	//----------------------------------------------------------------------

	try {
		DEBUG_ADD("[ InitGame ]  Socket - Before new Properties");
		TraceInitSocket("InitSocket loading properties");

		std::string configFilename;

		if( !g_pUserInformation->bKorean )
		{
			configFilename = g_pFileDef->getProperty("FILE_INFO_SERVERINFO").c_str();
			g_pConfigForeign = new ServerInfoFileParser( configFilename );
		}
		else
		{
			switch(g_Dimension)
			{
				//		case 0:
				//			configFilename = FILE_INFO_GAMECLIENT;
				//			break;
				
			case 1:
				configFilename = g_pFileDef->getProperty("FILE_INFO_GAMECLIENT2").c_str();
				break;
				
			default:
				configFilename = g_pFileDef->getProperty("FILE_INFO_GAMECLIENT").c_str();
				break;
			}
			
			g_pConfigKorean = new Properties();
			g_pConfigKorean->load(configFilename);			
		}
		TraceInitSocket("InitSocket properties loaded");

	} catch ( Throwable& t ) 	
	{
		TraceInitSocket(t.toString().c_str());
		TraceInitSocket("InitSocket properties failed");
		//InitFail(t.toString().c_str());				

		DEBUG_ADD_ERR(t.toString().c_str());

		SetMode( MODE_MAINMENU );
		UpdateDisconnected();


		return FALSE;
	}	

	//----------------------------------------------------------------------
	// Create Socket 
	//----------------------------------------------------------------------
	Socket * pSocket = NULL;
	try {		
		DEBUG_ADD("[ InitGame ]  Socket - Before new PacketFactoryManager");
		TraceInitSocket("InitSocket creating packet managers");

		
		g_pPacketFactoryManager = new PacketFactoryManager();

		DEBUG_ADD("[ InitGame ]  Socket - Before new PacketValidator");

		g_pPacketValidator = new PacketValidator();

		DEBUG_ADD("[ InitGame ]  Socket - Before Init...");

		g_pPacketFactoryManager->init();
		g_pPacketValidator->init();
		
		DEBUG_ADD("[ InitGame ]  Socket - Before new Socket...");

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		int maxAddress = 1;

		try {
			if( g_pUserInformation->bKorean )
				maxAddress = atoi(g_pConfigKorean->getProperty("MaxLoginServerAddress").c_str());
			else
				maxAddress = atoi( g_pConfigForeign->getProperty( g_Dimension, "MaxLoginServerAddress").c_str() );
		} catch (NoSuchElementException) {
			//maxAddress = 1;
		}
		TraceInitSocket("InitSocket resolved maxAddress");

		
		static int previousTryServer = 0;
		
			
		//for (int i=0; i<maxAddress; i++)
		{
			
			
			int i = previousTryServer % maxAddress;

			try {				
				std::string serverAddressString;
				serverAddressString = "LoginServerAddress";
				
				
				uint port;
				if( g_pUserInformation->bKorean )
					port = g_pConfigKorean->getPropertyInt("LoginServerPort");
				else
					port = g_pConfigForeign->getPropertyInt(g_Dimension, "LoginServerPort");


				
				std::string ServerAddress;				
				if (g_FutecPort==0)
				{				
					if (i!=0)
					{
						char str[10];
						sprintf(str, "%d", i);
						serverAddressString += str;
					}
					
					if( g_pUserInformation->bKorean )
						ServerAddress = g_pConfigKorean->getProperty(serverAddressString);
					else
						ServerAddress = g_pConfigForeign->getProperty( g_Dimension, serverAddressString);

					
					try {
						int portNum;
						if( g_pUserInformation->bKorean )
							portNum = g_pConfigKorean->getPropertyInt("LoginServerPortNum");
						else
							portNum = g_pConfigForeign->getPropertyInt( g_Dimension, "LoginServerPortNum");
						
						
						if (portNum>1)
						{
							if( g_pUserInformation->bKorean )
								port = g_pConfigKorean->getPropertyInt("LoginServerBasePort") + rand()%portNum;
							else
								port = g_pConfigForeign->getPropertyInt( g_Dimension, "LoginServerBasePort") + rand()%portNum;
						}
					} catch (NoSuchElementException&) {
					}
				}
				
				else
				{
					ServerAddress = g_FutecIP;
					port = g_FutecPort;
				}

				
				if (ServerAddress[0] < '0' || ServerAddress[0] > '9')
				{
					struct hostent* h;

					if ((h=gethostbyname(ServerAddress.c_str()))==NULL)
					{
						// -_-;
						throw ConnectException("LoginServer    .");
					}

					char* pIP = (char*)inet_ntoa(*((struct in_addr*)h->h_addr));

					ServerAddress = pIP;
				}
				
				//yckou: prohibit user connecting to local machine
				//--------------------------------------------------------------------
 
				//--------------------------------------------------------------------
				DEBUG_ADD_FORMAT("Conneting( %s : %d )", ServerAddress.c_str() , port);				
				{
					char trace[256];
					sprintf(trace, "InitSocket connecting %s:%u", ServerAddress.c_str(), port);
					TraceInitSocket(trace);
				}

				pSocket = new Socket( ServerAddress , port );

				DEBUG_ADD("[ InitGame ]  Socket - Before connect");
				TraceInitSocket("InitSocket socket allocated");

				// try to connect to server
				pSocket->connect();
				TraceInitSocket("InitSocket connect OK");
/*				if ( ServerAddress[0] == 0x31 
					&& ServerAddress[1] == 0x32
					&& ServerAddress[2] == 0x37)
				{
					return FALSE;
				}
*/
//				SOCKADDR_IN sa;
//				int lenSA = sizeof(sa);
//				getsockname( pSocket->getSOCKET(), (sockaddr*)&sa, &lenSA );
//				char str[128];


				
				//break;

			} catch ( ConnectException ) {
				TraceInitSocket("InitSocket connect exception");

				if (pSocket!=NULL)
				{
					delete pSocket;
					pSocket = NULL;
				}

				
				previousTryServer ++;

				
				//if (i==maxAddress-1)
				throw;
			}
		}



		// make nonblocking socket
	    pSocket->setNonBlocking();
		TraceInitSocket("InitSocket setNonBlocking OK");

		// make no-linger socket
		pSocket->setLinger(0);
		TraceInitSocket("InitSocket setLinger OK");

		DEBUG_ADD("[ InitGame ]  Socket - new ClientPlayer");

	    // create player
		g_pSocket = new ClientPlayer(pSocket);
		TraceInitSocket("InitSocket ClientPlayer created");
		pSocket = NULL;
	 
	} catch ( Throwable & t ) {	
		TraceInitSocket(t.toString().c_str());
		TraceInitSocket("InitSocket outer create failed");
		//MessageBox( g_hWnd, t.toString().c_str(), NULL, MB_OK );
		//InitFail(t.toString().c_str());
		DEBUG_ADD(t.toString().c_str());

		if (pSocket!=NULL)
		{
			delete pSocket;
		}

		SetMode( MODE_MAINMENU );		
		UpdateDisconnected();


		return FALSE;
	}

	//----------------------------------------------------------------------
	// RequestServerPlayerManager
	//----------------------------------------------------------------------
	if (g_pProfileManager!=NULL)
	{
		g_pProfileManager->ReleaseRequire();
	}

	if (g_pRequestServerPlayerManager!=NULL)
	{
		DEBUG_ADD("[ InitGame ]  delete RequestServerPlayerManager");
		delete g_pRequestServerPlayerManager;
	}

	if (g_pRequestClientPlayerManager!=NULL)
	{
		DEBUG_ADD("[ InitGame ]  delete RequestClientPlayerManager");
		delete g_pRequestClientPlayerManager;
	}

	if (g_pClientCommunicationManager!=NULL)
	{
		DEBUG_ADD("[ InitGame ]  delete ClientCommunicationManager");
		delete g_pClientCommunicationManager;
	}

	if (g_pRequestUserManager!=NULL)
	{
		DEBUG_ADD("[ InitGame ]  delete g_pRequestUserManager");
		delete g_pRequestUserManager;		
	}

	if (g_pWhisperManager!=NULL)
	{
		DEBUG_ADD("[ InitGame ]  delete g_pWhisperManager");
		delete g_pWhisperManager;		
	}

	if (g_pRequestFileManager!=NULL)
	{
		DEBUG_ADD("[ InitGame ]  delete g_pRequestFileManager");
		delete g_pRequestFileManager;		
	}

	TraceInitSocket("InitSocket end success");
	

	DEBUG_ADD("[ InitGame ] new ClientCommunicationManager");
	g_pClientCommunicationManager = new ClientCommunicationManager;

	#ifdef __METROTECH_TEST__
		g_UDPTest.Init();
	#endif

	// RequestServer functionality is for server mode only - not needed on macOS client
#ifndef PLATFORM_MACOS
	if (g_pClientConfig->MAX_REQUEST_SERVICE > 0)
	{
		DEBUG_ADD("[ InitGame ] new RequestServerPlayerManager");
		g_pRequestServerPlayerManager = new RequestServerPlayerManager;

		DEBUG_ADD("[ InitGame ] new RequestClientPlayerManager");
		g_pRequestClientPlayerManager = new RequestClientPlayerManager;

		DEBUG_ADD("[ InitGame ] RequestServerPlayerManager Init");
		g_pRequestServerPlayerManager->Init();
	}
#else
	// On macOS, disable server functionality to reduce CPU usage
	DEBUG_ADD("[ InitGame ] RequestServer functionality disabled on macOS");
#endif


	DEBUG_ADD("[ InitGame ] new g_pRequestUserManager");
	g_pRequestUserManager = new RequestUserManager;

	DEBUG_ADD("[ InitGame ] new g_pWhisperManager");
	g_pWhisperManager = new WhisperManager;

	DEBUG_ADD("[ InitGame ] new g_pRequestFileManager");
	g_pRequestFileManager = new RequestFileManager;



	DEBUG_ADD("[ InitGame ]  Socket OK");

	return TRUE;
}


//-----------------------------------------------------------------------------
// ReleaseGameUpdate()
//-----------------------------------------------------------------------------
void
ReleaseGameUpdate()
{
	if (g_pCOpeningUpdate!=NULL)
	{
		delete	g_pCOpeningUpdate;
		g_pCOpeningUpdate = NULL;
	}

	if (g_pCGameUpdate!=NULL)
	{
		delete	g_pCGameUpdate;
		g_pCGameUpdate = NULL;
	}

	if (g_pCWaitPacketUpdate!=NULL)
	{
		delete	g_pCWaitPacketUpdate;
		g_pCWaitPacketUpdate = NULL;
	}

	if (g_pCWaitUIUpdate!=NULL)
	{
		delete	g_pCWaitUIUpdate;
		g_pCWaitUIUpdate = NULL;
	}

}

//-----------------------------------------------------------------------------
// Name: ReleaseAllObjects()
// Desc: Finished with all objects we use; release them
//-----------------------------------------------------------------------------
void ReleaseAllObjects()
{
	DEBUG_ADD("---------------[ Start ReleaseAllObjects ]---------------");

	g_bActiveGame = false;

	EndTitleLoading();
	
	//----------------------------------------------------------------
	
	//
	
	//----------------------------------------------------------------
	ReleaseGameUpdate();

	/*
	if (g_pMP3 != NULL)
	{
		g_pMP3->Pause();
		delete g_pMP3;
	}
	*/

#ifndef __USE_MP3__

	if(g_pOGG != NULL )
	{		
		g_pOGG->streamClose();
		delete g_pOGG;
		g_pOGG = NULL;
		if( g_oggfile != NULL )
		{
			fclose(g_oggfile);
			g_oggfile = NULL;
		}
		delete g_pSoundBufferForOGG;
		g_pSoundBufferForOGG = NULL;
	}
#endif

	//----------------------------------------------------------------
	//
	//				 thread
	//
	//----------------------------------------------------------------
	/*
	if (g_hFileThread != 0)
	{
		StopFileThread();
		TerminateThread(g_hFileThread, 0);
		CloseHandle(g_hFileEvent);
	}
	*/
	DEBUG_ADD("[Release] Work Thread");

	if (g_pLoadingThread!=NULL)
	{
		delete g_pLoadingThread;
		g_pLoadingThread = NULL;
	}

	
	//----------------------------------------------------------------
	// 
	
	//
	//----------------------------------------------------------------
	ReleasePacketItemTable();

	
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	if (g_pUIDialog!=NULL)
	{
		DEBUG_ADD("[Release] UI Dialog");
		g_pUIDialog->Release();
	}

	DEBUG_ADD("[Release] UI");
	gC_vs_ui.Release();


	//----------------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	// Weather
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Weather");
	if (g_pWeather!=NULL)
	{
		delete g_pWeather;
		g_pWeather = NULL;
	}

	//----------------------------------------------------------------
	// EffectManager
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Inventory Effect Manager");
	if (g_pInventoryEffectManager != NULL)
	{
		delete g_pInventoryEffectManager;
		g_pInventoryEffectManager = NULL;
	}
	
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] g_pPlayer");
	if (g_pPlayer!=NULL)
	{
		if (g_pZone!=NULL)
		{
			g_pZone->RemoveCreature( g_pPlayer->GetID() );
		}
		SAFE_DELETE( g_pPlayer );		
	}

	//----------------------------------------------------------------
	// Zone
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Zone");
	SAFE_DELETE( g_pZoneSmall );
	SAFE_DELETE( g_pZoneLarge );
	g_pZone = NULL;
	
	DEBUG_ADD("[Release] MCreature::MoveTable");
	MCreature::ReleaseMoveTable();

	
	DEBUG_ADD("[Release] DirectInput");

	//----------------------------------------------------------------
	//
	//				System	& Util..
	//
	//----------------------------------------------------------------
	//----------------------------------------------------------------
	// Input
	//----------------------------------------------------------------
	SAFE_DELETE( g_pSDLInput );
	//----------------------------------------------------------------
	// Music
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Music");
	g_Music.Stop();
	g_Music.UnInit();
	//g_SDLMusic.Stop();

	//----------------------------------------------------------------
	// Sound
	//----------------------------------------------------------------
//	DEBUG_ADD("[Release] SoundStream");
//	if (g_pSDLStream!=NULL)
//	{
//		delete g_pSDLStream;
//		g_pSDLStream = NULL;
//	}
	DEBUG_ADD("[Release] MP3");
	SAFE_DELETE( g_pMP3 );	
	DEBUG_ADD("[Release] ZoneSound");


	SAFE_DELETE( g_pZoneSoundTable );
	SAFE_DELETE( g_pZoneSoundManager );
	SAFE_DELETE( g_pWavePackFileManager );
	
	DEBUG_ADD("[Release] Sound");

	UnInitSound();

	

	//----------------------------------------------------------------
	// Avi
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] AVI");

	if (g_pAvi!=NULL)
	{
		g_pAvi->Close();
		delete g_pAvi;
		g_pAvi = NULL;
	}


	//----------------------------------------------------------------
	// UnInit DirectInput
	//----------------------------------------------------------------
	//	this->g_di.UnInitJoyDevice();
	//g_pSDLInput->UnInitMouseDevice();
	//g_pSDLInput->UnInitKeyDevice();
	//g_pSDLInput->UnInit();

	//----------------------------------------------------------------
	// View
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] MTopView");
	
	if (g_pTopView!=NULL)
	{
		
		
		
		//if (g_bNeedUpdate)
		{
			g_pTopView->Release();
			delete g_pTopView;
		}

		g_pTopView = NULL;
	}

	

	//----------------------------------------------------------------
	// Cursor Surface
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] g_pCursorSurface");
	SAFE_DELETE(g_pCursorSurface );
	
	//----------------------------------------------------------------
	// delete Surfaces
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Back Surface");
	SAFE_DELETE( g_pBack );
	
	DEBUG_ADD("[Release] Last Surface");
	SAFE_DELETE( g_pLast );
	

	//----------------------------------------------------------------
	// Socket
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Socket");
	ReleaseSocket();

	//----------------------------------------------------------------
	// Socket cleanup (mingw socket doesn't need explicit cleanup)
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Socket cleaned up");

	//----------------------------------------------------------------
	// Volume
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] Volume");
	// Volume control is now handled by SDL_mixer

	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	//if (g_bNeedUpdate)
	{
		if (g_pActionInfoTable) g_pActionInfoTable->Release();	
		if (g_pCreatureSpriteTable!=NULL) g_pCreatureSpriteTable->Release();
//		if (g_pAddonSpriteTable!=NULL) g_pAddonSpriteTable->Release();
//		if (g_pCreatureActionSpriteTable!=NULL) g_pCreatureActionSpriteTable->Release();
		if (g_pCreatureTable) g_pCreatureTable->Release();
		if (g_pCreatureSpriteTypeMapper) g_pCreatureSpriteTypeMapper->Release();
		// FIXED: Delete immediately after Release to prevent use-after-free
		// Previously: Release() was called here, but SAFE_DELETE was 50+ lines later
		// This created a window where the table existed with m_pTypeInfo=NULL
		SAFE_DELETE( g_pEffectSpriteTypeTable );
		SAFE_DELETE( g_pActionEffectSpriteTypeTable );
		if (g_pEffectStatusTable) g_pEffectStatusTable->Release();
		if (g_pGameMessage!=NULL) g_pGameMessage->Release();
		if (g_pGameStringTable) g_pGameStringTable->Release();
		if (g_pInteractionObjectTable) g_pInteractionObjectTable->Release();
		if (g_pItemOptionTable) g_pItemOptionTable->Release();
		if (g_pNPCTable) g_pNPCTable->Release();
		if (g_pNPCTalkBox) g_pNPCTalkBox->Release();
		if (g_pPCTalkBox) g_pPCTalkBox->Release();
		if (g_pShopTemplateTable) g_pShopTemplateTable->Release();
		if (g_pSkillAvailable) g_pSkillAvailable->clear();
		if (g_pSkillInfoTable) g_pSkillInfoTable->Release();
		if (g_pSkillManager) g_pSkillManager->Release();
		if (g_pSlayerGear) g_pSlayerGear->Release();
		if (g_pVampireGear) g_pVampireGear->Release();
		if (g_pOustersGear) g_pOustersGear->Release();
		if (g_pInventory) g_pInventory->Release();
		if (g_pSoundManager) g_pSoundManager->Release();
		if (g_pSoundTable) g_pSoundTable->Release();
		if (g_pSystemMessage) g_pSystemMessage->Release();	
		if (g_pPlayerMessage) g_pPlayerMessage->Release();
		if (g_pUsingColorSetTable) g_pUsingColorSetTable->Release();
		if (g_pZoneTable) g_pZoneTable->Release();
		if (g_pZoneSoundManager) g_pZoneSoundManager->Release();
		if (g_pNPCScriptTable) g_pNPCScriptTable->Release();
		
		// 2004, 06, 18, sobeit add start - nick name table
		if (g_pNickNameStringTable!=NULL) g_pNickNameStringTable->Release();
		// 2004, 06, 18, sobeit add end - nick name table

	}

	SAFE_DELETE( g_pRegenTowerInfoManager );
	SAFE_DELETE( g_pSystemAvailableManager );
	SAFE_DELETE( g_pWorldPlayerConfigTable );
	SAFE_DELETE( g_pServerInformation );
	SAFE_DELETE( g_pUserInformation );
	SAFE_DELETE( g_pTempInformation );
	SAFE_DELETE( g_pGameMessage );
	SAFE_DELETE( g_pHelpMessage );
	SAFE_DELETE( g_pNoticeMessage );
	SAFE_DELETE( g_pSystemMessage );
	SAFE_DELETE( g_pPlayerMessage );
	SAFE_DELETE( g_pGameStringTable );
	SAFE_DELETE( g_pClientConfig );
	SAFE_DELETE( g_pUserOption );
	SAFE_DELETE( g_pUIDialog );
	SAFE_DELETE( g_pProfileManager );
	SAFE_DELETE( g_pGuildMarkManager );
	SAFE_DELETE( g_pGuildInfoMapper );
	SAFE_DELETE( g_pEventManager );
	// g_pEffectSpriteTypeTable and g_pActionEffectSpriteTypeTable already deleted above
	SAFE_DELETE( g_pCreatureTable );
	SAFE_DELETE( g_pCreatureSpriteTypeMapper );
	SAFE_DELETE( g_pItemTable );
	SAFE_DELETE( g_pCreatureSpriteTable );
//	SAFE_DELETE( g_pAddonSpriteTable );
//	SAFE_DELETE( g_pCreatureActionSpriteTable );
	SAFE_DELETE( g_pEffectStatusTable );
	SAFE_DELETE( g_pInteractionObjectTable );
	SAFE_DELETE( g_pItemOptionTable );
	SAFE_DELETE( g_pNPCTable );
	SAFE_DELETE( g_pJusticeAttackManager );
	SAFE_DELETE( g_pNPCScriptTable );	
	SAFE_DELETE( g_pShopTemplateTable );
	SAFE_DELETE( g_pSkillInfoTable );
	SAFE_DELETE( g_pRankBonusTable );
	SAFE_DELETE( g_pExperienceTable );
	SAFE_DELETE( g_pSoundTable );
	SAFE_DELETE( g_pMusicTable );
	SAFE_DELETE( g_pZoneTable );
	SAFE_DELETE( g_pLevelNameTable );
	SAFE_DELETE( g_pQuestInfoManager );
	SAFE_DELETE( g_pSoundManager );
	SAFE_DELETE( g_pSkillAvailable );
	SAFE_DELETE( g_pSkillManager );
	SAFE_DELETE( g_pSlayerGear );
	SAFE_DELETE( g_pVampireGear );
	SAFE_DELETE( g_pOustersGear );
	SAFE_DELETE( g_pInventory );
	SAFE_DELETE( g_pParty );
	SAFE_DELETE( g_pTradeManager );	
	SAFE_DELETE( g_pStorage );
	SAFE_DELETE( g_pStorage2 );
	SAFE_DELETE( g_pGameTime );
	SAFE_DELETE( g_pKeyAccelerator );
	SAFE_DELETE( g_pAcceleratorManager );
	SAFE_DELETE( g_pModifyStatusManager );
	SAFE_DELETE( g_pEffectGeneratorTable );
	SAFE_DELETE( g_pUsingColorSetTable );
	SAFE_DELETE( g_pActionInfoTable );
	SAFE_DELETE( g_pPriceManager );
	SAFE_DELETE( g_pMoneyManager );
	SAFE_DELETE( g_pPCTalkBox );
	SAFE_DELETE( g_pNPCTalkBox );
	SAFE_DELETE( g_pChatManager );	
	SAFE_DELETE( g_pHelpManager );
	SAFE_DELETE( g_pHelpStringTable );	
//	SAFE_DELETE( g_pCompareManager );
	SAFE_DELETE( g_pHelpDisplayer );
	SAFE_DELETE( g_pObjectSelector );	
	SAFE_DELETE( g_pUIMessageManager );	
	SAFE_DELETE( g_pMonsterNameTable  );
	SAFE_DELETE( g_pQuestInfoManager  );
	SAFE_DELETE( g_pTimeItemManager  );	
	SAFE_DELETE( g_pFameInfoTable );
	SAFE_DELETE( g_pWarManager );

	// 2004, 6, 18, sobeit add start
	SAFE_DELETE( g_pNickNameStringTable );
	// 2004, 6, 18, sobeit add end
	//----------------------------------------------------------------
	// DirectDraw
	//----------------------------------------------------------------
	DEBUG_ADD("[Release] CDirect3D");
	// CDirect3D::Release() removed (SDL2)

	DEBUG_ADD("[Release] CDirectDraw");
	CSDLGraphics::ReleaseAll();

	//---------------------------------------------------------------------
	// Profiler
	//---------------------------------------------------------------------
	DEBUG_ADD("[Release] Profiler");
	SAFE_DELETE( g_pProfiler );	
	DEBUG_ADD("[Release] All OK");		

	SAFE_DELETE(g_pFileDef);

#ifdef DEBUG_INFO
	ClearDebugInfo();
#endif
}

//-----------------------------------------------------------------------------
// Release Socket
//-----------------------------------------------------------------------------
void
ReleaseSocket()
{
	//----------------------------------------------------------------
	//
	//							socket
	//
	//----------------------------------------------------------------
		DEBUG_ADD("----- Start ReleaseSocket -----");
		
		if (g_pSocket!=NULL) 
		{			
			//g_pSocket->setPlayerStatus( CPS_END_SESSION );

			g_pSocket->disconnect(UNDISCONNECTED);

			//g_pSocket->getSocket()->close();			
			DEBUG_ADD("delete g_pSocket");
			
			delete g_pSocket;
			g_pSocket = NULL;
		}

		SAFE_DELETE( g_pConfigKorean );		
		SAFE_DELETE( g_pConfigForeign );

		if (g_pPacketFactoryManager != NULL)
		{
			DEBUG_ADD("delete g_pPacketFactoryManager");
			
			delete g_pPacketFactoryManager;
			g_pPacketFactoryManager = NULL;
		}			

		if (g_pPacketValidator != NULL)
		{
			DEBUG_ADD("delete g_pPacketValidator");
			
			delete g_pPacketValidator;
			g_pPacketValidator = NULL;
		}		

		if (g_pProfileManager!=NULL)
		{
			g_pProfileManager->ReleaseRequire();
		}
		
		if (g_pRequestServerPlayerManager!=NULL)
		{
			DEBUG_ADD("delete g_pRequestServerPlayerManager");
			delete g_pRequestServerPlayerManager;
			g_pRequestServerPlayerManager = NULL;
		}

		if (g_pRequestClientPlayerManager!=NULL)
		{
			DEBUG_ADD("delete g_pRequestClientPlayerManager");
			delete g_pRequestClientPlayerManager;
			g_pRequestClientPlayerManager = NULL;
		}

		if (g_pClientCommunicationManager!=NULL)
		{
			DEBUG_ADD("delete g_pClientCommunicationManager");
			delete g_pClientCommunicationManager;
			g_pClientCommunicationManager = NULL;
		}

		if (g_pWhisperManager!=NULL)
		{
			DEBUG_ADD("delete g_pWhisperManager");
			delete g_pWhisperManager;
			g_pWhisperManager = NULL;
		}

		if (g_pRequestFileManager!=NULL)
		{
			DEBUG_ADD("delete g_pRequestFileManager");
			delete g_pRequestFileManager;
			g_pRequestFileManager = NULL;
		}

		if (g_pRequestUserManager!=NULL)
		{
			DEBUG_ADD("delete g_pRequestUserManager");
			delete g_pRequestUserManager;
			g_pRequestUserManager = NULL;
		}

		
		DEBUG_ADD("----- End ReleaseSocket -----");
		
}

//-----------------------------------------------------------------------------
// UnInit Sound
//-----------------------------------------------------------------------------
void
UnInitSound()
{
	//-------------------------------------
	// UnInit DirectSound
	//-------------------------------------
	if (g_pSoundManager!=NULL)
	{
		g_pSoundManager->Release();	
	}

	g_SDLAudio.Release();
}

//-----------------------------------------------------------------------------
// Init GameObject
//-----------------------------------------------------------------------------
BOOL
InitGameObject()
{
	DEBUG_ADD("[ InitGame ]  GameObject");
	
	//----------------------------------------
	// 
	//----------------------------------------
	if (g_pSystemAvailableManager == NULL )
	{
		g_pSystemAvailableManager = new SystemAvailabilitiesManager;
		g_pSystemAvailableManager->LoadFromFile(g_pFileDef->getProperty("FILE_FILTER_INFO").c_str());
	}
	if (g_pServerInformation==NULL)
	{
		g_pServerInformation = new CServerInformation;
		g_pServerInformation->SetServerGroupName( "ServerGroup" );
		g_pServerInformation->SetServerName( "Server" );
	}

	if (g_pUserInformation==NULL)
	{
		g_pUserInformation = new UserInformation;
	}
	int nStartV = 73*25;
#ifdef __GAME_CLIENT__
	int version;
	std::ifstream versionFile;//(g_pFileDef->getProperty("FILE_INFO_ACTION").c_str(), ios::binary);
	if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_VERSION").c_str(), versionFile))
	{
		return FALSE;
	}
	versionFile.read((char*)&version, 4);
	versionFile.close();

	int i = 96;
	nStartV = nStartV*i;
//	g_pUserInformation->GameVersion = (406+nStartV)/2+88; //version=87891;
	//g_pUserInformation->GameVersion = 745389;2006-6-20
	
	//modify by viva for GameVersion
	g_pUserInformation->GameVersion = 7457757;
	//end

//	g_pUserInformation->GameVersion = 520909;//745389
#ifdef DEBUG_INFO
	if(g_nGameVersion > 0)
		g_pUserInformation->GameVersion = g_nGameVersion;
#endif

#endif
	
	if (g_pTempInformation==NULL)
	{
		g_pTempInformation = new TempInformation;
	}

	if (g_pSkillManager==NULL)
	{
		g_pSkillManager = new MSkillManager;		
	}

	if (g_pSkillAvailable==NULL)
	{
		g_pSkillAvailable = new MSkillSet;
	}

	if (g_pSlayerGear==NULL)
	{
		g_pSlayerGear = new MSlayerGear;
	}

	if (g_pVampireGear==NULL)
	{
		g_pVampireGear = new MVampireGear;
	}

	if (g_pOustersGear==NULL)
	{
		g_pOustersGear = new MOustersGear;
	}
	
	if (g_pInventory==NULL)
	{
		g_pInventory = new MInventory;
	}

	if (g_pParty==NULL)
	{
		g_pParty = new MParty;
	}
	else
	{
		g_pParty->Release();
	}


	//if (g_pTradeManager==NULL)
	//{
	//	g_pTradeManager = new MTradeManager;
	//}	

	if (g_pGameTime==NULL)
	{
		g_pGameTime = new MGameTime;
	}

	if (g_pPriceManager==NULL)
	{
		g_pPriceManager = new MPriceManager;
	}

	if (g_pMoneyManager==NULL)
	{
		g_pMoneyManager = new MMoneyManager;
	}

	if (g_pPCTalkBox==NULL)
	{
		g_pPCTalkBox = new PCTalkBox;
	}

	if (g_pNPCTalkBox==NULL)
	{
		g_pNPCTalkBox = new MStringList;
	}

	if (g_pUIMessageManager==NULL)
	{
		g_pUIMessageManager = new UIMessageManager;
		g_pUIMessageManager->Init();
	}
 
	if (g_pObjectSelector==NULL)
	{
		g_pObjectSelector = new MObjectSelector;
	}

	//----------------------------------------
	// Weather
	//----------------------------------------
	if (g_pWeather!=NULL)
	{
		delete g_pWeather;
	}
	g_pWeather = new MWeather;

	//----------------------------------------
	// ScreenEffectManager
	//----------------------------------------
	if (g_pInventoryEffectManager != NULL)
	{
		delete g_pInventoryEffectManager;			
	}
	g_pInventoryEffectManager = new MScreenEffectManager;

	//----------------------------------------
	
	//----------------------------------------
	if (g_pPlayer!=NULL)
	{	
		if (g_pZone!=NULL)
		{
			g_pZone->RemoveCreature( g_pPlayer->GetID() );
		}
	
		delete g_pPlayer;
	}

	
	g_pPlayer = new MPlayer;
	DEBUG_ADD_FORMAT("[g_pPlayer] %x", g_pPlayer);

	g_pPlayer->SetCreatureType( CREATURETYPE_SLAYER_MALE );


	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	g_pSkillManager->Init();

	return TRUE;
}
