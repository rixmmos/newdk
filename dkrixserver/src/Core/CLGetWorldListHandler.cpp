//////////////////////////////////////////////////////////////////////////////
// Filename    : CLGetWorldListHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLGetWorldList.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "GameWorldInfoManager.h"
#include "LCWorldList.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#include "WorldInfo.h"
#endif

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void CLGetWorldListHandler::execute(CLGetWorldList* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);
    // cout << "Start execute" << endl;

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    try {
        int Num = g_pGameWorldInfoManager->getSize();

        // cout << "WorldNum : " << Num << endl;

        // Num + 1, not Num: both loops below are 1-based and run to Num
        // inclusive (world IDs start at 1), so sizing this to Num made
        // aWorldInfo[Num] a write and then a read one element past the end of a
        // stack VLA. AddressSanitizer reports it as a dynamic-stack-buffer-
        // overflow and aborts the loginserver on the first CLGetWorldList --
        // i.e. every login. Slot 0 stays unused, which is what the 1-based
        // indexing already assumed. Same off-by-one shape as Bug 18-E.
        WorldInfo* aWorldInfo[Num + 1];

        for (int i = 1; i < Num + 1; i++) {
            WorldInfo* pWorldInfo = new WorldInfo();
            GameWorldInfo* pGameWorldInfo = g_pGameWorldInfoManager->getGameWorldInfo(i);
            pWorldInfo->setID(pGameWorldInfo->getID());
            pWorldInfo->setName(pGameWorldInfo->getName());

            // by bezz. 2002.12.20
            pWorldInfo->setStat(pGameWorldInfo->getStatus());

            aWorldInfo[i] = pWorldInfo;


            // if (i==2) pWorldInfo->setStat(WORLD_CLOSE);

            // cout << "AddWorld : " << pWorldInfo->getName() << endl;
        }

        LCWorldList lcWorldList;

        Statement* pStmt = NULL;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            PreparedStatement selectPlayerWorldIDStmt(pConn, "SELECT CurrentWorldID FROM Player where PlayerID=?");
            selectPlayerWorldIDStmt.bindString(1, pLoginPlayer->getID());
            Result* pResult = selectPlayerWorldIDStmt.execute();

            if (pResult->next()) {
                lcWorldList.setCurrentWorldID(pResult->getInt(1));
            }

            SAFE_DELETE(pStmt); // by sigi
        }
        END_DB(pStmt) // by sigi

        for (int k = 1; k < Num + 1; k++) {
            lcWorldList.addListElement(aWorldInfo[k]);
        }

        pLoginPlayer->sendPacket(&lcWorldList);

        //		pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);
    } catch (Throwable& t) {
        // cout << t.toString() << endl;
    }
    // cout << "End execute" << endl;

#endif

    __END_DEBUG_EX __END_CATCH
}
