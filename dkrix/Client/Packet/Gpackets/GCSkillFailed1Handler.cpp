//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillFailed1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files

#include "Client_PCH.h"
#include "GCSkillFailed1.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "MSkillManager.h"
#include "Mintr.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillFailed1Handler::execute ( GCSkillFailed1 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		

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
	int skillID = pPacket->getSkillType();
	
	if( skillID >= g_pActionInfoTable->GetSize() )
		return;

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	if (g_pPlayer->GetItemCheckBufferStatus()==MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY)
	{
		g_pPlayer->ClearItemCheckBuffer();
	}
	else if(g_pPlayer->IsOusters() && skillID == SKILL_ABSORB_SOUL)
	{

		g_pPlayer->SetStopAbsorbSoul();
	} else if (g_pPlayer->IsSlayer() && skillID == SKILL_ETERNITY )
	{
//		g_pSystemMessage->Add( (*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_RESURRECT].GetString() );
		UI_SetDelayEternity();
	}

		
	if (g_pSkillInfoTable!=NULL)
	{
		
		
		
		if(false == (*g_pActionInfoTable)[skillID].IsIgnoreSkillFailDelay())
//		if (skillID != MAGIC_PARALYZE
//			&& skillID != MAGIC_CAUSE_CRITICAL_WOUNDS
//			&& skillID != SKILL_ENERGY_DROP
//			&& skillID != SKILL_VIGOR_DROP
//			&& skillID != SKILL_POISON_STORM
//			&& skillID != SKILL_ACID_STORM
//			&& skillID != SKILL_BLOODY_STORM
//			&& skillID != SKILL_MAGIC_ELUSION
//			&& skillID != SKILL_POISON_MESH
//			&& skillID != SKILL_ILLUSION_OF_AVENGE			
//			)
		{
			if (skillID < MIN_RESULT_ACTIONINFO)
			{
				(*g_pSkillInfoTable)[skillID].SetAvailableTime();
			}
		}

		if( skillID == SKILL_WILL_OF_LIFE )
		{
			(*g_pSkillInfoTable)[skillID].SetAvailableTime( (3 + (g_pPlayer->GetLEVEL() / 10)) * 2 * 1000 );
		}
	}

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	switch (skillID)
	{
		case SKILL_BITE_OF_DEATH :
		case SKILL_BLOOD_DRAIN :
			g_pPlayer->SetStopBloodDrain();

//			g_pPlayer->StopBloodDrain();
			break;
		case SKILL_SOUL_CHAIN :
			//gC_vs_ui.SetCannotTrace();
			g_pPlayer->SetCannotTrace();
			break;		
		case SKILL_BURNING_SOUL_CHARGING:
			g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1);
			break;
	}

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );


	__END_CATCH
}
