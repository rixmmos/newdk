//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMineExplosionOK2Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMineExplosionOK2.h"
#include "ClientDef.h"
#include "SkillDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCMineExplosionOK2Handler::execute ( GCMineExplosionOK2 * pPacket , Player * pPlayer )
	 
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
		int skillID = GetMineActionInfo( pPacket->getItemType() );

		if (skillID >= 0)
		{
			//int resultActionInfo = skillID + (*g_pActionInfoTable).GetMinResultActionInfo();

			//------------------------------------------------------
			
			//------------------------------------------------------
			MActionResult* pResult = new MActionResult;
		
			DWORD delayFrame = 16;//ConvertDurationToFrame( pPacket->getDuration() );

			int size = pPacket->getCListNum();

			for (int i=0; i<size; i++)
			{
				//pResult->Add( new MActionResultNodeActionInfo( 
				
				//					pPacket->popCListElement(), 
				//					skillType,
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

					 
					pActionResultNode =  new MActionResultNodeActionInfo( 
																skillID, //resultActionInfo,
																targetID,
																targetID, 
																pTargetCreature->GetX(),
																pTargetCreature->GetY(),
																delayFrame);

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
			int direction = pPacket->getDir();

			SkillToSector(skillID, 
							pPacket->getX(),
							pPacket->getY(),
							direction, 
							delayFrame,
							pResult);
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] Mine Type is Wrong. type = %d", pPacket->getItemType());
		}
	}

#endif

	__END_CATCH
}
