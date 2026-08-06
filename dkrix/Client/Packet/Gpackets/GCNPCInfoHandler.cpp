//----------------------------------------------------------------------
//
// Filename    : GCNPCInfoHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCNPCInfo.h"

#ifdef __GAME_CLIENT__

	#include "../ClientPlayer.h"
	#include "../PCSlayerInfo2.h"
	#include "../PCVampireInfo2.h"
	#include "../Cpackets/CGReady.h"
	#include "ClientDef.h"
	#include "UIFunction.h"

#endif

//----------------------------------------------------------------------



//----------------------------------------------------------------------
void GCNPCInfoHandler::execute ( GCNPCInfo * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	
	
	
	NPCInfo* pInfo = pPacket->popNPCInfo();

	if(pInfo == NULL)
	{
		UI_SyncLoadedGameState();
		return;
	}

	UI_ClearNPC();

	while(pInfo != NULL)
	{
		const char* pName = pInfo->getName().c_str();
		int npcID = pInfo->getNPCID();
		int x = pInfo->getX();
		int y = pInfo->getY();

		UI_SetNPCInfo( pName, npcID, x, y );

		delete pInfo;
		pInfo = pPacket->popNPCInfo();
	}

	UI_SyncLoadedGameState();

#endif

	__END_CATCH
}
