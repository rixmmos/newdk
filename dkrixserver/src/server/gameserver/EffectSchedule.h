//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSchedule.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SCHEDULE_H__
#define __EFFECT_SCHEDULE_H__

#include <list>

#include "Exception.h"
#include "Mutex.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

enum EffectScheduleWorkCode {
    WORKCODE_ADD_VAMPIRE_PORTAL = 0,
    WORKCODE_DELETE,

    WORKCODE_MAX
};


//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Effect;


//////////////////////////////////////////////////////////////////////////////
// class EffectScheduleWork
//////////////////////////////////////////////////////////////////////////////

class EffectScheduleWork {
public:
    EffectScheduleWork();
    ~EffectScheduleWork();

public:
    int getCode(void) const {
        return m_Code;
    }
    void setCode(int code) {
        m_Code = code;
    }

    void* getData(void) const {
        return m_pData;
    }
    void setData(void* pData) {
        m_pData = pData;
    }

private:
    int m_Code;    
    void* m_pData; 
};


//////////////////////////////////////////////////////////////////////////////
// class EffectSchedule
//////////////////////////////////////////////////////////////////////////////

class EffectSchedule {
public:
    EffectSchedule();
    ~EffectSchedule();

public:
    
    Effect* getEffect(void) const {
        return m_pEffect;
    }
    void setEffect(Effect* pEffect) {
        m_pEffect = pEffect;
    }

    
    void addWork(int WorkCode, void* pData);

    
    EffectScheduleWork* getFrontWork(void);

private:
    Effect* m_pEffect;
    list<EffectScheduleWork*> m_WorkList;
};


//////////////////////////////////////////////////////////////////////////////
// class EffectScheduleManager
//////////////////////////////////////////////////////////////////////////////

class EffectScheduleManager {
public:
    EffectScheduleManager();
    ~EffectScheduleManager();

public:
    
    void addEffectSchedule(EffectSchedule* pEffectSchedule);

    
    void heartbeat(void);

protected:
    list<EffectSchedule*> m_EffectScheduleList;
    mutable Mutex m_Mutex;
};


#endif
