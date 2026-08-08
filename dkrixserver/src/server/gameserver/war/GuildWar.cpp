///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

#include "GuildWar.h"

#include <stdio.h>

#include "Assert.h"
#include "CGSay.h"
#include "CastleInfoManager.h"
#include "CastleShrineInfoManager.h"
#include "DB.h"
#include "GCNoticeEvent.h"
#include "GCSystemMessage.h"
#include "GCWarScheduleList.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildWarInfo.h"
#include "HolyLandRaceBonus.h"
#include "Mutex.h"
#include "PCManager.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "StringStream.h"
#include "WarSystem.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"

//--------------------------------------------------------------------------------
//
// constructor / destructor
//
//--------------------------------------------------------------------------------
GuildWar::GuildWar(ZoneID_t castleZoneID, GuildID_t challenger, WarState warState, WarID_t warID)
    : War(warState, warID), m_CastleZoneID(castleZoneID), m_ChallangerGuildID(challenger), m_RegistrationFee(0) {
    m_bModifyCastleOwner = false;
}

GuildWar::~GuildWar() {}

//--------------------------------------------------------------------------------
//
// executeStart
//
//--------------------------------------------------------------------------------

//


//--------------------------------------------------------------------------------
void GuildWar::executeStart()

{
    __BEGIN_TRY

    sendWarStartMessage();


    ZoneID_t guardShrineZoneID = g_pCastleShrineInfoManager->getGuardShrineZoneID(m_CastleZoneID);
    Zone* pZone = getZoneByZoneID(guardShrineZoneID);
    Assert(pZone != NULL);


    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
    Assert(pCastleInfo != NULL);

    GuildID_t OwnerGuildID = pCastleInfo->getGuildID();


    if (OwnerGuildID != SlayerCommon && OwnerGuildID != VampireCommon) {
        const list<ZoneID_t>& zoneIDs = pCastleInfo->getZoneIDList();

        list<ZoneID_t>::const_iterator itr = zoneIDs.begin();
        for (; itr != zoneIDs.end(); itr++) {
            ZoneID_t targetZoneID = *itr;
            Zone* pTargetZone = getZoneByZoneID(targetZoneID);


            if (targetZoneID != m_CastleZoneID) {
                pTargetZone->killAllMonsters();
            }
        }
    }

    g_pCastleShrineInfoManager->removeShrineShield(pZone);


    recordGuildWarStart();

    __END_CATCH
}

