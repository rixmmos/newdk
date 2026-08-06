//----------------------------------------------------------------------
// MAttackZoneBombEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MAttackZoneBombEffectGenerator.h"
#include "MParabolaEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MAttackZoneBombEffectGenerator	g_AttackZoneParabolaEffectGenerator;


//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MAttackZoneBombEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;

	MParabolaEffect* pEffect = new MParabolaEffect(bltType);	
	
	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
	
	pEffect->SetFrameID( frameID, maxFrame );		

	
	
	
	//POINT cxy = MTopView::GetChangeValueToDirection( egInfo.direction );

	int tx = egInfo.x1; // + cxy.x * TILE_X;
	int ty = egInfo.y1; // + cxy.y * TILE_Y;

	
	//pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0+TILE_Y );	
	pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0+(TILE_Y<<1) );	

	
	pEffect->SetDirection( egInfo.direction );
					
	
	pEffect->SetTarget( tx, ty, egInfo.z1, egInfo.step );	

	
	pEffect->SetCount( egInfo.count, egInfo.linkCount );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	if (g_pZone->AddEffect( pEffect ))
	{
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );

		return true;
	}

	return false;

}
