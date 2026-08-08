//////////////////////////////////////////////////////////////////////////////
// Filename    : CLQueryPlayerIDHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLQueryPlayerID.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "Connection.h"
#include "DatabaseManager.h"
#include "LCQueryResultPlayerID.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#include "Result.h"
#include "Statement.h"
#endif

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CLQueryPlayerIDHandler::execute(CLQueryPlayerID* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    Statement* pStmt = NULL;

    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectPlayerNameStmt(pConn, "SELECT Name FROM Player WHERE PlayerID = ?");
        selectPlayerNameStmt.bindString(1, pPacket->getPlayerID());
        Result* pResult = selectPlayerNameStmt.execute();

        LCQueryResultPlayerID lcQueryResultPlayerID;

        // cout << "Query Result : " << pPacket->getPlayerID() << " is ";

        lcQueryResultPlayerID.setPlayerID(pPacket->getPlayerID());

        if (pResult->getRowCount() == 0) {
            lcQueryResultPlayerID.setExist(false);

            // cout << "not ";

        } else {
            lcQueryResultPlayerID.setExist(true);
        }

        // cout << "exist..." << endl;

        pLoginPlayer->sendPacket(&lcQueryResultPlayerID);


        pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_REGISTER_PLAYER);

        SAFE_DELETE(pStmt);
        // delete pStmt;		// 2002.1.16 by sigi
    } catch (SQLQueryException& sqe) {
        SAFE_DELETE(pStmt);
        throw;
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
