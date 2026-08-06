//////////////////////////////////////////////////////////////////////
//
// Filename    : GCLearnSkillOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCLearnSkillOK.h"
#include "ClientDef.h"
#include "MSkillManager.h"
#include "MGameStringTable.h"
#include "TempInformation.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCLearnSkillOKHandler::execute ( GCLearnSkillOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
		
	
	
	//g_TempInformation.Mode = TempInformation::MODE_NULL;
	g_pTempInformation->SetMode(TempInformation::MODE_NULL);
						

	int domainType	= pPacket->getSkillDomainType();
	int skillType	= pPacket->getSkillType();


	
	MSkillDomain& swordDomain = (*g_pSkillManager)[domainType];

	swordDomain.SetNewSkill();
	bool bLearn = swordDomain.LearnSkill( (ACTIONINFO)skillType );
	
	if(bLearn == true || !g_pPlayer->IsOusters())
	{
		
		g_pSkillAvailable->AddSkill( (ACTIONINFO)skillType );
		
		SKILLINFO_NODE& skillInfo = (*g_pSkillInfoTable)[skillType];
		
		skillInfo.SetExpLevel( 1 );
		skillInfo.SetSkillExp( 0 );
		
		skillInfo.SetDelayTime( skillInfo.maxDelay );
		skillInfo.SetEnable( true );
		
		skillInfo.SetNextAvailableTime();
	}
	else if( g_pPlayer->IsOusters() )
	{
		
		SKILLINFO_NODE& skillInfo = (*g_pSkillInfoTable)[skillType];
		skillInfo.SetExpLevel( skillInfo.GetExpLevel()+1 );
		
	}
	
	// DEBUG_ADD_FORMAT( "[Error] TempInformation Mode is Not SKILL_LEARN : %d", (int)g_TempInformation.Mode);

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	g_pSkillAvailable->SetAvailableSkills();
	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	g_pGameMessage->AddFormat((*g_pGameStringTable)[STRING_LEARN_SKILL].GetString(), (*g_pSkillInfoTable)[skillType].GetHName());

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_SKILL_LEARNED );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
