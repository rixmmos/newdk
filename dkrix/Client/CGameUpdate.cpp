//-----------------------------------------------------------------------------
// CGameUpdate.cpp
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "Client_PCH.h"

#pragma warning(disable:4786)


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <MMSystem.h>
#else
#include "../../basic/Platform.h"
#endif
#include <string>
#include "TextSystem/TextService.h"
#include "Client.h"
#include "GameObject.h"
#include "UserInformation.h"
#include "ServerInfo.h"
#include "PacketDef.h"
#include "VS_UI.h"
#include "VS_UI_Mouse_pointer.h"
#include "UIDialog.h"
#include "DebugInfo.h"
#include "CGameUpdate.h"
#include "DXLibBackend.h"  // For SDL text input functions
#include "MMoneyManager.h"
#include "MGameStringTable.h"
#include "MObjectSelector.h"
#include "ClientFunction.h"
#include "MMusic.h"
#include "InputService.h"  // For DIK_* key code definitions
#include "Platform/InputManager.h"
#include "MZoneSoundManager.h"
#include "TempInformation.h"
#include "MFakeCreature.h"
#include "MParty.h"
#include "Profiler.h"
#include "MTestDef.h"
#include "MEventManager.h"
#include "UIFunction.h"
#include "cmp3.h"
#include "COGGSTREAM.H"
#include "SystemAvailabilities.h"
#include "MWarManager.H"
// EXECryptor and ThemidaSDK includes removed (SDL2) - Copy protection no longer needed
#include "packet/Cpackets/CGVerifyTime.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef OUTPUT_DEBUG
	#include "packet/Gpackets/GCSkillFailed2.h"
	#include "packet/Gpackets/GCSkillToObjectOK5.h"
	#include "packet/Gpackets/GCSkillToSelfOK2.h"
	#include "packet/Gpackets/GCSkillToTileOK2.h"
	#include "packet/Cpackets/CGTypeStringList.h"
#endif

//add by viva
#include "packet/Gpackets/GCFriendChatting.h"

#ifdef OUTPUT_DEBUG
	bool g_bSlideScreenShot = false;
	bool g_bSaveSlideScreenShot = false;
	bool g_bSlideRectSelect = false;
	RECT g_rectScreenShot = { 0, 0, 0, 0 };

	int gtx_op = 4;
	int gtx_src = 2;
	int gtx_dest = 2;
#endif

// Global
CGameUpdate*		g_pCGameUpdate = NULL;

bool g_bPreviousMove = false;
float g_RenderInterpolationAlpha = 0.0f;

static void TraceInteraction(const char* fmt, ...)
{
	(void)fmt;
}
	
extern DWORD g_double_click_time;

COGGSTREAM*					g_pOGG = NULL;
CDirectSoundBuffer*			g_pSoundBufferForOGG = NULL;
FILE	*					g_oggfile = NULL;

extern int g_SoundPerSecond;

extern int	g_previousSoundID;
extern bool g_bMouseInPortal;
		
extern POINT g_MouseSector;	
extern bool	g_bTestMode;

extern bool g_bZoneSafe;


extern void	SetWatchMode( bool );
extern bool g_bWatchMode;

extern void	CheckTime();
extern int g_MyCheckTime=0;
extern int g_CheckErrorTime=0;
extern BOOL g_MyFull;
extern RECT g_GameRect;
//add by sonic 2006.7.30

#ifdef PLATFORM_WINDOWS
	#include <tlhelp32.h>
	extern int GetCurrentUserNumber();
	extern int g_CheckTimeNum=0;
#else
	// Non-Windows platforms don't have tlhelp32.h
	// Forward declaration only - implementation is below
	extern int GetCurrentUserNumber();
	extern int g_CheckTimeNum=0;
#endif
//end
//extern bool CheckInvalidProcess();
extern void	KeepConnection();
extern BOOL	InitInfomation();
extern void	ExecuteLogout();

extern char	g_CWD[_MAX_PATH];

#define	MOUSE_POINTER_SKIP_LEFT		-16
#define	MOUSE_POINTER_SKIP_UP		-16

//#define __FIX_GUNFRAME__

#ifdef __FIX_GUNFRAME__
	extern int g_tempX;
	extern int g_tempY;
#endif


#if defined(OUTPUT_DEBUG) //&& defined(_DEBUG)
	#ifdef __METROTECH_TEST__
//		#define	OUTPUT_DEBUG_UPDATE_LOOP	
//		#define OUTPUT_DEBUG_PROCESS_INPUT

		extern int  g_iAutoHealPotion;
		extern bool g_bAutoManaPotion;
		extern int  g_iAutoReload;
		extern int	g_iSpeed;
		extern bool g_bLight;
	#endif
#endif

#ifdef OUTPUT_DEBUG
	extern int g_HISTORY_LINE;
#endif

bool	g_bFrameChanged	 = false;
bool	g_bTestMusic = false;

#ifdef OUTPUT_DEBUG
std::string g_musicfilename[8]=
{
	"data\\music\\blood.mp3",
	"data\\music\\chaos.mp3",
	"data\\music\\dominator of darkness.mp3",
	"data\\music\\oblivion.mp3",
	"data\\music\\rest.mp3",
	"data\\music\\ruin.mp3",
	"data\\music\\silence of battlefield.mp3",
	"data\\music\\underworld.mp3"
};

int			g_CurrentMusicNum = 0;
#endif

extern MCreature* AddClientCreature();

void	PacketTest();
bool IsExistCorpseFromPlayer(MCreature* OriginCreature, int creature_type);
bool			HasEffectStatusSummonSylph( MCreature* pCreature );

extern void	SetFlagTo( bool bTae );

static void TraceGameUpdate(const char* step)
{
	(void)step;
}

static void TraceGameHeartbeat(const char* step)
{
	(void)step;
}

static void TraceRenderPerfFrame(DWORD topViewMs, DWORD uiMs, DWORD cursorMs, DWORD fpsMs, DWORD copyMs, DWORD flipMs, DWORD totalMs)
{
	(void)topViewMs;
	(void)uiMs;
	(void)cursorMs;
	(void)fpsMs;
	(void)copyMs;
	(void)flipMs;
	(void)totalMs;
}

//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void
CGameUpdate::Init()
{
	TraceGameUpdate("CGameUpdate::Init begin");
	g_bPreviousMove = false;

	
	if (g_pSDLInput == NULL)
	{
		TraceGameUpdate("CGameUpdate::Init ERROR g_pSDLInput NULL");
		return;
	}
	g_pSDLInput->SetMouseEventReceiver( DXMouseEvent );
	TraceGameUpdate("CGameUpdate::Init after SetMouseEventReceiver");

	
	g_pSDLInput->SetKeyboardEventReceiver( DXKeyboardEvent );
	TraceGameUpdate("CGameUpdate::Init after SetKeyboardEventReceiver");

	
	platform_input_set_textinput_callback(SDLTextInputEvent);
	TraceGameUpdate("CGameUpdate::Init after set_textinput_callback");
	platform_input_start_text();  // Enable SDL text input
	TraceGameUpdate("CGameUpdate::Init end");
}

//-----------------------------------------------------------------------------
// DXMouseEvent
//-----------------------------------------------------------------------------
void		
CGameUpdate::DXMouseEvent(InputManager::E_MOUSE_EVENT event, int x, int y, int z)
{
	POINT point;

	if(g_pEventManager->GetEventByFlag(EVENTFLAG_DENY_INPUT_MOUSE))
	{
//		g_pEventManager->RemoveAllEvent();
		return;
	}

	static DWORD	last_click_time;
	static int		double_click_x, double_click_y;
	
	switch (event)
	{
			case InputManager::LEFTDOWN :
//				if ((DWORD)abs(GetTickCount() - last_click_time) <= g_double_click_time)
//				{
//					if (g_x>= double_click_x-1 && g_x <= double_click_x+1 &&
//						g_y>= double_click_y-1 && g_y <= double_click_y+1)
//					{
//						#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//						DEBUG_ADD("MLD");
//						#endif
//						
//						gC_vs_ui.MouseControl(M_LB_DOUBLECLICK, g_x, g_y);
//						
//						g_pSDLInput->m_lb_down = false;
//						g_pSDLInput->m_lb_up = false;
//						
//						#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//						DEBUG_ADD("MLD2");
//						#endif
//						
//						last_click_time = 0;
//						return;
//					}
//				}				
//				
				gC_vs_ui.MouseControl(M_LEFTBUTTON_DOWN, g_x, g_y);
				last_click_time = GetTickCount();
				double_click_x = g_x;
				double_click_y = g_y;
				
				#ifdef OUTPUT_DEBUG			
				if (g_pSDLInput->KeyDown(DIK_LSHIFT) && g_bSlideScreenShot)
				{
					g_rectScreenShot.left = g_x;
					g_rectScreenShot.right = g_x;
					g_rectScreenShot.top = g_y;
					g_rectScreenShot.bottom = g_y;
				}

				//---------------------------------------------------------
				
				//---------------------------------------------------------
				 
				#endif
			break;

			case InputManager::RIGHTUP :
				
				gC_vs_ui.EndInstallMineProgress();
				//gC_vs_ui.EndCreateMineProgress();
				//gC_vs_ui.EndCreateBombProgress();
			break;

			//case InputManager::RIGHTDOWN :
				//---------------------------------------------------------
				
				//---------------------------------------------------------
				
				//gC_vs_ui.ChatMouseControlExtra( M_RIGHTBUTTON_DOWN, g_x, g_y );
			//break;


		case InputManager::WHEELDOWN:
			gC_vs_ui.MouseControl(M_WHEEL_DOWN, g_x, g_y);
			///gC_vs_ui.ChatMouseControlExtra( M_WHEEL_DOWN, g_x, g_y );
			break;

		case InputManager::WHEELUP:
			gC_vs_ui.MouseControl(M_WHEEL_UP, g_x, g_y);
//			gC_vs_ui.ChatMouseControlExtra( M_WHEEL_UP, g_x, g_y );
			break;
	}
}

//#include "PacketDef.h"
				
//-----------------------------------------------------------------------------
// DXKeyboardEvent
//-----------------------------------------------------------------------------
void	
CGameUpdate::DXKeyboardEvent(InputManager::E_KEYBOARD_EVENT event, DWORD key)
{
	if(g_pEventManager->GetEventByFlag(EVENTFLAG_DENY_INPUT_KEYBOARD))
	{
		return;
	}

	if (event==InputManager::KEYDOWN)
	{
		if(key == 0xcc) return;		

		// Convert DIK scan codes to VK virtual key codes for control keys
		// These keys need WM_KEYDOWN messages for text editing to work
		UINT vk_key = 0;
		bool is_control_key = false;

		// DIK constants that might not be defined in all build configurations
		enum {
			DIK_CONST_BACK = 0x0E,
			DIK_CONST_DELETE = 0xD3,
			DIK_CONST_INSERT = 0xD2
		};

		switch (key) {
			case DIK_TAB:      vk_key = VK_TAB; is_control_key = true; break;
			case DIK_CONST_BACK:     vk_key = VK_BACK; is_control_key = true; break;
			case DIK_RETURN:   vk_key = VK_RETURN; is_control_key = true; break;
			case DIK_ESCAPE:   vk_key = VK_ESCAPE; is_control_key = true; break;
			case DIK_LEFT:     vk_key = VK_LEFT; is_control_key = true; break;
			case DIK_RIGHT:    vk_key = VK_RIGHT; is_control_key = true; break;
			case DIK_UP:       vk_key = VK_UP; is_control_key = true; break;
			case DIK_DOWN:     vk_key = VK_DOWN; is_control_key = true; break;
			case DIK_HOME:     vk_key = VK_HOME; is_control_key = true; break;
			case DIK_END:      vk_key = VK_END; is_control_key = true; break;
			case DIK_CONST_DELETE:   vk_key = VK_DELETE; is_control_key = true; break;
			case DIK_CONST_INSERT:   vk_key = VK_INSERT; is_control_key = true; break;
		}

		if (is_control_key) {
			static int keydown_debug_count = 0;
			if (keydown_debug_count < 10) {
				printf("  CGameUpdate DXKeyboardEvent: Sending WM_KEYDOWN, vk_key=%u (DIK=%lu)\n", vk_key, (unsigned long)key);
				keydown_debug_count++;
			}
#if !(defined(PLATFORM_WINDOWS) && (defined(USE_SDL_BACKEND) || defined(SPRITELIB_BACKEND_SDL)))
			gC_vs_ui.KeyboardControl(WM_KEYDOWN, vk_key, 0);
#endif
		}

		gC_vs_ui.DIKeyboardControl(event, key);
	
		switch (key)
		{	
//			__BEGIN_HELP_EVENT
//				case DIK_LMENU :

//				
//					ExecuteHelpEvent( HE_PRESSED_ALT );					
//				break;
//			__END_HELP_EVENT				


//			case DIK_F10 :
//				gC_vs_ui.HotKey_F10();
//			break;

			case DIK_SPACE :
				if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL) )
				{
				//	gC_vs_ui.HotKey_SummonPet(); 
//					gC_vs_ui.HotKey_WindowToggle();
				}
			break;

			//------------------------------------
			// L-CONTROL + S
			//------------------------------------
			//case DIK_S :				
			//	if (g_pSDLInput->KeyDown(DIK_LCONTROL))
			//	{	
					/*
					switch (rand()%3)
					{
						case 0 : (*g_pUIDialog).PopupMessageDlg(UIDialog::MESSAGE_CANNOT_BUY_NO_SPACE); break;
						case 1 : (*g_pUIDialog).PopupMessageDlg(UIDialog::MESSAGE_CANNOT_BUY_NO_MONEY); break;
						case 2 : (*g_pUIDialog).PopupMessageDlg(UIDialog::MESSAGE_CANNOT_BUY_NO_ITEM); break;
					}
					*/
					 


					// test
					/*
					ExecuteActionInfoFromMainNode(
							MAGIC_LIGHT,
						
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
							g_pPlayer->GetDirection(),
							
							g_pPlayer->GetID(),
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
							
							64,
							
							NULL
					);
					*/

					//---------------------------------------------------
					//
					// PC Talk Box Test
					//
					//---------------------------------------------------
					//---------------------------------------------------
					
					//---------------------------------------------------
					//
					// [ TEST CODE ]
					//
					
					 
			//	}
			//break;

			//------------------------------------
			// FPS Toggle
			//------------------------------------
			case DIK_F : 
				if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
				{
					if(g_pEventManager->IsEvent(EVENTID_OUSTERS_FIN))
					{
						g_pEventManager->RemoveEvent(EVENTID_OUSTERS_FIN);
					}
//					else
//					{
//						MEvent event;
//						event.eventID = EVENTID_OUSTERS_FIN;
//						event.eventType = EVENTTYPE_ZONE;
//						event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
//						event.parameter1 = 0;
//						event.parameter4 = EVENTBACKGROUNDID_CLOUD;
//						g_pEventManager->AddEvent(event);
//					}
					(*g_pUserOption).DrawFPS = !(*g_pUserOption).DrawFPS;
//							g_abHolyLandBonusSkills[4] = !g_abHolyLandBonusSkills[4];
//							g_pSkillAvailable->SetAvailableSkills();
				}

				#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
					else if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
					{
						if (g_pPlayer->HasEffectStatus( EFFECTSTATUS_FADE_OUT ))
						{
							g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_FADE_OUT );
						}
						else
						{
							
							g_pPlayer->AddEffectStatus( EFFECTSTATUS_FADE_OUT, 5*1000 );
						}
					}
				#endif
			break;				

			//------------------------------------
			// Minimap Toggle
			//------------------------------------
			/*
			case DIK_M : 
				if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).DrawMinimap = !(*g_pUserOption).DrawMinimap;
				}
			break;

			//------------------------------------
			// DrawZoneName Toggle
			//------------------------------------
			case DIK_Z :
				if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).DrawZoneName = !(*g_pUserOption).DrawZoneName;
				}
			break;

			//------------------------------------
			// DrawGameTime Toggle
			//------------------------------------
			case DIK_T :
				if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).DrawGameTime = !(*g_pUserOption).DrawGameTime;
				}
			break;
			*/

			//------------------------------------
			// Blending Shadow
			//------------------------------------
			/*
			case DIK_V : 
				if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).BlendingShadow = !(*g_pUserOption).BlendingShadow;
					if (g_pTopView!=NULL)
					{
						g_pTopView->ClearShadowManager();
					}
				}
			break;
			*/

			//------------------------------------
			
			//------------------------------------
			 

			//------------------------------------
			
			//------------------------------------
			 

			//------------------------------------
			// DrawInterface Toggle
			//------------------------------------
			 
			
		
			#ifdef OUTPUT_DEBUG

		
				case DIK_SCROLL :
					if (g_bSlideScreenShot)
					{
						g_bSaveSlideScreenShot = !g_bSaveSlideScreenShot;
						g_bSlideRectSelect = false;
					}
				break;

				case DIK_S :
					if (g_pSDLInput->KeyDown(DIK_LMENU))
					{
						g_bSlideScreenShot = !g_bSlideScreenShot;
						g_bSlideRectSelect = false;
					}
				break;

				case DIK_P :
					if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU) )
					{
						bool en = true;
						if(g_abHolyLandBonusSkills[0] == true)
							en = false;
						else
							en = true;

						for(int i = 0; i < HOLYLAND_BONUS_MAX; i++)
						{
							g_abHolyLandBonusSkills[i] = en;
						}

						g_pSkillAvailable->SetAvailableSkills();

//						if (g_pProfiler!=NULL)
//						{
//							g_pProfiler->WriteToFile("profiler.txt", true);
//						}
					}
				break;

				#if defined(_DEBUG) || defined(OUTPUT_DEBUG)
					case DIK_F1 :
					{
						
						if (g_pSDLInput->KeyDown(DIK_LMENU) 
							|| g_pSDLInput->KeyDown(DIK_RMENU))
						{
							InitInfomation();
							InitSound();
						}					
					}
					break;

					//------------------------------------------------------------
					//
					
					//
					//------------------------------------------------------------
					case DIK_L : 
					{
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						{
							MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();					

							for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
							{
								MCreature* pCreature = (*iCreature).second;

								if (pCreature!=g_pPlayer)
								{
									
									
									//int targetID;						
										
									//int nth;
									
									MZone::CREATURE_MAP::const_iterator iCreature2;

									
									if (rand()%2==0)
									{
										GCSkillToTileOK2 gcSkillToTileOK2;

										gcSkillToTileOK2.setObjectID( pCreature->GetID() );
										gcSkillToTileOK2.setSkillType( ((rand()%2)? 34 : 37) );
										gcSkillToTileOK2.setX( pCreature->GetX() );
										gcSkillToTileOK2.setY( pCreature->GetY() );
										gcSkillToTileOK2.setRange( rand()%8 );
										gcSkillToTileOK2.setDuration( 0 );
										gcSkillToTileOK2.addCListElement( g_pPlayer->GetID() );
										gcSkillToTileOK2.addShortData( (ModifyType)12, 97 );
										gcSkillToTileOK2.addLongData( (ModifyType)48, 68002 );										

//										DEBUG_ADD_FORMAT("[Execute] %s", gcSkillToTileOK2.toString().c_str());

										gcSkillToTileOK2.execute( g_pSocket );
										
										DEBUG_ADD("TileOK2OKOK");
									}
									 
								}

								iCreature++;
							}
						}
					}
					break;

					case DIK_1 :
//						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//						{
//							if( g_pSoundBufferForOGG == NULL )
//								g_pSoundBufferForOGG = new CDirectSoundBuffer(g_hWnd, SOUND_STEREO, SOUND_44K, SOUND_16BIT);
//
//							if( g_pOGG == NULL )
//								g_pOGG = new COGGSTREAM(g_hWnd, g_pSoundBufferForOGG, 44100, 11025, 8800,1);
//							
//							if( g_oggfile != NULL)
//								fclose(g_oggfile );
//							g_oggfile = fopen("C:\\ogggggg\\3\\blood.ogg","rb");
//							g_pOGG->streamLoad(g_oggfile, NULL);
//							g_pOGG->streamPlay(SOUND_PLAY_REPEAT);
//							
////							gC_vs_ui.SetOustersSkillDown();
////							g_pMP3->Stop();
////							g_pMP3->Open("data\\music\\blood.mp3");
////							g_pMP3->Play( true );
////							g_bTestMusic = false;

