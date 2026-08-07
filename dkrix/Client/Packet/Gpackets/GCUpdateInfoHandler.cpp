//----------------------------------------------------------------------
//
// Filename    : GCUpdateInfoHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "MTopView.h"
#include "GCUpdateInfo.h"
#include "ClientDef.h"
#include "AddonDef.h"
#include "MGameTime.h"
#include "MQuickSlot.h"
#include "MEffectStatusDef.h"
#include "MSkillManager.h"
#include "MMoneyManager.h"
#include "Platform/AudioManager.h"
#include "COrderedList.h"
#include "MZoneTable.h"
#include "UIDialog.h"
#include "TalkBox.h"
#include "MWeather.h"
#include "MStorage.h"
//#include "MFileDef.h"
#include "Packet/Properties.h"
#include "ServerInformation.h"
#include "UserInformation.h"
#include "MEventManager.h"
#include "UIMessageManager.h"
#include "ServerInfo.h"
#include "../ClientPlayer.h"
#include "UIFunction.h"
#include "MGuildInfoMapper.h"
#include "CSystemInfo.h"
#include <stdio.h>

#ifdef __GAME_CLIENT__

	#include "../ClientPlayer.h"
	#include "../PCSlayerInfo2.h"
	#include "../PCVampireInfo2.h"
	#include "../PCOustersInfo2.h"
	#include "../Cpackets/CGReady.h"
	#include "VS_UI_GameCommon2.h"
	#include "MGameStringTable.h"
#endif

static void TraceGameEntryFlow(const char* step)
{
	(void)step;
}

static void TraceGameEntryValue(const char* label, int value)
{
	char buffer[128];
	sprintf(buffer, "%s=%d", label, value);
	TraceGameEntryFlow(buffer);
}



extern DWORD		g_CurrentTime;
extern DWORD		g_CurrentFrame;
extern MGameTime*	g_pGameTime;
extern CMessageArray*			g_pNoticeMessage;

extern int			g_nZoneLarge;
extern MZone*		g_pZoneLarge;				
extern bool			g_bZonePlayerInLarge;

extern bool			LoadingAddonSPK( bool bLoadingAll );
extern bool				g_bHALAvailable;
extern DWORD			g_dwVideoMemory;
extern BOOL g_bEnable3DHAL;
extern WORD				g_ZoneCreatureColorSet;
extern WORD				g_MyBatColorSet;


DWORD	g_PreviousCreatureType = -1;
int		g_MorphCreatureType = 0;

//----------------------------------------------------------------------



