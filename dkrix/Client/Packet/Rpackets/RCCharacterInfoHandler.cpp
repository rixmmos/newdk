//----------------------------------------------------------------------
//
// Filename    : RCCharacterInfoHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "RCCharacterInfo.h"
#include "MParty.h"
#include "RequestUserManager.h"
#include "ClientDef.h"

#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
	extern CMessageArray*		g_pGameMessage;
#endif

//----------------------------------------------------------------------
// 
// RCCharacterInfoHander::execute()
// 
//----------------------------------------------------------------------
void RCCharacterInfoHandler::execute ( RCCharacterInfo * pPacket )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	//g_pClientCommunicationManager->sendPacket( pPacket->getHost() , pPacket->getPort() , &glIncomingConnectionOK );
	
	if ((g_Mode==MODE_GAME
			|| g_Mode==MODE_WAIT_UPDATEINFO			
			|| g_Mode==MODE_WAIT_SETPOSITION		
			)
		&& g_pParty!=NULL
		&& g_pGameMessage!=NULL
		&& g_pRequestUserManager!=NULL)
	{
		
		RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pPacket->getName().c_str() );

		if (pUserInfo!=NULL)
		{
			pUserInfo->IP = pPacket->getHost().c_str();
			pUserInfo->UDPPort = pPacket->getPort();
		}


		PARTY_INFO*	pInfo = NULL;
		
		
		if (pPacket->getName().size()==0)
		{
			pInfo = g_pParty->GetMemberInfoByIP( pPacket->getHost().c_str() );
		}
		else
		{
			pInfo = g_pParty->GetMemberInfo( pPacket->getName().c_str() );
		}
			
		
		if (pInfo!=NULL)
		{
			pInfo->guildID = pPacket->getGuildID();
			
			#if defined(_DEBUG) && defined(OUTPUT_DEBUG)
				if (g_pGameMessage!=NULL)
				{
					g_pGameMessage->AddFormat("[RCCharacterInfo] %d", pInfo->guildID);
				}
			#endif
		}
	}
		
	__END_CATCH
}

