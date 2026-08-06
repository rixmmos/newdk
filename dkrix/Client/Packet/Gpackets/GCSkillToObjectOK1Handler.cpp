//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToObjectOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToObjectOK1.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "PacketFunction2.h"


#include "SkillDef.h"	
#include "MFakeCreature.h"
void	SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID);
extern void Add_RocketRuncher(MCreature* UserCreature, MCreature* TargetCreature);
extern void Add_Wild_Wolf(MCreature* UserCreature, MCreature* TargetCreature, bool IsEatCorpse);
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK1Handler::execute ( GCSkillToObjectOK1 * pPacket , Player * pPlayer )
	 
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

	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);
  
	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

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
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getTargetObjectID() );

		// [ TEST CODE ]
		
		
		// g_pPlayer->>AddActionResult( ... )

		// 2005, 1, 3, sobeit add start
		if(pPacket->getSkillType() == SKILL_WILD_WOLF)
		{
			if(pCreature != NULL)
				Add_Wild_Wolf(g_pPlayer, pCreature, pPacket->getGrade()?true:false);
			else
			{
				MItem* pItem = g_pZone->GetItem(pPacket->getTargetObjectID());
				if(pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_CORPSE)
				{
					pCreature = ((MCorpse*)pItem)->GetCreature();
					if(pCreature != NULL)
					{
						Add_Wild_Wolf(g_pPlayer, pCreature, pPacket->getGrade()?true:false);
					}
				}
			}
		}
		// 2005, 1, 3, sobeit add end
		
		if (pCreature != NULL)
		{			
			int skillID = pPacket->getSkillType();
			
			if( g_pActionInfoTable->GetSize() <= skillID )
			{
				DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
				
				SendBugReport("[Error:GCSTOOK1H] Exceed SkillType %d", skillID );
				return;
			}

			// 2004, 11, 13, sobeit add start 
			if(skillID == SKILL_PLASMA_ROCKET_LAUNCHER)
			{
				Add_RocketRuncher(g_pPlayer, pCreature);
			}
			// 2004, 11, 13, sobeit add end
//			// 2005, 1, 3, sobeit add start
			else if(skillID == SKILL_HEAL_PASS)
			{
				ExecuteActionInfoFromMainNode(SKILL_HEAL_PASS,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),pCreature->GetID()	,	
					pCreature->GetX(), pCreature->GetY(), 0, 20 , NULL, false);
				
			}
//			// 2005, 1, 3, sobeit add end
			
			if ((*g_pActionInfoTable)[skillID].IsAffectCurrentWeaponAction())
			{
				skillID = g_pPlayer->GetBasicActionInfo();	

				if (skillID >= g_pActionInfoTable->GetMinResultActionInfo())
				{
					DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", skillID);
					return;
				}
			}

			if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
				skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);
	
				
			
			switch( skillID )
			{
			case SKILL_SHADOW_DANCING :
				SkillShadowDancing(g_pPlayer, pCreature, SKILL_SINGLE_BLOW);
				break;
			// add by Coffee 2007-4-15
			case SKILL_ILLUSION_INVERSION:
				SkillIllendue( g_pPlayer, pCreature, TEMP_SKILL_ILLUSION_INVERSION);
				break;
			// end by Coffee
			case SKILL_ILLENDUE :
				
				//SkillIllendue( g_pPlayer, pCreature, SKILL_LIGHT_BALL);
				SkillIllendue( g_pPlayer, pCreature, MAGIC_CAUSE_SERIOUS_WOUNDS);
				// 2004, 12, 15, sobeit modify end
				break;
				
			case SKILL_BLAZE_WALK :
				SkillBlazeWalk( dynamic_cast<MCreature*>(g_pPlayer), pCreature, SKILL_BLAZE_WALK_ATTACK , pPacket->getGrade() );
				break;

			case SKILL_BITE_OF_DEATH :
				pCreature->SetDrainCreatureID( g_pPlayer->GetID() );
				break;
			case SKILL_SET_AFIRE:
				g_pPlayer->AddEffectStatus(EFFECTSTATUS_SET_AFIRE_ATTACK, 20);
				break;
			}
			
			MActionResult* pResult = new MActionResult;

			
			//pCreature->SetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY());

			int delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

			
			//pCreature->PacketSpecialActionResult( pPacket->getSkillType() + g_ActionInfoTable.GetMinResultActionInfo());
			pResult->Add( new MActionResultNodeActionInfo( 
											skillID,
											g_pPlayer->GetID(), 
											pCreature->GetID(), 
											pCreature->GetX(),
											pCreature->GetY(),
											delayFrame ) 
							);	

			//------------------------------------------------------
			//
			
			//
			//------------------------------------------------------
			MActionResultNode* pActionResultNode = CreateActionResultNode(pCreature, skillID, pPacket->getGrade() );

			//------------------------------------------------------
			
			//------------------------------------------------------
			if (pActionResultNode!=NULL)
			{
				pResult->Add( pActionResultNode );
			}


			//------------------------------------------------------
			
			//------------------------------------------------------
			EFFECTSTATUS es = (*g_pActionInfoTable)[skillID].GetEffectStatus();
			
			if (es!=EFFECTSTATUS_NULL)
			{
				pResult->Add( new MActionResultNodeAddEffectStatus( pCreature->GetID(), 
																			es, 
																			delayFrame ) );
			}
			
			// [ TEST CODE ] 
			
			/*
			MActionResult* pResult = new MActionResult;
			pResult->Add( new MActionResultNodeActionInfo( g_pPlayer->GetID(), g_pPlayer->GetID(), SKILL_ATTACK_MELEE ) );			

			g_pPlayer->PacketAddActionResult(pPacket->getCEffectID(), pResult);
			*/

			
			g_pPlayer->PacketAddActionResult( pPacket->getCEffectID(), pResult );		
		}
		
	}
	

#endif

	__END_CATCH
}
