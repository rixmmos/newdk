//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillFailed2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillFailed2.h"
#include "ClientDef.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillFailed2Handler::execute ( GCSkillFailed2 * pPacket , Player * pPlayer )
	 
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
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	int skilltype = pPacket->getSkillType();
	
//	if(skilltype == SKILL_BLOOD_DRAIN)
//	{

//	}
	if( skilltype >= g_pActionInfoTable->GetSize() )
		return;
	
	if(pTargetCreature == NULL && pCreature != NULL && pCreature->IsOusters() && skilltype == SKILL_ABSORB_SOUL)
	{
		MItem *pItem = g_pZone->GetItem( pPacket->getTargetObjectID() );
		if(pItem != NULL)
		{
			pTargetCreature = ((MCorpse *)pItem)->GetCreature();
		}
	}

	if (pCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d, Skill=%d", pPacket->getObjectID(), SKILL_ATTACK_MELEE);				
		
		return;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (skilltype==SKILL_ATTACK_MELEE
		|| skilltype==SKILL_ATTACK_ARMS)
	{
		skilltype = pCreature->GetBasicActionInfo();
	}

	//
	if (skilltype >= g_pActionInfoTable->GetMinResultActionInfo())
	{
		DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", skilltype);
		return;
	}
	
	if( pPacket->getGrade() > 0 && (*g_pActionInfoTable)[skilltype].IsUseActionStep() )
		skilltype = (*g_pActionInfoTable)[skilltype].GetActionStep( pPacket->getGrade() - 1 );


	DEBUG_ADD_FORMAT("SkillType = %d", skilltype);

	int resultSkillID = g_pActionInfoTable->GetMinResultActionInfo() + skilltype;

	if (pCreature->IsOusters() && skilltype==SKILL_ABSORB_SOUL)
	{
		pCreature->SetStopAbsorbSoul();
	}
	
	
	if(skilltype == SKILL_GRENADE_ATTACK) 
		return;
	// 2005, 1, 10, sobeit add end

	//------------------------------------------------------
	// Other
	//------------------------------------------------------
	if ((*g_pActionInfoTable)[skilltype].IsTargetOther())
	{		
		if (skilltype==SKILL_BLOOD_DRAIN || skilltype == SKILL_BITE_OF_DEATH )
		{
			pCreature->SetStopBloodDrain();
			
		}
		
		
		if (pTargetCreature==NULL)
		{
			
			DEBUG_ADD_FORMAT("There's no such creature : TargetID=%d, Skill=%d", pPacket->getTargetObjectID(), SKILL_ATTACK_MELEE);

			//------------------------------------------------------
			
			//------------------------------------------------------
			pCreature->PacketSpecialActionToNobody(
							skilltype,
							pCreature->GetX(),
							pCreature->GetY()						
			);			
			
			return;
		}
		
		
		if (pCreature==pTargetCreature)
		{
			pCreature->PacketSpecialActionToSelf(
						skilltype, 				
						NULL
			);

			return;
		}
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		pCreature->SetDirectionToPosition( pTargetCreature->GetX(), pTargetCreature->GetY() );

		pCreature->PacketSpecialActionToOther(
						skilltype, 
						pPacket->getTargetObjectID(), 
						NULL
		);		
		
	}
	//------------------------------------------------------
	// Self
	//------------------------------------------------------
	else if ((*g_pActionInfoTable)[skilltype].IsTargetSelf())
	{
		pCreature->PacketSpecialActionToSelf(
						skilltype, 				
						NULL
		);
	}
	//------------------------------------------------------
	// Zone
	//------------------------------------------------------
	else if ((*g_pActionInfoTable)[skilltype].IsTargetZone())
	{
		pCreature->PacketSpecialActionToSector(
						skilltype, 
						pCreature->GetX(),
						pCreature->GetY(),
						NULL
		);
	}

#endif

	__END_CATCH
}
