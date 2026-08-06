//////////////////////////////////////////////////////////////////////
//
// Filename    : GCKnocksTargetBackOK2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCKnocksTargetBackOK2.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK2Handler::execute ( GCKnocksTargetBackOK2 * pPacket , Player * pPlayer )
	 
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
		
		return;
	}	


	int skillType = pPacket->getSkillType();	
	
	if( skillType >= g_pActionInfoTable->GetSize() )
	{
		DEBUG_ADD_FORMAT("[GCKnocksTargetBackOK2] SkillType Error %d",skillType );
		return;
	}
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	unsigned short x = pPacket->getX();
	unsigned short y = pPacket->getY();
		

	//------------------------------------------------------
	
	//------------------------------------------------------
	MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );
	
	if (pCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d, Skill=%d", pPacket->getObjectID(), pPacket->getSkillType());
		
		
		g_pPlayer->SetServerPosition( x, y );		
		g_pPlayer->MovePosition( x, y );
		g_pPlayer->SetStop();

		
		g_pPlayer->PacketSpecialActionResult( 
							pPacket->getSkillType() + (*g_pActionInfoTable).GetMinResultActionInfo(),
							OBJECTID_NULL,
							x,
							y
		);
	}
	else
	{	
		
		//MCreature::GetPositionToDirection(x, y, pPacket->getDir());


		//------------------------------------------------------
		
		//------------------------------------------------------
		pCreature->SetDirectionToPosition( g_pPlayer->GetX(), g_pPlayer->GetY() );

		//------------------------------------------------------
		
		//------------------------------------------------------
		
		//g_pPlayer->PacketSpecialActionResult( SKILL_ATTACK_MELEE + (*g_pActionInfoTable).GetMinResultActionInfo() );

		 

		//------------------------------------------------------
		
		//------------------------------------------------------
		pCreature->PacketSpecialActionToOther(
						skillType,
						g_pPlayer->GetID(), 
						NULL	//pResult
		);

		//------------------------------------------------------
		
		
		//------------------------------------------------------
		
		g_pPlayer->SetServerPosition( x, y );		
		g_pPlayer->MovePosition( x, y );
		g_pPlayer->SetStop();

		
		g_pPlayer->PacketSpecialActionResult( 
							pPacket->getSkillType() + (*g_pActionInfoTable).GetMinResultActionInfo(),
							pCreature->GetID(),
							pCreature->GetX(),
							pCreature->GetY()
		);
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
