//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToObjectOK6Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToObjectOK6.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK6Handler::execute ( GCSkillToObjectOK6 * pPacket , Player * pPlayer )
	 
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
		int skillID = pPacket->getSkillType();

		if( g_pActionInfoTable->GetSize() <= skillID )
		{
			DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
			SendBugReport("[ErrorGCSTOOK6H] Exceed SkillType %d", skillID );
			return;
		}


		if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
			skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);
	
		//------------------------------------------------------
		
		//------------------------------------------------------
		
		// [ TEST CODE ]
		MActionResult* pResult = new MActionResult;

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

		pResult->Add( new MActionResultNodeActionInfo( 
								skillID, 
								OBJECTID_NULL,//pCreature->GetID(), 
								g_pPlayer->GetID(), 
								g_pPlayer->GetX(),
								g_pPlayer->GetY(),
								delayFrame ) );
		



		// [ TEST CODE ]
		//
		
		//
		
		//pCreature->SetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY());

		//Duration_t	m_Duration;
		//pCreature->PacketSpecialActionToOther(
		//					pPacket->getSkillType(), 
		//					pPacket->getObjectID(),
		
		//);
		int direction = GetDirectionToPosition(
							pPacket->getX(), pPacket->getY(),
							g_pPlayer->GetX(), g_pPlayer->GetY());

		ExecuteActionInfoFromMainNode(
					pPacket->getSkillType() + (*g_pActionInfoTable).GetMinResultActionInfo(),										
				
					pPacket->getX(), pPacket->getY(), 0, 
					direction,														
					
					g_pPlayer->GetID(),												
					g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
					
					delayFrame,													
					
					pResult,										

					false);			
		
	}
		
	 



	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

#endif

	__END_CATCH
}
