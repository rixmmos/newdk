
// Billing Test

#include <iostream>

#include "BillingPlayerManager.h"
#include "CommonBillingPacket.h"
#include "DatabaseManager.h"
#include "Properties.h"

int main(int argc, char* argv[]) {
    CommonBillingPacket cbp;
    cbp.test();
    cout << cbp.toString().c_str() << endl;


    
    string* Argv;

    Argv = new string[argc];
    for (int i = 0; i < argc; i++)
        Argv[i] = argv[i];

    
    
    

    //-------------------------------------------------------------------
    // Load config file
    //-------------------------------------------------------------------
    try {
        if (Argv[1] != "-f") {
            throw Error("Usage : gameserver -f ");
        }

        
        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);

        // cout << g_pConfig->toString() << endl;
    } catch (Error& e) {
        cout << e.toString() << endl;
        return 0;
    }

    //-------------------------------------------------------------------
    // DatabaseManager
    //-------------------------------------------------------------------
    g_pDatabaseManager = new DatabaseManager();

    //-------------------------------------------------------------------
    // BillingPlayerManager
    //-------------------------------------------------------------------
    g_pBillingPlayerManager = new BillingPlayerManager();

    try {
        g_pBillingPlayerManager->init();
        g_pBillingPlayerManager->start();

    } catch (Throwable& t) {
        cout << t.toString().c_str() << endl;
    }

    //-------------------------------------------------------------------
    // Main Loop
    //-------------------------------------------------------------------
    string cmd;
    while (1) {
        cout << "command> ";
        cin >> cmd;

        if (cmd == "quit")
            break;
    }

    delete g_pBillingPlayerManager;
}
