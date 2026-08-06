//--------------------------------------------------------------------------------
//
// Filename    : GCMorph1Handler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCMorphSlayer2.h"
#include "ClientDef.h"
#include "MCreatureTable.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCMorphSlayer2Handler::execute ( GCMorphSlayer2 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__


	//------------------------------------------------------
	
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
		
	}	
	//------------------------------------------------------
	
	//------------------------------------------------------
	else
	{
		const PCSlayerInfo3 & si = pPacket->getSlayerInfo();

		MCreature* pCreature = g_pZone->GetCreature(si.getObjectID());

		//--------------------------------------------------
		
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			MCreatureWear* pCreatureWear = new MCreatureWear;

			pCreatureWear->SetZone( g_pZone );
	
	

			pCreatureWear->SetCreatureType( (si.getSex()==MALE)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );

			pCreatureWear->SetGroundCreature();
			pCreatureWear->SetID(si.getObjectID());
			//pCreatureWear->SetAction(ACTION_MOVE);
			pCreatureWear->SetPosition( si.getX(), si.getY() );
			pCreatureWear->SetServerPosition( si.getX(), si.getY() );
			pCreatureWear->SetDirection( si.getDir() );
			pCreatureWear->SetCurrentDirection( si.getDir() );
			pCreatureWear->SetAction( ACTION_STAND );

			
			pCreatureWear->SetBodyColor1( si.getSkinColor() );
			
			pCreatureWear->SetStatus( MODIFY_MAX_HP, si.getMaxHP() );
			pCreatureWear->SetStatus( MODIFY_CURRENT_HP, si.getCurrentHP() );

			
			pCreatureWear->SetName( si.getName().c_str() );

			

			
			//----------------------------------------	
			
			//----------------------------------------	
			SetAddonToSlayer( pCreatureWear, &si );

			
			pCreatureWear->SetGuildNumber( -1 );

			
			if (!g_pZone->AddCreature( pCreatureWear ))
			{
				delete pCreatureWear;
				pCreatureWear = NULL;
				pCreature = NULL;
			}
		}
		//--------------------------------------------------
		
		//--------------------------------------------------
		else
		{
			if (pCreature->GetClassType()==MCreature::CLASS_CREATUREWEAR)
			{
				MCreatureWear* pCreatureWear = (MCreatureWear*)pCreature;

				pCreatureWear->SetCreatureType( (si.getSex()==MALE)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );

				pCreatureWear->SetGroundCreature();
				//pCreatureWear->SetID(si.getObjectID());

				
				pCreatureWear->SetGuildNumber( -1 );
				
				pCreatureWear->SetServerPosition( si.getX(), si.getY() );				
				pCreatureWear->MovePosition( si.getX(), si.getY() );
				pCreatureWear->SetDirection( si.getDir() );
				//pCreatureWear->SetCurrentDirection( si.getDir() );
				pCreatureWear->SetAction( ACTION_STAND );

				
				pCreatureWear->SetBodyColor1( si.getSkinColor() );
			
				pCreatureWear->SetStatus( MODIFY_MAX_HP, si.getMaxHP() );
				pCreatureWear->SetStatus( MODIFY_CURRENT_HP, si.getCurrentHP() );

				
				pCreatureWear->SetName( si.getName().c_str() );

				

				//----------------------------------------	
				
				//----------------------------------------	
				SetAddonToSlayer( pCreatureWear, &si );
			}
			else
			{
				DEBUG_ADD_FORMAT("[Error] the Creature is Not CreatureWear. id=%d", pCreature->GetID());
			}
		}

	}

#endif

	__END_CATCH
}
