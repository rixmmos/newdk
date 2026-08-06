//--------------------------------------------------------------------------------
//
// Filename    : LGIncomingConnectionErrorHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "LGIncomingConnectionError.h"

#ifdef __GAME_SERVER__
#include "Assert.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "LogDef.h"
#endif

//--------------------------------------------------------------------------------
//
// LGIncomingConnectionErrorHander::execute()
//
//--------------------------------------------------------------------------------
void LGIncomingConnectionErrorHandler::execute(LGIncomingConnectionError* pPacket) {
    __BEGIN_TRY __BEGIN_DEBUG_EX
#ifdef __GAME_SERVER__

        
        //
        // *CAUTION*
        //
        
        
        
        
        //
        
        
        try {
        GamePlayer* pGamePlayer = g_pIncomingPlayerManager->getPlayer(pPacket->getPlayerID());

        Assert(pGamePlayer->getPlayerStatus() == GPS_AFTER_SENDING_GL_INCOMING_CONNECTION);

        
        cout << "Fail to join game server...(" << pPacket->getPlayerID() << ")" << endl;

        int fd = -1;
        Socket* pSocket = pGamePlayer->getSocket();
        if (pSocket != NULL)
            fd = (int)pSocket->getSOCKET();

        FILELOG_INCOMING_CONNECTION("incomingPenalty.log", "Error FD : %d, %s", fd,
                                    (pSocket == NULL ? "NULL" : pSocket->getHost().c_str()));


        pGamePlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
        pGamePlayer->setItemRatioBonusPoint(2);
    } catch (NoSuchElementException& nsee) {
        cout << "Player not exist or already disconnected." << endl;
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
