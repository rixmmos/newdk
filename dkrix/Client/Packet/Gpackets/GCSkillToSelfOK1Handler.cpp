//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToSelfOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToSelfOK1.h"
#include "ClientDef.h"
#include "PacketFunction2.h"
#include "SkillDef.h"
#include "MSkillManager.h"
#include "UIMessageManager.h"
#include "MEventManager.h"
#include "UIFunction.h"
#include "MGameStringTable.h"


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToSelfOK1Handler::execute ( GCSkillToSelfOK1 * pPacket , Player * pPlayer )
	 
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

	int skillID = pPacket->getSkillType();

	if( g_pActionInfoTable->GetSize() <= skillID )
	{
		DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
		SendBugReport("[ErrorGCSTSOK1H] Exceed SkillType %d", skillID );
		return;
	}

	if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
		skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);

	int resultActionInfo = skillID + (*g_pActionInfoTable).GetMinResultActionInfo();
	
	if( skillID == MAGIC_UN_TRANSFORM)
	{
		
		
		if(g_pPlayer->IsOusters())
		{
			g_pPlayer->SetWaitVerifyNULL();
			return;
		}
		else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
		{
			return;
			//resultActionInfo = RESULT_SKILL_INSTALL_TURRET;
		}
	}
	
	if( skillID == SKILL_ETERNITY && g_pPlayer->IsSlayer() )
	{	
		MEvent event;
		event.eventID = EVENTID_RESURRECT;
		event.eventDelay = 5000;
		event.eventFlag = EVENTFLAG_SHOW_DELAY_STRING;
		event.eventType = EVENTTYPE_ZONE;
		event.m_StringsID.push_back(STRING_MESSAGE_RESURRECT_AFTER_SECONDS);
		
		g_pEventManager->AddEvent(event);
		UI_CloseRequestResurrectWindow();
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );
	if(resultActionInfo == RESULT_SKILL_CONCEALMENT)				
	{
		int FrameSize = (*g_pActionInfoTable)[resultActionInfo][1].Count;
		int RemainFrame = delayFrame % FrameSize;
		if(RemainFrame < FrameSize / 2 && delayFrame > FrameSize)
		{
			delayFrame -= RemainFrame;
		} else
		{
			delayFrame += FrameSize - RemainFrame;
		}
	}

	g_pPlayer->SetEffectDelayFrame(resultActionInfo, delayFrame );

	
	if(skillID == SKILL_SOUL_CHAIN)
	{
		if (skillID < MIN_RESULT_ACTIONINFO)
		{
			(*g_pSkillInfoTable)[skillID].SetNextAvailableTime();
		}
//		g_pUIMessageManager->Execute(UI_CLOSE_TRACE_WINDOW, 0, 0, NULL);
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	g_pPlayer->PacketSpecialActionResult( 
					resultActionInfo,
					g_pPlayer->GetID(),
					g_pPlayer->GetX(),
					g_pPlayer->GetY()
	);

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	if (g_pPlayer->GetEFFECT_STAT()!=EFFECTSTATUS_NULL)
	{
		//int esDelayFrame = ConvertDurationToFrame( g_pPlayer->GetDURATION() );

		
		g_pPlayer->AddEffectStatus((EFFECTSTATUS)g_pPlayer->GetEFFECT_STAT(), delayFrame);	
		
		g_pPlayer->SetStatus( MODIFY_EFFECT_STAT, EFFECTSTATUS_NULL );
	}
	else
	{
		//------------------------------------------------------
		
		//------------------------------------------------------
		EFFECTSTATUS es = (*g_pActionInfoTable)[skillID].GetEffectStatus();

		
		if (es!=EFFECTSTATUS_NULL)
		{
			g_pPlayer->AddEffectStatus( es, delayFrame );
		}
	}

	if(skillID == SKILL_WILL_OF_LIFE )
	{
		g_pPlayer->CheckRegen();
		(*g_pSkillInfoTable)[skillID].SetAvailableTime( (3 + (g_pPlayer->GetLEVEL() / 10)) * 2 * 1000 );
	}

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );


	
	//------------------------------------------------------
	//
	
	//
	//------------------------------------------------------
	MActionResultNode* pActionResultNode = CreateActionResultNode(g_pPlayer, skillID);

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (pActionResultNode!=NULL)
	{
		pActionResultNode->Execute();

		delete pActionResultNode;
	}


	__END_CATCH
}
