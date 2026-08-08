//////////////////////////////////////////////////////////////////////////////
// Filename    : ZoneUtil.cpp
// Written by  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#include "ZoneUtil.h"

#include "Assert.h"
#include "CastleInfoManager.h"
#include "Corpse.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "Effect.h"
#include "EffectDarknessForbidden.h"
#include "EffectGnomesWhisper.h"
#include "EffectManager.h"
#include "EffectObservingEye.h"
#include "EffectPrecedence.h"
#include "EventTransport.h"
#include "GamePlayer.h"
#include "GameServerInfoManager.h"
#include "Gpackets/GCAddBurrowingCreature.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddInstalledMineToZone.h"
#include "Gpackets/GCAddMonster.h"
#include "Gpackets/GCAddMonsterFromBurrowing.h"
#include "Gpackets/GCAddMonsterFromTransformation.h"
#include "Gpackets/GCAddNPC.h"
#include "Gpackets/GCAddNewItemToZone.h"
#include "Gpackets/GCAddSlayer.h"
#include "Gpackets/GCAddVampire.h"
#include "Gpackets/GCAddVampireFromBurrowing.h"
#include "Gpackets/GCAddVampireFromTransformation.h"
#include "Gpackets/GCDeleteInventoryItem.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCDropItemToZone.h"
#include "Gpackets/GCFastMove.h"
#include "Gpackets/GCGetOffMotorCycle.h"
#include "Gpackets/GCMineExplosionOK1.h"
#include "Gpackets/GCMineExplosionOK2.h"
#include "Gpackets/GCMove.h"
#include "Gpackets/GCMoveError.h"
#include "Gpackets/GCMoveOK.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCSetPosition.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCUnburrowFail.h"
#include "Gpackets/GCUnburrowOK.h"
#include "Gpackets/GCUntransformFail.h"
#include "Gpackets/GCUntransformOK.h"
#include "IncomingPlayerManager.h"
#include "Item.h"
#include "LevelWarZoneInfoManager.h"
#include "MasterLairManager.h"
#include "Monster.h"
#include "MonsterCorpse.h"
#include "MonsterManager.h"
#include "MonsterSummonInfo.h"
#include "PKZoneInfoManager.h"
#include "PacketUtil.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "Relic.h"
#include "RelicUtil.h"
#include "ResurrectLocationManager.h"
#include "SkillHandler.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "StringPool.h"
#include "TimeManager.h"
#include "Vampire.h"
#include "VariableManager.h"
#include "VisionInfo.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "ZoneUtil.h"
#include "ctf/FlagManager.h"
#include "item/Mine.h"
#include "skill/EffectTrapInstalled.h"
#include "skill/SummonGroundElemental.h"
#include "war/WarSystem.h"
// #include "skill/EffectRevealer.h"

string correctString(const string& str) {
    __BEGIN_TRY

    string correct = str;

    unsigned int i = 0;
    unsigned int size = str.size();

    while (i < size) {
        if (correct[i] == '\\') {
            correct.replace(i, 1, "\\\\");
            i = i + 2;
            size++;
        } else if (correct[i] == '\'') {
            correct.replace(i, 1, "\\'");
            i = i + 2;
            size++;
        } else {
            i++;
        }
    }

    return correct;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
TPOINT findSuitablePosition(Zone* pZone, ZoneCoord_t cx, ZoneCoord_t cy, Creature::MoveMode MMode) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);

    int x = cx;
    int y = cy;
    int sx = 1;
    int sy = 0;
    int maxCount = 1;
    int count = 1;
    int checkCount = 300;
    TPOINT pt;

    do {
        if (x > 0 && y > 0 && x < pZone->getWidth() && y < pZone->getHeight()) {
            Tile& rTile = pZone->getTile(x, y);
            /*
            if (rTile.isBlocked(MMode) == false && rTile.hasPortal() == false)
            {
                pt.x = x;
                pt.y = y;
                return pt;
            }
            */
            if (rTile.isBlocked(MMode) == false && rTile.hasPortal() == false) {
                pt.x = x;
                pt.y = y;
                return pt;
            }

            /*
            if (rTile.isBlocked(MMode))
            {
                cout << "[" << checkCount << "] Block : (" << x << ", " << y << ")" << endl;
            }

            if (rTile.hasPortal())
            {
                cout << "[" << checkCount << "] Portal : (" << x << ", " << y << ")" << endl;
            }
            */
        }

        x += sx;
        y += sy;

        if (--count == 0) {
            if (sx == 0)
                maxCount++;

            int temp = sx;
            sx = -sy;
            sy = temp;

            count = maxCount;
        }

    } while (--checkCount);

    pt.x = -1;
    pt.y = -1;
    return pt;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
TPOINT findSuitablePositionForItem(Zone* pZone, ZoneCoord_t cx, ZoneCoord_t cy, bool bAllowCreature,
                                   bool bAllowSafeZone, bool bForce) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);

    int x = cx;
    int y = cy;
    int sx = 1;
    int sy = 0;
    int maxCount = 1;
    int count = 1;
    int checkCount = 300;
    TPOINT pt;

    do {
        if (x > 2 && y > 2 && x < pZone->getWidth() - 2 && y < pZone->getHeight() - 2) {
            Tile& rTile = pZone->getTile(x, y);


            if ((!rTile.isGroundBlocked() || rTile.hasWalkingCreature()) && rTile.hasItem() == false &&
                rTile.hasPortal() == false) {
                if (bAllowSafeZone || !(pZone->getZoneLevel(x, y) & SAFE_ZONE)) {
                    pt.x = x;
                    pt.y = y;
                    return pt;
                }


                //				if (bAllowCreature == false && rTile.hasCreature() == false)
                //				{
                //					pt.x = x;
                //					pt.y = y;
                //					return pt;
                //				}
                //				else
                //				{
                //					pt.x = x;
                //					pt.y = y;
                //					return pt;
                //				}
            }

            if (bForce && rTile.hasItem()) {
                Item* pTileItem = rTile.getItem();

                if (pTileItem != NULL) {
                    if (pTileItem->getItemClass() != Item::ITEM_CLASS_CORPSE) {
                        pZone->deleteItem(pTileItem, x, y);
                        pTileItem->destroy();
                        SAFE_DELETE(pTileItem);

                        pt.x = x;
                        pt.y = y;
                        return pt;
                    }
                }
            }
        }

        x += sx;
        y += sy;

        if (--count == 0) {
            if (sx == 0)
                maxCount++;

            int temp = sx;
            sx = -sy;
            sy = temp;

            count = maxCount;
        }

    } while (--checkCount);

    pt.x = -1;
    pt.y = -1;
    return pt;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
TPOINT findSuitablePositionForEffect(Zone* pZone, ZoneCoord_t cx, ZoneCoord_t cy, Effect::EffectClass EClass) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);

    int x = cx;
    int y = cy;
    int sx = 1;
    int sy = 0;
    int maxCount = 1;
    int count = 1;
    int checkCount = 300;
    TPOINT pt;

    do {
        if (x > 0 && y > 0 && x < pZone->getWidth() && y < pZone->getHeight()) {
            Tile& rTile = pZone->getTile(x, y);

            if (rTile.canAddEffect() && rTile.getEffect(EClass) == NULL) {
                bool bNearTileCheck = true;


                for (int i = 0; i < 8; i++) {
                    int tileX = x + dirMoveMask[i].x;
                    int tileY = y + dirMoveMask[i].y;

                    if (pZone->getOuterRect()->ptInRect(tileX, tileY)) {
                        Tile& rTile2 = pZone->getTile(tileX, tileY);
                        if (rTile2.getEffect(EClass) != NULL) {
                            bNearTileCheck = false;
                            break;
                        }
                    }
                }

                if (bNearTileCheck) {
                    pt.x = x;
                    pt.y = y;
                    return pt;
                }
            }
        }

        x += sx;
        y += sy;

        if (--count == 0) {
            if (sx == 0)
                maxCount++;

            int temp = sx;
            sx = -sy;
            sy = temp;

            count = maxCount;
        }

    } while (--checkCount);

    pt.x = -1;
    pt.y = -1;

    return pt;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
bool canAddCreature(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Creature::MoveMode MMode) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);

    if (x > 0 && y > 0 && x < pZone->getWidth() && y < pZone->getHeight()) {
        if (!pZone->getTile(x, y).isBlocked(MMode)) {
            return true;
        }
    }

    return false;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
bool canBurrow(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);

    return canAddCreature(pZone, x, y, Creature::MOVE_MODE_BURROWING);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
bool canUnburrow(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);

    return canAddCreature(pZone, x, y, Creature::MOVE_MODE_WALKING);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
