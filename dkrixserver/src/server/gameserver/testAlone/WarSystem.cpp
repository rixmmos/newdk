#include "WarSystem.h"

#include <stdio.h>

#include <algorithm>

#include "Assert.h"
#include "CastleInfoManager.h"
#include "ClientManager.h"
#include "EventRefreshHolyLandPlayer.h"
#include "GamePlayer.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCWarList.h"
#include "Gpackets/GCWarScheduleList.h"
#include "GuildWar.h"
#include "GuildWarInfo.h"
#include "HolyLandManager.h"
#include "Player.h"
#include "RaceWar.h"
#include "RaceWarInfo.h"
#include "ShrineInfoManager.h"
#include "SiegeWar.h"
#include "StringPool.h"
#include "StringStream.h"
#include "VariableManager.h"
#include "War.h"
#include "WarSchedule.h"
#include "WarScheduler.h"
#include "ZoneGroupManager.h"

WarID_t WarSystem::s_WarIDSuccessor = 0;

WarSystem* g_pWarSystem = NULL;

WarSystem::WarSystem() throw(Error) {
    __BEGIN_TRY

    m_Mutex.setName("WarSystem");
    m_MutexWarQueue.setName("WarSystemQueue");
    m_MutexActiveWars.setName("ActiveWars");
    m_MutexWarList.setName("WarList");

    m_bHasRaceWar = false;

    m_pRaceWarSchedule = NULL;

    __END_CATCH
}

WarSystem::~WarSystem() throw(Error) {
    __BEGIN_TRY

    SAFE_DELETE(m_pRaceWarSchedule);

    __END_CATCH
}

void WarSystem::init() throw(Error) {
    __BEGIN_TRY

    load();

    prepareRaceWar();

    __END_CATCH
}

void WarSystem::prepareRaceWar() throw(Error) {
    __BEGIN_TRY

    SAFE_DELETE(m_pRaceWarSchedule);

    
    VSDateTime warStartTime = WarScheduler::getNextWarDateTime(WAR_RACE, VSDateTime::currentDateTime());

    War* pRaceWar = new RaceWar(War::WAR_STATE_WAIT);
    pRaceWar->setWarStartTime(warStartTime);
    m_pRaceWarSchedule = new Schedule(pRaceWar, warStartTime);

    filelog("WarLog.txt", "[WarID=%d,Time=%s]   .", (int)pRaceWar->getWarID(),
            warStartTime.toString().c_str());

    __END_CATCH
}


void WarSystem::load() throw (Error){__BEGIN_TRY

                                         

                                         

                                         __END_CATCH}

VSDateTime WarSystem::getWarEndTime(WarType_t warType) const {
    int seconds = 0;
    switch (warType) {
    
    case WAR_GUILD:
        seconds = g_pVariableManager->getVariable(GUILD_WAR_TIME);
        break;

    
    case WAR_RACE:
        seconds = g_pVariableManager->getVariable(RACE_WAR_TIME);
        break;
    }

    VSDateTime dt(VSDateTime::currentDateTime());

    return dt.addSecs(seconds);
}

bool WarSystem::addWarDelayed(War* pWar) throw(Error) {
    __BEGIN_TRY

    Assert(pWar != NULL);

    if (hasActiveRaceWar() && pWar->getWarType() == WAR_RACE) {
        throw Error("   .");
    }

    __ENTER_CRITICAL_SECTION(m_MutexWarQueue);

    m_WarQueue.push_back(pWar);

    __LEAVE_CRITICAL_SECTION(m_MutexWarQueue);

    return true;

    __END_CATCH
}

bool WarSystem::addQueuedWar() throw(Error) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_MutexWarQueue);

    while (!m_WarQueue.empty()) {
        War* pWar = m_WarQueue.front();
        Assert(pWar != NULL);

        m_WarQueue.pop_front();

        addWar(pWar);
    }

    __LEAVE_CRITICAL_SECTION(m_MutexWarQueue);

    return true;

    __END_CATCH
}


