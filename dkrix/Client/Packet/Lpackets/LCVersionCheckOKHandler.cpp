//----------------------------------------------------------------------
//
// Filename    : LCVersionCheckOKHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCVersionCheckOK.h"
#include "ClientDef.h"

static void TraceLoginFlowPacket(const char* step)
{
	(void)step;
}


//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void LCVersionCheckOKHandler::execute ( LCVersionCheckOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	TraceLoginFlowPacket("LCVersionCheckOKHandler");


	

#endif
		
	__END_CATCH
}