//						}

						if(g_pSDLInput->KeyDown(DIK_LCONTROL))
						{ 
							
							//g_pPlayer->SetSpecialActionInfo(TEMP_SKILL_INFINITY_THUNDERBLOT);
//							if( g_pEventManager->GetEvent( EVENTID_WAR_EFFECT ) == NULL )
//							{
//								MEvent event;
//								event.eventID = EVENTID_WAR_EFFECT;
//								event.eventType = EVENTTYPE_NULL;
//								event.eventFlag = EVENTFLAG_SHAKE_SCREEN | EVENTFLAG_FADE_SCREEN;
//								event.parameter2 = RGB( -30, -30, 0 );
//								event.parameter3 = 1;
//								event.eventDelay = -1;
//								event.showTime = 2000;
//								event.totalTime = 30000;
//								g_pEventManager->AddEvent( event );
//							}
//							else
//							{
//								g_pEventManager->RemoveEvent( EVENTID_WAR_EFFECT );
//							}
//							ExecuteActionInfoFromMainNode(SKILL_DESTRUCTION_SPEAR_MASTERY,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0, 100, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SUMMON_GDR,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							MEvent event;
//
//							event.eventID = EVENTID_GDR_PRESENT;
//							event.eventType = EVENTTYPE_ZONE;
//							event.eventFlag = EVENTFLAG_SHAKE_SCREEN;
//							event.parameter3 = 2;
//							event.eventDelay =5000;
//							g_pEventManager->AddEvent(event);
//							//SetFadeStart(31, 0, 1, 0,0,0, 6);
							//gC_vs_ui.RunNameing();
							//g_pPlayer->SetAction(ACTION_DRAINED);
//							g_pPlayer->m_ActionCount = 0;
							//g_pPlayer->SetCauseCriticalWounds(100);
							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_GLACIER, 100);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_BLINDNESS,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 30, NULL, false);
						//	g_pPlayer->AddEffectStatus(EFFECTSTATUS_GRAND_MASTER_SLAYER_130, 0xffff);
							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_BLINDNESS, 10);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_CAN_ENTER_GDR_LAIR, 10);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_ICE_STICK_SE, 10);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_GDR_GLACIER_SLAYER, 100);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_GRAND_MASTER_OUSTERS_130, 0xffff);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_GRAND_MASTER_OUSTERS_150, 0xffff);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SLAYER_MASTER_130,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SLAYER_MASTER_150,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
////
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_VAMPIRE_MASTER_130,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_VAMPIRE_MASTER_150,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_OUSTERS_MASTER_130,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_VIOLENT_PHANTOM,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 20, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_SUMMON_GORE_GRAND_GROUND,g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0, 50, NULL, false);
//							ExecuteActionInfoFromMainNode(RESULT_CLIENT_TEST_SWORD_OF_THOR,g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0, 100, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_BURNING_SOL_1,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0, 100, NULL, false);
//							g_pPlayer->SetAction(ACTION_SLAYER_BLADE);
//							ExecuteActionInfoFromMainNode(SKILL_PLEASURE_EXPLOSION,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 0, NULL, false);			
							
//							if(g_pEventManager->IsEvent(EVENTID_BG_CLOUD))
//							{
//								g_pEventManager->RemoveEvent(EVENTID_BG_CLOUD);
//							}
//							else
//							{
//								MEvent event;
//								event.eventID = EVENTID_BG_CLOUD;
//								event.eventType = EVENTTYPE_ZONE;
//								event.eventFlag = EVENTFLAG_CLOUD_BACKGROUND;
//								event.parameter3 = 2;

//								g_pEventManager->AddEvent(event);
//							}
							
//							ExecuteActionInfoFromMainNode(SKILL_PLEASURE_EXPLOSION,g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0, 100, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_LAND_MINE_LIGHT,g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0, 100, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_PLEASUER_STATION_LIGHT,g_pPlayer->GetX()+3, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()+3, g_pPlayer->GetY(), 0, 100, NULL, false);

							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1, 1000);
//
//							MActionResult* pResult = new MActionResult;
////
//							if (g_pPlayer->GetCreatureType()!=742)
//							{									
//								g_pPlayer->SetCreatureType( 742 );
//
//								g_pPlayer->SetGroundCreature();
//					
//								g_pPlayer->SetDirection( 2 );
//								g_pPlayer->SetCurrentDirection( 2 );
//								g_pPlayer->SetAction( ACTION_STAND );			
//
//								//--------------------------------------------------

//								//--------------------------------------------------
//								MActionResult* pResult = new MActionResult;
//
//								pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), 742 ) );
//
//								//--------------------------------------------------

//								//--------------------------------------------------								
//								ExecuteActionInfoFromMainNode(

//								
//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0,

//									

//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 
//									

//									
//									NULL, //NULL,
//									

//
//							}
//							else
//							{
//								pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), CREATURETYPE_SLAYER_FEMALE ) );
//								//g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_MALE );
//								//g_pPlayer->SetGroundCreature();
//							}
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_BURNING_SOL_4,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 20, pResult, false);	
//
//							g_pPlayer->SetDelay( 1000 );

							
//							UI_RunPetStorage();SKILL_CLIENT_TEST_SUMMON_GORE_GRAND_GROUND
//							UI_SetPetStorage(g_pStorage);
							
//							for(int i = 0; i< 10; i++)
//								for(int j = 0; j < 6; j++)
//								{
//									const MItem * p_item = g_pInventory->GetItem(i, j);
//				

//									{
//										if(p_item->GetItemClass() == ITEM_CLASS_PET_ITEM)
//										{
//											gpC_base->SendMessage(UI_ITEM_USE, p_item->GetID(), 0, (MItem *)p_item);
//											return;
//										}
//									}
//								}
							

							
//							g_pPCTalkBox->Release();
//							//---------------------------------------------------
//							// normal
//							//---------------------------------------------------
//							g_pPCTalkBox->SetType( PCTalkBox::SELECT_QUEST );
//						
//							int scriptID = 101;
//							
//							//---------------------------------------------------

//							//---------------------------------------------------


//							g_pPCTalkBox->SetNPCID( 297 );
//							g_pPCTalkBox->SetCreatureType( 297 );
//							g_pPCTalkBox->SetScriptID( scriptID );
//							
//							//---------------------------------------------------

//							//---------------------------------------------------
//							int contentSize = 2;//g_pNPCScriptTable->GetContentSize( scriptID );
//							
//							//for (int i=0; i<contentSize; i++)
//							{



//							}
//							
//							g_pUIDialog->PopupPCTalkDlg();
							

//							gC_vs_ui.AddHelpMail(6); 
//							gC_vs_ui.AddHelpMail(0); 
//							gC_vs_ui.AddHelpMail(1); 
//							gC_vs_ui.AddHelpMail(2); 
//							gC_vs_ui.AddHelpMail(3); 
//							gC_vs_ui.AddHelpMail(4); 
//							gC_vs_ui.AddHelpMail(5); 
						
//							gC_vs_ui.RunSMSMessage();
//							extern void Add_GDR_Effect(int nEffect, bool bAppearBossMonster);
//							Add_GDR_Effect(1, false);
//							Add_GDR_Effect(2, false);
//
//							ExecuteActionInfoFromMainNode(TEMP_SKILL_SPIT_STREAM,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 0xffff, NULL, false);			
							//g_pPlayer->SetAction(ACTION_OUSTERS_CHAKRAM_SLOW);
	// create ghost
							{

//								extern void SetDragonTorando(int Type, DWORD ObjectID, int TileX, int TileY);
//								SetDragonTorando(EFFECTSTATUS_DRAGON_TORNADO, 11000, g_pPlayer->GetX()+4, g_pPlayer->GetY());

								UI_PopupMessage( UI_STRING_MESSAGE_CAMPAIGN_HELP_THANKS );
//
//								ExecuteActionInfoFromMainNode(SKILL_CLIENT_BIKE_CRASH, g_pPlayer->GetX(), g_pPlayer->GetY(), 0, g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);	
//								MFakeCreature *pFakeCreature = NewFakeCreature(CREATURETYPE_ROCKET_LUNCHER, g_pPlayer->GetX()+5, g_pPlayer->GetY(), rand()%8);
//
//								if (!g_pZone->AddFakeCreature( pFakeCreature ))
//								{
//									delete pFakeCreature;
//								}
//								else
//								{
//									pFakeCreature->SetZone(g_pZone);
//									pFakeCreature->SetFakeCreatureType(MFakeCreature::FAKE_CREATURE_ROCKET);
//									pFakeCreature->SetMoveType(MCreature::CREATURE_FAKE_FLYING);
//									pFakeCreature->AddEffectStatus( (EFFECTSTATUS)(EFFECTSTATUS_ROCKET_LUNCHER), 0xFFFF );
//									pFakeCreature->SetTraceID(g_pPlayer->GetID());
//									pFakeCreature->SetTime(timeGetTime()+3000); 
//								//	pFakeCreature->SetAction( ACTION_MOVE );
//								}
							}
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_DROP_SIGN,g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0, 10, NULL, false);			
//							
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER_SIGN,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10, NULL, false);			
//							
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_AUGER,g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_AUGER_SIGN,g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0, 10, NULL, false);	
//							
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SW,g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0, 0, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SW_SIGN,g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0, 0, NULL, false);			
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SE,g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SE_SIGN,g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0, 10, NULL, false);	
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NE,g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NE_SIGN,g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0, 10, NULL, false);
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NW,g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NW_SIGN,g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0, 10, NULL, false);
//							
//								for(int i = 0; i< 100 ; i++)
//								{
//									int MapX = rand()% g_pZone->GetWidth();
//									int MapY = rand()% g_pZone->GetHeight();
//									//MFakeCreature *pFakeCreature = NewFakeCreature(716, g_pPlayer->GetX(), g_pPlayer->GetY(), 2);
//									MFakeCreature *pFakeCreature = NewFakeCreature(CREATURETYPE_GHOST, MapX, MapY, rand()%8);
//
//									if (!g_pZone->AddFakeCreature( pFakeCreature ))
//									{
//										delete pFakeCreature;
//									}
//									pFakeCreature->SetZone(g_pZone);
//									pFakeCreature->SetFakeCreatureType(MFakeCreature::FAKE_CREATURE_GHOST);
//									pFakeCreature->SetMoveType(MCreature::CREATURE_FAKE_UNDERGROUND);
//									pFakeCreature->AddEffectStatus( (EFFECTSTATUS)(EFFECTSTATUS_GHOST_1+rand()%2), 0xFFFF );
//								}

								
//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_FIRE_ELEMENTAL))
//								g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL );
//							else
//								g_pPlayer->AddEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL, 0xFFFF );
//							g_pPlayer->SetDelay(2000);
//							TYPE_OBJECTID petID = g_pPlayer->GetPetID();
//							if(petID != OBJECTID_NULL)
//							{
//								MFakeCreature *pCreature = (MFakeCreature *)g_pZone->GetFakeCreature(petID);
//								pCreature->SetAction( ACTION_ATTACK );
//								pCreature->SyncTurretDirection();
//
//
//								ExecuteActionInfoFromMainNode(

//									
//									0, 0, 0,

//									

//									0, 0, 0, 
//									

//									
//									NULL,
//									
//									false);	
//								pCreature->AddEffectStatus(EFFECTSTATUS_COMA, 0xFFFF);
//								pCreature->SetDead();
//							SetFlagTo( true );
//						}
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_HAS_PET, 0);
						}
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						{
							//g_pPlayer->SetWeaponSpeed( MCreature::WEAPON_SPEED_SLOW );

							// 1 ~ 24
							/*
							if (++gtx_op > 24)
							{
								gtx_op = 1;
							}
							*/
							#ifdef __METROTECH_TEST__
								g_iAutoHealPotion = (g_iAutoHealPotion+1)%2;
							#endif
						}
					break;

					case DIK_2 :
						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
						{  
//							extern void SetDragonTorando(int Type, DWORD ObjectID, int TileX, int TileY);
//							SetDragonTorando(EFFECTSTATUS_DRAGON_TORNADO, 11000, g_pPlayer->GetX()+3, g_pPlayer->GetY());
////
//							UI_RunModifyTax();  
//							if(g_pEventManager->IsEvent(EVENTID_ADVANCEMENT_QUEST_ENDING))
//							{
//								g_pEventManager->RemoveEvent(EVENTID_ADVANCEMENT_QUEST_ENDING);
//							}
//							else
//							{
//								MEvent event;
//								event.eventID = EVENTID_ADVANCEMENT_QUEST_ENDING;
//								event.eventType = EVENTTYPE_ZONE;
//								event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
//								event.parameter1 = 0;
//								event.parameter4 = 3;
//								g_pEventManager->AddEvent(event);
//
////								MEvent event;
////								event.eventID = EVENTID_WAR_EFFECT;
////								event.eventType = EVENTTYPE_NULL;
////								event.eventFlag = EVENTFLAG_SHAKE_SCREEN | EVENTFLAG_FADE_SCREEN;
////								event.parameter2 = RGB( -30, -30, 0 );
////								event.parameter3 = 1;
////								event.eventDelay = -1;
////								event.showTime = 2000;
////								event.totalTime = 30000;
////								g_pEventManager->AddEvent( event );
//							}
//
							if(UI_IsRunning_WebBrowser())
								UI_Close_WebBrowser();
							else
							//add by zdj
					 			UI_Run_WebBrowser(/*"http://market.darkeden.com/"*/"");
//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_DIVINE_GUIDANCE))
//								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_DIVINE_GUIDANCE); 
//							else
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_DIVINE_GUIDANCE, 0xffff);
							//g_pPlayer->SetCreatureType(770); 
//							if(g_pPlayer->GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR)
//								g_pPlayer->SetCreatureType(358);
//							else
//								g_pPlayer->SetCreatureType(CREATURETYPE_SLAYER_OPERATOR);
//							
						//	g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_GLACIER);
						//	g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_BLESS_FIRE);

//							ExecuteActionInfoFromMainNode(TEMP_SKILL_NOOSE_OF_WRAITH,g_SelectSector.x, g_SelectSector.y, 0,g_pPlayer->GetDirection(), g_pPlayer->GetID()	,	
//								g_SelectSector.x, g_SelectSector.y, 0, 0xffff, NULL, false);
//							g_pPlayer->SetAction(ACTION_DRAINED);
//							ExecuteActionInfoFromMainNode(RESULT_MAGIC_CAUSE_CRITICAL_WOUNDS,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 0xffff, NULL, false);

//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2, 32);
//							else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3, 32);
//							else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4, 32);
//							else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4))
//							{
//								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4);
//								g_pPlayer->StopBurningSol();
//								ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_4,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX()-4, g_pPlayer->GetY(), 0, 100, NULL, false);	
//								ExecuteActionInfoFromMainNode(RESULT_CLIENT_BURNING_SOL_4,g_pPlayer->GetX()-4, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX()-4, g_pPlayer->GetY(), 0, 100, NULL, false);	
//							}
//							else 
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1, 32);
//
//							ExecuteActionInfoFromMainNode(RESULT_SKILL_SWORD_OF_THOR,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 20, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWORD_OF_THOR,g_pPlayer->GetX()-4, g_pPlayer->GetY()-4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()-4, g_pPlayer->GetY()-4, 0, 50, NULL, false);
//
//							ExecuteActionInfoFromMainNode(RESULT_SKILL_WHITSUNTIDE,g_pPlayer->GetX()+4, g_pPlayer->GetY()+4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+4, g_pPlayer->GetY()+4, 0, 50, NULL, false);
//
//							ExecuteActionInfoFromMainNode(SKILL_SWEEP_VICE_1,g_pPlayer->GetX(), g_pPlayer->GetY()+4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+4, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_SWEEP_VICE_5,g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0, 50, NULL, false);
//							g_pPlayer->SetAction(ACTION_MAGIC);

							
	//						EVENTFLAG_NOT_FADE_SCREEN
						//	g_pPlayer->AddEffectStatus(EFFECTSTATUS_ICICLE_LARGE_AUGER, 100);
						//	ExecuteActionInfoFromMainNode(SKILL_WIDE_ICE_FIELD,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
						//				g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 100, NULL, false);	
							
						//	ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER,g_pPlayer->GetX()-5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
						//		g_pPlayer->GetX()-5, g_pPlayer->GetY()+5, 0, 5, NULL, false);			
							//ExecuteActionInfoFromMainNode(SKILL_ICE_HAIL,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,0,	0,	
							//	g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10000, NULL, false);			
						//	ExecuteActionInfoFromMainNode(SKILL_ICE_HAIL,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,0,	0,	
						//		g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_ICE_WAVE,g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0, 100, NULL, false);			
							//ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SW,g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
							//	g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0, 0, NULL, false);			
						//	UI_RunNotice(7, 100000000);
						//	extern void Add_GDR_Effect(int nEffect, bool bAppearBossMonster);
//						//	Add_GDR_Effect(1,true);	
						//	Add_GDR_Effect(2,true);	
							
//							SetFlagTo( false );
//							TYPE_OBJECTID petID = g_pPlayer->GetPetID();
//							if(petID != OBJECTID_NULL)
//							{
//								MCreature *pCreature = g_pZone->GetFakeCreature(petID);
//								
//								g_pPlayer->SetAction( ACTION_DAMAGED );
//								ExecuteActionInfoFromMainNode(

//									
//									0, 0, 0,

//									

//									0, 0, 0, 
//									

//									
//									NULL,
//									
//									false);	
//							}
//							g_pPlayer->AddEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL_ATTACK, 21 );
//							MEvent event;
//							event.eventID = EVENTID_OUSTERS_FIN;
//							event.eventType = EVENTTYPE_ZONE;
//							event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | 
//								EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | 
//								EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | 
//								EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
//							event.parameter1 = 0;
//							event.parameter4 = EVENTBACKGROUNDID_QUEST_2;
//							g_pEventManager->AddEvent(event);		
						}
						
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						{
							//g_pPlayer->SetWeaponSpeed( MCreature::WEAPON_SPEED_NORMAL);

							// 1 ~ 13
							/*
							if (++gtx_src > 13)
							{
								gtx_src = 1;
							}
							*/
							#ifdef __METROTECH_TEST__
								g_bAutoManaPotion = !g_bAutoManaPotion;
							#endif
						}
					break;

					case DIK_3 :
						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
						{
//							g_pZone->RemoveTileEffect( g_pPlayer->GetX()+3, g_pPlayer->GetY(), EFFECTSTATUS_DRAGON_TORNADO, 11000 );

	#ifdef __TEST_SUB_INVENTORY__   
							MItem* pItem = MItem::NewItem( ITEM_CLASS_SUB_INVENTORY );
							pItem->SetID( rand()%10000 );
							pItem->SetItemType( 0 );
							gC_vs_ui.RunSubInventory( pItem );
	#endif


							//g_pPlayer->SetAction(ACTION_ADVANCEMENT_SLAYER_MAGIC_ATTACK);
							//gC_vs_ui.RunPopupMessage("ComeBackEventNotice.txt", C_VS_UI_POPUP_MESSAGE::POPUP_WELCOME );
//							UI_PopupMessage(UI_STRING_MESSAGE_GET_EVENT_ITEM_NOTICE);
							//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[UI_STRING_MESSAGE_GET_EVENT_ITEM_NOTICE].GetString() );
							//g_pPlayer->SetInvisibleSoon();
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_QUEST_MAGIC_ZONE,16, 16, 0,0,	0,	
//								16, 16, 0, 0xffff, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_QUEST_MAGIC_ZONE_POTAL,16, 16, 0,0,	0,	
//								16, 16, 0, 0xffff, NULL, false);			
//							if(gC_vs_ui.IsRunningPowerjjang())
//							{
//								gC_vs_ui.PowerjjangGambleResult(rand()%7);
//							}
//							else
//							gC_vs_ui.RunPowerjjang();
//							if(!g_pPlayer->HasEffectStatus(EFFECTSTATUS_VIEW_HP))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_VIEW_HP,0xffffffff);
//							else
//							{
////								ExecuteActionInfoFromMainNode(SKILL_TURRET_FIRE,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,2,	g_pPlayer->GetID(),	
////									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 200, NULL, true);	
//
//								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_VIEW_HP);
//							}
//							ExecuteActionInfoFromMainNode(SKILL_VIOLENT_PHANTOM,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,DIRECTION_DOWN,	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 200, NULL, true);	
//							g_pPlayer->SetAction(ACTION_DAMAGED);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX()+2, g_pPlayer->GetY()-2, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+2, g_pPlayer->GetY()-2, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX()+3, g_pPlayer->GetY()-1, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+3, g_pPlayer->GetY()-1, 0, 1000, NULL, false);

















							//g_pPlayer->AddEffectStatus( EFFECTSTATUS_SIEGE_DEFENDERL, 100 );
//							ExecuteActionInfoFromMainNode(SKILL_WIDE_ICE_HAIL,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,0,	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10000, NULL, false);	
//							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_BLINDNESS);
//							g_pZone->RemoveTileEffect(137, 80,EFFECTSTATUS_ELECTRIC_POST);
//							g_pZone->RemoveTileEffect(143, 86,EFFECTSTATUS_ELECTRIC_POST);
//							g_pZone->RemoveTileEffect(136, 93,EFFECTSTATUS_ELECTRIC_POST);
//							g_pZone->RemoveTileEffect(130, 87,EFFECTSTATUS_ELECTRIC_POST);

//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_WATER_ELEMENTAL))
//								g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL );
//							else
//								g_pPlayer->AddEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL, 0xFFFF );

//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\dominator of darkness.mp3");
//							g_pMP3->Play( true );

//							g_bTestMusic = false;
						}
						
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						{
							//g_pPlayer->SetWeaponSpeed( MCreature::WEAPON_SPEED_FAST );

							// 1 ~ 13
							/*
							if (++gtx_dest > 13)
							{
								gtx_dest = 1;
							}
							*/
							#ifdef __METROTECH_TEST__
								g_iAutoReload = (g_iAutoReload+1)%5;
							#endif
						}
					break;

					case DIK_4 :
						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
						{
							int TempCount = ShowCursor(FALSE);
//							g_pUserInformation->IsNonPK = !g_pUserInformation->IsNonPK;
//							if(g_pUserInformation->IsNonPK)
//								g_pSystemMessage->Add("PK Not Available");
//							else
//								g_pSystemMessage->Add("PK Available");
							//g_pPlayer->AddEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL_HEAL, 21 );
							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_BLINDNESS, 300);
