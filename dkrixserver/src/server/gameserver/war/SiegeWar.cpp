///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

#include "SiegeWar.h"

#include <stdio.h>

#include "Assert.h"
#include "CGSay.h"
#include "CastleInfoManager.h"
#include "DB.h"
#include "GCNPCResponse.h"
#include "GCNoticeEvent.h"
#include "GCSystemMessage.h"
#include "GCWarScheduleList.h"
#include "GGCommand.h"
#include "GameServerInfoManager.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildWarInfo.h"
#include "HolyLandRaceBonus.h"
#include "LoginServerManager.h"
#include "Mutex.h"
#include "PCManager.h"
#include "Properties.h"
#include "SiegeManager.h"
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
SiegeWar::SiegeWar(ZoneID_t castleZoneID, WarState warState, WarID_t warID)
    : War(warState, warID), m_CastleZoneID(castleZoneID), m_RegistrationFee(0) {
    m_bModifyCastleOwner = false;
    for (int i = 0; i < 5; ++i)
        m_ChallangerGuildID[i] = 0;

    m_ChallangerGuildCount = 0;
    m_ReinforceGuildID = 0;
    m_RecentReinforceCandidate = 0;
}

SiegeWar::~SiegeWar() {}

int SiegeWar::getGuildSide(GuildID_t guildID) const {
    if (guildID == m_ReinforceGuildID)
        return 2;

    for (int i = 0; i < 5; ++i) {
        if (m_ChallangerGuildID[i] == guildID)
            return 3 + i;
    }

    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
    Assert(pCastleInfo != NULL);

    GuildID_t OwnerGuildID = pCastleInfo->getGuildID();
    if (guildID == OwnerGuildID)
        return 1;

    return 0;
}

bool SiegeWar::addChallengerGuild(GuildID_t gID) {
    if (m_ChallangerGuildCount > 5)
        return false;
    if (gID == 0 || gID == 66 || gID == 99)
        return false;

    m_ChallangerGuildID[m_ChallangerGuildCount++] = gID;
    return true;
}

//--------------------------------------------------------------------------------
//
// executeStart
//
//--------------------------------------------------------------------------------

//


//--------------------------------------------------------------------------------
void SiegeWar::executeStart()

{
    __BEGIN_TRY

    sendWarStartMessage();
    clearReinforceRegisters();

    
    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
    Assert(pCastleInfo != NULL);

    ZoneID_t siegeZoneID = SiegeManager::Instance().getSiegeZoneID(m_CastleZoneID);
    Assert(siegeZoneID != 0);
     

    SiegeManager::Instance().start(siegeZoneID);

    
    recordSiegeWarStart();

    __END_CATCH
}

void SiegeWar::recordSiegeWarStart()

