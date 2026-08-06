//#define __3D_IMAGE_OBJECT__					// by sonee
//----------------------------------------------------------------------
// MTopViewDraw.cpp
//----------------------------------------------------------------------
//


//----------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)

#include <math.h>
#include <list>
#include <stdio.h>
#include <fstream>
#include "MZone.h"
#include "MCreature.h"
#include "MFakeCreature.h"
#include "MPlayer.h"
#include "MItem.h"
#include "MImageObject.h"
#include "MAnimationObject.h"
#include "MShadowAnimationObject.h"
#include "assert.h"

#include "MMovingEffect.h"
#include "MScreenEffect.h"
#include "MTopView.h"
#include "MItemTable.h"
#include "MCreatureTable.h"
#include "MWeather.h"
#include "CMessageArray.h"
#include "DXLib.h"
#include "SP.h"
#include "FL2.h"
//#include "2D.h"
#include "ClientConfig.h"
#include "VS_UI_BASE.h"
#include "VS_UI_Mouse_Pointer.h"
#include "UIFunction.h"
#include "AddonDef.h"
#include "MZoneTable.h"
#include "MGameTime.h"
#include "UserOption.h"
#include "UserInformation.h"
#include "SpriteIDDef.h"
#include "MGameStringTable.h"
//#include "MZoneInfo.h"
#include "MObjectSelector.h"
#include "MPortal.h"
#include "EffectSpriteTypeDef.h"
#include "MScreenEffectManager.h"
#include "MEffectSpriteTypeTable.h"
#include "Profiler.h"
//#include "MFileDef.h"
#include "Packet/Properties.h"
#include "MEventManager.h"
#include "DebugInfo.h"
#include "MItemOptionTable.h"
#include "Client.h"
#include "SkillDef.h"
#include "vs_ui_item.h"
#include "MTopViewDraw.inl"

extern int defaultSkinColor;
extern int defaultCoatColor;
extern int defaultTrouserColor;

namespace {
	POINT pointTemp;
	RECT rect;
};


int GetAdvancementPartFromItemClass( ITEM_CLASS itemClass , TYPE_FRAMEID frameID);
int ConvAdvancementSlayerActionFromSlayerAction( int CurAction, MCreatureWear* pCreatureWear );
int ConvAdvancementOustersActionFromOusterAction( int CurAction, bool bChakram );
int GetAdvancementVampireActionFromVampireAction( int CurAction, const MCreature* pCreature );
BYTE GetCreatureActionCountMax( const MCreature* pCreature, int action );

//----------------------------------------------------------------
// Draw CreatureShadow
//----------------------------------------------------------------


