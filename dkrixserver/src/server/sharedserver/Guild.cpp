//////////////////////////////////////////////////////////////////////////////
// Filename		: Guild.cpp
// Written by	: bezz
// Description	:
//////////////////////////////////////////////////////////////////////////////

#include "Guild.h"

#include <algorithm>

#include "DB.h"
#include "PreparedStatement.h"
#include "StringStream.h"

#ifdef __SHARED_SERVER__
#include "GuildInfo2.h"
#include "GuildMemberInfo2.h"
#endif

#include <stdio.h>

#include "GCGuildMemberList.h"
#include "GuildInfo.h"
#include "GuildMemberInfo.h"
#include "Properties.h"

//////////////////////////////////////////////////////////////////////////////
// class GuildMember member methods
//////////////////////////////////////////////////////////////////////////////

GuildMember::GuildMember() noexcept {
    m_bLogOn = false;
    //	m_ServerID = 255;
}

void GuildMember::create() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement checkStmt(pConn, "SELECT GuildID FROM GuildMember WHERE Name = ?");
        checkStmt.bindString(1, m_Name);
        Result* pResult = checkStmt.execute();

        if (pResult->getRowCount() != 0) {


            if (m_Rank == GUILDMEMBER_RANK_WAIT) {
                PreparedStatement updateStmt(pConn,
                                             "UPDATE GuildMember SET GuildID = ?, `Rank` = ?, ExpireDate = '', "
                                             "RequestDateTime = ? WHERE Name = ?");
                updateStmt.bindInt(1, m_GuildID);
                updateStmt.bindInt(2, m_Rank);
                updateStmt.bindString(3, getRequestDateTime());
                updateStmt.bindString(4, m_Name);
                updateStmt.execute();
            } else {
                PreparedStatement updateStmt(
                    pConn, "UPDATE GuildMember SET GuildID = ?, `Rank` = ?, ExpireDate = '' WHERE Name = ?");
                updateStmt.bindInt(1, m_GuildID);
                updateStmt.bindInt(2, m_Rank);
                updateStmt.bindString(3, m_Name);
                updateStmt.execute();
            }
        } else {
            if (m_Rank == GUILDMEMBER_RANK_WAIT) {
                PreparedStatement insertStmt(
                    pConn,
                    "INSERT INTO GuildMember( GuildID, Name, `Rank`, RequestDateTime ) VALUES ( ?, ?, ?, ? )");
                insertStmt.bindInt(1, m_GuildID);
                insertStmt.bindString(2, m_Name);
                insertStmt.bindInt(3, m_Rank);
                insertStmt.bindString(4, getRequestDateTime());
                insertStmt.execute();
            } else {
                PreparedStatement insertStmt(pConn,
                                             "INSERT INTO GuildMember( GuildID, Name, `Rank` ) VALUES ( ?, ?, ? )");
                insertStmt.bindInt(1, m_GuildID);
                insertStmt.bindString(2, m_Name);
                insertStmt.bindInt(3, m_Rank);
                insertStmt.execute();
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


bool GuildMember::load() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement loadStmt(pConn, "SELECT GuildID, Name, `Rank`, LogOn FROM GuildMember WHERE Name = ?");
        loadStmt.bindString(1, m_Name);
        Result* pResult = loadStmt.execute();

        if (pResult->getRowCount() != 1) {
            return false;
        }

        pResult->next();

        m_GuildID = pResult->getInt(1);
        m_Name = pResult->getString(2);
        m_Rank = pResult->getInt(3);
        m_bLogOn = pResult->getInt(4);

        //		m_ServerID  = g_pConfig->getPropertyInt("ServerID");
    }
    END_DB(pStmt)

    return true;

    __END_CATCH
}


void GuildMember::save() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveStmt(pConn, "UPDATE GuildMember SET GuildID = ?, `Rank` = ? WHERE Name = ?");
        saveStmt.bindInt(1, m_GuildID);
        saveStmt.bindInt(2, m_Rank);
        saveStmt.bindString(3, m_Name);
        saveStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


