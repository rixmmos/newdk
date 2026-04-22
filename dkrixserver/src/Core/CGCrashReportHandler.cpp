//////////////////////////////////////////////////////////////////////////////
// Filename    : CGCrashReportHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGCrashReport.h"

#ifdef __GAME_SERVER__
#include "DB.h"
#include "GamePlayer.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGCrashReportHandler::execute(CGCrashReport* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    Creature* pCreature = pGamePlayer->getCreature();

    Statement* pStmt = NULL;

    try {
        BEGIN_DB {
            Connection* pConnection = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement stmt(
                pConnection,
                "INSERT INTO CrashReportLog (PlayerID, Name, ReportTime, ExecutableTime, Version, Address, Message, "
                "OS, CallStack) VALUES (?, ?, now(), ?, ?, ?, ?, ?, ?)");
            stmt.bindString(1, pGamePlayer->getID());
            stmt.bindString(2, pCreature->getName());
            stmt.bindString(3, pPacket->getExecutableTime());
            stmt.bindUInt(4, pPacket->getVersion());
            stmt.bindString(5, pPacket->getAddress());
            stmt.bindString(6, pPacket->getMessage());
            stmt.bindString(7, pPacket->getOS());
            stmt.bindString(8, pPacket->getCallStack());
            stmt.execute();

            SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)
        // ė„ź°€ ģ¯´ģķ•ź±° ė‚ ė¦¬ė©´ ė¬´ģ‹ķ•ģ˛
    } catch (...) {
        filelog("CrashReport.log", "%s", pPacket->toString().c_str());
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
