//////////////////////////////////////////////////////////////////////////////
// Filename    : EventRefreshHolyLandPlayer.cpp
// Written by  : bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventRefreshHolyLandPlayer.h"

#include "HolyLandManager.h"
// #include "BloodBibleBonusManager.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"

// #include "GCHolyLandBonusInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class EventRefreshHolyLandPlayer member methods
//////////////////////////////////////////////////////////////////////////////

EventRefreshHolyLandPlayer::EventRefreshHolyLandPlayer(GamePlayer* pGamePlayer)

    : Event(pGamePlayer) {}

void EventRefreshHolyLandPlayer::activate()

{
    __BEGIN_TRY

    const unordered_map<ZoneGroupID_t, ZoneGroup*>& zoneGroups = g_pZoneGroupManager->getZoneGroups();

    unordered_map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = zoneGroups.begin();

    for (; itr != zoneGroups.end(); ++itr) {
        const unordered_map<ZoneID_t, Zone*>& zones = itr->second->getZones();
        unordered_map<ZoneID_t, Zone*>::const_iterator zItr = zones.begin();

        for (; zItr != zones.end(); ++zItr) {
            
            zItr->second->setRefreshHolyLandPlayer(true);
        }
    }

    
    //	g_pHolyLandManager->refreshHolyLandPlayers();

     
    __END_CATCH
}

string EventRefreshHolyLandPlayer::toString() const

{
    StringStream msg;
    msg << "EventRefreshHolyLandPlayer("
        << ")";
    return msg.toString();
}
