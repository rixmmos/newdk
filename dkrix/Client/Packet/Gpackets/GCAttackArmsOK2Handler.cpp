//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackArmsOK2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAttackArmsOK2.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAttackArmsOK2Handler::execute ( GCAttackArmsOK2 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	// message
	WriteCombatCrashMarker("GCAttackArmsOK2 attacker=%d skill=%d short=%d long=%d",
		pPacket->getObjectID(), pPacket->getSkillType(), pPacket->getShortCount(), pPacket->getLongCount());

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
	
	if (pCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d, Skill=%d", pPacket->getObjectID(), SKILL_ATTACK_MELEE);				
		
		if( pPacket->getSkillType() == SKILL_JABBING_VEIN || pPacket->getSkillType() == SKILL_MOLE_SHOT ||
					pPacket->getSkillType() == SKILL_TRIDENT || pPacket->getSkillType() == SKILL_QUICK_FIRE ||
					pPacket->getSkillType() == SKILL_ULTIMATE_BLOW || pPacket->getSkillType() == SKILL_HARPOON_BOMB)
		{
			TYPE_ACTIONINFO resultActionInfo = pPacket->getSkillType() + g_pActionInfoTable->GetMinResultActionInfo();
			
			g_pPlayer->PacketSpecialActionResult( 
									resultActionInfo,	
									g_pPlayer->GetID(), 
									g_pPlayer->GetX(),
									g_pPlayer->GetY()								
					);
		} else
		{
			
			g_pPlayer->PacketSpecialActionResult( 
									RESULT_SKILL_ATTACK_GUN_AR,	
									g_pPlayer->GetID(), 
									g_pPlayer->GetX(),
									g_pPlayer->GetY()								
					);
		}
	}
	else
	{
		TYPE_ACTIONINFO actionInfo = pPacket->getSkillType();
		//------------------------------------------------------
		
		//------------------------------------------------------
		pCreature->SetDirectionToPosition( g_pPlayer->GetX(), g_pPlayer->GetY() );

		//------------------------------------------------------
		
		//------------------------------------------------------
		//g_pPlayer->PacketSpecialActionResult( SKILL_ATTACK_MELEE + g_ActionInfoTable.GetMinResultActionInfo() );

		if( actionInfo == SKILL_JABBING_VEIN || actionInfo == SKILL_TRIDENT || actionInfo == SKILL_MOLE_SHOT ||
			actionInfo == SKILL_QUICK_FIRE || actionInfo == SKILL_ULTIMATE_BLOW || pPacket->getSkillType() == SKILL_HARPOON_BOMB)
		{
			MActionResult* pResult = new MActionResult;
			pResult->Add( new MActionResultNodeActionInfo( 
										actionInfo,	
										pPacket->getObjectID(), 
										g_pPlayer->GetID(), 
										g_pPlayer->GetX(),
										g_pPlayer->GetY()								
										) 
						);
			//------------------------------------------------------
			
			//------------------------------------------------------
			pCreature->PacketSpecialActionToOther(
							actionInfo,	
							g_pPlayer->GetID(), 
							pResult
			);
		} else
		{
			MActionResult* pResult = new MActionResult;
			pResult->Add( new MActionResultNodeActionInfo( 
										pCreature->GetBasicActionInfo(),	
										pPacket->getObjectID(), 
										g_pPlayer->GetID(), 
										g_pPlayer->GetX(),
										g_pPlayer->GetY()								
										) 
						);
			//------------------------------------------------------
			
			//------------------------------------------------------
			pCreature->PacketSpecialActionToOther(
							pCreature->GetBasicActionInfo(),	
							g_pPlayer->GetID(), 
							pResult
			);
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
