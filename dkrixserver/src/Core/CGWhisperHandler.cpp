//////////////////////////////////////////////////////////////////////////////
// Filename    : CGWhisperHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGWhisper.h"

#ifdef __GAME_SERVER__
#include "Creature.h"
#include "DB.h"
#include "GCWhisper.h"
#include "GCWhisperFailed.h"
#include "GGServerChat.h"
#include "GamePlayer.h"
#include "GameServerInfoManager.h"
#include "LogNameManager.h"
#include "LoginServerManager.h"
#include "PCFinder.h"
#include "Properties.h"

#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGWhisperHandler::execute(CGWhisper* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    try {
        Player* pTargetPlayer = NULL;

        Creature* pCreature = pGamePlayer->getCreature();

        bool Success = false;

        
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        Creature* pTargetCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getName());

        
        if (pTargetCreature != NULL) {
            
            if (LogNameManager::getInstance().isExist(pCreature->getName())) {
                filelog("chatLog.txt", "[Whisper] %s --> %s> %s", pCreature->getName().c_str(),
                        pTargetCreature->getName().c_str(), pPacket->getMessage().c_str());
            }

            pTargetPlayer = pTargetCreature->getPlayer();
            Assert(pTargetPlayer != NULL);
            Success = true;

            if (pGamePlayer->isPenaltyFlag(PENALTY_TYPE_MUTE)) {
                Success = false;
            }

            
            if (Success) {
                if (((GamePlayer*)pTargetPlayer)->getPlayerStatus() == GPS_NORMAL) {
                    if (pCreature != NULL && pTargetCreature != NULL) {
                        
                        GCWhisper gcWhisper;

                        
                        gcWhisper.setName(pCreature->getName());
                        gcWhisper.setColor(pPacket->getColor());
                        gcWhisper.setMessage(pPacket->getMessage());
                        gcWhisper.setRace(pCreature->getRace());
                        pTargetPlayer->sendPacket(&gcWhisper);

                    } else {
                        GCWhisperFailed gcWhisperFailed;
                        pGamePlayer->sendPacket(&gcWhisperFailed);
                    }
                }
            }

            
        } else {
            /*inthesky*/
             
            Statement* pStmt = NULL;
            Result* pResult = NULL;

            bool bServerFind = false;
            ServerGroupID_t CurrentServerGroupID;
            string LogOn;
            string PlayerID;

            try {
                BEGIN_DB {
                    pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

                    
                    pResult =
                        pStmt->executeQuery("SELECT PlayerID FROM Slayer WHERE Name='%s'", pPacket->getName().c_str());

                    if (pResult->next()) {
                        PlayerID = pResult->getString(1);
                        SAFE_DELETE(pStmt);

                        
                        pStmt = g_pDatabaseManager->getDistConnection("USERINFO")->createStatement();
                        pResult = pStmt->executeQuery(
                            "SELECT CurrentServerGroupID, LogOn FROM Player WHERE PlayerID='%s'", PlayerID.c_str());

                        
                        if (pResult->next()) {
                            CurrentServerGroupID = pResult->getInt(1);
                            LogOn = pResult->getString(2);

                            
                            if (LogOn == "GAME") {
                                bServerFind = true;
                                SAFE_DELETE(pStmt);
                            }
                        } else {
                            SAFE_DELETE(pStmt);
                        }
                    } else {
                        SAFE_DELETE(pStmt);
                    }

                    if (bServerFind) 
                    {
                         


                        GameServerInfo* pGameServerInfo = g_pGameServerInfoManager->getGameServerInfo(
                            1, CurrentServerGroupID, g_pConfig->getPropertyInt("WorldID"));
                        if (pGameServerInfo != NULL) {
                            GGServerChat ggServerChat;
                            ggServerChat.setSender(pCreature->getName());
                            ggServerChat.setReceiver(pPacket->getName());
                            ggServerChat.setColor(pPacket->getColor());
                            ggServerChat.setMessage(pPacket->getMessage());
                            ggServerChat.setRace(pCreature->getRace());

                            g_pLoginServerManager->sendPacket(pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(),
                                                              &ggServerChat);
                        } else {
                            GCWhisperFailed gcWhisperFailed;
                            pGamePlayer->sendPacket(&gcWhisperFailed);
                        }
                    } else {
                        GCWhisperFailed gcWhisperFailed;
                        pGamePlayer->sendPacket(&gcWhisperFailed);
                    }
                }
                END_DB(pStmt)
            } catch (...) { /* write log plz */
            }
        }

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
    } catch (Throwable& t) {
        // cout << t.toString() << endl;
        GCWhisperFailed gcWhisperFailed;
        pGamePlayer->sendPacket(&gcWhisperFailed);
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
