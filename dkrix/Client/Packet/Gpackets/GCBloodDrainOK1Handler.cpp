//////////////////////////////////////////////////////////////////////
//
// Filename    : GCBloodDrainOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
// include files
#include "GCBloodDrainOK1.h"
#include "ClientDef.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCBloodDrainOK1Handler::execute ( GCBloodDrainOK1 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__


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
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

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
			
			pCreature->SetDrainCreatureID( g_pPlayer->GetID() );

			
			pCreature->PacketSpecialActionResult( 
					SKILL_BLOOD_DRAIN + (*g_pActionInfoTable).GetMinResultActionInfo(),
					pCreature->GetID(),
					pCreature->GetX(),
					pCreature->GetY()
			);	

			int delayFrame = 691200;	
			pCreature->AddEffectStatus( EFFECTSTATUS_BLOOD_DRAIN, delayFrame );	
			

//			if(g_pPlayer->IsVampire())
//			{
//				g_pPlayer->SetDirectionToPosition(pCreature->GetX(), pCreature->GetY());
//				g_pPlayer->SetAction(ACTION_VAMPIRE_DRAIN);
//				
//			}
//			// 2004, 6, 9 sobeit add end
		}
	}
	
	g_pPlayer->ClearStopBloodDrain();

#endif

	__END_CATCH
}
