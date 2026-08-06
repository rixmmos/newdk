//----------------------------------------------------------------------
//
// Filename    : LCLoginOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LCLoginOK.h"

#ifdef __GAME_CLIENT__

#include "CLGetPCList.h"
#include "ClientPlayer.h"

#endif

//----------------------------------------------------------------------
//

//

//
//----------------------------------------------------------------------
void LCLoginOKHandler::execute(LCLoginOK* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__

        ClientPlayer* pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

    CLGetPCList clGetPCList;

    pClientPlayer->sendPacket(&clGetPCList);

    
    pClientPlayer->setPlayerStatus(CPS_AFTER_SENDING_CL_GET_PC_LIST);

#endif

    __END_DEBUG_EX __END_CATCH
}
