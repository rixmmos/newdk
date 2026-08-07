//////////////////////////////////////////////////////////////////////////////
// Filename		: Guild.h
// Written by	: bezz
// Description	:
//////////////////////////////////////////////////////////////////////////////

#ifndef __GUILD_H__
#define __GUILD_H__

#include <list>

#include <unordered_map>

#include "Assert.h"
#include "Exception.h"
#include "Mutex.h"
#include "Types.h"
#include "VSDateTime.h"

#ifdef __SHARED_SERVER__
class GuildInfo2;
#endif

class GuildInfo;
class GCGuildMemberList;

//////////////////////////////////////////////////////////////////////////////
// class GuildMember

//
//////////////////////////////////////////////////////////////////////////////

#ifdef __CHINA_SERVER__
const Gold_t REQUIRE_SLAYER_MASTER_GOLD = 50000000;
const Gold_t REQUIRE_VAMPIRE_MASTER_GOLD = 50000000;
const Gold_t REQUIRE_OUSTERS_MASTER_GOLD = 50000000;

const Gold_t REQUIRE_SLAYER_SUBMASTER_GOLD = 25000000;
const Gold_t REQUIRE_VAMPIRE_SUBMASTER_GOLD = 25000000;
const Gold_t REQUIRE_OUSTERS_SUBMASTER_GOLD = 25000000;

const Gold_t RETURN_SLAYER_MASTER_GOLD = 0;
const Gold_t RETURN_VAMPIRE_MASTER_GOLD = 0;
const Gold_t RETURN_OUSTERS_MASTER_GOLD = 0;

const Gold_t RETURN_SLAYER_SUBMASTER_GOLD = 0;
const Gold_t RETURN_VAMPIRE_SUBMASTER_GOLD = 0;
const Gold_t RETURN_OUSTERS_SUBMASTER_GOLD = 0;

const Fame_t REQUIRE_SLAYER_MASTER_FAME[SKILL_DOMAIN_ETC] = {
    500000, // SKILL_DOMAIN_BLADE
    500000, // SKILL_DOMAIN_SWORD
    500000, // SKILL_DOMAIN_GUN
    100000, // SKILL_DOMAIN_HEAL
    150000  // SKILL_DOMAIN_ENCHANT
};

const Fame_t REQUIRE_SLAYER_SUBMASTER_FAME[SKILL_DOMAIN_ETC] = {
    400000, // SKILL_DOMAIN_BLADE
    400000, // SKILL_DOMAIN_SWORD
    400000, // SKILL_DOMAIN_GUN
    75000,  // SKILL_DOMAIN_HEAL
    125000  // SKILL_DOMAIN_ENCHANT
};

#else
const Gold_t REQUIRE_SLAYER_MASTER_GOLD = 10000000;
const Gold_t REQUIRE_VAMPIRE_MASTER_GOLD = 10000000;
const Gold_t REQUIRE_OUSTERS_MASTER_GOLD = 10000000;

const Gold_t REQUIRE_SLAYER_SUBMASTER_GOLD = 0;
const Gold_t REQUIRE_VAMPIRE_SUBMASTER_GOLD = 0;
const Gold_t REQUIRE_OUSTERS_SUBMASTER_GOLD = 0;

const Gold_t RETURN_SLAYER_MASTER_GOLD = 9000000;
const Gold_t RETURN_VAMPIRE_MASTER_GOLD = 9000000;
const Gold_t RETURN_OUSTERS_MASTER_GOLD = 9000000;

const Gold_t RETURN_SLAYER_SUBMASTER_GOLD = 0;
const Gold_t RETURN_VAMPIRE_SUBMASTER_GOLD = 0;
const Gold_t RETURN_OUSTERS_SUBMASTER_GOLD = 0;

const Fame_t REQUIRE_SLAYER_MASTER_FAME[SKILL_DOMAIN_ETC] = {
    100000, // SKILL_DOMAIN_BLADE
    100000, // SKILL_DOMAIN_SWORD
    100000, // SKILL_DOMAIN_GUN
    20000,  // SKILL_DOMAIN_HEAL
    30000   // SKILL_DOMAIN_ENCHANT
};

const Fame_t REQUIRE_SLAYER_SUBMASTER_FAME[SKILL_DOMAIN_ETC] = {
    0, // SKILL_DOMAIN_BLADE
    0, // SKILL_DOMAIN_SWORD
    0, // SKILL_DOMAIN_GUN
    0, // SKILL_DOMAIN_HEAL
    0  // SKILL_DOMAIN_ENCHANT
};

