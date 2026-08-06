//----------------------------------------------------------------------
// MStopZoneEmptyHorizontalWallEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MStopZoneEmptyHorizontalWallEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"
#include "SkillDef.h"

#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MStopZoneEmptyHorizontalWallEffectGenerator	g_StopZoneWallEffectGenerator;
//---------------------------------------------

//---------------------------------------------
const int g_WallHorizontalDirValue[8][4] = { 
	{ 0, -1, 0, 1 },		// left
	{ -1, -1, 1, 1 },		// leftdown
	{ -1, 0, 1, 0 },		// down
	{ 1, -1, -1, 1 },		// rightdown
	{ 0, -1, 0, 1 },		// right
	{ -1, -1, 1, 1 },		// rightup
	{ -1, 0, 1, 0 },		// up
	{ -1, 1, 1, -1 }		// leftup
};

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MStopZoneEmptyHorizontalWallEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	bool bOK = false;

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;

	//---------------------------------------------
	
	//---------------------------------------------
	int	//sX0, sY0, 
		sX1, sY1;

	//sX0 = g_pTopView->PixelToMapX(egInfo.x0);
	//sY0 = g_pTopView->PixelToMapY(egInfo.y0);

	
	if (egInfo.nActionInfo>=MINE_ANKLE_KILLER 
		&& egInfo.nActionInfo<=MINE_COBRA)
	{
		
		sX1 = g_pTopView->PixelToMapX(egInfo.x0);
		sY1 = g_pTopView->PixelToMapY(egInfo.y0);
	}
	else
	{
		
		sX1 = g_pTopView->PixelToMapX(egInfo.x1);
		sY1 = g_pTopView->PixelToMapY(egInfo.y1);
	}

	
	int lookDirection = egInfo.direction;//MTopView::GetDirectionToPosition(sX0, sY0, sX1, sY1);

	
	//---------------------------------------------
	
	//---------------------------------------------
	
	// 1			 0
	// 3			 1
	// 5			 2
	int stepMulti = (egInfo.step>>1);
	int sX = sX1 + g_WallHorizontalDirValue[lookDirection][0] * stepMulti;
	int sY = sY1 + g_WallHorizontalDirValue[lookDirection][1] * stepMulti;
	int cX = g_WallHorizontalDirValue[lookDirection][2];
	int cY = g_WallHorizontalDirValue[lookDirection][3];

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	int x = egInfo.x1, 
		y = egInfo.y1;

	MEffect*	pEffect;
	//---------------------------------------------
	
	//---------------------------------------------
	for (int i=0; i<egInfo.step; i++)
	{
		
		if (i!=stepMulti)
		{
			pEffect = new MEffect(bltType);

			//---------------------------------------------
			
			//---------------------------------------------
			
			pEffect->SetFrameID( frameID, maxFrame );	

			pEffect->SetPosition(sX, sY);		
			pEffect->SetZ(egInfo.z0);			
			pEffect->SetStepPixel(egInfo.step);		
			pEffect->SetCount( egInfo.count , egInfo.linkCount );			

			
			pEffect->SetDirection( egInfo.direction );

			
			pEffect->SetPower(egInfo.power);

			
			//pEffect->SetLight( light );

			
			bool bAdd = g_pZone->AddEffect( pEffect );

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

			if (bAdd)
			{
				if (!bOK)
				{				
					
					pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );

					bOK = true;
				}
				else if (egInfo.pEffectTarget == NULL)
				{
					pEffect->SetLink( egInfo.nActionInfo, NULL );
				}
				else
				{
					MEffectTarget* pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
					pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
					pEffectTarget2->Set( x, y, egInfo.z0, egInfo.creatureID );
				}
			}
		}

		
		sX += cX;
		sY += cY;

		//
		x += TILE_X*cX;
		y += TILE_Y*cY;
	}

	return bOK;
}
