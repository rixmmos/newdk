//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddWolfHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddWolf.h"
#include "ClientDef.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddWolfHandler::execute ( GCAddWolf * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	
	int wolfCreatureType = 186;

	if( pPacket->getItemType() == 39 )			
		wolfCreatureType = CREATURETYPE_WER_WOLF;
	
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
		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreatureWear;
//			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, vi.getAdvancementLevel() );

			pCreature->SetZone( g_pZone );
			
			
			pCreature->SetName( pPacket->getName().c_str() );


			//pCreature->SetCreatureType( 0 );
			pCreature->SetCreatureType( wolfCreatureType );
			pCreature->SetGroundCreature();
			pCreature->SetID(pPacket->getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			//pPacket->getName()
			
			
			pCreature->SetGuildNumber( pPacket->getGuildID() );

			if (!g_pZone->AddCreature( pCreature ))
			{
				delete pCreature;
				pCreature = NULL;
			}
		}
		//--------------------------------------------------
		
		//--------------------------------------------------
		else
		{
			pCreature->SetCreatureType( wolfCreatureType );

			
			pCreature->SetGuildNumber( pPacket->getGuildID() );

			pCreature->SetGroundCreature();
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			pCreature->SetAction( ACTION_STAND );			

			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			//--------------------------------------------------
			
			//--------------------------------------------------
			MActionResult* pResult = new MActionResult;

			pResult->Add( new MActionResultNodeChangeCreatureType( pCreature->GetID(), wolfCreatureType ) );

			//--------------------------------------------------
			
			//--------------------------------------------------								
			ExecuteActionInfoFromMainNode(
				RESULT_MAGIC_TRANSFORM_TO_WOLF,										
			
				pCreature->GetX(), pCreature->GetY(), 0,
				pCreature->GetDirection(),														
				
				OBJECTID_NULL,												
				pCreature->GetX(), pCreature->GetY(), 0, 
				
				0,													
				
				pResult, //NULL,
				
				false);			

			//pCreature->SetDelay( 1000 );
		}	

		if( wolfCreatureType == CREATURETYPE_WER_WOLF )
			pCreature->SetBodyColor1( pPacket->getColor() );
	}

	
//	__BEGIN_HELP_EVENT
//		//ExecuteHelpEvent( HE_CREATURE_APPEAR_VAMPIRE );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
