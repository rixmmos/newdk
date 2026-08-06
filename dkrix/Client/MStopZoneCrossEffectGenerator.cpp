//----------------------------------------------------------------------
// MStopZoneCrossEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MStopZoneCrossEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
#include "SkillDef.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MStopZoneCrossEffectGenerator	g_StopZoneCrossEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MStopZoneCrossEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	bool bOK, bAdd;

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;
	BYTE			power = egInfo.power;

	if(egInfo.nActionInfo == SAND_CROSS)
		power = 3;

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

	pEffect->SetPosition(sX, sY);		
	pEffect->SetZ(egInfo.z0);			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count, egInfo.linkCount );			

	
	pEffect->SetDirection( egInfo.direction );

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	
	bAdd = g_pZone->AddEffect( pEffect );

	if (bAdd)
	{
		
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );	

		bOK = true;
	}	

	int sX1 = sX-power;
	int sY1 = sY-power;
	int sX2 = sX+power;
	int sY2 = sY+power;

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (sX1 < 0) 
	{					
		sX1 = 0;	
	}

	if (sX2 >= g_pZone->GetWidth())
	{
		sX2 = g_pZone->GetWidth()-1;
	}

	if (sY1 < 0)
	{
		sY1 = 0;	
	}

	if (sY2 >= g_pZone->GetHeight())
	{
		sY2 = g_pZone->GetHeight()-1;
	}


	
	MEffectTarget*	pEffectTarget2;
	
	int x, y;
	for (y=sY1; y<=sY2; y++)
	{
		for (x=sX1; x<=sX2; x++)
		{
			if (x==sX && y==sY || !(x == sX || y == sY))
				continue;


			pEffect = new MEffect(bltType);
	
		

			pEffect->SetFrameID( frameID, maxFrame );	

			pEffect->SetPosition(x, y);		
			pEffect->SetZ(egInfo.z0);			
			pEffect->SetStepPixel(egInfo.step);		
			pEffect->SetCount( egInfo.count, egInfo.linkCount );			

			
			pEffect->SetDirection( egInfo.direction );

			
			pEffect->SetPower(power);

			
			//pEffect->SetLight( light );

			
			bAdd = g_pZone->AddEffect( pEffect );

			if (bAdd)
			{
				
				if (!bOK)
				{
					pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );

					bOK = true;
				}
				else
				{
					
					if (egInfo.pEffectTarget == NULL)
					{
						pEffect->SetLink( egInfo.nActionInfo, NULL );
					}
					else
					{
						pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
						pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
						pEffectTarget2->Set( egInfo.x1+TILE_X*(x-sX1-1), 
												egInfo.y1+TILE_Y*(y-sY1-1), 
												egInfo.z0, 
												egInfo.creatureID );
					}
				}
			}

			/*
			if (bAdd)
			{
				int num = rand() % maxFrame;
				
				for (int nf=0; nf<num; nf++)
				{
					pEffect->NextFrame();
				}
			}
			*/

			//bOK = bOK || bAdd;
		}
	}

//	// Test 4 
//	MEffectTarget*	pEffectTarget2;
//	//---------------------------------------------

//	//---------------------------------------------
//	pEffect = new MEffect(bltType);
//	
//	
//	pEffect->SetFrameID( frameID, maxFrame );	
//

//	pEffect->SetZ(egInfo.z0);			


//

//	pEffect->SetDirection( egInfo.direction );
//

//	pEffect->SetPower(egInfo.power);
//

//	//pEffect->SetLight( light );
//

//	if (g_pZone->AddEffect( pEffect ))
//	{

//		if (egInfo.pEffectTarget == NULL)
//		{
//			pEffect->SetLink( egInfo.nActionInfo, NULL );
//		}
//		else
//		{
//			pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
//			pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
//			pEffectTarget2->Set( egInfo.x0-TILE_X, egInfo.y0, egInfo.z0, egInfo.creatureID );
//		}
//	}
//
//
//	//---------------------------------------------

//	//---------------------------------------------
//	pEffect = new MEffect(bltType);
//
//
//	pEffect->SetFrameID( frameID, maxFrame );	
//

//	pEffect->SetZ(egInfo.z0);			


//

//	pEffect->SetDirection( egInfo.direction );
//

//	pEffect->SetPower(egInfo.power);
//

//	//pEffect->SetLight( light );
//

//	if (g_pZone->AddEffect( pEffect ))
//	{

//		if (egInfo.pEffectTarget == NULL)
//		{
//			pEffect->SetLink( egInfo.nActionInfo, NULL );
//		}
//		else
//		{
//			pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
//			pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
//			pEffectTarget2->Set( egInfo.x0+TILE_X, egInfo.y0, egInfo.z0, egInfo.creatureID );
//		}
//	}
//
//	//---------------------------------------------

//	//---------------------------------------------
//	pEffect = new MEffect(bltType);
//
//
//	pEffect->SetFrameID( frameID, maxFrame );	
//

//	pEffect->SetZ(egInfo.z0);			


//

//	pEffect->SetDirection( egInfo.direction );
//

//	pEffect->SetPower(egInfo.power);
//

//	//pEffect->SetLight( light );
//

//	if (g_pZone->AddEffect( pEffect ))
//	{

//		if (egInfo.pEffectTarget == NULL)
//		{
//			pEffect->SetLink( egInfo.nActionInfo, NULL );
//		}
//		else
//		{
//			pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
//			pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
//			pEffectTarget2->Set( egInfo.x0, egInfo.y0-TILE_Y, egInfo.z0, egInfo.creatureID );
//		}
//	}
//
//	//---------------------------------------------

//	//---------------------------------------------
//	pEffect = new MEffect(bltType);
//
//	pEffect->SetFrameID( frameID, maxFrame );	
//

//	pEffect->SetZ(egInfo.z0);			


//

//	pEffect->SetDirection( egInfo.direction );
//

//	pEffect->SetPower(egInfo.power);
//

//	//pEffect->SetLight( light );
//

//	if (g_pZone->AddEffect( pEffect ))
//	{		

//		if (egInfo.pEffectTarget == NULL)
//		{
//			pEffect->SetLink( egInfo.nActionInfo, NULL );
//		}
//		else
//		{
//			pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
//			pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
//			pEffectTarget2->Set( egInfo.x0, egInfo.y0+TILE_Y, egInfo.z0, egInfo.creatureID );
//		}
//	}
	//*/


	return bOK;
}
