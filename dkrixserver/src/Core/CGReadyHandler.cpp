//////////////////////////////////////////////////////////////////////////////
// Filename    : CGReadyHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGReady.h"

#ifdef __GAME_SERVER__
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZonePlayerManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
void CGReadyHandler::execute(CGReady* pPacket, Player* pPlayer)

{
    __BEGIN_TRY
    __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    // filelog("CGReadyTrace.txt", "CGReadyHandler : BEGIN");

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    
    // Zone* pZone = pCreature->getZone();
    // Assert(pZone != NULL);

    // filelog("CGReadyTrace.txt", "CGReadyHandler : After pointer");

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    try {
        
        // g_pIncomingPlayerManager->deletePlayer_NOBLOCKED(pGamePlayer->getSocket()->getSOCKET());
        g_pIncomingPlayerManager->deletePlayer(pGamePlayer->getSocket()->getSOCKET());

        
        g_pIncomingPlayerManager->pushOutPlayer(pGamePlayer);

        // filelog("CGReadyTrace.txt", "CGReadyHandler : After deletePlayer[Name:%s]", pCreature->getName().c_str());

         
    } catch (NoSuchElementException& nsee) {
        StringStream msg;
        msg << "Critical Error : IPM  .  ..  - -;\n" << nsee.toString();
        throw Error(msg.toString());
    }

    
    pGamePlayer->setPlayerStatus(GPS_NORMAL);

    // filelog("CGReadyTrace.txt", "CGReadyHandler : END");

#endif

    __END_DEBUG_EX
    __END_CATCH
}
