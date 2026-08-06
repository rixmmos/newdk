
//////////////////////////////////////////////////////////////////////
//
// Filename    : CGAuthKeyHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "CGAuthKey.h"

#ifdef __GAME_SERVER__
#include "EventKick.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "gameguard/CSAuth.h"
#endif

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void CGAuthKeyHandler::execute(CGAuthKey* pPacket, Player* pPlayer)

{
    __BEGIN_TRY

#ifdef __GAME_SERVER__

    Assert(pPacket != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    DWORD key = pPacket->getKey();
    // 	if ( !pGamePlayer->getCSAuth().CheckAuthDword(key) )
    // 	{
    

    // 		GCSystemMessage gcSystemMessage;
    
    // 		pGamePlayer->sendPacket( &gcSystemMessage );

    // 		EventKick* pKick = new EventKick( pGamePlayer );
    // 		pKick->setDeadline(100);
    
    // 		pKick->sendMessage();

    // 		pGamePlayer->addEvent(pKick);
    // 	}

#endif

    __END_CATCH
}
