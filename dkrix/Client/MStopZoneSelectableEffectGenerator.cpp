//----------------------------------------------------------------------
// MStopZoneSelectableEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MStopZoneSelectableEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"

//#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MStopZoneSelectableEffectGenerator	g_StopZoneEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MStopZoneSelectableEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;
	bool			repeatFrame	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].RepeatFrame;

	int direction = egInfo.direction;

	//-----------------------------------------------------------
	
	
	//-----------------------------------------------------------
	if (frameID>=EFFECTSPRITETYPE_DARKNESS_1_1
		&& frameID<=EFFECTSPRITETYPE_DARKNESS_3_5 ||
		frameID >= EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 &&
		frameID <= EFFECTSPRITETYPE_GRAY_DARKNESS_3_5 )
	{
		if (egInfo.pPreviousEffect!=NULL)
		{
			TYPE_FRAMEID oldFrameID = (egInfo.pPreviousEffect)->GetFrameID();

			frameID = oldFrameID + 5;
		}
		else
		{
			if (frameID>=EFFECTSPRITETYPE_DARKNESS_2_1
				&& frameID<=EFFECTSPRITETYPE_DARKNESS_2_5)
			{
				frameID = EFFECTSPRITETYPE_DARKNESS_2_1 + rand()%5;
			}
			else if (frameID>=EFFECTSPRITETYPE_DARKNESS_1_1
				&& frameID<=EFFECTSPRITETYPE_DARKNESS_1_5)
			{
				frameID = EFFECTSPRITETYPE_DARKNESS_1_1 + rand()%5;
			}
			else if (frameID>=EFFECTSPRITETYPE_DARKNESS_3_1
				&& frameID<=EFFECTSPRITETYPE_DARKNESS_3_5)
			{
				frameID = EFFECTSPRITETYPE_DARKNESS_3_1 + rand()%5;
			}
			else if (frameID>=EFFECTSPRITETYPE_GRAY_DARKNESS_2_1
				&& frameID<=EFFECTSPRITETYPE_GRAY_DARKNESS_2_5)
			{
				frameID = EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 + rand()%5;
			}
			else if (frameID>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1
				&& frameID<=EFFECTSPRITETYPE_GRAY_DARKNESS_1_5)
			{
				frameID = EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 + rand()%5;
			}
			else if (frameID>=EFFECTSPRITETYPE_GRAY_DARKNESS_3_1
				&& frameID<=EFFECTSPRITETYPE_GRAY_DARKNESS_3_5)
			{
				frameID = EFFECTSPRITETYPE_GRAY_DARKNESS_3_1 + rand()%5;
			}
		}
	}

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (frameID==EFFECTSPRITETYPE_SWORD_WAVE_1)
	{
		direction = (direction + 1) % 8;	// +1
	}
	else if (frameID==EFFECTSPRITETYPE_SWORD_WAVE_2)
	{
		direction = (direction + 6) % 8;	// + 8 - 2
	}
	else if (frameID==EFFECTSPRITETYPE_SWORD_WAVE_3)
	{
		direction = (direction + 1) % 8;
	}
	

	//---------------------------------------------
	
	//---------------------------------------------
	TYPE_SECTORPOSITION	sX, sY;
	sX = g_pTopView->PixelToMapX(egInfo.x0);
	sY = g_pTopView->PixelToMapY(egInfo.y0);

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	MEffect*	pEffect;
	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MSelectableEffect(bltType);

	pEffect->SetFrameID( frameID, maxFrame );	

	pEffect->SetPosition(sX, sY);		
	pEffect->SetZ(egInfo.z0);			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count, egInfo.linkCount );			

	
	pEffect->SetDirection( direction );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	
	bool bAdd = g_pZone->AddEffect( pEffect );

	//---------------------------------------------
	
	
	//---------------------------------------------
	if (bAdd)
	{
		
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );
		
		if (repeatFrame)
		{
			if (maxFrame!=0) 
			{
				int num = rand() % maxFrame;
				
				for (int nf=0; nf<num; nf++)
				{
					pEffect->NextFrame();
				}
			}
		}		
	}

	return bAdd;

}
