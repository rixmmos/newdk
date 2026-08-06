//----------------------------------------------------------------------
// MRippleZonePixelEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MRippleZonePixelEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MCreature.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"
#include "UserInformation.h"

//#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MRippleZonePixelEffectGenerator	g_RippleZoneEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MRippleZonePixelEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	//------------------------------------------------------------
	
	
	
	//------------------------------------------------------------
	MEffectTarget* pTarget = egInfo.pEffectTarget;

	
	int sx = egInfo.x0;
	int sy = egInfo.y0;
	int sz = 0;//egInfo.z0;

	
	int tx = egInfo.x1; 
	int ty = egInfo.y1;
	int tz = 0;//egInfo.z1;

	int tx2 = tx;
	int ty2 = ty;

	int est = egInfo.effectSpriteType;

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	bool bPixelStepMove = false;
	bool bGroundEffect = false;
	bool bResetTarget = true;
	int startMoveStep = 0;

	//------------------------------------------------------------
	// Power of Land
	//------------------------------------------------------------
	if (est==EFFECTSPRITETYPE_POWER_OF_LAND_STONE_1)
	{
		
		est = EFFECTSPRITETYPE_POWER_OF_LAND_STONE_2 + rand()%3;
		
		bGroundEffect = true;
		bPixelStepMove = true;
		startMoveStep = 1;
		bResetTarget = false;
	}
	//------------------------------------------------------------
	// Earthquake
	//------------------------------------------------------------
	else if (est==EFFECTSPRITETYPE_EARTHQUAKE_1)
	{
		
		est = EFFECTSPRITETYPE_EARTHQUAKE_1 + rand()%3;
		
		bGroundEffect = true;
		bPixelStepMove = true;
		startMoveStep = 3;
	}
	//------------------------------------------------------------
	// Transfusion
	//------------------------------------------------------------
	else if (est==EFFECTSPRITETYPE_TRANSFUSION_1 || est==EFFECTSPRITETYPE_GREEN_TRANSFUSION_1)
	{
		MCreature* pCreature = g_pZone->GetCreature( egInfo.creatureID );

		int targetX, targetY;

		
		
		
		POINT pt;

 		if (pCreature==NULL)
		{
			targetX = egInfo.x1;
			targetY = egInfo.y1;

//			pt.x = targetX;
//			pt.y = targetY;
		}
		else
		{
			targetX = pCreature->GetX();
			targetY = pCreature->GetY();

		}
		pt = MTopView::MapToPixel( targetX, targetY );

		RECT rectEffect = 
		{
			egInfo.x0 + (24>>1),
			egInfo.y0 + (24>>1),
			egInfo.x0 + 24,
			egInfo.y0 + 24,
		};
		
		RECT rectChar =
		{
			pt.x,
			pt.y,
			pt.x + 24,
			pt.y + 24
		};

		if (rectChar.left <= rectEffect.right
			&& rectChar.right >= rectEffect.left
			&& rectChar.top <= rectEffect.bottom
			&& rectChar.bottom >= rectEffect.top)
		{
			return false;
		}

		
		if(g_pUserInformation->GoreLevel == false)
			est = EFFECTSPRITETYPE_GREEN_TRANSFUSION_1 + rand()%3;
		else
			est = EFFECTSPRITETYPE_TRANSFUSION_1 + rand()%3;

		bGroundEffect = true;
		bPixelStepMove = true;
		startMoveStep = 1;
	}

	if (bPixelStepMove)
	{
		
		
		int movePixel = egInfo.step;// * egInfo.count;

		int cx = sx-tx;
		int cy = sy-ty;

		if (bResetTarget && (cx==0 || cy==0))
		{
			//---------------------------------------------
			
			//---------------------------------------------
			TYPE_SECTORPOSITION	sX, sY;
			sX = g_pTopView->PixelToMapX( sx );
			sY = g_pTopView->PixelToMapY( sy );

			//---------------------------------------------
			
			//---------------------------------------------
			TYPE_SECTORPOSITION x=sX, y=sY;
			MCreature::GetPositionToDirection(x,y, egInfo.direction);

			//---------------------------------------------
			
			//---------------------------------------------
			tx = g_pTopView->MapToPixelX( x );
			ty = g_pTopView->MapToPixelY( y );

			
			cx = sx - tx;
			cy = sy - ty;
		}

		int currentPixel = sqrt(cx*cx + cy*cy);

		float basis = ((cx==0)? 0 : (float)cy / (float)cx);

		if (currentPixel==0)
		{			
		}
		else
		{
			movePixel = (float)movePixel * (1.0f - fabs((float)cy / (float)(2.0f*currentPixel)));
		
			if (pTarget!=NULL && pTarget->GetCurrentPhase()==1)
			{
				movePixel *= startMoveStep;
			}

			int movePixel2 = movePixel<<1;
			
			tx = sx - (cx * movePixel / currentPixel);
			ty = sy - (cy * movePixel / currentPixel);

			tx2 = sx - (cx * movePixel2 / currentPixel);
			ty2 = sy - (cy * movePixel2 / currentPixel);
		}
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------	
	if (pTarget!=NULL && bResetTarget)
	{
		pTarget->Set(tx2, ty2, tz, pTarget->GetID());		
	}


	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	MEffect*	pEffect;
	//---------------------------------------------
	
	//---------------------------------------------
	pEffect = new MEffect(bltType);

	pEffect->SetFrameID( frameID, maxFrame );	

	

	pEffect->SetPixelPosition(tx, ty, tz);		
	
	
	pEffect->SetDirection( egInfo.direction );

	//pEffect->SetZ( sz );			
	pEffect->SetStepPixel(egInfo.step);		
	pEffect->SetCount( egInfo.count, egInfo.linkCount );			

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );

	bool bAdd = false;
	
	if (bGroundEffect)
	{
		bAdd = g_pZone->AddGroundEffect( pEffect );
	}	
	else
	{
		bAdd = g_pZone->AddEffect( pEffect );
	}

	if (bAdd)
	{
		
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );	
	}

	return bAdd;
}
