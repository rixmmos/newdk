//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPickupMoneyHandler.cc
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGPickupMoney.h"

#ifdef __GAME_SERVER__
#include "GamePlayer.h"
#include "Item.h"
#include "ItemUtil.h"
#include "Ousters.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"
// #include "LogClient.h"
#include <stdio.h>

#include "GCCannotAdd.h"
#include "GCDeleteObject.h"
#include "GCDeleteandPickUpOK.h"
#include "VariableManager.h"
#include "ZoneUtil.h"
#include "item/Money.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGPickupMoneyHandler::execute(CGPickupMoney* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX __BEGIN_DEBUG

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    try {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
        Creature* pCreature = pGamePlayer->getCreature();
        bool bSuccess = false;
        bool bMargin = false;
        Gold_t itemGold = 0;
        Gold_t currentGold = 0;
        Gold_t marginGold = 0;
        Slayer* pSlayer = NULL;
        Vampire* pVampire = NULL;
        Ousters* pOusters = NULL;
        Zone* pZone = NULL;
        Coord_t ZoneX = pPacket->getZoneX();
        Coord_t ZoneY = pPacket->getZoneY();

        if (pCreature->isSlayer()) {
            pSlayer = dynamic_cast<Slayer*>(pCreature);
            pZone = pSlayer->getZone();
        } else if (pCreature->isVampire()) {
            pVampire = dynamic_cast<Vampire*>(pCreature);
            pZone = pVampire->getZone();
        } else if (pCreature->isOusters()) {
            pOusters = dynamic_cast<Ousters*>(pCreature);
            pZone = pOusters->getZone();
        } else
            throw ProtocolException("CGDropMoneyHandler::execute() :     .");

        Assert(pZone != NULL);

        
        if (!isValidZoneCoord(pZone, ZoneX, ZoneY)) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        // Anti-cheat: reach test. Without it a modified client could sweep every
        // dropped money pile in the zone. 8 tiles is under maxSight (13), so a
        // pile the player can see is always reachable.
        if (!isWithinReach(pCreature, ZoneX, ZoneY, 8)) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        
        Tile& _Tile = pZone->getTile(ZoneX, ZoneY);
        if (!_Tile.hasItem()) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        Item* pItem = _Tile.getItem();
        Assert(pItem != NULL);
        Item::ItemClass IClass = pItem->getItemClass();
        ObjectID_t ObjectID = pItem->getObjectID();

        
        if ((IClass == Item::ITEM_CLASS_MONEY) && (ObjectID == pPacket->getObjectID())) {
            Money* pMoney = dynamic_cast<Money*>(pItem);
            itemGold = pMoney->getAmount();
            marginGold = 0;

            if (pCreature->isSlayer())
                currentGold = pSlayer->getGold();
            else if (pCreature->isVampire())
                currentGold = pVampire->getGold();
            else if (pCreature->isOusters())
                currentGold = pOusters->getGold();

            

            if ((pCreature->isSlayer() && pItem->getItemType() != 0) ||
                (pCreature->isVampire() && pItem->getItemType() != 1) ||
                (pCreature->isOusters() && pItem->getItemType() != 2)) {
                GCCannotAdd _GCCannotAdd;
                _GCCannotAdd.setObjectID(pPacket->getObjectID());
                pPlayer->sendPacket(&_GCCannotAdd);
                return;
            }


            if (currentGold + itemGold > MAX_MONEY) {
                
                
                Gold_t pickupMoney = MAX_MONEY - currentGold;
                marginGold = currentGold + itemGold - MAX_MONEY;
                pMoney->setAmount(marginGold);

                // if (pCreature->isSlayer()) pSlayer->setGoldEx(MAX_MONEY);
                // else                       pVampire->setGoldEx(MAX_MONEY);
                //  by sigi. 2002.9.4
                if (pCreature->isSlayer())
                    pSlayer->increaseGoldEx(pickupMoney);
                else if (pCreature->isVampire())
                    pVampire->increaseGoldEx(pickupMoney);
                else if (pCreature->isOusters())
                    pOusters->increaseGoldEx(pickupMoney);

                bSuccess = true;
                bMargin = true;
            } else {
                // if (pCreature->isSlayer()) pSlayer->setGoldEx(currentGold + itemGold);
                // else                       pVampire->setGoldEx(currentGold + itemGold);
                //  by sigi. 2002.9.4
                if (pCreature->isSlayer())
                    pSlayer->increaseGoldEx(itemGold);
                else if (pCreature->isVampire())
                    pVampire->increaseGoldEx(itemGold);
                else if (pCreature->isOusters())
                    pOusters->increaseGoldEx(itemGold);

                bSuccess = true;
                bMargin = false;
            }
        } else 
        {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        if (bSuccess) {
            // log(LOG_PICKUP_MONEY, pCreature->getName(), "", pItem->toString());

            
            pZone->deleteItem(pItem, ZoneX, ZoneY);

            GCDeleteandPickUpOK _GCDeleteandPickUpOK;
            _GCDeleteandPickUpOK.setObjectID(pItem->getObjectID());
            pPlayer->sendPacket(&_GCDeleteandPickUpOK);

            if (pCreature->isSlayer()) {
                
                GCDeleteObject _GCDeleteObject;
                _GCDeleteObject.setObjectID(pItem->getObjectID());
                pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCDeleteObject, pSlayer);
            } else if (pCreature->isVampire()) {
                
                GCDeleteObject _GCDeleteObject;
                _GCDeleteObject.setObjectID(pItem->getObjectID());
                pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &_GCDeleteObject, pVampire);
            } else if (pCreature->isOusters()) {
                
                GCDeleteObject _GCDeleteObject;
                _GCDeleteObject.setObjectID(pItem->getObjectID());
                pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &_GCDeleteObject, pOusters);
            }

            
            if (bMargin) {
                TPOINT pt = pZone->addItem(pItem, ZoneX, ZoneY);
                if (pt.x != -1) {
                    // pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);
                    
                    char pField[80];
                    sprintf(pField, "Storage=%d, StorageID=%lu, X=%d, Y=%d", STORAGE_ZONE, pZone->getZoneID(), pt.x,
                            pt.y);
                    pItem->tinysave(pField);

                } else {
                    
                    // cout << "#############################################" << endl;
                    // cout << "# CRITICAL ERROR! CANNOT ADD MONEY TO ZONE! #" << endl;
                    // cout << "#############################################" << endl;

                    SAFE_DELETE(pItem);
                }
            } else {
                pItem->destroy();
                SAFE_DELETE(pItem);
            }

            
            if ((itemGold - marginGold) >= g_pVariableManager->getMoneyTraceLogLimit()) {
                char zoneName[15];
                sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), ZoneX, ZoneY);
                remainMoneyTraceLog(zoneName, pCreature->getName(), ITEM_LOG_MOVE, DETAIL_PICKUP,
                                    itemGold - marginGold);
            }

        } else {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
        }
    } catch (Throwable& t) {
        // cout << t.toString() << endl;
    }

#endif // __GAME_SERVER__

    __END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
