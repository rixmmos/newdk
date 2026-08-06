//////////////////////////////////////////////////////////////////////////////
// Filename    : ZoneInfoManager.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __ZONE_INFO_MANAGER_H__
#define __ZONE_INFO_MANAGER_H__

#include <unordered_map>

#include "Exception.h"
#include "Types.h"
#include "Zone.h"
#include "ZoneInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class ZoneInfoManager;

//////////////////////////////////////////////////////////////////////////////

class ZoneInfoManager {
public:
    ZoneInfoManager();
    ~ZoneInfoManager();

public:
    void init();
    void load();

    void addZoneInfo(ZoneInfo* pZoneInfo);
    void deleteZoneInfo(ZoneID_t zoneID);
    ZoneInfo* getZoneInfo(ZoneID_t zoneID);
    ZoneInfo* getZoneInfoByName(const string& ZoneName);
    int size() const {
        return m_ZoneInfos.size();
    }

    vector<Zone*> getNormalFields() const;

    string toString() const;

private:
    unordered_map<ZoneID_t, ZoneInfo*> m_ZoneInfos; 
    unordered_map<string, ZoneInfo*> m_FullNameMap;
    unordered_map<string, ZoneInfo*> m_ShortNameMap;
};

extern ZoneInfoManager* g_pZoneInfoManager;

#endif