//----------------------------------------------------------------
void
MTopView::DrawCreatureShadow(POINT* pPoint, MCreature* pCreature)
{
	if(g_pEventManager->GetEventByFlag(EVENTFLAG_NOT_DRAW_CREATURE_SHADOW)
		|| pCreature->IsGhost(1) && pCreature->IsGhost(2) && pCreature->IsGhost(4))
		return;

	if(pCreature->IsOusters() && pCreature->IsPlayer() && g_pPlayer->IsOusters() == false && pCreature->IsInGroundElemental())
	{
		return;
	}

	
	if(pCreature->HasEffectStatus(EFFECTSTATUS_GHOST))return;

	
	if(pCreature->IsFakeCreature())
	{
		MCreature *OwnerCreature = g_pZone->GetCreature(((MFakeCreature*)pCreature)->GetOwnerID());
		if(OwnerCreature)
		{
			if(OwnerCreature->IsInDarkness())
				return;
		} 
	}
	// 2004, 6, 7 sobeit add end

	if(
		(g_pPlayer->HasEffectStatus(EFFECTSTATUS_YELLOW_POISON_TO_CREATURE) || 
		g_pPlayer->HasEffectStatus(EFFECTSTATUS_FLARE ) ||
		g_pPlayer->HasEffectStatus(EFFECTSTATUS_BLINDNESS))
		&& pCreature != g_pPlayer 
		)
	{
		
		int sx,sy,ex,ey;

		sx = g_pPlayer->GetX() - 1;
		ex = g_pPlayer->GetX() + 1;

		sy = g_pPlayer->GetY() - 1;
		ey = g_pPlayer->GetY() + 1;

		if( pCreature->GetX() < sx || pCreature->GetX() > ex ||
			pCreature->GetY() < sy || pCreature->GetY() > ey )
			return;
	}

	#ifdef OUTPUT_DEBUG_DRAW_PROCESS
		DEBUG_ADD( "Start DrawCreatureShadow" );
	#endif

	bool bBlendingShadow = false;
	bool bSlayerPet_ShowTurret = false;

	int body, action, direction, frame, creature_type, frameCount;

	frameCount  = pCreature->GetCreatureFrameCount();

	for(int FrameIndex = 0; FrameIndex< frameCount ; FrameIndex++)
	{
		action		= pCreature->GetAction();		
		body		= pCreature->GetCreatureFrameID(FrameIndex);
		direction	= pCreature->GetDirection();
		frame		= pCreature->GetFrame();
		creature_type = pCreature->GetCreatureType();

		// 2004, 7, 23 sobeit modify start
		switch(creature_type)
		{
		case 672:
			if(action == ACTION_MOVE )
			{
				frame = (g_CurrentFrame + pCreature->GetID()) % 11;	
			}
			break;
		case 717:
		case 721:
		case 723:
			if( pCreature->HasEffectStatus( EFFECTSTATUS_GDR_FLOATING )/* && action == ACTION_MOVE */)
			{
				action = ACTION_VAMPIRE_DRAIN;
				frame = 6 + (frame & 0x07);		
			}
			if(pCreature->IsDead())
			{
				if(pCreature->GetActionCount()>=pCreature->GetActionCountMax())
				{
					if(creature_type == 723)
						frame = 46; 
				}
			}
			break;
		case 722:
			return;
		case 636:
			{
				action = 6;
				frame = 14;
			}
			break;
		case 698:
			{
				if(!pCreature->HasEffectStatus(EFFECTSTATUS_COMA) && !pCreature->IsDead())
				{
					action = ACTION_STAND;
				}
				else
					action = ACTION_DIE;
				
				direction = 2;
			}
			break;
		case 433:
			{
				if(!pCreature->HasEffectStatus(EFFECTSTATUS_COMA) && !pCreature->IsDead())
					action = ACTION_STAND;
				else
					action = ACTION_DIE;
				
				direction = 1;
			}
			break;
		case 674:
			bBlendingShadow = true;
			break;
		case 675:
			{
				action = ACTION_MAGIC;
				direction = 3;
				frame = 0;
				bBlendingShadow= true;
			}
			break;
		case 702:
		case 703:
		case 704:
			if( action == ACTION_STAND || action == ACTION_MOVE )
				bSlayerPet_ShowTurret = true;
			break;
		case 730:
		case 731:
		case 732: 
			{
				if(pCreature->IsDead()) return;
				action = ACTION_STAND;
				frame = 0;
				direction = 2;
				bBlendingShadow= true;
			}
			break;
		case 743:
		case 744:
		case 745:
		case 746:
		case 747:
		case 748: 
			{
				if(pCreature->IsDead())
				{
					action = ACTION_DIE;
					direction = 2;
					if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;
				}
				else
				{
					action = ACTION_STAND;
					frame = 0;
					direction = 2;
				}
			}
			break;
		case 738: 
		case 739: 
		case 741: 
			{ 
	//			if(pCreature->IsDead()) return;
	
	//			{
	//				if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) )
	//					return;
	//			}
				action = ACTION_STAND;
				frame = 0;
				direction = 2;
				bBlendingShadow= true;
			}
			break;
		case 740: 
			{
	//			if(pCreature->IsDead()) return;
	
	//			{
	//				if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) || 
	//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) )
	//					return;
	//			}
				action = ACTION_STAND;
				frame = 0;
				direction = 1;
			}
		case 734: 
			{
				direction = 1;
				if(pCreature->IsAlive())
				{
					action = ACTION_STAND;
					frame = 0;
				}
			}
			break;
		case 753:
			{
				direction = 2;
				if(pCreature->IsDead())
				{
					action = ACTION_DIE;
					if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;
					
				}
			}
			break;
		case 764:
		case 765: 
			if(action == ACTION_DIE && creature_type == 765)
				direction = 2;
			
			if(action == ACTION_DRAINED)
			{
				action = ACTION_ATTACK;
				if(frame>4)
				{
					frame = 4;
					
					if(g_CurrentFrame&0x01)
					{
						direction = ((direction - 1 ) + 8)%8;
						pCreature->SetCurrentDirection(direction);
					}
				}
			}
			break;
		case 767:
			frame = 0;
			action = ACTION_MOVE;
			break;
		}

		bool isSlayerCharacter = pCreature->IsSlayerCharacter();
		bool isOustersCharacter = pCreature->IsOusters() && pCreature->GetCreatureType() != CREATURETYPE_OUSTERS_OPERATOR
								&&	creature_type != 717 && creature_type != 721 && creature_type != 723;

		//----------------------------------------------------------
		
		//----------------------------------------------------------
		if (pCreature->IsHallu())
		{
			
			pCreature->DetermineHalluActionFrame();

			isSlayerCharacter = false;
			isOustersCharacter = false;

			body	= pCreature->GetHalluCreatureFrameID();
			action = pCreature->GetHalluAction();
			frame = pCreature->GetHalluFrame();
			creature_type = pCreature->GetHalluCreatureType();
		}

		
		if(
			creature_type >= 371 && creature_type <= 376 || 
			creature_type >= 560 && creature_type <= 563 || 
			creature_type == 482 || 
			creature_type >= 526 && creature_type <= 549 || 
			creature_type >= 550 && creature_type <= 557 || 
			creature_type == 650 || 
			creature_type >= 660 && creature_type <= 669 ||
			creature_type == 672 ||
			creature_type == 673
			)
		{
			direction = 2;
			bBlendingShadow = true;
		}


		
		 	

		bool bPlayerVampire = 
				(*g_pCreatureSpriteTable)[(*g_pCreatureTable)[pCreature->GetCreatureType()].SpriteTypes[0]].IsPlayerVampireOnlySprite()
	//			&& (pCreature==g_pPlayer || pCreature->HasEffectStatus(EFFECTSTATUS_COMA))
				&& pCreature->GetActionCount()>=pCreature->GetActionCountMax()	
				&& !pCreature->IsExistAttachEffect()
				&& pCreature->IsDead();

		bool bTeenVersion = 
				(
					g_pUserInformation->GoreLevel == false
					&& !(
					(creature_type >= 526 && creature_type <= 549) || 
					(
					creature_type >= 371 && creature_type <= 376 || 
					creature_type >= 560 && creature_type <= 563
					) || 
					creature_type == 482 || 
					creature_type == 650 || 
					creature_type == 670 || 
					creature_type == 672 ||
					creature_type == 673 ||
					creature_type == 730 || 
					creature_type == 731 || 
					creature_type == 732 || 
					creature_type == 636 
					)	
					|| (creature_type >= 377 && creature_type <= 386)
					|| creature_type == 480 
				)
				&&
				(
					pCreature->GetActionCount()>=pCreature->GetActionCountMax()/2
					&& pCreature->IsDead()
				)
				&&
				(
					creature_type < 431		
					|| creature_type > 436	
				);
		

		//---------------------------------------------------------
		
		//
		
		
		//---------------------------------------------------------	
		if (bPlayerVampire || bTeenVersion)
		{
			if(pCreature->IsSlayer())
			{
				CSprite *pSprite = &m_EtcSPK[ SPRITEID_SLAYER_CROSS_SHADOW ];
				
				POINT pointTemp;
				pointTemp.x = pPoint->x - 5;
				pointTemp.y = pPoint->y;				
				m_pSurface->BltSprite(&pointTemp, pSprite);	
			}

			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("EndDrawCreatureShadow-S");
			#endif

			return;		
		}

		//------------------------------------------------
		
		//------------------------------------------------
		if (pCreature->IsInDarkness() && !pCreature->IsNPC() && !g_pPlayer->IsVampire()
			&& g_pPlayer!=pCreature && (!g_pPlayer->HasEffectStatus( EFFECTSTATUS_LIGHTNESS)|| g_pZone->GetID() == 3001) )
		{
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("EndDrawCreatureSha-D");
			#endif

			return;
		}
			
		POINT pointTemp;


		//------------------------------------------------
		
		
		//------------------------------------------------
		bool bPerfectCasket = pCreature->IsInCasket() && pCreature->GetCasketCount()==0;
		if (pCreature->IsUndergroundCreature()
			
			|| pCreature->IsCutHeight()
			
			|| bPerfectCasket
			//|| pCreature->GetInvisibleCount()==64
			)
		{
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD( "End DrawCreatureShadow" );
			#endif

			//----------------------------------------------------
			
			//----------------------------------------------------
			if (bPerfectCasket)
			{
				
				int casketType = pCreature->GetCasketType();
				int casketID = SPRITEID_CASKET_1 + pCreature->GetCasketType()*2 + 1;

				if (casketID < m_EtcSPK.GetSize())
				{
					CSprite* pSpriteImage = &m_EtcSPK[casketID-1];
					CSprite* pSprite = &m_EtcSPK[casketID];

					if (pSprite!=NULL)
					{
						
						pointTemp.x = pPoint->x + 24 - (pSpriteImage->GetWidth()>>1);

						pointTemp.y = pPoint->y + TILE_Y - pSpriteImage->GetHeight();

						const POINT shadowPoint[] =
						{
							{ -27, 28 },
							{ -27, 23 },
							{ -31, 28 }
						};

						pointTemp.x += shadowPoint[casketType].x;
						pointTemp.y += shadowPoint[casketType].y;

						m_pSurface->BltSprite(&pointTemp, pSprite);					
					}
				}
			}

			
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("EndDrawCreature-C");
			#endif

			return;
		}

		//register int i;

		//----------------------------------------------------------
		
		//----------------------------------------------------------
		if (!pCreature->HasHead() 
			&& pCreature->GetActionCount()==pCreature->GetActionCountMax())
		{	
			if (action==ACTION_DIE)
			{
				
				//frame	= (*g_pCreatureTable)[creature_type].GetActionCount( action );
				frame	= GetCreatureActionCountMax( pCreature, action );
			}
		}
		
		
		
		if(g_pUserInformation->GoreLevel == false && action == ACTION_DIE
				&&
				(
					creature_type < 431		
					|| creature_type > 436	
				)
			)
		{
			frame = 0;
		}

		

		
		//pPoint->y -= pCreature->GetZ();

		//------------------------------------------------
		//
		
		//
		//------------------------------------------------
			
		//------------------------------------------------
		//
		
		//
		//------------------------------------------------
		if (body!=FRAMEID_NULL)
		{
			bool bBat = pCreature->GetCreatureType() == CREATURETYPE_BAT;
			bool bWolf = pCreature->GetCreatureType() == CREATURETYPE_WOLF;
			bool bWerWolf = pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF;

			if( pCreature->IsAdvancementClass() && !bBat && !bWolf && !bWerWolf )
				DrawShadowAdvancementClassVampireCharacter( pPoint, pCreature, action, direction, frame, body, bBlendingShadow, bSlayerPet_ShowTurret );		
			else
				DrawShadowVampireCharacter( pPoint, pCreature, action, direction, frame, body, bBlendingShadow, bSlayerPet_ShowTurret );		
			
		}	
		
		//----------------------------------------
		//
		
		//
		//----------------------------------------
		
		
		
		//if (pCreature->IsWear())
		if (isSlayerCharacter)
		{
			if( pCreature->IsAdvancementClass() )
				DrawShadowAdvancementClassSlayerCharacter( pPoint, pCreature, action, direction, frame );
			else
				DrawShadowSlayerCharacter( pPoint, pCreature, action, direction, frame );
		}
		else if(isOustersCharacter)
		{
			if( pCreature->IsAdvancementClass() )
				DrawShadowAdvancementClassOustersCharacter( pPoint, pCreature, action, direction, frame );
			else
				DrawShadowOustersCharacter( pPoint, pCreature, action, direction, frame );
			
		}
	}
	#ifdef OUTPUT_DEBUG_DRAW_PROCESS
		DEBUG_ADD( "End DrawCreatureShadow" );
	#endif

	
}

