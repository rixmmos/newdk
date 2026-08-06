//----------------------------------------------------------------------
//
// Filename    : LCWorldListHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCWorldList.h"
#include "ClientDef.h"
#include "ServerInformation.h"
#include "DebugInfo.h"
#include "UIFunction.h"

static void TraceLoginFlowPacket(const char* step)
{
	(void)step;
}

//----------------------------------------------------------------------



//----------------------------------------------------------------------
void LCWorldListHandler::execute ( LCWorldList * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	TraceLoginFlowPacket("LCWorldListHandler begin");

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (g_pServerInformation==NULL)
	{
		g_pServerInformation = new CServerInformation;
	}
	else
	{
		g_pServerInformation->Release();
	}

	int currentID = pPacket->getCurrentWorldID();
	
	bool bExistDefault = false;

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	int groupNum = pPacket->getListNum();
	int firstID = 0;

	for (int i=0; i<groupNum; i++)
	{
		WorldInfo* pWorldInfo = pPacket->popFrontListElement();

		if (pWorldInfo!=NULL)
		{
			if (i==0)
			{
				firstID = pWorldInfo->getID();
			}

			if (pWorldInfo->getID()==currentID)
			{
				bExistDefault = true;
			}

			//--------------------------------------------------------------
			
			//--------------------------------------------------------------
			ServerGroup* pNewGroup = g_pServerInformation->GetData( pWorldInfo->getID() );
			
			if (pNewGroup==NULL)
			{
				pNewGroup = new ServerGroup;
				g_pServerInformation->AddData( pWorldInfo->getID(), pNewGroup );
			}

			
			pNewGroup->SetGroupName( pWorldInfo->getName().c_str() );
			pNewGroup->SetGroupStatus( (int)pWorldInfo->getStat() );
			
			//--------------------------------------------------------------
			
			//--------------------------------------------------------------
			 

			delete pWorldInfo;
		}
		else
		{
			DEBUG_ADD( "[Error] ServerGroupInfo is NULL" );
		}	
	}

	
	if (currentID==0 || !bExistDefault)
	{
		g_pServerInformation->SetServerGroupID( firstID );
	}
	else
	{
		g_pServerInformation->SetServerGroupID( currentID );
	}

	UI_SetWorldList();

	SetMode( MODE_WAIT_SELECT_WORLD );
	TraceLoginFlowPacket("LCWorldListHandler set MODE_WAIT_SELECT_WORLD");

#endif
		
	__END_CATCH
}
