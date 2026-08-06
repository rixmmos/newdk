//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginServer.h
// Written By  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__


#ifndef __LOGIN_SERVER__
#define __LOGIN_SERVER__
#endif

// include files
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class LoginServer
//

//
//////////////////////////////////////////////////////////////////////

class LoginServer {
public:
    // constructor
    LoginServer() throw(Error);

    // destructor
    ~LoginServer() throw(Error);

    // intialize game server
    void init() throw(Error);

    // start game server
    void start() throw(Error);

    // stop game server
    void stop() throw(Error);
};

// global variable declaration
extern LoginServer* g_pLoginServer;

#endif
