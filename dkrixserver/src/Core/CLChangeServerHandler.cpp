//////////////////////////////////////////////////////////////////////////////
// Filename    : CLChangeServerHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLChangeServer.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "DatabaseManager.h"
#include "GameServerGroupInfoManager.h"
#include "GameServerInfoManager.h"
#include "LCPCList.h"
#include "LoginPlayer.h"
#include "OptionInfo.h"
#include "PreparedStatement.h"
#endif

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void CLChangeServerHandler::execute(CLChangeServer* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    ServerGroupID_t CurrentServerGroupID = pPacket->getServerGroupID();
    pLoginPlayer->setServerGroupID(CurrentServerGroupID);

    Statement* pStmt = NULL;

    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        LCPCList lcPCList;

        pLoginPlayer->makePCList(lcPCList);
        pLoginPlayer->sendPacket(&lcPCList);
        pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);

        PreparedStatement updatePlayerServerGroupStmt(pConn,
                                                      "UPDATE Player set CurrentServerGroupID = ? WHERE PlayerID = ?");
        updatePlayerServerGroupStmt.bindInt(1, (int)pPacket->getServerGroupID());
        updatePlayerServerGroupStmt.bindString(2, pLoginPlayer->getID());
        updatePlayerServerGroupStmt.execute();


        SAFE_DELETE(pStmt);
    } catch (SQLQueryException& sce) {
        // cout << sce.toString() << endl;


        SAFE_DELETE(pStmt);

        throw DisconnectException(sce.toString());
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
