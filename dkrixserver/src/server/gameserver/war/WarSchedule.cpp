#include "WarSchedule.h"

#include <stdio.h>

#include "CastleInfoManager.h"
#include "DB.h"
#include "GCWarList.h"
#include "GCWarScheduleList.h"
#include "GuildManager.h"
#include "GuildWar.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "SiegeWar.h"
#include "War.h"
#include "Zone.h"

WarSchedule::WarSchedule(Work* pWork, const VSDateTime& Time, ScheduleType type // = SCHEDULE_TYPE_ONCE
                         )

    : Schedule(pWork, Time, type){__BEGIN_TRY __END_CATCH}

      WarSchedule::~WarSchedule()

{}

void WarSchedule::makeWarScheduleInfo(WarScheduleInfo* pWSI) const

{
    __BEGIN_TRY

    Assert(m_pWork != NULL);

    War* pWar = dynamic_cast<War*>(m_pWork);
    Assert(pWar != NULL);

    pWar->makeWarScheduleInfo(pWSI);
    pWSI->year = m_ScheduledTime.date().year();
    pWSI->month = m_ScheduledTime.date().month();
    pWSI->day = m_ScheduledTime.date().day();
    pWSI->hour = m_ScheduledTime.time().hour();

    __END_CATCH
}

void WarSchedule::makeWarInfo(WarInfo* pWarInfo) const

{
    __BEGIN_TRY

    Assert(pWarInfo != NULL);

    const Work* pWork = getWork();
    Assert(pWork != NULL);

    const War* pWar = dynamic_cast<const War*>(pWork);
    Assert(pWar != NULL);

    //---------------------------------------------------

    //---------------------------------------------------
    VSDateTime dt(VSDateTime::currentDateTime());
    int endHour = m_ScheduledTime.time().hour();
    int endMin = m_ScheduledTime.time().minute();
    int endSec = m_ScheduledTime.time().second();
    int curHour = dt.time().hour();
    int curMin = dt.time().minute();
    int curSec = dt.time().second();
    int endSecs = endHour * 60 * 60 + endMin * 60 + endSec;
    int curSecs = curHour * 60 * 60 + curMin * 60 + curSec;

    int remainSec = 0;
    if (endSecs > curSecs)
        remainSec = endSecs - curSecs;

    //	cout << "makeWarInfo : " << m_ScheduledTime.toString() << endl;
    DWORD startTime = ((DWORD)((DWORD)(m_ScheduledTime.date().year() - 2000)) * 1000000) +
                      ((DWORD)((DWORD)m_ScheduledTime.date().month()) * 10000) +
                      ((DWORD)((DWORD)m_ScheduledTime.date().day()) * 100) +
                      ((DWORD)((DWORD)m_ScheduledTime.time().hour()));

    //	cout << "startTime : " << startTime << endl;

    //---------------------------------------------------

    //---------------------------------------------------
    pWar->makeWarInfo(pWarInfo);
    pWarInfo->setRemainTime(remainSec);
    pWarInfo->setStartTime(startTime);

    //	cout << "after set : " << pWarInfo->getStartTime() << endl;

    __END_CATCH
}

void WarSchedule::create()

