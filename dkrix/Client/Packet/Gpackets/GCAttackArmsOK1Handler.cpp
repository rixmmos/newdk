//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackArmsOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAttackArmsOK1.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "MSlayerGear.h"
#include "SkillDef.h"
#include "PacketFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAttackArmsOK1Handler::execute ( GCAttackArmsOK1 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	
	// message
	WriteCombatCrashMarker("GCAttackArmsOK1 target=%d skill=%d success=%d bullet=%d short=%d long=%d",
		pPacket->getObjectID(), pPacket->getSkillType(), pPacket->getSkillSuccess(),
		pPacket->getBullet(), pPacket->getShortCount(), pPacket->getLongCount());

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

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
	}
	//------------------------------------------------------
	
	//------------------------------------------------------
	else
	{
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		
		 

		//------------------------------------------------------
		
		//------------------------------------------------------
		if (pPacket->getSkillSuccess())
		{
			
			if (pCreature!=NULL)
			{
				MActionResult* pResult = new MActionResult;

				if( pPacket->getSkillType() == SKILL_JABBING_VEIN || pPacket->getSkillType() == SKILL_MOLE_SHOT ||
					pPacket->getSkillType() == SKILL_TRIDENT || pPacket->getSkillType() == SKILL_QUICK_FIRE || 
					pPacket->getSkillType() == SKILL_ULTIMATE_BLOW  || pPacket->getSkillType() == SKILL_HARPOON_BOMB)
				{
					pResult->Add( new MActionResultNodeActionInfo( 
												pPacket->getSkillType(),
												g_pPlayer->GetID(), 
												pCreature->GetID(), 
												pCreature->GetX(),
												pCreature->GetY()
												 ) 
								);
				} else
				{			
					pResult->Add( new MActionResultNodeActionInfo( 
												g_pPlayer->GetBasicActionInfo(),
												g_pPlayer->GetID(), 
												pCreature->GetID(), 
												pCreature->GetX(),
												pCreature->GetY()
												 ) 
								);
				}

				//g_pPlayer->PacketAddActionResult( effectID, pResult );
				g_pPlayer->PacketAddActionResult( 0, pResult );
			}
		}
					
		//------------------------------------------------------
		
		//------------------------------------------------------
		if (g_pCurrentMagazine==NULL)
		{
			DEBUG_ADD("[Error] Current Magazine Not Exist!");			
		}
		else
		{
			g_pCurrentMagazine->SetNumber( pPacket->getBullet() );
		}

	}	
		

#endif

	__END_CATCH
}
