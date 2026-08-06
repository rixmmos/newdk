//----------------------------------------------------------------------
//
// Filename    : LGIncomingConnectionHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LGIncomingConnection.h"
#include "Packet/Properties.h"

#ifdef __GAME_SERVER__

	#include "ConnectionInfo.h"
	#include "ConnectionInfoManager.h"
	#include "LoginServerManager.h"

	#include "Gpackets/GLIncomingConnectionError.h"
	#include "Gpackets/GLIncomingConnectionOK.h"

#endif

//----------------------------------------------------------------------
// 
// LGIncomingConnectionHander::execute()
// 


// 
//----------------------------------------------------------------------
void LGIncomingConnectionHandler::execute ( LGIncomingConnection * pPacket )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	//--------------------------------------------------------------------------------
	//
	
	//
	// *NOTE*
	//
	
	
	
	
	
	
	
	// 
	
	//
	// *TODO*
	//
	
	
	
	//
	
	//
	//--------------------------------------------------------------------------------

	DWORD authKey = rand() << ( time(0) % 10 ) + rand() >> ( time(0)% 10 );

	
	ConnectionInfo * pConnectionInfo = new ConnectionInfo();
	pConnectionInfo->setClientIP( pPacket->getClientIP() );
	pConnectionInfo->setKey( authKey );
	pConnectionInfo->setPCName( pPacket->getPCName() );

	//--------------------------------------------------------------------------------
	//
	
	//
	// *TODO*
	//
	
	//
	//--------------------------------------------------------------------------------
	Timeval currentTime;
	getCurrentTime(currentTime);	
	currentTime.tv_sec += 5;
	pConnectionInfo->setExpireTime( currentTime );

	// debug message
	cout << "+--------------------------------+" << endl
		 << "| Incoming Connection Infomation |" << endl
		 << "+--------------------------------+" << endl
		 << "ClientIP : " << pPacket->getClientIP() << endl
		 << "Auth Key : " << authKey << endl
		 << "P C Name : " << pPacket->getPCName() << endl;

	try {

		
		g_pConnectionInfoManager->addConnectionInfo( pConnectionInfo );

		
		GLIncomingConnectionOK glIncomingConnectionOK;
		glIncomingConnectionOK.setPlayerID( pPacket->getPlayerID() );
		
		if( g_pUserInformation->bKorean )
			glIncomingConnectionOK.setTCPPort( g_pConfigKorean->getPropertyInt("TCPPort") );
		else
			glIncomingConnectionOK.setTCPPort( g_pConfigForeign->getPropertyInt( g_Dimension, "TCPPort") );
		glIncomingConnectionOK.setKey( authKey );

		g_pLoginServerManager->sendPacket( pPacket->getHost() , pPacket->getPort() , &glIncomingConnectionOK );

	} catch ( DuplicatedException & de ) {

		
		delete pConnectionInfo;

		GLIncomingConnectionError glIncomingConnectionError;
		glIncomingConnectionError.setMessage( de.toString() );
		glIncomingConnectionError.setPlayerID( pPacket->getPlayerID() );

		g_pLoginServerManager->sendPacket( pPacket->getHost() , pPacket->getPort() , &glIncomingConnectionError );
	}
	
#endif
		
	__END_CATCH
}
