//////////////////////////////////////////////////////////////////////
//
// Filename    : TheOneServer.h
// Written By  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __THE_ONE_SERVER_H__
#define __THE_ONE_SERVER_H__


#ifndef __THE_ONE_SERVER__
#define __THE_ONE_SERVER__
#endif

// include files
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class TheOneServer
//

//
//////////////////////////////////////////////////////////////////////

class TheOneServer {
public:
    // constructor
    TheOneServer() throw(Error);

    // destructor
    ~TheOneServer() throw(Error);

    // intialize game server
    void init() throw(Error);

    // start game server
    void start() throw(Error);

    // stop game server
    void stop() throw(Error);
};

// global variable declaration
extern TheOneServer* g_pTheOneServer;

#endif
