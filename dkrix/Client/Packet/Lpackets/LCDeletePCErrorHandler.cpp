//----------------------------------------------------------------------
//
// Filename    : LCDeletePCErrorHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCDeletePCError.h"
#include "ClientDef.h"

extern void		PopupErrorMessage(ErrorID errorID);

//----------------------------------------------------------------------


//----------------------------------------------------------------------
void LCDeletePCErrorHandler::execute ( LCDeletePCError * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	//#ifdef __DEBUG_OUTPUT__
	//cout << "Fail to delete PC... (" << pPacket->toString() << ")" << endl;
	//throw ProtocolException("DeletePC Failed!");
	
	//UI_DeleteCharacterFailed();

	PopupErrorMessage( (ErrorID)pPacket->getErrorID() );

	g_ModeNext = MODE_WAIT_SELECTPC;

#endif
		
	__END_CATCH
}
