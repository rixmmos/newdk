//----------------------------------------------------------------------
// MBloodyWallEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MBloodyWallEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"

#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MBloodyWallEffectGenerator	g_BloodyWallEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MBloodyWallEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	bool bOK = false;

	int est = egInfo.effectSpriteType;

	
	if (est>=EFFECTSPRITETYPE_BLOODY_WALL_1
		&& est<=EFFECTSPRITETYPE_BLOODY_WALL_3)
	{
		est = EFFECTSPRITETYPE_BLOODY_WALL_1 + rand()%3;
	}


	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;
	bool			repeatFrame	= (*g_pEffectSpriteTypeTable)[est].RepeatFrame;

	//---------------------------------------------
	
	//---------------------------------------------
	int	tx, ty;
	
	tx = egInfo.x1;
	ty = egInfo.y1;

	
	int lookDirection = egInfo.direction;//MTopView::GetDirectionToPosition(sX0, sY0, sX1, sY1);

	//---------------------------------------------
	
	//---------------------------------------------
	
	 

	const POINT dirValue[8][5] =
	{
		{ { 0, -2 }, { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },	// left
		{ { -1, -1 }, { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },		// leftdown
		{ { -2, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } },			// down
		{ { 1, -1 }, { 1, 0 }, { 0, 0 }, { 0, 1 }, { -1, 1 } },			// rightdown
		{ { 0, -2 }, { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },			// right
		{ { 1, 1 }, { 1, 0 }, { 0, 0 }, { 0, -1 }, { -1, -1 } },		// rightup
		{ { -2, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } },	// up
		{ { 1, -1 }, { 0, -1 }, { 0, 0 }, { -1, 0 }, { -1, 1 } },	// leftup
	};

	TYPE_SECTORPOSITION	tX, tY;
	tX = g_pTopView->PixelToMapX(egInfo.x0);
	tY = g_pTopView->PixelToMapY(egInfo.y0);

	int sX, sY;
	int sx, sy;
	int z  = egInfo.z0;

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);	

	MEffect*	pEffect;
	
	//---------------------------------------------
	
	//---------------------------------------------
	for (int i=0; i<5; i++)
	{		
		sX = tX + dirValue[lookDirection][i].x;
		sY = tY + dirValue[lookDirection][i].y;
		sx = tx + dirValue[lookDirection][i].x * TILE_X;
		sy = ty + dirValue[lookDirection][i].y * TILE_Y;

		pEffect = new MEffect(bltType);
		
		pEffect->SetFrameID( frameID, maxFrame );	

		
		pEffect->SetPosition( sX, sY );
		pEffect->SetZ( z );			
		pEffect->SetStepPixel(egInfo.step);		
		pEffect->SetCount( egInfo.count , egInfo.linkCount );			

		
		pEffect->SetDirection( egInfo.direction );

		
		pEffect->SetPower(egInfo.power);

		
		bool bAdd = g_pZone->AddEffect( pEffect );

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
					MEffectTarget* pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
					pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
					pEffectTarget2->Set( sx, sy, z, egInfo.creatureID );
				}			
			}
		}

		//---------------------------------------------
		
		
		//---------------------------------------------
		if (bAdd && repeatFrame)
		{
			int num = rand() % maxFrame;
			
			for (int nf=0; nf<num; nf++)
			{
				pEffect->NextFrame();
			}
		}

		//
		//sx += cx;
		//sy += cy;

		
		if (est>=EFFECTSPRITETYPE_BLOODY_WALL_1
			&& est<=EFFECTSPRITETYPE_BLOODY_WALL_3)
		{
			if (++est > EFFECTSPRITETYPE_BLOODY_WALL_3)
			{
				est = EFFECTSPRITETYPE_BLOODY_WALL_1;
			}
		}
		frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;
		maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);	
	}

	return bOK;
}
