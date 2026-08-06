//----------------------------------------------------------------------
// MFallingEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MFallingEffectGenerator.h"
#include "MLinearEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE


//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MFallingEffectGenerator	g_FallingEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MFallingEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	MLinearEffect*	pEffect;
	int z;
	//---------------------------------------------
	
	//---------------------------------------------
	z = 300;
	pEffect = new MLinearEffect(bltType);

	pEffect->SetFrameID( frameID, maxFrame );	

	
	
	

	//pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z1+z );	
	pEffect->SetPixelPosition( egInfo.x1, egInfo.y1, egInfo.z1+z );	

	
	pEffect->SetDirection( egInfo.direction );
					
	
	pEffect->SetTarget( egInfo.x1, egInfo.y1, egInfo.z1, egInfo.step );

	
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
