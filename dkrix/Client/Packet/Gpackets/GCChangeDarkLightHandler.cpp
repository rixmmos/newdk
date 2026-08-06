//--------------------------------------------------------------------------------
//
// Filename    : GCChangeDarkLightHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "MTopView.h"
#include "GCChangeDarkLight.h"
#include "UserInformation.h"

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"
#endif

#include "ClientDef.h"

static void TraceDarkLightFlow(const char* step)
{
	(void)step;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCChangeDarkLightHandler::execute ( GCChangeDarkLight * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	#ifdef __EXPO_CLIENT__
		return;
	#endif

	TraceDarkLightFlow("GCChangeDarkLightHandler skipped for local test");
	return;

	// message


	int darkLevel = pPacket->getDarkLevel();
	int lightSight = pPacket->getLightLevel();

	if (g_pUserInformation->IsMaster)
	{
		darkLevel = 0;
		lightSight = 13;
	}

	
	g_pTopView->SetDarkBits( darkLevel );		
	
	
	g_pPlayer->SetTimeLightSight( lightSight );		

	

#endif

	__END_CATCH
}