//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\oblivion.mp3");
//							g_pMP3->Play( true );

//							g_bTestMusic = false;
						}
#ifdef __METROTECH_TEST__
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
							g_bLight = !g_bLight;
//							g_bHolyWater = !g_bHolyWater;
#endif
						break;
					case DIK_5 :
#ifdef __METROTECH_TEST__
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						UI_RunHorn(g_pZone->GetID());
#endif
//						if (g_pSDLInput->KeyDown(DIK_RCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//						{
//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\rest.mp3");
//							g_pMP3->Play( true );

//							g_bTestMusic = false;
//						}
						
						//						{
						//							g_pClientConfig->QuestItemColorSet += 15;
						//							
						//							if( g_pClientConfig->QuestItemColorSet >= 495 )
						//								g_pClientConfig->QuestItemColorSet = 0;
						//						} else
						//						{							
						//							g_pClientConfig->QuestItemColorSet += 15;
						//							
						//							if( g_pClientConfig->QuestItemColorSet >= 495 )
						//								g_pClientConfig->QuestItemColorSet = 0;
						//						}
						
						break;

					case DIK_6 :
//						{
//							if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//							{
//								g_pMP3->Stop();
//								g_pMP3->Open("data\\music\\ruin.mp3");
//								g_pMP3->Play( true );

//								g_bTestMusic = false;
//								
//								(*g_pUserOption).BlendingShadow = !(*g_pUserOption).BlendingShadow;
//								if (g_pTopView!=NULL)
//								{
//									g_pTopView->ClearShadowManager();
//								}
//							}				
//						}
#ifdef __METROTECH_TEST__
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						UI_RunMinigame(0);
#endif
						break;
						
					case DIK_7 :
//						{
//							if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//							{
//								g_pMP3->Stop();
//								g_pMP3->Open("data\\music\\silence of battlefield.mp3");
//								g_pMP3->Play( true );

//								g_bTestMusic = false;
//							}				
//						}
						break;

					case DIK_8 :
#ifdef __METROTECH_TEST__
						if (g_pSDLInput->KeyDown(DIK_RCONTROL) && g_pSDLInput->KeyDown(DIK_RSHIFT))// &&
							//							g_pSDLInput->KeyDown(DIK_LCONTROL) && g_pSDLInput->KeyDown(DIK_RCONTROL))
						{
							g_iSpeed *= -1;
						}
#endif
//						if (g_pSDLInput->KeyDown(DIK_RCONTROL) || g_pSDLInput->KeyDown(DIK_LCONTROL))// &&
//							//							g_pSDLInput->KeyDown(DIK_LCONTROL) && g_pSDLInput->KeyDown(DIK_RCONTROL))
//						{
////							g_iSpeed *= -1;
//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\underworld.mp3");
//							g_pMP3->Play( true );

//							g_bTestMusic = false;
//						}
						break;
						
					case DIK_9 :
//						if( g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//						{
//							g_bTestMusic = !g_bTestMusic;
//
//							if(g_bTestMusic)
//							{
//								if (g_CurrentMusicNum >= 8 ) g_CurrentMusicNum = 0;
//								g_pMP3->Stop();
//								g_pMP3->Open(g_musicfilename[g_CurrentMusicNum].c_str());
//								g_pMP3->Play( false );
//							} else
//							{
//								g_pMP3->Stop();								
//							}
//						}

#ifdef __METROTECH_TEST__
						if (g_pSDLInput->KeyDown(DIK_RCONTROL) && g_pSDLInput->KeyDown(DIK_RSHIFT))// &&
//							g_pSDLInput->KeyDown(DIK_LCONTROL) && g_pSDLInput->KeyDown(DIK_RCONTROL))
						{
							int mi = 1;
							if(g_iSpeed < 0)
							{
								mi = -1;
								g_iSpeed *= -1;
							}
							g_iSpeed = (g_iSpeed%4)+1;
							g_iSpeed *= mi;
						}
#endif
						break;
					case DIK_0 :
						{
							if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
							{
								if( g_bTestMusic )
								{
									g_pMP3->Stop();									
								}
							}				
						}

					case DIK_Z :
						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
						{
							CSDLGraphics::RestoreAllSurfaces();
// CDirect3D::Restore() removed (SDL2)

							if (g_pTopView!=NULL)
							{
								g_pTopView->RestoreSurface();
							}
						}
					break;
					
					case DIK_N :
						{
							if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
							{
								g_pUserInformation->IsNetmarble = !g_pUserInformation->IsNetmarble;
								if(g_pUserInformation->IsNetmarble)
									g_pSystemMessage->Add("Netmarble Set");
								else
									g_pSystemMessage->Add("Netmarble Unset");
							}
							else
								if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
								{
									g_pPCTalkBox->Release();
									
									//---------------------------------------------------
									// normal
									//---------------------------------------------------
									g_pPCTalkBox->SetType( PCTalkBox::NORMAL );
									
									
									int scriptID = 101;
									
									//---------------------------------------------------
									
									//---------------------------------------------------
									
									g_pPCTalkBox->SetContent( "" );
									g_pPCTalkBox->SetNPCID( 297 );
									g_pPCTalkBox->SetCreatureType( 297 );
									g_pPCTalkBox->SetScriptID( scriptID );
									
									//---------------------------------------------------
									
									//---------------------------------------------------
									int contentSize = 2;//g_pNPCScriptTable->GetContentSize( scriptID );
									
									//for (int i=0; i<contentSize; i++)
									{
										
										g_pPCTalkBox->AddString( "" );
										g_pPCTalkBox->AddString( "" );
									}
									
									g_pUIDialog->PopupPCTalkDlg();
								}
						}
						break;
						
					case DIK_M :
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						{
							if(g_pEventManager->IsEvent(EVENTID_METEOR))
							{
								g_pEventManager->RemoveEvent(EVENTID_METEOR);
							}
							else
							{
								MEvent event;
								event.eventID = EVENTID_METEOR;
								event.eventType = EVENTTYPE_ZONE;
								event.eventFlag = EVENTFLAG_FADE_SCREEN;
								event.parameter2 = 30 << 16;
//								event.parameter4 = EVENTBACKGROUNDID_COSMOS;
								g_pEventManager->AddEvent(event);
							}
						}
						break;

					case DIK_G:
						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
						{
//							MFakeCreature* pFakeCreature = NewFakeCreature( g_pPlayer, g_pPlayer->GetX(), g_pPlayer->GetY() );
//							
//							pFakeCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_FAST_MOVE_ACTION );
//							
//							pFakeCreature->SetFakeCreatureFastMoveAction(g_pPlayer->GetX()+10, g_pPlayer->GetY()+10, 0, 0);
//							
//							//------------------------------------------------------

//							//------------------------------------------------------
//							if (!g_pZone->AddFakeCreature( pFakeCreature ))
//							{
//								delete pFakeCreature;
//							}
						}
						break;

					case DIK_H :
					{
						if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
						{
							if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION))
							{
								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_HALLUCINATION);
							}
							else
							{
								g_pPlayer->AddEffectStatus(EFFECTSTATUS_HALLUCINATION, 16*15);
							}
						}
					}
					break;
				#endif
				
			#endif


			#if defined(OUTPUT_DEBUG) && defined(_DEBUG)	//!defined(CONNECT_SERVER)
				 				

				//------------------------------------------------------------
				//
				
				//
				//------------------------------------------------------------				
				case DIK_SUBTRACT :
				{
					if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
					{
						int volume = g_Music.GetVolume();

						DEBUG_ADD_FORMAT("[MIDI] Volume = %x", volume);

						volume -= 0x1000;

						if (volume < 0)
						{
							volume = 0;
						}

						g_Music.Pause();
						g_Music.SetVolume( volume );
						g_Music.Resume();
						
					}
					
				}
				break;
				
				case DIK_ADD :
				{
					if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
					{
						int volume = g_Music.GetVolume();

						DEBUG_ADD_FORMAT("[MIDI] Volume = %x", volume);

						volume += 0x1000;

						if (volume > 0xFFFF)
						{
							volume = 0xFFFF;
						}

						g_Music.Pause();
						g_Music.SetVolume( volume );
						g_Music.Resume();
						
					}
					
				}
				break;
				
				//------------------------------------------------------------
				//
				// wide chatting
				//
				//------------------------------------------------------------
				 
				//------------------------------------------------------------
				
				//------------------------------------------------------------
				 

				//------------------------------------------------------------
				// Warp
				//------------------------------------------------------------
				case DIK_W :
				{
					if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
					{
						int x, y;

						while (1)
						{
							x = rand()%g_pZone->GetWidth();
							y = rand()%g_pZone->GetHeight();

							if (!g_pZone->GetSector(x,y).IsBlockGround())
							{
								break;
							}
						}

						g_pPlayer->SetStop();						
						g_pPlayer->MovePosition( x, y );
					}
				}
				break;

				//------------------------------------------------------------
				//
				//			Portal
				// 
				//------------------------------------------------------------
				 

				//------------------------------------------------------------
				//
				// casket
				//
				//------------------------------------------------------------
				case DIK_V : 
				{
					if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
					{
						MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();

						for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
						{
							MCreature* pCreature = (*iCreature).second;

							//--------------------------------------------------
							
							//--------------------------------------------------					
							if (pCreature->GetID()!=g_pPlayer->GetID()
								&& pCreature->IsVampire()
								&& !pCreature->IsNPC())
							{							
								if (pCreature->IsInCasket())
								{
									pCreature->RemoveCasket();
								}
								else
								{
									pCreature->AddCasket( rand()%3 );
								}							
							}

							iCreature++;
						}
					}
				}
				break;

				case DIK_C : 
				{
					if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
					{
						if (g_pPlayer->IsInCasket())
						{
							g_pPlayer->RemoveCasket();
						}
						else
						{
							g_pPlayer->AddCasket( rand()%3 );
						}

						/*
						#ifdef OUTPUT_DEBUG
							if (g_pDebugMessage!=NULL)
							{
								char logFile[128];
								strcpy(logFile, g_pDebugMessage->GetFilename());
								
								g_pDebugMessage->Init(MAX_DEBUGMESSAGE, 256, logFile);
							}
						#endif
						*/
						/*
						int x = g_pPlayer->GetX();
						int y = g_pPlayer->GetY();
						int dir = g_pPlayer->GetDirection();

						MFakeCreature* pCreature = new MFakeCreature;

						pCreature->SetID( 0 );
						pCreature->SetSameBody( g_pPlayer );
						pCreature->SetSameWear( g_pPlayer );
						
						pCreature->SetZone( g_pZone );

						pCreature->SetPosition( x, y );
						pCreature->SetServerPosition( x, y );
						pCreature->SetDirection( dir );
						pCreature->SetCurrentDirection( dir );
						pCreature->SetAction( ACTION_ATTACK );
						pCreature->SetWeaponSpeed( g_pPlayer->GetWeaponSpeed() );

						pCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_FADE_ACTION );						

						if (!g_pZone->AddFakeCreature( pCreature ))
						{
							delete pCreature;
						}
						*/
					}
				}
				break;

				//------------------------------------------------------------
				//
				
				//
				//------------------------------------------------------------
				case DIK_D : 
					if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
					{
						UI_RunComputer();
					}					
				break;

				//------------------------------------------------------------
				//
				
				//
				//------------------------------------------------------------
				case DIK_K : 
				{
					if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
					{
						int objectID = MFakeCreature::GetFakeID();
						char name[80];
						sprintf(name, "Fake%d", objectID);
						int x = g_pPlayer->GetX() + rand()%30-15;
						int y = g_pPlayer->GetY() + rand()%30-15;
						int dir = rand()%8;

						if (x >=0 && x < g_pZone->GetWidth()
							&& y >= 0 && y < g_pZone->GetHeight())
						{
							MCreature* pCreature = g_pZone->GetCreature( objectID );

							if (pCreature!=NULL)
							{
								break;
							}


							//------------------------------------------------------------
							
							//------------------------------------------------------------
							if (rand()%2)
							{
								pCreature = new MCreatureWear;

								pCreature->SetZone( g_pZone );
								
								//--------------------------------------------------
								
								//--------------------------------------------------
								if (rand()%3)
								{
									pCreature->SetCreatureType( CREATURETYPE_BAT );
									pCreature->SetFlyingCreature();
								}
								else
								{
									pCreature->SetCreatureType( (rand()%2? CREATURETYPE_VAMPIRE_MALE1 : CREATURETYPE_VAMPIRE_FEMALE1) );
								}

								pCreature->SetMale( pCreature->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE1 );						
								

								pCreature->SetName( name );

								
								pCreature->SetGuildNumber( 2 );

								pCreature->SetGroundCreature();
								pCreature->SetID( objectID );
								//pCreature->SetAction(ACTION_MOVE);
								
								pCreature->SetPosition( x, y );
								pCreature->SetServerPosition( x, y );
								pCreature->SetDirection( dir );
								pCreature->SetCurrentDirection( dir );
								pCreature->SetAction( ACTION_STAND );

								
								pCreature->SetBodyColor1( rand()%MAX_COLORSET );
								pCreature->SetBodyColor2( rand()%MAX_COLORSET );
								
								pCreature->SetStatus( MODIFY_MAX_HP, 80 );
								pCreature->SetStatus( MODIFY_CURRENT_HP, 100 );
								pCreature->SetStatus( MODIFY_ALIGNMENT, 7500 );

								pCreature->SetWeaponSpeed( 70 );							

								if (!g_pZone->AddCreature( pCreature ))
								{
									delete pCreature;
								}
							}
							//------------------------------------------------------------
							
							//------------------------------------------------------------
							else
							{
								MCreatureWear* pCreatureWear = new MCreatureWear;

								pCreatureWear->SetZone( g_pZone );

								pCreatureWear->SetCreatureType( (rand()%2)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );
								pCreatureWear->SetGroundCreature();

								
								
								pCreatureWear->SetID( objectID );
								//pCreatureWear->SetAction(ACTION_MOVE);
								pCreatureWear->SetPosition( x, y );
								pCreatureWear->SetServerPosition( x, y );
								pCreatureWear->SetDirection( dir );
								pCreatureWear->SetCurrentDirection( dir );
								pCreatureWear->SetAction( ACTION_STAND );

								
								pCreatureWear->SetBodyColor2( rand()%MAX_COLORSET );

								pCreatureWear->SetStatus( MODIFY_MAX_HP, 80 );
								pCreatureWear->SetStatus( MODIFY_CURRENT_HP, 100 );

								
								pCreatureWear->SetName( name );

								
								SetAddonToSlayer( pCreatureWear, 250 );

								
								
								if (!g_pZone->AddCreature( pCreatureWear ))
								{
									delete pCreatureWear;
								}
							}
						}
					}
				}
				break;
				

				//-------------------------------------------------------------
				
				//-------------------------------------------------------------
				case DIK_LEFT :
				case DIK_RIGHT :
				{
					MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();

					for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
					{
						MCreature* pCreature = (*iCreature).second;

						//--------------------------------------------------
						
						//--------------------------------------------------					
						if (pCreature->GetID()!=g_pPlayer->GetID()
							&& !pCreature->IsNPC())
						{							
							int dir = pCreature->GetDirection();

							if (key==DIK_LEFT)
							{
								dir = (dir + 7) % 8;
							}
							else
							{
								dir = (dir + 1) % 8;
							}

							pCreature->SetDirection( dir );									
						}

						iCreature++;
					}
				}
				break;

				//------------------------------------
				
				//------------------------------------
				case DIK_R : 
					if (g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU))
					{
						g_pPlayer->SetStatus( MODIFY_CURRENT_HP, 0 );
						g_pPlayer->SetStatus( MODIFY_MAX_HP, 100 );
						g_pPlayer->SetRecoveryHP( 1, 100, 200 );
					}
				break;


				//------------------------------------------------------------
				//
				
				//
				//------------------------------------------------------------
				case DIK_LBRACKET : case DIK_RBRACKET :
					 
				break;						

				/*
				case DIK_GRAVE :
					if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
					{
						(*g_pUIDialog).PopupDirectionSelectDlg(0, 0);
					}					
				break;
				*/

				

//				case DIK_5 :
//				{	
//					if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//					{
//						g_pPlayer->SetMoveDevice( MCreature::MOVE_DEVICE_WALK );
//						g_pPlayer->SetBasicActionInfo( SKILL_ATTACK_MELEE );
//						g_pPlayer->SetStop();
//								
//						if (g_pPlayer->IsSlayer())
//						{
//							if (g_pPlayer->IsMale())
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_SLAYER_FEMALE );
//							}
//							else
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_SLAYER_MALE );
//							}
//						}
//						else
//						{
//							if (g_pPlayer->IsMale())
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_FEMALE1 );
//							}
//							else
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_MALE1 );
//							}
//						}
//						/*
//						if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//							for (int i=0; i<8; i++)
//							{
//								const MItem* pItem = g_Inventory.GetItem(i, 0);
//
//								if (pItem!=NULL)
//								{
//									AddNewInventoryEffect( pItem->GetID(),
//														RESULT_MAGIC_CREATE_HOLY_WATER,

//													);
//								}
//							}
//							*/
//					}
//				}
//				break;

				

//				case DIK_8 :
//				{
//					if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//					{
//						if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SNIPPING_MODE))
//						{
//							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_SNIPPING_MODE);
//						}
//						else
//						{
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_SNIPPING_MODE, 16*10);
//							g_pPlayer->SetInvisible();
//						}
//					}				
//				}
//				break;

//				case DIK_9 :
//				{
//					if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
//					{
//						if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS))
//						{
//							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS);
//						}
//						else
//						{
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS, 16*10);
//						}
////						if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SHUT_DOWN))
////						{
////							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_SHUT_DOWN);
////						}
////						else
////						{
////							g_pPlayer->AddEffectStatus(EFFECTSTATUS_SHUT_DOWN, 16*10);
////						}
//					}				
//				}
//				break;

				

				 

				case DIK_A :
					if (g_pSDLInput->KeyDown(DIK_LCONTROL) || g_pSDLInput->KeyDown(DIK_RCONTROL))
					{
						//------------------------------------------------------
						
						//------------------------------------------------------
						MCreature* pCreature = g_pZone->GetCreature( 1000 );

						if (pCreature!=NULL)
						{
							//------------------------------------------------------
							
							//------------------------------------------------------
							MActionResult* pResult = new MActionResult;
	
							pResult->Add( new MActionResultNodeActionInfo( 
														MAGIC_LIGHT,
														pCreature->GetID(),
														g_pPlayer->GetID(), 
														g_pPlayer->GetX(),
														g_pPlayer->GetY()
														) 
										);
								
							//------------------------------------------------------
							
							//------------------------------------------------------
							//Duration_t	m_Duration;
							pCreature->PacketSpecialActionToSector(
												MAGIC_LIGHT, 
												g_pPlayer->GetX(), g_pPlayer->GetY(),
												pResult						
							);

							
							pCreature->SetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY());
					
						}
					}
				break;


				 
			#endif			
		}
	}
	else // if (event==InputManager::KEYUP)
	{
	}
}



