//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddMonsterFromTransformationHandler.cpp
// Written By  : elca@ewestsoft.com
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddMonsterFromTransformation.h"
#include "ClientDef.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddMonsterFromTransformationHandler::execute ( GCAddMonsterFromTransformation * pPacket , Player * pPlayer )
	 
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
		#ifdef __DEBUG_OUTPUT__
			DEBUG_ADD_FORMAT("[AddMonster] type=%d, id=%d, xy=(%d,%d), dir=%d", 
								pPacket->getMonsterType(),
								pPacket->getObjectID(), 
								pPacket->getX(), pPacket->getY(), 
								pPacket->getDir());
		#endif

		//AfxMessageBox( pPacket->toString().c_str() );
		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreature;

			pCreature->SetZone( g_pZone );

			//pCreature->SetCreatureType( 0 );
			pCreature->SetCreatureType( pPacket->getMonsterType() );
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

			// [ TEST CODE ]
			
			//char str[20];
			//sprintf(str, "ID=%d", pCreature->GetID());
			//pCreature->SetName( (*g_pCreatureTable)[pPacket->getMonsterType()].Name.GetString() );
			pCreature->SetName( pPacket->getMonsterName().c_str() );

			
			pCreature->SetGuildNumber( 1 );

			
			//pCreature->SetName( pPacket->getName().toString().c_str() );

			
			

			if (!g_pZone->AddCreature( pCreature ))
			{	
				delete pCreature;
				pCreature = NULL;
			}
			else
			{
				//------------------------------------------------------------
				
				//------------------------------------------------------------
//				LoadCreatureType( pPacket->getMonsterType() );			
			}
		}
		else
		{
			
			bool reAdd = false;

			if (pCreature->GetClassType()!=MCreature::CLASS_CREATURE)
			{
				g_pZone->RemoveCreature( pCreature->GetID() );

				pCreature = new MCreature;

				pCreature->SetID(pPacket->getObjectID());

				pCreature->SetZone( g_pZone );
				
				//pCreature->SetName( (*g_pCreatureTable)[pPacket->getMonsterType()].Name.GetString() );
				pCreature->SetName( pPacket->getMonsterName().c_str() );

				pCreature->SetPosition( pPacket->getX(), pPacket->getY() );				

				reAdd = true;
			}
			else
			{
				pCreature->MovePosition( pPacket->getX(), pPacket->getY() );				
			}

			pCreature->SetCreatureType( pPacket->getMonsterType() );
			pCreature->SetGroundCreature();
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			pCreature->SetAction( ACTION_STAND );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			
			pCreature->SetGuildNumber( 1 );	
			
			if (reAdd)
			{
				if (!g_pZone->AddCreature( pCreature ))
				{	
					delete pCreature;
					pCreature = NULL;
				}
				else
				{
					//------------------------------------------------------------
					
					//------------------------------------------------------------
//					LoadCreatureType( pPacket->getMonsterType() );			
				}
			}
		}

		if (pCreature!=NULL)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );

			//--------------------------------------------------
			
			//--------------------------------------------------		
			ExecuteActionInfoFromMainNode(
				RESULT_MAGIC_HIDE,										
			
				pCreature->GetX(), pCreature->GetY(), 0,
				pCreature->GetDirection(),														
				
				OBJECTID_NULL,												
				pCreature->GetX(), pCreature->GetY(), 0, 
				
				0,													
				
				NULL,
				
				false);			
		}
	}

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_CREATURE_APPEAR_MONSTER );
//	__END_HELP_EVENT
#endif

	__END_CATCH
}
