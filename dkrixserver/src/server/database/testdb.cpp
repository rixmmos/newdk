#include <iostream>

#include "Connection.h"
#include "Exception.h"
#include "LogManager.h"
#include "Properties.h"
#include "Result.h"
#include "Statement.h"
#include "Types.h"

int main() {
    try {
        g_pConfig = new Properties();
        g_pConfig->load("/home/crazydog/Works/VampireSlayers/conf/gameserver.conf");

        g_pLogManager = new LogManager();
        g_pLogManager->start();

        Connection* pConn = new Connection("localhost", "PLAYER", "choon", "choon", 7777);

        Statement* pStmt = pConn->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT PID, ID, Name, Passwd FROM Player");
        cout << "SELECT PID, ID, Name, Passwd FROM Player" << endl;

        while (pResult->next()) {
            cout << "Player(PID:" << pResult->getInt(1) << " ID:" << pResult->getString(2)
                 << ",Name:" << pResult->getString(3) << ",Password:" << pResult->getString(4) << ")" << endl;
        }

        
        
        pResult = pStmt->executeQuery("SELECT ID, Name, Passwd FROM Player WHERE id = 11");
        cout << "SELECT ID, Name, Passwd FROM Player WHERE id = 11" << endl;

        
        cout << "#Rows : " << pResult->getRowCount() << endl;
        cout << "#Cols : " << pResult->getFieldCount() << endl;

        while (pResult->next()) {
            cout << "Player(PID:" << pResult->getInt(1) << " ID:" << pResult->getString(2)
                 << ",Name:" << pResult->getString(3) << ",Password:" << pResult->getString(4) << ")" << endl;
        }

        delete pStmt;
        delete pConn;

    } catch (Throwable& t) {
        cout << t.toString() << endl;
    }
}
