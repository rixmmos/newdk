//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToInventoryOK2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToInventoryOK2.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToInventoryOK2Handler::execute ( GCSkillToInventoryOK2 * pPacket , Player * pPlayer )
	 
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
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );
			
		
		if (pCreature != NULL)
		{
			int delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

			//------------------------------------------------------------
			
			//------------------------------------------------------------
			pCreature->SetEffectDelayFrame( pPacket->getSkillType(), delayFrame );


			//Duration_t	m_Duration;
			pCreature->PacketSpecialActionToInventoryItem(
								pPacket->getSkillType()								
			);
		}
	}	
	

#endif

	__END_CATCH
}
