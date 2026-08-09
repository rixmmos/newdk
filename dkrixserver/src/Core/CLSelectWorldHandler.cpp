//////////////////////////////////////////////////////////////////////////////
// Filename    : CLSelectWorldHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLSelectWorld.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "DatabaseManager.h"
#include "GameServerGroupInfoManager.h"
#include "GameServerInfoManager.h"
#include "GameWorldInfoManager.h"
#include "LCServerList.h"
#include "LoginPlayer.h"
#include "OptionInfo.h"
#include "PCSlayerInfo.h"
#include "PCVampireInfo.h"
#include "PreparedStatement.h"
#include "ServerGroupInfo.h"
#include "Shape.h"
#include "UserInfo.h"
#include "UserInfoManager.h"

#endif

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CLSelectWorldHandler::execute(CLSelectWorld* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);
    // cout << "Start execute" << endl;

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
    WorldID_t WorldID = pPacket->getWorldID();

    // Assert(WorldID <= g_pGameWorldInfoManager->getSize());
    //  by sigi. 2002.12.20
    if (WorldID > g_pGameWorldInfoManager->getSize()) {
        filelog("errorLogin.txt", "WorldID Over[%d/%d]", (int)WorldID, (int)g_pGameWorldInfoManager->getSize());
        throw DisconnectException("WorldID over");
    }


    GameWorldInfo* pGameWorldInfo = g_pGameWorldInfoManager->getGameWorldInfo(WorldID);
    if (pGameWorldInfo->getStatus() == WORLD_CLOSE) {
        filelog("errorLogin.txt", "WorldClosed[%d]", (int)WorldID);
        throw DisconnectException("WorldClosed");
    }


    // if (WorldID==2) throw DisconnectException();

    pLoginPlayer->setWorldID(WorldID);

    Statement* pStmt = NULL;

    try {
        int GroupNum = g_pGameServerGroupInfoManager->getSize(WorldID);

        // cout << "WorldNum : " << (int)GroupNum << endl;

        ServerGroupInfo* aServerGroupInfo[GroupNum];

        for (int i = 0; i < GroupNum; i++) {
            ServerGroupInfo* pServerGroupInfo = new ServerGroupInfo();
            GameServerGroupInfo* pGameServerGroupInfo =
                g_pGameServerGroupInfoManager->getGameServerGroupInfo(i, WorldID);
            pServerGroupInfo->setGroupID(pGameServerGroupInfo->getGroupID());
            pServerGroupInfo->setGroupName(pGameServerGroupInfo->getGroupName());
            // pServerGroupInfo->setStat(SERVER_FREE);

            UserInfo* pUserInfo = g_pUserInfoManager->getUserInfo(pGameServerGroupInfo->getGroupID(), WorldID);

            WORD UserModify = 800;
#ifdef __CHINA_SERVER__
            WORD UserMax = 1800;
#else
            WORD UserMax = 1500;
#endif

            if (pUserInfo->getUserNum() < 100 + UserModify) {
                pServerGroupInfo->setStat(SERVER_FREE);
            } else if (pUserInfo->getUserNum() < 250 + UserModify) {
                pServerGroupInfo->setStat(SERVER_NORMAL);
            } else if (pUserInfo->getUserNum() < 400 + UserModify) {
                pServerGroupInfo->setStat(SERVER_BUSY);
            }
#ifdef __CHINA_SERVER__
            else if (pUserInfo->getUserNum() < 1000 + UserModify)
#else
            else if (pUserInfo->getUserNum() < 500 + UserModify)
#endif
            {
                pServerGroupInfo->setStat(SERVER_VERY_BUSY);
            } else // if (pUserInfo->getUserNum() >= 500 + UserModify )
            {
                pServerGroupInfo->setStat(SERVER_FULL);
            }
            // else
            {
                // pServerGroupInfo->setStat(SERVER_DOWN);
            }

            if (pUserInfo->getUserNum() >= UserMax) {
                pServerGroupInfo->setStat(SERVER_FULL);
            }

            if (pGameServerGroupInfo->getStat() == SERVER_DOWN) {
                pServerGroupInfo->setStat(SERVER_DOWN);
            }

            aServerGroupInfo[i] = pServerGroupInfo;

            // cout << "AddServer : " << pServerGroupInfo->getGroupName() << endl;
        }

        LCServerList lcServerList;

        pStmt = NULL;
        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            PreparedStatement selectPlayerServerGroupStmt(pConn,
                                                          "SELECT CurrentServerGroupID FROM Player where PlayerID=?");
            selectPlayerServerGroupStmt.bindString(1, pLoginPlayer->getID());
            Result* pResult = selectPlayerServerGroupStmt.execute();

            if (pResult->next()) {
                lcServerList.setCurrentServerGroupID(pResult->getInt(1));
            }

            SAFE_DELETE(pStmt); // by sigi -_-
        }
        END_DB(pStmt) // by sigi T_T

        for (int k = 0; k < GroupNum; k++) {
            lcServerList.addListElement(aServerGroupInfo[k]);
        }

        pLoginPlayer->sendPacket(&lcServerList);

        //		pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);

    } catch (Throwable& t) {
        // cout << t.toString() << endl;
    }
    // cout << "End execute" << endl;

#endif

    __END_DEBUG_EX __END_CATCH
}
