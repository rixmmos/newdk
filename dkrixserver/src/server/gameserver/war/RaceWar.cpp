///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

#include "RaceWar.h"

#include <stdio.h>

#include "Assert.h"
#include "DB.h"
#include "Mutex.h"
#include "Properties.h"
#include "WarSystem.h"
#include "ZoneGroupManager.h"
// #include "HolyLandRaceBonus.h"
#include "CGSay.h"
#include "CastleInfoManager.h"
#include "DragonEyeManager.h"
#include "GCNoticeEvent.h"
#include "GCSystemMessage.h"
#include "GCWarScheduleList.h"
#include "HolyLandManager.h"
#include "PCManager.h"
#include "PreparedStatement.h"
#include "RaceWarInfo.h"
#include "RaceWarLimiter.h"
#include "RegenZoneManager.h"
#include "ShrineInfoManager.h"
#include "StringStream.h"
#include "VariableManager.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"

//--------------------------------------------------------------------------------
//
// constructor / destructor
//
//--------------------------------------------------------------------------------
RaceWar::RaceWar(WarState warState, WarID_t warID) : War(warState, warID) {}

RaceWar::~RaceWar() {}

//--------------------------------------------------------------------------------
//
// executeStart
//
//--------------------------------------------------------------------------------

//


//--------------------------------------------------------------------------------
void RaceWar::executeStart()

{
    __BEGIN_TRY

    sendWarStartMessage();


    //	g_pHolyLandRaceBonus->clear();


    // g_pCastleInfoManager->deleteAllNPCs();


    g_pCastleInfoManager->releaseAllSafeZone();


    g_pShrineInfoManager->removeAllShrineShield();


    // g_pShrineInfoManager->broadcastBloodBibleStatus();
    //	g_pHolyLandManager->sendBloodBibleStatus();


    g_pHolyLandManager->fixTimeband(g_pVariableManager->getVariable(RACE_WAR_TIMEBAND));

    g_pHolyLandManager->killAllMonsters();

    RegenZoneManager::getInstance()->putTryingPosition();
    RegenZoneManager::getInstance()->broadcastStatus();


    g_pDragonEyeManager->addAllDragonEyesToZone();


    // g_pHolyLandManager->remainRaceWarPlayers();


    recordRaceWarStart();

    __END_CATCH
}

void RaceWar::recordRaceWarStart()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectRaceWarPCLimitStmt(pConn,
                                                   "SELECT Race, SUM(CurrentNum) FROM RaceWarPCLimit GROUP BY Race");
        Result* pResult = selectRaceWarPCLimitStmt.execute();

        uint slayerSum = 0;
        uint vampireSum = 0;
        uint oustersSum = 0;

        string slayerOld;
        string vampireOld;
        string oustersOld;

        while (pResult->next()) {
            uint race = pResult->getInt(1);
            uint num = pResult->getInt(2);

            if (race == 0)
                slayerSum = num;
            else if (race == 1)
                vampireSum = num;
            else if (race == 2)
                oustersSum = num;
        }

        PreparedStatement selectShrineInfoStmt(pConn, "SELECT ID, OwnerRace FROM ShrineInfo");
        pResult = selectShrineInfoStmt.execute();

        while (pResult->next()) {
            uint id = pResult->getInt(1);
            uint ownerRace = pResult->getInt(2);

            if (ownerRace == 0)
                slayerOld = slayerOld + itos(id) + "|";
            else if (ownerRace == 1)
                vampireOld = vampireOld + itos(id) + "|";
            else if (ownerRace == 2)
                oustersOld = oustersOld + itos(id) + "|";
        }

        PreparedStatement insertRaceWarHistoryStmt(
            pConn, "INSERT INTO RaceWarHistory (RaceWarID, SlayerNum, VampireNum, OustersNum, SlayerOldBloodBible, "
                   "VampireOldBloodBible, OustersOldBloodBible) VALUES (?, ?, ?, ?, ?, ?, ?)");
        insertRaceWarHistoryStmt.bindString(1, getWarStartTime().toStringforWeb());
        insertRaceWarHistoryStmt.bindInt(2, slayerSum);
        insertRaceWarHistoryStmt.bindInt(3, vampireSum);
        insertRaceWarHistoryStmt.bindInt(4, oustersSum);
        insertRaceWarHistoryStmt.bindString(5, slayerOld);
        insertRaceWarHistoryStmt.bindString(6, vampireOld);
        insertRaceWarHistoryStmt.bindString(7, oustersOld);
        insertRaceWarHistoryStmt.execute();
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
void RaceWar::executeEnd()