Dir_t knockbackCreature(Zone* pZone, Creature* pCreature, ZoneCoord_t originX,
                        ZoneCoord_t originY) throw(ProtocolException, Error) {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pCreature != NULL);

    if (pCreature->isDead() || pCreature->isFlag(Effect::EFFECT_CLASS_COMA) ||
        pCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE) || pCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET)) {
        return UP;
    }

    if (pCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);
        if (pMonster != NULL && pMonster->getMonsterType() == GROUND_ELEMENTAL_TYPE)
            return UP;
        //		if ( pMonster->getMonsterType() == 722 ) return UP;
        switch (pMonster->getMonsterType()) {
        case 722:
        case 723:
        case 717:
        case 721:
        case 724:
        case 725:
        case 726:
        case 727:
        case 728:
        case 729:
        case 734:
        case 735:
        case 736:
        case 737:
        case 764:
        case 765:
            return UP;
        default:
            break;
        }

        //		if ( pMonster->hasRelic() || pMonster->getBrain() == NULL ) return UP;
    }


    ZoneCoord_t nx = pCreature->getX();
    ZoneCoord_t ny = pCreature->getY();
    ZoneCoord_t cx = nx;
    ZoneCoord_t cy = ny;
    ZoneCoord_t height = pZone->getHeight();
    ZoneCoord_t width = pZone->getWidth();
    Dir_t dir = calcDirection(originX, originY, nx, ny);

    Tile& rOriginTile = pZone->getTile(cx, cy);
    if (rOriginTile.getEffect(Effect::EFFECT_CLASS_TRYING_POSITION) != NULL)
        return UP;


    switch (dir) {
    case UP:
        if (ny > 0) {
            ny -= 1;
        }
        break;
    case DOWN:
        if (ny < (height - 1)) {
            ny += 1;
        }
        break;
    case LEFT:
        if (nx > 0) {
            nx -= 1;
        }
        break;
    case RIGHT:
        if (nx < (width - 1)) {
            nx += 1;
        }
        break;
    case LEFTUP:
        if (nx > 0 && ny > 0) {
            nx -= 1;
            ny -= 1;
        }
        break;
    case RIGHTUP:
        if (nx < (width - 1) && ny > 0) {
            nx += 1;
            ny -= 1;
        }
        break;
    case LEFTDOWN:
        if (nx > 0 && ny < (height - 1)) {
            nx -= 1;
            ny += 1;
        }
        break;
    case RIGHTDOWN:
        if (nx < (width - 1) && ny < (height - 1)) {
            nx += 1;
            ny += 1;
        }
        break;
    }


    Tile& rTargetTile = pZone->getTile(nx, ny);
    if (!pCreature->isFlag(Effect::EFFECT_CLASS_CASKET) && !rTargetTile.isBlocked(pCreature->getMoveMode()) &&
        !pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) && !rTargetTile.hasPortal()) {
        pCreature->setX(nx);
        pCreature->setY(ny);

        try {
            rOriginTile.deleteCreature(pCreature->getObjectID());


            if (!rTargetTile.addCreature(pCreature)) {
                return dir;
            }


            try {
                checkMine(pZone, pCreature, nx, ny);
                checkTrap(pZone, pCreature);
            } catch (Throwable& t) {
                filelog("CheckMineBug.txt", "%s : %s", "KnockBackCreature", t.toString().c_str());
            }


            if (pCreature->isPC()) {
                pZone->movePCBroadcast(pCreature, cx, cy, nx, ny, false, true);
            } else {
                pZone->moveCreatureBroadcast(pCreature, cx, cy, nx, ny, false, true);
            }
        } catch (NoSuchElementException& nsee) {
            throw Error("No creature on previous tile");
        } catch (DuplicatedException& de) {
            throw Error("Thers's a creature on new tile");
        } catch (PortalException&) {
        } catch (Error& e) {
            filelog("assertTile.txt", "knockbackCreature : %s", e.toString().c_str());
            throw;
        }
    }

    return dir;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addBurrowingCreature(Zone* pZone, Creature* pCreature, ZoneCoord_t cx,
                          ZoneCoord_t cy) throw(EmptyTileNotExistException, Error) {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pCreature != NULL);
    Assert(pCreature->isVampire() || pCreature->isMonster());

    TPOINT pt = findSuitablePosition(pZone, cx, cy, Creature::MOVE_MODE_BURROWING);

    if (pt.x != -1) {
        pCreature->setFlag(Effect::EFFECT_CLASS_HIDE);
        Assert(pCreature->getMoveMode() == Creature::MOVE_MODE_WALKING);

        Tile& oldTile = pZone->getTile(pCreature->getX(), pCreature->getY());
        Tile& newTile = pZone->getTile(pt.x, pt.y);

        try {
            oldTile.deleteCreature(pCreature->getObjectID());
        } catch (Error& e) {
            filelog("assertTile.txt", "addBurrowingCreature : %s", e.toString().c_str());
            throw;
        }

        pCreature->setMoveMode(Creature::MOVE_MODE_BURROWING);
        newTile.addCreature(pCreature);

        Assert(pCreature == newTile.getCreature(pCreature->getMoveMode()));


        pCreature->setXYDir(pt.x, pt.y, pCreature->getDir());

        // scanPC(pCreature);

        // gcDO.setObjectID(pCreature->getObjectID());


        GCAddBurrowingCreature gcABC;
        gcABC.setObjectID(pCreature->getObjectID());
        gcABC.setName(pCreature->getName());
        gcABC.setX(pt.x);
        gcABC.setY(pt.y);

        //--------------------------------------------------------------------------------
        //


        //
        //--------------------------------------------------------------------------------


        pZone->broadcastPacket(pt.x, pt.y, &gcABC, pCreature);
    } else
        throw EmptyTileNotExistException("addBurrowingCreature() : Tile is not empty.");

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addUnburrowCreature(Zone* pZone, Creature* pCreature, ZoneCoord_t cx, ZoneCoord_t cy,
                         Dir_t dir) throw(EmptyTileNotExistException, Error) {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pCreature != NULL);
    Assert(pCreature->isFlag(Effect::EFFECT_CLASS_HIDE));

    TPOINT pt = findSuitablePosition(pZone, cx, cy, Creature::MOVE_MODE_WALKING);

    if (pt.x != -1) {
        ZoneCoord_t oldX = pCreature->getX();
        ZoneCoord_t oldY = pCreature->getY();
        Tile& oldTile = pZone->getTile(oldX, oldY);
        Tile& newTile = pZone->getTile(pt.x, pt.y);


        GCDeleteObject gcDO;
        gcDO.setObjectID(pCreature->getObjectID());
        pZone->broadcastPacket(oldX, oldY, &gcDO, pCreature);


        pCreature->removeFlag(Effect::EFFECT_CLASS_HIDE);


        try {
            oldTile.deleteCreature(pCreature->getObjectID());
        } catch (Error& e) {
            filelog("assertTile.txt", "addUnburrowCreature : %s", e.toString().c_str());
            throw;
        }
        pCreature->setMoveMode(Creature::MOVE_MODE_WALKING);
        newTile.addCreature(pCreature);

        Assert(pCreature == newTile.getCreature(pCreature->getMoveMode()));


        pCreature->setXYDir(pt.x, pt.y, dir);

        // scanPC(pCreature);

        Creature::CreatureClass CClass = pCreature->getCreatureClass();
        if (CClass == Creature::CREATURE_CLASS_VAMPIRE) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            GCAddVampireFromBurrowing gcAVFB(pVampire->getVampireInfo3());
            gcAVFB.setEffectInfo(pVampire->getEffectInfo());
            pZone->broadcastPacket(pt.x, pt.y, &gcAVFB, pCreature);


            GCUnburrowOK gcUnburrowOK(pt.x, pt.y, dir);
            Player* pPlayer = pCreature->getPlayer();
            GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
            pGamePlayer->sendPacket(&gcUnburrowOK);
        } else if (CClass == Creature::CREATURE_CLASS_MONSTER) {
            Monster* pMonster = dynamic_cast<Monster*>(pCreature);

            GCAddMonsterFromBurrowing gcAMFB;
            gcAMFB.setObjectID(pMonster->getObjectID());
            gcAMFB.setMonsterType(pMonster->getMonsterType());
            gcAMFB.setMonsterName(pMonster->getMonsterName());
            gcAMFB.setX(pt.x);
            gcAMFB.setY(pt.y);
            gcAMFB.setDir(dir);
            gcAMFB.setEffectInfo(pMonster->getEffectInfo());
            gcAMFB.setCurrentHP(pMonster->getHP());
            gcAMFB.setMaxHP(pMonster->getHP(ATTR_MAX));

            pZone->broadcastPacket(pt.x, pt.y, &gcAMFB);
        } else {
            throw Error("invalid creature type");
        }
    } else {
        if (pCreature->isPC()) {
            GCUnburrowFail gcUnburrowFail;
            pCreature->getPlayer()->sendPacket(&gcUnburrowFail);
        } else {
            cerr << "addUnburrowCreature() : Cannot find suitable position" << endl;
            throw Error("Cannot unburrow monster.");
        }
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addUntransformCreature(Zone* pZone, Creature* pCreature, bool bForce) throw() {
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pZone != NULL);
    Assert(pCreature != NULL);


    Assert(pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF) ||
           pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
           pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) ||
           pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF));

    ZoneCoord_t cx = pCreature->getX();
    ZoneCoord_t cy = pCreature->getY();

    TPOINT pt = findSuitablePosition(pZone, cx, cy, Creature::MOVE_MODE_WALKING);

    if (pt.x != -1) {
        Range_t rangeDiff = 0;

        if (pt.x != cx || pt.y != cy) {
            rangeDiff = max(abs((int)(pt.x) - (int)(cx)), abs((int)(pt.y) - (int)(cy)));
        }

        ZoneCoord_t oldX = pCreature->getX();
        ZoneCoord_t oldY = pCreature->getY();

        GCDeleteObject gcDO;
        gcDO.setObjectID(pCreature->getObjectID());
        pZone->broadcastPacket(oldX, oldY, &gcDO, pCreature);


        EffectManager* pEffectManager = pCreature->getEffectManager();
        Assert(pEffectManager != NULL);

        if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) {
            pCreature->removeFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF);


            if (bForce) {
                GCRemoveEffect gcRemoveEffect;
                gcRemoveEffect.setObjectID(pCreature->getObjectID());
                gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF);
                if (pCreature->isPC()) {
                    Player* pPlayer = pCreature->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcRemoveEffect);
                }

                pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF);
            }

            if (pCreature->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                VAMPIRE_RECORD prev;

                pVampire->getVampireRecord(prev);
                pVampire->initAllStat();
                pVampire->sendModifyInfo(prev);
            } else if (pCreature->isMonster()) {
                Monster* pMonster = dynamic_cast<Monster*>(pCreature);
                pMonster->initAllStat();
            }
        }
        if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
            pCreature->removeFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF);


            if (bForce) {
                GCRemoveEffect gcRemoveEffect;
                gcRemoveEffect.setObjectID(pCreature->getObjectID());
                gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF);
                if (pCreature->isPC()) {
                    Player* pPlayer = pCreature->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcRemoveEffect);
                }

                pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF);
            }

            if (pCreature->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                VAMPIRE_RECORD prev;

                pVampire->getVampireRecord(prev);
                pVampire->initAllStat();
                pVampire->sendModifyInfo(prev);
            } else if (pCreature->isMonster()) {
                Monster* pMonster = dynamic_cast<Monster*>(pCreature);
                pMonster->initAllStat();
            }
        } else if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)) {
            pCreature->removeFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);


            if (bForce) {
                GCRemoveEffect gcRemoveEffect;
                gcRemoveEffect.setObjectID(pCreature->getObjectID());
                gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);
                if (pCreature->isPC()) {
                    Player* pPlayer = pCreature->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcRemoveEffect);
                }

                pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);
            }

            if (pCreature->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                VAMPIRE_RECORD prev;

                pVampire->getVampireRecord(prev);
                pVampire->initAllStat();
                pVampire->sendModifyInfo(prev);
            } else if (pCreature->isMonster()) {
                Monster* pMonster = dynamic_cast<Monster*>(pCreature);
                pMonster->initAllStat();
            }
        }


        Tile& oldTile = pZone->getTile(oldX, oldY);
        Tile& newTile = pZone->getTile(pt.x, pt.y);

        try {
            oldTile.deleteCreature(pCreature->getObjectID());
        } catch (Error& e) {
            filelog("assertTile.txt", "addUntransformCreature : %s", e.toString().c_str());
            throw;
        }
        pCreature->setMoveMode(Creature::MOVE_MODE_WALKING);
        newTile.addCreature(pCreature);

        Assert(pCreature == newTile.getCreature(pCreature->getMoveMode()));


        pCreature->setXYDir(pt.x, pt.y, pCreature->getDir());


        Creature::CreatureClass CClass = pCreature->getCreatureClass();

        if (CClass == Creature::CREATURE_CLASS_VAMPIRE) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            GCAddVampireFromTransformation gcAVFT(pVampire->getVampireInfo3());
            gcAVFT.setEffectInfo(pVampire->getEffectInfo());

            pZone->broadcastPacket(pt.x, pt.y, &gcAVFT, pVampire, true, rangeDiff);

            // send to myself
            GCUntransformOK gcUntransformOK(pt.x, pt.y, pCreature->getDir());
            pCreature->getPlayer()->sendPacket(&gcUntransformOK);
        } else if (CClass == Creature::CREATURE_CLASS_MONSTER) {
            Monster* pMonster = dynamic_cast<Monster*>(pCreature);

            GCAddMonsterFromTransformation gcAMFT;
            gcAMFT.setObjectID(pMonster->getObjectID());
            gcAMFT.setMonsterType(pMonster->getMonsterType());
            gcAMFT.setMonsterName(pMonster->getMonsterName());
            gcAMFT.setX(pt.x);
            gcAMFT.setY(pt.y);
            gcAMFT.setDir(pMonster->getDir());
            gcAMFT.setEffectInfo(pMonster->getEffectInfo());
            gcAMFT.setCurrentHP(pMonster->getHP());
            gcAMFT.setMaxHP(pMonster->getHP(ATTR_MAX));

            pZone->broadcastPacket(pt.x, pt.y, &gcAMFT, NULL, true, rangeDiff);
        } else {
            throw Error("invalid creature type");
        }
    } else {
        if (pCreature->isPC()) {
            GCUntransformFail gcUntransformFail;
            pCreature->getPlayer()->sendPacket(&gcUntransformFail);
        } else {
            // throw Error("Cannot unburrow monster.");
        }
    }


    if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        GCModifyInformation gcMI;
        gcMI.addShortData(MODIFY_ATTACK_SPEED, pVampire->getAttackSpeed());
        pVampire->getPlayer()->sendPacket(&gcMI);
    }

    __END_DEBUG
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addInvisibleCreature(Zone* pZone, Creature* pCreature, ZoneCoord_t cx, ZoneCoord_t cy) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pCreature != NULL);


    Assert(pCreature->isVampire() || pCreature->isMonster());

    ObjectID_t creatureID = pCreature->getObjectID();

    GCDeleteObject gcDO;
    gcDO.setObjectID(creatureID);

    pCreature->setFlag(Effect::EFFECT_CLASS_INVISIBILITY);

    // Tile& rTile = pZone->getTile(cx, cy);

    GCAddEffect gcAddEffect;
    gcAddEffect.setObjectID(creatureID);
    gcAddEffect.setEffectID(Effect::EFFECT_CLASS_INVISIBILITY);
    gcAddEffect.setDuration(0);

    //--------------------------------------------------------------------------------
    //


    //
    //--------------------------------------------------------------------------------
    for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1),
                     endx = min(pZone->getWidth() - 1, cx + maxViewportWidth + 1);
         ix <= endx; ix++) {
        for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1),
                         endy = min(pZone->getHeight() - 1, cy + maxViewportLowerHeight + 1);
             iy <= endy; iy++) {
            Tile& curTile = pZone->getTile(ix, iy);
            const slist<Object*>& objectList = curTile.getObjectList();

            slist<Object*>::const_iterator itr = objectList.begin();
            for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE;
                 itr++) {
                Assert(*itr != NULL);
                Creature* pViewer = dynamic_cast<Creature*>(*itr);

                if (pViewer != pCreature && pViewer->isPC() && (pViewer->getVisionState(cx, cy) >= IN_SIGHT)) {
                    EffectObservingEye* pEffectObservingEye = NULL;
                    if (pViewer->isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE)) {
                        pEffectObservingEye =
                            dynamic_cast<EffectObservingEye*>(pViewer->findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
                        // Assert( pEffectObservingEye != NULL );
                    }


                    EffectGnomesWhisper* pEffectGnomesWhisper = NULL;
                    if (pViewer->isFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER)) {
                        pEffectGnomesWhisper = dynamic_cast<EffectGnomesWhisper*>(
                            pViewer->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
                        // Assert( pEffectGnomesWhisper != NULL );
                    }

                    // cout << "checking" << endl;
                    if (!pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) ||
                        pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) ||
                        (pEffectGnomesWhisper != NULL &&
                         pEffectGnomesWhisper->canSeeHide())) // || ( pEffectRevealer != NULL &&
                                                              // pEffectRevealer->canSeeHide( pCreature ) ) ))
                    {
                        if (pViewer->isVampire() || pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ||
                            (pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility(pCreature)) ||
                            (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeInvisibility())) {
                            pViewer->getPlayer()->sendPacket(&gcAddEffect);
                            // cout << "send add invisible effect" << endl;
                        } else {
                            pViewer->getPlayer()->sendPacket(&gcDO);
                            // cout << "send delete object" << endl;
                        }

                        /*						if (!pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY)
                                                    && pViewer->isSlayer())
                                                {
                                                    pViewer->getPlayer()->sendPacket(&gcDO);
                                                }
                                                else
                                                {
                                                    pViewer->getPlayer()->sendPacket(&gcAddEffect);
                                                }*/
                    } else {
                    }
                } // if
            } // for
        } // for
    } // for

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addVisibleCreature(Zone* pZone, Creature* pCreature, bool bForced) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pCreature != NULL);


    Assert(pCreature->isVampire() || pCreature->isMonster());


    Assert(pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY));

    ZoneCoord_t cx = pCreature->getX();
    ZoneCoord_t cy = pCreature->getY();
    // Tile&       rTile       = pZone->getTile(cx, cy);

    Packet* pGCAddXXX = NULL;
    GCAddMonster gcAddMonster;
    GCAddVampire gcAddVampire;
    GCAddBurrowingCreature gcABC;

    Creature::CreatureClass CClass = pCreature->getCreatureClass();

    if (CClass == Creature::CREATURE_CLASS_MONSTER) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);

        if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)) {
            gcABC.setObjectID(pMonster->getObjectID());
            gcABC.setName(pMonster->getName());
            gcABC.setX(cx);
            gcABC.setY(cy);

            pGCAddXXX = &gcABC;
        } else {
            EffectInfo* pEffectInfo = new EffectInfo;
            pEffectInfo->addListElement(Effect::EFFECT_CLASS_INVISIBILITY, 0xFFFF);

            // make packet
            gcAddMonster.setObjectID(pMonster->getObjectID());
            gcAddMonster.setMonsterType(pMonster->getMonsterType());
            gcAddMonster.setMonsterName(pMonster->getName()); // by sigi - -;
            gcAddMonster.setX(cx);
            gcAddMonster.setY(cy);
            gcAddMonster.setDir(pMonster->getDir());
            gcAddMonster.setEffectInfo(pEffectInfo);
            gcAddMonster.setCurrentHP(pMonster->getHP());
            gcAddMonster.setMaxHP(pMonster->getHP(ATTR_MAX));

            pGCAddXXX = &gcAddMonster;
        }
    } else if (CClass == Creature::CREATURE_CLASS_VAMPIRE) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)) {
            gcABC.setObjectID(pVampire->getObjectID());
            gcABC.setName(pVampire->getName());
            gcABC.setX(cx);
            gcABC.setY(cy);

            pGCAddXXX = &gcABC;
        } else {
            //			gcAddVampire.setVampireInfo(pVampire->getVampireInfo3());
            //			gcAddVampire.setEffectInfo(pVampire->getEffectInfo());
            makeGCAddVampire(&gcAddVampire, pVampire);
            pGCAddXXX = &gcAddVampire;
        }
    } else {
        throw Error();
    }

    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_INVISIBILITY);
    if (pCreature->isPC()) {
        Player* pPlayer = pCreature->getPlayer();
        Assert(pPlayer != NULL);
        pPlayer->sendPacket(&gcRemoveEffect);
    }

    //--------------------------------------------------------------------------------
    //


    //
    //--------------------------------------------------------------------------------
    for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1),
                     endx = min(pZone->getWidth() - 1, cx + maxViewportWidth + 1);
         ix <= endx; ix++) {
        for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1),
                         endy = min(pZone->getHeight() - 1, cy + maxViewportLowerHeight + 1);
             iy <= endy; iy++) {
            Tile& curTile = pZone->getTile(ix, iy);
            const slist<Object*>& objectList = curTile.getObjectList();

            slist<Object*>::const_iterator itr = objectList.begin();
            for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE;
                 itr++) {
                Assert(*itr != NULL);

                Creature* pViewer = dynamic_cast<Creature*>(*itr);


                //				EffectRevealer* pEffectRevealer = NULL;
                //				if ( pViewer->isFlag( Effect::EFFECT_CLASS_REVEALER ) )
                //				{
                //					pEffectRevealer = dynamic_cast<EffectRevealer*>(pViewer->findEffect(
                // Effect::EFFECT_CLASS_REVEALER ) ); 					Assert( pEffectRevealer );
                //				}


                EffectObservingEye* pEffectObservingEye = NULL;
                if (pViewer->isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE)) {
                    pEffectObservingEye =
                        dynamic_cast<EffectObservingEye*>(pViewer->findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
                    // Assert( pEffectObservingEye != NULL );
                }


                EffectGnomesWhisper* pEffectGnomesWhisper = NULL;
                if (pViewer->isFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER)) {
                    pEffectGnomesWhisper =
                        dynamic_cast<EffectGnomesWhisper*>(pViewer->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
                    // Assert( pEffectGnomesWhisper != NULL );
                }

                if (pViewer != pCreature && pViewer->isPC() && (pViewer->getVisionState(cx, cy) >= IN_SIGHT)) {
                    // if ((!pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
                    //	|| pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN)))
                    {
                        if (!pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) &&
                            (pViewer->isSlayer() || pViewer->isOusters()) &&
                            !(pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility(pCreature)) &&
                            !(pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeInvisibility())) {
                            pViewer->getPlayer()->sendPacket(pGCAddXXX);
                        }
                    }
                    // else
                    {}


                    pViewer->getPlayer()->sendPacket(&gcRemoveEffect);

                } // if

            } // for

        } // for

    } // for


    //--------------------------------------------

    //--------------------------------------------
    if (bForced == true) {
        EffectManager* pEffectManager = pCreature->getEffectManager();
        Assert(pEffectManager);
        pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_INVISIBILITY);
    }

    /*
    GCRemoveEffect removeEffect;
    removeEffect.setObjectID(pCreature->getObjectID());
    removeEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_INVISIBILITY);
    pZone->broadcastPacket(cx, cy, &removeEffect);
    */

    pCreature->removeFlag(Effect::EFFECT_CLASS_INVISIBILITY);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addSnipingModeCreature(Zone* pZone, Creature* pCreature, ZoneCoord_t cx, ZoneCoord_t cy) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pCreature != NULL);


    Assert(pCreature->isSlayer());

    ObjectID_t creatureID = pCreature->getObjectID();

    GCDeleteObject gcDO;
    gcDO.setObjectID(creatureID);

    pCreature->setFlag(Effect::EFFECT_CLASS_SNIPING_MODE);

    // Tile& rTile = pZone->getTile(cx, cy);

    GCAddEffect gcAddEffect;
    gcAddEffect.setObjectID(creatureID);
    gcAddEffect.setEffectID(Effect::EFFECT_CLASS_SNIPING_MODE);
    gcAddEffect.setDuration(0);

    //--------------------------------------------------------------------------------
    //


    //
    //--------------------------------------------------------------------------------
    for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1),
                     endx = min(pZone->getWidth() - 1, cx + maxViewportWidth + 1);
         ix <= endx; ix++) {
        for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1),
                         endy = min(pZone->getHeight() - 1, cy + maxViewportLowerHeight + 1);
             iy <= endy; iy++) {
            Tile& curTile = pZone->getTile(ix, iy);
            const slist<Object*>& objectList = curTile.getObjectList();

            slist<Object*>::const_iterator itr = objectList.begin();
            for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE;
                 itr++) {
                Assert(*itr != NULL);
                Creature* pViewer = dynamic_cast<Creature*>(*itr);

                if (pViewer != pCreature && pViewer->isPC() && (pViewer->getVisionState(cx, cy) >= IN_SIGHT)) {
                    //					EffectRevealer* pEffectRevealer = NULL;
                    //					if ( pViewer->isFlag( Effect::EFFECT_CLASS_REVEALER ) )
                    //					{
                    //						pEffectRevealer = dynamic_cast<EffectRevealer*>(pViewer->findEffect(
                    // Effect::EFFECT_CLASS_REVEALER ) ); 						Assert( pEffectRevealer );
                    //					}

                    EffectGnomesWhisper* pEffectGnomesWhisper = NULL;
                    if (pViewer->isFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER)) {
                        pEffectGnomesWhisper = dynamic_cast<EffectGnomesWhisper*>(
                            pViewer->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
                        // Assert( pEffectGnomesWhisper != NULL );
                    }

                    if (!pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) ||
                        pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) ||
                        (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeHide()))
                    //						|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pCreature ) )
                    //))
                    {
                        if (pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ||
                            (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeSniping()))
                        //							|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeSniping(
                        // pCreature ) ) )
                        {
                            pViewer->getPlayer()->sendPacket(&gcAddEffect);
                        } else {
                            pViewer->getPlayer()->sendPacket(&gcDO);
                        }


                        /*						if (!pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ||
                           pViewer->isVampire())
                                                {
                                                    pViewer->getPlayer()->sendPacket(&gcDO);
                                                }
                                                else
                                                {
                                                    pViewer->getPlayer()->sendPacket(&gcAddEffect);
                                                }*/
                    } else {
                    }
                } // if
            } // for
        } // for
    } // for

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addUnSnipingModeCreature(Zone* pZone, Creature* pCreature, bool bForced) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pCreature != NULL);


    Assert(pCreature->isSlayer());


    Assert(pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE));

    ZoneCoord_t cx = pCreature->getX();
    ZoneCoord_t cy = pCreature->getY();
    // Tile&       rTile       = pZone->getTile(cx, cy);

    Packet* pGCAddXXX = NULL;
    GCAddSlayer gcAddSlayer;

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
    //	gcAddSlayer.setSlayerInfo(pSlayer->getSlayerInfo3());
    //	gcAddSlayer.setEffectInfo(pSlayer->getEffectInfo());
    makeGCAddSlayer(&gcAddSlayer, pSlayer);
    pGCAddXXX = &gcAddSlayer;

    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_SNIPING_MODE);

    if (pCreature->isPC()) {
        Player* pPlayer = pCreature->getPlayer();
        Assert(pPlayer);
        pPlayer->sendPacket(&gcRemoveEffect);
    }

    //--------------------------------------------------------------------------------
    //


    //
    //--------------------------------------------------------------------------------
    for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1),
                     endx = min(pZone->getWidth() - 1, cx + maxViewportWidth + 1);
         ix <= endx; ix++) {
        for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1),
                         endy = min(pZone->getHeight() - 1, cy + maxViewportLowerHeight + 1);
             iy <= endy; iy++) {
            Tile& curTile = pZone->getTile(ix, iy);
            const slist<Object*>& objectList = curTile.getObjectList();

            slist<Object*>::const_iterator itr = objectList.begin();
            for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE;
                 itr++) {
                Assert(*itr != NULL);

                Creature* pViewer = dynamic_cast<Creature*>(*itr);


                //				EffectRevealer* pEffectRevealer = NULL;
                //				if ( pViewer->isFlag( Effect::EFFECT_CLASS_REVEALER ) )
                //				{
                //					pEffectRevealer =
                // dynamic_cast<EffectRevealer*>(pViewer->findEffect(Effect::EFFECT_CLASS_REVEALER));
                // Assert( pEffectRevealer );
                //				}

                EffectGnomesWhisper* pEffectGnomesWhisper = NULL;
                if (pViewer->isFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER)) {
                    pEffectGnomesWhisper =
                        dynamic_cast<EffectGnomesWhisper*>(pViewer->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
                    // Assert( pEffectGnomesWhisper != NULL );
                }

                if (pViewer != pCreature && pViewer->isPC() && (pViewer->getVisionState(cx, cy) >= IN_SIGHT)) {
                    if (!pCreature->isFlag(Effect::EFFECT_CLASS_HIDE) ||
                        pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) ||
                        (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeHide()))
                    //						|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pCreature ) )
                    //))
                    {
                        if (!pViewer->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ||
                            (pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeSniping()))
                        //							&& !( pEffectRevealer != NULL && pEffectRevealer->canSeeSniping(
                        // pCreature ) ) )
                        {
                            pViewer->getPlayer()->sendPacket(pGCAddXXX);
                        }
                    } else {
                    }


                    pViewer->getPlayer()->sendPacket(&gcRemoveEffect);

                } // if

            } // for

        } // for

    } // for


    //--------------------------------------------

    //--------------------------------------------
    if (bForced == true) {
        EffectManager* pEffectManager = pCreature->getEffectManager();
        Assert(pEffectManager);
        pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_SNIPING_MODE);
    }

    /*	GCRemoveEffect removeEffect;
        removeEffect.setObjectID(pCreature->getObjectID());
        removeEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_SNIPING_MODE);
        pZone->broadcastPacket(cx, cy, &removeEffect);
    */
    pCreature->removeFlag(Effect::EFFECT_CLASS_SNIPING_MODE);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void addInstalledMine(Zone* pZone, Mine* pMine, ZoneCoord_t cx, ZoneCoord_t cy) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pMine != NULL);
    Assert(pMine->isFlag(Effect::EFFECT_CLASS_INSTALL));

    // Tile& rTile = pZone->getTile(cx, cy);

    GCDeleteObject gcDO;
    gcDO.setObjectID(pMine->getObjectID());

    GCAddInstalledMineToZone gcAddMine;
    gcAddMine.setObjectID(pMine->getObjectID());
    gcAddMine.setX(cx);
    gcAddMine.setY(cy);
    gcAddMine.setItemClass(pMine->getItemClass());
    gcAddMine.setItemType(pMine->getItemType());
    gcAddMine.setOptionType(pMine->getOptionTypeList());
    gcAddMine.setDurability(pMine->getDurability());

    //--------------------------------------------------------------------------------
    //


    //
    //--------------------------------------------------------------------------------
    for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1),
                     endx = min(pZone->getWidth() - 1, cx + maxViewportWidth + 1);
         ix <= endx; ix++) {
        for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1),
                         endy = min(pZone->getHeight() - 1, cy + maxViewportLowerHeight + 1);
             iy <= endy; iy++) {
            Tile& rTile2 = pZone->getTile(ix, iy);
            const slist<Object*>& objectList = rTile2.getObjectList();

            for (slist<Object*>::const_iterator itr = objectList.begin();
                 itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE; itr++) {
                Assert(*itr != NULL);

                Creature* pViewer = dynamic_cast<Creature*>(*itr);

                Assert(pViewer != NULL);

                if (pViewer->isPC() && (pViewer->getVisionState(cx, cy) >= IN_SIGHT)) {
                    Player* pPlayer = pViewer->getPlayer();
                    Assert(pPlayer);
                    pPlayer->sendPacket(&gcDO);

                    if (pViewer->isFlag(Effect::EFFECT_CLASS_REVEALER)) {
                        pPlayer->sendPacket(&gcAddMine);
                    }
                } // if
            } // for
        } // for
    } // for

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
bool checkMine(Zone* pZone, Creature* pCreature, ZoneCoord_t X, ZoneCoord_t Y) throw() {
    __BEGIN_TRY

    Assert(pCreature != NULL);

    static bool bNonPK =
        g_pGameServerInfoManager
            ->getGameServerInfo(1, g_pConfig->getPropertyInt("ServerID"), g_pConfig->getPropertyInt("WorldID"))
            ->isNonPKServer();
    if (bNonPK && pCreature->isPC())
        return false;


    if (pCreature->isSlayer())
        return false;

    Assert(pZone != NULL);


    if (pZone->getZoneLevel(X, Y) & SAFE_ZONE)
        return false;

    Tile& rTile = pZone->getTile(X, Y);


    if (!rTile.hasItem())
        return false;

    // cout << "Check Mine Start" << endl;

    Item* pItem = rTile.getItem();


    if (pItem->getItemClass() != Item::ITEM_CLASS_MINE)
        return false;
    if (pItem->isFlag(Effect::EFFECT_CLASS_INSTALL) == false)
        return false;
    if (pCreature->isWalking() == false)
        return false;

    GCMineExplosionOK1 _GCMineExplosionOK1;
    GCMineExplosionOK2 _GCMineExplosionOK2;

    list<Creature*> cList;

    Mine* pMine = dynamic_cast<Mine*>(pItem);
    Assert(pMine != NULL);

    Dir_t Dir = pMine->getDir();
    Damage_t Damage = pMine->getDamage();
    ItemType_t Type = pMine->getItemType();
    string InstallerName = pMine->getInstallerName();
    int PartyID = pMine->getInstallerPartyID();

    BYTE explodeType = Type;


    pZone->deleteItem(pMine, X, Y);

    GCDeleteObject gcDO;
    gcDO.setObjectID(pMine->getObjectID());
    pZone->broadcastPacket(X, Y, &gcDO);

    SAFE_DELETE(pMine);

    /*
    switch(Type)
    {
        case 0: // Viper
            explodeType = 0;
            break;
        case 1: // Diamond Back
            explodeType = 2;
            break;
        case 2: // Sidewinder
            explodeType = 5;
            break;
        case 3: // cobra
            explodeType = 3;
            break;
        default:
            Assert(false);
    };
    */

    int tileX, tileY;

    const int* xOffsetByEType = NULL;
    const int* yOffsetByEType = NULL;
    int tiles = 0;


    getExplosionTypeXYOffset(explodeType, Dir, xOffsetByEType, yOffsetByEType, tiles);

    VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

    for (int tileI = 0; tileI < tiles; tileI++) {
        tileX = X + xOffsetByEType[tileI];
        tileY = Y + yOffsetByEType[tileI];
        // cout << "Check1 Tile X : " << (int)tileX << "," << " Tile Y : " << (int)tileY << endl;


        if (rect.ptInRect(tileX, tileY) && !(pZone->getZoneLevel(tileX, tileY) & SAFE_ZONE)) {
            //			if( tileX != X || tileY != Y ) checkMine( pZone, tileX, tileY );
            const Tile& tile = pZone->getTile(tileX, tileY);
            const slist<Object*>& oList = tile.getObjectList();


            for (slist<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++) {
                Object* pObject = *itr;
                if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE) {
                    Creature* pTargetCreature = dynamic_cast<Creature*>(pObject);
                    if (pTargetCreature->isSlayer()) {
                        // Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
                        cList.push_back(pTargetCreature);
                    } else if (pTargetCreature->isVampire()) {
                        // Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
                        cList.push_back(pTargetCreature);
                    } else if (pTargetCreature->isOusters()) {
                        cList.push_back(pTargetCreature);
                    } else if (pTargetCreature->isMonster()) {
                        // Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
                        cList.push_back(pTargetCreature);
                    } else
                        continue;

                    ObjectID_t targetObjectID = pTargetCreature->getObjectID();
                    _GCMineExplosionOK1.addCListElement(targetObjectID);
                    _GCMineExplosionOK2.addCListElement(targetObjectID);
                }
            }
        }
    }

    _GCMineExplosionOK1.setXYDir(X, Y, Dir);
    _GCMineExplosionOK1.setItemType(Type);

    _GCMineExplosionOK2.setXYDir(X, Y, Dir);
    _GCMineExplosionOK2.setItemType(Type);

    for (list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++) {
        Creature* pTargetCreature = *itr;
        _GCMineExplosionOK1.clearList();
        // cout << "Set Damage : " << (int)Damage << endl;

        if (pTargetCreature->isSlayer())
            Damage = max(1, Damage / 2);
        setDamage(pTargetCreature, Damage, NULL, 0, &_GCMineExplosionOK1);

        if (pTargetCreature->isPC()) {
            pTargetCreature->getPlayer()->sendPacket(&_GCMineExplosionOK1);
        } else if (pTargetCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);


            pMonster->addPrecedence(InstallerName, PartyID, Damage);
            pMonster->setLastHitCreatureClass(Creature::CREATURE_CLASS_SLAYER);
        }
    }

    pZone->broadcastPacket(X, Y, &_GCMineExplosionOK2, cList);

    // cout << "Check Mine End" << endl;
    return true;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