void	MTopView::DrawShadowSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	// Check if m_AddonShadowFPK is initialized (fix crash in SDL backend)
	// CCreatureFramePack doesn't have IsNotInit(), so we check m_Size
	if (m_AddonShadowFPK.GetSize() == 0) {
		return;
	}

	if(pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		//frame = pCreature->GetInstallTurretCount();
		if(ACTION_STAND == action)
		{
			direction = 2;
			frame = pCreature->GetInstallTurretCount();
		}
		FRAME_ARRAY& FA = m_CreatureShadowFPK[222][action][direction];
		
		if (FA.GetSize() > frame)
		{			
			CFrame&	Frame = FA[frame];
			
			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			
			CShadowSprite* pSprite = &m_CreatureSSPK[ sprite ];
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();			
			
			m_pSurface->BltShadowSprite(&pointTemp, pSprite);
			
		}
	}
	else
	{
		MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;
		
		
		//action = pCreature->GetAction();
		
		
		//int id = pCreature->GetID();
		
		WORD clothes;
		BYTE clothesType;
		for (int i=0; i<ADDON_MAX; i++)
		{
			
			
			clothesType = MCreatureWear::s_AddonOrder[pCreature->GetDirection()][i];
			
			
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(clothesType);
			
			if (addonInfo.bAddon)
			{
				clothes = addonInfo.FrameID;
				
				
				FRAME_ARRAY &FA = m_AddonShadowFPK[clothes][action][direction];
				
				
				if (FA.GetSize() > frame)
				{
					CFrame &Frame = FA[frame];
					
					int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
					int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
					int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
					
					//CIndexSprite* pSprite = &(*pAddonSPK)[ sprite ];					
					
					
					//if (pSprite->IsNotInit())
					//	continue;
					
					pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
					pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
					CShadowSprite* pSprite = &m_AddonSSPK[ sprite ];
						
					if (g_pUserOption->BlendingShadow)
					{
						m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
					}
					else
					{
						m_pSurface->BltShadowSprite( &pointTemp, pSprite );
					}
				}
			}
		}
	}
}

