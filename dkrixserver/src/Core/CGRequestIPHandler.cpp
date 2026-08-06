//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestIPHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestIP.h"

#ifdef __GAME_SERVER__
#include "Creature.h"
#include "DB.h"
#include "GCRequestFailed.h"
#include "GCRequestedIP.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestIPHandler::execute(CGRequestIP* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    try {
        
        Statement* pStmt = NULL;

        BEGIN_DB {
            pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
            Result* pResult =
                pStmt->executeQuery("SELECT IP, Port FROM UserIPInfo WHERE Name='%s'", pPacket->getName().c_str());

            if (pResult->getRowCount() == 0) {
                SAFE_DELETE(pStmt);
                throw NoSuchElementException(" ID IP .");
            } else {
                pResult->next();
                IP_t IP = pResult->getDWORD(1);
                uint Port = pResult->getDWORD(2);
                // cout << "Requested IP : " << IP	<< endl;

                GCRequestedIP gcRequestedIP;
                gcRequestedIP.setIP(IP);
                gcRequestedIP.setPort(Port);
                gcRequestedIP.setName(pPacket->getName().c_str());
                pPlayer->sendPacket(&gcRequestedIP);

                SAFE_DELETE(pStmt);
            }
        }
        END_DB(pStmt)
    }
    // catch (NoSuchElementException & nsee)
    catch (Throwable& t) {
        
        GCRequestFailed gcRequestFailed;
        gcRequestFailed.setCode(REQUEST_FAILED_IP);
        gcRequestFailed.setName(pPacket->getName());
        pPlayer->sendPacket(&gcRequestFailed);
    } catch (...) {
        GCRequestFailed gcRequestFailed;
        gcRequestFailed.setCode(REQUEST_FAILED_IP);
        gcRequestFailed.setName(pPacket->getName());
        pPlayer->sendPacket(&gcRequestFailed);
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