void GuildMember::destroy() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement destroyStmt(pConn, "DELETE FROM GuildMember WHERE Name = ?");
        destroyStmt.bindString(1, m_Name);
        destroyStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void GuildMember::expire() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {

        time_t daytime = time(0);
        tm Timec;
        localtime_r(&daytime, &Timec);
        char ExpireDate[8];
        sprintf(ExpireDate, "%03d%02d%02d", Timec.tm_year, Timec.tm_mon, Timec.tm_mday);

        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement expireStmt(pConn, "UPDATE GuildMember SET `Rank` = ?, ExpireDate = ? WHERE Name = ?");
        expireStmt.bindInt(1, GUILDMEMBER_RANK_DENY);
        expireStmt.bindString(2, ExpireDate);
        expireStmt.bindString(3, m_Name);
        expireStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void GuildMember::leave() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {

        time_t daytime = time(0);
        tm Timec;
        localtime_r(&daytime, &Timec);
        char ExpireDate[8];
        sprintf(ExpireDate, "%03d%02d%02d", Timec.tm_year, Timec.tm_mon, Timec.tm_mday);

        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement leaveStmt(pConn, "UPDATE GuildMember SET `Rank` = ?, ExpireDate = ? WHERE Name = ?");
        leaveStmt.bindInt(1, GUILDMEMBER_RANK_LEAVE);
        leaveStmt.bindString(2, ExpireDate);
        leaveStmt.bindString(3, m_Name);
        leaveStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


void GuildMember::saveIntro(const string& intro) noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    string modifyIntro = Guild::correctString(intro);

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveIntroStmt(pConn, "UPDATE GuildMember SET Intro = ? WHERE Name = ?");
        saveIntroStmt.bindString(1, modifyIntro);
        saveIntroStmt.bindString(2, m_Name);
        saveIntroStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


string GuildMember::getIntro() const noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    string intro = "";

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement getIntroStmt(pConn, "SELECT Intro FROM GuildMember WHERE Name = ?");
        getIntroStmt.bindString(1, m_Name);
        Result* pResult = getIntroStmt.execute();

        if (pResult->next()) {
            intro = pResult->getString(1);
        }
    }
    END_DB(pStmt)

    return intro;

    __END_CATCH
}


string GuildMember::toString() const noexcept {
    StringStream msg;
    msg << "GuildID = " << (int)m_GuildID << " Name = " << m_Name << " Rank = " << (int)m_Rank << "\n";
    return msg.toString();
}


GuildMember& GuildMember::operator=(GuildMember& Member) {
    m_GuildID = Member.m_GuildID;
    m_Name = Member.m_Name;
    m_Rank = Member.m_Rank;

    return *this;
}

string GuildMember::getRequestDateTime() const noexcept(false) {
    __BEGIN_TRY

    char buf[20];

    sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", m_RequestDateTime.date().year(), m_RequestDateTime.date().month(),
            m_RequestDateTime.date().day(), m_RequestDateTime.time().hour(), m_RequestDateTime.time().minute(),
            m_RequestDateTime.time().second());

    cout << buf << endl;

    return string(buf);

    __END_CATCH
}


void GuildMember::setRank(GuildMemberRank_t rank) noexcept(false) {
    __BEGIN_TRY

    m_Rank = rank;

    __END_CATCH
}


void GuildMember::setRequestDateTime(const string& rtime) noexcept(false) {
    __BEGIN_TRY

    // 0123456789012345678
    // YYYY-MM-DD HH:MM:SS
    if (rtime.size() == 19) {
        int year = atoi(rtime.substr(0, 4).c_str());
        int month = atoi(rtime.substr(5, 2).c_str());
        int day = atoi(rtime.substr(8, 2).c_str());
        int hour = atoi(rtime.substr(11, 2).c_str());
        int min = atoi(rtime.substr(14, 2).c_str());
        int second = atoi(rtime.substr(17, 2).c_str());

        m_RequestDateTime.setDate(VSDate(year, month, day));
        m_RequestDateTime.setTime(VSTime(hour, min, second));
    } else {
        m_RequestDateTime.setDate(VSDate(2000, 1, 1));
        m_RequestDateTime.setTime(VSTime(0, 0, 0));
    }

    __END_CATCH
}

