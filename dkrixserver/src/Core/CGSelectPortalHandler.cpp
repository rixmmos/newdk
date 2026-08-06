//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectPortalHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSelectPortal.h"

#ifdef __GAME_SERVER__
#include "GamePlayer.h"
#include "Portal.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"
#include "ZoneUtil.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSelectPortalHandler::execute(CGSelectPortal* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    try {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

        
        if (pGamePlayer->getPlayerStatus() != GPS_NORMAL)
            return;

        Creature* pCreature = pGamePlayer->getCreature();
        Zone* pZone = pCreature->getZone();

        
        if (!isValidZoneCoord(pZone, pCreature->getX(), pCreature->getY()))
            return;

        Tile& rTile = pZone->getTile(pCreature->getX(), pCreature->getY());

        
        if (rTile.hasPortal()) {
            Portal* pPortal = rTile.getPortal();

            if (pPortal->getPortalClass() == PORTAL_CLASS_MULTI) {
                if (pCreature->isSlayer()) {
                    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
                    if (!pSlayer->hasRideMotorcycle() && (pPortal->getObjectType() == PORTAL_SLAYER)) {
                        pPortal->activate(pCreature, pPacket->getZoneID());
                    }
                } else {
                    if (pPortal->getObjectType() == PORTAL_NORMAL || pPortal->getObjectType() == PORTAL_SLAYER) {
                        pPortal->activate(pCreature, pPacket->getZoneID());
                    }
                }
            }
        }
    } catch (Throwable& t) {
        // cout << t.toString() << endl;
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
