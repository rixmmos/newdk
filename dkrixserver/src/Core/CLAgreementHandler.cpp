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
#include "PreparedStatement.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// ģ¯´ ķØķ‚·ģ¯€ ķ´ė¯¼ģ¯´ģ–øķøź°€ ģ•„ģ¯´ė””ģ™€ ķØģ¤ģ›ė“ė¼ ģ•”ķøķ™”ķ•´ģ„
// ėź·øģ¯ø ģ„ė²„ė ģ „ģ†ķ•ė‹¤. ėź·øģ¯ø ģ„ė²„ė” ģ¯´ ķØķ‚·ģ¯„ ė°›ģ•„ģ„
// ķ”ė ģ¯´ģ–´ģ¯ ģ•„ģ¯´ė””ģ™€ ķØģ¤ģ›ė“ź°€ ģ •ķ™•ķ•ģ§€ DBėė¶€ķ„° ģ¯½ģ–´ģ„
// ė¹„źµķ• ķ›„, ėź·øģ¯øģ¯ ģ„±ź³µ ģ—¬ė¶€ė¼ ģ „ģ†ķ•ė‹¤.
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
            // ė„·ė§ėø” ģ•½ź´€ ėÆøė¸™ģ¯ ė¦¬ģ¤ķøģ—ģ„ ģ‚­ģ 
            //----------------------------------------------------------------------
            Connection* pConnection = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement stmt(pConnection, "DELETE FROM PrivateAgreementRemain WHERE PlayerID = ?");
            stmt.bindString(1, pLoginPlayer->getID());
            stmt.execute();

            // ė‹¤ģ¯ ė‹Øź³„ė ģ§„ķ–‰ķ•  ģ ģ˛ź² ģ„¤ģ •
            pLoginPlayer->setAgree(true);

            SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)
    }

#endif
#endif

    __END_DEBUG_EX __END_CATCH
}
