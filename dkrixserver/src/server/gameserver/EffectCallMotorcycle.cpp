//----------------------------------------------------------------------
//
// Filename    : EffectCallMotorcycle.cpp
// Written by  : Reiot
//
//----------------------------------------------------------------------

// include files
#include "EffectCallMotorcycle.h"

#include "Assert.h"
#include "GCDeleteObject.h"
#include "Item.h"
#include "Tile.h"
#include "Zone.h"
#include "ZoneUtil.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
EffectCallMotorcycle::EffectCallMotorcycle(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Item* pItem, Turn_t delay,
                                           bool bDeleteFromDB)

    : Effect(pZone, x, y, pItem, delay) {
    __BEGIN_TRY

    Assert(getZone() != NULL);
    Assert(getTarget() != NULL);

    // m_ObjectID = pItem->getObjectID();
    m_ObjectID = pItem->getObjectID();
    m_bDeleteFromDB = bDeleteFromDB;

    
    m_bBroadcastingEffect = false;

    __END_CATCH
}


//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
EffectCallMotorcycle::~EffectCallMotorcycle()

{
    __BEGIN_TRY

    unaffect(m_pZone, m_X, m_Y, m_pTarget);

    __END_CATCH
}


//----------------------------------------------------------------------
// affect to target


//----------------------------------------------------------------------
void EffectCallMotorcycle::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pTarget)

{
    __BEGIN_TRY

    //	throw UnsupportedError();

    __END_CATCH
}


//----------------------------------------------------------------------
// remove effect from target
//----------------------------------------------------------------------
void EffectCallMotorcycle::unaffect(Motorcycle* pMotorcycle, Slayer* pSlayer)

{
    __BEGIN_TRY

    
    Zone* pZone = pSlayer->getZone();
    Coord_t x = pSlayer->getX();
    Coord_t y = pSlayer->get();

    

    
    Tile& tile = pZone->getTile(x, y);

    
    if (tile.hasItem()) {
    } else {
        pZone->addItem(pMotorcycle, x, y);

        
    }

    __END_CATCH
}

//----------------------------------------------------------------------
// unaffect()
//----------------------------------------------------------------------
void EffectCallMotorcycle::unaffect()

{
    __BEGIN_TRY
    __END_CATCH
}

//----------------------------------------------------------------------
// unaffect()
//----------------------------------------------------------------------
void EffectCallMotorcycle::unaffect(Creature* pCreature)

    {__BEGIN_TRY __END_CATCH}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string EffectCallMotorcycle::toString() const

{
    StringStream msg;

    msg << "EffectCallMotorcycle("
        << "ZoneID:" << (int)m_pZone->getZoneID() << ",X:" << (int)getX() << ",Y:" << (int)getY();

    if (m_pTarget)
        msg << ",Target:" << m_pTarget->toString();
    else
        msg << ",Target:NULL";

    msg << ",Deadline:" << (int)m_Deadline.tv_sec << "." << (int)m_Deadline.tv_usec;

    msg << ")";

    return msg.toString();
}
