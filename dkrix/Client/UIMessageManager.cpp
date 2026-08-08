//-----------------------------------------------------------------------------
// UIMessageManager.cpp
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#ifdef PLATFORM_WINDOWS
#include <io.h>
#include <process.h>
#endif
#include <stdio.h>
#include "Client.h"
#include "UIMessageManager.h"
#include "UIFunction.h"
#include "VS_UI.h" 
#include "VS_UI_mouse_pointer.h"
#include "TalkBox.h"
#include "MShopShelf.h"
#include "MPriceManager.h"
#include "MMoneyManager.h"
#include "UIDialog.h"
#include "SkillDef.h"
#include "ClientFunction.h"
#include "MGameStringTable.h"
#include "MItemOptionTable.h"
#include "CToken.h"
#include "UserOption.h"
#include "MWeather.h"
#include "MStatusManager.h"
#include "MChatManager.h"
#include "PacketDef.h"
#include "UtilityFunction.h"
#include "ClientConfig.h"
#include "MMusic.h"
#include "MMusicTable.h"
#include "SoundDef.h"
#include "ServerInformation.h"
#include "MParty.h"
#include "MItemFinder.h"
#include "ServerInfo.h"
#include "MZoneTable.h"
//#include "MFileDef.h"
#include "Packet/Properties.h"
#include "UserInformation.h"
#include "TempInformation.h"
#include "ProfileManager.h"
#include "CGameUpdate.h"
#include "MTestDef.h"

#include "Packet/Cpackets/CGRequestNewbieItem.h"
#include "CGSilverCoating.h"
#include "Packet/Cpackets/CLChangeServer.h"
#include "Packet/Cpackets/CLGetServerList.h"
#include "Packet/Cpackets/CGSelectPortal.h"
#include "Packet/Cpackets/CGSelectWayPoint.h"
#include "Packet/Cpackets/CGPartyInvite.h"
#include "CGPartyLeave.h"
#include "CGResurrect.h"
#include "Packet/Cpackets/CLSelectWorld.h"
#include "Packet/Cpackets/CLSelectServer.h"
#include "Packet/Cpackets/CLQueryCharacterName.h"
#include "Packet/Cpackets/CGRelicToObject.h"
#include "CGSelectGuild.h"
#include "CGSelectGuildMember.h"
#include "CGRequestGuildMemberList.h"
#include "Packet/Cpackets/CGTryJoinGuild.h"
#include "Packet/Cpackets/CGJoinGuild.h"
#include "Packet/Cpackets/CGRegistGuild.h"
#include "CGModifyGuildMember.h"
#include "CGAddItemToItem.h"
#include "CGModifyGuildIntro.h"
#include "CGModifyGuildMemberIntro.h"
#include "Packet/Cpackets/CGSkillToNamed.h"
#include "Packet/Cpackets/CGSelectRankBonus.h"
#include "Packet/Cpackets/CGWithdrawTax.h"
#include "Packet/Cpackets/CGTypeStringList.h"
#include "CGLotterySelect.h"
#include "Packet/Cpackets/CGTakeOutGood.h"
#include "Packet/Cpackets/CGMixItem.h"
#include "CGDownSkill.h"
#include "Packet/Gpackets/GCMiniGameScores.h"
#include "Packet/Cpackets/CGSubmitScore.h"
#include "CGAddItemToCodeSheet.h"
#include "CGFailQuest.h"
#include "CGSelectRegenZone.h"
#include "Packet/Cpackets/CGTameMonster.h"
#include "Packet/Cpackets/CGPetGamble.h"
#include "Packet/Cpackets/CGUseMessageItemFromInventory.h"
#include "Packet/Cpackets/CGPartySay.h"
#include "Packet/Cpackets/CGDepositPet.h"
#include "CGWithdrawPet.h"
#include "Packet/Cpackets/CGSMSSend.h"
#include "CGSMSAddressList.h"
#include "Packet/Cpackets/CGDeleteSMSAddress.h"
#include "Packet/Cpackets/CGAddSMSAddress.h"
#include "CGSelectNickname.h"
#include "Packet/Cpackets/CGModifyNickname.h"
#include "CGGQuestAccept.h"
#include "CGGQuestCancel.h"
#include "CGUseItemFromGQuestInventory.h"

#include "RequestFunction.h"
#include "Packet/RequestServerPlayerManager.h"
#include "RequestUserManager.h"
#include "Packet/ClientCommunicationManager.h"
#include "WhisperManager.h"
#include "Packet/Rpackets/RCSay.h"
#include "Packet/Cpackets/CGGuildChat.h"
#include "CMP3.h"
#include "RankBonusTable.h"
#include "Profiler.h"

#include "DebugInfo.h"
#include "MEventManager.h"
#include "MOustersGear.h"
#include "SoundSetting.h"
#include "ShrineInfoManager.h"

#include "MTimeItemManager.h"
#include "MFakeCreature.h"

#include "CGRequestUnion.h"
#include "CGQuitUnion.h"
#include "CGExpelGuild.h"
#include "CGRequestGuildList.h"
#include "Packet/Cpackets/CGRequestUnionInfo.h"
#include "CGAcceptUnion.h"
#include "CGDenyUnion.h"
#include "CGQuitUnionAccept.h"
#include "CGQuitUnionDeny.h"
#include "Packet/Cpackets/CGRangerSay.h"
#include "CGModifyTaxRatio.h"
#include "CGAppointSubmaster.h"
//
#include "CGRequestUnion.h"
#include "CGQuitUnion.h"
#include "CGExpelGuild.h"
#include "CGRequestGuildList.h"
#include "Packet/Cpackets/CGRequestUnionInfo.h"
#include "CGAcceptUnion.h"
#include "CGDenyUnion.h"
#include "CGQuitUnionAccept.h"
#include "CGQuitUnionDeny.h"
#include "Packet/Cpackets/CGRangerSay.h"
#include "CGModifyTaxRatio.h"
#include "CGAppointSubmaster.h"

#include "CGDisplayItem.h"
#include "CGUndisplayItem.h"
#include "CGStoreOpen.h"
#include "CGStoreClose.h"
#include "CGStoreSign.h"
#include "CGRequestStoreInfo.h"
#include "Packet/Gpackets/GCMyStoreInfo.h"
#include "Packet/Gpackets/GCOtherStoreInfo.h"
#include "CGBuyStoreItem.h"
#include "Packet/Gpackets/GCRemoveStoreItem.h"
#include "Packet/Gpackets/GCAddStoreItem.h"
//

#include "Packet/Cpackets/CGUsePowerPoint.h"
#include "Packet/Cpackets/CGRequestPowerPoint.h"
#include "Packet/Cpackets/CGDonationMoney.h"
#include "Packet/Cpackets/CGGetEventItem.h"
#include "Packet/Cpackets/CGUseMessageItemFromInventory.h"

//add by viva
#include "Packet/Cpackets/CGConnectSetKey.h"
#include "Packet/Gpackets/GCFriendChatting.h"
//end

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#define	SYMBOL_GLOBALCHAT			'!'			
#define	SYMBOL_WHISPER				'/'			
#define	SYMBOL_COMMAND				'@'			
#define SYMBOL_MASTER_COMMAND		'*'			

#define	STRING_IGNORE				""		
#define	STRING_IGNORE_ENG			"ignore"	
#define	STRING_ACCEPT				""		
#define	STRING_ACCEPT_ENG			"accept"	
#define	STRING_ACCEPT_CURSE			"?"	
#define	STRING_ACCEPT_CURSE_ENG		"what?"		
#define	STRING_FILTER_CURSE			""	
#define	STRING_FILTER_CURSE_ENG		"filter"	

int g_C2G = 0;

//-----------------------------------------------------------------------------
// extern
//-----------------------------------------------------------------------------
#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
	extern int g_ShowImageObjectID;
#endif

#ifdef OUTPUT_DEBUG
	extern MString g_ProfileInfoName;
#endif

bool							g_bEventQuestEnding = false;
extern bool		LoadingAddonSPK(bool bLoading);
extern CGameUpdate*		g_pCGameUpdate;
extern BOOL g_MyFull;
extern RECT g_GameRect;
//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
UIMessageManager*	g_pUIMessageManager = NULL;

extern BOOL g_bEnable3DHAL;
extern BOOL g_bSetHotKey;
extern bool g_bWatchMode;
extern bool	g_bTestMode;
extern bool g_bHolyLand;
extern bool g_bZoneSafe;
extern DWORD g_dwSeqNumL;
extern DWORD g_dwSeqNumG;

//-----------------------------------------------------------------------------
// Execute Logout
//-----------------------------------------------------------------------------
void
ExecuteLogout()
{
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (g_Mode==MODE_GAME)
	{
		DEBUG_ADD("ExecuteLogout");

		UI_SaveUserOption();

		
		SetWeather(WEATHER_CLEAR, 0);

		
		
		if (!g_pPlayer->IsAlive())
		{
				CGResurrect _CGResurrect;
				
				g_pSocket->sendPacket( &_CGResurrect );				

			gC_vs_ui.FinishRequestDie();
			gC_vs_ui.FinishRequestResurrect();
			gC_vs_ui.CloseRequestShrineMinimap();
		}

		if (1)//g_pPlayer->IsAlive())
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			if (g_pUserOption->PlayWaveMusic)
			{
//				g_pSDLStream->Stop();
				DEBUG_ADD("MP3 STOP7");
#ifdef __USE_MP3__
				g_pMP3->Stop();
#else
				if( g_SDLAudio.IsInit() )
					g_pOGG->streamClose();
#endif
				DEBUG_ADD("MP3 STOP7 OK");
			}
			else
			{
				g_Music.Stop();
			}

			//--------------------------------------------------
			
			//--------------------------------------------------
			StopLoadingThread();

			//
			
			//
			//--------------------------------------------------
			
			//--------------------------------------------------
				UI_SaveHotKeyToServer();

				CGLogout cgLogout;
			
				g_pSocket->sendPacket( &cgLogout );
				//g_pSocket->setPlayerStatus( CPS_END_SESSION );

				g_pSocket->processOutput();

				g_pSocket->setPlayerStatus( CPS_WAITING_FOR_GC_RECONNECT_LOGIN );	

				
				//g_pSocket->disconnect();

				
				//ReleaseSocket();

			 

			
			/*
			if (g_pCGameUpdate!=NULL)
			{
				gC_vs_ui.CloseGameMenu();

				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_WAIT_FOR_CHARACTER_SELECT_MODE].GetString(), -1, -1, 0 );			
				
				g_pCGameUpdate->UpdateDraw();				
				CSDLGraphics::Flip();

				g_pCGameUpdate->UpdateDraw();				
				CSDLGraphics::Flip();
			}
			*/

			
			//SetWeather(WEATHER_CLEAR, 0);
			ReleaseGameObject();

			
			//SetMode( MODE_MAINMENU );	
			SetMode( MODE_WAIT_RECONNECT_LOGIN );

			//------------------------------------------------------
			// [ TEST CODE ]
			
			//------------------------------------------------------
			//InitSound();
			g_CurrentFrame		= 0;

			g_bSetHotKey	= FALSE;

			//------------------------------------------------------
			// ui dialog
			//------------------------------------------------------
			g_pUIDialog->HidePCTalkDlg();
			g_pUIDialog->CloseMessageDlg();


			//------------------------------------------------------
			
			//------------------------------------------------------
			g_pChatManager->ClearID();
			g_pChatManager->SetAcceptMode();

//			gC_vs_ui.ServerDisconnectMessage();
//			gC_vs_ui.CloseAllDialog();

			
			//gC_vs_ui.EndTitle();
			gC_vs_ui.StartTitle();	
		
			gC_vs_ui.DropItem();

			g_bUIInput = FALSE;

			
			g_pUserInformation->WhisperID.Release();

			//----------------------------------------------
			
			//----------------------------------------------
			g_pSystemMessage->Clear();
			g_pGameMessage->Clear();
			g_pNoticeMessage->Clear();

			//------------------------------------------------------
			
			//------------------------------------------------------
			if (g_pUserOption->PlayWaveMusic)
			{
//				g_pSDLStream->Stop();			
				DEBUG_ADD("MP3 STOP8");
#ifdef __USE_MP3__
				g_pMP3->Stop();
#else
				if( g_SDLAudio.IsInit() )
					g_pOGG->streamClose();
#endif
				DEBUG_ADD("MP3 STOP8 OK");

				if (g_pUserOption->PlayMusic)
				{
					int musicID = g_pClientConfig->MUSIC_THEME;

					if (musicID!=MUSICID_NULL)
					{
#ifdef __USE_MP3__					
//						g_pSDLStream->Load( (*g_pMusicTable)[ musicID ].FilenameWav );
//						g_pSDLStream->Play( FALSE );
						DEBUG_ADD("MP3 OPEN2");
						g_pMP3->Open( (*g_pMusicTable)[ musicID ].FilenameWav );
						DEBUG_ADD("MP3 OPEN2 OK");
						DEBUG_ADD("MP3 PLAY4");
						g_pMP3->Play( false );
						DEBUG_ADD("MP3 PLAY4 OK");
#else
						if( g_oggfile != NULL )
							fclose(g_oggfile);
						
						g_oggfile = NULL;

						if( g_SDLAudio.IsInit() )
						{
							g_oggfile = fopen( (*g_pMusicTable)[ musicID ].FilenameWav ,"rb");
							if( g_oggfile != NULL )
							{
								g_pOGG->streamLoad( g_oggfile, NULL );
								g_pOGG->streamPlay( 0 );
								int volume = (g_pUserOption->VolumeMusic - 15) * 250;
								g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
							}
						}
#endif
					}
				}
			}
			else
			{
				g_Music.Stop();			

				if (g_pUserOption->PlayMusic)
				{
					int musicID = g_pClientConfig->MUSIC_THEME;

					if (musicID!=MUSICID_NULL)
					{
						g_Music.Play( (*g_pMusicTable)[ musicID ].Filename );
					}
				}
			}
		}
		else
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );
		}
	}
	else
	{
		DEBUG_ADD("Logout Not ModeGame");
	}
}

