
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


    // command-line parameter¸¦ string À¸·Î º¯È¯ÇÑ´Ù. ^^;
    string* Argv;

    Argv = new string[argc];
    for (int i = 0; i < argc; i++)
        Argv[i] = argv[i];

    // È¯°æ ÆÄÀÏÀ» ÀÐ¾îµéÀÎ´Ù.
    // ´Ü ½ÇÇà ÆÄÀÏÀº $VSHOME/bin¿¡, È¯°æ ÆÄÀÏÀº $VSHOME/conf ¿¡ Á¸ÀçÇØ¾ß ÇÑ´Ù.½
    // command line ¿¡¼­ È¯°æ ÆÄÀÏÀ» ÁöÁ¤ÇÒ ¼ö ÀÖµµ·Ï ÇÑ´Ù.

    //-------------------------------------------------------------------
    // Load config file
    //-------------------------------------------------------------------
    try {
        if (Argv[1] != "-f") {
            throw Error("Usage : gameserver -f È¯°æÆÄÀÏ");
        }

        // Ã¹¹øÂ° ÆÄ¶ó¹ÌÅÍ°¡ -f ÀÏ °æ¿ì, µÎ¹øÂ° ÆÄ¶ó¹ÌÅÍ´Â È¯°æÆÄÀÏÀÇ À§Ä¡°¡ µÈ´Ù.
        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);

        // cout << g_pConfig->toString() << endl;
    } catch (Throwable& e) {
        // Phase 13C follow-up: was `catch (Error&)` — missed IOException
        // and FileNotExistException (parallel class branch via Exception,
        // not Error). Mirrors the fix already in place on the three main
        // server submains (login/shared/game). Also changed `return 0`
        // to `return 1` so the shell sees a non-zero exit on config
        // failure instead of silent success.
        cout << "FATAL during config load: " << e.toString() << endl;
        return 1;
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
