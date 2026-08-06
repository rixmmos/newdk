//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackMeleeOK2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAttackMeleeOK2.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAttackMeleeOK2Handler::execute ( GCAttackMeleeOK2 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	// message
	WriteCombatCrashMarker("GCAttackMeleeOK2 attacker=%d short=%d long=%d",
		pPacket->getObjectID(), pPacket->getShortCount(), pPacket->getLongCount());

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
	WriteCombatCrashMarker("GCAttackMeleeOK2 creature lookup attacker=%d creature=%p",
		pPacket->getObjectID(), pCreature);

	if (pCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d", pPacket->getObjectID());				
		
		//------------------------------------------------------------------
		
		//------------------------------------------------------------------
		g_pPlayer->PacketSpecialActionResult( 
								RESULT_SKILL_ATTACK_MELEE,//pCreature->GetBasicActionInfo(),
								g_pPlayer->GetID(), 
								g_pPlayer->GetX(),
								g_pPlayer->GetY()
					);
	}
	else
	{
		WriteCombatCrashMarker("GCAttackMeleeOK2 before action attacker=%d basicAI=%d player=%d,%d",
			pPacket->getObjectID(), pCreature->GetBasicActionInfo(), g_pPlayer->GetX(), g_pPlayer->GetY());
		//------------------------------------------------------
		
		//------------------------------------------------------
		pCreature->SetDirectionToPosition( g_pPlayer->GetX(), g_pPlayer->GetY() );

		//------------------------------------------------------
		
		//------------------------------------------------------
		//g_pPlayer->PacketSpecialActionResult( SKILL_ATTACK_MELEE + g_ActionInfoTable.GetMinResultActionInfo() );
		 
		WriteCombatCrashMarker("GCAttackMeleeOK2 skipped monster action attacker=%d basicAI=%d",
			pPacket->getObjectID(), pCreature->GetBasicActionInfo());
	}

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	WriteCombatCrashMarker("GCAttackMeleeOK2 before modify attacker=%d short=%d long=%d",
		pPacket->getObjectID(), pPacket->getShortCount(), pPacket->getLongCount());
	AffectModifyInfo(g_pPlayer, pPacket);
	WriteCombatCrashMarker("GCAttackMeleeOK2 after modify attacker=%d hp=%d/%d",
		pPacket->getObjectID(), g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP());

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

#endif

	__END_CATCH
}