bool GuildMember::isRequestDateTimeOut(const VSDateTime& currentDateTime) const noexcept {
    VSDateTime limitDateTime = m_RequestDateTime.addDays(7);
    return currentDateTime > limitDateTime;
}


//////////////////////////////////////////////////////////////////////////////
// global variable initialization
//////////////////////////////////////////////////////////////////////////////

GuildID_t Guild::m_MaxGuildID = 0;
ZoneID_t Guild::m_MaxSlayerZoneID = 10000;
ZoneID_t Guild::m_MaxVampireZoneID = 20000;
ZoneID_t Guild::m_MaxOustersZoneID = 30000;

//////////////////////////////////////////////////////////////////////////////
// class Guild member methods
//////////////////////////////////////////////////////////////////////////////

Guild::Guild() noexcept(false) {
    m_ID = 0;
    m_Name = "";
    m_Type = 0;
    m_State = 0;
    m_ServerGroupID = 0;
    m_ZoneID = 0;
    m_Master = "";
    m_Date = "";
    m_Intro = "";

    m_ActiveMemberCount = 0;
    m_WaitMemberCount = 0;

    __BEGIN_TRY

    m_Mutex.setName("Guild");

    __END_CATCH
}


Guild::~Guild() noexcept {
    try {
        __ENTER_CRITICAL_SECTION(m_Mutex)

        HashMapGuildMemberItor itr = m_Members.begin();
        for (; itr != m_Members.end(); itr++) {
            SAFE_DELETE(itr->second);
        }

        m_Members.clear();

#ifdef __GAME_SERVER__
        m_CurrentMembers.clear();
#endif

        __LEAVE_CRITICAL_SECTION(m_Mutex)
    } catch (...) {
        // destructor must not throw
    }
}


void Guild::create() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    string correctIntro = correctString(m_Intro);

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement createStmt(
            pConn, "INSERT INTO GuildInfo ( GuildID, GuildName, GuildType, GuildRace, GuildState, ServerGroupID, "
                   "GuildZoneID, Master, Date, Intro ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )");
        createStmt.bindInt(1, m_ID);
        createStmt.bindString(2, m_Name);
        createStmt.bindInt(3, m_Type);
        createStmt.bindInt(4, m_Race);
        createStmt.bindInt(5, m_State);
        createStmt.bindInt(6, m_ServerGroupID);
        createStmt.bindInt(7, m_ZoneID);
        createStmt.bindString(8, m_Master);
        createStmt.bindString(9, m_Date);
        createStmt.bindString(10, correctIntro);
        createStmt.execute();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


bool Guild::load() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement loadStmt(pConn,
                                   "SELECT GuildName, GuildType, GuildRace, GuildState, ServerGroupID, GuildZoneID, "
                                   "Master, Date FROM GuildInfo WHERE GuildID = ?");
        loadStmt.bindInt(1, m_ID);
        Result* pResult = loadStmt.execute();

        if (pResult->getRowCount() != 1) {
            m_Mutex.unlock();

            return false;
        }

        pResult->next();

        m_Name = pResult->getString(1);
        m_Type = pResult->getInt(2);
        m_Race = pResult->getInt(3);
        m_State = pResult->getInt(4);
        m_ServerGroupID = pResult->getInt(5);
        m_ZoneID = pResult->getInt(6);
        m_Master = pResult->getString(7);
        m_Date = pResult->getString(8);
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return true;

    __END_CATCH
}


