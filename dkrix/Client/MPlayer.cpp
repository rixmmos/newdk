//----------------------------------------------------------------------
// MPlayer.cpp
//----------------------------------------------------------------------
//

//


//
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include <math.h>
#include "MZone.h"
#include "MTopView.h"
#include "MItem.h"
#include "MCreature.h"
#include "MInteractionObject.h"
#include "MLinearEffect.h"
#include "TempInformation.h"
#include "MParabolaEffect.h"
#include "MPlayer.h"
#include "MActionInfoTable.h"
#include "MEffectTarget.h"
#include "MEffectGeneratorTable.h"
#include "PacketDef.h" 
#include "ClientDef.h"
#include "MCreatureTable.h"
#include "MSoundTable.h"
#include "MItemTable.h"
#include "ClientConfig.h"
#include "MGameStringTable.h"
#include "MStatusManager.h"
#include "MItemOptionTable.h"
#include "TalkBox.h"
#include "UIDialog.h"
#include "MHelpManager.h"
#include "MPortal.h"
#include "PacketFunction.h"
#include "MGameTime.h"
#include "MObjectSelector.h"
#include "SoundDef.h"
#include "MItemFinder.h"
#include "Packet/Cpackets/CGSelectTileEffect.h"
#include "Packet/Cpackets/CGPartyInvite.h"
#include "EffectSpriteTypeDef.h"
#include "ModifyStatusManager.h"
#include "Packet/ClientCommunicationManager.h"
#include "MEventManager.h"
#include "MItemFinder.h"
#include "MZoneTable.h"

#include "Packet/Rpackets/RCPositionInfo.h"
#include "Packet/Rpackets/RCStatusHP.h"
#include "Packet/Rpackets/RCCharacterInfo.h"
#include "Packet/Cpackets/CGRequestInfo.h"
#include "Packet/Cpackets/CGUseItemFromGear.h"

#include "UserInformation.h"
#include "MParty.h"
#include "MJusticeAttackManager.h"
#include "RequestUserManager.h"
#include "MWarManager.h"

#include "MGuildType.h"
#include "ServerInfo.h"
#include "DebugInfo.h"
#include "MTestDef.h"
#include "RankBonusTable.h"
#include "RankBonusDef.h"
#include "MOustersGear.h"

#include "Packet/Cpackets/CGAbsorbSoul.h"

#include "UIMessageManager.h"
#include "UIFunction.h"
#include "Packet/RequestClientPlayerManager.h"
#include "SystemAvailabilities.h"

#include "UserOption.h"
#include "VS_UI_GameCommon2.h"
#include <stdarg.h>
#include <stdio.h>

static void TraceInteractionPlayer(const char* fmt, ...)
{
	(void)fmt;
}

#include "Packet/Cpackets/CGPartyPosition.h"

extern void UI_RunPetInfo(struct PETINFO *pPetInfo);
extern bool UpdateSocketOutput();
//GameNew Mode  add by sonc 2006.9.27
extern	LONG g_SECTOR_WIDTH;
extern	LONG g_SECTOR_HEIGHT;
extern	LONG g_SECTOR_WIDTH_HALF;
extern	LONG g_SECTOR_HEIGHT_HALF;
extern	LONG g_SECTOR_SKIP_PLAYER_LEFT;
extern	LONG g_SECTOR_SKIP_PLAYER_UP;
extern	LONG g_TILESURFACE_SECTOR_WIDTH;
extern	LONG g_TILESURFACE_SECTOR_HEIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_DOWN;
extern	LONG g_TILESURFACE_WIDTH;
extern	LONG g_TILESURFACE_HEIGHT;
extern	LONG g_TILESURFACE_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_OUTLINE_DOWN;
//end 
//#include "Client.h"
bool		GetMakeItemFitPosition(MItem* pItem, ITEM_CLASS itemClass, int itemType, POINT& fitPoint);


void		SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID);
bool			IsCreatureMove( MCreature *pCreature );
bool			IsCreatureActionAttack( MCreature *pCreature );
bool			HasEffectStatusSummonSylph( MCreature* pCreature );
void			RemoveEffectStatusSummonSylph( MCreature* pCreature );
BYTE GetCreatureActionCountMax( const MCreature* pCreature, int action );

// #ifndef CONNECT_SERVER
	#include "MFakeCreature.h"
// #endif


#include "MTestDef.h"

#ifdef __METROTECH_TEST__
//	#define OUTPUT_DEBUG_PLAYER_ACTION
#endif



#include "VS_UI.h" // KJTINC
#include "VS_UI_Mouse_pointer.h"
#ifdef PLATFORM_WINDOWS
#include "CImm.h"
#endif

extern	DWORD	g_CurrentTime;
extern	int		g_x;
extern	int		g_y;
extern	bool	g_bNetStatusGood;
extern BOOL					g_bLButtonDown;
extern BOOL					g_bRButtonDown;
extern BOOL					g_bCButtonDown;
extern bool g_bPreviousMove;

extern int					g_nZoneLarge;
extern int					g_nZoneSmall;
extern bool					g_bZonePlayerInLarge;
extern bool					g_bZoneSafe;
extern bool					g_bHolyLand;
extern POINT						g_SelectSector;

//#define	new			DEBUG_NEW

#define				TRADE_DISTANCE_NPC		4
#define				TRADE_DISTANCE_PC		2


		

//----------------------------------------------------------------------
//
//							Global Player
//
//----------------------------------------------------------------------
MPlayer*			g_pPlayer = NULL;

//----------------------------------------------------------------------
// define Functions
//----------------------------------------------------------------------

/*
#define GET_DYING_CREATURE( pCreature, id )								\
		if (pCreature==NULL)											\
		{																\
			MItem* pItem = g_pZone->GetItem( id );						\
																		\
			if (pItem!=NULL												\
				&& pItem->GetItemClass()==ITEM_CLASS_CORPSE)		\
			{															\
				pCreature = ((MCorpse*)pItem)->GetCreature();			\
																		\
				if (pCreature->GetActionCount() >= pCreature->GetActionCountMax())	\
				{														\
					pCreature = NULL;									\
				}														\
			}															\
		}
*/

//----------------------------------------------------------------------
// CHECK_REPEAT_LIMIT_TO_RETURN_FALSE
//----------------------------------------------------------------------
#define CHECK_REPEAT_LIMIT_TO_RETURN_FALSE(nActionInfo, repeatCount)		\
		if (nActionInfo < g_pActionInfoTable->GetSize())	\
		{													\
			if (repeatCount >= (*g_pActionInfoTable)[nActionInfo].GetRepeatLimit())	\
			{												\
				UnSetRepeatAction();						\
				return false;								\
			}												\
		}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
#ifdef __METROTECH_TEST__

	int  g_iAutoHealPotion = 1;
	bool g_bAutoManaPotion = true;
	int	 g_iAutoReload = 1;
	bool g_bLight = false;
	int	 g_iSpeed = 0;

	void
	AutoUseSkill(int skill)
	{
		if (g_pSkillInfoTable != NULL
			&& g_pInventory!=NULL 
			&& g_pPlayer!=NULL
			&& g_pPlayer->IsSlayer()
			&& g_pPlayer->IsItemCheckBufferNULL()
			&& !g_pPlayer->IsInSafeSector())
		{
			bool bUse = false;
			int skill_id = ACTIONINFO_NULL;

			if(skill == 0)	//healing
			{
				const int hp_gap = g_pPlayer->GetMAX_HP()-g_pPlayer->GetHP();
				
				if(hp_gap > 0)
				{
					if(g_iAutoHealPotion == 2)	
					{
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_CRITICAL_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_CRITICAL_WOUNDS].IsEnable() && 
							(*g_pSkillInfoTable)[MAGIC_CURE_CRITICAL_WOUNDS].GetExpLevel()/2+30 <= hp_gap)
						{
							skill_id = MAGIC_CURE_CRITICAL_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_SERIOUS_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_SERIOUS_WOUNDS].IsEnable() && 
							(*g_pSkillInfoTable)[MAGIC_CURE_SERIOUS_WOUNDS].GetExpLevel()/4+30 <= hp_gap)
						{
							skill_id = MAGIC_CURE_SERIOUS_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_LIGHT_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_LIGHT_WOUNDS].IsEnable() && 
							(*g_pSkillInfoTable)[MAGIC_CURE_LIGHT_WOUNDS].GetExpLevel()/4+10 <= hp_gap)
						{
							skill_id = MAGIC_CURE_LIGHT_WOUNDS;
							bUse = true;
						}
					}
					else if(g_iAutoHealPotion == 3)	
					{
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_CRITICAL_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_CRITICAL_WOUNDS].IsEnable())
						{
							skill_id = MAGIC_CURE_CRITICAL_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_SERIOUS_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_SERIOUS_WOUNDS].IsEnable())
						{
							skill_id = MAGIC_CURE_SERIOUS_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_LIGHT_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_LIGHT_WOUNDS].IsEnable())
						{
							skill_id = MAGIC_CURE_LIGHT_WOUNDS;
							bUse = true;
						}
					}
				}
			}

			if(skill == 1)	// create holy water
			{
				if(g_pSkillAvailable->IsEnableSkill( MAGIC_CREATE_HOLY_WATER ) &&
					(*g_pSkillInfoTable)[MAGIC_CREATE_HOLY_WATER].IsEnable() &&
					(*g_pSkillInfoTable)[MAGIC_CREATE_HOLY_WATER].IsAvailableTime())
				{
					skill_id = MAGIC_CREATE_HOLY_WATER;
					bUse = true;
				}
			}

			if(bUse == true && skill_id != ACTIONINFO_NULL)
			{
				
				g_pPlayer->UnSetRequestMode();
				
				TYPE_ACTIONINFO old_special = g_pPlayer->GetSpecialActionInfo();
				g_pPlayer->SetSpecialActionInfo(skill_id);
				g_pPlayer->SelfSpecialAction();
				g_pPlayer->SetSpecialActionInfo(old_special);
			
//				g_bPreviousMove = false;
			}
		}
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	void
	AutoUsePotion(int bHeal)	// 0:heal 1:mana 2:magazine
	{
		if (g_pInventory!=NULL 
			&& g_pPlayer!=NULL
			&& !g_pPlayer->IsVampire()
			&& g_pPlayer->IsItemCheckBufferNULL()
			&& !gbl_item_lock)
		{
			// Healing
			int badClass = -1;
			int badType = -1;

			int currentValue;
			int maxValue; 
			float modifier = 1.0f;

			switch(bHeal)
			{
			case 0:	// heal
				currentValue = g_pPlayer->GetHP();
				maxValue = g_pPlayer->GetMAX_HP();
				break;

			case 1: // mana
				currentValue = g_pPlayer->GetMP();
				maxValue = g_pPlayer->GetMAX_MP();

				modifier = 1.0f + g_pPlayer->GetINT() / 300.0f;
				break;

			case 2: // magazine
			case 3: // magazine
			case 4: // magazien
			case 5: // magazine
				if(g_pCurrentMagazine == NULL)
					return;

				currentValue = g_pCurrentMagazine->GetNumber();
				maxValue = 1;
				break;
			}

			if (maxValue<=0) return;

			int lost = maxValue - currentValue;
			int percent = currentValue * 100 / maxValue;

			bool forceUse = false;
			
			ITEM_CLASS itemClass = ITEM_CLASS_POTION;
			if(g_pPlayer->IsOusters())
				itemClass = ITEM_CLASS_PUPA;

			int itemType;
			int maxItemType = 4;
			
			if(bHeal >= 2)
			{
				if(g_pSlayerGear != NULL && !g_pSlayerGear->GetItem(MSlayerGear::GEAR_SLAYER_LEFTHAND)->IsGunItem())return;
//				badType = (g_pSlayerGear->GetItem(MSlayerGear::GEAR_SLAYER_LEFTHAND)->GetItemClass()-ITEM_CLASS_SG)*2-1;
				badType = (g_pCurrentMagazine->GetItemType() & ~1) -1;
				itemClass = ITEM_CLASS_MAGAZINE;
				
				if(currentValue > bHeal-2)
					return;

				itemType = badType+2;
			}
			else
			{
				if (lost >= 200.0f * modifier)		itemType = 4;
				else if (lost >= 100.0f * modifier)	itemType = 3;
				else if (lost >= 51.0f * modifier)	itemType = 2;
				else if (lost >= 24.0f * modifier)	itemType = 1;
				else if (lost >= 10.0f * modifier)	itemType = 0;
				else								itemType = badType;
			}

			switch(bHeal)
			{
			case 0:	// Heal
				
				forceUse = percent <= 50;
				break;

			case 1:	// Mana
				badType += 5;
				itemType += 5;
				maxItemType += 5;

				
				if (g_pSkillInfoTable!=NULL)
				{
					int skill = g_pPlayer->GetSpecialActionInfo();

					if (skill < g_pSkillInfoTable->GetSize())
					{
						forceUse = currentValue < 5
									|| (*g_pSkillInfoTable)[skill].GetMP()*2 >= currentValue;
					}
				}
				break;

			}
			
			if (itemType!=badType && itemClass!=badClass)
			{
				MItem* pItem = NULL;
				int type = itemType;

				do
				{
					pItem = g_pInventory->FindItem(itemClass , type );

					
					if(bHeal >= 2 && pItem == NULL) pItem = g_pInventory->FindItem(itemClass , type +8);

					if (pItem!=NULL) break;					
				}
				while (--type > badType);

				
				if (bHeal != 2 && pItem==NULL && forceUse)
				{
					type = badType + 1;

					do
					{
						pItem = g_pInventory->FindItem( ITEM_CLASS_POTION, type );

						if (pItem!=NULL) break;					
					}
					while (++type <= maxItemType);
				}
			
				if (pItem!=NULL && g_pPlayer->IsItemCheckBufferNULL())
				{
					if(bHeal >= 2)
					{
						CGReloadFromInventory _CGReloadFromInventory;
						_CGReloadFromInventory.setObjectID( pItem->GetID() );
						_CGReloadFromInventory.setX( pItem->GetGridX() );
						_CGReloadFromInventory.setY( pItem->GetGridY() );

						g_pSocket->sendPacket( &_CGReloadFromInventory );
					}
					else
					{
						CGUsePotionFromInventory _CGUsePotionFromInventory;
						_CGUsePotionFromInventory.setObjectID( pItem->GetID() );
						_CGUsePotionFromInventory.setX( pItem->GetGridX() );
						_CGUsePotionFromInventory.setY( pItem->GetGridY() );

						g_pSocket->sendPacket( &_CGUsePotionFromInventory );
					}

					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
				}
				else if (g_pQuickSlot!=NULL)
				{
					
					type = itemType;

					do
					{
						MItemClassTypeFinder itemFinder( itemClass, type );

						pItem = g_pQuickSlot->FindItem( itemFinder );

						
						if(bHeal >= 2 && pItem == NULL)
						{
							MItemClassTypeFinder itemFinder( itemClass, type +8 );

							pItem = g_pQuickSlot->FindItem( itemFinder );
						}

						if (pItem!=NULL) break;					
					}
					while (--type > badType);

					
					if (bHeal != 2 && pItem==NULL && forceUse)
					{
						type = badType + 1;

						do
						{
							MItemClassTypeFinder itemFinder( ITEM_CLASS_POTION, type );

							pItem = g_pQuickSlot->FindItem( itemFinder );

							if (pItem!=NULL) break;					
						}
						while (++type <= maxItemType);
					}

					if (pItem!=NULL && g_pPlayer->IsItemCheckBufferNULL() )
					{
						if(bHeal >= 2)
						{
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

							CGReloadFromQuickSlot _CGReloadFromQuickSlot;
							_CGReloadFromQuickSlot.setObjectID( pItem->GetID() );
							_CGReloadFromQuickSlot.setSlotID( pItem->GetItemSlot() );							

							g_pSocket->sendPacket( &_CGReloadFromQuickSlot );
						}
						else
						{
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

							CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
							_CGUsePotionFromQuickSlot.setObjectID( pItem->GetID() );
							_CGUsePotionFromQuickSlot.setSlotID( pItem->GetItemSlot() );							

							g_pSocket->sendPacket( &_CGUsePotionFromQuickSlot );
						}

						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);

						UI_LockGear();
					}					
				}			
				
			}
		}
	}

#endif

bool	CanActionByZoneInfo()
{	
	if(g_pZone != NULL)
	{
		ZONETABLE_INFO *pZoneInfo = g_pZoneTable->Get( g_pZone->GetID() );
		
		if(pZoneInfo->CannotAttackInSafe == true)					
		{			
			if ( g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeComplete() )
				return true;
			else
			{
				if(g_pPlayer->IsSlayer() &&	g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeSlayer() )
					return true;				
				if(g_pPlayer->IsVampire() && g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeVampire() ) 
					return true;
				if(g_pPlayer->IsOusters() && g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeOusters() ) 
					return true;
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
SendPositionInfoToParty()
{
	//------------------------------------------------------
	
	//------------------------------------------------------		
	if (g_pParty->GetSize()!=0
		&& g_pClientCommunicationManager!=NULL)
	{
		static int oldX = -1;
		static int oldY = -1;
		static int oldZoneID = -1;

		int x		= g_pPlayer->GetX();
		int y		= g_pPlayer->GetY();
		int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

		
		if (oldX!=x || oldY!=y || oldZoneID!=zoneID)			
		{
			if(g_pUserInformation->bKorean == true)
			{
				RCPositionInfo _RCPositionInfo;

				
				//_RCPositionInfo.setName( g_pUserInformation->CharacterID.GetString() );
				_RCPositionInfo.setZoneX( x );
				_RCPositionInfo.setZoneY( y );
				_RCPositionInfo.setZoneID( zoneID );

				
				oldX = x;
				oldY = y;
				oldZoneID = zoneID;

				for (int i=0; i<g_pParty->GetSize(); i++)
				{
					PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
					
					if (pInfo!=NULL
						
						
						//&& !pInfo->bInSight
						)
					{
						int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

						RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

						if (pUserInfo!=NULL)
						{
							if (pUserInfo->UDPPort!=0)
								port = pUserInfo->UDPPort;
						}

						g_pClientCommunicationManager->sendPacket( pInfo->IP, 
																port, 
																&_RCPositionInfo );
					}
				}			
			}
			else	
			{
				CGPartyPosition _CGPartyPosition;

				_CGPartyPosition.setZoneID ( zoneID );
				_CGPartyPosition.setXY( x, y );
				_CGPartyPosition.setHP( g_pPlayer->GetMAX_HP(), g_pPlayer->GetHP());

				g_pSocket->sendPacket( &_CGPartyPosition );
			}
		}
	}
}

//----------------------------------------------------------------------
// Send StatusInfo To Party
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
SendStatusInfoToParty()
{
	static DWORD nextTime = g_CurrentTime;

	if (g_CurrentTime >= nextTime)
	{
		//------------------------------------------------------
		
		//------------------------------------------------------		
		if (g_pParty->GetSize()!=0
			&& g_pClientCommunicationManager!=NULL)
		{
			static int oldHP = -1;
			static int oldMaxHP = -1;
			
			int HP		= g_pPlayer->GetHP();
			int MaxHP	= g_pPlayer->GetMAX_HP();
			
			
			if (oldHP!=HP || oldMaxHP!=MaxHP)
			{
				if(g_pUserInformation->bKorean == true)
				{
					RCStatusHP _RCStatusHP;

					
					//_RCStatusHP.setName( g_pUserInformation->CharacterID.GetString() );
					_RCStatusHP.setCurrentHP( HP );
					_RCStatusHP.setMaxHP( MaxHP );
					
					
					oldHP = HP;
					oldMaxHP = MaxHP;
					
					for (int i=0; i<g_pParty->GetSize(); i++)
					{
						PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
						
						if (pInfo!=NULL
							
							
							//&& !pInfo->bInSight
							)
						{
							int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

							RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

							if (pUserInfo!=NULL)
							{
								if (pUserInfo->UDPPort!=0)
									port = pUserInfo->UDPPort;
							}

							g_pClientCommunicationManager->sendPacket( pInfo->IP, 
																	port, 
																	&_RCStatusHP );
						}
					}	
				}
				else 
				{
					int x		= g_pPlayer->GetX();
					int y		= g_pPlayer->GetY();
					int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

					CGPartyPosition _CGPartyPosition;

					_CGPartyPosition.setZoneID ( zoneID );
					_CGPartyPosition.setXY( x, y );
					_CGPartyPosition.setHP( g_pPlayer->GetMAX_HP(), g_pPlayer->GetHP());

					g_pSocket->sendPacket( &_CGPartyPosition );
				}
			}
		}

		
		
		nextTime = g_CurrentTime + g_pClientConfig->CLIENT_COMMUNICATION_STATUS_DELAY;
	}
}

//----------------------------------------------------------------------
// Send StatusInfo To Party
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
SendCharacterInfoToParty()
{
	//------------------------------------------------------
	
	//------------------------------------------------------		
	if (g_pParty->GetSize()!=0
		&& g_pClientCommunicationManager!=NULL)
	{
		int guildID	= g_pPlayer->GetGuildNumber();
		if (guildID>0)
		{
			RCCharacterInfo _RCCharacterInfo;

			
			//_RCStatusHP.setName( g_pUserInformation->CharacterID.GetString() );
			_RCCharacterInfo.setGuildID( guildID );
			
			for (int i=0; i<g_pParty->GetSize(); i++)
			{
				PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
				
				if (pInfo!=NULL
					
					
					//&& !pInfo->bInSight
					)
				{
					int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

					RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

					if (pUserInfo!=NULL)
					{
						if (pUserInfo->UDPPort!=0)
							port = pUserInfo->UDPPort;
					}

					g_pClientCommunicationManager->sendPacket( pInfo->IP, 
															port, 
															&_RCCharacterInfo );
				}
			}
		}
	}
}

//----------------------------------------------------------------------
// Is Block All Direction
//----------------------------------------------------------------------
extern POINT g_DirectionValue[MAX_DIRECTION];
bool
IsBlockAllDirection()
{
	if (g_pPlayer!=NULL && g_pZone!=NULL)
	{
		int x = g_pPlayer->GetX();
		int y = g_pPlayer->GetY();

		for (int i=0; i<8; i++)
		{
			const POINT& pt = g_DirectionValue[i];

			int sx = x + pt.x;
			int sy = y + pt.y;

			if (g_pZone->CanMove( g_pPlayer->GetMoveType(), sx, sy ))
			{
				return false;
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------
// 
// contructor/destructor
//
//----------------------------------------------------------------------
MPlayer::MPlayer()
{	
	
	m_fTrace				= FLAG_TRACE_NULL;
	m_TraceDistance			= 0;
	m_TraceObjectAction		= 0;
	m_fTraceBuffer			= FLAG_TRACE_NULL;

	m_BasicAttackDistance	= 1;

	
	m_fNextTrace			= FLAG_TRACE_NULL;
	m_NextTraceID			= OBJECTID_NULL;
	m_NextTraceX			= SECTORPOSITION_NULL;
	m_NextTraceY			= SECTORPOSITION_NULL;
	m_NextTraceZ			= 0;
	m_NextTraceObjectAction	= 0;

	
	
	m_DestX = SECTORPOSITION_NULL;
	m_DestY = SECTORPOSITION_NULL;

	
	m_NextDestX = SECTORPOSITION_NULL;
	m_NextDestY = SECTORPOSITION_NULL;

	
	m_BlockDestX = SECTORPOSITION_NULL;
	m_BlockDestY = SECTORPOSITION_NULL;

	
	m_SendMove = 0;

	
	m_DelayTime	= 0;
	
	// attack mode
	m_AttackMode = ATTACK_MODE_NORMAL;

	
	m_ItemCheckBufferStatus = ITEM_CHECK_BUFFER_NULL;
	m_pItemCheckBuffer = NULL;
	m_ItemCheckBufferTick = 0;
	#ifdef __TEST_SUB_INVENTORY__   
		m_dwSubItemIDCheckBuffer = OBJECTID_NULL;
	#endif
	
	m_pEffectTarget = NULL;

	
	m_WaitVerify = WAIT_VERIFY_NULL;
	m_WaitVerifyActionInfo = ACTIONINFO_NULL;

	
	m_bKeepTraceCreature = true;

	
	m_bRepeatAction		= FALSE;

	
	m_ConversionDelayTime = 0;

	m_nNoPacketUsedActionInfo = ACTIONINFO_NULL;

	m_bLockMode = false;

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	m_TimeLightSight = 0;
//	m_TimeLightSightX = SECTORPOSITION_NULL;
//	m_TimeLightSightY = SECTORPOSITION_NULL;	
	m_ItemLightSight = 0;
	m_LightSight = 0;

	
	m_Sight = 0;
	m_PetDelayTime = 0;

	m_SweepVice_Value = 0;
	m_TempSelectPosX = SECTORPOSITION_NULL;
	m_TempSelectPosY = SECTORPOSITION_NULL;
	m_SweepViceX = SECTORPOSITION_NULL;
	m_SweepViceY = SECTORPOSITION_NULL;
	m_bShowAdamCloud = false;

	m_ResurrectZoneID = 0;
}

MPlayer::~MPlayer()
{
	
	
	
	
	g_pUserInformation->bCompetence = false;
	g_pUserInformation->bCompetenceShape = false;
	g_pPlayer = NULL;

	// remove priority queue 
	RemoveNodes();

	// remove list
	m_listDirection.clear();
	m_listSendDirection.clear();

	
	if (m_pEffectTarget!=NULL)
	{
		MEffectTarget* pTempEffectTarget = m_pEffectTarget;
		m_pEffectTarget = NULL;

		delete pTempEffectTarget;
	}
}


//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::SetZone(MZone* pZone)
{
	// base class functions
	MCreature::SetZone(pZone);

}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::SetStop()
{ 
	
	
	m_bKnockBack = 0;

	// 2004, 9, 1, sobeit add end

	m_listDirection.clear(); 

	
	m_fTrace		= FLAG_TRACE_NULL;
	//m_fNextTrace	= FLAG_TRACE_NULL;

	
	m_sX=0; 
	m_sY=0;

	int tempAction = m_MoveAction==ACTION_SLAYER_MOTOR_MOVE? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;
	if(IsOusters() && HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
		tempAction = ACTION_OUSTERS_FAST_MOVE_STAND;

	m_Action		= tempAction;
	
	m_ActionCount	= m_ActionCountMax; 
	//m_ActionCount	= (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
	// 2004, 11, 3, sobeit modify end
	m_MoveCount		= m_MoveCountMax; 

	
	m_bNextAction = false;
	m_NextX = SECTORPOSITION_NULL;
	m_NextY = SECTORPOSITION_NULL;
	
	//m_nNextUsedActionInfo = ACTIONINFO_NULL;
	//m_nUsedActionInfo = ACTIONINFO_NULL;

	
	
	m_DestX			= SECTORPOSITION_NULL; 
	m_DestY			= SECTORPOSITION_NULL; 
	m_NextDestX		= SECTORPOSITION_NULL; 
	m_NextDestY		= SECTORPOSITION_NULL; 
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::RemoveNodes()
{
	while (!m_pqDNodes.empty())
	{
		//DNode* pDNode = m_pqDNodes.top();
		m_pqDNodes.pop();
		//delete pDNode;		
	}


	
	DNODE_LIST::iterator iNode = m_listDNodes.begin();

	while (iNode != m_listDNodes.end())
	{
		DNode* pNode = *iNode;
		delete pNode;
		iNode++;		
	}

	m_listDNodes.clear();
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
int
MPlayer::CalculateDistance(int x, int y)
{
	//int xx = x-m_DestX,
	//	yy = y-m_DestY;

	//return abs(xx)+abs(yy);

	//return sqrt(xx*xx + yy*yy);
//	return xx*xx + yy*yy;

//	/*
	int xd = abs(x-m_DestX),
		yd = abs(y-m_DestY);	 

	int plus = !(m_DestX==x) + !(m_DestY==y);
		
	if (xd>yd)
	{
		return xd*xd + plus;
	}
	else return yd*yd + plus;

	return 0;
//	*/
}

//----------------------------------------------------------------------
// Get Destination()
//----------------------------------------------------------------------
void
MPlayer::GetDestination(POINT &dest)
{
	dest.x = m_DestX;
	dest.y = m_DestY;
}

//----------------------------------------------------------------------
// Set Move Position
//----------------------------------------------------------------------
bool	
MPlayer::SetMovePosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	
	if	(
		(
		!m_bAlive
		
		|| m_bInCasket
		
		|| IsUndergroundCreature()
		|| HasEffectStatus(EFFECTSTATUS_SLEEP)
		)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		return false;
	}

	
	//if (m_nUsedActionInfo!=ACTIONINFO_NULL)
	if (m_ActionCount>=m_ActionCountMax)
	{
		m_fTraceBuffer = FLAG_TRACE_NULL;
		m_TraceIDBuffer = OBJECTID_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;

		//if (m_ActionCount==m_ActionCountMax)
		//{			
			m_nUsedActionInfo = ACTIONINFO_NULL;

			/*
			DEBUG_ADD("Clear m_nUsedActionInfo");
		}
		else
		{
			DEBUG_ADD_FORMAT("Not Clear : action=%d, %d/%d", (int)m_Action, (int)m_ActionCount, (int)m_ActionCountMax);
		}
		*/
		TraceNULL();
	}
	
	return SetNextDestination(sX, sY);
}

//----------------------------------------------------------------------
// Set Next Destination
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MPlayer::SetNextDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{ 
	//DEBUG_ADD_FORMAT("[Set] NextDestination (%d, %d) --> (%d, %d)", m_X, m_Y, sX, sY);

	
	if	(
		(!m_bAlive || HasEffectStatus(EFFECTSTATUS_SLEEP))
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		return false;
	}

	
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
	{
		m_fTraceBuffer = FLAG_TRACE_NULL;
		m_TraceIDBuffer = OBJECTID_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;

		if (m_ActionCount>=m_ActionCountMax)
		{			
			m_nUsedActionInfo = ACTIONINFO_NULL;

			DEBUG_ADD("Clear m_nUsedActionInfo");			
		}
		else
		{
			DEBUG_ADD_FORMAT("Not Clear : action=%d, %d/%d", (int)m_Action, (int)m_ActionCount, (int)m_ActionCountMax);			
		}
		
		TraceNULL();
	}
	
	
	
	if (m_fTrace==FLAG_TRACE_NULL && sX==m_X && sY==m_Y)
	{
		return false;
	}	

	
	
	if (sX==SECTORPOSITION_NULL || sY==SECTORPOSITION_NULL
		|| sX==m_BlockDestX && sY==m_BlockDestY)
	{
		return false;
	}

	//---------------------------------------------------
	
	
	
	//---------------------------------------------------
	 

	if(m_fTrace==FLAG_TRACE_NULL)
	{
		BOOL bCanStand = m_pZone->CanMove(m_MoveType, sX,sY);
		if(!bCanStand)
		{
			extern POINT g_DirectionValue[MAX_DIRECTION];
			
			POINT MovePoint;

			if( sY > GetY() )
				MovePoint.y = -1;
			else if( sY < GetY() )
				MovePoint.y = 1;
			else
				MovePoint.y = 0;

			if( sX > GetX() )
				MovePoint.x = -1;
			else if( sX < GetX() )
				MovePoint.x = 1;
			else
				MovePoint.x = 0;

			int limit = 20;	

			while( bCanStand == FALSE && !( GetX() == sX && GetY() == sY ) &&
				sX >= 0 && sY >= 0 
				&& sX < m_pZone->GetWidth() && sY < m_pZone->GetHeight()
				&& limit-- > 0
				)
			{
				if( GetX() != sX )sX += MovePoint.x;
				if( GetY() != sY )sY += MovePoint.y;
				bCanStand = m_pZone->CanMove( m_MoveType, sX,sY );
			};

			if( bCanStand == FALSE || GetX() == sX && GetY() == sY)
				return false;
		}
	}
//	else
	//---------------------------------------------------
	
	
	//
	
	
	//---------------------------------------------------
//	if (m_fTrace==FLAG_TRACE_NULL && !m_pZone->CanMove(m_MoveType, sX, sY))
//	{
//		BYTE direction = MTopView::GetDirectionToPosition(m_X, m_Y, sX, sY);
//
//		sX = m_X;
//		sY = m_Y;
//
//		GetPositionToDirection(sX, sY, direction);
//

//		if (!m_pZone->CanMove(m_MoveType, sX, sY))
//		{
//			return false;
//		}
//	}


	
	m_NextDestX = sX; 
	m_NextDestY = sY; 

	return true;
}

//----------------------------------------------------------------------
// Reset Send Move
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::ResetSendMove()	
{ 
	g_bNetStatusGood=true; 
	m_SendMove = 0; 
	m_listSendDirection.clear(); 
}

//----------------------------------------------------------------------
// Get DeadDelay Last
//----------------------------------------------------------------------

//----------------------------------------------------------------------
DWORD			
MPlayer::GetDeadDelayLast() const
{
	int second = m_DeadDelayTime - g_CurrentTime;

	if (second < 0) 
	{
		return 0;
	}
	
	return second/1000;
}

//----------------------------------------------------------------------
// Get NextDestination()
//----------------------------------------------------------------------
void
MPlayer::GetNextDestination(POINT &dest)
{
	dest.x = m_NextDestX;
	dest.y = m_NextDestY;
}

//----------------------------------------------------------------------
// Get NextPosition()
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
bool
MPlayer::GetNextPosition(POINT &next)
{
	
	if (m_listDirection.empty())
	{
		//next.x = SECTORPOSITION_NULL;
		//next.y = SECTORPOSITION_NULL;
		return false;
	}

	
	

	
	next.x = m_X;
	next.y = m_Y;
	
	switch (m_listDirection.front())
	{
		case DIRECTION_LEFTDOWN		: next.x--;	next.y++;	break;
		case DIRECTION_RIGHTUP		: next.x++;	next.y--;	break;
		case DIRECTION_LEFTUP		: next.x--;	next.y--;	break;
		case DIRECTION_RIGHTDOWN	: next.x++;	next.y++;	break;
		case DIRECTION_LEFT			: next.x--;				break;
		case DIRECTION_DOWN			:			next.y++;	break;
		case DIRECTION_UP			:			next.y--;	break;
		case DIRECTION_RIGHT		: next.x++;				break;
	}

	return true;
}


//----------------------------------------------------------------------
// Set Destination(sX, sY)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
MPlayer::SetDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

	// message : find path
	//DEBUG_ADD_FORMAT("........Find Path........");				

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD_FORMAT("SetDest(%d, %d)", sX, sY);
	#endif

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (!m_bAlive)
	{
		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("dead");
		#endif
		return false;
	}

	//-------------------------------------------------------
	// if not empty, delete all elements
	//-------------------------------------------------------
	if (!m_listDirection.empty())
	{
		m_listDirection.clear();
	}

		
	bool bHasRelic = 
		( m_bEffectStatus[EFFECTSTATUS_HAS_VAMPIRE_RELIC] || m_bEffectStatus[EFFECTSTATUS_HAS_SLAYER_RELIC] ) && 
		(g_pZone->GetSector(sX, sY).IsSafeSlayer() && IsSlayer() ||
		g_pZone->GetSector(sX, sY).IsSafeVampire() && IsVampire() ||
		g_pZone->GetSector(sX, sY).IsSafeOusters() && IsOusters()
		);

	bool bHasBloodBible =
	(
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_ARMEGA] || m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_MIHOLE] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_KIRO] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_INI] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_GREGORI] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_CONCILIA] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_LEGIOS] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_HILLEL] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_JAVE] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_NEMA] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_AROSA] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_CHASPA] ) && 
		(g_pZone->GetSector(sX, sY).IsSafeSlayer() && IsSlayer() ||
		g_pZone->GetSector(sX, sY).IsSafeOusters() && IsOusters() ||
		g_pZone->GetSector(sX, sY).IsSafeVampire() && IsVampire()
		);

	if(bHasBloodBible)
	{
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_MOVE_SAFETY_ZONE_BLOOD_BIBLE].GetString());
	}

	if(bHasRelic)
	{
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_MOVE_SAFETY_ZONE_RELIC].GetString());
	}

	if( HasEffectStatus( EFFECTSTATUS_HAS_FLAG ) &&
		(g_pZone->GetSector(sX, sY).IsSafeSlayer() && IsSlayer() ||
		g_pZone->GetSector(sX, sY).IsSafeVampire() && IsVampire() ||
		g_pZone->GetSector(sX, sY).IsSafeOusters() && IsOusters()
		) )
	{
		bHasRelic = true;
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_MOVE_SAFETY_ZONE_FLAG].GetString() );
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (sX<0 || sY<0 
		|| sX>=m_pZone->GetWidth() || sY>=m_pZone->GetHeight()
		
		|| g_pZone->GetSector(m_X, m_Y).HasSanctuary()
		|| bHasRelic
		|| bHasBloodBible
		)
	{
		
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;
		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("outOfZone or Sanctuary");
		#endif

		return false;

		
		 
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (m_fTrace!=FLAG_TRACE_NULL
		&& max(abs(m_TraceX-m_X), abs(m_TraceY-m_Y))<=m_TraceDistance)
	{
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;

		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;
	
		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("already in dist");
		#endif

		return false;
	}

	/*
	// priority queue test code
	ofstream file("test.txt", ios::app);
	CString str;

	m_pqDNodes.push( new DNode(0,0,0,1) );
	m_pqDNodes.push( new DNode(0,0,0,4) );
	m_pqDNodes.push( new DNode(0,0,0,3) );
	m_pqDNodes.push( new DNode(0,0,0,8) );
	m_pqDNodes.push( new DNode(0,0,0,2) );
	m_pqDNodes.push( new DNode(0,0,0,4) );
	m_pqDNodes.push( new DNode(0,0,0,7) );

	while (!m_pqDNodes.empty())
	{
		DNode*	pDNode = m_pqDNodes.top();
		m_pqDNodes.pop();

		str.Format("Pop : (%d,%d) , [%d], dist=%d", pDNode->x, pDNode->y, pDNode->direction, pDNode->distance);
		
		file << str << endl;
	}

	return;
	*/

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	// Best First Search
	//--------------------------------------------------------------
	BOOL	bCanStand = m_pZone->CanMove(m_MoveType, sX,sY);

#ifdef __EXPO_CLIENT__
	bCanStand |= (BOOL)g_UserInformation.Invisible;
#endif

	//--------------------------------------------------------------
	
	
	//--------------------------------------------------------------
	if (bCanStand || m_fTrace!=FLAG_TRACE_NULL) 
	{		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		m_DestX = sX;
		m_DestY = sY;

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		RemoveNodes();	

		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		POINT firstSector = g_pTopView->GetFirstSector();
		//int x0 = m_DestX - g_pClientConfig->MAX_FINDPATH_WIDTH, 
		//	x1 = m_DestX + g_pClientConfig->MAX_FINDPATH_WIDTH+1, 
		//	y0 = m_DestY - g_pClientConfig->MAX_FINDPATH_HEIGHT,
		//	y1 = m_DestY + g_pClientConfig->MAX_FINDPATH_HEIGHT+1;
		int x0 = firstSector.x - 1, 
			x1 = firstSector.x + g_SECTOR_WIDTH + 5, 
			y0 = firstSector.y - 1,
			y1 = firstSector.y + g_SECTOR_HEIGHT + 5;

		if (x0 < 0)						
			x0 = 0;
		if (x1 > g_pZone->GetWidth())	
			x1 = g_pZone->GetWidth();
		
		if (y0 < 0)				
			y0 = 0;
		if (y1 > g_pZone->GetHeight())	
			y1 = g_pZone->GetHeight();
		
		int size = x1-x0;	
		for (int i=y0; i<y1; i++)
		{			
			for (int j=x0; j<x1; j++)
			{
				//m_ppVisited[i][j] = false;
				g_pZone->UnSetVisitedFlag(j, i);
			}
			//memset( m_ppVisited[i]+x0, false, size );
		}
			

		//-------------------------------------------------------
		//
		
		//
		//-------------------------------------------------------

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		static BYTE FirstDirection = GetDirectionToPosition(m_DestX, m_DestY);
		static TYPE_SECTORPOSITION previousDestX = SECTORPOSITION_NULL;
		static TYPE_SECTORPOSITION previousDestY = SECTORPOSITION_NULL;

		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		if (previousDestX!=m_DestX || previousDestY!=m_DestY)
		{
			FirstDirection = GetDirectionToPosition(m_DestX, m_DestY);
			previousDestX = m_DestX;
			previousDestY = m_DestY;
		}
		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		else
		{
			FirstDirection = m_Direction;
		}
		
		DNode*	currentDNode = new DNode(
									m_X,
									m_Y,
									FirstDirection,
									CalculateDistance(m_X, m_Y),
									0
									);
	
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		m_listDNodes.push_back( currentDNode );

		currentDNode->SetParent(NULL);
		m_pqDNodes.push(currentDNode);	
		
		POINT	next;
		int		dist;
		bool	bFound = false;		

		while (!bFound && !m_pqDNodes.empty())
		{
			//-------------------------------------------------------
			
			
			//-------------------------------------------------------
			DNode* pDNode = m_pqDNodes.top();
			m_pqDNodes.pop();

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (pDNode->distance==0) 
			{
				m_pqDNodes.push(pDNode);
				bFound = true;
				break;
			}
			
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (m_fTrace!=FLAG_TRACE_NULL && 
				max(abs(pDNode->x-m_DestX), abs(pDNode->y-m_DestY))<=m_TraceDistance)
			{
				m_pqDNodes.push(pDNode);
				bFound = true;
				break;
			}			

			int Direction = pDNode->direction;

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			for (int i=0; i<8; Direction++, i++)
			{		
				if (Direction==8) Direction=0;

				next.x = pDNode->x;
				next.y = pDNode->y;

				switch (Direction)
				{
					case DIRECTION_LEFTDOWN		: next.x--;	next.y++;	break;
					case DIRECTION_RIGHTUP		: next.x++;	next.y--;	break;
					case DIRECTION_LEFTUP		: next.x--;	next.y--;	break;
					case DIRECTION_RIGHTDOWN	: next.x++;	next.y++;	break;
					case DIRECTION_LEFT			: next.x--;				break;
					case DIRECTION_DOWN			: next.y++;				break;
					case DIRECTION_UP			: next.y--;				break;
					case DIRECTION_RIGHT		: next.x++;				break;
				}

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (next.x<0 || next.y<0 
					|| next.x>=m_pZone->GetWidth() || next.y>=m_pZone->GetHeight()) continue;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (g_pZone->IsVisitedFlag(next.x, next.y)) continue;

				
				if (next.x<x0 || next.y<y0 
					|| next.x>=x1 || next.y>=y1) continue;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (m_pZone->CanMove(m_MoveType, next.x, next.y)
					&& !bHasRelic && !bHasBloodBible

#ifdef __EXPO_CLIENT__
	|| g_UserInformation.Invisible
#endif
					
					)
				{		
					g_pZone->SetVisitedFlag(next.x, next.y);
//					m_ppVisited[next.y][next.x] = true;

					//-------------------------------------------------------
					
					//-------------------------------------------------------
					
					dist = CalculateDistance(next.x, next.y);

					
					DNode*	nextDNode = new DNode(
											next.x, 
											next.y, 
											Direction,
											dist,
											pDNode->step + 1);
					
				
					//-------------------------------------------------------
					
					//-------------------------------------------------------
					m_listDNodes.push_back( nextDNode );
					
					//CString str;
					//str.Format("NewNode : (%d,%d)-->(%d,%d) , [%d], dist=%d", next.x, next.y, m_Dest.x, m_Dest.y, i, dist);

					nextDNode->SetParent(pDNode);
					m_pqDNodes.push(nextDNode);					
				}

			
			}
		}

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (bFound)
		{				
			//-------------------------------------------------------
			
			//-------------------------------------------------------

			
			DNode* currentNode = m_pqDNodes.top();
			m_pqDNodes.pop();
			
			while (currentNode->pParent!=NULL)
			{
				m_listDirection.insert( m_listDirection.begin(), currentNode->direction);
				currentNode = currentNode->pParent;			
			}

			RemoveNodes();

			m_BlockDestX = SECTORPOSITION_NULL;
			m_BlockDestY = SECTORPOSITION_NULL;
		}
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		else
		{
			
			if (m_fTrace!=FLAG_TRACE_NULL)				
			{
				m_MoveCount = m_MoveCountMax;

				TraceNULL();

				
				m_nUsedActionInfo = ACTIONINFO_NULL;
				m_nNextUsedActionInfo = ACTIONINFO_NULL;
			}

			m_BlockDestX = m_DestX;
			m_BlockDestY = m_DestY;

			m_DestX = SECTORPOSITION_NULL;
			m_DestY = SECTORPOSITION_NULL;			
		}
	}
	
	else 
	{
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;

		
		//m_BlockDestX = m_DestX;
		//m_BlockDestY = m_DestY;

		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;		

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("no way!");
		#endif

		return false;
	}

	 

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("find way");
	#endif

	return true;
}

//----------------------------------------------------------------------
// Self Special Action
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MPlayer::SelfSpecialAction()
{
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay() || HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE ) )
	{
		return false;
	}

	
	//if (IsInCasket())
//	{
//		m_nSpecialActionInfo = MAGIC_OPEN_CASKET;
//	}

	
	if (m_nSpecialActionInfo == ACTIONINFO_NULL
		
		|| m_MoveDevice == MOVE_DEVICE_RIDE
		
		
		|| IsInSafeSector() == 2
		
		)
		return false;

	
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetSelf())
	{	
		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		// skillID            itemType
		// MINE_ANKLE_KILLER,	// 5
		// MINE_POMZ,			// 6
		// MINE_AP_C1,			// 7
		// MINE_DIAMONDBACK,	// 8
		// MINE_SWIFT_EX		// 9
		//
		
		//			
		if (m_nSpecialActionInfo==SKILL_INSTALL_MINE
			|| m_nSpecialActionInfo>=MINE_ANKLE_KILLER 
				&& m_nSpecialActionInfo<=MINE_SWIFT_EX)
		{
			int					useSkill = SKILL_INSTALL_MINE;					

			//-------------------------------------------------------
			
			
			//-------------------------------------------------------
			if (g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)useSkill )
				&& (*g_pSkillInfoTable)[useSkill].IsEnable()
				&& (*g_pSkillInfoTable)[useSkill].IsAvailableTime()
				&& !(*g_pSkillInfoTable)[useSkill].IsPassive()
				&& IsSlayer()
				&& !IsWaitVerify()
				&& IsItemCheckBufferNULL())
			{					

				ITEM_CLASS	itemClass = ITEM_CLASS_MINE;

				MItem* pItem = NULL;

				if (m_nSpecialActionInfo!=SKILL_INSTALL_MINE)
				{
					
					int itemType = m_nSpecialActionInfo-MINE_ANKLE_KILLER;

					MItemClassTypeFinder itemFinder(itemClass, itemType);
					pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );
				}
				else
				{
					
					MItemClassFinder itemFinder(itemClass);
					pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );
				}
				
				
				if (pItem!=NULL)
				{
					
					gC_vs_ui.StartInstallMineProgress( pItem->GetGridX(), pItem->GetGridY() );
				}									
			}

			
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			return false;	
		}

		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		if (!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo )
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}

		if (IsStop()) // || m_Action!=ACTION_ATTACK)
		{
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			switch (m_nSpecialActionInfo)
			{
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				case MAGIC_BLOODY_MARK :
					if (IsVampire() && g_bHolyLand == false && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
						//----------------------------------------------------
						
						//----------------------------------------------------
							//----------------------------------------------------
							
							//----------------------------------------------------
							if (!IsWaitVerify()
								&& IsItemCheckBufferNULL())
							{									
								

							#ifdef __TEST_SUB_INVENTORY__   
								MItem* pSubInventory = NULL;
								MVampirePortalItemFinder finder1( false );
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItemAll( finder1 , pSubInventory );
							#else
								MVampirePortalItemFinder finder1( false );
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( finder1 );
							#endif
								
								
								
								if (pItem!=NULL && pItem->GetNumber()>0 && pItem->IsAffectStatus())
								{	
									#ifdef __TEST_SUB_INVENTORY__   
										DWORD SubInventoryItemID = 0;
									#endif
									//----------------------------------------------------
									
									//----------------------------------------------------
										CGSkillToInventory _CGSkillToInventory;
										_CGSkillToInventory.setObjectID( pItem->GetID() );
										_CGSkillToInventory.setX( pItem->GetGridX() );
										_CGSkillToInventory.setY( pItem->GetGridY() );
										_CGSkillToInventory.setSkillType( MAGIC_BLOODY_MARK );
										//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							
									#ifdef __TEST_SUB_INVENTORY__   
										if(NULL != pSubInventory)
										{
											SubInventoryItemID = pSubInventory->GetID();
										}
										_CGSkillToInventory.setInventoryItemObjectID( SubInventoryItemID );
									#endif
										g_pSocket->sendPacket( &_CGSkillToInventory );

										//----------------------------------------------------
										
										//----------------------------------------------------
										SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS );

									#ifdef __TEST_SUB_INVENTORY__   
										SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY, SubInventoryItemID);
									#else
										SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);
									#endif


										(*g_pSkillInfoTable)[m_nSpecialActionInfo].SetNextAvailableTime();
								}
							}
					}						
					
					return false;
				break;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				case MAGIC_BLOODY_TUNNEL :					
					if (IsVampire() && g_bHolyLand == false && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
							//----------------------------------------------------
							
							//----------------------------------------------------
							if (!IsWaitVerify()
								&& IsItemCheckBufferNULL())
							{		
								
							#ifdef __TEST_SUB_INVENTORY__   
								MItem* pSubInventory = NULL;
								MVampirePortalItemFinder finder2( true );
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItemAll( finder2 , pSubInventory );
							#else
								MVampirePortalItemFinder finder2( true );
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( finder2 );
							#endif
								
								
								if (pItem!=NULL && pItem->GetNumber()>0 && pItem->IsAffectStatus())
								{				
									#ifdef __TEST_SUB_INVENTORY__   
										DWORD SubInventoryItemID = 0;
									#endif
										CGSkillToInventory _CGSkillToInventory;
										_CGSkillToInventory.setObjectID( pItem->GetID() );
										_CGSkillToInventory.setX( pItem->GetGridX() );
										_CGSkillToInventory.setY( pItem->GetGridY() );
										_CGSkillToInventory.setSkillType( MAGIC_BLOODY_TUNNEL );
										//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							
									#ifdef __TEST_SUB_INVENTORY__   
										if(NULL != pSubInventory)
										{
											SubInventoryItemID = pSubInventory->GetID();
										}
										_CGSkillToInventory.setInventoryItemObjectID( SubInventoryItemID );
									#endif
										g_pSocket->sendPacket( &_CGSkillToInventory );

										//----------------------------------------------------
										
										//----------------------------------------------------
										SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );

									#ifdef __TEST_SUB_INVENTORY__   
										SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY, SubInventoryItemID);
									#else
										SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);
									#endif
										

										(*g_pSkillInfoTable)[m_nSpecialActionInfo].SetNextAvailableTime();

								}
							}
					}						
					
					return false;
				break;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				case SUMMON_HELICOPTER :
					if (IsSlayer() && g_bHolyLand == false && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
							//----------------------------------------------------
							
							//----------------------------------------------------
							if (IsWaitVerifyNULL()
								&& IsItemCheckBufferNULL()
								&& g_pZone->GetHelicopter( GetID() )==NULL)				
							{		

							#ifdef __TEST_SUB_INVENTORY__   
								MItem* pSubInventory = NULL;
								MSlayerPortalItemFinder finder3;
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItemAll( finder3 , pSubInventory);
							#else
								MSlayerPortalItemFinder finder3;
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( finder3 );
							#endif
								
								
								if (pItem!=NULL && pItem->GetNumber()>0 && pItem->IsAffectStatus())
								{				
									#ifdef __TEST_SUB_INVENTORY__   
										DWORD SubInventoryItemID = 0;
									#endif
										CGUseItemFromInventory _CGUseItemFromInventory;
										_CGUseItemFromInventory.setObjectID( pItem->GetID() );
										_CGUseItemFromInventory.setX( pItem->GetGridX() );
										_CGUseItemFromInventory.setY( pItem->GetGridY() );

									#ifdef __TEST_SUB_INVENTORY__   
										if(NULL != pSubInventory)
										{
											SubInventoryItemID = pSubInventory->GetID();
										}
										_CGUseItemFromInventory.setInventoryItemObjectID( SubInventoryItemID );
									#endif

										g_pSocket->sendPacket( &_CGUseItemFromInventory );

										//----------------------------------------------------
										
										//----------------------------------------------------
									#ifdef __TEST_SUB_INVENTORY__   
										SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, SubInventoryItemID);
									#else
										SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
									#endif
										
								}
							}
					}						
					
					return false;
				break;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				case MAGIC_CREATE_HOLY_WATER :				
					if (
						IsSlayer()
						&& !IsWaitVerify()
						&& IsItemCheckBufferNULL())
					{					
						MItem* pItem = NULL;

						for( int i = 0; i <= 2; i++ )
						{
							pItem = g_pInventory->FindItem( ITEM_CLASS_WATER,  i);
							if( pItem != NULL )
								break;
						}

						if (pItem!=NULL)
						{


//							if (GetMakeItemFitPosition(pItem, 
//														ITEM_CLASS_HOLYWATER, 
//														pItem->GetItemType(), 
//														fitPoint))
							{
								CGSkillToInventory _CGSkillToInventory;
								_CGSkillToInventory.setObjectID( pItem->GetID() );
								_CGSkillToInventory.setX( pItem->GetGridX() );
								_CGSkillToInventory.setY( pItem->GetGridY() );
								_CGSkillToInventory.setTargetX( pItem->GetGridX() );
								_CGSkillToInventory.setTargetY( pItem->GetGridY() );
								_CGSkillToInventory.setSkillType( MAGIC_CREATE_HOLY_WATER );
								//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

								g_pSocket->sendPacket( &_CGSkillToInventory );

								
								
								//(*g_pInventory).RemoveItem( pItem->GetID() );

								//----------------------------------------------------
								
								//----------------------------------------------------
								SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY );

								(*g_pSkillInfoTable)[MAGIC_CREATE_HOLY_WATER].SetNextAvailableTime();

								//----------------------------------------------------
								
								//----------------------------------------------------
								AddNewInventoryEffect( pItem->GetID(),
													MAGIC_CREATE_HOLY_WATER, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
													g_pClientConfig->FPS*3	
												);
								
								m_fNextTrace = FLAG_TRACE_NULL;
								UnSetRepeatAction();
							}
//							else
//							{

//							}
						}						
					}
					
					return false;					
				break;
			case SKILL_CREATE_HOLY_POTION :				
					if (
						IsSlayer()
						&& !IsWaitVerify()
						&& IsItemCheckBufferNULL())
					{					
						
						MItem* pItem = NULL;

						for( int i = 3; i <= 6; i++ )
						{
							pItem = g_pInventory->FindItem( ITEM_CLASS_WATER,  i);
							if( pItem != NULL )
								break;
						}

						if (pItem!=NULL)
						{
							POINT fitPoint;			

							if (GetMakeItemFitPosition(pItem, 
														ITEM_CLASS_POTION, 
														(*g_pItemTable)[pItem->GetItemClass()][pItem->GetItemType()].Value3, 
														fitPoint))
							{
								CGSkillToInventory _CGSkillToInventory;
								_CGSkillToInventory.setObjectID( pItem->GetID() );
								_CGSkillToInventory.setX( pItem->GetGridX() );
								_CGSkillToInventory.setY( pItem->GetGridY() );
								_CGSkillToInventory.setTargetX( fitPoint.x );
								_CGSkillToInventory.setTargetY( fitPoint.y );
								_CGSkillToInventory.setSkillType( SKILL_CREATE_HOLY_POTION );
								//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

								g_pSocket->sendPacket( &_CGSkillToInventory );

								
								
								//(*g_pInventory).RemoveItem( pItem->GetID() );

								//----------------------------------------------------
								
								//----------------------------------------------------
								SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY );

								(*g_pSkillInfoTable)[SKILL_CREATE_HOLY_POTION].SetNextAvailableTime();

								//----------------------------------------------------
								
								//----------------------------------------------------
								AddNewInventoryEffect( pItem->GetID(),
													SKILL_CREATE_HOLY_POTION, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
													g_pClientConfig->FPS*3	
												);
								
								m_fNextTrace = FLAG_TRACE_NULL;
								UnSetRepeatAction();
							}
							else
							{
								
							}
						}						
					}
					
					return false;					
				break;

				//-------------------------------------------------------
				
				
				//-------------------------------------------------------
				case SKILL_MAKE_BOMB :
				case SKILL_MAKE_MINE :
					if (IsSlayer()
						&& !IsWaitVerify()
						&& IsItemCheckBufferNULL()
						&& !gC_vs_ui.IsInstallMineProgress())	
						
						
					{					

						int					skillID;
						ITEM_CLASS	itemClass;
						int					minItemType;
						int					maxItemType;
						int					itemTypeModifier;
							
						
						if (m_nSpecialActionInfo==SKILL_MAKE_BOMB)
						{
							skillID = SKILL_MAKE_BOMB;
							itemClass = ITEM_CLASS_BOMB;
							minItemType = 0;
							maxItemType = 4;
							itemTypeModifier = 0;
						}
						else
						{
							skillID = SKILL_MAKE_MINE;
							itemClass = ITEM_CLASS_MINE;
							minItemType = 5;
							maxItemType = 9;
							itemTypeModifier = -5;
						}

						MItemClassRangeTypeFinder itemFinder(ITEM_CLASS_BOMB_MATERIAL,
																minItemType, maxItemType);

						MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );

						if (pItem!=NULL)
						{
							POINT fitPoint;			

							if (GetMakeItemFitPosition(pItem, 
														itemClass, 
														pItem->GetItemType() + itemTypeModifier, 
														fitPoint))
							{
								
								if (skillID==SKILL_MAKE_BOMB)
								{
									gC_vs_ui.StartCreateBombProgress( pItem->GetGridX(), pItem->GetGridY() );
								}
								else
								{
									gC_vs_ui.StartCreateMineProgress( pItem->GetGridX(), pItem->GetGridY() );
								}

								
								
								
								
//								m_fNextTrace	= FLAG_TRACE_SELF;
//				
//								m_bRepeatAction = FALSE;
								
								
								m_fNextTrace = FLAG_TRACE_NULL;
								UnSetRepeatAction();
								
								 
							}
							else
							{
								
							}
						}						
					}
					
					return false;					
				break;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				case SKILL_TRANSFORM_TO_WERWOLF :
				case MAGIC_TRANSFORM_TO_WOLF :
				case MAGIC_TRANSFORM_TO_BAT :					
					//----------------------------------------------------
					//
					
					//
					//----------------------------------------------------
					if (IsVampire() && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
							//----------------------------------------------------
							
							//----------------------------------------------------
							if (!IsWaitVerify()
								&& IsItemCheckBufferNULL())
							{		
								MItem* pItem = NULL;

							#ifdef __TEST_SUB_INVENTORY__   
								MItem* pSubInventory = NULL;
							#endif

								if (m_nSpecialActionInfo==MAGIC_TRANSFORM_TO_WOLF)
								{
									
									pItem = g_pInventory->FindItem(ITEM_CLASS_VAMPIRE_ETC, 0);
								}
								else if ( m_nSpecialActionInfo == SKILL_TRANSFORM_TO_WERWOLF )
								{
									pItem = g_pInventory->FindItem(ITEM_CLASS_SKULL, 39);
								}
								else
								{
								#ifdef __TEST_SUB_INVENTORY__   
									pItem = g_pInventory->FindItemAll( MItemClassTypeFinder(ITEM_CLASS_VAMPIRE_ETC , 1), pSubInventory );
								#else
									pItem = g_pInventory->FindItem(ITEM_CLASS_VAMPIRE_ETC, 1);
								#endif
									
								}

								if (pItem!=NULL)
								{			
								#ifdef __TEST_SUB_INVENTORY__   
									DWORD dwSubInventoryID = 0;
								#endif
									CGSkillToInventory _CGSkillToInventory;
									_CGSkillToInventory.setObjectID( pItem->GetID() );
									_CGSkillToInventory.setX( pItem->GetGridX() );
									_CGSkillToInventory.setY( pItem->GetGridY() );
									_CGSkillToInventory.setSkillType( m_nSpecialActionInfo );
									//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;	
									
								#ifdef __TEST_SUB_INVENTORY__   
									if(NULL != pSubInventory)
										dwSubInventoryID = pSubInventory->GetID();

									_CGSkillToInventory.setInventoryItemObjectID(dwSubInventoryID);
								#endif

									g_pSocket->sendPacket( &_CGSkillToInventory );


									
									//(*g_pInventory).RemoveItem( pItem->GetID() );

									//----------------------------------------------------
									
									//----------------------------------------------------
									SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
									
								#ifdef __TEST_SUB_INVENTORY__   
									SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY , dwSubInventoryID);
								#else
									SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY );
								#endif
									

									(*g_pSkillInfoTable)[m_nSpecialActionInfo].SetNextAvailableTime();

									//----------------------------------------------------
									
									//----------------------------------------------------
									AddNewInventoryEffect( pItem->GetID(),
														m_nSpecialActionInfo, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
														g_pClientConfig->FPS*3	
													);
								}
							}
					}						
					
					return false;
				break;

				//----------------------------------------------------
				//
				
				// 
				//----------------------------------------------------
				case MAGIC_UN_TRANSFORM :
					if (IsVampire())
					{
						const MSector& sector = m_pZone->GetSector(m_X, m_Y);
						
						if (sector.IsBlockGround())
						{
							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

							return false;
						}						
					}
				break;
				
				case SKILL_SUMMON_SYLPH :
					if( IsOusters() && !HasEffectStatus( EFFECTSTATUS_HAS_FLAG )
						//----------------------------------------------------
						
						//----------------------------------------------------
						&& IsWaitVerifyNULL()
						&& IsItemCheckBufferNULL()
						)
					{
						//MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_OUSTERS_SUMMON_ITEM );

					#ifdef __TEST_SUB_INVENTORY__   
						MItem* pSubInventory = NULL;
						MOustersSummonGemItemFinder finder4;
						MItem* pItem = ((MItemManager*)g_pInventory)->FindItemAll( finder4, pSubInventory );
					#else
						MOustersSummonGemItemFinder finder4;
						MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( finder4 );
					#endif
						
						if( pItem != NULL )
						{
							if( pItem->IsAffectStatus() && pItem->IsChargeItem() && pItem->GetNumber() > 0 )
							{
								
							#ifdef __TEST_SUB_INVENTORY__   
								DWORD SubInventoryItemID = 0;
							#endif

								CGUseItemFromInventory _CGUseItemFromInventory;
								_CGUseItemFromInventory.setObjectID( pItem->GetID() );
								_CGUseItemFromInventory.setX( pItem->GetGridX() );
								_CGUseItemFromInventory.setY( pItem->GetGridY() );

							#ifdef __TEST_SUB_INVENTORY__   
								if(NULL != pSubInventory)
								{
									SubInventoryItemID = pSubInventory->GetID();
								}

								_CGUseItemFromInventory.setInventoryItemObjectID( SubInventoryItemID );
							#endif

								g_pSocket->sendPacket( &_CGUseItemFromInventory );

							#ifdef __TEST_SUB_INVENTORY__   
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, SubInventoryItemID);
							#else
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
							#endif

								(*g_pSkillInfoTable)[SKILL_SUMMON_SYLPH].SetAvailableTime( 4000 );
							}
						} else
						{
							
						}
					}
					return false;
				break;
				//-----------------------------------------------------
				//
				
				//
				//-----------------------------------------------------
				case SKILL_SOUL_CHAIN :
					

					if (!IsWaitVerify())
					{
						
						
						gC_vs_ui.RunTraceWindow();
						
//						int skillID = SKILL_SOUL_CHAIN;
//						
//						if (skillID < MIN_RESULT_ACTIONINFO)
//						{
//							(*g_pSkillInfoTable)[skillID].SetAvailableTime();
//						}
						
					}
					return false;
					
					break;

				case SKILL_LOVE_CHAIN :
					//----------------------------------------------------
					
					//----------------------------------------------------
					if (!IsWaitVerify()
						&& IsItemCheckBufferNULL())
					{	
						ITEM_CLASS ItemClass;
						switch( GetRace() )
						{
						case RACE_SLAYER :
							ItemClass = ITEM_CLASS_COUPLE_RING;
							break;
						case RACE_VAMPIRE :
							ItemClass = ITEM_CLASS_VAMPIRE_COUPLE_RING;
							break;
						default :
							ItemClass = ITEM_CLASS_COUPLE_RING;
							break;
						}
						MPlayerGear* pGear = GetGear();
						MItemClassFinder itemFinder( ItemClass );
						
						MItem *pItem = pGear->FindItem( itemFinder );
						if(pItem == NULL || !pItem->IsAffectStatus())
							return false;

						CGUseItemFromGear _CGUseItemFromGear;

						_CGUseItemFromGear.setObjectID ( pItem->GetID() );
						_CGUseItemFromGear.setPart ( pItem->GetItemSlot() );
						
						g_pSocket->sendPacket( &_CGUseItemFromGear );

						SetItemCheckBuffer( pItem, ITEM_CHECK_BUFFER_USE_FROM_GEAR );
						return false;
					}
					break;
				
// 				case SKILL_BLLODY_SCARIFY:
// 				case SKILL_BLOOD_CURSE:
// 					if (IsVampire() && !IsWaitVerify() && IsItemCheckBufferNULL())
// 					{
// 						MItem* pItem = NULL;
// 
// 					}
// 					return false;
// 					break;
// 				case SKILL_SHINE_SWORD:
// 				case SKILL_BOMB_CRASH_WALK:
// 				case SKILL_SATELLITE_BOMB:
// 				case SKILL_ILLUSION_INVERSION:
// 				case SKILL_HEAVEN_GROUND:
// 				case SKILL_DUMMY_DRAKE:
// 				case SKILL_HYDRO_CONVERGENCE:
// 				case SKILL_SUMMON_CLAY:
// 				case SKILL_HETER_CHAKRAM:
// 					break;
				// end add By Coffee 2007-6-9
			}			

			if( (*g_pActionInfoTable)[m_nSpecialActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
				m_nSpecialActionInfo = (*g_pActionInfoTable)[m_nSpecialActionInfo].GetParentActionInfo();

			
			m_nUsedActionInfo	= m_nSpecialActionInfo;

			
			SetTraceID( m_ID );
			m_fTrace	= FLAG_TRACE_SELF;
			m_TraceX	= m_X;//SECTORPOSITION_NULL;
			m_TraceY	= m_Y;//SECTORPOSITION_NULL;
			m_TraceZ	= 0;

			
			m_fTraceBuffer	= m_fTrace;	

			//------------------------------------------------------------
			
			
			//------------------------------------------------------------
			
			m_TraceDistance		= 0;
			
			//------------------------------------------------------------
			
			SetNextAction( GetActionInfoAction(m_nUsedActionInfo, true) );

			
			m_fNextTrace			= FLAG_TRACE_NULL;	

			//------------------------------------------------------------
			
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Self SpecialAction");					
		}
		else
		{
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				m_fNextTrace	= FLAG_TRACE_SELF;
				
				m_bRepeatAction = FALSE;

				return false;
			}
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Keep Trace Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::KeepTraceCreature()
{
	if (IsInDarkness())
	{
		TraceNULL();			
		SetStop();			
		UnSetRepeatAction();
			
		return;			
	}

	
	MCreature*	pCreature = m_pZone->GetCreature( m_TraceID );

	//GET_DYING_CREATURE( pCreature, m_TraceID );

	bool	bTraceTimer = false;

	if( m_TraceTimer != 0 )
	{
		if( ( GetTickCount() - m_TraceTimer ) / 1000 > g_pClientConfig->TRACE_CHARACTER_LIMIT_TIME )
		{
			if( pCreature != NULL &&					
				(pCreature->IsSlayer() && IsSlayer()) || 
				(pCreature->IsVampire() && IsVampire()) ||
				(pCreature->IsOusters() && IsOusters())
				)
			{
				bTraceTimer = true;
				m_TraceTimer = 0;
			}			
		}
	}
	
	//-------------------------------------------------------
	
	
	//-------------------------------------------------------
	if ((pCreature==NULL || 
		pCreature->IsInDarkness() && !pCreature->IsNPC() && 
		(!IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS) ||
		IsVampire() && g_pZone->GetID() == 3001)
		||bTraceTimer
		|| pCreature->IsInGroundElemental() && pCreature->IsOusters() && !g_pPlayer->IsOusters())
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		
		TraceNULL();
			
		
		SetStop();	
		
		UnSetRepeatAction();
		m_TraceTimer = 0;
			
		return;			
	}			
	
	if (max(abs(pCreature->GetX()-m_X), 
			abs(pCreature->GetY()-m_Y)) > 1)
	{		
		TraceCreatureToBasicAction( m_TraceID, false );	
		
		g_pTopView->SetSelectedSectorNULL();
	}	
}


//----------------------------------------------------------------------
// Get ActionInfo Range
//----------------------------------------------------------------------

//----------------------------------------------------------------------
int		
MPlayer::GetActionInfoRange(TYPE_ACTIONINFO nActionInfo)
{
	if( nActionInfo == SKILL_HEAD_SHOT )
		return 3;
	
	if( (*g_pActionInfoTable)[nActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
		nActionInfo = (*g_pActionInfoTable)[nActionInfo].GetParentActionInfo();

	//-------------------------------------------------------------
	
	//-------------------------------------------------------------
	if ((*g_pActionInfoTable)[nActionInfo].IsAffectCurrentWeaponRange())
	{
		return m_BasicAttackDistance;
		//nActionInfo = m_nBasicActionInfo;
	}
	
	SKILLINFO_NODE& skillInfo = (*g_pSkillInfoTable)[nActionInfo];	

	int minRange = skillInfo.minRange;
	int maxRange = skillInfo.maxRange;
	int skillLevel = skillInfo.GetExpLevel();
	
#ifdef __METROTECH_TEST__
	if(g_bLight)
		return maxRange;
#endif

	int maxLevel = 100;	
	
	if(IsOusters())
		maxLevel = 30;	

	
	
	switch( nActionInfo )
	{
	case MAGIC_RAPID_GLIDING :
		return ( min(6, (2+ (GetDEX() / 50)) ) );
		break;
	
	case MAGIC_BLOODY_ZENITH :
		return min(6, 2 + GetSTR()/80 + GetDEX()/120 + GetINT()/240 );
		break;	
	
	case SKILL_SOUL_REBIRTH:
		return min(5, 2+(*g_pSkillInfoTable)[SKILL_SOUL_REBIRTH_MASTERY].GetExpLevel()/10);
		break;
	}
	
	return (int)(minRange + (maxRange - minRange) * skillLevel/maxLevel);

}

//----------------------------------------------------------------------
// Trace Creature To BasicAction
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
MPlayer::TraceCreatureToBasicAction(TYPE_OBJECTID id, bool bForceAttack, bool bClick)
{
//	if(bForceAttack && !g_pSDLInput->KeyDown(DIK_LSHIFT))
//	{
//		bForceAttack = false;
//	}
	
	if((HasEffectStatus(EFFECTSTATUS_SLEEP) || HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| IsInCasket()	
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInDarkness() && 
		!HasEffectStatus( EFFECTSTATUS_LIGHTNESS )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		 )
	{
		return false;
	}

	if (m_nBasicActionInfo == ACTIONINFO_NULL)
		return false;

	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{		
		
		MCreature*	pCreature = m_pZone->GetCreature(id);
		if(pCreature == NULL)
		{
			MFakeCreature *pFakeCreature = (MFakeCreature *)m_pZone->GetFakeCreature(id);
			if(pFakeCreature != NULL && pFakeCreature->GetOwnerID() != OBJECTID_NULL)	
			{
				MPetItem *pPetItem = pFakeCreature->GetPetItem();
				if(pPetItem != NULL)
				{
					PETINFO petInfo;
					petInfo.HP = pPetItem->GetCurrentDurability();
					petInfo.ATTR = pPetItem->GetEnchantLevel();
					petInfo.ATTR_VALUE = pPetItem->GetSilver();
					petInfo.LEVEL = pPetItem->GetNumber();
					petInfo.FOODTYPE = pPetItem->GetPetFoodType();
					petInfo.EXP_REMAIN = pPetItem->GetPetExpRemain();
					petInfo.CUT_HEAD = pPetItem->IsCanCutHead();
					petInfo.GAMBLE = pPetItem->IsCanGamble();
					if(pPetItem->GetItemOptionListCount() == 0)
						petInfo.OPTION = -1;
					else
						petInfo.OPTION = pPetItem->GetItemOptionList().front();
					petInfo.ITEM_TYPE = pPetItem->GetItemType();
					petInfo.NAME = pPetItem->GetPetName();
					petInfo.ITEM_ID = pPetItem->GetID();
					petInfo.ATTACK = pPetItem->IsCanAttack();
					petInfo.NICK_NAME = pFakeCreature->GetNickName();
					if(pFakeCreature->GetOwnerID() == g_pPlayer->GetID())
						petInfo.ChangeAbleNickName = true;
					else
						petInfo.ChangeAbleNickName = false;
					UI_RunPetInfo(&petInfo);
				}
				return false;
			}
		}
		
		if(g_pTopView->GetRequestMode() == MRequestMode::REQUEST_INFO)
		{
			SetTraceID( id );
			SetRequestMode( g_pTopView->GetRequestMode() );
			BasicActionToCreature();				
			SetTraceID( NULL );
			return false;	
		}	
		// 2004,5, 28 sobeit add end
		//GET_DYING_CREATURE( pCreature, id );

		
		
		
		
		if( pCreature != NULL )
		{
			if( pCreature->GetObjectType() == MObject::TYPE_CREATURE )
			{
				if( IsSlayer() && g_pZone->GetSector( GetX(), GetY() ).IsSafeSlayer() && 
					pCreature->IsSlayer() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeSlayer() && pCreature->IsPlayerOnly() ||
					IsVampire() && g_pZone->GetSector( GetX(), GetY() ).IsSafeVampire() && 
					pCreature->IsVampire() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeVampire() && pCreature->IsPlayerOnly() ||
					IsOusters() && g_pZone->GetSector( GetX(), GetY() ).IsSafeOusters() && 
					pCreature->IsOusters() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeOusters() && pCreature->IsPlayerOnly()
					)
				{
					//						UnSetLockMode();
					UnSetRepeatAction();
					//						g_pTopView->SetSelectedNULL();
					return false;
				}
			}
		}
		
//		bool bOusters = IsOusters();
//		MItem *pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);
		if (pCreature==NULL
			|| pCreature->IsDead()
			|| pCreature->IsInDarkness() && !pCreature->IsNPC() && 
			( !IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) ||
			IsVampire() &&g_pZone->GetID() == 3001)
			|| pCreature->IsInGroundElemental() && pCreature->IsOusters() && !g_pPlayer->IsOusters()
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
			&& !g_bLight
#endif
			)
		{
			UnSetRepeatAction();
			
			m_fTraceBuffer	= FLAG_TRACE_NULL;	
			m_fNextTrace	= FLAG_TRACE_NULL;
			return false;
		}


		bool	bTraceCreatureToForceAttack;
		bool	bKeepTraceCreature;

		bool bOusters = IsOusters();
		MItem *pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);

		//------------------------------------------------
		
		//------------------------------------------------
		if (g_pTopView->IsRequestMode() 
			
			//&& pCreature->GetCreatureType()<=CREATURETYPE_VAMPIRE_FEMALE
			&& (*g_pCreatureSpriteTable)[(*g_pCreatureTable)[m_CreatureType].SpriteTypes[0]].IsPlayerOnlySprite()
			
			&& !pCreature->IsInCasket()
			
			&& !pCreature->IsUndergroundCreature()
			&& pCreature->GetCreatureType() != CREATURETYPE_SLAYER_OPERATOR
			&& pCreature->GetCreatureType() != CREATURETYPE_VAMPIRE_OPERATOR
			&& pCreature->GetCreatureType() != CREATURETYPE_OUSTERS_OPERATOR

			
			//&& m_CreatureType!=CREATURETYPE_BAT
			//&& m_CreatureType!=CREATURETYPE_WOLF
			)
		{
			bTraceCreatureToForceAttack = false;
			bKeepTraceCreature			= false;
			m_bNextForceAttack			= false;
			
			SetRequestMode( g_pTopView->GetRequestMode() );
		}
		//------------------------------------------------
		
		
		//------------------------------------------------
		else if (m_MoveDevice == MOVE_DEVICE_RIDE 
				|| m_MoveDevice == MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) )
				|| m_CreatureType==CREATURETYPE_BAT
				|| !(bOusters && pOustersItem!=NULL && pOustersItem->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM || !bOusters)
				|| CanActionByZoneInfo()
				)
		{
			bTraceCreatureToForceAttack = false;
			bKeepTraceCreature			= true;	
			
			UnSetRequestMode();
		}
		//------------------------------------------------
		
		//------------------------------------------------		
		else if (bForceAttack)
		{
//			if(! (IsSlayer() && g_pZone->GetSector( GetX(), GetY() ).IsSafeSlayer() && pCreature->IsSlayer() && pCreature->IsPlayerOnly()|| 
//				IsVampire() && g_pZone->GetSector( GetX(), GetY() ).IsSafeVampire() && pCreature->IsVampire() && pCreature->IsPlayerOnly()) )
				bTraceCreatureToForceAttack = true;

			bKeepTraceCreature			= false;			

			UnSetRequestMode();
		}
		//------------------------------------------------
		
		//------------------------------------------------
		else
		{
			bTraceCreatureToForceAttack = CanAttackTribe( pCreature )
										
										|| IsVampire() && CanAttackGuild( pCreature )
										|| g_pObjectSelector->IsWarEnemy( pCreature );
									


			
			bTraceCreatureToForceAttack |= g_pJusticeAttackManager->HasCreature( pCreature->GetName() );

			
			bKeepTraceCreature			= !bTraceCreatureToForceAttack;	
		}

		
		if(IsAdvancementClass())
		{
			const MCreatureWear::ADDON_INFO& TempaddonInfo = GetAddonInfo(ADDON_RIGHTHAND);
			MCreature*	pCreature = m_pZone->GetCreature(id);
			if(pCreature != NULL)
			{
				if(!pCreature->IsNPC() && IsSlayer() && (!TempaddonInfo.bAddon || TempaddonInfo.ItemClass == ITEM_CLASS_CROSS || TempaddonInfo.ItemClass == ITEM_CLASS_MACE)
					&& bTraceCreatureToForceAttack)
					return false;
			}
		}
		// 2005, 1, 8, sobeit add end
		//------------------------------------------------
		
		
		//------------------------------------------------
		if (bKeepTraceCreature)
		{
			
		}
		
		else if (!pCreature->IsNPC())
		{
			if(!IsRequestMode() && IsInSafeSector() == 2)
				return false;
			
			
			//if (IsSlayer())
			{
			//	const MItem* pWeapon = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );
				
				//------------------------------------------------
				
				//------------------------------------------------
			
			
				//	&&	

				if (
					(IsOusters() && 
					g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND ) != NULL &&
					g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND )->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM || 
					!IsOusters() ) 
					&& 	(*g_pActionInfoTable)[m_nBasicActionInfo].IsWeaponTypeGunAny() 
					)
				{
					if (!IsRequestMode() 
						
						&& g_pCurrentMagazine!=NULL)		
					{
						if (g_pCurrentMagazine->GetNumber()==0)	
						{
							
							PlaySound( SOUND_ITEM_NO_MAGAZINE );

							m_fNextTrace	= FLAG_TRACE_NULL;

							
							m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);

							
//							__BEGIN_HELP_EVENT
//								ExecuteHelpEvent( HE_ITEM_EMPTY_BULLET );
//							__END_HELP_EVENT

							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

						
							return false;
						}
					}
				}
			}
			
			
			m_nNextUsedActionInfo	= GetBasicActionInfo(); //m_nBasicActionInfo;
		}
		
		
		SetTraceID( id );

		if( !bForceAttack && bClick )
		{
			m_TraceTimer = GetTickCount();
		}
		
		m_fTrace	= FLAG_TRACE_CREATURE_BASIC;
		m_TraceX	= pCreature->GetX();
		m_TraceY	= pCreature->GetY();
		m_TraceZ	= pCreature->GetZ();
		
		//------------------------------------------------------------
		
		
		//------------------------------------------------------------
		m_bTraceCreatureToForceAttack	= bTraceCreatureToForceAttack;
		m_bKeepTraceCreature			= bKeepTraceCreature;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		
		
		//
		
		
		//
		
		//------------------------------------------------------------
		if (m_bKeepTraceCreature)
		{
			m_TraceDistance = 1;
		}
		else
		{
			m_TraceDistance		= m_BasicAttackDistance;	
		}

		// NPC 
		if (pCreature->IsNPC())
		{
			if(pCreature->GetCreatureType() == 659)	
				m_TraceDistance		= 1;	
			else
				m_TraceDistance		= TRADE_DISTANCE_NPC;	
		}
		
		else if (IsRequestMode())
		{
			m_TraceDistance		= TRADE_DISTANCE_PC;	
		}

		
		m_fTraceBuffer	= m_fTrace;	
		m_TraceIDBuffer = id;

		//------------------------------------------------------------

		if( HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
			m_bTraceCreatureToForceAttack = false;


		
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		
		m_fNextTrace			= FLAG_TRACE_NULL;	

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace Creature To BasicAction : ID=%d, (%d, %d)", id, m_TraceX, m_TraceY);				

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_bKeepTraceCreature)// || m_bTraceCreatureToForceAttack)
		{
			return false;
		}

		
		g_pTopView->SetSelectedSectorNULL();
			

		return true;
	}
	else
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;
			
			return false;
		}
		else
		{
			m_fNextTrace	= FLAG_TRACE_CREATURE_BASIC;
			m_NextTraceID	= id;
			m_bNextForceAttack = bForceAttack;

			m_TraceIDBuffer = id;

			m_bRepeatAction = FALSE;

			m_fTraceBuffer = FLAG_TRACE_NULL;

			return true;
		}
	}

	return true;
}


//----------------------------------------------------------------------
// Trace Creature To SpecialAction
//----------------------------------------------------------------------


//----------------------------------------------------------------------

//

//
//----------------------------------------------------------------------
bool	
MPlayer::TraceCreatureToSpecialAction(TYPE_OBJECTID id, bool bForceAttack)
{
	SetActionDelay( 0 );

	if((HasEffectStatus(EFFECTSTATUS_SLEEP) || HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

//	if(bForceAttack && !g_pSDLInput->KeyDown(DIK_LSHIFT))
//	{
//		bForceAttack = false;
//	}
		
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	
		|| IsInDarkness() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) || g_pZone->GetID() == 3001)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		return false;
	}

	if (m_nSpecialActionInfo == ACTIONINFO_NULL
		
		|| m_MoveDevice == MOVE_DEVICE_RIDE
		
		
		|| IsInSafeSector() == 2
		
		|| CanActionByZoneInfo()
		|| HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) )				
		)
	{
		m_fNextTrace = FLAG_TRACE_NULL;

		UnSetRepeatAction();

		return false;
	}

		
	
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetOther())
	{
		int useSkill = m_nSpecialActionInfo;
		int originalSkill = m_nSpecialActionInfo;


//		if(m_nSpecialActionInfo == SKILL_BLOOD_DRAIN)
//		{
//			m_fNextTrace = FLAG_TRACE_NULL;
//			//UnSetRepeatAction();
//			if(g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo ) && m_TraceID != -1)
//			{
//				CGBloodDrain _CGBloodDrain;
//				_CGBloodDrain.setObjectID( m_TraceID );
//				g_pSocket->sendPacket( &_CGBloodDrain );

//			}
//			return false;
//		}

	
		if( m_nSpecialActionInfo == SKILL_BLITZ_SLIDING_ATTACK ||
			m_nSpecialActionInfo == SKILL_BLAZE_WALK_ATTACK 
			)
		{
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();

			return false;
		}

		int grade = 0;
		
		if( IsOusters() )
		{
			grade = (*g_pSkillInfoTable)[originalSkill].GetExpLevel() / 15;
			DEBUG_ADD_FORMAT("OUSTERS LEVEL : %d %d",originalSkill,(*g_pSkillInfoTable)[originalSkill].GetExpLevel());
		}

		if( grade>0 && (*g_pActionInfoTable)[originalSkill].IsUseActionStep() )
			useSkill = (*g_pActionInfoTable)[originalSkill].GetActionStep( grade - 1 );
		// 2004, 9, 20, sobeit add start
		if((*g_pActionInfoTable)[useSkill].IsMasterySkillStep())
		{
			int TempSkill = (*g_pActionInfoTable)[useSkill].GetActionStep( (*g_pActionInfoTable)[useSkill].GetMasterySkillStep() );
			if(g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)TempSkill ))
				useSkill = TempSkill;
		}
		// 2004, 9, 20, sobeit add end
		if( (*g_pActionInfoTable)[useSkill].GetParentActionInfo() != ACTIONINFO_NULL )
			originalSkill = (*g_pActionInfoTable)[useSkill].GetParentActionInfo();

		DEBUG_ADD_FORMAT("SKILL : %d %d", originalSkill, useSkill );

		
		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
			if (!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)originalSkill )
				|| !(*g_pSkillInfoTable)[originalSkill].IsEnable()
				|| !(*g_pSkillInfoTable)[originalSkill].IsAvailableTime()
				|| (*g_pSkillInfoTable)[originalSkill].IsPassive())
			{
				m_fNextTrace = FLAG_TRACE_NULL;

				UnSetRepeatAction();

				return false;
			}
		
		if (IsStop()) // || m_Action!=ACTION_ATTACK)
		{
			
			MCreature*	pCreature = m_pZone->GetCreature(id);

			//GET_DYING_CREATURE( pCreature, id );
			
			
			
			if (pCreature==NULL 
				
				|| pCreature->IsDead() && originalSkill!=MAGIC_RESURRECT && originalSkill!=SKILL_TRANSFUSION  && originalSkill!=SKILL_SOUL_REBIRTH 
				|| pCreature->IsInGroundElemental() && pCreature->IsOusters() && !g_pPlayer->IsOusters()
				
//				|| pCreature->IsInDarkness() && !pCreature->IsNPC() && 
//				!(IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS )  ||
//				g_pZone->GetID() != 3001 && IsVampire() )
//				&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST ) 
				|| pCreature->IsInDarkness() && !pCreature->IsNPC() && !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST ) &&
				(!IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS )  ||	g_pZone->GetID() == 3001 && IsVampire() )
				
#ifdef __METROTECH_TEST__
				&& !g_bLight
#endif
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_SLAYER && (!pCreature->IsSlayer() || !pCreature->IsPlayerOnly())
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_VAMPIRE && (!pCreature->IsVampire() || !pCreature->IsPlayerOnly())
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_OUSTERS && (!pCreature->IsOusters() || !pCreature->IsPlayerOnly())
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_MONSTER && (!pCreature->IsVampire() || pCreature->IsPlayerOnly())

				|| IsSlayer() && !(*g_pActionInfoTable)[originalSkill].IsUserSlayer() && pCreature->IsSlayer() && pCreature->IsPlayerOnly() && !bForceAttack
				|| IsVampire() && !(*g_pActionInfoTable)[originalSkill].IsUserVampire() && pCreature->IsVampire() && pCreature->IsPlayerOnly() && !bForceAttack				
				|| IsOusters() && !(*g_pActionInfoTable)[originalSkill].IsUserOusters() && pCreature->IsOusters() && pCreature->IsPlayerOnly() && !bForceAttack				
				) 
			{
				
				
				
				if (IsRepeatAction())
				{
					m_nUsedActionInfo = originalSkill;

					UnSetRepeatAction();

					m_nUsedActionInfo = ACTIONINFO_NULL;
				}
				else
				{
					UnSetRepeatAction();
				}

				m_fNextTrace = FLAG_TRACE_NULL; 
				m_fTraceBuffer	= FLAG_TRACE_NULL;	
				return false;
			}

			if( pCreature != NULL )
			{
				if( pCreature->GetObjectType() == MObject::TYPE_CREATURE )
				{
					if( IsSlayer() && g_pZone->GetSector( GetX(), GetY() ).IsSafeSlayer() && 
						pCreature->IsSlayer() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeSlayer() && pCreature->IsPlayerOnly() ||
						IsVampire() && g_pZone->GetSector( GetX(), GetY() ).IsSafeVampire() && 
						pCreature->IsVampire() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeVampire() && pCreature->IsPlayerOnly() ||
						IsOusters() && g_pZone->GetSector( GetX(), GetY() ).IsSafeOusters() && 
						pCreature->IsOusters() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeOusters() && pCreature->IsPlayerOnly())
					{
//						UnSetLockMode();
						UnSetRepeatAction();
//						g_pTopView->SetSelectedNULL();
						return false;
					}
				}
			}

			bool bTraceCreatureToForceAttack = true;
			bool bKeepTraceCreature			= false;			

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (pCreature->IsNPC())
			{
				TraceCreatureToBasicAction(id, false);				

				return false;
			}
			else
				if (bForceAttack)
				{
					bTraceCreatureToForceAttack = true;
					bKeepTraceCreature			= false;			
					
					UnSetRequestMode();
				}


			//-------------------------------------------------------
			
			//-------------------------------------------------------
			switch (originalSkill)
			{
				//-------------------------------------------------------
				// SKILL_TRANSFUSION
				//-------------------------------------------------------
				case SKILL_TRANSFUSION :

					//--------------------------------------------------
					
					//--------------------------------------------------
					
					
					
					if (GetHP() < 30
						|| pCreature->IsSlayer()
							//|| GetGuildNumber()!=pCreature->GetGuildNumber()	
							
							//&& GetGuildNumber()!=0
						//	&& pCreature->GetGuildNumber()!=0
						|| pCreature->GetHP()==pCreature->GetMAX_HP()
						|| pCreature->IsUndergroundCreature()
						|| pCreature->IsFlyingCreature() )
					{
						m_fNextTrace = FLAG_TRACE_NULL;

						UnSetRepeatAction();

						return false;
					}
				break;

				//-------------------------------------------------------
				// SKILL_THROW_BOMB
				//-------------------------------------------------------
				case SKILL_THROW_BOMB :
				{
					MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_BOMB );
								
					if (pItem==NULL)
					{
						m_fNextTrace = FLAG_TRACE_NULL;
						UnSetRepeatAction();
						return false;
					}

					useSkill = pItem->GetUseActionInfo();
				}
				break;
			}

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if ((*g_pActionInfoTable)[originalSkill].HasOption())
			{
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if ((*g_pActionInfoTable)[originalSkill].IsOptionUseWithBless())
				{
					if (!m_bEffectStatus[EFFECTSTATUS_BLESS])
					{
							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

							return false;
					}
				}
			}

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			WORD fWeaponType = (*g_pActionInfoTable)[originalSkill].GetWeaponType();
			if (fWeaponType	& FLAG_ACTIONINFO_WEAPON_HOLY_WATER)
			{
				MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_HOLYWATER );

				if (pItem==NULL)
				{
					m_fNextTrace = FLAG_TRACE_NULL;

					UnSetRepeatAction();

					return false;
				}

				
				
				
				//SetItemCheckBuffer( pItem, ITEM_CHECK_BUFFER_SKILL_FROM_ITEM );
			}

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (fWeaponType & FLAG_ACTIONINFO_WEAPON_BOMB)
			{
				MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_BOMB );

				if (pItem==NULL)
				{
					m_fNextTrace = FLAG_TRACE_NULL;

					UnSetRepeatAction();

					return false;
				}

				
				
				
				//SetItemCheckBuffer( pItem, ITEM_CHECK_BUFFER_SKILL_FROM_ITEM );
			}

			//-------------------------------------------------------
			
			
			//-------------------------------------------------------
			if ((*g_pActionInfoTable)[originalSkill].IsWeaponTypeGunAny())
			{
					if (g_pCurrentMagazine==NULL)
					{
						
						PlaySound( SOUND_ITEM_NO_MAGAZINE );

						
							m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);

						m_fNextTrace	= FLAG_TRACE_NULL;
						return false;					
					}
					else
					{
						if (g_pCurrentMagazine->GetNumber()==0)
						{
							
							PlaySound( SOUND_ITEM_NO_MAGAZINE );

							
							m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);

							if (IsRepeatAction())
							{
								m_nUsedActionInfo = originalSkill;

								UnSetRepeatAction();

								m_nUsedActionInfo = ACTIONINFO_NULL;
							}
							else
							{
								UnSetRepeatAction();
							}

							m_fNextTrace	= FLAG_TRACE_NULL;

							
//							__BEGIN_HELP_EVENT
//								ExecuteHelpEvent( HE_ITEM_EMPTY_BULLET );
//							__END_HELP_EVENT
							

							return false;
						}
					}
			}

			
			//if (pCreature->IsDead()) return;

			
			m_nNextUsedActionInfo	= useSkill;

			
			SetTraceID( id );
			m_fTrace	= FLAG_TRACE_CREATURE_SPECIAL;		
			m_TraceX	= pCreature->GetX();
			m_TraceY	= pCreature->GetY();
			m_TraceZ	= pCreature->GetZ();

			
			//m_TraceCreatureTribe = pCreature->GetCreatureTribe();

			
			m_fTraceBuffer	= m_fTrace;	
			m_TraceIDBuffer = id;

			//------------------------------------------------------------
			
			
			//------------------------------------------------------------
			//------------------------------------------------------------
			
			
			//------------------------------------------------------------
			m_bTraceCreatureToForceAttack	= bTraceCreatureToForceAttack;
			m_bKeepTraceCreature			= bKeepTraceCreature;
			
			m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);
			
			//------------------------------------------------------------

			
			SetAction( m_MoveAction );
			SetNextDestination(m_TraceX, m_TraceY);

			
			m_fNextTrace			= FLAG_TRACE_NULL;

			
			m_bKeepTraceCreature	= false;
		
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Trace Creature To SpecialAction: ID=%d, (%d, %d)", id, m_TraceX, m_TraceY);					

			
			g_pTopView->SetSelectedSectorNULL();
		}
		else
		{
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				m_fNextTrace	= FLAG_TRACE_CREATURE_SPECIAL;
				m_bNextForceAttack = bForceAttack;
				m_NextTraceID	= id;

				m_TraceIDBuffer = id;

				m_bRepeatAction = FALSE;

				m_fTraceBuffer = FLAG_TRACE_NULL;

				return true;//false;
			}
		}

		return true;
	}
	
	
	return false;
}


//----------------------------------------------------------------------
// Trace Sector To BasicAction
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MPlayer::TraceSectorToBasicAction(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus ( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	
		|| g_bZoneSafe)
	{
		return false;
	}

	
	if (sX==SECTORPOSITION_NULL || sY==SECTORPOSITION_NULL)
		return false;
	
	if (m_nBasicActionInfo == ACTIONINFO_NULL
		
		|| m_MoveDevice == MOVE_DEVICE_RIDE
		|| CanActionByZoneInfo())
	{
		return false;
	}
		
	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{		
		
		m_nNextUsedActionInfo	= GetBasicActionInfo(); //m_nBasicActionInfo;
		
		
		m_fTrace	= FLAG_TRACE_SECTOR_BASIC;
		m_TraceX	= sX;
		m_TraceY	= sY;
		m_TraceZ	= 0;
		
		
		m_fTraceBuffer	= m_fTrace;	

		//------------------------------------------------------------
		
		
		//------------------------------------------------------------
		
		m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);
		
		//------------------------------------------------------------		

		
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		
		m_fNextTrace			= FLAG_TRACE_NULL;
		
		
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		#ifdef	OUTPUT_DEBUG
			DEBUG_ADD_FORMAT("Trace Sector To BasicAction: (%d, %d)", sX, sY);				
		#endif
	}
	else
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace	= FLAG_TRACE_SECTOR_BASIC;
			m_NextTraceX	= sX;
			m_NextTraceY	= sY;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------
// Trace Sector To SpecialAction
//----------------------------------------------------------------------


//----------------------------------------------------------------------

//

//
//----------------------------------------------------------------------
bool	
MPlayer::TraceSectorToSpecialAction(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	if((HasEffectStatus(EFFECTSTATUS_SLEEP) || HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	
		|| g_bZoneSafe
		|| CanActionByZoneInfo()
		)
	{
		return false;
	}

	
	if (sX==SECTORPOSITION_NULL || sY==SECTORPOSITION_NULL)
		return false;
	
	if (m_nSpecialActionInfo == ACTIONINFO_NULL		
		
		|| m_MoveDevice == MOVE_DEVICE_RIDE
		
		
		|| IsInSafeSector() == 2
		
		)
	{
		m_fNextTrace = FLAG_TRACE_NULL;

		UnSetRepeatAction();

		return false;
	}
	
	if (m_nSpecialActionInfo==SKILL_TURRET_FIRE)
	{
		m_TraceDistance		= GetActionInfoRange(m_nSpecialActionInfo);
		if (max(abs(m_X-sX), abs(m_Y-sY)) > m_TraceDistance || !g_pSkillAvailable->IsEnableSkill( SKILL_TURRET_FIRE ))
		{
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			InstallTurretStopAttack();
			return false;
		}
	}
	// 2004, 9, 18, sobeit add end

	
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	
	bool bAbleTargetSector = false;
//	if(m_nSpecialActionInfo == SKILL_DESTRUCTION_SPEAR || m_nSpecialActionInfo == SKILL_DESTRUCTION_SPEAR_MASTERY)
//	{
//		if(g_pSkillAvailable->IsEnableSkill( SKILL_DESTRUCTION_SPEAR_MASTERY ))
//			bAbleTargetSector = true;
//	}
//	else 
	if(m_nSpecialActionInfo == SKILL_ICE_LANCE || m_nSpecialActionInfo == SKILL_ICE_LANCE_MASTERY)
	{
		if(g_pSkillAvailable->IsEnableSkill( SKILL_ICE_LANCE_MASTERY ))
			bAbleTargetSector = true;
	}
	// 2004, 9, 21, sobeit add end
	
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetZone() || bAbleTargetSector)
	{
		int grade = 0;
		int		useSkill = m_nSpecialActionInfo;

		if( IsOusters() )
		{
			grade = (*g_pSkillInfoTable)[useSkill].GetExpLevel() / 15;
		}
		
		if( (*g_pActionInfoTable)[useSkill].IsUseActionStep() && grade > 0 )
			useSkill = (*g_pActionInfoTable)[useSkill].GetActionStep( grade - 1 );			
			
		// 2004, 9, 20, sobeit add start
		if((*g_pActionInfoTable)[useSkill].IsMasterySkillStep())
		{
			int TempSkill = (*g_pActionInfoTable)[useSkill].GetActionStep( (*g_pActionInfoTable)[useSkill].GetMasterySkillStep() );
			if(g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)TempSkill ))
				useSkill = TempSkill;
		}
		// 2004, 9, 20, sobeit add end
		
//		if (useSkill==SKILL_TURRET_FIRE)
//		{
//			if (max(abs(m_X-sX), abs(m_Y-sY)) 
//				> m_TraceDistance)
//			{
//				m_fNextTrace = FLAG_TRACE_NULL;
//				UnSetRepeatAction();
//				return false;
//			}
//		}
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (useSkill==SKILL_THROW_BOMB)
		{
			//-------------------------------------------------------
			
			
			//-------------------------------------------------------
			if (IsSlayer()
				&& !IsWaitVerify()
				&& IsItemCheckBufferNULL())
			{											
				ITEM_CLASS	itemClass = ITEM_CLASS_BOMB;

				MItemClassFinder itemFinder(itemClass);
				MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );
							
				if (pItem==NULL)
				{
					m_fNextTrace = FLAG_TRACE_NULL;
					UnSetRepeatAction();
					return false;
				}

				useSkill = pItem->GetUseActionInfo();				
			}
		}

		
		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		if (
			(!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo ) && m_nSpecialActionInfo != SKILL_TURRET_FIRE)
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}

		if (IsStop()) // || m_Action!=ACTION_ATTACK)
		{	
#ifdef __METROTECH_TEST__
			if (IsOusters())
			{
				bool bAbsorbSoul = true;
				
				MItem* pItem = g_pZone->GetItem( g_pZone->GetItemID( sX, sY ) );
				
				if (pItem==NULL 
					|| pItem->GetItemClass()!=ITEM_CLASS_CORPSE)
				{
					bAbsorbSoul = false;
					
					if(pItem == NULL)
					{
						MCreature *pCreature = g_pZone->GetCreature(g_pZone->GetCreatureID(sX, sY));
						
						if(pCreature != NULL && pCreature->HasEffectStatus(EFFECTSTATUS_COMA))
						{
							bAbsorbSoul = true;
						}
					}
				}
				
				if(pItem != NULL)
				{
					
					MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();
					if(pCreature != NULL)
					{
						if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || pCreature->GetCreatureType() == 670 || pCreature->GetCreatureType() == 672 || pCreature->GetCreatureType() == 673)
							bAbsorbSoul = false;
					}
				}
				if(bAbsorbSoul)
					useSkill = SKILL_ABSORB_SOUL;
			}
#endif
			
			int targetX	= sX;
			int targetY = sY;

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			switch (useSkill)
			{
				
				case SKILL_TRANSFUSION:
					return false;
					break;
					
				case SKILL_WILD_TYPHOON :
					return false;
					break;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				case SKILL_ABSORB_SOUL :
					if (IsOusters())
					{
						MItem* pItem = g_pZone->GetItem( g_pZone->GetItemID( sX, sY ) );
						
						if (pItem==NULL 
							|| pItem->GetItemClass()!=ITEM_CLASS_CORPSE)
						{
							if(pItem == NULL)
							{
								MCreature *pCreature = g_pZone->GetCreature(g_pZone->GetCreatureID(sX, sY));

								if(pCreature != NULL && pCreature->HasEffectStatus(EFFECTSTATUS_COMA))
									break;
							}

							m_fNextTrace = FLAG_TRACE_NULL;
							
							UnSetRepeatAction();
							
							return false;
						}
						
						
						MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();
						if(pCreature != NULL)
						{
							if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || pCreature->GetCreatureType() == 670 || pCreature->GetCreatureType() == 672 || pCreature->GetCreatureType() == 673)
								return false;
						}
						
					}
					else
					{
						m_fNextTrace = FLAG_TRACE_NULL;
						
						UnSetRepeatAction();
						
						return false;
					}
					break;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				case SKILL_BITE_OF_DEATH :
					if (m_CreatureType != CREATURETYPE_WER_WOLF )
					{
						return false;
					}
					break;
				case MAGIC_EAT_CORPSE :
					if (m_CreatureType==CREATURETYPE_WOLF)
					{
						MItem* pItem = g_pZone->GetItem( g_pZone->GetItemID( sX, sY ) );

						if (pItem==NULL 
							|| pItem->GetItemClass()!=ITEM_CLASS_CORPSE)
						{
							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

							return false;
						}

						
						MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();
						if(pCreature != NULL)
						{
							if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || pCreature->GetCreatureType() == 670 || pCreature->GetCreatureType() == 672 || pCreature->GetCreatureType() == 673)
								return false;
						}
					
					}
					else
					{
						m_fNextTrace = FLAG_TRACE_NULL;

						UnSetRepeatAction();

						return false;
					}
				break;
//				// 2004, 8, 30, sobeit add start - sweep vice test
//				case SKILL_SWEEP_VICE_1:

//					{
//						SetSweepViewValue(1);
//						g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
//						m_TraceX	= 	g_SelectSector.x;
//						m_TraceY	=	g_SelectSector.y;
//						ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWEEP_VICE,g_SelectSector.x, g_SelectSector.y, 0,g_pPlayer->GetDirection(),0	,	
//							g_SelectSector.x, g_SelectSector.y, 0, 0xffff, NULL, false);
//						UnSetRepeatAction();
//					//	return false ;
//					}
//					else
//					{
//						if(!g_bRButtonDown)
//						{
//							if(2 == GetSweepViewValue())
//								useSkill = SKILL_SWEEP_VICE_3;
//							else if(3 == GetSweepViewValue())
//								useSkill = SKILL_SWEEP_VICE_5;
//						}
//						else 
//							return false;
//					}
//					break;
				case SKILL_TURRET_FIRE :
					if (!HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
						return false;
					break;
			}

	

// 2004, 8, 30, sob
	

			WORD fWeaponType = (*g_pActionInfoTable)[useSkill].GetWeaponType();
			
			//------------------------------------------------
			
			//------------------------------------------------
			if (fWeaponType	& FLAG_ACTIONINFO_WEAPON_BOMB)
			{
				int skillLevel = (*g_pSkillInfoTable)[SKILL_THROW_BOMB].GetExpLevel();

				int hitRoll = 50 + skillLevel/2;

				if (rand()%100 < hitRoll)
				{
					
				}
				else
				{
					if (rand()%100 < 25
						&& skillLevel < 30)		
					{
						
						targetX += rand()%5 - 2;
						targetY += rand()%5 - 2;
					}
					else						
					{
						
						targetX += rand()%3 - 1;
						targetY += rand()%3 - 1;
					}
				}
			}

			//------------------------------------------------
			
			//------------------------------------------------
			if ((*g_pActionInfoTable)[useSkill].IsWeaponTypeGunAny())
			{
				if (!IsRequestMode() && g_pCurrentMagazine!=NULL)		
				{
					if (g_pCurrentMagazine->GetNumber()==0)	
					{
						
						PlaySound( SOUND_ITEM_NO_MAGAZINE );

						m_fNextTrace	= FLAG_TRACE_NULL;

						

						
//						__BEGIN_HELP_EVENT
//							ExecuteHelpEvent( HE_ITEM_EMPTY_BULLET );
//						__END_HELP_EVENT

						m_fNextTrace = FLAG_TRACE_NULL;

						UnSetRepeatAction();
						
						if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
						{
							InstallTurretStopAttack();
						}
						else
							
						m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);
						// 2004, 9, 23, sobeit add end
						return false;
					}
				}
			}

			
			m_nNextUsedActionInfo	= useSkill;

			
			m_fTrace	= FLAG_TRACE_SECTOR_SPECIAL;
			m_TraceX	= targetX;
			m_TraceY	= targetY;
			m_TraceZ	= 0;

			
			m_fTraceBuffer	= m_fTrace;	
			
			//------------------------------------------------------------
			
			
			//------------------------------------------------------------
			
			m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);
			
			//------------------------------------------------------------		

			
			SetAction( m_MoveAction );
			SetNextDestination(m_TraceX, m_TraceY);

			
			m_fNextTrace			= FLAG_TRACE_NULL;

			
			m_bKeepTraceCreature	= false;
			
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Trace Sector To SpecialAction: (%d, %d)", sX, sY);					
			if(useSkill == SKILL_TELEPORT)
			{
				return false;
			}
		}
		else
		{

			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				DEBUG_ADD_FORMAT("Trace Sector To SpecialAction: Buffering");					
				
				m_fNextTrace	= FLAG_TRACE_SECTOR_SPECIAL;
				m_NextTraceX	= sX;
				m_NextTraceY	= sY;

				m_bRepeatAction = FALSE;

				
				//return false;
				return true;
				// 200411, 12, 21, sobeit modify end
			}
		}

		return true;
	}

	
	return false;
}

//----------------------------------------------------------------------
// Trace Item
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MPlayer::TraceItem(TYPE_OBJECTID id)
{
	TraceInteractionPlayer("TraceItem begin id=%d player=%d,%d action=%d wait=%d stop=%d",
		(int)id, m_X, m_Y, (int)m_Action, (int)m_WaitVerify, IsStop());
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	
		|| IsInDarkness() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS )||g_pZone->GetID() == 3001)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		TraceInteractionPlayer("TraceItem blocked id=%d notDelay=%d pause=%d casket=%d darkness=%d",
			(int)id, IsNotDelay(), HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE), IsInCasket(), IsInDarkness());
		return false;
	}

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{
		
		MItem*	pItem = m_pZone->GetItem(id);

		
		if (pItem==NULL
			|| g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() && 
			( !IsVampire() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) || 
			IsVampire() && g_pZone->GetID() == 3001) )
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
			&& !g_bLight
#endif
			) 
		{
			TraceInteractionPlayer("TraceItem no item/dark id=%d item=%p", (int)id, (void*)pItem);
			return false;
		}
	
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		if (max(abs(pItem->GetX()-m_X), abs(pItem->GetY()-m_Y)) <= 1)
		{
			TraceInteractionPlayer("TraceItem in range id=%d item=%d,%d player=%d,%d class=%d type=%d",
				(int)id, pItem->GetX(), pItem->GetY(), m_X, m_Y,
				(int)pItem->GetItemClass(), (int)pItem->GetItemType());
			DEBUG_ADD("Pickup Item in Trace Dist.");

			TraceNULL();
			
			
			PickupItem( pItem );	
			
			return true;
		}


		
		SetTraceID( id );
		m_fTrace	= FLAG_TRACE_ITEM;
		m_TraceX	= pItem->GetX();
		m_TraceY	= pItem->GetY();
		m_TraceZ	= 0;

		
		m_fTraceBuffer	= m_fTrace;	
		

		//------------------------------------------------------------		
		
		//------------------------------------------------------------
		
		m_TraceDistance		= 1;
		
		//------------------------------------------------------------

		
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);
		TraceInteractionPlayer("TraceItem moving id=%d item=%d,%d player=%d,%d traceDist=%d",
			(int)id, m_TraceX, m_TraceY, m_X, m_Y, m_TraceDistance);

		
		m_fNextTrace			= FLAG_TRACE_NULL;
	
		
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace IteM: %d", id);				
	}
	else
	{
		TraceInteractionPlayer("TraceItem buffered id=%d action=%d repeat=%d",
			(int)id, (int)m_Action, m_bRepeatAction);
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace	= FLAG_TRACE_ITEM;
			m_NextTraceID	= id;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------
// Trace InteractionObject
//----------------------------------------------------------------------



//

//----------------------------------------------------------------------
bool	
MPlayer::TraceInteractionObject(TYPE_OBJECTID id, BYTE currentAction)
{
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay()
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket())	
	{
		return false;
	}

	//------------------------------------------------
	
	//------------------------------------------------
	if (m_MoveDevice == MOVE_DEVICE_RIDE)
	{
		return false;
	}

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{
		
		MImageObject*	pImageObject = m_pZone->GetImageObject(id);

		
		if (pImageObject==NULL) return false;

		
		if (pImageObject->GetObjectType()!=MObject::TYPE_INTERACTIONOBJECT)
			return false;	 	

		
		SetTraceID( id );
		m_fTrace			= FLAG_TRACE_INTERACTIONOBJECT;
		m_TraceObjectAction	= currentAction;
		m_TraceX			= pImageObject->GetX();
		m_TraceY			= pImageObject->GetY();
		m_TraceZ			= 0;

		
		m_fTraceBuffer	= m_fTrace;	

		//------------------------------------------------------------		
		
		//------------------------------------------------------------
		
		m_TraceDistance		= 1;
		
		//------------------------------------------------------------

		
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		
		m_fNextTrace			= FLAG_TRACE_NULL;
	
		
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace InteractionObject: %d", id);				
	}
	else
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace			= FLAG_TRACE_INTERACTIONOBJECT;
			m_NextTraceID			= id;
			m_NextTraceObjectAction	= currentAction;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------
// Trace Effect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MPlayer::TraceEffect(TYPE_OBJECTID id)
{
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay()
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket())	
	{
		return false;
	}

	//------------------------------------------------
	
	//------------------------------------------------
	if (m_MoveDevice == MOVE_DEVICE_RIDE)
	{
		return false;
	}

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{
		MEffect*	pEffect = m_pZone->GetEffect( id );

		if (pEffect==NULL)
		{
			pEffect = m_pZone->GetGroundEffect( id );

			if (pEffect==NULL)
			{
				return false;
			}
		}
		
		if (!pEffect->IsSelectable())
			return false;	 	

		
		SetTraceID( id );
		m_fTrace			= FLAG_TRACE_EFFECT;
		//m_TraceObjectAction	= currentAction;
		m_TraceX			= pEffect->GetX();
		m_TraceY			= pEffect->GetY();
		m_TraceZ			= 0;

		
		m_fTraceBuffer	= m_fTrace;	

		//------------------------------------------------------------		
		
		//------------------------------------------------------------
		
		m_TraceDistance		= 1;
		
		//------------------------------------------------------------

		
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		
		m_fNextTrace			= FLAG_TRACE_NULL;
	
		
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace Effect: %d", id);				
	}
	else
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace			= FLAG_TRACE_EFFECT;
			m_NextTraceID			= id;
			//m_NextTraceObjectAction	= currentAction;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}



//----------------------------------------------------------------------
// Set Trace Position
//----------------------------------------------------------------------



//----------------------------------------------------------------------
 			

//----------------------------------------------------------------------
// Can Attack Tribe
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MPlayer::CanAttackTribe( enum CREATURETRIBE to ) const
{
	switch ( m_AttackMode )
	{
		//--------------------------------------------
		
		//--------------------------------------------
		case ATTACK_MODE_PEACE :
			return false;
		break;

		//--------------------------------------------
		
		//--------------------------------------------
		case ATTACK_MODE_AGGRESS :
			return true;
		break;

		//--------------------------------------------
		
		//--------------------------------------------
		
		
		case ATTACK_MODE_NORMAL :
			return GetCreatureTribe()!=to;// && to!=CREATURETRIBE_NPC;
		break;		
	}

	return false;
}

//----------------------------------------------------------------------
// Can Attack Guild
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MPlayer::CanAttackGuild( const MCreature* pCreature ) const
{
	switch ( m_AttackMode )
	{
		//--------------------------------------------
		
		//--------------------------------------------
		case ATTACK_MODE_PEACE :
			return false;
		break;

		//--------------------------------------------
		
		//--------------------------------------------
		case ATTACK_MODE_AGGRESS :
			return true;
		break;

		//--------------------------------------------
		
		//--------------------------------------------
		
		case ATTACK_MODE_NORMAL :
		{
			int gn = pCreature->GetGuildNumber();

			return m_GuildNumber!=gn
					
					
					

					
					&& gn!=GUILDID_VAMPIRE_DEFAULT && gn<MIN_USER_GUILDID
					|| GetCreatureTribe()!=pCreature->GetCreatureTribe();
					// && to!=CREATURETRIBE_NPC;
		}
		break;		
	}

	return false;
}

//----------------------------------------------------------------------
// Action In Trace Distance
//----------------------------------------------------------------------

//


//----------------------------------------------------------------------
bool
MPlayer::ActionInTraceDistance()
{
	int nextAction;
	if (m_nUsedActionInfo==ACTIONINFO_NULL)
	{
		if (m_MoveDevice==MOVE_DEVICE_RIDE)
		{
			if (IsSlayer())
			{
				nextAction = ACTION_SLAYER_MOTOR_STAND;				
			}
			else
			{
				
				//nextAction = ACTION_STAND;
			}
		}
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		else if( m_MoveDevice == MOVE_DEVICE_SUMMON_SYLPH )
		{
			if( IsOusters() )
			{
				nextAction = ACTION_OUSTERS_FAST_MOVE_STAND;
			}
		}
		else
		{
			nextAction = ACTION_STAND;
		}
	}
	else
	{
		nextAction = GetActionInfoAction(m_nUsedActionInfo);
	}

	//-------------------------------------------------------
	//
	
	//
	
	//
	//-------------------------------------------------------			
	if (max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY)) 
		<= m_TraceDistance)
	{	
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
				
		//----------------------------------------------------
		
		
		//----------------------------------------------------
		//---------------------
		
		//---------------------
		if (m_fTrace & FLAG_TRACE_SELF)
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------					
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;					
			SetNextAction(nextAction);

			//--------------------
			// message
			//--------------------
			DEBUG_ADD("Do Self SpecialAction");							
			
			return true;
		}
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_CREATURE_BASIC)
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			// pCreature
			
			//---------------------------------------------------------------
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;

			BasicActionToCreature();			// by viva	

			return true;
		}
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_CREATURE_SPECIAL)
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			// pCreature
			
			//---------------------------------------------------------------
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;
			SetNextAction(nextAction);

			/*
			#ifdef	CONNECT_SERVER
				CGAttackNormal _CGAttackNormal;
				_CGAttackNormal.setX( m_X );
				_CGAttackNormal.setY( m_Y );
				_CGAttackNormal.setDir( m_Direction );
				_CGAttackNormal.setObjectID( m_TraceID );
				g_pSocket->sendPacket( &_CGAttackNormal );

				#ifdef	OUTPUT_DEBUG
					DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
				#endif
			#endif
			*/

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Special Attack : %d", m_TraceID);							
			
			return true;
		}
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_SECTOR_BASIC)
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			// pCreature
			
			//---------------------------------------------------------------
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;
			SetNextAction(nextAction);

			/*
			#ifdef	CONNECT_SERVER
				CGAttackNormal _CGAttackNormal;
				_CGAttackNormal.setX( m_X );
				_CGAttackNormal.setY( m_Y );
				_CGAttackNormal.setDir( m_Direction );
				_CGAttackNormal.setObjectID( m_TraceID );
				g_pSocket->sendPacket( &_CGAttackNormal );

				#ifdef	OUTPUT_DEBUG
					DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
				#endif
			#endif
			*/

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Basic Attack Sector : (%d, %d)", m_TraceX, m_TraceY);
			
			return true;
		}
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_SECTOR_SPECIAL)
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			// pCreature
			
			//---------------------------------------------------------------
			//m_NextAction = ACTION_SHOOT;
			//m_ActionCount = m_ActionCountMax; 
			m_MoveCount = m_MoveCountMax;
			SetNextAction(nextAction);

			/*
			#ifdef	CONNECT_SERVER
				CGAttackNormal _CGAttackNormal;
				_CGAttackNormal.setX( m_X );
				_CGAttackNormal.setY( m_Y );
				_CGAttackNormal.setDir( m_Direction );
				_CGAttackNormal.setObjectID( m_TraceID );
				g_pSocket->sendPacket( &_CGAttackNormal );

				#ifdef	OUTPUT_DEBUG
					DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
				#endif
			#endif
			*/

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Special Attack Sector : (%d, %d)", m_TraceX, m_TraceY);							
			//*/

			return true;
		}
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_ITEM)
		{
			MItem*	pItem = m_pZone->GetItem( m_TraceID );

			
			if (pItem!=NULL
				&& (!g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() || IsVampire() && g_pZone->GetID() != 3001 || 
				!IsVampire() && HasEffectStatus( EFFECTSTATUS_LIGHTNESS )
				&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
				&& !g_bLight
#endif
				||!IsOusters())
				)
			{
				TraceInteractionPlayer("ActionInTraceDistance pickup id=%d item=%d,%d player=%d,%d class=%d type=%d",
					(int)m_TraceID, pItem->GetX(), pItem->GetY(), m_X, m_Y,
					(int)pItem->GetItemClass(), (int)pItem->GetItemType());
				PickupItem( pItem );
			}

			return true;
		}
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_INVENTORY)
		{
			MItem* pItem = g_pInventory->GetItemToModify( m_TraceID );

			
			if (pItem!=NULL)
			{
				
				m_MoveCount = m_MoveCountMax;
				SetNextAction(nextAction);
			}
			else
			{
				DEBUG_ADD_FORMAT("No such Item in Inventory: id=%d", m_TraceID);
			}
			
			return true;
		}
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_EFFECT)
		{
			MEffect* pEffect = m_pZone->GetEffect( m_TraceID );

			if (pEffect==NULL)
			{
				pEffect = m_pZone->GetGroundEffect( m_TraceID );

				if (pEffect==NULL)
				{
					return true;
				}
			}

			if (pEffect->IsSelectable())
			{		
					MEffectTarget* pEffectTarget = pEffect->GetEffectTarget();

					if (pEffectTarget!=NULL)
					{
						bool bSendPacket = true;
						if (pEffectTarget->GetEffectTargetType()==MEffectTarget::EFFECT_TARGET_PORTAL)
						{					
							if (IsSlayer())
							{
								bSendPacket = false;
							}
						}						

						if (bSendPacket)
						{
							int serverID = pEffectTarget->GetServerID();

							CGSelectTileEffect _CGSelectTileEffect;
							_CGSelectTileEffect.setEffectObjectID( serverID );
							
							g_pSocket->sendPacket( &_CGSelectTileEffect );
						}
					}
			}

			return true;
		}	
		//----------------------------------------------------
		//
		
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_INTERACTIONOBJECT)
		{
			MImageObject* pImageObject = m_pZone->GetImageObject( m_TraceID );

			if (pImageObject->GetObjectType()==MObject::TYPE_INTERACTIONOBJECT)
			{
				MInteractionObject*	pInteractionObject = (MInteractionObject*)pImageObject;
				pInteractionObject->SetNextAction( m_TraceObjectAction );
				
			}

			return true;
		}				

		return false;
	}
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	else
	{
		//------------------------------------------
		
		//------------------------------------------
		if (m_listDirection.empty())
		{
			//---------------------
			
			//---------------------
			if (m_fTrace & FLAG_TRACE_CREATURE_BASIC)
			{
				TraceCreatureToBasicAction( m_TraceID, m_bTraceCreatureToForceAttack );
			}
			//---------------------
			
			//---------------------
			else  if (m_fTrace & FLAG_TRACE_CREATURE_SPECIAL)
			{
				TraceCreatureToSpecialAction( m_TraceID, m_bTraceCreatureToForceAttack );
			}
			//---------------------
			
			//---------------------
			else if (m_fTrace & FLAG_TRACE_SECTOR_BASIC)
			{
				TraceSectorToBasicAction( m_TraceX, m_TraceY );
			}
			//---------------------
			
			//---------------------
			else if (m_fTrace & FLAG_TRACE_SECTOR_SPECIAL)
			{
				
				if (m_nUsedActionInfo==SKILL_TURRET_FIRE)
				{
					m_fNextTrace = FLAG_TRACE_NULL;
					UnSetRepeatAction();
					InstallTurretStopAttack();
					//SetAction(ACTION_STAND);
					return false;
				}
				// 2004, 9, 18, sobeit add end
				TraceSectorToSpecialAction( m_TraceX, m_TraceY );
			}
			//---------------------
			
			//---------------------
			else if (m_fTrace & FLAG_TRACE_ITEM)
			{
				TraceItem( m_TraceID );
			}
			//---------------------
			
			//---------------------
			else if (m_fTrace & FLAG_TRACE_INVENTORY)
			{
				TraceInventoryItem( m_TraceID );
			}
			//---------------------
			
			//---------------------
			else if (m_fTrace & FLAG_TRACE_EFFECT)
			{
				TraceEffect( m_TraceID );
			}
			//---------------------
			
			//---------------------
			else if (m_fTrace & FLAG_TRACE_INTERACTIONOBJECT)
			{
				TraceInteractionObject( m_TraceID, m_TraceObjectAction );
			}

			
			//SetAction( m_MoveAction );
		}


		//m_NextAction = m_MoveAction;
		SetNextAction( m_MoveAction );
	}

	return false;
}

//----------------------------------------------------------------------
// Move
//----------------------------------------------------------------------


// 

//----------------------------------------------------------------------
void	
MPlayer::ActionMove()
{
	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("ActionMove");
	#endif

	//----------------------------------------------------
	//
	
	//
	//----------------------------------------------------
	
	
	//----------------------------------------------------
	if( HasEffectStatus( EFFECTSTATUS_TRYING ) )
	{
		MZone::EFFECT_MAP::const_iterator itr = g_pZone->GetGroundEffects();
		bool bFind = false;

		for(int i = 0; i < g_pZone->GetGroundEffectCount(); itr++,i++ )
		{
			const MEffect* pEffect = itr->second;
			
			if( GetX() == pEffect->GetX() &&
				GetY() == pEffect->GetY() &&
				pEffect->GetFrameID() == EFFECTSPRITETYPE_REGEN_TOWER_GROUND )
			{
				bFind = true;
				break;
			}
		}
		if( bFind == false )
			RemoveEffectStatus( EFFECTSTATUS_TRYING );
	}

	if (m_MoveCount==0)//m_sX==0 && m_sY==0)
	{
		TYPE_SECTORPOSITION	x, y;

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		if (m_bFastMove)
		{
			x = m_NextX;
			y = m_NextY;

			m_NextX = SECTORPOSITION_NULL;					

			//m_CurrentDirection = 
			m_Direction = GetDirectionToPosition( x, y );

		}
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		else
		{
			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_DestX==SECTORPOSITION_NULL && m_NextDestX==SECTORPOSITION_NULL) 
			{
				DEBUG_ADD("### Don't need to move any more - Move Stop! ###");
				
				m_MoveCount = m_MoveCountMax;
				return;
			}

				//sprintf(g_pDebugMessage->GetCurrent(), "ListSize = %d", m_listDirection.size());
			
			//----------------------------------------------------
			// 
			
			
			//
			//----------------------------------------------------
			
			if (m_SendMove != 0  && g_bNetStatusGood)
			{
				static int lastSendCount = 0;

				if (lastSendCount != m_SendMove)
				{
					DEBUG_ADD_FORMAT("Waiting MoveOK = %d", m_SendMove);						
					
					lastSendCount = m_SendMove;
				}
			}

			//----------------------------------------------------
			//
			//
			
			//
			//
			//----------------------------------------------------		
			if (m_fTrace != FLAG_TRACE_NULL)
			{				
				//----------------------------------------------------	
				
				
				
				//----------------------------------------------------	
				if ((m_fTrace & FLAG_TRACE_CREATURE_BASIC) 
					|| (m_fTrace & FLAG_TRACE_CREATURE_SPECIAL))
				{
					
					MCreature*	pCreature = m_pZone->GetCreature( m_TraceID );				

					//GET_DYING_CREATURE( pCreature, m_TraceID );

					//-------------------------------------------------------
					
					
					//-------------------------------------------------------								
					if (pCreature==NULL 
						|| pCreature->IsInDarkness() && !pCreature->IsNPC() && 
						(!IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS )  || 
						g_pZone->GetID() == 3001 && IsVampire() ||
						pCreature->IsOusters() && pCreature->IsInGroundElemental() && !g_pPlayer->IsOusters())
						&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )						
#ifdef __METROTECH_TEST__
						&& !g_bLight
#endif
						) 
					{
						if (IsRepeatAction())
						{
							m_fNextTrace = FLAG_TRACE_NULL;

							
							UnSetRepeatAction();
							//TraceNextNULL();
						}
						else
						{
							
							TraceNULL();
								
							
							SetStop();					
						}

						#ifdef OUTPUT_DEBUG_PLAYER_ACTION
							DEBUG_ADD("creature lost");
						#endif
							
						return;			
					}
				
					//-------------------------------------------------------			
					
					//-------------------------------------------------------			
					if (pCreature->GetX()!=m_TraceX ||
						pCreature->GetY()!=m_TraceY)
					{										
						m_NextDestX = pCreature->GetX();
						m_NextDestY = pCreature->GetY();
						
					}

					m_TraceX	= pCreature->GetX();
					m_TraceY	= pCreature->GetY();
					m_TraceZ	= pCreature->GetZ();
				}
				//----------------------------------------------------	
				
				
				//----------------------------------------------------
				else if (m_fTrace & FLAG_TRACE_ITEM)
				{
					
					MItem*	pItem = m_pZone->GetItem( m_TraceID );

					//-------------------------------------------------------
					
					//-------------------------------------------------------
					if (pItem==NULL
						|| g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness()&& 
						(!IsVampire() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) ||
						IsVampire() &&g_pZone->GetID() == 3001) )
						&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
						&& !g_bLight
#endif
						) 
					{
						
						TraceNULL();
							
						
						SetStop();					
							
						#ifdef OUTPUT_DEBUG_PLAYER_ACTION
							DEBUG_ADD("item lost");
						#endif

						return;			
					}
				}

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (ActionInTraceDistance())
				{
					
					if (m_nNextUsedActionInfo!=ACTIONINFO_NULL)
					{						
						m_nUsedActionInfo = m_nNextUsedActionInfo;
						m_nNextUsedActionInfo = ACTIONINFO_NULL;

						
						m_bNextAction = true;
						
						if(m_nUsedActionInfo == SKILL_SET_AFIRE)
						{
							if(max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY))<2)
								SetNextAction(ACTION_ATTACK);
							else
								SetNextAction(ACTION_STAND);
						}
						else
						// 2004, 11, 23, sobeit add end
							SetNextAction(GetActionInfoAction( m_nUsedActionInfo ));
					}

					
					m_listDirection.clear();

					
					//m_ActionCount = m_ActionCountMax;
					m_MoveCount = m_MoveCountMax;

					
					SetDirectionToPosition( m_TraceX, m_TraceY );
					
							
					//----------------------------------------------------			
					
					//----------------------------------------------------
					if (m_bKeepTraceCreature)
					{
						int fTrace = m_fTrace;
						
						
						SetStop();

						m_fTrace = fTrace;
					}	
					else
					{
						TraceNULL();

						
						SetStop();
					}
				}
			
			}

			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_NextDestX!=SECTORPOSITION_NULL)
			{
				
				//SetDestination(m_NextDestX, m_NextDestY);
				m_DestX		= m_NextDestX;	// TEST
				m_DestY		= m_NextDestY;	// TEST
				m_NextDestX = SECTORPOSITION_NULL;
				m_NextDestY = SECTORPOSITION_NULL;			
			}

			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_DestX!=SECTORPOSITION_NULL)
				SetDestination(m_DestX, m_DestY);

			//----------------------------------------------------
			
			//----------------------------------------------------
			//SetAction( m_MoveAction );

			
			//----------------------------------------------------
			
			
			//----------------------------------------------------
			if (m_SendMove > g_pClientConfig->MAX_CLIENT_MOVE)	
			{			
				g_bNetStatusGood = false;

				#ifdef OUTPUT_DEBUG_PLAYER_ACTION
					DEBUG_ADD("max client move");
				#endif
				
				return;
			}
			else
			{
				g_bNetStatusGood = true;
			}

			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_listDirection.empty())
			{
				//--------------------------------------------
				
				//--------------------------------------------
				if (m_DestX==m_X && m_DestY==m_Y)
				{
					m_DestX = SECTORPOSITION_NULL;
					m_DestY = SECTORPOSITION_NULL;

					#ifdef OUTPUT_DEBUG_PLAYER_ACTION
						DEBUG_ADD("dest reached");
					#endif

					return;
				}

				#ifdef OUTPUT_DEBUG_PLAYER_ACTION
					DEBUG_ADD("no way");
				#endif

				return;
			}		

			
			//----------------------------------------------------	
			//
			
			//
			//----------------------------------------------------			
			//----------------------------------------------------
			
			//----------------------------------------------------
			m_Direction = m_listDirection.front();
			m_listDirection.pop_front();	
				
			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_NextDestX!=SECTORPOSITION_NULL)
			{
				//m_NextAction = m_MoveAction;
				SetNextAction( m_MoveAction );
			}

			//----------------------------------------------------
			
			
			//----------------------------------------------------

			//----------------------------------------------------
			
			//----------------------------------------------------
			x = m_X;
			y = m_Y;

			switch (m_Direction)
			{
				case DIRECTION_LEFTDOWN		: x--;	y++;	break;
				case DIRECTION_RIGHTUP		: x++;	y--;	break;
				case DIRECTION_LEFTUP		: x--;	y--;	break;
				case DIRECTION_RIGHTDOWN	: x++;	y++;	break;
				case DIRECTION_LEFT			: x--;			break;
				case DIRECTION_DOWN			:		y++;	break;
				case DIRECTION_UP			:		y--;	break;
				case DIRECTION_RIGHT		: x++;			break;
			}
		}

		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		if (m_pZone->CanMove(m_MoveType, x,y)
			|| m_bFastMove		
			
#ifdef __EXPO_CLIENT__
			|| g_UserInformation.Invisible
#endif
			)
		{		
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			if (m_bFastMove)
			{
				// nothing
			}
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			else
			{
				//----------------------------------------------------
				
				//----------------------------------------------------
				if (gC_vs_ui.IsInstallMineProgress())
				{
					gC_vs_ui.EndInstallMineProgress();
				}

				CGMove _CGMove;
				_CGMove.setX( m_X );
				_CGMove.setY( m_Y );
				_CGMove.setDir( m_Direction );
				g_pSocket->sendPacket( &_CGMove );

				//yckou
//				UpdateSocketOutput();
				
				m_listSendDirection.push_back( m_Direction );
				m_SendMove++;
				

				//----------------------------------------------------
				
				//----------------------------------------------------
				if (m_WaitVerifyActionInfo==WAIT_VERIFY_NPC_ASK)
				{
					MCreature* pCreature = g_pZone->GetCreature( g_pPCTalkBox->GetNPCID() );

					if (pCreature!=NULL)
					{
						//----------------------------------------------------
						
						//----------------------------------------------------
						if ((max(abs(m_X-pCreature->GetX()), abs(m_Y-pCreature->GetY()))) > TRADE_DISTANCE_NPC)
						{
							
							SetWaitVerifyNULL();

							
							g_pUIDialog->ClosePCTalkDlg();
						}
					}
				}
			}

			//-----------------------
			
			//-----------------------
			//m_pZone->MoveLightSight(m_X, m_Y, m_LightSight, m_Direction);
			//m_pZone->UnSetLightSight(m_X, m_Y, m_LightSight);
			//m_pZone->SetLightSight(x, y, m_LightSight);

			
			TYPE_SECTORPOSITION ox = m_X;
			TYPE_SECTORPOSITION oy = m_Y;

			
			MovePosition( x, y );
			
			//CheckInDarkness();

			//--------------------------------------------
			//
			
			//
			//--------------------------------------------
			if (IsSlayer())
			{
				const MSector& sector = m_pZone->GetSector(x, y);
				if (sector.IsPortal())
				{
					int numPortal = sector.GetPortalSize();		
					
					if (numPortal!=0)
					{
						PORTAL_LIST::const_iterator	iPortal = sector.GetPortalBegin();

						PORTAL_INFO portalInfo = *iPortal;			

						//--------------------------------------------
						
						//--------------------------------------------
						if (portalInfo.Type==MPortal::TYPE_MULTI_PORTAL)
						{
							DEBUG_ADD("[MPlayer] Run Elevator");
							
							gC_vs_ui.RunElevator();

							
							g_pTopView->SetSelectedSectorNULL();

							
							m_DestX		= SECTORPOSITION_NULL;
							m_DestY		= SECTORPOSITION_NULL;	
							m_NextDestX = SECTORPOSITION_NULL;
							m_NextDestY = SECTORPOSITION_NULL;	
						}

						iPortal++;
					}
				}
			}

			//--------------------------------------------
			
			//--------------------------------------------
			//
			
			//
			
			
			// 
			//
			
			//
			
			
			//
			//--------------------------------------------
			
			
			//--------------------------------------------		
			//--------------------------------------------
			
			//--------------------------------------------
			if (m_bFastMove)
			{
				
				
				
				m_sX = (ox - m_X) * TILE_X;
				m_sY = (oy - m_Y) * TILE_Y;
					
				//------------------------------------------------
				
				//------------------------------------------------
				
				int moveTimes_div_2 = (*g_pCreatureTable)[m_CreatureType].MoveTimes >> 1;
				// 2005, 1, 5, sobeit add start
				if(HasEffectStatus(EFFECTSTATUS_BIKE_CRASH))
				{
//					moveTimes_div_2 = moveTimes_div_2/2;
					m_MoveCountMax = m_MoveCountMax*2;
				}
				// 2005, 1, 5, sobeit add end
				m_cX = -m_sX / moveTimes_div_2;
				m_cY = -m_sY / moveTimes_div_2;

				m_MoveCount += 2;

				if (m_MoveCount > m_MoveCountMax)
				{
					m_MoveCount = m_MoveCountMax;
				}
			}
			//--------------------------------------------	
			
			//--------------------------------------------
			else
			{	
				m_DirectionMoved = m_Direction;

				m_sX = m_sXTable[m_DirectionMoved];
				m_sY = m_sYTable[m_DirectionMoved];

				//????????????????????????????????????????????????????????
				//???                                                  ???
				
				//???                                                  ???
				//????????????????????????????????????????????????????????
				int moveTimes_1;
				
				#ifdef OUTPUT_DEBUG
					if (m_CreatureType >= g_pCreatureTable->GetSize())
					{
						DEBUG_ADD_FORMAT("[Error] CreatureType is exceed MAX. id=%d, type=%d", m_ID, m_CreatureType);
						
						SetStop();

						return;
					}
				#endif

				if (m_MoveDevice==MOVE_DEVICE_WALK)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimes-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_RIDE)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else
				{					
					DEBUG_ADD_FORMAT("[Error] What is MoveType of this Player??? id=%d", m_ID);							
					
					SetStop();

					return;
				}

				m_MoveTableCount = 0;
				m_cX = m_cXTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];
				m_cY = m_cYTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];
			}

			m_sX += m_cX;
			m_sY += m_cY;

			m_MoveCount++;

			
			m_NextMoveCount = (*g_pCreatureTable)[m_CreatureType].MoveRatio;

			//------------------------------------------------
			
			//------------------------------------------------
			TYPE_SOUNDID soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( m_MoveAction );

			if (soundID!=SOUNDID_NULL)
			{
				PlaySound( soundID,
							false,
							m_X,
							m_Y);
			}
		}	
		//--------------------------------------------
		
		//--------------------------------------------
		
		else
		{
			m_NextDestX = m_DestX;
			m_NextDestY = m_DestY;
			m_listDirection.empty();


			DEBUG_ADD("   Block.");					
		}

		
		if (!m_listDirection.empty())
		{
			//m_NextAction = m_MoveAction;
			SetNextAction( m_MoveAction );		
		}
	}
	//----------------------------------------------------
	//
	
	//
	//----------------------------------------------------
	else
	{
		
		/*
		switch (m_MoveDevice)
		{
			case MOVE_DEVICE_NULL :			
				m_sX += m_cXTableDeviceNull[m_Direction][m_ActionCount];
				m_sY += m_cYTableDeviceNull[m_Direction][m_ActionCount];
			break;

			case MOVE_DEVICE_MOTOR1 :						
				m_sX += m_cXTableDeviceMotor1[m_Direction][m_ActionCount];
				m_sY += m_cYTableDeviceMotor1[m_Direction][m_ActionCount];
			break;
		}
		*/

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("going");
		#endif

		
		if (m_MoveCount>=m_NextMoveCount)
		{			
			//--------------------------------------------
			
			//--------------------------------------------
			if (m_bFastMove)
			{
				
				if (m_sX!=0)	m_sX += m_cX;
				if (m_sY!=0)	m_sY += m_cY;		

				m_MoveCount += 2;

				if (m_MoveCount > m_MoveCountMax)
				{
					m_MoveCount = m_MoveCountMax;
				}

				
				m_NextMoveCount += (*g_pCreatureTable)[m_CreatureType].MoveRatio;

				if (m_MoveCount+1 >= m_MoveCountMax
					&& m_nNoPacketUsedActionInfo==ACTIONINFO_NULL)
				{
					//----------------------------------------------------------
					
					//----------------------------------------------------------
					
					// Flash Sliding
					//----------------------------------------------------------
					//m_TraceID	= id;		
					MCreature* pCreature = g_pZone->GetCreature( m_TraceID );

					//GET_DYING_CREATURE( pCreature, m_TraceID );

					if (pCreature!=NULL 
						&& m_nSpecialActionInfo != MAGIC_RAPID_GLIDING 
						&& m_nSpecialActionInfo != SKILL_TELEPORT
						&& !HasEffectStatus(EFFECTSTATUS_BIKE_CRASH)
						)		
					{
//						_MinTrace(" SpecialActionInfo : %d UsedAction : %d NextUsedActionInfo :%d\n",
//							m_nSpecialActionInfo, m_nUsedActionInfo, m_nNextUsedActionInfo);
						#ifdef OUTPUT_DEBUG
							DEBUG_ADD("[FastMovePosition] set next action to attack");
						#endif

						UnSetRequestMode();

						
						
						//---------------------------------------------------
						
						//---------------------------------------------------
						MActionResult* pResult = new MActionResult;

						
						int ActionInfo = GetBasicActionInfo() ; //m_nBasicActionInfo;
						if( m_nSpecialActionInfo == SKILL_BLITZ_SLIDING || m_nSpecialActionInfo == SKILL_BLAZE_WALK )
						{
							m_fTrace	= FLAG_TRACE_CREATURE_SPECIAL;
							if(m_nSpecialActionInfo == SKILL_BLITZ_SLIDING )
								m_nNextUsedActionInfo	= SKILL_BLITZ_SLIDING_ATTACK;
							else if( m_nSpecialActionInfo == SKILL_BLAZE_WALK )
								m_nNextUsedActionInfo	= SKILL_BLAZE_WALK_ATTACK;
							else
								return;
							m_TraceX	= pCreature->GetX();
							m_TraceY	= pCreature->GetY();
							m_TraceZ	= pCreature->GetZ();
							
							m_bTraceCreatureToForceAttack	= false;
							m_bKeepTraceCreature			= false;
							m_TraceDistance		= m_BasicAttackDistance;
							
							m_bNextAction = true;
							SetNextAction(GetActionInfoAction( m_nNextUsedActionInfo ));
							SetDirection( MTopView::GetDirectionToPosition( GetX(), GetY(), pCreature->GetX(), pCreature->GetY() ) );
							return;
						}
						if(m_nSpecialActionInfo == SKILL_SET_AFIRE)
						{
							m_nNextUsedActionInfo = SKILL_SET_AFIRE;
							m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
							m_NextAction = ACTION_ATTACK;
							
							return;
						}
//						if(m_nSpecialActionInfo == MAGIC_BLOODY_ZENITH)
//						{
//							SetBloodyZenith(16);
//						}

						m_nNextUsedActionInfo	= GetBasicActionInfo() ; //m_nBasicActionInfo;

						pResult->Add( new MActionResultNodeActionInfo(
														ActionInfo,
														m_ID,
														pCreature->GetID(), 
														pCreature->GetX(),
														pCreature->GetY(),
														0 ) 
										);	

						
						if( m_nUsedActionInfo == SKILL_BLITZ_SLIDING_ATTACK ||
							m_nUsedActionInfo == SKILL_BLAZE_WALK_ATTACK 
							)
							m_nUsedActionInfo = GetBasicActionInfo(); //m_nBasicActionInfo;
						
						int tempActionInfo = m_nUsedActionInfo;
						m_nUsedActionInfo = GetBasicActionInfo(); 
						
						
						if (m_listEffectTarget.size()==0)
						{
							m_pEffectTarget = new MEffectTarget( 0 );
							m_pEffectTarget->NewEffectID();

							m_listEffectTarget.push_back( m_pEffectTarget );							
						}

						PacketAddActionResult( 0, pResult );

						m_nUsedActionInfo = tempActionInfo;
						//---------------------------------------------------
						
						//---------------------------------------------------
						
						
						m_fTrace	= FLAG_TRACE_CREATURE_BASIC;
						m_TraceX	= pCreature->GetX();
						m_TraceY	= pCreature->GetY();
						m_TraceZ	= pCreature->GetZ();

						m_bTraceCreatureToForceAttack	= true;
						m_bKeepTraceCreature			= false;
						m_TraceDistance		= m_BasicAttackDistance;
						
						m_bNextAction = true;
						SetNextAction(GetActionInfoAction( m_nNextUsedActionInfo ));
						SetDirection( MTopView::GetDirectionToPosition( GetX(), GetY(), pCreature->GetX(), pCreature->GetY() ) );

						
						m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
					}
					else
					{
						#ifdef OUTPUT_DEBUG
							DEBUG_ADD_FORMAT("[FastMovePosition] there is no such creature.id=%d", m_TraceID);
						#endif
					}
				}
	
			}
			//--------------------------------------------
			
			//--------------------------------------------
			else
			{
				int moveTimes_1;
							
				if (m_MoveDevice==MOVE_DEVICE_WALK)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimes-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_RIDE)
				{	
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this)))
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] What is MoveType of this Creature??? id=%d", m_ID);					
					SetStop();					
					return;
				}

				m_MoveTableCount++;
				
				if(m_MoveTableCount > moveTimes_1)
					m_MoveTableCount = moveTimes_1;
				// 2004, 12, 22, sobeit add end
				m_cX = m_cXTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];
				m_cY = m_cYTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];

				
				if (m_sX!=0)	m_sX += m_cX;
				if (m_sY!=0)	m_sY += m_cY;		
			
				
				m_NextMoveCount += (*g_pCreatureTable)[m_CreatureType].MoveRatio;

				m_MoveCount++;
			}
		}	
		else
		{
			m_MoveCount++;
		}
		
		//m_ActionCount++;
		
		//if (++m_ActionCount == (*m_pFrames)[m_Action][m_Direction].GetCount())
		//	m_ActionCount = 0;

		//if (m_ActionCount == (m_ActionCountMax>>1))
		//if (m_MoveCount == (m_MoveCountMax>>1))
		//{				
		//	SetLightSight( m_LightSight );
		//}
	}

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("moveok");
	#endif
}

//----------------------------------------------------------------------
// Attach CastingEffect
//----------------------------------------------------------------------
void	
MPlayer::AttachCastingEffect(TYPE_ACTIONINFO nUsedActionInfo, BOOL bForceAttach)
{
	MCreature::AttachCastingEffect(nUsedActionInfo, bForceAttach);

	
}

//----------------------------------------------------------------------
// Affect UsedActionInfo
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::AffectUsedActionInfo(TYPE_ACTIONINFO nUsedActionInfo)
{
	if (nUsedActionInfo==ACTIONINFO_NULL)
	{
		return;
	}

	switch(nUsedActionInfo)
	{
	case SKILL_ACID_STORM:
		{
			if((*g_pRankBonusTable)[RANK_BONUS_WIDE_STORM].GetStatus() == RankBonusInfo::STATUS_LEARNED)
				nUsedActionInfo = SKILL_ACID_STORM_WIDE;
		}
		break;
		
	case SKILL_POISON_STORM:
		{
			if((*g_pRankBonusTable)[RANK_BONUS_WIDE_STORM].GetStatus() == RankBonusInfo::STATUS_LEARNED)
				nUsedActionInfo = SKILL_POISON_STORM_WIDE;
		}
		break;

	case SKILL_KASAS_ARROW:
		break;

	case SKILL_LARSLASH :
		
		break;					

	case SKILL_SWEEP_VICE_1:
	case SKILL_SWEEP_VICE_3:
	case SKILL_SWEEP_VICE_5:
		if(g_pSkillAvailable->IsEnableSkill( SKILL_SWEEP_VICE_1 ))
		{
		}
		else
			return;
		break;
		//by viva
//	case SKILL_SLAYER_HEAL_SACRED_STAMP:
//		return;
	}
	POINT point;	
#ifdef __METROTECH_TEST__
//	if(g_bLight)
//	{
//		int plusX = GetX()-m_TraceX;
//		int plusY = GetY()-m_TraceY;
//		int moveX = 0, moveY = 0;
//		if(plusX != 0)
//			moveX = plusX/abs(plusX);
//		if(plusY != 0)
//			moveY = plusY/abs(plusY);
//		
//		g_pPlayer->UnSetRequestMode();
//		
//		g_SelectSector.x = GetX()+moveX;
//		g_SelectSector.y = GetY()+moveY;
//		if(abs(g_SelectSector.x - m_TraceX) > GetActionInfoRange(nUsedActionInfo))
//			g_SelectSector.x = GetX();
//		if(abs(g_SelectSector.y - m_TraceY) > GetActionInfoRange(nUsedActionInfo))
//			g_SelectSector.y = GetY();
//		

//		g_pPlayer->TraceNULL();
//		
//		if (g_pPlayer->SetMovePosition(g_SelectSector.x, g_SelectSector.y))
//		{
//			g_pPlayer->SetNextActionToMove();
//		}
//		

//		POINT point;
//		g_pPlayer->GetNextDestination( point );		
//		if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
//		{

//			g_pPlayer->GetDestination( point );
//			if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
//			{
//				//-_-;;
//			}
//			else
//			{
//				g_pTopView->SetSelectedSector( point );
//			}
//		}
//		else
//		{
//			g_pTopView->SetSelectedSector( point );
//		}		
//	}
#endif

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	MEffectTarget* pEffectTarget = NULL;

	DEBUG_ADD_FORMAT("[MPlayer-StartEffect] ActionInfo=%d, ActionCount=%d/%d", nUsedActionInfo, m_ActionCount, m_ActionCountMax);
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	BOOL bCastingAction = (*g_pActionInfoTable)[nUsedActionInfo].IsCastingAction();

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	point = MTopView::MapToPixel(m_TraceX, m_TraceY);
	//point.x += m_sX;
	//point.y += m_sY;
	//point.x += TILE_X_HALF;
	
	//--------------------------------------------------------
	//
	
	//Z
	//--------------------------------------------------------
	//MEffectTarget* m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
	// 
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (bCastingAction)
	{
		
		pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
		pEffectTarget->NewEffectID();

		if( nUsedActionInfo == CASTING_FIRE || nUsedActionInfo == CASTING_WATER ||
			nUsedActionInfo == CASTING_GROUND || nUsedActionInfo == CASTING_FIRE2 ||
			nUsedActionInfo == CASTING_GROUND2 || nUsedActionInfo == CASTING_WATER2 )
			pEffectTarget->Set( point.x, point.y, m_TraceZ, GetID() );
		else
			pEffectTarget->Set( point.x, point.y, m_TraceZ, m_TraceID );

		//--------------------------------------------------------
		
		
		//--------------------------------------------------------
		
		//if (m_DelayActionInfo==nUsedActionInfo) - -;
		{
			pEffectTarget->SetDelayFrame( m_EffectDelayFrame );
			m_DelayActionInfo	= ACTIONINFO_NULL;
			m_EffectDelayFrame	= 0;
		}

	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	else
	{
		//--------------------------------------------------------
		
		
		//--------------------------------------------------------
		if (nUsedActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo())
		{
		}
		else
		{
			if (m_pEffectTarget != NULL)
			{
				DEBUG_ADD_FORMAT("RemoveEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, nUsedActionInfo, m_pEffectTarget);
				
				RemoveEffectTarget( m_pEffectTarget->GetEffectID() );
				MEffectTarget* pTempEffectTarget = m_pEffectTarget;
				m_pEffectTarget = NULL;
				
				
				delete pTempEffectTarget;				
			}

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
			m_pEffectTarget->NewEffectID();

			
			AddEffectTarget( m_pEffectTarget );
			//m_listEffectTarget.push_back( pEffectTarget );

			DEBUG_ADD_FORMAT("NewEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, nUsedActionInfo, m_pEffectTarget);
		}
		

		if (m_pEffectTarget==NULL)
		{	
			DEBUG_ADD_FORMAT("EffectTarget is NULL, newEffectTarget : %x", m_pEffectTarget);
			
			m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
			m_pEffectTarget->NewEffectID();

			DEBUG_ADD("new EffectTarget OK");
		}

//		if( nUsedActionInfo == MAGIC_ACID_TOUCH || nUsedActionInfo == MAGIC_BLOODY_NAIL ||
//			nUsedActionInfo == SKILL_FLOURISH || nUsedActionInfo == SKILL_ABSORB_SOUL)
		if(nUsedActionInfo != ACTIONINFO_NULL && (*g_pActionInfoTable)[nUsedActionInfo].IsAttachSelf() )
		{
			//m_pEffectTarget->Set( point.x, point.y, m_TraceZ, m_TraceID );
			m_pEffectTarget->Set( point.x, point.y, m_TraceZ, GetID() );
		} else
		{
			m_pEffectTarget->Set( point.x, point.y, m_TraceZ, m_TraceID );
		}

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		
		if (nUsedActionInfo >= g_pActionInfoTable->GetMinResultActionInfo())
		{
			int soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
									GetActionInfoAction(nUsedActionInfo) );

			PlaySound( soundID,	false, m_X, m_Y	);

			//-------------------------------------------------
			// Force Feel
			//-------------------------------------------------
			if (g_pUserOption->UseForceFeel && gpC_Imm!=NULL && gpC_Imm->IsDevice()
				&& soundID < g_pSoundTable->GetSize())
			{
				DEBUG_ADD_FORMAT("ForceAction-AffectUsedActionInfo(%d, %s)", soundID, strrchr((*g_pSoundTable)[soundID].Filename.GetString(), '\\'));

#ifdef PLATFORM_WINDOWS
				// Force feedback only supported on Windows with IFC
				gpC_Imm->ForceAction( soundID );
#endif
			}
		}

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (m_DelayActionInfo==nUsedActionInfo)
		{
			m_pEffectTarget->SetDelayFrame( m_EffectDelayFrame );
			m_DelayActionInfo	= ACTIONINFO_NULL;
			m_EffectDelayFrame	= 0;
		}

		
		pEffectTarget = m_pEffectTarget;
		
		
		m_pEffectTarget = NULL;
	}

	
	//AddEffectTarget( m_pEffectTarget );
	
	
	/*
	MActionResult* pResult = new MActionResult;
	pResult->Add( new MActionResultNodeActionInfo( m_ID, m_TraceID, nUsedActionInfo ) );
	
	m_pEffectTarget->SetResult( pResult );
	*/
	

	//--------------------------------------------------------
	//
	
	//
	//--------------------------------------------------------
	int x,y,z, direction;

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartUser())
	{
		point = MTopView::MapToPixel(m_X, m_Y);
		point.x += m_sX;
		point.y += m_sY;
		//point.x += TILE_X_HALF;

		x			= point.x;
		y			= point.y;
		z			= m_Z+1;//+60;			
	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	else if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartTarget())
	{
		x			= point.x;
		y			= point.y;
		z			= m_Z;//+60;			
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartSky())
	{
		z	= m_Z + (*g_pActionInfoTable)[nUsedActionInfo].GetValue();

		//direction	= DIRECTION_DOWN;
	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	//else
	//{
	if( nUsedActionInfo == SKILL_ABSORB_SOUL )
	{
		
		direction = g_pTopView->GetDirectionToPosition( 
			GetX(), GetY(),m_TraceX, m_TraceY);
	}
	else
		direction	= m_Direction;
	//}

	DEBUG_ADD_FORMAT("[MPlayer-StartEffect] Before Generate");
	
	//--------------------------------------------------------
	//
	
	//
	//--------------------------------------------------------
	g_pEffectGeneratorTable->Generate(
			x,y,z,				
			direction, 		
			1,					// power
			nUsedActionInfo,		
			pEffectTarget		
			, GetActionGrade()
	);
	ClearActionGrade();

	DEBUG_ADD_FORMAT("[MPlayer-StartEffect] AffectUsedActionInfo OK");
}

//----------------------------------------------------------------------
// Action to Send Packet
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::ActionToSendPacket()
{
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (m_nUsedActionInfo >= g_pActionInfoTable->GetMinResultActionInfo() || m_nUsedActionInfo == ACTIONINFO_NULL)
	{
		return;
	}

	switch(m_nUsedActionInfo)
	{
	case SKILL_ACID_STORM_WIDE:
		m_nUsedActionInfo = SKILL_ACID_STORM;
		break;

	case SKILL_POISON_STORM_WIDE:
		m_nUsedActionInfo = SKILL_POISON_STORM;
		break;
	case SKILL_BURNING_SOUL_CHARGING:
		if(IsBurningSol()) 
			return;
		break;
	case SKILL_SWEEP_VICE_1:
	case SKILL_SWEEP_VICE_3:
	case SKILL_SWEEP_VICE_5:

//		{
//			SetSweepViewValue(1);
//			g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
//			ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWEEP_VICE,g_SelectSector.x, g_SelectSector.y, 0,g_pPlayer->GetDirection(),0	,	
//				g_SelectSector.x, g_SelectSector.y, 0, 0xffff, NULL, false);
//			return;
//		}
		if(g_bRButtonDown/*||!GetSweepViewValue()*/) 
			return;
			
		if(2 == GetSweepViewValue() || 3 == GetSweepViewValue())
			m_fTraceBuffer = FLAG_TRACE_SECTOR_SPECIAL;
		break;
	case SKILL_PLASMA_ROCKET_LAUNCHER:
		if(g_pCurrentMagazine != NULL)
		 {
			 int MagazineCount = g_pCurrentMagazine->GetNumber();
			 if(MagazineCount>0)
			 {
				 g_pCurrentMagazine->SetNumber(MagazineCount-1);
			 }
			 else
				 return;
		 }
		else return;
		break;
	// add by Coffee	2007-6-9
		
	case SKILL_BLLODY_SCARIFY :
	case SKILL_BLOOD_CURSE :
	case SKILL_VAMPIRE_INNATE_DEADLY_CLAW://by viva
	case SKILL_VAMPIRE_POISON_VOODOO_RING://by viva
 		break;
	case SKILL_SHINE_SWORD :
	case SKILL_BOMB_CRASH_WALK :
	case SKILL_SATELLITE_BOMB :
	case SKILL_ILLUSION_INVERSION :
	case SKILL_HEAVEN_GROUND :
	case SKILL_SLAYER_SWORD_SKY_FIRE://by viva
	case SKILL_SLAYER_BLADE_CUT_STORM://by viva
	case SKILL_SLAYER_GUN_BOMB_XRL_MISSILE://by viva
	case SKILL_SLAYER_HEAL_SACRED_STAMP://by viva
	case SKILL_SLAYER_ENCHANT_BRAMBLE_HALO://by viva

 		break;
	case SKILL_DUMMY_DRAKE :
	case SKILL_HYDRO_CONVERGENCE :
	case SKILL_SUMMON_CLAY :
	case SKILL_HETER_CHAKRAM :
 		break;
	// add end by Coffee	
	}

	 

	BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND || IsOusters() && m_Action == ACTION_OUSTERS_FAST_MOVE_STAND);
	int		actionCountMax_1 = m_ActionCountMax-1;
	BOOL	bEndAction		= (m_ActionCount==actionCountMax_1) || bActionStand;
	int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

	BOOL	bStartAction	= (m_ActionCount==0);	
	

	
	
	
	BOOL	bStartEffect = m_ActionCount==StartFrame || 
							StartFrame >= m_ActionCountMax && bEndAction;
	
	//BOOL	bCastingEffect	= GetActionInfoCastingStartFrame(m_nUsedActionInfo)==m_ActionCount;
		
	bStartAction |= m_bRepeatAction 
					&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame()
					&& m_RepeatCount!=0
					&& m_ActionCount == GetActionInfoRepeatStartFrame( m_nUsedActionInfo );

	//----------------------------------------------------------------------
	// [ TEST CODE ]
	//----------------------------------------------------------------------
	
	
	//----------------------------------------------------------------------
	
	
	
	//----------------------------------------------------------------------
	
	//static MEffectTarget* 

	
	#ifdef OUTPUT_DEBUG
			//DEBUG_ADD_FORMAT("action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
	#endif

	//----------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------
	if (//bStartEffect)
		bStartAction || m_nUsedActionInfo == SKILL_BURNING_SOUL_LAUNCH)
	{			
		//----------------------------------------------------------
		
		//----------------------------------------------------------
		if (m_nNoPacketUsedActionInfo==m_nUsedActionInfo)
		{
			m_nNoPacketUsedActionInfo = ACTIONINFO_NULL;

			int soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
											GetActionInfoAction(m_nUsedActionInfo) );

			//------------------------------------------------
			
			//------------------------------------------------
			PlaySound( soundID,	false, m_X, m_Y	);

			//-------------------------------------------------
			// Force Feel
			//-------------------------------------------------
			if (g_pUserOption->UseForceFeel && gpC_Imm!=NULL && gpC_Imm->IsDevice())				
			{				
				if ((*g_pActionInfoTable)[m_nUsedActionInfo].IsWeaponTypeGunAny())
				{
					soundID = SOUND_SLAYER_ATTACK_AR;
				}

				if (soundID < g_pSoundTable->GetSize())
				{
					DEBUG_ADD_FORMAT("ForceAction-ActionToSendPacket(%d, %s)", soundID, strrchr((*g_pSoundTable)[soundID].Filename.GetString(), '\\'));
#ifdef PLATFORM_WINDOWS
					// Force feedback only supported on Windows with IFC
					gpC_Imm->ForceAction( soundID );
#endif
				}
			}

			return;
		}

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		
		
		//
		
		//
		
		
		//
		
		
		
		//----------------------------------------------------------------------

		
		
		if (m_nUsedActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo())
		{
			//--------------------------------------------------------
			
			
			//--------------------------------------------------------
			if (m_Action==ACTION_STAND)
			{
				
				m_DelayTime	= g_CurrentTime + 300;
			}
			else if (m_bRepeatAction)
			{				
			}
			else
			{
				m_DelayTime	= g_CurrentTime 
								+ GetActionInfoDelay(m_nUsedActionInfo);
								
								
			}

		//----------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------

			// 
			if (m_pEffectTarget != NULL)
			{
				DEBUG_ADD_FORMAT("RemoveEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
				
				RemoveEffectTarget( m_pEffectTarget->GetEffectID() );

				MEffectTarget* pTempEffectTarget = m_pEffectTarget;
				m_pEffectTarget = NULL;
				
				delete pTempEffectTarget;			
				pTempEffectTarget = NULL;
			}

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[m_nUsedActionInfo].GetSize() );
			
			DEBUG_ADD_FORMAT("NewEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
			
			//---------------------------------------------------------
			
			//----------------------------------------------------------
			AddEffectTarget( m_pEffectTarget );

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			m_pEffectTarget->NewEffectID();

			
			if (m_fTraceBuffer!=FLAG_TRACE_NULL)
			{
				//----------------------------------------------------------
				
				//----------------------------------------------------------
				int originalActionInfo = m_nUsedActionInfo;
				if( (*g_pActionInfoTable)[m_nUsedActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
					originalActionInfo = (*g_pActionInfoTable)[m_nUsedActionInfo].GetParentActionInfo();
				(*g_pSkillInfoTable)[originalActionInfo].SetNextAvailableTime();

				//----------------------------------------------------------
				
				//----------------------------------------------------------
				ACTIONINFO_PACKET	packetType		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetPacketType();
				

				//----------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------
				
				//----------------------------------------------------------
				if (m_fTraceBuffer & FLAG_TRACE_CREATURE_BASIC) 
				{
					MCreature* pAttackTarget = (g_pZone != NULL) ? g_pZone->GetCreature(m_TraceID) : NULL;
					if (pAttackTarget == NULL || pAttackTarget->IsDead())
					{
						WriteCombatCrashMarker("CGAttack skipped stale target=%d target=%p trace=%d next=%d",
							m_TraceID, pAttackTarget, m_fTraceBuffer, m_fNextTrace);
						m_fTraceBuffer = FLAG_TRACE_NULL;
						m_fNextTrace = FLAG_TRACE_NULL;
						m_TraceIDBuffer = OBJECTID_NULL;
						UnSetRepeatAction();
					}
					else
					{
						WriteCombatCrashMarker("CGAttack send target=%d player=%d,%d targetPos=%d,%d hp=%d/%d",
							m_TraceID, m_X, m_Y, pAttackTarget->GetX(), pAttackTarget->GetY(),
							pAttackTarget->GetHP(), pAttackTarget->GetMAX_HP());

						CGAttack _CGAttack;
						_CGAttack.setX( m_X );
						_CGAttack.setY( m_Y );
						_CGAttack.setDir( m_Direction );
						_CGAttack.setObjectID( m_TraceID );
						g_pSocket->sendPacket( &_CGAttack );
						WriteCombatCrashMarker("CGAttack sent target=%d", m_TraceID);
					}

					// [ TEST CODE ] - -;;
					// [ TEST CODE ] - -;;
					// [ TEST CODE ] - -;;
					/*
					#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__) && defined(_DEBUG)
						if (g_DXInput.KeyDown(DIK_LMENU))
						{
							for (int i=0; i<200; i++)
							{
								g_pSocket->sendPacket( &_CGAttack );
							}
						}
					#endif
					*/

				}
				//----------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_BLOOD_DRAIN)
				{
					CGBloodDrain _CGBloodDrain;
					_CGBloodDrain.setObjectID( m_TraceID );
					g_pSocket->sendPacket( &_CGBloodDrain );
			
					
//					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
				}
				//----------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_ABSORB_SOUL)
				{
					CGAbsorbSoul _CGAbsorbSoul;
					_CGAbsorbSoul.setObjectID( g_pZone->GetItemID( m_TraceX, m_TraceY ) );
					_CGAbsorbSoul.setTargetZoneX( m_TraceX );
					_CGAbsorbSoul.setTargetZoneY( m_TraceY );

					int larvaSize = (*g_pItemTable)[ITEM_CLASS_LARVA].GetSize();
					MItem* pLarvaItem = NULL;
					for(int i = 0; i < larvaSize; i++)
					{
						 pLarvaItem= g_pInventory->FindItem( ITEM_CLASS_LARVA, i );
						if(pLarvaItem != NULL)
							break;
					}

					if(pLarvaItem == NULL)
					{
						_CGAbsorbSoul.setInvenObjectID(OBJECTID_NULL);
						_CGAbsorbSoul.setInvenX(255);
						_CGAbsorbSoul.setInvenY(255);
						_CGAbsorbSoul.setTargetInvenX(255);
						_CGAbsorbSoul.setTargetInvenY(255);
					}
					else
					{
						_CGAbsorbSoul.setInvenObjectID(pLarvaItem->GetID());
						_CGAbsorbSoul.setInvenX(pLarvaItem->GetGridX());
						_CGAbsorbSoul.setInvenY(pLarvaItem->GetGridY());

						MOustersPupa pupa;

						
						pupa.SetItemType(pLarvaItem->GetItemType());

						POINT p;
						g_pInventory->GetFitPosition(&pupa, p);
						_CGAbsorbSoul.setTargetInvenX(p.x);
						_CGAbsorbSoul.setTargetInvenY(p.y);

						
						
						SetItemCheckBuffer(pLarvaItem, ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);
						
						
						
						
						SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
					}

					g_pSocket->sendPacket( &_CGAbsorbSoul );

					
					
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
				}
				//----------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------
				else if	(packetType==ACTIONINFO_PACKET_OTHER
						||	(m_fTraceBuffer & FLAG_TRACE_CREATURE_SPECIAL))
				{
					#ifndef __EXPO_CLIENT__

						//-----------------------------------------------
						
						//-----------------------------------------------					
						if ((*g_pActionInfoTable)[m_nUsedActionInfo].GetWeaponType()
							& FLAG_ACTIONINFO_WEAPON_HOLY_WATER)
						{						
							MItem* pUsingItem = g_pInventory->FindItem( ITEM_CLASS_HOLYWATER );
							
							//-----------------------------------------------
							
							//-----------------------------------------------
							if (pUsingItem==NULL)
							{
								//-------------------------------------------------
								
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();

								
								TraceNULL();

								//------------------------------------------
								
								//------------------------------------------
								SetAction( ACTION_STAND );

								m_ActionCount++;

								m_nUsedActionInfo = ACTIONINFO_NULL;
	
								return;
							}

							CGThrowItem	_CGThrowItem;
							_CGThrowItem.setObjectID( pUsingItem->GetID() );	// item id
							_CGThrowItem.setTargetObjectID( m_TraceID );	//
							_CGThrowItem.setX( pUsingItem->GetGridX() );
							_CGThrowItem.setY( pUsingItem->GetGridY() );

							g_pSocket->sendPacket( &_CGThrowItem );						
							
							//--------------------------------------------------
							
							//--------------------------------------------------
							if (pUsingItem->GetNumber()>1)
							{
								pUsingItem->SetNumber( pUsingItem->GetNumber() - 1 );
							}
							//--------------------------------------------------
							
							//--------------------------------------------------
							else
							{
								MItem* pRemovedItem = g_pInventory->RemoveItem( pUsingItem->GetGridX(), pUsingItem->GetGridY() );

								if (pRemovedItem!=NULL)
								{
									
									UI_RemoveDescriptor( (void*)pRemovedItem );

									delete pRemovedItem;								
								}
								else
								{
									DEBUG_ADD_FORMAT("[Error] Cannot Remove Item to use Skill. id=%d", pUsingItem->GetID());
								}

								//-------------------------------------------------
								
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();
							}
						}
						//-----------------------------------------------
						
						//-----------------------------------------------					
						if ((*g_pActionInfoTable)[m_nUsedActionInfo].GetWeaponType()
							& FLAG_ACTIONINFO_WEAPON_BOMB)
						{		
							int itemType;

							MItem* pUsingItem = NULL;

							if (m_nUsedActionInfo==SKILL_THROW_BOMB)
							{
								pUsingItem = g_pInventory->FindItem( ITEM_CLASS_BOMB );
							}
							else
							{
								itemType = m_nUsedActionInfo - BOMB_SPLINTER;

								pUsingItem = g_pInventory->FindItem( ITEM_CLASS_BOMB, itemType );
							}
							
							//-----------------------------------------------
							
							//-----------------------------------------------
							if (pUsingItem==NULL)
							{
								//-------------------------------------------------
								
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();

								
								TraceNULL();

								//------------------------------------------
								
								//------------------------------------------
								SetAction( ACTION_STAND );

								m_ActionCount++;

								m_nUsedActionInfo = ACTIONINFO_NULL;
	
								return;
							}

							CGThrowBomb	cgThrowBomb;

							cgThrowBomb.setZoneX( m_TraceX );
							cgThrowBomb.setZoneY( m_TraceY );
							cgThrowBomb.setBombX( pUsingItem->GetGridX() );
							cgThrowBomb.setBombY( pUsingItem->GetGridY() );
							cgThrowBomb.setAttackSlayerFlag( m_TraceID!=OBJECTID_NULL );

							g_pSocket->sendPacket( &cgThrowBomb );						
							
							//--------------------------------------------------
							
							//--------------------------------------------------
							if (pUsingItem->GetNumber()>1)
							{
								pUsingItem->SetNumber( pUsingItem->GetNumber() - 1 );
							}
							//--------------------------------------------------
							
							//--------------------------------------------------
							else
							{
								MItem* pRemovedItem = g_pInventory->RemoveItem( pUsingItem->GetGridX(), pUsingItem->GetGridY() );

								if (pRemovedItem!=NULL)
								{
									
									UI_RemoveDescriptor( (void*)pRemovedItem );

									delete pRemovedItem;								
								}
								else
								{
									DEBUG_ADD_FORMAT("[Error] Cannot Remove Item to use Skill. id=%d", pUsingItem->GetID());
								}

								//-------------------------------------------------
								
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();
							}
						}
						else
						{
							int SkillType = m_nUsedActionInfo;
							if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
								SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();

							if(SkillType == SKILL_TELEPORT || SkillType == SKILL_CHARGING_ATTACK)
							{
								g_pPlayer->SetDelay(1000);
							}
							CGSkillToObject _CGSkillToObject;
							_CGSkillToObject.setSkillType( SkillType );
							_CGSkillToObject.setCEffectID( m_pEffectTarget->GetEffectID() );			
							_CGSkillToObject.setTargetObjectID( m_TraceID );
							g_pSocket->sendPacket( &_CGSkillToObject );						
						}

					#else
					
					
					
						if (m_nUsedActionInfo==BOMB_TWISTER)
						{
							int SkillType = m_nUsedActionInfo;
							if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
								SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();

							CGSkillToTile _CGSkillToTile;
							_CGSkillToTile.setSkillType( SkillType );
							_CGSkillToTile.setCEffectID( m_pEffectTarget->GetEffectID() );
							_CGSkillToTile.setX( m_TraceX );
							_CGSkillToTile.setY( m_TraceY );
							
							g_pSocket->sendPacket( &_CGSkillToTile );
						}
						else
						{
							int SkillType = m_nUsedActionInfo;
							if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
								SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();

							if(SkillType == SKILL_TELEPORT || SkillType == SKILL_CHARGING_ATTACK)
							{
								g_pPlayer->SetDelay(1000);
							}
							CGSkillToObject _CGSkillToObject;
							_CGSkillToObject.setSkillType( SkillType );
							_CGSkillToObject.setCEffectID( m_pEffectTarget->GetEffectID() );			
							_CGSkillToObject.setTargetObjectID( m_TraceID );
							
							g_pSocket->sendPacket( &_CGSkillToObject );
						}
					#endif
						
					
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
				}
				//----------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_SELF
						|| (m_fTraceBuffer & FLAG_TRACE_SELF))
				{
					//----------------------------------------------------------
					// OLD_PACKET
					//----------------------------------------------------------
					///*
					int originalActionInfo = m_nUsedActionInfo;
					if( (*g_pActionInfoTable)[originalActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
						originalActionInfo = (*g_pActionInfoTable)[originalActionInfo].GetParentActionInfo();
					CGSkillToSelf _CGSkillToSelf;
					_CGSkillToSelf.setSkillType( originalActionInfo );
					_CGSkillToSelf.setCEffectID( m_pEffectTarget->GetEffectID() );
					g_pSocket->sendPacket( &_CGSkillToSelf );

					
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
					//*/
				}
				//----------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_ZONE
						|| (m_fTraceBuffer & FLAG_TRACE_SECTOR_BASIC) 
						|| (m_fTraceBuffer & FLAG_TRACE_SECTOR_SPECIAL))
				{
					//----------------------------------------------------------
					// OLD_PACKET
					//----------------------------------------------------------
					///*
					int SkillType = m_nUsedActionInfo;
					if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
						SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();

					if(SkillType == SKILL_TELEPORT || SkillType == SKILL_CHARGING_ATTACK)
					{
						g_pPlayer->SetDelay(1000);
					}
					else if(SkillType == SKILL_SWEEP_VICE_1)
					{
						if(1 == GetSweepViewValue())
							SkillType = SKILL_SWEEP_VICE_1;
						else if(2 == GetSweepViewValue())
							SkillType = SKILL_SWEEP_VICE_3;
						else if(3 == GetSweepViewValue())
							SkillType = SKILL_SWEEP_VICE_5;
						m_TraceX = m_SweepViceX;
						m_TraceY = m_SweepViceY;
					}
					else 	if(SkillType == SKILL_BURNING_SOUL_LAUNCH)
					{
						m_TraceX = m_TempSelectPosX;
						m_TraceY = m_TempSelectPosY;
					}
					
					CGSkillToTile _CGSkillToTile;
					_CGSkillToTile.setSkillType( SkillType );
					_CGSkillToTile.setCEffectID( m_pEffectTarget->GetEffectID() );
					_CGSkillToTile.setX( m_TraceX );
					_CGSkillToTile.setY( m_TraceY );
					g_pSocket->sendPacket( &_CGSkillToTile );

					
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
					//*/
				}
				//----------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_ITEM
						|| (m_fTraceBuffer & FLAG_TRACE_INVENTORY))
				{
					MItem* pItem = g_pInventory->GetItemToModify( m_TraceID );

					if (pItem!=NULL && IsItemCheckBufferNULL())
					{
						
						CGSkillToInventory _CGSkillToInventory;
						
						_CGSkillToInventory.setSkillType( m_nUsedActionInfo );
						//_CGSkillToInventory.setCEffectID( m_pEffectTarget->GetEffectID() );
						_CGSkillToInventory.setObjectID( m_TraceID );
						_CGSkillToInventory.setX( pItem->GetGridX() );
						_CGSkillToInventory.setY( pItem->GetGridY() );

							
						
						
						SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);
						
						
						
						
						SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
					}
					
				}

				
			}		
		}


		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		
		
		
		
		//
		
		
		
		//
		
		//----------------------------------------------------------------------

		

		//------------------------------------------------
		
		//------------------------------------------------
		//g_Sound.Play( g_SoundTable[(*g_pActionInfoTable)[m_nUsedActionInfo].GetSoundID()].pDSBuffer );
		//PlaySound( (*g_pActionInfoTable)[m_nUsedActionInfo].GetSoundID() ,
		//			false,
		//			m_X, m_Y);	

		//------------------------------------------------
		
		//------------------------------------------------
		int soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
										GetActionInfoAction(m_nUsedActionInfo) );
		if(!HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET) && !m_bBurningSol)
			PlaySound( soundID,	false, m_X, m_Y	);

		//-------------------------------------------------
		// Force Feel
		//-------------------------------------------------
		if (g_pUserOption->UseForceFeel && gpC_Imm!=NULL && gpC_Imm->IsDevice())
		{
			if ((*g_pActionInfoTable)[m_nUsedActionInfo].IsWeaponTypeGunAny())
			{
				soundID = SOUND_SLAYER_ATTACK_AR;
			}

			if (soundID < g_pSoundTable->GetSize())
			{
				DEBUG_ADD_FORMAT("ForceAction-ActionToSendPacket(%d, %s)", soundID, strrchr((*g_pSoundTable)[soundID].Filename.GetString(), '\\'));
#ifdef PLATFORM_WINDOWS
				// Force feedback only supported on Windows with IFC
				gpC_Imm->ForceAction( soundID );
#endif
			}
		}
	}
}

//----------------------------------------------------------------------
// Set ServerPosition
//----------------------------------------------------------------------
void		
MPlayer::SetServerPosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	m_listSendDirection.clear();

	m_SendMove = 0;
	
	g_bNetStatusGood = true;

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	m_ServerX	= sX;
	m_ServerY	= sY;

	//--------------------------------------------------------
	
	
	//--------------------------------------------------------
	if (m_pZone!=NULL)
	{
		m_pZone->KeepObjectInSight(m_ServerX, m_ServerY, m_Sight);
	}

	SendPositionInfoToParty();
}

//----------------------------------------------------------------------
// Action Effect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::ActionEffect()
{
	 

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("ActionEffect");
	#endif

	// 2002.3.12
	
	if (m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo())
	{
		
		if (max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY)) 
			<= m_TraceDistance)
		{
			#ifdef OUTPUT_DEBUG_PLAYER_ACTION
				DEBUG_ADD("inDist");
			#endif
		}
		else
		{
			#ifdef OUTPUT_DEBUG_PLAYER_ACTION
				DEBUG_ADD("not inDist");
			#endif

			
			//if (m_listDirection.empty())
			{
				if (m_DestX != SECTORPOSITION_NULL
					&& m_DestY != SECTORPOSITION_NULL)
				{
					
				}
				else if (abs((int)m_X-m_TraceX) < g_SECTOR_WIDTH
						&& abs((int)m_Y-m_TraceY) < g_SECTOR_HEIGHT)
				{
					
					m_DestX = m_TraceX;
					m_DestY = m_TraceY;
				}
				
				if (SetDestination(m_TraceX, m_TraceY))
				{
					#ifdef OUTPUT_DEBUG_PLAYER_ACTION
						DEBUG_ADD("NewWayOK");
					#endif

					
					m_nNextUsedActionInfo = m_nUsedActionInfo;
					m_nUsedActionInfo = ACTIONINFO_NULL;
					
					SetAction( ACTION_STAND );
				}
				else
				{				
					
					
					SetAction(((m_MoveDevice==MOVE_DEVICE_WALK)? ACTION_STAND : ACTION_SLAYER_MOTOR_STAND));
					SetNextAction(ACTION_STAND);
					m_fTraceBuffer = 0;
					m_fNextTrace = 0;
					m_bTraceCreatureToForceAttack = false;
					SetStop();
					UnSetRepeatAction();
					TraceNULL();
					m_nUsedActionInfo = ACTIONINFO_NULL;
					m_nNextUsedActionInfo = ACTIONINFO_NULL;

					#ifdef OUTPUT_DEBUG_PLAYER_ACTION
						DEBUG_ADD("Stop!");
					#endif

					return;
				}

				return;
			}
		}
	}

	BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND);
	int		actionCountMax_1 = m_ActionCountMax-1;
	BOOL	bEndAction		= (m_ActionCount==actionCountMax_1) || bActionStand;
	int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );
	
	BOOL	bStartAction	= (m_ActionCount==0 && !IsGunShotGuidance());
	
	
	
	
	BOOL	bStartEffect = m_ActionCount==StartFrame || 
							StartFrame >= m_ActionCountMax && bEndAction;

	
	bStartAction |= m_bRepeatAction 
					&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame()
					&& m_RepeatCount!=0
					&& m_ActionCount == GetActionInfoRepeatStartFrame( m_nUsedActionInfo );

	//BOOL	bCastingEffect	= GetActionInfoCastingStartFrame(m_nUsedActionInfo)==m_ActionCount;
	

	//----------------------------------------------------------------------
	// [ TEST CODE ]
	//----------------------------------------------------------------------
	
	
	//----------------------------------------------------------------------
	
	
	
	//----------------------------------------------------------------------
	
	//static MEffectTarget* 

	
	#ifdef OUTPUT_DEBUG
			//DEBUG_ADD_FORMAT("action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
	#endif

	int SkillInfo = m_nUsedActionInfo;
	if(SkillInfo == SKILL_SWEEP_VICE_1)
	{
		if(!GetSweepViewValue() && g_bRButtonDown) 
		{
			if(g_pSkillAvailable->IsEnableSkill( SKILL_SWEEP_VICE_1)
				&&!IsDead()
				&&!(*g_pSkillInfoTable)[SKILL_SWEEP_VICE_1].GetAvailableTimeLeft()
				)
			{
				SetSweepViewValue(1);
				g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);

				m_TempSelectPosX = g_SelectSector.x;
				m_TempSelectPosY = g_SelectSector.y;
				m_SweepViceX = m_TraceX;
				m_SweepViceY = m_TraceY;
				ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWEEP_VICE,m_SweepViceX, m_SweepViceY, 0,g_pPlayer->GetDirection(),0	,	
						m_SweepViceX, m_SweepViceY, 0, 0xffff, NULL, false);
			}
			return;
		}
		else if(g_bRButtonDown)
			return;

	//	bStartEffect = true;

		if(1 == GetSweepViewValue())
			SkillInfo = SKILL_SWEEP_VICE_1;
		else if(2 == GetSweepViewValue())
			SkillInfo = SKILL_SWEEP_VICE_3;
		else if(3 == GetSweepViewValue())
			SkillInfo = SKILL_SWEEP_VICE_5;
		else
		{
			SkillInfo = SKILL_SWEEP_VICE_1;
			SetSweepViewValue(0);
		}
	}
	
	//----------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------
	if (
		bStartAction)		
	{	
		if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
		{
			 if(bStartEffect && g_pCurrentMagazine != NULL)
			 {
				 int MagazineCount = g_pCurrentMagazine->GetNumber();
				 if(MagazineCount>0)
				 {
					 g_pCurrentMagazine->SetNumber(MagazineCount-1);
					 ActionToSendPacket();
				 }
			 }
		}
		else
			ActionToSendPacket();
	}

	//-------------------------------------------------------------
	//
	// Casting effect
	//
	//-------------------------------------------------------------
	AttachCastingEffect( SkillInfo );

	//-------------------------------------------------------------
	
	
	//-------------------------------------------------------------

	
	if (bActionStand)
	{
		m_ActionCount = actionCountMax_1;
	}

	//----------------------------------------------------------
	
	//----------------------------------------------------------
	
	
	//----------------------------------------------------------
	m_ActionCount++; 


	//--------------------------------------------------------	
	
	//--------------------------------------------------------	
	if (m_bStopBloodDrain)
	{
		StopBloodDrain();
	}
	if (m_bStopAbsorbSoul)
	{
		StopAbsorbSoul();
	}

		
	//----------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------
	
	TYPE_ACTIONINFO currentUsedActionInfo = SkillInfo;

	if (bStartEffect)
	{
		AffectUsedActionInfo(SkillInfo);
	}
	




	//----------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------
	if (bEndAction)
		//&& (*g_pActionInfoTable)[ m_nUsedActionInfo ].GetSize()!=0)		
	{
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		//m_TraceID	= OBJECTID_NULL;
		//m_Action	= ACTION_STAND;

		//--------------------------------------------------------
		
		
		//--------------------------------------------------------
		// [ TEST CODE ]
		//if (//currentUsedActionInfo!=ACTIONINFO_NULL && 
		//	currentUsedActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo())
		//{
		//	m_DelayTime	= g_CurrentTime + GetActionInfoDelay(currentUsedActionInfo);
		//}

		//m_bAffectUsedActionInfo = true;


		//------------------------------------------------------------
		
		//------------------------------------------------------------
			///*
		if (CheckRepeatAction())
		{
			CheckBufferAction();
		}

		
		m_nUsedActionInfo = ACTIONINFO_NULL;		

		//*/
	}	

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("AEEnd");
	#endif
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
//void	
//MPlayer::SetLightSight(char LightSight)
//{

////	if (m_TimeLightSightX != SECTORPOSITION_NULL)
////	{
//////		m_pZone->UnSetLight(m_LightSightX, m_LightSightY, m_LightSight);	
////	}
//

//	m_TimeLightSight = LightSight;
//

////	m_TimeLightSightX = m_X;
////	m_TimeLightSightY = m_Y;
//

////	m_pZone->SetLight(m_X, m_Y, m_LightSight);			
//}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
//void	
//MPlayer::UnSetLightSight()
//{

////	if (m_LightSightX != SECTORPOSITION_NULL)
////	{
////		m_pZone->UnSetLight(m_LightSightX, m_LightSightY, m_LightSight);	
////	}
//	
//	//m_LightSight = 0;
//
////	m_LightSightX = SECTORPOSITION_NULL;
////	m_LightSightY = SECTORPOSITION_NULL;
//}
//
//----------------------------------------------------------------------
// Set ItemCheckBuffer
//----------------------------------------------------------------------
	#ifdef __TEST_SUB_INVENTORY__   
void MPlayer::SetItemCheckBuffer(MItem* pItem, enum ITEM_CHECK_BUFFER status, TYPE_OBJECTID SubItem)
	#else
void MPlayer::SetItemCheckBuffer(MItem* pItem, enum ITEM_CHECK_BUFFER status)
	#endif

{
	m_pItemCheckBuffer = pItem;
	m_ItemCheckBufferTick = GetTickCount();

	#ifdef __TEST_SUB_INVENTORY__   
		m_dwSubItemIDCheckBuffer = SubItem;
	#endif

	m_ItemCheckBufferStatus = status;	

	#ifdef OUTPUT_DEBUG
	DEBUG_ADD_FORMAT("[SetItemCheckBuffer] status %d", status);
//		switch (status)
//		{
//			case ITEM_CHECK_BUFFER_PICKUP_TO_INVENTORY :
//				DEBUG_ADD("ITEM_CHECK_BUFFER_PICKUP_TO_INVENTORY");
//			break;
//
//			case ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE :
//				DEBUG_ADD("ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE");
//			break;
//
//			case ITEM_CHECK_BUFFER_DROP_TO_ZONE :
//				DEBUG_ADD("ITEM_CHECK_BUFFER_DROP_TO_ZONE");
//			break;
//		}
	#endif

	UI_LockItem();
	UI_LockGear();
	UI_LockItemTrade();
}

//----------------------------------------------------------------------
// Clear ItemCheckBuffer
//----------------------------------------------------------------------
void
MPlayer::ClearItemCheckBuffer()
{
	DEBUG_ADD("Set Item Check Buffer to NULL");
	
	m_pItemCheckBuffer = NULL;
	m_ItemCheckBufferTick = 0;
	#ifdef __TEST_SUB_INVENTORY__   
		m_dwSubItemIDCheckBuffer = OBJECTID_NULL;
	#endif
	m_ItemCheckBufferStatus = ITEM_CHECK_BUFFER_NULL;

	
	
	if(!IsInCasket())
		UI_UnlockGear();
	// 2004, 8, 27, sobeit add end - 
	UI_UnlockItem();
	UI_UnlockItemTrade();
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::SetAction(BYTE action)
{
//	if ( m_nSpecialActionInfo == MAGIC_BLOODY_ZENITH && m_Action == ACTION_ATTACK &&
//		m_ActionCount >= m_ActionCountMax && GetActionDelay() > 0)
//	{
//		SetActionDelay( GetActionDelay() - 1 );
//		m_ActionCount = m_ActionCountMax - 2;
//		return;
//	}	
	// 2004, 10, 6, sobeit add start
	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET) && (action == ACTION_DAMAGED || action == ACTION_DRAINED))
		return;
	// 2004, 10, 6, sobeit add end
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
//		return;
		action = ACTION_STAND;
	}

	
	m_bFastMove = false;
	//m_bFastMove = false;
	
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if (action < GetActionMax())
	{
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (m_MoveDevice==MOVE_DEVICE_RIDE)
		{
			if (IsSlayer())
			{
				if (action==ACTION_STAND)
				{
					action = ACTION_SLAYER_MOTOR_STAND;
				}
				else if (action==ACTION_MOVE)
				{
					action = ACTION_SLAYER_MOTOR_MOVE;
				}
			}
		}
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		else
		{
			if (IsSlayer())
			{
				if (action==ACTION_SLAYER_MOTOR_STAND)
				{
					action = ACTION_STAND;
				}
				else if (action==ACTION_SLAYER_MOTOR_MOVE)
				{
					action = ACTION_MOVE;
				}
			}
		}

		if( IsOusters() && (HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ) || m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH))
		{
			if (action==ACTION_STAND)
			{
				action = ACTION_OUSTERS_FAST_MOVE_STAND;
			} 
			else if( action == ACTION_MOVE )
			{
				action = ACTION_OUSTERS_FAST_MOVE_MOVE;
			}
		}

		#ifdef OUTPUT_DEBUG
			if (IsSlayer())
			{
				
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] Slayer's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else if (IsVampire())
			{
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] Vampire's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else if (IsOusters())
			{
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] Ousters's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else if (IsNPC())
			{
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] NPC's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else
			{
				// - -;;
				DEBUG_ADD("[Error] What is this Creature????");
				
				return;
			}
		#endif

		m_bNextAction = false;

		//------------------------------------------------
		
		//------------------------------------------------
		if (action==m_MoveAction)
		{
			
			
			if (m_Action==ACTION_STAND 
				|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND
				|| IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND)
			{			
				m_ActionCount = 0;
				m_ActionCountMax = 0;			
			}

			//------------------------------------------------
			
			//------------------------------------------------
			if (IsStop())
			{	
				m_Action = action;
				//SetAction( action );

				SetNextAction(ACTION_STAND);

				
				m_MoveCount = 0;
				m_NextMoveCount = 0;
				
				
				
				//m_MoveCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( action );
				m_MoveCountMax = GetCreatureActionCountMax( this, action );
					//g_pTopView->m_CreatureFPK[(*g_pCreatureTable)[m_CreatureType].FrameID]
					//									[action][m_Direction].GetSize();		
			
				//---------------------------------------------
				
				//---------------------------------------------
				//gC_vs_ui.LockItem();
				gC_vs_ui.LockGear();				
			}
			//------------------------------------------------
			
			//------------------------------------------------
			else
			{
				//---------------------------------------------
				
				//---------------------------------------------
				//gC_vs_ui.UnlockItem();
				gC_vs_ui.UnlockGear();

				// buffering
				SetNextAction(action);
			}
		}
		//------------------------------------------------
		
		//------------------------------------------------
		else
		{
			//------------------------------------------------
			
			//------------------------------------------------
//			if (HasEffectStatus(EFFECTSTATUS_EXTREME))
//			{
//				if (action==ACTION_MAGIC
//					|| action==ACTION_DIE
//					|| action==ACTION_VAMPIRE_DRAIN
//					|| action==ACTION_DRAINED)
//				{
//					RemoveEffectStatus(EFFECTSTATUS_EXTREME);
//				}
//			}

			if (
				m_MoveDevice==MOVE_DEVICE_RIDE && action!=ACTION_SLAYER_MOTOR_STAND
				
				|| action==ACTION_DAMAGED 						
						&& (
							m_Action!=ACTION_DAMAGED && m_Action!=ACTION_STAND && m_Action!=ACTION_MOVE

							
	//						m_Action==ACTION_DRAINED
							
	//						|| IsVampire() && m_Action==ACTION_VAMPIRE_DRAIN
							)
				)
			{
				SetNextAction(ACTION_STAND);
				return;
			}

			//---------------------------------------------
			
			//---------------------------------------------
			//gC_vs_ui.UnlockItem();
			if(!IsInCasket())
				gC_vs_ui.UnlockGear();		

			
			SetNextAction(ACTION_STAND);

			//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( action );
			m_ActionCountMax = GetCreatureActionCountMax( this, action );

			if (m_ActionCountMax==0)
			{
				
				m_Action = ((m_MoveDevice==MOVE_DEVICE_WALK)? ACTION_STAND : ACTION_SLAYER_MOTOR_STAND);
				//SetAction( ((m_MoveDevice==MOVE_DEVICE_WALK)? ACTION_STAND : ACTION_SLAYER_MOTOR_STAND) );
				m_ActionCount = 0;		
				m_RepeatCount = 0;
				//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
				m_ActionCountMax = GetCreatureActionCountMax( this, m_Action );
			}
			else
			{
				//---------------------------------------------
				
				//---------------------------------------------
				if (m_nUsedActionInfo!=ACTIONINFO_NULL 
					&& m_bRepeatAction 
					&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame())// m_WeaponSpeed ))
				{
					m_Action = action; 
					//SetAction( action );

					
					if (m_RepeatCount >= (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatLimit())
					{		
						m_RepeatCount = 0;

						
						m_ActionCount = 0;
					}
					else
					{
						//---------------------------------------------
						
						//---------------------------------------------
						BOOL bSlayer = IsSlayer();

						if (m_RepeatCount!=0 
							
							&& action!=ACTION_STAND 
							&& (!bSlayer || bSlayer && action!=ACTION_SLAYER_MOTOR_STAND))
						{
							
							//m_ActionCount = (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatStartFrame( m_WeaponSpeed );
							m_ActionCount = GetActionInfoRepeatStartFrame( m_nUsedActionInfo );
						}
						//---------------------------------------------
						
						//---------------------------------------------
						else
						{
							m_ActionCount = 0;				
						}
					}

					//m_ActionCountMax = (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatEndFrame( m_WeaponSpeed );			
					m_ActionCountMax = GetActionInfoRepeatEndFrame( m_nUsedActionInfo );
				}
				//---------------------------------------------
				
				//---------------------------------------------
				else
				{			
					if (m_ActionCount!=m_ActionCountMax
						&& (action==ACTION_STAND && m_Action==ACTION_STAND
							|| IsSlayer() && action==ACTION_SLAYER_MOTOR_STAND && m_Action==ACTION_SLAYER_MOTOR_STAND
							|| IsOusters() && action==ACTION_OUSTERS_FAST_MOVE_STAND && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND)
							)
					{
						
						//m_ActionCount = 0;
						//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
					}
					else
					{
						m_Action = action;
						//SetAction( action );

						
						m_ActionCount = 0;
						
							//g_pTopView->m_CreatureFPK[(*g_pCreatureTable)[m_CreatureType].FrameID]
							//												[m_Action][m_Direction].GetSize();
					}			
				}			
			}
		}		
	}


	
	
	if( HasEffectStatus( EFFECTSTATUS_REDIANCE ) )
	{
		if( 
			( m_Action == ACTION_SLAYER_SWORD_2 || m_Action == ACTION_SLAYER_SWORD_2_SLOW || m_Action == ACTION_SLAYER_SWORD_2_FAST ) 
			)
		{
			MAttachEffect *pEffect = CreateAttachEffect( EFFECTSPRITETYPE_REDIANCE_MALE_NORMAL, 0, 0 );
			if( pEffect != NULL )
			{
				pEffect->SetDirection( GetDirection() );
				pEffect->SetLink( ACTIONINFO_NULL, NULL );							
				pEffect->SetAttachCreatureID( GetID() );
			}
		} 
		else if(
			( m_Action == ACTION_SLAYER_SWORD || m_Action == ACTION_SLAYER_SWORD_FAST || m_Action == ACTION_SLAYER_SWORD_SLOW ) )
		{
			MAttachEffect *pEffect = CreateAttachEffect( EFFECTSPRITETYPE_REDIANCE_MALE_NORMAL_ATTACK_NORMAL, 0, 0 );
			if( pEffect != NULL )
			{
				pEffect->SetDirection( GetDirection() );
				pEffect->SetLink( ACTIONINFO_NULL, NULL );							
				pEffect->SetAttachCreatureID( GetID() );
			}
		}				
	}

	//------------------------------
	
	//------------------------------
	 
}

//----------------------------------------------------------------------
// Set AddonItem
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MPlayer::SetAddonItem( MItem* pItem )
{
	DEBUG_ADD("MPlayer::SetAddonItem");
	
	//-------------------------------------------------
	
	//-------------------------------------------------
	if (MCreatureWear::SetAddonItem( pItem ))
	{
		//-------------------------------------------------
		
		
		
		//-------------------------------------------------
		if (pItem->IsBasicWeapon())
		{
			//-------------------------------------------------
			
			
			
			//-------------------------------------------------
			//if (pItem->IsGunItem())
			//{
				SetBasicAttackDistance( pItem->GetReach() );
			//}
			//else
			//{								
			//	SetBasicAttackDistance( 1 );
			//}

			//-------------------------------------------------
			
			//-------------------------------------------------
			//(*g_pSkillAvailable).SetAvailableSkills();
		}

		return true;
	}

	return false;	
}

//----------------------------------------------------------------------
// Remove AddonItem
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MPlayer::RemoveAddonItem( MItem* pItem )
{
	DEBUG_ADD("MCreatureWear::RemoveAddonItem");
	
	//-------------------------------------------------
	
	//-------------------------------------------------
	if (MCreatureWear::RemoveAddonItem( pItem ))
	{
		//-------------------------------------------------
		
		
		
		//-------------------------------------------------
		if (pItem->IsBasicWeapon())
		{
			
			SetBasicActionInfo( SKILL_ATTACK_MELEE );


			
			SetBasicAttackDistance( 1 );

			//-------------------------------------------------
			
			//-------------------------------------------------
			//(*g_pSkillAvailable).SetAvailableSkills();
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Set Next Action
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void	
MPlayer::SetNextAction(BYTE action)
{ 	
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
//		return;
		action = ACTION_STAND;
	}

	if(action == ACTION_STAND)
	{
		action = ((m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND);
		if(IsOusters() && HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_FAST_MOVE_STAND;
	}
	else if(action == ACTION_MOVE)
		if(IsOusters() && HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_FAST_MOVE_MOVE;

	if (m_Action==ACTION_STAND || IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND) 
	{
		//SetAction( action );
		m_ActionCount = m_ActionCountMax;
	}
	//else
	//{
		m_NextAction = action;
	//}
}

//----------------------------------------------------------------------
// Set NextAction To Move
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::SetNextActionToMove()
{ 	
	if (m_Action==ACTION_STAND || IsOusters() && m_Action == ACTION_OUSTERS_FAST_MOVE_STAND
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND) 
	{
		//SetAction( action );
		m_ActionCount = m_ActionCountMax;
	}
	//else
	//{
		SetNextAction(m_MoveAction);
	//}
}

//----------------------------------------------------------------------
// Set Alive
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::SetAlive()
{
	m_bAlive = true;

	
	UI_UnlockItem();
	UI_UnlockGear();
	UI_UnlockItemTrade();

	SetAction( ACTION_STAND );

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	/*
	if (g_pEffectStatusTable!=NULL)
	{
		for (int e=0; e<g_pEffectStatusTable->GetSize(); e++)
		{
			RemoveEffectStatus( (EFFECTSTATUS)e );
		}
	}
	*/

	if (g_pTopView!=NULL)
	{
		g_pTopView->SetFadeEnd();
	}

	if (m_MoveType == CREATURE_FLYING)
	{
		m_Z=(*g_pCreatureTable)[m_CreatureType].FlyingHeight;
	}
}

//----------------------------------------------------------------------
// Set Dead
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void	
MPlayer::SetDead()
{
	
	if (m_bAlive)
	{
		if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
		{
			RemoveEffectStatus(EFFECTSTATUS_INSTALL_TURRET);
			SetAction( ACTION_STAND );
		}
		if(HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ) )
		{
			RemoveEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) );
			SetAction( ACTION_STAND );
		}
		if(HasEffectStatus(EFFECTSTATUS_GLACIER))
		{ 
			RemoveEffectStatus( EFFECTSTATUS_GLACIER );
			SetAction(ACTION_STAND);
			ClearAttachEffect();
		}
		g_pUIMessageManager->Execute(UI_FINISH_LEVELUP_BUTTON, 0, 0, NULL);
		
		StopDrain();
		StopAbsorb();

		
		UI_LockItem();
		UI_LockGear();
		UI_LockItemTrade();

		SetStop();	
//		StopCauseCriticalWounds();

		MovePosition( m_ServerX, m_ServerY );

		DEBUG_ADD_FORMAT("PlayerDeadPosition(%d, %d)", m_ServerX, m_ServerY);

		
		TraceNULL();
		
		
		m_nUsedActionInfo	= (*g_pCreatureTable)[m_CreatureType].DeadActionInfo;
		
		if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		{
			
			SetAction( GetActionInfoAction(m_nUsedActionInfo) );

			
			m_NextTraceID			= m_ID;
			m_NextTraceX			= m_X;
			m_NextTraceY			= m_Y;
			m_NextTraceZ			= 0;	

			SetTraceID( m_ID );
			m_TraceX			= m_X;
			m_TraceY			= m_Y;
			m_TraceZ			= 0;	
		}

		
		m_DeadDelayTime	= g_CurrentTime + g_pClientConfig->DELAY_PLAYER_DEAD;


		//-------------------------------------------------------
		
		//-------------------------------------------------------
		SetVisibleSoon();

		
		m_bNextAction = false;
		m_bKeepTraceCreature = false;
		SetNextAction(ACTION_STAND);
		UnSetRepeatAction();

		// HP
		StopRecoveryHP();
		StopRecoveryMP();
		SetStatus( MODIFY_CURRENT_HP, 0 );

		
		UnSetLockMode();

		
		//g_SDLMusic.SetCurrentTempo( g_SDLMusic.GetOriginalTempo() * 0.8f );

		m_Z = 0;
		
		UI_CloseSelectWayPoint();

		
		g_pUserInformation->LogoutTime = 0;	

		SetWaitVerifyNULL();

		g_pEventManager->RemoveEvent(EVENTID_LOGOUT);
		g_pEventManager->RemoveEvent(EVENTID_LOVECHAIN);

		
		m_bAlive = false;
		m_bInCasket = false;
	
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (g_pEffectStatusTable!=NULL)
		{
			for (int e=0; e<g_pEffectStatusTable->GetSize(); e++)
			{
				
				if (e!=EFFECTSTATUS_BLOOD_DRAIN && 
					
					e!=EFFECTSTATUS_SIEGE_ATTACKER_1 &&	
					e!=EFFECTSTATUS_SIEGE_ATTACKER_2 &&
					e!=EFFECTSTATUS_SIEGE_ATTACKER_3 &&
					e!=EFFECTSTATUS_SIEGE_ATTACKER_4 &&
					e!=EFFECTSTATUS_SIEGE_ATTACKER_5 &&
					// 2004, 11, 16, sobeit add end
					HasEffectStatus((EFFECTSTATUS)e))
				{
					RemoveEffectStatus( (EFFECTSTATUS)e );
				}
			}
		}
		ClearAttachEffect();

//		g_pTempInformation->SetMode( TempInformation::MODE_NULL );

		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		SendStatusInfoToParty();	

		
		ExecuteHelpEvent(HELP_EVENT_DIE);
		// 2004, 5, 7 sobeit add end
	}
}

//----------------------------------------------------------------------
// Check Repeat Action
//----------------------------------------------------------------------
bool
MPlayer::CheckRepeatAction()
{
	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	if (m_bKeepTraceCreature || IsRequestMode() || IsInDarkness())// || m_bTraceCreatureToForceAttack)
	{
		UnSetRepeatAction();
		return false;
	}

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	if (m_ActionCount>=m_ActionCountMax-1 
			|| m_Action==ACTION_STAND 
			|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND)
	{
		static DWORD repeatFrameIncTurn = 0;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_bRepeatAction)				
		{
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (m_fTraceBuffer & FLAG_TRACE_SELF)
			{
				//SelfSpecialAction();
				m_fNextTrace = m_fTraceBuffer;

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}

				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )

				return true;
			}				
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			else if (m_fTraceBuffer & FLAG_TRACE_CREATURE_BASIC)
			{	
				//TraceCreatureToBasicAction( m_TraceID, m_bTraceCreatureToForceAttack );
				m_fNextTrace = m_fTraceBuffer;

				m_NextTraceID  = m_TraceIDBuffer;				
				
				m_bNextForceAttack = m_bTraceCreatureToForceAttack;

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}

				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )

				return true;
			}				
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			else  if (m_fTraceBuffer & FLAG_TRACE_CREATURE_SPECIAL)
			{
				//TraceCreatureToSpecialAction( m_TraceID );
				m_fNextTrace = m_fTraceBuffer;

				m_NextTraceID = m_TraceIDBuffer;

				m_bNextForceAttack = m_bTraceCreatureToForceAttack;

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}
				
				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )				
				if (m_nUsedActionInfo < g_pActionInfoTable->GetSize())
				{
					
					if (m_RepeatCount >= (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatLimit())
					{		
						
						//m_RepeatCount = 0;

						BOOL bSlayer = IsSlayer();

						if ((*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame() //m_WeaponSpeed )
							&& m_Action!=ACTION_STAND 
							&& (!bSlayer || bSlayer && m_Action!=ACTION_SLAYER_MOTOR_STAND)
							&& m_ActionCountMax!=0 )	
						{	
							//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
							m_ActionCountMax = GetCreatureActionCountMax( this, m_Action );

							DEBUG_ADD_FORMAT("unsetrepeat: actionCount=%d / %d", m_ActionCount, m_ActionCountMax);		
						}						
					}												
				}
				
				return true;
			}
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			else if (m_fTraceBuffer & FLAG_TRACE_SECTOR_SPECIAL)
			{
				//TraceSectorToSpecialAction( m_TraceX, m_TraceY );
				m_fNextTrace = m_fTraceBuffer;

				//m_NextTraceX = m_TraceX;
				//m_NextTraceY = m_TraceY;

				//------------------------------------------------------------
				
				//------------------------------------------------------------
				POINT temp = g_pTopView->ScreenToPixel(g_x, g_y);
				temp = MTopView::PixelToMap( temp.x, temp.y );
				
				m_NextTraceX = temp.x;
				m_NextTraceY = temp.y;

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}
				
				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )

				return true;
			}		
		}
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		else
		{
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (m_fNextTrace!=FLAG_TRACE_NULL)
			{
				//m_fTraceBuffer = m_fNextTrace;
				return true;
			}			
		}
	}
	else
	{
	}

	return false;
}

//----------------------------------------------------------------------
// Check BufferAction
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool
MPlayer::CheckBufferAction()
{
	//DEBUG_ADD("Before CheckBufferAction");
	
	//------------------------------------------		
	
	//------------------------------------------
	if (m_fNextTrace != FLAG_TRACE_NULL)
	{			
		//SetAction( (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );

		//---------------------
		
		//---------------------
		if (m_fNextTrace & FLAG_TRACE_SELF)
		{
			SelfSpecialAction();
		}				
		//---------------------
		
		//---------------------
		if (m_fNextTrace & FLAG_TRACE_CREATURE_BASIC)
		{
			TraceCreatureToBasicAction( m_NextTraceID, m_bNextForceAttack );
		}				
		//---------------------
		
		//---------------------
		else  if (m_fNextTrace & FLAG_TRACE_CREATURE_SPECIAL)
		{
			TraceCreatureToSpecialAction( m_NextTraceID, m_bNextForceAttack );
		}
		//---------------------
		
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_SECTOR_BASIC)
		{
			TraceSectorToBasicAction( m_NextTraceX, m_NextTraceY );
		}
		//---------------------
		
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_SECTOR_SPECIAL)
		{
			TraceSectorToSpecialAction( m_NextTraceX, m_NextTraceY );
		}
		//---------------------
		
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_ITEM)
		{
			TraceItem( m_NextTraceID );
		}
		//---------------------
		
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_INVENTORY)
		{
			TraceInventoryItem( m_NextTraceID );
		}
		//---------------------
		
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_EFFECT)
		{
			TraceEffect( m_NextTraceID );
		}
		//---------------------
		
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_INTERACTIONOBJECT)
		{
			TraceInteractionObject( m_NextTraceID, m_NextTraceObjectAction );
		}

		//DEBUG_ADD("After CheckBufferAction : true");
		
		return true;
	}

	//------------------------------------------		
	
	//------------------------------------------
	BOOL bSlayer = IsSlayer();

	if (m_bNextAction || 
		m_NextAction!=ACTION_STAND 
		&& (!IsOusters() || IsOusters() && m_NextAction!=ACTION_OUSTERS_FAST_MOVE_STAND)// && m_fNextTrace==FLAG_TRACE_NULL)
		&& (!bSlayer || bSlayer && m_NextAction!=ACTION_SLAYER_MOTOR_STAND))// && m_fNextTrace==FLAG_TRACE_NULL)
	{
		SetAction( m_NextAction );	

		DEBUG_ADD("After CheckBufferAction : true - SetAction");
		
		return true;
	}	

	//DEBUG_ADD("After CheckBufferAction - false");
	
	return false;
}

//----------------------------------------------------------------------
// UnSet Repeat Action
//----------------------------------------------------------------------
void			
MPlayer::UnSetRepeatAction()				
{ 
	

	m_bRepeatAction = FALSE; 

	m_RepeatCount = 0;

	m_nNextUsedActionInfo = ACTIONINFO_NULL;

	BOOL bSlayer = IsSlayer();

	//m_fTraceBuffer = FLAG_TRACE_NULL;
	/*
	#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
			{
				if (m_nUsedActionInfo != ACTIONINFO_NULL)
				{
					DEBUG_ADD_FORMAT("UnSetRepeatAction:ai=%d, ir=%d, at=%d, sm=%d, acm=%d",
			
					(int)m_nUsedActionInfo, 
					(int)(*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame(),
					(int)m_Action!=ACTION_STAND,
					(int)(!bSlayer || bSlayer && m_Action!=ACTION_SLAYER_MOTOR_STAND),
					(int)m_ActionCountMax!=0);
				}
				else
				{
					DEBUG_ADD("UnSetRepeatAction: m_nUsedActionInfo is ACTIONINFO_NULL");
				}
			}
	#endif
	*/

	if (m_nUsedActionInfo!=ACTIONINFO_NULL 
		&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame() //m_WeaponSpeed )
		&& m_Action!=ACTION_STAND 
		&& (!bSlayer || bSlayer && m_Action!=ACTION_SLAYER_MOTOR_STAND)
		&& m_ActionCountMax!=0 )	
	{	
		//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );		
		m_ActionCountMax = GetCreatureActionCountMax( this, m_Action );

		DEBUG_ADD_FORMAT("unsetrepeat: actionCount=%d / %d", m_ActionCount, m_ActionCountMax);		
	}

	//m_fNextTrace = FLAG_TRACE_NULL; 
}

//----------------------------------------------------------------------
// Add EffectStatus
//----------------------------------------------------------------------
bool		
MPlayer::AddEffectStatus(EFFECTSTATUS status, DWORD delayFrame)
{	
	__BEGIN_HELP_EVENT
		switch (status)
		{		
		case EFFECTSTATUS_SUMMON_SYLPH_GREEN :
		case EFFECTSTATUS_SUMMON_SYLPH_ORANGE :
		case EFFECTSTATUS_SUMMON_SYLPH_RED :
		case EFFECTSTATUS_SUMMON_SYLPH_YELLOW :
		case EFFECTSTATUS_SUMMON_SYLPH_WHITE :
		case EFFECTSTATUS_SUMMON_SYLPH_BROWN :
		case EFFECTSTATUS_SUMMON_SYLPH_LILAC :
		case EFFECTSTATUS_SUMMON_SYLPH_BLACK :
		case EFFECTSTATUS_SUMMON_SYLPH:
			SetMoveDevice(MOVE_DEVICE_SUMMON_SYLPH);
			break;

			//--------------------------------------------------
			// Bless
			//--------------------------------------------------
			case EFFECTSTATUS_BLESS :
				
//				ExecuteHelpEvent( HE_EFFECT_BLESS );
			break;

			//--------------------------------------------------
			// Striking
			//--------------------------------------------------
			case EFFECTSTATUS_STRIKING :
				
//				ExecuteHelpEvent( HE_EFFECT_STRIKING );
			break;

			//--------------------------------------------------
			// Green Poison
			//--------------------------------------------------
			case EFFECTSTATUS_GREEN_POISON :
			case EFFECTSTATUS_POISON :
				
//				ExecuteHelpEvent( HE_EFFECT_GREEN_POISON );
			break;

			//--------------------------------------------------
			// Purple Wall
			//--------------------------------------------------
			//case EFFECTSTATUS_PURPLE_WALL :
			case EFFECTSTATUS_CURSE_PARALYSIS :
				
//				ExecuteHelpEvent( HE_EFFECT_PURPLE_WALL );
			break;

			//--------------------------------------------------
			// Yellow Poison
			//--------------------------------------------------
			case EFFECTSTATUS_YELLOW_POISON :
			case EFFECTSTATUS_YELLOW_POISON_TO_CREATURE :
				
//				ExecuteHelpEvent( HE_EFFECT_YELLOW_POISON );
			break;
		
			//--------------------------------------------------
			// BLOOD DRAIN 
			//--------------------------------------------------
			case EFFECTSTATUS_BLOOD_DRAIN :
			{
				
				ExecuteHelpEvent( HELP_EVENT_DRAIN_BLOOD );				
				// 2004, 5, 6 sobeit add end
			}
			break;		

			//--------------------------------------------------
			
			//--------------------------------------------------
			case EFFECTSTATUS_OBSERVING_EYE :
				
				if (!m_bEffectStatus[EFFECTSTATUS_OBSERVING_EYE])
				{
					CalculateLightSight();
				}
			break;

			case EFFECTSTATUS_GHOST:
				SetFlyingCreature();
//				g_pUserInformation->bCompetence = true;
			break;

		}
	__END_HELP_EVENT

	//--------------------------------------------------
	
	//--------------------------------------------------
	switch (status)
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		case EFFECTSTATUS_BLOOD_DRAIN :
		{
			DWORD conversionTime = delayFrame * 1000 / 16;// / g_pClientConfig->FPS;

			SetConversionDelay( conversionTime );

			DEBUG_ADD_FORMAT( "[BloodDrained] %ld --> %ld", g_CurrentTime, m_ConversionDelayTime );

//			delayFrame *= 16;
//			UI_SetHP( GetHP(), GetMAX_HP() );
		}
		break;

		//--------------------------------------------------
		// hallu
		//--------------------------------------------------
		case EFFECTSTATUS_HALLUCINATION :
			g_pZone->ChangeToHalluCreature();			
		break;

		case EFFECTSTATUS_LIGHTNESS :
		case EFFECTSTATUS_YELLOW_POISON :
		case EFFECTSTATUS_FLARE :
		case EFFECTSTATUS_YELLOW_POISON_TO_CREATURE :
//		case EFFECTSTATUS_BLINDNESS:
			CalculateSight();
			break;
		case EFFECTSTATUS_CAN_ENTER_GDR_LAIR:
			delayFrame = delayFrame*10;
			break;
		case EFFECTSTATUS_INSTALL_TURRET:
//			g_pSkillAvailable->SetAvailableSkills();
			SetDelay(1000);
			gC_vs_ui.UnselectSkill();
			//gC_vs_ui.SelectSkill( SKILL_TURRET_FIRE );
			break;

	}

	bool re = MCreature::AddEffectStatus( status, delayFrame );
	//--------------------------------------------------
	
	//--------------------------------------------------
	if(!(IsSlayer() && status == EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS))
		UI_AddEffectStatus( status, delayFrame );
	
	if(status == EFFECTSTATUS_INSTALL_TURRET)
	{
		g_pSkillAvailable->SetAvailableSkills();
		gC_vs_ui.SelectSkill( SKILL_TURRET_FIRE );
	}
	CheckRegen();

	return re; 
}

//----------------------------------------------------------------------
// Remove EffectStatus
//----------------------------------------------------------------------
bool		
MPlayer::RemoveEffectStatus(EFFECTSTATUS status)
{
	bool bOK = MCreatureWear::RemoveEffectStatus( status );

	//--------------------------------------------------
	
	//--------------------------------------------------
	UI_RemoveEffectStatus( status );

	switch (status)
	{		
	case EFFECTSTATUS_SUMMON_SYLPH_GREEN :
	case EFFECTSTATUS_SUMMON_SYLPH_ORANGE :
	case EFFECTSTATUS_SUMMON_SYLPH_RED :
	case EFFECTSTATUS_SUMMON_SYLPH_YELLOW :
	case EFFECTSTATUS_SUMMON_SYLPH_WHITE :
	case EFFECTSTATUS_SUMMON_SYLPH_BROWN :
	case EFFECTSTATUS_SUMMON_SYLPH_LILAC :
	case EFFECTSTATUS_SUMMON_SYLPH_BLACK :
	case EFFECTSTATUS_SUMMON_SYLPH:
		SetMoveDevice(MOVE_DEVICE_WALK);
		if(GetWaitVerify() == MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF)
			SetWaitVerifyNULL();
		
		 
		if(IsAdvancementClass() && status==EFFECTSTATUS_SUMMON_SYLPH)
		{
				m_bEffectStatus[status]=true;
				bool re = MCreature::RemoveEffectStatus( status );
		}
		break;
		/*   end    */
		
		//--------------------------------------------------
		// hallu
		//--------------------------------------------------
		case EFFECTSTATUS_HALLUCINATION :
			g_pZone->RemoveHalluCreature();			
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------
		case EFFECTSTATUS_BLOOD_DRAIN :
			UnSetConversionDelay();

			UI_SetHP( GetHP(), GetMAX_HP() );		
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------		
		case EFFECTSTATUS_COMA :
			
			if (g_pTempInformation->GetMode()==TempInformation::MODE_WAIT_RESURRECT)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}

			if (GetWaitVerify()==WAIT_VERIFY_RESURRECT)
			{
				SetWaitVerifyNULL();
			}

			gC_vs_ui.FinishRequestDie();
			gC_vs_ui.FinishRequestResurrect();
			gC_vs_ui.CloseRequestShrineMinimap();
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------
		case EFFECTSTATUS_OBSERVING_EYE :
			CalculateStatus();
		break;

		case EFFECTSTATUS_GHOST:
			if (!(*g_pCreatureTable)[m_CreatureType].bFlyingCreature)	
				SetGroundCreature();
			break;
		case EFFECTSTATUS_WILL_OF_LIFE :
			if(IsVampire() )
				MCreature::CheckRegen();
			break;
		case EFFECTSTATUS_FLARE :
		case EFFECTSTATUS_YELLOW_POISON :
		case EFFECTSTATUS_YELLOW_POISON_TO_CREATURE :
		case EFFECTSTATUS_LIGHTNESS :
			// 2004, 6, 21 sobeit add start - about blindness
//		case EFFECTSTATUS_BLINDNESS:
			// 2004, 6, 21 sobeit add end - about blindness
			if( bOK )
				CalculateSight();
			break;
//		case EFFECTSTATUS_GLACIER:
//			if (m_bAlive)
//			{
//				SetStop();
//				StopCauseCriticalWounds();
//			}			
//		break;
		case EFFECTSTATUS_INSTALL_TURRET:
			g_pSkillAvailable->SetAvailableSkills();
			ExecuteActionInfoFromMainNode(RESULT_SKILL_INSTALL_TURRET,GetX(), GetY(), 0,0,GetID()	,	
				GetX(), GetY(), 0, 0, NULL, false);
			SetDelay(1000);
			break;
	}
	
	return bOK;
}

//----------------------------------------------------------------------
// Set CreatureType
//----------------------------------------------------------------------
void	
MPlayer::SetCreatureType(TYPE_CREATURETYPE type)
{
	//add by viva 
//	type = 808; 
	MCreatureWear::SetCreatureType(type);

	//---------------------------------------------------
	
	//---------------------------------------------------
	if (g_pSkillAvailable!=NULL)
	{
		g_pSkillAvailable->SetAvailableSkills();		
	}	
	if(type==CREATURETYPE_SLAYER_OPERATOR
		|| type==CREATURETYPE_VAMPIRE_OPERATOR
		|| type==CREATURETYPE_OUSTERS_OPERATOR)
		g_pUserInformation->bCompetenceShape = true;
	else
		g_pUserInformation->bCompetenceShape = false;
}

//----------------------------------------------------------------------
// UpdateConversionTime
//----------------------------------------------------------------------
void
MPlayer::UpdateConversionTime()
{
	//--------------------------------------------------------
	
	
	//--------------------------------------------------------

	
	
	if (IsSlayer())
	{
		if ((int)m_ConversionDelayTime > (int)g_CurrentTime)
		{
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			
			int changeTime = ((int)m_ConversionDelayTime - (int)g_CurrentTime)/* * g_pGameTime->GetTimeRatio()*/ / 60000 ;
			g_char_slot_ingame.CHANGE_VAMPIRE = changeTime;

			static DWORD enableBlinkTime = 0;	
			
			//const DWORD sixHour = 6*60*60*1000;

			
			const int maxBlink = 12;
			const DWORD blinkValue[maxBlink][2] = 
			{
				{ 4*60*60*1000, 6*60*1000 },	
				{ 3*60*60*1000, 4*60*1000 },	
				{ 2*60*60*1000, 3*60*1000 },	
				{ 1*60*60*1000, 2*60*1000 },	
				{ 1*60*60*1000, 1*60*1000 },	
				{ 3*60*1000, 30*1000 },			
				{ 2*60*1000, 10*1000 },			
				{ 1*60*1000, 5*1000 },			
				{ 30*60*1000, 3*1000 },			
				{ 10*1000, 2*1000 },			
				{ 10*1000, 2*1000 },			
				{ 0*1000, 300 }					
			};

			//--------------------------------------------------------
			
			
			//--------------------------------------------------------
			if (g_CurrentTime > enableBlinkTime)
			{
				
				DWORD timeGap = m_ConversionDelayTime - g_CurrentTime;
				
				
				if (1)//timeGap < sixHour)
				{
					BOOL bBlink = FALSE;

					for (int i=0; i<maxBlink; i++)
					{
						if (timeGap > blinkValue[i][0])
						{
							g_pTopView->SetFadeStart(25, 31, 2, 31,0,0);

							
							enableBlinkTime = g_CurrentTime + blinkValue[i][1];

							bBlink = TRUE;

							break;
						}
					}

					if (!bBlink)
					{
						UnSetConversionDelay();
					}
				}
			}
		}
		else
		{
			m_ConversionDelayTime = 0;

			//--------------------------------------------------------
			
			//--------------------------------------------------------
			g_char_slot_ingame.CHANGE_VAMPIRE = 0;
		}
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::Action()
{	
	if( !HasEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL ) && !HasEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL ) && GetElementalID() != OBJECTID_NULL && g_pZone != NULL)
	{
		g_pZone->RemoveFakeCreature(GetElementalID());
	}

	
	if( GetPetID() != OBJECTID_NULL)
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(GetPetID());
		if(pFakeCreature != NULL)
		{
			MPetItem *pPetItem = pFakeCreature->GetPetItem();
			if(pPetItem != NULL)
			{
				const int petLevelMax = 50;
				int petLevel = pPetItem->GetNumber();
				int petDelayTime = 0;	
				TYPE_ITEMTYPE tmpItemType = pPetItem->GetItemType();
				
				if(tmpItemType == 6 || tmpItemType == 7 || tmpItemType == 8)
				{
					petDelayTime = 1200 + ((petLevelMax-petLevel)*120)/petLevelMax;
				}
				// end
				else if(pPetItem->GetItemType()>2)
					petDelayTime = 3200+((petLevelMax-petLevel)*2800)/petLevelMax;
				else
					petDelayTime = 1200+((petLevelMax-petLevel)*2800)/petLevelMax;

				if(m_PetDelayTime+petDelayTime < GetTickCount() )
				{
					int itemLimit = 1;	

					if(pPetItem->IsCanCutHead())	
						itemLimit = 0;

					MItem *pItem = g_pZone->GetCorpseKilledByMe(itemLimit);

					if(pItem != NULL)
					{
						CGDissectionCorpse _CGDissectionCorpse;
						_CGDissectionCorpse.setX( pItem->GetX() );
						_CGDissectionCorpse.setY( pItem->GetY() );
						_CGDissectionCorpse.setPet( true );
						_CGDissectionCorpse.setObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGDissectionCorpse );

						
						DEBUG_ADD_FORMAT("[PETPET] itemLimit %d", itemLimit);
						pItem->SetNumber( itemLimit );
					}
					
					m_PetDelayTime = GetTickCount();
				}
			}
		}
	}

	
	if( m_nUsedActionInfo == SKILL_BLITZ_SLIDING_ATTACK || m_nUsedActionInfo == SKILL_BLAZE_WALK_ATTACK )
		UnSetRepeatAction();

	if(IsRepeatAction())
	{
		if(m_RepeatTimer + g_pClientConfig->REPEAT_TIME < GetTickCount())
			UnSetRepeatAction();
	}
	
	if(IsLockMode())
	{
		if(m_LockTimer + g_pClientConfig->LOCK_TIME < GetTickCount())
			UnSetLockMode();
	}
	//if (m_ActionCount==m_ActionCountMax)
	//{
	//	int a =0;
	//}
	
	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	SendStatusInfoToParty();
	
#ifdef OUTPUT_DEBUG_PLAYER_ACTION
	DEBUG_ADD_FORMAT("[MPLayer::Action] Action[%d]=%d/%d. Move=%d/%d"
					, m_Action, m_ActionCount, m_ActionCountMax, m_MoveCount, m_MoveCountMax);	
#endif
	
#ifdef __METROTECH_TEST__
	if(!IsVampire())
	{
		if (g_iAutoHealPotion == 1 && m_RecoveryHPTimes==0)	AutoUsePotion(0);		// HP
		if (g_bAutoManaPotion && m_RecoveryMPTimes==0)	AutoUsePotion(1);		// MP
		if (g_iAutoReload)	AutoUsePotion(2+g_iAutoReload-1);					// Magazine
//		if (g_iAutoHealPotion == 2 || g_iAutoHealPotion == 3)  AutoUseSkill(0);									// Healing
//		if (g_bHolyWater)  AutoUseSkill(1);									// CreateHolyWater
	}
#endif
	 

	#ifdef OUTPUT_DEBUG
		if (m_ActionCount > m_ActionCountMax)
		{
			DEBUG_ADD("So Terrible!");
		}
	#endif

	//--------------------------------------------------------
	
	//--------------------------------------------------------
//	__BEGIN_HELP_EVENT

//		if (g_CurrentTime > nextHelpTime)
//		{
//			ExecuteHelpEvent( HE_TIME_PASSED );
//

//		}
//	__END_HELP_EVENT

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	UpdateStatus();	
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	UpdateConversionTime();
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (m_NextChatFadeTime < g_CurrentTime)
	{
		FadeChatString();
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if(UpDateInstallTurret())
		return;
	UpdateInvisible();
	UpdateCasket();
	UpdateCutHeight();
	UpdateTurning();
	UpdateCauseCriticalWounds();
//	UpdateBloodyZenith();
	UpdateGunShotGuidance();
	UpdateFakeDie();
	UpdateGhost();

	UpdateDarkness();

	UpdateSweepVice();
	UpdateBurningSol();		

	UpdateBikeCrash();
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	ChangeNearDirection();
	//m_CurrentDirection = m_Direction;

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("beforeCheckListEffect");
	#endif
	
	if (m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo())
	{		
		int numTarget = m_listEffectTarget.size();

		if (numTarget!=0)
		{
			//------------------------------------------------------------
			
			
			//------------------------------------------------------------
			BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND);
			//int		actionCountMax_1 = m_ActionCountMax-1;
			//BOOL	bEndAction		= (m_ActionCount==actionCountMax_1) || bActionStand;
			int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

			BOOL	bStartResult	= bActionStand 
										|| (m_ActionCount >= StartFrame)
										|| m_ActionCount >= m_ActionCountMax;

			bStartResult |= m_bRepeatAction 
							&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame()
							&& m_RepeatCount!=0
							&& m_ActionCount >= GetActionInfoRepeatStartFrame( m_nUsedActionInfo );

			
			if (bStartResult)
			{
				EFFECTTARGET_LIST::iterator iEffectTarget = m_listEffectTarget.begin();
		
				while (iEffectTarget != m_listEffectTarget.end())
				{
					MEffectTarget* pEffectTarget = *iEffectTarget;

					if (pEffectTarget->IsResultTime())// || pEffectTarget->IsEnd())
					{
						MActionResult* pResult = pEffectTarget->GetResult();

						if (pResult!=NULL)
						{
							
							int numTargetBeforeExecute = m_listEffectTarget.size();

							pEffectTarget->SetResultNULL();

							pResult->Execute();

							delete pResult;

							int numTargetAfterExecute = m_listEffectTarget.size();
							
							
							
							if (numTargetBeforeExecute!=numTargetAfterExecute)
							{
								
								iEffectTarget = m_listEffectTarget.begin();
								continue;
							}

							
							
							
							
							//delete pEffectTarget;
						}
					}

					iEffectTarget++;
				}		

				m_listEffectTarget.clear();
			}
		}
	}

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("bcleok");
	#endif

	//--------------------------------------------------------
	
	//--------------------------------------------------------	
	if (!m_bTurning	
		&& !HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS)
		&& !IsCauseCriticalWounds()
//		&& !IsBloodyZenith()
		&& !IsGunShotGuidance()
		&& !HasEffectStatus(EFFECTSTATUS_SOUL_CHAIN)
		&& !HasEffectStatus(EFFECTSTATUS_TRAP_TRIGGERED)
		&& !HasEffectStatus(EFFECTSTATUS_TRAPPED) 
//		&& g_pUserInformation->LogoutTime == 0
		|| g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		|| g_bLight
#endif
		)// && !HasEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS))	//IsNotDelay())
	{
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		/*
		if (m_nNextUsedActionInfo!=ACTIONINFO_NULL
			&& m_nUsedActionInfo==ACTIONINFO_NULL)
		{
			m_nUsedActionInfo = m_nNextUsedActionInfo;
			m_nNextUsedActionInfo = ACTIONINFO_NULL;
		}
		*/
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (HasEffectStatus( EFFECTSTATUS_HAS_SLAYER_RELIC )       || HasEffectStatus( EFFECTSTATUS_HAS_VAMPIRE_RELIC ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_GREGORI )|| HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_NEMA ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_LEGIOS ) || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_MIHOLE ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_AROSA )  || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_ARMEGA ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_INI )    || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_JAVE ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_CHASPA ) || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_CONCILIA ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_KIRO )   || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_HILLEL ) ||
			HasEffectStatus( EFFECTSTATUS_AMBER_OF_GUARD ) || HasEffectStatus( EFFECTSTATUS_AMBER_OF_AVENGER ) ||
			HasEffectStatus( EFFECTSTATUS_AMBER_OF_IMMORTAL ) || HasEffectStatus( EFFECTSTATUS_AMBER_OF_CURSE ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_CASTLE_SYMBOL_5 ) || HasEffectStatus( EFFECTSTATUS_HAS_CASTLE_SYMBOL_6 ) ||
			IsCreatureMove( dynamic_cast<MCreature*>(this) ) && HasEffectStatus( EFFECTSTATUS_ICE_FIELD_TO_CREATURE ) ||
			IsCreatureMove( dynamic_cast<MCreature*>(this) ) && HasEffectStatus( EFFECTSTATUS_FROZEN_ARMOR_TO_ENEMY ) ||
			IsCreatureActionAttack( this ) && (	HasEffectStatus( EFFECTSTATUS_ICE_OF_SOUL_STONE ) || (IsInFuryOfGnome() && !IsOusters()/*(IsSlayer() || IsVampire())*/)	) || 
			IsCreatureMove( dynamic_cast<MCreature*>(this) ) && HasEffectStatus( EFFECTSTATUS_JABBING_VEIN ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_2 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_3 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_4 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_5 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_6 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_7 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_8 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_9 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_10 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_11 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_12 )
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )			
#ifdef __METROTECH_TEST__
			&& !g_bLight
#endif
			)			
		{
			if (g_CurrentFrame & 0x01)
			{
				return;
			}
		}

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (m_bKnockBack > 0)
		{
			m_sX += m_cX;
			m_sY += m_cY;

			
			if (--m_bKnockBack==0)
			{
				m_sX = 0;
				m_sY = 0;

				SetAction( ACTION_STAND );

				return;
			}			
		}
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		else if (!m_bFastMove)
		{
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			if (m_bLockMode && !IsInDarkness())
			{
				int creatureID = FindEnemy();

				//--------------------------------------------------------
				
				//--------------------------------------------------------
				if (creatureID == OBJECTID_NULL)
				{
					m_listDirection.clear();

					UnSetRepeatAction();
					TraceNULL();
					m_fTraceBuffer	= FLAG_TRACE_NULL;	
					m_fNextTrace	= FLAG_TRACE_NULL;

					
					m_DestX		= SECTORPOSITION_NULL;
					m_DestY		= SECTORPOSITION_NULL;
					m_NextDestX	= SECTORPOSITION_NULL;
					m_NextDestY	= SECTORPOSITION_NULL;

					g_pTopView->SetSelectedNULL();
				}
				//--------------------------------------------------------
				
				//--------------------------------------------------------
				else
				{
					int bBasicAction = g_bLButtonDown;
					int bSpecialAction = g_bRButtonDown;

					//---------------------------------------------------------------
					
					//-------------------------------------------------------------
					if (bBasicAction)
					{
						if (TraceCreatureToBasicAction( 
									creatureID, 
									true ))		
						{
							//----------------------------------
							
							//----------------------------------
							//if (m_nBasicActionInfo!=ACTIONINFO_NULL 
							//	&& (*g_pActionInfoTable)[m_nBasicActionInfo].IsUseRepeatFrame()
							//	&& !IsRepeatAction())
							//{
							//	SetRepeatAction();
							//}

							g_pTopView->SetSelectedCreature( creatureID );			
							gpC_mouse_pointer->SetCursorAttack();					

							//g_bPreviousMove = false;
						}
					}
					//-------------------------------------------------------------
					
					//-------------------------------------------------------------
					else if (bSpecialAction)
					{
						if (TraceCreatureToSpecialAction( creatureID, true ))
						{
							//----------------------------------
							
							//----------------------------------
							if (m_nSpecialActionInfo!=ACTIONINFO_NULL 
								&& (*g_pActionInfoTable)[m_nSpecialActionInfo].IsUseRepeatFrame()
								&& !IsRepeatAction()
								)
							{
								SetRepeatAction();
							}							
							
							g_pTopView->SetSelectedCreature( creatureID );			
							gpC_mouse_pointer->SetCursorAttack();					

							//g_bPreviousMove = false;
						}
					}					

					
					g_pTopView->SetSelectedSectorNULL();
				}
				
			}

			//--------------------------------------------------------
			
			//--------------------------------------------------------
			if (m_bAlive)
			{
				CheckDropBlood();
			}

			
			//m_DelayTime	= 0;
			//----------------------------------------------------
			
			
			//----------------------------------------------------
			if (m_bKeepTraceCreature && (m_fTrace & FLAG_TRACE_CREATURE_BASIC))
			{							
				KeepTraceCreature();		
			}
			
			//------------------------------------------		
			
			//------------------------------------------
			BOOL bSlayer = IsSlayer();
			if (m_bNextAction ||
				(!IsOusters() || IsOusters() && m_NextAction!=ACTION_OUSTERS_FAST_MOVE_STAND )
				&& m_NextAction!=ACTION_STAND 
				&& (!bSlayer || bSlayer && m_NextAction!=ACTION_SLAYER_MOTOR_STAND)
				&& m_NextAction!=m_MoveAction)
			{
				SetAction( m_NextAction );
				
				#ifdef OUTPUT_DEBUG
					if (m_ActionCount > m_ActionCountMax)
					{
						DEBUG_ADD("So Terrible! return1");
					}
				#endif

				return;
			}

			//------------------------------------------
			// [ TEST CODE ]
			//------------------------------------------
			
			
			
			
			if (m_NextAction==m_MoveAction &&
				m_fTrace != FLAG_TRACE_NULL &&
				(max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY))) <= m_TraceDistance)
			{
				SetAction( m_NextAction );
			}

			//------------------------------------------
			
			//------------------------------------------
			if (m_ActionCount < m_ActionCountMax)
			{		
				if (m_nUsedActionInfo!=ACTIONINFO_NULL)
				{
					ActionEffect();
				}
				else
				{
					//m_ActionCount--;

					
					//switch (m_Action)
					//

					BOOL bSlayer = IsSlayer();

						
					if (m_Action==ACTION_STAND || IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_MOVE
						|| bSlayer && m_Action==ACTION_SLAYER_MOTOR_STAND)
					{
												
						m_ActionCount++;						

						//------------------------------------------------------------
						
						//------------------------------------------------------------
						if (CheckRepeatAction())
						{
							if (CheckBufferAction())
							{
								if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
								{
									
									m_nUsedActionInfo = m_nNextUsedActionInfo;
									m_nNextUsedActionInfo = ACTIONINFO_NULL;							

									if (ActionInTraceDistance())
									{
										if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
										{
											
											m_nUsedActionInfo = m_nNextUsedActionInfo;
											m_nNextUsedActionInfo = ACTIONINFO_NULL;
										}
										else
										{
											
											m_listDirection.clear();

											
											m_MoveCount = m_MoveCountMax;

											
											SetDirectionToPosition( m_TraceX, m_TraceY );

											SetAction( m_NextAction );

											//ActionToSendPacket();
										}
									}
								}
								#ifdef OUTPUT_DEBUG
									if (m_ActionCount > m_ActionCountMax)
									{
										DEBUG_ADD("So Terrible! return2");
									}
								#endif

								return;
							}
						}					

						 					
					}
					else
					{
					//default :
						if (m_nUsedActionInfo==ACTIONINFO_NULL)
						{	
							m_ActionCount++;

							if (m_bStopBloodDrain)
							{
								StopBloodDrain();
							}							
							if (m_bStopAbsorbSoul)
							{
								StopAbsorbSoul();
							}							
						}
						else
						{
							ActionEffect();
						}
					}
				}
			}
		}
		


		//------------------------------------------
		
		//------------------------------------------
		if (m_bAlive)
		{
			
			if (m_ActionCount>=m_ActionCountMax) //if (IsStop())
			{
				//--------------------------------------------------------
				
				
				
				//--------------------------------------------------------
				
				if (m_MoveCount>=m_MoveCountMax)
				{					
					//if (m_bRepeatAction && m_RepeatCount!=0 && (*g_pActionInfoTable)[m_nUsedActionInfo].IsRepeatFrame())
					//{						
					//	int a =0;
					//}
					//------------------------------------------------------------
					
					//------------------------------------------------------------
					//if (
						bool bCheckRepeat = CheckRepeatAction();
						//)
					//{
					//}

					if ((!m_bRepeatAction || !bCheckRepeat)
						&& m_fNextTrace != FLAG_TRACE_NULL)
					{			
						if (m_ActionCount>=m_ActionCountMax)
						{
							SetAction( ACTION_STAND );
						}
					}

					if (CheckBufferAction()) 
					{
						/*
						if (m_NextAction==m_MoveAction &&
							//m_fTrace != FLAG_TRACE_NULL &&
							(max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY))) <= m_TraceDistance)
						{
							SetAction( m_NextAction );
						}
						*/
						if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
						{
							
							m_nUsedActionInfo = m_nNextUsedActionInfo;
							m_nNextUsedActionInfo = ACTIONINFO_NULL;							

							if (ActionInTraceDistance())
							{
								if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
								{
									
									m_nUsedActionInfo = m_nNextUsedActionInfo;
									m_nNextUsedActionInfo = ACTIONINFO_NULL;
								}
								else
								{
									
									m_listDirection.clear();

									
									m_MoveCount = m_MoveCountMax;

									
									SetDirectionToPosition( m_TraceX, m_TraceY );

									SetAction( m_NextAction );

									//ActionToSendPacket();
								}
							}							
						}

						#ifdef OUTPUT_DEBUG
							if (m_ActionCount > m_ActionCountMax)
							{
								DEBUG_ADD("So Terrible! return3");
							}
						#endif

						return;
					}

					//------------------------------------------
					
					//------------------------------------------
					if (m_bRepeatAction)// && m_RepeatCount!=0)// && (*g_pActionInfoTable)[m_nUsedActionInfo].IsRepeatFrame())
					{						
					}
					else
					{
						SetAction( ACTION_STAND );
					}
					
				}
				//--------------------------------------------------------
				
				
				//--------------------------------------------------------					
				else
				{			
					m_Action = m_MoveAction;
					ActionMove();

					//----------------------------------------------------
					
					
					//----------------------------------------------------
					if (m_MoveCount==m_MoveCountMax)
					{		
						if (m_fNextTrace != FLAG_TRACE_NULL)
						{		
							if (m_ActionCount>=m_ActionCountMax)
							{
								SetAction( ACTION_STAND );
							}
						}

						if (CheckBufferAction()) 
						{
							if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
							{
								
								m_nUsedActionInfo = m_nNextUsedActionInfo;
								m_nNextUsedActionInfo = ACTIONINFO_NULL;							

								if (ActionInTraceDistance())
								{
									if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
									{
										
										m_nUsedActionInfo = m_nNextUsedActionInfo;
										m_nNextUsedActionInfo = ACTIONINFO_NULL;
									}
									else
									{
										
										m_listDirection.clear();

										
										m_MoveCount = m_MoveCountMax;

										
										SetDirectionToPosition( m_TraceX, m_TraceY );

										SetAction( m_NextAction );

										//ActionToSendPacket();
									}
								}
							}

							#ifdef OUTPUT_DEBUG
								if (m_ActionCount > m_ActionCountMax)
								{
									DEBUG_ADD("So Terrible! return4");
								}
							#endif

							return;
						}

						//------------------------------------------
						
						//------------------------------------------				
						if (m_ActionCount>=m_ActionCountMax)
						{
							SetAction( ACTION_STAND );
						}
					}
				}			
			}	
		}
	}

	
	//--------------------------------------------------------
	
	
	//--------------------------------------------------------
	 
	
}

//----------------------------------------------------------------------
// Add EffectTarget
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::AddEffectTarget(MEffectTarget* pEffectTarget)
{
	
	m_listEffectTarget.clear();

	
	m_listEffectTarget.push_back( pEffectTarget );
}

//----------------------------------------------------------------------
// Remove EffectTarget
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
MPlayer::RemoveEffectTarget(BYTE id)
{
	EFFECTTARGET_LIST::iterator iEffectTarget = m_listEffectTarget.begin();

	
	
	
	while (iEffectTarget != m_listEffectTarget.end())
	{
		
		if ((*iEffectTarget)->GetEffectID() == id)
		{
			
			//delete (*iEffectTarget);

			
			m_listEffectTarget.erase( iEffectTarget );

			break;
		}

		iEffectTarget++;
	}
}

//----------------------------------------------------------------------
// Packet Back
//----------------------------------------------------------------------




//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
// Packet MoveOK
//----------------------------------------------------------------------

//

//----------------------------------------------------------------------
void
MPlayer::PacketMoveOK(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, BYTE direction)
{
	
//	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
//	{
//		SetCurrentDirection(direction);
//		return;
//	}
	// 2004, 9, 16, sobeit add end
	
	m_bShowAdamCloud = false;
	if(NULL != g_pZone && g_pZone->GetID() == 74)
	{
		if(sY<60)
			m_bShowAdamCloud = true;
	}
	// 2004, 9, 21, sobeit add end
	//--------------------------------------------------------
	
	
	//--------------------------------------------------------
	if (!m_listSendDirection.empty())
	{
		m_listSendDirection.pop_front();
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (m_SendMove==0)
	{
		//--------------------------------------------------------
		
		
		
		
		//--------------------------------------------------------
		MovePosition( sX, sY );

		
		m_sX = 0;	
		m_sY = 0;

		m_MoveCount		= m_MoveCountMax;
		m_ActionCount	= m_ActionCountMax; 

		
		m_listDirection.empty();			

		//----------------------------------------------------
		
		//----------------------------------------------------
		if (m_bKeepTraceCreature						
			&& (m_fTrace & FLAG_TRACE_CREATURE_BASIC)	
			&& !m_bTraceCreatureToForceAttack)			
		{							
			KeepTraceCreature();		
		}
		//----------------------------------------------------
		
		//----------------------------------------------------
		else
		{		
			TraceNULL();	

			
			m_DestX		= SECTORPOSITION_NULL;
			m_DestY		= SECTORPOSITION_NULL;
			m_NextDestX	= SECTORPOSITION_NULL;
			m_NextDestY	= SECTORPOSITION_NULL;
		}
	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	else if (m_SendMove>0)
	{
		m_SendMove--;		
	}

	g_bNetStatusGood = true;

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	//POINT nextPoint;
	//MCreature::GetNextPosition(direction, nextPoint);
//	m_ServerX	= nextPoint.x;
//	m_ServerY	= nextPoint.y;

	m_ServerX	= sX;
	m_ServerY	= sY;


	//--------------------------------------------------------
	
	
	//--------------------------------------------------------
	if (m_pZone!=NULL)
	{
		m_pZone->KeepObjectInSight(m_ServerX, m_ServerY, m_Sight);
	}

	SendPositionInfoToParty();

	return;
}

//----------------------------------------------------------------------
// Packet MoveNO
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::PacketMoveNO()	
{
	//m_listSendDirection.front();
	if (!m_listSendDirection.empty())
	{
		m_listSendDirection.pop_front();
	}

	m_SendMove = 0;

	//------------------------------------------------------------
	
	//
	
	
	//
	
	//------------------------------------------------------------
	
		
	{
		
		
		MovePosition( m_ServerX, m_ServerY );
		

		
		m_sX = 0;	
		m_sY = 0;
		
		m_MoveCount		= m_MoveCountMax;
		//m_ActionCount	= m_ActionCountMax; 

		//----------------------------------------------------
		
		//----------------------------------------------------
		if (m_bKeepTraceCreature						
			&& (m_fTrace & FLAG_TRACE_CREATURE_BASIC)	
			&& !m_bTraceCreatureToForceAttack			
			&& m_ActionCount>=m_ActionCountMax)			
		{							
			KeepTraceCreature();		
		}
		//----------------------------------------------------
		
		//----------------------------------------------------
		else
		{		
			
			m_listDirection.empty();

			 
		}

		// 2001.8.22
		if (m_Action==m_MoveAction)
		{
			
			/*
			SetStop();

			m_DestX		= SECTORPOSITION_NULL;
			m_DestY		= SECTORPOSITION_NULL;
			m_NextDestX	= SECTORPOSITION_NULL;
			m_NextDestY	= SECTORPOSITION_NULL;

			TraceNULL();	

			UnSetRepeatAction();			
			*/
			
			m_listDirection.clear(); 

			
			//m_fTrace		= FLAG_TRACE_NULL;
			//m_fNextTrace	= FLAG_TRACE_NULL;

			
			//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;
			//m_ActionCount	= m_ActionCountMax; 	
			m_MoveCount		= m_MoveCountMax-1; 
			m_NextMoveCount = m_MoveCount;

			
			m_bNextAction = false;
			m_NextX = SECTORPOSITION_NULL;
			m_NextY = SECTORPOSITION_NULL;
			
			//m_nNextUsedActionInfo = ACTIONINFO_NULL;
			//m_nUsedActionInfo = ACTIONINFO_NULL;

			
			
			m_DestX			= SECTORPOSITION_NULL; 
			m_DestY			= SECTORPOSITION_NULL; 
			m_NextDestX		= SECTORPOSITION_NULL; 
			m_NextDestY		= SECTORPOSITION_NULL; 
		}
		// 2001.8.6
		
		else if (m_ActionCount>=m_ActionCountMax)
		{
			if (m_Action==m_MoveAction)
			{
				/*
				SetStop();

				m_DestX		= SECTORPOSITION_NULL;
				m_DestY		= SECTORPOSITION_NULL;
				m_NextDestX	= SECTORPOSITION_NULL;
				m_NextDestY	= SECTORPOSITION_NULL;

				TraceNULL();	

				UnSetRepeatAction();
				*/
				m_sX = 0;//-m_cX;
				m_sY = 0;//-m_cY;

				m_MoveCount = m_MoveCountMax - 1;
				m_NextMoveCount = m_MoveCount;
			}
		}

		//m_SendMove = 0;
		
	}
	 
}


//----------------------------------------------------------------------
// Packet SpecialAction Result
//----------------------------------------------------------------------


//



//----------------------------------------------------------------------
void		
MPlayer::PacketSpecialActionResult(TYPE_ACTIONINFO nResultActionInfo, TYPE_OBJECTID id, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, BYTE temp)
{
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (!m_bAlive)
		return;

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	if (nResultActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo()
		
		|| m_bFastMove || m_bKnockBack)
	{
		return;
	}

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	int actionInfoAction = GetActionInfoAction(nResultActionInfo);
	
	if (actionInfoAction==ACTION_DAMAGED)
	{
		//if (HasEffectStatus(EFFECTSTATUS_AURA_PRISM))
		{
		//	nResultActionInfo = REFLECT_AURA_PRISM;
		//	actionInfoAction = GetActionInfoAction(nResultActionInfo);
		}
		//else 
			if (HasEffectStatus(EFFECTSTATUS_AURA_SHIELD))
		{
			nResultActionInfo = REFLECT_AURA_PRISM;//REFLECT_AURA_SHIELD;
			actionInfoAction = GetActionInfoAction(nResultActionInfo);
		}

		//-------------------------------------------------
		// Force Feel
		//-------------------------------------------------
//		if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
//		{
//			gpC_Imm->ForceAction( CImm::FORCE_ACTION_DAMAGED );
//		}
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	int oldTraceID = m_TraceID;

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	///*
	m_nNextUsedActionInfo = ACTIONINFO_NULL;
	TraceNextNULL();

	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
	{
		BOOL	bStartActionInfo = m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo();
		BOOL	bSentPacket		= (m_ActionCount>0);
		int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

		
		
		
		BOOL	bStartedEffect = m_ActionCount>=StartFrame;

		//--------------------------------------------------
		
		//--------------------------------------------------		
		if (!bStartActionInfo						
			|| (bSentPacket && !bStartedEffect))	
		{
			TYPE_ACTIONINFO oldUsedActionInfo = m_nUsedActionInfo;

			m_nUsedActionInfo = ACTIONINFO_NULL;

			AffectUsedActionInfo(oldUsedActionInfo);
			
			
			if (m_nUsedActionInfo != ACTIONINFO_NULL)
			{
				AttachCastingEffect( m_nUsedActionInfo, TRUE );
				AffectUsedActionInfo(m_nUsedActionInfo);
			}
		}			
		//--------------------------------------------------		
		
		//--------------------------------------------------		
		else
		{
			m_nUsedActionInfo = ACTIONINFO_NULL;			
		}
		
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		if (m_nUsedActionInfo!=ACTIONINFO_NULL
			&& GetActionInfoCastingStartFrame(m_nUsedActionInfo) >= m_ActionCount)
		{
			AttachCastingEffect( m_nUsedActionInfo, TRUE );		
		}
	
	}
	//*/

	
	//m_sX = 0;	
	//m_sY = 0;
	
	////SetStop();
	//PacketBack();		
	
	//m_ActionCount = m_ActionCountMax; 

	
	//-------------------------------------------------
	
	//-------------------------------------------------
	//TraceNULL();
	m_fTrace = FLAG_TRACE_NULL;


	SetTraceID( id );
	m_TraceX	= sX;
	m_TraceY	= sY;

	
	m_listDirection.empty();
	m_DestX		= m_X;//SECTORPOSITION_NULL;
	m_DestY		= m_Y;//SECTORPOSITION_NULL;
	m_NextDestX	= SECTORPOSITION_NULL;
	m_NextDestY	= SECTORPOSITION_NULL;

	
	
//	m_nUsedActionInfo	= nResultActionInfo;	// + (*g_pActionInfoTable).GetMinResultActionInfo()
	//SetAction( (*g_pActionInfoTable)[m_nUsedActionInfo].GetAction() );

	
	
	
	
	//m_NextAction = (*g_pActionInfoTable)[m_nUsedActionInfo].GetAction();
	
	//SetNextAction( GetActionInfoAction(m_nUsedActionInfo) );
	//m_bNextAction = true;

	
	
	if (actionInfoAction!=ACTION_STAND)
	{
		SetAction( actionInfoAction );
	}

	if( temp != 0)
		SetActionGrade( temp );

	
	//-------------------------------------------------------------
	//
	// Casting effect
	//
	//-------------------------------------------------------------
	//-------------------------------------------------------------
	// Casting effect
	//-------------------------------------------------------------
	AttachCastingEffect( nResultActionInfo, TRUE );		
	AffectUsedActionInfo( nResultActionInfo );


	//-------------------------------------------------
	
	//-------------------------------------------------
	SetTraceID( oldTraceID );

	
	//-------------------------------------------------
	
	//-------------------------------------------------
	//m_TraceID = m_ID;

	// Test Code
	/*
	if (rand()%2)
		SetChatString( "Shit!" );
	else
		SetChatString( "Oops!" );
	*/
}

//----------------------------------------------------------------------
// Packet Add ActionResult
//----------------------------------------------------------------------



//

//----------------------------------------------------------------------




//----------------------------------------------------------------------
bool		
MPlayer::PacketAddActionResult(WORD effectID, MActionResult* pActionResult)
{
	/*
	MActionResult* pResult = new MActionResult;
	pResult->Add( new MActionResultNodeActionInfo( m_ID, m_TraceID, m_nUsedActionInfo ) );
		
	pEffectTarget->SetResult( pResult );
	*/
	
	
	if (pActionResult==NULL)
	{
		return false;
	}


	if (m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo())
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (m_listEffectTarget.size()!=0)
		{
			
			MEffectTarget* pEffectTarget = m_listEffectTarget.back();

			
			MActionResult* pResult = pEffectTarget->GetResult();

			if (pResult!=NULL)
			{
				pEffectTarget->SetResultNULL();
			}

			
			pEffectTarget->SetResult( pActionResult );

			if (pResult!=NULL)
			{
				
				pResult->Execute();

				delete pResult;
			}			

			return true;

			 
		}
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------			
	
	
	
	pActionResult->Execute();

	delete pActionResult;
	
	return false;	
}

//----------------------------------------------------------------------
// On Attacking
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MPlayer::OnAttacking() const				
{ 
	return m_bRepeatAction && m_bTraceCreatureToForceAttack; 
}

//----------------------------------------------------------------------
// Basic Action To Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::BasicActionToCreature()
{
	if (IsInDarkness())
	{
		return;
	}

	MCreature* pCreature = g_pZone->GetCreature( m_TraceID );

	if (pCreature==NULL)// || pCreature->IsDead())
	{
		return;
	}
	if(pCreature->CurPernalShop() == 1) // by csm 12/4
		return;

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	if (pCreature->IsNPC())
	{
		
		if (//m_CreatureType!=CREATURETYPE_BAT
			//&& m_CreatureType!=CREATURETYPE_WOLF)
			(*g_pCreatureSpriteTable)[(*g_pCreatureTable)[m_CreatureType].SpriteTypes[0]].IsPlayerOnlySprite())
		{		
			if(pCreature->GetCreatureType() == 659)	
				UI_RunHorn(g_pZone->GetID());
			else
				
				if (IsWaitVerifyNULL())
				{
					
					CGNPCTalk _CGNPCTalk;
					_CGNPCTalk.setObjectID( m_TraceID );

					g_pSocket->sendPacket( &_CGNPCTalk );

					SetWaitVerify( WAIT_VERIFY_NPC_ASK );
				}
		}

		
		m_bTraceCreatureToForceAttack = false;
		m_bKeepTraceCreature = false;

		
		SetNextAction(ACTION_STAND);
		m_nUsedActionInfo = ACTIONINFO_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;
		m_fNextTrace = FLAG_TRACE_NULL;
		UnSetRepeatAction();
	}
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	else if (IsRequestMode())
	{
		switch (m_RequestMode)
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			case REQUEST_TRADE :
			{
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& (IsSlayer() && pCreature->IsSlayer() || 
					IsVampire() && pCreature->IsVampire() || 
					IsOusters() && pCreature->IsOusters())
					)
				{
						
						CGTradePrepare _CGTradePrepare;
						_CGTradePrepare.setTargetObjectID( m_TraceID );
						_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_REQUEST );

						g_pSocket->sendPacket( &_CGTradePrepare );

					SetWaitVerify( WAIT_VERIFY_TRADE );
					
					
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_REQUEST);
					g_pTempInformation->Value1 = m_TraceID;

					//-----------------------------------------------------
					
					//-----------------------------------------------------
					UI_RunExchangeCancel( pCreature->GetName() );
				}				
			}
			break;

			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			case REQUEST_PARTY :
			{
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& g_pParty!=NULL
					
					&& !g_pParty->HasMember( pCreature->GetName() )
					&& (IsSlayer() && pCreature->IsSlayer() || IsVampire() && pCreature->IsVampire() || IsOusters() && pCreature->IsOusters())
					&& g_pSystemAvailableManager->IsAvailablePartySystem()
					)
				{
						
						CGPartyInvite _CGPartyInvite;
						_CGPartyInvite.setTargetObjectID( m_TraceID );
						_CGPartyInvite.setCode( CG_PARTY_INVITE_REQUEST );

						g_pSocket->sendPacket( &_CGPartyInvite );

					SetWaitVerify( WAIT_VERIFY_PARTY );

					
					g_pTempInformation->SetMode(TempInformation::MODE_PARTY_REQUEST);
					g_pTempInformation->Value1 = m_TraceID;

					//-----------------------------------------------------
					
					//-----------------------------------------------------
					UI_RunPartyCancel( pCreature->GetName() );
				}				
			}
			break;

			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			case REQUEST_INFO:
			{
//				MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(m_TraceID);

//				{
//					UI_RunPetInfo(pFakeCreature->GetPetItem());
//				}
//				else
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& (IsSlayer() && pCreature->IsSlayer() || 
					IsVampire() && pCreature->IsVampire() || 
					IsOusters() && pCreature->IsOusters() ||
					GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR ||
					GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR ||
					GetCreatureType() == CREATURETYPE_OUSTERS_OPERATOR ||
					GetCompetence() == 0 )
					)
				{
						const char* str = gC_vs_ui.GetInputString();
						if(strlen(str)>0)
						{
							if(str[0] == '*')
							{
								gC_vs_ui.AddInputString(pCreature->GetName());
								break;

							}
						}
						
						CGRequestInfo _CGRequestInfo;
						_CGRequestInfo.setValue( m_TraceID );
						_CGRequestInfo.setCode( CGRequestInfo::REQUEST_CHARACTER_INFO );

						g_pSocket->sendPacket( &_CGRequestInfo );

						MCreature *pCreature = g_pZone->GetCreature(m_TraceID);
						
						if(pCreature != NULL)
						{
							UI_RunOtherInfo(pCreature);

							if (g_pProfileManager!=NULL 
								&& g_pRequestUserManager!=NULL
								&& g_pRequestClientPlayerManager!=NULL)
								//&& !g_pProfileManager->HasProfile(pName)
								//&& !g_pProfileManager->HasProfileNULL(pName)
								//&& !g_pRequestUserManager->HasRequestingUser(pName)
								//&& !g_pRequestClientPlayerManager->HasConnection(pName)
								//&& !g_pRequestClientPlayerManager->HasTryingConnection(pName))
							{
								g_pProfileManager->RequestProfile(pCreature->GetName());
							}
						}


				}
			}
			break;
	
		}

		
		m_bTraceCreatureToForceAttack = false;
		m_bKeepTraceCreature = false;

		
		SetNextAction(ACTION_STAND);
		m_nUsedActionInfo = ACTIONINFO_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;
		m_fNextTrace = FLAG_TRACE_NULL;
		UnSetRepeatAction();

		UnSetRequestMode();
	}	
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	else
	{
		//---------------------------------------------------------------
		
		
		//---------------------------------------------------------------
		bool bOusters = IsOusters();
		MItem *pOustersItem = NULL;
		if( bOusters) 
			pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);

		if (m_bTraceCreatureToForceAttack 
			&& (bOusters && pOustersItem != NULL && pOustersItem->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM
				|| !bOusters)
			)
		{
			SetNextAction(GetActionInfoAction(m_nBasicActionInfo));

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Basic Action : %d", m_TraceID);					
			
			// 
			//m_bTraceCreatureToForceAttack = false;
		}
		else
		{
			DEBUG_ADD_FORMAT("Basic Action : %d - But don't attack", m_TraceID);					
			
			
			m_nUsedActionInfo = ACTIONINFO_NULL;
			
			//---------------------------------
			
			//---------------------------------
			if (m_bKeepTraceCreature)
			{
				return;
			}
		}
	}
	/*
	#ifdef	CONNECT_SERVER
		CGAttackNormal _CGAttackNormal;
		_CGAttackNormal.setX( m_X );
		_CGAttackNormal.setY( m_Y );
		_CGAttackNormal.setDir( m_Direction );
		_CGAttackNormal.setObjectID( m_TraceID );
		g_pSocket->sendPacket( &_CGAttackNormal );

		#ifdef	OUTPUT_DEBUG
			DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
		#endif
	#endif
	*/	
}
//void
//MPlayer::BasicActionToCreature()
//{
//	if (IsInDarkness())
//	{
//		return;
//	}
//
//	MCreature* pCreature = g_pZone->GetCreature( m_TraceID );
//
//	if (pCreature==NULL)// || pCreature->IsDead())
//	{
//		return;
//	}
//
//	//---------------------------------------------------------------

//	//---------------------------------------------------------------
//	if (pCreature->IsNPC())
//	{

//		if (//m_CreatureType!=CREATURETYPE_BAT
//			//&& m_CreatureType!=CREATURETYPE_WOLF)
//			(*g_pCreatureSpriteTable)[(*g_pCreatureTable)[m_CreatureType].SpriteType].IsPlayerOnlySprite())
//		{		

//				UI_RunHorn(g_pZone->GetID());
//			else
//				
//				if (IsWaitVerifyNULL())
//				{

//					CGNPCTalk _CGNPCTalk;
//					_CGNPCTalk.setObjectID( m_TraceID );
//
//					g_pSocket->sendPacket( &_CGNPCTalk );
//
//					SetWaitVerify( WAIT_VERIFY_NPC_ASK );
//				}
//		}
//

//		m_bTraceCreatureToForceAttack = false;
//		m_bKeepTraceCreature = false;
//

//		SetNextAction(ACTION_STAND);
//		m_nUsedActionInfo = ACTIONINFO_NULL;
//		m_nNextUsedActionInfo = ACTIONINFO_NULL;
//		m_fNextTrace = FLAG_TRACE_NULL;
//		UnSetRepeatAction();
//	}
//	//---------------------------------------------------------------

//	//---------------------------------------------------------------
//	else if (IsRequestMode())
//	{
//		switch (m_RequestMode)
//		{
//			//---------------------------------------------------------------

//			//---------------------------------------------------------------
//			case REQUEST_TRADE :
//			{
//				if (IsWaitVerifyNULL() 
//					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
//					&& (IsSlayer() && pCreature->IsSlayer() || 
//					IsVampire() && pCreature->IsVampire() || 
//					IsOusters() && pCreature->IsOusters())
//					)
//				{

//						CGTradePrepare _CGTradePrepare;
//						_CGTradePrepare.setTargetObjectID( m_TraceID );
//						_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_REQUEST );
//
//						g_pSocket->sendPacket( &_CGTradePrepare );
//
//					SetWaitVerify( WAIT_VERIFY_TRADE );
//					

//					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_REQUEST);
//					g_pTempInformation->Value1 = m_TraceID;
//
//					//-----------------------------------------------------

//					//-----------------------------------------------------
//					UI_RunExchangeCancel( pCreature->GetName() );
//				}				
//			}
//			break;
//
//			//---------------------------------------------------------------

//			//---------------------------------------------------------------
//			case REQUEST_PARTY :
//			{
//				if (IsWaitVerifyNULL() 
//					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
//					&& g_pParty!=NULL

//					&& !g_pParty->HasMember( pCreature->GetName() )
//					&& (IsSlayer() && pCreature->IsSlayer() || IsVampire() && pCreature->IsVampire() || IsOusters() && pCreature->IsOusters())
//					&& g_pSystemAvailableManager->IsAvailablePartySystem()
//					)
//				{

//						CGPartyInvite _CGPartyInvite;
//						_CGPartyInvite.setTargetObjectID( m_TraceID );
//						_CGPartyInvite.setCode( CG_PARTY_INVITE_REQUEST );
//
//						g_pSocket->sendPacket( &_CGPartyInvite );
//
//					SetWaitVerify( WAIT_VERIFY_PARTY );
//

//					g_pTempInformation->SetMode(TempInformation::MODE_PARTY_REQUEST);
//					g_pTempInformation->Value1 = m_TraceID;
//
//					//-----------------------------------------------------

//					//-----------------------------------------------------
//					UI_RunPartyCancel( pCreature->GetName() );
//				}				
//			}
//			break;
//
//			//---------------------------------------------------------------

//			//---------------------------------------------------------------
//			case REQUEST_INFO:
//			{
////				MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(m_TraceID);

////				{
////					UI_RunPetInfo(pFakeCreature->GetPetItem());
////				}
////				else
//				if (IsWaitVerifyNULL() 
//					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
//					&& (IsSlayer() && pCreature->IsSlayer() || 
//					IsVampire() && pCreature->IsVampire() || 
//					IsOusters() && pCreature->IsOusters() ||
//					GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR ||
//					GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR ||
//					GetCreatureType() == CREATURETYPE_OUSTERS_OPERATOR ||
//					GetCompetence() == 0 )
//					)
//				{
//						const char* str = gC_vs_ui.GetInputString();
//						if(strlen(str)>0)
//						{
//							if(str[0] == '*')
//							{
//								gC_vs_ui.AddInputString(pCreature->GetName());
//								break;
//
//							}
//						}

//						CGRequestInfo _CGRequestInfo;
//						_CGRequestInfo.setValue( m_TraceID );
//						_CGRequestInfo.setCode( CGRequestInfo::REQUEST_CHARACTER_INFO );
//
//						g_pSocket->sendPacket( &_CGRequestInfo );
//
//						MCreature *pCreature = g_pZone->GetCreature(m_TraceID);
//						
//						if(pCreature != NULL)
//						{
//							UI_RunOtherInfo(pCreature);
//
//							if (g_pProfileManager!=NULL 
//								&& g_pRequestUserManager!=NULL
//								&& g_pRequestClientPlayerManager!=NULL)
//								//&& !g_pProfileManager->HasProfile(pName)
//								//&& !g_pProfileManager->HasProfileNULL(pName)
//								//&& !g_pRequestUserManager->HasRequestingUser(pName)
//								//&& !g_pRequestClientPlayerManager->HasConnection(pName)
//								//&& !g_pRequestClientPlayerManager->HasTryingConnection(pName))
//							{
//								g_pProfileManager->RequestProfile(pCreature->GetName());
//							}
//						}
//
//
//				}
//			}
//			break;
//	
//		}
//

//		m_bTraceCreatureToForceAttack = false;
//		m_bKeepTraceCreature = false;
//

//		SetNextAction(ACTION_STAND);
//		m_nUsedActionInfo = ACTIONINFO_NULL;
//		m_nNextUsedActionInfo = ACTIONINFO_NULL;
//		m_fNextTrace = FLAG_TRACE_NULL;
//		UnSetRepeatAction();
//
//		UnSetRequestMode();
//	}	
//	//---------------------------------------------------------------

//	//---------------------------------------------------------------
//	else
//	{
//		//---------------------------------------------------------------


//		//---------------------------------------------------------------
//		bool bOusters = IsOusters();
//		MItem *pOustersItem = NULL;
//		if( bOusters) 
//			pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);
//
//		if (m_bTraceCreatureToForceAttack 
//			&& (bOusters && pOustersItem != NULL && pOustersItem->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM
//				|| !bOusters)
//			)
//		{
//			SetNextAction(GetActionInfoAction(m_nBasicActionInfo));
//
//			//--------------------
//			// message
//			//--------------------
//			DEBUG_ADD_FORMAT("Basic Action : %d", m_TraceID);					
//			
//			// 
//			//m_bTraceCreatureToForceAttack = false;
//		}
//		else
//		{
//			DEBUG_ADD_FORMAT("Basic Action : %d - But don't attack", m_TraceID);					
//			

//			m_nUsedActionInfo = ACTIONINFO_NULL;
//			
//			//---------------------------------

//			//---------------------------------
//			if (m_bKeepTraceCreature)
//			{
//				return;
//			}
//		}
//	}
//	/*
//	#ifdef	CONNECT_SERVER
//		CGAttackNormal _CGAttackNormal;
//		_CGAttackNormal.setX( m_X );
//		_CGAttackNormal.setY( m_Y );
//		_CGAttackNormal.setDir( m_Direction );
//		_CGAttackNormal.setObjectID( m_TraceID );
//		g_pSocket->sendPacket( &_CGAttackNormal );
//
//		#ifdef	OUTPUT_DEBUG
//			DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
//		#endif
//	#endif
//	*/	
//}

//----------------------------------------------------------------------
// Pickup Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::PickupItem(MItem* pItem)
{
	if (!IsItemCheckBufferNULL())
	{
		DWORD elapsed = GetTickCount() - m_ItemCheckBufferTick;
		if (elapsed > 1500 &&
			(m_ItemCheckBufferStatus == ITEM_CHECK_BUFFER_PICKUP_TO_INVENTORY ||
			 m_ItemCheckBufferStatus == ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE ||
			 m_ItemCheckBufferStatus == ITEM_CHECK_BUFFER_PICKUP_TO_QUICKSLOT ||
			 m_ItemCheckBufferStatus == ITEM_CHECK_BUFFER_PICKUP_MONEY))
		{
			TraceInteractionPlayer("PickupItem clearing stale buffer status=%d elapsed=%lu item=%p",
				(int)m_ItemCheckBufferStatus, (unsigned long)elapsed, (void*)m_pItemCheckBuffer);
			ClearItemCheckBuffer();
		}
	}

	if (pItem != NULL)
	{
		TraceInteractionPlayer("PickupItem begin id=%d item=%d,%d player=%d,%d class=%d type=%d checkNull=%d mouseItem=%p",
			(int)pItem->GetID(), pItem->GetX(), pItem->GetY(), m_X, m_Y,
			(int)pItem->GetItemClass(), (int)pItem->GetItemType(),
			IsItemCheckBufferNULL(), (void*)gpC_mouse_pointer->GetPickUpItem());
	}
	//----------------------------------------------------
	
	
	//----------------------------------------------------

	if( HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ) )
		return;

	if (IsItemCheckBufferNULL() 
		&& gpC_mouse_pointer->GetPickUpItem()==NULL)
	{
		//----------------------------------------------------------------
		
		//----------------------------------------------------------------
		//if (m_SendMove==0)
		{
			// [ TEST CODE ]
			
			
			
			
			//MItem*	pItem = m_pZone->GetItem( m_TraceID );

			//g_Sound.Play( g_SoundTable[ g_ItemTable[pItem->GetItemType()].PickupSoundID ].pDSBuffer );
			//PlaySound( g_ItemTable[pItem->GetItemClass()][pItem->GetItemType()].TileSoundID ,
			//			false,
			//			m_X, m_Y);

			
			
			

			//----------------------------------------------------------------
			//
			
			//
			//----------------------------------------------------------------
			if (pItem->GetItemClass()==ITEM_CLASS_MOTORCYCLE)
			{	
				if (IsSlayer()
					&& m_CreatureType!=CREATURETYPE_SLAYER_OPERATOR)
				{
					RideMotorcycle( (MMotorcycle*)pItem );
				}
			}	
			
			//----------------------------------------------------------------
			//
			
			//
			//----------------------------------------------------------------
			else if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
			{				
				//----------------------------------------------------
				
				//----------------------------------------------------
					MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();

					if ( pCreature != NULL &&
						(
						pCreature->GetActionCount() >= pCreature->GetActionCountMax() || 
						pCreature->GetCreatureType() == 672 
						)
						)
					{	
						if (pItem->GetNumber()>0 || 
							pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || 
							pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || 
							pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 ||
							pCreature->GetCreatureType() == 670 ||			
							pCreature->GetCreatureType() == 672	||			
							pCreature->GetCreatureType() == 673
							)
						{
							
							
							
							if (m_CreatureType!=CREATURETYPE_BAT
								&& m_MoveDevice==MOVE_DEVICE_WALK
								&& !m_bEffectStatus[EFFECTSTATUS_CURSE_PARALYSIS])
							{
								CGDissectionCorpse _CGDissectionCorpse;
								_CGDissectionCorpse.setX( pItem->GetX() );
								_CGDissectionCorpse.setY( pItem->GetY() );
								_CGDissectionCorpse.setObjectID( pItem->GetID() );

								g_pSocket->sendPacket( &_CGDissectionCorpse );

								if(pItem->GetNumber() > 0)
									pItem->SetNumber( pItem->GetNumber()-1 );
							}
						}
						
						else
						{
							
							
							if ( m_CreatureType==CREATURETYPE_WOLF && 
								!(
								pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || 
								pCreature->GetCreatureType() == 670	||			
								pCreature->GetCreatureType() == 672 ||			
								pCreature->GetCreatureType() == 673 ||
								pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || 
								pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563
								)								
							   )
							{
								
								m_nNextUsedActionInfo	= MAGIC_EAT_CORPSE;

								
								m_fTraceBuffer = m_fTrace = FLAG_TRACE_SECTOR_SPECIAL;
								m_TraceX	= pItem->GetX();
								m_TraceY	= pItem->GetY();
								m_TraceZ	= 0;

								m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);

								SetAction( m_MoveAction );
								SetNextDestination(m_TraceX, m_TraceY);

								m_fNextTrace			= FLAG_TRACE_NULL;
								
								m_bKeepTraceCreature	= false;
							}							
						}						
					}
			}

			//----------------------------------------------------------------
			//
			
			//
			//----------------------------------------------------------------
			else if (pItem->GetItemClass()==ITEM_CLASS_MONEY)
			{
				PickupMoney( (MMoney*)pItem );	
			}
			
			else //if (m_CreatureType!=CREATURETYPE_BAT || m_MoveDevice!=MOVE_DEVICE_WALK)
			{
				//----------------------------------------------------------------
				
				//----------------------------------------------------------------
				if (pItem->GetItemClass()==ITEM_CLASS_MINE)
				{
					if (((MMine*)pItem)->IsInstalled())
					{
						return;
					}
				}				
				//----------------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------------
//				else if (pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX)
//				{

//					if (NULL != g_pInventory->FindItem( ITEM_CLASS_EVENT_GIFT_BOX, pItem->GetItemType() ))
//					{

//						return;
//					}
//				}

				//----------------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------------
				if (pItem->IsGearItem())
				{					
					//------------------------------------------
					
					//------------------------------------------
					if (gC_vs_ui.GetGearOpenState() ||
						gC_vs_ui.GetInventoryOpenState())
					{							
						PickupItemToMouse( pItem );
					}
					//------------------------------------------
					
					//------------------------------------------
					else
					{
						PickupItemToInventory( pItem );
					}
				}
				//----------------------------------------------------------------
				//
				
				//
				//----------------------------------------------------------------
				else if (pItem->IsInventoryItem())
				{												
					//----------------------------------------------------------------
					
					//----------------------------------------------------------------
					if (gC_vs_ui.GetInventoryOpenState())
					{							
						PickupItemToMouse( pItem );
					}
					//----------------------------------------------------------------
					
					//----------------------------------------------------------------
					else
					{
						if (pItem->IsQuickItem()
							&& PickupItemToQuickslot( pItem ))
						{
						}
						else
						{
							PickupItemToInventory( pItem );
						}
					}	
				}
			}
		}
		//----------------------------------------------------------------
		
		//----------------------------------------------------------------
		//DEBUG_ADD_FORMAT("Cannot Pickup item (Position not verified) sendMove=%d", m_SendMove);		
	}
	#ifdef OUTPUT_DEBUG
	else
	{
		if (gpC_mouse_pointer->GetPickUpItem()!=NULL)
		{
			DEBUG_ADD_FORMAT("[UI] Already Handle a Item id=%d", gpC_mouse_pointer->GetPickUpItem()->GetID());
		}
	}
	#endif
	
}

//----------------------------------------------------------------------
// Pickup Item to Inventory
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::PickupItemToInventory(MItem* pItem)
{
	//------------------------------------------
	
	
	//------------------------------------------
		POINT fitPoint;
		//------------------------------------------
		
		//------------------------------------------
		if (IsItemCheckBufferNULL())
		{
			if (g_pInventory->GetFitPosition(pItem, fitPoint) && 
				(pItem->IsSlayerItem() && IsSlayer() || 
				pItem->IsVampireItem() && IsVampire() || 
				pItem->IsOustersItem() && IsOusters()))
			{
				CGAddZoneToInventory _CGAddZoneToInventory;
				
				_CGAddZoneToInventory.setObjectID( pItem->GetID() );
				_CGAddZoneToInventory.setZoneX( pItem->GetX() );
				_CGAddZoneToInventory.setZoneY( pItem->GetY() );
				_CGAddZoneToInventory.setInvenX( fitPoint.x );
				_CGAddZoneToInventory.setInvenY( fitPoint.y );

				TraceInteractionPlayer("SEND CGAddZoneToInventory id=%d zone=%d,%d inven=%d,%d",
					(int)pItem->GetID(), pItem->GetX(), pItem->GetY(), fitPoint.x, fitPoint.y);
				g_pSocket->sendPacket( &_CGAddZoneToInventory );

				
				pItem->SetGridXY( fitPoint.x, fitPoint.y );
				SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_TO_INVENTORY);											
			}
			else
			{
				//------------------------------------------------------------
				
				//------------------------------------------------------------
				pItem->SetDropping();

				
				ExecuteHelpEvent( HELP_EVENT_STORAGE_BUY );
				// 2004, 5, 6, sobeit add end
			}
		}
		//------------------------------------------
		
		
		//------------------------------------------
		else
		{
			
			
		}
}

//----------------------------------------------------------------------
// Pickup Money
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::PickupMoney(MMoney* pItem)
{
		//------------------------------------------
		
		//------------------------------------------
		if (IsItemCheckBufferNULL())
		{		
			CGPickupMoney _CGPickupMoney;
			
			_CGPickupMoney.setObjectID( pItem->GetID() );
			_CGPickupMoney.setZoneX( pItem->GetX() );
			_CGPickupMoney.setZoneY( pItem->GetY() );
		
			TraceInteractionPlayer("SEND CGPickupMoney id=%d zone=%d,%d",
				(int)pItem->GetID(), pItem->GetX(), pItem->GetY());
			g_pSocket->sendPacket( &_CGPickupMoney );

			
			SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_MONEY);											
		
		}
		//------------------------------------------
		
		
		//------------------------------------------
		else
		{
			
			
		}
}

//----------------------------------------------------------------------
// Pickup Item to Mouse
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::PickupItemToMouse(MItem* pItem)
{
	//------------------------------------------
	
	//------------------------------------------
	
	
	//------------------------------------------
		//------------------------------------------
		
		//------------------------------------------
		if (IsItemCheckBufferNULL())
		{
			if(pItem->IsSlayerItem() && IsSlayer() || 
				pItem->IsVampireItem() && IsVampire() ||
				pItem->IsOustersItem() && IsOusters())
			{
				CGAddZoneToMouse _CGAddZoneToMouse;
				
				_CGAddZoneToMouse.setObjectID( pItem->GetID() );
				_CGAddZoneToMouse.setZoneX( pItem->GetX() );
				_CGAddZoneToMouse.setZoneY( pItem->GetY() );
				
				TraceInteractionPlayer("SEND CGAddZoneToMouse id=%d zone=%d,%d",
					(int)pItem->GetID(), pItem->GetX(), pItem->GetY());
				g_pSocket->sendPacket( &_CGAddZoneToMouse );
				
				
				SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE);
			}
		}								
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			
		}
}

//----------------------------------------------------------------------
// Pickup Item to Quickslot
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MPlayer::PickupItemToQuickslot(MItem* pItem)
{
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	if ( (g_pQuickSlot!=NULL&&IsSlayer()) || ((g_pArmsBand1!=NULL||g_pArmsBand2!=NULL)&&IsOusters())
		&& IsItemCheckBufferNULL()
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL
		&& (pItem->IsSlayerItem() && IsSlayer() || 
		pItem->IsVampireItem() && IsVampire() || 
		pItem->IsOustersItem() && IsOusters()))
	{
		BOOL FirstArmsband = TRUE;
		int slot;

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (IsSlayer() && !g_pQuickSlot->FindSlotToAddItem(pItem, slot))
		{
			return false;
		} 
		else if( IsOusters() )
		{
			
			
			if( g_pArmsBand1 == NULL ||  !g_pArmsBand1->FindSlotToAddItem( pItem, slot ) )
			{
				if( g_pArmsBand2 == NULL || !g_pArmsBand2->FindSlotToAddItem( pItem,slot ) )
					return false;
				else FirstArmsband = FALSE;
			} else
			{
				FirstArmsband = TRUE;
			}
		}

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			CGAddZoneToMouse _CGAddZoneToMouse;

			_CGAddZoneToMouse.setObjectID( pItem->GetID() );
			_CGAddZoneToMouse.setZoneX( pItem->GetX() );
			_CGAddZoneToMouse.setZoneY( pItem->GetY() );

			g_pSocket->sendPacket( &_CGAddZoneToMouse );

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_PICKUP_TO_QUICKSLOT);
			g_pTempInformation->Value1 = FirstArmsband ? slot : slot + 3;
			g_pTempInformation->pValue = (void*)pItem;

			SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_TO_QUICKSLOT);


		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Pickup Item to Mouse
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MPlayer::RideMotorcycle(MMotorcycle* pMotorcycle)
{
	if (pMotorcycle==NULL)
	{
		DEBUG_ADD("[Error] Player::Motorcycle to ride is NULL");
		
		return;
	}

	//------------------------------------------
	
	//------------------------------------------
	if (m_MoveDevice==MOVE_DEVICE_WALK)
	{
		CGRideMotorCycle _CGRideMotorCycle;

		_CGRideMotorCycle.setObjectID( pMotorcycle->GetID() );
		_CGRideMotorCycle.setX( pMotorcycle->GetX() );
		_CGRideMotorCycle.setY( pMotorcycle->GetY() );

		g_pSocket->sendPacket( &_CGRideMotorCycle );

		
		SetWaitVerify( WAIT_VERIFY_MOTORCYCLE_GETON );
	}
	//------------------------------------------
	
	//------------------------------------------
	else
	{
		
	}
}

//----------------------------------------------------------------------
// Skill To Inventory Item
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
bool
MPlayer::TraceInventoryItem(TYPE_OBJECTID id)
{
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (!IsNotDelay()
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE ))
	{
		return false;
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (m_nSpecialActionInfo == ACTIONINFO_NULL
		
		|| m_MoveDevice == MOVE_DEVICE_RIDE
		
		
		)
	{
		return false;
	}
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetItem())
	{
		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		if (!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo )
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}

		
		if (IsStop() && IsItemCheckBufferNULL()) // || m_Action!=ACTION_ATTACK)
		{
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			const MItem* pItem = ((MItemManager)(*g_pInventory)).GetItem( id );
			
			if (pItem==NULL)
			{
				return false;
			}

			
			m_nNextUsedActionInfo	= m_nSpecialActionInfo;

			
			SetTraceID( id );
			m_fTrace	= FLAG_TRACE_INVENTORY;
			m_TraceX	= pItem->GetGridX();
			m_TraceY	= pItem->GetGridY();
			m_TraceZ	= 0;

			
			m_fTraceBuffer	= m_fTrace;	
			

			//------------------------------------------------------------		
			
			//------------------------------------------------------------	
			m_TraceDistance		= 1;		
			SetAction( m_MoveAction );
			SetNextDestination(m_X, m_Y);

			
			m_fNextTrace			= FLAG_TRACE_NULL;
		
			
			m_bKeepTraceCreature	= false;

			//------------------------------------------------------------
			
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Trace IteMM: %d", id);				
		}
		else
		{
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				m_fNextTrace	= FLAG_TRACE_INVENTORY;
				m_NextTraceID	= id;

				m_bRepeatAction = FALSE;

				return false;
			}
		}

		return true;
	}

	
	return false;
}

//----------------------------------------------------------------------
// Change To Slayer
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MPlayer::ChangeToSlayer()
{
	if (MCreature::ChangeToSlayer())
	{
		m_ConversionDelayTime = 0;

		g_pCurrentMagazine = NULL;
		
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (g_pStorage!=NULL)
		{
			delete g_pStorage;
			g_pStorage = NULL;
		}

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		g_pTopView->SetFadeStart(1, 31, 6,  5,5,31);

		UI_ChangeInterfaceRace(RACE_SLAYER);

		SetWaitVerifyNULL();

		ResetSendMove();
		
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Change To Vampire
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
MPlayer::ChangeToVampire()
{
	if (MCreature::ChangeToVampire())
	{
		m_ConversionDelayTime = 0;

		g_pCurrentMagazine = NULL;
		

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (g_pStorage!=NULL)
		{
			delete g_pStorage;
			g_pStorage = NULL;
		}

		g_pTopView->SetFadeStart(1, 31, 6,  31,0,0);

		UI_ChangeInterfaceRace(RACE_VAMPIRE);

		SetWaitVerifyNULL();

		ResetSendMove();

		DEBUG_ADD("MPlayer::Change to Vampire OK");
		
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Set Status
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::SetStatus(DWORD n, DWORD value)
{ 
	if (n >= MODIFY_MAX)
	{
		DEBUG_ADD_FORMAT("[Error] Modify Part is Wrong : part=%d, value=%d", n, value);
		
		return;
	}

	MODIFY_VALUE modifyValue;
	modifyValue.oldValue = m_Status[n];
	modifyValue.newValue = value;

	
	m_Status[n] = value; 
	
	
	g_pModifyStatusManager->Execute( n, (void*)&modifyValue );

	
	m_Status[n] = modifyValue.newValue;
}

//----------------------------------------------------------------------
// Check Status
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::CalculateStatus()
{
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	MItemManager* pGear;

	int weaponSpeed = 0;
	int weaponTohit = 0;

	switch(GetRace())
	{
	case RACE_SLAYER:
		{
			pGear = g_pSlayerGear;
			
			const MItem* pWeapon = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );
			
			SKILLDOMAIN domain = MAX_SKILLDOMAIN;
			int domainLevel = 0;
			
			//------------------------------------------------------
			
			//------------------------------------------------------
			if (pWeapon!=NULL && pWeapon->IsAffectStatus())
			{
				if (pWeapon->IsGunItem())
				{
					domain = SKILLDOMAIN_GUN;
					
					weaponTohit = pWeapon->GetToHit();
				}
				else if (pWeapon->GetItemClass()==ITEM_CLASS_SWORD)
				{
					domain = SKILLDOMAIN_SWORD;
				}
				else if (pWeapon->GetItemClass()==ITEM_CLASS_BLADE)
				{
					domain = SKILLDOMAIN_BLADE;
				}
				else if (pWeapon->GetItemClass()==ITEM_CLASS_CROSS)
				{
					domain = SKILLDOMAIN_HEAL;
				}
				else if (pWeapon->GetItemClass()==ITEM_CLASS_MACE)
				{
					domain = SKILLDOMAIN_ENCHANT;
				}
				
				if (domain < MAX_SKILLDOMAIN)
				{
					domainLevel = (*g_pSkillManager)[domain].GetDomainLevel();
				}
				
				
				//----------------------------------------------------
				
				//----------------------------------------------------			
				if(pWeapon->IsAffectStatus())
					weaponSpeed = pWeapon->GetOriginalSpeed();
			}
			
			g_StatusManager.SetCurrentWeaponDomain( domain, domainLevel );
		}
		break;

	case RACE_VAMPIRE:
		{
			pGear = g_pVampireGear;
			
			const MItem* pWeapon = g_pVampireGear->GetItem( MVampireGear::GEAR_VAMPIRE_RIGHTHAND );
			
			if (pWeapon!=NULL)
			{
				//----------------------------------------------------
				
				//----------------------------------------------------			
				if(pWeapon->IsAffectStatus())
					weaponSpeed = pWeapon->GetOriginalSpeed();
			}
			g_StatusManager.SetCurrentWeaponDomain( SKILLDOMAIN_VAMPIRE, GetLEVEL() );
		}
		break;

	case RACE_OUSTERS:
		{
			pGear = g_pOustersGear;
			
			const MItem* pWeapon = g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND );
			
			if (pWeapon!=NULL)
			{
				//----------------------------------------------------
				
				//----------------------------------------------------			
				if(pWeapon->IsAffectStatus())
					weaponSpeed = pWeapon->GetOriginalSpeed();
			}
			g_StatusManager.SetCurrentWeaponDomain( SKILLDOMAIN_OUSTERS, GetLEVEL() );
		}
		break;
	}

	
	//-----------------------------------------------------------------
	
	//
	
	//-----------------------------------------------------------------
	g_StatusManager.Set(GetSTR(), GetDEX(), GetINT());

	int DV		= g_StatusManager.GetDefense();
	int PV		= g_StatusManager.GetProtection();
	int TOHIT	= g_StatusManager.GetTOHIT() + weaponTohit;
	//int CC		= g_StatusManager.GetCC();
	int MinDAM	= m_Status[MODIFY_MIN_DAMAGE];	//g_StatusManager.GetMinDAM();
	int MaxDAM	= m_Status[MODIFY_MAX_DAMAGE];	//g_StatusManager.GetMaxDAM();
	int AttackSpeed = g_StatusManager.GetAttackSpeed() + weaponSpeed;

	if((*g_pRankBonusTable)[RANK_BONUS_HAWK_WING].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		AttackSpeed += (*g_pRankBonusTable)[RANK_BONUS_HAWK_WING].GetPoint();

	if((*g_pRankBonusTable)[RANK_BONUS_CROW_WING].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		AttackSpeed += (*g_pRankBonusTable)[RANK_BONUS_CROW_WING].GetPoint();
	
	if((*g_pRankBonusTable)[RANK_BONUS_SWIFT_ARM].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		AttackSpeed += (*g_pRankBonusTable)[RANK_BONUS_SWIFT_ARM].GetPoint();

	// Pet Bonus
	if( GetPetID() != OBJECTID_NULL)
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(GetPetID());
		if(pFakeCreature != NULL)
		{
			MPetItem *pPetItem = pFakeCreature->GetPetItem();
			if(pPetItem != NULL)
			{
				ITEMOPTION_TABLE::ITEMOPTION_PART attr = (ITEMOPTION_TABLE::ITEMOPTION_PART)pPetItem->GetEnchantLevel();
				if(attr == ITEMOPTION_TABLE::PART_ATTACK_SPEED)
				{
					AttackSpeed += pPetItem->GetSilver();
				}
			}
		}
	}

	int	ElementalFire = 0;
	int	ElementalWater = 0;
	int	ElementalEarth = 0;
	int	ElementalWind = 0;

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	SetItemLightSight(0);

	pGear->SetBegin();

	while (pGear->IsNotEnd())
	{
		const MItem* pItem = pGear->Get();

		//----------------------------------------------------------
		
		//----------------------------------------------------------
		if (pItem != NULL && pItem->IsAffectStatus())
		{
			if(pItem->GetItemClass() == ITEM_CLASS_OUSTERS_WRISTLET || pItem->GetItemClass() == ITEM_CLASS_OUSTERS_STONE)
			{
				ITEMTABLE_INFO iInfo = (*g_pItemTable)[pItem->GetItemClass()][pItem->GetItemType()];
				if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_FIRE)
					ElementalFire += iInfo.Elemental;
				else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_WATER)
					ElementalWater += iInfo.Elemental;
				else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_EARTH)
					ElementalEarth += iInfo.Elemental;
				else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_WIND)
					ElementalWind += iInfo.Elemental;

			}
			//---------------------------------------------------
			
			//---------------------------------------------------
			int def = pItem->GetDefenseValue();
			
			if (def != -1)
			{
				DV += def;
			}

			int pro = pItem->GetProtectionValue();
			
			if (pro != -1)
			{
				PV += pro;
			}

			const std::list<TYPE_ITEM_OPTION>& optionList = pItem->GetItemOptionList();

			std::list<TYPE_ITEM_OPTION>::const_iterator optionListItr = optionList.begin();

			while(optionListItr != optionList.end())
			{
				
				ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[*optionListItr];
				//---------------------------------------------------
				
				//---------------------------------------------------
				switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
				{
				case ITEMOPTION_TABLE::PART_DEFENSE		: DV += optionInfo.PlusPoint; break;
				case ITEMOPTION_TABLE::PART_TOHIT		: TOHIT += optionInfo.PlusPoint; break;
				case ITEMOPTION_TABLE::PART_PROTECTION	: PV += optionInfo.PlusPoint; break;
					//case ITEMOPTION_INFO::PART_DAMAGE		: MinDAM += optionInfo.PlusPoint; 
					//											MaxDAM += optionInfo.PlusPoint; 
					//break;
					
				case ITEMOPTION_TABLE::PART_ATTACK_SPEED : AttackSpeed += optionInfo.PlusPoint; break;
					
				case ITEMOPTION_TABLE::PART_VISION :	SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); break;
				}
				optionListItr++;
			}

			std::list<TYPE_ITEM_OPTION>::const_iterator itr = pItem->GetItemDefaultOptionList().begin();

			while(itr != pItem->GetItemDefaultOptionList().end())
			{
				TYPE_ITEM_OPTION option = *itr;

				ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[option];
				//---------------------------------------------------
				
				//---------------------------------------------------
				switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
				{
				case ITEMOPTION_TABLE::PART_DEFENSE		: DV += optionInfo.PlusPoint; break;
				case ITEMOPTION_TABLE::PART_TOHIT		: TOHIT += optionInfo.PlusPoint; break;
				case ITEMOPTION_TABLE::PART_PROTECTION	: PV += optionInfo.PlusPoint; break;
					//case ITEMOPTION_INFO::PART_DAMAGE		: MinDAM += optionInfo.PlusPoint; 
					//											MaxDAM += optionInfo.PlusPoint; 
					//break;
					
				case ITEMOPTION_TABLE::PART_ATTACK_SPEED : AttackSpeed += optionInfo.PlusPoint; break;
					
				case ITEMOPTION_TABLE::PART_VISION :	SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); break;
				}

				itr++;
			}
		}

		pGear->Next();
	}	

	
	if((*g_pRankBonusTable)[RANK_BONUS_SALAMANDERS_KNOWLEDGE].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		ElementalFire ++;
	if((*g_pRankBonusTable)[RANK_BONUS_UNDINES_KNOWLEDGE].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		ElementalWater ++;
	if((*g_pRankBonusTable)[RANK_BONUS_GNOMES_KNOWLEDGE].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		ElementalEarth ++;
	// 2004, 9, 20, sobeit add end
	
	SetStatus(MODIFY_ELEMENTAL_FIRE, ElementalFire);
	SetStatus(MODIFY_ELEMENTAL_WATER, ElementalWater);
	SetStatus(MODIFY_ELEMENTAL_EARTH, ElementalEarth);
	SetStatus(MODIFY_ELEMENTAL_WIND, ElementalWind);
	if(IsOusters())
		g_pSkillAvailable->SetAvailableSkills();

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	SetStatus(MODIFY_ATTACK_SPEED, AttackSpeed);

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	
	//SetStatus(MODIFY_DEFENSE, DV);
	//SetStatus(MODIFY_PROTECTION, PV);
	//SetStatus(MODIFY_TOHIT, TOHIT);
//	SetStatus(MODIFY_CARRYWEIGHT, CC);			

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------	
//	m_Status[MODIFY_MIN_DAMAGE] = MinDAM; 
//	m_Status[MODIFY_MAX_DAMAGE] = MaxDAM; 
	int maxDAM, minDAM;
	int maxSilverDAM = 0, minSilverDAM = 0;

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	switch(GetRace())
	{
		case RACE_SLAYER:
		{
			const MItem* pWeapon = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );

			if (pWeapon==NULL || !pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM;
				maxDAM = MaxDAM;
			}
			else //if (pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM + pWeapon->GetMinDamage();
				maxDAM = MaxDAM + pWeapon->GetMaxDamage();		

				//-----------------------------------------------------------------
				
				//-----------------------------------------------------------------
				if (pWeapon->GetItemClass()==ITEM_CLASS_MACE
					|| pWeapon->GetItemClass()==ITEM_CLASS_CROSS)
				{
					minSilverDAM = pWeapon->GetMinDamage() / 10;
					maxSilverDAM = pWeapon->GetMaxDamage() / 10;
				}

				//-----------------------------------------------------------------
				
				
				//-----------------------------------------------------------------
				if (!pWeapon->IsGunItem() && pWeapon->GetSilver() > 0
					|| pWeapon->IsGunItem() 
						&& g_pCurrentMagazine!=NULL 
						&& g_pCurrentMagazine->GetItemType()>=8
						&& g_pCurrentMagazine->GetItemType()<=15)
				{
					minSilverDAM += pWeapon->GetMinDamage() / 10;
					maxSilverDAM += pWeapon->GetMaxDamage() / 10;
				}
			}
		}
	break;

	case RACE_VAMPIRE:
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		{
			const MItem* pWeapon = g_pVampireGear->GetItem( MVampireGear::GEAR_VAMPIRE_RIGHTHAND );

			if (pWeapon==NULL || !pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM;
				maxDAM = MaxDAM;
			}
			else //if (pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM + pWeapon->GetMinDamage();
				maxDAM = MaxDAM + pWeapon->GetMaxDamage();		

			}
		}
		break;

	case RACE_OUSTERS:
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		{
			const MItem* pWeapon = g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND );
			
			if (pWeapon==NULL || !pWeapon->IsAffectStatus() || pWeapon->GetItemClass() == ITEM_CLASS_OUSTERS_WRISTLET)
			{
				minDAM = MinDAM;
				maxDAM = MaxDAM;
			}
			else //if (pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM + pWeapon->GetMinDamage();
				maxDAM = MaxDAM + pWeapon->GetMaxDamage();		
				
			}
		}
		break;
	}

	UI_SetCharInfoDAM( maxDAM, minDAM );
	
	g_char_slot_ingame.SILVER_DAM2	= minSilverDAM;
	g_char_slot_ingame.SILVER_DAM	= maxSilverDAM;

	
	if (g_Mode == MODE_WAIT_PCLIST)
	{
		UI_SetCharInfoDefense( DV );	
		UI_SetCharInfoProtection( PV );	
		UI_SetCharInfoTOHIT( TOHIT );
	}
	else
	{
		UI_SetCharInfoDefense( GetDefense() );	
		UI_SetCharInfoProtection( GetProtection() );	
		UI_SetCharInfoTOHIT( GetTOHIT() );
	}
}

//----------------------------------------------------------------------
// Fast Move Position
//----------------------------------------------------------------------
// (m_X, m_Y) --> (x, y)
//----------------------------------------------------------------------
bool
MPlayer::FastMovePosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y, bool server)
{	
	if (g_pTopView!=NULL)
	{
		g_pTopView->SetSelectedSectorNULL();
	}

	if (MCreature::FastMovePosition( x, y ))
	{
		if(server == true)
			SetServerPosition( x, y);
		
		
		//m_nUsedActionInfo = ACTIONINFO_NULL;
		//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;

		///*
		m_fNextTrace = FLAG_TRACE_NULL;

		m_bKeepTraceCreature = false;
		m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;

		UnSetRepeatAction();
		//*/

		
		ResetSendMove();

		
		m_nUsedActionInfo = ACTIONINFO_NULL;

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// KnockBackPosition
//----------------------------------------------------------------------
bool	
MPlayer::KnockBackPosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	if (MCreature::KnockBackPosition( x, y ))
	{
		
		
		//m_nUsedActionInfo = ACTIONINFO_NULL;
		//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;

		///*
		//m_fNextTrace = FLAG_TRACE_NULL;

		//m_bKeepTraceCreature = false;
		//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;

		//UnSetRepeatAction();
		//*/

		
		ResetSendMove();

		
		m_nUsedActionInfo = ACTIONINFO_NULL;

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Find Enemy
//----------------------------------------------------------------------


//----------------------------------------------------------------------
int		
MPlayer::FindEnemy()
{
	//---------------------------------------------------------------
	
	
	//---------------------------------------------------------------
	int bBasicAction = g_bLButtonDown;
	int bSpecialAction = g_bRButtonDown;
	int actionDistance = 0;

	//---------------------------------------------------------------
	
	//-------------------------------------------------------------
	if (bBasicAction)
	{
		actionDistance = m_BasicAttackDistance;		
	}
	//-------------------------------------------------------------
	
	//-------------------------------------------------------------
	else if (bSpecialAction)
	{
		
		if (m_nSpecialActionInfo==ACTIONINFO_NULL)
		{
			return NULL;
		}

		actionDistance = GetActionInfoRange( m_nSpecialActionInfo );
	}

	//-------------------------------------------------------------
	
	//-------------------------------------------------------------
	int	numCreature = g_pZone->GetCreatureNumber();

	MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();
		
	int targetCreatureID = OBJECTID_NULL;

	for (int i=0; i<numCreature; i++)
	{
		MCreature* pCreature = iCreature->second;

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (!pCreature->IsNPC() 
			&& pCreature->GetID()!=m_ID)
		{
			int tx = pCreature->GetServerX();
			int ty = pCreature->GetServerY();

			//--------------------------------------------------
			
			
			//--------------------------------------------------
			if (g_pObjectSelector->CanAttack(pCreature)
				&& max(abs(tx-m_X), abs(ty-m_Y)) <= actionDistance)
				//&& g_pObjectSelector->IsAblePKbyZone(pCreature))					
			{
				//--------------------------------------------------
				
				//--------------------------------------------------
				if (targetCreatureID==OBJECTID_NULL)
				{
					targetCreatureID = pCreature->GetID();
				}
				//--------------------------------------------------
				
				//--------------------------------------------------
				else
				{
					//--------------------------------------------------
					
					//--------------------------------------------------
					if (rand()%2)
					{
						targetCreatureID = pCreature->GetID();	
					}
					//--------------------------------------------------
					
					
					//--------------------------------------------------
					else
					{
						return targetCreatureID;
					}
				}
			}
		}

		iCreature++;
	}	

	return targetCreatureID;
}

//----------------------------------------------------------------------
// Stop Blood Drain
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::StopBloodDrain()
{
	if (m_Action==ACTION_VAMPIRE_DRAIN
		&& IsVampire()
		&& m_ActionCountMax>60)
	{
		if (m_ActionCount>9 && m_ActionCount<60)
		{
			m_ActionCount = 60;

			
			//m_DelayTime	= g_CurrentTime + (m_ActionCountMax-m_ActionCount)*60;		
			
			UnSetStopBloodDrain();
//			char TempBuffer[128];
//			sprintf(TempBuffer, "StopBD: D:%d, m:%d, C:%d", m_DelayTime,m_bStopBloodDrain, m_ActionCount);
//			g_pSystemMessage->Add(TempBuffer);

			//DEBUG_ADD_FORMAT("[Player] StopBloodDrain1(%d)", m_bStopBloodDrain);

		}	
	}
	else
	{
		//UnSetStopBloodDrain();
		//DEBUG_ADD_FORMAT("[Player] StopBloodDrain2(%d), action=%d(%d)", m_bStopBloodDrain, m_Action, m_ActionCountMax);
	}
}

//----------------------------------------------------------------------
// Stop Absorb Soul
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MPlayer::StopAbsorbSoul()
{
	if (m_Action==ACTION_OUSTERS_DRAIN
		&& IsOusters()
		&& m_ActionCountMax>30)
	{
		if (m_ActionCount>9 && m_ActionCount<30)
		{
			m_ActionCount = 30;
			
			ATTACHEFFECT_LIST::iterator itr = m_listEffect.begin();
			ATTACHEFFECT_LIST::iterator endItr = m_listEffect.end();
			
			while(itr != endItr)
			{
				MAttachEffect*	pEffect = *itr;
				
				if(pEffect->GetEffectSpriteType() == EFFECTSPRITETYPE_ABSORB_SOUL)
				{
					bool bUseEffectSprite = pEffect->IsEffectSprite();
					
					
					if (bUseEffectSprite)
					{
						m_bAttachEffect[pEffect->GetEffectSpriteType()] = false;	
					}
					
					delete pEffect;
					m_listEffect.erase(itr);
					break;
				}
				itr++;
			}
			
			UnSetStopAbsorbSoul();
		}	
	}
	else
	{
		//UnSetStopBloodDrain();
	}
}

//----------------------------------------------------------------------
// Check In Darkness
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
MPlayer::CheckInDarkness()
{
	if (g_pZone!=NULL && 
		( !IsVampire() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS)|| g_pZone->GetID() == 3001 ) || 
		g_pZone->GetID() == 3001 && IsVampire())
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		&& !HasEffectStatus( EFFECTSTATUS_INSTALL_TURRET)
		)
	{
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		
		const MSector& sector = g_pZone->GetSector( m_X, m_Y );

		if (sector.HasDarkness())
		{
			
			PlaceInDarkness();
		}
		else
		{		
			
			PlaceNotInDarkness();
		}
	}
	else
	{
		PlaceNotInDarkness();
	}
}

//----------------------------------------------------------------------
// Set SpecialActionInfo
//----------------------------------------------------------------------
void			
MPlayer::SetSpecialActionInfo( TYPE_ACTIONINFO n )	
{ 
	if (g_pActionInfoTable!=NULL 
		&& n<g_pActionInfoTable->GetMinResultActionInfo())
	{
		m_nSpecialActionInfo = n; 

		
		
		if (IsRepeatAction())
		{
			if (m_fTraceBuffer != FLAG_TRACE_NULL
				
				&& !(m_fTraceBuffer & FLAG_TRACE_CREATURE_BASIC)
				&& !(m_fTraceBuffer & FLAG_TRACE_SECTOR_BASIC)
				&& !(m_fTraceBuffer & FLAG_TRACE_ITEM)
				&& !(m_fTraceBuffer & FLAG_TRACE_INTERACTIONOBJECT)
				&& !(m_fTraceBuffer & FLAG_TRACE_EFFECT)
				)
			{			
				
				BOOL bOther = ((*g_pActionInfoTable)[n].IsTargetOther());
				BOOL bSelf = ((*g_pActionInfoTable)[n].IsTargetSelf());
				BOOL bZone = ((*g_pActionInfoTable)[n].IsTargetZone());
				BOOL bItem = ((*g_pActionInfoTable)[n].IsTargetItem());

				
				if (((m_fTraceBuffer & FLAG_TRACE_SELF) && bSelf)
					|| ((m_fTraceBuffer & FLAG_TRACE_CREATURE_SPECIAL) && bOther)
					|| ((m_fTraceBuffer & FLAG_TRACE_SECTOR_SPECIAL) && bZone)
					|| ((m_fTraceBuffer & FLAG_TRACE_INVENTORY) && bItem)
					)
				{				
				}
				
				else
				{
					if (bOther)
					{
						m_fTraceBuffer = FLAG_TRACE_CREATURE_SPECIAL;
					}
					else if (bSelf)
					{
						m_fTraceBuffer = FLAG_TRACE_SELF;
					}
					else if (bZone)
					{
						m_fTraceBuffer = FLAG_TRACE_SECTOR_SPECIAL;
					}
					else if (bItem)
					{
						m_fTraceBuffer = FLAG_TRACE_INVENTORY;
					}
				}
			}		
		}
	}
	
}

//----------------------------------------------------------------------
// Is In SafeSector
//----------------------------------------------------------------------
BOOL
MPlayer::IsInSafeSector() const
{
	if(g_pWarManager->IsExist(m_pZone->GetID()))
		return FALSE;

	if (m_pZone!=NULL)
	{
		if(m_pZone->GetSector(m_X, m_Y).IsSafeComplete() || g_bZoneSafe)
			return 2;

		if (IsSlayer())
		{
			return (m_pZone->GetSector(m_X, m_Y).IsSafeSlayer())?1:0;
		}
		else
		if (IsVampire())
		{
			return (m_pZone->GetSector(m_X, m_Y).IsSafeVampire())?1:0;
		} else
		if (IsOusters() )
		{
			return (m_pZone->GetSector(m_X, m_Y).IsSafeOusters())?1:0;
		}
	}

	return FALSE;
}

//-------------------------------------------------------------------------
// Soul Chain File
//-------------------------------------------------------------------------
void	
MPlayer::SetCannotTrace()
{
	gC_vs_ui.SetCannotTrace();
}

void	MPlayer::SetTimeLightSight(char s)
{
	m_TimeLightSight = s; 
	CalculateLightSight();
}

void	MPlayer::SetItemLightSight(int s)
{
	m_ItemLightSight = s; 
	CalculateLightSight();
}

void	MPlayer::CalculateLightSight()
{
//	CalculateSight();
	m_LightSight = GetTimeLightSight() + GetItemLightSight();

	
	if (m_bEffectStatus[EFFECTSTATUS_OBSERVING_EYE])
	{
		m_LightSight +=  1 + (*g_pSkillInfoTable)[SKILL_OBSERVING_EYE].GetLevel() / 50;
	}
	// Pet Bonus
	if( GetPetID() != OBJECTID_NULL)
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(GetPetID());
		if(pFakeCreature != NULL)
		{
			MPetItem *pPetItem = pFakeCreature->GetPetItem();
			if(pPetItem != NULL)
			{
				ITEMOPTION_TABLE::ITEMOPTION_PART attr = (ITEMOPTION_TABLE::ITEMOPTION_PART)pPetItem->GetEnchantLevel();
				if(attr == ITEMOPTION_TABLE::PART_VISION)
				{
					m_LightSight += pPetItem->GetSilver();
				}
			}
		}
	}
	
	if(g_pSkillAvailable->IsEnableSkill(SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI))
	{
		m_LightSight += 5;
	}
	
	if(g_pSkillAvailable->IsEnableSkill(SKILL_SWEEPER_BONUS_12))
	{
		m_LightSight += 5;
	}

	if(IsFlyingCreature())
	{
		m_LightSight += 3;
	}
	// 2004, 6, 24, sobeit add start
	if(HasEffectStatus(EFFECTSTATUS_BLINDNESS))
	{
		//m_LightSight = 1;
		PlaceInDarkness(true);
	}
	// 2004, 6, 24, sobeit add end
}

void	MPlayer::SetUndergroundCreature()
{
	MCreature::SetUndergroundCreature();
	CalculateLightSight();
}

void	MPlayer::SetGroundCreature()
{
	MCreature::SetGroundCreature();
	CalculateLightSight();
}

void	MPlayer::SetFlyingCreature()
{
	MCreature::SetFlyingCreature();
	CalculateLightSight();
}

void	MPlayer::CalculateSight()
{
	

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	MItemManager* pGear = GetGear();

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------	
	if(pGear == NULL )
		return;

	SetItemLightSight(0);

	pGear->SetBegin();

	while (pGear->IsNotEnd())
	{
		const MItem* pItem = pGear->Get();

		//----------------------------------------------------------
		
		//----------------------------------------------------------
		if (pItem->IsAffectStatus())
		{	
			const std::list<TYPE_ITEM_OPTION>& optionList = pItem->GetItemOptionList();
			std::list<TYPE_ITEM_OPTION>::const_iterator optionListItr = optionList.begin();
			while(optionListItr != optionList.end())
			{				
				ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[*optionListItr];
				//---------------------------------------------------
				
				//---------------------------------------------------
				switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
				{
				case ITEMOPTION_TABLE::PART_VISION :	
					SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); 
					break;
				}
				optionListItr++;
			}

			std::list<TYPE_ITEM_OPTION>::const_iterator itr = pItem->GetItemDefaultOptionList().begin();

			while(itr != pItem->GetItemDefaultOptionList().end())
			{
				TYPE_ITEM_OPTION option = *itr;
				ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[option];
				//---------------------------------------------------
				
				//---------------------------------------------------
				switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
				{
				case ITEMOPTION_TABLE::PART_VISION :	
					SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); 
					break;
				}
				itr++;
			}
		}
		pGear->Next();
	}
	if( HasEffectStatus(EFFECTSTATUS_LIGHTNESS) 
		|| g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		|| g_bLight
#endif
		)
	{
		
		SetItemLightSight( GetItemLightSight() + 15 );
	} else
	{
		if( HasEffectStatus( EFFECTSTATUS_FLARE ) && IsVampire() || 
			HasEffectStatus( EFFECTSTATUS_YELLOW_POISON ) && IsSlayer() ||
			HasEffectStatus( EFFECTSTATUS_YELLOW_POISON_TO_CREATURE ) )//||
//			HasEffectStatus( EFFECTSTATUS_BLINDNESS ))
			SetItemLightSight( 1 );
	}
}
// 2004, 8, 30, sobeit add start - sweep vice test
void
MPlayer::UpdateSweepVice()
{
	if(GetSweepViewValue())	
	{
		//if(IsDead() || m_nUsedActionInfo != SKILL_SWEEP_VICE_1 || m_SweepViceX == SECTORPOSITION_NULL ||	m_SweepViceY == SECTORPOSITION_NULL	)
		if(IsDead() ||!IsSlayer() || m_SweepViceX == SECTORPOSITION_NULL ||	m_SweepViceY == SECTORPOSITION_NULL	)
		{
			g_pZone->RemoveSwapViceType();
			SetSweepViewValue(0);
			return;
		}
		POINT CurrentPos = g_pTopView->GetSelectedSector(g_x, g_y);
		int TempVal = 0;
		int Distance = max(abs(m_TempSelectPosX - CurrentPos.x),abs(m_TempSelectPosY - CurrentPos.y));

		if(Distance<2)
		{
			TempVal = 1;
			if(GetSweepViewValue()!=TempVal)
			{
				g_pZone->ChangeSwapViceType(m_SweepViceX, m_SweepViceY, EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_1);
				PlaySound( SOUND_SLAYER_SWEEPVICE_RESIZE );
			}
				
		}
		else if(Distance<3)
		{
			TempVal = 2;
			if(GetSweepViewValue()!=TempVal)
			{
				g_pZone->ChangeSwapViceType(m_SweepViceX, m_SweepViceY, EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_2);
				PlaySound( SOUND_SLAYER_SWEEPVICE_RESIZE );
			}
		}
		else
		{
			TempVal = 3;
			if(GetSweepViewValue()!=TempVal)
			{
				g_pZone->ChangeSwapViceType(m_SweepViceX, m_SweepViceY, EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_3);
				PlaySound( SOUND_SLAYER_SWEEPVICE_RESIZE );
			}
		}
		SetSweepViewValue(TempVal);
	}
}

bool 
MPlayer::CheckRbuttonUpSkill()
{
	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		m_fNextTrace = FLAG_TRACE_NULL;
		UnSetRepeatAction();
		InstallTurretStopAttack();
	}
	if(GetSweepViewValue())
	{
		g_pZone->RemoveSwapViceType();
		m_nUsedActionInfo = SKILL_SWEEP_VICE_1;
		ActionEffect();
		SetSweepViewValue(0);
		m_nUsedActionInfo = SKILL_SWEEP_VICE_1;
		m_TempSelectPosX = SECTORPOSITION_NULL;
		m_TempSelectPosY = SECTORPOSITION_NULL;
		m_SweepViceX = SECTORPOSITION_NULL;
		m_SweepViceY = SECTORPOSITION_NULL;

		return true;
	}
//	else if(IsBurningSol())
//	{
//		return true;
//	}
	return false;
}
// 2004, 8, 30, sobeit add end - sweep vice test



bool MPlayer::UpdateBurningSol()
{
	if(m_bBurningSol)
	{
		if(IsDead()||!IsSlayer())
		{
			SetBurningSol(0);
			StopBurningSol();
			SetStop();
			m_nUsedActionInfo = ACTIONINFO_NULL;
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
		//	m_ActionCount = m_ActionCountMax;
			SetAction(ACTION_DIE);
			return false ;
		}
		else if(HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS)
			|| m_MoveDevice == MOVE_DEVICE_RIDE)
		{
			SetBurningSol(0);
			StopBurningSol();
			//SetStop();
			m_nUsedActionInfo = ACTIONINFO_NULL;
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1);
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2);
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3);
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4);
			return false;
		}
		if(m_TempSelectPosX == SECTORPOSITION_NULL || m_TempSelectPosY == SECTORPOSITION_NULL)
		{
			//SetBurningSol(0);
			StopBurningSol();
			m_nUsedActionInfo = ACTIONINFO_NULL;
			m_fNextTrace = FLAG_TRACE_NULL;
//			UnSetRepeatAction();
			//m_ActionCount = m_ActionCountMax;
			SetAction(ACTION_STAND);
			return false;
		}
		int bCheckFrame = 0;
		if(!IsAdvancementClass())
			bCheckFrame = IsMale()?7:11;
		else
			bCheckFrame = 7;

		m_bBurningSolCount++;

		//if(g_bRButtonDown)
		if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1)||
			HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2)||
			HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3)||
			HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4))
		{
			if(m_bBurningSolCount>=bCheckFrame)
				m_bBurningSolCount = bCheckFrame;
		}
		POINT  TempPos = g_pTopView->GetSelectedSector(g_x, g_y);
		m_TempSelectPosX = TempPos.x;
		m_TempSelectPosY = TempPos.y;
		m_Direction = MTopView::GetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY(),m_TempSelectPosX, m_TempSelectPosY);
		ChangeNearDirection();
		//else
		if(!g_bRButtonDown)
		{
			if(m_bBurningSolCount>=bCheckFrame  )
			{
//				POINT  TempPos = g_pTopView->GetSelectedSector(g_x, g_y);
//				m_TempSelectPosX = TempPos.x;
//				m_TempSelectPosY = TempPos.y;
//				m_CurrentDirection = MTopView::GetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY(),m_TempSelectPosX, m_TempSelectPosY);
//				

				bool ShowLunchEffect = true;
				int TempDistance = GetActionInfoRange(SKILL_BURNING_SOUL_LAUNCH);
				if (max(abs(m_X-m_TempSelectPosX), abs(m_Y-m_TempSelectPosY)) > TempDistance)
				{
					ShowLunchEffect = false;
				}
				
				if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_1,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX,m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1);
				}
				else if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_2,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX, m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2);
				}
				else if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_3,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX, m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3);
				}
				else if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_4,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX, m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4);
				}

				m_nUsedActionInfo = SKILL_BURNING_SOUL_LAUNCH;
				m_fTraceBuffer = FLAG_TRACE_SECTOR_SPECIAL;
				ActionToSendPacket();
				m_nNextUsedActionInfo = ACTIONINFO_NULL;
				//SetBurningSol(0);
				StopBurningSol();
				
				UnSetRepeatAction();
				m_TempSelectPosX = SECTORPOSITION_NULL;
				m_TempSelectPosY = SECTORPOSITION_NULL;
			}
		}
		m_ActionCount = m_bBurningSolCount;
			
		return true;
	#ifdef OUTPUT_DEBUG					
		DEBUG_ADD_FORMAT("[UpdateBurningSol] ActionCount  : %d", m_bBurningSolCount);
	#endif
	}
	return false;
}


bool MPlayer::UpDateInstallTurret()
{
	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		if(!IsNotDelay())
		{
			m_Action = ACTION_STAND;
			//m_CurrentDirection = 2; 
			return true;
		}
		if(IsAlive() && (m_Action == ACTION_STAND))
		{
			if (m_bInstallTurretDirect != m_CurrentDirection)
			{
				if( ((m_bInstallTurretDirect - m_CurrentDirection + 8)%8) < 4 )
				{
					m_bInstallTurretCount = ((m_bInstallTurretCount-1)+16)%16;
				}
				else
				{
					m_bInstallTurretCount = (m_bInstallTurretCount+1)%16;
				}

				if(!(m_bInstallTurretCount&0x01))
					m_bInstallTurretDirect = ((m_bInstallTurretCount /2) +2 )%8;
			}
			return true;
		}

		
		SetInstallTurretDirect(m_CurrentDirection);
		SetInstallTurretCount(((m_CurrentDirection+6)%8)*2);
	}
	return false;
}
// 2005, 1, 4, sobeit add start
void	
MPlayer::UseWildWolf_Corpse(MItem *pItem)
{
	if(pItem == NULL) return;
	m_TraceDistance = GetActionInfoRange(SKILL_WILD_WOLF);
	if (max(abs(m_X-pItem->GetX()), abs(m_Y-pItem->GetY())) <= m_TraceDistance && g_pSkillAvailable->IsEnableSkill( SKILL_WILD_WOLF ))
	{
		SetAction( ACTION_MAGIC );
		CGSkillToObject _CGSkillToObject;
		_CGSkillToObject.setSkillType( SKILL_WILD_WOLF );
		_CGSkillToObject.setCEffectID( 0 );			
		_CGSkillToObject.setTargetObjectID( pItem->GetID() );
		g_pSocket->sendPacket( &_CGSkillToObject );				
	}
}
// 2005, 1, 4, sobeit add end
