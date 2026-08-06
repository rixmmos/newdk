//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCreatureDiedHandler.cpp
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCreatureDied.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCCreatureDiedHandler::execute ( GCCreatureDied * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	// message

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
		int creatureID = pPacket->getObjectID();

		MCreature* pCreature = g_pZone->GetCreature(creatureID);

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			// message
			DEBUG_ADD_FORMAT("[Error] Not Exist Creature ID=%d", pPacket->getObjectID());
		}
		else
		{
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			//pCreature->SetDead();

			if (pCreature->IsAlive())
			{
				pCreature->SetDead();
			}
			else
			{
			//	pCreature->SetCorpse();
			}

			g_pZone->AddCorpseFromCreature( creatureID );
		}
	}

#endif

	__END_CATCH
}