bool WarSystem::addWar(War* pWar) throw(Error) {
    __BEGIN_TRY

    Assert(pWar != NULL);
    Assert(pWar->getState() == War::WAR_STATE_CURRENT);

    WarType_t warType = pWar->getWarType();
    VSDateTime warEndTime = getWarEndTime(warType);

    Schedule::ScheduleType scheduleType;

    scheduleType = Schedule::SCHEDULE_TYPE_ONCE;

    WarSchedule* pWarSchedule = new WarSchedule(pWar, warEndTime, scheduleType);

    addSchedule(pWarSchedule);

    
    if (makeGCWarList_LOCKED()) {
        GCWarList gcWarList;

        __ENTER_CRITICAL_SECTION(m_MutexWarList)

        gcWarList = m_GCWarList;

        __LEAVE_CRITICAL_SECTION(m_MutexWarList)

        g_pZoneGroupManager->broadcast(&gcWarList);
    }

    
    // pWarSchedule->create();

    
    
    if (pWar->getWarType() == WAR_GUILD) {
        SuildWar* pSiegeWar = dynamic_cast<SiegeWar*>(pWar);
        Assert(pSiegeWar != NULL);

        
        EventRefreshHolyLandPlayer* pEvent = new EventRefreshHolyLandPlayer(NULL);
        pEvent->setDeadline(0);
        g_pClientManager->addEvent(pEvent);

        __ENTER_CRITICAL_SECTION(m_MutexActiveWars)

        m_ActiveWars.push_back(ActiveWarInfo(pSiegeWar->getCastleZoneID(), pSiegeWar->getChallangerGuildID()));

        __LEAVE_CRITICAL_SECTION(m_MutexActiveWars)
    } else if (pWar->getWarType() == WAR_RACE) {
        m_bHasRaceWar = true;

        
        EventRefreshHolyLandPlayer* pEvent = new EventRefreshHolyLandPlayer(NULL);
        pEvent->setDeadline(0);
        g_pClientManager->addEvent(pEvent);

        
        g_pShrineInfoManager->broadcastBloodBibleStatus();

        
        g_pHolyLandManager->remainRaceWarPlayers();
    }

    return true;

    __END_CATCH
}


bool WarSystem::makeGCWarList_LOCKED() throw(Error) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_MutexWarList)

    m_GCWarList.clear();

    if (isEmpty()) {
        m_MutexWarList.unlock();
        return false;
    }

    const RecentSchedules::container_type& Schedules = m_RecentSchedules.getSchedules();
    RecentSchedules::const_iterator itr = Schedules.begin();

    for (; itr != Schedules.end(); itr++) {
        Schedule* pSchedule = *itr;

        WarSchedule* pWarSchedule = dynamic_cast<WarSchedule*>(pSchedule);
        Assert(pWarSchedule != NULL);

        War* pWar = pWarSchedule->getWar();
        Assert(pWar != NULL);

        WarInfo* pWarInfo = NULL;

        switch (pWar->getWarType()) {
        case WAR_GUILD:
            pWarInfo = new GuildWarInfo;
            break;
        case WAR_RACE:
            pWarInfo = new RaceWarInfo;
            break;
        default:
            throw Error("WarType .");
        }

        pWarSchedule->makeWarInfo(pWarInfo);

        m_GCWarList.addWarInfo(pWarInfo);
    }

    __LEAVE_CRITICAL_SECTION(m_MutexWarList)

    return true;

    __END_CATCH
}

bool WarSystem::makeGCWarList() throw(Error) {
    __BEGIN_TRY

    bool ret = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    ret = makeGCWarList_LOCKED();

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return ret;

    __END_CATCH

    return false;
}

void WarSystem::sendGCWarList(Player* pPlayer) throw(Error) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_MutexWarList)

    if (!m_GCWarList.isEmpty()) {
        pPlayer->sendPacket(&m_GCWarList);
        // cout << m_GCWarList.toString().c_str() << endl;
    }

    __LEAVE_CRITICAL_SECTION(m_MutexWarList)

    __END_CATCH
}

