//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectReloadTimer.h
// Written by  : crazydog

//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_RELOAD_TIMER__
#define __EFFECT_RELOAD_TIMER__

#include "Effect.h"
#include "Item.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectReloadTimer
//////////////////////////////////////////////////////////////////////////////

class EffectReloadTimer : public Effect {
public:
    EffectReloadTimer(Creature* pCreature);

public:
    EffectClass getEffectClass() const throw() {
        return EFFECT_CLASS_RELOAD_TIMER;
    }

    void affect() {}
    void affect(Creature* pCreature);
    void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject);

    void unaffect(Creature* pCreature);
    void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject);
    void unaffect(Item* pItem) {}
    void unaffect();

    string toString() const throw();

public:
    void setFromInventory(bool b) throw() {
        m_bFromInventory = b;
    }
    void setSlotID(SlotID_t id) {
        m_SlotID = id;
    }
    void setObjectID(ObjectID_t id) {
        m_ObjectID = id;
    }
    void setInventoryXY(CoordInven_t x, CoordInven_t y) throw() {
        m_invenX = x;
        m_invenY = y;
    }

private:
    CoordInven_t m_invenX; 
    CoordInven_t m_invenY; 
    ObjectID_t m_ObjectID; 
    SlotID_t m_SlotID;     
    bool m_bFromInventory; 
};


#endif // __EFFECT_RELOAD_TIMER__
