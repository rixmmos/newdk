//////////////////////////////////////////////////////////////////////////////
// Filename    : CLQueryCharacterNameHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLQueryCharacterName.h"

#ifdef __LOGIN_SERVER__
#include "Assert.h"
#include "Connection.h"
#include "DatabaseManager.h"
#include "GameWorldInfoManager.h"
#include "LCQueryResultCharacterName.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#include "Result.h"
#endif

bool isAvailableID(const char* pID);

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CLQueryCharacterNameHandler::execute(CLQueryCharacterName* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    WorldID_t WorldID = pLoginPlayer->getWorldID();

    Assert(WorldID <= g_pGameWorldInfoManager->getSize());

    PreparedStatement nameStmt(g_pDatabaseManager->getConnection(WorldID), "SELECT Name FROM Slayer WHERE Name = ?");
    nameStmt.bindString(1, pPacket->getCharacterName());
    Result* pResult = nameStmt.execute();

    LCQueryResultCharacterName lcQueryResultCharacterName;

    // cout << "Query Result : " << pPacket->getCharacterName() << " is ";

    lcQueryResultCharacterName.setCharacterName(pPacket->getCharacterName());

    if (pResult->getRowCount() == 0) {
        lcQueryResultCharacterName.setExist(false);

        // cout << "not ";

    } else {
        lcQueryResultCharacterName.setExist(true);
    }

    //--------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------
    // by sigi
    if (!isAvailableID(pPacket->getCharacterName().c_str())) {
        lcQueryResultCharacterName.setExist(true);
    }


    // cout << "exist..." << endl;

    pLoginPlayer->sendPacket(&lcQueryResultCharacterName);


    pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);

#endif

    __END_DEBUG_EX __END_CATCH
}
