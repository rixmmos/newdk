//----------------------------------------------------------------------
// MStopZoneEmptyCrossEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MStopZoneEmptyCrossEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MStopZoneEmptyCrossEffectGenerator	g_StopZoneCrossEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MStopZoneEmptyCrossEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	bool bOK;

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;

	//---------------------------------------------
	
	//---------------------------------------------
	TYPE_SECTORPOSITION	sX, sY;
	sX = g_pTopView->PixelToMapX(egInfo.x0);
	sY = g_pTopView->PixelToMapY(egInfo.y0);

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	MEffect*	pEffect;
	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MEffect(bltType);

	pEffect->SetFrameID( frameID, maxFrame );	

	pEffect->SetPosition(sX-1, sY);		
	pEffect->SetZ(egInfo.z0);			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count, egInfo.linkCount );			

	
	pEffect->SetDirection( egInfo.direction );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	
	bOK = g_pZone->AddEffect( pEffect );

	if (bOK)
	{
		
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );	
	}	

	// Test 4 
	MEffectTarget*	pEffectTarget2;

	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MEffect(bltType);


	pEffect->SetFrameID( frameID, maxFrame );	

	pEffect->SetPosition(sX+1, sY);		
	pEffect->SetZ(egInfo.z0);			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count, egInfo.linkCount );			

	
	pEffect->SetDirection( egInfo.direction );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	
	if (g_pZone->AddEffect( pEffect ))
	{
		
		if (egInfo.pEffectTarget == NULL)
		{
			pEffect->SetLink( egInfo.nActionInfo, NULL );
		}
		else
		{
			pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
			pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
			pEffectTarget2->Set( egInfo.x0+TILE_X, egInfo.y0, egInfo.z0, egInfo.creatureID );
		}
	}

	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MEffect(bltType);


	pEffect->SetFrameID( frameID, maxFrame );	

	pEffect->SetPosition(sX, sY-1);		
	pEffect->SetZ(egInfo.z0);			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count , egInfo.linkCount );			

	
	pEffect->SetDirection( egInfo.direction );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	
	if (g_pZone->AddEffect( pEffect ))
	{
		
		if (egInfo.pEffectTarget == NULL)
		{
			pEffect->SetLink( egInfo.nActionInfo, NULL );
		}
		else
		{
			pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
			pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
			pEffectTarget2->Set( egInfo.x0, egInfo.y0-TILE_Y, egInfo.z0, egInfo.creatureID );
		}
	}

	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MEffect(bltType);

	pEffect->SetFrameID( frameID, maxFrame );	

	pEffect->SetPosition(sX, sY+1);		
	pEffect->SetZ(egInfo.z0);			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count , egInfo.linkCount );			

	
	pEffect->SetDirection( egInfo.direction );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	
	if (g_pZone->AddEffect( pEffect ))
	{		
		
		if (egInfo.pEffectTarget == NULL)
		{
			pEffect->SetLink( egInfo.nActionInfo, NULL );
		}
		else
		{
			pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
			pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
			pEffectTarget2->Set( egInfo.x0, egInfo.y0+TILE_Y, egInfo.z0, egInfo.creatureID );
		}
	}
	//*/


	return bOK;
}
