//////////////////////////////////////////////////////////////////////
//
// Filename    : GCKnocksTargetBackOK4Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCKnocksTargetBackOK4.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK4Handler::execute ( GCKnocksTargetBackOK4 * pPacket , Player * pPlayer )
	 
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
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getTargetObjectID() );

		
		if (pCreature != NULL)
		{			
			unsigned short x = pPacket->getX();
			unsigned short y = pPacket->getY();

			
			
			//MCreature::GetPositionToDirection(x, y, pPacket->getDir());

			
			pCreature->MovePosition( x, y );
			pCreature->SetServerPosition( x, y );
			pCreature->SetStop();

			
			pCreature->PacketSpecialActionResult( 
								pPacket->getSkillType() + (*g_pActionInfoTable).GetMinResultActionInfo(),
								pCreature->GetID(),
								pCreature->GetX(),
								pCreature->GetY()
			);			
		}
	}

#endif

	__END_CATCH
}
