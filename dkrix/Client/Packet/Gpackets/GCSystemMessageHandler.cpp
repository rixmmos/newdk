//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSystemMessageHandler.cc
// Written By  : elca
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSystemMessage.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "MGameStringTable.h"
#include "Client.h"
//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCSystemMessageHandler::execute ( GCSystemMessage * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
    // 256 with bounded copies: the message is uncapped on the wire (BYTE, so up
    // to 255) while these were 128. They are only dedupe keys -- the message
    // itself is passed on separately -- so truncation is harmless.
    static char previous1[256] = {
        NULL,
    };
    switch (pPacket->getType()) {
    case SYSTEM_MESSAGE_HOLY_LAND:
        if (g_pUserOption->DoNotShowHolyLandMsg)
            return;
        break;

    case SYSTEM_MESSAGE_NORMAL:
        break;

    case SYSTEM_MESSAGE_OPERATOR:
        break;

    case SYSTEM_MESSAGE_MASTER_LAIR:
        if (g_pUserOption->DoNotShowLairMsg)
            return;
        break;

    case SYSTEM_MESSAGE_COMBAT:
        if (g_pUserOption->DoNotShowWarMsg)
            return;
        break;

    case SYSTEM_MESSAGE_INFO:
        break;

    case SYSTEM_MESSAGE_RANGER_CHAT: {
        char* message = (char*)pPacket->getMessage().c_str();
        if (NULL != message) {
            UI_SetRangerChatString(message);
        }
    }
        return;

    case SYSTEM_MESSAGE_PLAYER:
        char* message = (char*)pPacket->getMessage().c_str();

        if (NULL != message) {
            message = (char*)pPacket->getMessage().c_str();
            // 				if (strcmp(previous1, message)==0)
            // 				{
            // 					BOOL bExist = FALSE;
            //
            // 					//--------------------------------------------------------------------

            // 					//--------------------------------------------------------------------
            // 					for (int i=0; i<g_pPlayerMessage->GetSize(); i++)
            // 					{
            // 						if (strcmp((*g_pPlayerMessage)[i], message)==0)
            // 						{
            // 							bExist = TRUE;
            // 						}
            // 					}
            //
            // 					//--------------------------------------------------------------------

            // 					//--------------------------------------------------------------------
            // 					if (!bExist)
            // 					{
            // 						g_pPlayerMessage->Add( message );
            // 					}
            // 				}
            // 				//--------------------------------------------------------------------

            // 				//--------------------------------------------------------------------
            // 				else
            // 				{
            g_pPlayerMessage->Add(message);

            strncpy(previous1, pPacket->getMessage().c_str(), sizeof(previous1) - 1);
            previous1[sizeof(previous1) - 1] = 0;
            //				}
        }
        return;
    }

    // 512: this one also receives a prefixed form built with new char[len+20].
    static char previous[512] = {
        NULL,
    };

    // Fix: Store string in local variable to avoid use-after-free
    // The temporary string returned by getMessage() is destroyed at end of statement
    std::string messageStr = pPacket->getMessage();
    const char* message = messageStr.c_str();


    char* pMsg = NULL;
    if (message != NULL && pPacket->getType() != SYSTEM_MESSAGE_PLAYER) {
        pMsg = new char[strlen(message) + 20];
        sprintf(pMsg, (*g_pGameStringTable)[UI_STRING_MESSAGE_SYSTEM].GetString(), message);
        pPacket->setMessage(pMsg);
        SAFE_DELETE_ARRAY(pMsg);
    }
    // Update message pointer after potential setMessage()
    messageStr = pPacket->getMessage();
    message = messageStr.c_str();
    // add end by Coffee 2007-8-2
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	if (strcmp(previous, message)==0)
	{
		BOOL bExist = FALSE;

		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		for (int i=0; i<g_pSystemMessage->GetSize(); i++)
		{
			if (strcmp((*g_pSystemMessage)[i], message)==0)
			{
				bExist = TRUE;
			}
		}

		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		if (!bExist)
		{
			g_pSystemMessage->Add( message );
		}
	}
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	else
	{
		g_pSystemMessage->Add( message );

        strncpy(previous, pPacket->getMessage().c_str(), sizeof(previous) - 1);
        previous[sizeof(previous) - 1] = 0;
    }

#endif

	__END_CATCH
}
