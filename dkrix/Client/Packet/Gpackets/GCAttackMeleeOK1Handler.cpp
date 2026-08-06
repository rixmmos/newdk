//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackMeleeOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAttackMeleeOK1.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAttackMeleeOK1Handler::execute ( GCAttackMeleeOK1 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
	
	// message
	WriteCombatCrashMarker("GCAttackMeleeOK1 target=%d short=%d long=%d",
		pPacket->getObjectID(), pPacket->getShortCount(), pPacket->getLongCount());

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_SKILL_SUCCESS)
	{		
		g_pPlayer->SetWaitVerifyNULL();
	}
	else
	{
		DEBUG_ADD("[Error] Player is not WaitVerifySkillSuccess");
	}

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	WriteCombatCrashMarker("GCAttackMeleeOK1 before modify target=%d short=%d long=%d",
		pPacket->getObjectID(), pPacket->getShortCount(), pPacket->getLongCount());
	AffectModifyInfo(g_pPlayer, pPacket);
	WriteCombatCrashMarker("GCAttackMeleeOK1 after modify target=%d hp=%d/%d",
		pPacket->getObjectID(), g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP());

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
		WriteCombatCrashMarker("GCAttackMeleeOK1 creature lookup target=%d creature=%p",
			pPacket->getObjectID(), pCreature);

		
		if (pCreature != NULL)
		{
			
			 
			
			if (pCreature!=NULL)
			{
				WriteCombatCrashMarker("GCAttackMeleeOK1 before action target=%d basicAI=%d creature=%d,%d",
					pPacket->getObjectID(), g_pPlayer->GetBasicActionInfo(), pCreature->GetX(), pCreature->GetY());
				MActionResult* pResult = new MActionResult;
			
				pResult->Add( new MActionResultNodeActionInfo( 
											g_pPlayer->GetBasicActionInfo(),
											g_pPlayer->GetID(), 
											pCreature->GetID(), 
											pCreature->GetX(),
											pCreature->GetY()
											 ) 
							);

				//g_pPlayer->PacketAddActionResult( effectID, pResult );
				g_pPlayer->PacketAddActionResult( 0, pResult );
				WriteCombatCrashMarker("GCAttackMeleeOK1 after action target=%d", pPacket->getObjectID());
			}
		}
	}	
		

#endif

	__END_CATCH
}
