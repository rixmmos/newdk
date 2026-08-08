#include "FlagManager.h"

#include <stdio.h>

#include <cstdlib>

#include "DB.h"
#include "EffectFlagInsert.h"
#include "EffectManager.h"
#include "FlagWar.h"
#include "GCAddEffect.h"
#include "GCDeleteInventoryItem.h"
#include "GCFlagWarStatus.h"
#include "MonsterCorpse.h"
#include "NPCInfo.h"
#include "NewbieFlagWar.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "SystemAvailabilitiesManager.h"
#include "Zone.h"
#include "ZoneUtil.h"

FlagManager* g_pFlagManager = NULL;

FlagManager::FlagManager() {
    m_Mutex.setName("FlagManager");
    m_FlagCount.clear();

    m_StatusPacket.setTimeRemain(0);
    m_StatusPacket.setFlagCount(RACE_SLAYER, 0);
    m_StatusPacket.setFlagCount(RACE_VAMPIRE, 0);
    m_StatusPacket.setFlagCount(RACE_OUSTERS, 0);

    m_PutTime[RACE_SLAYER] = m_PutTime[RACE_VAMPIRE] = m_PutTime[RACE_OUSTERS] = VSDateTime::currentDateTime();

    FlagWar* pFlagWar = new FlagWar();
    addSchedule(new Schedule(pFlagWar, pFlagWar->getNextFlagWarTime()));

    pFlagWar = new NewbieFlagWar();
    addSchedule(new Schedule(pFlagWar, pFlagWar->getNextFlagWarTime()));
}

FlagManager::~FlagManager() {}

void FlagManager::init() {
    SYSTEM_RETURN_IF_NOT(SYSTEM_FLAG_WAR);
    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectFlagPoleStmt(
            pConn, "SELECT ZoneID, CenterX, CenterY, Width, Height, Race-1, MonsterType FROM FlagPolePosition");
        pResult = selectFlagPoleStmt.execute();

        while (pResult->next()) {
            ZoneID_t zoneID = (ZoneID_t)pResult->getInt(1);
            Zone* pZone = getZoneByZoneID(zoneID);

            ZoneCoord_t left = (ZoneCoord_t)pResult->getInt(2);
            ZoneCoord_t top = (ZoneCoord_t)pResult->getInt(3);
            uint width = (ZoneCoord_t)pResult->getInt(4);
            uint height = (ZoneCoord_t)pResult->getInt(5);
            Race_t race = (Race_t)pResult->getInt(6);
            MonsterType_t type = (MonsterType_t)pResult->getInt(7);

            addPoleField(pZone, left, top, width, height, race, type);
        }
    }
    END_DB(pStmt)
}

void FlagManager::addPoleField(Zone* pZone, ZoneCoord_t left, ZoneCoord_t top, uint width, uint height, Race_t race,
                               MonsterType_t type) {
    Assert(pZone != NULL);
    Assert(isValidZoneCoord(pZone, left, top));
    Assert(isValidZoneCoord(pZone, left + width, top + height));

    NPCInfo* pNPCInfo = new NPCInfo();
    pNPCInfo->setName("Flag Pole");
    pNPCInfo->setNPCID(type);
    pNPCInfo->setX(left);
    pNPCInfo->setY(top);

    pZone->addNPCInfo(pNPCInfo);

    for (uint i = 0; i < width; ++i)
        for (uint j = 0; j < height; ++j) {
            MonsterCorpse* pFlagPole = new MonsterCorpse(type, "Flag Pole", 2);
            Assert(pFlagPole != NULL);

            pFlagPole->setZone(pZone);
            pFlagPole->setShrine(true);
            pZone->registerObject(pFlagPole);

            m_FlagPoles[pFlagPole] = race;

            TPOINT tp = pZone->addItem(pFlagPole, left + (i * 2), top + (j * 2));
            Assert(tp.x != -1);

            forbidDarkness(pZone, tp.x, tp.y, 1);
        }

    m_PoleFields.push_back(PoleFieldInfo(pZone->getZoneID(), left, top, width * 2, height * 2));
}

void FlagManager::manualStart() {
    if (!isEmpty()) {
        cout << " .." << endl;
        addSchedule(new Schedule(popRecentWork(), VSDateTime::currentDateTime()));
    } else {
        cout << " .." << endl;
        addSchedule(new Schedule(new FlagWar(), VSDateTime::currentDateTime()));
    }
}

bool FlagManager::startFlagWar() {
    if (m_bHasFlagWar)
        return false;
    m_bHasFlagWar = true;

    Work* pWork = m_RecentSchedules.top()->getWork();
    FlagWar* pFlagWar = dynamic_cast<FlagWar*>(pWork);

    if (pFlagWar != NULL)
        m_EndTime = VSDateTime::currentDateTime().addSecs(pFlagWar->getWarTime());
    else
        m_EndTime = VSDateTime::currentDateTime().addSecs(3600);
    //	m_EndTime = m_RecentSchedules.top()->getScheduledTime();

    resetFlagCounts();

    m_StatusPacket.setTimeRemain(remainWarTimeSecs());
    m_StatusPacket.setFlagCount(RACE_SLAYER, 0);
    m_StatusPacket.setFlagCount(RACE_VAMPIRE, 0);
    m_StatusPacket.setFlagCount(RACE_OUSTERS, 0);

    broadcastStatus();

    return true;
}

