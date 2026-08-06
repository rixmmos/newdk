
//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAuthKeyHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAuthKey.h"
#ifdef __GAME_CLIENT__
	#include "Client.h"
#endif
//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////

void GCAuthKeyHandler::execute ( GCAuthKey * pPacket , Player * pPlayer )


throw ( ProtocolException , Error )
{
	__BEGIN_TRY
//		__BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__

#if !defined(OUTPUT_DEBUG) && !defined(_DEBUG) && !defined(__DEBUG_OUTPUT__) && defined(__NPROTECT__) //add by zdj
	Set_Auth(pPacket->getKey());

#endif

// REMOVED: nProtect anti-cheat code (SDL migration - no longer needed)

#endif
//	__END_DEBUG_EX
	__END_CATCH
}
