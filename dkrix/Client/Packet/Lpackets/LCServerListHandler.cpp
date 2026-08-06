//----------------------------------------------------------------------
//
// Filename    : LCServerListHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCServerList.h"
#include "ServerInformation.h"
#include "ClientDef.h"
#include "UIFunction.h"

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"	
#endif

static void TraceLoginFlowPacket(const char* step)
{
	(void)step;
}

//----------------------------------------------------------------------



//----------------------------------------------------------------------
void LCServerListHandler::execute ( LCServerList * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	TraceLoginFlowPacket("LCServerListHandler begin");


	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (g_pServerInformation==NULL)
	{
		DEBUG_ADD("[Error] g_pServerInformation is NULL");

		return;
	}

	int currentID = pPacket->getCurrentServerGroupID();
	
	bool bExistDefault = false;

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	int groupID = g_pServerInformation->GetServerGroupID();
	ServerGroup* pServerGroup = g_pServerInformation->GetData( groupID );

	if (pServerGroup!=NULL)
	{
		int groupNum = pPacket->getListNum();
		int firstID = 0;

		for (int i=0; i<groupNum; i++)
		{
			ServerGroupInfo* pServerInfo = pPacket->popFrontListElement();

			if (pServerInfo!=NULL)
			{
				if (i==0)
				{
					firstID = pServerInfo->getGroupID();
				}

				if (pServerInfo->getGroupID()==currentID)
				{
					bExistDefault = true;
				}

				//--------------------------------------------------------------
				
				//--------------------------------------------------------------
				SERVER_INFO* pNewServer = pServerGroup->GetData( pServerInfo->getGroupID() );
				
				if (pNewServer==NULL)
				{
					pNewServer = new SERVER_INFO;
					pServerGroup->AddData( pServerInfo->getGroupID(), pNewServer );
				}

				
				pNewServer->ServerName = pServerInfo->getGroupName().c_str();
				pNewServer->ServerStatus = (int)pServerInfo->getStat();
			
				delete pServerInfo;
			}
			else
			{
				DEBUG_ADD( "[Error] ServerGroupInfo is NULL" );
			}	
		}

		
		if (currentID==0 || !bExistDefault)
		{
			g_pServerInformation->SetServerID( firstID );
		}
		else
		{
			g_pServerInformation->SetServerID( currentID );
		}

		UI_SetServerList();

		SetMode( MODE_WAIT_SELECT_SERVER );
		TraceLoginFlowPacket("LCServerListHandler set MODE_WAIT_SELECT_SERVER");
	}
	else
	{
		DEBUG_ADD_FORMAT("[Error] ServerGroup(%d) is NULL", groupID);
	}

#endif
		
	__END_CATCH
}