bool checkMine(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y) throw() {
    __BEGIN_TRY

    Assert(pZone != NULL);


    if (pZone->getZoneLevel(X, Y) & SAFE_ZONE)
        return false;

    Tile& rTile = pZone->getTile(X, Y);


    if (rTile.hasItem() == false)
        return false;

    // cout << "Check Mine2 Start" << endl;

    Item* pItem = rTile.getItem();


    if (pItem->getItemClass() != Item::ITEM_CLASS_MINE)
        return false;
    if (pItem->isFlag(Effect::EFFECT_CLASS_INSTALL) == false)
        return false;

    GCMineExplosionOK1 _GCMineExplosionOK1;
    GCMineExplosionOK2 _GCMineExplosionOK2;

    list<Creature*> cList;

    Mine* pMine = dynamic_cast<Mine*>(pItem);
    Assert(pMine != NULL);

    Dir_t Dir = pMine->getDir();
    Damage_t Damage = pMine->getDamage();
    ItemType_t Type = pMine->getItemType();
    string InstallerName = pMine->getInstallerName();
    int PartyID = pMine->getInstallerPartyID();

    BYTE explodeType = Type;
    /*
    switch(Type)
    {
        case 0: // Viper
            explodeType = 0;
            break;
        case 1: // Diamond Back
            explodeType = 2;
            break;
        case 2: // Sidewinder
            explodeType = 5;
            break;
        case 3: // cobra
            explodeType = 3;
            break;
        default:
            Assert(false);
    };
    */


    pZone->deleteItem(pMine, X, Y);

    GCDeleteObject gcDO;
    gcDO.setObjectID(pMine->getObjectID());
    pZone->broadcastPacket(X, Y, &gcDO);

    SAFE_DELETE(pMine);


    int tileX, tileY;

    const int* xOffsetByEType = NULL;
    const int* yOffsetByEType = NULL;
    int tiles = 0;


    getExplosionTypeXYOffset(explodeType, Dir, xOffsetByEType, yOffsetByEType, tiles);

    VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

    for (int tileI = 0; tileI < tiles; tileI++) {
        tileX = X + xOffsetByEType[tileI];
        tileY = Y + yOffsetByEType[tileI];
        // cout << "Check2 Tile X : " << (int)tileX << "," << " Tile Y : " << (int)tileY << endl;


        if (rect.ptInRect(tileX, tileY) && !(pZone->getZoneLevel(tileX, tileY) & SAFE_ZONE)) {
            //			if( tileX != X || tileY != Y ) checkMine( pZone, tileX, tileY );

            const Tile& tile = pZone->getTile(tileX, tileY);
            const slist<Object*>& oList = tile.getObjectList();


            for (slist<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++) {
                Object* pObject = *itr;
                if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE) {
                    Creature* pTargetCreature = dynamic_cast<Creature*>(pObject);
                    if (pTargetCreature->isSlayer()) {
                        // Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
                        cList.push_back(pTargetCreature);
                    } else if (pTargetCreature->isVampire()) {
                        // Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
                        cList.push_back(pTargetCreature);
                    } else if (pTargetCreature->isOusters()) {
                        cList.push_back(pTargetCreature);
                    } else if (pTargetCreature->isMonster()) {
                        // Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
                        cList.push_back(pTargetCreature);
                    } else
                        continue;

                    ObjectID_t targetObjectID = pTargetCreature->getObjectID();
                    _GCMineExplosionOK1.addCListElement(targetObjectID);
                    _GCMineExplosionOK2.addCListElement(targetObjectID);
                }
            }
        }
    }

    _GCMineExplosionOK1.setXYDir(X, Y, Dir);
    _GCMineExplosionOK1.setItemType(Type);

    _GCMineExplosionOK2.setXYDir(X, Y, Dir);
    _GCMineExplosionOK2.setItemType(Type);

    for (list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++) {
        Creature* pTargetCreature = *itr;
        _GCMineExplosionOK1.clearList();
        // cout << "Set Damage : " << (int)Damage << endl;

        if (pTargetCreature->isSlayer())
            Damage = max(1, Damage / 2);
        setDamage(pTargetCreature, Damage, NULL, 0, &_GCMineExplosionOK1);

        if (pTargetCreature->isPC()) {
            pTargetCreature->getPlayer()->sendPacket(&_GCMineExplosionOK1);
        } else if (pTargetCreature->isMonster()) {
            Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);


            pMonster->addPrecedence(InstallerName, PartyID, Damage);
            pMonster->setLastHitCreatureClass(Creature::CREATURE_CLASS_SLAYER);
        }
    }

    pZone->broadcastPacket(X, Y, &_GCMineExplosionOK2, cList);

    // cout << "Check Mine2 End" << endl;
    return true;

    __END_CATCH
}

