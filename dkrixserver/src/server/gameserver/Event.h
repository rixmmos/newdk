//////////////////////////////////////////////////////////////////////////////
// Filename    : Event.h
// Written by  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_H__
#define __EVENT_H__

#include "Exception.h"
#include "Timeval.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class Event;


//////////////////////////////////////////////////////////////////////////////

class GamePlayer;

class Event {
public:
    enum EventClass {
        EVENT_CLASS_RESURRECT,                
        EVENT_CLASS_MORPH,                    
        EVENT_CLASS_RESTORE,                  
        EVENT_SAVE,                           
        EVENT_CLASS_REGENERATION,             
        EVENT_CLASS_RELOAD_INFO,              
        EVENT_CLASS_TRANSPORT,                
        EVENT_CLASS_KICK,                     
        EVENT_CLASS_SYSTEM_MESSAGE,           
        EVENT_CLASS_REFRESH_HOLY_LAND_PLAYER, 
        EVENT_CLASS_SHUTDOWN,                 
        EVENT_CLASS_HEAD_COUNT,               
        EVENT_CLASS_CBILLING,                 
        EVENT_CLASS_AUTH,                     
        EVENT_CLASS_MAX
    };

public:
    Event(GamePlayer* pGamePlayer);
    virtual ~Event();

public:
    virtual EventClass getEventClass() const = 0;

    // get event life-cycle
    
    virtual bool isTemporary() const {
        return true;
    }
    virtual bool isPermanent() const {
        return false;
    }

    virtual void activate() = 0;

    Timeval getDeadline() const {
        return m_Deadline;
    }
    void setDeadline(Turn_t delay);

    virtual string toString() const = 0;

protected:
    GamePlayer* m_pGamePlayer; 
    Timeval m_Deadline;        
};

#endif
