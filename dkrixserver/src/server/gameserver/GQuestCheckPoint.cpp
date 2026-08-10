#include "GQuestCheckPoint.h"

#include "MonsterCorpse.h"
#include "Properties.h"
#include "SXml.h"
#include "Zone.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"

void GQuestCheckPoint::load() {
    __BEGIN_TRY

    XMLTree* pTree = new XMLTree;
    pTree->LoadFromFile((g_pConfig->getProperty("HomePath") + "/data/EventCheckPoint.xml").c_str());

    // Every GetAttribute below is hoisted out of Assert(). That macro is
    // ((void)0) under NDEBUG and never evaluates its argument, so a Release
    // build would read none of EventCheckPoint.xml: these five locals are
    // declared outside the loop and never initialised, so each checkpoint would
    // be spawned from stack garbage -- getZoneByZoneID(zoneid) on an arbitrary
    // id, addItem at arbitrary coordinates -- or, worse, silently reuse the
    // previous iteration's values for every entry.
    DWORD type = 0, zoneid = 0, x = 0, y = 0, id = 0;
    for (size_t i = 0; i < pTree->GetChildCount(); ++i) {
        XMLTree* pChild = pTree->GetChild(i);
        bool bHasType = pChild->GetAttribute("type", type);
        bool bHasZoneID = pChild->GetAttribute("zoneid", zoneid);
        bool bHasX = pChild->GetAttribute("x", x);
        bool bHasY = pChild->GetAttribute("y", y);
        bool bHasID = pChild->GetAttribute("id", id);

        Assert(bHasType);
        Assert(bHasZoneID);
        Assert(bHasX);
        Assert(bHasY);
        Assert(bHasID);

        MonsterCorpse* pMonsterCorpse = new MonsterCorpse(type, "", 2);
        pMonsterCorpse->setTreasureCount(255);
        Zone* pZone = getZoneByZoneID(zoneid);
        Assert(pZone != NULL);
        pZone->registerObject(pMonsterCorpse);
        pZone->addItem(pMonsterCorpse, x, y, true, 0xffffffff);

        m_CheckPointMap[pMonsterCorpse] = id;
        Assert(m_IDMap[id] == NULL);
        m_IDMap[id] = pMonsterCorpse;

        StringStream name;
        name << g_pZoneInfoManager->getZoneInfo(pMonsterCorpse->getZone()->getZoneID())->getFullName() << "("
             << (int)pMonsterCorpse->getX() << "," << (int)pMonsterCorpse->getY() << ")";
        m_NameMap[id] = name.toString();
    }

    SAFE_DELETE(pTree);

    pTree = new XMLTree;
    pTree->LoadFromFile((g_pConfig->getProperty("HomePath") + "/data/TravelWay.xml").c_str());
    for (size_t i = 0; i < pTree->GetChildCount(); ++i) {
        XMLTree* pChild = pTree->GetChild(i);
        Assert(pChild->GetName() == "TravelWay");
        // Hoisted out of Assert() for the same reason. Under NDEBUG `race` would
        // be uninitialised and index m_EventWayPoints, and `grade` would be an
        // empty string whose grade[0] below is an out-of-range access.
        DWORD race = 0;
        string grade;
        bool bHasRace = pChild->GetAttribute("race", race);
        bool bHasGrade = pChild->GetAttribute("grade", grade);

        Assert(bHasRace);
        Assert(bHasGrade);
        DWORD nGrade = grade[0] - 'A';
        Assert(nGrade <= 3);
        vector<DWORD>& target = m_EventWayPoints[race][nGrade];
        for (size_t j = 0; j < pChild->GetChildCount(); ++j) {
            XMLTree* pWay = pChild->GetChild(j);
            Assert(pWay->GetName() == "Way");
            // Hoisted out of Assert() for the same reason -- under NDEBUG every
            // waypoint id pushed here would be an uninitialised local.
            DWORD id = 0;
            bool bHasWayID = pWay->GetAttribute("id", id);
            Assert(bHasWayID);
            target.push_back(id);
        }
    }

    __END_CATCH
}

DWORD GQuestCheckPoint::getCheckPointID(MonsterCorpse* pCheckPoint) {
    map<MonsterCorpse*, DWORD>::iterator itr = m_CheckPointMap.find(pCheckPoint);
    if (itr == m_CheckPointMap.end())
        return 0;
    return itr->second;
}

string GQuestCheckPoint::getTargetList(Race_t race, Level_t level, DWORD grade, DWORD basenum, vector<DWORD>& outList) {
    string ret;
    vector<DWORD>& waypoints = getWayPointVector(race, grade);
    int pointnum = basenum;
    if (level <= 50)
        pointnum += 0;
    else if (level <= 90)
        pointnum += 1;
    else
        pointnum += 2;

    cout << (int)grade << " " << (int)level << " " << pointnum << "" << endl;

    if (pointnum > waypoints.size())
        pointnum = waypoints.size();
    for (int i = 0; i < waypoints.size(); ++i) {
        int last = waypoints.size() - i;

        if ((rand() % last) < pointnum) {
            pointnum--;
            outList.push_back(waypoints[i]);
            ret += getStringFromWayPoint(waypoints[i]);
            if (pointnum <= 0)
                break;
            else
                ret += ", ";
        }
    }

    cout << " " << ret;
    return ret;
}

const string& GQuestCheckPoint::getStringFromWayPoint(DWORD id) const {
    static const string empty = "";
    map<DWORD, string>::const_iterator itr = m_NameMap.find(id);
    if (itr == m_NameMap.end())
        return empty;
    return itr->second;
}