bool checkTrap(Zone* pZone, Creature* pCreature) {
    if (!isValidZoneCoord(pZone, pCreature->getX(), pCreature->getY()))
        return false;

    Tile& rTile = pZone->getTile(pCreature->getX(), pCreature->getY());
    Effect* pEffect = rTile.getEffect(Effect::EFFECT_CLASS_TRAP_INSTALLED);
    if (pEffect == NULL)
        return false;

    int ratio = 0;

    if (pCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);
        ratio = 100 - (pMonster->getLevel() / 10);
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        ratio = 100 - (pVampire->getINT() / 8);
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
        ratio = 100 - (pOusters->getINT() / 8);
    } else
        return false;

    if (rand() % 100 > ratio)
        return false;

    EffectTrapInstalled* pTrap = dynamic_cast<EffectTrapInstalled*>(pEffect);
    pTrap->affect(pCreature);

    return true;
}

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
void transportCreature(Creature* pCreature, ZoneID_t TargetZoneID, ZoneCoord_t TX, ZoneCoord_t TY,
                       bool bSendMoveOK) throw() {
    __BEGIN_TRY

    Assert(pCreature->isPC());

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
    Zone* pZone = pCreature->getZone();


    // by sigi. 2002.12.10

    if (pGamePlayer->getPlayerStatus() != GPS_NORMAL) {
        filelog("transportCreatureError.log",
                "PlayerStatus not GPS_NORMAL: %d, Current[%d, (%d,%d)] --> Target[%d, (%d,%d)]",
                (int)pGamePlayer->getPlayerStatus(), (int)pZone->getZoneID(), (int)pCreature->getX(),
                (int)pCreature->getY(), (int)TargetZoneID, (int)TX, (int)TY);

        return;
    }

    cout << "ZoneUtil.cpp step 1" << endl;


    Assert(pGamePlayer != NULL);
    Assert(pZone != NULL);

    if (bSendMoveOK) {
        cout << "ZoneUtil.cpp step 2" << endl;

        GCMoveOK gcMoveOK(pCreature->getX(), pCreature->getY(), pCreature->getDir());
        pGamePlayer->sendPacket(&gcMoveOK);
    }


    // #if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)

    //  add by inthesky 2004.07.26

    ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(TargetZoneID);

    // add by Sonic 2006.10.21

    if (TargetZoneID == 1013) {
        cout << "ZoneUtil.cpp step New1013" << endl;
        if (pZoneInfo->isNoPortalZone()) {
            PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
            ItemType_t fitItem = 0;
            Item* pItem = pPC->getInventory()->findItem(Item::ITEM_CLASS_MONEY, fitItem);
            GCSystemMessage gcSystemMessage1;
            if (pItem == NULL) {
                gcSystemMessage1.setMessage("!");
                pGamePlayer->sendPacket(&gcSystemMessage1);
                return;
            }
            if (pItem->getNum() == 1) {
                if (pItem->isTimeLimitItem() == true && pItem->getHour() > 0) {
                } else {
                    pPC->getInventory()->deleteItem(pItem->getObjectID());
                    pItem->destroy();
                    SAFE_DELETE(pItem);
                }
            } else {
                pItem->setNum((pItem->getNum() - 1))
            }
        }
        GCSystemMessage gcSystemMessage1;
        gcSystemMessage1.setMessage("''");
        pGamePlayer->sendPacket(&gcSystemMessage1);
    }
    // end by sonic

    // if(!pZoneInfo->isAvailable())
    //{
    //	GCSystemMessage gcSystemMessage;
    //	gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER) );
    //	pGamePlayer->sendPacket (&gcSystemMessage);
    //	return;
    // }

    cout << "ZoneUtil.cpp step 3" << endl;
    // #endif

    bool bNoMoney = false;

    try {
        ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(TargetZoneID);


        if (pZoneInfo != NULL && (pZoneInfo->isPayPlay() || pZoneInfo->isPremiumZone()) &&
            !pGamePlayer->isPayPlaying() && !(g_pWarSystem->hasActiveRaceWar() && pZoneInfo->isHolyLand())) {
            cout << "ZoneUtil.cpp step 4" << endl;

            bool bEnterZone = true;

            // Statement* pStmt = NULL;
            string connectIP = pGamePlayer->getSocket()->getHost();


            if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID())) {
                cout << "ZoneUtil.cpp step 5" << endl;

                sendPayInfo(pGamePlayer);


                Zone* pZone = getZoneByZoneID(TargetZoneID);
                Assert(pZone != NULL);


                bEnterZone = enterMasterLair(pZone, pCreature);
            } else if (pZoneInfo->isPayPlay() && !pGamePlayer->isFamilyFreePass()) {
                cout << "ZoneUtil.cpp step 6" << endl;

                bEnterZone = false;
            }

            if (!bEnterZone) {
                cout << "ZoneUtil.cpp step 7" << endl;


                ZONE_COORD zoneCoord;
                bool bFindPos = false;

                if (pCreature->isSlayer())
                    bFindPos = g_pResurrectLocationManager->getSlayerPosition(13, zoneCoord);
                else if (pCreature->isVampire())
                    bFindPos = g_pResurrectLocationManager->getVampirePosition(23, zoneCoord);
                else if (pCreature->isOusters())
                    bFindPos = g_pResurrectLocationManager->getOustersPosition(1311, zoneCoord);

                if (bFindPos) {
                    TargetZoneID = zoneCoord.id;
                    TX = zoneCoord.x;
                    TY = zoneCoord.y;

                    bNoMoney = true;
                } else {
                    filelog("zoneUtilError.txt", "[ZoneUtil::transportCreature] ResurrectInfo is not esta..");
                    throw Error("Critical Error : ResurrectInfo is not established!1");
                }
            }
        }
    } catch (NoSuchElementException& no) {
        filelog("zoneUtilError.txt", "[ZoneUtil::transportCreature] %s", no.toString().c_str());
        throw Error(no.toString());
    }


    try {
        if (bNoMoney && pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            if (pSlayer->hasRideMotorcycle()) {
                pSlayer->getOffMotorcycle();

                GCGetOffMotorCycle _GCGetOffMotorCycle;
                _GCGetOffMotorCycle.setObjectID(pSlayer->getObjectID());
                pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCGetOffMotorCycle);
            }
        }

        cout << "ZoneUtil.cpp step 8" << endl;

        pCreature->save();

        ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(TargetZoneID);
        Assert(pZoneInfo != NULL);


        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE)) {
            if (pZone->isHolyLand()) {
                if (!pZoneInfo->isHolyLand() || (!pZoneInfo->isCastle() && g_pCastleInfoManager->isSameCastleZone(
                                                                               pZone->getZoneID(), TargetZoneID)))
                    dropRelicToZone(pCreature);
            }
        }


        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL)) {
            if (pZone->isHolyLand() && !pZoneInfo->isHolyLand() ||
                !g_pCastleInfoManager->isSameCastleZone(pCreature->getZone()->getZoneID(), TargetZoneID)

                || pZoneInfo->isCastle()) {
                dropRelicToZone(pCreature);
            }
        }

        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)) {
            if (g_pFlagManager->isFlagAllowedZone(pZone->getZoneID()) &&
                !g_pFlagManager->isFlagAllowedZone(pZoneInfo->getZoneID())) {
                dropFlagToZone(pCreature);
            }
        }

        if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER))
            ;
        dropSweeperToZone(pCreature);


        if (pZone->isHolyLand() != pZoneInfo->isHolyLand()) {
            pCreature->setFlag(Effect::EFFECT_CLASS_INIT_ALL_STAT);
        }

        if (g_pLevelWarZoneInfoManager->isCreatureBonusZone(pCreature, pZone->getZoneID()) !=
            g_pLevelWarZoneInfoManager->isCreatureBonusZone(pCreature, TargetZoneID)) {
            pCreature->setFlag(Effect::EFFECT_CLASS_INIT_ALL_STAT);
        }

        if (pZone->isLevelWarZone() != pZoneInfo->isLevelWarZone()) {
            pCreature->setFlag(Effect::EFFECT_CLASS_INIT_ALL_STAT);
        }


        //
        // *CAUTION*


        pZone->deleteCreature(pCreature, pCreature->getX(), pCreature->getY());


        // pZone->getZoneGroup()->getZonePlayerManager()->deletePlayer(pGamePlayer->getSocket()->getSOCKET());
        // pZone->getZoneGroup()->getZonePlayerManager()->deletePlayer_NOBLOCKED(pGamePlayer->getSocket()->getSOCKET());
        pZone->getZoneGroup()->getZonePlayerManager()->deletePlayer(pGamePlayer->getSocket()->getSOCKET());


        // pCreature->setXY(TX, TY);
        // pCreature->setZone(NULL);
        cout << "ZoneUtil.cpp step 9" << endl;


        // g_pIncomingPlayerManager->addPlayer(pGamePlayer);
        // g_pIncomingPlayerManager->pushPlayer(pGamePlayer);
        pZone->getZoneGroup()->getZonePlayerManager()->pushOutPlayer(pGamePlayer);
    } catch (NoSuchElementException& nsee) {
        filelog("zoneUtilError.txt", "[ZoneUtil::transportCreature2] %s", nsee.toString().c_str());
        throw Error(nsee.toString());
    }


    Zone* pNewZone = getZoneByZoneID(TargetZoneID);
    Assert(pNewZone != NULL);

    pCreature->setNewZone(pNewZone);
    pCreature->setNewXY(TX, TY);


    // pCreature->registerObject();


    if (!pZone->isHolyLand() && pNewZone->isHolyLand() || pZone->isHolyLand() && !pNewZone->isHolyLand()) {
        sendHolyLandWarpEffect(pCreature);
        cout << "ZoneUtil.cpp step 10" << endl;
    }

    // change player status
    pGamePlayer->setPlayerStatus(GPS_WAITING_FOR_CG_READY);

    cout << "ZoneUtil.cpp step 11" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
