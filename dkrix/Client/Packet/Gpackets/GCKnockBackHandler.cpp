//////////////////////////////////////////////////////////////////////
//
// Filename    : GCKnockBackHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCKnockBack.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCKnockBackHandler::execute ( GCKnockBack * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
	

	int objectID = pPacket->getObjectID();
	int ox = pPacket->getOriginX();
	int oy = pPacket->getOriginY();
	int nx = pPacket->getTargetX();
	int ny = pPacket->getTargetY();	
	
	MCreature* pCreature = g_pZone->GetCreature( objectID );

	if (pCreature!=NULL)
	{
		
		if (pCreature->GetID()!=g_pPlayer->GetID())
		{
			pCreature->SetStop();		
			
			pCreature->MovePosition( ox, oy );
			pCreature->SetServerPosition( ox, oy );
		}

		
		pCreature->KnockBackPosition( nx, ny );
	}
	else
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d", pPacket->getObjectID());
	}

#endif

	__END_CATCH
}