void Guild::save() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveStmt(
            pConn, "UPDATE GuildInfo SET GuildName = ?, GuildType = ?, GuildRace = ?, GuildState = ?, "
                   "ServerGroupID = ?, GuildZoneID = ?, Master = ?, Date = ? WHERE GuildID = ?");
        saveStmt.bindString(1, m_Name);
        saveStmt.bindInt(2, m_Type);
        saveStmt.bindInt(3, m_Race);
        saveStmt.bindInt(4, m_State);
        saveStmt.bindInt(5, m_ServerGroupID);
        saveStmt.bindInt(6, m_ZoneID);
        saveStmt.bindString(7, m_Master);
        saveStmt.bindString(8, m_Date);
        saveStmt.bindInt(9, m_ID);
        saveStmt.execute();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


void Guild::destroy() noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement deleteInfoStmt(pConn, "DELETE FROM GuildInfo WHERE GuildID = ?");
        deleteInfoStmt.bindInt(1, m_ID);
        deleteInfoStmt.execute();

        PreparedStatement deleteUnionMemberStmt(pConn, "DELETE FROM GuildUnionMember WHERE OwnerGuildID = ?");
        deleteUnionMemberStmt.bindInt(1, m_ID);
        deleteUnionMemberStmt.execute();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


#ifdef __SHARED_SERVER__
void Guild::saveIntro(const string& intro) noexcept(false) {
    __BEGIN_TRY

    m_Intro = intro;

    Statement* pStmt = NULL;

    string modifyIntro = Guild::correctString(intro);

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveIntroStmt(pConn, "UPDATE GuildInfo SET Intro = ? WHERE GuildID = ?");
        saveIntroStmt.bindString(1, modifyIntro);
        saveIntroStmt.bindUInt(2, m_ID);
        saveIntroStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void Guild::tinysave(const char* field) const noexcept(false) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // `field` is a caller-built "Column = value" SQL fragment (see
        // GSModifyGuildMemberHandler.cpp, e.g. "Master='Name'"), not a
        // single bindable value -- PreparedStatement has no way to
        // parameterise an entire dynamic assignment list without changing
        // tinysave()'s signature and every caller. Left spliced, same as
        // the pre-existing (un-migrated) parked-line copy of this
        // function; only the GuildID condition is bound.
        PreparedStatement tinysaveStmt(pConn, "UPDATE GuildInfo SET " + string(field) + " WHERE GuildID = ?");
        tinysaveStmt.bindUInt(1, m_ID);
        tinysaveStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void Guild::saveCount() const noexcept(false){__BEGIN_TRY

                                                  //	char buffer[200];
                                                  //	sprintf(buffer, "MemberCount=%u", m_ActiveMemberCount);
                                                  //	tinysave(buffer);

                                                  __END_CATCH}
#endif


GuildMember* Guild::getMember(const string& name) const noexcept(false) {
    __BEGIN_TRY

    HashMapGuildMemberConstItor itr;
    GuildMember* pGuildMember = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    itr = m_Members.find(name);

    if (itr == m_Members.end()) {
        // cout << "Guild::getMember() : NoSuchMember" << endl;
        m_Mutex.unlock();

        return NULL;
    }

    pGuildMember = itr->second;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pGuildMember;

    __END_CATCH
}


GuildMember* Guild::getMember_NOLOCKED(const string& name) const noexcept(false) {
    __BEGIN_TRY

    HashMapGuildMemberConstItor itr;
    GuildMember* pGuildMember = NULL;

    itr = m_Members.find(name);

    if (itr == m_Members.end()) {
        // cerr << "Guild::getMember() : NoSuchMember" << endl;

        return NULL;
    }

    pGuildMember = itr->second;

    return pGuildMember;

    __END_CATCH
}

void Guild::addMember(GuildMember* pMember) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    Assert(pMember);

    HashMapGuildMemberConstItor itr;

    itr = m_Members.find(pMember->getName());

    if (itr != m_Members.end()) {
        m_Mutex.unlock();
        throw DuplicatedException();
    }

    m_Members[pMember->getName()] = pMember;

    GuildMemberRank_t rank = pMember->getRank();

    if (rank == GuildMember::GUILDMEMBER_RANK_NORMAL || rank == GuildMember::GUILDMEMBER_RANK_MASTER ||
        rank == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
        
        m_ActiveMemberCount++;
    } else if (rank == GuildMember::GUILDMEMBER_RANK_WAIT) {
        
        m_WaitMemberCount++;
    }

#ifdef __SHARED_SERVER__
    saveCount();
#endif

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


void Guild::deleteMember(const string& name) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    HashMapGuildMemberItor itr;

    itr = m_Members.find(name);

    if (itr == m_Members.end()) {
        cerr << "Guild::deleteMember() : NoSuchElementException" << endl;
        m_Mutex.unlock();

        return;
    }

    GuildMemberRank_t rank = itr->second->getRank();

    if (rank == GuildMember::GUILDMEMBER_RANK_NORMAL || rank == GuildMember::GUILDMEMBER_RANK_MASTER ||
        rank == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
        
        m_ActiveMemberCount--;
    } else if (rank == GuildMember::GUILDMEMBER_RANK_WAIT) {
        m_WaitMemberCount--;
    }

    SAFE_DELETE(itr->second);

    m_Members.erase(itr);

#ifdef __SHARED_SERVER__
    saveCount();
#endif

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


void Guild::modifyMember(GuildMember& Member) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    HashMapGuildMemberConstItor itr;

    itr = m_Members.find(Member.getName());

    if (itr == m_Members.end()) {
        cerr << "Guild::modifyMember() : NoSuchElementException" << endl;
        m_Mutex.unlock();

        return;
    }

    *(itr->second) = Member;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


void Guild::modifyMemberRank(const string& name, GuildMemberRank_t rank) noexcept(false) {
    __BEGIN_TRY

    GuildMember* pMember = getMember(name);
    if (pMember == NULL)
        return;

    GuildMemberRank_t oldRank = pMember->getRank();

    if (oldRank == rank)
        return;

    if (oldRank == GuildMember::GUILDMEMBER_RANK_WAIT) {
        m_WaitMemberCount--;
    } else if (oldRank == GuildMember::GUILDMEMBER_RANK_NORMAL || oldRank == GuildMember::GUILDMEMBER_RANK_MASTER ||
               oldRank == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
        m_ActiveMemberCount--;
    }

    if (rank == GuildMember::GUILDMEMBER_RANK_WAIT) {
        m_WaitMemberCount++;
    } else if (rank == GuildMember::GUILDMEMBER_RANK_NORMAL || rank == GuildMember::GUILDMEMBER_RANK_MASTER ||
               rank == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
        m_ActiveMemberCount++;
    }

    pMember->setRank(rank);

#ifdef __SHARED_SERVER__
    pMember->save();
    saveCount();
#endif

    __END_CATCH
}


#ifdef __GAME_SERVER__
void Guild::addCurrentMember(const string& name) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex) 

    if (m_CurrentMembers.end() != find(m_CurrentMembers.begin(), m_CurrentMembers.end(), name)) {
        m_Mutex.unlock();
        return;
    }

    m_CurrentMembers.push_back(name);

    
    GuildMember* pGuildMember = getMember_NOLOCKED(name);
    if (pGuildMember == NULL) {
        m_Mutex.unlock();
        return;
    }

    pGuildMember->setLogOn(true);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