Zone* getZoneByZoneID(ZoneID_t ZID) throw(Error) {
    __BEGIN_TRY

    ZoneInfo* pZoneInfo = NULL;
    try {
        pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZID);
    } catch (NoSuchElementException&) {
        // cerr << "getZoneByZoneID() : No Such ZoneInfo: " << (int)ZID << endl;
        StringStream msg;
        msg << "getZoneByZoneID() : No Such ZoneInfo [" << (int)ZID << "]";
        throw Error(msg.toString());
    }

    ZoneGroup* pZoneGroup = NULL;
    try {
        pZoneGroup = g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());
    } catch (NoSuchElementException&) {
        // cerr << "getZoneByZoneID() : No Such ZoneGroup" << endl;
        throw Error("getZoneByZoneID() : No Such ZoneGroup");
    }

    Zone* pZone = pZoneGroup->getZone(ZID);
    Assert(pZone != NULL);

    return pZone;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void addMonstersToZone(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, SpriteType_t SType, MonsterType_t MType, int num,
                       const SUMMON_INFO& summonInfo, list<Monster*>* pSummonedMonsters) throw() {
    __BEGIN_TRY

    try {
        MonsterManager* pMonsterManager = pZone->getMonsterManager();
        Assert(pMonsterManager != NULL);
        // pMonsterManager->addMonsters(x, y, MType, num, summonInfo);

        /*
        if (summonInfo.clanType == SUMMON_INFO::CLAN_TYPE_RANDOM_GROUP)
        {
            summonInfo.clanID = rand()%90+2;
        }
        */

        if (SType != 0) {
            const vector<MonsterType_t>& monsterTypes = g_pMonsterInfoManager->getMonsterTypeBySprite(SType);

            if (!monsterTypes.empty()) {
                for (int i = 0; i < num; i++) {
                    MonsterType_t monsterType = monsterTypes[rand() % monsterTypes.size()];

                    pMonsterManager->addMonsters(x, y, monsterType, 1, summonInfo, pSummonedMonsters);
                }
            }
        } else if (MType != 0) {
            pMonsterManager->addMonsters(x, y, MType, num, summonInfo, pSummonedMonsters);
        }
    } catch (Throwable& t) {
        cerr << t.toString() << endl;
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void addMonstersToZone(Zone* pZone, const SUMMON_INFO2& summonInfo, list<Monster*>* pSummonedMonsters) throw() {
    __BEGIN_TRY

    try {
        MonsterCollection* pCollection = summonInfo.pMonsters;

        if (pCollection == NULL)
            return;

        MonsterManager* pMonsterManager = pZone->getMonsterManager();
        Assert(pMonsterManager != NULL);

        /*
        if (summonInfo.clanType == SUMMON_INFO::CLAN_TYPE_RANDOM_GROUP)
        {
            summonInfo.clanID = rand()%90+2;
        }
        */

        list<MonsterCollectionInfo>& Infos = pCollection->Infos;
        list<MonsterCollectionInfo>::const_iterator itr;
        for (itr = Infos.begin(); itr != Infos.end(); itr++) {
            const MonsterCollectionInfo& monsterInfo = *itr;

            if (monsterInfo.SpriteType != 0) {
                const vector<MonsterType_t>& monsterTypes =
                    g_pMonsterInfoManager->getMonsterTypeBySprite(monsterInfo.SpriteType);

                if (!monsterTypes.empty()) {
                    for (int i = 0; i < monsterInfo.Num; i++) {
                        MonsterType_t monsterType = monsterTypes[rand() % monsterTypes.size()];

                        pMonsterManager->addMonsters(summonInfo.X, summonInfo.Y, monsterType, 1, summonInfo,
                                                     pSummonedMonsters);
                    }
                }
            } else if (monsterInfo.MonsterType != 0) {
                pMonsterManager->addMonsters(summonInfo.X, summonInfo.Y, monsterInfo.MonsterType, monsterInfo.Num,
                                             summonInfo, pSummonedMonsters);
            }
        }
    } catch (Throwable& t) {
        cerr << t.toString() << endl;
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
bool isInSafeZone(Creature* pCreature) {
    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    ZoneLevel_t ZoneLevel = pZone->getZoneLevel(pCreature->getX(), pCreature->getY());

    if (pCreature->isSlayer() && (ZoneLevel & SLAYER_SAFE_ZONE))
        return true;
    if (pCreature->isVampire() && (ZoneLevel & VAMPIRE_SAFE_ZONE))
        return true;
    if (pCreature->isOusters() && (ZoneLevel & OUSTERS_SAFE_ZONE))
        return true;
    if (ZoneLevel & COMPLETE_SAFE_ZONE)
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool isValidZoneCoord(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, int offset) {
    Assert(pZone != NULL);

    VSRect rect;
    rect.left = 0 + offset;
    rect.top = 0 + offset;
    rect.right = pZone->getWidth() - offset - 1;
    rect.bottom = pZone->getHeight() - offset - 1;

    if (rect.ptInRect(x, y))
        return true;

    return false;
}

bool enterMasterLair(Zone* pZone, Creature* pCreature) throw(Error) {
    __BEGIN_TRY

    if (pZone == NULL || pCreature == NULL)
        return false;


    if (!pZone->isMasterLair()) {
        return true;
    }

    MasterLairManager* pMasterLairManager = pZone->getMasterLairManager();
    Assert(pMasterLairManager != NULL);

    if (pMasterLairManager->enterCreature(pCreature)) {
        return true;
    }

    __END_CATCH

    return false;
}

void getNewbieTransportZoneInfo(Slayer* pSlayer, ZONE_COORD& zoneInfo) {
    zoneInfo.x = 30;
    zoneInfo.y = 42;

    switch (pSlayer->getHighestSkillDomain()) {
    case SKILL_DOMAIN_HEAL:
    case SKILL_DOMAIN_ENCHANT:
        zoneInfo.id = 2010;
        break;

    case SKILL_DOMAIN_GUN:
        zoneInfo.id = 2000;
        break;

    // case SKILL_DOMAIN_SWORD :
    // case SKILL_DOMAIN_BLADE :
    default:
        zoneInfo.id = 2020;
        break;
    }
}


void checkNewbieTransportToGuild(Slayer* pSlayer) {
    try {
        if (pSlayer->isPLAYER() && g_pVariableManager->isNewbieTransportToGuild()) {
            ZONE_COORD transportZone;

            getNewbieTransportZoneInfo(pSlayer, transportZone);

            ZoneID_t zoneID = pSlayer->getZone()->getZoneID();
            if (zoneID == 2101) // || zoneID==2102)
            {
                Attr_t BasicSUM =
                    pSlayer->getSTR(ATTR_BASIC) + pSlayer->getDEX(ATTR_BASIC) + pSlayer->getINT(ATTR_BASIC);

                if (BasicSUM >= 39) {
                    GCSystemMessage gcSystemMessage;
                    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NEWBIE_TRANSPORT_TO_GUILD));
                    pSlayer->getPlayer()->sendPacket(&gcSystemMessage);
                }

                // else
                if (BasicSUM >= 40) {
                    Player* pPlayer = pSlayer->getPlayer();
                    Assert(pPlayer != NULL);

                    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
                    Event* pEvent = pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT);

                    if (pEvent == NULL) {
                        ZoneID_t ZoneID;
                        ZoneCoord_t ZoneX = 30, ZoneY = 42;
                        string ZoneName;

                        switch (pSlayer->getHighestSkillDomain()) {
                        case SKILL_DOMAIN_HEAL:
                        case SKILL_DOMAIN_ENCHANT:
                            ZoneID = 2010;
                            ZoneName = g_pStringPool->getString(STRID_CLERIC_GUILD);
                            break;

                        case SKILL_DOMAIN_GUN:
                            ZoneID = 2000;
                            ZoneName = g_pStringPool->getString(STRID_SOLDIER_GUILD);
                            break;

                        // case SKILL_DOMAIN_SWORD :
                        // case SKILL_DOMAIN_BLADE :
                        default:
                            ZoneID = 2020;
                            ZoneName = g_pStringPool->getString(STRID_KNIGHT_GUILD);
                            break;
                        }


                        // transportCreature( pSlayer, ZoneID, ZoneX, ZoneY, false );

                        Turn_t deadline = 600;
                        int timePenalty = (BasicSUM - 40) * 100;
                        deadline -= min(500, timePenalty);


                        Player* pPlayer = pSlayer->getPlayer();
                        Assert(pPlayer != NULL);

                        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

                        // pGamePlayer->deleteEvent(Event::EVENT_CLASS_TRANSPORT);

                        EventTransport* pEventTransport = new EventTransport(pGamePlayer);

                        pEventTransport->setDeadline(deadline);
                        pEventTransport->setTargetZone(ZoneID, ZoneX, ZoneY);
                        pEventTransport->setZoneName(ZoneName);


                        pEventTransport->sendMessage();

                        pGamePlayer->addEvent(pEventTransport);
                    } else {
                        EventTransport* pEventTransport = dynamic_cast<EventTransport*>(pEvent);
                        pEventTransport->sendMessage();
                    }
                }
            }
        }
    } catch (Throwable& t) {
        filelog("newbieTransportBUG.log", "%s", t.toString().c_str());
    }
}


bool addCorpseToZone(Corpse* pCorpse, Zone* pZone, ZoneCoord_t cx, ZoneCoord_t cy) throw(Error) {
    __BEGIN_TRY

    Assert(pCorpse != NULL);
    Assert(pZone != NULL);


    //	Tile & tile = pZone->getTile(cx , cy);


    TPOINT pt = pZone->addItem(pCorpse, cx, cy);
    if (pt.x == -1) {
        SAFE_DELETE(pCorpse);
        return false;
    }

    pCorpse->setX(pt.x);
    pCorpse->setY(pt.y);
    pCorpse->setZone(pZone);

    __END_CATCH

    return true;
}


bool checkCorpse(Zone* pZone, MonsterType_t MType, ZoneCoord_t x1, ZoneCoord_t y1, ZoneCoord_t x2,
                 ZoneCoord_t y2) throw() {
    __BEGIN_TRY

    x1 = max(0, (int)x1);
    y1 = max(0, (int)y1);
    x2 = min(pZone->getWidth() - 1, (int)x2);
    y2 = min(pZone->getHeight() - 1, (int)y2);

    // Assert( x1 <= x2 && y1 <= y2 );
    //  by sigi. 2002.12.25
    if (!isValidZoneCoord(pZone, x1, y1) || !isValidZoneCoord(pZone, x2, y2)) {
        return false;
    }

    ZoneCoord_t ix, iy;

    for (ix = x1; ix <= x2; ix++) {
        for (iy = y1; iy <= y2; iy++) {
            Tile& curTile = pZone->getTile(ix, iy);
            Item* pItem = curTile.getItem();

            if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_CORPSE &&
                pItem->getItemType() == MONSTER_CORPSE) {
                MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pItem);
                if (pMonsterCorpse->getMonsterType() == MType) {
                    return true;
                }
            }
        }
    }

    return false;

    __END_CATCH
}


