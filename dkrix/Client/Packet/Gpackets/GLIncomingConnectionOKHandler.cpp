//--------------------------------------------------------------------------------
//
// Filename    : GLIncomingConnectionOKHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GLIncomingConnectionOK.h"

#ifdef __LOGIN_SERVER__

	#include "LoginPlayer.h"
	#include "LoginPlayerManager.h"
#include "Packet/Assert.h"

#include "Lpackets/LCReconnect.h"

#endif

//--------------------------------------------------------------------------------
// 
// GLIncomingConnectionOKHander::execute()
// 



// 
//--------------------------------------------------------------------------------
void GLIncomingConnectionOKHandler::execute ( GLIncomingConnectionOK * pPacket )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __LOGIN_SERVER__

	try {	

		
		LoginPlayer * pLoginPlayer = g_pLoginPlayerManager->getPlayer( pPacket->getPlayerID() );

		//cout << "PlayerStatus : " << (int)pLoginPlayer->getPlayerStatus() << endl;

		Assert( pLoginPlayer->getPlayerStatus() == LPS_AFTER_SENDING_LG_INCOMING_CONNECTION );

		cout << "+--------------------------------+" << endl
			 << "| Please Reconnect to GameServer |" << endl
			 << "+--------------------------------+" << endl;

        
        LCReconnect lcReconnect;
        lcReconnect.setGameServerIP( pPacket->getHost() );
        lcReconnect.setGameServerPort( pPacket->getTCPPort() );
        lcReconnect.setKey( pPacket->getKey() );

		
        pLoginPlayer->sendPacket( &lcReconnect );

		
		pLoginPlayer->disconnect( UNDISCONNECTED );

		
		g_pLoginPlayerManager->deletePlayer( pLoginPlayer->getSocket()->getSOCKET() );

		
		delete pLoginPlayer;

	} catch ( NoSuchElementException & nsee ) {
		cout << "Player not exist or already disconnected" << endl;
	}

#endif
		
	__END_CATCH
}