//----------------------------------------------------------------------
void GCUpdateInfoHandler::execute ( GCUpdateInfo * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	TraceGameEntryFlow("GCUpdateInfoHandler begin");

	#ifndef __DEBUG_OUTPUT__ 
		DEBUG_ADD("Execute UpdateInfo");
	#endif

	
	g_pEventManager->RemoveAllEventByType(EVENTTYPE_ZONE);

	//-----------------------------------------------------------
	
	
	//-----------------------------------------------------------
	CLIENT_MODE	previousMode = g_Mode;
	g_Mode = MODE_WAIT_UPDATEINFO;
	TraceGameEntryFlow("GCUpdateInfoHandler set MODE_WAIT_UPDATEINFO");

	//UI_SaveUserOption();

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (g_pServerInformation!=NULL)
	{
		g_pServerInformation->SetServerGroupStatus( pPacket->getServerStat() );
	}

	//-----------------------------------------------------------
	
	
	//-----------------------------------------------------------
	LoadingAddonSPK( true );
	TraceGameEntryFlow("GCUpdateInfoHandler after LoadingAddonSPK");

//	g_pUserOption->LoadFromFile( FILE_INFO_USEROPTION );
//	UI_AffectUserOption();
	

	int i;

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	g_SDLAudio.SetMute();

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	DWORD	startTime	= timeGetTime();
	
	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

	//--------------------------------------------------
	
	//--------------------------------------------------
	pClientPlayer->setPlayerStatus( CPS_WAITING_FOR_LOADING );
	TraceGameEntryFlow("GCUpdateInfoHandler set CPS_WAITING_FOR_LOADING");

	
	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	Race wasInterfaceRace = UI_GetInterfaceRace();//TRUE;
	int spActionInfo = ACTIONINFO_NULL;
	g_PreviousCreatureType = -1;

	if (g_pPlayer!=NULL)
	{	
		if (g_MorphCreatureType!=0)
		{
			g_PreviousCreatureType = g_MorphCreatureType;
			g_MorphCreatureType = 0;
		}
		else
		{
			g_PreviousCreatureType = g_pPlayer->GetCreatureType();
		}

		//--------------------------------------------------
		
		//--------------------------------------------------
		/*
		if (g_pPlayer->IsVampire())
		{
			wasSlayerInterface = FALSE;
		}
		*/
		
		if (g_pPlayer->IsDead())
		{
			//UI_CloseAllDialog();
		}
		

		//--------------------------------------------------
		
		//--------------------------------------------------
		spActionInfo = g_pPlayer->GetSpecialActionInfo();
		if (g_pZone!=NULL)
		{
			g_pZone->RemoveCreature( g_pPlayer->GetID() );
		}
		delete g_pPlayer;
	}	
	g_pPlayer = new MPlayer;
	DEBUG_ADD_FORMAT("[g_pPlayer] %x", g_pPlayer);
	TraceGameEntryFlow("GCUpdateInfoHandler recreated g_pPlayer");

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	DEBUG_ADD( "Remove All Addon" );
	
	for (i=0; i<ADDON_MAX; i++)
	{
		g_pPlayer->RemoveAddon((enum ADDON)i);
	}			

	g_pPlayer->SetBasicActionInfo( SKILL_ATTACK_MELEE );
	g_pPlayer->SetBasicAttackDistance( 1 );
		


	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	DEBUG_ADD( "SkillInfoTable Init" );
	g_pSkillInfoTable->Init();

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	DEBUG_ADD( "SkillManager Init" );
	g_pSkillManager->Init();	

	g_PreviousCreatureType = -1;
	g_MorphCreatureType = 0;

	EffectInfo *pEInfo = new EffectInfo;	
	*pEInfo = *pPacket->getEffectInfo();
	
	if( pEInfo != NULL )
	{
		for( int i=0;i<pEInfo->getListNum();i++)
		{
			EFFECTSTATUS	status = (EFFECTSTATUS) pEInfo->popFrontListElement();
			WORD			delay = pEInfo->popFrontListElement();
			
			if( status == EFFECTSTATUS_TRANSFORM_TO_BAT )
			{
				g_PreviousCreatureType = CREATURETYPE_BAT;
			} else
			if( status == EFFECTSTATUS_TRANSFORM_TO_WOLF )
			{
				g_PreviousCreatureType = CREATURETYPE_WOLF;
			} else
			if( status == EFFECTSTATUS_TRANSFORM_TO_WERWOLF )
			{
				g_PreviousCreatureType = CREATURETYPE_WER_WOLF;
			}
		}
		delete pEInfo;
	}
	TraceGameEntryFlow("GCUpdateInfoHandler after effect preload");

	g_pGuildInfoMapper->ClearGuildName();

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------		
	TraceGameEntryValue("GCUpdateInfoHandler previousMode", previousMode);
	TraceGameEntryValue("GCUpdateInfoHandler wasInterfaceRace", wasInterfaceRace);
	TraceGameEntryValue("GCUpdateInfoHandler pcType", pPacket->getPCInfo()->getPCType());
	switch (pPacket->getPCInfo()->getPCType())
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_SLAYER :
		{
			SetPCSlayerInfo( (PCSlayerInfo2*)pPacket->getPCInfo() );

			if (previousMode != MODE_GAME || wasInterfaceRace != RACE_SLAYER)
			{
				UI_ChangeInterfaceRace(RACE_SLAYER);
			}
		}
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_VAMPIRE :
		{
			SetPCVampireInfo( (PCVampireInfo2*)pPacket->getPCInfo() );

			if (previousMode != MODE_GAME || wasInterfaceRace != RACE_VAMPIRE)
			{
				UI_ChangeInterfaceRace(RACE_VAMPIRE);
			}
		}
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_OUSTERS :
		{
			SetPCOustersInfo( (PCOustersInfo2*)pPacket->getPCInfo() );
			
			if (previousMode != MODE_GAME || wasInterfaceRace != RACE_OUSTERS)
			{
				UI_ChangeInterfaceRace(RACE_OUSTERS);
			}
		}
		break;
	}	
	TraceGameEntryFlow("GCUpdateInfoHandler after SetPCInfo");
	TraceGameEntryValue("GCUpdateInfoHandler playerCreatureType", g_pPlayer->GetCreatureType());
	TraceGameEntryValue("GCUpdateInfoHandler playerRace", g_pPlayer->GetRace());
	// 2004, 6, 4 sobeit add start - sms charge
	g_char_slot_ingame.m_SMS_Charge		= pPacket->GetSMSCharge();
	// 2004, 6, 4 sobeit add end - sms charge
	
	// 2004, 6, 15 sobeit add end - nick name
	NicknameInfo* TempNick	= pPacket->getNicknameInfo();
	const char* szNickName;
	if(TempNick != NULL)
	{
		if(TempNick->getNicknameType() == NicknameInfo::NICK_NONE)
		{
			szNickName = "";
		}
		else 
		if(TempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM_FORCED ||
			TempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM)
		{
			szNickName = (TempNick->getNickname()).c_str();
		}
		else 
		{
			if(g_pNickNameStringTable == NULL || g_pNickNameStringTable->GetSize() == 0)
			{
				szNickName = "No Title";
			}
			else
			{
				WORD tempIndex = TempNick->getNicknameIndex();
				if(tempIndex >= g_pNickNameStringTable->GetSize())
					tempIndex = 0;
				szNickName = (*g_pNickNameStringTable)[tempIndex].GetString();
			}
		}

		g_pPlayer->SetNickName(TempNick->getNicknameType(), (char*)szNickName);
//		g_char_slot_ingame.m_NickNameType = TempNick->getNicknameType();
//		g_char_slot_ingame.m_NickName = szNickName;
	}
	TraceGameEntryFlow("GCUpdateInfoHandler after SetNickName");
	// 2004, 6, 15 sobeit add end - nick name

	
	g_pUserInformation->IsNonPK = (pPacket->isNonPK())?true:false;
	
	
	
	g_pUserInformation->dwUnionID = pPacket->getGuildUnionID();
	g_pUserInformation->bUnionGrade = pPacket->getGuildUnionUserType();
	g_pPlayer->SetUnionGuildID(g_pUserInformation->dwUnionID);
	// 2004, 10, 18, sobeit add end
	
	
	g_char_slot_ingame.m_Powerjjang_Point		= pPacket->GetPowerjjang_Point();
	// 2004, 12, 2, sobeit add end

	//--------------------------------------------------
	
	//--------------------------------------------------
	g_pPlayer->SetSpecialActionInfo( spActionInfo );

	//--------------------------------------------------
	
	//--------------------------------------------------
	g_pPlayer->SetAttackModeNormal();



	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (g_pStorage!=NULL)
	{
		delete g_pStorage;
		g_pStorage = NULL;		
	}

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	UI_UnlockGear();
	UI_UnlockItem();
	UI_UnlockItemTrade();
	UI_CloseShop();
	UI_CloseBookcase();
	UI_CloseExchange();
	UI_CloseExchangeAsk();
	UI_CloseExchangeCancel();
	UI_CloseRepair();
	UI_CloseSell();
	UI_CloseStorage();
	UI_ClosePartyCancel();
	UI_ClosePartyAsk();
	UI_ClosePartyRequest();
	UI_ClosePartyCancel();
	g_pUIMessageManager->Execute(UI_CLOSE_TRACE_WINDOW, 0, 0, NULL);

	if (g_pUIDialog!=NULL)
	{
		g_pUIDialog->ClosePCTalkDlg();
		g_pUIDialog->CloseMessageDlg();
//		g_pUIDialog->CloseHelpDlg();
	}

	if (g_pPCTalkBox!=NULL)
	{
		g_pPCTalkBox->Release();
	}
		

	//--------------------------------------------------
	
	//--------------------------------------------------	
	g_pQuickSlot = NULL;
	g_pArmsBand1 = NULL;
	g_pArmsBand2 = NULL;

				
	UI_StartProgress( pPacket->getZoneID() );
	TraceGameEntryFlow("GCUpdateInfoHandler after UI_StartProgress");

	//UI_SetCurrentServerName(g_pServerInformation->GetServerGroupName(), 
	//						g_pServerInformation->GetServerName(), 
	//						g_pServerInformation->GetServerStatus());

	UI_DrawProgress(0);
	TraceGameEntryFlow("GCUpdateInfoHandler after UI_DrawProgress0");

	DEBUG_ADD("Init g_pTopView");
	
	//------------------------------
	
	//------------------------------
	if (!g_pTopView->IsInit())
	{
		g_pTopView->Init();			
	}
	TraceGameEntryFlow("GCUpdateInfoHandler after TopViewInit");

	UI_DrawProgress( 15 );
	TraceGameEntryFlow("GCUpdateInfoHandler after UI_DrawProgress15");
	
	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	DEBUG_ADD("[UpdateInfo] Set Weather");
	
	//--------------------------------------------------
	
	
	//--------------------------------------------------
	if (g_pWeather->GetWeatherType()==MWeather::WEATHER_SNOW
		&& pPacket->getWeather()!=WEATHER_SNOWY
		
		|| g_pWeather->GetWeatherType()==MWeather::WEATHER_RAIN
		&& pPacket->getWeather()!=WEATHER_RAINY	)
	{
		g_pWeather->Release();
	}
	
	SetWeather(pPacket->getWeather(), pPacket->getWeatherLevel());

	DEBUG_ADD_FORMAT( "[UpdateInfo] Set Dark/Light : %d / %d", pPacket->getDarkLevel(), pPacket->getLightLevel());
	
	int darkLevel = pPacket->getDarkLevel();
	int lightSight = pPacket->getLightLevel();

	if (g_pUserInformation->IsMaster)
	{
		darkLevel = 0;
		lightSight = 13;
	}

	
	g_pTopView->SetDarkBits( darkLevel );		
	
	
	g_pPlayer->SetTimeLightSight( lightSight );

	
	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	DEBUG_ADD( "Set Game Time" );
	
	GameTime gt = pPacket->getGameTime();
	
	
	g_pGameTime->SetStartTime(g_CurrentTime, 
							gt.getYear(),
							gt.getMonth(),
							gt.getDay(),
							gt.getHour(),
							gt.getMinute(),
							gt.getSecond()
							);
  
	
	g_pGameTime->SetTimeRatio( 24 );


	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	
	TraceGameEntryValue("GCUpdateInfoHandler gearListNumBefore", pPacket->getGearInfo() != NULL ? pPacket->getGearInfo()->getListNum() : -1);
	SetGearInfo( pPacket->getGearInfo(), pPacket->getBloodBibleSignInfo() );
	
	//--------------------------------------------------
	
	//--------------------------------------------------
	g_pPlayer->CalculateStatus();
	TraceGameEntryFlow("GCUpdateInfoHandler after CalculateStatus");


	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	/*
	MItem* pItem = MItem::NewItem( ITEM_CLASS_BELT );
			
	// object ID
	pItem->SetID( 9999 );

	// type / option
	pItem->SetItemType(	1 );
	pItem->SetItemOption( 0 );

	g_SlayerGear.AddItem( pItem, (MSlayerGear::GEAR_SLAYER)MSlayerGear::GEAR_SLAYER_BELT );			
	*/
	 

	 


	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	TraceGameEntryValue("GCUpdateInfoHandler inventoryListNumBefore", pPacket->getInventoryInfo() != NULL ? pPacket->getInventoryInfo()->getListNum() : -1);
	SetInventoryInfo( pPacket->getInventoryInfo() );
	TraceGameEntryFlow("GCUpdateInfoHandler after SetInventoryInfo");


	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	TraceGameEntryValue("GCUpdateInfoHandler extraListNumBefore", pPacket->getExtraInfo() != NULL ? pPacket->getExtraInfo()->getListNum() : -1);
	SetExtraInfo( pPacket->getExtraInfo() );
	TraceGameEntryFlow("GCUpdateInfoHandler after SetExtraInfo");


	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	if (pPacket->hasMotorcycle())
	{
		//--------------------------------------------
		
		//--------------------------------------------
		MItem*	pMotor = new MMotorcycle;	
		pMotor->SetID( 0 );
		pMotor->SetItemType( 0 ); 
//		pMotor->SetItemOption( 0 );
						
		//--------------------------------------------
		
		//--------------------------------------------
		RideMotorcycleInfo* pMotorInfo = pPacket->getRideMotorcycleInfo();

		if (pMotorInfo!=NULL)
		{
			pMotor->SetID( pMotorInfo->getObjectID() );
			pMotor->SetItemType( pMotorInfo->getItemType() );
			pMotor->SetItemOptionList( pMotorInfo->getOptionType() );

			int num = pMotorInfo->getListNum();

			for (i=0; i<num; i++)
			{
				RideMotorcycleSlotInfo* pMotorSlotInfo = pMotorInfo->popFrontListElement();

				if (pMotorSlotInfo!=NULL)
				{
					
					

					delete pMotorSlotInfo;
				}
			}
		}

		//--------------------------------------------
		
		//--------------------------------------------
		g_pPlayer->SetAddonItem( pMotor );	
		
		// -_-;;		
		
		
		delete pMotor;	
	}

	UI_DrawProgress(17);

	//--------------------------------------------------
	
	//--------------------------------------------------
	g_pSkillAvailable->SetAvailableSkills();

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	// [ TEST CODE ]
	
