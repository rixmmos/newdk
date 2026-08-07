//////////////////////////////////////////////////////////////////////
//
// Filename    : GCFriendChattingHandle.cpp
// Written By  : aliveviva@gmail.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "DB.h"
#include "GCFriendChatting.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#include "PreparedStatement.h"

void GCFriendChattingHandler::execute(GCFriendChatting* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX
        // cout<<"friend1"<<endl;
        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Creature* pCreature = pGamePlayer->getCreature();

    DWORD Command = pPacket->getCommand();

    if (Command > MAX_CG)
        throw InvalidProtocolException("Command Error");
    // cout<<"friend2"<<endl;
    switch (Command) {
        /////////////////////////////////////////////////CG_ADD_FRIEND_AGREE/////////////////////////////////////////
    case CG_ADD_FRIEND_AGREE: {
        Statement* pStmt = NULL;

        BEGIN_DB {
            PreparedStatement insertStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                         "INSERT INTO FriendList (Friend_Name, Owner_Name) VALUES (?, ?)");
            insertStmt.bindString(1, pCreature->getName());
            insertStmt.bindString(2, pPacket->getPlayerName());
            insertStmt.execute();

            insertStmt.bindString(1, pPacket->getPlayerName());
            insertStmt.bindString(2, pCreature->getName());
            insertStmt.execute();
        }
        END_DB(pStmt)

        Creature* pTargetCreature = NULL;
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))
        pTargetCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getPlayerName());
        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

        if (pTargetCreature != NULL) {
            Player* pTargetPlayer = pTargetCreature->getPlayer();
            if (pTargetPlayer == NULL)
                break;
            GamePlayer* pTargetGamePlayer = dynamic_cast<GamePlayer*>(pTargetPlayer);
            if (pTargetGamePlayer == NULL)
                break;

            GCFriendChatting gcFriend;
            gcFriend.setCommand(GC_ADD_FRIEND_OK);
            gcFriend.setPlayerName(pCreature->getName());
            pTargetGamePlayer->sendPacket(&gcFriend);

            GCFriendChatting gcFriend2;
            gcFriend2.setCommand(GC_ADD_FRIEND_OK);
            gcFriend2.setPlayerName(pPacket->getPlayerName());
            pGamePlayer->sendPacket(&gcFriend2);
        }
        break;
    }
        ///////////////////////////////////////////////CG_ADD_FRIEND///////////////////////////////////////////////////
    case CG_ADD_FRIEND: {
        Creature* pTargetCreature = NULL;
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))
        pTargetCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getPlayerName());
        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
        bool blResult = true;
        if (pTargetCreature != NULL) {
            Player* pTargetPlayer = pTargetCreature->getPlayer();
            if (pTargetPlayer == NULL)
                break;
            GamePlayer* pTargetGamePlayer = dynamic_cast<GamePlayer*>(pTargetPlayer);
            if (pTargetGamePlayer == NULL)
                break;
            Statement* pStmt = NULL;
            Result* pResult = NULL;

            BEGIN_DB {
                ////////////////////GC_ADD_FRIEND_EXIST//////////////////
                PreparedStatement existStmt(
                    g_pDatabaseManager->getConnection("DARKEDEN"),
                    "SELECT Friend_Name, IsBlack FROM FriendList WHERE Owner_Name = ? and Friend_Name=?");
                existStmt.bindString(1, pCreature->getName());
                existStmt.bindString(2, pPacket->getPlayerName());
                pResult = existStmt.execute();
                if (pResult->next()) {
                    blResult = false;
                    GCFriendChatting gcFriend3;
                    gcFriend3.setCommand(GC_ADD_FRIEND_EXIST);
                    gcFriend3.setPlayerName(pPacket->getPlayerName());
                    pGamePlayer->sendPacket(&gcFriend3);
                }
                /////////////////GC_ADD_FRIEND_BLACK///////////////////////
                PreparedStatement blackStmt(
                    g_pDatabaseManager->getConnection("DARKEDEN"),
                    "SELECT IsBlack FROM FriendList WHERE Owner_Name=? and Friend_Name=? and IsBlack=1");
                blackStmt.bindString(1, pPacket->getPlayerName());
                blackStmt.bindString(2, pCreature->getName());
                pResult = blackStmt.execute();
                if (pResult->next()) {
                    blResult = false;
                    GCFriendChatting gcFriend4;
                    gcFriend4.setCommand(GC_ADD_FRIEND_BLACK);
                    gcFriend4.setPlayerName(pPacket->getPlayerName());
                    pGamePlayer->sendPacket(&gcFriend4);
                }
            }
            END_DB(pStmt)

            if (blResult) {
                GCFriendChatting gcFriend;
                gcFriend.setCommand(GC_ADD_FRIEND_REQUEST);
                gcFriend.setPlayerName(pCreature->getName());
                pTargetGamePlayer->sendPacket(&gcFriend);

                GCFriendChatting gcFriend2;
                gcFriend2.setCommand(GC_ADD_FRIEND_WAIT);
                gcFriend2.setPlayerName(pPacket->getPlayerName());
                pGamePlayer->sendPacket(&gcFriend2);
            }
        }
        break;
    }
        ////////////////////////////////////////////////////CG_MESSAGE//////////////////////////////////////////////
    case CG_MESSAGE: {
        Creature* pTargetCreature = NULL;
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))
        pTargetCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getPlayerName());
        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

        if (pTargetCreature != NULL) {
            Player* pTargetPlayer = pTargetCreature->getPlayer();
            if (pTargetPlayer == NULL)
                break;
            GamePlayer* pTargetGamePlayer = dynamic_cast<GamePlayer*>(pTargetPlayer);
            if (pTargetGamePlayer == NULL)
                break;
            GCFriendChatting gcFriend;
            gcFriend.setCommand(GC_MESSAGE);
            gcFriend.setPlayerName(pCreature->getName());
            gcFriend.setMessage(pPacket->getMessage());
            pTargetGamePlayer->sendPacket(&gcFriend);
        } else {
            Statement* pStmt = NULL;
            BEGIN_DB {
                PreparedStatement historyStmt(
                    g_pDatabaseManager->getConnection("DARKEDEN"),
                    "INSERT INTO FriendHistory(HistoryMessage, Owner_Name, Friend_Name) VALUES (?, ?, ?)");
                historyStmt.bindString(1, pPacket->getMessage());
                historyStmt.bindString(2, pPacket->getPlayerName());
                historyStmt.bindString(3, pCreature->getName());
                historyStmt.execute();
            }
            END_DB(pStmt)
        }
        break;
    }
    ///////////////////////////////////////////CG_GETSTATE//////////////////////////////////////////////////////////
    case CG_UPDATE: {
        // cout<<"friend3"<<endl;
        Statement* pStmt = NULL;
        Result* pResult = NULL;

        BEGIN_DB {
            PreparedStatement listStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                       "SELECT Friend_Name, IsBlack FROM FriendList WHERE Owner_Name = ?");
            listStmt.bindString(1, pCreature->getName());
            pResult = listStmt.execute();
            while (pResult->next()) {
                GCFriendChatting gcFriend;
                gcFriend.setCommand(GC_UPDATE);
                gcFriend.setPlayerName(pResult->getString(1));
                gcFriend.setIsBlack(pResult->getBYTE(2));

                Creature* pTargetCreature = NULL;
                __ENTER_CRITICAL_SECTION((*g_pPCFinder))
                pTargetCreature = g_pPCFinder->getCreature_LOCKED(gcFriend.getPlayerName());
                __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

                if (pTargetCreature == NULL)
                    gcFriend.setIsOnLine(0);
                else
                    gcFriend.setIsOnLine(1);

                pGamePlayer->sendPacket(&gcFriend);
            }

            PreparedStatement historyStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                          "SELECT HistoryMessage,Friend_Name FROM FriendHistory WHERE Owner_Name = ?");
            historyStmt.bindString(1, pCreature->getName());
            pResult = historyStmt.execute();
            bool IsHave = false;
            while (pResult->next()) {
                IsHave = true;
                GCFriendChatting gcFriend2;
                gcFriend2.setCommand(GC_MESSAGE);
                gcFriend2.setMessage(pResult->getString(1));
                gcFriend2.setPlayerName(pResult->getString(2));

                pGamePlayer->sendPacket(&gcFriend2);
            }
            if (IsHave) {
                PreparedStatement clearStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                            "DELETE FROM FriendHistory WHERE Owner_Name=?");
                clearStmt.bindString(1, pCreature->getName());
                clearStmt.execute();
            }
        }
        END_DB(pStmt)

        break;
    }
        ////////////////////////////////////////CG_ADD_FRIEND_REFUSE/////////////////////////////
    case CG_ADD_FRIEND_REFUSE: {
        Creature* pTargetCreature = NULL;
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))
        pTargetCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getPlayerName());
        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

        if (pTargetCreature == NULL) {
            GCFriendChatting gcFriend;
            gcFriend.setCommand(GC_ADD_FRIEND_ERROR);
            pGamePlayer->sendPacket(&gcFriend);
        } else {
            Player* pTargetPlayer = pTargetCreature->getPlayer();
            if (pTargetPlayer == NULL)
                break;
            GamePlayer* pTargetGamePlayer = dynamic_cast<GamePlayer*>(pTargetPlayer);
            if (pTargetGamePlayer == NULL)
                break;
            GCFriendChatting gcFriend;
            gcFriend.setCommand(GC_ADD_FRIEND_REFUSE);
            gcFriend.setPlayerName(pCreature->getName());
            pTargetGamePlayer->sendPacket(&gcFriend);
        }
        break;
    }
        /////////////////////////////////////CG_ADD_FRIEND_BLACK///////////////////////////////////////
    case CG_ADD_FRIEND_BLACK: {
        Statement* pStmt = NULL;
        BEGIN_DB {
            PreparedStatement blackInsertStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                              "INSERT INTO FriendList (Friend_Name, Owner_Name, IsBlack) VALUES "
                                              "(?, ?, 1)");
            blackInsertStmt.bindString(1, pPacket->getPlayerName());
            blackInsertStmt.bindString(2, pCreature->getName());
            blackInsertStmt.execute();
        }
        END_DB(pStmt)

        Creature* pTargetCreature = NULL;
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))
        pTargetCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getPlayerName());
        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

        if (pTargetCreature == NULL) {
            GCFriendChatting gcFriend;
            gcFriend.setCommand(GC_ADD_FRIEND_ERROR);
            pGamePlayer->sendPacket(&gcFriend);
        } else {
            Player* pTargetPlayer = pTargetCreature->getPlayer();
            if (pTargetPlayer == NULL)
                break;
            GamePlayer* pTargetGamePlayer = dynamic_cast<GamePlayer*>(pTargetPlayer);
            if (pTargetGamePlayer == NULL)
                break;
            GCFriendChatting gcFriend;
            gcFriend.setCommand(GC_ADD_FRIEND_REFUSE);
            gcFriend.setPlayerName(pCreature->getName());
            pTargetGamePlayer->sendPacket(&gcFriend);
        }
        break;
    }
        //////////////////////////////////////////////////////CG_FRIEND_DELTET//////////////////////////////////
    case CG_FRIEND_DELETE: {
        Statement* pStmt = NULL;
        BEGIN_DB {
            PreparedStatement deleteStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                         "DELETE FROM FriendList WHERE Owner_Name=? and Friend_Name=?");
            deleteStmt.bindString(1, pCreature->getName());
            deleteStmt.bindString(2, pPacket->getPlayerName());
            deleteStmt.execute();

            deleteStmt.bindString(1, pPacket->getPlayerName());
            deleteStmt.bindString(2, pCreature->getName());
            deleteStmt.execute();
        }
        END_DB(pStmt)

        GCFriendChatting gcFriend;
        gcFriend.setCommand(GC_FRIEND_DELETE);
        gcFriend.setPlayerName(pPacket->getPlayerName());
        pGamePlayer->sendPacket(&gcFriend);

        Creature* pTargetCreature = NULL;
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))
        pTargetCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getPlayerName());
        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

        if (pTargetCreature) {
            Player* pTargetPlayer = pTargetCreature->getPlayer();
            if (pTargetPlayer == NULL)
                break;
            GamePlayer* pTargetGamePlayer = dynamic_cast<GamePlayer*>(pTargetPlayer);
            if (pTargetGamePlayer == NULL)
                break;
            GCFriendChatting gcFriend2;
            gcFriend2.setCommand(GC_FRIEND_DELETE);
            gcFriend2.setPlayerName(pCreature->getName());
            pTargetGamePlayer->sendPacket(&gcFriend2);
        }
        break;
    }
        ////////////////////////////////////////////DEFAULT/////////////////////////////////////////////////////////////////
    default:
        break;
    }
    __END_DEBUG_EX __END_CATCH
}
