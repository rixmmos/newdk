//----------------------------------------------------------------------
//
// Filename    : LGKickCharacterHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LGKickCharacter.h"

#ifdef __GAME_SERVER__

#include "Creature.h"
#include "GLKickVerify.h"
#include "GamePlayer.h"
#include "LogDef.h"
#include "LoginServerManager.h"
#include "PCFinder.h"

#endif

//----------------------------------------------------------------------
//
// LGKickCharacterHander::execute()
//


//
//----------------------------------------------------------------------
void LGKickCharacterHandler::execute(LGKickCharacter* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX
#ifdef __GAME_SERVER__

        
        /*
        if (!g_pPCFinder->setKickCharacter(pPacket->getPCName(), pPacket->getHost(), pPacket->getPort() ))
        {
            GLKickVerify glKickVerify;
            glKickVerify.setKicked(false);
            glKickVerify.setID(pPacket->getID());
            glKickVerify.setPCName(pPacket->getPCName());

            g_pLoginServerManager->sendPacket(pPacket->getHost() , pPacket->getPort() , &glKickVerify);

            //cout << "LGKickVerify Send Packet to ServerIP : " << pPacket->getHost() << endl;
            //cout << "LGKickVerify Send Packet to ServerPort : " << pPacket->getPort() << endl;

            return;
        }
        */

        try {

        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        
        Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getPCName());

        
        if (pCreature == NULL) {
            GLKickVerify glKickVerify;
            glKickVerify.setKicked(false);
            glKickVerify.setID(pPacket->getID());
            glKickVerify.setPCName(pPacket->getPCName());

            g_pLoginServerManager->sendPacket(pPacket->getHost(), pPacket->getPort(), &glKickVerify);

            // cout << "LGKickVerify Send Packet to ServerIP : " << pPacket->getHost() << endl;
            // cout << "LGKickVerify Send Packet to ServerPort : " << pPacket->getPort() << endl;

            g_pPCFinder->unlock();
            return;
        }

        // cout << "KickCharacter : " << pPacket->getPCName().c_str() << endl;

        GamePlayer* pGamePlayer = NULL;
        try {
            pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
        } catch (Throwable& t) {
            
            // filelog("kickCharacterAssert.txt", "%s", t.toString().c_str());
            g_pPCFinder->unlock();
            return;
        }

        // Assert(pGamePlayer!=NULL);
        if (pGamePlayer == NULL) 
        {
            g_pPCFinder->unlock();
            return;
        }

        int fd = -1;
        Socket* pSocket = pGamePlayer->getSocket();
        if (pSocket != NULL)
            fd = (int)pSocket->getSOCKET();

        FILELOG_INCOMING_CONNECTION("incomingDisconnect.log", "Kick FD : %d, %s", fd,
                                    (pSocket == NULL ? "NULL" : pSocket->getHost().c_str()));


        
        pGamePlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
        pGamePlayer->setItemRatioBonusPoint(4);
        pGamePlayer->setKickForLogin(true);

        
        pGamePlayer->setKickRequestHost(pPacket->getHost());
        pGamePlayer->setKickRequestPort(pPacket->getPort());

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

    } catch (NoSuchElementException&) {
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