void Guild::deleteCurrentMember(const string& name) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    list<string>::iterator itr = find(m_CurrentMembers.begin(), m_CurrentMembers.end(), name);

    if (m_CurrentMembers.end() == itr) {
        m_Mutex.unlock();
        return;
    }

    m_CurrentMembers.erase(itr);

    
    GuildMember* pGuildMember = getMember_NOLOCKED(name);
    if (pGuildMember == NULL) {
        m_Mutex.unlock();
        return;
    }

    pGuildMember->setLogOn(false);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

list<string> Guild::getCurrentMembers() noexcept(false) {
    __BEGIN_TRY

    list<string> cmList;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    cmList = m_CurrentMembers;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return cmList;

    __END_CATCH
}
#endif

#ifdef __SHARED_SERVER__
void Guild::makeInfo(GuildInfo2* pGuildInfo) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    pGuildInfo->setID(m_ID);
    pGuildInfo->setName(m_Name);
    pGuildInfo->setType(m_Type);
    pGuildInfo->setRace(m_Race);
    pGuildInfo->setState(m_State);
    pGuildInfo->setServerGroupID(m_ServerGroupID);
    pGuildInfo->setZoneID(m_ZoneID);
    pGuildInfo->setMaster(m_Master);
    pGuildInfo->setDate(m_Date);
    pGuildInfo->setIntro(m_Intro);

    HashMapGuildMemberConstItor itr = m_Members.begin();
    for (; itr != m_Members.end(); itr++) {
        GuildMemberInfo2* pGuildMemberInfo = new GuildMemberInfo2();
        pGuildMemberInfo->setGuildID(itr->second->getGuildID());
        pGuildMemberInfo->setName(itr->second->getName());
        pGuildMemberInfo->setRank(itr->second->getRank());
        pGuildMemberInfo->setLogOn(itr->second->getLogOn());

        pGuildInfo->addGuildMemberInfo(pGuildMemberInfo);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}
