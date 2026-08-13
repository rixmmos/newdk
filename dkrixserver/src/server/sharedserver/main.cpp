//////////////////////////////////////////////////////////////////////
//
// Filename    : main.cpp
// Written By  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

// include files
#include <stdlib.h>

#include <new>

#include <sys/resource.h>

#include "Exception.h"
#include "LogClient.h"
#include "Properties.h"
#include "SharedServer.h"
#include "StringStream.h"
#include "Types.h"

// A fatal OOM used to exit(0), so a supervisor read the crash as a clean,
// successful shutdown and would not restart or alert on it. abort() is what the
// gameserver's handleMemoryError() already does: it is the only correct ending
// for a new_handler that cannot free memory (returning would loop forever), and
// unlike exit() it does not run atexit handlers or static destructors -- code
// that would itself try to allocate on an already-exhausted heap.
void memoryError() {
    cerr << "==============================================================================" << endl;
    cerr << "CRITICAL ERROR! NOT ENOUGH MEMORY!" << endl;
    cerr << "==============================================================================" << endl;
    filelog("CriticalError.log", "CRITICAL ERROR! NOT ENOUGH MEMORY!");
    abort();
}

//////////////////////////////////////////////////////////////////////
//
// main()
//
//////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
    set_new_handler(memoryError);

    if (argc < 3) {
        cout << "Usage : sharedserver -f " << endl;
        exit(1);
    }


    string* Argv;

    Argv = new string[argc];
    for (int i = 0; i < argc; i++)
        Argv[i] = argv[i];


    try {
        if (Argv[1] != "-f") {
            throw Error("Usage : sharedserver -f ");
        }


        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);

        cout << g_pConfig->toString() << endl;

    } catch (Error& e) {
        cout << e.toString() << endl;
    }


    try {
        string LogServerIP = g_pConfig->getProperty("LogServerIP");
        int LogServerPort = g_pConfig->getPropertyInt("LogServerPort");
        g_pLogClient = new LogClient(LogServerIP, LogServerPort);
        LogClient::setLogLevel(g_pConfig->getPropertyInt("LogLevel"));

        log(LOG_SHAREDSERVER, "", "", "Shared Server Start");
    } catch (Throwable& t) {
        cout << t.toString() << endl;
    }

    //

    //
    try {
        struct rlimit rl;
        rl.rlim_cur = RLIM_INFINITY;
        rl.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE, &rl);


        g_pSharedServer = new SharedServer();


        g_pSharedServer->init();


        g_pSharedServer->start();
    } catch (Throwable& e) {
        ofstream ofile("../log/instant.log", ios::out);
        ofile << e.toString() << endl;
        ofile.close();


        log(LOG_SHAREDSERVER_ERROR, "", "", e.toString());


        cout << e.toString() << endl;


        g_pSharedServer->stop();
    }
}
