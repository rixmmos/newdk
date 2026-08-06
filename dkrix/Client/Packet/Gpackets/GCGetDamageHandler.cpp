//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGetDamageHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCGetDamage.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCGetDamageHandler::execute ( GCGetDamage * pGCGetDamage , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// message
	WriteCombatCrashMarker("GCGetDamage object=%d damage=%d",
		pGCGetDamage->getObjectID(), pGCGetDamage->getDamage());

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (pGCGetDamage->getObjectID()==g_pPlayer->GetID())
	{
		g_pPlayer->PacketSpecialActionResult( 
					SKILL_ATTACK_MELEE + (*g_pActionInfoTable).GetMinResultActionInfo(),
					g_pPlayer->GetID(),
					g_pPlayer->GetX(),
					g_pPlayer->GetY()
			);
	}
	else
	{
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
			MCreature* pCreature = g_pZone->GetCreature( pGCGetDamage->getObjectID() );

			
			if (pCreature != NULL)
			{
				
				pCreature->PacketSpecialActionResult( 
								SKILL_ATTACK_MELEE + (*g_pActionInfoTable).GetMinResultActionInfo(),
								pCreature->GetID(),
								pCreature->GetX(),
								pCreature->GetY()
				);
				
			}
		}
	}

	__END_CATCH
}
