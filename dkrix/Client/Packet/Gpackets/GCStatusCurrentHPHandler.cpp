//////////////////////////////////////////////////////////////////////
//
// Filename    : GCStatusCurrentHPHandler.cpp
// Written By  : elca@ewestsoft.com
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCStatusCurrentHP.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCStatusCurrentHPHandler::execute ( GCStatusCurrentHP * pPacket , Player * pPlayer )
	 
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
			DEBUG_ADD_FORMAT("[Error] SetHP:There is no such Creature. id=%d", pPacket->getObjectID());
		}
		else
		{		
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );
		}
	}

#endif

	__END_CATCH
}
