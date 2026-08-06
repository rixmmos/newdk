//----------------------------------------------------------------------
// MHelicopterManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MHelicopterManager.h"
#include "MChaseEffect.h"
#include "MZone.h"
#include "MTopView.h"
#include "MPlayer.h"
#include "EffectSpriteTypeDef.h"
#include "SoundDef.h"
#include "MZoneSoundManager.h"
#include "MEffectSpriteTypeTable.h"
#include "UIFunction.h"

//----------------------------------------------------------------------
// define
//----------------------------------------------------------------------
#define PIXEL_HELICOPTER_MOVE		20

//----------------------------------------------------------------------
//
//						MHelicopter
//
//----------------------------------------------------------------------
MHelicopter::MHelicopter()
{
	m_CreatureID = OBJECTID_NULL;
	m_EffectID = OBJECTID_NULL;
	m_pSound = NULL;
}

MHelicopter::~MHelicopter()
{
	Release();
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MHelicopter::Release()
{
	if (g_pZone!=NULL)
	{
		g_pZone->RemoveEffect( m_EffectID );
	}

	if (m_pSound!=NULL)
	{
		delete m_pSound;
		m_pSound = NULL;
	}

	m_CreatureID = OBJECTID_NULL;
	m_EffectID = OBJECTID_NULL;
}

//----------------------------------------------------------------------
// Set ChaseCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MHelicopter::SetChaseCreature(TYPE_OBJECTID id, int x0, int y0)
{
	
	MCreature* pCreature = g_pZone->GetCreature( id );

	if (pCreature!=NULL)
	{
		m_CreatureID = id;

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (m_pSound!=NULL)
		{
			delete m_pSound;
		}
		m_pSound = new ZONESOUND_NODE(SOUND_WORLD_PROPELLER);
		m_pSound->Play( x0, y0, true );	// loop
		
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		int cx, cy, cz;
		cx = g_pTopView->MapToPixelX( pCreature->GetX() );
		cy = g_pTopView->MapToPixelY( pCreature->GetY() );
		cz = 0;//pCreature->GetZ();

		
		int effectSpriteType = EFFECTSPRITETYPE_SUMMON_HELICOPTER;
		int direction = 2;
		int step = PIXEL_HELICOPTER_MOVE;
		int x0	= cx + 500;
		int	y0	= cy - 300;
		int z0	= cz;

		BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[effectSpriteType].BltType;
		TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[effectSpriteType].FrameID;		
		

		MChaseEffect* pEffect = new MChaseEffect(bltType);	

		
		m_EffectID = pEffect->GetID();
		
		int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
		
		pEffect->SetFrameID( frameID, maxFrame );		

		
		pEffect->SetPixelPosition( x0, y0, z0 );

		pEffect->SetDirection( direction );
		
		
		pEffect->SetTraceCreatureID( id );
		pEffect->SetStepPixel( step );
		

		
		//pEffect->SetCount( egInfo.count, egInfo.linkCount );

		
		//pEffect->SetPower(egInfo.power);

		
		//pEffect->SetLight( light );

		g_pZone->AddEffect( pEffect );

		return true;
	}

	DEBUG_ADD_FORMAT("MHelicopter::SetChase-No Such Creature id=%d", id);

	Release();

	return false;
}

//----------------------------------------------------------------------
// Update Sound
//----------------------------------------------------------------------
void			
MHelicopter::UpdateSound(int x, int y)
{
	if (m_pSound!=NULL)
	{
		m_pSound->Play( x, y, true );
	}
}

//----------------------------------------------------------------------
//
//						MHelicopterManager
//
//----------------------------------------------------------------------
MHelicopterManager::MHelicopterManager()
{
}

MHelicopterManager::~MHelicopterManager()
{
	Release();
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MHelicopterManager::Release()
{
	CTypeMap2<MHelicopter>::Release();
}

//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------
void
MHelicopterManager::Update()
{
	iterator iHelicopter = begin();

	while (iHelicopter != end())
	{
		MHelicopter* pHelicopter = iHelicopter->second;

		bool bDeleteHelicopter = false;
	
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		MEffect* pEffect = g_pZone->GetEffect( pHelicopter->GetEffectID() );

		if (pEffect==NULL)
		{
			bDeleteHelicopter = true;			
		}
		else
		{
			if (pEffect->GetEffectType()==MEffect::EFFECT_CHASE)
			{
				MChaseEffect *pChaseEffect = (MChaseEffect*)pEffect;

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				pHelicopter->UpdateSound( pEffect->GetX(), pEffect->GetY() );

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				if (pChaseEffect->GetTraceCreatureID()==g_pPlayer->GetID())
				{
					if (pChaseEffect->IsChaseOver())
					{
						
						if (!UI_IsRunningSelectWayPoint())
						{
							UI_RunSelectWayPoint();							
						}
					}
				}

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				else if (pChaseEffect->GetTraceCreatureID()==OBJECTID_NULL
					&& pChaseEffect->IsChaseOver())
				{
					bDeleteHelicopter = true;					
				}		
				
			}
		}

		//---------------------------------------------
		
		//---------------------------------------------
		if (bDeleteHelicopter)
		{
			delete pHelicopter;

			
			iterator iTemp = iHelicopter;
			iHelicopter ++;

			erase( iTemp );
		}
		else
		{
			iHelicopter ++;
		}
	}
}

//----------------------------------------------------------------------
// Add Helicopter
//----------------------------------------------------------------------
bool
MHelicopterManager::AddHelicopter(MHelicopter* pHelicopter)
{
	TYPE_OBJECTID creatureID = pHelicopter->GetCreatureID();

	iterator iHelicopter = find( creatureID );

	
	if (iHelicopter != end())
	{
		delete iHelicopter->second;

		erase( iHelicopter );
	}

	insert(TYPE_MAP::value_type(creatureID, pHelicopter));

	return true;
}


//----------------------------------------------------------------------
// Get Helicopter
//----------------------------------------------------------------------
MHelicopter*
MHelicopterManager::GetHelicopter(TYPE_OBJECTID creatureID) const
{
	const_iterator iHelicopter = find( creatureID );

	if (iHelicopter == end())
	{
		return NULL;
	}
	
	return iHelicopter->second;
}

//----------------------------------------------------------------------
// Remove Helicopter Soon
//----------------------------------------------------------------------
void
MHelicopterManager::RemoveHelicopterSoon(TYPE_OBJECTID creatureID)
{
	iterator iHelicopter = find( creatureID );

	if (iHelicopter != end())
	{
		delete iHelicopter->second;

		erase( iHelicopter );
	}	
}

//----------------------------------------------------------------------
// Remove Helicopter
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MHelicopterManager::RemoveHelicopter(TYPE_OBJECTID creatureID)
{
	DEBUG_ADD_FORMAT("RemoveHelicopter. cid=%d", creatureID);
	iterator iHelicopter = find( creatureID );

	if (iHelicopter != end())
	{
		MHelicopter* pHelicopter = iHelicopter->second;

		MEffect* pEffect = g_pZone->GetEffect( pHelicopter->GetEffectID() );

		if (pEffect==NULL)
		{
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			delete pHelicopter;

			erase( iHelicopter );
		}
		else
		{
			if (pEffect->GetEffectType()==MEffect::EFFECT_CHASE)
			{				
				MChaseEffect *pChaseEffect = (MChaseEffect*)pEffect;

				if (pChaseEffect->GetTraceCreatureID()!=OBJECTID_NULL)
				{
					
					pChaseEffect->SetTraceCreatureID( OBJECTID_NULL );

					int x = pEffect->GetPixelX() - 1000;
					int y = pEffect->GetPixelY() + 600;
					pChaseEffect->SetTarget(x, y, 0, PIXEL_HELICOPTER_MOVE);
				}
			}
		}
	}	
}