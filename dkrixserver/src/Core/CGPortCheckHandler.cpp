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
// ź²ģ˛„ ģ„ė²„ź°€ ėź·øģ¯ø ģ„ė²„ėė¶€ķ„° CGPortCheck ķØķ‚·ģ¯„ ė°›ź² ėė©´,
// ConnectionInfoė¼ ģė ģ¶”ź°€ķ•ź² ėė‹¤.
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
        Connection* pConnection = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertStmt(
            pConnection,
            "INSERT IGNORE INTO UserIPInfo (Name, IP, Port, ServerID) VALUES (?, ?, ?, ?)");
        insertStmt.bindString(1, pPacket->getPCName());
        insertStmt.bindULong(2, (unsigned long long)IP);
        insertStmt.bindUInt(3, port);
        insertStmt.bindInt(4, g_pConfig->getPropertyInt("ServerID"));
        insertStmt.execute();

        if (insertStmt.getAffectedRowCount() == 0) {
            // ė‹¤ģ‹ ķ•ė² ģ‹ė¸„
            PreparedStatement updateStmt(pConnection, "UPDATE UserIPInfo SET IP = ?, Port = ? WHERE Name = ?");
            updateStmt.bindULong(1, (unsigned long long)IP);
            updateStmt.bindUInt(2, port);
            updateStmt.bindString(3, pPacket->getPCName());
            updateStmt.execute();

            // log(LOG_CGCONNECT, pPacket->getPCName(), "", host);
        }

    } catch (SQLQueryException&) {
        /*
        try {
            // ė‹¤ģ‹ ķ•ė² ģ‹ė¸„
            pStmt->executeQuery( "UPDATE UserIPInfo Set IP=%ld, Port=%d WHERE Name='%s'",
                                    IP,
                                    port,
                                    pPacket->getPCName().c_str());

            //log(LOG_CGCONNECT, pPacket->getPCName(), "", host);

            SAFE_DELETE(pStmt);

        } catch (SQLQueryException & sqe) {	//se) {

            SAFE_DELETE(pStmt);
            // ė¬´ģ‹ķ•ė‹¤.
            //throw ProtocolException("Duplicated IPInfo");
        }
        */
        SAFE_DELETE(pStmt);
    }
#else
            cout
        << pPacket->toString() << endl;

#endif

    __END_DEBUG_EX __END_CATCH
}
