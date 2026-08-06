//----------------------------------------------------------------------
// MAttachZoneSelectableEffectGenerator.cpp
//----------------------------------------------------------------------

//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MAttachZoneSelectableEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
//#define	new			DEBUG_NEW

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MAttachZoneSelectableEffectGenerator	g_StopZoneEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MAttachZoneSelectableEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	int est = egInfo.effectSpriteType;
	
	//---------------------------------------------
	
	//---------------------------------------------
	 
	// 2001.10.6
	POINT pixelPoint = { egInfo.x0, egInfo.y0 };

	
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	if (est==EFFECTSPRITETYPE_BLOOD_GROUND_2_1)
	{
		est = EFFECTSPRITETYPE_BLOOD_GROUND_2_1 + rand()%4;

		pixelPoint.x += (rand()%24) - (24>>1);
		pixelPoint.y += (rand()%24) - (24>>1);
	}
	else if (est==EFFECTSPRITETYPE_BLOOD_GROUND_1_1)
	{
		est = EFFECTSPRITETYPE_BLOOD_GROUND_1_1 + rand()%5;

		pixelPoint.x += (rand()%24) - (24>>1);
		pixelPoint.y += (rand()%24) - (24>>1);
	}

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;



	//---------------------------------------------
	
	//---------------------------------------------
	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	MEffect*	pEffect;
	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MSelectableEffect(bltType);

	
	pEffect->SetFrameID( frameID, maxFrame );	

	pEffect->SetPixelPosition(pixelPoint.x, pixelPoint.y, egInfo.z0);		

	pEffect->SetStepPixel(egInfo.step);		

	pEffect->SetCount( egInfo.count, egInfo.linkCount );			

	
	pEffect->SetDirection( egInfo.direction );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	
	
	
	if (g_pZone->AddGroundEffect( pEffect ))
	{
		
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );

		return true;
	}

	return false;
}
