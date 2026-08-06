//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackArmsOK4Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAttackArmsOK4.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "MSlayerGear.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAttackArmsOK4Handler::execute ( GCAttackArmsOK4 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

			// message
	WriteCombatCrashMarker("GCAttackArmsOK4 target=%d skill=%d",
		pPacket->getTargetObjectID(), pPacket->getSkillType());

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
	MCreature* pTargetCreature = g_pZone->GetCreature( pPacket->getTargetObjectID() );
	
	if (pTargetCreature==NULL)
	{
		
		DEBUG_ADD_FORMAT("There's no such creature : TargetID=%d, Skill=%d", pPacket->getTargetObjectID(), SKILL_ATTACK_MELEE);
		
		return;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	
	if( pPacket->getSkillType() == SKILL_JABBING_VEIN || pPacket->getSkillType() == SKILL_MOLE_SHOT ||
					pPacket->getSkillType() == SKILL_TRIDENT || pPacket->getSkillType() == SKILL_QUICK_FIRE ||
					pPacket->getSkillType() == SKILL_ULTIMATE_BLOW || pPacket->getSkillType() == SKILL_HARPOON_BOMB)
	{
		pTargetCreature->PacketSpecialActionResult( 
					pPacket->getSkillType()+g_pActionInfoTable->GetMinResultActionInfo(),
					pTargetCreature->GetID(),
					pTargetCreature->GetX(),
					pTargetCreature->GetY()
		);
	}
	else
	{
		pTargetCreature->PacketSpecialActionResult( 
					RESULT_SKILL_ATTACK_GUN_SR,				//pPacket->getSkillType()+g_ActionInfoTable.GetMinResultActionInfo()
					pTargetCreature->GetID(),
					pTargetCreature->GetX(),
					pTargetCreature->GetY()
		);
	}



#endif

	__END_CATCH
}
