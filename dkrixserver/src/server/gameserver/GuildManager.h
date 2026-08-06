//////////////////////////////////////////////////////////////////////////////
// Filename    : GuildManager.h

// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GUILDMANAGER_H__
#define __GUILDMANAGER_H__

#include <unordered_map>

#include "Assert.h"
#include "Exception.h"
#include "Mutex.h"
#include "Timeval.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class GuildManager


//
//////////////////////////////////////////////////////////////////////////////

class Guild;

typedef unordered_map<GuildID_t, Guild*> HashMapGuild;
typedef unordered_map<GuildID_t, Guild*>::iterator HashMapGuildItor;
typedef unordered_map<GuildID_t, Guild*>::const_iterator HashMapGuildConstItor;

#ifdef __SHARED_SERVER__
class SGGuildInfo;
#endif

class GCWaitGuildList;
class GCActiveGuildList;
class PlayerCreature;

class GuildManager {
    ///// Member methods /////

public: // constructor & destructor
    GuildManager();
    ~GuildManager();


public: // initializing related methods
    void init();
    void load();


public: // memory related methods
    void addGuild(Guild* pGuild);
    void addGuild_NOBLOCKED(Guild* pGuild);
    void deleteGuild(GuildID_t id);
    Guild* getGuild(GuildID_t id);
    Guild* getGuild_NOBLOCKED(GuildID_t id);

    void clear();
    void clear_NOBLOCKED();


public: // misc methods
    ushort getGuildSize() const {
        return m_Guilds.size();
    }
    HashMapGuild& getGuilds() {
        return m_Guilds;
    }
    const HashMapGuild& getGuilds_const() const {
        return m_Guilds;
    }

#ifdef __SHARED_SERVER__
public:
    void makeSGGuildInfo(SGGuildInfo& sgGuildInfo);
#endif

    void makeWaitGuildList(GCWaitGuildList& gcWaitGuildList, GuildRace_t race);
    void makeActiveGuildList(GCActiveGuildList& gcWaitGuildList, GuildRace_t race);

public:
    void lock() {
        m_Mutex.lock();
    }
    void unlock() {
        m_Mutex.unlock();
    }


public:
    void heartbeat();

public:
    bool isGuildMaster(GuildID_t guildID, PlayerCreature* pPC);

    string getGuildName(GuildID_t guildID);

    
    bool hasCastle(GuildID_t guildID);
    bool hasCastle(GuildID_t guildID, ServerID_t& serverID, ZoneID_t& zoneID);

    
    bool hasWarSchedule(GuildID_t guildID);

    
    bool hasActiveWar(GuildID_t guidlID);

public: // debug
    string toString(void) const;


    ///// Member data /////

protected:
    unordered_map<GuildID_t, Guild*> m_Guilds; 

    Timeval m_WaitMemberClearTime; 

    // mutex
    mutable Mutex m_Mutex;
};

extern GuildManager* g_pGuildManager;

#endif // __GUILDINFO_H__
