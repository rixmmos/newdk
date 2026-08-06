//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPhoneSayHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCPhoneSay.h"
#include "UserInformation.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCPhoneSayHandler::execute ( GCPhoneSay * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
	

	int slot = pPacket->getSlotID();

	char message[128];
	sprintf(message, "[%s] %s", 
						g_pUserInformation->PCSUserName[ slot ].GetString(), 
						pPacket->getMessage().c_str());

	//--------------------------------------------------
	
	//--------------------------------------------------
	//g_pChatManager->RemoveCurse( message );

	//------------------------------------------------------
	
	//------------------------------------------------------
	//UI_AddChatToHistory( message );


#endif

	__END_CATCH
}