{
    __BEGIN_TRY

    War* pWar = dynamic_cast<War*>(m_pWork);
    Assert(pWar != NULL);

    if (pWar->getWarType() != WAR_GUILD)
        return;

    SiegeWar* pSiegeWar = dynamic_cast<SiegeWar*>(pWar);
    Assert(pSiegeWar != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertWarScheduleInfoStmt(
            pConn, "INSERT IGNORE INTO WarScheduleInfo ( WarID, ServerID, ZoneID, WarType, AttackGuildID, WarFee, "
                   "StartTime, Status ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ? )");
        insertWarScheduleInfoStmt.bindUInt(1, (int)pSiegeWar->getWarID());
        insertWarScheduleInfoStmt.bindUInt(2, g_pConfig->getPropertyInt("ServerID"));
        insertWarScheduleInfoStmt.bindUInt(3, (int)pSiegeWar->getCastleZoneID());
        insertWarScheduleInfoStmt.bindString(4, pSiegeWar->getWarType2DBString());
        insertWarScheduleInfoStmt.bindUInt(5, (int)pSiegeWar->getChallangerGuildID());
        insertWarScheduleInfoStmt.bindUInt(6, (int)pSiegeWar->getRegistrationFee());
        insertWarScheduleInfoStmt.bindString(7, m_ScheduledTime.toDateTime());
        insertWarScheduleInfoStmt.bindString(8, pSiegeWar->getState2DBString());
        insertWarScheduleInfoStmt.execute();

        if (insertWarScheduleInfoStmt.getAffectedRowCount() == 0) {
            filelog("WarError.log", "WarSchedule::create() :   War    .");
            return;
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

void WarSchedule::save()

{
    __BEGIN_TRY

    War* pWar = dynamic_cast<War*>(m_pWork);
    Assert(pWar != NULL);

    if (pWar->getWarType() != WAR_GUILD)
        return;

    SiegeWar* pSiegeWar = dynamic_cast<SiegeWar*>(pWar);
    Assert(pSiegeWar != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement replaceWarScheduleInfoStmt(
            pConn, "REPLACE INTO WarScheduleInfo ( WarID, ServerID, ZoneID, WarType, AttackerCount, "
                   "AttackGuildID, AttackGuildID2, AttackGuildID3, AttackGuildID4, AttackGuildID5, WarFee, "
                   "StartTime, Status ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )");
        replaceWarScheduleInfoStmt.bindUInt(1, (int)pSiegeWar->getWarID());
        replaceWarScheduleInfoStmt.bindUInt(2, g_pConfig->getPropertyInt("ServerID"));
        replaceWarScheduleInfoStmt.bindUInt(3, (int)pSiegeWar->getCastleZoneID());
        replaceWarScheduleInfoStmt.bindString(4, pSiegeWar->getWarType2DBString());
        replaceWarScheduleInfoStmt.bindUInt(5, (int)pSiegeWar->getChallengerGuildCount());
        replaceWarScheduleInfoStmt.bindUInt(6, (int)pSiegeWar->getChallangerGuildID(0));
        replaceWarScheduleInfoStmt.bindUInt(7, (int)pSiegeWar->getChallangerGuildID(1));
        replaceWarScheduleInfoStmt.bindUInt(8, (int)pSiegeWar->getChallangerGuildID(2));
        replaceWarScheduleInfoStmt.bindUInt(9, (int)pSiegeWar->getChallangerGuildID(3));
        replaceWarScheduleInfoStmt.bindUInt(10, (int)pSiegeWar->getChallangerGuildID(4));
        replaceWarScheduleInfoStmt.bindUInt(11, (int)pSiegeWar->getRegistrationFee());
        replaceWarScheduleInfoStmt.bindString(12, m_ScheduledTime.toDateTime());
        replaceWarScheduleInfoStmt.bindString(13, pSiegeWar->getState2DBString());
        replaceWarScheduleInfoStmt.execute();

        if (replaceWarScheduleInfoStmt.getAffectedRowCount() == 0) {
            filelog("WarError.log", "WarSchedule::create() :   War    .");
            return;
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

void WarSchedule::tinysave(const string& query)

{
    __BEGIN_TRY

    War* pWar = dynamic_cast<War*>(m_pWork);
    Assert(pWar != NULL);

    Statement* pStmt = NULL;
    // Result*		pResult = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // query is a caller-built "Column=value" SQL fragment (e.g. "Status='WAR_STARTED'"), never
        // packet/user input; PreparedStatement cannot bind identifiers/fragments.
        PreparedStatement updateWarScheduleInfoStmt(pConn, "UPDATE WarScheduleInfo SET " + query +
                                                               " WHERE WarID = ? AND ServerID = ?");
        updateWarScheduleInfoStmt.bindInt(1, pWar->getWarID());
        updateWarScheduleInfoStmt.bindInt(2, g_pConfig->getPropertyInt("ServerID"));
        updateWarScheduleInfoStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

bool WarSchedule::heartbeat()

{
    __BEGIN_TRY

    if (Schedule::heartbeat()) {
        if (m_pWork != NULL) {
            War* pWar = dynamic_cast<War*>(m_pWork);
            Assert(pWar != NULL);

            char pState[20];
            sprintf(pState, "Status='%s'", pWar->getState2DBString().c_str());
            tinysave(string(pState));
        }

        return true;
    }

    return false;

    __END_CATCH
}
