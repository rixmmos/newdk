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
#include "PreparedStatement.h"

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
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement insertUserIPStmt(
            pConn, "INSERT IGNORE INTO UserIPInfo (Name, IP, Port, ServerID) VALUES ( ?, ?, ?, ? )");
        insertUserIPStmt.bindString(1, pPacket->getPCName());
        insertUserIPStmt.bindULong(2, IP);
        insertUserIPStmt.bindUInt(3, port);
        insertUserIPStmt.bindInt(4, g_pConfig->getPropertyInt("ServerID"));
        insertUserIPStmt.execute();

        if (insertUserIPStmt.getAffectedRowCount() == 0) {
            PreparedStatement updateUserIPStmt(pConn, "UPDATE UserIPInfo Set IP=?, Port=? WHERE Name=?");
            updateUserIPStmt.bindULong(1, IP);
            updateUserIPStmt.bindUInt(2, port);
            updateUserIPStmt.bindString(3, pPacket->getPCName());
            updateUserIPStmt.execute();

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
