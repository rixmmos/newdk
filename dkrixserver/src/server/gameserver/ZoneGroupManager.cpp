//--------------------------------------------------------------------------------
//
// Filename    : ZoneGroupManager.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "ZoneGroupManager.h"

#include <stdio.h>

#include <list>

#include <unordered_map>

#include "DB.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "LoginServerManager.h"
#include "PCManager.h"
#include "Portal.h"
#include "PreparedStatement.h"
#include "Tile.h"
#include "ZoneGroup.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ZoneGroupManager::ZoneGroupManager()

    : m_ZoneGroups(10){__BEGIN_TRY __END_CATCH}


      //--------------------------------------------------------------------------------
      // destructor
      //--------------------------------------------------------------------------------
      ZoneGroupManager::~ZoneGroupManager()

{
    __BEGIN_TRY

    unordered_map<ZoneGroupID_t, ZoneGroup*>::iterator itr = m_ZoneGroups.begin();
    for (; itr != m_ZoneGroups.end(); itr++) {
        ZoneGroup* pZoneGroup = itr->second;
        SAFE_DELETE(pZoneGroup);
    }


    m_ZoneGroups.clear();

    __END_CATCH_NO_RETHROW
}


//--------------------------------------------------------------------------------
// initialize zone manager
//--------------------------------------------------------------------------------
void ZoneGroupManager::init()

{
    __BEGIN_TRY

    load();

    __END_CATCH
}


//--------------------------------------------------------------------------------
//
// load data from database
//

//
//--------------------------------------------------------------------------------
void ZoneGroupManager::load()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL;
    list<ZoneGroupID_t> ZoneGroupIDList;


    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectZoneGroupIDStmt(pConn, "SELECT ZoneGroupID FROM ZoneGroupInfo ORDER BY ZoneGroupID");
        Result* pResult = selectZoneGroupIDStmt.execute();

        while (pResult->next()) {
            ZoneGroupID_t ID = pResult->getInt(1);
            ZoneGroupIDList.push_back(ID);
        }
    }
    END_DB(pStmt)


    list<ZoneGroupID_t>::iterator itr = ZoneGroupIDList.begin();
    for (; itr != ZoneGroupIDList.end(); itr++) {
        ZoneGroupID_t ID = (*itr);


        ZoneGroup* pZoneGroup = new ZoneGroup(ID);
        ZonePlayerManager* pZonePlayerManager = new ZonePlayerManager();
        pZonePlayerManager->setZGID(ID);
        pZoneGroup->setZonePlayerManager(pZonePlayerManager);
        addZoneGroup(pZoneGroup);


        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            // Result* pResult = pStmt->executeQuery("SELECT ZoneID FROM ZoneInfo WHERE ZoneGroupID = %d", ID);
            PreparedStatement selectZoneIDStmt(pConn,
                                               "SELECT ZoneID FROM ZoneInfo WHERE ZoneGroupID = ? ORDER BY ZoneID");
            selectZoneIDStmt.bindInt(1, (int)ID);
            Result* pResult = selectZoneIDStmt.execute();

            while (pResult->next()) {
                ZoneID_t zoneID = pResult->getInt(1);


                Zone* pZone = new Zone(zoneID);
                Assert(pZone != NULL);

                pZone->setZoneGroup(pZoneGroup);

                pZoneGroup->addZone(pZone);

                //--------------------------------------------------------------------------------


                //--------------------------------------------------------------------------------

                printf("\n@@@@@@@@@@@@@@@ [%d]th ZONE INITIALIZATION START @@@@@@@@@@@@@@@\n", zoneID);

                pZone->init();

                printf("\n@@@@@@@@@@@@@@@ [%d]th ZONE INITIALIZATION SUCCESS @@@@@@@@@@@@@@@\n", zoneID);
            }
        }
        END_DB(pStmt)
    }

    ZoneGroupIDList.clear();


    __END_DEBUG
    __END_CATCH
}


//--------------------------------------------------------------------------------
// save data to database
//--------------------------------------------------------------------------------
void ZoneGroupManager::save()

