//----------------------------------------------------------------------
// MRisingEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MRisingEffectGenerator.h"
#include "MLinearEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"
#include "EffectSpriteTypeDef.h"
#include "DebugInfo.h"
#include "SkillDef.h"

#define		PI			3.141592f

//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MRisingEffectGenerator	g_RisingEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MRisingEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;
	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
	MEffectTarget* pTarget = egInfo.pEffectTarget;
	
	if(egInfo.nActionInfo >= SKILL_FIRE_CRACKER_VOLLEY_1 &&
		egInfo.nActionInfo <= SKILL_FIRE_CRACKER_WIDE_VOLLEY_4 ||
		egInfo.nActionInfo ==SKILL_DRAGON_FIRE_CRACKER)
	{
		
		int angle = 18;
		int i;
		int tx[3],tz[3],step[3];
		int coord_z = egInfo.step * egInfo.count;

		for(i=0;i<3;i++)
		{
			tx[i] = egInfo.x0;
			tz[i] = egInfo.z0 + coord_z;
			step[i] = egInfo.step;
		}

		

		float Radian = float( angle ) * ( PI / 180.0f );
		float sinValue = float(coord_z) * sinf( Radian );
		float cosValue = float(coord_z) * cosf( Radian );
		int step_count = sqrt(int(sinValue * sinValue)+ int(cosValue)*int(cosValue)) / egInfo.count;		
		
		tx[0] = egInfo.x0 + int(sinValue);
		tz[0] = egInfo.z0 + int(cosValue);
		step[0] = step_count;
		tx[2] = egInfo.x0 - int(sinValue);
		tz[2] = egInfo.z0 + int(cosValue);
		step[2] = step_count;
				
		for(i=0;i<3;i++)
		{
			MLinearEffect* pEffect = new MLinearEffect(bltType);
			
			pEffect->SetFrameID( frameID, maxFrame );
			pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0 );
			pEffect->SetDirection( 2 );
			pEffect->SetTarget( tx[i], egInfo.y0, tz[i], step[i] );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetPower( egInfo.power );
			
			if(g_pZone->AddEffect( pEffect ) )
			{
				if(pTarget == NULL )
				{
					pEffect->SetLink( egInfo.nActionInfo, NULL );
				} else
				{
					if( i == 1 )
					{
						pEffect->SetLink( egInfo.nActionInfo, pTarget );
						pTarget->Set( tx[i], egInfo.y0, tz[i], egInfo.creatureID );
					} else
					{
						MEffectTarget *pEffectTarget = new MEffectTarget( *pTarget );						
						pEffect->SetLink( egInfo.nActionInfo, pEffectTarget );		
						pEffectTarget->Set( tx[i], egInfo.y0, tz[i], egInfo.creatureID );
					}
				}
			}
		}
		return true;
	}
	else
	if(egInfo.nActionInfo == SKILL_FIRE_CRACKER_STORM)
	{
		int angle1 = 10, angle2 = 30;
		int i;
		int tx[4],tz[4],step[4];
		int coord_z = egInfo.step * egInfo.count;

		for(i=0;i<4;i++)
		{
			tx[i] = egInfo.x0;
			tz[i] = egInfo.z0 + coord_z;
			step[i] = egInfo.step;
		}

		
				
		float Radian = float( angle1 ) * ( PI / 180.0f );
		float sinValue = float(coord_z) * sinf( Radian );
		float cosValue = float(coord_z) * cosf( Radian );
		int step_count = sqrt(int(sinValue * sinValue)+ int(cosValue)*int(cosValue)) / egInfo.count;		
		
		tx[0] = egInfo.x0 + int(sinValue);
		tz[0] = egInfo.z0 + int(cosValue);
		step[0] = step_count;
		tx[3] = egInfo.x0 - int(sinValue);
		tz[3] = egInfo.z0 + int(cosValue);
		step[3] = step_count;

		Radian = float( angle2 ) * (PI / 180.0f);
		sinValue = float(coord_z) * sinf( Radian );
		cosValue = float(coord_z) * cosf( Radian );
		step_count = sqrt(int(sinValue * sinValue) + int(cosValue)*int(cosValue)) / egInfo.count;

		tx[1] = egInfo.x0 + int(sinValue);
		tz[1] = egInfo.z0 + int(cosValue);
		tx[2] = egInfo.x0 - int(sinValue);
		tz[2] = egInfo.z0 + int(cosValue);
		step[1] = step[2] = step_count;
				
		for(i=0;i<4;i++)
		{
			MLinearEffect* pEffect = new MLinearEffect(bltType);
			pEffect->SetFrameID( frameID, maxFrame );
			pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0 );
			pEffect->SetDirection( 2 );
			pEffect->SetTarget( tx[i], egInfo.y0, tz[i], step[i] );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetPower( egInfo.power );
			
			if(g_pZone->AddEffect( pEffect ) )
			{
				if(pTarget == NULL )
				{
					pEffect->SetLink( egInfo.nActionInfo, NULL );
				} else
				{
					if( i == 1 )
					{
						pEffect->SetLink( egInfo.nActionInfo, pTarget );
						pTarget->Set( tx[i], egInfo.y0, tz[i], egInfo.creatureID );												
					} else
					{
						MEffectTarget *pEffectTarget = new MEffectTarget( *pTarget );						
						pEffect->SetLink( egInfo.nActionInfo, pEffectTarget );		
						pEffectTarget->Set( tx[i], egInfo.y0, tz[i], egInfo.creatureID );
					}
				}
			}
		}
		return true;
	}
	else
	{
		MLinearEffect* pEffect = new MLinearEffect(bltType);	
		
		
		pEffect->SetFrameID( frameID, maxFrame );		
		
		pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0 );			
		
		pEffect->SetDirection( egInfo.direction );		
		
		pEffect->SetTarget( egInfo.x0, egInfo.y0, egInfo.z0+egInfo.step*egInfo.count, egInfo.step );
		//pEffect->SetTarget( egInfo.x0, egInfo.y0, egInfo.z1, egInfo.step );		
		
		pEffect->SetCount( egInfo.count, egInfo.linkCount );
		
		
		pEffect->SetPower(egInfo.power);
		
		
		//pEffect->SetLight( light );
		
		if (g_pZone->AddEffect( pEffect ))
		{
			pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );
			
			return true;
		}
	}

	return false;
}