//-----------------------------------------------------------------------------
// Play TitleMusic
//-----------------------------------------------------------------------------
void
PlayTitleMusic()
{
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	if (g_pUserOption->PlayWaveMusic)
	{
		g_Music.Stop();

		if (g_pUserOption->PlayMusic)
		{
			int musicID = g_pClientConfig->MUSIC_THEME;

			if (musicID!=MUSICID_NULL)
			{
#ifdef __USE_MP3__
				LONG volume = g_pUserOption->VolumeMusic*16*257;//*SOUND_DEGREE + SOUND_MIN;
//				g_pSDLStream->SetVolumeLimit( volume );
				DEBUG_ADD("MP3 SetVolume2");
				g_pMP3->SetVolume( volume );
				DEBUG_ADD("MP3 SetVolume2 OK");

				DEBUG_ADD("MP3 OPEN3");
				g_pMP3->Open( (*g_pMusicTable)[ musicID ].FilenameWav );
				DEBUG_ADD("MP3 OPEN3 OK");
				DEBUG_ADD("MP3 PLAY5");
				g_pMP3->Play( false );				
				DEBUG_ADD("MP3 PLAY5 OK");
//				g_pSDLStream->Load( (*g_pMusicTable)[ musicID ].FilenameWav );
//				g_pSDLStream->Play( FALSE );				
#else
				if( g_oggfile != NULL )
					fclose(g_oggfile);

				g_oggfile = NULL;

				if( g_SDLAudio.IsInit() )
				{
					g_oggfile = fopen( (*g_pMusicTable)[ musicID ].FilenameWav, "rb") ;
					if( g_oggfile != NULL )
					{					
						int volume = (g_pUserOption->VolumeMusic - 15) * 250;

						g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
						g_pOGG->streamLoad( g_oggfile,NULL );
						g_pOGG->streamPlay( 0 );					
					}
				}
#endif
			}									
		}
		else
		{
//			g_pSDLStream->Stop();
			DEBUG_ADD("MP3 STOP9");
#ifdef __USE_MP3__
			g_pMP3->Stop();
#else
			if( g_SDLAudio.IsInit() )
				g_pOGG->streamClose();
#endif
			DEBUG_ADD("MP3 STOP9 OK");
		}
	}
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	else
	{
//		g_pSDLStream->Stop();
		DEBUG_ADD("MP3 STOP10");
#ifdef __USE_MP3__
		g_pMP3->Stop();
#else
		if( g_SDLAudio.IsInit() )
			g_pOGG->streamClose();
#endif
		DEBUG_ADD("MP3 STOP10 OK");

		if (g_pUserOption->PlayMusic)
		{
			if (g_Music.IsPlay())
			{
				g_Music.Resume();
			}
			else //if (g_Mode==MODE_GAME)
			{	
				WORD volume = g_pUserOption->VolumeMusic*0x1000 + 0x0FFF;

				if (g_Music.IsPause())
				{
					g_Music.SetVolume( volume );					
					g_Music.Resume();
				}
				else
				{
					g_Music.Pause();
					g_Music.SetVolume( volume );
					g_Music.Resume();					

					int musicID = g_pClientConfig->MUSIC_THEME;

					if (musicID!=MUSICID_NULL)
					{
						g_Music.Play( (*g_pMusicTable)[ musicID ].Filename );
					}
				}					
			}				
		}
		else
		{
			if (g_Music.IsPlay())
			{
				g_Music.Pause();
			}
			else
			{
				g_Music.Stop();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// PlayGameMusic
//-----------------------------------------------------------------------------
void
PlayGameMusic()
{
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	if (g_pUserOption->PlayWaveMusic)
	{
		g_Music.Stop();

		if (g_pUserOption->PlayMusic)
		{
#ifdef __USE_MP3__
			LONG volume = g_pUserOption->VolumeMusic*16*257;//*SOUND_DEGREE + SOUND_MIN;
//			g_pSDLStream->SetVolumeLimit( volume );
			DEBUG_ADD("MP3 SetVolume3");
			g_pMP3->SetVolume( volume );
			DEBUG_ADD("MP3 SetVolume3 OK");
#else
			if( g_SDLAudio.IsInit() )
			{
				int volume = (g_pUserOption->VolumeMusic - 15) * 250;

				g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
			}
#endif

			PlayMusicCurrentZone();				
		}
		else
		{					
//			g_pSDLStream->Stop();				
			DEBUG_ADD("MP3 STOP11");
#ifdef __USE_MP3__
			g_pMP3->Stop();
#else
			if( g_SDLAudio.IsInit() )
				g_pOGG->streamClose();
#endif
			DEBUG_ADD("MP3 STOP11 OK");
		}
	}
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	else
	{
//		g_pSDLStream->Stop();
		DEBUG_ADD("MP3 STOP12");
#ifdef __USE_MP3__
		g_pMP3->Stop();
#else
		if( g_SDLAudio.IsInit() )
			g_pOGG->streamClose();
#endif
		
		DEBUG_ADD("MP3 STOP12 OK");

		if (g_pUserOption->PlayMusic)
		{
			DEBUG_ADD("MP3 STOP12 OK2");
			if (g_Music.IsPlay())
			{
				DEBUG_ADD("MP3 STOP12 RESUME");
				g_Music.Resume();
				DEBUG_ADD("MP3 STOP12 RESUME OK");
			}
			else //if (g_Mode==MODE_GAME)
			{				
				DEBUG_ADD("MP3 STOP12 pp");
				WORD volume = g_pUserOption->VolumeMusic*0x1000 + 0x0FFF;
				DEBUG_ADD("MP3 STOP12 pp1");
				g_Music.SetVolume( volume );
				DEBUG_ADD("MP3 STOP12 pp2");
				
				PlayMusicCurrentZone();
				DEBUG_ADD("MP3 STOP12 PlayMusicCurrentZone OK");
			}				
		}
		else
		{
			if (g_Music.IsPlay())
			{
				g_Music.Pause();
			}
			else
			{
				g_Music.Stop();
			}
		}
	}
	DEBUG_ADD("PlayGameMusic OK");
}

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
UIMessageManager::UIMessageManager()
{
	for (int i=0; i<MAX_UI_MESSAGE; i++)
	{
		m_UIMessageFunction[i] = NULL;
	}
}

UIMessageManager::~UIMessageManager()
{
	
	for (int i=0; i<MAX_UI_MESSAGE; i++)
	{
		m_UIMessageFunction[i] = NULL;
	}
}

//-----------------------------------------------------------------------------
//
// member functions
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void
UIMessageManager::Init()
{	
	m_UIMessageFunction[UI_STORAGE_BUY] = Execute_UI_STORAGE_BUY;
	m_UIMessageFunction[UI_SELECT_STORAGE_SLOT] = Execute_UI_SELECT_STORAGE_SLOT;
	m_UIMessageFunction[UI_RUNNING_GAMEMENU] = Execute_UI_RUNNING_GAMEMENU;
	m_UIMessageFunction[UI_FINISH_LEVELUP_BUTTON] = Execute_UI_FINISH_LEVELUP_BUTTON;
	m_UIMessageFunction[UI_CLICK_BONUS_POINT] = Execute_UI_CLICK_BONUS_POINT;
	m_UIMessageFunction[UI_INFO] = Execute_UI_INFO;
//	m_UIMessageFunction[UI_MINIMAP_TOGGLE] = Execute_UI_MINIMAP_TOGGLE;
	m_UIMessageFunction[UI_DROP_MONEY] = Execute_UI_DROP_MONEY;
	m_UIMessageFunction[UI_EXCHANGE_REQUEST_CANCEL] = Execute_UI_EXCHANGE_REQUEST_CANCEL;
	m_UIMessageFunction[UI_EXCHANGE_ACCEPT] = Execute_UI_EXCHANGE_ACCEPT;
	m_UIMessageFunction[UI_EXCHANGE_MONEY] = Execute_UI_EXCHANGE_MONEY;
	m_UIMessageFunction[UI_WITHDRAW_MONEY] = Execute_UI_WITHDRAW_MONEY;
	m_UIMessageFunction[UI_ITEM_SELECT_EXCHANGE] = Execute_UI_ITEM_SELECT_EXCHANGE;
	m_UIMessageFunction[UI_DEPOSIT_MONEY] = Execute_UI_DEPOSIT_MONEY;
	m_UIMessageFunction[UI_CHECK_EXIST_ID] = Execute_UI_CHECK_EXIST_ID;
	m_UIMessageFunction[UI_RUN_NEWUSER_REGISTRATION] = Execute_UI_RUN_NEWUSER_REGISTRATION;
	m_UIMessageFunction[UI_INFO_CLOSE] = Execute_UI_INFO_CLOSE;
	m_UIMessageFunction[UI_GAMEMENU_CONTINUE] = Execute_UI_GAMEMENU_CONTINUE;

	//
	// PDS
	//


	


//	m_UIMessageFunction[UI_SEND_PCS_NUMBER] = Execute_UI_SEND_PCS_NUMBER;





//	m_UIMessageFunction[UI_CLOSE_SKILL_VIEW] = Execute_UI_CLOSE_SKILL_VIEW;

	m_UIMessageFunction[UI_BACKGROUND_MOUSE_FOCUS] = Execute_UI_BACKGROUND_MOUSE_FOCUS;			
	m_UIMessageFunction[UI_REMOVE_BACKGROUND_MOUSE_FOCUS] = Execute_UI_REMOVE_BACKGROUND_MOUSE_FOCUS;

	//
	
	//
	m_UIMessageFunction[UI_BUY_ITEM] = Execute_UI_BUY_ITEM;

	m_UIMessageFunction[UI_SELL_ITEM] = Execute_UI_SELL_ITEM;
	m_UIMessageFunction[UI_SELL_ALL_ITEM] = Execute_UI_SELL_ALL_ITEM;
	m_UIMessageFunction[UI_REPAIR_ITEM] = Execute_UI_REPAIR_ITEM;
	m_UIMessageFunction[UI_SILVERING_ITEM] = Execute_UI_SILVERING_ITEM;
	m_UIMessageFunction[UI_ITEM_SELL_FINISHED] = Execute_UI_ITEM_SELL_FINISHED;
	m_UIMessageFunction[UI_ITEM_REPAIR_FINISHED] = Execute_UI_ITEM_REPAIR_FINISHED;
	m_UIMessageFunction[UI_ITEM_SILVERING_FINISHED] = Execute_UI_ITEM_SILVERING_FINISHED;

	m_UIMessageFunction[UI_CLOSE_SHOP] = Execute_UI_CLOSE_SHOP;
	m_UIMessageFunction[UI_CLOSE_STORAGE] = Execute_UI_CLOSE_STORAGE;
	m_UIMessageFunction[UI_CLOSE_EXCHANGE] = Execute_UI_CLOSE_EXCHANGE;
	m_UIMessageFunction[UI_CANCEL_EXCHANGE] = Execute_UI_CANCEL_EXCHANGE;
	m_UIMessageFunction[UI_OK_EXCHANGE] = Execute_UI_OK_EXCHANGE;

	m_UIMessageFunction[UI_CHARACTER_MANAGER_FINISHED] = Execute_UI_CHARACTER_MANAGER_FINISHED;	
	m_UIMessageFunction[UI_TERMINATION] = Execute_UI_TERMINATION;						
	m_UIMessageFunction[UI_LOGIN] = Execute_UI_LOGIN;								
	m_UIMessageFunction[UI_LOGOUT] = Execute_UI_LOGOUT;								
	
	m_UIMessageFunction[UI_CONNECT] = Execute_UI_CONNECT;							   // Game connection.
	m_UIMessageFunction[UI_CHAT_RETURN] = Execute_UI_CHAT_RETURN;						

	m_UIMessageFunction[UI_SELECT_SKILL] = Execute_UI_SELECT_SKILL;						// Skill selection.
	m_UIMessageFunction[UI_CANCEL_SELECT_SKILL] = Execute_UI_CANCEL_SELECT_SKILL;				

	//
	// Item message
	//

	// drop
	m_UIMessageFunction[UI_ITEM_DROP_TO_CLIENT] = Execute_UI_ITEM_DROP_TO_CLIENT;				
	m_UIMessageFunction[UI_ITEM_DROP_TO_INVENTORY] = Execute_UI_ITEM_DROP_TO_INVENTORY; 		
	
	m_UIMessageFunction[UI_ITEM_DROP_TO_GEAR] = Execute_UI_ITEM_DROP_TO_GEAR;				
	m_UIMessageFunction[UI_ITEM_DROP_TO_QUICKSLOT] = Execute_UI_ITEM_DROP_TO_QUICKSLOT;			

	// pick up
	
	//m_UIMessageFunction[UI_ITEM_PICKUP_FROM_TRADEGRID] = Execute_UI_ITEM_PICKUP_FROM_TRADEGRID;
	m_UIMessageFunction[UI_ITEM_PICKUP_FROM_INVENTORY] = Execute_UI_ITEM_PICKUP_FROM_INVENTORY;
	m_UIMessageFunction[UI_ITEM_PICKUP_FROM_GEAR] = Execute_UI_ITEM_PICKUP_FROM_GEAR;
	m_UIMessageFunction[UI_ITEM_PICKUP_FROM_QUICKSLOT] = Execute_UI_ITEM_PICKUP_FROM_QUICKSLOT;

	// insert to other Item
	//m_UIMessageFunction[UI_ITEM_INSERT_FROM_TRADEGRID] = Execute_UI_ITEM_INSERT_FROM_TRADEGRID;
	m_UIMessageFunction[UI_ITEM_INSERT_FROM_INVENTORY] = Execute_UI_ITEM_INSERT_FROM_INVENTORY;
	m_UIMessageFunction[UI_ITEM_INSERT_FROM_GEAR] = Execute_UI_ITEM_INSERT_FROM_GEAR;
	m_UIMessageFunction[UI_ITEM_INSERT_FROM_QUICKSLOT] = Execute_UI_ITEM_INSERT_FROM_QUICKSLOT;  

	// use
	m_UIMessageFunction[UI_ITEM_USE] = Execute_UI_ITEM_USE;							
	m_UIMessageFunction[UI_ITEM_USE_QUICKSLOT] = Execute_UI_ITEM_USE_QUICKSLOT;				

	//
	// Character management
	//
	m_UIMessageFunction[UI_NEW_CHARACTER] = Execute_UI_NEW_CHARACTER;
	m_UIMessageFunction[UI_DELETE_CHARACTER] = Execute_UI_DELETE_CHARACTER;

	//m_UIMessageFunction[UI_CLOSE_BBS] = Execute_UI_CLOSE_BBS;

//	m_UIMessageFunction[UI_CHANGE_GAME_OPTION] = Execute_UI_CHANGE_GAME_OPTION;
//	m_UIMessageFunction[UI_CLOSE_GAME_OPTION] = Execute_UI_CLOSE_GAME_OPTION;
	m_UIMessageFunction[UI_CHANGE_OPTION] = Execute_UI_CHANGE_OPTION;
	m_UIMessageFunction[UI_CLOSE_OPTION] = Execute_UI_CLOSE_OPTION;
	m_UIMessageFunction[UI_CLOSE_BOOKCASE] = Execute_UI_CLOSE_BOOKCASE;
	m_UIMessageFunction[UI_CLOSE_BRIEFING] = Execute_UI_CLOSE_BRIEFING;
	m_UIMessageFunction[UI_CLOSE_COMPUTER] = Execute_UI_CLOSE_COMPUTER;
	m_UIMessageFunction[UI_CLOSE_TUTORIAL_EXIT] = Execute_UI_CLOSE_TUTORIAL_EXIT;

	m_UIMessageFunction[UI_CLOSE_DESC_DIALOG] = Execute_UI_CLOSE_DESC_DIALOG;
	m_UIMessageFunction[UI_CLOSE_ELEVATOR] = Execute_UI_CLOSE_ELEVATOR;
	m_UIMessageFunction[UI_SELECT_ELEVATOR] = Execute_UI_SELECT_ELEVATOR;

	//m_UIMessageFunction[UI_SELECT_SERVER] = Execute_UI_SELECT_SERVER;

	//m_UIMessageFunction[UI_REQUEST_SERVER_LIST] = Execute_UI_REQUEST_SERVER_LIST;

	m_UIMessageFunction[UI_ITEM_TO_QUICKITEMSLOT] = Execute_UI_ITEM_TO_QUICKITEMSLOT;

	m_UIMessageFunction[UI_CLOSE_SLAYER_PORTAL] = Execute_UI_CLOSE_SLAYER_PORTAL;
	m_UIMessageFunction[UI_SLAYER_PORTAL] = Execute_UI_SLAYER_PORTAL;

	m_UIMessageFunction[UI_FINISH_REQUEST_PARTY_BUTTON] = Execute_UI_FINISH_REQUEST_PARTY_BUTTON;
	m_UIMessageFunction[UI_PARTY_REQUEST_CANCEL] = Execute_UI_PARTY_REQUEST_CANCEL;
	m_UIMessageFunction[UI_PARTY_ACCEPT] = Execute_UI_PARTY_ACCEPT;

	m_UIMessageFunction[UI_CLOSE_PARTY_MANAGER] = Execute_UI_CLOSE_PARTY_MANAGER;
	m_UIMessageFunction[UI_AWAY_PARTY] = Execute_UI_AWAY_PARTY;

	m_UIMessageFunction[UI_FINISH_REQUEST_DIE_BUTTON] = Execute_UI_FINISH_REQUEST_DIE_BUTTON;

	m_UIMessageFunction[UI_CLOSE_TEAM_LIST] = Execute_UI_CLOSE_TEAM_LIST;
	m_UIMessageFunction[UI_CLOSE_TEAM_INFO] = Execute_UI_CLOSE_TEAM_INFO;
	m_UIMessageFunction[UI_CLOSE_TEAM_MEMBER_INFO] = Execute_UI_CLOSE_TEAM_MEMBER_INFO;
	m_UIMessageFunction[UI_CLOSE_TEAM_REGIST] = Execute_UI_CLOSE_TEAM_REGIST;
	m_UIMessageFunction[UI_CLOSE_TEAM_MEMBER_LIST] = Execute_UI_CLOSE_TEAM_MEMBER_LIST;
	
	m_UIMessageFunction[UI_REQUEST_GUILD_INFO] = Execute_UI_REQUEST_GUILD_INFO;
	m_UIMessageFunction[UI_REQUEST_GUILD_MEMBER_LIST] = Execute_UI_REQUEST_GUILD_MEMBER_LIST;
	m_UIMessageFunction[UI_MODIFY_GUILD_MEMBER] = Execute_UI_MODIFY_GUILD_MEMBER;

	m_UIMessageFunction[UI_SELECT_TEAM_MEMBER_LIST] = Execute_UI_SELECT_TEAM_MEMBER_LIST;		// void_ptr = TEAM_NAME
	m_UIMessageFunction[UI_SELECT_READY_TEAM_LIST] = Execute_UI_SELECT_READY_TEAM_LIST;		// void_ptr = TEAM_NAME
	m_UIMessageFunction[UI_SELECT_REGIST_TEAM_LIST] = Execute_UI_SELECT_REGIST_TEAM_LIST;		// void_ptr = TEAM_NAME
//add by viva
	m_UIMessageFunction[UI_CLOSE_FRIEND_CHATTING_INFO] = Execute_UI_CLOSE_FRIEND_CHATTING_INFO;
	m_UIMessageFunction[UI_OPEN_FRIEND_CHATTING_INFO] = Execute_UI_OPEN_FRIEND_CHATTING_INFO;
	//friend message
	m_UIMessageFunction[UI_FRIEND_CHATTING_SEND_MESSAGE] = Execute_UI_FRIEND_CHATTING_SEND_MESSAGE;
	m_UIMessageFunction[UI_FRIEND_CHATTING_UPDATE] = Execute_UI_FRIEND_CHATTING_UPDATE;
	m_UIMessageFunction[UI_FRIEND_CHATTING_ADD_FRIEND] = Execute_UI_FRIEND_CHATTING_ADD_FRIEND;
	//ask_friend_request
	m_UIMessageFunction[UI_FRIEND_REQUEST_ACCEPT] = Execute_UI_FRIEND_REQUEST_ACCEPT;
	//ask_friend_ask_close
	m_UIMessageFunction[UI_FRIEND_ASK_CLOSE] = Execute_UI_FRIEND_ASK_CLOSE;
	//ask_friend_delete_request
	m_UIMessageFunction[UI_FRIEND_DELETE_ASK] = Execute_UI_FRIEND_DELETE_ASK;
	m_UIMessageFunction[UI_FRIEND_DELETE_ACCEPT] = Execute_UI_FRIEND_DELETE_ACCEPT;
//end
	m_UIMessageFunction[UI_JOIN_READY_TEAM] = Execute_UI_JOIN_READY_TEAM;				// void_ptr = TEAM_NAME
	m_UIMessageFunction[UI_JOIN_REGIST_TEAM] = Execute_UI_JOIN_REGIST_TEAM;			// void_ptr = TEAM_NAME

	m_UIMessageFunction[UI_REGIST_GUILD_MEMBER] = Execute_UI_REGIST_GUILD_MEMBER;			
	m_UIMessageFunction[UI_REGIST_GUILD_TEAM] = Execute_UI_REGIST_GUILD_TEAM;			

	m_UIMessageFunction[UI_CLOSE_FILE_DIALOG] = Execute_UI_CLOSE_FILE_DIALOG;


	m_UIMessageFunction[UI_CONNECT_SERVER] = Execute_UI_CONNECT_SERVER;
	m_UIMessageFunction[UI_CLOSE_SERVER_SELECT] = Execute_UI_CLOSE_SERVER_SELECT;

	m_UIMessageFunction[UI_NEWCHARACTER_CHECK] = Execute_UI_NEWCHARACTER_CHECK;
	
//	m_UIMessageFunction[UI_SELECT_EXPLOSIVE] = Execute_UI_SELECT_EXPLOSIVE;
//	m_UIMessageFunction[UI_CLOSE_SELECT_EXPLOSIVE] = Execute_UI_CLOSE_SELECT_EXPLOSIVE;

	m_UIMessageFunction[UI_ENCHANT_ACCEPT] = Execute_UI_ENCHANT_ACCEPT;
	m_UIMessageFunction[UI_ENCHANT_CANCEL] = Execute_UI_ENCHANT_CANCEL;

	m_UIMessageFunction[UI_MESSAGE_BOX] = Execute_UI_MESSAGE_BOX;

	m_UIMessageFunction[UI_CLOSE_OTHER_INFO] = Execute_UI_CLOSE_OTHER_INFO;

	m_UIMessageFunction[UI_MODIFY_TEAM_INFO] = Execute_UI_MODIFY_TEAM_INFO;
	m_UIMessageFunction[UI_MODIFY_GUILD_MEMBER_INTRO] = Execute_UI_MODIFY_GUILD_MEMBER_INTRO;

	m_UIMessageFunction[UI_SEND_NAME_FOR_SOUL_CHAIN] = Execute_UI_SEND_NAME_FOR_SOUL_CHAIN;
	m_UIMessageFunction[UI_CLOSE_TRACE_WINDOW] = Execute_UI_CLOSE_TRACE_WINDOW;

	m_UIMessageFunction[UI_RUN_CONNECT] = Execute_UI_RUN_CONNECT;

	m_UIMessageFunction[UI_SELECT_GRADE_SKILL] = Excute_UI_SELECT_GRADE_SKILL;

	m_UIMessageFunction[UI_USE_XMAS_TREE] = Excute_UI_USE_XMAS_TREE;
	m_UIMessageFunction[UI_CLOSE_XMAS_CARD_WINDOW] = Excute_UI_CLOSE_XMAS_CARD_WINDOW;

	m_UIMessageFunction[UI_SEND_BRING_FEE] = Excute_UI_SEND_BRING_FEE;
	m_UIMessageFunction[UI_CLOSE_BRING_FEE_WINDOW] = Excute_UI_CLOSE_BRING_FEE_WINDOW;

	m_UIMessageFunction[UI_CLOSE_WAR_LIST_WINDOW] = Excute_UI_CLOSE_WAR_LIST;

	m_UIMessageFunction[UI_CLOSE_BLOOD_BIBLE_STATUS] = Execute_UI_CLOSE_BLOOD_BIBLE_STATUS;
	m_UIMessageFunction[UI_SEND_NAME_FOR_COUPLE] = Execute_UI_SEND_NAME_FOR_COUPLE;
	m_UIMessageFunction[UI_CLOSE_INPUT_NAME] = Execute_UI_CLOSE_INPUT_NAME_WINDOW;

	m_UIMessageFunction[UI_ITEM_USE_GEAR] = Execute_UI_ITEM_USE_GEAR;
	m_UIMessageFunction[UI_GO_BILING_PAGE] = Execute_GO_BILING_PAGE;
	m_UIMessageFunction[UI_CLOSE_POPUP_MESSAGE] = Execute_UI_CLOSE_POPUP_MESSAGE;
	m_UIMessageFunction[UI_CLOSE_QUEST_STATUS] = Execute_UI_CLOSE_QUEST_STATUS;

	m_UIMessageFunction[UI_CLOSE_LOTTERY_CARD] = Execute_UI_CLOSE_LOTTERY_CARD;
	m_UIMessageFunction[UI_LOTTERY_CARD_STATUS] = Execute_UI_LOTTERY_CARD_STATUS;
	m_UIMessageFunction[UI_FINISH_SCRATCH_LOTTERY]	= Execute_UI_FINISH_SCRATCH_LOTTERY;
	m_UIMessageFunction[UI_CLOSE_IMAGE_NOTICE] = Execute_UI_CLOSE_IMAGE_NOTICE;

	m_UIMessageFunction[UI_CLOSE_ITEM_LIST]	= Execute_UI_CLOSE_ITEM_LIST;
	m_UIMessageFunction[UI_SELECT_ITEM_FROM_SHOP] = Execute_UI_SELECT_ITEM_FROM_SHOP;
	m_UIMessageFunction[UI_CLOSE_BULLETIN_BOARD_WINDOW] = Execute_UI_CLOSE_BULLETIN_BOARD;

	m_UIMessageFunction[UI_TRANS_ITEM_CANCEL] = Execute_UI_TRANS_ITEM_CANCEL;
	m_UIMessageFunction[UI_TRANS_ITEM_ACCEPT] = Execute_UI_TRANS_ITEM_ACCEPT;
	m_UIMessageFunction[UI_FINISH_REQUEST_RESURRECT_BUTTON] = Execute_UI_REQUEST_RESURRECT;

	m_UIMessageFunction[UI_CLOSE_MIXING_FORGE] = Execute_UI_CLOSE_MIXING_FORGE;
	m_UIMessageFunction[UI_USE_MIXING_FORGE] = Execute_UI_MIXING_FORGE;

	m_UIMessageFunction[UI_CLOSE_REMOVE_OPTION] = Execute_UI_CLOSE_REMOVE_OPTION;
	m_UIMessageFunction[UI_SEND_REMOVE_OPTION] = Execute_UI_SEND_REMOVE_OPTION;

	m_UIMessageFunction[UI_CLOSE_OUSTERS_SKILL_INFO] = Execute_UI_CLOSE_OUSTERS_SKILL_INFO;
	m_UIMessageFunction[UI_OUSTERS_LEARN_SKILL] = Execute_UI_LEARN_OUSTERS_SKILL;

	m_UIMessageFunction[UI_RUN_LEVELUP] = Execute_UI_RUN_LEVELUP;
	m_UIMessageFunction[UI_CLOSE_HORN] = Execute_UI_CLOSE_HORN;
	m_UIMessageFunction[UI_OUSTERS_DOWN_SKILL] = Execute_UI_OUSTERS_DOWN_SKILL;	
	
	m_UIMessageFunction[UI_CLEAR_ALL_STAGE] = Execute_UI_CLEAR_ALL_STAGE;
	m_UIMessageFunction[UI_CLOSE_FINDING_MINE] = Execute_UI_CLOSE_FINDING_MINE;
	m_UIMessageFunction[UI_CLOSE_NEMONEMO] = Execute_UI_CLOSE_NEMONEMO;
	m_UIMessageFunction[UI_CLOSE_PUSHPUSH] = Execute_UI_CLOSE_PUSHPUSH;
	m_UIMessageFunction[UI_CLOSE_CRAZY_MINE] = Execute_UI_CLOSE_CRAZY_MINE;
	m_UIMessageFunction[UI_CLOSE_ARROW_TILE] = Execute_UI_CLOSE_ARROW_TILE;
	m_UIMessageFunction[UI_CLEAR_STAGE] = Execute_UI_CLEAR_STAGE;
	m_UIMessageFunction[UI_REQUEST_FORCE_DIE] = Execute_UI_FORCE_DIE;
	m_UIMessageFunction[UI_ADD_ITEM_TO_CODE_SHEET] = Execute_UI_ADD_ITEM_TO_CODE_SHEET;
	m_UIMessageFunction[UI_SEND_BUG_REPORT] = Execute_UI_SEND_BUG_REPORT;
	m_UIMessageFunction[UI_GO_TO_BEGINNER_ZONE] = Execute_UI_GO_BEGINNER_ZONE;
	m_UIMessageFunction[UI_POPUP_MESSAGE_OK] = Execute_UI_POPUP_MESSAGE_OK;

	m_UIMessageFunction[UI_CLOSE_SHRINE_MINIMAP] = Execute_UI_CLOSE_SHRINE_MINIMAP;
	m_UIMessageFunction[UI_WARP_REGEN_TOWER] = Execute_UI_WARP_TO_REGEN_TOWER;
	m_UIMessageFunction[UI_CLOSE_MAILBOX] = Execute_UI_CLOSE_MAILBOX;
	m_UIMessageFunction[UI_CLOSE_PET_INFO] = Execute_UI_CLOSE_PET_INFO;
	m_UIMessageFunction[UI_PET_GAMBLE] = Execute_UI_PET_GAMBLE;
	m_UIMessageFunction[UI_CLOSE_USE_PET_FOOD] = Execute_UI_CLOSE_USE_PET_FOOD;

	
	m_UIMessageFunction[UI_CLOSE_PETSTORAGE] = Execute_UI_CLOSE_PETSTORAGE;
	m_UIMessageFunction[UI_CLOSE_KEEP_PETITEM] = Execute_UI_CLOSE_KEEP_PETITEM;
	m_UIMessageFunction[UI_CLOSE_GET_KEEP_PETITEM] = Execute_UI_CLOSE_GET_KEEP_PETITEM;
	m_UIMessageFunction[UI_KEEP_PETITEM] = Execute_UI_KEEP_PETITEM;
	m_UIMessageFunction[UI_GET_KEEP_PETITEM] = Execute_UI_GET_KEEP_PETITEM;
	

	
	m_UIMessageFunction[UI_CLOSE_SMS_MESSAGE] = Execute_UI_CLOSE_SMS_MESSAGE;
	m_UIMessageFunction[UI_SEND_SMS_MESSAGE] = Execute_UI_SEND_SMS_MESSAGE;
	m_UIMessageFunction[UI_SMS_OPEN_LIST] = Execute_UI_SMS_OPEN_LIST;
	m_UIMessageFunction[UI_SMS_RECORD] = Execute_UI_SMS_RECORD;
	m_UIMessageFunction[UI_SMS_DELETE] = Execute_UI_SMS_DELETE;
	m_UIMessageFunction[UI_CLOSE_SMS_LIST] = Execute_UI_CLOSE_SMS_LIST;
	m_UIMessageFunction[UI_CLOSE_SMS_RECORD] = Execute_UI_CLOSE_SMS_RECORD;
	m_UIMessageFunction[UI_SMS_ADD_SEND_LIST] = Execute_UI_SMS_ADD_SEND_LIST;
	// 2004, 5, 28 sobeit add end

	
	m_UIMessageFunction[UI_CLOSE_NAMING] = Execute_UI_CLOSE_NAMING;
	m_UIMessageFunction[UI_CHANGE_CUSTOM_NAMING] = Execute_UI_CHANGE_CUSTOM_NAMING;
	m_UIMessageFunction[UI_SELECT_NAMING] = Execute_UI_SELECT_NAMING;
	m_UIMessageFunction[UI_CLOSE_NAMING_CHANGE] = Execute_UI_CLOSE_NAMING_CHANGE;
	m_UIMessageFunction[UI_RUN_NAMING_CHANGE] = Execute_UI_RUN_NAMING_CHANGE;
	// 2004, 6, 15 sobeit add end

	
	m_UIMessageFunction[UI_CLOSE_QUEST_MANAGER] = Execute_UI_CLOSE_QUEST_MANAGER;
	m_UIMessageFunction[UI_CLOSE_QUEST_LIST] = Execute_UI_CLOSE_QUEST_LIST;
	m_UIMessageFunction[UI_CLOSE_QUEST_DETAIL] = Execute_UI_CLOSE_QUEST_DETAIL;
	m_UIMessageFunction[UI_CLOSE_QUEST_MISSION] = Execute_UI_CLOSE_QUEST_MISSION;
	m_UIMessageFunction[UI_CLOSE_QUEST_ITEM] = Execute_UI_CLOSE_QUEST_ITEM;
	m_UIMessageFunction[UI_CLOSE_QUEST_ICON] = Execute_UI_CLOSE_QUEST_ICON;
	m_UIMessageFunction[UI_GQUEST_ACCEPT] = Execute_UI_GQUEST_ACCEPT;
	m_UIMessageFunction[UI_GQUEST_GIVEUP] = Execute_UI_GQUEST_GIVEUP;

	

	m_UIMessageFunction[UI_ITEM_USE_GQUEST_ITEM] = Execute_UI_ITEM_USE_GQUEST_ITEM;

	
	m_UIMessageFunction[UI_REQUEST_UNION] =	 Execute_UI_ITEM_USE_REQUEST_UNION;
	m_UIMessageFunction[UI_REQUEST_UNION_QUIT] =	 Execute_UI_ITEM_USE_QUIT;
	m_UIMessageFunction[UI_REQUEST_UNION_EXPERGUILD] =	 Execute_UI_ITEM_USE_EXPER;
	m_UIMessageFunction[UI_REQUEST_UNION_REQUEST_INFO] =   Execute_UI_ITEM_USE_UNION_INFO;
	m_UIMessageFunction[UI_REQUEST_UNION_REQUEST_GUILD_LIST] =		Execute_UI_ITEM_USE_REQUEST_GUILD_LIST;
	m_UIMessageFunction[UI_REQUEST_UNION_ACCEPT] =		Execute_UI_ITEM_USE_UNION_ACCEPT;
	m_UIMessageFunction[UI_REQUEST_UNION_DENY] =	Execute_UI_ITEM_USE_UNION_DENY;
	m_UIMessageFunction[UI_REQUEST_UNION_QUIT_ACCEPT] =		Execute_UI_ITEM_USE_UNION_QUIT_ACCEPT;
	m_UIMessageFunction[UI_REQUEST_UNION_QUIT_DENY] =	Execute_UI_ITEM_USE_UNION_QUIT_DENY;

	m_UIMessageFunction[UI_RECALL_BY_NAME] =	 Execute_UI_RECALL_BY_NAME;

	m_UIMessageFunction[UI_MODIFY_TAX] =	 Execute_UI_UI_MODIFY_TAX;

	m_UIMessageFunction[UI_APPOINT_SUBMASTER] =	 Execute_UI_APPOINT_SUBMASTER;


	m_UIMessageFunction[UI_DISPLAY_ITEM] =	             Execute_UI_DISPLAY_ITEM;        
    m_UIMessageFunction[UI_UNDISPLAY_ITEM] =	         Execute_UI_UNDISPLAY_ITEM;
    m_UIMessageFunction[UI_STORE_SIGN] =	             Execute_UI_STORE_SIGN;
    m_UIMessageFunction[UI_STORE_OPEN] =				 Execute_UI_STORE_OPEN;
    m_UIMessageFunction[UI_STORE_CLOSE] =				 Execute_UI_STORE_CLOSE;
	m_UIMessageFunction[UI_REQUEST_STORE_INFO] =		 Execute_UI_REQUEST_STORE_INFO;
	m_UIMessageFunction[UI_MY_STORE_INFO] =				 Execute_UI_MY_STORE_INFO;
    m_UIMessageFunction[UI_OTHER_STORE_INFO] =			Execute_UI_OTHER_STORE_INFO;
    m_UIMessageFunction[UI_BUY_STORE_ITEM] =			Execute_UI_BUY_STORE_ITEM;
    m_UIMessageFunction[UI_REMOVE_STORE_ITEM] =			Execute_UI_REMOVE_STORE_ITEM;
    m_UIMessageFunction[UI_ADD_STORE_ITEM] =			Execute_UI_ADD_STORE_ITEM;
	m_UIMessageFunction[UI_CLOSE_PERSNALSHOP] =			Execute_UI_CLOSE_PERSNALSHOP;
	
	m_UIMessageFunction[UI_CLOSE_POWER_JJANG]			=	 Execute_UI_CLOSE_POWER_JJANG;
	m_UIMessageFunction[UI_REQUEST_POWER_JJANG_POINT]	=	 Execute_UI_REQUEST_POWER_JJANG_POINT;
	m_UIMessageFunction[UI_POWER_JJANG_GAMBLE]			=	 Execute_UI_POWER_JJANG_GAMBLE;

	m_UIMessageFunction[UI_CLOSE_SWAPADVANCEMENTITEM]			=	 Execute_UI_CLOSE_SWAPADVANCEMENTITEM;
	m_UIMessageFunction[UI_SWAPADVANCEMENTITEM]			=	 Execute_UI_SWAPADVANCEMENTITEM;

	m_UIMessageFunction[UI_LEARN_ADVANCE_SKILL]			=	 Execute_UI_LEARN_ADVANCE_SKILL;
	
	
	m_UIMessageFunction[UI_CAMPAIGN_HELP] =	 Execute_UI_CAMPAIGN_HELP;

	m_UIMessageFunction[UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT] =	 Execute_UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT;

	m_UIMessageFunction[UI_GQUEST_SET_ACTION] =	 Execute_UI_GQUEST_SET_ACTION;
	m_UIMessageFunction[UI_GQUEST_ENDING_EVENT] =	 Execute_UI_GQUEST_ENDING_EVENT;

	m_UIMessageFunction[UI_REQUEST_EVENT_ITEM] =	 Execute_UI_REQUEST_EVENT_ITEM;

	#ifdef __TEST_SUB_INVENTORY__   
		m_UIMessageFunction[UI_CLOSE_INVENTORY_SUB]			=	 Execute_UI_CLOSE_INVENTORY_SUB;
		m_UIMessageFunction[UI_ITEM_DROP_TO_INVENTORY_SUB]		=	 Execute_UI_ITEM_DROP_TO_INVENTORY_SUB;
		m_UIMessageFunction[UI_ITEM_PICKUP_FROM_INVENTORY_SUB]	=	 Execute_UI_ITEM_PICKUP_FROM_INVENTORY_SUB;

		m_UIMessageFunction[UI_ITEM_USE_SUBINVENTORY]	=	 Execute_UI_ITEM_USE_SUBINVENTORY;
	#endif
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void			
UIMessageManager::Execute(DWORD message, int left, int right, void* void_ptr)
{
	
	
	if (g_Mode==MODE_GAME
		&& (g_pZone==NULL
			|| g_pPlayer==NULL
			|| g_pInventory==NULL
			|| g_pPlayer->IsSlayer() && g_pSlayerGear==NULL
			|| g_pPlayer->IsVampire() && g_pVampireGear==NULL
			|| g_pPlayer->IsOusters() && g_pOustersGear==NULL
			|| g_pTempInformation==NULL
			// 2004, 12, 17, sobeit add start
			|| message >= MAX_UI_MESSAGE
			// 2004, 12, 17, sobeit add end
			)
		)
	{
		DEBUG_ADD("[Error] UI message. game object pointer NULL");

		return;
	}
		
	if (m_UIMessageFunction[message]==NULL)
	{
		DEBUG_ADD_FORMAT("[Error] NO UI_MessageFunction. msg=%d", message);
	}
	else
	{
		(*m_UIMessageFunction[message])(left, right, void_ptr);

		DEBUG_ADD("[UI] Execute OK");
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_NEW_CHARACTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("UI_NEW_CHARACTER");
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}

	//
	// ((C_VS_UI::NEW_CHARACTER *)void_ptr) = 
	//	char *	sz_name;
	//	bool		bl_female;
	//	int		STR;
	//	int		DEX;
	//	int		INT;
	//	int		slot;
	//	
	//	int		face;
	//	int		skin_color;
	//	int		hair_color;
	//	int		coat_color;
	//	int		trouser_color;
	//
	NEW_CHARACTER* pChar = (NEW_CHARACTER *)void_ptr;
	
	BOOL bAllOK = TRUE;

	//---------------------------------------------
	
	//---------------------------------------------
	int len = strlen(pChar->sz_name);	
	
	if (len<PlayerInfo::minIDLength || len>PlayerInfo::maxIDLength)
	{
		char strTemp[128];
		sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), PlayerInfo::minIDLength, PlayerInfo::maxIDLength);
		g_pUIDialog->PopupFreeMessageDlg( strTemp );
		bAllOK = FALSE;
	}

	if (bAllOK)
	{
		if (!IsValidID(pChar->sz_name, "-_"))
		{
			//---------------------------------------------
			
			//---------------------------------------------						
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
			bAllOK = FALSE;
		}
		else
		{
			char strName[80];
			strcpy(strName, pChar->sz_name);

			
			if (g_pChatManager->RemoveCurse(strName))
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
				bAllOK = FALSE;
			}
			else
			{
				MChatManager badNameManager;
				badNameManager.LoadFromFile( g_pFileDef->getProperty("FILE_INFO_CHAT_2").c_str());

				if (badNameManager.RemoveCurse(strName))
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
					bAllOK = FALSE;
				}
			}
		}
	}

	//---------------------------------------------
	
	//---------------------------------------------
	if (bAllOK)
	{

			//char wansungName[16];

			//---------------------------------------------
			
			//---------------------------------------------
			//UI_JohapToWansung( pChar->sz_name, wansungName );

			CLCreatePC	_CLCreatePC;
			_CLCreatePC.setName ( pChar->sz_name );//wansungName );
			
			g_pUserInformation->Character[pChar->slot] = pChar->sz_name;
			//g_pUserInformation->CharacterW[pChar->slot] = wansungName;

			HairStyle hairStyle[3] =
			{
				HAIR_STYLE1,
				HAIR_STYLE2,
				HAIR_STYLE3
			};

			
			pChar->hair_color = max(0, min(pChar->hair_color, MAX_COLORSET-1));
			pChar->skin_color = max(0, min(pChar->skin_color, MAX_COLORSET-1));
			
			_CLCreatePC.setSlot ( pChar->slot==0? SLOT1 : pChar->slot==1? SLOT2 : SLOT3 );
			_CLCreatePC.setRace ( pChar->race );
			_CLCreatePC.setSex ( pChar->bl_female? FEMALE : MALE );
			_CLCreatePC.setHairStyle ( hairStyle[pChar->face] );
			_CLCreatePC.setHairColor ( pChar->hair_color );					
			_CLCreatePC.setSkinColor ( pChar->skin_color );
			_CLCreatePC.setShirtColor ( pChar->coat_color, MAIN_COLOR );
			_CLCreatePC.setShirtColor ( pChar->coat_color, SUB_COLOR );
			_CLCreatePC.setJeansColor ( pChar->trouser_color, MAIN_COLOR );
			_CLCreatePC.setJeansColor ( pChar->trouser_color, SUB_COLOR );
			_CLCreatePC.setSTR( pChar->STR );
			_CLCreatePC.setDEX( pChar->DEX );
			_CLCreatePC.setINT( pChar->INT );

			g_pSocket->sendPacket( &_CLCreatePC );

			g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_CREATE_PC );	

			

		SetMode(MODE_WAIT_CREATEPCOK);
	}
}


//-----------------------------------------------------------------------------
//
// Run NewUser Registration
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_RUN_NEWUSER_REGISTRATION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_RUN_NEWUSER_REGISTRATION");
	
	switch (g_pClientConfig->NEW_USER_REGISTERATION_MODE)
	{
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		case ClientConfig::NUR_CLIENT :
		{
//			gC_vs_ui.RunNewUserRegistration();
		}
//		break;

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		case ClientConfig::NUR_HOMEPAGE :		
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_HOMEPAGE].GetString() );			

			
			SetMode( MODE_QUIT );

#ifdef PLATFORM_WINDOWS
			char str[256];

			GetWindowsDirectory(
				str,  // address of buffer for Windows directory
				255        // size of directory buffer
			);

			sprintf(str, "%s\\Explorer.exe", str);

			CSDLGraphics::GetDD()->RestoreDisplayMode();
#ifdef __YHDK2__
			_spawnl(_P_NOWAIT, str, "Explorer.exe", "http://www.yhdk2.cn", NULL);
#else
			_spawnl(_P_NOWAIT, str, "Explorer.exe", "http://www.ttdk2.com", NULL);
#endif
#endif

			//_spawnl(_P_NOWAIT, str, "Explorer.exe", "http://www.ttdk2.cn", NULL);
		}
		break;

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		case ClientConfig::NUR_MESSAGE_HOMEPAGE :	
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_HOMEPAGE].GetString() );			
		}
		break;
		
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		case ClientConfig::NUR_DENY :				
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_DENY].GetString() );						
		}
		break;	
	}

}

//-----------------------------------------------------------------------------
//
// Char Info Close
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_INFO_CLOSE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_INFO_CLOSE");
	
	if (g_Mode!=MODE_WAIT_SELECTPC
		&& g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}
	
	gC_vs_ui.CloseInfo();
}

//-----------------------------------------------------------------------------
//
// Check Exist ID
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CHECK_EXIST_ID(int left, int right, void* void_ptr)
{
	//
	// left = id
	//
	DEBUG_ADD("[UI] UI_CHECK_EXIST_ID");
	
	const char* pName = (const char*)void_ptr;

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (IsValidID(pName, NULL))
	{
		int len = strlen(pName);	
		
		if (len<PlayerInfo::minIDLength || len>PlayerInfo::maxIDLength)
		{
			char strTemp[128];
			sprintf(strTemp, "ID %d~%d", PlayerInfo::minIDLength, PlayerInfo::maxIDLength);
			g_pUIDialog->PopupFreeMessageDlg( strTemp );						
		}
		else
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			if (!InitSocket())
			{						
				//InitFail("[Error] Can't init Socket");
				return;
			}

				CLQueryPlayerID _CLQueryPlayerID;
				//_CLDeletePC.setName( g_pUserInformation->CharacterW[pChar->slot].GetString() );
				_CLQueryPlayerID.setPlayerID( pName );

				g_pSocket->sendPacket( &_CLQueryPlayerID );

				g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_QUERY_PLAYER_ID );


			
			
		}
	}
	else
	{
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
	}

	
	DeleteNewArray( void_ptr );
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_DELETE_CHARACTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_DELETE_CHARACTER");
	
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}
	
	DELETE_CHARACTER *pChar = (DELETE_CHARACTER*)void_ptr;

	//--------------------------------------------------
	
	//--------------------------------------------------
	
	CLDeletePC _CLDeletePC;
	//_CLDeletePC.setName( g_pUserInformation->CharacterW[pChar->slot].GetString() );
	_CLDeletePC.setName( g_pUserInformation->Character[pChar->slot].GetString() );
	_CLDeletePC.setSlot( pChar->slot==0? SLOT1 : pChar->slot==1? SLOT2 : SLOT3 );
	_CLDeletePC.setSSN( "CONFIRM" );

	g_pSocket->sendPacket( &_CLDeletePC );
	g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_DELETE_PC );

	DeleteNewArray( pChar->sz_part1 );
	DeleteNewArray( pChar->sz_part2 );

	g_pUserInformation->Slot = pChar->slot;
	SetMode( MODE_WAIT_DELETEPCOK );
}

	
//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_TERMINATION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_TERMINATION");
	
	if (g_Mode!=MODE_MAINMENU)		
	{
		DEBUG_ADD("Not Mode MODE_MAINMENU");
		return;
	}

	SetMode( MODE_QUIT );
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
//
// Login
//
//-----------------------------------------------------------------------------
extern BYTE g_macAddress[6];
static void TraceLoginFlow(const char* step)
{
	(void)step;
}

void
UIMessageManager::Execute_UI_LOGIN(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_LOGIN");
	TraceLoginFlow("Execute_UI_LOGIN begin");

	g_dwSeqNumL = rand()%1024;
	g_dwSeqNumG = 0;

	UI_SaveUserOption();

	//
	// ((C_VS_UI::LOGIN *)void_ptr) = id, password
	//
	
	//
	LOGIN*	login = (LOGIN*)void_ptr;

	if (g_Mode!=MODE_MAINMENU)
	{
		TraceLoginFlow("Execute_UI_LOGIN rejected: not MODE_MAINMENU");
		DeleteNewArray(login->sz_id);
		DeleteNewArray(login->sz_password);

		DEBUG_ADD("Not Mode MODE_MAINMENU");
		return;
	}	

	
	LoadingAddonSPK( false );

	if (login->sz_id!=NULL && login->sz_password!=NULL)
	{
		char trace[200];
		sprintf(trace, "Execute_UI_LOGIN credentials id_len=%d pass_len=%d",
			(int)strlen(login->sz_id), (int)strlen(login->sz_password));
		TraceLoginFlow(trace);
		//if (IsValidID(login->sz_id, NULL))
		{
			
			//UI_BackupLoginID(((LOGIN *)void_ptr)->sz_id);
			
			if (IsValidPassword(login->sz_password))
			{
				{
					DEBUG_ADD("[Execute_UI_LOGIN]");
					if (!InitSocket())
					{
						TraceLoginFlow("Execute_UI_LOGIN InitSocket failed");
						//InitFail("[Error] Can't init Socket");
						DEBUG_ADD("[Execute_UI_LOGIN] Can't init Socket");
						DeleteNewArray(login->sz_id);
						DeleteNewArray(login->sz_password);
						DEBUG_ADD("[Execute_UI_LOGIN] Can't init Socket2");
						return;
					} 
					TraceLoginFlow("Execute_UI_LOGIN InitSocket OK");

					//char wansungID[16];

					//---------------------------------------------
					
					//---------------------------------------------
					//UI_JohapToWansung( login->sz_id, wansungID );
	
					//--------------------------------------------------
					
					
					
					
					//--------------------------------------------------
					//--------------------------------------------------
					// CLVersionCheck
					//--------------------------------------------------
					#if !defined(_DEBUG) && !defined(OUTPUT_DEBUG)
						int version = g_pUserInformation->GameVersion;
//						std::ifstream versionFile;//(FILE_INFO_ACTION, ios::binary);
//						if (!FileOpenBinary(FILE_INFO_VERSION, versionFile))
//						{
//							DeleteNewArray(login->sz_id);
//							DeleteNewArray(login->sz_password);
//							return;
//						}
//						versionFile.read((char*)&version, 4);
//						versionFile.close();

						CLVersionCheck _CLVersionCheck;
						_CLVersionCheck.setVersion( version );

						g_pSocket->sendPacket( &_CLVersionCheck );

						
					#endif
					// Windows SDL + local modernized server: the legacy encode-key handshake
					// can drop the connection before CLLogin is processed.
					if (g_pConfigKorean == NULL ||
						g_pConfigKorean->getProperty("LoginServerAddress") != "127.0.0.1")
					{
						CGConnectSetKey cgConnectSetKey;
						cgConnectSetKey.setEncryptKey(static_cast<WORD>(rand() & 0xFFFF));
						cgConnectSetKey.setHashKey(static_cast<WORD>(rand() & 0xFFFF));
						g_pSocket->sendPacket(&cgConnectSetKey);
						TraceLoginFlow("Execute_UI_LOGIN sent CGConnectSetKey");
						UpdateSocketOutput();
						cgConnectSetKey.execute(g_pSocket);
						Sleep(500);
					}
					else
					{
						TraceLoginFlow("Execute_UI_LOGIN skipped CGConnectSetKey for localhost");
					}
					//--------------------------------------------------
					// CLLogin
					//--------------------------------------------------
					CLLogin clLogin;

					//clLogin.setID("Reiot");
					//clLogin.setPassword("fpdldhxm");

					
					std::string temp_id;
					DEBUG_ADD("[Execute_UI_LOGIN] #");
					#ifdef __METROTECH_TEST__
						temp_id = "#";
					#endif
					
					//if(g_pUserInformation->IsNetmarble)
					//	temp_id += "@";
					
					temp_id += login->sz_id;
										
					clLogin.setID( temp_id );//wansungID);
					clLogin.setPassword(login->sz_password);			

					// 2004,03,16 sobeit add start - mac address
//					BYTE MacAddress[6];
//					memset(MacAddress, 0,sizeof(BYTE)*6);
//					memcpy(MacAddress, g_macAddress, sizeof(BYTE)*6);
//					GetMacAddressFromNetBIOS(MacAddress);
					clLogin.setMacAddress(g_macAddress);
					// 2004,03,16 sobeit add end - mac address
					clLogin.SetLoginMode(g_pUserInformation->IsAutoLogIn);

					if (g_pSocket)
					{
						g_pSocket->sendPacket( &clLogin );
						g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_LOGIN );
						TraceLoginFlow("Execute_UI_LOGIN sent CLLogin");
					}
					else
					{
						TraceLoginFlow("Execute_UI_LOGIN g_pSocket NULL before CLLogin");
					}
					DEBUG_ADD("[Execute_UI_LOGIN] SendPacket OK");

					//----------------------------------------------------
					
					//----------------------------------------------------
					g_pUserInformation->UserID = login->sz_id;
					//g_pUserInformation->UserIDW = wansungID;
				}
				
				DEBUG_ADD("[Execute_UI_LOGIN] ClearAllCharacter");
				gC_vs_ui.ClearAllCharacter();
				DEBUG_ADD("[Execute_UI_LOGIN] ClearAllCharacter OK");
				if(false == g_pUserInformation->IsAutoLogIn) 
				{
					SetMode( MODE_WAIT_LOGINOK );
					TraceLoginFlow("Execute_UI_LOGIN set MODE_WAIT_LOGINOK");
				}
				DEBUG_ADD("[Execute_UI_LOGIN] SetMode OK");
			}
			else
			{
				
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_PASSWORD_SPECIAL].GetString() );
			}
		}
		//---------------------------------------------
		
		//---------------------------------------------
		//else
		//{
			//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );							
		//}
	}
	

	

	//gC_vs_ui.StartGame();
	//gC_vs_ui.StartCharacterManager();

	DeleteNewArray(login->sz_id);
	DeleteNewArray(login->sz_password);
	DEBUG_ADD("[Execute_UI_LOGIN] OK");
	TraceLoginFlow("Execute_UI_LOGIN end");
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CHARACTER_MANAGER_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CHARACTER_MANAGER_FINISHED");

	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("g_Mode not MODE_WAIT_SELECTPC");

		return;
	}

	
	if(g_pUserInformation->IsNetmarble)
	{
		SetMode( MODE_MAINMENU );
		return;
	}

	//
	
	//
	//--------------------------------------------------
	
	//--------------------------------------------------
	 

	UI_SetServerList();

	SetMode( MODE_WAIT_SELECT_SERVER );
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

