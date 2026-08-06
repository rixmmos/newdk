//////////////////////////////////////////////////////////////////////
//
// Thread.h
//
// by Reiot
//
//////////////////////////////////////////////////////////////////////
//
// POSIX Thread Class
//



//

//
// MDerivedThread * dt = new DerivedThread (...);
// dt->Start();
// delete dt;
//



//
// Thread * t = new DerivedThread (...);
// t->Start();
// delete t;
//

//
//////////////////////////////////////////////////////////////////////


#ifndef __THREAD_H__
#define __THREAD_H__


//////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////

#include "Exception.h"
#include "Types.h"
#include "pthreadAPI.h"


//////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////
class ThreadAttr;
class ThreadException;


//////////////////////////////////////////////////////////////////////
//
// class Thread
//
// POSIX Thread Class
//
//////////////////////////////////////////////////////////////////////

class Thread {
    //////////////////////////////////////////////////
    // constants
    //////////////////////////////////////////////////
public:
    enum ThreadStatus {
        READY,   
        RUNNING, 
        EXITING, 
        EXIT     
    };


    //////////////////////////////////////////////////
    // constructor and destructor
    //////////////////////////////////////////////////

public:
    // constructor
    Thread(ThreadAttr* attr = NULL);

    // destructor
    virtual ~Thread();


    //////////////////////////////////////////////////
    // public methods
    //////////////////////////////////////////////////

public:
    
    
    
    void start();

    
    
    virtual void stop();

    
    
    
    
    //
    
    
    
    //
    // ex> Thread t;
    //     Thread::Join ( t );
    static void join(const Thread& t);
    static void join(const Thread* t);
    static void join(const Thread& t, void* retval);
    static void join(const Thread* t, void* retval);

    
    void detach();

    
    
    
    
    //
    // ex> Thread::Exit();
    //     or
    //     Thread::Exit(retval);
    static void exit(void* retval = NULL);

    
    
    virtual void run() {};


    //////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////
public:
    // get current thread's tid
    static TID self();

    
    virtual string toString() const;

    // get thread identifier
    TID getTID() const {
        return m_TID;
    }

    // get/set thread's status
    ThreadStatus getStatus() const {
        return m_Status;
    }
    void setStatus(ThreadStatus status) {
        m_Status = status;
    }

    // get thread name
    virtual string getName() const {
        return "Thread";
    }


    //////////////////////////////////////////////////
    // data members
    //////////////////////////////////////////////////

private:
    // thread identifier variable
    TID m_TID;

    // thread-attribute object
    ThreadAttr* m_ThreadAttr;

    // thread status
    ThreadStatus m_Status;
};


//////////////////////////////////////////////////
// thread function used at pthread_create()
//////////////////////////////////////////////////
void* start_routine(void* derivedThread);

#endif
