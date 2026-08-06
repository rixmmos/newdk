//////////////////////////////////////////////////////////////////////
//
// Filename    : SharedServer.h
// Written By  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __SHARED_SERVER_H__
#define __SHARED_SERVER_H__


#ifndef __SHARED_SERVER__
#define __SHARED_SERVER__
#endif

// include files
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class SharedServer
//

//
//////////////////////////////////////////////////////////////////////

class SharedServer {
public:
    // constructor
    SharedServer() throw(Error);

    // destructor
    ~SharedServer() throw(Error);

    // intialize game server
    void init() throw(Error);

    // start game server
    void start() throw(Error);

    // stop game server
    void stop() throw(Error);
};

// global variable declaration
extern SharedServer* g_pSharedServer;

#endif
