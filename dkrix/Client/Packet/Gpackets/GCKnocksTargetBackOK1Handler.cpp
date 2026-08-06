//////////////////////////////////////////////////////////////////////
//
// Filename    : GCKnocksTargetBackOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCKnocksTargetBackOK1.h"
#include "ClientDef.h"
#include "MSlayerGear.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK1Handler::execute ( GCKnocksTargetBackOK1 * pPacket , Player * pPlayer )
	 
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

		//------------------------------------------------------
		
		//------------------------------------------------------
		if (pPacket->getSkillSuccess())
		{
			
			if (pCreature != NULL)
			{			
				unsigned short x = pPacket->getX();
				unsigned short y = pPacket->getY();

				
				
				//MCreature::GetPositionToDirection(x, y, pPacket->getDir());

				MActionResult* pResult = new MActionResult;

				pResult->Add( new MActionResultNodeChangePosition(
										pPacket->getObjectID(),
										x, y)
							);

			

				
				//pCreature->SetPosition( x, y );
				//pCreature->SetServerPosition( x, y );
				//pCreature->SetStop();

				
				//pCreature->PacketSpecialActionResult( 
				//					RESULT_SKILL_KNOCKS_TARGET_BACK//pPacket->getSkillType() + g_ActionInfoTable.GetMinResultActionInfo()
				//);			

				g_pPlayer->PacketAddActionResult( 0 , pResult);

				//------------------------------------------------------
				
				
				//------------------------------------------------------
				pCreature->SetServerPosition( x, y );
			}
		}
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		if (g_pCurrentMagazine==NULL)
		{
			DEBUG_ADD("[Error] Current Magazine Not Exist!");			
		}
		else
		{
			g_pCurrentMagazine->SetNumber( pPacket->getBullet() );
		}
	}

	
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

#endif

	__END_CATCH
}