//	DEBUG_ADD( "Load Creature Sprites" );
	
	//----------------------------------------
	
	//----------------------------------------
	
//	LoadCreature( 0 );
//	UI_DrawProgress(25);
//

//	LoadCreature( 1 );
//	UI_DrawProgress(35);
//

//	LoadCreature( 2 );
//	UI_DrawProgress(38);
//

//	LoadCreature( 3 );
//	UI_DrawProgress(39);
//

//	LoadCreature( 58 );

//	LoadCreature( 59 );
//	UI_DrawProgress(40);


	int numNPC = pPacket->getNPCCount();
	int numMonster = pPacket->getMonsterCount();
	
	int total = numNPC+numMonster;

	if (total==0) total = 1;

	int progress = 40;
	int progressInc = 30 / total;

	COrderedList<int>	listUse;

	//--------------------------------------------------
	// NPC Load
	//--------------------------------------------------
	for (i=0; i<numNPC; i++)
	{
		int spriteType = pPacket->getNPCType( i );

		
		//LoadCreature( spriteType );

		
		listUse.Add( spriteType );

		progress += progressInc;
		UI_DrawProgress(progress);		
	}

	//--------------------------------------------------
	// Monster Load
	//--------------------------------------------------
	for (i=0; i<numMonster; i++)
	{	
		int spriteType = pPacket->getMonsterType( i );

		
		if(g_pUserOption->IsPreLoadMonster)
			LoadCreature( spriteType );

		
		listUse.Add( spriteType );

		progress += progressInc;
		UI_DrawProgress(progress);		
	}

	//--------------------------------------------------
	
	//	
	
	//--------------------------------------------------
	if (g_bZonePlayerInLarge && g_nZoneLarge!=ZONEID_NULL && g_pZoneLarge!=NULL)
	{
		ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( pPacket->getZoneID() );

		if (pZoneInfo->Property & FLAG_ZONESIZE_SMALL)
		{
			g_pTopView->AddMonsterSpriteTypes( listUse );
		}
		//--------------------------------------------------
		
		//--------------------------------------------------	
		else
		{
			ReleaseUselessCreatureSPKExcept( listUse ); 
		}
	}	
	else
	{
		g_pTopView->AddMonsterSpriteTypes( listUse );
	}
	//*/

	if (progress<70)
	{
		UI_DrawProgress( 70 );
	}
	TraceGameEntryFlow("GCUpdateInfoHandler after creature preload");

	//--------------------------------------------------
	// Zone Loading...			
	//--------------------------------------------------
	
	DEBUG_ADD( "Load Zone" );
	
	//--------------------------------------------------
	
	//--------------------------------------------------
	TraceGameEntryFlow("GCUpdateInfoHandler before zone fields");
	ZoneID_t zoneID = pPacket->getZoneID();
	Coord_t zoneX = pPacket->getZoneX();
	Coord_t zoneY = pPacket->getZoneY();
	TraceGameEntryValue("GCUpdateInfoHandler zoneID", zoneID);
	TraceGameEntryValue("GCUpdateInfoHandler zoneX", zoneX);
	TraceGameEntryValue("GCUpdateInfoHandler zoneY", zoneY);

	g_pPlayer->SetX( zoneX );
	g_pPlayer->SetY( zoneY );
	TraceGameEntryFlow("GCUpdateInfoHandler after SetXY");

	if (previousMode == MODE_GAME)
	{
		TraceGameEntryFlow("GCUpdateInfoHandler before MoveZone");
		MoveZone( zoneID );
		TraceGameEntryFlow("GCUpdateInfoHandler after MoveZone");
	}
	
	else
	{
		TraceGameEntryFlow("GCUpdateInfoHandler before LoadZone");
		if (!LoadZone( zoneID ))
		{
			TraceGameEntryFlow("GCUpdateInfoHandler ERROR LoadZone failed");
			SetMode( MODE_QUIT );
			return;
		}
	}
	TraceGameEntryFlow("GCUpdateInfoHandler after LoadZone");

	if (g_pZone == NULL || g_Mode == MODE_QUIT)
	{
		TraceGameEntryFlow("GCUpdateInfoHandler ERROR zone unavailable after load");
		return;
	}
	
	// Packet Encrypt
	g_pSocket->setEncryptCode();
	TraceGameEntryFlow("GCUpdateInfoHandler after setEncryptCode");

	//-----------------------------------------------------------
	//
	
	//
	//-----------------------------------------------------------
	UI_ClearNPC();
	while (1)
	{
		NPCInfo* pInfo = pPacket->popNPCInfo();

		if (pInfo==NULL)
		{
			break;
		}

		const char* pName = pInfo->getName().c_str();
		int npcID = pInfo->getNPCID();
		int x = pInfo->getX();
		int y = pInfo->getY();

		UI_SetNPCInfo( pName, npcID, x, y );

		delete pInfo;		
	}
	TraceGameEntryFlow("GCUpdateInfoHandler after NPC loop");
	UI_SyncLoadedGameState();
	TraceGameEntryFlow("GCUpdateInfoHandler after UI_SyncLoadedGameState");

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	
	//--------------------------------------------------
	
	//--------------------------------------------------
	
	//--------------------------------------------------
	g_CurrentTime = timeGetTime();	
	
	
	
	int delayedFrame = ConvertMillisecondToFrame( g_CurrentTime - startTime );		

	
	g_CurrentFrame += delayedFrame;
	g_MyBatColorSet = 0xFFFF;

	switch (pPacket->getPCInfo()->getPCType())
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_VAMPIRE :
		{
			PCVampireInfo2 *pInfo = dynamic_cast< PCVampireInfo2* >( pPacket->getPCInfo() );
			if( pInfo->getBatColor() != 0 )
				g_MyBatColorSet = pInfo->getBatColor();

//			g_pPlayer->SetChangeColorSet( pInfo->getBatColor() );
		}
		break;
	}



	SetEffectInfo( g_pPlayer, pPacket->getEffectInfo(), delayedFrame );
	TraceGameEntryFlow("GCUpdateInfoHandler after SetEffectInfo");

	
	UI_DrawProgress(100);
	UI_DrawProgress(100);

	//--------------------------------------------------
	//
	// Skill Tree
	//
	//--------------------------------------------------
	 

	/*
	g_SkillAvailable.AddSkill( SKILL_DOUBLE_IMPACT );
	g_SkillAvailable.AddSkill( SKILL_TRIPLE_SLASHER );
	g_SkillAvailable.DisableSkill( SKILL_TRIPLE_SLASHER );
	g_SkillAvailable.AddSkill( SKILL_SCREW_SLASHER );
	
	g_SkillAvailable.AddSkill( SKILL_SINGLE_BLOW );
	g_SkillAvailable.DisableSkill( SKILL_SINGLE_BLOW );
	
	g_SkillAvailable.AddSkill( SKILL_ARMOR_CRASH );

	g_SkillAvailable.AddSkill( SKILL_SNIPPING );
	g_SkillAvailable.AddSkill( SKILL_KNOCKS_TARGET_BACK );

	g_SkillAvailable.AddSkill( MAGIC_LIGHT );
	g_SkillAvailable.AddSkill( MAGIC_DETECT_HIDDEN );
	g_SkillAvailable.DisableSkill( MAGIC_DETECT_HIDDEN );
	
	g_SkillAvailable.AddSkill( MAGIC_CURE_POISON );
	g_SkillAvailable.AddSkill( MAGIC_PROTECTION_FROM_CURSE );
	
	g_SkillAvailable.AddSkill( MAGIC_CURE_POISON );
	g_SkillAvailable.DisableSkill( MAGIC_CURE_POISON );
	g_SkillAvailable.AddSkill( MAGIC_PROTECTION_FROM_POISON );
	g_SkillAvailable.AddSkill( MAGIC_CURE_PARALYSIS );
	*/

	UI_EndProgress();	
	TraceGameEntryFlow("GCUpdateInfoHandler after UI_EndProgress");


	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	g_SDLAudio.UnSetMute();
	TraceGameEntryFlow("GCUpdateInfoHandler after UnSetMute");

	
	UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	
	//UI_AffectUserOption();

	//--------------------------------------------------
	
	//--------------------------------------------------
	g_pSystemMessage->Clear();
	g_pGameMessage->Clear();
	g_pNoticeMessage->Clear();
	TraceGameEntryFlow("GCUpdateInfoHandler after ClearMessages");

	//--------------------------------------------------
	
	//--------------------------------------------------
	SetMode( MODE_WAIT_SETPOSITION );	
	TraceGameEntryFlow("GCUpdateInfoHandler set MODE_WAIT_SETPOSITION");


//	FILE *fp=fopen("InventoryLog.txt","rt");
//	if( fp != NULL )
//	{
//		char szBuffer[128] = "InventoryLogStart012301230123012301230123012301230123InventoryLogEnd";		                                       
//		fseek(fp,0,SEEK_END);
//		int filesize = ftell(fp);
//
//		if( filesize < strlen(szBuffer) )
//		{
//			char filebuffer[128];
//			sprintf(filebuffer,"InventoryLog:%d", filesize);
//			
//			SendBugReport( filebuffer );
//		}
//		fclose(fp);
//	}

/*
	FILE *fp=fopen("fs.his","rt");
	if( fp == NULL )
	{
		fp = fopen("fs.his","wt");
		fprintf(fp,"!@#");
		CSystemInfo SystemInfo;
		SendBugReport("SystemInfo : %d %d CPU : %dMhz",g_bEnable3DHAL, g_dwVideoMemory,SystemInfo.GetCpuClock());
	}
	fclose(fp);
*/
//	ExecuteHelpEvent( HELP_EVENT_INTERFACE );

#endif

	__END_CATCH
}
