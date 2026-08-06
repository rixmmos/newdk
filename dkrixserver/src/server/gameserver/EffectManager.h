//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectManager.h
// Written by  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MANAGER_H__
#define __EFFECT_MANAGER_H__

#include <list>

#include "Effect.h"
#include "EffectInfo.h"
#include "Exception.h"
#include "Timeval.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectManager;
//////////////////////////////////////////////////////////////////////////////

class EffectManager {
public:
    EffectManager();
    virtual ~EffectManager();

public:
    void save(const string& ownerID);

    EffectInfo* getEffectInfo();

    bool isEffect(Effect::EffectClass EClass);

    void deleteEffect(Effect::EffectClass EClass);
    void deleteEffect(ObjectID_t ObjectID);
    void deleteEffect(Creature* pCreature, Effect::EffectClass EClass);
    Effect* findEffect(Effect::EffectClass EClass) const;
    Effect* findEffect(ObjectID_t ObjectID) const;

    
    Effect* findEffect(Effect::EffectClass EClass, string EnemyName) const;

    
    void addEffect(Effect* pEffect);

    Effect* getEffect() const {
        return m_Effects.front();
    }

    
    void setTimeOutAllEffect();

    
    
    //	int heartbeat() ;
    int heartbeat(const Timeval& currentTime);

    void sendEffectInfo(Creature* pCreature, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y);

    
    uint getSize() const {
        return m_Effects.size();
    }

protected:
    list<Effect*> m_Effects;

    // by sigi. for debugging. 2002.12.23
    int m_LastEffectClass;

    EffectInfo m_EffectInfo;
};

#endif