void makeZoneIDList(const string& zoneIDs, list<ZoneID_t>& zoneIDList) throw(Error) {
    __BEGIN_TRY

    size_t a = 0, b = 0;

    //////////////////////////////////////////////
    // 12345,67890,
    // a    ba    b
    //////////////////////////////////////////////
    zoneIDList.clear();
    if (zoneIDs.size() <= 1)
        return;

    do {
        b = zoneIDs.find_first_of(',', a);

        string zoneID = trim(zoneIDs.substr(a, b - a));


        zoneIDList.push_back(atoi(zoneID.c_str()));

        a = b + 1;

    } while (b != string::npos && b < zoneIDs.size() - 1);

    __END_CATCH
}

uint getZoneTimeband(Zone* pZone) {
    if (pZone == NULL) {
        return g_pTimeManager->getTimeband();
    }

    return pZone->getTimeband();
}

bool createBulletinBoard(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y, MonsterType_t type, const string& msg,
                         const VSDateTime& timeLimit) {
    __BEGIN_TRY

    if (pZone->isMasterLair() || checkCorpse(pZone, type, X - 2, Y - 2, X + 2, Y + 2))
        return false;

    MonsterCorpse* pCorpse = new MonsterCorpse(type, msg, 2);
    Assert(pCorpse != NULL);

    pZone->registerObject(pCorpse);

    int delayTime = VSDateTime::currentDateTime().secsTo(timeLimit);
    TPOINT pt = pZone->addItem(pCorpse, X, Y, true, delayTime * 10);

    if (pt.x == -1) {
        SAFE_DELETE(pCorpse);
        return false;
    }

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertBulletinStmt(pConn, "INSERT INTO BulletinBoardObject VALUES (0, ?, ?, ?, ?, ?, ?, ?)");
        insertBulletinStmt.bindUInt(1, g_pConfig->getPropertyInt("ServerID"));
        insertBulletinStmt.bindUInt(2, pZone->getZoneID());
        insertBulletinStmt.bindUInt(3, pt.x);
        insertBulletinStmt.bindUInt(4, pt.y);
        // correctString() was a manual backslash-escape for embedding into a raw
        // SQL string literal; PreparedStatement sends the value out-of-band, so
        // bind the raw message instead (re-applying the escape would corrupt
        // stored quotes/backslashes).
        insertBulletinStmt.bindString(5, msg);
        insertBulletinStmt.bindUInt(6, (uint)type);
        insertBulletinStmt.bindString(7, timeLimit.toDateTime());
        insertBulletinStmt.execute();

        if (insertBulletinStmt.getAffectedRowCount() == 0) {
            filelog("BulletinBoard.log", "DB  . : %u, %u, %u, [%u:%s]", pZone->getZoneID(), pt.x, pt.y, type,
                    msg.c_str());
        }
    }
    END_DB(pStmt)

    return true;

    __END_CATCH
}

