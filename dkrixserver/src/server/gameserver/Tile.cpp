//////////////////////////////////////////////////////////////////////////////
// FileName 	: Tile.cpp
// Written By	: reiot@ewestsoft.com
// Description	:
//////////////////////////////////////////////////////////////////////////////

#include "Tile.h"

#include <algorithm>

#include "Assert.h"
#include "Creature.h"
#include "EffectDarkness.h"
#include "EffectGreenPoison.h"
#include "EffectTryingPosition.h"
#include "EffectYellowPoison.h"
#include "GamePlayer.h"
#include "Ousters.h"
#include "Player.h"
#include "Sector.h"
#include "Slayer.h"
#include "StringStream.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
Tile::Tile(WORD wFlags, WORD wOption)

{
    __BEGIN_TRY

    m_wFlags = wFlags;
    m_wOption = wOption;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
Tile::~Tile()

{
    __BEGIN_TRY

    
    while (!m_Objects.empty()) {
        Object* pObj = m_Objects.front();
        SAFE_DELETE(pObj);
        m_Objects.pop_front();
    }

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////////////

//

//////////////////////////////////////////////////////////////////////////////
bool Tile::addCreature(Creature* pCreature, bool bCheckEffect, bool bCheckPortal) {
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pCreature != NULL);

    
    Creature::MoveMode mode = pCreature->getMoveMode();

    
    Assert(!isBlocked(mode));

    
    // Assert(! hasCreature(mode));
    if (hasCreature(mode)) {
        StringStream msg;

        Creature* pWalkingCreature = getCreature(Creature::MOVE_MODE_WALKING);
        Creature* pFlyingCreature = getCreature(Creature::MOVE_MODE_FLYING);
        Creature* pBurrowingCreature = getCreature(Creature::MOVE_MODE_BURROWING);
        Item* pItem = getItem();

        msg << "TileInfo: ";

        if (pWalkingCreature != NULL) {
            msg << "Walking(" << pWalkingCreature->toString().c_str() << ") ";
        }
        if (pFlyingCreature != NULL) {
            msg << "Flying(" << pFlyingCreature->toString().c_str() << ") ";
        }
        if (pBurrowingCreature != NULL) {
            msg << "Burrowing(" << pBurrowingCreature->toString().c_str() << ") ";
        }
        if (pItem != NULL) {
            msg << "Item(" << pItem->toString().c_str() << ") ";
        }

        filelog("tileError.txt", "%s", msg.toString().c_str());

        Assert(false);
    }

    
    addObject(pCreature);

    
    FLAG_SET(m_wFlags, TILE_WALKING_CREATURE + mode);

    
    FLAG_SET(m_wFlags, TILE_GROUND_BLOCKED + mode);

    Assert(isBlocked(mode));
    Assert(hasCreature(mode));

    if (bCheckPortal) {
        
        if (hasPortal() && pCreature->isPC()) {
            Portal* pPortal = getPortal();
            if (pPortal->activate(pCreature))
                return false;

             
        }
    }

    
    if (hasEffect()) {
        if (bCheckEffect) {
            EffectGreenPoison* pEGP = (EffectGreenPoison*)getEffect(Effect::EFFECT_CLASS_GREEN_POISON);
            // if (pCreature->isSlayer() && (pEGP = (EffectGreenPoison*)getEffect(Effect::EFFECT_CLASS_GREEN_POISON)))
            if (pEGP != NULL) {
                pEGP->affectCreature(pCreature, true);
            }
            EffectYellowPoison* pEYP = NULL;
            if ((pCreature->isSlayer() || pCreature->isOusters()) &&
                (pEYP = (EffectYellowPoison*)getEffect(Effect::EFFECT_CLASS_YELLOW_POISON))) {
                pEYP->affectCreature(pCreature, true);
            }
            
            else if ((pEYP = (EffectYellowPoison*)getEffect(Effect::EFFECT_CLASS_YELLOW_POISON)) && pEYP->isForce()) {
                pEYP->affectCreature(pCreature, true);
            }

            EffectDarkness* pDarkness = NULL;
            if (pCreature->isSlayer() && (pDarkness = (EffectDarkness*)getEffect(Effect::EFFECT_CLASS_DARKNESS))) {
                pDarkness->affectObject(pCreature, true);
            }

            EffectTryingPosition* pTP;
            if (pCreature->isPC() &&
                (pTP = dynamic_cast<EffectTryingPosition*>(getEffect(Effect::EFFECT_CLASS_TRYING_POSITION)))) {
                pTP->affect(pCreature);
            }
        }
    } else {
        if ((pCreature->isOusters() || pCreature->isSlayer()) && pCreature->isFlag(Effect::EFFECT_CLASS_DARKNESS)) {
            pCreature->removeFlag(Effect::EFFECT_CLASS_DARKNESS);
        }
    }

    
    return true;

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
void Tile::deleteCreature(ObjectID_t creatureID) {
    __BEGIN_TRY
    __BEGIN_DEBUG

    try {
        
        Assert(isGroundBlocked() || isAirBlocked() || isUndergroundBlocked());

        
        // Assert(hasWalkingCreature() || hasFlyingCreature() || hasBurrowingCreature());
        Assert(hasCreature()); // by sigi. 2002.5.8

        Creature* pCreature = dynamic_cast<Creature*>(getObject(creatureID));

        
        /*
        if (hasEffect())
        {
            Effect* pEffect = getEffect();
            pEffect->unaffect(pCreature);
        }
        */

        
        if (pCreature == NULL) {
            return;
        }

        if (hasEffect()) {
            EffectTryingPosition* pTP;
            if (pCreature->isPC() &&
                (pTP = dynamic_cast<EffectTryingPosition*>(getEffect(Effect::EFFECT_CLASS_TRYING_POSITION)))) {
                pTP->unaffect(pCreature);
            }
        }


        
        deleteObject(creatureID);

        
        FLAG_CLEAR(m_wFlags, TILE_WALKING_CREATURE + pCreature->getMoveMode());

        
        FLAG_CLEAR(m_wFlags, TILE_GROUND_BLOCKED + pCreature->getMoveMode());
    } catch (Throwable& t) {
        
        // cerr << t.toString() << endl;
        filelog("tileError.txt", "Tile::deleteCreature - %s", t.toString().c_str());
    }

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::deleteCreature(Creature::MoveMode mode) {
    __BEGIN_TRY

    
    Assert(isBlocked(mode));

    
    Assert(hasCreature(mode));

    if (hasEffect()) {
        EffectTryingPosition* pTP;
        Creature* pCreature = getCreature(mode);
        if (pCreature != NULL && pCreature->isPC() &&
            (pTP = dynamic_cast<EffectTryingPosition*>(getEffect(Effect::EFFECT_CLASS_TRYING_POSITION)))) {
            pTP->unaffect(pCreature);
        }
    }

    
    deleteObject(OBJECT_PRIORITY_WALKING_CREATURE + mode);

    
    FLAG_CLEAR(m_wFlags, TILE_WALKING_CREATURE + mode);

    
    FLAG_CLEAR(m_wFlags, TILE_GROUND_BLOCKED + mode);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Creature* Tile::getCreature(ObjectID_t creatureID) {
    __BEGIN_TRY

    Assert(hasWalkingCreature() || hasFlyingCreature() || hasBurrowingCreature());

    return (Creature*)(getObject(creatureID));

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Creature* Tile::getCreature(Creature::MoveMode mode) {
    __BEGIN_TRY

    Assert(hasCreature(mode));
    return (Creature*)getObject(ObjectPriority(OBJECT_PRIORITY_WALKING_CREATURE + mode));

    __END_CATCH
}

//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
void Tile::addItem(Item* pItem)

{
    __BEGIN_TRY

    Assert(pItem != NULL);

    Assert(!hasItem());
    Assert(!hasBuilding());
    Assert(!hasObstacle());
    Assert(!hasPortal());
    /*
        EffectDarkness* pDarkness;
        if ((pDarkness = getEffect(Effect::EFFECT_CLASS_DARKNESS)))
        {
            pDarkness->affectObject(pItem, false);
        }
    */
    addObject(pItem);

    FLAG_SET(m_wFlags, TILE_ITEM);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::deleteItem() {
    __BEGIN_TRY
    __BEGIN_DEBUG

    // Assert(hasItem());
    if (!hasItem()) {
        
        return;
    }

    deleteObject(OBJECT_PRIORITY_ITEM);

    FLAG_CLEAR(m_wFlags, TILE_ITEM);

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Item* Tile::getItem() {
    __BEGIN_TRY

    // Assert(hasItem());
    if (!hasItem())
        return NULL;

    return (Item*)getObject(OBJECT_PRIORITY_ITEM);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::addObstacle(Obstacle* pObstacle)

{
    __BEGIN_TRY

    Assert(pObstacle != NULL);

    // must be empty tile...
    Assert(!hasWalkingCreature());
    Assert(!hasFlyingCreature());
    Assert(!hasBurrowingCreature());
    Assert(!hasEffect());
    Assert(!hasObstacle());
    Assert(!hasItem());
    Assert(!hasBuilding());
    Assert(!hasPortal());
    Assert(!isTerrain());

    FLAG_SET(m_wFlags, TILE_OBSTACLE);

    addObject(pObstacle);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::deleteObstacle() {
    __BEGIN_TRY

    Assert(hasObstacle());

    deleteObject(OBJECT_PRIORITY_OBSTACLE);

    FLAG_CLEAR(m_wFlags, TILE_OBSTACLE);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Obstacle* Tile::getObstacle() {
    __BEGIN_TRY

    Assert(hasObstacle());

    return (Obstacle*)getObject(OBJECT_PRIORITY_OBSTACLE);

    __END_CATCH
}

bool Tile::canAddEffect()

{
    return !(hasObstacle() || hasBuilding() || hasPortal());
}

//////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////
void Tile::addEffect(Effect* pEffect)

{
    __BEGIN_TRY

    Assert(pEffect != NULL);

    Assert(!hasObstacle());
    Assert(!hasBuilding());
    Assert(!hasPortal());

    addObject(pEffect);

    
    // pEffect->affectTile();

    FLAG_SET(m_wFlags, TILE_EFFECT);

    __END_CATCH
}

//////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////
void Tile::deleteEffect(ObjectID_t effectID) {
    __BEGIN_TRY

    if (!hasEffect()) {
        filelog("TileEffectBug.txt", "there is no effect with effect id %d", effectID);
        return;
    }
    //	Assert(hasEffect());

    
    deleteObject(effectID);

    
    
    // effect->unaffectTile();

    
     

    
    if (getObject(OBJECT_PRIORITY_EFFECT) == NULL) {
        FLAG_CLEAR(m_wFlags, TILE_EFFECT);
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Effect* Tile::getEffect(ObjectID_t effectID) {
    __BEGIN_TRY

    if (hasEffect() == false)
        return NULL;
    return (Effect*)getObject(effectID);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Effect* Tile::getEffect(Effect::EffectClass effectClass)

{
    __BEGIN_TRY

    if (hasEffect()) {
        for (forward_list<Object*>::const_iterator itr = m_Objects.begin(); itr != m_Objects.end(); itr++) {
            Effect* pEffect = NULL;
            if ((*itr)->getObjectClass() == Object::OBJECT_CLASS_EFFECT) {
                if (effectClass == ((Effect*)(*itr))->getEffectClass()) {
                    
                    pEffect = dynamic_cast<Effect*>(*itr);
                    return pEffect;
                }
            }
        }
    }

    return NULL;

    __END_CATCH
}


//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::addBuilding(BuildingID_t buildingID)

{
    __BEGIN_TRY

    Assert(!hasWalkingCreature());
    Assert(!hasFlyingCreature());
    Assert(!hasBurrowingCreature());
    Assert(!hasEffect());
    Assert(!hasObstacle());
    Assert(!hasItem());
    Assert(!hasBuilding());
    Assert(!hasPortal());
    Assert(!isTerrain());

    FLAG_SET(m_wFlags, TILE_BUILDING);

    m_wOption = buildingID;

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::deleteBuilding()

{
    __BEGIN_TRY

    Assert(hasBuilding());

    FLAG_CLEAR(m_wFlags, TILE_BUILDING);

    m_wOption = 0;

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
BuildingID_t Tile::getBuilding() const

{
    __BEGIN_TRY

    Assert(hasBuilding());

    return m_wOption;

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::addPortal(Portal* pPortal)

{
    __BEGIN_TRY

    Assert(pPortal != NULL);

    
    
    Assert(!hasWalkingCreature());
    Assert(!hasFlyingCreature());
    Assert(!hasBurrowingCreature());
    Assert(!hasEffect());
    Assert(!hasObstacle());
    Assert(!hasItem());
    Assert(!hasBuilding());
    Assert(!hasPortal());
    Assert(!isTerrain());

    addObject(pPortal);

    FLAG_SET(m_wFlags, TILE_PORTAL);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::deletePortal()

{
    __BEGIN_TRY

    Assert(hasPortal());

    deleteObject(OBJECT_PRIORITY_PORTAL);

    FLAG_CLEAR(m_wFlags, TILE_PORTAL);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Portal* Tile::getPortal() const

{
    __BEGIN_TRY

    Assert(hasPortal());

    return (Portal*)getObject(OBJECT_PRIORITY_PORTAL);

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::addTerrain(TerrainID_t terrainID)

{
    __BEGIN_TRY

    
    
    Assert(!hasObstacle());
    Assert(!hasBuilding());
    Assert(!hasPortal());

    
    FLAG_SET(m_wFlags, TILE_TERRAIN);

    
    m_wOption = terrainID;

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::deleteTerrain()

{
    __BEGIN_TRY

    Assert(isTerrain());

    
    FLAG_CLEAR(m_wFlags, TILE_TERRAIN);

    
    m_wOption = 0;

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
TerrainID_t Tile::getTerrain() const

{
    __BEGIN_TRY

    Assert(isTerrain());

    return m_wOption;

    __END_CATCH
}


//////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////
string Tile::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "Tile(";
    msg << "Flag:" << m_wFlags;
    msg << "\nObjects:";
    forward_list<Object*>::const_iterator itr = m_Objects.begin();
    for (; itr != m_Objects.end(); itr++) {
        msg << (*itr)->toString() << "\n";
    }

    msg << "TileOption:" << (int)m_wOption;
    msg << ")";

    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////
// add object into object list
//////////////////////////////////////////////////////////////
void Tile::addObject(Object* pObject) {
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pObject != NULL);
    Assert(pObject->getObjectID() != 0);

    forward_list<Object*>::iterator before = m_Objects.end();
    forward_list<Object*>::iterator current = m_Objects.begin();

     

    for (; current != m_Objects.end(); before = current, current++) {
        
        
        

        if (pObject->getObjectPriority() < (*current)->getObjectPriority()) {
            if (before == m_Objects.end()) {
                
                m_Objects.push_front(pObject);
            } else {
                
                // O(1) insertion
                m_Objects.insert_after(before, pObject);
            }
            return;
        } else if (pObject->getObjectPriority() == (*current)->getObjectPriority()) {
            
            if (pObject->getObjectPriority() == OBJECT_PRIORITY_EFFECT) {
                if (before == m_Objects.end()) {
                    m_Objects.push_front(pObject);
                } else {
                    m_Objects.insert_after(before, pObject);
                }
                return;
            } else {
                cerr << toString() << endl;
                cerr << " tile priority  = " << (int)pObject->getObjectPriority() << endl;
                cerr << "  = " << m_wFlags << endl;
                filelog("TILEBUG.log", "%s", toString().c_str());
                throw DuplicatedException("tile priority duplicated");
            }
        }
    }

    
    
    
    if (current == m_Objects.end()) {
        if (before == m_Objects.end()) {
            
            m_Objects.push_front(pObject);
        } else {
            
            // O(1) insertion
            m_Objects.insert_after(before, pObject);
        }
    }

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////
// Delete object from object list
//////////////////////////////////////////////////////////////
void Tile::deleteObject(ObjectID_t objectID) {
    __BEGIN_TRY

     

    forward_list<Object*>::iterator before = m_Objects.end();
    forward_list<Object*>::iterator current = m_Objects.begin();

    int i = 0;
    for (; current != m_Objects.end(); before = current++) {
        if (objectID == (*current)->getObjectID()) {
            
            if (before == m_Objects.end()) {
                // Delete first node
                m_Objects.pop_front();
            } else {
                // O(1) deletion
                m_Objects.erase_after(before);
            }

            return;
        }
        i++;
    }

    Assert(false);

     

    
    // throw NoSuchElementException("invalid object id");

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void Tile::deleteObject(ObjectPriority objectPriority) {
    __BEGIN_TRY

    forward_list<Object*>::iterator before = m_Objects.end();
    forward_list<Object*>::iterator current = m_Objects.begin();

     

    for (; current != m_Objects.end(); before = current++) {
        if (objectPriority == (*current)->getObjectPriority()) {
            
            if (before == m_Objects.end()) {
                // Delete first node
                m_Objects.pop_front();
            } else {
                // O(1) deletion
                m_Objects.erase_after(before);
            }

            return;
        } else if (objectPriority < (*current)->getObjectPriority()) {
            
            
            
            
            break;
        }
    }

    
    // throw NoSuchElementException("invalid object priority");

    __END_CATCH
}
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
Object* Tile::getObject(ObjectID_t objectID) const {
    __BEGIN_TRY

    for (forward_list<Object*>::const_iterator itr = m_Objects.begin(); itr != m_Objects.end(); itr++) {
        if (objectID == (*itr)->getObjectID()) {
            
            return *itr;
        }
    }

    
    
    // throw NoSuchElementException("invalid object id");

    
    return NULL;

    __END_CATCH
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
Object* Tile::getObject(ObjectPriority objectPriority) const {
    __BEGIN_TRY

    for (forward_list<Object*>::const_iterator itr = m_Objects.begin(); itr != m_Objects.end(); itr++) {
        if (objectPriority == (*itr)->getObjectPriority()) {
            
            return *itr;
        } else if (objectPriority < (*itr)->getObjectPriority()) {
            
            
            
            
            break;
        }
    }

    
    // throw NoSuchElementException("invalid tile priority");
    return NULL;

    __END_CATCH
}
