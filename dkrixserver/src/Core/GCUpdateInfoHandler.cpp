//----------------------------------------------------------------------
//
// Filename    : GCUpdateInfoHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GCUpdateInfo.h"

#ifdef __GAME_CLIENT__

#include "CGReady.h"
#include "ClientPlayer.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"

#endif

//----------------------------------------------------------------------



//----------------------------------------------------------------------
void GCUpdateInfoHandler::execute(GCUpdateInfo* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__

        ClientPlayer* pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

    // cout << pPacket->toString() << endl;

    //--------------------------------------------------
    
    //--------------------------------------------------
    pClientPlayer->setPlayerStatus(CPS_WAITING_FOR_LOADING);

    

    //--------------------------------------------------
    
    //--------------------------------------------------
    CGReady cgReady;
    pClientPlayer->sendPacket(&cgReady);
    pClientPlayer->setPlayerStatus(CPS_WAITING_FOR_GC_SET_POSITION);

#endif

    __END_DEBUG_EX __END_CATCH
}