{
    __BEGIN_TRY

    throw UnsupportedError(__PRETTY_FUNCTION__);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// add zone to zone manager
//--------------------------------------------------------------------------------
void ZoneGroupManager::addZoneGroup(ZoneGroup* pZoneGroup)

{
    __BEGIN_TRY

    unordered_map<ZoneGroupID_t, ZoneGroup*>::iterator itr = m_ZoneGroups.find(pZoneGroup->getZoneGroupID());

    if (itr != m_ZoneGroups.end())

        throw Error("duplicated zone id");


    m_ZoneGroups[pZoneGroup->getZoneGroupID()] = pZoneGroup;

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get zone from zone manager
//--------------------------------------------------------------------------------
ZoneGroup* ZoneGroupManager::getZoneGroupByGroupID(ZoneGroupID_t ZoneGroupID) const {
    __BEGIN_TRY

    ZoneGroup* pZoneGroup = NULL;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = m_ZoneGroups.find(ZoneGroupID);

    if (itr != m_ZoneGroups.end()) {
        pZoneGroup = itr->second;

    } else {
        StringStream msg;
        msg << "ZoneGroupID : " << ZoneGroupID;
        throw NoSuchElementException(msg.toString());
    }

    return pZoneGroup;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// Delete zone from zone manager
//--------------------------------------------------------------------------------
void ZoneGroupManager::deleteZoneGroup(ZoneGroupID_t zoneID) {
    __BEGIN_TRY

    unordered_map<ZoneGroupID_t, ZoneGroup*>::iterator itr = m_ZoneGroups.find(zoneID);

    if (itr != m_ZoneGroups.end()) {
        SAFE_DELETE(itr->second);


        m_ZoneGroups.erase(itr);
    } else {
        StringStream msg;
        msg << "ZoneGroupID : " << zoneID;
        throw NoSuchElementException(msg.toString());
    }

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get zone from zone manager
//--------------------------------------------------------------------------------
ZoneGroup* ZoneGroupManager::getZoneGroup(ZoneGroupID_t zoneID) const {
    __BEGIN_TRY

    ZoneGroup* pZoneGroup = NULL;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = m_ZoneGroups.find(zoneID);

    if (itr != m_ZoneGroups.end()) {
        pZoneGroup = itr->second;

    } else {
        StringStream msg;
        msg << "ZoneGroupID : " << zoneID;
        throw NoSuchElementException(msg.toString());
    }

    return pZoneGroup;

    __END_CATCH
}

void ZoneGroupManager::broadcast(Packet* pPacket)

{
    ZoneGroup* pZoneGroup = NULL;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = m_ZoneGroups.begin();

    for (; itr != m_ZoneGroups.end(); itr++) {
        pZoneGroup = itr->second;

        pZoneGroup->getZonePlayerManager()->broadcastPacket(pPacket);
    }
}

void ZoneGroupManager::pushBroadcastPacket(Packet* pPacket, BroadcastFilter* pFilter)

{
    ZoneGroup* pZoneGroup = NULL;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = m_ZoneGroups.begin();

    for (; itr != m_ZoneGroups.end(); itr++) {
        pZoneGroup = itr->second;

        pZoneGroup->getZonePlayerManager()->pushBroadcastPacket(pPacket, pFilter);
    }
}

void ZoneGroupManager::outputLoadValue()

{
    //------------------------------------------------------------------
    // ZoneGroup load
    //------------------------------------------------------------------
    ofstream file("loadBalance.txt", ios::app);

    VSDateTime current = VSDateTime::currentDateTime();
    file << current.toString() << endl;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr;

    for (itr = m_ZoneGroups.begin(); itr != m_ZoneGroups.end(); itr++) {
        ZoneGroup* pZoneGroup = itr->second;
        file << "[" << (int)pZoneGroup->getZoneGroupID() << "] ";

        const unordered_map<ZoneID_t, Zone*>& zones = pZoneGroup->getZones();
        unordered_map<ZoneID_t, Zone*>::const_iterator iZone;


        int totalLoad = 0;
        for (iZone = zones.begin(); iZone != zones.end(); iZone++) {
            Zone* pZone = iZone->second;

            int load = pZone->getLoadValue();
            int playerLoad = pZone->getPCCount();

            file << (int)pZone->getZoneID() << "(" << load << ", " << playerLoad << ") ";

            totalLoad += load;
        }

        file << " = " << totalLoad << endl;
    }

    file << endl;
    file.close();
}

//---------------------------------------------------------------------------
// make Balanced LoadInfo
//---------------------------------------------------------------------------
//


//


//
//     load = (loadLimit - load)*loadMultiplier;
//
//---------------------------------------------------------------------------
bool ZoneGroupManager::makeBalancedLoadInfo(LOAD_INFOS& loadInfos, bool bForce)

{
    const int maxGroup = m_ZoneGroups.size();

    const int loadLimit = 500;
    const int stableLoad = 120;


    const int minLoadGap = 20;
    const int averageLoadPercent = 90;

    int i;

    // LOAD_INFOS 	loadInfos;
    GROUPS groups;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr;


    int totalLoad = 0;


    //------------------------------------------------------------------

    //------------------------------------------------------------------
    int maxLoadValue = 0;
    int minLoadValue = loadLimit;
    for (itr = m_ZoneGroups.begin(); itr != m_ZoneGroups.end(); itr++) {
        ZoneGroup* pZoneGroup = itr->second;

        const unordered_map<ZoneID_t, Zone*>& zones = pZoneGroup->getZones();
        unordered_map<ZoneID_t, Zone*>::const_iterator iZone;


        for (iZone = zones.begin(); iZone != zones.end(); iZone++) {
            Zone* pZone = iZone->second;

            int load = pZone->getLoadValue();
            load = min(load, loadLimit);

            // 10~500
            maxLoadValue = max(maxLoadValue, load);
            minLoadValue = min(minLoadValue, load);


            int playerLoad = pZone->getPCCount() / 10;
            playerLoad = max(1, playerLoad);

            load = (loadLimit - load) * playerLoad;

            LoadInfo* pInfo = new LoadInfo;
            pInfo->id = pZone->getZoneID();
            pInfo->oldGroupID = itr->first;
            pInfo->groupID = -1;
            pInfo->load = load;


            DWORD key = (load << 8) | pInfo->id;

            loadInfos[key] = pInfo;

            totalLoad += load;
        }
    }

    //------------------------------------------------------------------
    //

    //
    //------------------------------------------------------------------
    if (!bForce) {
        int loadBoundary = stableLoad;
        // int loadBoundary = ( loadLimit - stableLoad ) * loadMultiplier;


        // if (maxLoad <= loadBoundary
        if (minLoadValue >= loadBoundary || maxLoadValue - minLoadValue <= minLoadGap) {
            for (itr = m_ZoneGroups.begin(); itr != m_ZoneGroups.end(); itr++) {
                ZoneGroup* pZoneGroup = itr->second;


                const unordered_map<ZoneID_t, Zone*>& zones = pZoneGroup->getZones();
                unordered_map<ZoneID_t, Zone*>::const_iterator iZone;


                for (iZone = zones.begin(); iZone != zones.end(); iZone++) {
                    Zone* pZone = iZone->second;

                    pZone->initLoadValue();
                }
            }

            return false;
        }
    }


    // int avgLoad = totalLoad / maxGroups;

    int avgLoad = totalLoad * averageLoadPercent / maxGroup / 100;


    groups.reserve(maxGroup);
    for (i = 0; i < maxGroup; i++) {
        groups[i] = 0;
    }


    // outputLoadValue();

    //------------------------------------------------------------------
    //
    // load balancing
    //

    //------------------------------------------------------------------
    LOAD_INFOS::const_iterator iInfo = loadInfos.begin();

    int index = 0;

    for (; iInfo != loadInfos.end(); iInfo++) {
        LoadInfo* pInfo = iInfo->second;


        int newGroupID = -1;
        for (int k = 0; k < maxGroup; k++) {
            int groupLoad = groups[index];

            if (groupLoad + pInfo->load <= avgLoad) {
                newGroupID = index;

                if (++index >= maxGroup)
                    index = 0;

                break;
            }

            if (++index >= maxGroup)
                index = 0;
        }


        if (newGroupID == -1) {
            newGroupID = 0;
            for (int k = 1; k < maxGroup; k++) {
                if (groups[k] < groups[newGroupID]) {
                    newGroupID = k;
                }
            }
        }


        pInfo->groupID = newGroupID + 1;
        groups[newGroupID] += pInfo->load;
    }

    return true;
}

//---------------------------------------------------------------------------
// make DefaultLoadInfo
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool ZoneGroupManager::makeDefaultLoadInfo(LOAD_INFOS& loadInfos)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL;
    list<ZoneGroupID_t> ZoneGroupIDList;


    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectZoneGroupIDStmt(pConn, "SELECT ZoneGroupID FROM ZoneGroupInfo");
        Result* pResult = selectZoneGroupIDStmt.execute();

        while (pResult->next()) {
            ZoneGroupID_t ID = pResult->getInt(1);
            ZoneGroupIDList.push_back(ID);
        }
    }
    END_DB(pStmt)


    list<ZoneGroupID_t>::iterator itr = ZoneGroupIDList.begin();
    for (; itr != ZoneGroupIDList.end(); itr++) {
        ZoneGroupID_t ID = *itr;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            // Result* pResult = pStmt->executeQuery("SELECT ZoneID FROM ZoneInfo WHERE ZoneGroupID = %d", ID);
            PreparedStatement selectZoneIDStmt(pConn, "SELECT ZoneID FROM ZoneInfo WHERE ZoneGroupID = ?");
            selectZoneIDStmt.bindInt(1, ID);
            Result* pResult = selectZoneIDStmt.execute();

            while (pResult->next()) {
                ZoneID_t zoneID = pResult->getInt(1);

                LoadInfo* pInfo = new LoadInfo;
                pInfo->id = zoneID;

                try {
                    pInfo->oldGroupID = g_pZoneInfoManager->getZoneInfo(zoneID)->getZoneGroupID();
                } catch (NoSuchElementException&) {
                    filelog("makeDefaultLoadInfoError.txt", "NoSuch ZoneInfo : %d", zoneID);
                    pInfo->oldGroupID = ID;
                }

                pInfo->groupID = ID;
                pInfo->load = 0;

                loadInfos[zoneID] = pInfo;
            }
        }
        END_DB(pStmt)
    }

    ZoneGroupIDList.clear();

    __END_DEBUG
    __END_CATCH

    return true;
}

//---------------------------------------------------------------------------
// balance ZoneGroup ( bForce )
//---------------------------------------------------------------------------
//


//

//


//
//     load = (loadLimit - load)*loadMultiplier;
//
//---------------------------------------------------------------------------
void ZoneGroupManager::balanceZoneGroup(bool bForce, bool bDefault)

{
    __BEGIN_TRY

    filelog("balanceZoneGroup.txt", "  .");
    return;


    __END_CATCH
}

//--------------------------------------------------------------------------------
// lock all ZoneGroup and LoginServerManager
//--------------------------------------------------------------------------------
void ZoneGroupManager::lockZoneGroups()

{
    __BEGIN_TRY

    //------------------------------------------------------------------
    // LoginServerManager UNLOCK
    //------------------------------------------------------------------
    g_pLoginServerManager->lock();

    //------------------------------------------------------------------
    //
    // 					LOCK all ZoneGroups
    //
    //------------------------------------------------------------------
    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr;

    for (itr = m_ZoneGroups.begin(); itr != m_ZoneGroups.end(); itr++) {
        ZoneGroup* pZoneGroup = itr->second;
        pZoneGroup->lock();
    }

    __END_CATCH
}

//--------------------------------------------------------------------------------
// lock all ZoneGroup and LoginServerManager
//--------------------------------------------------------------------------------
void ZoneGroupManager::unlockZoneGroups()

{
    __BEGIN_TRY

    //------------------------------------------------------------------
    //
    // 					UNLOCK all ZoneGroups
    //
    //------------------------------------------------------------------
    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr;

    for (itr = m_ZoneGroups.begin(); itr != m_ZoneGroups.end(); itr++) {
        ZoneGroup* pZoneGroup = itr->second;
        pZoneGroup->unlock();
    }

    //------------------------------------------------------------------
    // LoginServerManager UNLOCK
    //------------------------------------------------------------------
    g_pLoginServerManager->unlock();


    __END_CATCH
}

//--------------------------------------------------------------------------------
// get PlayerNum. by sigi. 2002.12.30
//--------------------------------------------------------------------------------
int ZoneGroupManager::getPlayerNum() const

{
    __BEGIN_TRY

    int numPC = 0;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = m_ZoneGroups.begin();

    for (; itr != m_ZoneGroups.end(); itr++) {
        ZoneGroup* pZoneGroup = itr->second;


        numPC += pZoneGroup->getZonePlayerManager()->size();
    }

    return numPC;

    __END_CATCH
}

void ZoneGroupManager::removeFlag(Effect::EffectClass EC)

{
    __BEGIN_TRY

    ZoneGroup* pZoneGroup = NULL;

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = m_ZoneGroups.begin();

    for (; itr != m_ZoneGroups.end(); itr++) {
        pZoneGroup = itr->second;

        pZoneGroup->getZonePlayerManager()->removeFlag(EC);
    }

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ZoneGroupManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "ZoneGroupManager(";

    for (unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = m_ZoneGroups.begin(); itr != m_ZoneGroups.end();
         itr++) {
        msg << itr->second->toString();
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}

// global variable definition
ZoneGroupManager* g_pZoneGroupManager = NULL;
