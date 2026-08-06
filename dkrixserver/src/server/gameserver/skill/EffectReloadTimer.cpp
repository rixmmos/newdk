//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectReloadTimer.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectReloadTimer.h"

#include <stdio.h>

#include "Creature.h"
#include "GCReloadOK.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Player.h"
#include "Slayer.h"
#include "item/AR.h"
#include "item/Belt.h"
#include "item/Magazine.h"
#include "item/SG.h"
#include "item/SMG.h"
#include "item/SR.h"

//////////////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////////////
EffectReloadTimer::EffectReloadTimer(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// EffectReloadTimer::affect()
//////////////////////////////////////////////////////////////////////////////
void EffectReloadTimer::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// EffectReloadTimer::affect()
//////////////////////////////////////////////////////////////////////////////
void EffectReloadTimer::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// EffectReloadTimer::unaffect()
//////////////////////////////////////////////////////////////////////////////
void EffectReloadTimer::unaffect()

{
    __BEGIN_TRY

    unaffect((Creature*)m_pTarget);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// EffectReloadTimer::unaffect()
//////////////////////////////////////////////////////////////////////////////
void EffectReloadTimer::unaffect(Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "EffectReloadTimer " << "unaffect BEGIN" << endl;

    Assert(pCreature != NULL);
    Assert(pCreature->isSlayer());

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
    Player* pPlayer = pSlayer->getPlayer();
    Item* pArmsItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
    Bullet_t BulletNum = 0;
    Item* pItem = NULL;
    Item* pBelt = NULL;
    Inventory* pInventory = NULL;
    bool bSuccess = false;

    
    pSlayer->removeFlag(Effect::EFFECT_CLASS_RELOAD_TIMER);

    if (pArmsItem != NULL) {
        if (isArmsWeapon(pArmsItem)) {
            if (m_bFromInventory) {
                
                
                pInventory = pSlayer->getInventory();
                pItem = pInventory->getItem(m_invenX, m_invenY);
            } else {
                
                
                if (pSlayer->isWear(Slayer::WEAR_BELT)) {
                    pBelt = pSlayer->getWearItem(Slayer::WEAR_BELT);
                    pInventory = ((Belt*)pBelt)->getInventory();
                    pItem = pInventory->getItem(m_SlotID, 0);
                }
            }

            if (pItem == NULL || pInventory == NULL) {
                
                // cout << "EffectReloadTimer " << "unaffect END" << endl;
                return;
            }

            ObjectID_t ItemObjectID = pItem->getObjectID();

            
            if (ItemObjectID == m_ObjectID && pItem->getItemClass() == Item::ITEM_CLASS_MAGAZINE) {
                BulletNum = reloadArmsItem(pArmsItem, pItem);

                
                if (BulletNum != 0) {
                    // pArmsItem->save(pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_RIGHTHAND, 0);

                    
                    // by sigi. 2002.5.16
                    //					Gun* pGun = dynamic_cast<Gun*>(pArmsItem);
                    char pField[80];
                    sprintf(pField, "BulletCount=%d, Silver=%d", pArmsItem->getBulletCount(), pArmsItem->getSilver());
                    pArmsItem->tinysave(pField);

                    
                    if (pItem->getNum() > 1) {
                        
                        
                        pItem->setNum(pItem->getNum() - 1);
                        pInventory->decreaseItemNum();
                        pInventory->decreaseWeight(pItem->getWeight());

                        
                        if (m_bFromInventory) {
                            // pItem->save(pSlayer->getName(), STORAGE_INVENTORY, 0, m_invenX, m_invenY);
                            //  by sigi. 2002.5.16
                            sprintf(pField, "Num=%d", pItem->getNum());
                            pItem->tinysave(pField);
                        } else {
                            // pItem->save(pSlayer->getName(), STORAGE_BELT, pBelt->getItemID(), m_SlotID, 0);
                            //  by sigi. 2002.5.16
                            sprintf(pField, "Num=%d", pItem->getNum());
                            pItem->tinysave(pField);
                        }
                    }
                    
                    else {
                        if (m_bFromInventory)
                            pInventory->deleteItem(m_invenX, m_invenY);
                        else
                            pInventory->deleteItem(m_SlotID, 0);

                        pItem->destroy();
                        SAFE_DELETE(pItem);
                    }

                    bSuccess = true;
                } // if (BulletNum != 0)
            }
        }
    }

    if (bSuccess) {
        GCReloadOK ok;
        ok.setBulletNum(BulletNum);
        pPlayer->sendPacket(&ok);
    }

    // cout << "EffectReloadTimer " << "unaffect END" << endl;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectReloadTimer::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

    {__BEGIN_TRY __END_CATCH}


//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string EffectReloadTimer::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectReloadTimer("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}
