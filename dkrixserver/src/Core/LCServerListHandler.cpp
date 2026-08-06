//----------------------------------------------------------------------
//
// Filename    : LCServerListHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LCServerList.h"

#ifdef __GAME_CLIENT__
#include "CLCreateServer.h"
#include "CLDeleteServer.h"
#include "CLSelectBoard.h"
#include "CLSelectServer.h"
#include "ClientPlayer.h"

#endif

//----------------------------------------------------------------------



//----------------------------------------------------------------------
void LCServerListHandler::execute(LCServerList* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__
#endif

        __END_DEBUG_EX __END_CATCH
}
