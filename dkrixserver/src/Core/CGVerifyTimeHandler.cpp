//////////////////////////////////////////////////////////////////////////////
// Filename    : CGVerifyTimeHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGVerifyTime.h"

#ifdef __GAME_SERVER__
#include "Assert.h"
#include "DB.h"
#include "GamePlayer.h"
#include "Ousters.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "Slayer.h"
#include "Vampire.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGVerifyTimeHandler::execute(CGVerifyTime* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    if (!(pGamePlayer->verifySpeed(pPacket))) {
        saveSpeedHackPlayer(pPlayer);
        throw DisconnectException(" ! !!");
    }

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGVerifyTimeHandler::saveSpeedHackPlayer(Player* pPlayer) {
    __BEGIN_TRY

#ifdef __GAME_SERVER__
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Creature* pCreature = pGamePlayer->getCreature();

    string ID = pPlayer->getID();
    string IP = pPlayer->getSocket()->getHost();
    string Name = "";

    if (pCreature != NULL) {
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            Name = "Slayer:" + pSlayer->getName();
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            Name = "Vampire:" + pVampire->getName();
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            Name = "Ousters:" + pOusters->getName();
        }
    }

    filelog("SpeedHack.log", "ID[%s], Name[%s], IP[%s]", ID.c_str(), Name.c_str(), IP.c_str());

    /////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////
    Statement* pStmt = NULL;

    try {
        static WorldID_t WorldID = g_pConfig->getPropertyInt("WorldID");
        static ServerGroupID_t ServerGroupID = g_pConfig->getPropertyInt("ServerID");

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getDistConnection("PLAYER_DB");

            PreparedStatement updateSpeedHackPlayerStmt(
                pConn, "UPDATE SpeedHackPlayer SET IP = ?, NAME = ?, WorldID = ?, ServerGroupID = ?, "
                       "Date = now(), Count = Count + 1 WHERE PlayerID = ?");
            updateSpeedHackPlayerStmt.bindString(1, IP);
            updateSpeedHackPlayerStmt.bindString(2, Name);
            updateSpeedHackPlayerStmt.bindInt(3, (int)WorldID);
            updateSpeedHackPlayerStmt.bindInt(4, (int)ServerGroupID);
            updateSpeedHackPlayerStmt.bindString(5, ID);
            updateSpeedHackPlayerStmt.execute();

            if (updateSpeedHackPlayerStmt.getAffectedRowCount() == 0) {
                PreparedStatement insertSpeedHackPlayerStmt(
                    pConn, "INSERT IGNORE INTO SpeedHackPlayer( PlayerID, IP, Name, WorldID, ServerGroupID, "
                           "Date, Count ) VALUES ( ?, ?, ?, ?, ?, now(), 1 )");
                insertSpeedHackPlayerStmt.bindString(1, ID);
                insertSpeedHackPlayerStmt.bindString(2, IP);
                insertSpeedHackPlayerStmt.bindString(3, Name);
                insertSpeedHackPlayerStmt.bindInt(4, (int)WorldID);
                insertSpeedHackPlayerStmt.bindInt(5, (int)ServerGroupID);
                insertSpeedHackPlayerStmt.execute();
            }
        }
        END_DB(pStmt)
    } catch (Throwable& t) {
        filelog("SpeedHackLogError.log", "%s", t.toString().c_str());
    }

#endif

    __END_CATCH
}
