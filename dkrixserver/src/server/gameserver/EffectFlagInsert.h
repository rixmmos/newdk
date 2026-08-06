//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFlagInsert.h
// Written by  :

//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_FLAG_INSERT__
#define __EFFECT_FLAG_INSERT__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectFlagInsert
//////////////////////////////////////////////////////////////////////////////


class EffectFlagInsert : public Effect {
public:
    EffectFlagInsert(Creature* pCreature);
    EffectFlagInsert(Item* pItem);

public:
    EffectClass getEffectClass() const {
        return EFFECT_CLASS_FLAG_INSERT;
    }

    void affect() {}
    void affect(Creature* pCreature);
    void affect(Item* pItem);

    void unaffect(Creature* pCreature);
    void unaffect(Item* pItem);
    void unaffect();

    string toString() const;
};

#endif // __EFFECT_DOOM__