//-----------------------------------------------------------------------------
// ProcessInput RButton Down
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
ProcessInputRButtonDown(MObject* pObject, bool bForceAttack = false)
{
	g_bRButtonDown = TRUE;
	
	
	if (g_pTopView!=NULL)
	{
		g_pTopView->SetSelectedSectorNULL();
	}
	// add by Sonic
		//START_START
		//CRYPT_START
		// Local test client: ignore the legacy speed-check error counter.
		// The verification packets are disabled in the game loop below.
		//VM_END
		//CRYPT_END
	// end 
//	UI_RunRaceWarNotice(0xFFFFFFFF);
	
	//bool bFreePKZone = g_pZoneTable->Get( g_pZone->GetID() )->FreePK;
	if(NULL != pObject)
		bForceAttack |= (g_pObjectSelector->IsWarEnemy( (MCreature*) pObject ) == TRUE);
	//bForceAttack |= ((g_pZone->GetID() == 1303)||(g_pZone->GetID() == 1020));
//	bForceAttack |= g_pZone->IsFreePKZone();

	if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_DRAGON_EYES))
		return;
	if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		if(!g_pPlayer->IsStop()) return;
		if(g_pPlayer->GetSpecialActionInfo() == MAGIC_UN_TRANSFORM)
		{
		//	if(!g_pPlayer->IsStop()) return;
			CGSkillToSelf _CGSkillToSelf;
			_CGSkillToSelf.setSkillType( MAGIC_UN_TRANSFORM );
			g_pSocket->sendPacket( &_CGSkillToSelf );
		//	g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF );
			return ;
		}
		else if(g_pPlayer->GetSpecialActionInfo() == SKILL_TURRET_FIRE)
			PlaySound( SOUND_SLAYER_TURRET_FIRE_START );
	}
	if( HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(g_pPlayer) )
//		&& //g_pPlayer->HasEffectStatus( EFFECTSTATUS_SUMMON_SYLPH ) && 
//		!g_bZoneSafe && !g_pPlayer->IsInSafeSector() 
		) 
	{ 
		if(!g_pPlayer->IsStop()) return;
		
		if(!g_bZoneSafe && !g_pPlayer->IsInSafeSector())
		{
			CGSkillToSelf _CGSkillToSelf;
			_CGSkillToSelf.setSkillType( MAGIC_UN_TRANSFORM );
	//		_CGSkillToSelf.setCEffectID( m_pEffectTarget->GetEffectID() );
			g_pSocket->sendPacket( &_CGSkillToSelf );
			g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF );
			(*g_pSkillInfoTable)[SKILL_SUMMON_SYLPH].SetAvailableTime( 4000 );
	//		gC_vs_ui.UnselectSkill();
		}
		else 
			return;
	} else
	//---------------------------------------------------------------
	// 
	
	//
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	if (g_pPlayer->GetMoveDevice()==MCreature::MOVE_DEVICE_RIDE
		&& g_pPlayer->IsStop()
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_EXPLOSION_WATER)
		)
	{
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("MotorOff");
#endif
	
		if( ! IsExistCorpseFromPlayer(dynamic_cast<MCreature*>(g_pPlayer), 670) )
		{		
			CGGetOffMotorCycle _CGGetOffMotorCycle;
			
			_CGGetOffMotorCycle.setObjectID( 0 );
			
			g_pSocket->sendPacket(&_CGGetOffMotorCycle);
			
#ifdef __DEBUG_OUTPUT__
			DEBUG_ADD_FORMAT("[Send] %s", _CGGetOffMotorCycle.toString().c_str() );
#endif
			
			g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_MOTORCYCLE_GETOFF );
		} else
		{
			g_pSystemMessage->Add((*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_ACTION_MOTORCYCLE_FLAG].GetString() );
		}
	}
	//---------------------------------------------------------------
	//
	
	//
	//---------------------------------------------------------------
	else if (g_pPlayer->IsUndergroundCreature())
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("Unburrow");
		#endif

		CGUnburrow _CGUnburrow;

		_CGUnburrow.setX( g_pPlayer->GetX() );
		_CGUnburrow.setY( g_pPlayer->GetY() );
		_CGUnburrow.setDir( g_pPlayer->GetDirection() );

		g_pSocket->sendPacket(&_CGUnburrow);

		#ifdef __DEBUG_OUTPUT__
			DEBUG_ADD_FORMAT("[Send] %s", _CGUnburrow.toString().c_str() );
		#endif

		g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
	}
	//---------------------------------------------------------------
	//
	
	//
	//---------------------------------------------------------------
	else
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("TileSkill");
		#endif

		
		//g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
		//g_pTopView->SetSelectedSector(g_SelectSector.x, g_SelectSector.y);

		//
		//MObject*	pObject = g_pTopView->GetSelectedObject(g_x, g_y);
	
		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pObject == NULL)
		{		
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();

			//--------------------------------------------------
			
			//--------------------------------------------------
			if (g_pPlayer->SelfSpecialAction())
			{
				g_pPlayer->SetRepeatAction();
			}
			//--------------------------------------------------
			
			//--------------------------------------------------				
			else if (!g_pPlayer->IsSpecialActionInfoTargetSelf())
			{
				g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);

				//--------------------------------------------------				
				
				//--------------------------------------------------				
				if (g_pPlayer->TraceSectorToSpecialAction( g_SelectSector.x, g_SelectSector.y ))
				{
					
					g_pTopView->SetSelectedSector( g_SelectSector );
				
					g_pPlayer->SetRepeatAction();

					g_bPreviousMove = false;
				}
			}

			//----------------------------------
			
			//----------------------------------						
			g_pTopView->SetSelectedSectorNULL();
			
		}		
		//--------------------------------------------------
		
		//--------------------------------------------------
		else 
		{				
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("Obj");
			#endif

			switch (pObject->GetObjectType())
			{	
				//--------------------------------------------------
				// Creature
				//--------------------------------------------------
				case MObject::TYPE_CREATURE :		
				{
					
					MCreature* pCreature = ((MCreature*)pObject);
					
					//--------------------------------------------------
					
					//--------------------------------------------------
					if (g_pPlayer->TraceCreatureToSpecialAction( pCreature->GetID(), bForceAttack ))
					{
						g_pPlayer->SetRepeatAction();								

						g_bPreviousMove = false;
					}
					else if (!g_pPlayer->IsSpecialActionInfoTargetOther())
					{
						//--------------------------------------------------
						
						//--------------------------------------------------
						
						if (g_pPlayer->SelfSpecialAction())
						{
							g_pPlayer->SetRepeatAction();
						}
						else
						{	
							//--------------------------------------------------
							
							//--------------------------------------------------
							
							
							if (!g_pPlayer->IsSpecialActionInfoTargetSelf()
								&& g_pPlayer->TraceSectorToSpecialAction( pCreature->GetX(), pCreature->GetY()))
							{
								g_pPlayer->SetRepeatAction();

								g_bPreviousMove = false;
							}
						}									
					}
					
					//----------------------------------
					
					//----------------------------------
					g_pTopView->SetSelectedSectorNULL();
				}
				break;

				//--------------------------------------------------
				// Item
				//--------------------------------------------------
				
				case MObject::TYPE_ITEM :		
				{
					//--------------------------------------------------
					
					//--------------------------------------------------
					if (g_pPlayer->SelfSpecialAction())
					{
						g_pPlayer->SetRepeatAction();
					}
					else
					{	
						//--------------------------------------------------
						
						//--------------------------------------------------
						
						if (!g_pPlayer->IsSpecialActionInfoTargetSelf())
						{
							MItem* pItem = ((MItem*)pObject);

							if (pItem!=NULL)
							{
								
								if(g_pPlayer->GetSpecialActionInfo() == SKILL_WILD_WOLF)
								{
									g_pPlayer->UseWildWolf_Corpse(pItem);
								}
								
								
								else if (g_pPlayer->TraceSectorToSpecialAction( pItem->GetX(), pItem->GetY()))
								{
									g_pPlayer->SetRepeatAction();
									
									g_bPreviousMove = false;
								}
							}
						}
					}
				}
				break;

			}
		}		
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("RbOK");
	#endif
}

//-----------------------------------------------------------------------------
// SDLTextInputEvent
//-----------------------------------------------------------------------------

void CGameUpdate::SDLTextInputEvent(const char* text, int* window_coords)
{
	// Convert SDL text input (UTF-8) to WM_CHAR messages
	// This allows the IME system to handle text input properly
	if (text == NULL || text[0] == '\0') {
		return;
	}

	// Process each character in the UTF-8 string
	int i = 0;
	while (text[i] != '\0') {
		// Get the Unicode code point from UTF-8
		UINT char_code = 0;
		BYTE c = (BYTE)text[i];

		if (c < 0x80) {
			// ASCII character (1 byte)
			char_code = c;
			i += 1;
		} else if ((c & 0xE0) == 0xC0) {
			// 2-byte UTF-8
			if (text[i+1] != '\0') {
				char_code = ((c & 0x1F) << 6) | (text[i+1] & 0x3F);
				i += 2;
			} else {
				break;
			}
		} else if ((c & 0xF0) == 0xE0) {
			// 3-byte UTF-8
			if (text[i+1] != '\0' && text[i+2] != '\0') {
				char_code = ((c & 0x0F) << 12) | ((text[i+1] & 0x3F) << 6) | (text[i+2] & 0x3F);
				i += 3;
			} else {
				break;
			}
		} else if ((c & 0xF8) == 0xF0) {
			// 4-byte UTF-8 (beyond BMP, convert to replacement char)
			char_code = '?';  // Replacement character
			i += 4;
		} else {
			// Invalid UTF-8, skip
			i += 1;
			continue;
		}

		// Send WM_CHAR message to the IME system
#if !(defined(PLATFORM_WINDOWS) && (defined(USE_SDL_BACKEND) || defined(SPRITELIB_BACKEND_SDL)))
		gC_vs_ui.KeyboardControl(WM_CHAR, char_code, 0);
#endif
	}
}

//---------------------------------------------------------------------------
// Process Game Input
//---------------------------------------------------------------------------
void
CGameUpdate::ProcessInput()
{
	if(g_pEventManager->GetEventByFlag(EVENTFLAG_DENY_INPUT))
		return;

	//-----------------------------------------------
	// Quit
	//-----------------------------------------------
	///*
	#ifdef OUTPUT_DEBUG
		if (g_pSDLInput->KeyDown(DIK_LMENU) && g_pSDLInput->KeyDown(DIK_X))
		{	
			//TerminateThread(g_hFileThread, 0);
			SetMode( MODE_QUIT );
			
			return;
		}	
	#endif
	//*/	


	//-----------------------------------------------
	
	//-----------------------------------------------
	#ifdef OUTPUT_DEBUG
		if (g_pSDLInput->KeyDown(DIK_M) 
			&& g_pSDLInput->KeyDown(DIK_LMENU))
		{			
			PacketTest();
		}
	#endif
	
	/*
	// [ TEST CODE ] DEBUG MESSAGE
	if (g_pSDLInput->m_lb_down || g_pSDLInput->m_lb_up)
			DEBUG_ADD("----------");

	if (g_pSDLInput->m_lb_down && g_pSDLInput->m_lb_up)
	{
			DEBUG_ADD("LButtonDown & Up");
	}
	else if (g_pSDLInput->m_lb_down)
	{
			DEBUG_ADD("LButtonDown");
	}
	else if (g_pSDLInput->m_lb_up)
	{
			DEBUG_ADD("LButtonUp");
	}
	*/

	
	//g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
	

	//---------------------------------------------------	
	// LButton Up
	//---------------------------------------------------	
	if (g_bLButtonDown
		
		&& (g_pSDLInput->m_lb_up	|| g_pSDLInput->m_rb_down)
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("LBUP");
		#endif

		g_bLButtonDown = FALSE;

		
		g_pPlayer->UnSetLockMode();
	
		if (g_pPlayer->IsRepeatAction())
		{
			
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					//DEBUG_ADD("plu");//[ProcessInput] Lbutton UP");
			#endif
		}
		else 
		{		
			//---------------------------------------------------	
			
			//---------------------------------------------------	
			
			POINT point;
			g_pPlayer->GetNextDestination( point );		
			if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
			{
				
				g_pPlayer->GetDestination( point );
				if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
				{
					//-_-;;
				}
				else
				{
					g_pTopView->SetSelectedSector( point );
				}
			}
			else
			{
				g_pTopView->SetSelectedSector( point );
			}
		}
	}

	//---------------------------------------------------	
	// RButton Up
	//---------------------------------------------------	
	if (g_bRButtonDown
		
		&& (g_pSDLInput->m_rb_up || g_pSDLInput->m_lb_down)
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("RBUP");
		#endif

		g_bRButtonDown = FALSE;

		g_pPlayer->UnSetLockMode();

		// 2004, 8, 30, sobeit add start
		g_pPlayer->CheckRbuttonUpSkill();
		// 2004, 8, 30, sobeit add end
		{
			if (g_pPlayer->IsRepeatAction())
			{
				
				g_pPlayer->UnSetRepeatAction();
				//g_pPlayer->TraceNextNULL();

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
						//DEBUG_ADD("pru");//[ProcessInput] Rbutton UP");
				#endif
			}
		}
	}

	if(g_bCButtonDown && g_pSDLInput->m_cb_up)
	{
		g_bCButtonDown = FALSE;
	}
	
	//---------------------------------------------------	
	// UI Input
	//---------------------------------------------------	
	POINT cursorPoint;
	GetCursorPos(&cursorPoint);

	ScreenToClient(g_hWnd, &cursorPoint);//by viva

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("UIMM");
	#endif
	gC_vs_ui.MouseControl(M_MOVING, cursorPoint.x, cursorPoint.y);
	//gC_vs_ui.MouseControl(M_MOVING, g_x, g_y);
	//if (gC_vs_ui.MouseControl(M_MOVING, g_x, g_y))
	//	return;

	
	//#ifdef __EXPO_CLIENT__	
	//	if ((*g_pUserOption).DrawInterface)
	//	{
	//#endif

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("IRP");
	#endif

	
	bool bRunningParty = UI_IsRunningParty();

	if (g_pSDLInput->m_lb_down)
	{
		#ifdef OUTPUT_DEBUG
			if (g_pSDLInput->KeyDown(DIK_LSHIFT) && g_bSlideScreenShot)
			{
				g_bSlideRectSelect = true;
			}
		#endif
		
		if (gC_vs_ui.MouseControl(M_LEFTBUTTON_DOWN, g_x, g_y) )
		{
			TraceInteraction("LBDown consumed by UI screen=%d,%d uiInput=%d lock=%d",
				g_x, g_y, g_bUIInput, g_pUIDialog ? g_pUIDialog->IsLockInput() : 0);
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI LButton Down return");
			#endif

			return;				
		}
	}

	if (g_pSDLInput->m_lb_up)
	{
		#ifdef OUTPUT_DEBUG
			g_bSlideRectSelect = false;			
		#endif

		if (gC_vs_ui.MouseControl(M_LEFTBUTTON_UP, g_x, g_y))
		{
			TraceInteraction("LBUp consumed by UI screen=%d,%d uiInput=%d lock=%d",
				g_x, g_y, g_bUIInput, g_pUIDialog ? g_pUIDialog->IsLockInput() : 0);
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI LButton Up return");
			#endif

			return;
		}
	}
	if (g_pSDLInput->m_rb_down)
	{
		if (gC_vs_ui.MouseControl(M_RIGHTBUTTON_DOWN, g_x, g_y)
			&& !bRunningParty)
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI RButton Down return");
			#endif

			return;
		}
	}

	if (g_pSDLInput->m_rb_up)
	{
		if (gC_vs_ui.MouseControl(M_RIGHTBUTTON_UP, g_x, g_y)
			&& !bRunningParty)
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI RButton Up return");
			#endif

			return;
		}
	}	

	if( g_pSDLInput->m_cb_up)
	{
		if( gC_vs_ui.MouseControl(M_CENTERBUTTON_UP, g_x,g_y)
			&& !bRunningParty)
		{
			return;
		}
	}

	if( g_pSDLInput->m_cb_down)
	{
		if( gC_vs_ui.MouseControl( M_CENTERBUTTON_DOWN, g_x,g_y)
			&& !bRunningParty)
		{
			return;
		}
	}
	
	//#ifdef OUTPUT_DEBUG
	
//		if (gC_vs_ui.ChatMouseControlExtra( M_MOVING, g_x, g_y ))
//			return;
	//#endif

	// 2000.7.8. KJTINC
	/*
	static bool bl_ctrl;
	if (g_pSDLInput->KeyDown(DIK_LCONTROL) ||
		g_pSDLInput->KeyDown(DIK_RCONTROL))
	{
		bl_ctrl = true;
	}
	//if (g_pSDLInput->KeyUp(DIK_LCONTROL) &&
	//	 g_pSDLInput->KeyUp(DIK_RCONTROL))
	//{
	//	bl_ctrl = false;
	//}
	if (g_pSDLInput->KeyDown(DIK_SPACE) && bl_ctrl == true)
	{
		gC_vs_ui.PressCtrlSpace();
	}*/


	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("CINL");
	#endif

	//---------------------------------------------------	
	
	//---------------------------------------------------	
	if ((g_pSDLInput->KeyDown(DIK_LMENU) || g_pSDLInput->KeyDown(DIK_RMENU) || g_pSDLInput->KeyDown(DIK_RCONTROL)
#ifdef __FRIEND_SYSTEM_VIVA__	// add by viva  g_pSDLInput->KeyDown(DIK_LCONTROL
		|| g_pSDLInput->KeyDown(DIK_LCONTROL)
#endif
		)  
		&& !g_pSDLInput->KeyDown(DIK_LSHIFT)
		&& g_pTradeManager==NULL && g_pPlayer->GetWaitVerify()!=MPlayer::WAIT_VERIFY_TRADE)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("DINL");
		#endif

		g_pTopView->SetDrawItemNameList();

		if (!g_bWatchMode 
			
			&& !g_pPlayer->IsRepeatAction()
			
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_BAT
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_WOLF
			)
		{
			g_pTopView->UnSetRequestMode();
			g_pTopView->SetSelectedSectorNULL();					

			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
			{					
				if (g_pSDLInput->KeyDown(DIK_LMENU))
				{
					g_pTopView->SetRequestMode( MTopView::REQUEST_TRADE );
				}
				else if (g_pSDLInput->KeyDown(DIK_RMENU))
				{
					if (!gC_vs_ui.IsRunningRequestParty()
						&& g_pSystemAvailableManager->IsAvailablePartySystem() )
					{
						g_pTopView->SetRequestMode( MTopView::REQUEST_PARTY );
					}
				}
				else if (g_pSDLInput->KeyDown(DIK_RCONTROL))
				{
					g_pTopView->SetRequestMode( MTopView::REQUEST_INFO );
				}
#ifdef __FRIEND_SYSTEM_VIVA__
				///add by viva
				else if(g_pSDLInput->KeyDown(DIK_LCONTROL))
				{
					gpC_mouse_pointer->SetCursorAddFriend();
				//	g_pPlayer->SetRepeatAction();
				}
#endif
			}
		}
	}
	else
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("NDINL");
		#endif

		g_pTopView->UnSetDrawItemNameList();
		g_pTopView->UnSetRequestMode();		
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("PMC");
	#endif

		
	//---------------------------------------------------	
	
	
	//---------------------------------------------------	
//	if (g_pParty!=NULL 
//		&& g_pParty->GetSize()==0
//		&& UI_IsRunningParty()
//		&& !gC_vs_ui.IsRunningRequestParty()
//		&& g_pTempInformation->Mode==TempInformation::MODE_NULL
//		&& !g_bWatchMode 

//		&& !g_pPlayer->IsRepeatAction()

