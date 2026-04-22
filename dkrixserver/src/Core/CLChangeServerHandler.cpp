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
// ķ´ė¯¼ģ¯´ģ–øķøź°€ PC ģ¯ ė¦¬ģ¤ķøė¼ ė‹¬ė¯¼ź³  ģ”ģ²­ķ•´ģ¤ė©´, ėź·øģ¯ø ģ„ė²„ė” DBėė¶€ķ„°
// PCė“¤ģ¯ ģ •ė³´ė¼ ėė”©ķ•´ģ„ LCPCList ķØķ‚·ģ— ė‹´ģ•„ģ„ ģ „ģ†ķ•ė‹¤.
//////////////////////////////////////////////////////////////////////////////
void CLChangeServerHandler::execute(CLChangeServer* pPacket, Player* pPlayer) throw(ProtocolException, Error)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    ServerGroupID_t CurrentServerGroupID = pPacket->getServerGroupID();
    pLoginPlayer->setServerGroupID(CurrentServerGroupID);

    try {
        Connection* pConnection = g_pDatabaseManager->getConnection("DARKEDEN");

        //----------------------------------------------------------------------
        // ģ¯´ģ  LCPCList ķØķ‚·ģ¯„ ė§ė“¤ģ–´ģ„ ė³´ė‚´ģ˛
        //----------------------------------------------------------------------
        LCPCList lcPCList;

        pLoginPlayer->makePCList(lcPCList);
        pLoginPlayer->sendPacket(&lcPCList);
        pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);

        PreparedStatement stmt(pConnection, "UPDATE Player SET CurrentServerGroupID = ? WHERE PlayerID = ?");
        stmt.bindInt(1, (int)pPacket->getServerGroupID());
        stmt.bindString(2, pLoginPlayer->getID());
        stmt.execute();
    } catch (SQLQueryException& sce) {
        throw DisconnectException(sce.toString());
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
