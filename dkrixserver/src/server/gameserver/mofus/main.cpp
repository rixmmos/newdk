#include <iostream.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/time.h>

#include "MPacketManager.h"
#include "MPlayerManager.h"
#include "Properties.h"
#include "StringParser.h"


int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "mofus -f conf" << endl;
        return 1;
    }

    struct rlimit rl;
    rl.rlim_cur = RLIM_INFINITY;
    rl.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &rl);

    string* Argv = new string[argc];
    for (int i = 0; i < argc; ++i) {
        Argv[i] = argv[i];
    }

    try {
        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);
    } catch (Throwable& e) {
        // Phase 13C follow-up: was `catch (Error&)` — missed IOException
        // and FileNotExistException (parallel class branch via Exception,
        // not Error). Mirrors the fix already in place on the three main
        // server submains (login/shared/game) and billing/main.cpp.
        cout << "FATAL during config load: " << e.toString() << endl;
        return 1;
    }

    g_pMPacketManager = new MPacketManager();
    g_pMPlayerManager = new MPlayerManager();

    g_pMPacketManager->init();
    g_pMPlayerManager->init();

    g_pMPlayerManager->start();

    while (1) {
        char command[256];
        cin >> command;
        string sCommand(command);
        StringParser parser(sCommand);

        string action = parser.getPart(0);

        if (action == "ui") {
            g_pMPlayerManager->addJob(parser.getPart(1), parser.getPart(2), parser.getPart(3));
        }

        usleep(100);
    }
}