void	MTopView::DrawShadowVampireCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int body, bool bBlendingShadow , bool bSlayerPet_ShowTurret )
{
	FRAME_ARRAY &FA = m_CreatureShadowFPK[body][action][direction];
	
	if (FA.GetSize() > frame)
	{
		CFrame &Frame = FA[frame];
		
		int sprite = Frame.GetSpriteID(),
			cx		= Frame.GetCX(),
			cy		= Frame.GetCY();
		
		
		pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
		CShadowSprite* pSprite = &m_CreatureSSPK[sprite];
		
		//				if (pSprite->IsNotInit())
		//				{
		//					LoadFromFileCreatureActionSPK( body, action );
		//				}
		
		if (bBlendingShadow)
		{
			m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
		}
		else
		{
			m_pSurface->BltShadowSprite( &pointTemp, pSprite );
		}
	}		
	
	
	if( bSlayerPet_ShowTurret )
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)pCreature;
		BYTE direct = pFakeCreature->GetTurretDirection();
		
		FRAME_ARRAY& FA = m_CreatureShadowFPK[body][ACTION_VAMPIRE_DRAIN][2];
		
		CFrame& Frame =	FA[direct];
		int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
			cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
			cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
		
		if( pCreature->GetCreatureType() == 702 )
		{
			POINT pointGap[8] = 
			{
				{ 11, 6 },	
				{ 8, 2 },	
				{ 0, 0 },	
				{ -7, 2 },	
				{ -10, 6 },	
				{ -7, 10 },	
				{ 0, 12 },	
				{ 8, 10 },	
			};
			cx += pointGap[pFakeCreature->GetDirection()].x;
			cy += pointGap[pFakeCreature->GetDirection()].y;
		}
		if( pFakeCreature->GetTurretDelay() > 0 )
		{
			extern POINT g_DirectionValue[MAX_DIRECTION];
			BYTE direct = (pFakeCreature->GetTurretDirection()/4+2)%8;
			cx -= g_DirectionValue[direct].x*pFakeCreature->GetTurretDelay()*2;
			cy -= g_DirectionValue[direct].y*pFakeCreature->GetTurretDelay()*2;
		}
		
		
		pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
		
		CShadowSprite* pSprite = &m_CreatureSSPK[ sprite ];
		
		if (bBlendingShadow)
		{
			m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
		}
		else
		{
			m_pSurface->BltShadowSprite( &pointTemp, pSprite );
		}
	}
}