bool FlagManager::endFlagWar() {
    if (m_bHasFlagWar) {
        recordFlagWarHistory();


        char cmd[100];
        sprintf(cmd, "/home/darkeden/vs/bin/script/recordFlagWarHistory.py %s %d %d %d %d %d %d %d ",
                m_EndTime.toStringforWeb().c_str(), (int)getWinnerRace(), g_pConfig->getPropertyInt("Dimension"),
                g_pConfig->getPropertyInt("WorldID"), g_pConfig->getPropertyInt("ServerID"), m_FlagCount[SLAYER],
                m_FlagCount[VAMPIRE], m_FlagCount[OUSTERS]);

        filelog("script.log", cmd);
        system(cmd);

        m_bHasFlagWar = false;
        return true;
    }

    return false;
}

bool FlagManager::putFlag(PlayerCreature* pPC, MonsterCorpse* pFlagPole) {
    if (!isFlagPole(pFlagPole))
        return false;
    if (pPC->getRace() != m_FlagPoles[pFlagPole])
        return false;
    if (!pPC->isFlag(Effect::EFFECT_CLASS_HAS_FLAG))
        return false;

    lock();
    m_FlagCount[(RACEINDEX)(pPC->getRace())]++;
    m_StatusPacket.setFlagCount(pPC->getRace(), m_FlagCount[(RACEINDEX)(pPC->getRace())]);
    m_PutTime[pPC->getRace()] = VSDateTime::currentDateTime();
    filelog("FlagWar.log", "%s    . S : %d, V : %d, O : %d", pPC->getName().c_str(), m_FlagCount[SLAYER],
            m_FlagCount[VAMPIRE], m_FlagCount[OUSTERS]);
    unlock();

    broadcastStatus();

    return true;
}

bool FlagManager::getFlag(PlayerCreature* pPC, MonsterCorpse* pFlagPole) {
    if (!isFlagPole(pFlagPole))
        return false;
    if (pPC->getRace() == m_FlagPoles[pFlagPole])
        return false;
    if (pPC->isFlag(Effect::EFFECT_CLASS_HAS_FLAG))
        return false;
    if (m_FlagCount[(RACEINDEX)(m_FlagPoles[pFlagPole])] == 0)
        return false;

    lock();
    m_FlagCount[(RACEINDEX)(m_FlagPoles[pFlagPole])]--;
    m_StatusPacket.setFlagCount(m_FlagPoles[pFlagPole], m_FlagCount[(RACEINDEX)(m_FlagPoles[pFlagPole])]);
    filelog("FlagWar.log", "%s   . S : %d, V : %d, O : %d", pPC->getName().c_str(), m_FlagCount[SLAYER],
            m_FlagCount[VAMPIRE], m_FlagCount[OUSTERS]);
    unlock();

    broadcastStatus();

    return true;
}

bool FlagManager::putFlag(PlayerCreature* pPC, Item* pItem, MonsterCorpse* pFlagPole) {
    Assert(pItem->getObjectID() == pPC->getExtraInventorySlotItem()->getObjectID());

    if (pPC->getRace() != getFlagPoleRace(pFlagPole))
        return false;
    if (pFlagPole->getTreasureCount() != 0)
        return false;
    if (!putFlag(pPC, pFlagPole))
        return false;

    pPC->deleteItemFromExtraInventorySlot();
    GCDeleteInventoryItem gcDeleteInventoryItem;
    gcDeleteInventoryItem.setObjectID(pItem->getObjectID());

    pPC->getPlayer()->sendPacket(&gcDeleteInventoryItem);

    Effect* pEffect = pPC->findEffect(Effect::EFFECT_CLASS_HAS_FLAG);
    if (pEffect != NULL) {
        pEffect->setDeadline(0);
    }

    pFlagPole->addTreasure(pItem);

    pFlagPole->setFlag(Effect::EFFECT_CLASS_FLAG_INSERT);
    EffectFlagInsert* pFlagEffect = new EffectFlagInsert(pFlagPole);
    pFlagPole->getEffectManager().addEffect(pFlagEffect);

    GCAddEffect gcAddEffect;
    gcAddEffect.setEffectID(Effect::EFFECT_CLASS_FLAG_INSERT);
    gcAddEffect.setObjectID(pFlagPole->getObjectID());
    gcAddEffect.setDuration(65535);

    recordPutFlag(pPC, pItem);

    pFlagPole->getZone()->broadcastPacket(pFlagPole->getX(), pFlagPole->getY(), &gcAddEffect);

    return true;
}

Race_t FlagManager::getWinnerRace() {
    uint max = 0;
    RACEINDEX maxRace = SLAYER;

    map<RACEINDEX, uint>::const_iterator itr = m_FlagCount.begin();
    map<RACEINDEX, uint>::const_iterator endItr = m_FlagCount.end();

    for (; itr != endItr; ++itr) {
        if (itr->second > max) {
            maxRace = itr->first;
            max = itr->second;
        }
        if (itr->second == max) {
            if (m_PutTime[(Race_t)itr->first] > m_PutTime[(Race_t)maxRace]) {
                maxRace = itr->first;
                max = itr->second;
            }
        }
    }

    return (Race_t)maxRace;
}

