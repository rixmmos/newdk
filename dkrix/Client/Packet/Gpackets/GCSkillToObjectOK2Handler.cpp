//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToObjectOK2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToObjectOK2.h"
#include "ClientDef.h"
#include "MSkillManager.h"
#include "PacketFunction2.h"


#include "SkillDef.h"	
void	SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID);
extern void Add_RocketRuncher(MCreature* UserCreature, MCreature* TargetCreature);
extern void Add_Wild_Wolf(MCreature* UserCreature, MCreature* TargetCreature, bool IsEatCorpse);
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK2Handler::execute ( GCSkillToObjectOK2 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);


	int skillID = pPacket->getSkillType();
	
	if( g_pActionInfoTable->GetSize() <= skillID )
	{
		DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
		SendBugReport("[Error:GCSTOOK2H] Exceed SkillType %d", skillID );
		return;
	}


	if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0 )
		skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);

	int delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

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


		
		// [ TEST CODE ]
		MActionResult* pResult = new MActionResult;

		// 2005, 1, 3, sobeit add start
		if(pPacket->getSkillType() == SKILL_WILD_WOLF)
		{
			if(pCreature != NULL)
				Add_Wild_Wolf(pCreature, g_pPlayer, pPacket->getGrade()?true:false);
			else
			{
				MItem* pItem = g_pZone->GetItem(pPacket->getObjectID());
				if(pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_CORPSE)
				{
					pCreature = ((MCorpse*)pItem)->GetCreature();
					if(pCreature != NULL)
					{
						Add_Wild_Wolf(pCreature, g_pPlayer, pPacket->getGrade()?true:false);
					}
				}
			}
		}
		// 2005, 1, 3, sobeit add end
		//------------------------------------------------------------------
		
		//------------------------------------------------------------------
		if (g_pPlayer->GetEFFECT_STAT()!=EFFECTSTATUS_NULL)
		{
			//int esDelayFrame = ConvertDurationToFrame( g_pPlayer->GetDURATION() );
			pResult->Add( new MActionResultNodeAddEffectStatus( 
										g_pPlayer->GetID(), 
										(EFFECTSTATUS)g_pPlayer->GetEFFECT_STAT(), 
										delayFrame ));

			g_pPlayer->SetStatus( MODIFY_EFFECT_STAT, EFFECTSTATUS_NULL );
		}
		else
		{
			//------------------------------------------------------
			
			//------------------------------------------------------
			EFFECTSTATUS es = (*g_pActionInfoTable)[skillID].GetEffectStatus();
			
			if (es!=EFFECTSTATUS_NULL)
			{
				pResult->Add( new MActionResultNodeAddEffectStatus( g_pPlayer->GetID(),
																		es, 
																		delayFrame ) );
			}
		}

		//------------------------------------------------------
		
		
		
		
		//------------------------------------------------------
		if (skillID==SKILL_TYPHOON || skillID == SKILL_WILD_TYPHOON)
		{
			if( pPacket->getGrade() == 1 || skillID == SKILL_WILD_TYPHOON )
				g_pPlayer->SetDelay( 1000 );

		}
		// 2004, 11, 13, sobeit add start 
		if(skillID == SKILL_PLASMA_ROCKET_LAUNCHER)
		{
			Add_RocketRuncher(pCreature, g_pPlayer);
		}
		// 2004, 11, 13, sobeit add end
	
		
		if ((*g_pActionInfoTable)[skillID].IsAffectCurrentWeaponAction()
			&& pCreature!=NULL)
		{
			skillID = pCreature->GetBasicActionInfo();

			if (skillID >= g_pActionInfoTable->GetMinResultActionInfo())
			{
				DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", skillID);
				return;
			}
		}

		pResult->Add( new MActionResultNodeActionInfo( 
								skillID, 
								pPacket->getObjectID(), 
								g_pPlayer->GetID(), 
								g_pPlayer->GetX(),
								g_pPlayer->GetY(),
								delayFrame ) );	

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		MActionResultNode* pActionResultNode = CreateActionResultNode(g_pPlayer, skillID, pPacket->getGrade() );

		//------------------------------------------------------
		
		//------------------------------------------------------
		if (pActionResultNode!=NULL)
		{
			pResult->Add( pActionResultNode );
		}

		//------------------------------------------------------
		
		//------------------------------------------------------
		if (pCreature == NULL)
		{
			ExecuteActionInfoFromMainNode(
						skillID + (*g_pActionInfoTable).GetMinResultActionInfo(),										
					
						g_pPlayer->GetX(), g_pPlayer->GetY(), g_pPlayer->GetZ(),
						g_pPlayer->GetDirection(),														
						
						g_pPlayer->GetID(),												
						g_pPlayer->GetX(), g_pPlayer->GetY(), g_pPlayer->GetZ(),
						
						delayFrame,													
						
						pResult,
						
						false);			
		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else
		{
			if(skillID == SKILL_ABSORB_SOUL)
				pCreature->ClearStopBloodDrain();

			
			

			switch( skillID )
			{
			case SKILL_SHADOW_DANCING :
				SkillShadowDancing(pCreature, g_pPlayer, SKILL_SINGLE_BLOW);
				break;
			// add by Coffee 2007-4-15
			case SKILL_ILLUSION_INVERSION:
				SkillIllendue( pCreature, g_pPlayer, TEMP_SKILL_ILLUSION_INVERSION);
				break;
			// end by Coffee	
			case SKILL_ILLENDUE :
				
				//SkillIllendue( pCreature, g_pPlayer, SKILL_LIGHT_BALL );
				SkillIllendue( pCreature, g_pPlayer, MAGIC_CAUSE_SERIOUS_WOUNDS );
				// 2004, 12, 15, sobeit modify end
				break;
				
			case SKILL_BLAZE_WALK :
				SkillBlazeWalk( pCreature , dynamic_cast<MCreature*>(g_pPlayer), SKILL_BLAZE_WALK_ATTACK, pPacket->getGrade()  );
				break;

			case SKILL_BITE_OF_DEATH :
				g_pPlayer->SetDrainCreatureID( pPacket->getObjectID() );
				break;
			case SKILL_SET_AFIRE:
				pCreature->AddEffectStatus(EFFECTSTATUS_SET_AFIRE_ATTACK, 20);
				break;
			}
			
			//------------------------------------------------------
			
			//------------------------------------------------------
			if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_PROTECTION_FROM_ACID))
			{
				//------------------------------------------------------
				// PROTECTION_FROM_ACID
				//------------------------------------------------------
				if ((*g_pSkillInfoTable)[skillID].GetSkillStep()==SKILL_STEP_VAMPIRE_ACID)
				{
					skillID = PROTECTION_FROM_ACID;					
				}				
			}			


			// [ TEST CODE ]
			//
			
			//
			
			pCreature->SetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY());

			//Duration_t	m_Duration;
			pCreature->PacketSpecialActionToOther(
								skillID, 
								g_pPlayer->GetID(),//pPacket->getObjectID(),
								pResult						
			);
		}
	}		
	
	 


	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

#endif

	__END_CATCH
}