void
UIMessageManager::Execute_UI_CONNECT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CONNECT");
	
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not MODE_WAIT_SELECTPC");
		return;
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (g_pUserOption!=NULL)
	{
		if (g_pUserOption->PlayWaveMusic)
		{
//			if (g_pSDLStream!=NULL)
//			{
//				g_pSDLStream->Stop();
//			}
#ifdef __USE_MP3__
			if (g_pMP3 != NULL)
			{
				DEBUG_ADD("MP3 STOP13");
				g_pMP3->Stop();
				DEBUG_ADD("MP3 STOP13 OK");
			}
#else
			if (g_pOGG != NULL)
			{
				DEBUG_ADD("MP3 STOP13");
				if( g_SDLAudio.IsInit() )
					g_pOGG->streamClose();
				DEBUG_ADD("MP3 STOP13 OK");
			}
		}
		else
		{
			g_Music.Stop();
		}
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (g_pUIDialog!=NULL)
	{
		//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CONNECTING_SERVER].GetString(), -1, -1, 0 );

//		if (true)
//		{
//			DEBUG_ADD("UI show1");
//			gC_vs_ui.Show();
//			//gC_vs_ui.DrawMousePointer();		
//			DEBUG_ADD("UI show1 OK");
//		}
//		else
		{
			POINT point;
			
			DEBUG_ADD("UI show2");
			gC_vs_ui.Show();
			DEBUG_ADD("UI show2 OK");

			//gC_vs_ui.DrawMousePointer();

			//-----------------------------------------------------------------
			
			//-----------------------------------------------------------------		
			point.x = 0;
			point.y = 0;
			RECT rect = { 0, 0, g_GameRect.right, g_GameRect.bottom };
			g_pBack->BltNoColorkey( &point, g_pLast, &rect );	
		}
		CSDLGraphics::Flip();

		
		DEBUG_ADD("close msg dlg");
		g_pUIDialog->CloseMessageDlg();
	}

	DEBUG_ADD("LoadingAddonSPK");

	//------------------------------------------------------------
	// Test 2001.8.20
	//------------------------------------------------------------
	
	
	LoadingAddonSPK( false );
	

	DEBUG_ADD_FORMAT("CLSelectPC(%d)", left);

	CLSelectPC clSelectPC;
	
	//clSelectPC.setPCName( (const char *)g_pUserInformation->CharacterW[left] );//"sigi");
	clSelectPC.setPCName( (const char *)g_pUserInformation->Character[left] );//"sigi");
	clSelectPC.setPCType( (PCType)(g_pUserInformation->Race[left]));
	{
		char trace[256];
		sprintf(trace, "UI select slot=%d name=%s userRace=%d pcType=%d", left, (const char*)g_pUserInformation->Character[left], g_pUserInformation->Race[left], clSelectPC.getPCType());
		TraceLoginFlow(trace);
	}

	DEBUG_ADD_FORMAT("socket Set PC type");

	g_pSocket->setPCType(clSelectPC.getPCType());
	g_pSocket->setPCName(clSelectPC.getPCName());
	{
		char trace[256];
		sprintf(trace, "ClientPlayer selected name=%s pcType=%d", g_pSocket->getPCName().c_str(), g_pSocket->getPCType());
		TraceLoginFlow(trace);
	}

	DEBUG_ADD_FORMAT("user Information");

	g_pUserInformation->CharacterID = g_pUserInformation->Character[left];
	g_pUserInformation->FaceStyle = g_pUserInformation->FaceStyleSlot[left];	
	g_pUserInformation->IsMaster = 
		strncmp( g_pUserInformation->CharacterID, (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetString(), (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetLength() ) == 0 

		|| g_pInventory->FindItem(ITEM_CLASS_ETC, 0);
//		|| g_pPlayer->GetCreatureType()==CREATURETYPE_SLAYER_OPERATOR
//		|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_OPERATOR;


	#ifdef OUTPUT_DEBUG
		DEBUG_ADD("MasterCheck");
		std::ifstream file("master.txt", );

		char str[256];
		while (!file.eof())
		{
			file.getline(str, 256);
			
			int n = file.gcount();

			if (n <= 0)
				break;
			
			str[n] = '\0';

			g_pUserInformation->IsMaster |= (strstr(g_pUserInformation->CharacterID, str)!=NULL);
		}
		
		file.close();
		DEBUG_ADD("MasterCheck OK");
	#endif
	
	//g_pUserInformation->CharacterIDW = g_pUserInformation->CharacterW[left];
	DEBUG_ADD("send CLSelectPC");
	g_pSocket->sendPacket( &clSelectPC );
	g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_SELECT_PC );

	
	UpdateSocketOutput();

	SaveLastSelectedCharacter( left );

	DEBUG_ADD("set mode MODE_WAIT_RECONNECT");
	SetMode(MODE_WAIT_RECONNECT);
#endif
	//------------------------------
	
	//------------------------------
	//gC_vs_ui.StartGame();
					
	//
	
	//
	//gC_vs_ui.AddSlayerSkill(SKILL_FLASH_SLASHER);
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

void
UIMessageManager::Execute_UI_CHAT_RETURN(int left, int right, void* void_ptr)
{
	//
	
	//
	
	//   => gC_vs_ui.GetSendPossibleSlot();
	
	//
	// void_ptr = input string
	//	right = color
	//

	DEBUG_ADD("[UI] UI_CHAT_RETURN");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	DEBUG_ADD_FORMAT("[Original] %s", (char*)void_ptr );
	
	char* chatString = (char*)void_ptr; 
//	g_pChatManager->RemoveCurse(chatString);
	//-------------------------------------------------------------
	
	//-------------------------------------------------------------
	if (chatString!=NULL 
		&& chatString[0]!=SYMBOL_WHISPER	
		//&& chatString[0]!=SYMBOL_MASTER_COMMAND
		&& left==CLD_PARTY)
	{
		if (g_pParty!=NULL )
		{

				CGPartySay _CGPartySay;
				_CGPartySay.setColor(right);
				_CGPartySay.setMessage(chatString);
				g_pSocket->sendPacket( &_CGPartySay );

		}
	}
	
	else if (chatString!=NULL 
		&& chatString[0]!=SYMBOL_WHISPER	
		&& chatString[0]!=SYMBOL_MASTER_COMMAND
		&& (left==CLD_GUILD || left == CLD_UNION))
	{
		if(g_pUserInformation->GuildName.GetLength() > 0)
		{
			CGGuildChat _CGGuildChat;
			// 2004, 11, 11, sobeit add start
			if(left==CLD_GUILD)
				_CGGuildChat.SetType(0);
			else
				_CGGuildChat.SetType(1);
			// 2004, 11, 11, sobeit add end
			_CGGuildChat.setMessage( std::string(chatString) );
			_CGGuildChat.setColor( right );
			
			g_pSocket->sendPacket( &_CGGuildChat );
		}
	}
	//-------------------------------------------------------------
	
	//-------------------------------------------------------------
	else //if (g_pPlayer->IsAlive())
	{
		BOOL bZoneChat = (left==CLD_ZONECHAT);
	
		 

		//int pcsChat = left;

		//-------------------------------------------------------------
		//
		// PCS chat
		//
		//-------------------------------------------------------------
		//if (pcsChat)
		//{
			/*
			int slot = gC_vs_ui.GetSendPossibleSlot();

			if (slot!=NOT_SELECTED)
			{
				char* strUI = (char*)chatString;

				#ifdef	CONNECT_SERVER
					CGPhoneSay _CGPhoneSay;
					_CGPhoneSay.setSlotID( slot );
					_CGPhoneSay.setMessage( strUI );

					g_pSocket->sendPacket( &_CGPhoneSay );					
				#endif

				char temp[128];
				sprintf(temp, "[%s] %s", g_pUserInformation->CharacterID.GetString(), strUI);
				UI_AddChatToHistory( temp );								
			}
			*/
		//}
		//-------------------------------------------------------------
		//
		
		//
		//-------------------------------------------------------------
		//else
		{
			//
			
			//
			//break;

			
			//g_Socket.Send(g_String);
			char* strUI = chatString;
			char* strOrg = new char[128];
			char* str = strOrg;

			strcpy( strOrg, strUI );

			if (str!=NULL && str[0]!=NULL)
			{
				RemoveStringSpace( str );							

				DEBUG_ADD_FORMAT("[After RemoveSpace] %s", str );
					
				if (str!=NULL && str[0]!=NULL)
				{
					
					if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_DRAGON_EYES))
					{
						if(0 == strncmp(str, (*g_pGameStringTable)[UI_STRING_MESSAGE_RANGER_SAY].GetString(),(*g_pGameStringTable)[UI_STRING_MESSAGE_RANGER_SAY].GetLength()))
						{
							char TempBuffer[128]; 
							strcpy(TempBuffer, str+(*g_pGameStringTable)[UI_STRING_MESSAGE_RANGER_SAY].GetLength());
							CGRangerSay _CGRangerSay;
							_CGRangerSay.setMessage(TempBuffer);

							g_pSocket->sendPacket( &_CGRangerSay );
										
							
							g_pPlayer->SetChatString( str );//+1 );

							
							char temp[128];
							strcpy(temp, str );//+1);
							//sprintf(temp, "[%s] %s", g_pUserInformation->CharacterID.GetString(), str+1);
							//UI_AddChatToHistory( temp );								
							UI_AddChatToHistory( temp, g_pUserInformation->CharacterID.GetString(), CLD_ZONECHAT, right );
							return;
						}
					}
					
					//UI_STRING_MESSAGE_PLAYER_SAY
					if(0 == strncmp(str, (*g_pGameStringTable)[UI_STRING_MESSAGE_PLAYER_SAY].GetString(),(*g_pGameStringTable)[UI_STRING_MESSAGE_PLAYER_SAY].GetLength()))
					{
						
						
						MItem* pItem = g_pInventory->FindItem(ITEM_CLASS_EFFECT_ITEM,10);
						if (pItem == NULL)
						{
							
							pItem = g_pInventory->FindItem(ITEM_CLASS_EFFECT_ITEM,11);
							if (pItem == NULL)
							{
								
								pItem = g_pInventory->FindItem(ITEM_CLASS_EFFECT_ITEM,12);
							}
						}
						if (pItem!=NULL)
						{
							char TempBuffer[128]; 
							if (strlen(str)>60)
							{
								str[(*g_pGameStringTable)[UI_STRING_MESSAGE_PLAYER_SAY].GetLength()+60]=NULL;
							}
							
							strcpy(TempBuffer, str+(*g_pGameStringTable)[UI_STRING_MESSAGE_PLAYER_SAY].GetLength());	
							string msg="";
							msg = g_pUserInformation->CharacterID.GetString();
							msg +=">";
							msg +=TempBuffer;
							CGUseMessageItemFromInventory _CGUseMessageItemFromInventory;
							_CGUseMessageItemFromInventory.setObjectID( pItem->GetID() );
							_CGUseMessageItemFromInventory.setX( pItem->GetGridX() );
							_CGUseMessageItemFromInventory.setY( pItem->GetGridY() );
							_CGUseMessageItemFromInventory.setMessage( msg );
							g_pSocket->sendPacket( &_CGUseMessageItemFromInventory );
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
						}else
						{
							g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[UI_STRING_NO_ITEM_MESSAGE].GetString() );
						}
						return;
					}
					
					if (bZoneChat && str[0]!=SYMBOL_WHISPER
						&& str[0]!=SYMBOL_COMMAND && str[0]!=SYMBOL_MASTER_COMMAND)			
					{
						// system message test
						//------------------------------------------------------------
						//
						
						//
						//------------------------------------------------------------
						//case SYMBOL_GLOBALCHAT :	//if (str[0]=='!')//pWansungString[0]=='/')
						{
							//if (strlen(str) > 1 )	//pWansungString) > 1)
							{		
								//---------------------------------------------------------
								
								
								//---------------------------------------------------------
								if (g_pPlayer->IsUndergroundCreature()
									|| g_pPlayer->IsInCasket()
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_BAT
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_WOLF
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_WER_WOLF)
								{
									
									//char temp[128];
									//sprintf(temp, "[%s] .....", g_pUserInformation->CharacterID.GetString());
									//UI_AddChatToHistory( temp );								
									UI_AddChatToHistory( ".....", g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
								}
								//---------------------------------------------------------
								// global say
								//---------------------------------------------------------
								else
								{
									//---------------------------------------------------------
									
									//---------------------------------------------------------
									if (1)//g_CurrentTime > g_pUserInformation->GlobalSayTime+g_pClientConfig->DELAY_GLOBAL_SAY
										//#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
										//	|| 1
										//#endif
										//)
									{
										CGGlobalChat _CGGlobalChat;
										_CGGlobalChat.setMessage( str );//+ 1 );	//pWansungString+1 );
										_CGGlobalChat.setColor( right );
										g_pSocket->sendPacket( &_CGGlobalChat );

										
										
										g_pPlayer->SetChatString( str, right );//+1 );

										
										char temp[128];
										strcpy(temp, str );//+1);
										//sprintf(temp, "[%s] %s", g_pUserInformation->CharacterID.GetString(), str+1);
										//UI_AddChatToHistory( temp );								
										UI_AddChatToHistory( temp, g_pUserInformation->CharacterID.GetString(), CLD_ZONECHAT, right );

										
										//g_pUserInformation->GlobalSayTime = g_CurrentTime;

										
//										__BEGIN_HELP_EVENT
////											ExecuteHelpEvent( HE_CHAT_SHOUT );	
//										__END_HELP_EVENT
									}
									//else
									//{
										
									//	UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_GLOBAL_SAY].GetString(), NULL, CLD_INFO );
									//}
								}
							}
						}
						//break;

					}					
					else 
					switch ( str[0] )
					{
						//------------------------------
						
						//------------------------------
						//char* pWansungString = new char [strlen(str)+1];

						//UI_JohapToWansung( str, pWansungString );

						
						//------------------------------------------------------------
						//
						
						//
						//------------------------------------------------------------
						case SYMBOL_WHISPER :						
						{
							
							//else if (//g_pPlayer->IsVampire() && 
									//str[0]=='/')

							if (strlen(str) > 1 )
							{
								
								CToken strToken(str+1);

								
								const char* pName = strToken.GetToken();
								const char* pMessage = strToken.GetEnd();										
								
								if (pName!=NULL && pMessage!=NULL)
								{									
									int nameLen = strlen(pName);
									
									//------------------------------------------------------
									
									//------------------------------------------------------
									if (nameLen<PlayerInfo::minIDLength || nameLen>PlayerInfo::maxIDLength)
									{
										char strTemp[256];
										sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), 
													PlayerInfo::minIDLength,
													PlayerInfo::maxIDLength);

										UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
									}
									//------------------------------------------------------
									
									//------------------------------------------------------
									else
									{
										if (g_pUserInformation->CharacterID==pName)
										{
											
											UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_WHISPER_SELF].GetString(), NULL, CLD_INFO, right );
										}
										else
										{
											
											char strWhisperID[128];
											sprintf(strWhisperID, "%s ", pName);
											g_pUserInformation->WhisperID = strWhisperID;

											/*
											CGWhisper _CGWhisper;
											_CGWhisper.setName( pName );
											_CGWhisper.setMessage( pMessage );

											g_pSocket->sendPacket( &_CGWhisper );
											*/
											g_pWhisperManager->SendWhisperMessage( pName, pMessage, right );

											
											char strMessage[128];
											char strName[128];
											//sprintf(temp, "[%s] <%s> %s", g_pUserInformation->CharacterID.GetString(), pName, pMessage);
											//UI_AddChatToHistory( temp );
											strcpy(strMessage, pMessage);
											
											sprintf(strName, "[%s] %s", g_pUserInformation->CharacterID.GetString(), pName);
											UI_AddChatToHistory( strMessage, strName, CLD_WHISPER, right );

											
//											__BEGIN_HELP_EVENT
////												ExecuteHelpEvent( HE_CHAT_WHISPER );	
//											__END_HELP_EVENT
										}
									}
								}
							}
						}
						break;

						//------------------------------------------------------------
						//
						
						//
						//------------------------------------------------------------
						case SYMBOL_COMMAND :
						//else if (//g_pPlayer->IsVampire() && 
						//			str[0]=='@')
						{
							if (strlen(str) > 1 )
							{
								
								CToken strToken(str+1);

								
								const char* pCommand = strToken.GetToken();
								const char* pData = strToken.GetEnd();

								char pLwrCommand[128];
								strcpy(pLwrCommand, pCommand);
#ifdef PLATFORM_WINDOWS
								strcpy(pLwrCommand, _strlwr(pLwrCommand));
#else
								// Simple lowercase conversion for macOS
								for (int i = 0; pLwrCommand[i]; i++) {
									pLwrCommand[i] = tolower(pLwrCommand[i]);
								}
#endif


								DEBUG_ADD_FORMAT("[Command] %s %s", pCommand, pData);
								
								//-------------------------------------------------------
								
								//-------------------------------------------------------
								if (strcmp(pCommand, STRING_IGNORE)==0
									|| strcmp(pLwrCommand, STRING_IGNORE_ENG)==0)
								{
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									if (pData==NULL)
									{
										g_pChatManager->ClearID();
										g_pChatManager->SetIgnoreMode();	

										
										g_pChatManager->AddID( g_pUserInformation->CharacterID.GetString() );

										char strTemp[128];
										sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_IGNORE_ALL].GetString(), pData);
										UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );

										
//										__BEGIN_HELP_EVENT
////											ExecuteHelpEvent( HE_CHAT_REJECT );	
//										__END_HELP_EVENT
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else
									{
										int nameLen = strlen( pData );

										//------------------------------------------------------
										
										//------------------------------------------------------
										if (nameLen<PlayerInfo::minIDLength || nameLen>PlayerInfo::maxIDLength)
										{
											char strTemp[256];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), 
													PlayerInfo::minIDLength,
													nameLen>PlayerInfo::maxIDLength);

											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
										}
										//------------------------------------------------------
										
										//------------------------------------------------------
										else
										{
											if (g_pChatManager->IsAcceptMode())
											{
												g_pChatManager->AddID( pData );
											}
											else
											{
												g_pChatManager->RemoveID( pData );
											}

											char strTemp[128];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_IGNORE].GetString(), pData);
											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );

											
//											__BEGIN_HELP_EVENT
////												ExecuteHelpEvent( HE_CHAT_REJECT_USER );	
//											__END_HELP_EVENT
										}
									}
								}
								//-------------------------------------------------------
								
								//-------------------------------------------------------
								else if (strcmp(pCommand, STRING_ACCEPT)==0
										|| strcmp(pLwrCommand, STRING_ACCEPT_ENG)==0)
								{
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									if (pData==NULL)
									{
										g_pChatManager->ClearID();
										g_pChatManager->SetAcceptMode();	

										char strTemp[128];
										sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_ACCEPT_ALL].GetString(), pData);
										UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else
									{
										int nameLen = strlen( pData );

										//------------------------------------------------------
										
										//------------------------------------------------------
										if (nameLen<PlayerInfo::minIDLength || nameLen>PlayerInfo::maxIDLength)
										{
											char strTemp[256];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), 
													PlayerInfo::minIDLength,
													nameLen>PlayerInfo::maxIDLength);

											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
										}
										//------------------------------------------------------
										
										//------------------------------------------------------
										else
										{
											if (g_pChatManager->IsAcceptMode())
											{
												g_pChatManager->RemoveID( pData );
											}
											else
											{	
												g_pChatManager->AddID( pData );
											}

											char strTemp[128];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_ACCEPT].GetString(), pData);
											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
										}
									}
								}
								//-------------------------------------------------------
								
								//-------------------------------------------------------
								else if (strcmp(pCommand, STRING_ACCEPT_CURSE)==0
										|| strcmp(pLwrCommand, STRING_ACCEPT_CURSE_ENG)==0)
								{
									g_pUserOption->FilteringCurse = FALSE;

									char strTemp[128];
									sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_ACCEPT_CURSE].GetString(), pData);
									UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
								}
								//-------------------------------------------------------
								
								//-------------------------------------------------------
								else if (strcmp(pCommand, STRING_FILTER_CURSE)==0
										|| strcmp(pLwrCommand, STRING_FILTER_CURSE_ENG)==0)
								{
									g_pUserOption->FilteringCurse = TRUE;

									char strTemp[128];
									sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_FILTER_CURSE].GetString(), pData);
									UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
								}

								#if defined(OUTPUT_DEBUG) //&& defined(_DEBUG)
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "connect")==0
											|| strcmp(pLwrCommand, "connect")==0)
									{
										CToken strToken(pData);

										// [IP]+[]+[Name]
										const char* pIP = strToken.GetToken();
										const char* pName = strToken.GetEnd();

										//RequestConnect( pIP, pName );
										if (g_pParty!=NULL)
										{
											PARTY_INFO* pInfo = g_pParty->GetMemberInfo( pName );

											if (pInfo!=NULL)
											{
												pInfo->IP = pIP;
											}
										}
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "disconnect")==0
											|| strcmp(pLwrCommand, "disconnect")==0)
									{
										const char* pName = pData;
										//RequestDisconnect( pData );
										if (g_pParty!=NULL)
										{
											PARTY_INFO* pInfo = g_pParty->GetMemberInfo( pName );

											if (pInfo!=NULL)
											{
												pInfo->IP = "0.0.0.0";
											}
										}
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "profile")==0
											|| strcmp(pLwrCommand, "profile")==0)
									{
										const char* pName = pData;

										if (!g_pProfileManager->HasProfile(pName)
											&& !g_pProfileManager->HasProfileNULL(pName))
										{
											g_pProfileManager->RequestProfile(pName);
										}
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "say")==0
											|| strcmp(pLwrCommand, "say")==0)
									{
										if (g_pParty!=NULL
											&& g_pClientCommunicationManager!=NULL)
										{
											RCSay _RCSay;
											_RCSay.setName( g_pUserInformation->CharacterID.GetString() );
											_RCSay.setMessage( pData );
											_RCSay.setColor( right );
										
											for (int i=0; i<g_pParty->GetSize(); i++)
											{
												PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
												
												if (pInfo!=NULL)										
												{
													int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

													RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

													if (pUserInfo!=NULL)
													{
														if (pUserInfo->UDPPort!=0)
															port = pUserInfo->UDPPort;
													}

													g_pClientCommunicationManager->sendPacket( 
															pInfo->IP, 
															port, 
															&_RCSay );
												}
											}	
										}
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "npc")==0
											|| strcmp(pLwrCommand, "npc")==0)
									{
										g_MaxNPC = atoi( pData );
									}									
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "profileName")==0
											|| strcmp(pLwrCommand, "profilename")==0)
									{
										if (g_pProfiler!=NULL)
										{
											g_ProfileInfoName = pData;
										}
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "profileClear")==0
											|| strcmp(pLwrCommand, "profileclear")==0)
									{
										if (g_pProfiler!=NULL)
										{
											g_pProfiler->Release();
										}
									}
									//-------------------------------------------------------
									
									//-------------------------------------------------------
									else if (strcmp(pCommand, "gamma")==0
											|| strcmp(pLwrCommand, "gamma")==0)
									{
										const char* pValue = pData;

										int value = atoi( pValue );

										if (value==999)
										{
											CSDLGraphics::RestoreGammaRamp();
										}
										else
										{
											CSDLGraphics::SetGammaRamp( value );
										}
									}
									
								#endif
							}
						}
						break;

						case SYMBOL_MASTER_COMMAND:
						{
							static std::vector<std::string> vMasterCommand;
							static std::string	strTempCommand;

							char *pMessage = str;
							static bool bInvincible = false;

							if(strcmp(str, "*iddqd") == 0 || strcmp(str, "*Tnrrkt") == 0 || strcmp(str, "*") == 0)
							{
								if(bInvincible == true)
									strTempCommand = "*command invincible off";
								else
									strTempCommand = "*command invincible on";
								bInvincible = !bInvincible;

								pMessage = const_cast<char*>(strTempCommand.c_str());
							}
							else if(strcmp(str, "*gaonashi") == 0 || strcmp(str, "*gaonasi") == 0 || strcmp(str, "*rkdhsktl") == 0 || strcmp(str, "*") == 0 || strcmp(str,"*ghost") == 0)
							{
								if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_GHOST))
									strTempCommand = "*command ghost off";
								else
									strTempCommand = "*command ghost on";
								pMessage = const_cast<char*>(strTempCommand.c_str());
							}
							else if(strcmp(str, "*goes") == 0 )
							{
								strTempCommand = "*warp 8000 75 77";
								pMessage = const_cast<char*>(strTempCommand.c_str());
							}
							else if(strncmp(str, "*mc", 3) == 0 && isdigit(str[4]))
							{
//								if(vMasterCommand.empty())
								{
									std::string filename = "MasterCommand";
									filename += str[4];
									filename += ".txt";
									if(!_access(filename.c_str(), 0))
									{
										FILE *fp;

										fp = fopen(filename.c_str(), "rt");

										if(fp != NULL)
										{
											char szTemp[512];
											while(fgets(szTemp, 512, fp))
											{
												if( strlen(szTemp) > 0 )
												{
													if(szTemp[strlen(szTemp)-1] == '\n')
													szTemp[strlen(szTemp)-1] = '\0';
													char *pszTemp = new char[strlen(szTemp)+1];
													strcpy( pszTemp, szTemp );
													
													Execute_UI_CHAT_RETURN(left, right, (void *)pszTemp);
												}

//												CGSay _CGSay;
//												_CGSay.setMessage( szTemp );	//pWansungString );
//												_CGSay.setColor( right );
//												g_pSocket->sendPacket( &_CGSay );
											}
										}
										else
										{
											DEBUG_ADD("[Master Command] File pointer is NULL.");
										}
									}
									else
									{
										DEBUG_ADD("[Master Command] File access is Denied.");
									}
								}
							}
							else if(strncmp(str, "*C2G", 4) == 0 )
							{
								if( strcmp(str+5, "on") == 0 )
								{
									g_C2G = 1;
								}
								else
								{
									g_C2G--;
								}
//								for( int grid_y = 0; grid_y < g_pInventory->GetHeight(); grid_y++ )
//								for( int grid_x = 0; grid_x < g_pInventory->GetWidth(); grid_x++ )
//								{
//									C_VS_UI_INVENTORY::AutoMove( grid_x, grid_y );
//									gpC_base->DispatchMessage();
//								}
							}
							else if( g_C2G > 0 && strncmp(str, "*create", 7 ) == 0 )
							{
								g_C2G++;
							}

//							{
//								std::string aaaa[4];
//								aaaa[0] = "*set war_active off";
//								aaaa[1] = "*command removeRaceWar";
//								aaaa[2] = "*set war_active on";
//								aaaa[3] = "*set war_active off";
//
//								CGSay _CGSay;
//
//								_CGSay.setColor( right );
//
//								for(int i=0;i<4;i++)
//								{
//									_CGSay.setMessage( aaaa[i].c_str() );									
//									g_pSocket->sendPacket( &_CGSay );
//								}
//							}

							bool bSay = true;
#ifndef __METROTECH_TEST
							if(g_pUserInformation->bCompetence == true && g_pUserInformation->bCompetenceShape != true)
							{
								if(g_pZoneTable->Get( g_pZone->GetID() )->CompetenceZone == true)
								{
								}
								else
								{
									if(strstr(pMessage, "*command ghost off") != NULL)
										bSay = false;
								}
							}
#endif

							if(bSay == true)
							{
								CGSay _CGSay;
								_CGSay.setMessage( pMessage );	//pWansungString );
								_CGSay.setColor( right );
								g_pSocket->sendPacket( &_CGSay );
								
//								UI_AddChatToHistory( pMessage, g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
							}
							break;
						}

						//------------------------------------------------------------
						//
						
						//
						//------------------------------------------------------------
						default :
						{
							//------------------------------
							
							
							//------------------------------
							if (g_pPlayer->IsUndergroundCreature()
								|| g_pPlayer->IsInCasket()
								|| g_pPlayer->GetCreatureType()==CREATURETYPE_BAT
								|| g_pPlayer->GetCreatureType()==CREATURETYPE_WOLF
								|| g_pPlayer->GetCreatureType()==CREATURETYPE_WER_WOLF)
							{
								
								//char temp[128];
								//sprintf(temp, "[%s] .....", g_pUserInformation->CharacterID.GetString());
								//UI_AddChatToHistory( temp );
								UI_AddChatToHistory( ".....", g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
							}
							else
							{
								CGSay _CGSay;
								_CGSay.setMessage( str );	//pWansungString );
								_CGSay.setColor( right );
								g_pSocket->sendPacket( &_CGSay );

									
								
								g_pPlayer->SetChatString( str, right );

								
								//char temp[256];
								//sprintf(temp, "%s> %s", g_pUserInformation->CharacterID.GetString(), str);
								//UI_AddChatToHistory( temp );
								UI_AddChatToHistory( str, g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
							}
						}	
						
						//delete [] str;
						//delete [] pWansungString;
					}
				}
			}

			delete [] strOrg;
		}					
	}

	DeleteNewArray(void_ptr);

//	__BEGIN_HELP_EVENT
		ExecuteHelpEvent( HELP_EVENT_CHAT );
//	__END_HELP_EVENT

}


//-----------------------------------------------------------------------------
//
// Logout
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_LOGOUT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_LOGOUT");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	if (g_pPlayer->IsWaitVerify())
	{
		DEBUG_ADD_FORMAT("WaitVerify %d", g_pPlayer->GetWaitVerify());
		return;
	}
	
	//-------------------------------------------------------------------
	
	
	//-------------------------------------------------------------------
	int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

	//ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( zoneID );
	
	//-------------------------------------------------------------------
	
	
	//-------------------------------------------------------------------
	if (g_bZoneSafe //pZoneInfo!=NULL	&& pZoneInfo->Safety
		
		|| g_pPlayer->IsInSafeSector()
		|| g_pZone->GetID() == 8000				
		
//		|| g_pPlayer->IsDead() && g_pPlayer->GetDeadDelayLast() < 4)
		//|| g_pZone->GetCreatureNumber()==1)
#ifdef __METROTECH_TEST__
		|| 1
#endif
		)
	{
		ExecuteLogout();
	}
	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	else if (g_pPlayer!=NULL && g_pPlayer->IsAlive())
	{		
		if (g_pUserInformation->LogoutTime == 0)
		{
			
			
			#ifdef _DEBUG
				g_pUserInformation->LogoutTime = g_CurrentTime + 2000;
			#else
				g_pUserInformation->LogoutTime = g_CurrentTime + 5000;
			#endif

			MEvent event;
			event.eventID = EVENTID_LOGOUT;
			event.eventDelay = 5000;
			event.eventFlag = EVENTFLAG_SHOW_DELAY_STRING;
			event.eventType = EVENTTYPE_ZONE;
			event.m_StringsID.push_back(STRING_MESSAGE_LOGOUT_AFTER_SECOND);

			g_pEventManager->AddEvent(event);

			g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_LOGOUT);
//			g_pSystemMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_LOGOUT_AFTER_SECOND].GetString(), 5);
		}
		else if (g_pUserInformation->LogoutTime > g_CurrentTime)
		{
			DWORD sec = (g_pUserInformation->LogoutTime - g_CurrentTime) / 1000;

			if (sec > 0)
			{
				MEvent event;
				event.eventID = EVENTID_LOGOUT;
				event.eventDelay = sec*1000;
				event.eventFlag = EVENTFLAG_SHOW_DELAY_STRING;
				event.eventType = EVENTTYPE_ZONE;
				event.m_StringsID.push_back(STRING_MESSAGE_LOGOUT_AFTER_SECOND);
				
				g_pEventManager->AddEvent(event);
				g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_LOGOUT);

//				g_pSystemMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_LOGOUT_AFTER_SECOND].GetString(), sec);
			}
		}

		gC_vs_ui.HotKey_ESC();
	}
	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	else
	{
		g_pSystemMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );

		gC_vs_ui.HotKey_ESC();
	}	
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_SELECT_SKILL(int left, int right, void* void_ptr)
{
	DEBUG_ADD_FORMAT("[UI] UI_SELECT_SKILL(%d)", left);

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	//
	// dw_left = Skill ID
	//
	if (g_pPlayer!=NULL)
	{
		g_pPlayer->SetSpecialActionInfo( left );
		
		
		
		//g_pPlayer->UnSetRepeatAction();
	}

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_SKILL_ICON_CHANGE );
//	__END_HELP_EVENT
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CANCEL_SELECT_SKILL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CANCEL_SELECT_SKILL");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	//
	
	//
	if (g_pPlayer!=NULL)
	{
		g_pPlayer->SetSpecialActionInfoNULL();
	}
}


