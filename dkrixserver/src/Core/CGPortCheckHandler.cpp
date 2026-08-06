//----------------------------------------------------------------------
//
// Filename    : CGPortCheckHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include <stdio.h>

#include "CGPortCheck.h"
#include "Properties.h"

#ifdef __GAME_SERVER__
#include "DB.h"

#endif

//----------------------------------------------------------------------
//
// CGPortCheckHander::execute()
//


//
//----------------------------------------------------------------------
void CGPortCheckHandler::execute(CGPortCheck* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        const string& host = pPacket->getHost();
    DWORD IP = inet_addr(host.c_str());
    uint port = pPacket->getPort();

    // cout << "CGPortCheck: [" << IP << "] " << host.c_str() << ":" << port << endl;

    Statement* pStmt = NULL;

    try {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        pStmt->executeQuery("INSERT IGNORE INTO UserIPInfo (Name, IP, Port, ServerID) VALUES ( '%s', %lu, %u, %d )",
                            pPacket->getPCName().c_str(), IP, port, g_pConfig->getPropertyInt("ServerID"));

        if (pStmt->getAffectedRowCount() == 0) {
            
            pStmt->executeQuery("UPDATE UserIPInfo Set IP=%lu, Port=%u WHERE Name='%s'", IP, port,
                                pPacket->getPCName().c_str());

            // log(LOG_CGCONNECT, pPacket->getPCName(), "", host);
        }

        SAFE_DELETE(pStmt);

    } catch (SQLQueryException&) {
         
        SAFE_DELETE(pStmt);
    }
#else
            cout
        << pPacket->toString() << endl;

#endif

    __END_DEBUG_EX __END_CATCH
}
