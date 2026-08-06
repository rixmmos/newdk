//----------------------------------------------------------------------
//
// Filename    : EffectDecayItem.cpp
// Written by  : Reiot
//
//----------------------------------------------------------------------

// include files
#include "EffectDecayItem.h"

#include <stdio.h>

#include "Assert.h"
#include "GCDeleteObject.h"
#include "Item.h"
#include "ItemUtil.h"
#include "Money.h"
#include "Tile.h"
#include "UniqueItemManager.h"
#include "VariableManager.h"
#include "Zone.h"
#include "ZoneUtil.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
EffectDecayItem::EffectDecayItem(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Item* pItem, Turn_t delay,
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
EffectDecayItem::~EffectDecayItem()

{
    __BEGIN_TRY

    unaffect(m_pZone, m_X, m_Y, m_pTarget);

    __END_CATCH_NO_RETHROW
}


//----------------------------------------------------------------------
// affect to target


//----------------------------------------------------------------------
void EffectDecayItem::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pTarget)

{
    __BEGIN_TRY

    //	throw UnsupportedError();

    __END_CATCH
}


//----------------------------------------------------------------------
// remove effect from target
//----------------------------------------------------------------------
void EffectDecayItem::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pTarget)

{
    __BEGIN_TRY

    
    Assert(isValidZoneCoord(pZone, x, y));

    
    Item* pTempItem = NULL;

    
    
    
    Tile& tile = pZone->getTile(x, y);

    if (tile.hasItem()) {
        pTempItem = tile.getItem();

        if (pTempItem != NULL) {
            
            // if (pTempItem->getObjectID() == m_ObjectID) {
            if (pTempItem->getObjectID() == m_ObjectID) {
                pZone->deleteItem(pTempItem, x, y);

                
                GCDeleteObject gcDeleteObject;
                gcDeleteObject.setObjectID(m_ObjectID);

                pZone->broadcastPacket(x, y, &gcDeleteObject);

                if (m_bDeleteFromDB) {
                    // ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( pTempItem->getItemClass(),
                    // pTempItem->getItemType() ); Assert(pItemInfo!=NULL);

                    
                    if (pTempItem->isUnique()) {
                        
                        if (pTempItem->getCreateType() != Item::CREATE_TYPE_CREATE)
                            UniqueItemManager::deleteItem(pTempItem->getItemClass(), pTempItem->getItemType());

                        filelog("uniqueItem.txt", "[EffectDecayItem] %s", pTempItem->toString().c_str());
                    }

                    
                     

                    
                    if (pTempItem->getItemClass() == Item::ITEM_CLASS_MONEY) {
                        Money* pMoney = dynamic_cast<Money*>(pTempItem);
                        if (pMoney->getAmount() >= g_pVariableManager->getMoneyTraceLogLimit()) {
                            char zoneName[15];
                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), x, y);
                            remainMoneyTraceLog(zoneName, "GOD", ITEM_LOG_DELETE, DETAIL_TIMEOUT, pMoney->getAmount());
                        }
                    }

                    pTempItem->destroy();
                }

                SAFE_DELETE(pTempItem);
            }
        }
    }

    pTarget = NULL;

    __END_CATCH
}

//----------------------------------------------------------------------
// unaffect()
//----------------------------------------------------------------------
void EffectDecayItem::unaffect()

{
    __BEGIN_TRY
    __END_CATCH
}

//----------------------------------------------------------------------
// unaffect()
//----------------------------------------------------------------------
void EffectDecayItem::unaffect(Creature* pCreature)

    {__BEGIN_TRY __END_CATCH}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string EffectDecayItem::toString() const

{
    StringStream msg;

    msg << "EffectDecayItem("
        << "ZoneID:" << (int)m_pZone->getZoneID() << ",X:" << (int)getX() << ",Y:" << (int)getY();

    if (m_pTarget)
        msg << ",Target:" << m_pTarget->toString();
    else
        msg << ",Target:NULL";

    msg << ",Deadline:" << (int)m_Deadline.tv_sec << "." << (int)m_Deadline.tv_usec;

    msg << ")";

    return msg.toString();
}