//		&& g_pPlayer->GetCreatureType()!=CREATURETYPE_BAT
//		&& g_pPlayer->GetCreatureType()!=CREATURETYPE_WOLF)
//	{
//		g_pTopView->SetSelectedSectorNULL();
//		g_pTopView->SetRequestMode( MTopView::REQUEST_PARTY );
//	}

	//---------------------------------------------------	
	//
	
	//
	//---------------------------------------------------	
	
	//
	
	//
	
	//
	
	//
	//---------------------------------------------------	
	if (g_pPlayer->IsWaitVerify() 
		|| gC_vs_ui.IsInstallMineProgress()	
		
		
		|| g_pPlayer->IsFastMove()	
		
//		|| g_pPlayer->CurPernalShop() == 1  || gC_vs_ui.inventory_mode == 2
//		|| gC_vs_ui.IsRunningPersnalShop()
		
		|| UI_IsRunning_WebBrowser()
		)	
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD_FORMAT("[ProcessInput] Is WaitVerify Return. %d %d", g_pPlayer->IsWaitVerify(), g_pPlayer->IsFastMove());
		#endif

		return;
	}

	
	if (g_pPlayer->IsRepeatAction())
	{
		g_pTopView->SetSelectedSectorNULL();

		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("[ProcessInput] Is RepeatAction return");
		#endif

		return;
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("CUILock");
	#endif

	//---------------------------------------------------	
	
	
	//---------------------------------------------------	
	if ((g_pUIDialog->IsLockInput() || g_bUIInput)
		
		
		&& !bRunningParty

		
		|| gC_vs_ui.IsRunningElevator()
		|| gC_vs_ui.IsRunningXmasCardWindow()
		|| UI_IsRunningSelectWayPoint())
	{
		g_pTopView->SetSelectedNULL();
			
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD_FORMAT("[ProcessInput] UI return. %d %d %d", g_pUIDialog->IsLockInput(), g_bUIInput, gC_vs_ui.IsRunningElevator());
		#endif

		return;
	}

	
	
	//---------------------------------------------------	
	//
	
	//
	//---------------------------------------------------		
	if (g_pPlayer->IsDead())
	{
		g_pTopView->SetSelectedNULL();
		g_pTopView->SetSelectedSectorNULL();

		//---------------------------------------------------	
		
		
		//---------------------------------------------------	
		//if (g_pPlayer->IsNotDeadDelay() && g_pSDLInput->KeyDown( DIK_SPACE ))
		//{
		//	g_pPlayer->SetAlive();
		//	g_pTopView->SetFadeEnd();
		//}
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("[ProcessInput] Player Dead return.");
		#endif

		return;
	}
 


	
	//POINT temp = g_pTopView->ScreenToPixel(g_x, g_y);
	//temp = MTopView::PixelToMap( temp.x, temp.y );
	//g_pPlayer->SetDirectionToPosition(temp.x, temp.y);

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("FA");
	#endif

	//---------------------------------------------------	
	//
	
	//
	//---------------------------------------------------	
	
	
	//---------------------------------------------------	
	

	
	bool bForceAttack = (g_pSDLInput->KeyDown(DIK_LSHIFT) 
						|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION));
						
						

	if (bForceAttack
		//|| !g_bZonePlayerInLarge
		|| g_bZoneSafe
		|| g_pPlayer->IsInSafeSector()
		|| g_pPlayer->IsSlayer() && g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL && (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetUser() == FLAG_ACTIONINFO_USER_SLAYER
		|| g_pPlayer->IsVampire() && g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL && (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetUser() == FLAG_ACTIONINFO_USER_VAMPIRE
		|| g_pPlayer->IsOusters() && g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL && (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetUser() == FLAG_ACTIONINFO_USER_OUSTERS
		
		|| g_pPlayer->GetSpecialActionInfo() == SKILL_INTIMATE_GRAIL
		// 2004, 11, 26, sobeit add end
		)
	{
		g_pObjectSelector->SelectAll();				
	}
	//---------------------------------------------------	
	
	//---------------------------------------------------	
	else if (g_pSDLInput->KeyDown(DIK_LCONTROL)
		|| g_pTopView->IsRequestMode())
	{
		g_pObjectSelector->SelectFriend();		
	}
	//---------------------------------------------------	
	
	//---------------------------------------------------	
	else
	{
		
		
		
		
		
		
		g_pObjectSelector->SelectEnemy();		
	}

	//---------------------------------------------------	
	
	//
	
	
	//
	
	//
	
	
	//---------------------------------------------------	
	
	//---------------------------------------------------	
	if (g_pPlayer->IsSlayer() || g_pPlayer->IsOusters() )
	{
		g_pObjectSelector->SelectByRace();
	}
	//---------------------------------------------------	
	
	//---------------------------------------------------	
	else
	{
		g_pObjectSelector->SelectByGuild();
	}


	//MObject*	pObject = g_pTopView->GetSelectedObjectSprite(g_x, g_y);
	MObject*	pObject = NULL;
	
	
	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("PMF");
	#endif

	int partyMember = (bRunningParty? gC_vs_ui.GetPartyManagerFocused() : -1) - 1;

	//---------------------------------------------------	
	
	//---------------------------------------------------	
	if (partyMember >= 0
		&& partyMember < g_pParty->GetSize())
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("SelP");
		#endif

		PARTY_INFO* pInfo = g_pParty->GetMemberInfo( partyMember );

		if (pInfo!=NULL)
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("SPM");
			#endif

			
			
			
			pObject = g_pZone->GetCreature( g_pZone->GetCreatureID( pInfo->Name.GetString(), 1 ) );

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("SPMk");
			#endif
		}
	}
	//---------------------------------------------------	
	
	//---------------------------------------------------	
	else
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("selZ");
		#endif

		//---------------------------------------------------
		
		
		//---------------------------------------------------
		if (partyMember==-2		
			&& g_bUIInput)		
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selZk1");
			#endif

			return;
		}

		//---------------------------------------------------	
		
		//---------------------------------------------------	
		if (g_pTopView->IsDrawItemNameList())
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("drawItemNL");
			#endif

			MItem* pItem = g_pTopView->SelectItemName(g_x, g_y);

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selIN");
			#endif

			if (pItem==NULL)
			{
				pObject = NULL;
			}
			else
			{
				pObject = (MObject*)pItem;
			}

			//---------------------------------------------------	
			
			//---------------------------------------------------	
			if (pObject==NULL)
			{
				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("selObj");
				#endif

				pObject = g_pTopView->GetSelectedObject(g_x, g_y);

				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("selObjK");
				#endif
			}
		}
		//---------------------------------------------------	
		
		//---------------------------------------------------	
		else
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selObj2");
			#endif

			pObject = g_pTopView->GetSelectedObject(g_x, g_y);

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selObj2K");
			#endif
		}

		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("selZk");
		#endif

		if (g_pSDLInput->m_lb_down || g_pSDLInput->m_rb_down)
		{
			int objectType = pObject ? (int)pObject->GetObjectType() : -1;
			int objectID = pObject ? (int)pObject->GetID() : -1;
			TraceInteraction("World click screen=%d,%d sector=%d,%d player=%d,%d objectType=%d objectID=%d lb=%d rb=%d uiInput=%d",
				g_x, g_y, g_MouseSector.x, g_MouseSector.y,
				g_pPlayer ? g_pPlayer->GetX() : -1, g_pPlayer ? g_pPlayer->GetY() : -1,
				objectType, objectID,
				g_pSDLInput->m_lb_down, g_pSDLInput->m_rb_down, g_bUIInput);
		}
	}