#endif

void Guild::makeInfo(GuildInfo* pGuildInfo) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    pGuildInfo->setGuildID(m_ID);
    pGuildInfo->setGuildName(m_Name);
    pGuildInfo->setGuildMaster(m_Master);
    pGuildInfo->setGuildMemberCount(m_ActiveMemberCount);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

void Guild::makeMemberInfo(GCGuildMemberList& gcGuildMemberList) noexcept(false) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    HashMapGuildMember& Members = getMembers();
    HashMapGuildMemberConstItor itr = Members.begin();

    for (; itr != Members.end(); itr++) {
        GuildMember* pGuildMember = itr->second;

        GuildMemberInfo* pGuildMemberInfo = new GuildMemberInfo();
        pGuildMemberInfo->setName(pGuildMember->getName());
        pGuildMemberInfo->setRank(pGuildMember->getRank());
        pGuildMemberInfo->setLogOn(pGuildMember->getLogOn());
        //		pGuildMemberInfo->setServerID( pGuildMember->getServerID() );

        gcGuildMemberList.addGuildMemberInfo(pGuildMemberInfo);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


void Guild::expireTimeOutWaitMember(VSDateTime currentDateTime, list<string>& mList) noexcept(false) {
    __BEGIN_TRY

#ifdef __SHARED_SERVER__

    HashMapGuildMemberItor itr = m_Members.begin();

    while (itr != m_Members.end()) {
        GuildMember* pGuildMember = itr->second;

        if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT &&
            pGuildMember->isRequestDateTimeOut(currentDateTime)) {
            mList.push_back(pGuildMember->getName());

            
            m_WaitMemberCount--;

            pGuildMember->expire();

            SAFE_DELETE(pGuildMember);

            m_Members.erase(itr++);
        } else {
            itr++;
        }
    }

#endif

    __END_CATCH
}


string Guild::toString() const noexcept {
    StringStream msg;
    msg << " GuildID = " << m_ID << " GuildName = " << m_Name << " GuildType = " << (int)m_Type
        << " GuildState = " << (int)m_State << " ServerGroupID = " << (int)m_ServerGroupID
        << " GuildZoneID = " << (int)m_ZoneID << " Master = " << m_Master << " Date = " << m_Date << " \n";

    return msg.toString();
}

string Guild::correctString(const string& str) noexcept {
    string correct = str;

    unsigned int i = 0;
    unsigned int size = str.size();

    while (i < size) {
        if (correct[i] == '\\') {
            correct.replace(i, 1, "\\\\");
            i = i + 2;
            size++;
        } else if (correct[i] == '\'') {
            correct.replace(i, 1, "\\'");
            i = i + 2;
            size++;
        } else {
            i++;
        }
    }

    return correct;
}