{
    __BEGIN_TRY

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    sendWarEndMessage();

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------


    //	g_pHolyLandRaceBonus->refresh();

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    g_pShrineInfoManager->returnAllBloodBible();

    g_pShrineInfoManager->addAllShrineShield();

    g_pCastleInfoManager->resetAllSafeZone();

    g_pCastleInfoManager->transportAllOtherRace();

    // g_pCastleInfoManager->loadAllNPCs();


    // g_pHolyLandManager->sendBloodBibleStatus();
    g_pShrineInfoManager->broadcastBloodBibleStatus();


    g_pHolyLandManager->resumeTimeband();


    RaceWarLimiter::clearPCList();


    RaceWarLimiter::getInstance()->clearCurrent();
    RegenZoneManager::getInstance()->deleteTryingPosition();
    RegenZoneManager::getInstance()->reload();


    g_pZoneGroupManager->removeFlag(Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET);

    CGSayHandler::opworld(NULL, "*world *load blood_bible_owner", 0, true);


    g_pDragonEyeManager->removeAllDragonEyes();


    recordRaceWarEnd();

    __END_CATCH
}

void RaceWar::recordRaceWarEnd()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectShrineInfoStmt(pConn, "SELECT ID, OwnerRace FROM ShrineInfo");
        Result* pResult = selectShrineInfoStmt.execute();

        string slayerNew;
        string vampireNew;
        string oustersNew;

        while (pResult->next()) {
            uint id = pResult->getInt(1);
            uint ownerRace = pResult->getInt(2);

            if (ownerRace == 0)
                slayerNew = slayerNew + itos(id) + "|";
            else if (ownerRace == 1)
                vampireNew = vampireNew + itos(id) + "|";
            else if (ownerRace == 2)
                oustersNew = oustersNew + itos(id) + "|";
        }

        PreparedStatement updateRaceWarHistoryStmt(
            pConn, "UPDATE RaceWarHistory SET SlayerBloodBible = ?, VampireBloodBible = ?, "
                   "OustersBloodBible = ? WHERE RaceWarID = ?");
        updateRaceWarHistoryStmt.bindString(1, slayerNew);
        updateRaceWarHistoryStmt.bindString(2, vampireNew);
        updateRaceWarHistoryStmt.bindString(3, oustersNew);
        updateRaceWarHistoryStmt.bindString(4, getWarStartTime().toStringforWeb());
        updateRaceWarHistoryStmt.execute();
    }
    END_DB(pStmt)


    char cmd[100];
    sprintf(cmd, "/home/darkeden/vs/bin/script/recordRaceWarHistory.py %s %d %d ",
            getWarStartTime().toStringforWeb().c_str(), g_pConfig->getPropertyInt("Dimension"),
            g_pConfig->getPropertyInt("WorldID"));

    filelog("script.log", cmd);
    system(cmd);

    __END_CATCH
}

string RaceWar::getWarName() const

{
    __BEGIN_TRY

    return "";

    __END_CATCH
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void RaceWar::sendWarEndMessage() const

{
    __BEGIN_TRY

    War::sendWarEndMessage();


    GCNoticeEvent gcNoticeEvent;
    gcNoticeEvent.setCode(NOTICE_EVENT_RACE_WAR_OVER);
    g_pZoneGroupManager->broadcast(&gcNoticeEvent);

    __END_CATCH
}


void RaceWar::makeWarScheduleInfo(WarScheduleInfo* pWSI) const

{
    __BEGIN_TRY

    pWSI->warType = getWarType();
    //    pWSI->challengerGuildID		= 0;
    //   pWSI->challengerGuildName	= "";

    __END_CATCH
}

void RaceWar::makeWarInfo(WarInfo* pWarInfo) const

{
    __BEGIN_TRY

    Assert(pWarInfo != NULL);
    Assert(pWarInfo->getWarType() == WAR_RACE);

    RaceWarInfo* pRaceWarInfo = dynamic_cast<RaceWarInfo*>(pWarInfo);
    Assert(pRaceWarInfo != NULL);

    const unordered_map<ZoneID_t, CastleInfo*>& castleInfos = g_pCastleInfoManager->getCastleInfos();

    unordered_map<ZoneID_t, CastleInfo*>::const_iterator itr = castleInfos.begin();

    for (; itr != castleInfos.end(); itr++) {
        CastleInfo* pCastleInfo = itr->second;
        pRaceWarInfo->addCastleID(pCastleInfo->getZoneID());
    }

    __END_CATCH
}


string RaceWar::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "RaceWar("
        << "WarID:" << (int)getWarID() << ",State:" << (int)getState() << ")";

    return msg.toString();

    __END_CATCH
}