void GuildWar::recordGuildWarStart()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);


    if (pCastleInfo == NULL)
        return;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertGuildWarHistoryStmt(
            pConn, "INSERT IGNORE INTO GuildWarHistory (WarID, GuildWarID, ServerID, CastleName, DefenseGuildID, "
                   "DefenseGuildName, AttackGuildID, AttackGuildName) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        insertGuildWarHistoryStmt.bindInt(1, (int)getWarID());
        insertGuildWarHistoryStmt.bindString(2, getWarStartTime().toStringforWeb());
        insertGuildWarHistoryStmt.bindInt(3, g_pConfig->getPropertyInt("ServerID"));
        insertGuildWarHistoryStmt.bindString(4, pCastleInfo->getName());
        insertGuildWarHistoryStmt.bindInt(5, (int)pCastleInfo->getGuildID());
        insertGuildWarHistoryStmt.bindString(6, g_pGuildManager->getGuildName(pCastleInfo->getGuildID()));
        insertGuildWarHistoryStmt.bindInt(7, getChallangerGuildID());
        insertGuildWarHistoryStmt.bindString(8, g_pGuildManager->getGuildName(getChallangerGuildID()));
        insertGuildWarHistoryStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
// executeEnd
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GuildWar::executeEnd()

{
    __BEGIN_TRY

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    sendWarEndMessage();

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    if (m_bModifyCastleOwner) {
        g_pCastleInfoManager->modifyCastleOwner(m_CastleZoneID, m_WinnerRace, m_WinnerGuildID);

        if (g_pConfig->getPropertyInt("IsNetMarble") == 1) {
            char sCommand[100];
            sprintf(sCommand, "*world *command setCastleOwnerGuild %u %u", m_CastleZoneID, m_WinnerGuildID);
            CGSayHandler::opworld(NULL, sCommand, 0, true);
        }
    } else {
        CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
        m_WinnerGuildID = pCastleInfo->getGuildID();
    }

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    g_pCastleShrineInfoManager->returnAllCastleSymbol(m_CastleZoneID);

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------


    ZoneID_t guardShrineZoneID = g_pCastleShrineInfoManager->getGuardShrineZoneID(m_CastleZoneID);
    Zone* pZone = getZoneByZoneID(guardShrineZoneID);
    Assert(pZone != NULL);

    /*
    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo( m_CastleZoneID );
    Assert(pCastleInfo!=NULL);

    __ENTER_CRITICAL_SECTION( (*pZone) )

    pZone->resetSafeZone();

    pZone->loadNPCs( pCastleInfo->getRace() );

    g_pShrineInfoManager->addShrineShield_LOCKED( pZone );

    __LEAVE_CRITICAL_SECTION( (*pZone) )
    */

    g_pCastleShrineInfoManager->addShrineShield(pZone);

    //----------------------------------------------------------------------------


    //----------------------------------------------------------------------------
    g_pCastleInfoManager->increaseTaxBalance(m_CastleZoneID, m_RegistrationFee);
    m_RegistrationFee = 0;


    recordGuildWarEnd();

    __END_CATCH
}

void GuildWar::recordGuildWarEnd()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement updateGuildWarHistoryStmt(
            pConn, "UPDATE GuildWarHistory SET WinnerGuildID = ? , WinnerGuildName = ? WHERE WarID = ?");
        updateGuildWarHistoryStmt.bindInt(1, (int)m_WinnerGuildID);
        updateGuildWarHistoryStmt.bindString(2, g_pGuildManager->getGuildName(m_WinnerGuildID));
        updateGuildWarHistoryStmt.bindInt(3, (int)getWarID());
        updateGuildWarHistoryStmt.execute();
    }
    END_DB(pStmt)


    char cmd[100];
    sprintf(cmd, "/home/darkeden/vs/bin/script/recordGuildWarHistory.py %d %d %d ", (int)getWarID(),
            g_pConfig->getPropertyInt("Dimension"), g_pConfig->getPropertyInt("WorldID"));

    filelog("script.log", cmd);
    system(cmd);

    __END_CATCH
}

string GuildWar::getWarName() const

{
    __BEGIN_TRY

    ZoneInfo* pZoneInfo = NULL;
    Guild* pGuild = NULL;

    try {
        pGuild = g_pGuildManager->getGuild(m_ChallangerGuildID);
        pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_CastleZoneID);

        if (pGuild == NULL || pZoneInfo == NULL)
            return " ";

    } catch (Throwable& t) {
        return " ";
    }

    Assert(pZoneInfo != NULL);
    Assert(pGuild != NULL);

    StringStream msg;

    msg << pGuild->getName() << " ";
    msg << pZoneInfo->getFullName() << "   ";

    return msg.toString();

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
//	isModifyCastleOwner( PlayerCreature* pPC )
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
bool GuildWar::isModifyCastleOwner(PlayerCreature* pPC)

{
    __BEGIN_TRY

    Assert(pPC != NULL);

    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
    Assert(pCastleInfo != NULL);


    if (pPC->getGuildID() == m_ChallangerGuildID ||
        (!pCastleInfo->isCommon() && pPC->getCommonGuildID() == pPC->getGuildID())) {
        return true;
    }

    return false;

    __END_CATCH
}


//--------------------------------------------------------------------------------
//
// getWinnerGuildID( PlayerCreature* pPC )
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
GuildID_t GuildWar::getWinnerGuildID(PlayerCreature* pPC)

