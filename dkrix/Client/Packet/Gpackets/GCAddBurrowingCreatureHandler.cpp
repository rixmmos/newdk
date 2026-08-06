//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddBurrowingCreatureHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddBurrowingCreature.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddBurrowingCreatureHandler::execute ( GCAddBurrowingCreature * pPacket , Player * pPlayer )
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
		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreature;

			pCreature->SetZone( g_pZone );

			
			pCreature->SetCreatureType( CREATURETYPE_VAMPIRE_MALE1 );	


			pCreature->SetName( pPacket->getName().c_str() );

			
			pCreature->SetUndergroundCreature();
			pCreature->SetID(pPacket->getObjectID());
			
			pCreature->SetPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( DIRECTION_DOWN );
			pCreature->SetCurrentDirection( DIRECTION_DOWN );
			pCreature->SetAction( ACTION_STAND );

			pCreature->SetGuildNumber( -1 );

			pCreature->SetStatus( MODIFY_MAX_HP, 100 );
			pCreature->SetStatus( MODIFY_CURRENT_HP, 100 );

			//si.getName()
			

			

			if (!g_pZone->AddCreature( pCreature ))
			{
				delete pCreature;
			}
		}
		else
		{
			
			pCreature->SetUndergroundCreature();

			
			pCreature->SetName( pPacket->getName().c_str() );

			
			pCreature->MovePosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );

			pCreature->SetGuildNumber( -1 );
		}
	}

#endif

	__END_CATCH
}
