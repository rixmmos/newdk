//--------------------------------------------------------------------------------
//
// Filename    : GCUnburrowOKHandler.cpp
// Written By  : elca, Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "GCUnburrowOK.h"

#ifdef __GAME_CLIENT__
#include "ClientPlayer.h"
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCUnburrowOKHandler::execute(GCUnburrowOK* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__

        ClientPlayer* pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

    
    pClientPlayer->setX(pPacket->getX());
    pClientPlayer->setY(pPacket->getY());
    pClientPlayer->setDir(pPacket->getDir());

    // cout << "Unburrow OK to (" << (int)pPacket->getX() << "," << (int)pPacket->getY() << ","
    << Dir2String[pPacket->getDir()] << ")" << endl;

#endif

    __END_DEBUG_EX __END_CATCH
}
