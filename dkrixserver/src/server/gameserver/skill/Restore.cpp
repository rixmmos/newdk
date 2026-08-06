//////////////////////////////////////////////////////////////////////////////
// Filename    : Restore.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Restore.h"

#include <stdio.h>

#include "DB.h"
#include "EffectBloodDrain.h"
#include "EffectRestore.h"
#include "GCDeleteObject.h"
#include "GCMorph1.h"
#include "GCMorphSlayer2.h"
#include "GCRemoveEffect.h"
#include "GCSkillToObjectOK1.h"
#include "GCSkillToSelfOK1.h"
#include "GSGuildMemberLogOn.h"
#include "Guild.h"
#include "GuildManager.h"
#include "ItemUtil.h"
#include "NPC.h"
#include "PCFinder.h"
#include "Party.h"
#include "RelicUtil.h"
#include "SharedServerManager.h"
#include "TradeManager.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Restore::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SkillSlot* pSkillSlot, CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    Assert(pSlayer != NULL);
    Assert(pSkillSlot != NULL);

    try {
        Player* pPlayer = pSlayer->getPlayer();
        Zone* pZone = pSlayer->getZone();
        Assert(pPlayer != NULL);
        Assert(pZone != NULL);

        Creature* pFromCreature = pZone->getCreature(TargetObjectID);

        
        
        if (pFromCreature == NULL || !pFromCreature->isVampire()) {
            executeSkillFailException(pSlayer, getSkillType());
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }

        GCSkillToObjectOK1 _GCSkillToObjectOK1; 
        GCMorph1 _GCMorph1;                     
        GCMorphSlayer2 _GCMorphSlayer2;         

        SkillType_t SkillType = pSkillSlot->getSkillType();
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

        bool bRangeCheck = verifyDistance(pSlayer, pFromCreature, pSkillInfo->getRange());
        bool bHitRoll = true;

        if (bRangeCheck && bHitRoll) {
            dropRelicToZone(pFromCreature);
            dropFlagToZone(pFromCreature);

            //////////////////////////////////////////////////////////////////////
            
            //////////////////////////////////////////////////////////////////////

            
            PartyInviteInfoManager* pPIIM = pZone->getPartyInviteInfoManager();
            Assert(pPIIM != NULL);
            pPIIM->cancelInvite(pFromCreature);

            
            int PartyID = pFromCreature->getPartyID();
            if (PartyID != 0) {
                
                LocalPartyManager* pLPM = pZone->getLocalPartyManager();
                Assert(pLPM != NULL);
                pLPM->deletePartyMember(PartyID, pFromCreature);

                
                deleteAllPartyInfo(pFromCreature);
            }

            
            TradeManager* pTM = pZone->getTradeManager();
            Assert(pTM != NULL);
            pTM->cancelTrade(pFromCreature);

            //////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////

            Slayer* pNewSlayer = new Slayer;
            Vampire* pVampire = dynamic_cast<Vampire*>(pFromCreature);

            
            Statement* pStmt = NULL;
            BEGIN_DB {
                pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                StringStream sql;
                sql << "DELETE FROM EffectBloodDrain WHERE OwnerID = '" + pFromCreature->getName() + "'";
                pStmt->executeQueryString(sql.toString());
                SAFE_DELETE(pStmt);
            }
            END_DB(pStmt)

            pNewSlayer->setName(pFromCreature->getName());

            
            Player* pFromPlayer = pFromCreature->getPlayer();
            pNewSlayer->setPlayer(pFromPlayer);
            GamePlayer* pFromGamePlayer = dynamic_cast<GamePlayer*>(pFromPlayer);
            pFromGamePlayer->setCreature(pNewSlayer);

            
            pNewSlayer->load();
            pNewSlayer->setZone(pZone);
            pNewSlayer->setObjectID(pFromCreature->getObjectID());
            // pZone->getObjectRegistry().registerObject(pNewSlayer);
            pNewSlayer->setMoveMode(Creature::MOVE_MODE_WALKING);

            ZoneCoord_t x = pFromCreature->getX();
            ZoneCoord_t y = pFromCreature->getY();
            Dir_t dir = pFromCreature->getDir();
            Tile& tile = pZone->getTile(x, y);

            
            
            
            g_pPCFinder->deleteCreature(pFromCreature->getName());
            g_pPCFinder->addCreature(pNewSlayer);

            
            if (pVampire->getGuildID() != 0) {
                Guild* pGuild = g_pGuildManager->getGuild(pVampire->getGuildID());
                if (pGuild != NULL) {
                    pGuild->deleteCurrentMember(pVampire->getName());

                    GSGuildMemberLogOn gsGuildMemberLogOn;
                    gsGuildMemberLogOn.setGuildID(pGuild->getID());
                    gsGuildMemberLogOn.setName(pVampire->getName());
                    gsGuildMemberLogOn.setLogOn(false);

                    g_pSharedServerManager->sendPacket(&gsGuildMemberLogOn);

                    Statement* pStmt = NULL;
                    
                    BEGIN_DB {
                        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                        pStmt->executeQuery("UPDATE GuildMember SET LogOn = 0 WHERE Name = '%s'",
                                            pVampire->getName().c_str());
                    }
                    END_DB(pStmt)
                } else
                    filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pVampire->getGuildID(),
                            pVampire->getName().c_str());
            }

            
            Inventory* pInventory = pVampire->getInventory();
            pNewSlayer->setInventory(pInventory);
            pVampire->setInventory(NULL);

            
            pNewSlayer->deleteStash();
            pNewSlayer->setStash(pVampire->getStash());
            pNewSlayer->setStashNum(pVampire->getStashNum());
            pNewSlayer->setStashStatus(false);
            pVampire->setStash(NULL);

             

            
            pNewSlayer->deleteFlagSet();
            pNewSlayer->setFlagSet(pVampire->getFlagSet());
            pVampire->setFlagSet(NULL);

            Item* pItem = NULL;
            _TPOINT point;

            
            for (int part = 0; part < (int)Vampire::VAMPIRE_WEAR_MAX; part++) {
                pItem = pVampire->getWearItem((Vampire::WearPart)part);
                if (pItem != NULL) {
                    
                    if (isTwohandWeapon(pItem)) {
                        Assert(((Vampire::WearPart)part == Vampire::WEAR_RIGHTHAND) ||
                               ((Vampire::WearPart)part == Vampire::WEAR_LEFTHAND));
                        Assert(pVampire->getWearItem(Vampire::WEAR_RIGHTHAND) ==
                               pVampire->getWearItem(Vampire::WEAR_LEFTHAND));
                        
                        pVampire->deleteWearItem(Vampire::WEAR_RIGHTHAND);
                        pVampire->deleteWearItem(Vampire::WEAR_LEFTHAND);
                    } else {
                        pVampire->deleteWearItem((Vampire::WearPart)part);
                    }

                    
                    if (pInventory->getEmptySlot(pItem, point)) {
                        pInventory->addItem(point.x, point.y, pItem);
                        pItem->save(pNewSlayer->getName(), STORAGE_INVENTORY, 0, point.x, point.y);
                    } else if (pItem->isTimeLimitItem()) {
                        pVampire->deleteItemByMorph(pItem);

                        pItem->destroy();
                        SAFE_DELETE(pItem);
                    }
                    
                    else {
                        ZoneCoord_t ZoneX = pVampire->getX();
                        ZoneCoord_t ZoneY = pVampire->getY();

                        TPOINT pt;

                        pt = pZone->addItem(pItem, ZoneX, ZoneY);

                        if (pt.x != -1) {
                            pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);

                            
                            if (pItem != NULL && pItem->isTraceItem()) {
                                char zoneName[15];
                                sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                remainTraceLog(pItem, pFromCreature->getName(), zoneName, ITEM_LOG_MOVE, DETAIL_DROP);
                            }
                        } else {
                            
                            if (pItem != NULL && pItem->isTraceItem()) {
                                remainTraceLog(pItem, pFromCreature->getName(), "GOD", ITEM_LOG_DELETE, DETAIL_DROP);
                            }

                            pItem->destroy();
                            SAFE_DELETE(pItem);
                        }
                    }
                }
            }

            pItem = pVampire->getExtraInventorySlotItem();
            if (pItem != NULL) {
                pVampire->deleteItemFromExtraInventorySlot();

                
                if (pInventory->getEmptySlot(pItem, point)) {
                    pInventory->addItem(point.x, point.y, pItem);
                    pItem->save(pNewSlayer->getName(), STORAGE_INVENTORY, 0, point.x, point.y);
                } else if (pItem->isTimeLimitItem()) {
                    pVampire->deleteItemByMorph(pItem);

                    pItem->destroy();
                    SAFE_DELETE(pItem);
                }
                
                else {
                    TPOINT pt;
                    ZoneCoord_t ZoneX = pVampire->getX();
                    ZoneCoord_t ZoneY = pVampire->getY();

                    pt = pZone->addItem(pItem, ZoneX, ZoneY);

                    if (pt.x != -1) {
                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);

                        
                        if (pItem != NULL && pItem->isTraceItem()) {
                            char zoneName[15];
                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                            remainTraceLog(pItem, pFromCreature->getName(), zoneName, ITEM_LOG_MOVE, DETAIL_DROP);
                        }
                    } else {
                        
                        if (pItem != NULL && pItem->isTraceItem()) {
                            remainTraceLog(pItem, pFromCreature->getName(), "GOD", ITEM_LOG_DELETE, DETAIL_DROP);
                        }

                        pItem->destroy();
                        SAFE_DELETE(pItem);
                    }
                }
            }

            
            pNewSlayer->loadTimeLimitItem();

            
            
            
            pNewSlayer->setGoldEx(0);
            pNewSlayer->setStashGoldEx(0);

            
            pNewSlayer->sendSlayerSkillInfo();


            
            tile.deleteCreature(pFromCreature->getObjectID());
            pZone->deletePC(pFromCreature);

            TPOINT pt = findSuitablePosition(pZone, x, y, Creature::MOVE_MODE_WALKING);
            Tile& newtile = pZone->getTile(pt.x, pt.y);

            newtile.addCreature(pNewSlayer);
            pNewSlayer->setXYDir(pt.x, pt.y, dir);

            pZone->addPC(pNewSlayer);

            
            pZone->updateHiddenScan(pNewSlayer);

            _GCMorph1.setPCInfo2(pNewSlayer->getSlayerInfo2());
            _GCMorph1.setInventoryInfo(pNewSlayer->getInventoryInfo());
            _GCMorph1.setGearInfo(pNewSlayer->getGearInfo());
            _GCMorph1.setExtraInfo(pNewSlayer->getExtraInfo());

            _GCMorphSlayer2.setSlayerInfo(pNewSlayer->getSlayerInfo3());

            pFromPlayer->sendPacket(&_GCMorph1);
            // pFromGamePlayer->deleteEvent(Event::EVENT_CLASS_REGENERATION);


            _GCSkillToObjectOK1.setSkillType(SkillType);
            _GCSkillToObjectOK1.setCEffectID(CEffectID);
            _GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
            _GCSkillToObjectOK1.setDuration(0);

            pPlayer->sendPacket(&_GCSkillToObjectOK1);


            GCDeleteObject _GCDeleteObject;
            _GCDeleteObject.setObjectID(TargetObjectID);
            pZone->broadcastPacket(x, y, &_GCDeleteObject, pNewSlayer);
            pZone->broadcastPacket(x, y, &_GCMorphSlayer2, pNewSlayer);


            pNewSlayer->tinysave("Race='SLAYER'");
            SAFE_DELETE(pFromCreature);


            pSkillSlot->setRunTime(0);

            EffectRestore* pEffectRestore = new EffectRestore(pNewSlayer);
            pEffectRestore->setDeadline(60 * 60 * 24 * 7 * 10); 
            pNewSlayer->addEffect(pEffectRestore);
            pNewSlayer->setFlag(Effect::EFFECT_CLASS_RESTORE);
            pEffectRestore->create(pNewSlayer->getName());
        } else {
            executeSkillFailNormal(pSlayer, getSkillType(), pFromCreature);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pSlayer, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Restore::execute(NPC* pNPC, Creature* pFromCreature)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    Assert(pNPC != NULL);
    Assert(pFromCreature != NULL);

    try {
        Zone* pZone = pNPC->getZone();
        Assert(pZone != NULL);

        
        if (!pFromCreature->isVampire()) {
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }

        GCMorph1 _GCMorph1;             
        GCMorphSlayer2 _GCMorphSlayer2; 

        // SkillType_t SkillType = SKILL_RESTORE;

        // bool bRangeCheck = verifyDistance(pNPC, pFromCreature, pSkillInfo->getRange());
        bool bHitRoll = true;

        // if (bRangeCheck && bHitRoll)
        if (bHitRoll) {
            //////////////////////////////////////////////////////////////////////
            
            //////////////////////////////////////////////////////////////////////

            
            PartyInviteInfoManager* pPIIM = pZone->getPartyInviteInfoManager();
            Assert(pPIIM != NULL);
            pPIIM->cancelInvite(pFromCreature);

            
            int PartyID = pFromCreature->getPartyID();
            if (PartyID != 0) {
                
                LocalPartyManager* pLPM = pZone->getLocalPartyManager();
                Assert(pLPM != NULL);
                pLPM->deletePartyMember(PartyID, pFromCreature);

                
                deleteAllPartyInfo(pFromCreature);
            }

            
            TradeManager* pTM = pZone->getTradeManager();
            Assert(pTM != NULL);
            pTM->cancelTrade(pFromCreature);

            //////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////

            Slayer* pNewSlayer = new Slayer;
            Vampire* pVampire = dynamic_cast<Vampire*>(pFromCreature);

            
            Statement* pStmt = NULL;
            BEGIN_DB {
                pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                StringStream sql;
                sql << "DELETE FROM EffectBloodDrain WHERE OwnerID = '" + pFromCreature->getName() + "'";
                pStmt->executeQueryString(sql.toString());
                SAFE_DELETE(pStmt);
            }
            END_DB(pStmt)

            pNewSlayer->setName(pFromCreature->getName());
            pNewSlayer->setPlayer(pFromCreature->getPlayer());
            pNewSlayer->load();
            
            pNewSlayer->setZone(pZone);
            pNewSlayer->setObjectID(pFromCreature->getObjectID());
            // pZone->getObjectRegistry().registerObject(pNewSlayer);
            pNewSlayer->setMoveMode(Creature::MOVE_MODE_WALKING);

            ZoneCoord_t x = pFromCreature->getX();
            ZoneCoord_t y = pFromCreature->getY();
            Dir_t dir = pFromCreature->getDir();
            Tile& tile = pZone->getTile(x, y);

            pNewSlayer->setXYDir(x, y, dir);

            
            Player* pFromPlayer = pFromCreature->getPlayer();
            // pNewSlayer->setPlayer(pFromPlayer);
            GamePlayer* pFromGamePlayer = dynamic_cast<GamePlayer*>(pFromPlayer);
            pFromGamePlayer->setCreature(pNewSlayer);

            
            
            
            g_pPCFinder->deleteCreature(pFromCreature->getName());
            g_pPCFinder->addCreature(pNewSlayer);

            
            if (pVampire->getGuildID() != 0) {
                Guild* pGuild = g_pGuildManager->getGuild(pVampire->getGuildID());
                if (pGuild != NULL)
                    pGuild->deleteCurrentMember(pVampire->getName());
                else
                    filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pVampire->getGuildID(),
                            pVampire->getName().c_str());
            }

            
            Inventory* pInventory = pVampire->getInventory();
            pNewSlayer->setInventory(pInventory);
            pVampire->setInventory(NULL);

            
            pNewSlayer->deleteStash();
            pNewSlayer->setStash(pVampire->getStash());
            pNewSlayer->setStashNum(pVampire->getStashNum());
            pNewSlayer->setStashStatus(false);
            pVampire->setStash(NULL);

            
            pNewSlayer->deleteFlagSet();
            pNewSlayer->setFlagSet(pVampire->getFlagSet());
            pVampire->setFlagSet(NULL);

            Item* pItem = NULL;
            _TPOINT point;

            
            for (int part = 0; part < (int)Vampire::VAMPIRE_WEAR_MAX; part++) {
                pItem = pVampire->getWearItem((Vampire::WearPart)part);
                if (pItem != NULL) {
                    
                    if (isTwohandWeapon(pItem)) {
                        Assert(((Vampire::WearPart)part == Vampire::WEAR_RIGHTHAND) ||
                               ((Vampire::WearPart)part == Vampire::WEAR_LEFTHAND));
                        Assert(pVampire->getWearItem(Vampire::WEAR_RIGHTHAND) ==
                               pVampire->getWearItem(Vampire::WEAR_LEFTHAND));
                        
                        pVampire->deleteWearItem(Vampire::WEAR_RIGHTHAND);
                        pVampire->deleteWearItem(Vampire::WEAR_LEFTHAND);
                    } else {
                        pVampire->deleteWearItem((Vampire::WearPart)part);
                    }

                    
                    if (pInventory->getEmptySlot(pItem, point)) {
                        pInventory->addItem(point.x, point.y, pItem);
                        pItem->save(pNewSlayer->getName(), STORAGE_INVENTORY, 0, point.x, point.y);
                    } else if (pItem->isTimeLimitItem()) {
                        pVampire->deleteItemByMorph(pItem);

                        pItem->destroy();
                        SAFE_DELETE(pItem);
                    }
                    
                    else {
                        ZoneCoord_t ZoneX = pVampire->getX();
                        ZoneCoord_t ZoneY = pVampire->getY();

                        TPOINT pt;

                        pt = pZone->addItem(pItem, ZoneX, ZoneY);

                        if (pt.x != -1) {
                            pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);

                            
                            if (pItem != NULL && pItem->isTraceItem()) {
                                char zoneName[15];
                                sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                                remainTraceLog(pItem, pFromCreature->getName(), zoneName, ITEM_LOG_MOVE, DETAIL_DROP);
                            }
                        } else {
                            
                            if (pItem != NULL && pItem->isTraceItem()) {
                                remainTraceLog(pItem, pFromCreature->getName(), "GOD", ITEM_LOG_DELETE, DETAIL_DROP);
                            }

                            pItem->destroy();
                            SAFE_DELETE(pItem);
                        }
                    }
                }
            }

            pItem = pVampire->getExtraInventorySlotItem();
            if (pItem != NULL) {
                pVampire->deleteItemFromExtraInventorySlot();

                
                if (pInventory->getEmptySlot(pItem, point)) {
                    pInventory->addItem(point.x, point.y, pItem);
                    pItem->save(pNewSlayer->getName(), STORAGE_INVENTORY, 0, point.x, point.y);
                } else if (pItem->isTimeLimitItem()) {
                    pVampire->deleteItemByMorph(pItem);

                    pItem->destroy();
                    SAFE_DELETE(pItem);
                }
                
                else {
                    TPOINT pt;
                    ZoneCoord_t ZoneX = pVampire->getX();
                    ZoneCoord_t ZoneY = pVampire->getY();

                    pt = pZone->addItem(pItem, ZoneX, ZoneY);

                    if (pt.x != -1) {
                        pItem->save("", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);

                        
                        if (pItem != NULL && pItem->isTraceItem()) {
                            char zoneName[15];
                            sprintf(zoneName, "%4d%3d%3d", pZone->getZoneID(), pt.x, pt.y);
                            remainTraceLog(pItem, pFromCreature->getName(), zoneName, ITEM_LOG_MOVE, DETAIL_DROP);
                        }
                    } else {
                        
                        if (pItem != NULL && pItem->isTraceItem()) {
                            remainTraceLog(pItem, pFromCreature->getName(), "GOD", ITEM_LOG_DELETE, DETAIL_DROP);
                        }

                        pItem->destroy();
                        SAFE_DELETE(pItem);
                    }
                }
            }

            pNewSlayer->loadTimeLimitItem();

            
            
            // pNewSlayer->setGoldEx(pVampire->getGold());
            pNewSlayer->setGoldEx(0);
            pNewSlayer->setStashGoldEx(0);


            
            pNewSlayer->sendSlayerSkillInfo();

            _GCMorph1.setPCInfo2(pNewSlayer->getSlayerInfo2());
            _GCMorph1.setInventoryInfo(pNewSlayer->getInventoryInfo());
            _GCMorph1.setGearInfo(pNewSlayer->getGearInfo());
            _GCMorph1.setExtraInfo(pNewSlayer->getExtraInfo());

            _GCMorphSlayer2.setSlayerInfo(pNewSlayer->getSlayerInfo3());

            pFromPlayer->sendPacket(&_GCMorph1);
            // pFromGamePlayer->deleteEvent(Event::EVENT_CLASS_REGENERATION);


            
            tile.deleteCreature(pFromCreature->getObjectID());
            pZone->deletePC(pFromCreature);

            TPOINT pt = findSuitablePosition(pZone, x, y, Creature::MOVE_MODE_WALKING);
            Tile& newtile = pZone->getTile(pt.x, pt.y);

            newtile.addCreature(pNewSlayer);
            pNewSlayer->setXYDir(pt.x, pt.y, dir);

            pZone->addPC(pNewSlayer);


            GCDeleteObject _GCDeleteObject;
            _GCDeleteObject.setObjectID(pFromCreature->getObjectID());
            pZone->broadcastPacket(x, y, &_GCDeleteObject, pNewSlayer);
            pZone->broadcastPacket(x, y, &_GCMorphSlayer2, pNewSlayer);

            pNewSlayer->tinysave("Race='SLAYER'");
            SAFE_DELETE(pFromCreature);


            
            pZone->updateHiddenScan(pNewSlayer);

            EffectRestore* pEffectRestore = new EffectRestore(pNewSlayer);
            pEffectRestore->setDeadline(60 * 60 * 24 * 7 * 10); 
            pNewSlayer->addEffect(pEffectRestore);
            pNewSlayer->setFlag(Effect::EFFECT_CLASS_RESTORE);
            pEffectRestore->create(pNewSlayer->getName());
        } else {
            executeSkillFailNormal(pNPC, getSkillType(), pFromCreature);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pNPC, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

Restore g_Restore;
