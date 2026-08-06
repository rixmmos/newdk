//--------------------------------------------------------------------------------
//
// Filename    : GCReconnectLoginHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "GCReconnectLogin.h"

#ifdef __GAME_CLIENT__
#include "CGConnect.h"
#include "ClientPlayer.h"
#endif

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
void GCReconnectLoginHandler::execute(GCReconnectLogin* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX
#ifdef __GAME_CLIENT__
#endif
        __END_DEBUG_EX __END_CATCH
}
