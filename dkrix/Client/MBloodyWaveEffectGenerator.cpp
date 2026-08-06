//----------------------------------------------------------------------
// MBloodyWaveEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MBloodyWaveEffectGenerator.h"
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
//MBloodyWaveEffectGenerator	g_BloodyWaveEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MBloodyWaveEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
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

	static int lastPhase = 0;

	int currentPhase = (egInfo.pEffectTarget==NULL? lastPhase +1 : egInfo.pEffectTarget->GetCurrentPhase());
	lastPhase = currentPhase;
	
	//---------------------------------------------
	
	//---------------------------------------------
	std::vector<POINT> v_cp;
	POINT p;

	switch (currentPhase)
	{
		
		case 1 :
		{
			p.x = 0; p.y = -1;
			v_cp.push_back(p);
			p.x = 0; p.y = 1;
			v_cp.push_back(p);
			p.x = 1; p.y = 0;
			v_cp.push_back(p);
			p.x = -1; p.y = 0;
			v_cp.push_back(p);
		}
		break;

		
		case 2 :
		{
			p.x = 1; p.y = -1;
			v_cp.push_back(p);
			p.x = 1; p.y = 1;
			v_cp.push_back(p);
			p.x = -1; p.y = -1;
			v_cp.push_back(p);
			p.x = -1; p.y = 1;
			v_cp.push_back(p);
		}
		break;

		
		case 3 :
		{
			p.x = 0; p.y = -2;
			v_cp.push_back(p);
			p.x = 0; p.y = 2;
			v_cp.push_back(p);
			p.x = 2; p.y = 0;
			v_cp.push_back(p);
			p.x = -2; p.y = 0;
			v_cp.push_back(p);
		}
		break;

		
		case 4 :
		{
			p.x = 1; p.y = -2;
			v_cp.push_back(p);
			p.x = 2; p.y = -1;
			v_cp.push_back(p);
			p.x = 1; p.y = 2;
			v_cp.push_back(p);
			p.x = 2; p.y = 1;
			v_cp.push_back(p);
			p.x = -1; p.y = -2;
			v_cp.push_back(p);
			p.x = -2; p.y = -1;
			v_cp.push_back(p);
			p.x = -1; p.y = 2;
			v_cp.push_back(p);
			p.x = -2; p.y = 1;
			v_cp.push_back(p);
		}
		break;

		
		default:
		{
			p.x = 0; p.y = -3;
			v_cp.push_back(p);
			p.x = 0; p.y = 3;
			v_cp.push_back(p);
			p.x = 3; p.y = 0;
			v_cp.push_back(p);
			p.x = -3; p.y = 0;
			v_cp.push_back(p);
		}
		break;
	}

	TYPE_SECTORPOSITION	tX, tY;
	tX = g_pTopView->PixelToMapX(egInfo.x1);
	tY = g_pTopView->PixelToMapY(egInfo.y1);

	int sX, sY;
	int z  = egInfo.z1;

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);	

	MEffect*	pEffect;
	
	//---------------------------------------------
	
	//---------------------------------------------
	for (int i=0; i<v_cp.size(); i++)
	{		
		sX = tX + v_cp[i].x;
		sY = tY + v_cp[i].y;
		
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
				
				
				pEffect->SetLink( egInfo.nActionInfo, NULL );
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
