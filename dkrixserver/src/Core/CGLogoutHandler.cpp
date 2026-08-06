//////////////////////////////////////////////////////////////////////////////
// Filename    : CGLogoutHandler.cc
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGLogout.h"

#ifdef __GAME_SERVER__
#include <stdio.h>

#include "Creature.h"
#include "GCSystemMessage.h"
#include "GLIncomingConnection.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "Inventory.h"
#include "LoginServerManager.h"
#include "Ousters.h"
#include "PKZoneInfoManager.h"
#include "PlayerCreature.h"
#include "Properties.h"
#include "Relic.h"
#include "RelicUtil.h"
#include "ResurrectLocationManager.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "ZoneUtil.h"
#endif


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void CGLogoutHandler::execute(CGLogout* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        //	Assert(pPacket != NULL);
        Assert(pPlayer != NULL);

    
    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    // cout << "CGLogoutHandler: " << pGamePlayer->getID() << endl;

    Creature* pCreature = pGamePlayer->getCreature();

    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();

    Assert(pZone != NULL);


    
    pGamePlayer->logLoginoutDateTime();

    try {
        
        //		bool bSendPacket = false;
        //		dropRelicToZone( pCreature, bSendPacket );

        if (pCreature->isPLAYER() && g_pPKZoneInfoManager->isPKZone(pCreature->getZoneID())) {
            g_pPKZoneInfoManager->leavePKZone(pCreature->getZoneID());
        }

        if (g_pConfig->hasKey("Hardcore") && g_pConfig->getPropertyInt("Hardcore") != 0 && pPacket == NULL) {
        } else {
            
            pCreature->save();

            if (pCreature->isSlayer()) {
                Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
                pSlayer->tinysave("LastPlayDate=now()");
            } else if (pCreature->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                pVampire->tinysave("LastPlayDate=now()");
            } else if (pCreature->isOusters()) {
                Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
                pOusters->tinysave("LastPlayDate=now()");
            }

            //////////////////////////////////////////////////////////////
            
            
            
            //
            
            //////////////////////////////////////////////////////////////

            
            if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA) || pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY)) {
                

                ZoneID_t ZoneID = 0;
                ZoneCoord_t ZoneX = 0;
                ZoneCoord_t ZoneY = 0;
                ZONE_COORD ResurrectCoord;

                if (pCreature->isPC()) {
                    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

                    g_pResurrectLocationManager->getPosition(pPC, ResurrectCoord);

                    ZoneID = ResurrectCoord.id;
                    ZoneX = ResurrectCoord.x;
                    ZoneY = ResurrectCoord.y;

                    char pField[80];
                    sprintf(pField, "ZoneID=%d, XCoord=%d, YCoord=%d, CurrentHP=HP", ZoneID, ZoneX, ZoneY);

                    if (pPC->isSlayer()) {
                        Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
                        pSlayer->tinysave(pField);
                    } else if (pPC->isVampire()) {
                        Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
                        pVampire->tinysave(pField);
                    } else if (pPC->isOusters()) {
                        Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
                        pOusters->tinysave(pField);
                    }
                }
            }
             
        }


        //
        
        //
        // *CAUTION*
        //
        
        
        //
        pZone->deleteCreature(pCreature, pCreature->getX(), pCreature->getY());

        ////cout << "PC deleted from Zone >> ";

        
        
        pZone->getZoneGroup()->getZonePlayerManager()->deletePlayer(pGamePlayer->getSocket()->getSOCKET());

        
        // g_pIncomingPlayerManager->pushPlayer(pGamePlayer);

        
        
        pZone->getZoneGroup()->getZonePlayerManager()->pushOutPlayer(pGamePlayer);

        ////cout << "Move PC to IPM >> ";
    } catch (NoSuchElementException& nsee) {
        throw DisconnectException();
    }

    
    
    // add by zdj
    GLIncomingConnection glIncomingConnection;
    glIncomingConnection.setPlayerID(pGamePlayer->getID());
    glIncomingConnection.setClientIP(pGamePlayer->getSocket()->getHost());

    g_pLoginServerManager->sendPacket(g_pConfig->getProperty("LoginServerIP"), 9999, &glIncomingConnection);

    pGamePlayer->setPlayerStatus(GPS_AFTER_SENDING_GL_INCOMING_CONNECTION);

#endif

    __END_DEBUG_EX __END_CATCH
}