Work* WarSystem::heartbeat() throw(Error) {
    __BEGIN_TRY

    Work* pWork = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    addQueuedWar();

    pWork = Scheduler::heartbeat();

    if (pWork != NULL) {
        
        War* pWar = dynamic_cast<War*>(pWork);
        Assert(pWar != NULL);

        
        if (pWar->getWarType() == WAR_GUILD) {
            SiegeWar* pSiegeWar = dynamic_cast<SiegeWar*>(pWar);
            Assert(pSiegeWar != NULL);

            
            __ENTER_CRITICAL_SECTION(m_MutexActiveWars)

            list<ActiveWarInfo>::iterator itr =
                find(m_ActiveWars.begin(), m_ActiveWars.end(), ActiveWarInfo(pSiegeWar->getCastleZoneID()));
            Assert(itr != m_ActiveWars.end());

            m_ActiveWars.erase(itr);

            __LEAVE_CRITICAL_SECTION(m_MutexActiveWars)
        } else if (pWar->getWarType() == WAR_RACE) {
            m_bHasRaceWar = false;

            
            EventRefreshHolyLandPlayer* pEvent = new EventRefreshHolyLandPlayer(NULL);
            pEvent->setDeadline(0);
            g_pClientManager->addEvent(pEvent);
        }

        SAFE_DELETE(pWork);
    }

    
    if (m_pRaceWarSchedule != NULL && !m_bHasRaceWar && g_pVariableManager->isAutoStartRaceWar()) {
        checkStartRaceWar();
    }

    
    static Timeval nextTime = {0, 0};
    Timeval currentTime;
    getCurrentTime(currentTime);

    if (currentTime > nextTime) {
        makeGCWarList_LOCKED();
        nextTime.tv_sec = currentTime.tv_sec + 10;
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pWork;

    __END_CATCH
}

bool WarSystem::checkStartRaceWar() throw(Error) {
    __BEGIN_TRY

    if (m_pRaceWarSchedule->heartbeat()) {
        Work* pWork = m_pRaceWarSchedule->popWork();
        Assert(pWork != NULL);

        War* pRaceWar = dynamic_cast<War*>(pWork);
        addWarDelayed(pRaceWar);

        prepareRaceWar();

        return true;
    }

    return false;

    __END_CATCH
}

bool WarSystem::getAttackGuildID(ZoneID_t zoneID, GuildID_t& guildID) const throw(Error) {
    __BEGIN_TRY

    bool bHasCastleActiveWar = false;

    __ENTER_CRITICAL_SECTION(m_MutexActiveWars)

    list<ActiveWarInfo>::const_iterator itr = find(m_ActiveWars.begin(), m_ActiveWars.end(), ActiveWarInfo(zoneID));

    if (itr != m_ActiveWars.end()) {
        bHasCastleActiveWar = true;
        guildID = (*itr).AttackGuildID;
    }

    __LEAVE_CRITICAL_SECTION(m_MutexActiveWars)

    return bHasCastleActiveWar;

    __END_CATCH
}

bool WarSystem::hasCastleActiveWar(ZoneID_t zoneID) const throw(Error) {
    __BEGIN_TRY

    bool bHasCastleActiveWar = false;

    __ENTER_CRITICAL_SECTION(m_MutexActiveWars)

    list<ActiveWarInfo>::const_iterator itr = find(m_ActiveWars.begin(), m_ActiveWars.end(), ActiveWarInfo(zoneID));

    if (itr != m_ActiveWars.end()) {
        bHasCastleActiveWar = true;
    }

    __LEAVE_CRITICAL_SECTION(m_MutexActiveWars)

     

    return bHasCastleActiveWar;

    __END_CATCH
}

WarSchedule* WarSystem::getActiveWarSchedule(ZoneID_t zoneID) throw(Error) {
    __BEGIN_TRY

    WarSchedule* pWarSchedule = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    pWarSchedule = getActiveWarSchedule_LOCKED(zoneID);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pWarSchedule;

    __END_CATCH
}


WarSchedule* WarSystem::getActiveWarSchedule_LOCKED(ZoneID_t zoneID) throw(Error) {
    __BEGIN_TRY

    const RecentSchedules::container_type& schedules = m_RecentSchedules.getSchedules();
    RecentSchedules::const_iterator itr = schedules.begin();

    for (; itr != schedules.end(); itr++) {
        WarSchedule* pWarSchedule = dynamic_cast<WarSchedule*>(*itr);
        Assert(pWarSchedule != NULL);

        War* pWar = dynamic_cast<War*>(pWarSchedule->getWork());
        if (pWar == NULL) {
            cout << "WarSystem  Schedule Work War  NULL. ~~~~" << endl;
            continue;
        }

        if (pWar->getWarType() == WAR_GUILD) {
            SiegeWar* pSiegeWar = dynamic_cast<SiegeWar*>(pWar);
            Assert(pSiegeWar != NULL);

            if (pSiegeWar->getCastleZoneID() == zoneID) {
                return pWarSchedule;
            }
        }
    }

    return NULL;

    __END_CATCH
}

War* WarSystem::getActiveWar(ZoneID_t zoneID) const throw(Error) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    const RecentSchedules::container_type& schedules = m_RecentSchedules.getSchedules();
    RecentSchedules::const_iterator itr = schedules.begin();

    for (; itr != schedules.end(); itr++) {
        War* pWar = dynamic_cast<War*>((*itr)->getWork());
        if (pWar == NULL)
            continue;

        if (pWar->getWarType() == WAR_GUILD) {
            SiegeWar* pSiegeWar = dynamic_cast<SiegeWar*>(pWar);
            Assert(pSiegeWar != NULL);

            if (pSiegeWar->getCastleZoneID() == zoneID) {
                m_Mutex.unlock();
                return pWar;
            }
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return NULL;

    __END_CATCH
}

bool WarSystem::isEndCondition(Item* pItem, MonsterCorpse* pMonsterCorpse) throw(Error) {
    __BEGIN_TRY

    Assert(pItem != NULL);
    Assert(pMonsterCorpse != NULL);

    
    // return pBloodBibleItem->getBibleMonsterType()==pMonsterCorpse->getMonter()->getMonsterType()

    return true;

    __END_CATCH
}

bool WarSystem::isModifyCastleOwner(ZoneID_t castleZoneID, PlayerCreature* pPC) throw(Error) {
    __BEGIN_TRY

    War* pWar = getActiveWar(castleZoneID);
    Assert(pWar != NULL);

    return pWar->isModifyCastleOwner(pPC);

    __END_CATCH
}


bool WarSystem::endWar(PlayerCreature* pPC, ZoneID_t castleZoneID) throw(Error) {
    __BEGIN_TRY

    Assert(pPC != NULL);

    bool bEndWar = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    WarSchedule* pWarSchedule = getActiveWarSchedule_LOCKED(castleZoneID);

    if (pWarSchedule != NULL) {
        Work* pWork = pWarSchedule->getWork();
        Assert(pWork != NULL);

        War* pWar = dynamic_cast<War*>(pWork);
        Assert(pWar != NULL);

        if (pWar->endWar(pPC)) {
            
            pWarSchedule->setScheduledTime(VSDateTime::currentDateTime());

            
            m_RecentSchedules.arrange();

            bEndWar = true;
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return bEndWar;

    __END_CATCH
}


bool WarSystem::removeWar(ZoneID_t castleZoneID) throw(Error) {
    __BEGIN_TRY

    bool bRemoved = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    WarSchedule* pWarSchedule = getActiveWarSchedule_LOCKED(castleZoneID);

    if (pWarSchedule != NULL) {
        
        pWarSchedule->setScheduledTime(VSDateTime::currentDateTime());

        
        m_RecentSchedules.arrange();

        bRemoved = true;
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return bRemoved;

    __END_CATCH
}


bool WarSystem::removeRaceWar() throw(Error) {
    __BEGIN_TRY

    bool bRemoved = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    const RecentSchedules::container_type& schedules = m_RecentSchedules.getSchedules();
    RecentSchedules::const_iterator itr = schedules.begin();

    for (; itr != schedules.end(); itr++) {
        Schedule* pSchedule = *itr;
        War* pWar = dynamic_cast<War*>(pSchedule->getWork());
        if (pWar == NULL)
            continue;

        if (pWar->getWarType() == WAR_RACE) {
            
            pSchedule->setScheduledTime(VSDateTime::currentDateTime());

            
            m_RecentSchedules.arrange();

            bRemoved = true;
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return bRemoved;

    __END_CATCH
}


void WarSystem::broadcastWarList(GamePlayer* pGamePlayer) const throw(Error) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    const RecentSchedules::container_type& schedules = m_RecentSchedules.getSchedules();
    RecentSchedules::const_iterator itr = schedules.begin();

    GCSystemMessage gcSystemMessage;
    bool warExist = false;

    if (isEmpty()) {
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NO_WAR_IN_ACTIVE));
        pGamePlayer->sendPacket(&gcSystemMessage);

        m_Mutex.unlock();
        return;
    }

    for (; itr != schedules.end(); itr++) {
        WarSchedule* pSchedule = dynamic_cast<WarSchedule*>(*itr);
        if (pSchedule == NULL)
            continue;

        War* pWar = dynamic_cast<War*>(pSchedule->getWork());
        if (pWar == NULL)
            continue;

        warExist = true;

         

        char msg[100];
        sprintf(msg, g_pStringPool->c_str(STRID_WAR_STATUS), pWar->getWarName().c_str(),
                (pSchedule->getScheduledTime()).toString().c_str());
        gcSystemMessage.setMessage(msg);
        pGamePlayer->sendPacket(&gcSystemMessage);
    }

    if (!warExist) {
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NO_WAR_IN_ACTIVE));
        pGamePlayer->sendPacket(&gcSystemMessage);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

War* WarSystem::getActiveRaceWar() const throw(Error) {
    __BEGIN_TRY

    War* pWar = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    pWar = getActiveRaceWarAtSameThread();

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pWar;

    __END_CATCH
}

bool WarSystem::startRaceWar() throw(Error) {
    __BEGIN_TRY

    if (hasActiveRaceWar())
        return false;

    __ENTER_CRITICAL_SECTION(m_Mutex);

    if (m_pRaceWarSchedule != NULL) {
        m_pRaceWarSchedule->setScheduledTime(VSDateTime::currentDateTime());

        checkStartRaceWar();
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex);

    return true;

    __END_CATCH
}

War* WarSystem::getActiveRaceWarAtSameThread() const throw(Error) {
    __BEGIN_TRY

    const RecentSchedules::container_type& schedules = m_RecentSchedules.getSchedules();
    RecentSchedules::const_iterator itr = schedules.begin();

    for (; itr != schedules.end(); itr++) {
        War* pWar = dynamic_cast<War*>((*itr)->getWork());
        if (pWar == NULL)
            continue;
        if (pWar->getWarType() == WAR_RACE) {
            return pWar;
        }
    }

    return NULL;

    __END_CATCH
}

bool WarSystem::addRaceWarScheduleInfo(WarScheduleInfo* pWSI) throw(Error) {
    __BEGIN_TRY

    if (m_pRaceWarSchedule == NULL)
        return false;

    Assert(pWSI != NULL);

    __ENTER_CRITICAL_SECTION(m_Mutex);

    const VSDateTime& DT = m_pRaceWarSchedule->getScheduledTime();

    Work* pWork = m_pRaceWarSchedule->getWork();
    Assert(pWork != NULL);

    War* pWar = dynamic_cast<War*>(pWork);
    Assert(pWar != NULL);

    pWar->makeWarScheduleInfo(pWSI);
    pWSI->year = DT.date().year();
    pWSI->month = DT.date().month();
    pWSI->day = DT.date().day();
    pWSI->hour = DT.time().hour();

    __LEAVE_CRITICAL_SECTION(m_Mutex);

    return true;

    __END_CATCH
}
