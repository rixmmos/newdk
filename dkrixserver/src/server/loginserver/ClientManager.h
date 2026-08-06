//////////////////////////////////////////////////////////////////////
//
// Filename    : ClientManager.h
// Written by  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __LOGIN_CLIENT_MANAGER_H__
#define __LOGIN_CLIENT_MANAGER_H__

// include files
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class ClientManager;
//


//



//
// *NOTES*
//


//
//////////////////////////////////////////////////////////////////////

class ClientManager {
public:
    // constructor
    ClientManager() throw(Error);

    // destructor
    ~ClientManager() throw(Error);

    
    void init() throw(Error);

    
    void start() throw(Error);

    
    void stop() throw(Error);

    
    void run() throw(Error);
};

// external variable declaration
extern ClientManager* g_pClientManager;

#endif
