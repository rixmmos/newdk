//--------------------------------------------------------------------------------
//
// Filename    : GLIncomingConnectionErrorHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GLIncomingConnectionError.h"

#ifdef __LOGIN_SERVER__
	#include "LoginPlayer.h"
	#include "LoginPlayerManager.h"
#include "Packet/Assert.h"
#endif

//--------------------------------------------------------------------------------
// 
// GLIncomingConnectionErrorHander::execute()
// 
//--------------------------------------------------------------------------------
void GLIncomingConnectionErrorHandler::execute ( GLIncomingConnectionError * pPacket )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __LOGIN_SERVER__

	
	//
	// *CAUTION*
	//
	
	
	
	
	//
	
	
	//
	try {
		LoginPlayer * pLoginPlayer = g_pLoginPlayerManager->getPlayer( pPacket->getPlayerID() );
	
		Assert( pLoginPlayer->getPlayerStatus() == LPS_AFTER_SENDING_LG_INCOMING_CONNECTION );

		
		cout << "Fail to join game server...(" << pPacket->getPlayerID() << ")" << endl;

		
		pLoginPlayer->disconnect(UNDISCONNECTED);

		
		g_pLoginPlayerManager->deletePlayer( pLoginPlayer->getSocket()->getSOCKET() );

		
		delete pLoginPlayer;

	} catch ( NoSuchElementException & nsee ) {
		cout << "Player not exist or already disconnected." << endl;
	}

#endif
		
	__END_CATCH
}
