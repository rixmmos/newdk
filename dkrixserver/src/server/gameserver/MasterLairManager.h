//////////////////////////////////////////////////////////////////////////////
// Filename    : MasterLairManager.h

// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __MASTER_LAIR_MANAGER_H__
#define __MASTER_LAIR_MANAGER_H__

#include <vector>

#include <unordered_map>

#include "Item.h"
#include "MonsterCounter.h"
#include "Mutex.h"
#include "Timeval.h"

//////////////////////////////////////////////////////////////////////////////
// class MasterLairManager
//////////////////////////////////////////////////////////////////////////////

class Zone;

class MasterLairManager {
public:
    enum MasterLairEvent {
        EVENT_WAITING_PLAYER,   
        EVENT_MINION_COMBAT,    
        EVENT_MASTER_COMBAT,    
        EVENT_WAITING_KICK_OUT, 
        EVENT_WAITING_REGEN,    

        EVENT_MAX
    };


public:
    MasterLairManager(Zone* pZone);
    ~MasterLairManager();

    MasterLairEvent getCurrentEvent() const {
        return m_Event;
    }

    bool enterCreature(Creature* pCreature); 
    bool leaveCreature(Creature* pCreature); 

    bool heartbeat();

    // void increaseSummonedMonsterNumber(int num) ;
    bool isMasterReady() const {
        return m_bMasterReady;
    }
    void setMasterReady(bool bReady = true) {
        m_bMasterReady = bReady;
    }

    void startEvent();
    void stopEvent();

    void lock() {
        m_Mutex.lock();
    }
    void ulnock() {
        m_Mutex.unlock();
    }

    string toString() const;

protected:
    void processEventWaitingPlayer();
    void processEventMinionCombat();
    void processEventMasterCombat();
    void processEventWaitingKickOut();
    void processEventWaitingRegen();

    void activeEventWaitingPlayer();
    void activeEventMinionCombat();
    void activeEventMasterCombat();
    void activeEventWaitingKickOut();
    void activeEventWaitingRegen();

    void deleteAllMonsters(); 
    void kickOutPlayers();    
    void giveKillingReward(); 
    void killAllMonsters();   

private:
    Zone* m_pZone;
    ObjectID_t m_MasterID; 
    ZoneCoord_t m_MasterX;
    ZoneCoord_t m_MasterY;

    bool m_bMasterReady; 

    
    

    int m_nMaxPassPlayer; 
    int m_nPassPlayer;    

    MasterLairEvent m_Event; 
    Timeval m_EventTime;     
    int m_EventValue;        

    Timeval m_RegenTime; 


    mutable Mutex m_Mutex; 
};

#endif
