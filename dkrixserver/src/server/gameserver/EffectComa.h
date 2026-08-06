//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectComa.h
// Written by  : excel96
// Description :



// (CGMove, CGUseItem, CGSkillTo..., CGSay)
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_COMA__
#define __EFFECT_COMA__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectComa
//////////////////////////////////////////////////////////////////////////////

class EffectComa : public Effect {
public:
    EffectComa(Creature* pCreature);
    ~EffectComa();

public:
    EffectClass getEffectClass() const {
        return EFFECT_CLASS_COMA;
    }

    void affect();
    void affect(Creature* pCreature);

    void unaffect();
    void unaffect(Creature* pCreature);

    virtual void create(const string& ownerID);
    virtual void save(const string& ownerID);
    virtual void destroy(const string& ownerID);

    string toString() const;

public:
    Timeval getStartTime(void) const {
        return m_StartTime;
    }
    void setStartTime(void);
    bool canResurrect(void);

private:
    Timeval m_StartTime;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectComaLoader
//////////////////////////////////////////////////////////////////////////////

class EffectComaLoader : public EffectLoader {
public:
    virtual Effect::EffectClass getEffectClass() const {
        return Effect::EFFECT_CLASS_COMA;
    }
    virtual string getEffectClassName() const {
        return "EffectComa";
    }

public:
    virtual void load(Creature* pCreature);
};

extern EffectComaLoader* g_pEffectComaLoader;

#endif // __EFFECT_COMA__