bool IsExistCorpseFromPlayer(MCreature* OriginCreature, int creature_type)
{
	int sx, sy;

	sx = OriginCreature->GetX();
	sy = OriginCreature->GetY();

	for(int i=-1;i<=1;i++)
	{
		for(int j=-1;j<=1;j++)
		{
			if( sx+i < 0 || sy + j < 0 || sx+i >= g_pZone->GetWidth() || sy+j >= g_pZone->GetHeight() )
				continue;

			const MSector& sector = g_pZone->GetSector( sx+i, sy+j );
			MItem* pItem = sector.GetItem();
			
			if( pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_CORPSE)
			{
				MCreature* pCreature = (dynamic_cast<MCorpse*>(pItem))->GetCreature();
				if( pCreature != NULL && pCreature->GetCreatureType() == creature_type )
					return true;
			}
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_CLIENT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_CLIENT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	MItem* pItem = UI_GetMouseItem();//(MItem*)void_ptr;		

	
	
	

	if(!( pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && pItem->GetItemType() == 27 ) )
	{
		if( IsExistCorpseFromPlayer( dynamic_cast<MCreature*>( g_pPlayer ), 670 ) )
		{
			g_pSystemMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_DROP_ITEM_BY_FLAG].GetString() );
			return;
		}
	}
				
	if (!g_bWatchMode
		
		&& !UI_IsRunningExchange()
		
		&& g_pUserInformation->ItemDropEnableTime < g_CurrentTime)
	{
		//void_ptr = MItem *

		if( pItem != NULL && 
				(
					gC_vs_ui.IsRunningRemoveOptionFromRareItem() && pItem->GetItemClass() == ITEM_CLASS_MIXING_ITEM 
				)
			)
//			pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_QUEST_ITEM && pItem->GetItemType() == 4 )
			return;
				
		if (pItem != NULL && (pItem->GetItemClass() == ITEM_CLASS_RELIC ||
			pItem->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE || 
			pItem->GetItemClass() == ITEM_CLASS_CASTLE_SYMBOL ||
			pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() == 27 || pItem->GetItemType() == 31) ||
			pItem->GetItemClass() == ITEM_CLASS_SWEEPER || pItem->GetItemClass() == ITEM_CLASS_PET_FOOD
			))
		{
			
			int CreatureID = g_pTopView->GetSelectedCreature();

			MCreature *pCreature = NULL;
			bool		bCorpse = false;

			if(CreatureID == -1)
			{
				CreatureID = g_pTopView->GetSelectedItemID();
				MItem *selectedItem = g_pZone->GetItem(CreatureID);
				if(selectedItem != NULL)
				{
					if(selectedItem->GetItemClass() == ITEM_CLASS_CORPSE)
					{
						MCorpse *pCorpse = (MCorpse *)selectedItem;
						pCreature = pCorpse->GetCreature();
						bCorpse = true;
					}
				}
			}
			else
			{
//				((g_pPlayer->IsSlayer() && pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 373)
//				|| (g_pPlayer->IsVampire() && pCreature->GetCreatureType() >= 374 && pCreature->GetCreatureType() <= 376))
				pCreature = g_pZone->GetCreature(CreatureID);
			}

			if(g_pPlayer->IsItemCheckBufferNULL() && pCreature != NULL)
			{
				if(bCorpse)	
				{
					if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || 
						pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 ||
						pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 ||
						pCreature->GetCreatureType() == 670	&& pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM || 
						pCreature->GetCreatureType() == 672 && pItem->GetItemClass() == ITEM_CLASS_SWEEPER
					)
					{
						if(abs(g_pPlayer->GetX()-pCreature->GetX()) < 3 && abs(g_pPlayer->GetY()-pCreature->GetY()) < 3)
						{
							CGRelicToObject _CGRelicToObject;
							_CGRelicToObject.setItemObjectID(pItem->GetID());
							_CGRelicToObject.setObjectID(CreatureID);
							g_pSocket->sendPacket( &_CGRelicToObject );
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_RELICTABLE );
						}
						else
						{
							g_pSystemMessage->Add((*g_pGameStringTable)[UI_STRING_MESSAGE_TOO_FAR].GetString());
						}
						return;
					}
				}
				else	
				{
					if(pItem->GetItemClass() == ITEM_CLASS_PET_FOOD &&
						pCreature->GetCreatureType() == 687 && pItem->GetNumber() == 1)	
					{
						if(abs(g_pPlayer->GetX()-pCreature->GetX()) < 3 && abs(g_pPlayer->GetY()-pCreature->GetY()) < 3)
						{
							CGTameMonster _CGTameMonster;
							_CGTameMonster.setObjectID(CreatureID);
							g_pSocket->sendPacket( &_CGTameMonster );
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_CREATURE );
						}
						else
						{
							g_pSystemMessage->Add((*g_pGameStringTable)[UI_STRING_MESSAGE_TOO_FAR].GetString());
						}
						return;
					}
					
					else if(pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && pItem->GetItemType() == 31 &&
						(pCreature->GetCreatureType() == 793 || pCreature->GetCreatureType() == 794 || pCreature->GetCreatureType() == 795))
					{
						CGRelicToObject _CGRelicToObject;
						_CGRelicToObject.setItemObjectID(pItem->GetID());
						_CGRelicToObject.setObjectID(CreatureID);
						g_pSocket->sendPacket( &_CGRelicToObject );
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_RELICTABLE );
						pCreature->RemoveCauseCriticalWoundsEffect();

					}
				}
			}

		}

		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		if (pItem!=NULL 
			&& !pItem->IsQuestItem()
			&& g_pPlayer->IsItemCheckBufferNULL()
			
			&& !(pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX&& pItem->GetItemType()==1)
			&& !(pItem->GetItemClass()==ITEM_CLASS_COUPLE_RING)
			&& !(pItem->GetItemClass()==ITEM_CLASS_VAMPIRE_COUPLE_RING 
			&& (pItem->GetItemClass() == ITEM_CLASS_CODE_SHEET && !gC_vs_ui.IsRunningQuestInventory() ) )
			&& !(pItem->GetItemClass() == ITEM_CLASS_LUCKY_BAG && pItem->GetItemType() == 3)
			&& !(pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() == 28||pItem->GetItemType() == 31))
				
			)
		{
			//-----------------------------------------------------------------------
			
			//-----------------------------------------------------------------------
			if (g_pTradeManager==NULL)
			{
					//---------------------------------------------------
					
					//---------------------------------------------------
					//if (g_pPlayer->IsItemCheckBufferNULL())
					{
						//---------------------------------------------------
						
						
						
						//---------------------------------------------------
						CGAddMouseToZone _CGAddMouseToZone;
						_CGAddMouseToZone.setObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGAddMouseToZone );

						
						//---------------------------------------------------
						
						
						//---------------------------------------------------
						
						//gC_vs_ui.DropItem();

						
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_ZONE );

						
						//delete pItem;
					}

			}
			//-----------------------------------------------------------------------
			
			//-----------------------------------------------------------------------
			else
			{
				DEBUG_ADD("Cannot Drop Item to Zone while Trading.");
			}
		}
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		else
		{
			DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer or Mouse NULL");
		}
	}
	else
	{
		DEBUG_ADD( "Can't Drop");
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_INVENTORY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_INVENTORY");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = grid_x, right = grid_y
	
	
	//
	MItem* pItem = (MItem*)void_ptr;		
	MItem* pMouseItem = UI_GetMouseItem();	

	
	
	
	BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());
	BOOL bAcceptOtherTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptOtherTrade());

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferNULL() && pMouseItem!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{		
		BOOL bSendPacketTradeRemove = FALSE;	
		BOOL bSendPacket = TRUE;				// mouse --> inventory
		BOOL bSendPacketTradeAdd = FALSE;		
		
		TYPE_OBJECTID removeItemID = OBJECTID_NULL;
		TYPE_OBJECTID toInventoryItemID = (pMouseItem==NULL)? OBJECTID_NULL : pMouseItem->GetID();

		//---------------------------------------------------
		
		//---------------------------------------------------
		if (bAcceptMyTrade)
		{
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
				&& g_pPlayer->IsItemCheckBufferNULL())
			{	
				g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_DROP_TO_INVENTORY);
				g_pTempInformation->Value1 = pMouseItem->GetID();
				g_pTempInformation->Value2 = left;
				g_pTempInformation->Value3 = right;
				g_pTempInformation->pValue = pMouseItem;

				g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_INVENTORY );

				//toInventoryItemID = pMouseItem->GetID();

				//-------------------------------------------------------------
				
				//-------------------------------------------------------------
				if (pItem!=NULL && pItem->IsTrade())
				{
					removeItemID = pItem->GetID();				
					
					bSendPacketTradeRemove = TRUE;
				}

				//bSendPacketTradeAdd = TRUE;
			}
			else
			{
				bSendPacket = FALSE;
			}
		}	
		//---------------------------------------------------
		
		//---------------------------------------------------
		else if (gC_vs_ui.IsRunningExchange())
		{
			//---------------------------------------------------
			
			//---------------------------------------------------
			if (pItem!=NULL && pItem->IsTrade())
			{
				removeItemID = pItem->GetID();

				bSendPacketTradeRemove = TRUE;
			}

			//---------------------------------------------------
			
			//---------------------------------------------------
			if (pMouseItem->IsTrade())
			{
				bSendPacketTradeAdd = TRUE;

				
				if (bAcceptOtherTrade)
				{
					g_pTradeManager->RefuseOtherTrade();
				}
			}
			//---------------------------------------------------
			
			//---------------------------------------------------
			else
			{
				/*
				if (bAcceptMyTrade)
				#ifdef CONNECT_SERVER
					CGTradeFinish _CGTradeFinish;
					_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
					_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

					g_pSocket->sendPacket( &_CGTradeFinish );
				#endif
				*/
			}
		}

		//---------------------------------------------------------
		// SendPacket - Trade Remove Item
		//---------------------------------------------------------
		if (bSendPacketTradeRemove)
		{
				CGTradeRemoveItem _CGTradeRemoveItem;
				_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeRemoveItem.setItemObjectID( removeItemID );

				g_pSocket->sendPacket( &_CGTradeRemoveItem );

				g_pTradeManager->SetNextAcceptTime();
		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		if (bSendPacket)
		{

				//---------------------------------------------------
				
				
				
				//---------------------------------------------------
				CGAddMouseToInventory _CGAddMouseToInventory;
				_CGAddMouseToInventory.setObjectID( toInventoryItemID );
				_CGAddMouseToInventory.setInvenX( left );//pItem->GetGridX() );
				_CGAddMouseToInventory.setInvenY( right );//pItem->GetGridY() );

				g_pSocket->sendPacket( &_CGAddMouseToInventory );

		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		if (bSendPacketTradeAdd)
		{
				CGTradeAddItem _CGTradeAddItem;
				_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeAddItem.setItemObjectID( pMouseItem->GetID() );

				g_pSocket->sendPacket( &_CGTradeAddItem );
				
		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		if (!bAcceptMyTrade)
		{
			MItem* pOldItem = NULL;
			if (g_pInventory->ReplaceItem(pMouseItem,		
												left, right,	
												pOldItem))		
			{
				if (pOldItem != NULL) 
				{				
					UI_PickUpItem( pOldItem );
				}
				else
				{
					UI_DropItem();
				}
			}

			
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_TREE 
				&& pMouseItem->GetItemType() != 12
				&& pMouseItem->GetItemType() != 25
				&& pMouseItem->GetItemType() != 26
				&& pMouseItem->GetItemType() != 27
				&& pMouseItem->GetItemType() != 28
				&& pMouseItem->GetItemType() != 41
				)
			{
				int mx,my;
				
				
				if(pMouseItem->GetItemType() < 12)
				{
					my = pMouseItem->GetItemType()/3;
					mx = pMouseItem->GetItemType()%3;
				}
				else if( pMouseItem->GetItemType() < 25 )
				{
					my = (pMouseItem->GetItemType()-13)/3;
					mx = (pMouseItem->GetItemType()-13)%3;
				} 
				else if( pMouseItem->GetItemType() < 41 )
				{
					my = (pMouseItem->GetItemType()-29)/3;
					mx = (pMouseItem->GetItemType()-29)%3;
				}
				const int pointX = pMouseItem->GetGridX() - mx;
				const int pointY = pMouseItem->GetGridY() - my;
				
				bool bTree = true;

				for(int y = 0; y < 4; y++)
				{
					for(int x = 0; x < 3; x++)
					{
						MItem *pPartOfTree = g_pInventory->GetItem(x+pointX, y+pointY);
						if(pPartOfTree != NULL && 
							pPartOfTree->GetItemClass() == ITEM_CLASS_EVENT_TREE &&
							(pPartOfTree->GetItemType() == y*3+x ||
							pPartOfTree->GetItemType() == y*3+x + 13 ||
							pPartOfTree->GetItemType() == y*3+x + 29) )
						{
						}
						else
						{
							bTree = false;
						}
					}
				}

				if(bTree)
					g_pPlayer->SetItemCheckBuffer(NULL, MPlayer::ITEM_CHECK_BUFFER_TREE_MERGE);
			}
		}
	}
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	else
	{
		DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer or Mouse NULL");		

		if(!g_pPlayer->IsItemCheckBufferNULL())
			DEBUG_ADD_FORMAT("[ITEMDROP] !g_pPlayer->IsItemCheckBufferNULL() %d", g_pPlayer->GetItemCheckBufferStatus());
		if(pMouseItem == NULL)
			DEBUG_ADD("[ITEMDROP] pMouseItem == NULL)");
		if(g_pTempInformation->GetMode()!=TempInformation::MODE_NULL)
			DEBUG_ADD_FORMAT("[ITEMPDROP] g_pTempInformation->Mode!=TempInformation::MODE_NULL %d", g_pTempInformation->Mode);
	}

	
	//gC_vs_ui.DropItem();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_QUICKSLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_QUICKSLOT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	
	MOustersArmsBand* pQuickSlot = NULL;

	int slot = left;
	
	if( g_pPlayer->IsOusters() )
	{
		if( g_pArmsBand1 == NULL )
			pQuickSlot = g_pArmsBand2;
		else
		{
			if( g_pArmsBand1->GetPocketNumber() > slot )
				pQuickSlot = g_pArmsBand1;
			else				
			{
				pQuickSlot = g_pArmsBand2;
				slot -= g_pArmsBand1->GetPocketNumber();
			}
		}

		if( pQuickSlot == NULL )
			return;
	}

	//
	// left = grid_x
	// void_ptr = MItem *
	//
	//---------------------------------------------------
	
	
	
	
	//---------------------------------------------------
	MItem* pSlotItem = NULL ;// = (MItem*)g_pQuickSlot->GetItem( left );;//gpC_mouse_pointer->GetPickUpItem();
	if( g_pPlayer->IsSlayer() )
	{
		if (g_pQuickSlot == NULL)
			return;

		pSlotItem = (MItem*)g_pQuickSlot->GetItem( left );
	}
	else
	{
		if (pQuickSlot == NULL)
			return;

		pSlotItem = (MItem*)pQuickSlot->GetItem( slot );
	}
	
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();//g_pQuickSlot->GetItem( left );
	if (pMouseItem == NULL)
		return;
	
	bool	Replace = FALSE;
	
	if( g_pPlayer->IsSlayer() )
		Replace = g_pQuickSlot->ReplaceItem( pMouseItem, left, pSlotItem );
	else
		Replace = pQuickSlot->ReplaceItem( pMouseItem, slot, pSlotItem );

	if (Replace)
	{
		if (pSlotItem) 
		{
			gpC_mouse_pointer->PickUpItem((MItem *)pSlotItem);
		}
		else
		{
			// not replace, just drop at empty grid.
			gpC_mouse_pointer->DropItem();
		}
	}
	else
	{
		return;
	}
	
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)//g_pPlayer->IsItemCheckBufferNULL())
	{
		
		if (1)//g_pQuickSlot->GetItem( left )==NULL)
		{

				//MItem* pItem = (MItem*)void_ptr;

				//---------------------------------------------------
				
				
				//---------------------------------------------------
				CGAddMouseToQuickSlot _CGAddMouseToQuickSlot;
				_CGAddMouseToQuickSlot.setObjectID( pMouseItem->GetID() );
				
				if( g_pPlayer->IsSlayer() )
					_CGAddMouseToQuickSlot.setSlotID( left );//pItem->GetItemSlot() );
				else
				{
					if( pQuickSlot == g_pArmsBand2 )
						_CGAddMouseToQuickSlot.setSlotID( slot + 3 );
					else
						_CGAddMouseToQuickSlot.setSlotID( slot );
				}
				
				g_pSocket->sendPacket( &_CGAddMouseToQuickSlot );
		
				//---------------------------------------------------
				
				//---------------------------------------------------
				
				//g_pQuickSlot->AddItem( pItem, left );
				
				//UI_DropItem();

			PlaySound( pMouseItem->GetInventorySoundID() );
		}
		//---------------------------------------------------
		
		//---------------------------------------------------
		else
		{						
			DEBUG_ADD_FORMAT("[Error] Can't add item to quickslot, already exist slot=%d", left);
		}
	}
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	else
	{
		DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer");
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------				
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ADD_ITEM_TO_GEAR");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot enum
	
	//

	if (!g_bWatchMode)
	{
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		if (g_pPlayer->IsItemCheckBufferNULL()
			&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
		{
			MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();	
			MItem* pGearItem = (MItem*)void_ptr;	
			
			//-----------------------------------------------------------------
			
			//-----------------------------------------------------------------
			if (pMouseItem!=NULL)
			{
				const ObjectID_t mouseObjectID = pMouseItem->GetID();
				SlotID_t targetSlotID = (SlotID_t)left;
				bool bGearChanged = false;

				//-----------------------------------------------------------------
				
				//-----------------------------------------------------------------
				if (g_pPlayer->SetAddonItem( pMouseItem ))
				{				
					MItem* pRemovedItem = NULL;

					// mouse --> gear
					switch(g_pPlayer->GetRace())
					{
					case RACE_SLAYER:
						bGearChanged = g_pSlayerGear->ReplaceItem( pMouseItem, (MSlayerGear::GEAR_SLAYER)left, pRemovedItem );
						if (bGearChanged)
							targetSlotID = (SlotID_t)pMouseItem->GetItemSlot();
						
						//-----------------------------------------------------------
						
						//-----------------------------------------------------------
						if (bGearChanged && (MSlayerGear::GEAR_SLAYER)left == MSlayerGear::GEAR_SLAYER_BELT)
						{
							gC_vs_ui.ResetSlayerQuickItemSize();
						}
						break;

					case RACE_VAMPIRE:
						bGearChanged = g_pVampireGear->ReplaceItem( pMouseItem, (MVampireGear::GEAR_VAMPIRE)left, pRemovedItem );
						if (bGearChanged)
							targetSlotID = (SlotID_t)pMouseItem->GetItemSlot();
						break;

					case RACE_OUSTERS:
						bGearChanged = g_pOustersGear->ReplaceItem( pMouseItem, (MOustersGear::GEAR_OUSTERS)left, pRemovedItem );
						if (bGearChanged)
							targetSlotID = (SlotID_t)pMouseItem->GetItemSlot();
						break;
					}

					if (!bGearChanged)
						return;

					UI_DropItem();

					//-----------------------------------------------------------
					
					//-----------------------------------------------------------
					if (pRemovedItem!=NULL)
					{
						UI_PickUpItem( pRemovedItem );
					}								

					//----------------------------------------------------
					
					//----------------------------------------------------
					g_pSkillAvailable->SetAvailableSkills();

						//---------------------------------------------------
						
						
						//---------------------------------------------------
						CGAddMouseToGear _CGAddMouseToGear;
						_CGAddMouseToGear.setObjectID( mouseObjectID );
						_CGAddMouseToGear.setSlotID( targetSlotID );//pItem->GetItemSlot() );
						
						g_pSocket->sendPacket( &_CGAddMouseToGear );
						

					//----------------------------------------------------
					
					//----------------------------------------------------
//					__BEGIN_HELP_EVENT
//						ITEM_CLASS itemClass = pMouseItem->GetItemClass();
//
//						//----------------------------------------------------

//						//----------------------------------------------------
//						if (pMouseItem->IsGunItem())
//						{

////							ExecuteHelpEvent( HE_ITEM_WEAR_GUN );
//						}
//						//----------------------------------------------------

//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_BELT)
//						{

////							ExecuteHelpEvent( HE_ITEM_WEAR_BELT );
//						}
//						//----------------------------------------------------

//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_SWORD)
//						{

////							ExecuteHelpEvent( HE_ITEM_WEAR_SWORD );
//						}
//						//----------------------------------------------------

//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_BLADE)
//						{

////							ExecuteHelpEvent( HE_ITEM_WEAR_BLADE );
//						}
//						//----------------------------------------------------

//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_CROSS)
//						{

////							ExecuteHelpEvent( HE_ITEM_WEAR_CROSS );
//						}
//						//----------------------------------------------------

//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_MACE)
//						{

////							ExecuteHelpEvent( HE_ITEM_WEAR_MACE );
//						}				
//					__END_HELP_EVENT
				}		
			}
			else
			{
				DEBUG_ADD( "[Error] Mouse Item is NULL" );
			}
		}
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		else
		{
			DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer");
		}
	}
	else
	{
		DEBUG_ADD( "[Watch Mode]");
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	//if (g_pPlayer->IsStop())
	{
		//-----------------------------------------
		
		//-----------------------------------------
		//g_pPlayer->SetAddonItem( pItem );

		//-----------------------------------------
		
		//-----------------------------------------
		//gC_vs_ui.DropItem();
	}
	//else
	{
	//	(*g_pSlayerGear).AddItem( pItem, (enum MSlayerGear::GEAR_SLAYER)left );
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_QUICKSLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_PICKUP_FROM_QUICKSLOT");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	//
	// left = slot enum
	// void_ptr = MItem *
	//
	MOustersArmsBand* pQuickSlot = NULL;
		
	if( g_pPlayer->IsOusters() )
	{
		if( g_pArmsBand1 == NULL )
			pQuickSlot = g_pArmsBand2;
		else
		{
			if( g_pArmsBand1->GetPocketNumber() > left )
				pQuickSlot = g_pArmsBand1;
			else
				pQuickSlot = g_pArmsBand2;
		}
		
		if( pQuickSlot == NULL )
			return;
	}
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = (MItem*)void_ptr;

			//---------------------------------------------------
			
			
			//---------------------------------------------------
			CGAddQuickSlotToMouse _CGAddQuickSlotToMouse;
			_CGAddQuickSlotToMouse.setObjectID( pItem->GetID() );
			if( pQuickSlot != NULL)
			{
				if( pQuickSlot == g_pArmsBand2 )
					_CGAddQuickSlotToMouse.setSlotID( pItem->GetItemSlot() + 3 );
				else
					_CGAddQuickSlotToMouse.setSlotID( pItem->GetItemSlot() );
			} else
				_CGAddQuickSlotToMouse.setSlotID( pItem->GetItemSlot() );
			
			g_pSocket->sendPacket( &_CGAddQuickSlotToMouse );
			

		
		
		if( g_pPlayer->IsSlayer() )
			g_pQuickSlot->RemoveItem( pItem->GetID() );
		else
		{
			if( pQuickSlot != NULL )
				pQuickSlot->RemoveItem( pItem->GetID() );
		}

		UI_PickUpItem(pItem);
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------				
void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_INVENTORY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_PICKUP_FROM_INVENTORY");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = grid x
	// right = grid y
	// void_ptr = MItem *
	//
	//if (g_pTempInformation->Mode==TempInformation::MODE_NULL)
	{
		MItem* pItem = (MItem*)void_ptr;

		BOOL bSendPacket = TRUE;
		
		BOOL bSendPacketTradeRemove = FALSE;

		BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());
		BOOL bAcceptOtherTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptOtherTrade());

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (bAcceptMyTrade)
		{
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
				&& g_pPlayer->IsItemCheckBufferNULL())
			{	
				g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_PICKUP_FROM_INVENTORY);
				g_pTempInformation->Value1 = pItem->GetID();
				g_pTempInformation->Value2 = left;
				g_pTempInformation->Value3 = right;
				g_pTempInformation->pValue = pItem;

				g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_PICKUP_FROM_INVENTORY );

				if (pItem->IsTrade())
				{
					bSendPacketTradeRemove = TRUE;
				}
			}
			else
			{
				bSendPacket = FALSE;
			}
		}
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		else if (gC_vs_ui.IsRunningExchange())
		{
			//---------------------------------------------------
			
			//---------------------------------------------------
			if (pItem->IsTrade())
			{
				UI_PickUpItem( pItem );

				bSendPacketTradeRemove = TRUE;
				
				
				if (bAcceptOtherTrade)
				{
					g_pTradeManager->RefuseOtherTrade();
				}
			}
			//---------------------------------------------------
			
			//---------------------------------------------------
			else
			{
				UI_PickUpItem( pItem );

				/*
				if (bAcceptMyTrade)
				{
					#ifdef CONNECT_SERVER
						CGTradeFinish _CGTradeFinish;
						_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

						g_pSocket->sendPacket( &_CGTradeFinish );					
					#endif
				}
				*/
			}
		}
		else
		{
			UI_PickUpItem( pItem );
		}

		//---------------------------------------------------------
		// SendPacket - Trade Remove Item
		//---------------------------------------------------------
		if (bSendPacketTradeRemove)
		{
				CGTradeRemoveItem _CGTradeRemoveItem;
				_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

				g_pSocket->sendPacket( &_CGTradeRemoveItem );

				g_pTradeManager->SetNextAcceptTime();
		}

		//---------------------------------------------------------
		// SendPacket - Inventory to Mouse
		//---------------------------------------------------------
		if (bSendPacket)
		{
				//---------------------------------------------------
				
				
				//---------------------------------------------------
				CGAddInventoryToMouse _CGAddInventoryToMouse;
				_CGAddInventoryToMouse.setObjectID( pItem->GetID() );
				_CGAddInventoryToMouse.setX( pItem->GetGridX() );
				_CGAddInventoryToMouse.setY( pItem->GetGridY() );
				
				g_pSocket->sendPacket( &_CGAddInventoryToMouse );				
		}
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------				
void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_REMOVE_ITEM_FROM_GEAR");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot enum
	// void_ptr = MItem *
	//

	if (!g_bWatchMode)
	{
		int itemSlot = left;
		MItem* pItem = (MItem*)void_ptr;	

		if (pItem!=NULL)
		{
			//-------------------------------------------------
			
			//-------------------------------------------------
			//if (g_pPlayer->IsStop())
			{
				//---------------------------------------------
				
				//---------------------------------------------
				if (g_pPlayer->RemoveAddonItem( pItem ))
				{
					MItem* pRemovedItem = NULL;

					//---------------------------------------------
					
					//---------------------------------------------
					switch(g_pPlayer->GetRace())
					{
					case RACE_SLAYER:
						pRemovedItem = g_pSlayerGear->RemoveItem( (MSlayerGear::GEAR_SLAYER)itemSlot );
						
						//-----------------------------------------------------------
						
						//-----------------------------------------------------------
						if (pRemovedItem!=NULL 
							&& pRemovedItem->GetItemSlot() == MSlayerGear::GEAR_SLAYER_BELT)
						{
							gC_vs_ui.ResetSlayerQuickItemSize();
						}					
						break;

					case RACE_VAMPIRE:
						pRemovedItem = g_pVampireGear->RemoveItem( (MVampireGear::GEAR_VAMPIRE)itemSlot );
						break;

					case RACE_OUSTERS:
						pRemovedItem = g_pOustersGear->RemoveItem( (MOustersGear::GEAR_OUSTERS)itemSlot );
						break;
					}


					//---------------------------------------------
					
					//---------------------------------------------
					if (pRemovedItem!=NULL)
					{
						UI_PickUpItem( pRemovedItem );
						
							//---------------------------------------------------
							
							
							//---------------------------------------------------
							CGAddGearToMouse _CGAddGearToMouse;
							_CGAddGearToMouse.setObjectID( pRemovedItem->GetID() );
							_CGAddGearToMouse.setSlotID( pRemovedItem->GetItemSlot() );
							
							g_pSocket->sendPacket( &_CGAddGearToMouse );							
					}

					//----------------------------------------------------
					
					//----------------------------------------------------
					g_pSkillAvailable->SetAvailableSkills();
				}			
			}
			//else
			{
			//	(*g_pSlayerGear).AddItem( pItem, (enum MSlayerGear::GEAR_SLAYER)left );
			}
		}
	}
	else
	{
		DEBUG_ADD( "[Watch Mode]");		
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_USE");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// void_ptr = MItem *
	//
	MItem* pItem = g_pInventory->GetItemToModify( left );//(MItem*)void_ptr;

	if (pItem!=NULL && g_pPlayer->IsItemCheckBufferNULL())
	{
		
		if(g_pPlayer->IsSlayer() && pItem->IsSlayerItem() ||
			g_pPlayer->IsVampire() && pItem->IsVampireItem() ||
			g_pPlayer->IsOusters() && pItem->IsOustersItem())
		{
			pItem->UseInventory();
		}
	}
	//----------------------------------------------------
	// item check buffer not null
	//----------------------------------------------------
	else
	{
		#ifdef OUTPUT_DEBUG
			if (pItem==NULL)
			{
				DEBUG_ADD("[Wait] Item is NULL");
			}
			else 
			{
				DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
			}
		#endif
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE_QUICKSLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_USE_QUICKSLOT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot
	// right = by FunctionKey
	//
	//MItem* pItem = (MItem*)void_ptr;
	if (g_pPlayer!=NULL && (g_pQuickSlot!=NULL && g_pPlayer->IsSlayer() || 
		g_pPlayer->IsOusters() && (g_pArmsBand1 != NULL || g_pArmsBand2 != NULL) ) )
	{
		int bByFunctionKey = right;	// by FunctionKey
		MSlotItemManager *pQuickSlot = g_pQuickSlot;

		int slot = left;
//		const MItem* pSlotItem = g_pQuickSlot->GetItem( left );
		MItem * pSlotItem = NULL;
		
		if( g_pPlayer->IsOusters() )
		{
			if( g_pArmsBand1 == NULL )
				pQuickSlot = g_pArmsBand2;
			else
			{
				if( g_pArmsBand1->GetPocketNumber() > slot )
					pQuickSlot = g_pArmsBand1;
				else
				{
					pQuickSlot = g_pArmsBand2;
					slot -= g_pArmsBand1->GetPocketNumber();
				}
			}
		}

		if( pQuickSlot == NULL )
			return;

		pSlotItem = pQuickSlot->GetItem( slot );

		if (pSlotItem!=NULL)
		{
			MItem *pItem = NULL;
			
			pItem = pQuickSlot->GetItemToModify( pSlotItem->GetID() );

			if (g_pPlayer->IsItemCheckBufferNULL())
			{
//				__BEGIN_HELP_EVENT
//					if (bByFunctionKey)
//					{

////						ExecuteHelpEvent( HE_PRESSED_FUNCTION_KEY_FOR_QUICKITEM );
//					}
//				__END_HELP_EVENT

				
				if(g_pPlayer->IsSlayer() && pItem->IsSlayerItem() ||
					g_pPlayer->IsVampire() && pItem->IsVampireItem() ||
					g_pPlayer->IsOusters() && pItem->IsOustersItem())
				{
					pItem->UseQuickItem();
				}
			}
			//----------------------------------------------------
			// item check buffer not null
			//----------------------------------------------------
			else
			{
				DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
			}
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] There is no item in slot=%d", left);
		}
	}
	else
	{
		DEBUG_ADD("[Error] There is no QuickSlot");
	}

}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_INVENTORY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_INVENTORY");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	
	
	
	MItem* pItem = (MItem*)void_ptr;
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}

	if (pItem==NULL)
	{
		DEBUG_ADD("[Error] InvenItem is NULL");
		return;
	}
	
	if (pItem->IsQuestItem())
	{
		return;
	}

	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());

	if (pItem->IsPileItem())
	{
		if (pItem->GetItemClass()==pMouseItem->GetItemClass()
			&& pItem->GetItemType()==pMouseItem->GetItemType()
			&& pItem->GetNumber() < pItem->GetMaxNumber())
		{
			BOOL bSendPacket = TRUE;

			BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());
			BOOL bAcceptOtherTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptOtherTrade());

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			if (bAcceptMyTrade)
			{	
				
				
				if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& g_pPlayer->IsItemCheckBufferNULL())
				{	
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_INSERT_TO_INVENTORY);
					g_pTempInformation->Value1 = pItem->GetID();
					g_pTempInformation->Value2 = left;
					g_pTempInformation->Value3 = right;
					g_pTempInformation->pValue = pItem;

					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
				}
				else
				{
					bSendPacket = FALSE;
				}
			}
			else
			{
				//g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
				MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

				//----------------------------------------------------
				
				//----------------------------------------------------
				int total = pMouseItem->GetNumber() + pItem->GetNumber();
				//yckou
				if(!bAcceptOtherTrade && (pItem->GetItemClass() == ITEM_CLASS_MONEY))
				{
					if(total>99)
					{
						pMouseItem->SetNumber( total - 99 );
						pItem->SetNumber( 99 );
					}
					else
					{
						
						pItem->SetNumber( total );
						UI_DropItem();
						
						delete pMouseItem;
					}
				}
				else
				//end yckou
				
				if ( total > pItem->GetMaxNumber())
				{
					
					pMouseItem->SetNumber( total - pItem->GetMaxNumber() );
					pItem->SetNumber( pItem->GetMaxNumber() );
				}
				else
				{
					
					pItem->SetNumber( total );
					UI_DropItem();

					delete pMouseItem;
				}

				//----------------------------------------------------
				
				//----------------------------------------------------
				// Remove and Add - -;
				if (gC_vs_ui.IsRunningExchange() && pItem->IsTrade())
				{
						//----------------------------------------------------
						
						//----------------------------------------------------
						CGTradeRemoveItem _CGTradeRemoveItem;
						_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeRemoveItem );

						g_pTradeManager->SetNextAcceptTime();

						
						//----------------------------------------------------
						
						//----------------------------------------------------
						CGAddMouseToInventory _CGAddMouseToInventory;
						_CGAddMouseToInventory.setObjectID( mouseItemID );
						_CGAddMouseToInventory.setInvenX( pItem->GetGridX() );
						_CGAddMouseToInventory.setInvenY( pItem->GetGridY() );								

						g_pSocket->sendPacket( &_CGAddMouseToInventory );

						bSendPacket = FALSE;

						
						//----------------------------------------------------
						
						//----------------------------------------------------
						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeAddItem );
						

					
					if (bAcceptOtherTrade)
					{
						g_pTradeManager->RefuseOtherTrade();
					}
				}

				PlaySound( pItem->GetInventorySoundID() );
			}

			//----------------------------------------------------
			
			//----------------------------------------------------
				if (bSendPacket)
				{
					CGAddMouseToInventory _CGAddMouseToInventory;
					_CGAddMouseToInventory.setObjectID( mouseItemID );
					_CGAddMouseToInventory.setInvenX( pItem->GetGridX() );
					_CGAddMouseToInventory.setInvenY( pItem->GetGridY() );								

					g_pSocket->sendPacket( &_CGAddMouseToInventory );

					
					//----------------------------------------------------
					
					//----------------------------------------------------
					//if (pItem->GetItemClass()==ITEM_CLASS_MONEY)
					//{
					//}
				}
				
		}
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_GEAR");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	
	
	// Gear Slot(left)

	if (!g_bWatchMode
		&& g_pPlayer->IsItemCheckBufferNULL())
	{
		MItem* pItem = (MItem*)void_ptr;
		MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

		//----------------------------------------------------
		// Magazine
		//----------------------------------------------------
		if (pItem->GetItemClass()==ITEM_CLASS_MAGAZINE)
		{
			//----------------------------------------------------
			
			//----------------------------------------------------
				//---------------------------------------------------
				
				
				//---------------------------------------------------
				CGAddMouseToGear _CGAddMouseToGear;
				_CGAddMouseToGear.setObjectID( pMouseItem->GetID() );
				_CGAddMouseToGear.setSlotID( pItem->GetItemSlot() );
				
				g_pSocket->sendPacket( &_CGAddMouseToGear );

				
				
				g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_GEAR );

				
		}
		//----------------------------------------------------
		
		//----------------------------------------------------
		else
		{
			
		}
	}
	else
	{
		DEBUG_ADD( "[Watch Mode]");
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_QUICKSLOT(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_QUICKSLOT");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	
	if (g_pPlayer->IsItemCheckBufferNULL())
	{
		
		
		
		MItem* pItem = (MItem*)void_ptr;
		MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();

		if (pItem->GetItemClass()==pMouseItem->GetItemClass()
			&& pItem->GetItemType()==pMouseItem->GetItemType())
		{
			//----------------------------------------------------
			
			//----------------------------------------------------
				CGAddMouseToQuickSlot _CGAddMouseToQuickSlot;
				_CGAddMouseToQuickSlot.setObjectID( pMouseItem->GetID() );
				if ( g_pPlayer->IsOusters() )
				{
					int slotID = left;
					
					if( g_pArmsBand1 != NULL &&g_pArmsBand1->GetPocketNumber() > slotID )
						_CGAddMouseToQuickSlot.setSlotID( slotID );
					else
					{
						_CGAddMouseToQuickSlot.setSlotID( pItem->GetItemSlot() + 3);
					}
				}
				else
					_CGAddMouseToQuickSlot.setSlotID( pItem->GetItemSlot() );

				g_pSocket->sendPacket( &_CGAddMouseToQuickSlot );

				
				
				
				//g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
				//MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();

				//----------------------------------------------------
				
				//----------------------------------------------------
				int total = pMouseItem->GetNumber() + pItem->GetNumber();
				if ( total > pItem->GetMaxNumber() )
				{
					
					pMouseItem->SetNumber( total - pItem->GetMaxNumber() );
					pItem->SetNumber( pItem->GetMaxNumber() );
				}
				else
				{
					
					pItem->SetNumber( total );
					UI_DropItem();

					delete pMouseItem;
				}

				PlaySound( pItem->GetInventorySoundID() );

				//----------------------------------------------------
				
				//----------------------------------------------------
				//if (pItem->GetItemClass()==ITEM_CLASS_MONEY)
				//{
				//}
				
		}
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_SHOP(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_SHOP");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseShop();

		
		gC_vs_ui.SetShop( NULL );


		
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
//
// void_ptr = MShop
// left = slot
// right = item count
//
void
UIMessageManager::Execute_UI_BUY_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_BUY_ITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		int index = left;					
		int number = right;					
		MShop* pShop = (MShop*)void_ptr;	
		int npcID = (*g_pPCTalkBox).GetNPCID();					

		if (pShop!=NULL)
		{					
			MShopShelf* pShopShelf = pShop->GetCurrentShelf();
			
			if (pShopShelf!=NULL)
			{
				MItem* pItem = pShopShelf->GetItem( index );
				
				if (pItem!=NULL)
				{
					POINT point;
					
					int oldNumber = pItem->GetNumber();					

					//-------------------------------------------------
					
					//-------------------------------------------------							
					bool bBuyPossible = false;
					//GAME_STRINGID buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY;
					int buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY;

					switch (pShop->GetShopType())
					{
						//-------------------------------------------------
						
						//-------------------------------------------------
						case MShop::SHOP_NORMAL :
						{
							int price = number * (*g_pPriceManager).GetItemPrice(pItem, MPriceManager::NPC_TO_PC, pShopShelf->GetShelfType()==MShopShelf::SHELF_UNKNOWN);
							int money = (*g_pMoneyManager).GetMoney();

							bBuyPossible = (price <= money);

							buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY_NO_MONEY;
						}
						break;

						//-------------------------------------------------
						
						//-------------------------------------------------
						case MShop::SHOP_EVENT_STAR :
						{
							STAR_ITEM_PRICE starPrice;

							g_pPriceManager->GetItemPrice( pItem, starPrice );

							if (starPrice.type!=-1 && starPrice.number!=0)
							{
								
								MItemClassTypeNumberFinder starFinder(ITEM_CLASS_EVENT_STAR, 
																		starPrice.type);


								((MItemManager*)g_pInventory)->FindItem( starFinder );

								
								bBuyPossible = (starFinder.GetTotalNumber() >= starPrice.number);

								buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY_NO_STAR;
							}							
							
						}
						break;
					}

					//-------------------------------------------------
					
					//-------------------------------------------------
					if (bBuyPossible)
					{								
						//-------------------------------------------------
						
						//-------------------------------------------------
						pItem->SetNumber( number );

						if (g_pInventory->GetFitPosition(pItem, point))
						{								
								CGShopRequestBuy _CGShopRequestBuy;
								_CGShopRequestBuy.setObjectID( npcID );

								_CGShopRequestBuy.setShopType( pShop->GetCurrent() );
								_CGShopRequestBuy.setShopIndex( index );

								_CGShopRequestBuy.setX(point.x);
								_CGShopRequestBuy.setY(point.y);

								_CGShopRequestBuy.setItemNum( number );

								g_pSocket->sendPacket( &_CGShopRequestBuy );

								
								//-------------------------------------------------
								
								//-------------------------------------------------
								(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_BUY;
								(*g_pTempInformation).Value1 = pShop->GetCurrent();
								(*g_pTempInformation).Value2 = index;
								(*g_pTempInformation).Value3 = point.x;
								(*g_pTempInformation).Value4 = point.y;
								(*g_pTempInformation).pValue = (void*)pShop;

								//-------------------------------------------------
								
								//-------------------------------------------------
								UI_LockItemTrade();
						}
						else
						{
							
							g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_SPACE ].GetString());
							
							
							ExecuteHelpEvent( HELP_EVENT_STORAGE_BUY );
							// 2004, 5, 6, sobeit add end
						}

						pItem->SetNumber( oldNumber );

					}
					//-------------------------------------------------
					
					//-------------------------------------------------					
					else
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[buyImpossibleMessage].GetString());
					}
				}
				else
				{
					
					DEBUG_ADD_FORMAT("[Error] There is NO Item in index=%d", index);
				}
			}
			else
			{
				
				DEBUG_ADD_FORMAT("[Error] There is NO Shelf type=%d", (int)pShop->GetCurrent());
			}
		}
		else
		{
			
			DEBUG_ADD_FORMAT("[Error] There is NO Shop. npc id=%d", npcID);
		}
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_SELL_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_SELL_FINISHED");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.FinishItemSelling();

		
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
//
// gridX   //void_ptr = Mitem*
// gridY
//
void
UIMessageManager::Execute_UI_SELL_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_SELL_ITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//MItem* pItem = (MItem*)void_ptr;
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		const MItem* pItem = g_pInventory->GetItem( left, right );

		if (pItem!=NULL && g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
		{
			
			
			if (//pItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX &&
				!pItem->IsUniqueItem() || (g_pTimeItemManager->IsExist( pItem->GetID())))
			{
				//-----------------------------------------------------
				
				//-----------------------------------------------------
					CGShopRequestSell	_CGShopRequestSell;

					_CGShopRequestSell.setObjectID( (*g_pPCTalkBox).GetNPCID() );		// NPC ID	
					_CGShopRequestSell.setOpCode( SHOP_REQUEST_SELL_NORMAL );
					_CGShopRequestSell.setItemObjectID( pItem->GetID() );	// Item Object ID

					g_pSocket->sendPacket( &_CGShopRequestSell );
			
						
					//-------------------------------------------------
					
					//-------------------------------------------------
					(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_SELL;
					(*g_pTempInformation).pValue = (void*)pItem;

					//-------------------------------------------------
					
					//-------------------------------------------------
					UI_LockItemTrade();
			}
			else
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SELL ].GetString());
			}
		}
		else
		{
			DEBUG_ADD("[Error] Item Is NULL");
		}
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
//
// gridX   //void_ptr = Mitem*
// gridY
//
void
UIMessageManager::Execute_UI_SELL_ALL_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_SELL_ALL_ITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//MItem* pItem = (MItem*)void_ptr;
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		//-----------------------------------------------------
		
		//-----------------------------------------------------
			CGShopRequestSell	_CGShopRequestSell;

			_CGShopRequestSell.setObjectID( (*g_pPCTalkBox).GetNPCID() );		// NPC ID	
			_CGShopRequestSell.setOpCode( SHOP_REQUEST_SELL_ALL_SKULL );
			_CGShopRequestSell.setItemObjectID( 0 );	// Item Object ID

			g_pSocket->sendPacket( &_CGShopRequestSell );
	
				
			//-------------------------------------------------
			
			//-------------------------------------------------
			(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_SELL_ALL_SKULL;
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			UI_LockItemTrade();
	}
}

