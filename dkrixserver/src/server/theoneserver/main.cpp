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
#include <vector>

#include <sys/resource.h>
#include <sys/time.h>

#include "Exception.h"
#include "Properties.h"
#include "StringStream.h"
#include "TheOneServer.h"
#include "Types.h"

void memoryError() {
    cerr << "CRITICAL ERROR! NOT ENOUGH MEMORY!" << endl;
    exit(0);
}

//////////////////////////////////////////////////////////////////////
//
// main()
//
//////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
    
    set_new_handler(memoryError);
    cerr << "Processing Arguments..." << endl;

    if (argc < 3) {
        cout << "Usage : theoneserver -f  [-p port]" << endl;
        exit(1);
    }

    
    vector<string> Argv;
    Argv.reserve(argc);

    for (int i = 0; i < argc; i++)
        Argv.push_back(argv[i]);

    
    
    

    try {
        if (Argv[1] != "-f") {
            throw Error("Usage : loginserver -f  [-p port]");
        }

        cerr << "Making Properties.." << endl;

        
        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);

        cerr << "Making Properties Complete!" << endl;

        cout << g_pConfig->toString() << endl;

    } catch (Error& e) {
        cerr << e.toString() << endl;
        exit(1);
    }

    try {
        if (argc > 3) {
            if (argc < 5 || Argv[3] != "-p")
                throw Error("Usage : theoneserver -f  [-p port]");

            g_pConfig->setProperty("TheOneServerUDPPort", Argv[4]);
            cout << "TheOneServerUDPPort : " << g_pConfig->getProperty("TheOneServerUDPPort") << endl;
            ;
        }

    } catch (Error& e) {
        cerr << e.toString() << endl;
        exit(1);
    }

    cerr << "Processing Arguments Complete!" << endl;

    //
    
    //
    try {
        struct rlimit rl;
        rl.rlim_cur = RLIM_INFINITY;
        rl.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE, &rl);

        
        g_pTheOneServer = new TheOneServer();

        
        g_pTheOneServer->init();

        
        g_pTheOneServer->start();
    } catch (Throwable& e) {
        cerr << e.toString() << endl;

        
        
        g_pTheOneServer->stop();
    }
}
