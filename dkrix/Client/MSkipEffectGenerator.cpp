 //----------------------------------------------------------------------
// MSkipEffectGenerator.cpp
//----------------------------------------------------------------------

//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MSkipEffectGenerator.h"
#include "MSkipEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
//#define	new			DEBUG_NEW

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MAttachZoneEffectGenerator	g_StopZoneEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MSkipEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	int est = egInfo.effectSpriteType;
	
	POINT pixelPoint = { egInfo.x0, egInfo.y0 };
	
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;
	
	//---------------------------------------------
	
	//---------------------------------------------
	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
	int currentPhase = egInfo.pEffectTarget != NULL ?egInfo.pEffectTarget->GetCurrentPhase() : -1;
	
	
	if( egInfo.temp1 == 0 )				// UseActionGrade
	{
		MEffect*	pEffect;
		//---------------------------------------------
		
		//---------------------------------------------
		pEffect = new MSkipEffect(bltType);
		
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
	} else
	{
		
		std::vector<POINT> effectlist;
		POINT pt = g_pTopView->PixelToMap( pixelPoint.x, pixelPoint.y );
		
		if( egInfo.temp1 == 1 )
		{
			for( int i=-1;i<=1;i++)
			{
				POINT point = pt;
				point.x += i;
				effectlist.push_back( point );
			}
		} 
		else if( egInfo.temp1 == 2 )
		{
			for( int i=-1;i<=1;i++)
			{
				if( i == 0)
					continue;
				POINT point = pt;
				point.x += i;				
				effectlist.push_back( point );
				point.x -= i;
				point.y += i;
				effectlist.push_back( point );
			}
			effectlist.push_back( pt );
		} 
		else if( egInfo.temp1 == 3 )
		{
			for(int i=-1;i<=1;i++)
			{
				for(int j=-1;j<=1;j++)
				{
					POINT point = pt;
					point.x += i;
					point.y += j;
					effectlist.push_back( point );
				}
			}
		}		
		for(int i=0;i<effectlist.size();i++)
		{
			MEffect*	pEffect;
			pEffect = new MSkipEffect(bltType);
			
			pEffect->SetFrameID( frameID, maxFrame );				
			pEffect->SetPixelPosition(g_pTopView->MapToPixelX( effectlist[i].x), g_pTopView->MapToPixelY(effectlist[i].y), egInfo.z0);		
			pEffect->SetStepPixel(egInfo.step);		
			pEffect->SetCount( egInfo.count, egInfo.linkCount );			
			pEffect->SetDirection( egInfo.direction );			
			pEffect->SetPower(egInfo.power);			
			if (g_pZone->AddGroundEffect( pEffect ) && i == 0)
			{
				pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );				
			}  else
			{
				MEffectTarget *pTarget = egInfo.pEffectTarget;
				
				if( pTarget != NULL )
				{
					MEffectTarget *pEffectTarget = new MEffectTarget( *pTarget );
					pEffect->SetLink( egInfo.nActionInfo, pEffectTarget );						
				}
			}
		}
		//		egInfo.temp1= 0;
		return true;
	}
	return false;	
}