//-----------------------------------------------------------------------------
//
//	Mouse Focus
// 
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_REMOVE_BACKGROUND_MOUSE_FOCUS(int left, int right, void* void_ptr)
{	
	if (g_Mode!=MODE_GAME)
	{
		return;
	}

	g_bUIInput = TRUE;
}

void
UIMessageManager::Execute_UI_BACKGROUND_MOUSE_FOCUS(int left, int right, void* void_ptr)
{	
	if (g_Mode!=MODE_GAME)
	{
		return;
	}

	//
	// (left, right) = mouse (x, y)
	//
	g_bUIInput = FALSE;
}
 
//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_PDS_CLOSED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PDS_CLOSED");
	
	gC_vs_ui.AcquirePDSClosedMessage();
}
*/

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_PLEASE_SET_SLAYER_VALUE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PLEASE_SET_SLAYER_VALUE");
	
	//C_VS_UI_SLAYER_PDS::m_str		= g_pPlayer->GetSTR();
	//C_VS_UI_SLAYER_PDS::m_dex		= g_pPlayer->GetDEX();
	//C_VS_UI_SLAYER_PDS::m_int		= g_pPlayer->GetINT();
	//C_VS_UI_SLAYER_PDS::m_hp		= g_pPlayer->GetHP();
	//C_VS_UI_SLAYER_PDS::m_mp		= g_pPlayer->GetMP();
	//C_VS_UI_SLAYER_PDS::m_th		= g_pPlayer->GetTOHIT();
	//C_VS_UI_SLAYER_PDS::m_ac		= g_pPlayer->GetAC();
	//C_VS_UI_SLAYER_PDS::m_cc		= g_pPlayer->GetCARRYWEIGHT();
	//C_VS_UI_SLAYER_PDS::m_str_max	= g_pPlayer->GetSTR(); // (!)
	//C_VS_UI_SLAYER_PDS::m_dex_max	= g_pPlayer->GetDEX(); // (!)
	//C_VS_UI_SLAYER_PDS::m_int_max	= g_pPlayer->GetINT();
	//C_VS_UI_SLAYER_PDS::m_HP_MAX	= g_pPlayer->GetMAX_HP();
	//C_VS_UI_SLAYER_PDS::m_MP_MAX	= g_pPlayer->GetMAX_MP();
	//C_VS_UI_SLAYER_PDS::m_th_max	= g_pPlayer->GetTOHIT(); // (!)
	//C_VS_UI_SLAYER_PDS::m_ac_max	= g_pPlayer->GetAC();	 // (!)
	//C_VS_UI_SLAYER_PDS::m_cc_max	= g_pPlayer->GetCARRYWEIGHT(); // (!)	
}
*/

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

 

//-----------------------------------------------------------------------------
//
// GameMenu Continue
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_GAMEMENU_CONTINUE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_GAMEMENU_CONTINUE");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.CloseGameMenu();
}

//-----------------------------------------------------------------------------
//
// Minimap Toggle
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_MINIMAP_TOGGLE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_MINIMAP_TOGGLE");
	
	g_pUserOption->DrawMinimap = !g_pUserOption->DrawMinimap;
}
*/

//-----------------------------------------------------------------------------
//
// Drop Money
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_DROP_MONEY(int left, int right, void* void_ptr)
{
	return;
	//
	
	//
	// left = money
	//
	DEBUG_ADD("[UI] UI_DROP_MONEY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	if (left > 0)
	{
		if (g_pMoneyManager!=NULL)
		{
				CGDropMoney _CGDropMoney;
				_CGDropMoney.setAmount( left );

				g_pSocket->sendPacket( &_CGDropMoney );

				

			//-----------------------------------------------
			
			//-----------------------------------------------
			g_pMoneyManager->UseMoney( left );
		}
		else
		{
			DEBUG_ADD("[Error] MoneyManager is NULL");
		}
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLICK_BONUS_POINT(int left, int right, void* void_ptr)
{
	//
	
	//
	
	//
	// left = { 0:STR, 1:DEX, 2:INT }
	//
	DEBUG_ADD("[UI] UI_CLICK_BONUS_POINT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	const int bonusPart[] =
	{
		INC_STR,
		INC_DEX,
		INC_INT
	};

	
	if ((*g_pTempInformation).Mode==TempInformation::MODE_NULL)
	{
		int bonusPoint = g_pPlayer->GetBonusPoint();

		//----------------------------------------------
		
		//----------------------------------------------
		
//		if (!g_pPlayer->IsSlayer()// || g_pPlayer->IsOusters()
//			&& bonusPoint > 0)
		if(bonusPoint > 0)
		
		{
			
				CGUseBonusPoint _CGUseBonusPoint;

				_CGUseBonusPoint.setWhich( bonusPart[left] );

				g_pSocket->sendPacket( &_CGUseBonusPoint );

				
				//----------------------------------------------
				
				//----------------------------------------------
				(*g_pTempInformation).Mode = TempInformation::MODE_BONUSPOINT_USE;
				(*g_pTempInformation).Value1 = bonusPart[left];				
		
				
		}
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_INFO(int left, int right, void* void_ptr)
{
	//
	
	//
//	DEBUG_ADD("[UI] UI_CHARINFO");
//	
//	
//	if (g_Mode!=MODE_GAME)
//	{
//		DEBUG_ADD("Not Mode MODE_GAME");
//		return;
//	}
//
//
//	if (gC_vs_ui.IsRunningElevator())
//	{
//		DEBUG_ADD("[Execute_UI_CHARINFO] before RunCharInfo-Elevator Running");
//	}
//
//	//----------------------------------------------

//	//----------------------------------------------
//	if (gC_vs_ui.IsRunningCharInfo())
//	{
//		gC_vs_ui.CloseCharInfo();
//	}
//	//----------------------------------------------

//	//----------------------------------------------
//	else
//	{
//		// set slot
//

//		/*
//		g_char_slot_ingame.sz_name = "yaho";
//		g_char_slot_ingame.bl_vampire = true;//false;
//		g_char_slot_ingame.MP_MAX = 100;
//		g_char_slot_ingame.HP_MAX = 100;
//		g_char_slot_ingame.HP = 100;
//		g_char_slot_ingame.MP = 100;
//		g_char_slot_ingame.bl_female = false;
//
//		g_char_slot_ingame.bonus_point = 5;
//		*/
//		g_char_slot_ingame.sz_name = g_pUserInformation->CharacterID.GetString();
//		g_char_slot_ingame.bl_vampire = g_pPlayer->IsVampire();
//		g_char_slot_ingame.bl_female = !g_pPlayer->IsMale();
//		
//		
//		if (g_pPlayer->IsSlayer())
//		{
//			g_char_slot_ingame.level = 1;//g_pPlayer->GetLEVEL();
//
//			//g_char_slot_ingame.skin_color = g_pPlayer->GetAddonInfo(ADDON_COAT).ColorSet1;
//			//g_char_slot_ingame.hair_color = g_pPlayer->GetAddonInfo(ADDON_HAIR).ColorSet1;
//			g_char_slot_ingame.hair_color = g_pUserInformation->HairColor;
//			g_char_slot_ingame.skin_color = g_pUserInformation->SkinColor;
//		}
//		else
//		{
//			g_char_slot_ingame.level = g_pPlayer->GetVampExp();
//
//			//g_char_slot_ingame.skin_color = g_pPlayer->GetBodyColor1();//pCreatureWear->GetAddonInfo(ADDON_COAT).ColorSet1;
//			//g_char_slot_ingame.hair_color = pCreatureWear->GetAddonInfo(ADDON_HAIR).ColorSet1;
//			g_char_slot_ingame.hair_color = g_pUserInformation->HairColor;
//			g_char_slot_ingame.skin_color = g_pUserInformation->SkinColor;
//		}
//		
//		if (g_pPlayer->IsMale())
//		{
//			switch (g_pUserInformation->FaceStyle)
//			{
//				case HAIR_STYLE1 :
//					g_char_slot_ingame.man_info.face = M_FACE1;
//				break;
//				case HAIR_STYLE2 :
//					g_char_slot_ingame.man_info.face = M_FACE2;
//				break;
//				case HAIR_STYLE3 :
//					g_char_slot_ingame.man_info.face = M_FACE3;
//				break;
//			}
//		}
//		else
//		{
//			switch (g_pUserInformation->FaceStyle)
//			{
//				case HAIR_STYLE1 :
//					g_char_slot_ingame.woman_info.face = W_FACE1;
//				break;
//				case HAIR_STYLE2 :
//					g_char_slot_ingame.woman_info.face = W_FACE2;
//				break;
//				case HAIR_STYLE3 :
//					g_char_slot_ingame.woman_info.face = W_FACE3;
//				break;
//			}
//		}
//
//		g_char_slot_ingame.level = g_pPlayer->GetLEVEL();
//		g_char_slot_ingame.alignment = (ALIGNMENT)ConvertAlignment( g_pPlayer->GetAlignment() );
//		g_char_slot_ingame.EXP_CUR	= g_pPlayer->GetVampExp();
//
//		g_char_slot_ingame.STR_CUR =g_pPlayer->GetSTR();
//		g_char_slot_ingame.DEX_CUR =g_pPlayer->GetDEX();
//		g_char_slot_ingame.INT_CUR =g_pPlayer->GetINT();
//		g_char_slot_ingame.STR_MAX =g_pPlayer->GetMAX_STR();
//		g_char_slot_ingame.DEX_MAX =g_pPlayer->GetMAX_DEX();
//		g_char_slot_ingame.INT_MAXX =g_pPlayer->GetMAX_INT();
//		g_char_slot_ingame.STR_EXP_CUR =g_pPlayer->GetSTR_EXP();
//		g_char_slot_ingame.DEX_EXP_CUR =g_pPlayer->GetDEX_EXP();
//		g_char_slot_ingame.INT_EXP_CUR =g_pPlayer->GetINT_EXP();

		// test code
//		g_char_slot_ingame.STR_EXP_NEXT_LEVEL	= 10000;
//		g_char_slot_ingame.DEX_EXP_NEXT_LEVEL	= 10000;
		//g_char_slot_ingame.INT_EXP_NEXT_LEVEL	= 10000;
//		g_char_slot_ingame.STR_EXP_CUR_LEVEL	= 5000;
//		g_char_slot_ingame.DEX_EXP_CUR_LEVEL	= 5000;
//		g_char_slot_ingame.INT_EXP_CUR_LEVEL	= 5000;


		//g_char_slot_ingame.DAM =g_pPlayer->GetDAM();
		//g_char_slot_ingame.AC =g_pPlayer->GetAC();
		//g_char_slot_ingame.TOHIT =g_pPlayer->GetTOHIT();
//		g_char_slot_ingame.HP =g_pPlayer->GetHP();
//		g_char_slot_ingame.MP =g_pPlayer->GetMP();
//		g_char_slot_ingame.HP_MAX =g_pPlayer->GetMAX_HP();
//		g_char_slot_ingame.MP_MAX =g_pPlayer->GetMAX_MP();
//		g_char_slot_ingame.SILVER_HP = g_pPlayer->GetSilverDamage();
//		//g_char_slot_ingame.CC =g_pPlayer->GetCARRYWEIGHT();
//		g_char_slot_ingame.FAME =g_pPlayer->GetFAME();
//		g_char_slot_ingame.NOTERITY =g_pPlayer->GetNotoriety();
//
//		DEBUG_ADD("[UI] CalculateStatus");
//		
//		g_pPlayer->CalculateStatus();
//
//		DEBUG_ADD("[UI] RunCharInfo");
//		
//
//		gC_vs_ui.RunInfo();
//	}
//
//	if (gC_vs_ui.IsRunningElevator())
//	{
//		DEBUG_ADD("[Execute_UI_CHARINFO] after RunCharInfo-Elevator Running");
//	}
//
//	DEBUG_ADD("[UI] UI_CHARINFO OK");
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
//
// void_ptr = Mitem*
//
void
UIMessageManager::Execute_UI_REPAIR_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_REPAIR_ITEM");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = NULL;// = (MItem*)void_ptr;
		int itemID = left;

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		if (itemID==0)
		{
			//-----------------------------------------------------
			
			//-----------------------------------------------------
				CGRequestRepair	_CGRequestRepair;

				_CGRequestRepair.setObjectID( 0 );
				
				g_pSocket->sendPacket( &_CGRequestRepair );
			
				//-------------------------------------------------
				
				//-------------------------------------------------
				(*g_pTempInformation).Mode		= TempInformation::MODE_SHOP_REPAIR;
				(*g_pTempInformation).Value1	= false;	
				(*g_pTempInformation).Value2	= true;		
				(*g_pTempInformation).pValue	= NULL;

				//-------------------------------------------------
				
				//-------------------------------------------------
				UI_LockItemTrade();
		}
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		else
		{
			BOOL bInInventory = FALSE;
			BOOL bInGear = FALSE;

			//------------------------------------------------------------------------------
			
			//------------------------------------------------------------------------------
			pItem = g_pInventory->GetItemToModify( itemID );

			if (pItem==NULL)
			{
				//------------------------------------------------------------------------------
				
				//------------------------------------------------------------------------------
				switch(g_pPlayer->GetRace())
				{
				case RACE_SLAYER:
					pItem = g_pSlayerGear->GetItemToModify( itemID );
					
					if (pItem!=NULL)
					{
						bInGear = TRUE;
					}
					break;

				case RACE_VAMPIRE:
					pItem = g_pVampireGear->GetItemToModify( itemID );
					
					if (pItem!=NULL)
					{
						bInGear = TRUE;
					}
					break;

				case RACE_OUSTERS:
					pItem = g_pOustersGear->GetItemToModify( itemID );
					
					if (pItem!=NULL)
					{
						bInGear = TRUE;
					}
					break;
				}
			}
			else
			{
				bInInventory = TRUE;
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			if (bInInventory || bInGear)
			{
				//-------------------------------------------------
				
				
				//-------------------------------------------------
				if (pItem->GetItemClass() != ITEM_CLASS_VAMPIRE_AMULET
					|| !pItem->IsUniqueItem()
					|| !pItem->IsQuestItem()
					|| pItem->GetMaxDurability() != -1
					|| pItem->GetItemClass()==ITEM_CLASS_KEY
					|| pItem->IsChargeItem())
				{
					//-------------------------------------------------
					
					//-------------------------------------------------
					int price = g_pPriceManager->GetItemPrice(pItem, MPriceManager::REPAIR);
					int money = g_pMoneyManager->GetMoney();

					//-------------------------------------------------					
					
					
					//-------------------------------------------------					
					if (price!=0 || pItem->GetItemClass()==ITEM_CLASS_KEY)
					{
						//-------------------------------------------------
						
						//-------------------------------------------------
						if (money!=0 && price <= money)
						{														
							//-----------------------------------------------------
							
							//-----------------------------------------------------
								CGRequestRepair	_CGRequestRepair;

								_CGRequestRepair.setObjectID( pItem->GetID() );
								
								g_pSocket->sendPacket( &_CGRequestRepair );
							
								//-------------------------------------------------
								
								//-------------------------------------------------
								(*g_pTempInformation).Mode		= TempInformation::MODE_SHOP_REPAIR;
								(*g_pTempInformation).Value1	= bInInventory;	
								(*g_pTempInformation).Value2	= bInGear;		
								(*g_pTempInformation).pValue	= (void*)pItem;

								//-------------------------------------------------
								
								//-------------------------------------------------
								UI_LockItemTrade();
						}
						//-------------------------------------------------
						
						//-------------------------------------------------					
						else
						{
							g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_MONEY ].GetString());
						}
					}
					//-------------------------------------------------
					
					//-------------------------------------------------
					else
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_REPAIR].GetString());
					}
				}
				//-------------------------------------------------
				
				//-------------------------------------------------
				else
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_REPAIR].GetString());
				}
			}		
		}
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
//
// void_ptr = Mitem*
//
void
UIMessageManager::Execute_UI_SILVERING_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_SILVERING_ITEM");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = NULL;// = (MItem*)void_ptr;
		int itemID = left;

		BOOL bInInventory = FALSE;
		BOOL bInGear = FALSE;

		//------------------------------------------------------------------------------
		
		//------------------------------------------------------------------------------
		pItem = g_pInventory->GetItemToModify( itemID );

		if (pItem==NULL)
		{
			//------------------------------------------------------------------------------
			
			//------------------------------------------------------------------------------
			switch(g_pPlayer->GetRace())
			{
			case RACE_SLAYER:
				pItem = g_pSlayerGear->GetItemToModify( itemID );
				
				if (pItem!=NULL)
				{
					bInGear = TRUE;
				}
				break;

			case RACE_VAMPIRE:
				pItem = g_pVampireGear->GetItemToModify( itemID );
				
				if (pItem!=NULL)
				{
					bInGear = TRUE;
				}
				break;

			case RACE_OUSTERS:
				pItem = g_pOustersGear->GetItemToModify( itemID );
				
				if (pItem!=NULL)
				{
					bInGear = TRUE;
				}
				break;
			}
		}
		else
		{
			bInInventory = TRUE;
		}

		//-------------------------------------------------
		
		//-------------------------------------------------
		if (bInInventory || bInGear)
		{
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (pItem->GetSilverMax() > 0)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				int price = (*g_pPriceManager).GetItemPrice(pItem, MPriceManager::SILVERING);
				int money = (*g_pMoneyManager).GetMoney();

				//-------------------------------------------------					
				
				//-------------------------------------------------					
				if (price!=0)
				{
					//-------------------------------------------------
					
					//-------------------------------------------------
					if (money!=0 && price <= money)
					{														
						//-----------------------------------------------------
						
						//-----------------------------------------------------
							CGSilverCoating	_CGSilverCoating;

							_CGSilverCoating.setObjectID( pItem->GetID() );
							
							g_pSocket->sendPacket( &_CGSilverCoating );
						
							//-------------------------------------------------
							
							//-------------------------------------------------
							(*g_pTempInformation).Mode		= TempInformation::MODE_SHOP_SILVERING;
							(*g_pTempInformation).Value1	= bInInventory;	
							(*g_pTempInformation).Value2	= bInGear;		
							(*g_pTempInformation).pValue	= (void*)pItem;

							//-------------------------------------------------
							
							//-------------------------------------------------
							UI_LockItemTrade();
					}
					//-------------------------------------------------
					
					//-------------------------------------------------					
					else
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_MONEY ].GetString());
					}
				}
				//-------------------------------------------------
				
				//-------------------------------------------------
				else
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SILVERING].GetString());
				}
			}
			//-------------------------------------------------
			
			//-------------------------------------------------
			else
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SILVERING].GetString());
			}
		}		
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_REPAIR_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_REPAIR_FINISHED");
	
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.FinishItemRepairing();

		
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_SILVERING_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_SILVERING_FINISHED");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	if (g_pTempInformation!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.FinishItemSilvering();

		
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_FINISH_LEVELUP_BUTTON(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_FINISH_LEVELUP_BUTTON");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.FinishLevelUp();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_RUNNING_GAMEMENU(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_RUNNING_GAMEMENU");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
//	g_pUIDialog->CloseHelpDlg();
	g_pUIDialog->CloseMessageDlg();
	g_pUIDialog->ClosePCTalkDlg();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

void
UIMessageManager::Execute_UI_STORAGE_BUY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_STORAGE_BUY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL buy = (BOOL)left;

	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	if (buy)
	{
		if (g_pTempInformation->GetMode()==TempInformation::MODE_STORAGE_BUY)
		{		
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			if (g_pMoneyManager->GetMoney() >= g_pTempInformation->Value1)
			{
					CGStashRequestBuy _CGStashRequestBuy;
					
					g_pSocket->sendPacket( &_CGStashRequestBuy );					

				
				g_pTempInformation->SetMode(TempInformation::MODE_STORAGE_BUY_WAIT);

			}
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			else
			{
				
				g_pUIDialog->ShowPCTalkDlg();

				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_MONEY ].GetString());
				
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}
		}
		else
		{
			DEBUG_ADD("[Error] Mode is Not MODE_STORAGE_BUY");
		}

		
		
	}
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	else
	{
		
		g_pUIDialog->ShowPCTalkDlg();

		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
	}					
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
// left = tab (0, 1, 2)
// right = slot
//
void
UIMessageManager::Execute_UI_SELECT_STORAGE_SLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_SELECT_STORAGE_SLOT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if(g_pStorage == NULL)
		return;

	g_pStorage->SetCurrent( left );	

	int slot = right;

	MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();
	const MItem* pStorageItem = g_pStorage->GetItem( slot );

	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	if (pMouseItem==NULL)
	{
		//---------------------------------------------------
		
		//---------------------------------------------------
		if (pStorageItem!=NULL)
		{
			MItem* pRemovedItem = g_pStorage->RemoveItem( slot );

			if (pRemovedItem!=NULL)
			{
				 UI_PickUpItem( (MItem*)pStorageItem );

					CGStashToMouse _CGStashToMouse;
					_CGStashToMouse.setObjectID( pStorageItem->GetID() );
					_CGStashToMouse.setRack( g_pStorage->GetCurrent() );
					_CGStashToMouse.setIndex( slot );									

					g_pSocket->sendPacket( &_CGStashToMouse );					
			}
		}
	}
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	else 
	{
		BOOL bSendPacket = TRUE;

		TYPE_OBJECTID mouseItemID = pMouseItem->GetID();		
	
		
		if (pMouseItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX
			&& pMouseItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_AMULET
			&& pMouseItem->GetItemClass()!=ITEM_CLASS_COUPLE_RING
			&& pMouseItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_COUPLE_RING
			&& !pMouseItem->IsQuestItem()
			&& pMouseItem->IsNormalItem()
			&& !((pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_TREE) && pMouseItem->GetItemType() >= 13  )
			&& pMouseItem->GetItemClass() != ITEM_CLASS_EVENT_ITEM
			&& pMouseItem->GetItemClass() != ITEM_CLASS_CODE_SHEET
			&& !(pMouseItem->GetItemClass() == ITEM_CLASS_LUCKY_BAG && pMouseItem->GetItemType() == 3)
			)
		{
			//---------------------------------------------------
			
			//---------------------------------------------------
			if (pStorageItem==NULL)
			{
				UI_DropItem();	

				g_pStorage->SetItem( slot, pMouseItem );							
			}
			//---------------------------------------------------
			
			//---------------------------------------------------
			else
			{
				//---------------------------------------------------
				
				//---------------------------------------------------
				if (pStorageItem->IsPileItem()
					&& pStorageItem->GetItemClass()==pMouseItem->GetItemClass()
					&& pStorageItem->GetItemType()==pMouseItem->GetItemType())
				{
					//---------------------------------------------------
					
					//---------------------------------------------------
					if (pStorageItem->GetNumber() < pStorageItem->GetMaxNumber())
					{
						PlaySound( pMouseItem->GetInventorySoundID() );
						
						MItem* pModifyStorageItem = g_pStorage->RemoveItem( slot );

						//----------------------------------------------------
						
						//----------------------------------------------------
						int total = pMouseItem->GetNumber() + pStorageItem->GetNumber();
						if ( total > pStorageItem->GetMaxNumber() )
						{
							
							pMouseItem->SetNumber( total - pStorageItem->GetMaxNumber() );
							pModifyStorageItem->SetNumber( pStorageItem->GetMaxNumber() );
						}
						else
						{
							
							pModifyStorageItem->SetNumber( total );
							UI_DropItem();

							delete pMouseItem;
						}	

						//----------------------------------------------------
						
						//----------------------------------------------------
						g_pStorage->SetItem( slot, pModifyStorageItem );					
						
					}
					//---------------------------------------------------
					
					//---------------------------------------------------				
					else
					{
						bSendPacket = FALSE;
					}
				}
				//---------------------------------------------------				
				
				//---------------------------------------------------				
				else
				{				
					MItem* pTempItem = pMouseItem;
					
					UI_PickUpItem( (MItem*)pStorageItem );

					g_pStorage->RemoveItem( slot );	

					g_pStorage->SetItem( slot, pTempItem );
				}
			}

				if (bSendPacket)
				{		
					CGMouseToStash _CGMouseToStash;
					_CGMouseToStash.setObjectID( mouseItemID );
					_CGMouseToStash.setRack( g_pStorage->GetCurrent() );
					_CGMouseToStash.setIndex( slot );									

					g_pSocket->sendPacket( &_CGMouseToStash );				
				}
		}
		else
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_STORAGE].GetString());
		}


	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
// left = money
//
void
UIMessageManager::Execute_UI_DEPOSIT_MONEY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_DEPOSIT_MONEY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	if (left > 0)
	{
		if (g_pMoneyManager!=NULL && g_pStorage!=NULL)
		{
				CGStashDeposit _CGStashDeposit;
				_CGStashDeposit.setAmount( left );

				g_pSocket->sendPacket( &_CGStashDeposit );				

			//-----------------------------------------------
			
			//-----------------------------------------------
			g_pMoneyManager->UseMoney( left );
			g_pStorage->GetMoneyManager()->AddMoney( left );
		}
		else
		{
			DEBUG_ADD("[Error] MoneyManager or Storage is NULL");
		}
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
// left = money
//
void
UIMessageManager::Execute_UI_WITHDRAW_MONEY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_WITHDRAW_MONEY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	if (left > 0)
	{
		if (g_pMoneyManager!=NULL && g_pStorage!=NULL)
		{
				CGStashWithdraw _CGStashWithdraw;
				_CGStashWithdraw.setAmount( left );

				g_pSocket->sendPacket( &_CGStashWithdraw );				

			//-----------------------------------------------
			
			//-----------------------------------------------
			g_pStorage->GetMoneyManager()->UseMoney( left );
			g_pMoneyManager->AddMoney( left );						
		}
		else
		{
			DEBUG_ADD("[Error] MoneyManager or Storage is NULL");
		}
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_STORAGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_STORAGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseStorage();		

	
	g_pUIDialog->ShowPCTalkDlg();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

// right = money
//
void
UIMessageManager::Execute_UI_EXCHANGE_MONEY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_EXCHANGE_MONEY");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTradeManager!=NULL)
	{
		BOOL bGive = left;
		int money = right;
			
		BOOL bAcceptMyTrade = g_pTradeManager->IsAcceptMyTrade();

		int code;		

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (bAcceptMyTrade)
		{	
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
			{
				if (bGive)
				{
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_ADD_MONEY);
				}
				else
				{
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_REMOVE_MONEY);

					g_pTradeManager->SetNextAcceptTime();
				}

				g_pTempInformation->Value1	= money;
			}
		}
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		else
		{
			 

			if (!bGive)
			{
				g_pTradeManager->SetNextAcceptTime();
			}
		}	

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		if (bGive)
		{
			code = CG_TRADE_MONEY_INCREASE;
		}
		else
		{
			code = CG_TRADE_MONEY_DECREASE;
		}
		
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
			CGTradeMoney _CGTradeMoney;
			_CGTradeMoney.setTargetObjectID( g_pTradeManager->GetOtherID() );
			_CGTradeMoney.setAmount( money );
			_CGTradeMoney.setCode( code );				

			g_pSocket->sendPacket( &_CGTradeMoney );			

//			g_pTradeManager->SetNextAcceptTime();
	}
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
	}


}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
// gridXY = (left,right)

 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
// gridXY = (left, right)

/*
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_TRADEGRID(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_TRADEGRID");
	
	int		gridX = left;
	int		gridY = right;
	MItem*	pItem = (MItem *)void_ptr;
	MItem*	pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();
}
*/

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
// gridXY = (left, right)

/*
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_TRADEGRID(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_TRADEGRID");
	
	int		gridX = left;
	int		gridY = right;
	MItem*	pItem = (MItem *)void_ptr;
}
*/

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
// gridXY = (left, right)

void
UIMessageManager::Execute_UI_ITEM_SELECT_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_SELECT_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTradeManager!=NULL)
	{
		int		gridX = left;
		int		gridY = right;
		MItem*	pItem = (MItem *)void_ptr;

		BOOL bAcceptMyTrade = g_pTradeManager->IsAcceptMyTrade();		

		
		if(pItem->GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX && pItem->GetItemType() > 1 && pItem->IsTrade() == false)	//
		{
			MEventGiftBoxItemFinder finder;
			MItem* pFindItem = ((MItemManager*)g_pInventory)->FindItem( finder );
			if(pFindItem != NULL)
			{
				
				return;
			}
		}
	

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (bAcceptMyTrade)
		{	
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
				&& g_pPlayer->IsItemCheckBufferNULL())
			{
				g_pTempInformation->Value1 = pItem->GetID();
				g_pTempInformation->Value2 = gridX;
				g_pTempInformation->Value3 = gridY;
				g_pTempInformation->pValue = pItem;

				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				if (pItem->IsTrade())
				{
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_REMOVE_ITEM);

					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_TRADE_REMOVE );

						CGTradeRemoveItem _CGTradeRemoveItem;
						_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeRemoveItem );

						g_pTradeManager->SetNextAcceptTime();

					
//					__BEGIN_HELP_EVENT
////						ExecuteHelpEvent( HE_TRADE_ITEM_REMOVE );	
//					__END_HELP_EVENT
				}
				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				else
				{
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_ADD_ITEM);

					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_TRADE_ADD );

						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeAddItem );
						

					
