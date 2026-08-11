//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestRepairHandler.cpp

// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestRepair.h"

#ifdef __GAME_SERVER__
#include "CheckedCast.h"
#include "GamePlayer.h"
#include "ItemInfo.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "NPC.h"
#include "Ousters.h"
#include "PriceManager.h"
#include "Slayer.h"
#include "Vampire.h"
// #include "LogClient.h"
#include <stdio.h>

#include "GCNPCResponse.h"
#include "ZoneUtil.h"
#include "item/Key.h"
#include "item/OustersSummonItem.h"
#include "item/SlayerPortalItem.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::execute(CGRequestRepair* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    ObjectID_t ITEMOID = pPacket->getObjectID();
    GamePlayer* pGamePlayer = checkedCast<GamePlayer*>(pPlayer);
    Creature* pPC = pGamePlayer->getCreature();
    bool bSlayer = false;
    bool bVampire = false;
    bool bOusters = false;
    Item* pItem = NULL;

    
    if (pPC->isSlayer())
        bSlayer = true;
    else if (pPC->isVampire())
        bVampire = true;
    else if (pPC->isOusters())
        bOusters = true;
    else
        throw ProtocolException("CGRequestRepairHandler::execute() : Unknown player creature!");

    if (ITEMOID == 0) {
        
        executeAll(pPacket, pPlayer);
    } else {
        if (bSlayer)
            pItem = (dynamic_cast<Slayer*>(pPC))->findItemOID(ITEMOID);
        else if (bVampire)
            pItem = (dynamic_cast<Vampire*>(pPC))->findItemOID(ITEMOID);
        else if (bOusters)
            pItem = (dynamic_cast<Ousters*>(pPC))->findItemOID(ITEMOID);

        
        if (pItem != NULL) {
            
            if (pItem->getItemClass() == Item::ITEM_CLASS_KEY && pItem->getItemType() == 2) {
                executeMotorcycle(pPacket, pPlayer);
                return;
            } else
                executeNormal(pPacket, pPlayer);
        } else {
            
            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_REPAIR_FAIL_ITEM_NOT_EXIST);
            pPlayer->sendPacket(&response);
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::executeNormal(CGRequestRepair* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        ObjectID_t ITEMOID = pPacket->getObjectID();
    Creature* pPC = checkedCast<GamePlayer*>(pPlayer)->getCreature();
    bool bSlayer = false;
    bool bVampire = false;
    bool bOusters = false;
    Gold_t playerMoney = 0;
    Price_t repairPrice = 0;
    Item* pItem = NULL;
    Slayer* pSlayer = NULL;
    Vampire* pVampire = NULL;
    Ousters* pOusters = NULL;
    int storage = 0;
    int X = 0;
    int Y = 0;
    GCNPCResponse response;

    
    if (pPC->isSlayer())
        bSlayer = true;
    else if (pPC->isVampire())
        bVampire = true;
    else if (pPC->isOusters())
        bOusters = true;

    
    if (bSlayer) {
        pSlayer = dynamic_cast<Slayer*>(pPC);
        playerMoney = pSlayer->getGold();
        pItem = pSlayer->findItemOID(ITEMOID, storage, X, Y);
    } else if (bVampire) {
        pVampire = dynamic_cast<Vampire*>(pPC);
        playerMoney = pVampire->getGold();
        pItem = pVampire->findItemOID(ITEMOID, storage, X, Y);
    } else if (bOusters) {
        pOusters = dynamic_cast<Ousters*>(pPC);
        playerMoney = pOusters->getGold();
        pItem = pOusters->findItemOID(ITEMOID, storage, X, Y);
    }

    
    
    
    if (isRepairableItem(pItem) == false) {
        response.setCode(NPC_RESPONSE_REPAIR_FAIL_ITEM_TYPE);
        pPlayer->sendPacket(&response);
        return;
    }

    
    Durability_t oldDurability = pItem->getDurability();

    repairPrice = g_pPriceManager->getRepairPrice(pItem);

    if (repairPrice > playerMoney) {
        response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
        pPlayer->sendPacket(&response);
        return;
    }

    
    repairItem(pItem);

    
    if (storage == STORAGE_GEAR && oldDurability == 0) {
        if (bSlayer && pSlayer != NULL) {
            pSlayer->initAllStatAndSend();
            pSlayer->sendRealWearingInfo();
        } else if (bVampire && pVampire != NULL) {
            pVampire->initAllStatAndSend();
            pVampire->sendRealWearingInfo();
        } else if (bOusters && pOusters != NULL) {
            pOusters->initAllStatAndSend();
            pOusters->sendRealWearingInfo();
        }
    }

    
    if (bSlayer) {
        // pSlayer->setGoldEx(playerMoney-repairPrice);
        //  by sigi. 2002.9.4
        pSlayer->decreaseGoldEx(repairPrice);
        // log(LOG_REPAIR_ITEM, pSlayer->getName(), "", pItem->toString());
    } else if (bVampire) {
        // by sigi. 2002.9.4
        pVampire->decreaseGoldEx(repairPrice);
        // log(LOG_REPAIR_ITEM, pVampire->getName(), "", pItem->toString());
    } else if (bOusters) {
        // by sigi. 2002.9.4
        pOusters->decreaseGoldEx(repairPrice);
        // log(LOG_REPAIR_ITEM, pOusters->getName(), "", pItem->toString());
    }

    
    
    
    


    
    if (repairPrice > 0) {
        char pField[80];

        if (pItem->getItemClass() == Item::ITEM_CLASS_SLAYER_PORTAL_ITEM) {
            SlayerPortalItem* pSPItem = checkedCast<SlayerPortalItem*>(pItem);
            sprintf(pField, "Charge=%d", pSPItem->getCharge());
        } else if (pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM) {
            OustersSummonItem* pOSItem = checkedCast<OustersSummonItem*>(pItem);
            sprintf(pField, "Charge=%d", pOSItem->getCharge());
        } else {
            sprintf(pField, "Durability=%d", pItem->getDurability());
        }

        pItem->tinysave(pField);
    }

     

    
    response.setCode(NPC_RESPONSE_REPAIR_OK);
    response.setParameter(playerMoney - repairPrice);
    pPlayer->sendPacket(&response);

#endif

    __END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::executeMotorcycle(CGRequestRepair* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        ObjectID_t ITEMOID = pPacket->getObjectID();
    Creature* pPC = checkedCast<GamePlayer*>(pPlayer)->getCreature();
    Zone* pZone = pPC->getZone();
    Gold_t playerMoney = 0;
    ZoneCoord_t CenterX = pPC->getX();
    ZoneCoord_t CenterY = pPC->getY();
    Item* pItem = NULL;
    bool bSlayer = false;
    bool bVampire = false;
    bool bOusters = false;
    GCNPCResponse response;

    
    if (pPC->isSlayer())
        bSlayer = true;
    else if (pPC->isVampire())
        bVampire = true;
    else if (pPC->isOusters())
        bOusters = true;
    else
        throw ProtocolException("CGRequestRepairHandler::execute() : Unknown player creature!");

    
    
    if (bSlayer) {
        pItem = (dynamic_cast<Slayer*>(pPC))->findItemOID(ITEMOID);
        playerMoney = (dynamic_cast<Slayer*>(pPC))->getGold();
    } else if (bVampire) {
        pItem = (dynamic_cast<Vampire*>(pPC))->findItemOID(ITEMOID);
        playerMoney = (dynamic_cast<Vampire*>(pPC))->getGold();
    } else if (bOusters) {
        pItem = (dynamic_cast<Ousters*>(pPC))->findItemOID(ITEMOID);
        playerMoney = (dynamic_cast<Ousters*>(pPC))->getGold();
    }

    
    for (ZoneCoord_t zx = CenterX - 5; zx <= CenterX + 5; zx++) {
        for (ZoneCoord_t zy = CenterY - 5; zy <= CenterY + 5; zy++) {
            if (!isValidZoneCoord(pZone, zx, zy))
                continue;

            Tile& tile = pZone->getTile(zx, zy);

            if (tile.hasItem()) {
                Item* pItemOnTile = tile.getItem();
                Assert(pItemOnTile != NULL);

                
                if (pItemOnTile->getItemClass() == Item::ITEM_CLASS_MOTORCYCLE) {
                    DWORD targetID = dynamic_cast<Key*>(pItem)->getTarget();
                    ItemID_t motorcycleID = pItemOnTile->getItemID();

                    if (targetID == motorcycleID) {
                        Price_t repairPrice = g_pPriceManager->getRepairPrice(pItemOnTile);

                        if (repairPrice > playerMoney) {
                            response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
                            pPlayer->sendPacket(&response);
                            return;
                        }

                        
                        repairItem(pItemOnTile);

                        
                        // pItemOnTile->save(pPC->getName(), STORAGE_ZONE, pZone->getZoneID(), zx, zy);
                        
                        char pField[80];
                        sprintf(pField, "Durability=%d", pItemOnTile->getDurability());
                        pItemOnTile->tinysave(pField);


                        
                        // if (bSlayer) (dynamic_cast<Slayer*>(pPC))->setGoldEx(playerMoney-repairPrice);
                        // else         (dynamic_cast<Vampire*>(pPC))->setGoldEx(playerMoney-repairPrice);

                        // by sigi. 2002.9.4
                        (dynamic_cast<PlayerCreature*>(pPC))->decreaseGoldEx(repairPrice);

                        response.setCode(NPC_RESPONSE_REPAIR_OK);
                        response.setParameter(playerMoney - repairPrice);
                        pPlayer->sendPacket(&response);

                        return;
                    } // if (targetID ==
                } // if (itemclas == MOTORCYCLE
            }
        } // end of for (ZoneCoord_t zy=CenterY-5; zy<=CenterY+5; zy++)
    } // end of for (ZoneCoord_t zx=CenterX-5; zx<=CenterX+5; zx++)

    
    
    response.setCode(NPC_RESPONSE_REPAIR_FAIL_ITEM_NOT_EXIST);
    pPlayer->sendPacket(&response);

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::executeAll(CGRequestRepair* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Creature* pPC = checkedCast<GamePlayer*>(pPlayer)->getCreature();
    Price_t repairPrice = 0;
    GCNPCResponse response;

    bool bSendRealWearingInfo = false;

    if (pPC->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);

        
        for (int i = 0; i < Slayer::WEAR_MAX; i++) {
            Item* pItem = pSlayer->getWearItem((Slayer::WearPart)i);
            if (pItem != NULL) {
                if (i == Slayer::WEAR_RIGHTHAND && isTwohandWeapon(pItem)) {
                    
                    
                } else {
                    repairPrice += g_pPriceManager->getRepairPrice(pItem);
                }
            }
        }

        
        if (pSlayer->getGold() < repairPrice) {
            response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
            pPlayer->sendPacket(&response);
            return;
        }

        
        char pField[80];

        for (int i = 0; i < Slayer::WEAR_MAX; i++) {
            Item* pItem = pSlayer->getWearItem((Slayer::WearPart)i);
            if (pItem != NULL) {
                if (i == Slayer::WEAR_RIGHTHAND && isTwohandWeapon(pItem)) {
                    
                    
                } else if (isRepairableItem(pItem)) {
                    Durability_t oldDurability = pItem->getDurability();
                    repairItem(pItem);
                    if (pItem->getDurability() != oldDurability) {
                        
                        
                        // pItem->save(pSlayer->getName(), STORAGE_GEAR, 0, i, 0);
                        
                        sprintf(pField, "Durability=%d", pItem->getDurability());
                        pItem->tinysave(pField);
                    }

                    if (oldDurability == 0)
                        bSendRealWearingInfo = true;
                }
            }
        }

        
        // pSlayer->setGoldEx(pSlayer->getGold() - repairPrice);

        // by sigi.2002.9.4
        pSlayer->decreaseGoldEx(repairPrice);

        
        // log(LOG_REPAIR_ITEM, pSlayer->getName(), "", "ALL");

        
        response.setCode(NPC_RESPONSE_REPAIR_OK);
        response.setParameter(pSlayer->getGold());
        pPlayer->sendPacket(&response);
    } else if (pPC->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pPC);

        
        for (int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; i++) {
            Item* pItem = pVampire->getWearItem((Vampire::WearPart)i);
            if (pItem != NULL) {
                if (i == Vampire::WEAR_RIGHTHAND && isTwohandWeapon(pItem)) {
                    
                } else {
                    repairPrice += g_pPriceManager->getRepairPrice(pItem);
                }
            }
        }

        
        if (pVampire->getGold() < repairPrice) {
            response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
            pPlayer->sendPacket(&response);
            return;
        }

        
        char pField[80];

        for (int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; i++) {
            Item* pItem = pVampire->getWearItem((Vampire::WearPart)i);
            if (pItem != NULL) {
                if (i == Vampire::WEAR_RIGHTHAND && isTwohandWeapon(pItem)) {
                    
                } else {
                    Durability_t oldDurability = pItem->getDurability();
                    repairItem(pItem);
                    if (pItem->getDurability() != oldDurability) {
                        
                        
                        // pItem->save(pVampire->getName(), STORAGE_GEAR, 0, i, 0);
                        
                        sprintf(pField, "Durability=%d", pItem->getDurability());
                        pItem->tinysave(pField);
                    }

                    if (oldDurability == 0)
                        bSendRealWearingInfo = true;
                }
            }
        }

        
        // pVampire->setGoldEx(pVampire->getGold() - repairPrice);
        // by sigi.2002.9.4
        pVampire->decreaseGoldEx(repairPrice);

        
        // log(LOG_REPAIR_ITEM, pVampire->getName(), "", "ALL");

        
        response.setCode(NPC_RESPONSE_REPAIR_OK);
        response.setParameter(pVampire->getGold());
        pPlayer->sendPacket(&response);
    } else if (pPC->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pPC);

        
        for (int i = 0; i < Ousters::OUSTERS_WEAR_MAX; i++) {
            Item* pItem = pOusters->getWearItem((Ousters::WearPart)i);
            if (pItem != NULL) {
                if (i == Ousters::WEAR_RIGHTHAND && isTwohandWeapon(pItem)) {
                    
                } else {
                    repairPrice += g_pPriceManager->getRepairPrice(pItem);
                }
            }
        }

        
        if (pOusters->getGold() < repairPrice) {
            response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
            pPlayer->sendPacket(&response);
            return;
        }

        
        char pField[80];

        for (int i = 0; i < Ousters::OUSTERS_WEAR_MAX; i++) {
            Item* pItem = pOusters->getWearItem((Ousters::WearPart)i);
            if (pItem != NULL) {
                if (i == Ousters::WEAR_RIGHTHAND && isTwohandWeapon(pItem)) {
                    
                } else {
                    Durability_t oldDurability = pItem->getDurability();
                    repairItem(pItem);
                    if (pItem->getDurability() != oldDurability) {
                        
                        
                        // pItem->save(pOusters->getName(), STORAGE_GEAR, 0, i, 0);
                        
                        sprintf(pField, "Durability=%d", pItem->getDurability());
                        pItem->tinysave(pField);
                    }

                    if (oldDurability == 0)
                        bSendRealWearingInfo = true;
                }
            }
        }

        
        // pOusters->setGoldEx(pOusters->getGold() - repairPrice);
        // by sigi.2002.9.4
        pOusters->decreaseGoldEx(repairPrice);

        
        // log(LOG_REPAIR_ITEM, pOusters->getName(), "", "ALL");

        
        response.setCode(NPC_RESPONSE_REPAIR_OK);
        response.setParameter(pOusters->getGold());
        pPlayer->sendPacket(&response);
    }

    if (bSendRealWearingInfo) {
        if (pPC->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
            Assert(pSlayer != NULL);

            pSlayer->initAllStatAndSend();
            pSlayer->sendRealWearingInfo();
        } else if (pPC->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
            Assert(pVampire != NULL);

            pVampire->initAllStatAndSend();
            pVampire->sendRealWearingInfo();
        } else if (pPC->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
            Assert(pOusters != NULL);

            pOusters->initAllStatAndSend();
            pOusters->sendRealWearingInfo();
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
