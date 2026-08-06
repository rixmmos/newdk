//////////////////////////////////////////////////////////////////////
//
// Filename    : ZoneGroupThread.h
// Written by  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __ZONE_THREAD_H__
#define __ZONE_THREAD_H__

// include files
#include "Exception.h"
#include "Thread.h"
#include "Types.h"
#include "ZoneGroup.h"

//////////////////////////////////////////////////////////////////////
//
// class ZoneGroupThread;
//



//
//////////////////////////////////////////////////////////////////////

class ZoneGroupThread : public Thread {
public:
    // constructor
    ZoneGroupThread(ZoneGroup* pZoneGroup);

    // destructor
    ~ZoneGroupThread();

    // main method
    void run();

    // get debug string
    string toString() const;

    // get thread's name
    string getName() const {
        return "ZoneGroupThread";
    }

    ZoneGroup* getZoneGroup() {
        return m_pZoneGroup;
    }

private:
    ZoneGroup* m_pZoneGroup;
};

#endif
