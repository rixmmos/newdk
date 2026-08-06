//////////////////////////////////////////////////////////////////////////////
// Filename    : CLAgreementHandler.cpp
// Written By  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLAgreement.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "LoginPlayer.h"
#endif

//////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
void CLAgreementHandler::execute(CLAgreement* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__
#ifdef __NETMARBLE_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
    Assert(pLoginPlayer != NULL);

    if (pPacket->isAgree()) {
        Statement* pStmt = NULL;

        BEGIN_DB {
            //----------------------------------------------------------------------
            
            //----------------------------------------------------------------------
            pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
            pStmt->executeQuery("DELETE FROM PrivateAgreementRemain WHERE PlayerID = '%s'",
                                pLoginPlayer->getID().c_str());

            
            pLoginPlayer->setAgree(true);

            SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)
    }

#endif
#endif

    __END_DEBUG_EX __END_CATCH
}