//					__BEGIN_HELP_EVENT
////						ExecuteHelpEvent( HE_TRADE_ITEM_ADD );	
//					__END_HELP_EVENT
				}				
			}			
		}
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		else
		{		
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			if (pItem->IsTrade())
			{
				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				g_pTradeManager->RefuseOtherTrade();
				g_pTradeManager->RefuseMyTrade();

				pItem->UnSetTrade();		

					CGTradeRemoveItem _CGTradeRemoveItem;
					_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
					_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

					g_pSocket->sendPacket( &_CGTradeRemoveItem );

					g_pTradeManager->SetNextAcceptTime();

				
//				__BEGIN_HELP_EVENT
////					ExecuteHelpEvent( HE_TRADE_ITEM_REMOVE );	
//				__END_HELP_EVENT
			}
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			else
			{
				
				if (pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX
					&& pItem->GetItemType()==1)
				{
				}
				else
				{
					
					if (pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX)// && pItem->GetItemClass() < 2)
					{
						g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_ADD_ITEM);
						g_pTempInformation->pValue	= pItem;
						
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_TRADE_ADD );
					}
					
					else
					{
						//-----------------------------------------------------------
						
						//-----------------------------------------------------------
						g_pTradeManager->RefuseOtherTrade();
						g_pTradeManager->RefuseMyTrade();

						pItem->SetTrade();		
					}

						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeAddItem );
						

					
//					__BEGIN_HELP_EVENT
////						ExecuteHelpEvent( HE_TRADE_ITEM_ADD );	
//					__END_HELP_EVENT
				}
			}	
		}
	}
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_OK_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_OK_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTradeManager!=NULL
		&& g_pTradeManager->IsAcceptTime())
	{
		MItem* pMouseItem = UI_GetMouseItem();

		//--------------------------------------------------------
		
		
		//--------------------------------------------------------
		if ((pMouseItem==NULL || pMouseItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX)
			&& g_pTradeManager->CanTrade())
		{
			g_pTradeManager->AcceptMyTrade();
			
				CGTradeFinish _CGTradeFinish;
				_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeFinish.setCode( CG_TRADE_FINISH_ACCEPT );

				g_pSocket->sendPacket( &_CGTradeFinish );
				
		}
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		else
		{
			
			
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TRADE_NO_SPACE].GetString() );
		}
	}
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL or NotAcceptTime");
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CANCEL_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CANCEL_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL bSendPacket = FALSE;

	if (g_pTradeManager!=NULL)
	{	
		BOOL bAcceptMyTrade = g_pTradeManager->IsAcceptMyTrade();

		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		if (bAcceptMyTrade)
		{
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
			{
				g_pTempInformation->SetMode(g_pTempInformation->MODE_TRADE_VERIFY_CANCEL);

				bSendPacket = TRUE;
			}
			else
			{
				DEBUG_ADD( "[Error] Mode is not MODE_NULL");
			}
		}
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		else
		{		
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			g_pTradeManager->RefuseMyTrade();
			
			bSendPacket = TRUE;
		}
	}	
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
	}

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	if (bSendPacket)
	{
			CGTradeFinish _CGTradeFinish;
			_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
			_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

			g_pSocket->sendPacket( &_CGTradeFinish );
			
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());

	BOOL bSendPacket = FALSE;

	int otherID;

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	if (bAcceptMyTrade)
	{
		if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
		{
			g_pTempInformation->SetMode(g_pTempInformation->MODE_TRADE_VERIFY_CLOSE);
			
			otherID = g_pTradeManager->GetOtherID();

			bSendPacket = TRUE;
		}
		else
		{
			DEBUG_ADD( "[Error] Mode is not MODE_NULL");
		}
	}
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	else
	{		
		if (g_pTradeManager!=NULL)
		{			
			otherID = g_pTradeManager->GetOtherID();

			g_pTradeManager->CancelTrade();

			bSendPacket = TRUE;
		}

		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		UI_CloseExchange();	
	}
	
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	if (bSendPacket)
	{
			CGTradeFinish _CGTradeFinish;
			_CGTradeFinish.setTargetObjectID( otherID );
			_CGTradeFinish.setCode( CG_TRADE_FINISH_REJECT );

			g_pSocket->sendPacket( &_CGTradeFinish );
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------


/*
void
UIMessageManager::Execute_UI_CHAT_SELECT_NAME(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CHAT_SELECT_NAME");
	
	char str[80];

	const char* pID = (const char*)void_ptr;
		
	switch (left)
	{
		case M_LEFTBUTTON_DOWN :
			sprintf(str, "/%s ", pID);
			gC_vs_ui.SetInputString( str );
		break;

		case M_RIGHTBUTTON_DOWN :
			#ifdef OUTPUT_DEBUG
				sprintf(str, "*trace %s", pID);
				gC_vs_ui.SetInputString( str );
			#else
				gC_vs_ui.AddInputString( pID );
			#endif
		break;
	}
}
*/

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

//
void
UIMessageManager::Execute_UI_EXCHANGE_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_EXCHANGE_ACCEPT");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL accept = (BOOL)left;

	int code;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_TRADE_REQUEST)
	{
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (accept)
		{
			code = CG_TRADE_PREPARE_CODE_ACCEPT;

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			UI_RunExchange( g_pTempInformation->Value1 );	
		}
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		else
		{	
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE)
			{
				g_pPlayer->SetWaitVerifyNULL();
			}

			code = CG_TRADE_PREPARE_CODE_REJECT;
		}

			CGTradePrepare _CGTradePrepare;
			_CGTradePrepare.setTargetObjectID( g_pTempInformation->Value1 );
			_CGTradePrepare.setCode( code );				

			g_pSocket->sendPacket( &_CGTradePrepare );			
	
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);		
	}	
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_EXCHANGE_REQUEST_CANCEL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_EXCHANGE_REQUEST_CANCEL");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_TRADE_REQUEST)
	{	
		if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE)
		{
			g_pPlayer->SetWaitVerifyNULL();
		}

			CGTradePrepare _CGTradePrepare;
			_CGTradePrepare.setTargetObjectID( g_pTempInformation->Value1 );
			_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_CANCEL );				

			g_pSocket->sendPacket( &_CGTradePrepare );			
	
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		
		
		UI_CloseExchangeCancel();
	}	
}

 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------


void
UIMessageManager::Execute_UI_CHANGE_OPTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CHANGE_TITLE_OPTION");
	
//	if (g_Mode!=MODE_MAINMENU)
//	{
//		DEBUG_ADD("Not Mode MODE_MAINMENU");
//		return;
//	}

	
	int value = right;

	switch (left)
	{
		//-------------------------------------------------------------------
		// USE 3D HAL
		//-------------------------------------------------------------------
//		case C_VS_UI_OPTION::CHECK_3D :
//			g_pUserOption->Use3DHAL = (BOOL)value;
//		break;

		//-------------------------------------------------------------------
		// DRAW_MINIMAP
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_MINIMAP :
		//	g_pUserOption->DrawMinimap = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_ZONE_NAME
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_ZONE_NAME :
		//	g_pUserOption->DrawZoneName = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_GAME_TIME
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_GAME_TIME :
		//	g_pUserOption->DrawGameTime = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_BLENDING_CHARACTER_SHADOW
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_BLENDING_CHARACTER_SHADOW :
	//		g_pUserOption->BlendingShadow = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// PLAY_WAV
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_WAV :
			g_pUserOption->PlayWaveMusic = (BOOL)value;

			if(g_Mode == MODE_MAINMENU)
				PlayTitleMusic();			
			else 
				PlayGameMusic();			
		break;

		//-------------------------------------------------------------------
		// BLOOD_DROP
		//-------------------------------------------------------------------
//		case C_VS_UI_OPTION::BLOOD_DROP :
//			g_pUserOption->BloodDrop = (BOOL)value;
//		break;

		//-------------------------------------------------------------------
		// PLAY_SOUND
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_SOUND :
			g_pUserOption->PlaySound = (BOOL)value;

			if (g_pUserOption->PlaySound)
			{
				g_SDLAudio.UnSetMute();
			}
			else
			{
				if (g_pSoundManager != NULL)
				{
					g_pSoundManager->Stop();
				}
				
				g_SDLAudio.SetMute();
			}
		break;

		//-------------------------------------------------------------------
		// PLAY_MUSIC
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_MUSIC :
			g_pUserOption->PlayMusic = (BOOL)value;

			PlayTitleMusic();			
		break;

		//-------------------------------------------------------------------
		// VOLUME_SOUND
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_VALUE_SOUND_VOLUME :
			if (value>=0 && value<16)
			{
				g_pUserOption->VolumeSound = value;	

				LONG volume = value*SOUND_DEGREE + SOUND_MIN;

				g_SDLAudio.SetVolumeLimit( volume );	
				
				PlaySound(SOUND_SLAYER_BUTTON);
			}
		break;

		//-------------------------------------------------------------------
		// VOLUME_MUSIC 
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_VALUE_MUSIC_VOLUME :

			if (value>=0 && value<16)
			{
				g_pUserOption->VolumeMusic = value;
				
				if (g_pUserOption->PlayWaveMusic)
				{
					 
//					LONG volume = value*16*257;//*SOUND_DEGREE + SOUND_MIN;

//					DEBUG_ADD_FORMAT("Change VOLUME_MUSIC = %ld", volume);
					
//					g_pSDLStream->SetVolumeLimit( volume );
					DEBUG_ADD("MP3 SetVolume4");
#ifdef __USE_MP3__
					g_pMP3->SetVolume( volume );
#else
					if( g_SDLAudio.IsInit() && g_pOGG != NULL )
					{
						//int step = (DSBVOLUME_MIN) / 16;
						int volume = (value - 15) * 250;

						g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
					}
#endif
					DEBUG_ADD("MP3 SetVolume4 OK");
				}
				else
				{
					WORD volume = (value)*0x1000 + 0x0FFF;

					if (!g_pUserOption->PlayMusic || g_Music.IsPause())
					{
						g_Music.SetVolume( volume );
					}
					else
					{
						g_Music.Pause();
						g_Music.SetVolume( volume );
						g_Music.Resume();
					}				
				}
			}
		break;

		//-------------------------------------------------------------------
		
		//-------------------------------------------------------------------
//		case C_VS_UI_OPTION::PARTY_HPBAR_ALPHA :
//			g_pUserOption->DrawTransHPBar = (BOOL)value;
//		break;
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_OPTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_TITLE_OPTION");

//	if (g_Mode!=MODE_MAINMENU)
//	{
//		DEBUG_ADD("Not Mode MODE_MAINMENU");
//		return;
//	}

	
	gC_vs_ui.CloseOption();

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	g_pClientConfig->SaveToFile( g_pFileDef->getProperty("FILE_INFO_CLIENTCONFIG").c_str());
	g_pUserOption->SaveToFile( g_pFileDef->getProperty("FILE_INFO_USEROPTION").c_str());

	if (g_Mode==MODE_MAINMENU)
	{
		// SDL Migration: 3D HAL options are legacy features not used in SDL backend
#ifdef PLATFORM_WINDOWS
		if (true)
		{
			
			if (!g_pUserOption->Use3DHAL)
			{
				g_Mode = MODE_CHANGE_OPTION;
			}
		}
		else
#endif
		{
			
			if (g_bEnable3DHAL && g_pUserOption->Use3DHAL)
			{
				g_Mode = MODE_CHANGE_OPTION;
			}
		}
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_BOOKCASE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_BOOKCASE");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseBookcase();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_BRIEFING(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_BRIEFING");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseBriefing();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_COMPUTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_COMPUTER");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseComputer();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_TUTORIAL_EXIT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_TUTORIAL_EXIT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseTutorialExit();

	int arms = left;	

	if (arms >= 0 && arms <=7)
	{
		ITEM_CLASS itemClass[] =
		{
			ITEM_CLASS_BLADE,
			ITEM_CLASS_SWORD,
			ITEM_CLASS_MACE,
			ITEM_CLASS_CROSS,
			ITEM_CLASS_AR,
			ITEM_CLASS_SR,
			ITEM_CLASS_SMG,
			ITEM_CLASS_SG
		};
		

			CGRequestNewbieItem _CGRequestNewbieItem;
		
			_CGRequestNewbieItem.setItemClass( itemClass[arms] );
			
			g_pSocket->sendPacket( &_CGRequestNewbieItem );
			
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_DESC_DIALOG(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_DESC_DIALOG");

	
	//if (g_Mode!=MODE_GAME)
	{
	//	DEBUG_ADD("Not Mode MODE_GAME");
	//	return;
	}
	
	// Close	
	gC_vs_ui.CloseDescDialog();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_ELEVATOR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_ELEVATOR");
	
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	// Close
	gC_vs_ui.CloseElevator();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_SELECT_ELEVATOR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_SELECT_ELEVATOR");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	
	//		1 : 3
	//		2 : 2
	//		3 : 1
	
	int selectPortal = 4-left;

	if (selectPortal <= 4)
	{
		const MSector& sector = g_pZone->GetSector(g_pPlayer->GetX(), g_pPlayer->GetY());

		if (sector.IsPortal())
		{
			int numPortal = sector.GetPortalSize();		
			
			selectPortal = min(selectPortal, numPortal);

			PORTAL_LIST::const_iterator	iPortal = sector.GetPortalBegin();

			for (int i=0; i<selectPortal; i++)
			{
				iPortal++;		
			}		

			PORTAL_INFO portalInfo = *iPortal;

			int zoneID = portalInfo.ZoneID;

			
			//-----------------------------------------------------
			
			//-----------------------------------------------------
				CGSelectPortal _CGSelectPortal;

				_CGSelectPortal.setZoneID( zoneID );

				g_pSocket->sendPacket( &_CGSelectPortal );
		}
	}

	gC_vs_ui.CloseElevator();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
/*
void	
UIMessageManager::Execute_UI_REQUEST_SERVER_LIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_REQUEST_SERVER_LIST");

	
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}

	
	#ifdef CONNECT_SERVER
		if (gC_vs_ui.IsCharManagerEnable())
		{
			CLGetServerList _CLGetServerList;					
			
			g_pSocket->sendPacket( &_CLGetServerList );

			gC_vs_ui.CharManagerDisable();
		}
	#endif
}
*/

//-----------------------------------------------------------------------------
//
// Inventory --> QuickSlot
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_ITEM_TO_QUICKITEMSLOT(int left, int right, void* void_ptr)
{
//	DEBUG_ADD("[UI] UI_ITEM_TO_QUICKITEMSLOT");
//	// focus_grid_x, focus_grid_y, (MItem *)p_item
//
//	
//	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
//	{
//		DEBUG_ADD("Not Mode MODE_GAME or Dead");
//		return;
//	}
//				
//	//-----------------------------------------------------------------

//	//-----------------------------------------------------------------
//	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL
//		&& g_pPlayer->IsItemCheckBufferNULL())
//	{
//		if (gC_vs_ui.IsRunningExchange())
//		{

//			Execute_UI_ITEM_PICKUP_FROM_INVENTORY(left, right, void_ptr);
//		}
//		else
//		{
//			MItem* pItem = (MItem*)void_ptr;
//
//			//---------------------------------------------------------


//			//---------------------------------------------------------
//			if (pItem->IsQuickItem() && g_pQuickSlot!=NULL)
//			{		
//				int itemID = pItem->GetID();
//				int itemX = pItem->GetGridX();
//				int itemY = pItem->GetGridY();
//
//				int num = g_pQuickSlot->GetSize();
//				int addTotal = -1;
//
//				int addIndex = -1;
//
//				//---------------------------------------------------------

//				//---------------------------------------------------------
//				for (int i=0; i<num; i++)
//				{
//					MItem* pQuickItem = g_pQuickSlot->GetItem( i );
//
//					//---------------------------------------------------------

//					//---------------------------------------------------------
//					if (pQuickItem==NULL)
//					{						
//						addIndex = i;
//
//						MItem* pRemoveItem = g_pInventory->RemoveItem( itemX, itemY );
//
//						g_pQuickSlot->AddItem( pRemoveItem, addIndex );
//
//						PlaySound( pRemoveItem->GetInventorySoundID() );
//
//						break;
//					}
//					//---------------------------------------------------------

//					//---------------------------------------------------------
//					else
//					{
//						//--------------------------------------------------------

//						//--------------------------------------------------------
//						if (pQuickItem->GetItemClass()==pItem->GetItemClass()
//							&& pQuickItem->GetItemType()==pItem->GetItemType())
//						{
//							//----------------------------------------------------

//							//----------------------------------------------------
//							addTotal = pQuickItem->GetNumber() + pItem->GetNumber();
//							if ( addTotal <= pQuickItem->GetMaxNumber() )
//							{

//								addIndex = i;
//
//								//---------------------------------------------------

//								//---------------------------------------------------
//								MItem* pRemoveItem = g_pInventory->RemoveItem( itemX, itemY );
//								if (pRemoveItem!=NULL)
//								{
//									// assert( pRemoveItem==pItem );
//									delete pRemoveItem;
//								}
//
//								pQuickItem->SetNumber( addTotal );
//
//								PlaySound( pQuickItem->GetInventorySoundID() );
//								
//								break;
//							}
//						}
//					}
//				}
//
//				if (addIndex != -1)
//				{
//					//---------------------------------------------------------
//					// SendPacket - Inventory to QuickItem

//					//---------------------------------------------------------
//						//---------------------------------------------------


//						//---------------------------------------------------
//						CGAddInventoryToMouse _CGAddInventoryToMouse;
//						_CGAddInventoryToMouse.setObjectID( itemID );
//						_CGAddInventoryToMouse.setX( itemX );
//						_CGAddInventoryToMouse.setY( itemY );
//						
//						g_pSocket->sendPacket( &_CGAddInventoryToMouse );
//
//						//---------------------------------------------------


//						//---------------------------------------------------
//						CGAddMouseToQuickSlot _CGAddMouseToQuickSlot;
//						_CGAddMouseToQuickSlot.setObjectID( itemID );
//						_CGAddMouseToQuickSlot.setSlotID( addIndex );
//						
//						g_pSocket->sendPacket( &_CGAddMouseToQuickSlot );
//						
//				}
//
//			}
//			//---------------------------------------------------------


//			//---------------------------------------------------------
//			else
//			{
//				//---------------------------------------------------------

//				//---------------------------------------------------------
//				g_pInventory->RemoveItem( pItem->GetID() );
//				Execute_UI_ITEM_PICKUP_FROM_INVENTORY(left, right, void_ptr);
//
//				int maxSlot;
//
//				//---------------------------------------------------------

//				//---------------------------------------------------------
//				if (g_pPlayer->IsSlayer())
//				{
//					maxSlot = MSlayerGear::MAX_GEAR_SLAYER;
//				}
//				else
//				{
//					maxSlot = MVampireGear::MAX_GEAR_VAMPIRE;
//				}
//
//				MItem* pGearItem = NULL;
//				MItem* pChangeItem = NULL;
//				int addSlot = -1;
//
//				for (int slot=0; slot<maxSlot; slot++)
//				{
//					MItem* pGearItem = NULL;
//
//					if (g_pPlayer->IsSlayer() && g_pSlayerGear->CanReplaceItem(pItem, (MSlayerGear::GEAR_SLAYER)slot, pGearItem)
//						|| g_pPlayer->IsVampire() && g_pVampireGear->CanReplaceItem(pItem, (MVampireGear::GEAR_VAMPIRE)slot, pGearItem)
//						|| g_pPlayer->IsOusters() && g_pOustersGear->CanReplaceItem(pItem, (MOustersGear::GEAR_OUSTERS)slot, pGearItem))
//					{
//						addSlot = slot;
//						pChangeItem = pGearItem;
//


//						if (pGearItem==NULL)
//						{
//							break;
//						}							
//					} 
//				}
//				
//				//---------------------------------------------------------

//				//---------------------------------------------------------
//				if (addSlot != -1)
//				{
//					Execute_UI_ITEM_DROP_TO_GEAR(addSlot, 0, pChangeItem);
//				}
//			}
//		}		
//	}
//	//-----------------------------------------------------------------

//	//-----------------------------------------------------------------
//	else
//	{
//		DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer");
//	}
}

//-----------------------------------------------------------------------------
//
// Close Slayer Portal
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_SLAYER_PORTAL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_SLAYER_PORTAL");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


		CGSelectWayPoint _CGSelectWayPoint;
		
		_CGSelectWayPoint.setZoneID( 0 );
		_CGSelectWayPoint.setX( 0 );
		_CGSelectWayPoint.setY( 0 );

		g_pSocket->sendPacket( &_CGSelectWayPoint );

	g_pZone->RemoveHelicopter( g_pPlayer->GetID() );

	UI_CloseSelectWayPoint();
}

//-----------------------------------------------------------------------------
//
// Slayer Portal
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_SLAYER_PORTAL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_SLAYER_PORTAL");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	int zoneID = left;
	int zoneX = HIWORD(right);
	int zoneY = LOWORD(right);
	
	int cost = int( pow(g_pPlayer->GetLEVEL(), 1.3) * 100 );

//	if( g_pMoneyManager->GetMoney() < cost && g_pPlayer->IsOusters() )
//	{
//		UI_PopupMessage( STRING_MESSAGE_WAR_NOT_ENOUGH_MONEY );
//		return;
//	}
	
		CGSelectWayPoint _CGSelectWayPoint;
		
		_CGSelectWayPoint.setZoneID( zoneID );
		_CGSelectWayPoint.setX( zoneX );
		_CGSelectWayPoint.setY( zoneY );

		g_pSocket->sendPacket( &_CGSelectWayPoint );


	if(g_pPlayer->IsSlayer())
	{
		g_pZone->RemoveHelicopter( g_pPlayer->GetID() );
		UI_CloseSelectWayPoint();
	}
	else if(g_pPlayer->IsOusters())
		gC_vs_ui.CloseHorn();
		
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_FINISH_REQUEST_PARTY_BUTTON(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_FINISH_REQUEST_PARTY_BUTTON");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	BOOL bTimeout = left;

	UI_ClosePartyRequest();

	if (bTimeout)
	{
			
			CGPartyInvite _CGPartyInvite;
			_CGPartyInvite.setTargetObjectID( g_pTempInformation->PartyInviter );
			_CGPartyInvite.setCode( CG_PARTY_INVITE_REJECT );	

			g_pSocket->sendPacket( &_CGPartyInvite );		
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_PARTY_REQUEST_CANCEL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PARTY_REQUEST_CANCEL");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_PARTY_REQUEST)
	{	
		if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_PARTY)
		{
			g_pPlayer->SetWaitVerifyNULL();
		}

			CGPartyInvite _CGPartyInvite;
			_CGPartyInvite.setTargetObjectID( g_pTempInformation->Value1 );
			_CGPartyInvite.setCode( CG_PARTY_INVITE_CANCEL );				

			g_pSocket->sendPacket( &_CGPartyInvite );			
	
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		
		
		UI_ClosePartyCancel();
	}	
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_PARTY_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PARTY_ACCEPT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	BOOL accept = (BOOL)left;

	int code;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	//if (g_pTempInformation->Mode == TempInformation::MODE_PARTY_REQUEST)
	{
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (accept)
		{
			code = CG_PARTY_INVITE_ACCEPT;
		}
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		else
		{	
			//if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE)
			{
			//	g_pPlayer->SetWaitVerifyNULL();
			}

			code = CG_PARTY_INVITE_REJECT;
		}

			CGPartyInvite _CGPartyInvite;
			_CGPartyInvite.setTargetObjectID( g_pTempInformation->PartyInviter );
			_CGPartyInvite.setCode( code );				

			g_pSocket->sendPacket( &_CGPartyInvite );			
	
		//g_pTempInformation->Mode = TempInformation::MODE_NULL;		
	}
}

//-----------------------------------------------------------------------------
//
// Close Party Manager
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_PARTY_MANAGER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_PARTY_MANAGER");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	UI_CloseParty();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_AWAY_PARTY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_AWAY_PARTY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	
	
	BOOL bLeftParty = (left==-1);

	if (g_pPlayer->IsWaitVerifyNULL())
	{
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		if (bLeftParty)
		{
			if (g_pParty->GetSize() > 0)
			{
					CGPartyLeave _CGPartyLeave;
				
					//_CGPartyLeave.setTargetName( g_pUserInformation->CharacterID.GetString() );
					
					g_pSocket->sendPacket( &_CGPartyLeave );
					
					g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_PARTY_LEAVE );
			}
		}
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		else
		{
			if (g_pParty->IsKickAvailableTime())
			{
				int kickIndex = left;

				PARTY_INFO*	pInfo = g_pParty->GetMemberInfo( kickIndex );

				if (pInfo!=NULL)
				{
						CGPartyLeave _CGPartyLeave;
					
						_CGPartyLeave.setTargetName( pInfo->Name.GetString() );
						
						g_pSocket->sendPacket( &_CGPartyLeave );			

						g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_PARTY_LEAVE );
				}
			}
			else
			{
				g_pGameMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_NO_AUTHORITY].GetString() );
			}
		}		
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_FINISH_REQUEST_DIE_BUTTON(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_FINISH_REQUEST_DIE_BUTTON");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	
	BOOL bTimeout = left;

	if (g_pPlayer->IsDead() && !g_pPlayer->IsWaitVerify())
	{
		int TempZoneID = g_pPlayer->GetResurrectZoneID();
		if(TempZoneID)
		{
			CGSelectWayPoint _CGSelectWayPoint;

			_CGSelectWayPoint.setZoneID( TempZoneID );
			_CGSelectWayPoint.setX( 0 );
			_CGSelectWayPoint.setY( 0 );

			g_pSocket->sendPacket( &_CGSelectWayPoint );

			g_pPlayer->SetResurrectZoneID(0);
		}
		else
		{
			CGResurrect _CGResurrect;
			
			g_pSocket->sendPacket( &_CGResurrect );
		}	
			
			
		g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_RESURRECT );

		gC_vs_ui.FinishRequestDie();
		gC_vs_ui.FinishRequestResurrect();
		gC_vs_ui.CloseRequestShrineMinimap();
	}
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CONNECT_SERVER(int left, int right, void* void_ptr)
{
//	DEBUG_ADD_FORMAT("[UI] UI_CONNECT_SERVER. %s(%d)", (left? "group" : "server"), right);

	// left == true : group
	//         false : server
    //  right : id

	if(NULL == g_pSocket)
		return;
	
	BOOL bSelectGroup = left;
	int	selectID = right;

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (bSelectGroup)
	{
		if (g_Mode==MODE_WAIT_SELECT_WORLD)
		{
			// CLSelectWorld
				CLSelectWorld clSelectWorld;
			
				clSelectWorld.setWorldID( selectID );				
				g_pUserInformation->WorldID = selectID;

				g_pSocket->sendPacket( &clSelectWorld );				

			g_pServerInformation->SetServerGroupID( selectID );

			SetMode( MODE_WAIT_SERVER_LIST );
		}
		else
		{
			DEBUG_ADD("Not MODE_WAIT_SELECT_WORLD");
		}
	}
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	else
	{
		if (g_Mode==MODE_WAIT_SELECT_SERVER)
		{
			// CLSelectServer
				CLSelectServer clSelectServer;
			
				clSelectServer.setServerGroupID( selectID );				
				g_pUserInformation->ServerID = selectID;

				g_pSocket->sendPacket( &clSelectServer );				

				g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_GET_PC_LIST );

			g_pServerInformation->SetServerID( selectID );			

			SetMode( MODE_WAIT_PCLIST );
		}
		else
		{
			DEBUG_ADD("Not MODE_WAIT_SELECT_SERVER");
		}
	}
		
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_SERVER_SELECT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_SERVER_SELECT");

	// left == true : group 
	//         false : server
	BOOL bSelectGroup = left;

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (bSelectGroup)
	{
		SetMode( MODE_MAINMENU );
	}
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	else
	{
		UI_SetWorldList();

		SetMode( MODE_WAIT_SELECT_WORLD );
	}	
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_NEWCHARACTER_CHECK(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_NEWCHARACTER_CHECK");

	char* pName = (char*)void_ptr;

	if (pName!=NULL)
	{
		if (g_pUserInformation->WhisperID.GetLength()==0)
		{
			//---------------------------------------------
			
			//---------------------------------------------
			BOOL bAllOK = TRUE;

			int len = strlen(pName);	
			
			if (len<PlayerInfo::minIDLength || len>PlayerInfo::maxIDLength)
			{
				char strTemp[128];
				sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), PlayerInfo::minIDLength, PlayerInfo::maxIDLength);
				g_pUIDialog->PopupFreeMessageDlg( strTemp );
				bAllOK = FALSE;
			}

			if (bAllOK)
			{
				if (!IsValidID(pName, "-_"))
				{
					//---------------------------------------------
					
					//---------------------------------------------						
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
					bAllOK = FALSE;
				}
				else
				{
					char strName[80];
					strcpy(strName, pName);

					
					if (g_pChatManager->RemoveCurse(strName))
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
						bAllOK = FALSE;
					}
					else
					{
						MChatManager badNameManager;
						badNameManager.LoadFromFile( g_pFileDef->getProperty("FILE_INFO_CHAT_2").c_str());

						if (badNameManager.RemoveCurse(strName))
						{
							g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
							bAllOK = FALSE;
						}
					}
				}
			}

			if (bAllOK)
			{
					CLQueryCharacterName clQueryCharacterName;

					clQueryCharacterName.setCharacterName( pName );

					g_pSocket->sendPacket( &clQueryCharacterName );

					
					g_pUserInformation->WhisperID = pName;
			}
		}
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_CLOSE_SELECT_EXPLOSIVE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_SELECT_EXPLOSIVE");
	
	UI_CloseSelectExplosive();
}
*/

void
UIMessageManager::Execute_UI_CLOSE_TEAM_LIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_TEAM_LIST");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseTeamList();

		g_pUIDialog->UnSetLockInputPCTalk();
//		g_pUIDialog->ClosePCTalkDlg();
//		g_pPlayer->SetWaitVerifyNULL();
		
		//g_pUIDialog->ShowPCTalkDlg();
	}
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseTeamInfo(); 
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_MEMBER_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseTeamMemberInfo();
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_REGIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_TEAM_REGIST");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		
		if(!gC_vs_ui.ReturnTeamRegistMember())
			g_pUIDialog->ShowPCTalkDlg();

		gC_vs_ui.CloseTeamRegist();
	}
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_MEMBER_LIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_TEAM_MEMBER_LIST");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.CloseTeamMemberList();
//	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
//	{
//

//		g_pUIDialog->ShowPCTalkDlg();
//	}
}

void
UIMessageManager::Execute_UI_REQUEST_GUILD_INFO(int left, int right, void* void_ptr)
{
		CGSelectGuild  _CGSelectGuild ;
			
		_CGSelectGuild.setGuildID(g_pPlayer->GetGuildNumber());
		g_pSocket->sendPacket( &_CGSelectGuild  );				

}

	
void
UIMessageManager::Execute_UI_REQUEST_GUILD_MEMBER_LIST(int left, int right, void* void_ptr)
{
		CGRequestGuildMemberList  _CGRequestGuildMemberList ;
			
		g_pSocket->sendPacket( &_CGRequestGuildMemberList  );				

}

void
UIMessageManager::Execute_UI_MODIFY_GUILD_MEMBER(int left, int right, void* void_ptr)
{
		CGModifyGuildMember _CGModifyGuildMember;

		_CGModifyGuildMember.setGuildID(left);
		_CGModifyGuildMember.setGuildMemberRank(right);
		_CGModifyGuildMember.setName(std::string((char *)void_ptr));

		g_pSocket->sendPacket( &_CGModifyGuildMember );				

		gC_vs_ui.CloseTeamMemberInfo();
}

void
UIMessageManager::Execute_UI_SELECT_TEAM_MEMBER_LIST(int left, int right, void* void_ptr)
{
	char *pName = (char *)void_ptr;

	if(pName != NULL)
	{
			CGSelectGuildMember  _CGSelectGuildMember ;
			
			_CGSelectGuildMember.setName(std::string(pName));
			g_pSocket->sendPacket( &_CGSelectGuildMember  );				
	}
}


void
UIMessageManager::Execute_UI_SELECT_READY_TEAM_LIST(int left, int right, void* void_ptr)		// void_ptr = TEAM_NAME
{
	char *pName = (char *)void_ptr;

	if(pName != NULL)
	{
			CGSelectGuild  _CGSelectGuild ;
			
			_CGSelectGuild.setGuildID(left);
			g_pSocket->sendPacket( &_CGSelectGuild  );				
	}
}

//add by viva
void
UIMessageManager::Execute_UI_CLOSE_FRIEND_CHATTING_INFO(int left, int right, void* void_ptr)
{
	C_VS_UI_FRIEND_CHATTING_INFO* pInfo = (C_VS_UI_FRIEND_CHATTING_INFO*)void_ptr;
	gC_vs_ui.CloseFriendChattingInfo(pInfo);
}

void
UIMessageManager::Execute_UI_OPEN_FRIEND_CHATTING_INFO(int left, int right, void* void_ptr)
{
	C_VS_UI_FRIEND_INFO::FRIEND_LIST* pList = (C_VS_UI_FRIEND_INFO::FRIEND_LIST*)void_ptr;
	gC_vs_ui.OpenFriendChattingInfo(pList);
}

////////////////////////friend message////////////////////////////////////////////////////////
void UIMessageManager::Execute_UI_FRIEND_CHATTING_SEND_MESSAGE(int left, int right, void* void_ptr)
{
	C_VS_UI_FRIEND_CHATTING_INFO::FRIEND_SEND_MESSAGE* pMessage = (C_VS_UI_FRIEND_CHATTING_INFO::FRIEND_SEND_MESSAGE*)void_ptr;
	GCFriendChatting gcFriend;
	gcFriend.setCommand(CG_MESSAGE);
	gcFriend.setPlayerName(pMessage->Name);
	gcFriend.setMessage(pMessage->Message);
	g_pSocket->sendPacket( &gcFriend );
	delete pMessage;
}
void UIMessageManager::Execute_UI_FRIEND_CHATTING_UPDATE(int left, int right, void* void_ptr)
{
	GCFriendChatting gcFriend;
	gcFriend.setCommand(CG_UPDATE);
	g_pSocket->sendPacket( &gcFriend  );
}
void UIMessageManager::Execute_UI_FRIEND_CHATTING_ADD_FRIEND(int left, int right, void* void_ptr)
{
	if(!gC_vs_ui.IsFriendWaitAskRuning())
	{
		char* pName = (char*)void_ptr;
		GCFriendChatting gcFriend;
		gcFriend.setCommand(CG_ADD_FRIEND);
		gcFriend.setPlayerName(string(pName));
		g_pSocket->sendPacket( &gcFriend );
//		Delete(pName);
	}
//	gC_vs_ui.SendAddFriend(pName);
}

