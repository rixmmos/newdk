//////////////////////////////////////////////////////////////////////
//
// Filename    : GCKnocksTargetBackOK5Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCKnocksTargetBackOK5.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK5Handler::execute ( GCKnocksTargetBackOK5 * pPacket , Player * pPlayer )
	 
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
		
		return;
	}	

	//------------------------------------------------------
	
	//------------------------------------------------------
	MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );
	MCreature* pTargetCreature = g_pZone->GetCreature( pPacket->getTargetObjectID() );
	
	if (pCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d, Skill=%d", pPacket->getObjectID(), pPacket->getSkillType());				
		
		return;
	}

	if (pTargetCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : TargetID=%d, Skill=%d", pPacket->getTargetObjectID(), pPacket->getSkillType());
		
		return;
	}


	int skillType = pPacket->getSkillType();	//pCreature->GetBasicActionInfo()	

	if( skillType >= g_pActionInfoTable->GetSize() )
	{
		DEBUG_ADD_FORMAT("[GCKnocksTargetBackOK5] SkillType Error %d", skillType );
		return;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------	
	unsigned short x = pPacket->getX();
	unsigned short y = pPacket->getY();

	
	//MCreature::GetPositionToDirection(x, y, pPacket->getDir());

	//------------------------------------------------------
	
	//------------------------------------------------------
	MActionResult* pResult = new MActionResult;
	pResult->Add( new MActionResultNodeActionInfo( 
								skillType, 
								pPacket->getObjectID(), 
								pPacket->getTargetObjectID(),
								pTargetCreature->GetX(),
								pTargetCreature->GetY()
								 ) 
				);

	
	pResult->Add( new MActionResultNodeChangePosition( 
								pPacket->getTargetObjectID(),
								x, y)
				);
								
	//------------------------------------------------------
	
	//------------------------------------------------------
	pCreature->SetDirectionToPosition( pTargetCreature->GetX(), pTargetCreature->GetY() );

	//------------------------------------------------------
	
	//------------------------------------------------------
	pCreature->PacketSpecialActionToOther(
					
					skillType	, 
					pPacket->getTargetObjectID(), 
					pResult
	);

	//------------------------------------------------------
	
	
	//------------------------------------------------------
	pTargetCreature->SetServerPosition( x, y );

#endif

	__END_CATCH
}
