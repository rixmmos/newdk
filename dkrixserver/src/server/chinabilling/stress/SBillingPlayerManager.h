/////////////////////////////////////////////////////////////////////////////
//
// filename		: SBillingPlayerManager.h
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __SBILLING_PLAYER_MANAGER__
#define __SBILLING_PLAYER_MANAGER__

// include files
#include "CBillingInfo.h"
#include "Exception.h"
#include "Mutex.h"
#include "PayUser.h"
#include "Socket.h"
#include "Thread.h"
#include "Types.h"

class SBillingPlayer;

/////////////////////////////////////////////////////
// class SBillingPlayerManager
//

/////////////////////////////////////////////////////
class SBillingPlayerManager : public Thread {
public:
    SBillingPlayerManager() throw(Error);
    ~SBillingPlayerManager() throw(Error);

public:
    void init() throw(Error) {}
    void stop() throw(Error);
    void run() throw();

    
    bool sendLogin(PayUser* pPayUser, int i) throw(ProtocolException, Error);
    void sendIntervalValidation(int i) throw(ProtocolException, Error);
    bool sendMinusPoint(PayUser* pPayUser, int i) throw(ProtocolException, Error);
    void sendMinusMinute(PayUser* pPayUser, int i) throw(ProtocolException, Error);
    void sendLogout(PayUser* pPayUser, int i) throw(ProtocolException, Error);

    int getVersionNumber() const throw();
    int getMinusIntervalInt() const throw();
    string getMinusInterval() const throw();

private:
    SBillingPlayer** m_pSBillingPlayer;
    int m_SBillingPlayers;
    mutable Mutex* m_Mutex;
};

// global variable declaration
extern SBillingPlayerManager* g_pSBillingPlayerManager;

#endif
