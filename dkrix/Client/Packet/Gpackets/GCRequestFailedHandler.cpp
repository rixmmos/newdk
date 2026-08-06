//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRequestFailedHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRequestFailed.h"
#include "RequestUserManager.h"
#include "ClientDef.h"
#include "MGameStringTable.h"
#include "WhisperManager.h"
#include "UIFunction.h"

void GCRequestFailedHandler::execute ( GCRequestFailed * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	if (g_Mode==MODE_GAME
		&& g_pGameStringTable!=NULL
		&& g_pWhisperManager!=NULL
		&& g_pRequestUserManager!=NULL)
	{
		switch (pPacket->getCode())
		{
			//----------------------------------------------------------------
			// REQUEST_FAILED_IP
			//----------------------------------------------------------------
			case REQUEST_FAILED_IP :
			{
				switch (g_pRequestUserManager->GetRequestingFor(pPacket->getName().c_str()))
				{
					//--------------------------------------------------------
					// REQUESTING_FOR_WHISPER
					//--------------------------------------------------------
					
					//--------------------------------------------------------
					case RequestUserManager::REQUESTING_FOR_WHISPER :
						
						UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_WHISPER_FAILED].GetString(), NULL, 5, RGB(255, 255, 255) );

						
						g_pWhisperManager->RemoveWhisperMessage( pPacket->getName().c_str() );
					break;
				}
			}
			break;
		}

		
		g_pRequestUserManager->RemoveRequestingUser( pPacket->getName().c_str() );
	}

#endif

	__END_CATCH
}
