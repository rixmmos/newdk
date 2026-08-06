//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToSelfOK3Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToSelfOK3.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToSelfOK3Handler::execute ( GCSkillToSelfOK3 * pPacket , Player * pPlayer )
	 
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
		//MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );


		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------			

		int skillID = pPacket->getSkillType();
		
		if( g_pActionInfoTable->GetSize() <= skillID )
		{
			DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
			SendBugReport("[Error:GCSTSOK3H] Exceed SkillType %d", skillID );
			return;
		}


		if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
			skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);
		
		DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

		 

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		/*
		MActionResultNode* pActionResultNode = NULL;
		switch ((*g_pActionInfoTable)[skillID].GetActionResultID())
		{
			//------------------------------------------------------
			// Burrow
			//------------------------------------------------------
			case ACTIONRESULTNODE_CREATURE_BURROW :
				pActionResultNode = new MActionResultNodeCreatureBurrow(pCreature->GetID());
			break;
		}
		*/

		//------------------------------------------------------
		
		//------------------------------------------------------
		/*
		if (pActionResultNode!=NULL)
		{
			pResult->Add( pActionResultNode );
		}
		*/

		//Duration_t	m_Duration;
		//pCreature->PacketSpecialActionToSelf( 
		//						pPacket->getSkillType(),
		
		//	);

		//------------------------------------------------------
		
		//------------------------------------------------------
		ExecuteActionInfoFromMainNode(
			skillID + (*g_pActionInfoTable).GetMinResultActionInfo(),
		
			pPacket->getX(), pPacket->getY(), 0, 
			0,														
			
			OBJECTID_NULL,												
			pPacket->getX(), pPacket->getY(), 0, 
			
			delayFrame,													
			
			NULL,

			false);			
	}

#endif

	__END_CATCH
}