/*VSDateTime FlagManager::getNextFlagWarTime()
{
}*/

void FlagManager::resetFlagCounts() {
    m_FlagCount[SLAYER] = 0;
    m_FlagCount[VAMPIRE] = 0;
    m_FlagCount[OUSTERS] = 0;


    list<PoleFieldInfo>::iterator itr = m_PoleFields.begin();
    list<PoleFieldInfo>::iterator endItr = m_PoleFields.end();
    for (; itr != endItr; ++itr) {
        Zone* pZone = getZoneByZoneID(itr->zoneID);

        ZoneCoord_t ix, iy;
        for (ix = itr->l; ix <= (itr->l + itr->w * 2); ix += 2)
            for (iy = itr->t; iy <= (itr->t + itr->h * 2); iy += 2) {
                if (!isValidZoneCoord(pZone, ix, iy))
                    continue;
                Tile& tile = pZone->getTile(ix, iy);
                Item* pCorpse = tile.getItem();
                if (pCorpse == NULL || pCorpse->getItemClass() != Item::ITEM_CLASS_CORPSE ||
                    pCorpse->getItemType() != MONSTER_CORPSE) {
                    continue;
                }

                MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pCorpse);
                Item* pItem = pMonsterCorpse->getTreasure();

                if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_EVENT_ITEM &&
                    pItem->getItemType() == 27) {
                    pZone->deleteItem(pItem, ix, iy);
                    pItem->destroy();
                    SAFE_DELETE(pItem);
                }
            }
    }


    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement deleteFlagWarStatStmt(pConn, "DELETE FROM FlagWarStat");
        pResult = deleteFlagWarStatStmt.execute();
    }
    END_DB(pStmt)
}

bool FlagManager::isInPoleField(ZONE_COORD zc) {
    list<PoleFieldInfo>::iterator itr = m_PoleFields.begin();
    list<PoleFieldInfo>::iterator endItr = m_PoleFields.end();

    for (; itr != endItr; ++itr) {
        if (itr->isInField(zc))
            return true;
    }

    return false;
}

void FlagManager::recordPutFlag(PlayerCreature* pPC, Item* pItem)

{
    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectStatStmt(pConn, "SELECT Name FROM FlagWarStat WHERE Name = ? AND ItemID = ?");
        selectStatStmt.bindString(1, pPC->getName());
        selectStatStmt.bindInt(2, pItem->getItemID());
        pResult = selectStatStmt.execute();


        if (!pResult->next()) {
            PreparedStatement insertStatStmt(
                pConn, "INSERT INTO FlagWarStat (PlayerID, Name, Race, ServerID, ItemID) VALUES (?,?,?,?,?)");
            insertStatStmt.bindString(1, pPC->getPlayer()->getID());
            insertStatStmt.bindString(2, pPC->getName());
            insertStatStmt.bindInt(3, (int)pPC->getRace());
            insertStatStmt.bindInt(4, g_pConfig->getPropertyInt("ServerID"));
            insertStatStmt.bindInt(5, pItem->getItemID());
            pResult = insertStatStmt.execute();
        }
    }
    END_DB(pStmt)
}

void FlagManager::recordFlagWarHistory()

{
    Statement* pStmt = NULL;

    Result* pResult = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectStatsStmt(
            pConn, "SELECT PlayerID, Name, Race, ServerID, count(*) FROM FlagWarStat GROUP BY Name, ServerID");
        pResult = selectStatsStmt.execute();

        PreparedStatement insertHistoryStmt(pConn,
                                            "INSERT INTO FlagWarHistory (FlagWarID, PlayerID, Name, Race, ServerID, "
                                            "FlagNum) VALUES (?,?,?,?,?,?)");

        while (pResult->next()) {
            string playerID = pResult->getString(1);
            string name = pResult->getString(2);
            Race_t race = pResult->getInt(3);
            int serverID = pResult->getInt(4);
            int num = pResult->getInt(5);

            insertHistoryStmt.bindString(1, m_EndTime.toStringforWeb());
            insertHistoryStmt.bindString(2, playerID);
            insertHistoryStmt.bindString(3, name);
            insertHistoryStmt.bindInt(4, (int)race);
            insertHistoryStmt.bindInt(5, serverID);
            insertHistoryStmt.bindInt(6, num);
            insertHistoryStmt.execute();
        }
    }
    END_DB(pStmt)
}

void FlagManager::broadcastPacket(Packet* pPacket) const {
    map<ZoneID_t, uint>::const_iterator itr = m_FlagAllowMap.begin();
    map<ZoneID_t, uint>::const_iterator endItr = m_FlagAllowMap.end();

    for (; itr != endItr; ++itr) {
        Zone* pZone = getZoneByZoneID(itr->first);
        pZone->broadcastPacket(pPacket);
    }
}
