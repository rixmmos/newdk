//----------------------------------------------------------------------
// MMultipleFallingEffectGenerator.cpp
//----------------------------------------------------------------------

//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MMultipleFallingEffectGenerator.h"
#include "MLinearEffect.h"
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
//MMultipleFallingEffectGenerator	g_StopZoneCrossEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MMultipleFallingEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	bool bOK = false;

	MLinearEffect*	pEffect;
	int x, y, z;
	int ez1 = egInfo.z1 - TILE_Y;
	int zt = ez1;


	MEffectTarget*	pEffectTarget2;
	
	//---------------------------------------------
	
	//---------------------------------------------
	int numEffectPhase = 4;

	int baseZ = 350;

	int randY = 50;
	int randX = TILE_X;
	const int numEffect = 4;

	if(egInfo.nActionInfo == SKILL_ACID_STORM_WIDE || egInfo.nActionInfo == SKILL_POISON_STORM_WIDE)
	{
		randY = 100;
		randX = TILE_X*2;
		numEffectPhase = 6;
	}
	else if(egInfo.nActionInfo == SKILL_ICE_HAIL)
	{
		randY = 24*2;
		randX = TILE_X*2;
		numEffectPhase = 13;
	}
	else if(egInfo.nActionInfo == SKILL_WIDE_ICE_HAIL)
	{
		randY = 24*4;
		randX = TILE_X*4;
		numEffectPhase = 25;
	}


	int ex[numEffect];
	int ey[numEffect];
	int ez[numEffect];

	int dropCount = egInfo.count;
	const int phaseUpper = 150;	
	const int dropCountInc = phaseUpper / egInfo.step;

	
	for (int i=0; i<numEffectPhase; i++)
	{
		int n = 0;
	
		ex[n] = egInfo.x0 - rand()%randX - 24;
		ey[n] = egInfo.y0 - rand()%randY;
		ez[n] = baseZ + rand()%50;

		n++;
		ex[n] = egInfo.x0 - rand()%randX - 24;
		ey[n] = egInfo.y0 + rand()%randY;
		ez[n] = baseZ + rand()%50;

		n++;
		ex[n] = egInfo.x0 + rand()%randX + 24;
		ey[n] = egInfo.y0 - rand()%randY;
		ez[n] = baseZ + rand()%50;

		n++;
		ex[n] = egInfo.x0 + rand()%randX + 24;
		ey[n] = egInfo.y0 + rand()%randY;
		ez[n] = baseZ + rand()%50;

		baseZ		+= phaseUpper;
		dropCount	+= dropCountInc;

		for (int j=0; j<numEffect; j++)
		{
			x = ex[j];
			y = ey[j];
			z = ez[j];

			pEffect = new MLinearEffect(bltType);
			
			pEffect->SetFrameID( frameID, maxFrame );	

			
			pEffect->SetPixelPosition( x, y, egInfo.z0+z );	

			
			pEffect->SetDirection( egInfo.direction );
							
			
			pEffect->SetTarget( x, y, zt, egInfo.step );

			
			pEffect->SetCount( dropCount, egInfo.linkCount );

			
			pEffect->SetPower(egInfo.power);

			
			if (g_pZone->AddEffect( pEffect ))
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
						pEffectTarget2->Set( x, y, zt, egInfo.creatureID );
					}
				}
			}
		}
	}



	return bOK;
}
