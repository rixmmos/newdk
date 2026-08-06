//////////////////////////////////////////////////////////////////////
//
// Filename    : main.cpp
// Written By  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

// include files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <new>

#include <sys/resource.h>
#include <sys/time.h>

#include "Exception.h"
#include "LogClient.h"
#include "LoginServer.h"
#include "Properties.h"
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
        cout << "Usage : loginserver -f  [-p port]" << endl;
        exit(1);
    }

    
    string* Argv;

    Argv = new string[argc];
    for (int i = 0; i < argc; i++)
        Argv[i] = argv[i];

    
    
    

    try {
        if (Argv[1] != "-f") {
            throw Error("Usage : loginserver -f  [-p port]");
        }

        
        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);

        cout << g_pConfig->toString() << endl;

    } catch (Error& e) {
        cout << e.toString() << endl;
    }

    try {
        if (argc > 3) {
            if (argc < 5 || Argv[3] != "-i")
                throw Error("Usage : loginserver -f  [-i ID]");

            
            char sLoginServerPort[5], sLoginServerUDPPort[5], sLoginServerID[5];
            sprintf(sLoginServerPort, "%d", g_pConfig->getPropertyInt("LoginServerBasePort") + atoi(argv[4]));
            sprintf(sLoginServerUDPPort, "%d", g_pConfig->getPropertyInt("LoginServerBaseUDPPort") + atoi(argv[4]));
            sprintf(sLoginServerID, "%d", g_pConfig->getPropertyInt("LoginServerBaseID") + atoi(argv[4]));

            g_pConfig->setProperty("LoginServerPort", sLoginServerPort);
            g_pConfig->setProperty("LoginServerUDPPort", sLoginServerUDPPort);
            g_pConfig->setProperty("LoginServerID", sLoginServerID);

            cout << "LoginServerPort : " << sLoginServerPort << endl;
            cout << "LoginServerUDPPort : " << sLoginServerUDPPort << endl;
            cout << "LoginServerID : " << sLoginServerID << endl;
        }

    } catch (Error& e) {
        cout << e.toString() << endl;
    }


    
    
    
    
    

    try {
        string LogServerIP = g_pConfig->getProperty("LogServerIP");
        int LogServerPort = g_pConfig->getPropertyInt("LogServerPort");
        g_pLogClient = new LogClient(LogServerIP, LogServerPort);
        LogClient::setLogLevel(g_pConfig->getPropertyInt("LogLevel"));

        log(LOG_LOGINSERVER, "", "", "Login Server Start");
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

        
        g_pLoginServer = new LoginServer();

        
        g_pLoginServer->init();

        
        g_pLoginServer->start();
    } catch (Throwable& e) {
        
        ofstream ofile("../log/instant.log", ios::out);
        ofile << e.toString() << endl;
        ofile.close();

        
        
        log(LOG_LOGINSERVER_ERROR, "", "", e.toString());

        
        cout << e.toString() << endl;

        
        
        g_pLoginServer->stop();
    }
}
