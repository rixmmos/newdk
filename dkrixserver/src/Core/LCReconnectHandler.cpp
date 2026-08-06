//--------------------------------------------------------------------------------
//
// Filename    : LCReconnectHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "LCReconnect.h"

#ifdef __GAME_CLIENT__
#include "CGConnect.h"
#include "ClientPlayer.h"
#endif

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
void LCReconnectHandler::execute(LCReconnect* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__

        ClientPlayer* pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

    
    
    // cout << "Disconnecting from login server" << endl;
    pClientPlayer->disconnect();

    
    // cout << "Reconnecting to " << pPacket->getGameServerIP() << ":" << pPacket->getGameServerPort() << endl;

    try {
        pClientPlayer->getSocket()->reconnect(pPacket->getGameServerIP(), pPacket->getGameServerPort());

        
        
        pClientPlayer->getSocket()->setNonBlocking();
        pClientPlayer->getSocket()->setLinger(0);

    } catch (ConnectException& ce) {
        throw Error(ce.toString());
    }

    
    
    // cout << "Sending CGConnect with Key(" << pPacket->getKey() << ")" << endl;

    CGConnect cgConnect;
    cgConnect.setKey(pPacket->getKey());
    cgConnect.setPCType(pClientPlayer->getPCType());
    cgConnect.setPCName(pClientPlayer->getPCName());

    pClientPlayer->sendPacket(&cgConnect);
    pClientPlayer->setPlayerStatus(CPS_AFTER_SENDING_CG_CONNECT);

#endif

    __END_DEBUG_EX __END_CATCH
}
