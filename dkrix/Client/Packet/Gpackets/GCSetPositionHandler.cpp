//----------------------------------------------------------------------
//
// Filename    : GCSetPositionHandler.cc
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCSetPosition.h"
#include "ClientDef.h"

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"
	#include "UIFunction.h"
#endif

static void TraceGameEntryFlow(const char* step)
{
	(void)step;
}

//----------------------------------------------------------------------
// 
// GCSetPositionHander::execute
//
//----------------------------------------------------------------------
void GCSetPositionHandler::execute ( GCSetPosition * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
	TraceGameEntryFlow("GCSetPositionHandler begin");

	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

	DEBUG_ADD_FORMAT("Set Position To ( %d, %d ) to Dir(%d)", (int)pPacket->getX(), (int)pPacket->getY(), (int)pPacket->getDir());		
	
	pClientPlayer->setXY( pPacket->getX() , pPacket->getY() );

	pClientPlayer->setPlayerStatus( CPS_NORMAL ); 
	TraceGameEntryFlow("GCSetPositionHandler set CPS_NORMAL");


	//--------------------------------------------------------
	
	//--------------------------------------------------------
	InitPlayer(	pPacket->getX(), 
				pPacket->getY(),
				pPacket->getDir());

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	SetMode( MODE_GAME );
	TraceGameEntryFlow("GCSetPositionHandler set MODE_GAME");
	UI_SyncLoadedGameState();
	TraceGameEntryFlow("GCSetPositionHandler after UI_SyncLoadedGameState");

#endif
	
	__END_CATCH
}
