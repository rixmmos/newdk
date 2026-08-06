//////////////////////////////////////////////////////////////////////
//
// Filename    : HeartbeatManager.h
// Written by  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __HEARTBEAT_MANAGER_H__
#define __HEARTBEAT_MANAGER_H__

// include files
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class HeartbeatManager;
//


//



//
//////////////////////////////////////////////////////////////////////

class HeartbeatManager {
public:
    // constructor
    HeartbeatManager() throw(Error);

    // destructor
    ~HeartbeatManager() throw(Error);

    
    void init() throw(Error);

    
    void start() throw(Error);

    
    void stop() throw(Error);

    
    void run() throw(Error);
};

// external variable declaration
extern HeartbeatManager* g_pHeartbeatManager;

#endif
