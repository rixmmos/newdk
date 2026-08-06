//----------------------------------------------------------------------
// MStopZoneEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MStopZoneEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"
#include "MEventManager.h"
#include "SkillDef.h"

//#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MStopZoneEffectGenerator	g_StopZoneEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MStopZoneEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	int est = egInfo.effectSpriteType;

	TYPE_FRAMEID newFrameID = FRAMEID_NULL;

	//-----------------------------------------------------------
	
	
	//-----------------------------------------------------------
	
	
	if (est>=EFFECTSPRITETYPE_DARKNESS_1_1
		&& est<=EFFECTSPRITETYPE_DARKNESS_3_5 ||
		est>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 &&
		est<=EFFECTSPRITETYPE_GRAY_DARKNESS_3_5)
	{
		if (egInfo.pPreviousEffect!=NULL)
		{
			newFrameID = 5 + (egInfo.pPreviousEffect)->GetFrameID();
		}
		else
		{
			if (est>=EFFECTSPRITETYPE_DARKNESS_2_1
				&& est<=EFFECTSPRITETYPE_DARKNESS_2_5)
			{
				est = EFFECTSPRITETYPE_DARKNESS_2_1 + rand()%5;
			}
			else if (est>=EFFECTSPRITETYPE_DARKNESS_1_1
				&& est<=EFFECTSPRITETYPE_DARKNESS_1_5)
			{
				est = EFFECTSPRITETYPE_DARKNESS_1_1 + rand()%5;
			}
			else if (est>=EFFECTSPRITETYPE_DARKNESS_3_1
				&& est<=EFFECTSPRITETYPE_DARKNESS_3_5)
			{
				est = EFFECTSPRITETYPE_DARKNESS_3_1 + rand()%5;
			}
			else if (est>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 &&
				est<=EFFECTSPRITETYPE_GRAY_DARKNESS_1_5 )
			{
				est = EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 + rand()%5;
			}
			else if (est>=EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 &&
				est<=EFFECTSPRITETYPE_GRAY_DARKNESS_2_5 )
			{
				est = EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 + rand()%5;
			}
			else if (est>=EFFECTSPRITETYPE_GRAY_DARKNESS_3_1 &&
				est<=EFFECTSPRITETYPE_GRAY_DARKNESS_3_5 )
			{
				est = EFFECTSPRITETYPE_DARKNESS_3_1 + rand()%5;
			}
		}
	}
	else if(est == EFFECTSPRITETYPE_INFINITY_THUNDERBOLT_CENTER)
	{
		est = EFFECTSPRITETYPE_INFINITY_THUNDERBOLT_CENTER + rand()%3;
	}

	if (est >= g_pEffectSpriteTypeTable->GetSize())
	{
		return false;
	}

	
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;
	bool			repeatFrame	= (*g_pEffectSpriteTypeTable)[est].RepeatFrame;

	if (newFrameID != FRAMEID_NULL)
	{
		frameID = newFrameID;
	}

	int direction = egInfo.direction;


	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (est==EFFECTSPRITETYPE_SWORD_WAVE_1)
	{
		direction = (direction + 1) % 8;	// +1
	}
	else if (est==EFFECTSPRITETYPE_SWORD_WAVE_2)
	{
		direction = (direction + 6) % 8;	// + 8 - 2
	}
	else if (est==EFFECTSPRITETYPE_SWORD_WAVE_3)
	{
		direction = (direction + 1) % 8;
	}

	
	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if(est == EFFECTSPRITETYPE_METEOR_ROCK)
	{
		MEvent event;
		event.eventID = EVENTID_METEOR_SHAKE;
		event.eventType = EVENTTYPE_ZONE;
		event.eventDelay = 63;
		event.eventFlag = EVENTFLAG_SHAKE_SCREEN;
		event.parameter3 = 3;
		g_pEventManager->AddEvent(event);
	}

	//---------------------------------------------
	
	//---------------------------------------------
	TYPE_SECTORPOSITION	sX, sY;
	sX = g_pTopView->PixelToMapX(egInfo.x0);
	sY = g_pTopView->PixelToMapY(egInfo.y0);

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	
	
	if( egInfo.nActionInfo == RESULT_SKILL_STONE_AUGER || 
		egInfo.nActionInfo == RESULT_STEP_SKILL_STONE_AUGER_2 ||
		egInfo.nActionInfo == RESULT_STEP_SKILL_STONE_AUGER_3 )
	{
		
		POINT pt[] = 
		{
			{ 0, 1 },
			{ 1, 0 },
			{ 0, 0 },
			{ 0, -1 },
			{ -1, 0 }
		};

		sX = g_pTopView->PixelToMapX(egInfo.x1);
		sY = g_pTopView->PixelToMapY(egInfo.y1);

		for(int i=0;i<sizeof(pt)/sizeof(POINT);i++)
		{
			MEffect* pEffect;

			pEffect = new MEffect( bltType );

			pEffect->SetFrameID( frameID, maxFrame );

			pEffect->SetPosition(sX+ pt[i].x, sY+pt[i].y );
			pEffect->SetZ( egInfo.z0 );

			pEffect->SetStepPixel( egInfo.step );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetDirection( direction );
			pEffect->SetPower( egInfo.power);
			pEffect->SetMulti( true );

			if( g_pZone->AddEffect( pEffect ) )
			{
				if( egInfo.pEffectTarget != NULL )
				{
					if( i == 0 )
						pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget);
					else
					{
						MEffectTarget *pTarget = new MEffectTarget(*egInfo.pEffectTarget);
						pEffect->SetLink( egInfo.nActionInfo, pTarget );
					}
				}

				if (repeatFrame)
				{
					int num = rand() % maxFrame;
					
					for (int nf=0; nf<num; nf++)
					{
						pEffect->NextFrame();
					}
				}	
			}
		}
		return true;
		
	} else
	{
			MEffect*	pEffect;
			//---------------------------------------------
			
			//---------------------------------------------
			pEffect = new MEffect(bltType);

			// Debug: Log effect creation
			DEBUG_ADD_FORMAT("StopZoneEffect: Created pEffect=%p, bltType=%d", pEffect, bltType);

			pEffect->SetFrameID( frameID, maxFrame );

			// Debug: Verify effect state before AddEffect
			#ifdef __SANITIZE_ADDRESS__
			// With ASAN, add extra validation
			if (pEffect) {
				TYPE_FRAMEID testFrameID = pEffect->GetFrameID();
				DEBUG_ADD_FORMAT("StopZoneEffect: pEffect=%p FrameID=%d before AddEffect", pEffect, testFrameID);
			}
			#endif	
			if((est == EFFECTSPRITETYPE_FIRE_CRACKER_1 ||
				est == EFFECTSPRITETYPE_FIRE_CRACKER_2 ||
				est == EFFECTSPRITETYPE_FIRE_CRACKER_3 ) ||
				(est >= EFFECTSPRITETYPE_DRAGON_FIRE_CRACKER && est <= EFFECTSPRITETYPE_FIRE_CRACKER_4) ||
				est == EFFECTSPRITETYPE_PET_SLAYER_3TH_ATTACK_1 || est == EFFECTSPRITETYPE_PET_SLAYER_3TH_ATTACK_2
				)
				pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0 );
			else
				pEffect->SetPosition(sX, sY);		
			pEffect->SetZ(egInfo.z0);			
			pEffect->SetStepPixel(egInfo.step);		
			pEffect->SetCount( egInfo.count, egInfo.linkCount );			
			
			
			pEffect->SetDirection( direction );
			
			
			pEffect->SetPower(egInfo.power);
			
			
			//pEffect->SetLight( light );
			
			
			if(
				(est == EFFECTSPRITETYPE_FIRE_CRACKER_1 ||
				est == EFFECTSPRITETYPE_FIRE_CRACKER_2 ||
				est == EFFECTSPRITETYPE_FIRE_CRACKER_3 ) ||
				(est >= EFFECTSPRITETYPE_DRAGON_FIRE_CRACKER && est <= EFFECTSPRITETYPE_FIRE_CRACKER_4) ||
				est == EFFECTSPRITETYPE_GREAT_RUFFIAN_2_AXE_THROW ||
				est == EFFECTSPRITETYPE_GREAT_RUFFIAN_2_AXE_THROW_SHADOW ||
				est == EFFECTSPRITETYPE_GREAT_RUFFIAN_1_AXE_GROUND ||
				est == EFFECTSPRITETYPE_GREAT_RUFFIAN_1_AXE_WAVE||
				est == EFFECTSPRITETYPE_NEW_PLASMA_ROCKET_LAUNCHER_BLOW 
//				|| est == EFFECTSPRITETYPE_ACID_ERUPTION_1
//				|| est == EFFECTSPRITETYPE_ACID_ERUPTION_2
//				|| est == EFFECTSPRITETYPE_ACID_ERUPTION_3
//				|| est == EFFECTSPRITETYPE_ACID_ERUPTION_4
//				|| est == EFFECTSPRITETYPE_ACID_ERUPTION_5
				)
				pEffect->SetMulti(true);
		bool bAdd = g_pZone->AddEffect( pEffect );

		if (bAdd)
		{
			
			pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );
		
			//---------------------------------------------
			
			
			//---------------------------------------------
			if (repeatFrame)
			{
				int num = rand() % maxFrame;
				
				for (int nf=0; nf<num; nf++)
				{
					pEffect->NextFrame();
				}
			}	
		}
		return bAdd;
	}
	return false;
}
