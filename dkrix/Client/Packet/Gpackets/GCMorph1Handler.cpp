//--------------------------------------------------------------------------------
//
// Filename    : GCMorph1Handler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCMorph1.h"
#include "ClientDef.h"
#include "DXLib/AudioManager.h"
#include "MQuickSlot.h"
#include "SkillDef.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCMorph1Handler::execute ( GCMorph1 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		DEBUG_ADD("[Error] g_pZone or g_pPlayer NULL");

		return;
	}

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	g_SDLAudio.SetMute();

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	int objectID;
	
	switch (pPacket->getPCInfo2()->getPCType())
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_SLAYER :		
			objectID = ((PCSlayerInfo2*)pPacket->getPCInfo2())->getObjectID();
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_VAMPIRE :
			objectID = ((PCVampireInfo2*)pPacket->getPCInfo2())->getObjectID();			
		break;

		default :
			DEBUG_ADD("[Error] wrong PC Type");
		return;
	}

	if (g_pPlayer->GetID()!=objectID)
	{
		
		g_pZone->RemoveCreature( g_pPlayer->GetID() );

		
		g_pPlayer->SetID( objectID );

		g_pZone->SetPlayer();
		g_pPlayer->SetZone(g_pZone);
	}

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	if (g_pPlayer->IsSlayer())
	{
		g_pPlayer->ChangeToVampire();

		
		g_pPlayer->SetGuildNumber( 2 );
	}
	else
	{
		g_pPlayer->ChangeToSlayer();

		
		g_pPlayer->SetGuildNumber( -1 );
	}


	//--------------------------------------------------
	
	//--------------------------------------------------
	//g_pPlayer->SetSpecialActionInfo( MAGIC_LIGHT );

	//--------------------------------------------------
	
	//--------------------------------------------------
//	g_pPlayer->SetAttackModeNormal();


	//--------------------------------------------------
	
	//--------------------------------------------------	
	g_pQuickSlot = NULL;
	g_pArmsBand1 = NULL;
	g_pArmsBand2 = NULL;

			
	//--------------------------------------------------
	
	//--------------------------------------------------
	
	//--------------------------------------------------
	DEBUG_ADD( "Remove All Addon" );
	
	//for (int i=0; i<ADDON_MAX; i++)
	//{
	//	g_pPlayer->RemoveAddon((enum ADDON)i);
	//}			

	g_pPlayer->SetBasicActionInfo( SKILL_ATTACK_MELEE );
	g_pPlayer->SetBasicAttackDistance( 1 );
		

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------		
	switch (pPacket->getPCInfo2()->getPCType())
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_SLAYER :
		{
			SetPCSlayerInfo( (PCSlayerInfo2*)pPacket->getPCInfo2() );
		}
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_VAMPIRE :
		{
			SetPCVampireInfo( (PCVampireInfo2*)pPacket->getPCInfo2() );
		}
		break;

		//--------------------------------------------------
		
		//--------------------------------------------------
		case PC_OUSTERS :
		{
			SetPCOustersInfo( (PCOustersInfo2*)pPacket->getPCInfo2() );
		}
		break;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	SetGearInfo( pPacket->getGearInfo() );

	
	g_pPlayer->CalculateStatus();

	//--------------------------------------------------
	
	//--------------------------------------------------
	SetInventoryInfo( pPacket->getInventoryInfo() );

	//--------------------------------------------------
	
	//--------------------------------------------------
	SetExtraInfo( pPacket->getExtraInfo() );
	

	//--------------------------------------------------
	// Skill 
	//--------------------------------------------------


	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	g_SDLAudio.UnSetMute();


#endif

	__END_CATCH
}
