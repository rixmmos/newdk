//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRequestedIPHandler.cc
// Written By  : crazydog
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRequestedIP.h"
//#include "MParty.h"
//#include "RequestFunction.h"

#include "../RequestClientPlayerManager.h"
#include "RequestUserManager.h"
#include "WhisperManager.h"
#include "ClientDef.h"
#include "UserInformation.h"
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCRequestedIPHandler::execute ( GCRequestedIP * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	if( g_pUserInformation->bKorean == false || 1 )
		return;
		
#ifdef __GAME_CLIENT__

	if (g_Mode==MODE_GAME
		&& g_pRequestUserManager!=NULL
		&& g_pRequestClientPlayerManager!=NULL)
	{
		const char* pName = pPacket->getName().c_str();

		struct in_addr sa;
#ifdef PLATFORM_WINDOWS
		sa.S_un.S_addr = pPacket->getIP();
#else
		sa.s_addr = pPacket->getIP();
#endif

		const char* pIP = inet_ntoa( sa );

		
		g_pRequestUserManager->AddRequestUser( pName, pIP, pPacket->getPort() );	


		switch (g_pRequestUserManager->GetRequestingFor(pName))
		{
			//--------------------------------------------------------
			// REQUESTING_FOR_WHISPER
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			case RequestUserManager::REQUESTING_FOR_WHISPER :
				if (g_pWhisperManager->HasWhisperMessage( pName ))
				{
					g_pRequestClientPlayerManager->Connect( pIP, 
															pName, 
															REQUEST_CLIENT_MODE_WHISPER );
				}
			break;

			//--------------------------------------------------------
			// REQUESTING_FOR_PROFILE
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			case RequestUserManager::REQUESTING_FOR_PROFILE :
				g_pRequestClientPlayerManager->Connect( pIP, 
														pName, 
														REQUEST_CLIENT_MODE_PROFILE );
			break;
		}


		
		g_pRequestUserManager->RemoveRequestingUser( pName );	
	}

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	 

#endif

	__END_CATCH
}