{
    __BEGIN_TRY

    Assert(pPC != NULL);


    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
    Assert(pCastleInfo != NULL);

    if (m_ChallangerGuildID == pPC->getGuildID() || pPC->getGuildID() == pCastleInfo->getGuildID()) {
        return pPC->getGuildID();
    }

    return pPC->getCommonGuildID();

    __END_CATCH
}

bool GuildWar::endWar(PlayerCreature* pPC)

{
    __BEGIN_TRY

    Assert(pPC != NULL);


    if (isModifyCastleOwner(pPC)) {
        m_WinnerRace = pPC->getRace();
        m_WinnerGuildID = getWinnerGuildID(pPC);
        m_bModifyCastleOwner = true;

        return true;
    }

    return false;

    __END_CATCH
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GuildWar::sendWarEndMessage() const

{
    __BEGIN_TRY

    War::sendWarEndMessage();


    GCNoticeEvent gcNoticeEvent;
    gcNoticeEvent.setCode(NOTICE_EVENT_WAR_OVER);
    gcNoticeEvent.setParameter(m_CastleZoneID);
    g_pZoneGroupManager->broadcast(&gcNoticeEvent);

    __END_CATCH
}

void GuildWar::makeWarScheduleInfo(WarScheduleInfo* pWSI) const

{
    __BEGIN_TRY

    pWSI->warType = getWarType();
    pWSI->challengerGuildID[0] = getChallangerGuildID();
    for (int i = 1; i < 5; ++i)
        pWSI->challengerGuildID[i] = 0;
    pWSI->reinforceGuildID = 0;

    pWSI->challengerGuildName[0] = g_pGuildManager->getGuildName(getChallangerGuildID());

    __END_CATCH
}

void GuildWar::makeWarInfo(WarInfo* pWarInfo) const

{
    __BEGIN_TRY

    Assert(pWarInfo != NULL);
    Assert(pWarInfo->getWarType() == WAR_GUILD);

    GuildWarInfo* pGuildWarInfo = dynamic_cast<GuildWarInfo*>(pWarInfo);
    Assert(pGuildWarInfo != NULL);

    //---------------------------------------------------

    //---------------------------------------------------
    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(getCastleZoneID());
    if (pCastleInfo == NULL) {
        filelog("WarError.log", "CastleInfo (%d)", getCastleZoneID());
        return;
    }

    GuildID_t ownGuildID = pCastleInfo->getGuildID();
    GuildID_t challangerGuildID = getChallangerGuildID();

    pGuildWarInfo->addJoinGuild(ownGuildID);
    pGuildWarInfo->addJoinGuild(challangerGuildID);
    pGuildWarInfo->setCastleID(getCastleZoneID());


    static const string commonSlayerGuild("");
    static const string commonVampireGuild("");

    string attackGuildName;
    string defenseGuildName;

    if (challangerGuildID == SlayerCommon)
        attackGuildName = commonSlayerGuild;
    else if (challangerGuildID == VampireCommon)
        attackGuildName = commonVampireGuild;
    else
        attackGuildName = g_pGuildManager->getGuildName(getChallangerGuildID());

    if (ownGuildID == SlayerCommon)
        defenseGuildName = commonSlayerGuild;
    else if (ownGuildID == VampireCommon)
        defenseGuildName = commonVampireGuild;
    else
        defenseGuildName = g_pGuildManager->getGuildName(ownGuildID);

    pGuildWarInfo->setAttackGuildName(attackGuildName);
    pGuildWarInfo->setDefenseGuildName(defenseGuildName);

    __END_CATCH
}


string GuildWar::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GuildWar("
        << "WarID:" << (int)getWarID() << ",State:" << (int)getState() << ",CastleZoneID:" << (int)m_CastleZoneID
        << ",WarType:" << getWarType2DBString() << ",ChallengerGuildID:" << (int)m_ChallangerGuildID
        << ",RegistrationFee:" << (int)m_RegistrationFee << ")";

    return msg.toString();

    __END_CATCH
}
