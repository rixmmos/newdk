//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToTileOK5Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToTileOK5.h"
#include "ClientDef.h"
#include "PacketFunction2.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK5Handler::execute ( GCSkillToTileOK5 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

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


		
		if (pCreature != NULL)
		{			
			int skillID = pPacket->getSkillType();

			if( g_pActionInfoTable->GetSize() <= skillID )
			{
				DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
				SendBugReport("[Error:GCSTTOK5H] Exceed SkillType %d", skillID );
				return;
			}
			// 2004, 9, 9, sobeit add start
			if(skillID == SKILL_BURNING_SOUL_LAUNCH)
			{
				int TempskillID = SKILL_CLIENT_BURNING_SOL_1+pPacket->getGrade();
				ExecuteActionInfoFromMainNode(TempskillID,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),pCreature->GetID()	,	
					pPacket->getX(), pPacket->getY(), 0, ConvertDurationToFrame( pPacket->getDuration() ), NULL, false);
			}
			else if(skillID == SKILL_ROTTEN_APPLE)
			{
				ExecuteActionInfoFromMainNode(SKILL_CLIENT_ROTTEN_APPLE,pPacket->getX(), pPacket->getY(), 0,0,0	,	
					pPacket->getX(), pPacket->getY(), 0, ConvertDurationToFrame( pPacket->getDuration() ), NULL, false);
			}
			// 2004, 9, 9, sobeit add end

			int useSkillID = skillID;

			if ((*g_pActionInfoTable)[skillID].IsAffectCurrentWeaponAction()&& skillID != SKILL_PIERCING )
			{
				useSkillID = pCreature->GetBasicActionInfo();

				if (useSkillID >= g_pActionInfoTable->GetMinResultActionInfo())
				{
					DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", useSkillID);
					return;
				}
			}
			
			DEBUG_ADD_FORMAT("GRADE : %d %d",skillID, pPacket->getGrade());
			if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
				useSkillID = skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);

			
			
			MActionResult* pResult = new MActionResult;
		
			DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

			int targetID = pCreature->GetID();
			if ((*g_pActionInfoTable)[skillID].GetActionResultID()==ACTIONRESULTNODE_ACTIONINFO)
			{
				targetID = OBJECTID_NULL;
			}
			

			if((*g_pActionInfoTable)[skillID].IsUseActionGrade() )
			{
				pResult->Add( new MActionResultNodeActionInfo(
					skillID,
					g_pPlayer->GetID(), 
					targetID, 
					pPacket->getX(),
					pPacket->getY(),
					delayFrame, pPacket->getRange() ) 
					);	
			} else
			{
				pResult->Add( new MActionResultNodeActionInfo( 
					skillID,
					pCreature->GetID(), 
					targetID,
					pPacket->getX(),
					pPacket->getY(),
					delayFrame ) 
					);
			}
//
//			pResult->Add( new MActionResultNodeActionInfo( 
//											skillID,
//											pCreature->GetID(), 
//											targetID,	//pCreature->GetID(), 
//											pPacket->getX(),
//											pPacket->getY(),
//											delayFrame ) 
//							);


			int size = pPacket->getCListNum();
	
			for (int i=0; i<size; i++)
			{
				//pResult->Add( new MActionResultNodeActionInfo( 
				//					pCreature->GetID(), 
				//					pPacket->popCListElement(), 
				//					pPacket->getSkillType(),
				//					delayFrame ) );
				//------------------------------------------------------
				//
				
				//
				//------------------------------------------------------
				int targetID = pPacket->popCListElement();

				MCreature* pTargetCreature = g_pZone->GetCreature( targetID );

				if (pTargetCreature!=NULL)
				{
					//------------------------------------------------------
					
					//------------------------------------------------------
					EFFECTSTATUS es = (*g_pActionInfoTable)[skillID].GetEffectStatus();
							
					if (es!=EFFECTSTATUS_NULL)
					{
						pResult->Add( new MActionResultNodeAddEffectStatus( targetID, es, delayFrame ) );
					}

					MActionResultNode* pActionResultNode = NULL;

					switch ((*g_pActionInfoTable)[skillID].GetActionResultID())
					{
						//------------------------------------------------------
						
						//------------------------------------------------------
						case ACTIONRESULTNODE_ACTIONINFO :
							pActionResultNode =  new MActionResultNodeActionInfo( 
																useSkillID,	//(*g_pActionInfoTable)[skillID].GetActionResultValue(),
																pCreature->GetID(), 
																targetID, 
																pTargetCreature->GetX(),
																pTargetCreature->GetY(),
																delayFrame);
						break;

						//------------------------------------------------------
						// Burrow
						//------------------------------------------------------
						default :
							pActionResultNode = CreateActionResultNode(pTargetCreature, useSkillID);
					}

					//------------------------------------------------------
					
					//------------------------------------------------------
					if (pActionResultNode!=NULL)
					{
						pResult->Add( pActionResultNode );
					}
				}
			}

			//------------------------------------------------------
			
			//------------------------------------------------------
			pCreature->SetDirectionToPosition(pPacket->getX(), pPacket->getY());
			
			//------------------------------------------------------
			
			//------------------------------------------------------
			if ((*g_pActionInfoTable)[pPacket->getSkillType()].IsOptionRangeToDirection())
			{
				pCreature->SetDirection( pPacket->getRange() );
			}

			//------------------------------------------------------
			
			//------------------------------------------------------
			//Duration_t	m_Duration;
			pCreature->PacketSpecialActionToSector(
								skillID, 
								pPacket->getX(), pPacket->getY(),
								pResult		
			);		

//			_MinTrace("Incomming GCSkillToTileOK5Handler(SkillType:%d X:%d Y:%d Dir:%d",
//				pPacket->getSkillType(), pPacket->getX(), pPacket->getY(), pCreature->GetDirection() );

		
		}
	}

#endif

	__END_CATCH
}
