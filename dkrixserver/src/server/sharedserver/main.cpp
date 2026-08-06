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

void memoryError() {
    cout << "CRITICAL ERROR! NOT ENOUGH MEMORY!" << endl;
    exit(0);
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
