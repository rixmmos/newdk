//--------------------------------------------------------------------------------
//
// Filename    : GCPartySayHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCPartySay.h"
#include "../Player.h"
#include "ClientDef.h"
#include "UserInformation.h"
#include "MChatManager.h"
#include "UIFunction.h"
#include "MGameStringTable.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCPartySayHandler::execute ( GCPartySay * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY 
		
#ifdef __GAME_CLIENT__
	if ((g_Mode==MODE_GAME
			|| g_Mode==MODE_WAIT_UPDATEINFO			
			|| g_Mode==MODE_WAIT_SETPOSITION		
			)
		&& g_pUserInformation!=NULL
		&& g_pChatManager!=NULL)
	{
		//g_pClientCommunicationManager->sendPacket( pPacket->getHost() , pPacket->getPort() , &glIncomingConnectionOK );
		// 256: a 128-byte message plus NUL does not fit char[128]. Capping the
		// read alone is not enough -- the cap and the buffer must not be equal.
		char str[256];
		char strName[256];
		strcpy(str, pPacket->getMessage().c_str());
		strcpy(strName, pPacket->getName().c_str());

		//bool bMasterWords = (strstr(strName, "GM")!=NULL);
		bool bMasterWords = strncmp( strName, (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetString(), (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetLength() ) == 0;

		if (bMasterWords 
			|| g_pChatManager->IsAcceptID( strName ))
		{
			//--------------------------------------------------
			
			
			
			//--------------------------------------------------
			if (!bMasterWords && !g_pUserInformation->IsMaster 
				&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
				&& !g_bLight
#endif
				)
			{
				g_pChatManager->RemoveCurse( str );

				 
			}

			//sprintf(str, "<%s> %s", pPacket->getName().c_str(), pPacket->getMessage().c_str());
			// party = 3
			UI_AddChatToHistory( str, strName, 3, pPacket->getColor() );

			
			//char strWhisperID[128];
			//sprintf(strWhisperID, "%s ", pPacket->getName().c_str());
			//g_pUserInformation->WhisperID = strWhisperID;

			
//			__BEGIN_HELP_EVENT
////				ExecuteHelpEvent( HE_CHAT_WHISPERED );	
//			__END_HELP_EVENT
		}
	}
		
#endif

	__END_CATCH
}
