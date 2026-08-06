//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackArmsOK3Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAttackArmsOK3.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "MSlayerGear.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAttackArmsOK3Handler::execute ( GCAttackArmsOK3 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

			// message
	WriteCombatCrashMarker("GCAttackArmsOK3 attacker=%d skill=%d target=%d,%d",
		pPacket->getObjectID(), pPacket->getSkillType(), pPacket->getTargetX(), pPacket->getTargetY());

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
		
		DEBUG_ADD_FORMAT("There's no such creature : ID=%d, Skill=%d", pPacket->getObjectID(), pPacket->getSkillType());				
		
		return;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	pCreature->SetDirectionToPosition( pPacket->getTargetX(), pPacket->getTargetY() );

	//------------------------------------------------------
	
	
	//------------------------------------------------------
	if( pPacket->getSkillType() == SKILL_JABBING_VEIN || pPacket->getSkillType() == SKILL_MOLE_SHOT ||
					pPacket->getSkillType() == SKILL_TRIDENT || pPacket->getSkillType() == SKILL_QUICK_FIRE ||
					pPacket->getSkillType() == SKILL_ULTIMATE_BLOW || pPacket->getSkillType() == SKILL_HARPOON_BOMB)
	{
		pCreature->PacketSpecialActionToNobody(
						pPacket->getSkillType()	,	
						pPacket->getTargetX(),
						pPacket->getTargetY()						
		);
	} else
	{
		pCreature->PacketSpecialActionToNobody(
						pCreature->GetBasicActionInfo()	,	
						pPacket->getTargetX(),
						pPacket->getTargetY()						
		);
	}



#endif

	__END_CATCH
}
