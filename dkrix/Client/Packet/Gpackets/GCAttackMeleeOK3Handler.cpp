//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackMeleeOK3Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAttackMeleeOK3.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAttackMeleeOK3Handler::execute ( GCAttackMeleeOK3 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	// message
	WriteCombatCrashMarker("GCAttackMeleeOK3 attacker=%d target=%d",
		pPacket->getObjectID(), pPacket->getTargetObjectID());

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
	
	if (pCreature==NULL && pTargetCreature==NULL)
	{
		return;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (pCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d, Skill=%d", pPacket->getObjectID(), SKILL_ATTACK_MELEE);

		pTargetCreature->PacketSpecialActionResult( 
				RESULT_SKILL_ATTACK_MELEE,				//pPacket->getSkillType()+g_ActionInfoTable.GetMinResultActionInfo()
				pTargetCreature->GetID(),
				pTargetCreature->GetX(),
				pTargetCreature->GetY()
		);
		
		return;
	}
	//------------------------------------------------------
	
	//------------------------------------------------------
	else if (pTargetCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : TargetID=%d, Skill=%d", pPacket->getTargetObjectID(), SKILL_ATTACK_MELEE);

		//------------------------------------------------------
		
		//------------------------------------------------------
		pCreature->PacketSpecialActionToNobody(
						pCreature->GetBasicActionInfo(), 
						pCreature->GetX(),
						pCreature->GetY()						
		);

		
		
		
		
		
		return;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	MActionResult* pResult = new MActionResult;
	pResult->Add( new MActionResultNodeActionInfo( 
								pCreature->GetBasicActionInfo(),
								pPacket->getObjectID(), 
								pPacket->getTargetObjectID(), 
								pTargetCreature->GetX(),
								pTargetCreature->GetY()
								 ) 
				);

	//------------------------------------------------------
	
	//------------------------------------------------------
	pCreature->SetDirectionToPosition( pTargetCreature->GetX(), pTargetCreature->GetY() );

	//------------------------------------------------------
	
	//------------------------------------------------------
	pCreature->PacketSpecialActionToOther(
					pCreature->GetBasicActionInfo(), 
					pPacket->getTargetObjectID(), 
					pResult
	);

#endif

	__END_CATCH
}