void loadBulletinBoard(Zone* pZone) {
    __BEGIN_TRY

    VSDateTime currentDateTime = VSDateTime::currentDateTime();

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectBulletinBoardStmt(
            pConn,
            "SELECT ID, X, Y, Message, Type, TimeLimit FROM BulletinBoardObject WHERE ServerID = ? AND ZoneID = ?");
        selectBulletinBoardStmt.bindUInt(1, g_pConfig->getPropertyInt("ServerID"));
        selectBulletinBoardStmt.bindUInt(2, pZone->getZoneID());
        Result* pResult = selectBulletinBoardStmt.execute();


        while (pResult->next()) {
            uint ID = pResult->getInt(1);
            ZoneCoord_t X = pResult->getInt(2);
            ZoneCoord_t Y = pResult->getInt(3);
            string msg = pResult->getString(4);
            MonsterType_t type = pResult->getInt(5);
            VSDateTime timeLimit(pResult->getString(6));

            if (timeLimit < currentDateTime) {
                cout << "   ." << ID << " : [" << X << "," << Y << "] " << msg << " [" << type << "] " << endl;
                PreparedStatement deleteBulletinBoardStmt(pConn, "DELETE FROM BulletinBoardObject WHERE ID = ?");
                deleteBulletinBoardStmt.bindUInt(1, ID);
                deleteBulletinBoardStmt.execute();
                continue;
            }

            int delayTime = currentDateTime.secsTo(timeLimit);

            MonsterCorpse* pCorpse = new MonsterCorpse(type, msg, 2);
            Assert(pCorpse != NULL);

            pZone->registerObject(pCorpse);

            TPOINT pt = pZone->addItem(pCorpse, X, Y, true, delayTime * 10);

            if (pt.x == -1) {
                filelog("BulletinBoard.log", "DB   . : %u, %u, %u, [%u:%s]", pZone->getZoneID(), X, Y, type,
                        msg.c_str());
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

void forbidDarkness(Zone* pZone, ZoneCoord_t tX, ZoneCoord_t tY, int range) {
    for (int ti = -range; ti <= range; ++ti)
        for (int tj = -range; tj <= range; ++tj) {
            ZoneCoord_t X = tX + ti;
            ZoneCoord_t Y = tY + tj;

            if (!isValidZoneCoord(pZone, X, Y))
                continue;

            Tile& rTile = pZone->getTile(X, Y);
            if (!rTile.canAddEffect() || rTile.getEffect(Effect::EFFECT_CLASS_DARKNESS_FORBIDDEN) != NULL)
                continue;

            EffectDarknessForbidden* pEffect = new EffectDarknessForbidden(pZone, X, Y);
            pZone->registerObject(pEffect);
            rTile.addEffect(pEffect);
        }
}