{
    __BEGIN_TRY

     

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
// executeEnd
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void SiegeWar::executeEnd()

{
    __BEGIN_TRY

    //----------------------------------------------------------------------------
    
    //----------------------------------------------------------------------------
    sendWarEndMessage();

    if (m_bModifyCastleOwner) {
        g_pCastleInfoManager->modifyCastleOwner(m_CastleZoneID, m_WinnerRace, m_WinnerGuildID);

        /*		if ( g_pConfig->getPropertyInt("IsNetMarble") == 1 )
                {
                    char sCommand[100];
                    sprintf( sCommand, "*world *command setCastleOwnerGuild %u %u", m_CastleZoneID, m_WinnerGuildID );
                    CGSayHandler::opworld( NULL, sCommand, 0, true );
                }*/

        char sCommand[100];
        sprintf(sCommand, "*command setCastleOwnerGuild %u %u", m_CastleZoneID, m_WinnerGuildID);
        GGCommand ggCommand;
        ggCommand.setCommand(sCommand);

        static int myWorldID = g_pConfig->getPropertyInt("WorldID");
        static int myServerID = g_pConfig->getPropertyInt("ServerID");

        HashMapGameServerInfo* pInfos = g_pGameServerInfoManager->getGameServerInfos()[myWorldID];
        int maxServerGroupID = g_pGameServerInfoManager->getMaxServerGroupID();

        for (int groupID = 0; groupID < maxServerGroupID; ++groupID) {
            HashMapGameServerInfo& gameServerInfo = pInfos[groupID];

            if (!gameServerInfo.empty()) {
                HashMapGameServerInfo::const_iterator itr = gameServerInfo.begin();
                for (; itr != gameServerInfo.end(); itr++) {
                    GameServerInfo* pGameServerInfo = itr->second;

                    if (pGameServerInfo->getWorldID() == myWorldID) {
                        
                        if (pGameServerInfo->getGroupID() == myServerID) {
                        } else if (pGameServerInfo->getCastleFollowingServerID() == myServerID) {
                            g_pLoginServerManager->sendPacket(pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(),
                                                              &ggCommand);
                            cout << "send change castle packet to " << pGameServerInfo->getIP() << ", "
                                 << pGameServerInfo->getUDPPort() << endl;
                        }
                    }
                }
            }
        }
    } else {
        
        CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
        m_WinnerGuildID = pCastleInfo->getGuildID();
    }

    //----------------------------------------------------------------------------
    
    
    //----------------------------------------------------------------------------
    g_pCastleInfoManager->increaseTaxBalance(m_CastleZoneID, m_RegistrationFee);
    m_RegistrationFee = 0;
    

    ZoneID_t siegeZoneID = SiegeManager::Instance().getSiegeZoneID(m_CastleZoneID);
    Assert(siegeZoneID != 0);

    filelog("SiegeWar.log", "[%u] executeEnd : reset zone %u", getWarID(), siegeZoneID);
    SiegeManager::Instance().reset(siegeZoneID);

    
    recordSiegeWarEnd();

    __END_CATCH
}

void SiegeWar::recordSiegeWarEnd()

    {__BEGIN_TRY

          
         __END_CATCH}

string SiegeWar::getWarName() const

{
    __BEGIN_TRY

    return "";

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
//	isModifyCastleOwner( PlayerCreature* pPC )
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
bool SiegeWar::isModifyCastleOwner(PlayerCreature* pPC)

{
    __BEGIN_TRY

    Assert(pPC != NULL);

    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
    Assert(pCastleInfo != NULL);

    if (pPC->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_1))
        return true;
    if (pPC->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_2))
        return true;
    if (pPC->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_3))
        return true;
    if (pPC->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_4))
        return true;
    if (pPC->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_5))
        return true;

    return false;

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
// getWinnerGuildID( PlayerCreature* pPC )
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
GuildID_t SiegeWar::getWinnerGuildID(PlayerCreature* pPC)

{
    __BEGIN_TRY

    Assert(pPC != NULL);

    
    
    
    //	CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo( m_CastleZoneID );
    //	Assert( pCastleInfo!=NULL );

    return pPC->getGuildID();

    __END_CATCH
}

bool SiegeWar::endWar(PlayerCreature* pPC)

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
void SiegeWar::sendWarEndMessage() const

{
    __BEGIN_TRY

    War::sendWarEndMessage();

    
    GCNoticeEvent gcNoticeEvent;
    gcNoticeEvent.setCode(NOTICE_EVENT_WAR_OVER);
    gcNoticeEvent.setParameter(m_CastleZoneID);
    g_pZoneGroupManager->broadcast(&gcNoticeEvent);

    __END_CATCH
}

void SiegeWar::makeWarScheduleInfo(WarScheduleInfo* pWSI) const

{
    __BEGIN_TRY

    pWSI->warType = getWarType();

    for (int i = 0; i < 5; ++i) {
        pWSI->challengerGuildID[i] = m_ChallangerGuildID[i];
        if (m_ChallangerGuildID[i] != 0)
            pWSI->challengerGuildName[i] = g_pGuildManager->getGuildName(m_ChallangerGuildID[i]);
    }

    pWSI->reinforceGuildID = m_ReinforceGuildID;
    if (m_ReinforceGuildID != 0)
        pWSI->reinforceGuildName = g_pGuildManager->getGuildName(m_ReinforceGuildID);

    __END_CATCH
}

void SiegeWar::makeWarInfo(WarInfo* pWarInfo) const

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

    pGuildWarInfo->addJoinGuild(ownGuildID); 

    for (uint i = 0; i < m_ChallangerGuildCount; ++i)
        pGuildWarInfo->addJoinGuild(m_ChallangerGuildID[i]);

    pGuildWarInfo->setCastleID(getCastleZoneID());

    
    static const string commonGuild("");

    string attackGuildName;
    string defenseGuildName;

    attackGuildName = g_pGuildManager->getGuildName(m_ChallangerGuildID[0]);
    if (m_ChallangerGuildCount > 1) {
        char buffer[40];
        snprintf(buffer, 40, "%s %u", attackGuildName.c_str(), m_ChallangerGuildCount - 1);
        attackGuildName = buffer;
    }

    if (pCastleInfo->isCommon())
        defenseGuildName = commonGuild;
    else
        defenseGuildName = g_pGuildManager->getGuildName(ownGuildID);

    pGuildWarInfo->setAttackGuildName(attackGuildName);
    pGuildWarInfo->setDefenseGuildName(defenseGuildName);

    __END_CATCH
}


