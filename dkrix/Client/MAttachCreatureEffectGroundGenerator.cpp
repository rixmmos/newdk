//----------------------------------------------------------------------
// MAttachCreatureEffectGroundGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MAttachCreatureEffectGroundGenerator.h"
#include "MAttachEffect.h"
#include "MCreature.h"
#include "MZone.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MAttachCreatureEffectGroundGenerator	g_AttachCreatureEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MAttachCreatureEffectGroundGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	int est = egInfo.effectSpriteType;

	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;
	int direction = egInfo.direction;

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (est==EFFECTSPRITETYPE_SWORD_WAVE_1)
	{
		direction = (direction + 7) % 8;	// +8 - 1
	}
	

	MCreature* pCreature = g_pZone->GetCreature( egInfo.creatureID );

	
	if (pCreature == NULL)
	{	
		pCreature = g_pZone->GetFakeCreature( egInfo.creatureID );

		if (pCreature==NULL)
		{
			return false;
		}
	}

	
	MAttachEffect* pEffect = pCreature->CreateAttachEffect( 
											egInfo.effectSpriteType, 
											egInfo.count, 
											egInfo.linkCount, 
											TRUE );	

	if (pEffect == NULL)
	{
		return false;
	}
	

	pEffect->SetDirection( direction );

	pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );

	
	//pEffect->SetAttachCreatureID( creatureID );		

	
	pEffect->SetPower(egInfo.power);

	
	//pEffect->SetLight( light );
	
	return true;
}
