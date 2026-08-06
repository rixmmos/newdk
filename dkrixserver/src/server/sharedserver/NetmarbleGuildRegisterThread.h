/////////////////////////////////////////////////////////////////////
// Filename		: NetmarbleGuildRegisterThread.h
// Written by	: bezz@darkeden.com
// Description	:
/////////////////////////////////////////////////////////////////////

#ifndef __NETMARBLE_GUILD_REGISTER_THREAD_H__
#define __NETMARBLE_GUILD_REGISTER_THREAD_H__

#include <queue>

#include "Exception.h"
#include "Mutex.h"
#include "Thread.h"
#include "Types.h"


/////////////////////////////////////////////////////////////////////
// class NetmarbleGuildRegisterThread
/////////////////////////////////////////////////////////////////////
class NetmarbleGuildRegisterThread : public Thread {
public:
    // type definition
    typedef queue<GuildID_t> QueueGuildID;

public:
    // constructor & destructor
    NetmarbleGuildRegisterThread() throw(Error);
    ~NetmarbleGuildRegisterThread() throw(Error);

public:
    // initialize
    void init() throw(Error);

    // thread main loop
    void run() throw(Error);

public:
    
    void pushGuildID(GuildID_t guildID);

    
    void registerGuild();

private:
    // Guild ID list for register
    QueueGuildID m_GuildIDs;

    // mutex
    mutable Mutex m_Mutex;
};

// external variable declaration
extern NetmarbleGuildRegisterThread* g_pNetmarbleGuildRegisterThread;

#endif