string SiegeWar::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "SiegeWar("
        << "WarID:" << (int)getWarID() << ",State:" << (int)getState() << ",CastleZoneID:" << (int)m_CastleZoneID
        << ",WarType:" << getWarType2DBString() << ",ChallengerGuildID:" << (int)m_ChallangerGuildID[0]
        << ",RegistrationFee:" << (int)m_RegistrationFee << ")";

    return msg.toString();

    __END_CATCH
}

BYTE SiegeWar::canReinforce(GuildID_t gID) {
    if (m_ReinforceGuildID != 0)
        return NPC_RESPONSE_ALREADY_REINFORCE_ACCEPTED;
    if (g_pGuildManager->hasWarSchedule(gID))
        return NPC_RESPONSE_WAR_ALREADY_REGISTERED;

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("Darkeden")->createStatement();
        Result* pResult = pStmt->executeQuery(
            "SELECT COUNT(*) FROM ReinforceRegisterInfo WHERE WarID=%u AND ServerID=%u AND Status='WAIT'", getWarID(),
            g_pConfig->getPropertyInt("ServerID"));

        if (pResult->next()) {
            if (pResult->getInt(1) > 3)
                return NPC_RESPONSE_TOO_MANY_GUILD_REGISTERED;
        }

        pResult = pStmt->executeQuery("SELECT COUNT(*) FROM ReinforceRegisterInfo WHERE WarID=%u AND ServerID=%u AND "
                                      "ReinforceGuildID=%u AND Status='DENY'",
                                      getWarID(), g_pConfig->getPropertyInt("ServerID"), gID);

        if (pResult->next()) {
            if (pResult->getInt(1) > 0)
                return NPC_RESPONSE_REINFORCE_DENYED;
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    return NPC_RESPONSE_WAR_REGISTRATION_OK;
}

GuildID_t SiegeWar::recentReinforceGuild() {
    GuildID_t ret = 0;
    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("Darkeden")->createStatement();
        Result* pResult = pStmt->executeQuery(
            "SELECT ReinforceGuildID FROM ReinforceRegisterInfo WHERE WarID=%u AND ServerID=%u AND Status='WAIT'",
            getWarID(), g_pConfig->getPropertyInt("ServerID"));

        if (pResult->next()) {
            ret = pResult->getInt(1);
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    m_RecentReinforceCandidate = ret;

    return ret;
}

BYTE SiegeWar::registerReinforce(GuildID_t gID) {
    BYTE ret = canReinforce(gID);
    if (ret != NPC_RESPONSE_WAR_REGISTRATION_OK)
        return ret;

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("Darkeden")->createStatement();
        pStmt->executeQuery("INSERT INTO ReinforceRegisterInfo (WarID, ServerID, ReinforceGuildID, Status) VALUES "
                            "(%u, %u, %u, 'WAIT')",
                            getWarID(), g_pConfig->getPropertyInt("ServerID"), gID);

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    return ret;
}

bool SiegeWar::acceptReinforce() {
    bool ret = false;
    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("Darkeden")->createStatement();
        pStmt->executeQuery(
            "UPDATE ReinforceRegisterInfo SET Status='ACCEPT' WHERE WarID=%u AND ServerID=%u AND ReinforceGuildID=%u",
            getWarID(), g_pConfig->getPropertyInt("ServerID"), m_RecentReinforceCandidate);

        if (pStmt->getAffectedRowCount() > 0)
            ret = true;
        if (ret)
            m_ReinforceGuildID = m_RecentReinforceCandidate;

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    return ret;
}

bool SiegeWar::denyReinforce() {
    bool ret = false;
    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("Darkeden")->createStatement();
        pStmt->executeQuery(
            "UPDATE ReinforceRegisterInfo SET Status='DENY' WHERE WarID=%u AND ServerID=%u AND ReinforceGuildID=%u",
            getWarID(), g_pConfig->getPropertyInt("ServerID"), m_RecentReinforceCandidate);

        if (pStmt->getAffectedRowCount() > 0)
            ret = true;

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    return ret;
}

void SiegeWar::clearReinforceRegisters() {
    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("Darkeden")->createStatement();
        pStmt->executeQuery("DELETE FROM ReinforceRegisterInfo WHERE WarID=%u AND ServerID=%u", getWarID(),
                            g_pConfig->getPropertyInt("ServerID"));

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)
}