#endif

const SkillLevel_t REQUIRE_SLAYER_MASTER_SKILL_DOMAIN_LEVEL = 50;
const Level_t REQUIRE_VAMPIRE_MASTER_LEVEL = 50;
const Level_t REQUIRE_OUSTERS_MASTER_LEVEL = 50;

const SkillLevel_t REQUIRE_SLAYER_SUBMASTER_SKILL_DOMAIN_LEVEL = 30;
const Level_t REQUIRE_VAMPIRE_SUBMASTER_LEVEL = 40;
const Level_t REQUIRE_OUSTERS_SUBMASTER_LEVEL = 40;

class GuildMember {
public:
    GuildMember();

    ///// Member constants /////

public:
    enum GuildRank {
        GUILDMEMBER_RANK_NORMAL = 0,
        GUILDMEMBER_RANK_MASTER,
        GUILDMEMBER_RANK_SUBMASTER,
        GUILDMEMBER_RANK_WAIT,
        GUILDMEMBER_RANK_DENY,
        GUILDMEMBER_RANK_LEAVE,

        GUILDMEMBER_RANK_MAX
    };


public: // DB methods
    void create();
    bool load();
    void save();
    void destroy();
    void expire();
    void leave();

    void saveIntro(const string& intro);
    string getIntro() const;


public: // identity methods
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t guildID) {
        m_GuildID = guildID;
    }

    string getName() const {
        return m_Name;
    }
    void setName(const string& name) {
        m_Name = name;
    }

    GuildMemberRank_t getRank() const {
        return m_Rank;
    }
    void setRank(GuildMemberRank_t rank);

    bool getLogOn() const {
        return m_bLogOn;
    }
    void setLogOn(bool logOn) {
        m_bLogOn = logOn;
    }

    ServerID_t getServerID() const {
        return m_ServerID;
    }
    void setServerID(ServerID_t ServerID) {
        m_ServerID = ServerID;
    }

    string getRequestDateTime() const;
    void setRequestDateTime(const VSDateTime& vsdatetime) {
        m_RequestDateTime = vsdatetime;
    }
    void setRequestDateTime(const string& rdatetime);
    bool isRequestDateTimeOut(const VSDateTime& currentDateTime) const;

public: // debug
    string toString() const;


    ///// operator overloadgin /////

public:
    GuildMember& operator=(GuildMember& Member);


    ///// Member data /////

protected:
    GuildID_t m_GuildID;
    string m_Name;
    GuildMemberRank_t m_Rank;
    VSDateTime m_RequestDateTime;
    bool m_bLogOn;
    ServerID_t m_ServerID;
};


//////////////////////////////////////////////////////////////////////////////
// class Guild

//

// ----------------------------------------
// GuildID            INT
// GuildName          VARCHAR(20)
// GuildType          TINYINT
// GuildGold          INT
// ----------------------------------------
//////////////////////////////////////////////////////////////////////////////

#define MAX_GUILDMEMBER_ACTIVE_COUNT 100
#define MIN_GUILDMEMBER_COUNT 3
#define MAX_GUILDMEMBER_WAIT_COUNT 15

typedef unordered_map<string, GuildMember*> HashMapGuildMember;
typedef HashMapGuildMember::iterator HashMapGuildMemberItor;
typedef HashMapGuildMember::const_iterator HashMapGuildMemberConstItor;

class Guild {
    ///// Member constants /////

public:
    enum GuildTypes {
        GUILD_TYPE_NORMAL = 0,
        GUILD_TYPE_JUDGE,
        GUILD_TYPE_ASSASSIN,

        GUILD_TYPE_MAX
    };

    enum GuildState {
        GUILD_STATE_ACTIVE = 0,
        GUILD_STATE_WAIT,
        GUILD_STATE_CANCEL,
        GUILD_STATE_BROKEN,

        GUILD_STATE_MAX
    };

    enum GuildRace {
        GUILD_RACE_SLAYER = 0,
        GUILD_RACE_VAMPIRE,
        GUILD_RACE_OUSTERS,

        GUILD_RACE_MAX
    };

public: // constructor & destructor
    Guild();
    virtual ~Guild();


public: // DB methods
    void create();
    bool load();
    void save();
    void destroy();


public: // identity methods
    GuildID_t getID() const {
        return m_ID;
    }
    void setID(GuildID_t id) {
        m_ID = id;
    }