//////////////////////////ask_friend_request///////////////////////////////////////////////////
void UIMessageManager::Execute_UI_FRIEND_REQUEST_ACCEPT(int left, int right, void* void_ptr)
{
	C_VS_UI_ASK_DIALOG* pDialog = (C_VS_UI_ASK_DIALOG*)void_ptr;
	char* pName = (char*)pDialog->GetpTemporayValue();
	bool IsAgree = (bool)left;
	GCFriendChatting gcFriend;

	if(left == 0)
		gcFriend.setCommand(CG_ADD_FRIEND_AGREE);
	else if(left == 1)
		gcFriend.setCommand(CG_ADD_FRIEND_REFUSE);
	else if(left == 2)
		gcFriend.setCommand(CG_ADD_FRIEND_BLACK);

	gcFriend.setPlayerName(string(pName));
	g_pSocket->sendPacket(&gcFriend);
	
	DeleteNew(pDialog);
	DeleteNew(pName);
}
////////////////////////////////////////////ask_friend_close///////////////////////////////////
void UIMessageManager::Execute_UI_FRIEND_ASK_CLOSE(int left, int right, void* void_ptr)
{
	C_VS_UI_ASK_DIALOG* pDialog = (C_VS_UI_ASK_DIALOG*)void_ptr;
	char* pName = (char*)pDialog->GetpTemporayValue();
	if(pDialog->GetAskType() == C_VS_UI_ASK_DIALOG::ASK_FRIEND_WAIT)
		gC_vs_ui.setFriendWaitAskNull();

	DeleteNew(pDialog);
	DeleteNew(pName);
}
//////////////////////////////////////////////ask_friend_delete_ask//////////////////////////////
void UIMessageManager::Execute_UI_FRIEND_DELETE_ASK(int left, int right, void* void_ptr)
{
	C_VS_UI_FRIEND_INFO::FRIEND_LIST* pList = (C_VS_UI_FRIEND_INFO::FRIEND_LIST*)void_ptr;
	char* pName = new char[pList->Name.size()+1];
	strcpy(pName, pList->Name.c_str());
	gC_vs_ui.RunFriendDeleteAsk(pName);
}
////////////////////////////////////////////////ask_friend_delete_accept/////////////////////////
void UIMessageManager::Execute_UI_FRIEND_DELETE_ACCEPT(int left, int right, void* void_ptr)
{
	C_VS_UI_ASK_DIALOG* pDialog = (C_VS_UI_ASK_DIALOG*)void_ptr;
	char* pName = (char*)pDialog->GetpTemporayValue();
	GCFriendChatting gcFriend;
	gcFriend.setCommand(CG_FRIEND_DELETE);
	gcFriend.setPlayerName(string(pName));
	g_pSocket->sendPacket(&gcFriend);

	DeleteNew(pDialog);
	DeleteNew(pName);
}
//end

void
UIMessageManager::Execute_UI_SELECT_REGIST_TEAM_LIST(int left, int right, void* void_ptr)		// void_ptr = TEAM_NAME
{
	char *pName = (char *)void_ptr;

	if(pName != NULL)
	{
			CGSelectGuild  _CGSelectGuild ;
			
			_CGSelectGuild.setGuildID(left);
			g_pSocket->sendPacket( &_CGSelectGuild  );				
	}
}

	
void
UIMessageManager::Execute_UI_JOIN_READY_TEAM(int left, int right, void* void_ptr)				// void_ptr = TEAM_NAME
{
	CGTryJoinGuild _CGTryJoinGuild;
	_CGTryJoinGuild.setGuildID(left);
	_CGTryJoinGuild.setGuildMemberRank(2);
	g_pSocket->sendPacket( &_CGTryJoinGuild );
}

void
UIMessageManager::Execute_UI_JOIN_REGIST_TEAM(int left, int right, void* void_ptr)			// void_ptr = TEAM_NAME
{
	CGTryJoinGuild _CGTryJoinGuild;
	_CGTryJoinGuild.setGuildID(left);
	_CGTryJoinGuild.setGuildMemberRank(3);
	g_pSocket->sendPacket( &_CGTryJoinGuild );
}

	
void
UIMessageManager::Execute_UI_REGIST_GUILD_MEMBER(int left, int right, void* void_ptr)			
{

	if(void_ptr == NULL)
	{
		
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_GUILD_REGIST_FAIL_INTRO].GetString() );
		return;
	}

	CGJoinGuild _CGJoinGuild;
	_CGJoinGuild.setGuildID(left);
	_CGJoinGuild.setGuildMemberRank(right);
	_CGJoinGuild.setGuildMemberIntro(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGJoinGuild );

	gC_vs_ui.CloseTeamRegist();
	gC_vs_ui.CloseTeamInfo();
}

void
UIMessageManager::Execute_UI_REGIST_GUILD_TEAM(int left, int right, void* void_ptr)			
{

	if(void_ptr == NULL || (char *)left == NULL)
	{
		
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_GUILD_REGIST_FAIL_INTRO].GetString() );
		return;
	}

	CGRegistGuild _CGRegistGuild;
	_CGRegistGuild.setGuildName(std::string((char *)left));
	_CGRegistGuild.setGuildIntro(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGRegistGuild );

	gC_vs_ui.CloseTeamRegist();
}

	
void
UIMessageManager::Execute_UI_CLOSE_FILE_DIALOG(int left, int right, void* void_ptr)
{
	switch(left)
	{
	case C_VS_UI_FILE_DIALOG::MODE_PROFILE_SELECT:
		if(void_ptr != NULL)
		{
			char *p_str = (char *)void_ptr;
			const int str_size = strlen(p_str);

			if(str_size > 4)
			{
				const POINT bigSize = { 55, 70 };
				
				CDirectDrawSurface surface;

				surface.InitOffsurface( bigSize.x, bigSize.y, DDSCAPS_SYSTEMMEMORY );
				
				RECT destBigRect = { 0, 0, bigSize.x, bigSize.y };

				CDirectDrawSurface bmpSurface;
				
				if (LoadImageToSurface(p_str, bmpSurface))
				{
					
					
					RECT bmpRect = { 0, 0, bmpSurface.GetWidth(), bmpSurface.GetHeight() };

					// BigSize
					surface.FillSurface( 0 );
					surface.Blt(&destBigRect, &bmpSurface, &bmpRect);

					char saveBmpName[512];
					strcpy(saveBmpName, g_pFileDef->getProperty("DIR_PROFILE").c_str());
					strcat(saveBmpName, "\\");
					strcat(saveBmpName, g_char_slot_ingame.sz_name.c_str());
					strcat(saveBmpName, ".bmp");
					surface.SaveToBMP(saveBmpName);

					g_pProfileManager->InitProfiles();
					gC_vs_ui.RefreshInfoImage();

				}

			}
		}
		break;
	}
	gC_vs_ui.CloseFileDialog();
}

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ENCHANT_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ENCHANT_ACCEPT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	
	
	
	MItem* pItem = (MItem*)void_ptr;
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}

	if (pItem==NULL)
	{
		DEBUG_ADD("[Error] InvenItem is NULL");
		return;
	}

	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());

	
	if (g_pTradeManager!=NULL)
		return;
	
	//g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
	
	//----------------------------------------------------
	
	//----------------------------------------------------
	if(g_pTempInformation->GetMode() == TempInformation::MODE_NULL && g_pPlayer->IsItemCheckBufferNULL())
	{
		CGAddItemToItem _CGAddItemToItem;
		_CGAddItemToItem.setObjectID( mouseItemID );
		_CGAddItemToItem.setX( pItem->GetGridX() );
		_CGAddItemToItem.setY( pItem->GetGridY() );								
		
		g_pSocket->sendPacket( &_CGAddItemToItem );
		
		g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );
		g_pTempInformation->SetMode(TempInformation::MODE_ITEM_TO_ITEM);
		g_pTempInformation->Value1 = pItem->GetGridX();
		g_pTempInformation->Value2 = pItem->GetGridY();

//		UI_LockItem();
	}
}

void
UIMessageManager::Execute_UI_ENCHANT_CANCEL(int left, int right, void* void_ptr)
{
	UI_CloseEnchant();
}

void 
UIMessageManager::Execute_UI_MESSAGE_BOX(int left, int right, void* void_ptr)
{
	if(left < MAX_GAME_STRING)
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[left].GetString() );
}


void 
UIMessageManager::Execute_UI_CLOSE_OTHER_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseOtherInfo();
}

void UIMessageManager::Execute_UI_MODIFY_TEAM_INFO(int left, int right, void* void_ptr)
{
	if(void_ptr != NULL)
	{
		CGModifyGuildIntro _CGModifyGuildIntro;
		_CGModifyGuildIntro.setGuildID(left);
		_CGModifyGuildIntro.setGuildIntro(std::string((char *)void_ptr));
		g_pSocket->sendPacket( &_CGModifyGuildIntro );
	}
}

void UIMessageManager::Execute_UI_MODIFY_GUILD_MEMBER_INTRO(int left, int right, void* void_ptr)
{
	if(void_ptr != NULL)
	{
		CGModifyGuildMemberIntro _CGModifyGuildMemberIntro;
		_CGModifyGuildMemberIntro.setGuildID(left);
		_CGModifyGuildMemberIntro.setGuildMemberIntro(std::string((char *)void_ptr));
		g_pSocket->sendPacket( &_CGModifyGuildMemberIntro );
	}
}

void UIMessageManager::Execute_UI_SEND_NAME_FOR_SOUL_CHAIN(int left, int right, void* void_ptr)
{
	
	if(g_pPlayer->IsWaitVerify())
		return;

	if(void_ptr != NULL)
	{
		CGSkillToNamed _CGSkillToNamed;
		_CGSkillToNamed.setSkillType(SKILL_SOUL_CHAIN);
		_CGSkillToNamed.setTargetName(std::string((char *)void_ptr));
		_CGSkillToNamed.setCEffectID(0);
		g_pSocket->sendPacket( &_CGSkillToNamed );

		g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);

//		(*g_pSkillInfoTable)[SKILL_SOUL_CHAIN].SetNextAvailableTime();
	}
}

void UIMessageManager::Execute_UI_CLOSE_TRACE_WINDOW(int left, int right, void* void_ptr)
{
//	//------------------------------------------------------------------

//	//------------------------------------------------------------------	
//	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_SKILL_SUCCESS)
//	{		
//		g_pPlayer->SetWaitVerifyNULL();
//	}
//	else
//	{
//		DEBUG_ADD("[Error] Player is not WaitVerifySkillSuccess");
//	}
//
//	//------------------------------------------------------------------

//	//------------------------------------------------------------------
//	if (g_pPlayer->GetItemCheckBufferStatus()==MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY)
//	{
//		g_pPlayer->ClearItemCheckBuffer();
//	}

	gC_vs_ui.CloseTraceWindow();
}


void UIMessageManager::Execute_UI_RUN_CONNECT(int left, int right, void* void_ptr)
{
	UI_RunConnect();
}

void UIMessageManager::Excute_UI_SELECT_GRADE_SKILL(int left, int right, void* void_ptr)
{
	if(left >= g_pRankBonusTable->GetSize() || (*g_pRankBonusTable)[left].GetStatus() == RankBonusInfo::STATUS_CANNOT_LEARN || (*g_pRankBonusTable)[left].GetStatus() == RankBonusInfo::STATUS_LEARNED
		|| !g_pPlayer->IsWaitVerifyNULL() || g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;

	CGSelectRankBonus _CGSelectRankBonus;
	_CGSelectRankBonus.setRankBonusType(left);
	g_pSocket->sendPacket( &_CGSelectRankBonus );
	g_pTempInformation->SetMode(TempInformation::MODE_SKILL_LEARN);

}

void UIMessageManager::Excute_UI_USE_XMAS_TREE(int left, int right, void* void_ptr)				// left = to, right = from, void_ptr = message
{
	DEBUG_ADD("Excute_UI_USE_XMAS_TREE");
	MItem *pItem = (MItem *)left;

	if(pItem == NULL  || !g_pPlayer->IsItemCheckBufferNULL())
		return;

	char *sz_ptr = (char *)void_ptr;

	if(sz_ptr == NULL)
	{
		
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_XMAS_CARD_CANNOT_USE].GetString() );
		return;
	}

	CGUseMessageItemFromInventory _CGUseMessageItemFromInventory;
	_CGUseMessageItemFromInventory.setObjectID( pItem->GetID() );
	_CGUseMessageItemFromInventory.setX( pItem->GetGridX() );
	_CGUseMessageItemFromInventory.setY( pItem->GetGridY() );
	_CGUseMessageItemFromInventory.setMessage(sz_ptr);
					
	g_pSocket->sendPacket( &_CGUseMessageItemFromInventory );
					
	g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

	if(right == 1)
		gC_vs_ui.CloseBulletinBoardWindow();
	else
		gC_vs_ui.CloseXmasCardWindow();
	
	DEBUG_ADD("Excute_UI_USE_XMAS_TREE OK");
}

void UIMessageManager::Excute_UI_CLOSE_XMAS_CARD_WINDOW(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseXmasCardWindow();
}


void UIMessageManager::Excute_UI_SEND_BRING_FEE(int left,int right, void* void_ptr)
{
	
	if(g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
	{
		DEBUG_ADD("Excute_UI_SEND_BRING_FEE TempInformation is not NULL");
		return;
	}
	
	DEBUG_ADD("Excute_UI_SEND_BRING_FEE");
	

	g_pTempInformation->SetMode(TempInformation::MODE_WAIT_BRING_FEE);
	g_pTempInformation->Value1 = left;

	CGWithdrawTax _CGWithdrawTax;

	_CGWithdrawTax.setGold((DWORD)left);
	g_pSocket->sendPacket( &_CGWithdrawTax );

	DEBUG_ADD("Excute_UI_SEND_BRING_FEE OK");
}

void UIMessageManager::Excute_UI_CLOSE_BRING_FEE_WINDOW(int left,int right, void* void_ptr)
{
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseBringFeeWindow();
		g_pUIDialog->ShowPCTalkDlg();
	}
}

void UIMessageManager::Excute_UI_CLOSE_WAR_LIST(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseWarListWindow();	
}

void UIMessageManager::Execute_UI_CLOSE_BLOOD_BIBLE_STATUS(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseBloodBibleStatus();	
}

void UIMessageManager::Execute_UI_SEND_NAME_FOR_COUPLE(int left, int right, void *void_ptr)
{	
	if( g_pPlayer->GetWaitVerify() != MPlayer::WAIT_VERIFY_NULL 
		&& g_pTempInformation->GetMode()!=TempInformation::MODE_NULL)
	{
		g_pUIDialog->ClosePCTalkDlg();
		return;
	}

	int npcID = (*g_pPCTalkBox).GetNPCID();
	
	CGTypeStringList _CGTypeStringList;

	_CGTypeStringList.setType((CGTypeStringList::StringType) left);
	_CGTypeStringList.addString( std::string((char*) void_ptr) );	
	_CGTypeStringList.setParam( npcID );
	
	g_pSocket->sendPacket( &_CGTypeStringList );

//	gC_vs_ui.CloseInputNameWindow();
	gC_vs_ui.SetDisableInputName();

	g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_NPC_ASK);
	g_pTempInformation->SetMode(TempInformation::MODE_INPUT_NAME);
	g_pTempInformation->Value1 = (int)timeGetTime();
}

void UIMessageManager::Execute_UI_CLOSE_INPUT_NAME_WINDOW(int left, int right, void *void_ptr)
{
	if( g_pPlayer->GetWaitVerify() != MPlayer::WAIT_VERIFY_NPC_ASK	)
	{
		g_pUIDialog->ClosePCTalkDlg();
		gC_vs_ui.CloseInputNameWindow();
		g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_NULL );
	}  else
	{
		if(g_pTempInformation->GetMode() == TempInformation::MODE_INPUT_NAME)
		{
			DWORD dwTime = (DWORD) g_pTempInformation->Value1;
			
			if( timeGetTime() - dwTime >= 1000*60 )
			{
				g_pUIDialog->ClosePCTalkDlg();
				gC_vs_ui.CloseInputNameWindow();
				g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_NULL );
				g_pTempInformation->SetMode( TempInformation::MODE_NULL );
			}
		}
	}
}


//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_USE_GEAR");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot
	// right = by FunctionKey
	//
	//MItem* pItem = (MItem*)void_ptr;
	if (g_pPlayer!=NULL && (g_pPlayer->IsSlayer() && g_pSlayerGear!=NULL || g_pPlayer->IsVampire() && g_pVampireGear!= NULL || g_pPlayer->IsOusters() && g_pOustersGear!= NULL))
	{
		MItem* pSlotItem = static_cast<MItem*>(void_ptr);

		if (pSlotItem!=NULL && pSlotItem->IsAffectStatus())
		{

			if (g_pPlayer->IsItemCheckBufferNULL())
			{
				
				if(g_pPlayer->IsSlayer() && pSlotItem->IsSlayerItem() ||
					g_pPlayer->IsVampire() && pSlotItem->IsVampireItem() ||
					g_pPlayer->IsOusters() && pSlotItem->IsOustersItem())
				{
					pSlotItem->UseQuickItem();
				}
			}
			//----------------------------------------------------
			// item check buffer not null
			//----------------------------------------------------
			else
			{
				DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
			} 
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] There is no item in slot=%d", left);
		}
	}
	else
	{
		DEBUG_ADD("[Error] There is no Gear");
	}

}

void
UIMessageManager::Execute_GO_BILING_PAGE(int left, int right, void* void_ptr)
{
	if(left == TRUE)
	{
		
		SetMode( MODE_QUIT );

#ifdef PLATFORM_WINDOWS
		char str[256];

		GetWindowsDirectory(
			str,  // address of buffer for Windows directory
			255        // size of directory buffer
			);

		sprintf(str, "%s\\Explorer.exe", str);

		CSDLGraphics::GetDD()->RestoreDisplayMode();

	//	_spawnl(_P_NOWAIT, str, "Explorer.exe", g_pClientConfig->URL_HOMEPAGE_BILING.GetString(), NULL);
#endif
	}
}

void 
UIMessageManager::Execute_UI_CLOSE_POPUP_MESSAGE(int left, int right, void *void_ptr)
{
	gC_vs_ui.ClosePopupMessage();
}


void 
UIMessageManager::Execute_UI_CLOSE_QUEST_STATUS(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseQuestStatusWindow();
}

void 
UIMessageManager::Execute_UI_CLOSE_LOTTERY_CARD(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseLotteryCard();
	gC_vs_ui.SetQuestStatusInit();

	if(g_bEventQuestEnding && right == 5)
	{
		UI_CloseAllDialog();
		MEvent event;
		event.eventID = EVENTID_OUSTERS_FIN;
		event.eventType = EVENTTYPE_ZONE;
		event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | 
			EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | 
			EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | 
			EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
		event.parameter1 = 0;
		event.parameter4 = EVENTBACKGROUNDID_QUEST_2;
		g_pEventManager->AddEvent(event);			
	}
	g_bEventQuestEnding = false;	
}

void 
UIMessageManager::Execute_UI_LOTTERY_CARD_STATUS(int left, int right, void *void_ptr)
{
	
	
	CGLotterySelect _CGLotterySelect;
	
	_CGLotterySelect.setType( TYPE_SELECT_LOTTERY );
	_CGLotterySelect.setQuestLevel ( (DWORD) left-1 );
	_CGLotterySelect.setGiftID ( (DWORD) right );

	g_pSocket->sendPacket( &_CGLotterySelect );
}

void 
UIMessageManager::Execute_UI_FINISH_SCRATCH_LOTTERY(int left, int right, void *void_ptr)
{
	CGLotterySelect _CGLotterySelect;
	
	_CGLotterySelect.setType( TYPE_FINISH_SCRATCH );
	_CGLotterySelect.setQuestLevel ( 0 );
	_CGLotterySelect.setGiftID ( 0 );

	g_pSocket->sendPacket( &_CGLotterySelect );
	
}

void 
UIMessageManager::Execute_UI_CLOSE_IMAGE_NOTICE(int left,int right, void *void_ptr)
{
	gC_vs_ui.CloseImageNotice();
}

void 
UIMessageManager::Execute_UI_SELECT_ITEM_FROM_SHOP(int left, int right, void *void_ptr)
{
	CGTakeOutGood _CGTakeOutGood;

	_CGTakeOutGood.setObjectID( (DWORD) left );

	g_pSocket->sendPacket( &_CGTakeOutGood );
}

void 
UIMessageManager::Execute_UI_CLOSE_ITEM_LIST( int left, int right, void *void_ptr)
{
	g_pUIDialog->ClosePCTalkDlg();	
	gC_vs_ui.CloseItemListWindow();
}

void 
UIMessageManager::Execute_UI_CLOSE_BULLETIN_BOARD(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseBulletinBoardWindow();
}

void 
UIMessageManager::Execute_UI_TRANS_ITEM_CANCEL(int left, int right, void *void_ptr)
{
	UI_CloseTransItem();
}

void 
UIMessageManager::Execute_UI_TRANS_ITEM_ACCEPT(int left, int right, void *void_ptr)
{	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	
	
	
	MItem* pItem = (MItem*)void_ptr;
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}
	if (pItem==NULL)
	{
		DEBUG_ADD("[Error] InvenItem is NULL");
		return;
	}
	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());

	
	if (g_pTradeManager!=NULL)
		return;
	
	//g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
	
	//----------------------------------------------------
	
	//----------------------------------------------------
	if(g_pTempInformation->GetMode() == TempInformation::MODE_NULL && g_pPlayer->IsItemCheckBufferNULL())
	{
		CGAddItemToItem _CGAddItemToItem;
		_CGAddItemToItem.setObjectID( mouseItemID );
		_CGAddItemToItem.setX( pItem->GetGridX() );
		_CGAddItemToItem.setY( pItem->GetGridY() );								
		
		g_pSocket->sendPacket( &_CGAddItemToItem );
		
		g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );
		g_pTempInformation->SetMode(TempInformation::MODE_ITEM_TO_ITEM);
		g_pTempInformation->Value1 = pItem->GetGridX();
		g_pTempInformation->Value2 = pItem->GetGridY();

//		UI_LockItem();
	}
}

void 
UIMessageManager::Execute_UI_REQUEST_RESURRECT(int left, int right, void *void_ptr)
{
	if( left == 0 || left == 1)
	{
		// Resurrect Scroll
		MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_RESURRECT_ITEM, left );
		if( pItem == NULL )
		{
			return;
		}

		CGUseItemFromInventory _CGUseItemFromInventory;
		_CGUseItemFromInventory.setObjectID( pItem->GetID() );
		_CGUseItemFromInventory.setX( pItem->GetGridX() );
		_CGUseItemFromInventory.setY( pItem->GetGridY() );
		
		g_pSocket->sendPacket( &_CGUseItemFromInventory );				
		g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
	}
	else
	if( left == 2 )
	{
		// Eternity
		CGSkillToSelf			_CGSkillToSelf;

		_CGSkillToSelf.setSkillType( SKILL_ETERNITY );
		g_pSocket->sendPacket( &_CGSkillToSelf );
	}
	else if(left == 3)
	{
		CGSelectRegenZone _CGSelectRegenZone;
		_CGSelectRegenZone.setRegenZoneID(14);
		g_pSocket->sendPacket( &_CGSelectRegenZone );
	}
}

void 
UIMessageManager::Execute_UI_CLOSE_MIXING_FORGE(int left, int right, void *void_ptr)
{
	if(left == 0 )
	{
		gC_vs_ui.CloseMixingForge();
		g_pPlayer->ClearItemCheckBuffer();
		g_pTempInformation->SetMode( TempInformation::MODE_NULL );
	} else
	{
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[left].GetString() );		
	}
}

void 
UIMessageManager::Execute_UI_MIXING_FORGE(int left, int right, void* void_ptr)
{
	if( g_pPlayer->GetItemCheckBufferStatus() != MPlayer::ITEM_CHECK_BUFFER_USE_MIXING_ITEM ||
		g_pTempInformation->GetMode() != TempInformation::MODE_NULL )
		return;

	MItem *pFirstItem = g_pInventory->GetItemToModify( left );
	MItem *pSecondItem = g_pInventory->GetItemToModify( right );

	if( pFirstItem == NULL || pSecondItem == NULL )
	{
		return;
	}

	if( pFirstItem->GetItemClass() == pSecondItem->GetItemClass() &&
		pFirstItem->GetItemType() == pSecondItem->GetItemType() &&
		pFirstItem->GetItemOptionListCount() == 1 &&
		pSecondItem->GetItemOptionListCount() == 1 &&
		!pFirstItem->IsUniqueItem() && ! pSecondItem->IsUniqueItem() &&
		!pFirstItem->IsQuestItem() && ! pSecondItem->IsQuestItem() )
	{		
		MItem* pMixingItem = g_pPlayer->GetItemCheckBuffer();

		CGMixItem _CGMixItem;

		_CGMixItem.setObjectID( pMixingItem->GetID() );
		_CGMixItem.setX( pMixingItem->GetGridX() );
		_CGMixItem.setY( pMixingItem->GetGridY() );
		_CGMixItem.setTargetObjectID( 0, pFirstItem->GetID() );
		_CGMixItem.setTargetObjectID( 1, pSecondItem->GetID() );
		
		g_pSocket->sendPacket( &_CGMixItem );
		g_pPlayer->SetItemCheckBuffer( pMixingItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY );
		
		g_pTempInformation->SetMode( TempInformation::MODE_ITEM_MIXING );
		g_pTempInformation->Value1 = pFirstItem->GetGridX();
		g_pTempInformation->Value2 = pFirstItem->GetGridY();

		g_pTempInformation->Value3 = pSecondItem->GetGridX();
		g_pTempInformation->Value4 = pSecondItem->GetGridY();
		
		g_pTempInformation->pValue = (void*) pMixingItem;
	}		
}

void 
UIMessageManager::Execute_UI_CLOSE_REMOVE_OPTION(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseRemoveOptionFromRareItem();
}

void 
UIMessageManager::Execute_UI_SEND_REMOVE_OPTION(int left, int right, void *void_ptr)
{
	MItem *pMouseItem = (MItem*) void_ptr;
	MItem *pCurItem = g_pInventory->GetItemToModify( right );


	if( left < 0 || left > 1 || pMouseItem == NULL || pCurItem == NULL)
	{
		g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_ERROR_ETC_ERROR].GetString());
		return;
	}

	CGMixItem _CGMixItem;
	_CGMixItem.setObjectID( pMouseItem->GetID() );
	_CGMixItem.setX( pCurItem->GetGridX() );
	_CGMixItem.setY( pCurItem->GetGridY() );
	_CGMixItem.setTargetObjectID( 0, pCurItem->GetID() );
	_CGMixItem.setTargetObjectID( 1, left );
	g_pSocket->sendPacket( &_CGMixItem );
	gC_vs_ui.CloseRemoveOptionFromRareItem();

	g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );
	
	g_pTempInformation->SetMode(TempInformation::MODE_ITEM_TO_ITEM);
	g_pTempInformation->Value1 = pCurItem->GetGridX();
	g_pTempInformation->Value2 = pCurItem->GetGridY();
	g_pTempInformation->Value3 = left;
}

void 
UIMessageManager::Execute_UI_CLOSE_OUSTERS_SKILL_INFO(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseOustersSkillInfo();
}

void 
UIMessageManager::Execute_UI_LEARN_OUSTERS_SKILL(int left, int right, void *void_ptr)
{
	if( g_pTempInformation->GetMode() != TempInformation::MODE_NULL )
		return;		
		
	DEBUG_ADD("[UI] UI_LEARN_OUSTERS_SKILL");
	
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	
	g_pTempInformation->SetMode(TempInformation::MODE_SKILL_LEARN);
	{
		(*g_pTempInformation).Value3 = left;
		
		CGLearnSkill _CGLearnSkill;
		
		//_CGLearnSkill.setObjectID( (*g_pPCTalkBox).GetNPCID() );
		_CGLearnSkill.setSkillType( left );
		_CGLearnSkill.setSkillDomainType( SKILLDOMAIN_OUSTERS );
		
		g_pSocket->sendPacket( &_CGLearnSkill );
		
	}
}

void 
UIMessageManager::Execute_UI_RUN_LEVELUP(int left, int right, void *void_ptr)
{
	UI_LevelUp();
}

void 
UIMessageManager::Execute_UI_CLOSE_HORN(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseHorn();
}


void 
UIMessageManager::Execute_UI_OUSTERS_DOWN_SKILL(int left, int right, void *void_ptr)
{
	if( left < 0 || left >= MAX_ACTIONINFO)
		return;

	int curLevel = (*g_pSkillInfoTable)[left].GetExpLevel();

	
	if( curLevel > 30 || (curLevel == 1 && (*g_pSkillInfoTable)[left].CanDelete == 0) )
		return;	
	// 2004, 11, 8, sobeit modify end
	
	CGDownSkill	_CGDownSkill;

	_CGDownSkill.setSkillType( (SkillType_t) left );

	g_pSocket->sendPacket( &_CGDownSkill );

	DEBUG_ADD("[UI] Execute_UI_OUSTERS_DOWN_SKILL"); 
}

void 
UIMessageManager::Execute_UI_CLEAR_ALL_STAGE(int left, int right, void *void_ptr)
{	
	// LEFT = GAMETYPE
	switch( left )
	{
	case 0 :				
		break;
	case 1 :
		break;
	}
	gC_vs_ui.CloseArrowTile();
	gC_vs_ui.CloseCrazyMine();
}

void 
UIMessageManager::Execute_UI_CLOSE_FINDING_MINE(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseFindingMine();
}

void 
UIMessageManager::Execute_UI_CLOSE_NEMONEMO(int left, int right, void *void_ptr)
{
	//gC_vs_ui.CloseNemo
}

void 
UIMessageManager::Execute_UI_CLOSE_PUSHPUSH(int left, int right, void *void_ptr)
{
}

void 
UIMessageManager::Execute_UI_CLOSE_CRAZY_MINE(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseCrazyMine();

	CGFailQuest _CGFailQuest;

	_CGFailQuest.setFail( false );

	g_pSocket->sendPacket( &_CGFailQuest );
	gC_vs_ui.SetQuestStatusInit();
}

void 
UIMessageManager::Execute_UI_CLOSE_ARROW_TILE(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseArrowTile();
	CGFailQuest _CGFailQuest;

	_CGFailQuest.setFail( false );

	g_pSocket->sendPacket( &_CGFailQuest );
	gC_vs_ui.SetQuestStatusInit();
}

void 
UIMessageManager::Execute_UI_CLEAR_STAGE(int left, int right, void *void_ptr)
{
	WORD	GameType = HIWORD(left);
	WORD	Stage = LOWORD(left);
	DWORD	Time = (DWORD)right;

	Time/=10;

	Time = min(65535,Time);
	DEBUG_ADD_FORMAT("[Execute_UI_CLEAR_STAGE] GameType:%d Stage:%d Time:%d", GameType, Stage,Time);	
	
	CGSubmitScore _CGSubmitScore;

	_CGSubmitScore.setGameType( GameType == 0 ? GAME_ARROW : GAME_MINE );
	_CGSubmitScore.setLevel( Stage );
	_CGSubmitScore.setScore( WORD(Time) );
	g_pSocket->sendPacket( &_CGSubmitScore );
}

void
UIMessageManager::Execute_UI_FORCE_DIE(int left, int right, void *void_ptr)
{
	
	gC_vs_ui.CloseAllDialog();
	
	switch(left)
	{
	case 10 :						
		break;
	case 11 :						
		break;
	}
	
	
	CGFailQuest		_CGFailQuest;
	
	_CGFailQuest.setFail( true );
	g_pSocket->sendPacket( &_CGFailQuest );
	gC_vs_ui.SetQuestStatusInit();
}

void 
UIMessageManager::Execute_UI_ADD_ITEM_TO_CODE_SHEET(int left, int right, void *void_ptr)
{
	MItem* pItem = (MItem*)void_ptr;

	if( pItem == NULL || right < 0 || right >= 5)
		return;

	CGAddItemToCodeSheet _CGAddItemToCodeSheet;

	_CGAddItemToCodeSheet.setX( HIWORD( left ) );
	_CGAddItemToCodeSheet.setY( LOWORD( left ) );
	_CGAddItemToCodeSheet.setObjectID( pItem->GetID() );
	
	g_pSocket->sendPacket( &_CGAddItemToCodeSheet );
	g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );	

	g_pTempInformation->SetMode(TempInformation::MODE_ITEM_CODE_SHEET);
	g_pTempInformation->Value1 = right;	
	g_pTempInformation->Value2 = left;
	g_pTempInformation->pValue = (void*)gpC_mouse_pointer->GetPickUpItem();
}


void 
UIMessageManager::Execute_UI_SEND_BUG_REPORT(int left,int right, void *void_ptr)
{
	SendBugReport("[UI_BUG] %d, %s", left, reinterpret_cast<char*>( void_ptr ) );
}

void 
UIMessageManager::Execute_UI_GO_BEGINNER_ZONE(int left, int right, void* void_ptr)
{
//	Execute_UI_CLOSE_POPUP_MESSAGE(left,right,void_ptr);
//
//	CGSelectWayPoint _CGSelectWayPoint;
//
//	_CGSelectWayPoint.setZoneID( 1122 );
//	_CGSelectWayPoint.setX( 0 );
//	_CGSelectWayPoint.setY( 0 );
//
//	g_pSocket->sendPacket( &_CGSelectWayPoint );
}

void
UIMessageManager::Execute_UI_POPUP_MESSAGE_OK(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseMailBox();
	
	Execute_UI_CLOSE_POPUP_MESSAGE(left,right,void_ptr);

	DWORD recvID = (DWORD) left;
	
	int zoneID = 0;

	switch(recvID)
	{
	
	case 1:
		zoneID = 1122;
		break;

	case 2:
		zoneID = 1122;
		break;
		
	
	case 4:
		zoneID = 1131;
		break;

	
	case 6:
		zoneID = 72;
		break;

	}

	CGSelectWayPoint _CGSelectWayPoint;
	
	_CGSelectWayPoint.setZoneID( zoneID );
	_CGSelectWayPoint.setX( 0 );
	_CGSelectWayPoint.setY( 0 );
	
	g_pSocket->sendPacket( &_CGSelectWayPoint );
}

void 
UIMessageManager::Execute_UI_CLOSE_SHRINE_MINIMAP(int left, int right, void* void_ptr)
{
	

	bool bSended = false;

	for(int i = 0; i < g_pRegenTowerInfoManager->GetSize(); i++ )
	{
		const RegenTowerInfo* pInfo = &g_pRegenTowerInfoManager->Get(i);
		
		if( pInfo->owner == (int)g_pPlayer->GetRace() )
		{
			CGSelectRegenZone _CGSelectRegenZone;
			_CGSelectRegenZone.setRegenZoneID( i );
			g_pSocket->sendPacket( &_CGSelectRegenZone );
			bSended = true;
			break;
		}
	}
	gC_vs_ui.CloseRequestShrineMinimap();

	if( bSended == false )
	{
		Execute_UI_FINISH_REQUEST_DIE_BUTTON(left, right, void_ptr);
	}
	gC_vs_ui.FinishRequestDie();
	gC_vs_ui.FinishRequestResurrect();	
}

void 
UIMessageManager::Execute_UI_WARP_TO_REGEN_TOWER(int left, int right, void* void_ptr)
{
	if( g_pRegenTowerInfoManager == NULL || left < 0 || 
		left >= g_pRegenTowerInfoManager->GetSize() || 
		g_pRegenTowerInfoManager->Get(left).owner != g_pPlayer->GetRace() )
		return;

	CGSelectRegenZone _CGSelectRegenZone;
	
	_CGSelectRegenZone.setRegenZoneID( left );

	g_pSocket->sendPacket( &_CGSelectRegenZone );
	gC_vs_ui.CloseRequestShrineMinimap();
	gC_vs_ui.FinishRequestDie();
	gC_vs_ui.FinishRequestResurrect();	
}

void 
UIMessageManager::Execute_UI_CLOSE_MAILBOX(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseMailBox();
}

void
UIMessageManager::Execute_UI_CLOSE_PET_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.ClosePetInfo();
}

void	
UIMessageManager::Execute_UI_PET_GAMBLE(int left, int right, void* void_ptr)
{
	CGPetGamble _CGPetGamble;
	g_pSocket->sendPacket( &_CGPetGamble );
	
	UI_LockItem();
}

