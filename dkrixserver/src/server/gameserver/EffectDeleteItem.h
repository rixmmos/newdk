//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDeleteItem.h
// Written by  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_DELETE_ITEM_H__
#define __EFFECT_DELETE_ITEM_H__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectDeleteItem;

//////////////////////////////////////////////////////////////////////////////

class Item;

class EffectDeleteItem : public Effect {
public:
    EffectDeleteItem(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject, Turn_t delay);
    virtual ~EffectDeleteItem();

public:
    virtual EffectClass getEffectClass() const {
        return EFFECT_CLASS_DELETE_ITEM;
    }

    
    virtual ObjectPriority getObjectPriority() const {
        return OBJECT_PRIORITY_NONE;
    }

    virtual void affect() {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }
    virtual void affect(Creature* pCreature) {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }
    virtual void affect(Item* pItem) {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }
    virtual void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pTarget);

    virtual void unaffect();
    virtual void unaffect(Creature* pCreature);
    virtual void unaffect(Item* pItem = NULL) {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }
    virtual void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pTarget);

    virtual void create(const string& ownerID) {}
    virtual void save(const string& ownerID) {}
    virtual void destroy(const string& ownerID) {}

    // get debug string
    virtual string toString() const;

private:
    ObjectID_t m_ObjectID;
    bool m_bAllowCreature;
};

#endif
