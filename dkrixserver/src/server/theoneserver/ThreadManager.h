//--------------------------------------------------------------------------------
//
// Filename    : ThreadManager.h
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __THREAD_MANAGER_H__
#define __THREAD_MANAGER_H__

// include files
#include "Exception.h"
#include "Types.h"

// forward declaration
class ThreadPool;


//////////////////////////////////////////////////////////////////////
//
// class ThreadManager
//


//



//
//////////////////////////////////////////////////////////////////////

class ThreadManager {
public:
    // constructor
    ThreadManager() throw(Error);

    // destructor
    ~ThreadManager() throw(Error);


public:
    
    void init() throw(Error);

    // activate sub thread pools
    
    void start() throw(Error);

    // deactivate sub thread pools
    
    void stop() throw(Error);

    // #ifdef __NO_COMBAT__
    ThreadPool* getThreadPool() {
        return m_pZoneGroupThreadPool;
    } 
    // #endif

private:
    
    ThreadPool* m_pZoneGroupThreadPool;
};


// global variable declaration
extern ThreadManager* g_pThreadManager;

#endif