void
UIMessageManager::Execute_UI_CLOSE_USE_PET_FOOD(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	gC_vs_ui.CloseUsePetFood();
}

void	 
UIMessageManager::Execute_UI_CLOSE_PETSTORAGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	gC_vs_ui.ClosePetStorage();
	
	
	if (g_pStorage!=NULL)
	{
		delete g_pStorage;
		g_pStorage = NULL;
	}

	
	gC_vs_ui.SetPetStorage( NULL );


//	g_pUIDialog->ShowPCTalkDlg();

	g_pTempInformation->SetMode(TempInformation::MODE_NULL);

}
void	 
UIMessageManager::Execute_UI_CLOSE_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseKeepPetItemDialog();
}
void	 
UIMessageManager::Execute_UI_CLOSE_GET_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseGetKeepPetItemDialog();
}

void	 
UIMessageManager::Execute_UI_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_KEEP_PETITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if (g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;
	MItem* pItem = (MItem*)void_ptr;

	if(NULL == pItem)
		return;

	CGDepositPet _CGDepositPet;
	_CGDepositPet.setObjectID(pItem->GetID());
	if(-1 != right)
		_CGDepositPet.setIndex(right);
	else
		return;
	g_pSocket->sendPacket( &_CGDepositPet );	

	g_pTempInformation->SetMode(TempInformation::MODE_PETITEM_MOVETO_PETSTORAGE);
	g_pTempInformation->Value1 = right;
	g_pTempInformation->Value2 = pItem->GetGridX();
	g_pTempInformation->Value3 = pItem->GetGridY();
	g_pTempInformation->pValue = pItem;
	UI_LockItem(); 
}
void	 
UIMessageManager::Execute_UI_GET_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_GET_KEEP_PETITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if (g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;
	MItem* pItem = (MItem*)void_ptr;

	if(NULL == pItem)
		return;

	CGWithdrawPet _CGWithdrawPet;
	_CGWithdrawPet.setObjectID(pItem->GetID());
	if(right<20 &&right>-1)
		_CGWithdrawPet.setIndex(right);
	else
		return;
	g_pSocket->sendPacket( &_CGWithdrawPet );	
	g_pTempInformation->SetMode(TempInformation::MODE_PETITEM_MOVETO_INVENTORY);
	g_pTempInformation->Value1 = right;
	g_pTempInformation->Value2 = pItem->GetGridX();
	g_pTempInformation->Value3 = pItem->GetGridY();
	g_pTempInformation->pValue = pItem;
	UI_LockItem();
}


void	 
UIMessageManager::Execute_UI_CLOSE_SMS_MESSAGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		if(gC_vs_ui.IsRunningSMSRecord())
			gC_vs_ui.CloseSMSRecord();
		if(gC_vs_ui.IsRunningSMSList())
			gC_vs_ui.CloseSMSList();
		gC_vs_ui.CloseSMSMessage(); 
		
	}

}
void
UIMessageManager::Execute_UI_SEND_SMS_MESSAGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if (g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;

	std::list<std::string> *TempStr = (std::list<std::string>*)(void_ptr);

	if(NULL == TempStr || TempStr->size()>5)
		return ;
	
	CGSMSSend _CGSMSSend;
	_CGSMSSend.setCallerNumber(std::string((char *)left));
	_CGSMSSend.setMessage(std::string((char *)right));
	_CGSMSSend.clearString(); 
	std::list<std::string>::iterator itr = TempStr->begin();
	while(itr != TempStr->end())
	{
		string str = (*itr);
		_CGSMSSend.addString(str);
		itr++;
	}
	g_pSocket->sendPacket( &_CGSMSSend );
	
	gC_vs_ui.CloseSMSMessage();
	if(gC_vs_ui.IsRunningSMSList())
		gC_vs_ui.CloseSMSList();
	if(gC_vs_ui.IsRunningSMSRecord())
		gC_vs_ui.CloseSMSRecord();
}


void	 
UIMessageManager::Execute_UI_SMS_OPEN_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.RunSMSList();
		
		CGSMSAddressList _CGSMSAddressList;
		g_pSocket->sendPacket( &_CGSMSAddressList );
	}
}

void	 
UIMessageManager::Execute_UI_SMS_RECORD(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGAddSMSAddress	_CGAddSMSAddress;
		_CGAddSMSAddress.setCharacterName(std::string((char *)left));
		_CGAddSMSAddress.setCustomName((std::string((char *)right)));
		_CGAddSMSAddress.setNumber((std::string((char *)void_ptr)));
		g_pSocket->sendPacket( &_CGAddSMSAddress );

		g_pTempInformation->SetMode(TempInformation::MODE_SMS_ADD_ADDRESS);
		g_pTempInformation->Value1 = (int)left;
		g_pTempInformation->Value2 = (int)right;
		g_pTempInformation->Value3 = (int)(intptr_t)void_ptr;

	}

}


void	 
UIMessageManager::Execute_UI_SMS_DELETE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGDeleteSMSAddress	_CGDeleteSMSAddress;
		_CGDeleteSMSAddress.setElementID((DWORD)left);
		g_pSocket->sendPacket( &_CGDeleteSMSAddress );

		g_pTempInformation->SetMode(TempInformation::MODE_SMS_DELETE_ADDRESS);
		g_pTempInformation->Value1 = left;
	}
}

void	 
UIMessageManager::Execute_UI_CLOSE_SMS_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseSMSList();
	}
}

void	 
UIMessageManager::Execute_UI_CLOSE_SMS_RECORD(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseSMSRecord();
	}
}
void	 
UIMessageManager::Execute_UI_SMS_ADD_SEND_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.AddSMSSendList((char*)left);
	}
}


void	 
UIMessageManager::Execute_UI_CLOSE_NAMING(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseNaming();
	}
}

void	 
UIMessageManager::Execute_UI_SELECT_NAMING(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGSelectNickname	_CGSelectNickName;
		_CGSelectNickName.setNicknameID((WORD)left);
		g_pSocket->sendPacket( &_CGSelectNickName );

//		g_pTempInformation->SetMode(TempInformation::MODE_NICKNAME_CHANGE_CUSTOM);
//		g_pTempInformation->Value1 = left;
	}
}


void	 
UIMessageManager::Execute_UI_CLOSE_NAMING_CHANGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseNamingChange();
	}
}


void	 
UIMessageManager::Execute_UI_CHANGE_CUSTOM_NAMING(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
//	return;
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		char szTemp[22];
		strcpy(szTemp, (char*) left);
		if(strlen(szTemp) == 0)
			return;
//		if (!IsValidID(szTemp, "-_"))
//		{
//			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
//			bAllOK = FALSE;
//		}
//		else
//		{
//			char strName[80];
//			strcpy(strName, pChar->sz_name);
//

//			if (g_pChatManager->RemoveCurse(strName))
//			{
//				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
//				bAllOK = FALSE;
//			}
//		}
		g_pChatManager->RemoveCurse(szTemp );

		
		MItem* pItem = NULL;	
		
		if(right != 0)
			pItem = g_pInventory->GetItemToModify( right );//(MItem*)void_ptr;
		
		if(pItem!= NULL || right == 0)
		{
			CGModifyNickname	_CGModifyNickname; 
			_CGModifyNickname.setNickname(std::string((char *)szTemp));

			if(right != 0)
			{ 
				
			//	Execute_UI_ITEM_USE(pItem->GetID(), 0, (void*)pItem);
				
				_CGModifyNickname.setNicknameID(pItem->GetID());
				g_pSocket->sendPacket( &_CGModifyNickname );

				g_pTempInformation->SetMode(TempInformation::MODE_NICKNAME_CHANGE_CUSTOM);
				g_pTempInformation->Value1 = left;					
				g_pTempInformation->Value2 = pItem->GetItemType();  

				if(25 != g_pTempInformation->Value2)  
					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
			}
			else
			{
				
				_CGModifyNickname.setNicknameID(0);
				g_pSocket->sendPacket( &_CGModifyNickname );
				g_pTempInformation->SetMode(TempInformation::MODE_NICKNAME_CHANGE_CUSTOM);
				g_pTempInformation->Value1 = left;					
				g_pTempInformation->Value2 = 0;  // 
			}
		}
		else
			UI_PopupMessage (UI_STRING_MESSAGE_FAIL_SEARCH_ITEM);
	}
}

void	 
UIMessageManager::Execute_UI_RUN_NAMING_CHANGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = (MItem*)left;
		if(pItem->GetItemClass() != ITEM_CLASS_EVENT_GIFT_BOX)
			return;
		
			
			
		
			
		
		if(pItem->GetItemType() == 22 || pItem->GetItemType() == 25) 
			gC_vs_ui.RunNamingChange(pItem, (char*)g_pPlayer->GetNickName().c_str());

		else if(pItem->GetItemType() == 24) 
			gC_vs_ui.RunNamingChange(pItem, (char*)(*g_pGameStringTable)[UI_STRING_MESSAGE_ADD_PLAYER_NICKNAME].GetString());


//			gC_vs_ui.RunNamingChange(pItem, (char*)g_pPlayer->GetNickName().c_str());
		else if(pItem->GetItemType() == 23) 
		{
			MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(g_pPlayer->GetPetID());
			if(NULL != pFakeCreature)
			{
				MPetItem *pPetItem = pFakeCreature->GetPetItem();
				if(pPetItem != NULL)
				{
					if(2 == pPetItem->GetItemType()) 
						UI_PopupMessage( UI_STRING_MESSAGE_PET_NAMING_WOLVERINE );
					else
						gC_vs_ui.RunNamingChange(pItem, (char*)pFakeCreature->GetNickName().c_str());
				}
			}
			else
				UI_PopupMessage( UI_STRING_MESSAGE_PET_NAMING_SUMMON );
		}
		//gC_vs_ui.RunNamingChange(pItem, char* szCustom);

	}
}

void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_MANAGER(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestManager();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestList();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_DETAIL(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestDetail();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_MISSION(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestMission();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_ITEM(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestItem();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_ICON(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestIcon(left);
	}
}
void	 
UIMessageManager::Execute_UI_GQUEST_ACCEPT(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGGQuestAccept	_CGGQuestAccept;
		_CGGQuestAccept.setQuestID((DWORD)left);
		g_pSocket->sendPacket( &_CGGQuestAccept );
	}
}
void	 
UIMessageManager::Execute_UI_GQUEST_GIVEUP(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGGQuestCancel	_CGGQuestCancel;
		_CGGQuestCancel.setQuestID((DWORD)left);
		g_pSocket->sendPacket( &_CGGQuestCancel );
	}
}
//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE_GQUEST_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_GQUEST_ITEM");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	MItem* pItem =  (MItem*)void_ptr ;//(MItem*)void_ptr; 

	if(NULL != pItem)
	{
		CGUseItemFromGQuestInventory _CGUseItemFromGQuestInventory;
		_CGUseItemFromGQuestInventory.setIndex( left );
		
		g_pSocket->sendPacket( &_CGUseItemFromGQuestInventory );
		
		g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_GQUEST_INVENTORY);
	}
}


void
UIMessageManager::Execute_UI_ITEM_USE_REQUEST_GUILD_LIST(int left, int right, void* void_ptr)
{

	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_REQUEST_GUILD_LIST");
	CGRequestGuildList _CGRequestGuildlist;	
	_CGRequestGuildlist.setGuildType(left);
	g_pSocket->sendPacket(&_CGRequestGuildlist); 

}


void
UIMessageManager::Execute_UI_ITEM_USE_UNION_INFO(int left, int right, void* void_ptr)
{

	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_INFO");
	CGRequestUnionInfo _CGRequestUnioninfo;	
	g_pSocket->sendPacket(&_CGRequestUnioninfo); 

}


void
UIMessageManager::Execute_UI_ITEM_USE_REQUEST_UNION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_REQUEST_UNION");
	
	if(g_pUserInformation->GuildGrade != 1) 
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_1);
		return;
	}
	else if(g_pUserInformation->dwUnionID != 0)
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_2);
		return;
	}
	CGRequestUnion _CGRequestUnion;	
	_CGRequestUnion.setGuildID(left);
	g_pSocket->sendPacket(&_CGRequestUnion); 

	gC_vs_ui.CloseTeamInfoDialogBox();
}
void
UIMessageManager::Execute_UI_ITEM_USE_QUIT(int left, int right, void* void_ptr)
{	
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_REQUEST_DENY");

	if(g_pUserInformation->GuildGrade != 1) 
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_1);
		return;
	}

	CGQuitUnion   _CGQuitUnion;
	_CGQuitUnion.setGuildID(left);
	_CGQuitUnion.setQuitMethod(right);
	g_pSocket->sendPacket(&_CGQuitUnion); 

	gC_vs_ui.ClosePopupMessage();
	gC_vs_ui.CloseTeamInfoDialogBox();
}
void
UIMessageManager::Execute_UI_ITEM_USE_EXPER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_EXPER"); 
	if(g_pUserInformation->GuildGrade != 1) 
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_1);
		return;
	}
	else if(g_pUserInformation->bUnionGrade != 0)
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_3);
		return;
	}
	CGExpelGuild  _CGExpelGuild;
	_CGExpelGuild.setGuildID(left);
	g_pSocket->sendPacket(&_CGExpelGuild); 

	gC_vs_ui.CloseTeamInfoDialogBox();
	gC_vs_ui.CloseTeamInfo();
	gC_vs_ui.CloseTeamList();
}

void
UIMessageManager::Execute_UI_ITEM_USE_UNION_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_ACCEPT");
	CGAcceptUnion  _CGAcceptUnion;
	_CGAcceptUnion.setGuildID(left);
	g_pSocket->sendPacket(&_CGAcceptUnion); 

	gC_vs_ui.CloseMailBoxDialogBox();
}

void
UIMessageManager::Execute_UI_ITEM_USE_UNION_DENY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_DENY");
	CGDenyUnion  _CGDenyUnion;
	_CGDenyUnion.setGuildID(left);
	g_pSocket->sendPacket(&_CGDenyUnion); 

	gC_vs_ui.CloseMailBoxDialogBox();

}


void
UIMessageManager::Execute_UI_ITEM_USE_UNION_QUIT_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_QUIT_ACCEPT");
	CGQuitUnionAccept _CGQuitUnionAccept;
	_CGQuitUnionAccept.setGuildID(left);
	g_pSocket->sendPacket(&_CGQuitUnionAccept); 

	gC_vs_ui.CloseMailBoxDialogBox();
}

void
UIMessageManager::Execute_UI_ITEM_USE_UNION_QUIT_DENY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_QUIT_DENY");
	CGQuitUnionDeny _CGQuitUnionDeny;
	_CGQuitUnionDeny.setGuildID(left);
	g_pSocket->sendPacket(&_CGQuitUnionDeny); 

	gC_vs_ui.CloseMailBoxDialogBox();
}




void
UIMessageManager::Execute_UI_RECALL_BY_NAME(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_RECALL_BY_NAME");
	char *szSelectedID = gC_vs_ui.GetTeamMember_SelectedID();
	if(szSelectedID != NULL && strlen(szSelectedID)>0)
	{
		char szBuf[128];
		sprintf(szBuf, "*recall %s", szSelectedID );
		CGSay _CGSay;
		_CGSay.setMessage( szBuf );	//pWansungString );
		_CGSay.setColor( 0 );
		g_pSocket->sendPacket( &_CGSay );
	}
}

void
UIMessageManager::Execute_UI_UI_MODIFY_TAX(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_UI_MODIFY_TAX");

	CGModifyTaxRatio _CGModifyTaxRatio;
	_CGModifyTaxRatio.setRatio(left);
	g_pSocket->sendPacket( &_CGModifyTaxRatio );

}


void
UIMessageManager::Execute_UI_APPOINT_SUBMASTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_APPOINT_SUBMASTER");

	CGAppointSubmaster _CGAppointSubmaster;
	_CGAppointSubmaster.setGuildID(left);
	_CGAppointSubmaster.setName(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGAppointSubmaster );

}

//2004.11.27
 void
UIMessageManager::Execute_UI_DISPLAY_ITEM(int left, int right, void* void_ptr)
 {
	  	DEBUG_ADD("[UI] UI_SELECT_PERSNALSHOP_SLOT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if(g_pStorage2 == NULL)
		return;	


	MItem* pItem = (MItem*)void_ptr;
	g_pStorage2->SetCurrent( 0 );	
 
 	int slot = right;

	
	const MItem* pStorageItem = g_pStorage2->GetItem( slot );
	//TYPE_OBJECTID mouseItemID = pMouseItem->GetID();		
	
	
	if (pItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX
		//&& pItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_AMULET
//		&& pItem->GetItemClass()!=ITEM_CLASS_COUPLE_RING
//		&& pItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_COUPLE_RING
//		&& !pItem->IsQuestItem()
//		&& pItem->IsNormalItem()
//		&& !((pItem->GetItemClass() == ITEM_CLASS_EVENT_TREE) && pItem->GetItemType() >= 13  )
//		&& pItem->GetItemClass() != ITEM_CLASS_EVENT_ITEM
//		&& pItem->GetItemClass() != ITEM_CLASS_CODE_SHEET
//		&& !(pItem->GetItemClass() == ITEM_CLASS_LUCKY_BAG && pItem->GetItemType() == 3)
		)
	{
		int current_storage = g_pStorage2->GetCurrent();
		const int storage_size = g_pStorage2->GetSize();
		const int storage_max = STORAGE_SLOT;
		
			
			for(int i = 0; i < storage_max; i++)
			{
				const MItem *p_slot_item = g_pStorage2->GetItem(i);
				
				
				if(p_slot_item == NULL)
				{
					CGDisplayItem _CGDisplayItem;
					_CGDisplayItem.setIndex(i);
					_CGDisplayItem.setItemObjectID(pItem->GetID());
					_CGDisplayItem.setPrice(pItem->GetPersnalPrice());
					//_CGDisplayItem.setXY(pMouseItem->GetGridX(),pMouseItem->GetGridY());
					_CGDisplayItem.setXY(pItem->GetGridX(),pItem->GetGridY());
					g_pSocket->sendPacket( &_CGDisplayItem ); 
					break;
				}
			}
			current_storage ++;
			if(current_storage >= storage_size)
				current_storage = 0;

		}
		else
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_STORAGE].GetString());
		}
	}

 void
UIMessageManager::Execute_UI_UNDISPLAY_ITEM(int left, int right, void* void_ptr)
 {

	MItem* pMouseItem = (MItem*)void_ptr;
	const MItem* pStorageItem = g_pStorage2->GetItem( right );

	if (pMouseItem!=NULL)
	{
//		if (pStorageItem!=NULL)
//		{
			MItem* pRemovedItem = g_pStorage2->RemoveItem( left );

			if (pRemovedItem!=NULL)
			{
				 CGUndisplayItem _CGUndisplayItem;
				 _CGUndisplayItem.setIndex(left);
				 _CGUndisplayItem.setItemObjectID(pRemovedItem->GetID());
				 //_CGUndisplayItem.setXY(left,right);
				 _CGUndisplayItem.setXY(pMouseItem->GetGridX(),pMouseItem->GetGridY());
				 
				 g_pSocket->sendPacket( &_CGUndisplayItem );
			}
//		}
	}
	DEBUG_ADD("[UI] Execute_UI_UNDISPLAY_ITEM");
 }

 void
UIMessageManager::Execute_UI_STORE_SIGN(int left, int right, void* void_ptr)
 {
	 DEBUG_ADD("[UI] Execute_UI_STORE_SIGN");

	DEBUG_ADD_FORMAT("[Original] %s", (char*)void_ptr );

	char * pernalshop_message = (char*)void_ptr;
    CGStoreSign _CGStoreSign;
	
	char str[250];
	memset(str,0,250);
	strcpy(str, (char*)void_ptr);
	
	g_pChatManager->RemoveCurse( str );
	_CGStoreSign.setSign(str);
	g_pSocket->sendPacket( &_CGStoreSign );

	gC_vs_ui.ClosePersnalShopMessage();
 }

 void
 UIMessageManager::Execute_UI_STORE_OPEN(int left, int right, void* void_ptr)
 {
	 	DEBUG_ADD("[UI] Execute_UI_STORE_OPEN");
		
		CGStoreOpen _CGCGStoreOpen;
		gC_vs_ui.SetPersnalShoptoUI(2);
		g_pSocket->sendPacket( &_CGCGStoreOpen );
				
 }
 
 void
UIMessageManager::Execute_UI_STORE_CLOSE(int left, int right, void* void_ptr)
 {
	 DEBUG_ADD("[UI] Execute_UI_STORE_CLOSE");
	 
	 CGStoreClose _CGStoreClose;
	 gC_vs_ui.SetPersnalShoptoUI(3);
	 
	 g_pSocket->sendPacket( &_CGStoreClose );


 }

 void
UIMessageManager::Execute_UI_REQUEST_STORE_INFO(int left, int right, void* void_ptr)
 {
	 DEBUG_ADD("[UI] Execute_UI_REQUEST_STORE_INFO");
	 
	 if(g_pPlayer->IsFlyingCreature() 
		 || g_pPlayer->IsUndergroundCreature() 
		 //|| g_pPlayer->IsGroundCreature() 
		 || g_pPlayer->IsFakeCreature() 
		 || g_pPlayer->GetMoveDevice()== 1
		 || g_pPlayer->GetMoveDevice()== 2
		 || gC_vs_ui.IsRunningSlayerPortal()
		 || gC_vs_ui.IsRunningHorn()
		)
	 {
		 DEBUG_ADD("[UI] Execute_DON'T OPEN PERSNAL SHOP Cuz your's type can't open PersnalShop./......");
	 }
	 else
	 {
		 if(right == 1)
			 gC_vs_ui.SetPersnalShoptoUI(1); 
	 

		 CGRequestStoreInfo _CGRequestStoreInfo;
		 _CGRequestStoreInfo.setOwnerObjectID(left);
		 g_pSocket->sendPacket( &_CGRequestStoreInfo );
	 }

 }

 void
UIMessageManager::Execute_UI_MY_STORE_INFO(int left, int right, void* void_ptr)
 {

	 DEBUG_ADD("[UI] Execute_UI_MY_STORE_INFO"); 
	 
	 GCMyStoreInfo _GCMyStoreInfo;
	 _GCMyStoreInfo.getStoreInfo();

//	 if (g_pStorage2!=NULL)
//		 delete g_pStorage2;
//	 
//	 g_pStorage2 = new MStorage;
//	 
//	 g_pStorage2->Init( left ); 
//	 gC_vs_ui.RunPersnalShop();
//	 gC_vs_ui.SetPersnalShop( g_pStorage2 );
//	 
	 
	 g_pSocket->sendPacket( &_GCMyStoreInfo );

 }

void
UIMessageManager::Execute_UI_OTHER_STORE_INFO(int left, int right, void* void_ptr)
{
	// not sure
		 DEBUG_ADD("[UI] Execute_UI_OTHER_STORE_INFO");
	 
	  GCOtherStoreInfo _GCOtherStoreInfo;
	 _GCOtherStoreInfo.setObjectID(g_pTopView->GetSelectedCreature());
	  g_pSocket->sendPacket( &_GCOtherStoreInfo );
}

void
UIMessageManager::Execute_UI_BUY_STORE_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_BUY_STORE_ITEM");
	
	if(g_pTopView->GetSelectedCreature() == NULL)
		return;
	MItem* pItem = (MItem*)void_ptr;		
	
	if(pItem != NULL)		
	{
		CGBuyStoreItem _CGBuyStoreItem;
		_CGBuyStoreItem.setIndex(left);
		_CGBuyStoreItem.setItemObjectID(pItem->GetID());
		_CGBuyStoreItem.setOwnerObjectID(gC_vs_ui.GetOtherObjectID());
		
		g_pSocket->sendPacket( &_CGBuyStoreItem );
	}

}

void
UIMessageManager::Execute_UI_REMOVE_STORE_ITEM(int left, int right, void* void_ptr)
{
	 DEBUG_ADD("[UI] Execute_UI_REMOVE_STORE_ITEM");
	 
	 GCRemoveStoreItem _GCRemoveStoreItem;
	 _GCRemoveStoreItem.setIndex(0);
	 _GCRemoveStoreItem.setOwnerObjectID(g_pPlayer->GetID());
	 g_pSocket->sendPacket( &_GCRemoveStoreItem );
}

void
UIMessageManager::Execute_UI_ADD_STORE_ITEM(int left, int right, void* void_ptr)
{

	DEBUG_ADD("[UI] Execute_UI_ADD_STORE_ITEM");
	 
	 GCAddStoreItem _GCAddStoreItem;
	 _GCAddStoreItem.setIndex(0);
	 _GCAddStoreItem.setOwnerObjectID(g_pPlayer->GetID());
	 g_pSocket->sendPacket( &_GCAddStoreItem );
}


void
UIMessageManager::Execute_UI_CLOSE_PERSNALSHOP(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_PERSNALSHOP");
	gC_vs_ui.ClosePersnalShop();	
	
	if (g_pStorage2!=NULL)
	{
		delete g_pStorage2;
		g_pStorage2 = NULL;
	}
	 
	gC_vs_ui.SetPersnalShop( NULL );
}

void
UIMessageManager::Execute_UI_CLOSE_POWER_JJANG(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_POWER_JJANG");
	gC_vs_ui.ClosePowerjjang();
}
void
UIMessageManager::Execute_UI_REQUEST_POWER_JJANG_POINT(int left, int right, void* void_ptr)
{ 
	DEBUG_ADD("[UI] Execute_UI_REQUEST_POWER_JJANG_POINT");

	CGRequestPowerPoint _CGRequestPowerPoint;
	_CGRequestPowerPoint.setCellNum(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGRequestPowerPoint );
}
void
UIMessageManager::Execute_UI_POWER_JJANG_GAMBLE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_POWER_JJANG_GAMBLE");

	CGUsePowerPoint _CGUsePowerPoint;
	g_pSocket->sendPacket( &_CGUsePowerPoint );
}


void
UIMessageManager::Execute_UI_CLOSE_SWAPADVANCEMENTITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_SWAPADVANCEMENTITEM");

	gC_vs_ui.FinishSwapAdvancementItem();
}

void
UIMessageManager::Execute_UI_SWAPADVANCEMENTITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_SWAPADVANCEMENTITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		const MItem* pItem = g_pInventory->GetItem( left, right );

		if (pItem!=NULL && g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
		{
			
			
			if (!pItem->IsUniqueItem() && !pItem->IsQuestItem())
			{
				//-----------------------------------------------------
				
				//-----------------------------------------------------
					CGShopRequestSell	_CGShopRequestSell;

					_CGShopRequestSell.setObjectID( (*g_pPCTalkBox).GetNPCID() );		// NPC ID	
					_CGShopRequestSell.setOpCode( SHOP_REQUEST_SWAP_ADVANCEMENT_ITEM );
					_CGShopRequestSell.setItemObjectID( pItem->GetID() );	// Item Object ID

					g_pSocket->sendPacket( &_CGShopRequestSell );
			
						
					//-------------------------------------------------
					
					//-------------------------------------------------
					(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_SELL;
					(*g_pTempInformation).pValue = (void*)pItem;

					//-------------------------------------------------
					
					//-------------------------------------------------
					UI_LockItemTrade();
			}
			else
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[UI_STRING_MESSAGE_SWAP_ADVANCEMENT_ITEM_ERROR ].GetString());
			}
		}
		else
		{
			DEBUG_ADD("[Error] Item Is NULL");
		}
	}
}

void
UIMessageManager::Execute_UI_LEARN_ADVANCE_SKILL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_LEARN_ADVANCE_SKILL");
	CGLearnSkill _CGLearnSkill;
	
	_CGLearnSkill.setSkillType( left );
	_CGLearnSkill.setSkillDomainType( right );
	
	g_pSocket->sendPacket( &_CGLearnSkill );
}


void
UIMessageManager::Execute_UI_CAMPAIGN_HELP(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CAMPAIGN_HELP");
	
	

	if(left>0)
	{
		CGDonationMoney _CGDonationMoney;
		_CGDonationMoney.setGold(left*10000);
		_CGDonationMoney.setDonationType(right);
		g_pSocket->sendPacket( &_CGDonationMoney );
	}

}


void
UIMessageManager::Execute_UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT(int left, int right, void* void_ptr)
{
	
	DEBUG_ADD("[UI] Execute_UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT");
	gC_vs_ui.RunNextGQuestExcuteElement();
}

void
UIMessageManager::Execute_UI_GQUEST_SET_ACTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_GQUEST_SET_ACTION");
	
	if(left == 1) // dead
	{
		g_pPlayer->SetDead();
		g_pPlayer->SetResurrectZoneID(right);
	}
}

void
UIMessageManager::Execute_UI_GQUEST_ENDING_EVENT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_GQUEST_ENDING_EVENT");
	
	MEvent event;
	event.eventID = EVENTID_ADVANCEMENT_QUEST_ENDING;
	event.eventType = EVENTTYPE_ZONE;
	event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
	switch(left)
	{
	case 1: 
		event.parameter1 = 0;
		event.parameter4 = 1;
		g_pEventManager->AddEvent(event);
		break;
	case 2: 
		event.parameter1 = 0;
		event.parameter4 = 3;
		g_pEventManager->AddEvent(event);
		break;
	case 3: 
		event.parameter1 = 0;
		event.parameter4 = 5;
		g_pEventManager->AddEvent(event);
		break;
	}
	
}



void
UIMessageManager::Execute_UI_REQUEST_EVENT_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_REQUEST_EVENT_ITEM");
	{
		CGGetEventItem _CGGetEventItem;
		_CGGetEventItem.setEventType(left);
		g_pSocket->sendPacket( &_CGGetEventItem );
	}
}

#ifdef __TEST_SUB_INVENTORY__   
	
	void
	UIMessageManager::Execute_UI_CLOSE_INVENTORY_SUB(int left, int right, void* void_ptr)
	{
		DEBUG_ADD("[UI] Execute_UI_CLOSE_INVENTORY_SUB");
		{
			gC_vs_ui.CloseSubInventory();
		}
	}

	void
	UIMessageManager::Execute_UI_ITEM_DROP_TO_INVENTORY_SUB(int left, int right, void* void_ptr)
	{
		DEBUG_ADD("[UI] Execute_UI_ITEM_DROP_TO_INVENTORY_SUB");
		
		if (g_Mode!=MODE_GAME || g_pPlayer->IsDead() || !gC_vs_ui.IsRunningSubInventory())
		{
			DEBUG_ADD("Not Mode MODE_GAME or Dead");
			return;
		}

		MItem* pMultiPackItem = gC_vs_ui.GetSubInventoryItem();

		if(NULL == pMultiPackItem)
			return;

		MItem* pMouseItem = UI_GetMouseItem();	


		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		if (g_pPlayer->IsItemCheckBufferNULL() && pMouseItem!=NULL
			&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
		{		
			if(pMouseItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
				return;
			TYPE_OBJECTID toInventoryItemID = (pMouseItem==NULL)? OBJECTID_NULL : pMouseItem->GetID();
			
			CGAddMouseToInventory _CGAddMouseToInventory;
			_CGAddMouseToInventory.setObjectID( toInventoryItemID );
			_CGAddMouseToInventory.setInvenX( left );//pItem->GetGridX() );
			_CGAddMouseToInventory.setInvenY( right );//pItem->GetGridY() );
			_CGAddMouseToInventory.setInventoryItemObjectID(pMultiPackItem->GetID());

			g_pSocket->sendPacket( &_CGAddMouseToInventory );

			MItem* pOldItem = NULL;
			if (gC_vs_ui.ReplaceSubInventoryItem(pMouseItem,		
												left, right,	
												pOldItem))		
			{
				if (pOldItem != NULL) 
				{				
					UI_PickUpItem( pOldItem );
				}
				else
				{
					UI_DropItem();
				}
			}
		}
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		else
		{
			DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer or Mouse NULL");		

			if(!g_pPlayer->IsItemCheckBufferNULL())
				DEBUG_ADD_FORMAT("[ITEMDROP] !g_pPlayer->IsItemCheckBufferNULL() %d", g_pPlayer->GetItemCheckBufferStatus());
			if(pMouseItem == NULL)
				DEBUG_ADD("[ITEMDROP] pMouseItem == NULL)");
			if(g_pTempInformation->GetMode()!=TempInformation::MODE_NULL)
				DEBUG_ADD_FORMAT("[ITEMPDROP] g_pTempInformation->Mode!=TempInformation::MODE_NULL %d", g_pTempInformation->Mode);
		}
	}


	void
	UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_INVENTORY_SUB(int left, int right, void* void_ptr)
	{
		DEBUG_ADD("[UI] UI_ITEM_PICKUP_FROM_INVENTORY"); 
		
		if (g_Mode!=MODE_GAME || g_pPlayer->IsDead() || !gC_vs_ui.IsRunningSubInventory())
		{
			DEBUG_ADD("Not Mode MODE_GAME or Dead");
			return;
		}
		MItem* pItem = (MItem*)void_ptr;

		MItem* pMultiPackItem = gC_vs_ui.GetSubInventoryItem();

		TYPE_OBJECTID MultiPackItemID = (pMultiPackItem==NULL)? OBJECTID_NULL : pMultiPackItem->GetID();

		UI_PickUpItem( pItem );
		//---------------------------------------------------
		
		
		//---------------------------------------------------
		CGAddInventoryToMouse _CGAddInventoryToMouse;
		_CGAddInventoryToMouse.setObjectID( pItem->GetID() );
		_CGAddInventoryToMouse.setX( pItem->GetGridX() );
		_CGAddInventoryToMouse.setY( pItem->GetGridY() );
		_CGAddInventoryToMouse.setInventoryItemObjectID(MultiPackItemID);
		
		g_pSocket->sendPacket( &_CGAddInventoryToMouse );				
	}

	


	// 2005, 3, 2, sobeit add start
	void
	UIMessageManager::Execute_UI_ITEM_USE_SUBINVENTORY(int left, int right, void* void_ptr)
	{
		DEBUG_ADD("[UI] Execute_UI_ITEM_USE_SUBINVENTORY"); 
		
		if (g_Mode!=MODE_GAME || g_pPlayer->IsDead() )
		{
			DEBUG_ADD("Not Mode MODE_GAME or Dead");
			return;
		}
		MItem* pSubInventory = g_pInventory->GetItem(right);

		if(NULL == pSubInventory)
			return;

		MItem* pItem = ((MSubInventory*)pSubInventory)->GetItemToModify( left );//(MItem*)void_ptr;

		if (pItem!=NULL && g_pPlayer->IsItemCheckBufferNULL())
		{
			
			if(g_pPlayer->IsSlayer() && pItem->IsSlayerItem() ||
				g_pPlayer->IsVampire() && pItem->IsVampireItem() ||
				g_pPlayer->IsOusters() && pItem->IsOustersItem())
			{
				pItem->UseInventory(pSubInventory->GetID());
			}
		}
		//----------------------------------------------------
		// item check buffer not null
		//----------------------------------------------------
		else
		{
			#ifdef OUTPUT_DEBUG
				if (pItem==NULL)
				{
					DEBUG_ADD("[Wait] Item is NULL");
				}
				else 
				{
					DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
				}
			#endif
		}
	}

	// 2005, 3, 2, sobeit add end
#endif