    string getName() const {
        return m_Name;
    }
    void setName(const string& name) {
        m_Name = name;
    }

    GuildType_t getType() const {
        return m_Type;
    }
    void setType(GuildType_t type) {
        m_Type = type;
    }

    GuildRace_t getRace() const {
        return m_Race;
    }
    void setRace(GuildRace_t race) {
        m_Race = race;
    }

    GuildState_t getState() const {
        return m_State;
    }
    void setState(GuildState_t state) {
        m_State = state;
    }

    ServerGroupID_t getServerGroupID() const {
        return m_ServerGroupID;
    }
    void setServerGroupID(ServerGroupID_t serverGroupID) {
        m_ServerGroupID = serverGroupID;
    }

    ZoneID_t getZoneID() const {
        return m_ZoneID;
    }
    void setZoneID(ZoneID_t zoneID) {
        m_ZoneID = zoneID;
    }

    string getMaster() const {
        return m_Master;
    }
    void setMaster(const string& master) {
        m_Master = master;
    }

    string getDate() const {
        return m_Date;
    }
    void setDate(const string& Date) {
        m_Date = Date;
    }

    string getIntro() const {
        return m_Intro;
    }
    void setIntro(const string& intro) {
        m_Intro = intro;
    }

#ifdef __SHARED_SERVER__
    void saveIntro(const string& intro);
    void tinysave(const char* field) const;
    void saveCount() const;
#endif


    ///// GuildMember get/add/delete/modify /////
    GuildMember* getMember(const string& name) const;
    GuildMember* getMember_NOLOCKED(const string& name) const;
    void addMember(GuildMember* pMember);
    void deleteMember(const string& name);
    void modifyMember(GuildMember& Member);

    void modifyMemberRank(const string& name, GuildMemberRank_t rank);

    HashMapGuildMember& getMembers() {
        return m_Members;
    }

    int getActiveMemberCount() const {
        return m_ActiveMemberCount;
    }
    int getWaitMemberCount() const {
        return m_WaitMemberCount;
    }

#ifdef __GAME_SERVER__
    void addCurrentMember(const string& name);
    void deleteCurrentMember(const string& name);
    list<string> getCurrentMembers();
#endif

#ifdef __SHARED_SERVER__
    void makeInfo(GuildInfo2* pGulidInfo);
#endif

    void makeInfo(GuildInfo* pGuildInfo);
    void makeMemberInfo(GCGuildMemberList& gcGuildMemberList);


public: // static
    static GuildID_t getMaxGuildID() {
        return m_MaxGuildID;
    }
    static void setMaxGuildID(GuildID_t id) {
        m_MaxGuildID = id;
    }

    static ZoneID_t getMaxSlayerZoneID() {
        return m_MaxSlayerZoneID;
    }
    static void setMaxSlayerZoneID(ZoneID_t zoneID) {
        m_MaxSlayerZoneID = zoneID;
    }

    static ZoneID_t getMaxVampireZoneID() {
        return m_MaxVampireZoneID;
    }
    static void setMaxVampireZoneID(ZoneID_t zoneID) {
        m_MaxVampireZoneID = zoneID;
    }

    static ZoneID_t getMaxOustersZoneID() {
        return m_MaxOustersZoneID;
    }
    static void setMaxOustersZoneID(ZoneID_t zoneID) {
        m_MaxOustersZoneID = zoneID;
    }

public:
    void expireTimeOutWaitMember(VSDateTime currentDateTime, list<string>& mList);

public: // debug
    string toString() const;

    static string correctString(const string& str);


    ///// Member data /////

protected:
    GuildID_t m_ID;
    string m_Name;
    GuildType_t m_Type;
    GuildRace_t m_Race;
    GuildState_t m_State;
    ServerGroupID_t m_ServerGroupID;
    ZoneID_t m_ZoneID;
    string m_Master;
    string m_Date;
    string m_Intro;

    HashMapGuildMember m_Members;
    int m_ActiveMemberCount; // Active Member Count
    int m_WaitMemberCount;   // Wait Member Count

    static GuildID_t m_MaxGuildID;
    static ZoneID_t m_MaxSlayerZoneID;
    static ZoneID_t m_MaxVampireZoneID;
    static ZoneID_t m_MaxOustersZoneID;

    mutable Mutex m_Mutex; // Mutex for Guild

#ifdef __GAME_SERVER__
    list<string> m_CurrentMembers;
#endif
};

#endif
