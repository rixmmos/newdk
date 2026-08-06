//////////////////////////////////////////////////////////////////////////////
// Filename    : PCManager.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __PC_MANANGER_H__
#define __PC_MANANGER_H__

#include <vector>

#include "CreatureManager.h"

//////////////////////////////////////////////////////////////////////////////
// class PCManager
//////////////////////////////////////////////////////////////////////////////
const BYTE defaultRaceValue = 0xFF;

class PCManager : public CreatureManager {
public:
    PCManager();
    virtual ~PCManager();

public:
    
    // virtual void addCreature(Creature* pCreature) ;

    
    // virtual void deleteCreature(ObjectID_t objectID) ;

    
    // Creature* getCreature(ObjectID_t objectID) const ;

    
    virtual void processCreatures();

    
    virtual void killCreature(Creature* pDeadCreature);

    
    // void broadcastPacket(Packet* pPacket, Creature* owner) ;

    
    void transportAllCreatures(ZoneID_t ZoneID, ZoneCoord_t ZoneX = 0xffff, ZoneCoord_t ZoneY = 0xffff,
                               Race_t race = defaultRaceValue, Turn_t delay = 10) const;

    // get debug string
    string toString() const;

    
    void setRefreshHolyLandPlayer(bool bRefresh) {
        m_bRefreshHolyLandPlayer = bRefresh;
    }
    //	void setRefreshLevelWarBonusZonePlayer( bool bRefresh ) { m_bRefreshLevelWarBonusZonePlayer = bRefresh; }

    vector<uint> getPCNumByRace() const;

private:
    bool m_bRefreshHolyLandPlayer;
    //	bool m_bRefreshLevelWarBonusZonePlayer;
};

#endif