//	if (g_pPlayer->IsRepeatAction())
//	{
//		g_pTopView->SetSelectedSectorNULL();
//		
//#ifdef OUTPUT_DEBUG_PROCESS_INPUT
//		DEBUG_ADD("[ProcessInput] Is RepeatAction return");
//#endif
//		
//		return;
//	}
	
	//---------------------------------------------------		
	// Watch mode
	//---------------------------------------------------		
	if (g_bWatchMode)
	{
		int cx = 0, cy = 0;

		if (g_x < 10)		cx = -1;
		else if (g_x > 790)	cx = 1;
		if (g_y < 10)		cy = -1;
		else if (g_y > 590)	cy = 1;

		int x = g_pPlayer->GetX();
		int y = g_pPlayer->GetY();

		x += cx;
		y += cy;

		if (x < 0) x = 0;
		else if (x >= g_pZone->GetWidth()) x = g_pZone->GetWidth()-1;
		if (y < 0) y = 0;
		else if (y >= g_pZone->GetHeight()) y = g_pZone->GetHeight()-1;

		g_pPlayer->SetPosition( x, y );

		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("[ProcessInput] Watch Mode");
		#endif

		return;
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("ckL");
	#endif

	//---------------------------------------------------	
	
	//---------------------------------------------------	
	if (g_pSDLInput->KeyDown(DIK_CAPITAL) 
		&& !g_pPlayer->IsInCasket()
		&& !g_pPlayer->IsUndergroundCreature())
	{
		g_pPlayer->SetLockMode();

		
		gpC_mouse_pointer->SetCursorAttack();

		if (g_pSDLInput->m_lb_down)
		{		
			g_bLButtonDown = TRUE;			
		}
		else if (g_pSDLInput->m_rb_down)
		{
			g_bRButtonDown = TRUE;
		}
		else if (g_pSDLInput->m_cb_down)
		{
			g_bCButtonDown = TRUE;
		}

		
		g_pTopView->SetSelectedSectorNULL();
	}
	else
	{
		
		
		if (!g_bLButtonDown && !g_bRButtonDown
#ifdef __METROTECH_TEST__
			&& !g_bCButtonDown
#endif
			)
		{
			g_pPlayer->UnSetLockMode();
		}
	}

	//---------------------------------------------------	
	
	
	//---------------------------------------------------	
	if (!g_pPlayer->IsLockMode())
	{	
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("notLock");
#endif
		
		//---------------------------------------------------	
		
		//---------------------------------------------------	
		if (pObject==NULL)
		{
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("noObj");
#endif
			
			
			g_pTopView->SetSelectedNULL();
			
			//---------------------------------------------------	
			
			//---------------------------------------------------	
			//if (!g_bMouseInPortal)
			{
				if (g_pPlayer->OnAttacking() || g_pSDLInput->KeyDown(DIK_LSHIFT))
				{
					gpC_mouse_pointer->SetCursorAttack();
				}
				else
				{
					if (g_MouseSector.x >= 0 
						&& g_MouseSector.y >= 0
						&& g_MouseSector.x < g_pZone->GetWidth() 
						&& g_MouseSector.y < g_pZone->GetHeight())
					{
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD_FORMAT("getSec(%d, %d)", g_MouseSector.x, g_MouseSector.y);
#endif
						
						const MSector& sector = g_pZone->GetSector(g_MouseSector.x, g_MouseSector.y);
						
						//---------------------------------------------------
						
						//---------------------------------------------------
						if (g_pPlayer->IsGroundCreature() && sector.IsBlockGround()
							|| g_pPlayer->IsUndergroundCreature() && sector.IsBlockUnderground()
							|| g_pPlayer->IsFlyingCreature() && sector.IsBlockFlying())
							//g_pZone->CanMove(g_pPlayer->GetMoveType(), g_MouseSector.x, g_MouseSector.y))
						{	
							
							if (!g_bMouseInPortal
								
								&& !g_bUIInput
								
								&& g_pTopView->GetSelectedCreature()==OBJECTID_NULL)
							{
								
								gpC_mouse_pointer->SetCursorNotMove();					
							}
						}
						//---------------------------------------------------
						
						//---------------------------------------------------
						else
						{
							if (!g_bMouseInPortal)
							{
								gpC_mouse_pointer->SetCursorDefault();
							}
						}
					}
					else
					{
						if (!g_bMouseInPortal)
						{
							gpC_mouse_pointer->SetCursorDefault();
						}
					}
				}
			}
			
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("noObjk");
#endif
		}
		//---------------------------------------------------	
		//
		
		//
		//---------------------------------------------------	
		else
		{
			BYTE objectType = pObject->GetObjectType();
			
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD_FORMAT("obj(%d)", objectType);
#endif
			
			//------------------------------------------------
			
			//------------------------------------------------
			if (objectType==MObject::TYPE_CREATURE)
			{
				MCreature* pCreature = (MCreature*)pObject;

				g_pTopView->SetSelectedCreature( pCreature->GetID() );
					
				if (!g_bMouseInPortal 
						
					
					
					//&& (g_pPlayer->CanAttackTribe( pCreature ) || bForceAttack)
					&& g_pObjectSelector->CanAttack( pCreature ) )
					
						
				{
					if (pCreature->IsNPC())
					{
						gpC_mouse_pointer->SetCursorDefault();
					}
					else
					{
						gpC_mouse_pointer->SetCursorAttack();
					}
				}
#ifdef __FRIEND_SYSTEM_VIVA__//add by viva
				if(g_pSDLInput->KeyDown(DIK_LCONTROL) && g_pSDLInput->m_lb_down)	
				{
					gpC_base->SendMessage(UI_FRIEND_CHATTING_ADD_FRIEND, 0, 0, (void*)pCreature->GetName());
				}
#endif
					
			}
			
			//------------------------------------------------
			
			//------------------------------------------------
			else if (pObject->GetObjectType()==MObject::TYPE_ITEM)
			{
				MItem* pItem = (MItem*)pObject;

				if(g_pZone != NULL && pItem != NULL )
				{
					
					

					if(!g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() ||
						g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() && g_pPlayer->IsVampire() && g_pZone->GetID() != 3001||
						g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() && !g_pPlayer->IsVampire() &&
						g_pPlayer->HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) && g_pZone->GetID() != 3001
						|| g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
						|| g_bLight
#endif
						)
					{
						g_pTopView->SetSelectedItem( pItem->GetID() );			
						
						COLORREF color;
						//------------------------------------------------
						
						//------------------------------------------------
						if (pItem->IsSpecialColorItem() )
						{
							WORD temp_color = CIndexSprite::ColorSet[pItem->GetSpecialColorItemColorset()][16];
							color = RGB(CSDLGraphics::Red(temp_color)<<3, CSDLGraphics::Green(temp_color)<<3, CSDLGraphics::Blue(temp_color)<<3);
						}
						else
						if (pItem->IsEmptyItemOptionList())
						{
							color = g_pClientConfig->COLOR_NAME_ITEM;	
						}
						else if(pItem->GetItemOptionListCount() == 2)
						{
							color = g_pClientConfig->COLOR_NAME_ITEM_RARE_OPTION;
						}
						
						else if(pItem->GetItemOptionListCount() > 2)
						{
							color = g_pClientConfig->COLOR_NAME_VAMPIRE; //Red
						}
						
						else
						{
							color = g_pClientConfig->COLOR_NAME_ITEM_OPTION;
						}
						
						//------------------------------------------------
						
						//------------------------------------------------
						if (!g_bMouseInPortal&&!g_pPlayer->IsInDarkness())
						{						
							
							if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
							{
								gpC_mouse_pointer->SetCursorPickUp( "", color );
							}
							
							else
							{
								char str[80];
								strcpy(str, pItem->GetName());
								
								gpC_mouse_pointer->SetCursorPickUp( str, color );
							}
						}
					} 
				}
			}

			//------------------------------------------------
			
			//------------------------------------------------
			else if (pObject->GetObjectType()==MObject::TYPE_EFFECT)
			{
				g_pTopView->SetSelectedEffect( pObject->GetID() );
			}

			//------------------------------------------------
			
			//------------------------------------------------
//			else if (pObject->GetObjectType()==MObject::TYPE_INTERACTIONOBJECT)
//			{
//				g_pTopView->SetSelectedInteractionObject( pObject->GetID() );
//			}
		}


		//---------------------------------------------------	
		//
		
		//
		//---------------------------------------------------	
		if (g_pPlayer->IsNotDelay() && !g_pPlayer->HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE ) )
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("noDel");
			#endif

			//-----------------------------------------------
			
			//-----------------------------------------------
			if (g_pSDLInput->KeyDown(DIK_LSHIFT))
			{
				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("shiAt");
				#endif
				//-----------------------------------------------
				
				//-----------------------------------------------
				if (g_pSDLInput->m_lb_down
					
					&& !g_pPlayer->IsInCasket()
					&& !g_pPlayer->IsUndergroundCreature()
					&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
				{
					g_pPlayer->UnSetRequestMode();

					//-----------------------------------------------
					
					//-----------------------------------------------
					if (pObject!=NULL)
					{
						if (pObject->GetObjectType()==MObject::TYPE_CREATURE)
						{
							
							
							MCreature *pCreature = dynamic_cast<MCreature*>(pObject);
							g_bLButtonDown = TRUE;
							
							BOOL	ForceAttack = TRUE;

							if (g_pPlayer->TraceCreatureToBasicAction( 
										pCreature->GetID(), 
										true))		
							{
								//----------------------------------
								
								//----------------------------------
								g_pPlayer->SetRepeatAction();
								g_bPreviousMove = false;
							}
							
							g_pTopView->SetSelectedSectorNULL();
						}
					}
					//-----------------------------------------------
					
					//-----------------------------------------------
					//g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
					
					
					//g_pTopView->SetSelectedSector( g_SelectSector );

					//g_pPlayer->TraceSectorToBasicAction( g_SelectSector.x, g_SelectSector.y );			

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("shiKK");
					#endif
				}		
				//-----------------------------------------------
				
				//-----------------------------------------------
				else if (g_pSDLInput->m_rb_down)
				{
					g_pPlayer->UnSetRequestMode();

					ProcessInputRButtonDown(pObject, true);

					g_bPreviousMove = false;
					 

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("shiRK");
					#endif
				}
				
			}

			//---------------------------------------------------------------
			// L/R ButtonDown only
			//---------------------------------------------------------------
			else
			{
				
				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("LRonly");
				#endif
				//---------------------------------------------------------------
				//
				//				LButtonDown
				//
				//---------------------------------------------------------------
				if (g_pSDLInput->m_lb_down 
					
					&& !g_pPlayer->IsInCasket()
					&& !g_pPlayer->IsUndergroundCreature()
					&& g_pPlayer->CurPernalShop() != 2
					&& g_pPlayer->CurPernalShop() != 1
					&& gC_vs_ui.IsRunningPersnalShop() == false
					)
				{		
					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("Lon");
					#endif

					g_pPlayer->UnSetRequestMode();

					g_bLButtonDown = TRUE;
					
					
					g_pTopView->SetSelectedSectorNULL();

					
					g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
					//g_pTopView->SetSelectedSector(g_SelectSector.x, g_SelectSector.y);

					//
					//MObject*	pObject = g_pTopView->GetSelectedObject(g_x, g_y);

					
					if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
					{		
						int TempDir = MTopView::GetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY(),g_SelectSector.x, g_SelectSector.y);
						
//							if(g_pPlayer->GetDirection() != TempDir)
//							{
//								CGMove _CGMove;
//								_CGMove.setX( g_pPlayer->GetX() );
//								_CGMove.setY( g_pPlayer->GetY() );
//								_CGMove.setDir( TempDir );
//								g_pSocket->sendPacket( &_CGMove );
//							}
						if(g_pPlayer->GetDirection() == TempDir)
						{
//							g_pPlayer->UnSetRequestMode();
//							g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
//							g_pPlayer->SetSpecialActionInfo(SKILL_TURRET_FIRE);
//							if (g_pPlayer->TraceSectorToSpecialAction( g_SelectSector.x, g_SelectSector.y ))
//							{

//								g_pTopView->SetSelectedSector( g_SelectSector );
//							
//								g_pPlayer->SetRepeatAction();
//							}
//
//							g_bPreviousMove = false;
//								char szTemp[128];

//								g_pSystemMessage->Add(szTemp);
						}
						else
						{
//							CGMove _CGMove;
//							_CGMove.setX( g_pPlayer->GetX() );
//							_CGMove.setY( g_pPlayer->GetY() );
//							_CGMove.setDir( TempDir );
//							g_pSocket->sendPacket( &_CGMove );
							if(g_pPlayer->IsStop())
								g_pPlayer->SetCurrentDirection(TempDir);
						}
					}
					//--------------------------------------------------
					
					//--------------------------------------------------
					else if (pObject == NULL)
					{		
						
						if (//!g_pSDLInput->KeyDown(DIK_LSHIFT) &&
							!g_pSDLInput->KeyDown(DIK_LCONTROL))
						{		
							if (g_pPlayer->IsNotDelay() && !g_pPlayer->HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE ) 
								
								//&&  !g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET ) 
								
								)
							{
							if (g_pPlayer->SetMovePosition(g_SelectSector.x, g_SelectSector.y))
							{
								TraceInteraction("Move target accepted sector=%d,%d player=%d,%d",
									g_SelectSector.x, g_SelectSector.y, g_pPlayer->GetX(), g_pPlayer->GetY());
								//if (g_pPlayer->IsStop())
								{
										
										g_pPlayer->TraceNULL();							
										
										g_pPlayer->SetNextActionToMove();
										
										g_bPreviousMove = true;
									}
								}
							}
							else
							{
								TraceInteraction("Move target rejected sector=%d,%d player=%d,%d",
									g_SelectSector.x, g_SelectSector.y, g_pPlayer->GetX(), g_pPlayer->GetY());
							}
						}

						 
					}
					//--------------------------------------------------
					
					//--------------------------------------------------
					else 
					{	
						
						
						//g_bLButtonDown = FALSE;

						switch (pObject->GetObjectType())
						{			
							case MObject::TYPE_CREATURE :	
								{
									
									MCreature* TempCreature = (MCreature*)pObject;
									if(TempCreature->CurPernalShop() == 1 )
//										|| g_pPlayer->IsFlyingCreature() || g_pPlayer->IsUndergroundCreature() 
//										|| g_pPlayer->IsGroundCreature() || g_pPlayer->IsFakeCreature() 
//										|| g_pPlayer->GetMoveDevice()== 1|| g_pPlayer->GetMoveDevice()== 2)
									{
										gpC_base->SendMessage(UI_REQUEST_STORE_INFO, pObject->GetID(),0);
										gC_vs_ui.SetOtherObjectID(pObject->GetID());
									}
									

									if (g_pPlayer->TraceCreatureToBasicAction( pObject->GetID(), 
																				bForceAttack, true ))
									{
										//----------------------------------
										
										//----------------------------------
										g_pPlayer->SetRepeatAction();
										g_bPreviousMove = false;
									}
									
									g_pTopView->SetSelectedSectorNULL();
								}
							break;

							case MObject::TYPE_ITEM :
								TraceInteraction("TraceItem requested objectID=%d player=%d,%d",
									pObject->GetID(), g_pPlayer->GetX(), g_pPlayer->GetY());
								g_pPlayer->TraceItem( pObject->GetID() );

								g_bLButtonDown = FALSE;

								g_bPreviousMove = false;
							break;

							case MObject::TYPE_EFFECT :
								g_pPlayer->TraceEffect( pObject->GetID() );

								g_bLButtonDown = FALSE;

								g_bPreviousMove = false;								
							break;
							
							case MObject::TYPE_INTERACTIONOBJECT :
								g_pPlayer->TraceInteractionObject( pObject->GetID(), ((MInteractionObject*)pObject)->GetAction() );

								g_bLButtonDown = FALSE;

								g_bPreviousMove = false;
								//-------------------------
								// TEST CODE
								//-------------------------
								//((MInteractionObject*)pObject)->SetAction( !((MInteractionObject*)pObject)->GetAction() );
							break;
						}
					}
					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("LK");
					#endif
				}

				//---------------------------------------------------------------
				//
				//		 RButtonDown
				//
				//---------------------------------------------------------------
				else if (g_pSDLInput->m_rb_down)
				{
					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("Ro");
					#endif

					g_pPlayer->UnSetRequestMode();

					ProcessInputRButtonDown(pObject);

					g_bPreviousMove = false;

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("RK");
					#endif
				}

				//---------------------------------------------------------------
				
				
				
				//---------------------------------------------------------------
				else if (g_bLButtonDown 				
						&& !g_pSDLInput->KeyDown(DIK_LSHIFT)
#ifdef __METROTECH_TEST__
						&& !g_bCButtonDown
#endif
						)
				{
					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("ConM");
					#endif

					//--------------------------------------------------
					
					
					//--------------------------------------------------
					if (pObject == NULL && g_bPreviousMove)
					{	
						if (g_pPlayer->IsNotDelay() && !g_pPlayer->IsRepeatAction())
						{
							g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);

							
							g_pPlayer->TraceNULL();
							
							if (g_pPlayer->SetMovePosition(g_SelectSector.x, g_SelectSector.y))
							{
								g_pPlayer->SetNextActionToMove();
							}
						}
					}

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("ConMK");
					#endif
				}
			}
		}
	}	// lock mode

	//---------------------------------------------------	
	// LButton Up
	//---------------------------------------------------	
	if (g_pSDLInput->m_lb_up && g_bLButtonDown
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("LUP2");
		#endif

		g_bLButtonDown = FALSE;
	
		if (g_pPlayer->IsRepeatAction())
		{
			
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();

			g_bPreviousMove = false;
		}
		else
		{	
			
			POINT point;
			
			
			g_pPlayer->GetNextDestination( point );		
			if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
			{
				
				g_pPlayer->GetDestination( point );
				if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
				{
					//-_-;;
				}
				else
				{
					g_pTopView->SetSelectedSector( point );
				}
			}
			else
			{
				g_pTopView->SetSelectedSector( point );
			}		
		}
	}

	//---------------------------------------------------	
	// RButton Up
	//---------------------------------------------------	
	if (g_pSDLInput->m_rb_up && g_bRButtonDown
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("RUP2");
		#endif

		g_bRButtonDown = FALSE;
	
		if (g_pPlayer->IsRepeatAction())
		{
			
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();
		}
	}

	if(g_pSDLInput->m_cb_up && g_bCButtonDown)
	{
		g_bCButtonDown = FALSE;
	}

	
	if (g_pPlayer->GetAction()==ACTION_STAND)
	{
		g_pTopView->SetSelectedSectorNULL();
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("IAEnd");
	#endif


	//---------------------------------------------------
	//
	// keyboard
	//
	//---------------------------------------------------
	 

	#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
		//---------------------------------------------------
		
		//---------------------------------------------------
		if (g_pSDLInput->KeyDown(DIK_8))
		{
			SetWeather( WEATHER_RAINY, rand()%10+10);
		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		if (g_pSDLInput->KeyDown(DIK_9))
		{
			SetWeather( WEATHER_SNOWY, rand()%10+10);
		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		if (g_pSDLInput->KeyDown(DIK_0))
		{
			SetWeather( WEATHER_CLEAR, 0 );
		}



		//---------------------------------------------------
		
		//---------------------------------------------------

		if (g_pSDLInput->KeyDown(DIK_SUBTRACT) 
			)//&& g_pPlayer->GetLightSight()!=0)
		{
//			g_pPlayer->SetLightSight( g_pPlayer->GetLightSight() - 1 );		
						g_pClientConfig->QuestItemColorSet -= 15;

					if( g_pClientConfig->QuestItemColorSet <= 0 )
						g_pClientConfig->QuestItemColorSet = 495-15;
		}
		
		if (g_pSDLInput->KeyDown(DIK_ADD) )// && g_pPlayer->GetLightSight()!=13)
		{
//			g_pPlayer->SetLightSight( g_pPlayer->GetLightSight() + 1 );		
			
			
		}

		//-----------------------------------------------
		// play Sound
		//-----------------------------------------------
		if (g_pSDLInput->KeyDown(DIK_F4))
		{
			//g_sb2 = g_Sound.LoadWav((*g_pSoundTable)[rand()%g_pSoundTable->GetSize()].Filename);
			//g_sb2 = g_Sound.LoadWav("Data\\Sound\\Sound1.wav");
			// [ TEST CODE ]
			/*
			static int pan = -10000;
			static int p = 200;
			pan += p;

			if (pan > 10000) 
			{
				pan = 10000;
				p = -p;
			}

			if (pan < -10000) 
			{
				pan = -10000;
				p = -p;
			}
			*/

			//g_Sound.ChangePan((*g_pSoundTable)[rand()%g_pSoundTable->GetSize()].pDSBuffer, pan);
			//g_Sound.Play((*g_pSoundTable)[rand()%g_pSoundTable->GetSize()].pDSBuffer);

			//PlaySound( rand()%g_pSoundTable->GetSize(), 
			//			false,
			//			g_pPlayer->GetX(), g_pPlayer->GetY() );
			PlaySound( SOUND_S_DIE, 
						false,
						g_pPlayer->GetX() + rand()%20-10, g_pPlayer->GetY() + rand()%20-10 );			

		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		if (g_pSDLInput->KeyDown(DIK_F9))
		{
			BYTE DarkBits = g_pTopView->GetDarkBits()-1;
			g_pTopView->SetDarkBits(DarkBits);
		}
		
		//---------------------------------------------------
		
		//---------------------------------------------------
		if (g_pSDLInput->KeyDown(DIK_F10))
		{
			BYTE DarkBits = g_pTopView->GetDarkBits()+1;
			g_pTopView->SetDarkBits(DarkBits);
		}	
		
		// Gamma Test
		/*
		static BYTE gammaStep = 255;
		if (CSDLGraphics::IsSupportGammaControl())
		{
			if (g_pSDLInput->KeyDown(DIK_LBRACKET))
			{
				gammaStep -= 10;
				CSDLGraphics::SetGammaRamp( gammaStep );
			}
			if (g_pSDLInput->KeyDown(DIK_RBRACKET))
			{		
				gammaStep += 10;		
				CSDLGraphics::SetGammaRamp( gammaStep );
			}
		}
		*/
	#endif

	//-----------------------------------------------
	// File Loading Test with Thread
	//-----------------------------------------------		
	/*
	if (g_pSDLInput->KeyDown(DIK_Z))
	{	
		// test for state of event nonsignaled?
		if (WaitForSingleObject(g_hFileEvent, 0) != WAIT_OBJECT_0)
		{
			g_ThreadJob = THREADJOB_LOAD_CREATURE;
			SetEvent(g_hFileEvent);			
		}		
	} 

	if (g_pSDLInput->KeyDown(DIK_X) && WaitForSingleObject(g_hFileEvent, 0) != WAIT_OBJECT_0)
	{	
		g_pTopView->ReleaseCreatureSPK( 0 );
		g_pTopView->ReleaseCreatureSPK( 1 );
	}
	*/

	//-----------------------------------------------
	// Thread Priority Test
	//-----------------------------------------------	
	/*
	if (g_pSDLInput->KeyDown(DIK_9))
	{
		SetThreadPriority(g_hFileThread, 
							THREAD_PRIORITY_BELOW_NORMAL);
	}

	if (g_pSDLInput->KeyDown(DIK_0))
	{
		SetThreadPriority(g_hFileThread, 
							THREAD_PRIORITY_HIGHEST);
	}
	*/
		
	#ifdef OUTPUT_DEBUG
		
		/*
		if (g_pSDLInput->KeyDown(DIK_1))
		{	
			MoveZone( 1 );				
		}
		else if (g_pSDLInput->KeyDown(DIK_2))
		{				
			MoveZone( 2 );				
		}	
		else if (g_pSDLInput->KeyDown(DIK_3))
		{							
			MoveZone( 3 );			
		}
		else if (g_pSDLInput->KeyDown(DIK_4))
		{			
			MoveZone( 4 );				
		}
		*/
		

		//-----------------------------------------------
		// Test for Effect
		//-----------------------------------------------
		#ifdef _DEBUG
			static DWORD start=0, current = 0;

			

			current++;

			if (current-start > 10)
			{
				if (g_pSDLInput->KeyDown(DIK_LCONTROL) && g_pSDLInput->KeyDown(DIK_RCONTROL))
				{					
					
					POINT playerPoint = MTopView::MapToPixel(g_pPlayer->GetX(), g_pPlayer->GetY());

					for (int i=-1; i<=1; i++)
					{
						for (int j=-1; j<=1; j++)
						{
							if (i==0 && j==0)
								continue;
							 

							BYTE effectSpriteType = rand()%(*g_pEffectSpriteTypeTable).GetSize();
			
							//if (g_pSDLInput->KeyDown(DIK_1))	effectSpriteType = 0;
							//else if (g_pSDLInput->KeyDown(DIK_2))	effectSpriteType = 1;
							//else if (g_pSDLInput->KeyDown(DIK_3))	effectSpriteType = 2;
							//else if (g_pSDLInput->KeyDown(DIK_4))	effectSpriteType = 3;

							
							TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[effectSpriteType].FrameID;
							BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[effectSpriteType].BltType;
							
							// BLT_NORMAL
							int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
							
							// Sound ... Test *_*;;
							//g_Sound.Play(g_sb1);



							MLinearEffect*	pEffect;
							//pEffect = new MParabolaEffect(BLT_EFFECT);
							pEffect = new MLinearEffect(BLT_EFFECT);

							pEffect->SetFrameID(frameID, maxFrame);		

							
							pEffect->SetPixelPosition(playerPoint.x, playerPoint.y, 0);	
							
							
							pEffect->SetTarget(playerPoint.x + -i*300, 
												playerPoint.y + -j*350,
												0,
												20);	// step

							
							pEffect->SetCount(1000);							

							//pEffect->SetLight(1);							

							g_pZone->AddEffect( pEffect );
							

							//*/	
							 
						}					
					}
				}

				start = current;
			}
		#endif
	#endif
}

//---------------------------------------------------------------------------
// UpdateGame Draw
//---------------------------------------------------------------------------
void
CGameUpdate::UpdateDraw()
{
	const DWORD perfStartMs = timeGetTime();
	DWORD perfTopViewMs = 0;
	DWORD perfUiMs = 0;
	DWORD perfCursorMs = 0;
	DWORD perfFpsMs = 0;
	DWORD perfCopyMs = 0;
	DWORD perfFlipMs = 0;
	DWORD perfSectionStartMs = 0;

	TraceGameHeartbeat("UpdateDraw begin");
	static bool s_firstGameDraw = true;
	if (s_firstGameDraw)
	{
		TraceGameUpdate("CGameUpdate::UpdateDraw first call begin");
		s_firstGameDraw = false;
	}
	//--------------------------------------------------------
	/*
	RECT rectClip =
	{
		50,
		50,
		750,
		550
	};
	g_pBack->SetClip(&rectClip);
	*/
	static bool outputInfo = false;		
	

	POINT	point;
	RECT	rect;
	
	//#ifdef	OUTPUT_DEBUG
		char	str[128];
	//#endif

	
	//
	
	
	// [ TEST CODE ]
	/*
	rect.left = 0;
	rect.right = CLIPSURFACE_WIDTH;
	rect.top = CLIPSURFACE_HEIGHT;
	rect.bottom = SURFACE_HEIGHT;
	g_pBack->FillRect(&rect, CSDLGraphics::Color(5,7,7));
	*/

	//-----------------------------------------------------------------		
	
	//-----------------------------------------------------------------
	GetCursorPos(&point);
	ScreenToClient(g_hWnd, &point);//by viva
	
		
	
	gC_vs_ui.MouseControl(M_MOVING, point.x, point.y);

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			//DEBUG_ADD("dd");//[Update-Draw] Before Draw");
	#endif

//	if (true)
//	{
//		//-----------------------------------------------------------------

//		//-----------------------------------------------------------------
//		__BEGIN_PROFILE("GameDraw3D")
//			
//		const MEvent *event = g_pEventManager->GetEventByFlag(EVENTFLAG_SHAKE_SCREEN);
//		if(event != NULL)
//		{		
//			static int count = 0;
//			const int maxStep = 13;
//			const int step[maxStep] = { 0, 2, 3, 6, 5, 3, 0, -4, -6, -5, -3, -1, 0 };
//			
//			g_pTopView->Draw( step[count]*2, ((g_pEventManager->IsEvent(EVENTID_METEOR_SHAKE) == true)?((count%2)*20-10):(count%3-1)*3) );
//			
//			if(g_bFrameChanged)
//			{
//				count ++;
//				count %= maxStep;			
//			}
//		}
//		else
//			g_pTopView->Draw(0,0);
//
//		__END_PROFILE("GameDraw3D")
//
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//				DEBUG_ADD("us");//[Update-Draw] Before UI Show");
//		#endif
//
//		//-----------------------------------------------------------------

//		//-----------------------------------------------------------------		
//		__BEGIN_PROFILE("UIDraw3D")
//
//		gC_vs_ui.Show();		
//
//		__END_PROFILE("UIDraw3D")
//
//
//		__BEGIN_PROFILE("DrawFPS")
//			if (outputInfo || g_pUserOption->DrawFPS)
//			{
//				//-----------------------------------------------------------------

//				//-----------------------------------------------------------------
//				sprintf(str, "%d FPS(HAL)", g_FrameRate);	
//			
//				g_pBack->GDI_Text(11,11, str, RGB(20,20,20));
//				g_pBack->GDI_Text(10,10, str, 0xFFFFFF);
//			}
//		__END_PROFILE("DrawFPS")
//		
//
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//				DEBUG_ADD("usk");//[Update-Draw] After UI Show");
//		#endif
//
//		__BEGIN_HELP_EVENT
//			UpdateDrawHelp();
//		__END_HELP_EVENT
//	}
//	else
	{
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		//g_SetFL2Surface( g_pLast->GetSurface() );

		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------		
		__BEGIN_PROFILE("GameDraw2D")
		perfSectionStartMs = timeGetTime();

//		#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
//			if (g_pSDLInput->KeyDown(DIK_SPACE))
			const MEvent *event = g_pEventManager->GetEventByFlag(EVENTFLAG_SHAKE_SCREEN);
			if(event != NULL && event->IsShowTime() == true)
			{		
				static int count = 0;
				const int maxStep = 13;

				// 2004, 5, 21 sobeit modify start
				//const int step[maxStep] = { 0, 2, 3, 6, 5, 3, 0, -4, -6, -5, -3, -1, 0 };
				//g_pTopView->Draw( step[count]*event->parameter3, ((g_pEventManager->IsEvent(EVENTID_METEOR_SHAKE) == true)?((count%2)*3-3):(count%3-1)*3)*event->parameter3 );
				const int step[maxStep] = { 0, 1, 2, 2, 2, 1, 0, -2, -2, -2, -1, -1, 0 };
				g_pTopView->Draw( step[count]*event->parameter3, ((g_pEventManager->IsEvent(EVENTID_METEOR_SHAKE) == true)?((count%2)*3-2):(count%3-1)*3)*event->parameter3 );
				// 2004, 5, 21 sobeit modify end

				if(g_bFrameChanged)
				{
					count ++;
					count %= maxStep;			
				}
			}
			else
//		#endif
			{
				g_pTopView->Draw(0,0);
				TraceGameHeartbeat("UpdateDraw after TopView Draw normal");
			}

		__END_PROFILE("GameDraw2D")
		perfTopViewMs = timeGetTime() - perfSectionStartMs;

		//g_SetFL2Surface( g_pBack->GetSurface() );

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD("us");//[Update-Draw] Before UI Show");
		#endif

		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------		
		__BEGIN_PROFILE("UIDraw2D")
		perfSectionStartMs = timeGetTime();

		TraceGameHeartbeat("UpdateDraw before UI Show");
		gC_vs_ui.Show();
		TraceGameHeartbeat("UpdateDraw after UI Show");

		__END_PROFILE("UIDraw2D")
		perfUiMs = timeGetTime() - perfSectionStartMs;

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD("usk");//[Update-Draw] After UI Show");
		#endif

//		__BEGIN_HELP_EVENT
//			UpdateDrawHelp();
//		__END_HELP_EVENT

		if (!g_pTopView->IsDrawRequest())
		{
			perfSectionStartMs = timeGetTime();
			TraceGameHeartbeat("UpdateDraw before DrawMousePointer");
			gC_vs_ui.DrawMousePointer();
			TraceGameHeartbeat("UpdateDraw after DrawMousePointer");
			perfCursorMs = timeGetTime() - perfSectionStartMs;
		}

		
		/* profiler disabled for local hang isolation: DrawFPS */
			perfSectionStartMs = timeGetTime();
			TraceGameHeartbeat("UpdateDraw before DrawFPS");
			if (outputInfo || g_pUserOption->DrawFPS)
			{
				//-----------------------------------------------------------------
				
				//-----------------------------------------------------------------				
				sprintf(str, "%d FPS", g_FrameRate);	
				
				// g_pLast->GDI_Text(11,11, str, RGB(20,20,20));
				TextSystem::TextService::RenderText(11, 11, str);
				// g_pLast->GDI_Text(10,10, str, 0xFFFFFF);
				TextSystem::TextService::RenderText(10, 10, str);
			}
			TraceGameHeartbeat("UpdateDraw after DrawFPS");
			perfFpsMs = timeGetTime() - perfSectionStartMs;
		/* profiler disabled for local hang isolation: DrawFPS */

		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		/* profiler disabled for local hang isolation: LastToBack */
			perfSectionStartMs = timeGetTime();
			TraceGameHeartbeat("UpdateDraw before LastToBack");

//		if(true)
//		{
//			point.x = 0;
//			point.y = 0;
//			rect.left = 0;
//			rect.top = 0;
//			rect.right = SURFACE_WIDTH;
//			rect.bottom = SURFACE_HEIGHT;
//			g_pLast->SetTransparency(0x001F);
//			g_pBack->Blt( &point, g_pLast, &rect );		
//		}
//		else
//		{
			point.x = 0;
			point.y = 0;
			rect.left = 0;
			rect.top = 0;
			rect.right = g_GameRect.right;
			rect.bottom = g_GameRect.bottom;
			g_pBack->BltNoColorkey( &point, g_pLast, &rect );		
//		}
			TraceGameHeartbeat("UpdateDraw after LastToBack");
			perfCopyMs = timeGetTime() - perfSectionStartMs;

		/* profiler disabled for local hang isolation: LastToBack */
	}
	
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			//DEBUG_ADD("ddk");//[Update-Draw] After Draw");
	#endif


	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	/*
	WORD*	lpSurface;
	long	lPitch;
	g_pLast->LockW(NULL, lpSurface, lPitch);

	for (int y=g_y-30; y<g_y+30; y++)
		for (int x=g_x-60; x<g_x+60; x++)
		{
			point = g_pTopView->GetSelectedSector(x,y);

			if (point.x==g_SelectSector.x && point.y==g_SelectSector.y)
			{
				*(lpSurface + y*lPitch + x) = 0xCCCC;
			}
		}

	g_pLast->Unlock();

	sprintf(str, "Selected Sector = (%d, %d)", g_SelectSector.x, g_SelectSector.y);
	// g_pLast->GDI_Text(10,200, str, RGB(220,220,220));
	TextSystem::TextService::RenderText(10, 200, str);
	*/


	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	//if (g_bDrawMinimap)
	//{	
	//	g_pTopView->DrawMinimap(
	//			800-g_pZone->GetWidth()*2,	//10
	//			0, //-g_pZone->GetHeight(),	// 465
	//			1);
	//}

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	/*
	if (!g_bNetStatusGood)
	{
		DEBUG_ADD("Network Condition is Bad.");

		// g_pBack->GDI_Text(351,11, (*g_pGameStringTable)[STRING_NETWORK_CONDITION_BAD].GetString(), RGB(10,10,10));
		TextSystem::TextService::RenderText(351, 11, (*g_pGameStringTable)[STRING_NETWORK_CONDITION_BAD].GetString());
		// g_pBack->GDI_Text(350,10, (*g_pGameStringTable)[STRING_NETWORK_CONDITION_BAD].GetString(), RGB(230,50,50));		
		TextSystem::TextService::RenderText(350, 10, (*g_pGameStringTable)[STRING_NETWORK_CONDITION_BAD].GetString());		
	}
	*/

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if (g_pPlayer->IsDead())
	{
		TraceGameHeartbeat("UpdateDraw player dead overlay enter");
		if (g_pPlayer->GetActionCount()==g_pPlayer->GetActionCountMax())		
		{	
			static BYTE a = 0;
			static BYTE count = 0;

			int second = g_pPlayer->GetDeadDelayLast();
			//char str[128];

			// 30~20
			int blackValue = 20;
			if (second >= 3)
			{
				//blackValue = max(0, (g_pClientConfig->DELAY_PLAYER_DEAD/1000 - (second-5)*2)) * 2;
				blackValue = min(31, second+20);
			}

			
			g_pTopView->SetFadeStart(blackValue, 0, 0, 5,5,5);
		

			
			if ((a & 0x01)==0)
			{
				//if (second==0)
				{
					
					//g_pBack->GDI_Text(270,400, str, RGB(230,230,230));
				}
				//else
				{
				//	sprintf(str, (*g_pGameStringTable)[STRING_RESURRECTION_AFTER_SECONDS].GetString(), second);
					//strcpy(str, "... Please Wait for Your Resurrection ...");
				//	g_pBack->GDI_Text(301,401, str, RGB(0,0,0)); 
				//	g_pBack->GDI_Text(300,400, str, RGB(230,230,230));
				}
				
				
				if (second < 4
					&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
				{
					// FixMe Please oh shit by ssuk
					if( (g_pWarManager->IsExist( g_pZone->GetID() ) || g_pZone->GetID() == 71 || g_pZone->GetID() == 72 || g_pZone->GetID() == 73) && gC_vs_ui.IsRunningBloodBibleStatus() )
						gC_vs_ui.RequestShrineMinimap( g_pClientConfig->DELAY_PLAYER_RESURRECT_TIMEOUT );

					gC_vs_ui.RequestDie( g_pClientConfig->DELAY_PLAYER_RESURRECT_TIMEOUT );

					g_pTempInformation->SetMode(TempInformation::MODE_WAIT_RESURRECT);
					bool bResurrect = false, bElixir= false, bEternity= false, IsSiegeAttacker= false ;

					
					if(0 == g_pPlayer->GetResurrectZoneID())
					// 2005, 1, 18, sobeit add end
					{
						if( g_pInventory->FindItem( ITEM_CLASS_RESURRECT_ITEM, 0 ) != NULL )
							bResurrect = true;

						if(g_pInventory->FindItem( ITEM_CLASS_RESURRECT_ITEM, 1 ) != NULL )
							bElixir = true;

						//bEternity = (*g_pSkillInfoTable)[SKILL_ETERNITY].IsEnable();						
						bEternity = g_pSkillAvailable->IsEnableSkill( SKILL_ETERNITY ) &&
									(*g_pSkillInfoTable)[SKILL_ETERNITY].IsAvailableTime() 
									
									&& (*g_pSkillManager)[SKILLDOMAIN_HEAL].GetDomainLevel() >= (*g_pSkillInfoTable)[SKILL_ETERNITY].GetLearnLevel();
									// 2004, 11, 11, sobeit add end
						IsSiegeAttacker = g_pZone->GetPKType() == PK_TYPE_SIEGE &&
										 ( g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) );
					}
					gC_vs_ui.RequestResurrect( bResurrect, bElixir, bEternity , IsSiegeAttacker);
				}
			}

			static DWORD lastTime = g_CurrentTime;
			if (g_CurrentTime - lastTime >= g_UpdateDelay)
			{
				if (++count==6) { count = 0; a++; }
				lastTime = g_CurrentTime;
			}
		}
		TraceGameHeartbeat("UpdateDraw player dead overlay exit");
	}

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	//
	
	
	
	// 	
	if (g_pUserOption->UseSmoothCursor)
	{
		if (CSDLGraphics::IsFullscreen() && g_FrameRate > LIMIT_FPS_FOR_CURSOR)
		{
			GetCursorPos(&point);
			
			ScreenToClient(g_hWnd, &point);//by viva
			
			
			gC_vs_ui.MouseControl(M_MOVING, point.x, point.y);

			
			MOUSEPOINTER_INFO mp_info;
			gC_vs_ui.GetCurrentMousePointerInfo(mp_info);

			g_pCursorSurface->Init(2, mp_info.width, mp_info.height);
			//DEBUG_ADD_FORMAT("[MouseCursor] %d,%d, %d,%d", mp_info.width, mp_info.height, mp_info.rx, mp_info.ry);
			
			POINT tempPoint;
			tempPoint.x = mp_info.x;
			tempPoint.y = mp_info.y;
			g_pCursorSurface->Store(0, g_pBack, &tempPoint);
			
			g_bSmoothCursor = true;
		}
		else
		{
			g_bSmoothCursor = false;
		}	
	}
	else
	{
		//GetCursorPos(&point);	
			
		
		//gC_vs_ui.MouseControl(M_MOVING, point.x, point.y);
	}

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if (!g_pTopView->IsDrawRequest())
	{
		gC_vs_ui.DrawMousePointer();
	}

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	__BEGIN_PROFILE("DrawDebugInfo")

	PrintInfo* pPrintInfo = &gpC_base->m_chatting_pi;

	#ifdef	OUTPUT_DEBUG
		static DWORD lastCount = 0;	
		
		if (g_bSlideScreenShot)
		{
			if (g_bSlideRectSelect)
			{
				g_rectScreenShot.right = g_x;
				g_rectScreenShot.bottom = g_y;
			}

			RECT rect = g_rectScreenShot;

			g_pBack->DrawRect( &rect, CSDLGraphics::Color(3, 30, 10) );
		}
		
		if (g_pSDLInput->KeyDown(DIK_LSHIFT) && g_pSDLInput->KeyDown(DIK_RSHIFT) 
			&& g_CurrentFrame-lastCount > 7)
		{
			outputInfo = !outputInfo;
			lastCount = g_CurrentFrame;
		}

		//----------------------------------------------------------------
		// UDP packet test
		//----------------------------------------------------------------
		#ifdef __METROTECH_TEST__

			if (g_pPlayer!=NULL)
				
			{
				if (g_iAutoHealPotion && !g_pPlayer->IsVampire())
				{
					RECT rect = 
					{ 
						2,
						448,
						10,
						455
					};

					if(g_iAutoHealPotion == 1)
						g_pBack->FillRect(&rect, CSDLGraphics::Color(30, 10, 10));
					else if(g_iAutoHealPotion == 2)
						g_pBack->FillRect(&rect, CSDLGraphics::Color(30, 20, 10));
					else if(g_iAutoHealPotion == 3)
						g_pBack->FillRect(&rect, CSDLGraphics::Color(30, 10, 20));
				}

				if (g_bAutoManaPotion && !g_pPlayer->IsVampire())
				{
					RECT rect = 
					{ 
						12,
						448,
						20,
						455
					};

					g_pBack->FillRect(&rect, CSDLGraphics::Color(10, 10, 30));
				}

				if (g_iAutoReload && g_pPlayer->IsSlayer())
				{
//					char sz_temp[3];
//					wsprintf(sz_temp, "%d", g_iAutoReload-1);
//					g_PrintColorStr(22, 448, sz_temp, gpC_base->m_chatting_pi, RGB_GREEN);

					RECT rect = 
					{ 
						22,
						448,
						30,
						455
					};

					g_pBack->FillRect(&rect, CSDLGraphics::Color(10, 30, 10));

					rect.left++;
					rect.right--;
					rect.bottom++;

					for(int i = 0; i < g_iAutoReload-1; i++)
					{
						rect.bottom -= 2;
						rect.top = rect.bottom -1;
						g_pBack->FillRect(&rect, CSDLGraphics::Color(0, 0, 0));
					}
				}

				if (g_bLight)
				{
					RECT rect = 
					{ 
						32,
						448,
						40,
						455
					};
					
					g_pBack->FillRect(&rect, CSDLGraphics::Color(10, 30, 30));
				}

				if (g_iSpeed)
				{
					RECT rect = 
					{ 
						42,
						448,
						50,
						455
					};

					if(g_iSpeed < 0)
						g_pBack->FillRect(&rect, CSDLGraphics::Color(30, 0, 0));
					else
						g_pBack->FillRect(&rect, CSDLGraphics::Color(30, 30, 30));
					
					rect.left++;
					rect.right--;
					rect.bottom++;
					
					for(int i = 0; i < abs(g_iSpeed)-1; i++)
					{
						rect.bottom -= 2;
						rect.top = rect.bottom -1;
						g_pBack->FillRect(&rect, CSDLGraphics::Color(0, 0, 0));
					}
				}
			}

			if (outputInfo)
			{
				const int numValue = 5;
				int value[numValue] = 
				{
					g_UDPTest.UDPPacketAvailable,
					g_UDPTest.UDPPacketReceive,
					g_UDPTest.UDPPacketRead,
					g_UDPTest.UDPPacketExecute,
					g_UDPTest.UDPPacketSend
				};

				WORD color[numValue] =
				{
					CSDLGraphics::Color(10, 20, 20),
					CSDLGraphics::Color(20, 20, 20),
					CSDLGraphics::Color(30, 20, 20),
					CSDLGraphics::Color(20, 30, 20),
					CSDLGraphics::Color(20, 20, 30)
				};

				for (int v=0; v<numValue; v++)
				{
					RECT rect = 
					{ 
						210,
						455 + v*8,
						210 + (value[v]%100)*4,
						455 + v*8 + 8
					};

					g_pBack->DrawRect(&rect, color[v]);
				}

				if (outputInfo)
				{
					if (!true)
					{
						g_SetFL2Surface( g_pBack );
					}

					const COLORREF color = RGB(28<<3, 28<<3, 28<<3);				
					pPrintInfo->text_color	= color;
		
					char str[10];
					for (int i=0; i<numValue; i++)
					{
						sprintf(str, "%d", value[i]);
						g_Print(200 + i*40, 455 + i*8, str, pPrintInfo);
					}				

					if (!true)
					{
						g_SetFL2Surface( g_pLast );
					}
				}
			}
		#endif


		if (outputInfo)//!g_pSDLInput->KeyDown(DIK_RSHIFT))
		{
			g_pTopView->DrawDebugInfo( g_pBack );

			if (!true)
			{
				g_SetFL2Surface( g_pBack );
			}

			if (g_bPutMessage && g_pDebugMessage!=NULL)
			{
				int strY = 565;
				
				for (int c=g_pDebugMessage->GetSize()-1; c>=0; c--)
				{
					if ((*g_pDebugMessage)[c][0] != NULL)
					{
						//g_pBack->GDI_Text(10,strY, g_DebugMessage[c], RGB(230,230,230));

						const COLORREF debugColor = RGB(28<<3, 28<<3, 28<<3);				
						pPrintInfo->text_color	= debugColor;
			
						g_Print(10, strY, (*g_pDebugMessage)[c], pPrintInfo);
						strY-=15;
					}
				}

				
				if (g_pDebugMessage->GetFilename()!=NULL)
				{
					sprintf(str, "LogFile : %s", g_pDebugMessage->GetFilename());

					const COLORREF txColor = RGB(31<<3, 11<<3, 11<<3);										
					pPrintInfo->text_color	= txColor;
			
					g_Print(10, 580, str, pPrintInfo);
				}				
			}	
			//*/
			
			if (g_pLoadingThread!=NULL && g_pLoadingThread->IsWorking())
			{		
				sprintf(str, "<<< Loading >>>");
				//g_pBack->GDI_Text(700,580, str, 0xFFFFFF);
				const COLORREF txColor = RGB(31<<3, 31<<3, 31<<3);				

				pPrintInfo->text_color	= txColor;
				
				g_Print(300, 5, str, pPrintInfo);
			}
			
			
			///*	 
			if (g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL)
			{
				sprintf(str, "[%d] %s", g_pPlayer->GetSpecialActionInfo(), (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetName());
				
				//g_pBack->GDI_Text(550,10, str, RGB(20,20,20));
				//g_pBack->GDI_Text(551,11, str, RGB(120,255,120));


				const COLORREF bkColor = RGB(0, 0, 0);
				const COLORREF txColor = RGB(16<<3, 31<<3, 16<<3);				

				pPrintInfo->text_color	= bkColor;
				g_Print(500, 10, str, pPrintInfo);

				pPrintInfo->text_color	= txColor;
				g_Print(501, 11, str, pPrintInfo);				
			}	
			//*/

			
			sprintf(str, "Money : %d", (*g_pMoneyManager).GetMoney());
			
			//g_pBack->GDI_Text(550,10, str, RGB(20,20,20));
			//g_pBack->GDI_Text(551,11, str, RGB(120,255,120));
			
			const COLORREF bkMoneyColor = RGB(0, 0, 0);
			const COLORREF txMoneyColor = RGB(16<<3, 31<<3, 16<<3);				

			pPrintInfo->text_color	= bkMoneyColor;
			g_Print(500, 30, str, pPrintInfo);

			pPrintInfo->text_color	= txMoneyColor;
			g_Print(501, 31, str, pPrintInfo);

			sprintf(str, "QuestColorset = %d", g_pClientConfig->QuestItemColorSet);
			pPrintInfo->text_color	= txMoneyColor;
			g_Print(501, 51, str, pPrintInfo);

			 

			if (!true)
			{
				g_SetFL2Surface( g_pLast );
			}
		}
	#endif

	 

	__END_PROFILE("DrawDebugInfo")

	//---------------------------------------------------------------------
	//
	
	//
	//---------------------------------------------------------------------
	 

	
	//g_pBack->HLine(point.x-7, point.y, 7, color);
	//g_pBack->HLine(point.x+1, point.y, 7, color);
	//g_pBack->VLine(point.x, point.y-7, 7, color);
	//g_pBack->VLine(point.x, point.y+1, 7, color);	
	//*/
	

	//-----------------------------------------------------------------
	// flip
	//-----------------------------------------------------------------
	TraceGameHeartbeat("UpdateDraw before Flip");
	perfSectionStartMs = timeGetTime();
	CSDLGraphics::Flip();
	perfFlipMs = timeGetTime() - perfSectionStartMs;
	TraceGameHeartbeat("UpdateDraw after Flip");
	TraceRenderPerfFrame(perfTopViewMs, perfUiMs, perfCursorMs, perfFpsMs, perfCopyMs, perfFlipMs, timeGetTime() - perfStartMs);
}

//-----------------------------------------------------------------------------
// Update Draw Help
//-----------------------------------------------------------------------------
void
CGameUpdate::UpdateDrawHelp()
{
	int strX = 350;
	int strY = (g_pPlayer->IsSlayer()? 430 : 460);

	PrintInfo* pPrintInfo = &gpC_base->m_info_pi;

	for (int c=g_pHelpMessage->GetSize()-1; c>=0; c--)
	{
		if ((*g_pHelpMessage)[c][0] != NULL)
		{
			const COLORREF color = RGB(28<<3, 28<<3, 8<<3);

			pPrintInfo->text_color = 0;
			g_Print(strX+1, strY+1, (*g_pHelpMessage)[c], pPrintInfo);

			pPrintInfo->text_color = color;
			g_Print(strX, strY, (*g_pHelpMessage)[c], pPrintInfo);

			strY-=20;
		}		
	}

	
	static DWORD HelplastTime = g_CurrentTime;
	if (g_CurrentTime - HelplastTime >= g_pClientConfig->DELAY_GAMEMESSAGE)
	{
		g_pHelpMessage->Add("\0");
		HelplastTime = g_CurrentTime;
	}
}

//-----------------------------------------------------------------------------
// PacketTest
//-----------------------------------------------------------------------------
void
PacketTest()
{
	MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();

	for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
	{
		MCreature* pCreature = (*iCreature).second;

		if (pCreature->GetID()!=g_pPlayer->GetID()
			&& !pCreature->IsNPC()
			&& (rand()%8==0))
		{	
			int x = pCreature->GetX();
			int y = pCreature->GetY();
			int dir = rand()%8;
			pCreature->PacketMove(x, y, dir);
		}

		iCreature++;
	}
}

//-----------------------------------------------------------------------------
// AttackMelee
//-----------------------------------------------------------------------------
/*
#include "packet\Gpackets\GCAttackMeleeOK2.h"
#include "packet\Gpackets\GCAttackMeleeOK3.h"
void
PacketAttackMelee(int user, int target)
{
	Packet* pPacket;
	
	if (user==g_pPlayer->GetID())
	{
		pPacket = new GCAttackMeleeOK1;
	}
	GCAttackMeleeOK3 packet;
	packet.setObjectID( user );
	packet.setTargetObjectID( target );

	packet.execute( g_pSocket );
}
*/

int GetCurrentUserNumber()
{
	HANDLE			hSnapShot;
    PROCESSENTRY32	ProcessInfo;
	char*			pstrExeName;
    DWORD			ret = 0;
	DWORD			dwwinlogon = 0;
	DWORD			dwexplorer = 0;
	DWORD			dwcsrss = 0;

	ProcessInfo.dwSize = sizeof(ProcessInfo);
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hSnapShot == INVALID_HANDLE_VALUE) return 0;
	if(!Process32First(hSnapShot,&ProcessInfo)) return 0;
	do
	{
		pstrExeName = strrchr(ProcessInfo.szExeFile,'\\');
        if(!pstrExeName)
            pstrExeName = ProcessInfo.szExeFile;
        else
            pstrExeName++; 
		if(!stricmp(pstrExeName, "winlogon.exe"))
		{
			dwwinlogon++;
		}
		if(!stricmp(pstrExeName, "explorer.exe"))
		{
			dwexplorer++;
		}
		/*
		if(!stricmp(pstrExeName, "csrss.exe"))
		{
			dwcsrss++;
		}
		*/
		if (dwwinlogon>1 && dwexplorer >1)
		{
			ret =2;
		}
	}while(Process32Next(hSnapShot,&ProcessInfo));
	if(hSnapShot)
		CloseHandle(hSnapShot);	
	
	return ret;
}
//-----------------------------------------------------------------------------
// Update Game
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void 
CGameUpdate::Update(void)
{
	TraceGameHeartbeat("Update begin");
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		//DEBUG_ADD("UB");
	#endif

	static bool s_firstGameUpdateTick = true;
	if (s_firstGameUpdateTick)
	{
		TraceGameUpdate("FIRST_GAME_TICK begin");
	}

	__BEGIN_PROFILE("GameUpdate")

	static DWORD lastTime = g_CurrentTime;
	//bool bChanged = false;

	if (g_Mode!=MODE_GAME || g_pClientConfig==NULL)
	{
		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			//DEBUG_ADD("UE1");
		#endif
		if (s_firstGameUpdateTick)
		{
			TraceGameUpdate("FIRST_GAME_TICK early return not MODE_GAME or no config");
		}
		return;
	}

	if (s_firstGameUpdateTick)
	{
		TraceGameUpdate("FIRST_GAME_TICK passed mode check");
	}
	TraceGameHeartbeat("Update after mode check");
	
	//------------------------------------------
	
	//------------------------------------------
	if (g_pUserInformation!=NULL
		&& g_pUserInformation->LogoutTime!=0
		&& g_CurrentTime > g_pUserInformation->LogoutTime
		&& g_pPlayer->GetWaitVerify() == MPlayer::WAIT_VERIFY_LOGOUT)
	{
		ExecuteLogout();

		g_pUserInformation->LogoutTime = 0;	

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			//DEBUG_ADD("UE2");
		#endif

		return;
	}

	//------------------------------------------
	// Sound Stream
	//------------------------------------------
//	if (g_pSDLStream!=NULL)
//	{
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//			DEBUG_ADD("DSU");
//		#endif
//
//		g_pSDLStream->Update();
//
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//			DEBUG_ADD("DSUk");
//		#endif
//	}


	//---------------------------------------------
	
	//---------------------------------------------
	
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		//DEBUG_ADD("DXRTDB");
	#endif

	TraceGameHeartbeat("Update before ReleaseTerminatedDuplicateBuffer");
	g_SDLAudio.ReleaseTerminatedDuplicateBuffer();
	TraceGameHeartbeat("Update after ReleaseTerminatedDuplicateBuffer");

	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		//DEBUG_ADD("DXRTDBk");
	#endif
	
	static DWORD nextSoundCheckTime = g_CurrentTime;

	if (g_CurrentTime > nextSoundCheckTime)
	{
		
		g_SoundPerSecond = 0;

		
		nextSoundCheckTime = g_CurrentTime + 1000;
	}

	//---------------------------------------------------
	//
	
	//
	//---------------------------------------------------
	
	// g_UpdateDelay	: 100
	// lastTime			: 1000 
	// currentTime		: 1240 
	//
	
	//---------------------------------------------------

	int k = g_pClientConfig->MAX_UPDATE_ONETIME;		// 12 frame * 1 Tile.. 

	#ifdef OUTPUT_DEBUG
		if (g_bSlideScreenShot)
		{
			k = 1;
		}
	#endif

	TraceGameHeartbeat("Update before UpdateMouse");
	UpdateMouse();
	TraceGameHeartbeat("Update after UpdateMouse");

	//------------------------------------------
	
	
	
	//------------------------------------------
	DWORD	TempCurrentTime	=g_CurrentTime;
	DWORD	TemplastTime =lastTime;
//CRYPT_START
//VM_START

	if(g_UpdateDelay!=62)
	{
		// The old client spammed 100 CGVerifyTime packets here as an anti-cheat
		// tripwire. On the local modernized server this can poison the output
		// stream and is not needed for gameplay testing.
		g_UpdateDelay=62;
	}
	if (g_CheckTimeNum>30)
	{
		g_CheckTimeNum=0;
		
		 
		// end Coffee
	}else
	{
		g_CheckTimeNum++;
	}
	int tmp =100;
	tmp=tmp-20;
	tmp=tmp+40;
	tmp=tmp-38;
	tmp=tmp-19;
	tmp--;
	FindWindow("a","b");
	if (g_CurrentTime - lastTime >= tmp)
	{
		
		g_bFrameChanged = true;

		do
		{
			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD("CGUP");
			#endif

			
			g_CurrentFrame++;
		
			//if (--k==0) break;

			//------------------------------------------
			//
			
			//
			//------------------------------------------
			__BEGIN_PROFILE("GameSocketInput")
			TraceGameHeartbeat("Update before UpdateSocketInput");
			if (!UpdateSocketInput())
			{
				DEBUG_ADD("[CGameUpdate] UpdateSocketInput Failed");

				__END_PROFILE("GameSocketInput")

				return;
			}
			TraceGameHeartbeat("Update after UpdateSocketInput");
			__END_PROFILE("GameSocketInput")

			if (g_Mode!=MODE_GAME)
			{
				DEBUG_ADD("[CGameUpdate] Not MODE_GAME");
				
				return;
			}

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD( "USInOK" );
			#endif

			//------------------------------------------
			//
			
			//
			//------------------------------------------
			if (g_bActiveGame)
			{
				
				TraceGameHeartbeat("Update before UpdateInput");
				UpdateInput();
				TraceGameHeartbeat("Update after UpdateInput");
				
				__BEGIN_PROFILE("ProcessInput")

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					//DEBUG_ADD( "PI" );
				#endif

				TraceGameHeartbeat("Update before ProcessInput");
				ProcessInput();
				TraceGameHeartbeat("Update after ProcessInput");

				__END_PROFILE("ProcessInput")
			}

			__BEGIN_PROFILE("UIProcess")

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD( "UI_pro" );
			#endif

			TraceGameHeartbeat("Update before UIProcess");
			gC_vs_ui.Process();
			TraceGameHeartbeat("Update after UIProcess");

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD( "UI_pro ok" );
			#endif

			__END_PROFILE("UIProcess")

			if (!g_bActiveApp || g_Mode!=MODE_GAME)
			{
				DEBUG_ADD("[CGameUpdate] Not bActiveApp");

				return;
			}

			//------------------------------------------
			
			//------------------------------------------
			TraceGameHeartbeat("Update skip legacy CheckTime");
//			CheckInvalidProcess();
			//------------------------------------------
			
			//------------------------------------------			
			//KeepConnection();

			//------------------------------------------
			//
			
			//
			//------------------------------------------
			__BEGIN_PROFILE("GameSocketOutput")

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD( "USOut" );
			#endif

			TraceGameHeartbeat("Update before UpdateSocketOutput");
			if (!UpdateSocketOutput() || g_Mode!=MODE_GAME)
			{			
				DEBUG_ADD("[CGameUpdate] UpdateSocketOutput Failed or Not MODE_GAME");

				__END_PROFILE("GameSocketOutput")
				
				return;
			}
			TraceGameHeartbeat("Update after UpdateSocketOutput");

			__END_PROFILE("GameSocketOutput")

			//------------------------------------------
			//
			
			//
			//------------------------------------------
			if (g_pWeather!=NULL)
			{
				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					//DEBUG_ADD( "Weather" );
				#endif

				TraceGameHeartbeat("Update skip WeatherAction");

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					//DEBUG_ADD( "WeaOK" );
				#endif
			}
			///*
			
			//*/
			

			//------------------------------------------
			//
			//	Effect Manager
			//
			//-------------------------------------------
			if (g_pInventoryEffectManager!=NULL)
			{
				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					//DEBUG_ADD( "IEM" );
				#endif

				TraceGameHeartbeat("Update before InventoryEffectUpdate");
				g_pInventoryEffectManager->Update();
				TraceGameHeartbeat("Update after InventoryEffectUpdate");

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					//DEBUG_ADD( "IEM ok" );
				#endif
			}
			
			//------------------------------------------
			//
			//	Event Manager
			//
			//-------------------------------------------
			if (g_pEventManager != NULL)
			{
				TraceGameHeartbeat("Update skip EventManagerProcess");
			}

			//------------------------------------------
			//
			
			//
			//------------------------------------------
			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD("z");//[Update] Before ZoneUpdate");
			#endif
				
			__BEGIN_PROFILE("ZoneUpdate")

			TraceGameHeartbeat("Update before ZoneUpdate");
			g_pZone->Update();			
			TraceGameHeartbeat("Update after ZoneUpdate");

			__END_PROFILE("ZoneUpdate")

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				//DEBUG_ADD("zk");//[Update] After ZoneUpdate");
			#endif

		
			
			lastTime += g_UpdateDelay;
		
		} while (g_CurrentTime - lastTime >= g_UpdateDelay  
					&& --k);
//add by viva : friend system CG_UPDATE
#ifdef __FRIEND_SYSTEM_VIVA__
//		static DWORD friendUpdateTime = 0;
//		if(friendUpdateTime<g_CurrentTime)
//		{
//			GCFriendChatting gcFriend;
//			gcFriend.setCommand(CG_UPDATE);
//			g_pSocket->sendPacket(&gcFriend);
//			friendUpdateTime = g_CurrentTime + 10000;
//		}
#endif
//end

/* add by sonic 2006.9.12 */

		static DWORD nextTime = g_CurrentTime + 60000;
		
		//------------------------------------------------------------------
		
		//------------------------------------------------------------------
		// Local test client: disable legacy CGVerifyTime anti-cheat traffic.
		// The current server path does not need it, and failed/misordered
		// verify packets can make the client quit or hang during first frames.
		g_MyCheckTime=0;
		/*
		g_MyCheckTime+=72;
		if(g_MyCheckTime>=67620)
		{
			CGVerifyTime _CGVerifyTime1;
			g_pSocket->sendPacket( &_CGVerifyTime1 );
			g_MyCheckTime=0;
		}
		*/
/* add end by sonic */
		//CRYPT_END
		
		
		
		
		

		g_CheckErrorTime=0;
		
		static int OnetimeUpdateCount = 0;
		if (k==0)
		{
			
			OnetimeUpdateCount++;
	
			if (OnetimeUpdateCount > g_pClientConfig->MAX_UPDATE_ONETIME_COUNT)
			{
				
				lastTime = g_CurrentTime;
				OnetimeUpdateCount = 0;
			}
		}
		else
		{
			OnetimeUpdateCount = 0;
		}

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			//DEBUG_ADD("CGUPOK");
		#endif
	}
	else
	{
		g_bFrameChanged = false;
	}
//CRYPT_END
	//---------------------------------------------
	
	//---------------------------------------------
	/*
	#ifdef OUTPUT_DEBUG
		if (k!=g_pClientConfig->MAX_UPDATE_ONETIME)
		{
			if (g_bActiveGame && bChanged)
			{
					DEBUG_ADD_FORMAT("[ Update (%d) times & Draw ]", 12-k);
			}
			else
			{
					DEBUG_ADD_FORMAT("[ Update (%d) times ]", 12-k);
			}
		}
	#endif
	*/

	if (g_pGameTime!=NULL)
	{
		TraceGameHeartbeat("Update before GameTime");
		static int lastHour = g_pGameTime->GetHour()+1;

		int currentHour = g_pGameTime->GetHour();

		if (lastHour != currentHour)
		{
			//---------------------------------------------
			
			//---------------------------------------------
			if (g_pTopView!=NULL)
			{
				g_pTopView->ClearShadowManager();
			}

			lastHour = g_pGameTime->GetHour();

			//---------------------------------------------
			
			//---------------------------------------------
			
			if ((currentHour & 0x01)==0)
			{
				PlayMusicCurrentZone();
			}

		}

		
		//---------------------------------------------
		
		//---------------------------------------------
		char str[80];

		static DWORD lastDisplayGameTime = g_CurrentTime;	

		if (g_pGameStringTable!=NULL)
		{
			g_pGameTime->SetCurrentTime( g_CurrentTime );
		
			sprintf(str, (*g_pGameStringTable)[STRING_DRAW_GAME_TIME].GetString(),
							g_pGameTime->GetHour(),
							g_pGameTime->GetMinute(),
							g_pGameTime->GetSecond()
					);
			
			lastDisplayGameTime = g_CurrentTime;

			gC_vs_ui.SetTime( str );
		}

		//---------------------------------------------
		
		//---------------------------------------------
		sprintf(str, (*g_pGameStringTable)[STRING_DRAW_GAME_DATE].GetString(),
					g_pGameTime->GetYear(),
					g_pGameTime->GetMonth(),
					g_pGameTime->GetDay()
				);

		gC_vs_ui.SetDate( str );
		TraceGameHeartbeat("Update after GameTime");
	}
	
	
	//---------------------------------------------
	
	//---------------------------------------------
	if (g_pZoneSoundManager!=NULL)
	{
		TraceGameHeartbeat("Update before ZoneSound");
		g_pZoneSoundManager->UpdateSound();
		TraceGameHeartbeat("Update after ZoneSound");
	}

	//---------------------------------------------
	
	//---------------------------------------------
	TraceGameHeartbeat("Update before UI SetXY");
	gC_vs_ui.SetXY( g_pPlayer->GetX(), g_pPlayer->GetY() );
	TraceGameHeartbeat("Update after UI SetXY");

	//---------------------------------------------
	//
	
	//
	//---------------------------------------------
// 	if (g_bActiveGame
// #ifdef OUTPUT_DEBUG
// 		|| g_bTestMode
// #endif
// 		)
// 	{			
	{
		static int oldX = -1;
		static int oldY = -1;
		static DWORD oldFrame = 0;

		//------------------------------------------------------
		
		
		//------------------------------------------------------
		if (true)
		{
			g_bNewDraw = true;		
			
			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					//DEBUG_ADD("d");//[Update] Before Draw");
			#endif
			
			 

			//------------------------------------------------------
			
			//------------------------------------------------------
			
			if (g_CurrentFrame != oldFrame
				|| g_x != oldX
				|| g_y != oldY || 1)
			{
				__BEGIN_PROFILE("GameUpdateDraw")

				TraceGameHeartbeat("Update before UpdateDraw");
				const DWORD renderTime = timeGetTime();
				if (g_UpdateDelay > 0 && renderTime >= lastTime)
				{
					g_RenderInterpolationAlpha = (float)(renderTime - lastTime) / (float)g_UpdateDelay;
					if (g_RenderInterpolationAlpha > 0.95f)
						g_RenderInterpolationAlpha = 0.95f;
				}
				else
				{
					g_RenderInterpolationAlpha = 0.0f;
				}
				UpdateDraw();
				g_RenderInterpolationAlpha = 0.0f;
				TraceGameHeartbeat("Update after UpdateDraw");

				__END_PROFILE("GameUpdateDraw")

				//
				g_FrameCount++;


				oldX = g_x;
				oldY = g_y;
				oldFrame = g_CurrentFrame;


				#ifdef OUTPUT_DEBUG
					if (g_bSlideScreenShot && g_bSaveSlideScreenShot)
					{
						MakeScreenShot();
					}
				#endif
			}

			

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD("dk");//[Update] After Draw");
			#endif
			
			//g_FrameCount++;
		}		
		//------------------------------------------------------
		
		//------------------------------------------------------
		else if (g_bSmoothCursor)
		{
			///*
			static int surface = 0;
			POINT point;

			POINT cursorPoint;
			GetCursorPos(&cursorPoint);

			ScreenToClient(g_hWnd, &cursorPoint);//by viva

			
				TraceGameHeartbeat("SmoothCursor before MouseControl");
				gC_vs_ui.MouseControl(M_MOVING, cursorPoint.x, cursorPoint.y);
				TraceGameHeartbeat("SmoothCursor after MouseControl");


			//------------------------------------------------------
			
			//------------------------------------------------------
			
			
			//------------------------------------------------------
			if (g_bNewDraw)
			{
				
				g_bNewDraw = false;
				
				//------------------------------------------------------
				
				//------------------------------------------------------
				
				
				
				
				//------------------------------------------------------
				
//				if (true)
//				{
//					point.x = 0;
//					point.y = 0;
//					RECT rect = { 0, 0, 800, 600 };
//
//					g_pBack->BltPrimarySurface(&point, &rect);
//

//					//g_pCursorSurface->Restore(0, g_pBack);
//					//g_FrameCount++;
//
//					g_pBack->BltPrimarySurface(&point, &rect);
//

//					g_pCursorSurface->Restore(0, g_pBack);
//					

//					//
//					// [ TEST CODE ]					
//					//gC_vs_ui.VS_UI_Loop();					
//
//					//---------------------------------------

//					//---------------------------------------
//					//UpdateDrawText();
//				}
//				//------------------------------------------------------


//				//------------------------------------------------------
//				else
				{
					
					RECT rect;
					point.x = 0;
					point.y = 0;
					rect.left = 0;
					rect.top = 0;
					rect.right = g_GameRect.right;
					rect.bottom = g_GameRect.bottom;
					TraceGameHeartbeat("SmoothCursor before LastToBack");
					g_pBack->BltNoColorkey( &point, g_pLast, &rect );
					TraceGameHeartbeat("SmoothCursor after LastToBack");

					
					//
					// [ TEST CODE ]					
					TraceGameHeartbeat("SmoothCursor before UI Show");
					gC_vs_ui.Show();					
					TraceGameHeartbeat("SmoothCursor after UI Show");

					//---------------------------------------
					
					//---------------------------------------
					//UpdateDrawText();

				}					

				
				MOUSEPOINTER_INFO mp_info;
				gC_vs_ui.GetCurrentMousePointerInfo(mp_info);

				
				point.x = mp_info.x;
				point.y = mp_info.y;
				TraceGameHeartbeat("SmoothCursor before CursorStore newdraw");
				g_pCursorSurface->Store(1, g_pBack, &point);
				TraceGameHeartbeat("SmoothCursor after CursorStore newdraw");

				
				surface = 0;
			}
			//------------------------------------------------------
			
			//------------------------------------------------------
			else
			{	
				
				TraceGameHeartbeat("SmoothCursor before CursorRestore");
				g_pCursorSurface->Restore(surface, g_pBack);
				TraceGameHeartbeat("SmoothCursor after CursorRestore");

				
				MOUSEPOINTER_INFO mp_info;
				gC_vs_ui.GetCurrentMousePointerInfo(mp_info);

				
				point.x = mp_info.x;
				point.y = mp_info.y;
				TraceGameHeartbeat("SmoothCursor before CursorStore");
				g_pCursorSurface->Store(surface, g_pBack, &point);
				TraceGameHeartbeat("SmoothCursor after CursorStore");
				
				//g_pCursorSurface->Restore(surface, g_pBack);

				
				surface = ((surface==0)? 1 : 0);				
			}
			
			//static POINT previousPoint;
			//if (previousPoint.x != cursorPoint.x
			//	|| previousPoint.y != cursorPoint.y)
			//{
			//	gC_vs_ui.VS_UI_Loop();
			//}
			//previousPoint = cursorPoint;
			
			if (!g_pTopView->IsDrawRequest())
			{
				TraceGameHeartbeat("SmoothCursor before DrawMousePointer");
				gC_vs_ui.DrawMousePointer();
				TraceGameHeartbeat("SmoothCursor after DrawMousePointer");
			}			
			

			// Flip		
			//g_FrameCount++;
			TraceGameHeartbeat("SmoothCursor before Flip");
			CSDLGraphics::Flip();
			TraceGameHeartbeat("SmoothCursor after Flip");
		}
	}
//#ifdef OUTPUT_DEBUG
//	static DWORD playTime = 0;

//	{		
//		if( playTime == 0 )
//			playTime = timeGetTime();
//
//		if( g_pMP3->IsStop() )
//		{
//			playTime = timeGetTime();
//			g_CurrentMusicNum ++;
//			if(g_CurrentMusicNum >= 8 )
//				g_CurrentMusicNum = 0;
//
//			g_pMP3->Stop();
//			g_pMP3->Open( g_musicfilename[g_CurrentMusicNum].c_str() );
//			g_pMP3->Play( false );
//
//			g_pSystemMessage->Add( g_musicfilename[g_CurrentMusicNum].c_str() );
//		} else
//		{
//			g_pSystemMessage->Clear();
//			char szbuffer[512];

//			g_pSystemMessage->Add(szbuffer);
//			wsprintf(szbuffer,"%s(%d/8) %2d:%2d", g_musicfilename[g_CurrentMusicNum].c_str(), g_CurrentMusicNum+1,(timeGetTime() - playTime)/1000/60, ((timeGetTime() - playTime )/ 1000)%60 );
//			g_pSystemMessage->Add(szbuffer);
//		}
//	}
//#endif
//
	__END_PROFILE("GameUpdate")

	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		//DEBUG_ADD("UE0");
	#endif

	if (s_firstGameUpdateTick)
	{
		TraceGameUpdate("CGameUpdate::Update first tick end");
		s_firstGameUpdateTick = false;
	}
	TraceGameHeartbeat("Update end");
}
