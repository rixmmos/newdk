//----------------------------------------------------------------------
// MRippleZoneEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MRippleZoneEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MCreature.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MRippleZoneEffectGenerator	g_RippleZoneEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MRippleZoneEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	//---------------------------------------------
	
	//---------------------------------------------
	TYPE_SECTORPOSITION	sX, sY;
	sX = g_pTopView->PixelToMapX(egInfo.x0);
	sY = g_pTopView->PixelToMapY(egInfo.y0);

	
	TYPE_SECTORPOSITION x=sX, y=sY;
	MCreature::GetPositionToDirection(x,y, egInfo.direction);

	
	if (x<0 || y<0 || x>=g_pZone->GetWidth() || y>=g_pZone->GetHeight())
			return false;

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	MEffect*	pEffect;
	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MEffect(bltType);

	pEffect->SetFrameID( frameID, maxFrame );	

	pEffect->SetPosition(x, y);		
	
	
	pEffect->SetDirection( egInfo.direction );

	pEffect->SetZ(egInfo.z0);			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count, egInfo.linkCount );			

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	

	
	if (egInfo.effectSpriteType==EFFECTSPRITETYPE_EARTHQUAKE_1
		|| egInfo.effectSpriteType==EFFECTSPRITETYPE_EARTHQUAKE_2
		|| egInfo.effectSpriteType==EFFECTSPRITETYPE_EARTHQUAKE_3
		|| egInfo.effectSpriteType==EFFECTSPRITETYPE_POWER_OF_LAND_STONE_1
		|| egInfo.effectSpriteType==EFFECTSPRITETYPE_POWER_OF_LAND_STONE_2
		|| egInfo.effectSpriteType==EFFECTSPRITETYPE_POWER_OF_LAND_STONE_3
		)
	{
		return g_pZone->AddGroundEffect( pEffect );
	}		
		
	if (g_pZone->AddEffect( pEffect ))
	{
		
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );

		return true;
	}

	return false;
}
