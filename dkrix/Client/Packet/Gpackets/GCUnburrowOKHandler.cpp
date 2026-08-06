//--------------------------------------------------------------------------------
//
// Filename    : GCUnburrowOKHandler.cpp
// Written By  : elca, Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCUnburrowOK.h"
#include "ClientDef.h"
#include "SkillDef.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCUnburrowOKHandler::execute ( GCUnburrowOK * pPacket , Player * pPlayer )
	 
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

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	g_pPlayer->SetServerPosition( pPacket->getX(), pPacket->getY() );	
	g_pPlayer->MovePosition( pPacket->getX(), pPacket->getY() );
	g_pPlayer->SetDirection( pPacket->getDir() );

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	g_pPlayer->SetGroundCreature();

	//--------------------------------------------------
	
	//--------------------------------------------------		
	ExecuteActionInfoFromMainNode(
		RESULT_MAGIC_HIDE,										
	
		g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
		g_pPlayer->GetDirection(),														
		
		OBJECTID_NULL,												
		g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 
		
		0,													
		
		NULL,
		
		false);			

#endif

	__END_CATCH
}
