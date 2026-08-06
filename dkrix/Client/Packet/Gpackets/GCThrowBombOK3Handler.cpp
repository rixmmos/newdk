//////////////////////////////////////////////////////////////////////
//
// Filename    : GCThrowBombOK3Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCThrowBombOK3.h"
#include "ClientDef.h"
#include "PacketFunction2.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCThrowBombOK3Handler::execute ( GCThrowBombOK3 * pPacket , Player * pPlayer )
	 
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
			int skillID = GetBombActionInfo( pPacket->getItemType() );//pPacket->getSkillType();	

			if (skillID < 0)
			{
				DEBUG_ADD_FORMAT("[Error] no match skillType. itemType=%d", pPacket->getItemType());
				return;
			}

			int useSkillID = skillID;

			if ((*g_pActionInfoTable)[skillID].IsAffectCurrentWeaponAction())
			{
				useSkillID = pCreature->GetBasicActionInfo();

				if (useSkillID >= g_pActionInfoTable->GetMinResultActionInfo())
				{
					DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", useSkillID);
					return;
				}
			}

			
			MActionResult* pResult = new MActionResult;
		
			
			DWORD delayFrame = 16;//ConvertDurationToFrame( pPacket->getDuration() );

			int targetID = pCreature->GetID();
			if ((*g_pActionInfoTable)[skillID].GetActionResultID()==ACTIONRESULTNODE_ACTIONINFO)
			{
				targetID = OBJECTID_NULL;
			}

			pResult->Add( new MActionResultNodeActionInfo( 
											skillID,
											pCreature->GetID(), 
											targetID,	//pCreature->GetID(), 
											pPacket->getX(),
											pPacket->getY(),
											delayFrame ) 
							);


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
			pCreature->SetDirection( pPacket->getDir() );

			//------------------------------------------------------
			
			//------------------------------------------------------
			//Duration_t	m_Duration;
			pCreature->PacketSpecialActionToSector(
								skillID, 
								pPacket->getX(), pPacket->getY(),
								pResult		
			);		

		
		}
	}


#endif

	__END_CATCH
}
