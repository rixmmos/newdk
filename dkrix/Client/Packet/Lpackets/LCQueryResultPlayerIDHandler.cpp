//--------------------------------------------------------------------------------
//
// Filename    : LCQueryResultPlayerIDHandler.cpp
// Written By  : Reiot
// Description : 
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCQueryResultPlayerID.h"
#include "ClientDef.h"
#include "UIDialog.h"
#include "UIFunction.h"

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"
	#include "CLQueryPlayerID.h"
	#include "CLRegisterPlayer.h"
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void LCQueryResultPlayerIDHandler::execute ( LCQueryResultPlayerID * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	 


	
	if (pPacket->isExist())
	{
		
		UI_AlreadyExistIDMessage();
	}
	else
	{
		
		UI_NoAlreadyExistIDMessage();
	}

#endif
		
	__END_CATCH
}