void	MTopView::DrawShadowOustersCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	
	int tempAction = action;
	if(bChakram == false)
	{
		switch(tempAction)
		{
		case ACTION_STAND:
			tempAction = ACTION_OUSTERS_STAND;
			break;
			
		case ACTION_MOVE:
			tempAction = ACTION_OUSTERS_MOVE;
			break;
		}
	}
	
	
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	
	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{
		int clothes = addonInfo.FrameID;
		
		FRAME_ARRAY &FA = m_OustersShadowFPK[clothes][tempAction][direction];
		
		
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];
			
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//				if(true)
			//				{
			//					m_pSurface->BltShadowSprite( &pointTemp, &(m_pOustersShadowManager->GetSprite(sprite)) );
			//				}
			//				else
			{
				CShadowSprite* pSprite = &m_OustersSSPK[ sprite ];
				
				if (g_pUserOption->BlendingShadow)
				{
					m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
				}
				else
				{
					m_pSurface->BltShadowSprite( &pointTemp, pSprite );
				}
			}
		}
	}
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		int clothes = addonInfoChakram.FrameID;
		
		FRAME_ARRAY &FA = m_OustersShadowFPK[clothes][tempAction][direction];
		
		
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CShadowSprite* pSprite = &m_OustersSSPK[ sprite ];					
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//				if(true)
			//				{
			//					m_pSurface->BltShadowSprite( &pointTemp, &(m_pOustersShadowManager->GetSprite(sprite)) );
			//				}
			//				else
			{
				
				if (g_pUserOption->BlendingShadow)
				{
					m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
				}
				else
				{
					m_pSurface->BltShadowSprite( &pointTemp, pSprite );
				}
			}
			
		}
	}
}

void	MTopView::DrawShadowAdvancementClassSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	if(pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		//frame = pCreature->GetInstallTurretCount();
		if(ACTION_STAND == action)
		{
			direction = 2;
			frame = pCreature->GetInstallTurretCount();
		}
		FRAME_ARRAY& FA = m_CreatureShadowFPK[222][action][direction];
		
		if (FA.GetSize() > frame)
		{			
			CFrame&	Frame = FA[frame];
			
			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			
			CShadowSprite* pSprite = &m_CreatureSSPK[ sprite ];
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();			
			
			m_pSurface->BltShadowSprite(&pointTemp, pSprite);
			
		}
	}
	else
	{
		MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;
		
		action = ConvAdvancementSlayerActionFromSlayerAction( action, dynamic_cast< MCreatureWear* >(pCreature) );

		if( action == -1 )
			return;
		else
			action -= ADVANCEMENT_ACTION_START;
		
		int clothes;
		BYTE clothesType;

		CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerManShadowFPK : m_AdvancementSlayerWomanShadowFPK;
		CShadowSpritePack& addonSSPK = pCreature->IsMale() ? m_AdvancementSlayerManSSPK : m_AdvancementSlayerWomanSSPK;

		for (int i=0; i<AC_ADDON_MAX; i++)
		{
			
			
			clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];
			
			
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo(clothesType);
			
			if (addonInfo.bAddon || i == AC_ADDON_BODY )
			{
				clothes = GetAdvancementPartFromItemClass( addonInfo.ItemClass , addonInfo.FrameID);		

				if( clothes == -1 )
					continue;
				
				FRAME_ARRAY &FA = slayerFPK[clothes][action][direction];
				
				
				if (FA.GetSize() > frame)
				{
					CFrame &Frame = FA[frame];
					
					int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
					int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
					int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
					
					//CIndexSprite* pSprite = &(*pAddonSPK)[ sprite ];					
					
					
					//if (pSprite->IsNotInit())
					//	continue;
					
					pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
					pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
					CShadowSprite* pSprite = &addonSSPK[ sprite ];
						
					if (g_pUserOption->BlendingShadow)
					{
						m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
					}
					else
					{
						m_pSurface->BltShadowSprite( &pointTemp, pSprite );
					}
				}
			}
		}
	}
}

