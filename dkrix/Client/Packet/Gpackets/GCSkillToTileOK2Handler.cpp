//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToTileOK2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToTileOK2.h"
#include "ClientDef.h"
#include "MEffectStatusDef.h"
#include "PacketFunction2.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK2Handler::execute ( GCSkillToTileOK2 * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

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
		//------------------------------------------------------
		
		
		//------------------------------------------------------
		if (pPacket->getObjectID()==g_pPlayer->GetID())
		{
			DEBUG_ADD("[Error] the User of GCSkillToTileOK2 is Player!");			
		}
		else
		{
			//------------------------------------------------------
			
			//------------------------------------------------------
			int creatureID = pPacket->getObjectID();
			MCreature* pCreature = g_pZone->GetCreature( creatureID );

			int skillID = pPacket->getSkillType();
			
			if( g_pActionInfoTable->GetSize() <= skillID )
			{
				//DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);				
				SendBugReport("[%s,%d] %d,%d",__FILE__, __LINE__, skillID );
				return;
			}
	
			// 2004, 9, 9, sobeit add start
			if(skillID == SKILL_BURNING_SOUL_LAUNCH)
			{
				int TempskillID = SKILL_CLIENT_BURNING_SOL_1+pPacket->getGrade();
				ExecuteActionInfoFromMainNode(TempskillID,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(), pCreature->GetID()	,	
					pPacket->getX(), pPacket->getY(), 0, ConvertDurationToFrame( pPacket->getDuration() ), NULL, false);
			//	return;
			}
			else if(skillID == SKILL_ROTTEN_APPLE)
			{
				ExecuteActionInfoFromMainNode(SKILL_CLIENT_ROTTEN_APPLE,pPacket->getX(), pPacket->getY(), 0,0,0	,	
					pPacket->getX(), pPacket->getY(), 0, ConvertDurationToFrame( pPacket->getDuration() ), NULL, false);
			}
			// 2004, 9, 9, sobeit add end

			DEBUG_ADD_FORMAT("GRADE : %d %d",skillID, pPacket->getGrade());
			if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
				skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);

				int useSkillID = skillID;
		
			DEBUG_ADD("CurWA");

			if ((*g_pActionInfoTable)[skillID].IsAffectCurrentWeaponAction()
				&& pCreature!=NULL&& skillID != SKILL_PIERCING )
			{
				DEBUG_ADD("SetBA");

				useSkillID = pCreature->GetBasicActionInfo();

				if (useSkillID >= g_pActionInfoTable->GetMinResultActionInfo())
				{
					DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", useSkillID);
					return;
				}
			}

			//------------------------------------------------------
			
			//------------------------------------------------------				
			int size = pPacket->getCListNum();

			MActionResult* pResult = new MActionResult;

			//------------------------------------------------------------------
			
			//------------------------------------------------------------------
			if (g_pPlayer->GetEFFECT_STAT()!=EFFECTSTATUS_NULL)
			{
				DEBUG_ADD("AddES");

				pResult->Add( new MActionResultNodeAddEffectStatus( 
											g_pPlayer->GetID(), 
											(EFFECTSTATUS)g_pPlayer->GetEFFECT_STAT(), 
											delayFrame ));

				g_pPlayer->SetStatus( MODIFY_EFFECT_STAT, EFFECTSTATUS_NULL );
			}

			DEBUG_ADD("AResultID?");

			int targetID = creatureID;
			if ((*g_pActionInfoTable)[skillID].GetActionResultID()==ACTIONRESULTNODE_ACTIONINFO)
			{
				targetID = OBJECTID_NULL;
			}

			DEBUG_ADD("AddAI");
			
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
					creatureID, 
					targetID,
					pPacket->getX(),
					pPacket->getY(),
					delayFrame ) 
					);
			}

			
		
			if (size!=0)
			{
				
				for (int i=0; i<size; i++)
				{
					//MCreature* pTargetCreature = g_pZone->GetCreature( pPacket->getCListElement() );
						
					
					//if (pTargetCreature != NULL)
					//{
						
						// [ TEST CODE ]
						
					//	pResult->Add( new MActionResultNodeActionInfo( 
					//								pCreature->GetID(), 
					//								pPacket->popCListElement(), 
					//								pPacket->getSkillType(),
					//								delayFrame ) 
					//				);
					//}
					int targetID = pPacket->popCListElement();

					DEBUG_ADD("getTC");

					MCreature* pTargetCreature = g_pZone->GetCreature( targetID );

					if (pTargetCreature!=NULL)
					{
						DEBUG_ADD("TCNotNULL");

						//------------------------------------------------------
						
						//------------------------------------------------------
						EFFECTSTATUS es = (*g_pActionInfoTable)[skillID].GetEffectStatus();
						
						if (es!=EFFECTSTATUS_NULL)
						{
							pResult->Add( new MActionResultNodeAddEffectStatus( targetID, es, delayFrame ) );
						}

						//------------------------------------------------------
						//
						
						//
						//------------------------------------------------------
						MActionResultNode* pActionResultNode = NULL;

						switch ((*g_pActionInfoTable)[skillID].GetActionResultID())
						{
							//------------------------------------------------------
							
							//------------------------------------------------------
							case ACTIONRESULTNODE_ACTIONINFO :
								
								DEBUG_ADD("ANodeAI");

								pActionResultNode =  new MActionResultNodeActionInfo( 
																	skillID, //(*g_pActionInfoTable)[skillID].GetActionResultValue(),
																	creatureID, 
																	targetID, 
																	pTargetCreature->GetX(),
																	pTargetCreature->GetY(),
																	delayFrame);

								DEBUG_ADD("ANodeAI-ok");
							break;

							//------------------------------------------------------
							
							//------------------------------------------------------
							default :
								DEBUG_ADD("default");
								pActionResultNode = CreateActionResultNode(pTargetCreature, useSkillID);
						}

						//------------------------------------------------------
						
						//------------------------------------------------------
						if (pActionResultNode!=NULL)
						{
							DEBUG_ADD("add");
							pResult->Add( pActionResultNode );
							DEBUG_ADD("ok");
						}
					}
					else
					{
						DEBUG_ADD("TCNull");
					}
				}
			}


			DEBUG_ADD("pCreture?");

			//------------------------------------------------------
			
			//------------------------------------------------------
			if (pCreature==NULL)
			{
				DEBUG_ADD_FORMAT("[Error] The Creature is not Exist. id=%d", pPacket->getObjectID());
				
				int direction = 0;

				//------------------------------------------------------
				
				//------------------------------------------------------
				if ((*g_pActionInfoTable)[skillID].IsOptionRangeToDirection())
				{
					direction = pPacket->getRange();
				}

				ExecuteActionInfoFromMainNode(
							skillID,										
						
							pPacket->getX(), pPacket->getY(), 0,
							direction,														
							
							OBJECTID_NULL,		
							pPacket->getX(), pPacket->getY(), 0,
							
							delayFrame,													
							
							pResult,
							
							false);			
			}
			//------------------------------------------------------
			
			//------------------------------------------------------
			else
			{				
				DEBUG_ADD("C-OK");

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
				DEBUG_ADD("AtoSector");

				pCreature->PacketSpecialActionToSector(
									skillID, 
									pPacket->getX(), pPacket->getY(),
									pResult						
				);

				DEBUG_ADD("AtoSectorOK");
			}
//			_MinTrace("Incomming GCSkillToTileOK2Handler(SkillType:%d X:%d Y:%d Dir:%d",
//				pPacket->getSkillType(), pPacket->getX(), pPacket->getY(), pCreature->GetDirection() );			
		}
	}

	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	//g_pPlayer->SetEffectDelayFrame( resultActionInfo, delayFrame );




	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );
	
	DEBUG_ADD("TileOK2End");

#endif

	__END_CATCH
}
