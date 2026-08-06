//////////////////////////////////////////////////////////////////////////////
// Filename    : CLVersionCheckHandler.cpp
// Written By  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLVersionCheck.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "LCVersionCheckError.h"
#include "LCVersionCheckOK.h"
#include "LoginPlayer.h"
#endif

//////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
void CLVersionCheckHandler::execute(CLVersionCheck* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        //----------------------------------------------------------------------
        // *CAUTION*
        
        //----------------------------------------------------------------------

        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT Version FROM ClientVersion");

        
        
        
        
        Assert(pResult->getRowCount() != 0);
        pResult->next();

        /* Version info intentionally unused; simply acknowledge latest record */
        pResult->getInt(1);
        pPacket->getVersion();

         

        LCVersionCheckOK lcVersionCheckOK;
        pLoginPlayer->sendPacket(&lcVersionCheckOK);

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

#endif

    __END_DEBUG_EX __END_CATCH
}