void	MTopView::DrawShadowAdvancementClassVampireCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int body, bool bBlendingShadow , bool bSlayerPet_ShowTurret )
{		
	CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? m_AdvancementVampireManShadowFPK : m_AdvancementVampireWomanShadowFPK;
	CShadowSpritePack& advanceVampireSSPK = pCreature->IsMale() ? m_AdvancementVampireManSSPK : m_AdvancementVampireWomanSSPK;

	action = GetAdvancementVampireActionFromVampireAction( action, pCreature );

	if( action == -1 )
		return;
	else
		action -= ADVANCEMENT_ACTION_START;


	FRAME_ARRAY &FA = advanceVampireFPK[0][action][direction];
	
	if (FA.GetSize() > frame)
	{
		CFrame &Frame = FA[frame];
		
		int sprite	= Frame.GetSpriteID(),
			cx		= Frame.GetCX(),
			cy		= Frame.GetCY();
		
		
		pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
		CShadowSprite* pSprite = &advanceVampireSSPK[sprite];
		
		//				if (pSprite->IsNotInit())
		//				{
		//					LoadFromFileCreatureActionSPK( body, action );
		//				}
		
		if (bBlendingShadow)
		{
			m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
		}
		else
		{
			m_pSurface->BltShadowSprite( &pointTemp, pSprite );
		}
	}		

	bool bWeapon = false;
	
	bool bBat = pCreature->GetCreatureType() == CREATURETYPE_BAT;
	bool bWolf = pCreature->GetCreatureType() == CREATURETYPE_WOLF;
	bool bWerWolf = pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF;

	if( bWeapon && !bBat && !bWolf && !bWerWolf )
	{
		FRAME_ARRAY &FAW = advanceVampireFPK[1][action][direction];
	
		if (FAW.GetSize() > frame)
		{
			CFrame &Frame = FAW[frame];
			
			int sprite	= Frame.GetSpriteID(),
				cx		= Frame.GetCX(),
				cy		= Frame.GetCY();
			
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			CShadowSprite* pSprite = &advanceVampireSSPK[sprite];
			
			//				if (pSprite->IsNotInit())
			//				{
			//					LoadFromFileCreatureActionSPK( body, action );
			//				}
			
			if (bBlendingShadow)
			{
				m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
			}
			else
			{
				m_pSurface->BltShadowSprite( &pointTemp, pSprite );
			}
		}
	}
	
	
	if( bSlayerPet_ShowTurret )
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)pCreature;
		BYTE direct = pFakeCreature->GetTurretDirection();
		
		FRAME_ARRAY& FA = m_CreatureShadowFPK[body][ACTION_VAMPIRE_DRAIN][2];
		
		CFrame& Frame =	FA[direct];
		int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
			cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
			cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
		
		if( pCreature->GetCreatureType() == 702 )
		{
			POINT pointGap[8] = 
			{
				{ 11, 6 },	
				{ 8, 2 },	
				{ 0, 0 },	
				{ -7, 2 },	
				{ -10, 6 },	
				{ -7, 10 },	
				{ 0, 12 },	
				{ 8, 10 },	
			};
			cx += pointGap[pFakeCreature->GetDirection()].x;
			cy += pointGap[pFakeCreature->GetDirection()].y;
		}
		if( pFakeCreature->GetTurretDelay() > 0 )
		{
			extern POINT g_DirectionValue[MAX_DIRECTION];
			BYTE direct = (pFakeCreature->GetTurretDirection()/4+2)%8;
			cx -= g_DirectionValue[direct].x*pFakeCreature->GetTurretDelay()*2;
			cy -= g_DirectionValue[direct].y*pFakeCreature->GetTurretDelay()*2;
		}
		
		
		pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
		
		CShadowSprite* pSprite = &m_CreatureSSPK[ sprite ];
		
		if (bBlendingShadow)
		{
			m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
		}
		else
		{
			m_pSurface->BltShadowSprite( &pointTemp, pSprite );
		}
	}
}

void	MTopView::DrawShadowAdvancementClassOustersCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
		
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	
	int tempAction = ConvAdvancementOustersActionFromOusterAction( action, bChakram );		

	if( tempAction == -1 )
		return;
	else
		tempAction -= ADVANCEMENT_ACTION_START;
	
	
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	
	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{
		int clothes = addonInfo.FrameID;
		
		FRAME_ARRAY &FA = m_AdvancementOustersShadowFPK[1][tempAction][direction];
		
		
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];
			
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			CShadowSprite* pSprite = &m_AdvancementOustersSSPK[ sprite ];
			
			if (g_pUserOption->BlendingShadow)
			{
				m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
			}
			else
			{
				m_pSurface->BltShadowSprite( &pointTemp, pSprite );
			}
		}
	}
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		int clothes = addonInfoChakram.FrameID;
		
		FRAME_ARRAY &FA = m_AdvancementOustersShadowFPK[0][tempAction][direction];
		
		
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CShadowSprite* pSprite = &m_AdvancementOustersSSPK[ sprite ];
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			if (g_pUserOption->BlendingShadow)
			{
				m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
			}
			else
			{
				m_pSurface->BltShadowSprite( &pointTemp, pSprite );
			}
		}
	}
}
